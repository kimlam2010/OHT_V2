# Plan Tiến độ API Integration Implementation - OHT-50 Master Module

**Phiên bản:** 2.0.0  
**Ngày cập nhật:** 2025-01-28  
**Team:** FW  
**Mức độ ưu tiên:** CRITICAL  
**Trạng thái:** INTEGRATED WITH PROJECT SLC PLANNING

## 🔗 **INTEGRATION WITH PROJECT SLC PLANNING**

**Related Document:** `/docs/03-ARCHITECTURE/architecture/SLC_PLANNING_OHT-50.md`

### **Project SLC Integration:**
- **Project Phase 3:** Implementation (In Progress - Module Data API)
- **Firmware Sprint 1-3:** Aligned với Project Sprint 1-3
- **Timeline:** 6 weeks firmware implementation
- **Scope:** Module Data API implementation

---

## 📋 **Tổng quan Plan**

### **Mục tiêu**
Implement lại hoàn toàn hệ thống API cho OHT-50 Master Module theo đúng yêu cầu và kiến trúc đã định nghĩa.

### **Phạm vi**
- Core API Manager implementation
- HTTP REST API endpoints
- WebSocket real-time communication
- Integration với core systems
- Module-specific endpoints
- Security và authentication

---

## 🚨 **CRITICAL ISSUE IDENTIFIED - IMMEDIATE ACTION REQUIRED**

### **Issue: API Manager NOT Integrated vào main.c**
- **Problem:** API Manager implemented but not used in main.c
- **Root Cause:** main.c still uses Communication Manager API placeholders
- **Impact:** FW cannot communicate with BE, blocking development
- **Priority:** CRITICAL
- **Timeline:** 3 days to fix

---

## 🎯 **Phase 1: CRITICAL FIX - API Manager Integration (IMMEDIATE)**
**ALIGNED WITH PROJECT SPRINT 1: CRITICAL FIX - FW-BE Communication**

### **Task 1.1: Integrate API Manager vào main.c**
**Thời gian:** 1 ngày  
**Responsible:** FW Team  
**Dependencies:** None  
**Project Sprint:** Sprint 1 - CRITICAL FIX
**Priority:** CRITICAL

**Deliverables:**
- **CRITICAL:** Replace Communication Manager API calls trong main.c
- **CRITICAL:** Add API Manager init/start vào main.c
- **CRITICAL:** Update main loop với API Manager processing
- **CRITICAL:** Remove API placeholder functions
- **CRITICAL:** Enable real HTTP/WebSocket servers

**Acceptance Criteria:**
- [ ] **CRITICAL:** FW starts without errors
- [ ] **CRITICAL:** Port 8080 actually listening (not simulated)
- [ ] **CRITICAL:** Port 8081 actually listening (not simulated)
- [ ] **CRITICAL:** API endpoints respond to HTTP requests
- [ ] **CRITICAL:** WebSocket connections work
- [ ] **CRITICAL:** BE can communicate with FW
- [ ] **CRITICAL:** End-to-end communication validated
- [ ] Basic error handling

### **Task 1.2: HTTP Server Foundation**
**Thời gian:** 4 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 1.1

**Deliverables:**
- `src/app/http_server.h` - HTTP Server interface
- `src/app/http_server.c` - HTTP Server implementation
- Basic HTTP request/response handling
- Route management system
- Port 8080 configuration

**Acceptance Criteria:**
- [ ] HTTP server start/stop hoạt động
- [ ] Route registration system hoạt động
- [ ] Basic request/response handling
- [ ] Port 8080 accessible

### **Task 1.3: WebSocket Server Foundation**
**Thời gian:** 3 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 1.1

**Deliverables:**
- `src/app/websocket_server.h` - WebSocket Server interface
- `src/app/websocket_server.c` - WebSocket Server implementation
- WebSocket protocol handling
- Client connection management
- Port 8081 configuration

**Acceptance Criteria:**
- [ ] WebSocket server start/stop hoạt động
- [ ] Client connection management hoạt động
- [ ] Basic message handling
- [ ] Port 8081 accessible

---

## 🎯 **Phase 2: Core System Integration (Week 3-4)**
**ALIGNED WITH PROJECT SPRINT 2: WebSocket & Real-time**

### **Task 2.1: System Controller Integration**
**Thời gian:** 3 ngày  
**Responsible:** FW Team  
**Dependencies:** Phase 1 complete  
**Project Sprint:** Sprint 2 - WebSocket & Real-time

**Deliverables:**
- Integration với `system_controller.h`
- System status endpoints
- System health endpoints
- Real-time system data

**Acceptance Criteria:**
- [ ] `/api/v1/system/status` endpoint hoạt động
- [ ] `/api/v1/system/health` endpoint hoạt động
- [ ] Real system data thay vì mock data
- [ ] Error handling cho system failures

### **Task 2.2: Safety Manager Integration**
**Thời gian:** 3 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 2.1

**Deliverables:**
- Integration với `safety_manager.h`
- Safety status endpoints
- E-Stop handling
- Safety configuration endpoints

**Acceptance Criteria:**
- [ ] `/api/v1/safety/status` endpoint hoạt động
- [ ] `/api/v1/safety/estop` endpoint hoạt động
- [ ] Real safety data thay vì mock data
- [ ] E-Stop trigger hoạt động

### **Task 2.3: Module Manager Integration**
**Thời gian:** 4 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 2.2

**Deliverables:**
- Integration với `module_manager.h`
- Module discovery endpoints
- Module status endpoints
- Module list management

**Acceptance Criteria:**
- [ ] `/api/v1/modules/list` endpoint hoạt động
- [ ] `/api/v1/modules/{id}/status` endpoint hoạt động
- [ ] Real module data thay vì mock data
- [ ] Module discovery hoạt động

---

## 🎯 **Phase 3: Module-Specific Endpoints (Week 5-6)**
**ALIGNED WITH PROJECT SPRINT 3: Security & Performance**

### **Task 3.1: Power Module Endpoints**
**Thời gian:** 4 ngày  
**Responsible:** FW Team  
**Dependencies:** Phase 2 complete  
**Project Sprint:** Sprint 3 - Security & Performance

**Deliverables:**
- Integration với `power_module_handler.h`
- Power status endpoints
- Battery monitoring endpoints
- Charging control endpoints

**Acceptance Criteria:**
- [ ] `/api/v1/modules/power/status` endpoint hoạt động
- [ ] `/api/v1/modules/power/battery` endpoint hoạt động
- [ ] `/api/v1/modules/power/charging` endpoint hoạt động
- [ ] Real power data thay vì mock data

### **Task 3.2: Safety Module Endpoints**
**Thời gian:** 3 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 3.1

**Deliverables:**
- Integration với `safety_module_handler.h`
- Safety sensor endpoints
- Safety relay endpoints
- Safety zone endpoints

**Acceptance Criteria:**
- [ ] `/api/v1/modules/safety/sensors` endpoint hoạt động
- [ ] `/api/v1/modules/safety/relays` endpoint hoạt động
- [ ] `/api/v1/modules/safety/zones` endpoint hoạt động
- [ ] Real safety sensor data

### **Task 3.3: Motor Module Endpoints**
**Thời gian:** 4 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 3.2

**Deliverables:**
- Integration với `travel_motor_module_handler.h`
- Motor status endpoints
- Motor control endpoints
- Motor position/velocity endpoints

**Acceptance Criteria:**
- [ ] `/api/v1/modules/motor/status` endpoint hoạt động
- [ ] `/api/v1/modules/motor/control` endpoint hoạt động
- [ ] `/api/v1/modules/motor/position` endpoint hoạt động
- [ ] Real motor data và control

### **Task 3.4: Dock Module Endpoints**
**Thời gian:** 3 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 3.3

**Deliverables:**
- Integration với `dock_module_handler.h`
- Dock status endpoints
- Dock control endpoints
- Docking sequence endpoints

**Acceptance Criteria:**
- [ ] `/api/v1/modules/dock/status` endpoint hoạt động
- [ ] `/api/v1/modules/dock/control` endpoint hoạt động
- [ ] `/api/v1/modules/dock/sequence` endpoint hoạt động
- [ ] Real dock data và control

---

## 🎯 **Phase 4: Advanced Features (Week 7-8)**

### **Task 4.1: Configuration Management**
**Thời gian:** 4 ngày  
**Responsible:** FW Team  
**Dependencies:** Phase 3 complete

**Deliverables:**
- Configuration get/set endpoints
- Configuration validation
- Configuration persistence
- Configuration export/import

**Acceptance Criteria:**
- [ ] `/api/v1/config/get` endpoint hoạt động
- [ ] `/api/v1/config/set` endpoint hoạt động
- [ ] Configuration validation hoạt động
- [ ] Configuration persistence hoạt động

### **Task 4.2: Telemetry & Real-time Data**
**Thời gian:** 4 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 4.1

**Deliverables:**
- Integration với `telemetry_manager.h`
- Real-time telemetry endpoints
- WebSocket telemetry streaming
- Telemetry data serialization

**Acceptance Criteria:**
- [ ] `/api/v1/telemetry` endpoint hoạt động
- [ ] WebSocket telemetry streaming hoạt động
- [ ] Real-time data transmission
- [ ] JSON serialization hoạt động

### **Task 4.3: Diagnostics & Logging**
**Thời gian:** 3 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 4.2

**Deliverables:**
- Diagnostics endpoints
- System logging endpoints
- Error reporting endpoints
- Performance monitoring

**Acceptance Criteria:**
- [ ] `/api/v1/diagnostics` endpoint hoạt động
- [ ] System logging hoạt động
- [ ] Error reporting hoạt động
- [ ] Performance monitoring hoạt động

---

## 🎯 **Phase 5: Security & Testing (Week 9-10)**

### **Task 5.1: Authentication & Authorization**
**Thời gian:** 4 ngày  
**Responsible:** FW Team  
**Dependencies:** Phase 4 complete

**Deliverables:**
- Basic authentication system
- Request authorization
- Session management
- Security headers

**Acceptance Criteria:**
- [ ] Authentication system hoạt động
- [ ] Request authorization hoạt động
- [ ] Session management hoạt động
- [ ] Security headers implemented

### **Task 5.2: Error Handling & Validation**
**Thời gian:** 3 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 5.1

**Deliverables:**
- Comprehensive error handling
- Input validation
- Response validation
- Error logging

**Acceptance Criteria:**
- [ ] Error handling hoạt động
- [ ] Input validation hoạt động
- [ ] Response validation hoạt động
- [ ] Error logging hoạt động

### **Task 5.3: Integration Testing**
**Thời gian:** 3 ngày  
**Responsible:** FW Team  
**Dependencies:** Task 5.2

**Deliverables:**
- API integration tests
- End-to-end testing
- Performance testing
- Security testing

**Acceptance Criteria:**
- [ ] API integration tests pass
- [ ] End-to-end tests pass
- [ ] Performance requirements met
- [ ] Security requirements met

---

## 📊 **Milestones & Gates**

### **Gate 1: Foundation Complete**
**Criteria:**
- [ ] Phase 1 hoàn thành 100%
- [ ] HTTP server hoạt động
- [ ] WebSocket server hoạt động
- [ ] Basic API Manager hoạt động

### **Gate 2: Core Integration Complete**
**Criteria:**
- [ ] Phase 2 hoàn thành 100%
- [ ] System integration hoạt động
- [ ] Safety integration hoạt động
- [ ] Module integration hoạt động

### **Gate 3: Module Endpoints Complete**
**Criteria:**
- [ ] Phase 3 hoàn thành 100%
- [ ] Tất cả module endpoints hoạt động
- [ ] Real data thay vì mock data
- [ ] Module control hoạt động

### **Gate 4: Advanced Features Complete**
**Criteria:**
- [ ] Phase 4 hoàn thành 100%
- [ ] Configuration management hoạt động
- [ ] Telemetry streaming hoạt động
- [ ] Diagnostics hoạt động

### **Gate 5: Production Ready**
**Criteria:**
- [ ] Phase 5 hoàn thành 100%
- [ ] Security implemented
- [ ] Testing passed
- [ ] Documentation complete

---

## 🚨 **Risk Management**

### **High Risk Items:**
1. **Integration Complexity** - Core systems integration có thể phức tạp
2. **Real-time Performance** - WebSocket streaming có thể chậm
3. **Security Implementation** - Authentication có thể không đủ mạnh

### **Mitigation Strategies:**
1. **Phased Approach** - Implement từng phase một cách cẩn thận
2. **Performance Testing** - Test performance sớm và thường xuyên
3. **Security Review** - Review security implementation ở mỗi phase

---

## 📈 **Success Metrics**

### **Technical Metrics:**
- [ ] 100% endpoints hoạt động
- [ ] Response time < 100ms cho tất cả endpoints
- [ ] WebSocket latency < 50ms
- [ ] 0 critical security vulnerabilities

### **Quality Metrics:**
- [ ] 100% test coverage cho API endpoints
- [ ] 0 memory leaks
- [ ] 0 crash trong 24h continuous operation
- [ ] 100% documentation complete

---

## 📅 **Timeline Summary - INTEGRATED WITH PROJECT SLC**

| Phase | Duration | Start Week | End Week | Project Sprint | Status |
|-------|----------|------------|----------|----------------|--------|
| Phase 1 | 2 weeks | Week 1 | Week 2 | Sprint 1 - Firmware HTTP Server | 🔄 PLANNING |
| Phase 2 | 2 weeks | Week 3 | Week 4 | Sprint 2 - WebSocket & Real-time | ⏳ PENDING |
| Phase 3 | 2 weeks | Week 5 | Week 6 | Sprint 3 - Security & Performance | ⏳ PENDING |
| Phase 4 | 2 weeks | Week 7 | Week 8 | Sprint 4 - Backend Integration | ⏳ PENDING |
| Phase 5 | 2 weeks | Week 9 | Week 10 | Sprint 5 - Frontend Integration | ⏳ PENDING |

**Total Duration:** 10 weeks  
**Target Completion:** End of Week 10  
**Project Integration:** Aligned với Project SLC Planning Sprint 1-5

---

**🚨 Lưu ý:** Plan này phải được thực hiện tuần tự, không được bỏ qua phase nào. Mỗi phase phải hoàn thành 100% trước khi chuyển sang phase tiếp theo.

**🔗 PROJECT INTEGRATION:** Firmware implementation đã được integrate với Project SLC Planning. Timeline và scope đã được align để ensure coordinated development across all teams.

**📋 MODULE DATA API FOCUS:** Implementation focus vào Module Data API theo CTO requirements và Project SLC Planning Sprint 1-3.

---

**Changelog v2.0.0:**
- ✅ Integrated với Project SLC Planning
- ✅ Aligned timeline với Project Sprint 1-5
- ✅ Added Project Sprint mapping cho each phase
- ✅ Updated status và priority
- ✅ Added Module Data API focus
- ✅ Enhanced integration documentation
