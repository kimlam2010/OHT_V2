# API tối thiểu (Status/Command/Telemetry)

Phiên bản: v1.0 (khởi tạo)

## 1) Endpoints
- GET `/status` → trạng thái hệ thống
- POST `/command` → gửi lệnh (enable/disable/move/dock/reset_fault)
- WS `/telemetry` → stream telemetry

## 2) Mẫu dữ liệu
- Status:
```json
{
  "state": "Idle|Move|Dock|Fault|EStop",
  "enabled": true,
  "pos_mm": 0.0,
  "vel_mms": 0.0,
  "fault": null,
  "safety": {"estop": false}
}
```
- Command:
```json
{
  "action": "enable|disable|move|dock|reset_fault",
  "args": {"target_pos_mm": 1200.0, "vel_mms": 300.0}
}
```
- Error:
```json
{ "error": "BAD_REQUEST", "detail": "missing target_pos_mm" }
```

## 3) Mã lỗi
- BAD_REQUEST, FORBIDDEN (interlock), CONFLICT (state không phù hợp), INTERNAL

## 4) Bảo mật tối thiểu
- Token đơn giản (bearer) nếu cần; CORS cấu hình rõ

## 5) Giới hạn tốc độ
- Chống spam lệnh; queue lệnh với timestamp
