# 08) Ghi log & Telemetry

## API cục bộ (tối thiểu)
- HTTP:
  - `GET /health`: tình trạng tiến trình, phiên bản, thời gian chạy.
  - `GET /status`: state machine, vị trí `s_on_rail`, feedback trục, I/O, an toàn.
  - `POST /command`: lệnh đơn giản (vd: moveTo, stop, doorOpen/Close) — validate đầu vào.
- WebSocket:
  - `/telemetry`: đẩy 10–50 Hz các trường theo `interfaces.md`.
- Gợi ý triển khai: FastAPI + uvicorn; không chặn vòng điều khiển (sử dụng queue và tác vụ async riêng).

## Logging
- Mức: DEBUG/INFO/WARN/ERROR; định dạng JSON dòng.
- Luồng: file `/var/log/oht50/*.log`, rotate theo dung lượng.
- Nội dung bắt buộc: timestamp, state, events, commands, faults, RTT bus, CRC errors.

## Telemetry
- Tần số 10–50 Hz; payload theo `interfaces.md` (s_on_rail, axes, safety, io, mission).
- Batch nén (tùy chọn) nếu băng thông hẹp.

## Chẩn đoán
- Counters: `crc_fail`, `retry_count`, `timeout_count`, `bus_rtt_ms`.
- Healthcheck endpoint local: `GET /health`.

## TBD
- Format log cụ thể và retention.
