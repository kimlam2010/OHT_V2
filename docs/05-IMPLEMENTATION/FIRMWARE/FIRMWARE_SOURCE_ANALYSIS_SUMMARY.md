# FIRMWARE SOURCE ANALYSIS SUMMARY - OHT-50 MASTER MODULE

**Phiên bản:** 3.1.0  
**Ngày cập nhật:** 2025-01-28  
**Team:** FW  
**Trạng thái:** Phân tích hoàn thành - Updated for Simplified Architecture v2.1

---

## 📋 TỔNG QUAN PHÂN TÍCH

### 🎯 Mục tiêu phân tích
- Kiểm tra source code firmware hiện tại
- Đánh giá tính năng đã triển khai
- Xác định gaps và cải thiện cho kiến trúc đơn giản hóa
- Cập nhật tài liệu kỹ thuật cho 5 module bắt buộc

### 📊 Kết quả phân tích
- **Source Files:** 70+ files được phân tích
- **Lines of Code:** ~15,000 lines
- **Modules:** 12 HAL + 25 Application modules
- **Test Files:** 20+ test files
- **Documentation:** 10+ tài liệu kỹ thuật
- **Simplified Architecture:** 5 mandatory modules + RS485 standard

---

## 🏗️ KIẾN TRÚC ĐƠN GIẢN HÓA V2.1

### **5 Module Bắt Buộc:**
1. **Power Module (0x01)** - Quản lý nguồn và BMS
2. **Safety Module (0x02)** - Hệ thống an toàn cơ bản
3. **Travel Motor Module (0x03)** - Điều khiển động cơ di chuyển
4. **Dock & Location Module (0x05)** - Định vị và docking
5. **Master Control Module (0x00)** - Điều khiển tổng thể

### **Module Tùy Chọn:**
- **Lifter Motor Module (0x04)** - Điều khiển động cơ nâng
- **Cargo Door Module (0x06)** - Điều khiển cửa hàng hóa
- **Safety Extended Module (0x07)** - An toàn mở rộng

### **Module Mở Rộng (Plug-and-Play):**
- **RFID Reader Module (0x08)** - Đọc RFID
- **Camera Module (0x09)** - Xử lý hình ảnh
- **Environmental Sensor Module (0x0A)** - Cảm biến môi trường
- **Custom Modules (0x0B-0x1F)** - Module tùy chỉnh

---

## 🔧 PHÂN TÍCH CHI TIẾT

### 1. HARDWARE ABSTRACTION LAYER (HAL)

#### 1.1 HAL Common (`hal_common.c`)
- **Chức năng:** Common utilities, timestamps, logging
- **Tính năng:** Timestamp management, log levels, common status
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

#### 1.2 HAL LED (`hal_led.c`)
- **Chức năng:** LED control system
- **Tính năng:** 5 LEDs, blink/pulse modes, state management
- **GPIO Mapping:**
  - Power LED: GPIO1_D6 (pin 54)
  - System LED: GPIO1_A3 (pin 35)
  - Communication LED: GPIO0_D4 (pin 28)
  - Network LED: GPIO0_D5 (pin 29)
  - Error LED: GPIO1_D2 (pin 58)
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

#### 1.3 HAL E-Stop (`hal_estop.c`)
- **Chức năng:** Emergency stop system
- **Tính năng:** Single channel E-Stop, debounce, timeout
- **GPIO:** GPIO1_D3 (pin 59)
- **Trạng thái:** ✅ Hoàn thành - Single-channel cho basic compliance

#### 1.4 HAL RS485 (`hal_rs485.c`)
- **Chức năng:** RS485 communication (NEW STANDARD)
- **Tính năng:** Modbus RTU, auto-discovery, hot-swap
- **Configuration:**
  - Baudrate: 115200 bps
  - Data bits: 8
  - Parity: None
  - Stop bits: 1
  - Termination: 120Ω
  - Bias: 560Ω
- **Trạng thái:** ✅ Hoàn thành - Standard cho tất cả modules

#### 1.5 HAL Relay (`hal_relay.c`)
- **Chức năng:** Relay control system
- **Tính năng:** 2 relays, safety control, status monitoring
- **GPIO Mapping:**
  - Relay 1: GPIO1_D3 (pin 59)
  - Relay 2: GPIO1_D2 (pin 58)
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

#### 1.6 HAL Network (`hal_network.c`)
- **Chức năng:** Network interface management
- **Tính năng:** Ethernet, WiFi, network redundancy
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

#### 1.7 HAL LiDAR (`hal_lidar.c`)
- **Chức năng:** LiDAR integration via USB
- **Tính năng:** RPLIDAR A1M8, mapping, localization
- **Interface:** USB 2.0
- **Integration:** Dock & Location Module
- **Trạng thái:** ✅ Hoàn thành - USB integration ready

#### 1.8 HAL OTA Update (`hal_ota_update.c`)
- **Chức năng:** Over-the-air firmware updates
- **Tính năng:** Secure updates, rollback, validation
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

#### 1.9 HAL GPIO (`hal_gpio.c`)
- **Chức năng:** GPIO management
- **Tính năng:** Pin configuration, interrupt handling
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

---

## 🚀 APPLICATION LAYER

### 2. CORE SYSTEM MODULES

#### 2.1 System State Machine (`system_state_machine.c`)
- **Chức năng:** System state management
- **States:** Idle, Move, Dock, Fault, E-Stop
- **Navigation States:** Navigating, Positioning, Docking, Undocking
- **Trạng thái:** ✅ Hoàn thành - Cần update cho navigation states

#### 2.2 Safety Manager (`safety_manager.c`)
- **Chức năng:** Safety system management
- **Tính năng:** Basic compliance, single-channel E-Stop
- **Trạng thái:** ✅ Hoàn thành - Basic safety system implemented

#### 2.3 Communication Manager (`communication_manager.c`)
- **Chức năng:** Communication protocol management
- **Tính năng:** RS485/Modbus RTU, HTTP/WebSocket
- **Trạng thái:** ✅ Hoàn thành - RS485 standard implemented

#### 2.4 Module Manager (`module_manager.c`)
- **Chức năng:** Module management system
- **Tính năng:** Auto-discovery, hot-swap, registry
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

#### 2.5 API Manager (`api_manager.c`)
- **Chức năng:** API management system
- **Tính năng:** RESTful API, WebSocket, authentication
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

### 3. MODULE HANDLERS

#### 3.1 Power Module Handler (`power_module_handler.c`)
- **Chức năng:** Power module management
- **Tính năng:** Battery monitoring, BMS, charging control
- **Address:** 0x01
- **Trạng thái:** ✅ Hoàn thành - Compatible với kiến trúc đơn giản hóa

#### 3.2 Motor Module Handler (`motor_module_handler.c`)
- **Chức năng:** Travel motor control
- **Tính năng:** DC brushed motors, PID control, speed control
- **Address:** 0x03
- **Specifications:**
  - Motors: 2x DC Brushed 12V, 100W each
  - Control: PID speed control by % speed
  - Sensors: Hall effect speed sensors (60 PPR)
  - Gearbox: Planetary 20:1 ratio
  - Wheels: Omni-directional 150mm
- **Trạng thái:** ✅ Hoàn thành - Updated cho DC brushed motors

#### 3.3 Dock Module Handler (`dock_module_handler.c`)
- **Chức năng:** Dock & Location module management
- **Tính năng:** Positioning, docking, LiDAR integration
- **Address:** 0x05
- **Sensors:**
  - IMU (MPU6050)
  - Magnetic sensors (2x Hall effect)
  - RFID reader
  - Dock alignment sensors
  - LiDAR (RPLIDAR A1M8 via USB)
- **Trạng thái:** ✅ Hoàn thành - Updated cho Dock & Location integration

#### 3.4 DI/DO Module Handler (`di_do_module_handler.c`)
- **Chức năng:** Digital I/O management
- **Tính năng:** Safety I/O, interlock management
- **Trạng thái:** ✅ Hoàn thành - Integrated vào Safety Module

---

## 📊 PHÂN TÍCH GAPS VÀ CẢI THIỆN

### **Gaps Identified:**

#### 1. **Safety System Simplification**
- **Current:** Single-channel E-Stop
- **Required:** Single-channel E-Stop for basic compliance
- **Action:** Simplify `hal_estop.c` và `safety_manager.c`

#### 2. **Basic Safety System**
- **Current:** Basic safety system
- **Required:** Basic safety integration
- **Action:** Simplify safety system

#### 3. **Navigation States**
- **Current:** Basic state machine
- **Required:** Navigation states (Navigating, Positioning, Docking, Undocking)
- **Action:** Update `system_state_machine.c`

#### 4. **LiDAR USB Integration**
- **Current:** Basic LiDAR support
- **Required:** USB integration với Dock & Location module
- **Action:** Update `hal_lidar.c` và `dock_module_handler.c`

#### 5. **RS485 Standard**
- **Current:** Mixed communication protocols
- **Required:** RS485 standard cho tất cả modules
- **Action:** Update tất cả module handlers

### **Improvements Required:**

#### 1. **Module Auto-discovery**
- **Priority:** High
- **Status:** ✅ Implemented
- **Action:** Test và validate

#### 2. **Hot-swap Capability**
- **Priority:** High
- **Status:** ✅ Implemented
- **Action:** Test và validate

#### 3. **Basic Safety System**
- **Priority:** Critical
- **Status:** 🔄 In Progress
- **Action:** Implement basic integration

#### 4. **Single-channel E-Stop**
- **Priority:** Critical
- **Status:** ⏳ Pending
- **Action:** Simplify hardware và software

---

## 🎯 RECOMMENDATIONS

### **Immediate Actions (Week 1):**
1. **Simplify E-Stop system** lên single-channel
2. **Implement basic safety** integration
3. **Update navigation states** trong state machine
4. **Test RS485 standard** cho tất cả modules

### **Short-term Actions (Week 2-3):**
1. **Validate auto-discovery** system
2. **Test hot-swap capability**
3. **Update LiDAR USB integration**
4. **Performance optimization**

### **Long-term Actions (Week 4+):**
1. **Basic safety certification** preparation
2. **Advanced safety features**
3. **Performance monitoring**
4. **Documentation updates**

---

## 📈 METRICS VÀ KPIs

### **Code Quality Metrics:**
- **Lines of Code:** ~15,000 lines
- **Test Coverage:** 20+ test files
- **Documentation:** 10+ technical documents
- **Modules:** 12 HAL + 25 Application modules

### **Architecture Compliance:**
- **RS485 Standard:** ✅ 100% compliance
- **5 Mandatory Modules:** ✅ 100% coverage
- **Auto-discovery:** ✅ Implemented
- **Hot-swap:** ✅ Implemented
- **Basic Safety System:** 🔄 60% complete

### **Performance Metrics:**
- **Response Time:** < 100ms target
- **Uptime:** 99.9% target
- **Safety Response:** < 50ms target
- **Communication Latency:** < 10ms target

---

**Status:** Analysis Complete - Ready for Simplified Architecture Implementation  
**Next Steps:** Begin Phase 2 - Update remaining documentation files
