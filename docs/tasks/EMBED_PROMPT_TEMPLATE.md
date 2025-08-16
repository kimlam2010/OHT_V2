# 📋 EMBED Prompt Template - OHT-50

**Version:** 1.0.0  
**Date:** 2025-01-27  
**Team:** EMBED  
**Purpose:** Standardized prompts for EMBED team tasks

---

## 🎯 **Template Structure**

### **Standard Prompt Format:**
```
# [TASK_NAME] - [BRIEF_DESCRIPTION]

**Task:** [TASK_ID]  
**Priority:** [HIGH/MEDIUM/LOW]  
**Estimated Duration:** [X hours/days]  
**Dependencies:** [List dependencies]

## 📋 **Task Description**
[Detailed task description]

## 🎯 **Objectives**
- [Objective 1]
- [Objective 2]
- [Objective 3]

## 📁 **Required Files**
- [File 1]
- [File 2]
- [File 3]

## 🔧 **Implementation Steps**
1. [Step 1]
2. [Step 2]
3. [Step 3]

## ✅ **Acceptance Criteria**
- [Criterion 1]
- [Criterion 2]
- [Criterion 3]

## 🧪 **Testing Requirements**
- [Test 1]
- [Test 2]
- [Test 3]

## 📝 **Documentation Requirements**
- [Doc 1]
- [Doc 2]
- [Doc 3]
```

---

## 🔧 **EM-12: LiDAR Driver & USB Integration**

### **Task:** EM-12  
**Priority:** HIGH  
**Estimated Duration:** 3-4 days  
**Dependencies:** EM-01 to EM-11 completed

### **📋 Task Description**
Implement LiDAR driver và USB integration cho OHT-50 system. Tích hợp RPLIDAR A1M8 sensor qua USB interface để phát hiện chướng ngại vật và đảm bảo an toàn trong quá trình di chuyển.

### **🎯 Objectives**
- Implement HAL interface cho LiDAR sensor
- Tích hợp USB communication với RPLIDAR A1M8
- Tích hợp với safety system (E-Stop)
- Tích hợp với control system
- Tích hợp với telemetry system
- Đảm bảo performance requirements (8Hz scan rate, ±2cm accuracy)

### **📁 Required Files**
- `docs/specs/lidar_wiring_guide.md` ✅ (Created)
- `docs/specs/lidar_setup_guide.md` ✅ (Created)
- `docs/specs/lidar_test_procedures.md` ✅ (Created)
- `docs/specs/lidar_integration_guide.md` ✅ (Created)
- `firmware/include/hal_lidar.h` (To be created)
- `firmware/src/hal/hal_lidar.c` (To be created)
- `firmware/tests/test_lidar.c` (To be created)

### **🔧 Implementation Steps**

#### **Step 1: Create HAL Interface**
```bash
# Create HAL header file
mkdir -p firmware/include
touch firmware/include/hal_lidar.h

# Create HAL implementation
mkdir -p firmware/src/hal
touch firmware/src/hal/hal_lidar.c
```

**Implementation Requirements:**
- Define data structures cho LiDAR scan data
- Implement HAL functions: init, deinit, start_scanning, stop_scanning
- Implement safety functions: check_safety, get_safety_status
- Implement utility functions: calculate_min_distance, is_obstacle_detected

#### **Step 2: Implement USB Communication**
```bash
# Implement USB serial communication
# Use RPLIDAR SDK for protocol handling
# Implement thread-safe communication
```

**Implementation Requirements:**
- USB device detection và initialization
- Serial port configuration (115200 baud, 8N1)
- RPLIDAR protocol implementation
- Thread-safe data acquisition
- Error handling và recovery

#### **Step 3: Safety Integration**
```bash
# Integrate với safety system
# Implement E-Stop triggers
# Implement warning system
```

**Implementation Requirements:**
- E-Stop trigger khi phát hiện vật < 0.5m
- Warning alert khi phát hiện vật < 1.0m
- Safe operation khi không có vật < 2.0m
- Real-time safety monitoring (100Hz)

#### **Step 4: Control System Integration**
```bash
# Integrate với control system
# Implement speed control based on obstacles
# Implement movement restrictions
```

**Implementation Requirements:**
- Speed reduction khi có warning
- Movement stop khi có E-Stop
- Obstacle avoidance logic
- Control system callbacks

#### **Step 5: Telemetry Integration**
```bash
# Integrate với telemetry system
# Implement data logging
# Implement status reporting
```

**Implementation Requirements:**
- LiDAR data telemetry
- Safety status telemetry
- Performance metrics logging
- Error reporting

#### **Step 6: Testing & Validation**
```bash
# Create test programs
# Implement unit tests
# Implement integration tests
```

**Implementation Requirements:**
- Unit tests cho HAL functions
- Integration tests cho safety system
- Performance tests cho scan rate
- Accuracy tests cho distance measurement

### **✅ Acceptance Criteria**
- ✅ HAL interface implemented và tested
- ✅ USB communication working reliably
- ✅ Safety integration functional
- ✅ Control integration functional
- ✅ Telemetry integration functional
- ✅ Performance requirements met (8Hz scan rate, ±2cm accuracy)
- ✅ All tests passing
- ✅ Documentation complete

### **🧪 Testing Requirements**

#### **Unit Tests:**
- Test HAL initialization/deinitialization
- Test USB communication
- Test safety threshold detection
- Test data processing functions

#### **Integration Tests:**
- Test safety system integration
- Test control system integration
- Test telemetry system integration
- Test system-wide functionality

#### **Performance Tests:**
- Test scan rate (target: 8Hz)
- Test distance accuracy (target: ±2cm)
- Test response time (target: < 100ms)
- Test system stability

#### **Safety Tests:**
- Test E-Stop triggering
- Test warning system
- Test false positive prevention
- Test emergency scenarios

### **📝 Documentation Requirements**
- ✅ `docs/specs/lidar_wiring_guide.md` (Hardware wiring)
- ✅ `docs/specs/lidar_setup_guide.md` (Software setup)
- ✅ `docs/specs/lidar_test_procedures.md` (Testing procedures)
- ✅ `docs/specs/lidar_integration_guide.md` (Integration guide)
- 🔄 `firmware/README.md` (Firmware documentation)
- 🔄 `docs/EMBED/EMBED_PROGRESS_UPDATE.md` (Progress update)

---

## 🔧 **Other EMBED Tasks**

### **EM-01: Hardware Bring-up** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** Orange Pi 5B platform ready, UART1 configuration

### **EM-02: RS485 Transceiver** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** UART1 RS485 working, HAL interface complete

### **EM-03: UART/CAN Init + DMA** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** UART initialization với DMA, performance validated

### **EM-04: Bootloader Layout** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** Bootloader configuration, device tree overlay

### **EM-05: IO Drivers** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** Encoder, limit switch, E-Stop drivers

### **EM-06: EMI/ESD Guidelines** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** EMI/ESD guidelines, compliance documentation

### **EM-07: HIL Testing Bench** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** HIL testing setup, test procedures

### **EM-08: Production Test Checklist** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** Production testing procedures, quality assurance

### **EM-09: RS485 Wiring Verification** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** RS485 wiring verified, performance validated

### **EM-11: UART1 Enable và Validation** ✅ COMPLETED
**Status:** ✅ HOÀN THÀNH  
**Deliverables:** UART1 enabled, validation complete

---

## 📋 **Progress Tracking**

### **Current Status:**
- **Completed Tasks:** 10/11 (90.9%)
- **Current Task:** EM-12 (LiDAR Driver & USB Integration)
- **Next Task:** Integration với FW team

### **Progress Update Template:**
```bash
# Update progress trong docs/EMBED/EMBED_PROGRESS_UPDATE.md

## EM-12 Progress Update
**Date:** [YYYY-MM-DD]
**Status:** [IN_PROGRESS/COMPLETED/BLOCKED]
**Progress:** [X%]

### Completed:
- [ ] Task 1
- [ ] Task 2

### In Progress:
- [ ] Task 3
- [ ] Task 4

### Blocked:
- [ ] Issue 1
- [ ] Issue 2

### Next Steps:
1. [Next step 1]
2. [Next step 2]
```

---

## 🚨 **Common Issues & Solutions**

### **USB Communication Issues:**
- **Device not detected:** Check udev rules, cable connection
- **Permission denied:** Add user to dialout group
- **Data corruption:** Check USB bandwidth, interference

### **Safety Integration Issues:**
- **E-Stop not triggering:** Check threshold settings, integration
- **False triggers:** Adjust sensitivity, check environment
- **Slow response:** Optimize processing, check system load

### **Performance Issues:**
- **Low scan rate:** Check USB bandwidth, system load
- **Poor accuracy:** Calibrate sensor, check mounting
- **High latency:** Optimize processing, reduce overhead

---

## 📞 **Support & Communication**

### **Team Communication:**
- **Progress Updates:** `docs/EMBED/EMBED_PROGRESS_UPDATE.md`
- **Team Notices:** `docs/EMBED/EMBED_TEAM_NOTICE.md`
- **PM Communication:** `docs/EMBED/PM_NOTICE.md`

### **Technical Support:**
- **Hardware Issues:** Check wiring guides, datasheets
- **Software Issues:** Check HAL documentation, SDK guides
- **Integration Issues:** Check integration guides, system documentation

---

**🎯 Template Complete!**
Sử dụng template này để standardize tất cả EMBED team tasks và đảm bảo consistency trong implementation.
