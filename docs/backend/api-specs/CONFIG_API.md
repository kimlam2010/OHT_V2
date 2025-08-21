# CONFIG API - OHT-50 Backend (v0.3)

## Mục lục
- [Tổng quan & Auth](#tổng-quan--auth)
- [Models chính](#models-chính)
- [Endpoints](#endpoints)
  - [GET /api/v1/config/](#get-apiv1config)
  - [GET /api/v1/config/history](#get-apiv1confighistory)
  - [GET /api/v1/config/version/{version}](#get-apiv1configversionversion)
  - [PUT /api/v1/config/](#put-apiv1config)
  - [POST /api/v1/config/rollback/{version}](#post-apiv1configrollbackversion)
  - [GET /api/v1/config/export](#get-apiv1configexport)
  - [POST /api/v1/config/import](#post-apiv1configimport)
  - [POST /api/v1/config/validate](#post-apiv1configvalidate)
  - [GET /api/v1/config/schema](#get-apiv1configschema)
- [Lưu ý hành vi](#lưu-ý-hành-vi)
- [Tham chiếu](#tham-chiếu)

---

## Tổng quan & Auth
- Tất cả endpoint có `Depends(verify_token)` → cần header `Authorization: Bearer <token>` khi `DEBUG=false`.
- Khi `DEBUG=true` (dev), có thể gọi không cần token.

## Models chính
- `SystemConfig` (cấu hình đầy đủ): các field chính
  - `version: int`
  - `hardware: { motor, encoder, ... }`
  - `safety: { zones[], estop_reset_policy }`
  - `telemetry: { ws_hz, history_size? }`
  - `comm: { rs485, can }`
- `ConfigHistory`: { version, timestamp, action, user, config_data }
- `ConfigResponse`: { success, message, config?, version? }

Chi tiết schema xem `backend/app/models/config.py` (đã có validator) và JSON Schema từ endpoint `/schema`.

## Endpoints

### GET /api/v1/config/
Trả về trực tiếp `SystemConfig`.
```bash
curl -s http://localhost:8000/api/v1/config/
# Prod (có token)
# curl -s -H 'Authorization: Bearer <TOKEN>' http://<host>:8000/api/v1/config/
```

### GET /api/v1/config/history
Tham số: `limit=1..50` (mặc định 10). Trả về danh sách `ConfigHistory`.
```bash
curl -s 'http://localhost:8000/api/v1/config/history?limit=5'
```

### GET /api/v1/config/version/{version}
Trả về `SystemConfig` theo version, 404 nếu không có.
```bash
curl -s http://localhost:8000/api/v1/config/version/1
```

### PUT /api/v1/config/
Cập nhật cấu hình; service tự tăng version. Query: `user=<name>`.
```bash
# file config.json chứa cấu hình đầy đủ (xem ví dụ mẫu trong models)
curl -s -X PUT 'http://localhost:8000/api/v1/config/?user=alice' \
  -H 'Content-Type: application/json' \
  -d @config.json
# → { "success": true, "message": "...", "config": {"version": 2, ...}, "version": 2 }
```

### POST /api/v1/config/rollback/{version}
Rollback về version cũ; service tạo version mới.
```bash
curl -s -X POST 'http://localhost:8000/api/v1/config/rollback/1?user=alice'
```

### GET /api/v1/config/export
Xuất cấu hình hiện tại dạng JSON hoặc YAML.
```bash
curl -s 'http://localhost:8000/api/v1/config/export?format=json'
curl -s 'http://localhost:8000/api/v1/config/export?format=yaml'
```

### POST /api/v1/config/import
Import cấu hình qua query `content` (string) và `format`.
```bash
# Ví dụ đơn giản (JSON ngắn ngay trong URL-encoded)
curl -s -X POST \
  --data-urlencode 'content={"version":0,"hardware":{"motor":{"interface":"rs485","r":0.05,"vmax_mms":100,"amax_mms2":100,"jmax_mms3":1000},"encoder":{"type":"incremental","resolution_ppr":1000,"invert":false}},"safety":{"zones":[{"start_mm":0,"end_mm":200,"vmax_mms":200}],"estop_reset_policy":"manual"},"telemetry":{"ws_hz":10},"comm":{"rs485":{"device":"/dev/ttyOHT485","baud":115200}}}' \
  'http://localhost:8000/api/v1/config/import?format=json&user=alice'
```

### POST /api/v1/config/validate
Validate cấu hình (không lưu).
```bash
curl -s -X POST 'http://localhost:8000/api/v1/config/validate' \
  -H 'Content-Type: application/json' \
  -d @config.json
# → { "valid": true, "message": "Configuration is valid" }
```

### GET /api/v1/config/schema
Trả về JSON schema của `SystemConfig`.
```bash
curl -s http://localhost:8000/api/v1/config/schema | jq .title
```

## Lưu ý hành vi
- `GET /api/v1/config/` trả về object trực tiếp (không bọc `{ success, data }`).
- `PUT/rollback/import` trả về `ConfigResponse` (envelope có `success`, `version`, ...).
- `import` nhận nội dung qua query `content` (không upload file); dùng `--data-urlencode` cho chuỗi dài.

## Tham chiếu
- Code: `backend/app/api/v1/config.py`, `backend/app/services/config_service.py`
- Spec tổng: `docs/FIRMWARE/01_SPECIFICATIONS/API_SPEC.md`
