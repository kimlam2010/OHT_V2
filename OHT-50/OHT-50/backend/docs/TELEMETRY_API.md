# Tài liệu API Telemetry (Backend OHT-50)

Phiên bản: v1.0
Ngôn ngữ: Tiếng Việt

## Tổng quan
- Base URL (cục bộ): `http://localhost:8000`
- Base path: `/api/v1/telemetry`
- Định dạng: JSON (HTTP) và JSON line (WebSocket)
- Bao lỗi/bao thành công (envelope):
  - Thành công: `{ "success": true, "data": ... }`
  - Lỗi HTTP: `{ "success": false, "error_code": "HTTP_<code>", "message": "..." }`
- Tham chiếu schema: `docs/specs/telemetry_schema.md`

Ghi chú: Các trường liên quan Location (`tag_id`, `rssi`) có thể xuất hiện theo sự kiện RFID; `enc.count` là tổng xung encoder.

## 1) GET /api/v1/telemetry/current
- Chức năng: Lấy snapshot telemetry hiện tại
- Trả về: `{ success, data }`

Ví dụ phản hồi (rút gọn):
```json
{
  "success": true,
  "data": {
    "ts": 1712345678.123,
    "status": {
      "state": "Move",
      "pos_mm": 1234.56,
      "vel_mms": 350.0,
      "acc_mms2": 200.0,
      "target": { "pos_mm": 1500.0, "vel_mms": 400.0 },
      "safety": { "estop": false, "zone_blocked": false, "interlock_active": false },
      "enc": { "health": "ok", "fault": null, "count": 123456 },
      "errors": [],
      "warn": []
    },
    "mission_id": null,
    "station_id": null,
    "cargo_present": false,
    "cargo_locked": false,
    "cpu_usage": null,
    "memory_usage": null,
    "temperature": null,
    "center_connected": true,
    "rs485_connected": true,
    "tag_id": "TAG-ABC-123",
    "rssi": -55
  }
}
```

## 2) GET /api/v1/telemetry/history?limit={n}
- Chức năng: Lấy danh sách mẫu telemetry gần nhất
- Tham số: `limit` (int, 1..1000, mặc định 100)
- Trả về: `{ success, data: TelemetryData[] }`

Ví dụ phản hồi (một phần tử):
```json
{
  "success": true,
  "data": [
    {
      "ts": 1712345677.000,
      "status": {
        "state": "Idle",
        "pos_mm": 1200.00,
        "vel_mms": 0.0,
        "acc_mms2": 0.0,
        "target": { "pos_mm": 1500.0, "vel_mms": 400.0 },
        "safety": { "estop": false, "zone_blocked": false, "interlock_active": false },
        "enc": { "health": "ok", "fault": null, "count": 123400 },
        "errors": [],
        "warn": []
      },
      "mission_id": null,
      "station_id": null,
      "cargo_present": false,
      "cargo_locked": false,
      "cpu_usage": null,
      "memory_usage": null,
      "temperature": null,
      "center_connected": true,
      "rs485_connected": true
    }
  ]
}
```

## 3) WebSocket /api/v1/telemetry/ws
- Chức năng: Stream telemetry theo thời gian thực
- Tần số phát: theo `ws_hz` runtime (mặc định 10 Hz; có thể đổi qua POST `/rate`)
- Định dạng bản tin: TelemetryData (JSON) mỗi khung

Ví dụ một khung JSON (tương tự `current`):
```json
{
  "ts": 1712345678.223,
  "status": {
    "state": "Move",
    "pos_mm": 1236.00,
    "vel_mms": 200.0,
    "acc_mms2": 150.0,
    "target": { "pos_mm": 1500.0, "vel_mms": 400.0 },
    "safety": { "estop": false, "zone_blocked": false, "interlock_active": false },
    "enc": { "health": "ok", "fault": null, "count": 123470 },
    "errors": [],
    "warn": []
  },
  "center_connected": true,
  "rs485_connected": true
}
```

## 4) POST /api/v1/telemetry/rate?hz={value}
- Chức năng: Cài đặt tần số phát (WS) thời gian thực
- Tham số: `hz` (float, 0.1..50.0; mặc định 10.0)
- Trả về: `{ success: true, hz }`

Ví dụ:
```http
POST /api/v1/telemetry/rate?hz=20
```
```json
{ "success": true, "hz": 20.0 }
```

## 5) POST /api/v1/telemetry/reset
- Chức năng: Reset trạng thái mock và xoá lịch sử
- Trả về: `{ success: true }`

Ví dụ:
```http
POST /api/v1/telemetry/reset
```
```json
{ "success": true }
```

## 6) GET /api/v1/telemetry/stats
- Chức năng: Thống kê nhanh lịch sử gần nhất trong bộ nhớ
- Trả về: `{ success: true, data: { count, last_ts, vel_min, vel_max, vel_avg } }`

Ví dụ:
```json
{
  "success": true,
  "data": {
    "count": 128,
    "last_ts": 1712345679.999,
    "vel_min": -200.0,
    "vel_max": 200.0,
    "vel_avg": 12.34
  }
}
```

## Lưu ý bảo mật và logging
- Mỗi request/response cần ghi log có `correlation_id` (đã tích hợp ở middleware `app/main.py`).
- Áp dụng rate-limit nếu đưa ra internet; hiện tại phục vụ LAN/local dashboard.

## Roadmap (liên quan BE-15)
- Đồng bộ bắt buộc các trường `tag_id`, `enc.count` trong payload `/current` và WS cho môi trường thật; mock có thể phát theo kịch bản RFID.
- Bổ sung filter lịch sử theo thời gian: `/history?since_ts=&until_ts=` (tùy yêu cầu).
