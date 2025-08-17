# FW Team - Master Module Application Development Plan

**Version:** 1.0.0  
**Date:** August 17, 2025  
**Team:** Firmware  
**Project:** OHT-50 Master Module  
**Status:** Ready for Development

---

## 🎯 **Executive Summary**

FW Team cần phát triển **application layer** và **communication protocols** cho Master Module, sử dụng HAL interfaces đã hoàn thành từ EMBED Team.

### **✅ EMBED Team Deliverables Ready:**
- **7 HAL modules** với 100% test success rate
- **Hardware bring-up** hoàn tất
- **Safety systems** validated (SIL2 compliant)
- **Complete documentation** và test coverage

---

## 📊 **Development Phases**

### **Phase 1: Core Application Development (Weeks 1-2)**

#### **Task #FW-APP-001: System State Machine**
- **Objective:** Implement system state management cho OHT-50
- **States:** IDLE → MOVE → DOCK → FAULT → E-STOP
- **Deliverables:**
  - `system_state_machine.h/.c`
  - State transition logic
  - State validation functions
  - State monitoring system
- **Estimate:** 1 week
- **Priority:** Critical

#### **Task #FW-APP-002: Safety System Integration**
- **Objective:** Integrate E-Stop safety với application logic
- **Requirements:** E-Stop event handling, emergency procedures
- **Deliverables:**
  - `safety_manager.h/.c`
  - Emergency procedures
  - Safety validation
  - Fault handling
- **Estimate:** 1 week
- **Priority:** Critical

#### **Task #FW-APP-003: LED Status Management**
- **Objective:** Implement LED status patterns cho system states
- **Requirements:** LED patterns, status indication, error patterns
- **Deliverables:**
  - `led_manager.h/.c`
  - Status patterns
  - Error patterns
  - Network status
- **Estimate:** 0.5 week
- **Priority:** High

### **Phase 2: Communication Protocols (Weeks 3-4)**

#### **Task #FW-COMM-001: Modbus RTU Master**
- **Objective:** Implement Modbus RTU master cho slave modules
- **Requirements:** Protocol stack, slave communication (0x02-0x07)
- **Deliverables:**
  - `modbus_master.h/.c`
  - Protocol implementation
  - Slave communication
  - Error handling
- **Estimate:** 1.5 weeks
- **Priority:** High

#### **Task #FW-COMM-002: TCP/IP Communication**
- **Objective:** Implement TCP/IP communication cho Center server
- **Requirements:** TCP client/server, JSON protocol, network redundancy
- **Deliverables:**
  - `tcp_communication.h/.c`
  - JSON protocol
  - Connection management
  - Network failover
- **Estimate:** 1 week
- **Priority:** High

#### **Task #FW-COMM-003: WebSocket Real-time**
- **Objective:** Implement WebSocket cho real-time communication
- **Requirements:** WebSocket server, telemetry streaming, event system
- **Deliverables:**
  - `websocket_server.h/.c`
  - Telemetry streaming
  - Event system
  - Connection handling
- **Estimate:** 0.5 week
- **Priority:** Medium

### **Phase 3: Module Management (Weeks 5-6)**

#### **Task #FW-MOD-001: Auto-Discovery System**
- **Objective:** Implement module auto-discovery
- **Requirements:** Module detection, address scanning, registration
- **Deliverables:**
  - `module_discovery.h/.c`
  - Discovery algorithm
  - Registration system
  - Configuration management
- **Estimate:** 1 week
- **Priority:** High

#### **Task #FW-MOD-002: Module Communication**
- **Objective:** Implement module communication protocols
- **Requirements:** Command interface, status monitoring, health checking
- **Deliverables:**
  - `module_communication.h/.c`
  - Command interface
  - Status monitoring
  - Health checking
- **Estimate:** 1 week
- **Priority:** High

#### **Task #FW-MOD-003: Configuration Management**
- **Objective:** Implement configuration management system
- **Requirements:** JSON parser, validation, storage, hot-reload
- **Deliverables:**
  - `config_manager.h/.c`
  - Configuration parser
  - Validation system
  - Storage management
- **Estimate:** 0.5 week
- **Priority:** Medium

### **Phase 4: System Integration (Weeks 7-8)**

#### **Task #FW-INT-001: Main Application**
- **Objective:** Integrate tất cả components thành main application
- **Requirements:** Main loop, component initialization, event handling
- **Deliverables:**
  - `main.c`
  - Application loop
  - Component integration
  - Error handling
- **Estimate:** 1 week
- **Priority:** High

#### **Task #FW-INT-002: System Testing**
- **Objective:** Comprehensive system testing
- **Requirements:** Unit testing, integration testing, system testing
- **Deliverables:**
  - Test suites
  - Test results
  - Performance metrics
  - Test documentation
- **Estimate:** 1 week
- **Priority:** High

---

## 🔧 **Technical Architecture**

### **Application Layer Structure:**
```
firmware/app/
├── system_state_machine.h/.c    # System state management
├── safety_manager.h/.c          # Safety system integration
├── led_manager.h/.c             # LED status management
├── communication/               # Communication protocols
│   ├── modbus_master.h/.c      # Modbus RTU master
│   ├── tcp_communication.h/.c  # TCP/IP communication
│   └── websocket_server.h/.c   # WebSocket server
├── module_management/           # Module management
│   ├── module_discovery.h/.c   # Auto-discovery system
│   ├── module_communication.h/.c # Module communication
│   └── config_manager.h/.c     # Configuration management
├── main.c                      # Main application
└── tests/                      # Test suites
    ├── test_state_machine.c
    ├── test_safety.c
    ├── test_communication.c
    └── test_modules.c
```

### **HAL Integration:**
- **Use existing HAL interfaces** từ EMBED Team
- **Standardized error handling** với `hal_status_t`
- **Event-driven architecture** với callbacks
- **Thread-safe operations** cho real-time requirements

---

## 📋 **Success Criteria**

### **Functional Requirements:**
- ✅ System state machine working
- ✅ Safety system integrated
- ✅ LED status patterns implemented
- ✅ Modbus RTU communication working
- ✅ TCP/IP communication working
- ✅ WebSocket real-time streaming
- ✅ Module auto-discovery working
- ✅ Configuration management working

### **Performance Requirements:**
- ✅ Response time < 100ms cho safety operations
- ✅ Real-time communication < 50ms latency
- ✅ System stability > 99.9% uptime
- ✅ Memory usage < 512MB
- ✅ CPU usage < 80%

### **Quality Requirements:**
- ✅ 100% code coverage cho critical modules
- ✅ No memory leaks
- ✅ Thread-safe operations
- ✅ Comprehensive error handling
- ✅ Complete documentation

---

## 🚀 **Immediate Actions**

### **Week 1 Goals:**
1. **Review HAL Interfaces:** Examine EMBED Team's HAL modules
2. **Setup Development Environment:** Configure build system
3. **Start System State Machine:** Begin state machine implementation
4. **Begin Safety Integration:** Start E-Stop integration

### **Week 2 Goals:**
1. **Complete State Machine:** Finish state management
2. **Complete Safety Integration:** Finish E-Stop integration
3. **Start LED Management:** Begin LED status patterns
4. **Begin Communication:** Start Modbus RTU implementation

---

## 📊 **Timeline Summary**

| Phase | Duration | Tasks | Priority |
|-------|----------|-------|----------|
| Phase 1 | 2 weeks | Core Application | Critical |
| Phase 2 | 2 weeks | Communication | High |
| Phase 3 | 2 weeks | Module Management | High |
| Phase 4 | 2 weeks | System Integration | High |

**Total Duration:** 8 weeks  
**Team Size:** 2-3 engineers  
**Status:** Ready for Development

---

## 📝 **Next Steps**

1. **Review EMBED Team deliverables**
2. **Setup development environment**
3. **Begin Phase 1 development**
4. **Regular progress updates**
5. **Integration testing with HAL modules**

---

**Prepared by:** Project Manager  
**Reviewed by:** Technical Lead  
**Approved by:** FW Team Lead  
**Date:** August 17, 2025
