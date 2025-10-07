#!/usr/bin/env python3
"""
RS485 Modbus RTU quick tester for OHT-50

- Default port: /dev/ttyOHT485 (falls back to /dev/ttyS1 if specified)
- Default addr: 2 (0x02)
- Default baud: 115200 8N1

Usage examples:
  # Read holding register 0 once (FC=3)
  python3 tools/rs485_quick_test.py --port /dev/ttyOHT485 --addr 2 --read 0 1 --fc 3

  # Read holding register 0 ten times and report latency/loss
  python3 tools/rs485_quick_test.py --addr 2 --read 0 1 --fc 3 --count 10

  # Write 0x1234 to register 0 then read back 1 value
  python3 tools/rs485_quick_test.py --addr 2 --write 0 0x1234 --read 0 1 --fc 3

  # Read input registers (FC=4)
  python3 tools/rs485_quick_test.py --addr 2 --read 0 2 --fc 4

Requires: pyserial (pip install pyserial)
"""

from __future__ import annotations

import argparse
import binascii
import struct
import sys
import time

try:
    import serial  # type: ignore
except Exception as exc:  # pragma: no cover
    print("[ERROR] Missing dependency: pyserial. Install with: pip3 install pyserial", file=sys.stderr)
    sys.exit(2)


def crc16_modbus(data: bytes) -> int:
    """Compute CRC16 (Modbus polynomial 0xA001). Returns 16-bit integer."""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc & 0xFFFF


def build_read_registers(slave_id: int, start_address: int, quantity: int, function_code: int) -> bytes:
    if function_code not in (0x03, 0x04):
        raise ValueError("function_code must be 0x03 or 0x04")
    pdu = struct.pack(
        ">B B H H", slave_id & 0xFF, function_code & 0xFF, start_address & 0xFFFF, quantity & 0xFFFF
    )
    crc = crc16_modbus(pdu)
    return pdu + struct.pack("<H", crc)


def build_write_single_register(slave_id: int, address: int, value: int) -> bytes:
    pdu = struct.pack(
        ">B B H H", slave_id & 0xFF, 0x06, address & 0xFFFF, value & 0xFFFF
    )
    crc = crc16_modbus(pdu)
    return pdu + struct.pack("<H", crc)


def open_port(path: str, baud: int, timeout: float) -> serial.Serial:
    ser = serial.Serial()
    ser.port = path
    ser.baudrate = baud
    ser.bytesize = serial.EIGHTBITS
    ser.parity = serial.PARITY_NONE
    ser.stopbits = serial.STOPBITS_ONE
    ser.timeout = timeout
    ser.write_timeout = timeout
    ser.open()
    return ser


def exchange(ser: serial.Serial, req: bytes, expect_len: int | None) -> bytes:
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    ser.write(req)
    ser.flush()
    if expect_len is None:
        # Best-effort read
        time.sleep(0.02)
        return ser.read_all()
    # Read exactly expect_len with timeout
    data = b""
    deadline = time.time() + ser.timeout
    while len(data) < expect_len and time.time() < deadline:
        chunk = ser.read(expect_len - len(data))
        if chunk:
            data += chunk
    return data


def parse_read_response(resp: bytes, slave_id: int, quantity: int, function_code: int) -> list[int]:
    if len(resp) < 5:
        raise ValueError("short response")
    if resp[0] != (slave_id & 0xFF):
        raise ValueError("slave id mismatch")
    if resp[1] & 0x80:
        # Modbus exception response; propagate as ValueError with code
        raise ValueError(f"exception code: 0x{resp[2]:02X}")
    if resp[1] != (function_code & 0xFF):
        raise ValueError("function mismatch")
    byte_count = resp[2]
    expected = 3 + byte_count + 2
    if len(resp) != expected:
        raise ValueError(f"unexpected length {len(resp)} != {expected}")
    data_no_crc = resp[:-2]
    crc_expected = struct.unpack("<H", resp[-2:])[0]
    crc_actual = crc16_modbus(data_no_crc)
    if crc_expected != crc_actual:
        raise ValueError("CRC mismatch")
    if byte_count != quantity * 2:
        raise ValueError("byte_count mismatch")
    values: list[int] = []
    for i in range(quantity):
        hi = resp[3 + i * 2]
        lo = resp[3 + i * 2 + 1]
        values.append((hi << 8) | lo)
    return values


def parse_write_single_response(resp: bytes, slave_id: int, address: int, value: int) -> None:
    if len(resp) != 8:
        raise ValueError("unexpected length")
    if resp[0] != (slave_id & 0xFF) or resp[1] != 0x06:
        raise ValueError("header mismatch")
    data_no_crc = resp[:-2]
    crc_expected = struct.unpack("<H", resp[-2:])[0]
    crc_actual = crc16_modbus(data_no_crc)
    if crc_expected != crc_actual:
        raise ValueError("CRC mismatch")
    addr_rx = (resp[2] << 8) | resp[3]
    val_rx = (resp[4] << 8) | resp[5]
    if addr_rx != (address & 0xFFFF) or val_rx != (value & 0xFFFF):
        raise ValueError("echo mismatch")


def run(args: argparse.Namespace) -> int:
    port = args.port
    addr = int(args.addr, 0) if isinstance(args.addr, str) else int(args.addr)
    baud = args.baud
    timeout = args.timeout
    count = args.count
    fc = 3 if args.fc is None else int(args.fc)
    retries = max(0, int(args.retries))
    delay_ms = max(0, int(args.delay_ms))

    try:
        ser = open_port(port, baud, timeout)
    except Exception as e:
        print(f"[ERROR] Cannot open {port} @ {baud}: {e}", file=sys.stderr)
        return 2
    if args.pre_delay_ms:
        time.sleep(int(args.pre_delay_ms) / 1000.0)

    successes = 0
    latencies_ms: list[float] = []

    try:
        if args.write is not None:
            w_addr = int(args.write[0], 0)
            w_val = int(args.write[1], 0)
            req = build_write_single_register(addr, w_addr, w_val)
            t0 = time.perf_counter()
            resp = exchange(ser, req, expect_len=8)
            dt_ms = (time.perf_counter() - t0) * 1000.0
            parse_write_single_response(resp, addr, w_addr, w_val)
            successes += 1
            latencies_ms.append(dt_ms)
            print(f"WRITE ok: addr=0x{w_addr:04X} value=0x{w_val:04X} latency={dt_ms:.2f} ms")

        if args.read is not None:
            r_addr = int(args.read[0], 0)
            r_qty = int(args.read[1], 0)
            for i in range(count):
                attempt = 0
                while True:
                    req = build_read_registers(addr, r_addr, r_qty, fc)
                    expected_len = 3 + r_qty * 2 + 2
                    t0 = time.perf_counter()
                    resp = exchange(ser, req, expect_len=expected_len)
                    dt_ms = (time.perf_counter() - t0) * 1000.0
                    try:
                        values = parse_read_response(resp, addr, r_qty, fc)
                        successes += 1
                        latencies_ms.append(dt_ms)
                        values_hex = ", ".join(f"0x{v:04X}" for v in values)
                        print(f"READ ok [{i+1}/{count}]: @0x{r_addr:04X} (FC={fc}) -> [{values_hex}] latency={dt_ms:.2f} ms")
                        break
                    except ValueError as e:
                        msg = str(e)
                        busy = msg.endswith("0x06")  # Slave Device Busy
                        ack = msg.endswith("0x05")    # Acknowledge
                        if (busy or ack) and attempt < retries:
                            # Small delay then retry
                            attempt += 1
                            if delay_ms:
                                time.sleep(delay_ms / 1000.0)
                            continue
                        # Print error and stop this iteration
                        raw_hex = resp.hex(" ")
                        print(f"READ fail [{i+1}/{count}] attempt={attempt}: {e}; raw={raw_hex}")
                        break
    finally:
        ser.close()

    if latencies_ms:
        avg = sum(latencies_ms) / len(latencies_ms)
        p95 = sorted(latencies_ms)[int(max(0, len(latencies_ms) * 0.95) - 1)]
        print(
            f"SUMMARY: success={successes}, loss={0 if args.read is None else max(0, count - successes)}",
        )
        print(f"Latency: avg={avg:.2f} ms, p95={p95:.2f} ms")
    return 0


def build_argparser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description="RS485 Modbus RTU quick tester")
    p.add_argument("--port", default="/dev/ttyOHT485", help="Serial port path (default: /dev/ttyOHT485)")
    p.add_argument("--addr", default="2", help="Slave address (e.g. 2 or 0x02)")
    p.add_argument("--baud", type=int, default=115200, help="Baudrate (default: 115200)")
    p.add_argument("--timeout", type=float, default=0.5, help="Timeout seconds (default: 0.5)")
    p.add_argument("--count", type=int, default=1, help="Number of read iterations (default: 1)")
    p.add_argument("--read", nargs=2, metavar=("START_ADDR", "QUANTITY"), help="Read registers: start, quantity")
    p.add_argument("--write", nargs=2, metavar=("ADDR", "VALUE"), help="Write single register then echo: addr, value")
    p.add_argument("--fc", choices=["3", "4"], help="Function code for read: 3=holding, 4=input (default: 3)")
    p.add_argument("--retries", default="3", help="Retries on busy/ack exceptions (default: 3)")
    p.add_argument("--delay-ms", default="80", help="Delay between retries in ms (default: 80)")
    p.add_argument("--pre-delay-ms", default="200", help="Initial delay after port open in ms (default: 200)")
    return p


if __name__ == "__main__":  # pragma: no cover
    sys.exit(run(build_argparser().parse_args()))


