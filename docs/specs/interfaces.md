# Interfaces hệ thống OHT-50 (Phiên bản 1.0)

## Mục tiêu
- Chuẩn hóa giao diện giữa Master ↔ Slave (RS485) và Master ↔ Center.

## Master ↔ Slave (RS485 / Modbus RTU)
- Tốc độ tham chiếu: 115200 8N1 (có thể cấu hình).
- Địa chỉ thiết bị: 1–32; auto‑discover bằng quét địa chỉ.
- Bản đồ thanh ghi: tham chiếu `docs/technical/final-modbus-register-map.md`.
- Cơ chế retry/timeout/CRC, backoff khi bus nhiễu.

## Master ↔ Center (Backend)
- HTTP: `GET /health`, `GET /status`, `POST /command`, `POST /config/apply`.
- WebSocket: `/telemetry` (realtime: trạng thái, lỗi, logs, metrics).
- Bảo mật: TLS + token; audit trail cấu hình/command.

## Cấu hình runtime (versioned)
- Định dạng JSON/YAML; kiểm tra schema + version trước khi áp dụng.
- Ghi NVM từng module; lưu `last_known_good` để rollback.

### Ví dụ JSON rút gọn
```json
{
  "bus": { "baud": 115200, "parity": "N", "addrs": [1,2,3,4,5] },
  "modules": {
    "1": { "type": "POWER", "chemistry": "LiFePO4", "capacity_Ah": 60 },
    "2": { "type": "DRIVE", "motor": {"kv": 110, "gear": 12.0}, "limits": {"v_max": 1.2, "a_max": 0.8} },
    "3": { "type": "LIFT", "stroke_mm": 500, "v_max": 0.15 },
    "4": { "type": "IO", "inputs": [{"name":"bumper","pin":1,"invert":true}] },
    "5": { "type": "DOCK", "align_thresh_mm": 10, "charge_voltage": 54.6 }
  }
}
```

## Telemetry & Logging
- Telemetry định kỳ và theo sự kiện: trạng thái, lỗi, metrics (latency, CRC fail...).
- Lưu trữ vòng (ring buffer) cục bộ + truyền Center; nhãn thời gian đồng bộ.

## Changelog
- 1.0: Khởi tạo tài liệu, chuẩn hóa endpoints và cấu hình runtime.


