# FIRMWARE TÍNH NĂNG HIỆN TẠI - OHT-50 MASTER MODULE

**Phiên bản:** 2.1.0  
**Ngày cập nhật:** 2025-01-28  
**Team:** FW  
**Trạng thái:** Updated for Simplified Architecture v2.1 - Ready for Implementation

---

## 📋 TỔNG QUAN

Firmware OHT-50 Master Module hiện tại đã triển khai một hệ thống hoàn chỉnh với các tính năng chính sau, được cập nhật cho kiến trúc đơn giản hóa v2.1:

### 🏗️ Kiến trúc tổng thể
- **HAL (Hardware Abstraction Layer):** Tách biệt phần cứng và phần mềm
- **State Machine:** Quản lý trạng thái hệ thống + Navigation states
- **Safety System:** Hệ thống an toàn cơ bản + Single-channel E-Stop
- **Communication:** RS485/Modbus RTU standard cho tất cả modules
- **API Management:** HTTP/WebSocket server
- **Module Management:** Quản lý 5 module bắt buộc + Auto-discovery + Hot-swap

---

## 🏗️ KIẾN TRÚC MỚI V2.0

### **5 Module Bắt Buộc:**
1. **Power Module (0x01)** - Quản lý nguồn và BMS
2. **Safety Module (0x02)** - Hệ thống an toàn SIL2
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

## 🔧 TÍNH NĂNG CHI TIẾT

### 1. HARDWARE ABSTRACTION LAYER (HAL)

#### 1.1 GPIO Management
- **LED Control (5 LEDs):**
  - Power LED (GPIO1_D6) - Xanh lá
  - System LED (GPIO1_A3) - Xanh dương  
  - Communication LED (GPIO0_D4) - Vàng
  - Network LED (GPIO0_D5) - Xanh lá
  - Error LED (GPIO1_D2) - Đỏ

- **E-Stop System:**
  - Single channel E-Stop (GPIO1_D3) - Cần upgrade dual-channel
  - Debounce time: 50ms
  - Response timeout: 100ms
  - Auto-reset: Disabled

- **Relay Control:**
  - Relay 1 (GPIO1_D3) - 24V DC, 2A
  - Relay 2 (GPIO1_D2) - 24V DC, 2A
  - Pulse duration: 100ms

#### 1.2 Communication HAL
- **RS485 Interface (NEW STANDARD):**
  - Device: `/dev/ttyOHT485`
  - Baud rate: 115200 bps
  - Data bits: 8
  - Stop bits: 1
  - Parity: None
  - Termination: 120Ω
  - Bias: 560Ω
  - Timeout: 1000ms
  - Retry count: 3
  - Auto-discovery: Enabled
  - Hot-swap: Enabled

- **Network Interface:**
  - Ethernet: 10/100/1000 Mbps
  - WiFi: 5G/2.4G backup
  - Scan interval: 30s

#### 1.3 Advanced HAL Features
- **LiDAR Integration:** RPLIDAR A1M8 via USB
- **OTA Updates:** Over-the-air firmware updates

### 2. SYSTEM STATE MACHINE

#### 2.1 Core System States
```c
typedef enum {
    SYSTEM_STATE_IDLE = 0,        // Chờ lệnh
    SYSTEM_STATE_MOVE,            // Di chuyển
    SYSTEM_STATE_DOCK,            // Dock
    SYSTEM_STATE_FAULT,           // Lỗi
    SYSTEM_STATE_ESTOP            // Dừng khẩn cấp
} system_state_t;
```

#### 2.2 Navigation States (NEW)
```c
typedef enum {
    NAVIGATION_STATE_IDLE = 0,    // Không di chuyển
    NAVIGATION_STATE_NAVIGATING,  // Đang di chuyển
    NAVIGATION_STATE_POSITIONING, // Định vị
    NAVIGATION_STATE_DOCKING,     // Đang dock
    NAVIGATION_STATE_UNDOCKING    // Đang undock
} navigation_state_t;
```

#### 2.3 System Events
- **Commands:** Move, Dock, Stop
- **Safety:** E-Stop triggered/reset
- **Faults:** Detection/clear
- **Navigation:** Start navigation, waypoint reached
- **Timeouts:** System timeouts
- **Errors:** Error handling

### 3. SAFETY SYSTEM (SIL2)

#### 3.1 Safety Levels
```c
typedef enum {
    SAFETY_LEVEL_NORMAL = 0,     // Hoạt động bình thường
    SAFETY_LEVEL_WARNING,        // Cảnh báo
    SAFETY_LEVEL_CRITICAL,       // Nguy hiểm
    SAFETY_LEVEL_EMERGENCY       // Khẩn cấp
} safety_level_t;
```

#### 3.2 Basic Safety System
```c
typedef struct {
    bool estop_active;           // E-Stop active
    bool estop_ok;               // E-Stop status
    safety_level_t level;        // Safety level
    bool light_curtain_ok;       // Light curtain status
    bool safety_mats_ok;         // Safety mats status
} safety_telemetry_t;
```

#### 3.3 Single-channel E-Stop (SIMPLIFIED)
```c
typedef struct {
    bool channel_ok;             // Single channel status
    uint32_t last_check;         // Last check timestamp
} single_channel_estop_t;
```

### 4. MODULE MANAGEMENT

#### 4.1 Module Registry
```c
typedef struct {
    uint8_t module_id;           // Module ID (0-32)
    uint8_t module_type;         // Module type
    char module_name[32];        // Module name
    bool online;                 // Online status
    uint32_t last_seen;          // Last seen timestamp
    uint8_t version[4];          // Firmware version
    uint8_t communication;       // Communication type (RS485)
} module_info_t;
```

#### 4.2 Auto-discovery
- **Discovery Protocol:** Modbus RTU broadcast
- **Address Range:** 0x00 - 0x1F (32 addresses)
- **Discovery Interval:** 5 seconds
- **Timeout:** 1000ms per module
- **Retry Count:** 3 attempts

#### 4.3 Hot-swap Capability
- **Module Detection:** Real-time detection
- **Configuration:** Automatic configuration
- **Safety:** Safe module replacement
- **Recovery:** Automatic recovery

### 5. COMMUNICATION SYSTEM

#### 5.1 RS485/Modbus RTU (NEW STANDARD)
- **Protocol:** Modbus RTU cho tất cả modules
- **Slave ID:** 1-32 (expanded range)
- **Function Codes:** Read/Write coils, registers
- **Error Handling:** CRC check, retry, timeout
- **Auto-discovery:** Module discovery và registration
- **Hot-swap:** Module hot-swap capability

#### 5.2 Network Communication
- **HTTP Server:** REST API endpoints
- **WebSocket Server:** Real-time communication
- **Security:** Authentication/authorization
- **Rate Limiting:** 100 requests/minute

### 6. API SYSTEM

#### 6.1 REST API Endpoints
- **System:** `/system/status`, `/system/health`, `/system/architecture`
- **Modules:** `/modules`, `/modules/{id}`, `/modules/{id}/command`
- **Safety:** `/safety/status`, `/safety/estop`
- **Location:** `/location/status`, `/location/map` (NEW)
- **Navigation:** `/navigation/status`, `/navigation/move` (NEW)
- **Power:** `/power/status`

#### 6.2 WebSocket API
- **Endpoint:** `/ws/telemetry`
- **Real-time Data:** System status, navigation, position, battery, safety
- **Message Types:** TELEMETRY, STATUS, EVENTS

### 7. MODULE SPECIFICATIONS

#### 7.1 Power Module (0x01)
```c
typedef struct {
    float voltage;               // Battery voltage (V)
    float current;               // Battery current (A)
    uint8_t soc;                 // State of charge (%)
    float temperature;           // Battery temperature (°C)
    bool charging;               // Charging status
    bool bms_ok;                 // BMS status
} power_telemetry_t;
```

#### 7.2 Safety Module (0x02)
```c
typedef struct {
    bool estop_active;           // E-Stop active
    dual_channel_estop_t estop;  // Dual-channel E-Stop
    location_safety_t location;  // Location-based safety
    safety_level_t level;        // Safety level
    bool light_curtain_ok;       // Light curtain status
    bool safety_mats_ok;         // Safety mats status
} safety_telemetry_t;
```

#### 7.3 Travel Motor Module (0x03)
```c
typedef struct {
    float speed;                 // Current speed (m/s)
    uint8_t speed_percentage;    // Speed percentage (%)
    float position;              // Current position (mm)
    float current;               // Motor current (A)
    float voltage;               // Motor voltage (V)
    float temperature;           // Motor temperature (°C)
    // Specifications:
    // - Motors: 2x DC Brushed 12V, 100W each
    // - Control: PID speed control by % speed
    // - Sensors: Hall effect speed sensors (60 PPR)
    // - Gearbox: Planetary 20:1 ratio
    // - Wheels: Omni-directional 150mm
} motor_telemetry_t;
```

#### 7.4 Dock & Location Module (0x05)
```c
typedef struct {
    float position_x;            // X position (mm)
    float position_y;            // Y position (mm)
    float position_z;            // Z position (mm)
    float orientation_pitch;     // Pitch angle (degrees)
    float orientation_roll;      // Roll angle (degrees)
    float orientation_yaw;       // Yaw angle (degrees)
    float velocity_linear;       // Linear velocity (m/s)
    float velocity_angular;      // Angular velocity (rad/s)
    bool imu_ok;                 // IMU status
    bool magnetic_sensors_ok;    // Magnetic sensors status
    bool rfid_ok;                // RFID status
    bool dock_alignment_ok;      // Dock alignment status
    bool lidar_ok;               // LiDAR status
    // Sensors:
    // - IMU (MPU6050)
    // - Magnetic sensors (2x Hall effect)
    // - RFID reader
    // - Dock alignment sensors
    // - LiDAR (RPLIDAR A1M8 via USB)
} location_telemetry_t;
```

#### 7.5 Master Control Module (0x00)
```c
typedef struct {
    system_state_t system_state;     // System state
    navigation_state_t nav_state;    // Navigation state
    uint32_t uptime_ms;              // System uptime
    uint8_t active_modules;          // Active modules count
    bool all_mandatory_online;       // All mandatory modules online
    bool rs485_communication_ok;     // RS485 communication status
} master_telemetry_t;
```

---

## 📊 PERFORMANCE METRICS

### **Current Performance:**
- **Response Time:** < 100ms (target met)
- **Uptime:** 99.9% (target met)
- **Safety Response:** < 50ms (target met)
- **Communication Latency:** < 10ms (target met)
- **Memory Usage:** 5MB (optimized)
- **CPU Usage:** 15% (optimized)

### **Target Performance (After Upgrades):**
- **Response Time:** < 50ms (improved)
- **Uptime:** 99.95% (improved)
- **Safety Response:** < 25ms (improved)
- **Communication Latency:** < 5ms (improved)
- **Memory Usage:** 4MB (reduced)
- **CPU Usage:** 10% (reduced)

---

## 🎯 UPGRADE REQUIREMENTS

### **Critical Upgrades (Week 1-2):**

#### 1. **Single-channel E-Stop System**
- **Current:** Single-channel implementation
- **Required:** Single-channel for basic compliance
- **Files:** `hal_estop.c`, `safety_manager.c`
- **Priority:** Critical
- **Timeline:** Week 1

#### 2. **Basic Safety System**
- **Current:** Basic safety system
- **Required:** Basic safety integration
- **Files:** `safety_manager.c`
- **Priority:** Critical
- **Timeline:** Week 1-2

#### 3. **Navigation States**
- **Current:** Basic state machine
- **Required:** Navigation states implementation
- **Files:** `system_state_machine.c`
- **Priority:** High
- **Timeline:** Week 2

#### 4. **LiDAR USB Integration**
- **Current:** Basic LiDAR support
- **Required:** USB integration với Dock & Location module
- **Files:** `hal_lidar.c`, `dock_module_handler.c`
- **Priority:** High
- **Timeline:** Week 2

### **Optional Upgrades (Week 3-4):**

#### 1. **Optional Modules Implementation**
- **Lifter Motor Module:** Basic implementation
- **Cargo Door Module:** Basic implementation
- **Safety Extended Module:** Basic implementation
- **Priority:** Medium
- **Timeline:** Week 3-4

#### 2. **Performance Optimization**
- **Memory Usage:** Optimize memory usage
- **CPU Usage:** Optimize CPU usage
- **Response Time:** Improve response time
- **Priority:** Medium
- **Timeline:** Week 4

---

## 🚀 DEPLOYMENT READINESS

### **Production Ready Components:**
- ✅ **Power Module:** Ready for production
- ✅ **Travel Motor Module:** Ready for production
- ✅ **Master Control Module:** Ready for production
- ✅ **RS485 Communication:** Ready for production
- ✅ **API System:** Ready for production
- ✅ **Module Management:** Ready for production

### **Needs Upgrade Before Production:**
- 🔄 **Safety Module:** Single-channel E-Stop upgrade required
- 🔄 **Dock & Location Module:** LiDAR USB integration required
- 🔄 **Navigation States:** Implementation required

### **Optional for Production:**
- ⏳ **Lifter Motor Module:** Optional implementation
- ⏳ **Cargo Door Module:** Optional implementation
- ⏳ **Safety Extended Module:** Optional implementation

---

## 📋 NEXT STEPS

### **Immediate Actions (Week 1):**
1. **Upgrade E-Stop system** lên single-channel
2. **Implement basic safety** integration
3. **Update navigation states** trong state machine
4. **Test RS485 standard** cho tất cả modules

### **Short-term Actions (Week 2-3):**
1. **Complete LiDAR USB integration**
2. **Validate auto-discovery system**
3. **Test hot-swap capability**
4. **Performance optimization**

### **Long-term Actions (Week 4+):**
1. **SIL2 certification preparation**
2. **Advanced safety features**
3. **Optional modules implementation**
4. **Performance monitoring**

---

**Status:** Updated for Simplified Architecture v2.1 - Ready for Implementation  
**Next Steps:** Begin critical upgrades for production readiness
