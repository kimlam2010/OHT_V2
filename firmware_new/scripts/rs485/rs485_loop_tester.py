#!/usr/bin/env python3

import argparse
import os
import sys
import time
import threading
from collections import deque
from dataclasses import dataclass
from typing import Optional, Deque, Tuple

try:
    import serial
    from serial.rs485 import RS485Settings
except Exception as exc:  # noqa: BLE001
    print("pyserial is required. Install with: pip install pyserial", file=sys.stderr)
    raise


START_BYTE = 0xAA


def crc16_modbus(data: bytes) -> int:
    """Compute CRC16 Modbus polynomial 0xA001 (reflected), init 0xFFFF."""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
            crc &= 0xFFFF
    return crc


def build_frame(addr: int, cmd: int, payload: bytes) -> bytes:
    length = len(payload)
    hdr = bytes([START_BYTE, addr & 0xFF, cmd & 0xFF, length & 0xFF])
    crc = crc16_modbus(hdr[1:] + payload)  # CRC over ADDR..PAYLOAD
    return hdr + payload + bytes([crc & 0xFF, (crc >> 8) & 0xFF])  # little-endian


def parse_frame(buf: bytes) -> Optional[Tuple[int, int, bytes]]:
    if len(buf) < 6 or buf[0] != START_BYTE:
        return None
    addr, cmd, length = buf[1], buf[2], buf[3]
    need = 1 + 3 + length + 2
    if len(buf) < need:
        return None
    payload = buf[4:4 + length]
    crc_rx = buf[4 + length] | (buf[5 + length - 1] << 8)
    calc = crc16_modbus(buf[1:4] + payload)
    if crc_rx != calc:
        return None
    return addr, cmd, payload


@dataclass
class Stats:
    sent_frames: int = 0
    recv_frames: int = 0
    crc_errors: int = 0
    timeouts: int = 0
    retries: int = 0
    bytes_tx: int = 0
    bytes_rx: int = 0


class RingBuffer:
    def __init__(self, capacity_bytes: int) -> None:
        self.capacity = capacity_bytes
        self.buffer: Deque[int] = deque(maxlen=capacity_bytes)
        self.lock = threading.Lock()

    def write(self, data: bytes) -> None:
        with self.lock:
            for b in data:
                self.buffer.append(b)

    def read(self, max_len: int) -> bytes:
        with self.lock:
            out = bytearray()
            while self.buffer and len(out) < max_len:
                out.append(self.buffer.popleft())
            return bytes(out)


def reader_thread(ser: serial.Serial, rb: RingBuffer, stats: Stats, stop: threading.Event) -> None:
    ser.timeout = 0.02
    while not stop.is_set():
        try:
            data = ser.read(512)
            if data:
                rb.write(data)
                stats.bytes_rx += len(data)
        except serial.SerialException:
            break


def main() -> int:
    parser = argparse.ArgumentParser(description="RS485 loop tester for OHT-50")
    parser.add_argument("--dev", default=os.environ.get("OHT_RS485_DEV", "/dev/ttyS1"), help="serial device path")
    parser.add_argument("--baud", type=int, default=int(os.environ.get("OHT_RS485_BAUD", 115200)))
    parser.add_argument("--count", type=int, default=1000, help="frames to send")
    parser.add_argument("--payload", type=int, default=8, help="payload bytes")
    parser.add_argument("--addr", type=int, default=1)
    parser.add_argument("--cmd", type=int, default=1)
    parser.add_argument("--timeout", type=float, default=0.03)
    parser.add_argument("--retries", type=int, default=2)
    parser.add_argument("--auto_rts", action="store_true", help="enable RS485 auto-direction via RTS")
    args = parser.parse_args()

    try:
        ser = serial.Serial(
            port=args.dev,
            baudrate=args.baud,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=args.timeout,
            write_timeout=args.timeout,
        )
    except Exception as exc:  # noqa: BLE001
        print(f"Failed to open {args.dev}: {exc}", file=sys.stderr)
        return 2

    if args.auto_rts:
        try:
            ser.rs485_mode = RS485Settings(
                rts_level_for_tx=True,
                rts_level_for_rx=False,
                delay_before_tx=0,
                delay_before_rx=0,
            )
        except Exception as exc:  # noqa: BLE001
            print(f"Failed to enable RS485 auto_rts: {exc}", file=sys.stderr)

    rb = RingBuffer(capacity_bytes=64 * 1024)
    stats = Stats()
    stop = threading.Event()
    t = threading.Thread(target=reader_thread, args=(ser, rb, stats, stop), daemon=True)
    t.start()

    start = time.time()
    for i in range(args.count):
        payload = bytes((j % 256 for j in range(args.payload)))
        frame = build_frame(args.addr, args.cmd, payload)
        try:
            n = ser.write(frame)
            ser.flush()
            stats.bytes_tx += n
            stats.sent_frames += 1
        except serial.SerialTimeoutException:
            stats.timeouts += 1
            continue

        # Wait for echo or response (loopback/partner)
        deadline = time.time() + args.timeout
        buf = bytearray()
        got = False
        while time.time() < deadline:
            chunk = rb.read(1024)
            if chunk:
                buf.extend(chunk)
                # try to find a frame
                if START_BYTE in buf:
                    idx = buf.find(bytes([START_BYTE]))
                    fr = parse_frame(bytes(buf[idx:]))
                    if fr is not None:
                        stats.recv_frames += 1
                        got = True
                        break
            else:
                time.sleep(0.001)
        if not got:
            # Simple retry policy
            retried = False
            for _ in range(args.retries):
                try:
                    n = ser.write(frame)
                    ser.flush()
                    stats.bytes_tx += n
                    stats.retries += 1
                    deadline = time.time() + args.timeout
                    while time.time() < deadline:
                        chunk = rb.read(1024)
                        if chunk:
                            buf.extend(chunk)
                            if START_BYTE in buf:
                                idx = buf.find(bytes([START_BYTE]))
                                fr = parse_frame(bytes(buf[idx:]))
                                if fr is not None:
                                    stats.recv_frames += 1
                                    retried = True
                                    break
                        else:
                            time.sleep(0.001)
                    if retried:
                        break
                except serial.SerialTimeoutException:
                    stats.timeouts += 1
            if not retried:
                stats.timeouts += 1

    stop.set()
    t.join(timeout=0.5)
    elapsed = max(1e-6, time.time() - start)
    throughput = stats.bytes_tx / elapsed

    print(
        "\nRESULTS",
        f"\n  Dev: {args.dev}  Baud: {args.baud}",
        f"\n  Sent frames: {stats.sent_frames}",
        f"\n  Recv frames: {stats.recv_frames}",
        f"\n  Retries: {stats.retries}  Timeouts: {stats.timeouts}",
        f"\n  Bytes TX/RX: {stats.bytes_tx}/{stats.bytes_rx}",
        f"\n  Elapsed: {elapsed:.3f}s  TX throughput: {throughput:.1f} B/s",
        sep="",
    )

    return 0 if stats.recv_frames > 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())


