# FW TEAM - IMPLEMENTATION CHECKLIST & TEST PLAN

**Phiên bản:** 1.0  
**Ngày tạo:** 2025-01-28  
**Team:** FW Team  
**Trạng thái:** 🚀 ACTIVE DEVELOPMENT  
**Dựa trên:** Source code hiện tại + Requirements docs

---

## 📋 **TỔNG QUAN HIỆN TẠI**

### **✅ ĐÃ HOÀN THÀNH:**
- **HAL Layer:** 100% Complete (GPIO, E-Stop, RS485, LED, Network, Storage)
- **Module Handlers:** 95% Complete (Power, Safety, Travel Motor, Dock, DI/DO)
- **Main Application:** Basic structure với initialization flow
- **Build System:** 100% Success với 15 test executables

### **🔄 CẦN HOÀN THIỆN:**
- **State Machine Logic:** Core state transitions theo REQ_State_Machine.md
- **Module Integration:** Auto-discovery và communication
- **Telemetry System:** Data packing theo REQ_Telemetry_Schema.md
- **API Integration:** Backend communication endpoints

---

## 🎯 **PHASE 1: STATE MACHINE IMPLEMENTATION**

### **1.1 Core State Machine Logic**

#### **Yêu cầu từ REQ_State_Machine.md:**
```c
// Core States cần implement:
- Idle: Chờ lệnh, hệ an toàn OK, location system ready
- Move: Di chuyển theo lệnh (profile v-a-j), giám sát an toàn liên tục
- Navigating: Đang di chuyển đến target position
- Positioning: Đang align với target position
- Dock: Quy trình docking/charging, interlock sạc
- Undocking: Đang thực hiện undocking procedure
- Fault: Lỗi có thể khôi phục
- E-Stop: Khẩn cấp, dừng ngay
```

#### **Source code hiện tại (`system_state_machine.c`):**
```c
// Cần kiểm tra và cập nhật:
- State transitions theo mermaid diagram
- Guard conditions (safety_ok, location_ok, target_valid)
- Event handling (cmd.move, cmd.stop, estop_pressed)
- Timeout handling cho mỗi state
```

#### **Checklist Implementation:**
- [ ] **State Definitions:** Định nghĩa đầy đủ 8 states
- [ ] **Transition Logic:** Implement tất cả transitions theo mermaid
- [ ] **Guard Conditions:** Safety, location, target validation
- [ ] **Event Processing:** Command events và safety events
- [ ] **Timeout Handling:** State timeout cho mỗi state
- [ ] **Error Recovery:** Fault detection và recovery logic

#### **Test Cases:**
```c
// Test state transitions
test_idle_to_move_transition();
test_move_to_navigating_transition();
test_navigating_to_positioning_transition();
test_positioning_to_dock_transition();
test_dock_to_undocking_transition();
test_fault_detection_and_recovery();
test_estop_emergency_stop();
```

---

## 🎯 **PHASE 2: MODULE INTEGRATION**

### **2.1 Auto-Discovery System**

#### **Yêu cầu từ REQ_System_Architecture.md:**
```c
// Module Addresses:
- 0x01: Master Control Module
- 0x02: Power Module (BẮT BUỘC)
- 0x03: Safety Module (BẮT BUỘC)
- 0x04: Travel Motor Module (BẮT BUỘC)
- 0x05: Dock & Location Module (BẮT BUỘC)
- 0x06: Lifter Motor Module (TÙY CHỌN)
- 0x07: Cargo Door Module (TÙY CHỌN)
- 0x08: Safety Extended Module (TÙY CHỌN)
```

#### **Source code hiện tại (`module_registry.c`):**
```c
// Cần kiểm tra và cập nhật:
- Auto-discovery scan range (0x01-0x20)
- Module type detection và validation
- Online/offline status tracking
- Hot-plug support
```

#### **Checklist Implementation:**
- [ ] **Discovery Scan:** Scan range 0x01-0x20 với timeout
- [ ] **Module Validation:** Verify module type và capabilities
- [ ] **Status Tracking:** Online/offline/error status
- [ ] **Hot-Plug Support:** Auto-detect khi module connect/disconnect
- [ ] **Registry Management:** Add/remove/update module info
- [ ] **Error Handling:** Timeout, CRC errors, communication failures

#### **Test Cases:**
```c
// Test auto-discovery
test_module_discovery_scan();
test_module_type_validation();
test_hot_plug_detection();
test_offline_module_handling();
test_communication_timeout();
test_crc_error_handling();
```

### **2.2 Module Communication**

#### **Yêu cầu từ REQ_System_Interfaces.md:**
```c
// Modbus RTU Communication:
- Function Codes: 0x03 (Read), 0x06 (Write), 0x10 (Write Multiple)
- Register Map: 256 registers per module (0x0000-0x00FF)
- Auto-Discovery: Module identification registers (0x00F0-0x00FF)
- Error Handling: CRC validation, timeout, retry mechanism
```

#### **Source code hiện tại (`communication_manager.c`):**
```c
// Cần kiểm tra và cập nhật:
- Modbus RTU implementation
- Register read/write operations
- Error handling và retry logic
- CRC validation
```

#### **Checklist Implementation:**
- [ ] **Modbus RTU:** Complete function code support
- [ ] **Register Operations:** Read/write single và multiple registers
- [ ] **Error Handling:** CRC errors, timeouts, retries
- [ ] **Retry Logic:** Exponential backoff, max retry count
- [ ] **Performance:** Response time < 100ms per command
- [ ] **Reliability:** 99.9% success rate

#### **Test Cases:**
```c
// Test communication
test_modbus_read_holding_registers();
test_modbus_write_single_register();
test_modbus_write_multiple_registers();
test_crc_error_detection();
test_timeout_handling();
test_retry_mechanism();
test_communication_performance();
```

---

## 🎯 **PHASE 3: TELEMETRY SYSTEM**

### **3.1 Telemetry Data Packing**

#### **Yêu cầu từ REQ_Telemetry_Schema.md:**
```json
{
  "ts": 1640995200000,
  "status": {
    "state": "idle",
    "pos_mm": 1000,
    "vel_mms": 0,
    "acc_mms2": 0,
    "safety": {
      "estop": false,
      "location_safe": true,
      "obstacle_detected": false
    }
  },
  "location": {
    "position": {"x": 1000, "y": 2000, "z": 0},
    "orientation": {"pitch": 0, "roll": 0, "yaw": 45},
    "accuracy": 10,
    "system_status": "ok"
  },
  "navigation": {
    "status": "idle",
    "target": {"x": 0, "y": 0, "z": 0},
    "progress": 0,
    "distance_to_target": 0
  }
}
```

#### **Source code hiện tại:**
```c
// Cần implement telemetry system:
- Data collection từ tất cả modules
- JSON serialization theo schema
- Rate limiting (50ms/100ms/500ms)
- Ring buffer cho local storage
```

#### **Checklist Implementation:**
- [ ] **Data Collection:** Collect data từ tất cả modules
- [ ] **Schema Compliance:** JSON format theo REQ_Telemetry_Schema.md
- [ ] **Rate Limiting:** Different update frequencies cho different data
- [ ] **Local Storage:** Ring buffer với retention policy
- [ ] **Event System:** Real-time event broadcasting
- [ ] **Performance:** Serialization < 1ms, memory usage < 1MB

#### **Test Cases:**
```c
// Test telemetry
test_telemetry_data_collection();
test_json_serialization();
test_schema_compliance();
test_rate_limiting();
test_ring_buffer_operations();
test_event_broadcasting();
test_telemetry_performance();
```

---

## 🎯 **PHASE 4: API INTEGRATION**

### **4.1 Backend Communication**

#### **Yêu cầu từ main.c:**
```c
// API Manager Configuration:
- HTTP Port: 8080
- WebSocket Port: 8081
- CORS enabled
- Authentication disabled (for testing)
- SSL disabled (for testing)
```

#### **Source code hiện tại (`api_manager.c`):**
```c
// Cần kiểm tra và cập nhật:
- HTTP endpoints cho module control
- WebSocket real-time telemetry
- Error handling và response codes
- Request/response validation
```

#### **Checklist Implementation:**
- [ ] **HTTP Endpoints:** GET/POST cho module control
- [ ] **WebSocket:** Real-time telemetry streaming
- [ ] **Error Handling:** Proper HTTP status codes
- [ ] **Request Validation:** Input validation và sanitization
- [ ] **Response Format:** JSON responses theo API spec
- [ ] **Performance:** Response time < 100ms

#### **Test Cases:**
```c
// Test API endpoints
test_http_get_module_status();
test_http_post_module_command();
test_websocket_telemetry_stream();
test_error_handling();
test_request_validation();
test_api_performance();
```

---

## 🎯 **PHASE 5: SAFETY SYSTEM INTEGRATION**

### **5.1 Safety Monitoring**

#### **Yêu cầu từ REQ_State_Machine.md:**
```c
// Safety Conditions:
- safety_ok: Hệ thống an toàn OK
- location_ok: Location system hoạt động
- estop_pressed: E-Stop được nhấn
- obstacle_detected: Phát hiện chướng ngại
```

#### **Source code hiện tại (`safety_manager.c`):**
```c
// Cần kiểm tra và cập nhật:
- Continuous safety monitoring
- E-Stop integration
- Safety zone monitoring
- Fault detection và reporting
```

#### **Checklist Implementation:**
- [ ] **E-Stop Monitoring:** Continuous E-Stop status monitoring
- [ ] **Safety Zones:** Zone-based safety monitoring
- [ ] **Obstacle Detection:** Obstacle detection integration
- [ ] **Fault Detection:** Automatic fault detection
- [ ] **Safety Response:** Immediate safety response
- [ ] **Fault Recovery:** Fault clearing procedures

#### **Test Cases:**
```c
// Test safety system
test_estop_monitoring();
test_safety_zone_monitoring();
test_obstacle_detection();
test_fault_detection();
test_safety_response_time();
test_fault_recovery();
```

---

## 📊 **TEST EXECUTION PLAN**

### **5.1 Unit Testing**
```bash
# Run all unit tests
make test_all

# Expected results:
- All tests pass (100% success rate)
- No memory leaks
- No segmentation faults
- Performance within limits
```

### **5.2 Integration Testing**
```bash
# Test module integration
./test_module_integration

# Test communication
./test_communication_integration

# Test safety system
./test_safety_integration
```

### **5.3 System Testing**
```bash
# Test complete system
./oht50_main --debug

# Expected behavior:
- System initializes successfully
- All modules discovered
- State machine transitions correctly
- Safety system monitors continuously
- API endpoints respond correctly
```

---

## 🎯 **SUCCESS CRITERIA**

### **Functional Requirements:**
- [ ] **State Machine:** All 8 states và transitions working
- [ ] **Module Discovery:** Auto-discovery cho tất cả modules
- [ ] **Communication:** Modbus RTU communication reliable
- [ ] **Telemetry:** JSON telemetry theo schema
- [ ] **API:** HTTP/WebSocket endpoints working
- [ ] **Safety:** E-Stop response < 100ms

### **Performance Requirements:**
- [ ] **Response Time:** < 100ms cho critical operations
- [ ] **Throughput:** Handle 100+ requests/second
- [ ] **Memory Usage:** < 50MB total memory
- [ ] **CPU Usage:** < 30% average CPU
- [ ] **Reliability:** 99.9% uptime

### **Quality Requirements:**
- [ ] **Code Coverage:** > 90% test coverage
- [ ] **Documentation:** Complete API documentation
- [ ] **Error Handling:** Comprehensive error handling
- [ ] **Logging:** Structured logging với levels
- [ ] **Monitoring:** Health monitoring và metrics

---

## 🚀 **IMPLEMENTATION TIMELINE**

### **Week 1: State Machine & Module Integration**
- Day 1-2: State machine implementation
- Day 3-4: Module auto-discovery
- Day 5: Integration testing

### **Week 2: Communication & Telemetry**
- Day 1-2: Modbus RTU communication
- Day 3-4: Telemetry system
- Day 5: Performance optimization

### **Week 3: API & Safety Integration**
- Day 1-2: API endpoints
- Day 3-4: Safety system integration
- Day 5: System testing

### **Week 4: Testing & Optimization**
- Day 1-2: Comprehensive testing
- Day 3-4: Performance optimization
- Day 5: Documentation và deployment

---

## 📞 **SUPPORT & CONTACTS**

### **Technical Support:**
- **FW Team Lead:** Available for technical questions
- **PM Team:** Available for project coordination
- **EMBED Team:** Available for hardware integration

### **Documentation:**
- **Requirements:** `docs/02-REQUIREMENTS/`
- **Architecture:** `docs/03-ARCHITECTURE/`
- **Implementation:** `docs/05-IMPLEMENTATION/`

---

**Status:** 🚀 READY FOR IMPLEMENTATION  
**Next Steps:** Begin Phase 1 - State Machine Implementation  
**Priority:** HIGH  
**Timeline:** 4 weeks total
