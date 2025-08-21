# Register Map Summary - OHT-50 Master Module (Phiên bản 1.0)

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Team:** FW Team  
**Trạng thái:** ⚡ PRODUCTION READY

---

## 📊 **TỔNG HỢP REGISTER MAP TẤT CẢ MODULES**

### **Bảng Register Map Tổng Quan:**

| Module | Address | Type | Register Range | Description |
|--------|---------|------|----------------|-------------|
| Power | 0x02 | MODULE_TYPE_POWER | 0x0000-0x000F | Power management & monitoring |
| Motor | 0x03 | MODULE_TYPE_MOTOR | 0x0000-0x0011 | Motor control & position |
| IO | 0x04 | MODULE_TYPE_IO | 0x0000-0x0015 | Digital/Analog I/O |
| Dock | 0x05 | MODULE_TYPE_DOCK | 0x0000-0x0012 | Docking mechanism |
| AI | 0x07 | MODULE_TYPE_AI | 0x0000-0x0018 | AI processing & vision |

---

## ⚡ **POWER MODULE (0x02) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| VOLTAGE_IN | 0x0001 | uint16_t | Input voltage (V) | 0.1V |
| VOLTAGE_OUT | 0x0002 | uint16_t | Output voltage (V) | 0.1V |
| CURRENT_IN | 0x0003 | uint16_t | Input current (A) | 0.01A |
| CURRENT_OUT | 0x0004 | uint16_t | Output current (A) | 0.01A |
| POWER_IN | 0x0005 | uint16_t | Input power (W) | 0.1W |
| POWER_OUT | 0x0006 | uint16_t | Output power (W) | 0.1W |
| TEMPERATURE | 0x0007 | uint16_t | Module temperature (°C) | 0.1°C |
| BATTERY_VOLTAGE | 0x0008 | uint16_t | Battery voltage (V) | 0.1V |
| BATTERY_CURRENT | 0x0009 | uint16_t | Battery current (A) | 0.01A |
| BATTERY_SOC | 0x000A | uint16_t | Battery state of charge (%) | 0.1% |
| BATTERY_SOH | 0x000B | uint16_t | Battery state of health (%) | 0.1% |
| FAULT_STATUS | 0x000C | uint16_t | Fault status | - |
| FAULT_CODE | 0x000D | uint16_t | Fault code | - |
| UPTIME | 0x000E | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x000F | uint32_t | Firmware version | - |

---

## 🚗 **MOTOR MODULE (0x03) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| POSITION | 0x0001 | int32_t | Current position (steps) | 1 step |
| TARGET_POSITION | 0x0002 | int32_t | Target position (steps) | 1 step |
| SPEED | 0x0003 | uint16_t | Current speed (RPM) | 0.1 RPM |
| TARGET_SPEED | 0x0004 | uint16_t | Target speed (RPM) | 0.1 RPM |
| TORQUE | 0x0005 | uint16_t | Current torque (Nm) | 0.01 Nm |
| TARGET_TORQUE | 0x0006 | uint16_t | Target torque (Nm) | 0.01 Nm |
| CURRENT | 0x0007 | uint16_t | Motor current (A) | 0.01A |
| VOLTAGE | 0x0008 | uint16_t | Motor voltage (V) | 0.1V |
| TEMPERATURE | 0x0009 | uint16_t | Motor temperature (°C) | 0.1°C |
| ENCODER_COUNT | 0x000A | int32_t | Encoder count | 1 count |
| ENCODER_VELOCITY | 0x000B | int16_t | Encoder velocity | 0.1 RPM |
| FAULT_STATUS | 0x000C | uint16_t | Fault status | - |
| FAULT_CODE | 0x000D | uint16_t | Fault code | - |
| CONTROL_MODE | 0x000E | uint8_t | Control mode | - |
| MOTION_STATUS | 0x000F | uint8_t | Motion status | - |
| UPTIME | 0x0010 | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x0011 | uint32_t | Firmware version | - |

---

## 🔌 **IO MODULE (0x04) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| DI_STATUS | 0x0001 | uint16_t | Digital input status | - |
| DO_STATUS | 0x0002 | uint16_t | Digital output status | - |
| AI_VALUE_1 | 0x0003 | uint16_t | Analog input 1 (V) | 0.001V |
| AI_VALUE_2 | 0x0004 | uint16_t | Analog input 2 (V) | 0.001V |
| AI_VALUE_3 | 0x0005 | uint16_t | Analog input 3 (V) | 0.001V |
| AI_VALUE_4 | 0x0006 | uint16_t | Analog input 4 (V) | 0.001V |
| AO_VALUE_1 | 0x0007 | uint16_t | Analog output 1 (V) | 0.001V |
| AO_VALUE_2 | 0x0008 | uint16_t | Analog output 2 (V) | 0.001V |
| DI_CONFIG | 0x0009 | uint16_t | Digital input configuration | - |
| DO_CONFIG | 0x000A | uint16_t | Digital output configuration | - |
| AI_CONFIG | 0x000B | uint16_t | Analog input configuration | - |
| AO_CONFIG | 0x000C | uint16_t | Analog output configuration | - |
| SAFETY_IO_STATUS | 0x000D | uint16_t | Safety I/O status | - |
| FAULT_STATUS | 0x000E | uint16_t | Fault status | - |
| FAULT_CODE | 0x000F | uint16_t | Fault code | - |
| UPTIME | 0x0010 | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x0011 | uint32_t | Firmware version | - |
| TEMPERATURE | 0x0012 | uint16_t | Module temperature (°C) | 0.1°C |
| SIGNAL_QUALITY | 0x0013 | uint16_t | Signal quality indicators | - |
| CALIBRATION_STATUS | 0x0014 | uint16_t | Calibration status | - |
| CALIBRATION_DATA | 0x0015 | uint16_t | Calibration data | - |

---

## 🎯 **DOCK MODULE (0x05) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| DOCK_STATUS | 0x0001 | uint16_t | Docking status | - |
| POSITION_X | 0x0002 | int16_t | X position (mm) | 0.1mm |
| POSITION_Y | 0x0003 | int16_t | Y position (mm) | 0.1mm |
| POSITION_Z | 0x0004 | int16_t | Z position (mm) | 0.1mm |
| ALIGNMENT_X | 0x0005 | int16_t | X alignment (mm) | 0.01mm |
| ALIGNMENT_Y | 0x0006 | int16_t | Y alignment (mm) | 0.01mm |
| ALIGNMENT_Z | 0x0007 | int16_t | Z alignment (mm) | 0.01mm |
| CONNECTION_STATUS | 0x0008 | uint16_t | Connection status | - |
| LATCH_STATUS | 0x0009 | uint16_t | Latch status | - |
| FORCE_SENSOR_X | 0x000A | int16_t | Force sensor X (N) | 0.1N |
| FORCE_SENSOR_Y | 0x000B | int16_t | Force sensor Y (N) | 0.1N |
| FORCE_SENSOR_Z | 0x000C | int16_t | Force sensor Z (N) | 0.1N |
| DOCKING_MODE | 0x000D | uint8_t | Docking mode | - |
| ALIGNMENT_MODE | 0x000E | uint8_t | Alignment mode | - |
| FAULT_STATUS | 0x000F | uint16_t | Fault status | - |
| FAULT_CODE | 0x0010 | uint16_t | Fault code | - |
| UPTIME | 0x0011 | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x0012 | uint32_t | Firmware version | - |

---

## 🤖 **AI MODULE (0x07) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| AI_STATUS | 0x0001 | uint16_t | AI processing status | - |
| VISION_STATUS | 0x0002 | uint16_t | Computer vision status | - |
| ML_STATUS | 0x0003 | uint16_t | Machine learning status | - |
| PATH_STATUS | 0x0004 | uint16_t | Path planning status | - |
| MAINTENANCE_STATUS | 0x0005 | uint16_t | Predictive maintenance status | - |
| ANOMALY_STATUS | 0x0006 | uint16_t | Anomaly detection status | - |
| DECISION_STATUS | 0x0007 | uint16_t | Decision making status | - |
| PROCESSING_TIME | 0x0008 | uint32_t | Processing time (ms) | 1ms |
| MEMORY_USAGE | 0x0009 | uint16_t | Memory usage (%) | 0.1% |
| CPU_USAGE | 0x000A | uint16_t | CPU usage (%) | 0.1% |
| TEMPERATURE | 0x000B | uint16_t | Module temperature (°C) | 0.1°C |
| CONFIDENCE_LEVEL | 0x000C | uint16_t | AI confidence level (%) | 0.1% |
| DETECTION_COUNT | 0x000D | uint32_t | Object detection count | 1 |
| TRACKING_COUNT | 0x000E | uint32_t | Object tracking count | 1 |
| PREDICTION_COUNT | 0x000F | uint32_t | Prediction count | 1 |
| FAULT_STATUS | 0x0010 | uint16_t | Fault status | - |
| FAULT_CODE | 0x0011 | uint16_t | Fault code | - |
| UPTIME | 0x0012 | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x0013 | uint32_t | Firmware version | - |
| MODEL_VERSION | 0x0014 | uint32_t | AI model version | - |
| DATASET_VERSION | 0x0015 | uint32_t | Dataset version | - |
| PERFORMANCE_SCORE | 0x0016 | uint16_t | Performance score | 0.1 |
| ACCURACY_SCORE | 0x0017 | uint16_t | Accuracy score | 0.1 |
| LATENCY_SCORE | 0x0018 | uint16_t | Latency score | 0.1 |

---

## 🔧 **COMMON REGISTER PATTERNS**

### **Status Register (0x0000) - All Modules:**
```c
// Status Register Bit Definitions
#define STATUS_BIT_READY              0x0001  // Module ready
#define STATUS_BIT_ACTIVE             0x0002  // Module active
#define STATUS_BIT_FAULT              0x0004  // Fault detected
#define STATUS_BIT_WARNING            0x0008  // Warning condition
#define STATUS_BIT_COMM_ERROR         0x0010  // Communication error
#define STATUS_BIT_CONFIG_VALID       0x0020  // Configuration valid
#define STATUS_BIT_CALIBRATED         0x0040  // Module calibrated
#define STATUS_BIT_SAFETY_OK          0x0080  // Safety OK
```

### **Fault Register (0x000C/0x000E) - All Modules:**
```c
// Fault Register Bit Definitions
#define FAULT_BIT_COMM_TIMEOUT        0x0001  // Communication timeout
#define FAULT_BIT_HARDWARE_ERROR      0x0002  // Hardware error
#define FAULT_BIT_SOFTWARE_ERROR      0x0004  // Software error
#define FAULT_BIT_TEMPERATURE_HIGH    0x0008  // Temperature too high
#define FAULT_BIT_VOLTAGE_LOW         0x0010  // Voltage too low
#define FAULT_BIT_VOLTAGE_HIGH        0x0020  // Voltage too high
#define FAULT_BIT_CURRENT_HIGH        0x0040  // Current too high
#define FAULT_BIT_SAFETY_VIOLATION    0x0080  // Safety violation
```

### **Version Register (0x000F/0x0011/0x0012/0x0013) - All Modules:**
```c
// Version Register Format
typedef struct {
    uint8_t major;                   // Major version
    uint8_t minor;                   // Minor version
    uint8_t patch;                   // Patch version
    uint8_t build;                   // Build number
} version_t;
```

---

## 📋 **REGISTER ACCESS PATTERNS**

### **Read Operations:**
```c
// Read single register
uint16_t read_register(uint8_t module_addr, uint16_t reg_addr);

// Read multiple registers
int read_registers(uint8_t module_addr, uint16_t reg_addr, uint16_t count, uint16_t *data);

// Read module status
uint16_t read_module_status(uint8_t module_addr);
```

### **Write Operations:**
```c
// Write single register
int write_register(uint8_t module_addr, uint16_t reg_addr, uint16_t value);

// Write multiple registers
int write_registers(uint8_t module_addr, uint16_t reg_addr, uint16_t count, uint16_t *data);

// Write module command
int write_module_command(uint8_t module_addr, uint8_t command, uint16_t *data, uint16_t count);
```

---

## 🚨 **SAFETY CONSIDERATIONS**

### **Critical Registers:**
- **Status Registers:** Always check before operations
- **Fault Registers:** Monitor continuously for safety
- **Safety I/O:** Handle with highest priority
- **Emergency Commands:** Immediate response required

### **Register Validation:**
```c
// Validate register address
bool is_valid_register(uint8_t module_type, uint16_t reg_addr);

// Validate register value
bool is_valid_register_value(uint8_t module_type, uint16_t reg_addr, uint16_t value);

// Check register access permissions
bool can_write_register(uint8_t module_type, uint16_t reg_addr);
```

---

## 📊 **PERFORMANCE REQUIREMENTS**

### **Register Access Timing:**
- **Read Operation:** < 10ms response time
- **Write Operation:** < 5ms response time
- **Status Polling:** 100ms interval
- **Fault Detection:** < 50ms response time

### **Data Integrity:**
- **CRC Check:** All register operations
- **Retry Mechanism:** 3 attempts maximum
- **Timeout:** 100ms per operation
- **Validation:** Range and type checking

---

## 🔄 **CHANGELOG**

### **v1.0 (2025-01-28):**
- ✅ Created comprehensive register map for all modules
- ✅ Added data types and resolutions
- ✅ Defined common register patterns
- ✅ Added safety considerations
- ✅ Added performance requirements
- ✅ Added access patterns and validation

---

**📝 Lưu ý:** Tài liệu này tổng hợp register map từ tất cả các module specification files. Mỗi module có register map chi tiết trong file riêng của nó.
