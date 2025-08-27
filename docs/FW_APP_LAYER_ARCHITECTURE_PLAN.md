# 🔧 **FW APP LAYER ARCHITECTURE & PLAN - OHT-50**

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Team:** FW Team  
**Mục tiêu:** Documentation về cấu trúc file và plan lập trình cho App Layer refactor

---

## 📁 **CẤU TRÚC FILE HIỆN TẠI**

### **Core Layer (src/app/core/)**
```
src/app/core/
├── system_state_machine.c/h     # ✅ Existing - Enhanced
├── control_loop.c/h             # ✅ Existing - Enhanced  
├── safety_monitor.c/h           # ✅ NEW - Safety-first approach
├── system_controller.c/h        # ✅ NEW - Main coordination
└── CMakeLists.txt               # ✅ Updated - Include new modules
```

### **Managers Layer (src/app/managers/)**
```
src/app/managers/
├── module_manager.c/h           # ✅ Existing - Need enhancement
├── communication_manager.c/h    # ✅ Existing - Need enhancement
├── safety_manager.c/h           # ✅ Existing - Need enhancement
├── telemetry_manager.c/h        # ✅ Existing - Need enhancement
├── configuration_manager.c/h    # 🔄 NEW - To be created
├── diagnostics_manager.c/h      # 🔄 NEW - To be created
└── CMakeLists.txt               # 🔄 Need update
```

### **Modules Layer (src/app/modules/)**
```
src/app/modules/
├── power_module_handler.c/h     # ✅ Existing
├── safety_module_handler.c/h    # ✅ Existing
├── travel_motor_module_handler.c/h # ✅ Existing
├── dock_module_handler.c/h      # ✅ Existing
├── module_registry.c/h          # ✅ Existing
└── CMakeLists.txt               # ✅ Existing
```

### **API Layer (src/app/api/)**
```
src/app/api/
├── api_manager.c/h              # ✅ Existing - Need enhancement
├── api_endpoints.c/h            # ✅ Existing - Need enhancement
├── websocket_server.c           # ✅ Existing - Need enhancement
├── http_server.c                # ✅ Existing - Need enhancement
└── CMakeLists.txt               # 🔄 Need update
```

### **Documentation (docs/)**
```
docs/
├── FW_APP_LAYER_ARCHITECTURE_PLAN.md  # ✅ NEW - This file
├── FW_APP_LAYER_REFACTOR_PLAN.md      # 🔄 Need recreate
├── PHASE_1_COMPLETION_REPORT.md       # 🔄 Need recreate
└── generated/                          # 🔄 Auto-generated docs
```

---

## 🎯 **PLAN LẬP TRÌNH 4 PHASES**

### **PHASE 1: CORE FOUNDATION (Week 1-2) - ✅ COMPLETED**

#### **1.1 Safety Monitor Module (CRITICAL)**
- ✅ **Header file:** `src/app/core/safety_monitor.h`
- ✅ **Implementation:** `src/app/core/safety_monitor.c`
- ✅ **Safety-first approach** với E-Stop integration
- ✅ **Comprehensive error handling** và recovery mechanisms
- ✅ **State machine** với safety validation
- ✅ **Emergency procedures** với fail-safe design

#### **1.2 System Controller Module (HIGH)**
- ✅ **Header file:** `src/app/core/system_controller.h`
- ✅ **Implementation:** `src/app/core/system_controller.c`
- ✅ **Main system coordination** và error handling
- ✅ **Performance monitoring** và diagnostics
- ✅ **Auto-recovery mechanisms**
- ✅ **Comprehensive logging system**

#### **1.3 Build System Integration**
- ✅ **CMakeLists.txt updated** để include new modules
- ✅ **Library dependencies** properly configured
- ✅ **Include directories** set up correctly

### **PHASE 2: MANAGERS ENHANCEMENT (Week 3-4) - 🔄 IN PROGRESS**

#### **2.1 Module Manager Enhancement (Priority: HIGH)**
- [ ] **Auto-discovery** modules trên RS485 bus
- [ ] **Health monitoring** với timeout detection
- [ ] **Module registry** với persistent storage
- [ ] **Module communication** coordination

**Files cần update:**
- `src/app/managers/module_manager.c/h`

#### **2.2 Communication Manager Enhancement (Priority: HIGH)**
- [ ] **RS485 multi-drop** communication
- [ ] **Modbus RTU** protocol implementation
- [ ] **Error handling** với CRC validation
- [ ] **Timeout management** với retry mechanisms

**Files cần update:**
- `src/app/managers/communication_manager.c/h`

#### **2.3 Safety Manager Enhancement (Priority: CRITICAL)**
- [ ] **Enhanced E-Stop handling**
- [ ] **Safety zone monitoring**
- [ ] **Interlock validation**
- [ ] **Emergency procedures**

**Files cần update:**
- `src/app/managers/safety_manager.c/h`

#### **2.4 Configuration Manager (Priority: MEDIUM)**
- [ ] **Configuration persistence**
- [ ] **Configuration validation**
- [ ] **Configuration versioning**
- [ ] **Configuration backup/restore**

**Files cần tạo:**
- `src/app/managers/configuration_manager.c/h`

#### **2.5 Diagnostics Manager (Priority: MEDIUM)**
- [ ] **System diagnostics**
- [ ] **Module diagnostics**
- [ ] **Performance monitoring**
- [ ] **Diagnostics reporting**

**Files cần tạo:**
- `src/app/managers/diagnostics_manager.c/h`

### **PHASE 3: API LAYER ENHANCEMENT (Week 5-6) - 🔄 PLANNED**

#### **3.1 API Manager Enhancement (Priority: HIGH)**
- [ ] **HTTP endpoint management**
- [ ] **WebSocket server management**
- [ ] **API request routing**
- [ ] **API authentication/authorization**

**Files cần update:**
- `src/app/api/api_manager.c/h`

#### **3.2 API Endpoints Enhancement (Priority: HIGH)**
- [ ] **System Status endpoints**
- [ ] **Module Management endpoints**
- [ ] **Safety System endpoints**
- [ ] **Configuration endpoints**
- [ ] **Telemetry endpoints**

**Files cần update:**
- `src/app/api/api_endpoints.c/h`

#### **3.3 WebSocket Server Enhancement (Priority: MEDIUM)**
- [ ] **Real-time event streaming**
- [ ] **WebSocket connection management**
- [ ] **Event broadcasting**
- [ ] **Connection monitoring**

**Files cần update:**
- `src/app/api/websocket_server.c`

### **PHASE 4: TESTING & INTEGRATION (Week 7-8) - 🔄 PLANNED**

#### **4.1 Unit Tests (Priority: HIGH)**
- [ ] **System state machine tests**
- [ ] **Safety manager tests**
- [ ] **Communication manager tests**
- [ ] **Module manager tests**
- [ ] **API endpoint tests**

**Files cần tạo:**
- `tests/unit/test_system_state_machine.c`
- `tests/unit/test_safety_manager.c`
- `tests/unit/test_communication_manager.c`
- `tests/unit/test_module_manager.c`
- `tests/unit/test_api_endpoints.c`

#### **4.2 Integration Tests (Priority: HIGH)**
- [ ] **System integration tests**
- [ ] **Module communication tests**
- [ ] **Safety integration tests**
- [ ] **API integration tests**

**Files cần tạo:**
- `tests/integration/test_system_integration.c`
- `tests/integration/test_module_communication.c`
- `tests/integration/test_safety_integration.c`
- `tests/integration/test_api_integration.c`

#### **4.3 Performance Tests (Priority: MEDIUM)**
- [ ] **Communication performance tests**
- [ ] **Safety response time tests**
- [ ] **API response time tests**
- [ ] **Memory usage tests**

**Files cần tạo:**
- `tests/performance/test_communication_performance.c`
- `tests/performance/test_safety_response_time.c`
- `tests/performance/test_api_response_time.c`
- `tests/performance/test_memory_usage.c`

---

## 🔧 **BUILD PROCESS (MANUAL ONLY - KHÔNG DÙNG SCRIPT)**

**🚨 QUY ĐỊNH: KHÔNG ĐƯỢC DÙNG SCRIPT BUILD - CHỈ DÙNG MANUAL COMMANDS!**

### **Manual Build Steps:**

#### **1. Validate Files Exist:**
```bash
# Check Phase 1 files
ls -la src/app/core/safety_monitor.*
ls -la src/app/core/system_controller.*
ls -la src/app/core/CMakeLists.txt
```

#### **2. Configure Build:**
```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
```

#### **3. Build Project:**
```bash
# Build all modules
make -j$(nproc)

# Build specific library
make app_core
```

#### **4. Run Static Analysis:**
```bash
# Run clang-tidy (if available)
find ../src -name "*.c" -o -name "*.h" | xargs clang-tidy -p .
```

#### **5. Run Tests:**
```bash
# Run unit tests (if available)
./test_app_core

# Run integration tests (if available)
./test_system_integration
```

#### **6. Generate Documentation:**
```bash
# Generate API docs (if doxygen available)
doxygen ../Doxyfile

# Create build report
echo "Build completed at $(date)" > build_report.txt
```

---

## 📊 **IMPLEMENTATION STATUS**

### **Phase 1 Status: ✅ COMPLETED**
- ✅ **Safety Monitor:** Complete implementation
- ✅ **System Controller:** Complete implementation
- ✅ **Build Integration:** CMakeLists.txt updated
- ✅ **Documentation:** Architecture plan created

### **Phase 2 Status: 🔄 IN PROGRESS**
- 🔄 **Module Manager:** Need enhancement
- 🔄 **Communication Manager:** Need enhancement
- 🔄 **Safety Manager:** Need enhancement
- ⏳ **Configuration Manager:** Not started
- ⏳ **Diagnostics Manager:** Not started

### **Phase 3 Status: ⏳ PLANNED**
- ⏳ **API Manager:** Not started
- ⏳ **API Endpoints:** Not started
- ⏳ **WebSocket Server:** Not started

### **Phase 4 Status: ⏳ PLANNED**
- ⏳ **Unit Tests:** Not started
- ⏳ **Integration Tests:** Not started
- ⏳ **Performance Tests:** Not started

---

## 🚨 **CRITICAL CONSTRAINTS**

### **Safety Requirements:**
- ✅ **E-Stop override** cho tất cả operations
- ✅ **Safety validation** trước mọi state transition
- ✅ **Fail-safe design** cho tất cả safety systems
- ✅ **Hardware watchdog** integration

### **Performance Requirements:**
- ✅ **E-Stop Response Time:** < 100ms
- ✅ **Update Frequency:** 10ms cho safety monitoring
- ✅ **Memory Usage:** < 50MB
- ✅ **CPU Usage:** < 10%

### **Reliability Requirements:**
- ✅ **Fault tolerance** cho communication errors
- ✅ **Auto-recovery** từ failures
- ✅ **Data integrity** validation
- ✅ **Backup mechanisms** cho critical functions

---

## 📋 **NEXT STEPS**

### **Immediate Actions (Today):**
1. **Test Phase 1 build** manually
2. **Verify safety_monitor.c** compilation
3. **Verify system_controller.c** compilation
4. **Check CMakeLists.txt** integration
5. **Prepare Phase 2** implementation

### **Phase 2 Preparation (This Week):**
1. **Review existing managers** code
2. **Plan enhancements** cho each manager
3. **Create configuration_manager.c/h**
4. **Create diagnostics_manager.c/h**
5. **Update managers CMakeLists.txt**

### **Phase 3 Preparation (Next Week):**
1. **Review existing API** code
2. **Plan API enhancements**
3. **Design new endpoints**
4. **Plan WebSocket improvements**

### **Phase 4 Preparation (Week 6):**
1. **Set up testing framework**
2. **Create test templates**
3. **Plan test scenarios**
4. **Prepare test hardware**

---

## 🎯 **ACCEPTANCE CRITERIA**

### **Functional Requirements:**
- ✅ **Safety-first approach** implemented
- ✅ **E-Stop integration** complete
- ✅ **Error handling** comprehensive
- ✅ **State management** robust
- [ ] **Module discovery** (Phase 2)
- [ ] **Communication reliability** (Phase 2)

### **Quality Requirements:**
- ✅ **Modular design** implemented
- ✅ **Clear separation** of concerns
- ✅ **Comprehensive error handling**
- [ ] **Test coverage > 90%** (Phase 4)
- [ ] **Zero memory leaks** (Phase 4)

### **Documentation Requirements:**
- ✅ **Complete API documentation**
- ✅ **Architecture documentation**
- [ ] **User manual** (Phase 4)
- [ ] **Developer guide** (Phase 4)
- [ ] **Troubleshooting guide** (Phase 4)

---

## 📞 **COMMUNICATION & REPORTING**

### **Daily Standups:**
- **Progress updates** mỗi ngày
- **Blockers identification** và resolution
- **Code review** cho completed features
- **Testing status** updates

### **Weekly Reviews:**
- **Milestone validation** mỗi tuần
- **Quality metrics** review
- **Risk assessment** và mitigation
- **Resource allocation** review

### **Phase Deliverables:**
- **Complete source code** với documentation
- **Test results** và coverage reports
- **Performance benchmarks**
- **User acceptance testing** results

---

## 🚀 **DEPLOYMENT STRATEGY**

### **Phase 1 Deployment:**
```bash
# Manual deployment steps
cd build
cp libapp_core.a /usr/local/lib/
cp oht50_firmware /usr/local/bin/
chmod +x /usr/local/bin/oht50_firmware
```

### **Phase 2 Deployment:**
```bash
# Update managers library
cp libapp_managers.a /usr/local/lib/
# Update configuration files
cp config/* /etc/oht50/
```

### **Phase 3 Deployment:**
```bash
# Update API library
cp libapp_api.a /usr/local/lib/
# Restart services
systemctl restart oht50-firmware
```

### **Phase 4 Deployment:**
```bash
# Full system deployment
# Run all tests first
./run_all_tests.sh
# Deploy if tests pass
./deploy_full_system.sh
```

---

**🎯 PHASE 1 HOÀN THÀNH! SẴN SÀNG CHO PHASE 2!**

**📞 LIÊN HỆ PM ĐỂ SCHEDULE PHASE 2 KICKOFF MEETING!**

---

**Changelog v1.0:**
- ✅ Created comprehensive architecture plan
- ✅ Documented file structure
- ✅ Defined 4-phase implementation plan
- ✅ Specified manual build process (no scripts)
- ✅ Established implementation status
- ✅ Defined acceptance criteria
- ✅ Created deployment strategy
