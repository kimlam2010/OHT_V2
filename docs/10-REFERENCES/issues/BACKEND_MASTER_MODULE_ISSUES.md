# Backend Team Issues - Master Module Implementation

## 📋 **Tổng quan**
Issues cho Backend team để implement Master Module backend services và API.

**Ngày tạo:** 2025-01-27  
**Priority:** High  
**Team:** Backend  
**Sprint:** Master Module v3.0

---

## 🎯 **Issue #BE-001: Master Module REST API**

### **Mô tả:**
Implement REST API cho Master Module configuration và control.

### **Requirements:**
- [ ] LED status API endpoints
- [ ] E-Stop status API endpoints
- [ ] Relay control API endpoints
- [ ] System status API endpoints
- [ ] Network status API endpoints
- [ ] Module management API endpoints

### **Tasks:**
1. **API Design:**
   - [ ] API specification (OpenAPI/Swagger)
   - [ ] Endpoint design
   - [ ] Request/response models
   - [ ] Error handling
   - [ ] Authentication/authorization

2. **Implementation:**
   - [ ] LED control endpoints
   - [ ] E-Stop status endpoints
   - [ ] Relay control endpoints
   - [ ] System status endpoints
   - [ ] Network status endpoints
   - [ ] Module management endpoints

3. **Integration:**
   - [ ] Firmware integration
   - [ ] Database integration
   - [ ] Logging integration
   - [ ] Security integration
   - [ ] Monitoring integration

### **Acceptance Criteria:**
- [ ] All API endpoints working
- [ ] API documentation complete
- [ ] Authentication working
- [ ] Error handling working
- [ ] Unit tests passed
- [ ] Integration tests passed

### **Estimate:** 1.5 weeks
### **Priority:** High

---

## 🎯 **Issue #BE-002: WebSocket Real-time Communication**

### **Mô tả:**
Implement WebSocket server cho real-time communication với frontend.

### **Requirements:**
- [ ] WebSocket server implementation
- [ ] Real-time status updates
- [ ] Event-driven communication
- [ ] Connection management
- [ ] Message routing
- [ ] Error handling

### **Tasks:**
1. **WebSocket Server:**
   - [ ] WebSocket server setup
   - [ ] Connection handling
   - [ ] Message handling
   - [ ] Event broadcasting
   - [ ] Error handling

2. **Real-time Features:**
   - [ ] LED status updates
   - [ ] E-Stop status updates
   - [ ] Relay status updates
   - [ ] System status updates
   - [ ] Network status updates
   - [ ] Module status updates

3. **Integration:**
   - [ ] Firmware integration
   - [ ] Event system integration
   - [ ] Monitoring integration
   - [ ] Security integration

### **Acceptance Criteria:**
- [ ] WebSocket server working
- [ ] Real-time updates working
- [ ] Connection management working
- [ ] Error handling working
- [ ] Unit tests passed
- [ ] Integration tests passed

### **Estimate:** 1 week
### **Priority:** High

---

## 🎯 **Issue #BE-003: Module Management Service**

### **Mô tả:**
Implement module management service cho auto-discovery và configuration.

### **Requirements:**
- [ ] Module auto-discovery service
- [ ] Module configuration management
- [ ] Module status monitoring
- [ ] Module health check
- [ ] Hot-plug support
- [ ] Module communication

### **Tasks:**
1. **Discovery Service:**
   - [ ] Auto-discovery implementation
   - [ ] Module detection
   - [ ] Module registration
   - [ ] Module identification
   - [ ] Discovery monitoring

2. **Configuration Management:**
   - [ ] JSON configuration parser
   - [ ] Configuration validation
   - [ ] Configuration storage
   - [ ] Configuration reload
   - [ ] Configuration backup

3. **Module Communication:**
   - [ ] RS485 communication
   - [ ] Modbus RTU protocol
   - [ ] Module commands
   - [ ] Module responses
   - [ ] Error handling

### **Acceptance Criteria:**
- [ ] Auto-discovery working
- [ ] Configuration management working
- [ ] Module communication working
- [ ] Hot-plug support working
- [ ] Unit tests passed
- [ ] Integration tests passed

### **Estimate:** 2 weeks
### **Priority:** High

---

## 🎯 **Issue #BE-004: Telemetry & Logging System**

### **Mô tả:**
Implement telemetry và logging system cho Master Module.

### **Requirements:**
- [ ] Telemetry data collection
- [ ] Real-time logging
- [ ] Data storage
- [ ] Data analysis
- [ ] Performance monitoring
- [ ] Error tracking

### **Tasks:**
1. **Telemetry System:**
   - [ ] Data collection implementation
   - [ ] Data processing
   - [ ] Data storage
   - [ ] Data retrieval
   - [ ] Data analysis

2. **Logging System:**
   - [ ] Log collection
   - [ ] Log storage
   - [ ] Log search
   - [ ] Log analysis
   - [ ] Log retention

3. **Monitoring:**
   - [ ] Performance monitoring
   - [ ] Error tracking
   - [ ] Alert system
   - [ ] Dashboard integration
   - [ ] Reporting

### **Acceptance Criteria:**
- [ ] Telemetry collection working
- [ ] Logging system working
- [ ] Data storage working
- [ ] Monitoring working
- [ ] Unit tests passed
- [ ] Integration tests passed

### **Estimate:** 1.5 weeks
### **Priority:** Medium

---

## 🎯 **Issue #BE-005: Security & Authentication**

### **Mô tả:**
Implement security và authentication cho Master Module.

### **Requirements:**
- [ ] SSL/TLS encryption
- [ ] API authentication
- [ ] User authorization
- [ ] Session management
- [ ] Security monitoring
- [ ] Access control

### **Tasks:**
1. **Security Implementation:**
   - [ ] SSL/TLS setup
   - [ ] Certificate management
   - [ ] Encryption implementation
   - [ ] Security headers
   - [ ] CORS configuration

2. **Authentication:**
   - [ ] API key authentication
   - [ ] Token-based authentication
   - [ ] Session management
   - [ ] Password management
   - [ ] Multi-factor authentication

3. **Authorization:**
   - [ ] Role-based access control
   - [ ] Permission management
   - [ ] Access control lists
   - [ ] Security policies
   - [ ] Audit logging

### **Acceptance Criteria:**
- [ ] SSL/TLS working
- [ ] Authentication working
- [ ] Authorization working
- [ ] Security monitoring working
- [ ] Unit tests passed
- [ ] Security tests passed

### **Estimate:** 1.5 weeks
### **Priority:** High

---

## 🎯 **Issue #BE-006: System Integration & Testing**

### **Mô tả:**
Integrate tất cả backend components và perform system testing.

### **Requirements:**
- [ ] Component integration
- [ ] End-to-end testing
- [ ] Performance testing
- [ ] Security testing
- [ ] Load testing

### **Tasks:**
1. **System Integration:**
   - [ ] Component integration
   - [ ] Interface integration
   - [ ] Data flow integration
   - [ ] Error handling integration
   - [ ] Performance optimization

2. **Testing:**
   - [ ] Unit testing
   - [ ] Integration testing
   - [ ] System testing
   - [ ] Performance testing
   - [ ] Security testing
   - [ ] Load testing

3. **Validation:**
   - [ ] Requirements validation
   - [ ] Performance validation
   - [ ] Security validation
   - [ ] Reliability validation
   - [ ] Documentation validation

### **Acceptance Criteria:**
- [ ] System integration completed
- [ ] All tests passed
- [ ] Performance requirements met
- [ ] Security requirements met
- [ ] Reliability requirements met
- [ ] Documentation completed

### **Estimate:** 1.5 weeks
### **Priority:** High

---

## 📊 **Sprint Planning**

### **Sprint 1 (Week 1-2):**
- [ ] Issue #BE-001: REST API
- [ ] Issue #BE-002: WebSocket communication

### **Sprint 2 (Week 3-4):**
- [ ] Issue #BE-003: Module management service
- [ ] Issue #BE-004: Telemetry & logging

### **Sprint 3 (Week 5-6):**
- [ ] Issue #BE-005: Security & authentication
- [ ] Issue #BE-006: System integration & testing

### **Total Estimate:** 6 weeks
### **Team Size:** 2-3 engineers

---

## 🔧 **Resources Required**

### **Hardware:**
- Development server
- Network equipment
- Test equipment

### **Software:**
- Node.js/Python development environment
- Database (PostgreSQL/MongoDB)
- Redis for caching
- Testing frameworks
- Monitoring tools

### **Documentation:**
- API documentation
- System architecture
- Test procedures
- User manuals

---

## 📋 **Definition of Done**

### **Code:**
- [ ] Source code implemented
- [ ] API endpoints completed
- [ ] Unit tests written
- [ ] Code review completed

### **Testing:**
- [ ] Unit tests passed
- [ ] Integration tests passed
- [ ] System tests passed
- [ ] Performance tests passed
- [ ] Security tests passed

### **Documentation:**
- [ ] API documentation updated
- [ ] Code comments added
- [ ] Test procedures written
- [ ] User manual updated

### **Quality:**
- [ ] Code quality standards met
- [ ] Performance requirements met
- [ ] Security requirements met
- [ ] Reliability requirements met

---

## 🎯 **Issue #BE-007: Align System Status Endpoint**

### **Mô tả:**
Docs `docs/specs/interfaces.md` và `docs/dev_radxa/11_ui_local_dashboard.md` tham chiếu `GET /status`, nhưng backend hiện chỉ cung cấp `GET /api/v1/telemetry/current`. Cần cung cấp alias endpoint hoặc cập nhật tài liệu.

### **Requirements:**
- [ ] Thêm `GET /api/v1/status` (alias, trả về `TelemetryStatus` rút gọn hoặc `TelemetryData` tùy quyết định)
- [ ] Hoặc cập nhật docs để dùng `GET /api/v1/telemetry/current` (giữ backward-compat bằng alias tối thiểu)
- [ ] Cập nhật OpenAPI và docs `TELEMETRY_API.md`

### **Acceptance Criteria:**
- [ ] FE gọi status trả về thành công
- [ ] Docs và triển khai đồng bộ

### **Estimate:** 0.5 ngày
### **Priority:** High

---

## 🎯 **Issue #BE-008: Implement Control Command API**

### **Mô tả:**
FE đang gọi `POST /api/v1/control/command` (`frontend/src/services/control.ts`) nhưng backend chưa có router `control`. Cần implement API lệnh control tối thiểu theo UI docs (`moveTo/stop/dock/undock/...`) với guard an toàn.

### **Requirements:**
- [ ] Tạo router `app/api/v1/control.py`
- [ ] `POST /api/v1/control/command` body `{ command: string, ...params }`
- [ ] Validate theo state machine và giới hạn an toàn
- [ ] Audit log + correlation id

### **Acceptance Criteria:**
- [ ] FE gửi lệnh nhận 200 hoặc 4xx hợp lệ khi bị chặn
- [ ] Test unit/integ cơ bản pass

### **Estimate:** 1–2 ngày
### **Priority:** High

---

## 🎯 **Issue #BE-009: Add Config Apply Endpoint**

### **Mô tả:**
`docs/specs/interfaces.md` tham chiếu `POST /config/apply` nhưng backend chỉ có `PUT /api/v1/config`, `POST /import`, `POST /validate`, `POST /rollback`.

### **Requirements:**
- [ ] Thêm `POST /api/v1/config/apply` (safe-apply + rollback guard) → wrap service `update_config`
- [ ] Cập nhật docs API `CONFIG_API.md` và interfaces

### **Acceptance Criteria:**
- [ ] Endpoint apply hoạt động với validate trước khi áp dụng
- [ ] Docs đồng bộ

### **Estimate:** 0.5–1 ngày
### **Priority:** Medium

---

## 🎯 **Issue #BE-010: Extend Telemetry Safety With Latched**

### **Mô tả:**
UI spec (`docs/dev_radxa/11_ui_local_dashboard.md`) kỳ vọng `safety.latched`. Schema hiện tại có `estop`, `zone_blocked`, `interlock_active` (không có `latched`).

### **Requirements:**
- [ ] Mở rộng `TelemetrySafety` thêm `latched: bool`
- [ ] Cập nhật `backend/app/models/telemetry.py` và producer
- [ ] Cập nhật `docs/specs/telemetry_schema.md` và `TELEMETRY_API.md`

### **Acceptance Criteria:**
- [ ] WS/HTTP trả về trường `latched`
- [ ] UI hiển thị chính xác

### **Estimate:** 0.5 ngày
### **Priority:** Medium

---

## 🎯 **Issue #BE-011: Fix README WebSocket Path Mismatch**

### **Mô tả:**
`backend/README.md` ghi `WebSocket /ws/telemetry` trong khi code dùng `/api/v1/telemetry/ws`.

### **Tasks:**
- [ ] Cập nhật README cho đúng đường dẫn WS
- [ ] Kiểm tra ví dụ script/smoke tests

### **Priority:** Low
### **Estimate:** 0.25 ngày

## 🎯 **Issue #BE-012: Add Config Spec Documentation**

### **Mô tả:**
`backend/TASK_BE-01_COMPLETED.md` tham chiếu `docs/specs/config_spec.md` nhưng file chưa tồn tại. Cần bổ sung spec cấu hình chính thức và liên kết từ `interfaces.md`/`CONFIG_API.md`.

### **Tasks:**
- [ ] Tạo `docs/specs/config_spec.md` (schema, versioning, validation rules)
- [ ] Đồng bộ với `backend/app/models/config.py`
- [ ] Cập nhật liên kết chéo trong tài liệu

### **Priority:** Medium
### **Estimate:** 0.5 ngày
