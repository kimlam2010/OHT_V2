# 03) Driver RS485 (frame, CRC, timeout, retry)

## Serial
- Thông số: baud 115200 (khởi đầu), 8N1, timeout đọc 30 ms (TBD), retry 2 (TBD).
- Half‑duplex: điều khiển DE/RE (GPIO) hoặc auto‑direction phần cứng.

## Khung bản tin
- `[0xAA][ADDR][CMD][LEN][PAYLOAD...][CRC16]` (tham chiếu `bus_rs485.md`).
- CRC16: CCITT/Modbus (TBD) — chọn 1 và dùng thống nhất.

## CRC16 (Python, ví dụ CCITT)
```python
CRC16_TABLE = [...]  # tiền tính hoặc dùng thư viện

def crc16_ccitt(data: bytes, seed: int = 0xFFFF) -> int:
    crc = seed
    for b in data:
        idx = ((crc >> 8) ^ b) & 0xFF
        crc = ((crc << 8) ^ CRC16_TABLE[idx]) & 0xFFFF
    return crc
```

## Trình tự gửi/nhận
1. Build frame; bật DE; gửi; tắt DE.
2. Chờ phản hồi trong `timeout_ms`.
3. Kiểm CRC; nếu fail → retry tối đa `retry` lần; sau đó báo lỗi module.

## Quản lý bus
- Lock bus theo request để tránh chồng xung.
- Heartbeat: mỗi module ping định kỳ, cập nhật `module_registry`.
- Backoff ngắn giữa lần gửi (ví dụ 1–2 ms) để nhường bus.

## Chẩn đoán & đo đạc
- Ghi lại RTT, lỗi CRC, số lần retry; metric để tối ưu baud/timeout.

## TBD
- Chọn CRC chuẩn, bảng CRC, chân GPIO DE/RE.
