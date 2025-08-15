"""
RS485 Bus service: mở/kết nối serial, gửi/nhận khung, CRC, retry, timeout.
Ghi chú: Đây là skeleton; integration thật cần pyserial và driver cụ thể.
"""

from __future__ import annotations

import struct
import threading
import time
from dataclasses import dataclass
from typing import Optional, Tuple

from app.core.logging import get_logger
from fastapi import Request
import os
try:
    import serial  # type: ignore
except Exception:  # pragma: no cover
    serial = None  # fallback when pyserial missing

logger = get_logger(__name__)


@dataclass
class RS485Settings:
    device: str
    baud: int = 115200
    timeout_s: float = 0.05  # 50ms
    max_retries: int = 2


def crc16_modbus(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if (crc & 0x0001) != 0:
                crc >>= 1
                crc ^= 0xA001
            else:
                crc >>= 1
    return crc & 0xFFFF


class RS485Bus:
    def __init__(self, settings: RS485Settings) -> None:
        self.settings = settings
        self._lock = threading.Lock()
        self._opened = False
        self._serial: Optional["serial.Serial"] = None

    def open(self) -> None:
        logger.info("RS485 open", device=self.settings.device, baud=self.settings.baud)
        use_mock = os.getenv("USE_RS485_MOCK", "false").lower() == "true"
        if use_mock or serial is None:
            # Mock: mark opened but do not create real serial port
            self._serial = None
            self._opened = True
            return
        self._serial = serial.Serial(
            port=self.settings.device,
            baudrate=self.settings.baud,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=self.settings.timeout_s,
        )
        self._opened = True

    def close(self) -> None:
        logger.info("RS485 close")
        if self._serial and getattr(self._serial, "is_open", False):
            self._serial.close()
        self._opened = False

    def _build_frame(self, addr: int, cmd: int, payload: bytes) -> bytes:
        header = bytes([0xAA, addr & 0xFF, cmd & 0xFF, len(payload) & 0xFF])
        frame_wo_crc = header + payload
        crc = crc16_modbus(frame_wo_crc)
        return frame_wo_crc + struct.pack('<H', crc)

    def _parse_frame(self, data: bytes) -> Tuple[int, int, bytes]:
        if len(data) < 6 or data[0] != 0xAA:
            raise ValueError("Khung không hợp lệ")
        addr = data[1]
        cmd = data[2]
        length = data[3]
        payload = data[4:4+length]
        crc_rx = struct.unpack('<H', data[4+length:6+length])[0]
        if crc_rx != crc16_modbus(data[:4+length]):
            raise ValueError("CRC sai")
        return addr, cmd, payload

    def transact(self, addr: int, cmd: int, payload: bytes = b'', request: Optional[Request] = None) -> Optional[bytes]:
        if not self._opened:
            self.open()
        frame = self._build_frame(addr, cmd, payload)
        for attempt in range(self.settings.max_retries + 1):
            with self._lock:
                try:
                    if self._serial is None:
                        # Mock path: return None safely
                        payload_out = None
                    else:
                        self._serial.reset_input_buffer()
                        self._serial.write(frame)
                        self._serial.flush()
                        # Đọc tối thiểu header (4) + CRC (2), sau đó đọc phần còn lại theo LEN
                        header = self._serial.read(4)
                        if len(header) < 4 or header[0] != 0xAA:
                            raise TimeoutError("Header không hợp lệ/timeout")
                        length = header[3]
                        rest = self._serial.read(length + 2)
                        if len(rest) < length + 2:
                            raise TimeoutError("Đọc payload/CRC không đủ")
                        resp = header + rest
                        _, _, payload_out = self._parse_frame(resp)
                    logger.info(
                        "RS485 transact ok",
                        extra={
                            "addr": addr,
                            "cmd": cmd,
                            "len_in": len(payload),
                            "len_out": 0 if payload_out is None else len(payload_out),
                            "correlation_id": getattr(request.state, 'correlation_id', None) if request else None,
                        },
                    )
                    return payload_out
                except Exception as e:
                    logger.warning(
                        "RS485 transact lỗi, retry",
                        extra={
                            "attempt": attempt,
                            "addr": addr,
                            "cmd": cmd,
                            "error": str(e),
                            "correlation_id": getattr(request.state, 'correlation_id', None) if request else None,
                        },
                    )
                    time.sleep(0.01)
        return None


