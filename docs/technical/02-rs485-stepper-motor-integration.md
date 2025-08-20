# HƯỚNG DẪN KẾT NỐI VÀ ĐIỀU KHIỂN MODULE ĐỘNG CƠ BƯỚC QUA RS485
## OHT-50 Master Module - Stepper Motor Integration

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-27  
**Team:** FW Team  
**Trạng thái:** Implementation Guide  

---

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với Stepper Motor Module qua RS485
- Điều khiển động cơ bước chính xác (position, speed, acceleration)
- Giám sát trạng thái động cơ (position, speed, torque, temperature)
- Đảm bảo định vị chính xác ±0.5mm

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │ Stepper Motor   │
│  (Orange Pi 5B) │             │    Module       │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ Stepper     │ │
│ │ Driver      │ │             │ │ Controller  │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ Stepper Mgr │ │             │ │ Stepper     │ │
│ │ Application │ │             │ │ Motor       │ │
│ └─────────────┘ │             │ └─────────────┘ │
└─────────────────┘             └─────────────────┘
```

---

## 🔧 **PHẦN CỨNG VÀ KẾT NỐI**

### **1. Thông số kỹ thuật RS485:**

#### **Master Module (Orange Pi 5B):**
- **UART Interface:** UART1 (`/dev/ttyS1`)
- **RS485 Transceiver:** MAX485 hoặc tương đương
- **Baud Rate:** 115200 bps (có thể điều chỉnh)
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Flow Control:** None

#### **Stepper Motor Module:**
- **Protocol:** Modbus RTU
- **Slave Address:** 0x03 (cố định cho Stepper Module)
- **Response Time:** < 100ms
- **Error Detection:** CRC-16

### **2. Kết nối phần cứng:**

#### **Wiring Diagram:**
```
Orange Pi 5B          RS485 Bus          Stepper Module
┌─────────────┐      ┌─────────┐        ┌─────────────┐
│ UART1_TX    │──────│   A+    │────────│   A+        │
│ (GPIO)      │      │         │        │             │
│             │      │         │        │             │
│ UART1_RX    │──────│   B-    │────────│   B-        │
│ (GPIO)      │      │         │        │             │
│             │      │         │        │             │
│             │──────│         │────────│             │
│ (Control)   │      │         │        │             │
│             │      │         │        │             │
│ GND         │──────│   GND   │────────│   GND       │
└─────────────┘      └─────────┘        └─────────────┘
```

### **3. Cấu hình GPIO:**

#### **Pin Assignment:**
```bash
# UART1 Configuration
UART1_TX: GPIO1_A0 (Pin 8)
UART1_RX: GPIO1_A1 (Pin 10)


# udev rule để tạo symlink
echo 'KERNEL=="ttyS1", SYMLINK+="ttyOHT485"' > /etc/udev/rules.d/99-oht-rs485.rules
```

---

## 💻 **IMPLEMENTATION SOFTWARE**

### **1. HAL RS485 Driver (`hal_rs485.c`)**

#### **Core Functions:**
```c
// Stepper Motor Module specific functions
hal_status_t stepper_module_init(void);
hal_status_t stepper_module_set_position(int32_t position);
hal_status_t stepper_module_get_position(int32_t *position);
hal_status_t stepper_module_set_speed(uint16_t speed);
hal_status_t stepper_module_get_speed(uint16_t *speed);
hal_status_t stepper_module_set_acceleration(uint16_t accel);
hal_status_t stepper_module_get_acceleration(uint16_t *accel);
hal_status_t stepper_module_enable(bool enable);
hal_status_t stepper_module_is_enabled(bool *enabled);
hal_status_t stepper_module_home(void);
hal_status_t stepper_module_stop(void);
hal_status_t stepper_module_emergency_stop(void);
hal_status_t stepper_module_read_status(stepper_status_t *status);
```

### **2. Stepper Manager Application (`stepper_manager.c`)**

#### **Core Functions:**
```c
// Stepper management functions
stepper_status_t stepper_manager_get_status(void);
hal_status_t stepper_manager_move_to_position(int32_t position);
hal_status_t stepper_manager_move_relative(int32_t distance);
hal_status_t stepper_manager_set_speed_profile(speed_profile_t *profile);
hal_status_t stepper_manager_calibrate(void);
hal_status_t stepper_manager_handle_faults(void);
hal_status_t stepper_manager_save_config(void);
hal_status_t stepper_manager_load_config(void);
```

---

## 📡 **MODBUS REGISTER MAP**

### **1. Control Registers (0x0000-0x00FF):**

#### **Position Control (0x0000-0x000F):**
```c
// Current position (steps)
#define STEPPER_REG_CURRENT_POSITION     0x0000  // 32-bit signed

// Target position (steps)
#define STEPPER_REG_TARGET_POSITION      0x0002  // 32-bit signed

// Position error (steps)
#define STEPPER_REG_POSITION_ERROR       0x0004  // 32-bit signed

// Position tolerance (steps)
#define STEPPER_REG_POSITION_TOLERANCE   0x0006  // 16-bit unsigned

// Home position (steps)
#define STEPPER_REG_HOME_POSITION        0x0007  // 32-bit signed

// Software limits (min/max)
#define STEPPER_REG_SOFT_LIMIT_MIN       0x0009  // 32-bit signed
#define STEPPER_REG_SOFT_LIMIT_MAX       0x000B  // 32-bit signed
```

#### **Speed Control (0x0010-0x001F):**
```c
// Current speed (steps/s)
#define STEPPER_REG_CURRENT_SPEED        0x0010  // 16-bit unsigned

// Target speed (steps/s)
#define STEPPER_REG_TARGET_SPEED         0x0011  // 16-bit unsigned

// Maximum speed (steps/s)
#define STEPPER_REG_MAX_SPEED            0x0012  // 16-bit unsigned

// Minimum speed (steps/s)
#define STEPPER_REG_MIN_SPEED            0x0013  // 16-bit unsigned

// Acceleration (steps/s²)
#define STEPPER_REG_ACCELERATION         0x0014  // 16-bit unsigned

// Deceleration (steps/s²)
#define STEPPER_REG_DECELERATION         0x0015  // 16-bit unsigned

// Jerk (steps/s³)
#define STEPPER_REG_JERK                 0x0016  // 16-bit unsigned
```

#### **Motor Control (0x0020-0x002F):**
```c
// Motor enable/disable
#define STEPPER_REG_MOTOR_ENABLE         0x0020  // 0=Disable, 1=Enable

// Motor direction
#define STEPPER_REG_MOTOR_DIRECTION      0x0021  // 0=Forward, 1=Reverse

// Microstepping mode
#define STEPPER_REG_MICROSTEP_MODE       0x0022  // 1,2,4,8,16,32,64,128,256

// Current limit (mA)
#define STEPPER_REG_CURRENT_LIMIT        0x0023  // 16-bit unsigned

// Hold current (mA)
#define STEPPER_REG_HOLD_CURRENT         0x0024  // 16-bit unsigned

// Sleep mode
#define STEPPER_REG_SLEEP_MODE           0x0025  // 0=Normal, 1=Sleep

// Stall detection
#define STEPPER_REG_STALL_DETECTION      0x0026  // 0=Disable, 1=Enable
```

#### **Status Information (0x0030-0x003F):**
```c
// Motor status
#define STEPPER_REG_MOTOR_STATUS         0x0030  // Bit field

// Position reached
#define STEPPER_REG_POSITION_REACHED     0x0031  // 0=No, 1=Yes

// Motor moving
#define STEPPER_REG_MOTOR_MOVING         0x0032  // 0=No, 1=Yes

// Motor stalled
#define STEPPER_REG_MOTOR_STALLED        0x0033  // 0=No, 1=Yes

// Motor temperature (°C)
#define STEPPER_REG_MOTOR_TEMPERATURE    0x0034  // 16-bit signed

// Motor current (mA)
#define STEPPER_REG_MOTOR_CURRENT        0x0035  // 16-bit unsigned

// Motor voltage (0.1V)
#define STEPPER_REG_MOTOR_VOLTAGE        0x0036  // 16-bit unsigned

// Encoder position
#define STEPPER_REG_ENCODER_POSITION     0x0037  // 32-bit signed
```

#### **Configuration (0x0040-0x004F):**
```c
// Steps per revolution
#define STEPPER_REG_STEPS_PER_REV        0x0040  // 16-bit unsigned

// Encoder resolution
#define STEPPER_REG_ENCODER_RESOLUTION   0x0041  // 16-bit unsigned

// Gear ratio
#define STEPPER_REG_GEAR_RATIO           0x0042  // 16-bit unsigned

// Lead screw pitch (0.1mm)
#define STEPPER_REG_LEAD_SCREW_PITCH     0x0043  // 16-bit unsigned

// Units per revolution
#define STEPPER_REG_UNITS_PER_REV        0x0044  // 32-bit float

// Position units
#define STEPPER_REG_POSITION_UNITS       0x0046  // 0=Steps, 1=mm, 2=inches
```

#### **Fault Status (0x0050-0x005F):**
```c
// Fault status register
#define STEPPER_REG_FAULT_STATUS         0x0050  // Bit field

// Overcurrent fault
#define STEPPER_REG_OVERCURRENT_FAULT    0x0051  // 0=OK, 1=Fault

// Overtemperature fault
#define STEPPER_REG_OVERTEMP_FAULT       0x0052  // 0=OK, 1=Fault

// Stall fault
#define STEPPER_REG_STALL_FAULT          0x0053  // 0=OK, 1=Fault

// Position error fault
#define STEPPER_REG_POSITION_ERROR_FAULT 0x0054  // 0=OK, 1=Fault

// Communication fault
#define STEPPER_REG_COMM_FAULT           0x0055  // 0=OK, 1=Fault
```

#### **System Registers (0x00F0-0x00FF):**
```c
// Device ID (Modbus slave address)
#define STEPPER_REG_DEVICE_ID            0x00F0  // 16-bit unsigned

// Firmware version (e.g. 0x0101 = v1.01)
#define STEPPER_REG_FIRMWARE_VERSION     0x00F1  // 16-bit unsigned

// System status (bit field)
#define STEPPER_REG_SYSTEM_STATUS        0x00F2  // 16-bit unsigned

// System error (global error code)
#define STEPPER_REG_SYSTEM_ERROR         0x00F3  // 16-bit unsigned

// Reset error command (write 1 to reset all error flags)
#define STEPPER_REG_RESET_ERROR_CMD      0x00F4  // 16-bit unsigned

// Config baudrate (1=9600, 2=19200, 3=38400,...)
#define STEPPER_REG_CONFIG_BAUDRATE      0x00F5  // 16-bit unsigned

// Config parity (0=None, 1=Even, 2=Odd)
#define STEPPER_REG_CONFIG_PARITY        0x00F6  // 16-bit unsigned

// Module type (0x0003 = Stepper Motor)
#define STEPPER_REG_MODULE_TYPE          0x00F7  // 16-bit unsigned

// Module name (low word)
#define STEPPER_REG_MODULE_NAME_LOW      0x00F8  // 16-bit unsigned

// Module name (high word)
#define STEPPER_REG_MODULE_NAME_HIGH     0x00F9  // 16-bit unsigned

// Hardware version
#define STEPPER_REG_HARDWARE_VERSION     0x00FA  // 16-bit unsigned

// Serial number (low word)
#define STEPPER_REG_SERIAL_NUMBER_LOW    0x00FB  // 16-bit unsigned

// Serial number (high word)
#define STEPPER_REG_SERIAL_NUMBER_HIGH   0x00FC  // 16-bit unsigned

// Build date (low word)
#define STEPPER_REG_BUILD_DATE_LOW       0x00FD  // 16-bit unsigned

// Build date (high word)
#define STEPPER_REG_BUILD_DATE_HIGH      0x00FE  // 16-bit unsigned

// Register checksum
#define STEPPER_REG_CHECKSUM             0x00FF  // 16-bit unsigned
```

---

## 🔧 **CONFIGURATION**

### **1. Stepper Module Configuration:**
```json
{
  "stepper_module": {
    "address": "0x03",
    "name": "Stepper Motor Module",
    "type": "stepper",
    "version": "1.0",
    "enabled": true,
    "motor_config": {
      "steps_per_revolution": 200,
      "microstepping": 16,
      "gear_ratio": 1,
      "lead_screw_pitch": 8.0,
      "units_per_revolution": 8.0,
      "position_units": "mm"
    },
    "speed_config": {
      "max_speed": 4000,
      "min_speed": 100,
      "default_speed": 2000,
      "acceleration": 1000,
      "deceleration": 1000,
      "jerk": 100
    },
    "current_config": {
      "current_limit": 2000,
      "hold_current": 500,
      "sleep_mode": false,
      "stall_detection": true
    },
    "position_config": {
      "position_tolerance": 10,
      "soft_limit_min": -10000,
      "soft_limit_max": 10000,
      "home_position": 0,
      "home_speed": 500
    },
    "safety_config": {
      "enabled": true,
      "stall_detection": true,
      "overcurrent_protection": true,
      "overtemperature_protection": true,
      "position_error_protection": true
    },
    "system_config": {
      "baudrate": 3,
      "parity": 0,
      "device_id": 3
    }
  }
}
```

### **2. HAL Configuration:**
```c
// Stepper module HAL configuration
typedef struct {
    uint8_t address;
    uint32_t timeout_ms;
    uint8_t retry_count;
    uint16_t steps_per_revolution;
    uint16_t microstepping;
    uint16_t gear_ratio;
    float lead_screw_pitch;
    float units_per_revolution;
    uint8_t position_units;
    uint16_t max_speed;
    uint16_t min_speed;
    uint16_t default_speed;
    uint16_t acceleration;
    uint16_t deceleration;
    uint16_t jerk;
    uint16_t current_limit;
    uint16_t hold_current;
    bool sleep_mode;
    bool stall_detection;
    int32_t position_tolerance;
    int32_t soft_limit_min;
    int32_t soft_limit_max;
    int32_t home_position;
    uint16_t home_speed;
    uint16_t safety_thresholds[4];
} stepper_module_config_t;
```

---

## 🧪 **TESTING**

### **1. Unit Tests:**
```c
// Stepper module unit tests
void test_stepper_module_init(void);
void test_stepper_module_set_position(void);
void test_stepper_module_get_position(void);
void test_stepper_module_set_speed(void);
void test_stepper_module_get_speed(void);
void test_stepper_module_set_acceleration(void);
void test_stepper_module_enable_disable(void);
void test_stepper_module_home(void);
void test_stepper_module_stop(void);
void test_stepper_module_emergency_stop(void);
void test_stepper_module_read_status(void);
void test_stepper_module_fault_handling(void);
```

### **2. Integration Tests:**
```c
// Stepper module integration tests
void test_stepper_module_position_control(void);
void test_stepper_module_speed_control(void);
void test_stepper_module_acceleration_control(void);
void test_stepper_module_homing_cycle(void);
void test_stepper_module_limits_handling(void);
void test_stepper_module_fault_recovery(void);
void test_stepper_module_communication_fault(void);
```

### **3. Performance Tests:**
```c
// Stepper module performance tests
void test_stepper_module_response_time(void);
void test_stepper_module_position_accuracy(void);
void test_stepper_module_speed_accuracy(void);
void test_stepper_module_repeatability(void);
void test_stepper_module_stability(void);
void test_stepper_module_efficiency(void);
```

---

## 📊 **MONITORING VÀ DIAGNOSTICS**

### **1. Real-time Monitoring:**
```c
// Stepper module monitoring data
typedef struct {
    int32_t current_position;
    int32_t target_position;
    int32_t position_error;
    uint16_t current_speed;
    uint16_t target_speed;
    uint16_t acceleration;
    uint16_t deceleration;
    bool motor_enabled;
    bool motor_moving;
    bool position_reached;
    bool motor_stalled;
    int16_t motor_temperature;
    uint16_t motor_current;
    uint16_t motor_voltage;
    int32_t encoder_position;
    stepper_fault_status_t faults;
    uint32_t uptime;
    uint32_t communication_errors;
} stepper_module_status_t;
```

### **2. Diagnostic Functions:**
```c
// Stepper module diagnostics
hal_status_t stepper_module_self_test(void);
hal_status_t stepper_module_calibrate_position(void);
hal_status_t stepper_module_calibrate_current(void);
hal_status_t stepper_module_reset_faults(void);
hal_status_t stepper_module_get_diagnostics(stepper_diagnostics_t *diag);
hal_status_t stepper_module_export_logs(void);
```

---

## 🚨 **SAFETY VÀ FAULT HANDLING**

### **1. Safety Features:**
- **Overcurrent Protection:** Tự động ngắt khi current vượt ngưỡng
- **Overtemperature Protection:** Tự động ngắt khi nhiệt độ quá cao
- **Stall Detection:** Phát hiện động cơ bị kẹt
- **Position Error Protection:** Bảo vệ lỗi định vị
- **Soft Limits:** Giới hạn vị trí mềm
- **Emergency Stop:** Dừng khẩn cấp

### **2. Fault Handling:**
```c
// Stepper module fault handling
typedef enum {
    STEPPER_FAULT_NONE = 0,
    STEPPER_FAULT_OVERCURRENT,
    STEPPER_FAULT_OVERTEMP,
    STEPPER_FAULT_STALL,
    STEPPER_FAULT_POSITION_ERROR,
    STEPPER_FAULT_COMMUNICATION,
    STEPPER_FAULT_SOFT_LIMIT,
    STEPPER_FAULT_HARD_LIMIT
} stepper_fault_type_t;

hal_status_t stepper_module_handle_fault(stepper_fault_type_t fault);
hal_status_t stepper_module_clear_fault(stepper_fault_type_t fault);
hal_status_t stepper_module_get_fault_status(stepper_fault_status_t *status);
```

---

## 📈 **PERFORMANCE METRICS**

### **1. Key Performance Indicators:**
- **Position Accuracy:** ±0.5mm
- **Speed Range:** 0-4000 steps/s
- **Acceleration:** 0-10000 steps/s²
- **Response Time:** < 100ms for position commands
- **Repeatability:** ±0.1mm
- **Reliability:** > 99.9% uptime

### **2. Monitoring Metrics:**
- **Position:** Real-time position monitoring
- **Speed:** Real-time speed monitoring
- **Current:** Motor current monitoring
- **Temperature:** Motor temperature monitoring
- **Fault Detection:** Real-time fault monitoring
- **Communication:** RS485 communication status

---

## 🔄 **MAINTENANCE**

### **1. Regular Maintenance:**
- **Daily:** Check motor status và position accuracy
- **Weekly:** Verify speed và acceleration settings
- **Monthly:** Calibrate position sensors
- **Quarterly:** Full system diagnostics
- **Annually:** Motor performance assessment

### **2. Troubleshooting:**
- **Communication Issues:** Check RS485 wiring và termination
- **Position Issues:** Check encoder connections và calibration
- **Speed Issues:** Verify motor parameters và settings
- **Fault Issues:** Review fault logs và clear faults
- **Accuracy Issues:** Check mechanical alignment và backlash

---

**Changelog:**
- v1.0 (2025-01-27): Initial Stepper Motor Module integration guide
- v1.1 (2025-01-27): Removed GPIO_DE pin (not needed for RS485)
- v1.2 (2025-01-27): Added System Registers (0x00F0-0x00FF) for auto-detect compatibility
