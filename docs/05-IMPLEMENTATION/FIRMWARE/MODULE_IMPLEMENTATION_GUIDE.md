# MODULE IMPLEMENTATION GUIDE - OHT-50 (v2.0)

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** FW  
**Trạng thái:** New Implementation Guide for Architecture v2.0

---

## 📋 TỔNG QUAN

Hướng dẫn implementation chi tiết cho 5 module bắt buộc trong kiến trúc OHT-50 v2.0, bao gồm hardware interface, software implementation, và integration guidelines.

### **🏗️ Kiến trúc mới v2.0:**
- **5 Module bắt buộc:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **RS485 Standard:** Tất cả modules đều sử dụng RS485/Modbus RTU
- **Auto-discovery:** Tự động phát hiện và cấu hình modules
- **Hot-swap:** Thay đổi modules khi đang hoạt động
- **Location-based Safety:** Hệ thống an toàn dựa trên vị trí

---

## 🔧 MODULE 1: POWER MODULE (0x01)

### **Hardware Specifications:**
```
Module ID:          0x01
Communication:      RS485/Modbus RTU
Power Supply:       48V DC (LiFePO4 Battery)
BMS:                Integrated Battery Management System
Charging:           External charging station
Monitoring:         Voltage, Current, Temperature, SOC
Protection:         Overvoltage, Undervoltage, Overcurrent, Overtemperature
```

### **Register Map:**
```c
// Power Module Register Map (0x01)
#define POWER_REG_VOLTAGE          0x0001    // Battery voltage (V * 100)
#define POWER_REG_CURRENT          0x0002    // Battery current (A * 100)
#define POWER_REG_SOC              0x0003    // State of charge (0-100%)
#define POWER_REG_TEMPERATURE      0x0004    // Battery temperature (°C)
#define POWER_REG_STATUS           0x0005    // Power status flags
#define POWER_REG_CHARGING_STATUS  0x0006    // Charging status
#define POWER_REG_BMS_STATUS       0x0007    // BMS status flags
#define POWER_REG_PROTECTION       0x0008    // Protection status
#define POWER_REG_CAPACITY         0x0009    // Battery capacity (Ah)
#define POWER_REG_CYCLE_COUNT      0x000A    // Charge cycle count
```

### **Implementation:**
```c
typedef struct {
    uint16_t voltage;              // Battery voltage (V * 100)
    int16_t current;               // Battery current (A * 100)
    uint8_t soc;                   // State of charge (0-100%)
    int8_t temperature;            // Battery temperature (°C)
    uint16_t status;               // Power status flags
    uint8_t charging_status;       // Charging status
    uint16_t bms_status;           // BMS status flags
    uint16_t protection;           // Protection status
    uint16_t capacity;             // Battery capacity (Ah)
    uint16_t cycle_count;          // Charge cycle count
} power_module_data_t;

// Power status flags
#define POWER_STATUS_OK            0x0001
#define POWER_STATUS_LOW_BATTERY   0x0002
#define POWER_STATUS_CHARGING      0x0004
#define POWER_STATUS_FAULT         0x0008
#define POWER_STATUS_EMERGENCY     0x0010

// Charging status
#define CHARGING_STATUS_IDLE       0x00
#define CHARGING_STATUS_CHARGING   0x01
#define CHARGING_STATUS_FULL       0x02
#define CHARGING_STATUS_ERROR      0x03
```

### **Integration Guidelines:**
1. **Battery Monitoring:** Real-time voltage, current, temperature monitoring
2. **BMS Integration:** Battery Management System for protection
3. **Charging Control:** External charging station communication
4. **Safety Integration:** Low battery warning và emergency shutdown
5. **Telemetry:** Continuous power status reporting

---

## 🔧 MODULE 2: SAFETY MODULE (0x02)

### **Hardware Specifications:**
```
Module ID:          0x02
Communication:      RS485/Modbus RTU
E-Stop:            Dual-channel E-Stop (SIL2 compliant)
Safety Sensors:    Light curtain, Safety mats, Door sensors
Emergency Brake:   Emergency brake control
Location Safety:   Location-based safety integration
Monitoring:        Real-time safety status
```

### **Register Map:**
```c
// Safety Module Register Map (0x02)
#define SAFETY_REG_ESTOP_STATUS    0x0001    // E-Stop status
#define SAFETY_REG_ESTOP_CHANNEL1  0x0002    // E-Stop channel 1
#define SAFETY_REG_ESTOP_CHANNEL2  0x0003    // E-Stop channel 2
#define SAFETY_REG_SAFETY_LEVEL    0x0004    // Safety level
#define SAFETY_REG_SENSORS         0x0005    // Safety sensors status
#define SAFETY_REG_LOCATION_SAFETY 0x0006    // Location-based safety
#define SAFETY_REG_EMERGENCY_BRAKE 0x0007    // Emergency brake status
#define SAFETY_REG_FAULT_STATUS    0x0008    // Fault status
#define SAFETY_REG_RESET_STATUS    0x0009    // Reset status
#define SAFETY_REG_SIL2_STATUS     0x000A    // SIL2 compliance status
```

### **Implementation:**
```c
typedef struct {
    uint16_t estop_status;         // E-Stop status
    uint8_t estop_channel1;        // E-Stop channel 1
    uint8_t estop_channel2;        // E-Stop channel 2
    uint8_t safety_level;          // Safety level
    uint16_t sensors;              // Safety sensors status
    uint16_t location_safety;      // Location-based safety
    uint8_t emergency_brake;       // Emergency brake status
    uint16_t fault_status;         // Fault status
    uint8_t reset_status;          // Reset status
    uint16_t sil2_status;          // SIL2 compliance status
} safety_module_data_t;

// E-Stop status
#define ESTOP_STATUS_OK            0x0001
#define ESTOP_STATUS_ACTIVATED     0x0002
#define ESTOP_STATUS_FAULT         0x0004
#define ESTOP_STATUS_RESET         0x0008

// Safety levels
#define SAFETY_LEVEL_NORMAL        0x00
#define SAFETY_LEVEL_WARNING       0x01
#define SAFETY_LEVEL_CRITICAL      0x02
#define SAFETY_LEVEL_EMERGENCY     0x03

// Safety sensors
#define SENSOR_LIGHT_CURTAIN       0x0001
#define SENSOR_SAFETY_MATS         0x0002
#define SENSOR_DOOR_SENSORS        0x0004
#define SENSOR_EMERGENCY_BRAKE     0x0008
```

### **Integration Guidelines:**
1. **Dual-channel E-Stop:** SIL2 compliant dual-channel implementation
2. **Safety Sensors:** Light curtain, safety mats, door sensors integration
3. **Location-based Safety:** Integration với Dock & Location module
4. **Emergency Brake:** Emergency brake control system
5. **Fault Handling:** Comprehensive fault detection và handling

---

## 🔧 MODULE 3: TRAVEL MOTOR MODULE (0x03)

### **Hardware Specifications:**
```
Module ID:          0x03
Communication:      RS485/Modbus RTU
Motors:            2x DC Brushed Motors (12V, 100W each)
Control:           PID speed control by % speed
Sensors:           Hall effect speed sensors (60 PPR)
Gearbox:           Planetary gearbox (20:1 ratio)
Wheels:            Omni-directional wheels (150mm diameter)
Load Capacity:     50kg
Speed Range:       0-2 m/s
```

### **Register Map:**
```c
// Travel Motor Module Register Map (0x03)
#define MOTOR_REG_SPEED            0x0001    // Current speed (m/s * 100)
#define MOTOR_REG_SPEED_PERCENT    0x0002    // Speed percentage (0-100%)
#define MOTOR_REG_TARGET_SPEED     0x0003    // Target speed (m/s * 100)
#define MOTOR_REG_POSITION         0x0004    // Current position (mm)
#define MOTOR_REG_CURRENT          0x0005    // Motor current (A * 100)
#define MOTOR_REG_VOLTAGE          0x0006    // Motor voltage (V * 100)
#define MOTOR_REG_TEMPERATURE      0x0007    // Motor temperature (°C)
#define MOTOR_REG_STATUS           0x0008    // Motor status
#define MOTOR_REG_ENCODER_LEFT     0x0009    // Left encoder count
#define MOTOR_REG_ENCODER_RIGHT    0x000A    // Right encoder count
#define MOTOR_REG_PID_KP           0x000B    // PID proportional gain
#define MOTOR_REG_PID_KI           0x000C    // PID integral gain
#define MOTOR_REG_PID_KD           0x000D    // PID derivative gain
```

### **Implementation:**
```c
typedef struct {
    uint16_t speed;                // Current speed (m/s * 100)
    uint8_t speed_percent;         // Speed percentage (0-100%)
    uint16_t target_speed;         // Target speed (m/s * 100)
    int32_t position;              // Current position (mm)
    int16_t current;               // Motor current (A * 100)
    uint16_t voltage;              // Motor voltage (V * 100)
    int8_t temperature;            // Motor temperature (°C)
    uint16_t status;               // Motor status
    int32_t encoder_left;          // Left encoder count
    int32_t encoder_right;         // Right encoder count
    uint16_t pid_kp;               // PID proportional gain
    uint16_t pid_ki;               // PID integral gain
    uint16_t pid_kd;               // PID derivative gain
} motor_module_data_t;

// Motor status flags
#define MOTOR_STATUS_OK            0x0001
#define MOTOR_STATUS_RUNNING       0x0002
#define MOTOR_STATUS_STOPPED       0x0004
#define MOTOR_STATUS_FAULT         0x0008
#define MOTOR_STATUS_OVERLOAD      0x0010
#define MOTOR_STATUS_OVERTEMP      0x0020

// PID Control
#define PID_KP_DEFAULT             1000      // Proportional gain
#define PID_KI_DEFAULT             100       // Integral gain
#define PID_KD_DEFAULT             50        // Derivative gain
```

### **Integration Guidelines:**
1. **PID Control:** Speed control using PID algorithm
2. **Encoder Feedback:** Hall effect sensors for position feedback
3. **Speed Control:** Percentage-based speed control
4. **Backend Integration:** Velocity control (m/s) via backend settings
5. **Safety Integration:** Emergency stop và overload protection

---

## 🔧 MODULE 4: DOCK & LOCATION MODULE (0x05)

### **Hardware Specifications:**
```
Module ID:          0x05
Communication:      RS485/Modbus RTU
IMU:               MPU6050 (pitch, roll, yaw)
Magnetic Sensors:  2x Hall effect sensors
RFID Reader:       RFID tag reading
Dock Alignment:    Dock alignment sensors
LiDAR:             RPLIDAR A1M8 (via USB)
GPS/GNSS:          Optional GPS module
Positioning:       SLAM-based localization
Mapping:           Real-time mapping
```

### **Register Map:**
```c
// Dock & Location Module Register Map (0x05)
#define LOCATION_REG_POSITION_X    0x0001    // X position (mm)
#define LOCATION_REG_POSITION_Y    0x0002    // Y position (mm)
#define LOCATION_REG_POSITION_Z    0x0003    // Z position (mm)
#define LOCATION_REG_ORIENTATION_P 0x0004    // Pitch angle (degrees * 10)
#define LOCATION_REG_ORIENTATION_R 0x0005    // Roll angle (degrees * 10)
#define LOCATION_REG_ORIENTATION_Y 0x0006    // Yaw angle (degrees * 10)
#define LOCATION_REG_VELOCITY_L    0x0007    // Linear velocity (m/s * 100)
#define LOCATION_REG_VELOCITY_A    0x0008    // Angular velocity (rad/s * 100)
#define LOCATION_REG_SENSORS       0x0009    // Sensors status
#define LOCATION_REG_LIDAR_STATUS  0x000A    // LiDAR status
#define LOCATION_REG_DOCK_STATUS   0x000B    // Dock status
#define LOCATION_REG_RFID_DATA     0x000C    // RFID data
#define LOCATION_REG_MAP_STATUS    0x000D    // Map status
#define LOCATION_REG_NAVIGATION    0x000E    // Navigation state
```

### **Implementation:**
```c
typedef struct {
    int32_t position_x;            // X position (mm)
    int32_t position_y;            // Y position (mm)
    int32_t position_z;            // Z position (mm)
    int16_t orientation_pitch;     // Pitch angle (degrees * 10)
    int16_t orientation_roll;      // Roll angle (degrees * 10)
    int16_t orientation_yaw;       // Yaw angle (degrees * 10)
    int16_t velocity_linear;       // Linear velocity (m/s * 100)
    int16_t velocity_angular;      // Angular velocity (rad/s * 100)
    uint16_t sensors;              // Sensors status
    uint16_t lidar_status;         // LiDAR status
    uint8_t dock_status;           // Dock status
    uint32_t rfid_data;            // RFID data
    uint16_t map_status;           // Map status
    uint8_t navigation_state;      // Navigation state
} location_module_data_t;

// Sensors status
#define SENSOR_IMU_OK              0x0001
#define SENSOR_MAGNETIC_OK         0x0002
#define SENSOR_RFID_OK             0x0004
#define SENSOR_DOCK_ALIGNMENT_OK   0x0008
#define SENSOR_LIDAR_OK            0x0010
#define SENSOR_GPS_OK              0x0020

// LiDAR status
#define LIDAR_STATUS_ACTIVE        0x0001
#define LIDAR_STATUS_SCANNING      0x0002
#define LIDAR_STATUS_MAPPING       0x0004
#define LIDAR_STATUS_ERROR         0x0008
#define LIDAR_STATUS_USB_CONNECTED 0x0010

// Navigation states
#define NAV_STATE_IDLE             0x00
#define NAV_STATE_NAVIGATING       0x01
#define NAV_STATE_POSITIONING      0x02
#define NAV_STATE_DOCKING          0x03
#define NAV_STATE_UNDOCKING        0x04
```

### **Integration Guidelines:**
1. **Multi-sensor Fusion:** IMU, magnetic sensors, RFID integration
2. **LiDAR USB Integration:** RPLIDAR A1M8 via USB connection
3. **SLAM Implementation:** Simultaneous Localization and Mapping
4. **Dock Alignment:** Precise docking alignment system
5. **Location-based Safety:** Integration với Safety module

---

## 🔧 MODULE 5: MASTER CONTROL MODULE (0x00)

### **Hardware Specifications:**
```
Module ID:          0x00
Communication:      RS485/Modbus RTU + HTTP/WebSocket
Processor:         Orange Pi 5B (RK3588)
Memory:            8GB RAM
Storage:           64GB eMMC
Network:           Ethernet + WiFi
USB:               USB 2.0 (LiDAR, debug)
GPIO:              5x LEDs, 2x Relays, E-Stop
System Control:    Overall system coordination
Module Management: Auto-discovery và hot-swap
```

### **Register Map:**
```c
// Master Control Module Register Map (0x00)
#define MASTER_REG_SYSTEM_STATE    0x0001    // System state
#define MASTER_REG_NAVIGATION_STATE 0x0002   // Navigation state
#define MASTER_REG_UPTIME          0x0003    // System uptime (seconds)
#define MASTER_REG_ACTIVE_MODULES  0x0004    // Active modules count
#define MASTER_REG_MANDATORY_STATUS 0x0005   // Mandatory modules status
#define MASTER_REG_RS485_STATUS    0x0006    // RS485 communication status
#define MASTER_REG_NETWORK_STATUS  0x0007    // Network status
#define MASTER_REG_API_STATUS      0x0008    // API server status
#define MASTER_REG_WEBSOCKET_STATUS 0x0009   // WebSocket status
#define MASTER_REG_ERROR_COUNT     0x000A    // Error count
#define MASTER_REG_VERSION         0x000B    // Firmware version
#define MASTER_REG_BUILD_DATE      0x000C    // Build date
```

### **Implementation:**
```c
typedef struct {
    uint8_t system_state;          // System state
    uint8_t navigation_state;      // Navigation state
    uint32_t uptime;               // System uptime (seconds)
    uint8_t active_modules;        // Active modules count
    uint16_t mandatory_status;     // Mandatory modules status
    uint16_t rs485_status;         // RS485 communication status
    uint16_t network_status;       // Network status
    uint16_t api_status;           // API server status
    uint16_t websocket_status;     // WebSocket status
    uint16_t error_count;          // Error count
    uint32_t version;              // Firmware version
    uint32_t build_date;           // Build date
} master_module_data_t;

// System states
#define SYSTEM_STATE_IDLE          0x00
#define SYSTEM_STATE_MOVE          0x01
#define SYSTEM_STATE_DOCK          0x02
#define SYSTEM_STATE_FAULT         0x03
#define SYSTEM_STATE_ESTOP         0x04

// Navigation states
#define NAV_STATE_IDLE             0x00
#define NAV_STATE_NAVIGATING       0x01
#define NAV_STATE_POSITIONING      0x02
#define NAV_STATE_DOCKING          0x03
#define NAV_STATE_UNDOCKING        0x04

// Mandatory modules status
#define MODULE_POWER_OK            0x0001
#define MODULE_SAFETY_OK           0x0002
#define MODULE_TRAVEL_MOTOR_OK     0x0004
#define MODULE_DOCK_LOCATION_OK    0x0008
#define MODULE_MASTER_CONTROL_OK   0x0010
```

### **Integration Guidelines:**
1. **System Coordination:** Overall system state management
2. **Module Management:** Auto-discovery và hot-swap capability
3. **Communication:** RS485 + HTTP/WebSocket dual communication
4. **API Management:** RESTful API và WebSocket server
5. **Network Management:** Ethernet + WiFi redundancy

---

## 🔄 AUTO-DISCOVERY PROTOCOL

### **Discovery Process:**
```c
// Auto-discovery protocol
#define DISCOVERY_BROADCAST_ADDR   0xFF      // Broadcast address
#define DISCOVERY_CMD_DISCOVER     0x01      // Discovery command
#define DISCOVERY_CMD_RESPONSE     0x02      // Discovery response
#define DISCOVERY_CMD_REGISTER     0x03      // Register command

// Discovery message format
typedef struct {
    uint8_t command;               // Discovery command
    uint8_t module_id;             // Module ID
    uint8_t module_type;           // Module type
    char module_name[32];          // Module name
    uint32_t version;              // Firmware version
    uint8_t communication;         // Communication type
} discovery_message_t;
```

### **Implementation:**
1. **Broadcast Discovery:** Master sends discovery broadcast
2. **Module Response:** Modules respond with their information
3. **Registration:** Master registers discovered modules
4. **Health Monitoring:** Continuous health monitoring
5. **Hot-swap Support:** Module replacement without restart

---

## 🔧 INTEGRATION GUIDELINES

### **RS485 Communication:**
```c
// RS485 configuration
#define RS485_BAUDRATE             115200    // Baud rate
#define RS485_DATA_BITS            8         // Data bits
#define RS485_STOP_BITS            1         // Stop bits
#define RS485_PARITY               NONE      // Parity
#define RS485_TERMINATION          120       // Termination (Ω)
#define RS485_BIAS                 560       // Bias (Ω)
#define RS485_TIMEOUT              1000      // Timeout (ms)
#define RS485_RETRY_COUNT          3         // Retry count
```

### **Module Communication:**
1. **Modbus RTU:** Standard Modbus RTU protocol
2. **Register Access:** Read/Write register operations
3. **Error Handling:** CRC check, retry, timeout
4. **Broadcast Support:** Broadcast commands support
5. **Multi-drop:** Up to 32 modules on single bus

### **Safety Integration:**
1. **Location-based Safety:** Integration với Dock & Location module
2. **Dual-channel E-Stop:** SIL2 compliant safety system
3. **Emergency Brake:** Emergency brake control
4. **Fault Detection:** Comprehensive fault detection
5. **Safe Shutdown:** Safe shutdown procedures

---

## 📊 TESTING GUIDELINES

### **Unit Testing:**
1. **Module Communication:** Test RS485 communication
2. **Register Access:** Test register read/write operations
3. **Error Handling:** Test error conditions
4. **Auto-discovery:** Test discovery protocol
5. **Safety Functions:** Test safety features

### **Integration Testing:**
1. **Module Integration:** Test module interactions
2. **System Coordination:** Test system coordination
3. **Safety Integration:** Test safety system integration
4. **Performance Testing:** Test performance requirements
5. **Stress Testing:** Test under stress conditions

### **Validation Testing:**
1. **Functional Testing:** Test all functions
2. **Safety Validation:** Validate safety requirements
3. **Performance Validation:** Validate performance requirements
4. **Compliance Testing:** Test compliance requirements
5. **User Acceptance:** User acceptance testing

---

**Status:** Implementation Guide Complete  
**Next Steps:** Create detailed module specifications và integration documents
