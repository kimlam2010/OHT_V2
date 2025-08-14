# Schema Telemetry & Logging

Phiên bản: v1.0 (khởi tạo)

## 1) Kênh & định dạng
- WS `/telemetry`: JSON line
- HTTP `/status`: snapshot JSON
- Logging: JSON với severity

## 2) Trường dữ liệu (ví dụ)
```json
{
  "ts": 1712345678.123,
  "state": "Move",
  "pos_mm": 1234.56,
  "vel_mms": 350.0,
  "acc_mms2": 200.0,
  "target": {"pos_mm": 1500.0, "vel_mms": 400.0},
  "safety": {"estop": false, "zone_blocked": false},
  "enc": {"health": "ok", "fault": null, "count": 123456},
  "tag_id": "TAG-ABC-123",           
  "rssi": -55,                        
  "errors": [],
  "warn": []
}
```

## 3) Mức severity
- DEBUG < INFO < WARN < ERROR < FATAL
- Quy tắc phát: ERROR khi mất điều khiển/timeout; FATAL cho E‑Stop phần mềm

## 4) Tối ưu băng thông
- Tốc độ phát WS: 10–20 Hz (cấu hình)
- Gộp trường ít thay đổi; mã hoá gzip nếu cần

## 5) Truy vết sự kiện an toàn
- Sự kiện: E‑Stop, FaultCode thay đổi, reset fault, enable/disable driver
- Lưu bộ sự kiện gần nhất N mục (cấu hình)

## 6) Ghi chú Location (RFID + Encoder)
- `tag_id` phát theo sự kiện (chỉ xuất hiện khi có tag mới), `rssi` tuỳ chọn.
- `enc.count` là tổng xung encoder, được dùng nội suy quãng đường giữa các tag.
- Trường `s_on_rail` nếu được tính ở HAL có thể thêm vào payload.
