---
title: "Readme"
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

# API Specifications - OHT-50 Backend

## 📋 Tổng quan
Thư mục này chứa đặc tả chi tiết cho tất cả API endpoints của OHT-50 Backend.

## 📁 Cấu trúc

| File | Mô tả | Endpoints chính |
|------|-------|-----------------|
| [CONFIG_API.md](./CONFIG_API.md) | API quản lý cấu hình hệ thống | `GET/PUT /api/v1/config/`, `POST /api/v1/config/validate` |
| [TELEMETRY_API.md](./TELEMETRY_API.md) | API telemetry và monitoring | `GET /api/v1/telemetry/current`, `GET /api/v1/telemetry/history` |
| [WEBSOCKET_EVENTS.md](./WEBSOCKET_EVENTS.md) | WebSocket events và streaming | `WS /api/v1/telemetry/stream`, `POST /api/v1/telemetry/rate` |
| [INTEGRATION_CENTER.md](./INTEGRATION_CENTER.md) | Tích hợp với Center system | `POST /api/v1/center/mission`, `GET /api/v1/center/modules` |

## 🔗 Liên kết quan trọng

- **OpenAPI UI**: `http://localhost:8000/docs`
- **Testing Tools**: `../tools/` - Postman collection, Insomnia workspace
- **Error Handling**: `../ERRORS_CONVENTION.md`
- **Security**: `../operational/SECURITY_GUIDE.md`

## 📊 API Coverage

### ✅ Implemented
- Configuration Management (CRUD operations)
- Telemetry Data (current, history, streaming)
- Center Integration (mission, permission, status)
- Health Monitoring (basic, detailed)

### 🔄 Planned
- Authentication & Authorization
- User Management
- Audit Logging
- Advanced Analytics

## 🧪 Testing

### Quick Test
```bash
# Test health endpoint
curl http://localhost:8000/health/

# Test config API
curl http://localhost:8000/api/v1/config/

# Test telemetry
curl http://localhost:8000/api/v1/telemetry/current
```

### Advanced Testing
- Import `../tools/postman_collection.json` vào Postman
- Import `../tools/INSOMNIA_WORKSPACE.json` vào Insomnia
- Chạy `../tools/curl_suite.sh` cho automated testing

## 📝 Maintenance

### Cập nhật API Specs
1. Thêm endpoint mới → Cập nhật file tương ứng
2. Thay đổi response format → Cập nhật examples
3. Thêm validation → Cập nhật error codes
4. Thay đổi authentication → Cập nhật security guide

### Validation
- Verify với OpenAPI schema tại `/docs`
- Test với Postman/Insomnia collections
- Update cross-references trong docs khác
