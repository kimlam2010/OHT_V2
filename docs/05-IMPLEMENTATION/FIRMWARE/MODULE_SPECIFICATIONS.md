# MODULE SPECIFICATIONS - OHT-50 (v2.0)

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** FW  
**Trạng thái:** Detailed Specifications for 5 Mandatory Modules

---

## 📋 TỔNG QUAN

Specifications chi tiết cho 5 module bắt buộc trong kiến trúc OHT-50 v2.0, bao gồm hardware requirements, software interfaces, communication protocols, và performance specifications.

### **🏗️ Kiến trúc mới v2.0:**
- **5 Module bắt buộc:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **RS485 Standard:** Tất cả modules đều sử dụng RS485/Modbus RTU
- **Auto-discovery:** Tự động phát hiện và cấu hình modules
- **Hot-swap:** Thay đổi modules khi đang hoạt động
- **Location-based Safety:** Hệ thống an toàn dựa trên vị trí

---

## 🔋 MODULE 1: POWER MODULE (0x01)

### **Hardware Requirements:**

#### **Battery System:**
```
Type:               LiFePO4 Battery Pack
Voltage:            24V DC (nominal)
Capacity:           50Ah (configurable)
Chemistry:          Lithium Iron Phosphate
Cycle Life:         2000+ cycles
Operating Temperature: -20°C to +60°C
Storage Temperature: -40°C to +85°C
Weight:             ~15kg
Dimensions:         400mm x 300mm x 150mm
```

#### **Battery Management System (BMS):**
```
Microcontroller:    STM32F407 or equivalent
ADC Resolution:     12-bit
Voltage Monitoring: 16-cell monitoring
Current Monitoring: ±100A range, 0.1A resolution
Temperature Monitoring: 8x NTC sensors
Protection Features:
  - Overvoltage protection (3.65V per cell)
  - Undervoltage protection (2.5V per cell)
  - Overcurrent protection (100A continuous, 200A peak)
  - Overtemperature protection (60°C)
  - Short circuit protection
  - Cell balancing (passive)
```

#### **Charging System:**
```
Charging Method:    CC-CV (Constant Current - Constant Voltage)
Charging Current:   20A maximum
Charging Voltage:   28.8V (3.6V per cell)
Charging Time:      5-6 hours (0-100%)
Charging Efficiency: >95%
Charging Interface: 4-pin connector (power, ground, communication, temperature)
```

### **Software Interface:**

#### **Register Map (Modbus RTU):**
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
#define POWER_REG_CELL_VOLTAGES    0x000B    // Cell voltages (16 cells)
#define POWER_REG_CELL_TEMPERATURES 0x001B   // Cell temperatures (8 sensors)
#define POWER_REG_BALANCING_STATUS 0x0023    // Cell balancing status
#define POWER_REG_FAULT_CODES      0x0024    // Fault codes
#define POWER_REG_SERIAL_NUMBER    0x0025    // Serial number
#define POWER_REG_FIRMWARE_VERSION 0x0026    // Firmware version
```

#### **Data Structures:**
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
    uint16_t cell_voltages[16];    // Cell voltages (mV)
    int8_t cell_temperatures[8];   // Cell temperatures (°C)
    uint16_t balancing_status;     // Cell balancing status
    uint16_t fault_codes;          // Fault codes
    uint32_t serial_number;        // Serial number
    uint32_t firmware_version;     // Firmware version
} power_module_data_t;
```

### **Performance Specifications:**
```
Voltage Accuracy:   ±0.5% of full scale
Current Accuracy:   ±1% of full scale
Temperature Accuracy: ±1°C
SOC Accuracy:       ±2% of full scale
Update Rate:        10Hz
Response Time:      <100ms
Communication:      RS485/Modbus RTU, 115200 bps
```

---

## 🛡️ MODULE 2: SAFETY MODULE (0x02)

### **Hardware Requirements:**

#### **Dual-channel E-Stop System:**
```
Channels:           2 independent channels
Compliance:         SIL2 (Safety Integrity Level 2)
Response Time:      <50ms
Debounce Time:      20ms per channel
Reset Method:       Manual reset required
Fault Detection:    Cross-monitoring between channels
Diagnostics:        Continuous self-diagnostics
```

#### **Safety Sensors:**
```
Light Curtain:
  - Type:           Safety light curtain
  - Resolution:     14mm
  - Range:          0.5m - 3m
  - Response Time:  <20ms
  - Protection Level: Type 4 (IEC 61496-1)

Safety Mats:
  - Type:           Pressure-sensitive safety mats
  - Sensitivity:    5kg minimum
  - Response Time:  <100ms
  - Coverage:       Configurable zones

Door Sensors:
  - Type:           Magnetic door sensors
  - Response Time:  <50ms
  - Contact Rating: 24V DC, 2A
```

#### **Emergency Brake System:**
```
Type:               Electromagnetic brake
Activation Time:    <100ms
Holding Force:      500N minimum
Power Supply:       24V DC
Current Draw:       2A maximum
Status Monitoring:  Brake engagement/disengagement
```

### **Software Interface:**

#### **Register Map (Modbus RTU):**
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
#define SAFETY_REG_DIAGNOSTICS     0x000B    // Diagnostic data
#define SAFETY_REG_SAFETY_ZONES    0x000C    // Safety zones configuration
#define SAFETY_REG_SPEED_LIMITS    0x000D    // Speed limits per zone
#define SAFETY_REG_FAULT_HISTORY   0x000E    // Fault history
#define SAFETY_REG_TEST_RESULTS    0x000F    // Self-test results
```

#### **Data Structures:**
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
    uint32_t diagnostics;          // Diagnostic data
    uint16_t safety_zones[8];      // Safety zones configuration
    uint16_t speed_limits[8];      // Speed limits per zone (m/s * 100)
    uint16_t fault_history[10];    // Fault history
    uint16_t test_results;         // Self-test results
} safety_module_data_t;
```

### **Performance Specifications:**
```
E-Stop Response Time: <50ms
Sensor Response Time: <100ms
Diagnostic Coverage:  >90%
Fault Detection Time: <200ms
Communication:       RS485/Modbus RTU, 115200 bps
Update Rate:         20Hz
```

---

## 🚗 MODULE 3: TRAVEL MOTOR MODULE (0x03)

### **Hardware Requirements:**

#### **Motor System:**
```
Motors:             2x DC Brushed Motors
Voltage:            12V DC
Power:              100W each (200W total)
Speed Range:        0-3000 RPM
Torque:             2.5 Nm each
Efficiency:         >85%
```

#### **Motor Specifications:**
```
Type:               DC Brushed Motor
Voltage:            12V DC
Current:            8.3A maximum per motor
Power:              100W continuous
Speed:              0-3000 RPM
Torque:             2.5 Nm maximum
Efficiency:         >85%
Weight:             1.2kg each
```

#### **Encoder System:**
```
Type:               Hall effect sensors
Resolution:         60 PPR (Pulses Per Revolution)
Accuracy:           ±0.1%
Response Time:      <1ms
Output:             Quadrature signals (A, B, Index)
```

#### **Gearbox System:**
```
Type:               Planetary gearbox
Ratio:              20:1
Efficiency:         >90%
Backlash:           <1°
Lubrication:        Grease lubricated
Maintenance:        10,000 hours
```

#### **Wheel System:**
```
Type:               Omni-directional wheels
Diameter:           150mm
Width:              50mm
Load Capacity:      50kg total
Material:           Polyurethane
Grip:               High traction
```

### **Software Interface:**

#### **Register Map (Modbus RTU):**
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
#define MOTOR_REG_CONTROL_MODE     0x000E    // Control mode
#define MOTOR_REG_ACCELERATION     0x000F    // Acceleration rate
#define MOTOR_REG_DECELERATION     0x0010    // Deceleration rate
#define MOTOR_REG_FAULT_CODES      0x0011    // Fault codes
#define MOTOR_REG_MAINTENANCE      0x0012    // Maintenance data
```

#### **Data Structures:**
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
    uint8_t control_mode;          // Control mode
    uint16_t acceleration;         // Acceleration rate (m/s² * 100)
    uint16_t deceleration;         // Deceleration rate (m/s² * 100)
    uint16_t fault_codes;          // Fault codes
    uint32_t maintenance;          // Maintenance data
} motor_module_data_t;
```

### **Performance Specifications:**
```
Speed Accuracy:     ±2% of setpoint
Position Accuracy:  ±1mm
Response Time:      <100ms
Speed Range:        0-2 m/s
Acceleration:       0.5 m/s² maximum
Deceleration:       1.0 m/s² maximum
Communication:      RS485/Modbus RTU, 115200 bps
Update Rate:        50Hz
```

---

## 🧭 MODULE 4: DOCK & LOCATION MODULE (0x05)

### **Hardware Requirements:**

#### **IMU System:**
```
Model:              MPU6050
Accelerometer:      3-axis, ±2g/±4g/±8g/±16g
Gyroscope:          3-axis, ±250/±500/±1000/±2000°/s
Temperature:        -40°C to +85°C
Resolution:         16-bit
Update Rate:        1kHz
Interface:          I2C
```

#### **Magnetic Sensors:**
```
Type:               Hall effect sensors
Quantity:           2 sensors
Range:              ±50mT
Resolution:         0.1mT
Accuracy:           ±1%
Response Time:      <1ms
Interface:          Analog output
```

#### **RFID System:**
```
Type:               RFID reader/writer
Frequency:          13.56 MHz (HF)
Protocol:           ISO14443A/B, ISO15693
Read Range:         5-10cm
Write Range:        2-5cm
Tags:               Passive RFID tags
Interface:          UART/SPI
```

#### **Dock Alignment Sensors:**
```
Type:               Infrared sensors
Quantity:           4 sensors
Range:              0-50cm
Accuracy:           ±1mm
Response Time:      <10ms
Interface:          Digital output
```

#### **LiDAR System:**
```
Model:              RPLIDAR A1M8
Interface:          USB 2.0
Range:              0.15m - 12m
Angular Resolution: 0.9°
Scan Frequency:     5.5Hz
Rotation Speed:     330 RPM
Accuracy:           ±2cm (0.15m-1m), ±3cm (1m-6m), ±5cm (6m-12m)
```

### **Software Interface:**

#### **Register Map (Modbus RTU):**
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
#define LOCATION_REG_ACCURACY      0x000F    // Position accuracy
#define LOCATION_REG_FAULT_CODES   0x0010    // Fault codes
#define LOCATION_REG_CALIBRATION   0x0011    // Calibration status
```

#### **Data Structures:**
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
    uint16_t accuracy;             // Position accuracy (mm)
    uint16_t fault_codes;          // Fault codes
    uint16_t calibration;          // Calibration status
} location_module_data_t;
```

### **Performance Specifications:**
```
Position Accuracy:  ±10mm (static), ±50mm (dynamic)
Orientation Accuracy: ±1° (static), ±5° (dynamic)
Update Rate:        20Hz
Response Time:      <100ms
Communication:      RS485/Modbus RTU, 115200 bps
LiDAR Update Rate:  5.5Hz
```

---

## 🎛️ MODULE 5: MASTER CONTROL MODULE (0x00)

### **Hardware Requirements:**

#### **Processor System:**
```
Processor:          Orange Pi 5B (RK3588)
CPU:                4x Cortex-A76 + 4x Cortex-A55
Frequency:          2.4GHz maximum
Memory:             8GB LPDDR4X
Storage:            64GB eMMC
GPU:                Mali-G610 MP4
NPU:                6 TOPS AI accelerator
```

#### **Communication Interfaces:**
```
RS485:              UART1 (115200 bps)
Ethernet:           10/100/1000 Mbps
WiFi:               802.11ac (2.4G/5G)
USB:                4x USB 3.0, 1x USB 2.0
GPIO:               40-pin header
I2C:                4x I2C interfaces
SPI:                2x SPI interfaces
```

#### **GPIO Configuration:**
```
LEDs:               5x status LEDs
Relays:             2x relay outputs (24V, 2A)
E-Stop:             1x E-Stop input
Debug:              1x debug UART
```

### **Software Interface:**

#### **Register Map (Modbus RTU):**
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
#define MASTER_REG_CPU_USAGE       0x000D    // CPU usage (%)
#define MASTER_REG_MEMORY_USAGE    0x000E    // Memory usage (%)
#define MASTER_REG_TEMPERATURE     0x000F    // System temperature (°C)
#define MASTER_REG_DISCOVERY_STATUS 0x0010   // Auto-discovery status
#define MASTER_REG_CONFIG_STATUS   0x0011    // Configuration status
```

#### **Data Structures:**
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
    uint8_t cpu_usage;             // CPU usage (%)
    uint8_t memory_usage;          // Memory usage (%)
    int8_t temperature;            // System temperature (°C)
    uint16_t discovery_status;     // Auto-discovery status
    uint16_t config_status;        // Configuration status
} master_module_data_t;
```

### **Performance Specifications:**
```
Response Time:      <50ms
Update Rate:        10Hz
Communication:      RS485/Modbus RTU + HTTP/WebSocket
API Performance:    1000 requests/second
WebSocket Clients:  10 concurrent clients
Uptime:             >99.9%
```

---

## 🔄 COMMUNICATION PROTOCOL

### **RS485 Configuration:**
```
Baud Rate:          115200 bps
Data Bits:          8
Stop Bits:          1
Parity:             None
Termination:        120Ω
Bias:               560Ω
Timeout:            1000ms
Retry Count:        3
Address Range:      0x00 - 0x1F (32 addresses)
```

### **Modbus RTU Protocol:**
```
Function Codes:
  - 0x03: Read Holding Registers
  - 0x06: Write Single Register
  - 0x10: Write Multiple Registers
  - 0x14: Read File Record
  - 0x15: Write File Record

Error Handling:
  - CRC check
  - Timeout detection
  - Retry mechanism
  - Error reporting
```

### **Auto-discovery Protocol:**
```
Discovery Process:
  1. Master sends broadcast discovery command
  2. Modules respond with their information
  3. Master registers discovered modules
  4. Health monitoring begins
  5. Hot-swap support enabled
```

---

## 📊 INTEGRATION REQUIREMENTS

### **System Integration:**
1. **Module Coordination:** All modules must coordinate through Master Control
2. **Safety Integration:** Safety module must integrate with all other modules
3. **Location-based Safety:** Location data must integrate with safety system
4. **Power Management:** Power module must provide status to all modules
5. **Communication:** All modules must use RS485 standard

### **Performance Requirements:**
1. **Response Time:** <100ms for all critical operations
2. **Update Rate:** 10Hz minimum for all modules
3. **Reliability:** >99.9% uptime
4. **Safety:** SIL2 compliance for safety functions
5. **Scalability:** Support for up to 32 modules

### **Testing Requirements:**
1. **Unit Testing:** Individual module testing
2. **Integration Testing:** Module interaction testing
3. **System Testing:** Full system testing
4. **Safety Testing:** Safety function validation
5. **Performance Testing:** Performance validation

---

**Status:** Module Specifications Complete  
**Next Steps:** Create integration documents và testing procedures
