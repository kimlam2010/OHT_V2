---
title: "Errors Convention"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "Backend Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['software-development']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# ERRORS CONVENTION - OHT-50 Backend (v0.3)

## Mục lục
- [Envelope & Handlers](#envelope--handlers)
- [Bảng mapping phổ biến](#bảng-mapping-phổ-biến)
- [Ví dụ phản hồi lỗi](#ví-dụ-phản-hồi-lỗi)
- [Tham chiếu](#tham-chiếu)

---

## Envelope & Handlers
- ValidationError (422): `{ success: false, error_code: "VALIDATION_FAILED", message: "Dữ liệu không hợp lệ", details: [...] }`
- HTTPException: `{ success: false, error_code: "HTTP_<status>", message: "..." }`
- OHT50Exception (400): `{ success: false, error_code: "<CODE>", message: "...", details: {...} }`
- General (500): `{ success: false, error_code: "INTERNAL_SERVER_ERROR", message: "Lỗi nội bộ server" }`

## Bảng mapping phổ biến
| Tình huống | Mã | error_code | Ghi chú |
|---|---|---|---|
| Dữ liệu body sai schema | 422 | VALIDATION_FAILED | details chứa danh sách lỗi |
| Không có token | 401 | HTTP_401 | Khi DEBUG=false |
| Không tìm thấy cấu hình | 404 | HTTP_404 | Ví dụ GET version không tồn tại |
| Cấu hình không hợp lệ | 400 | CONFIG_INVALID | Từ service raise OHT50Exception |
| Lỗi hệ thống | 500 | INTERNAL_SERVER_ERROR | Bắt ở general handler |

## Ví dụ phản hồi lỗi
```json
{ "success": false, "error_code": "HTTP_404", "message": "Configuration version 999 not found" }
```
```json
{ "success": false, "error_code": "VALIDATION_FAILED", "message": "Dữ liệu không hợp lệ", "details": [{"loc": ["body","telemetry","ws_hz"], "msg": "value is not a valid integer"}] }
```

## Tham chiếu
- Handlers: `backend/app/main.py`
- Error codes: `backend/app/core/exceptions.py`
