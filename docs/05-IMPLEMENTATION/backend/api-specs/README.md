# API Specifications - OHT-50 Backend v2.0

## 📋 Tổng quan
Thư mục này chứa đặc tả chi tiết cho tất cả API endpoints của OHT-50 Backend - Architecture v2.0 với 5 mandatory modules.

## 📁 Cấu trúc

| File | Mô tả | Endpoints chính | Status |
|------|-------|-----------------|--------|
| [CONFIG_API.md](./CONFIG_API.md) | API quản lý cấu hình hệ thống | `GET/PUT /api/v1/config/`, `POST /api/v1/config/validate` | ✅ v2.0 |
| [TELEMETRY_API.md](./TELEMETRY_API.md) | API telemetry và monitoring | `GET /api/v1/telemetry/current`, `GET /api/v1/telemetry/history` | ✅ Complete |
| [WEBSOCKET_EVENTS.md](./WEBSOCKET_EVENTS.md) | WebSocket events và streaming | `WS /api/v1/telemetry/stream`, `POST /api/v1/telemetry/rate` | ✅ Complete |
| [POWER_API.md](./POWER_API.md) | Power module management (24V system) | `GET /api/v1/power/status`, `GET /api/v1/power/battery` | ✅ v2.0 |
| [SAFETY_API.md](./SAFETY_API.md) | Safety module management | `GET /api/v1/safety/status`, `POST /api/v1/safety/estop/trigger` | ✅ v2.0 |
| [DOCK_LOCATION_API.md](./DOCK_LOCATION_API.md) | Dock & Location module (LiDAR USB) | `GET /api/v1/dock-location/status`, `POST /api/v1/dock-location/navigate` | ✅ v2.0 |
| [MASTER_CONTROL_API.md](./MASTER_CONTROL_API.md) | Master Control module coordination | `GET /api/v1/master-control/status`, `POST /api/v1/master-control/missions` | ✅ v2.0 |
| [DC_MOTOR_API.md](./DC_MOTOR_API.md) | DC motor control (12V motors) | `GET /api/v1/dc-motor/status`, `POST /api/v1/dc-motor/control` | ✅ Complete |
| [INTEGRATION_CENTER.md](./INTEGRATION_CENTER.md) | Tích hợp với Center system | `POST /api/v1/center/mission`, `GET /api/v1/center/modules` | ✅ Complete |

## 🔗 Liên kết quan trọng

- **OpenAPI UI**: `http://localhost:8000/docs`
- **Testing Tools**: `../tools/` - Postman collection, Insomnia workspace
- **Error Handling**: `../ERRORS_CONVENTION.md`
- **Security**: `../operational/SECURITY_GUIDE.md`

## 📊 Module Addresses
| Module | Address | API Endpoint | Status |
|--------|---------|--------------|--------|
| Power | 0x01 | `/power` | ✅ v2.0 |
| Safety | 0x02 | `/safety` | ✅ v2.0 |
| Travel Motor | 0x03 | `/dc-motor` | ✅ Complete |
| Dock & Location | 0x04 | `/dock-location` | ✅ v2.0 |
| Master Control | 0x05 | `/master-control` | ✅ v2.0 |

## 🔧 Architecture v2.0 Features
- **5 Mandatory Modules:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **RS485 Communication:** Standardized cho tất cả modules
- **LiDAR USB Integration:** RPLIDAR A1M8 qua USB 2.0
- **24V Nominal System:** Standardized voltage specifications
- **Real-time Coordination:** Master Control Module orchestration
- **Safety Integration:** E-Stop, zones, monitoring
- **Mission Management:** Complete mission lifecycle
- **State Machine:** System state management

## 📊 API Coverage

### ✅ Implemented (v2.0)
- Configuration Management (CRUD operations)
- Telemetry Data (current, history, streaming)
- Power Module Management (24V system)
- Safety Module Management (E-Stop, zones)
- Dock & Location Module (LiDAR USB, navigation)
- Master Control Module (coordination, missions)
- DC Motor Control (12V motors)
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
