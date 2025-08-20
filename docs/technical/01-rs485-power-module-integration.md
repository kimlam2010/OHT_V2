# HƯỚNG DẪN KẾT NỐI VÀ ĐIỀU KHIỂN MODULE NGUỒN QUA RS485
## OHT-50 Master Module - Power Module Integration

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-27  
**Team:** FW Team  
**Trạng thái:** Implementation Guide  

---

**Changelog:**
- v1.0 (2025-01-27): Initial Power Module integration guide
- v1.1 (2025-01-27): Removed GPIO_DE pin (not needed for RS485)
- v1.2 (2025-01-27): Added System Registers (0x00F0-0x00FF) for auto-detect compatibility

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với Power Module qua RS485
- Quản lý nguồn điện (pin, charger, power distribution)
- Giám sát trạng thái pin (voltage, current, temperature, SoC)
- Điều khiển sạc pin tự động và bảo vệ an toàn

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │  Power Module   │
│  (Orange Pi 5B) │             │   (Controller)  │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ Power Ctrl  │ │
│ │ Driver      │ │             │ │ Logic       │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ Power Mgr   │ │             │ │ Battery     │ │
│ │ Application │ │             │ │ Management  │ │
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

#### **Power Module:**
- **Protocol:** Modbus RTU
- **Slave Address:** 0x02 (cố định cho Power Module)
- **Response Time:** < 100ms
- **Error Detection:** CRC-16

### **2. Kết nối phần cứng:**

#### **Wiring Diagram:**
```
Orange Pi 5B          RS485 Bus          Power Module
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
// Power Module specific functions
hal_status_t power_module_init(void);
hal_status_t power_module_read_battery_voltage(float *voltage);
hal_status_t power_module_read_battery_current(float *current);
hal_status_t power_module_read_battery_temperature(float *temperature);
hal_status_t power_module_read_battery_soc(uint8_t *soc);
hal_status_t power_module_read_charge_status(power_charge_status_t *status);
hal_status_t power_module_set_charge_current(float current);
hal_status_t power_module_enable_charging(bool enable);
hal_status_t power_module_read_power_outputs(power_output_status_t *outputs);
```

### **2. Power Manager Application (`power_manager.c`)**

#### **Core Functions:**
```c
// Power management functions
power_status_t power_manager_get_status(void);
hal_status_t power_manager_monitor_battery(void);
hal_status_t power_manager_control_charging(void);
hal_status_t power_manager_distribute_power(void);
hal_status_t power_manager_handle_faults(void);
hal_status_t power_manager_save_config(void);
hal_status_t power_manager_load_config(void);
```

---

## 📡 **MODBUS REGISTER MAP**

### **1. Battery Information Registers (0x0000-0x00FF):**

#### **Battery Status (0x0000-0x000F):**
```c
// Battery voltage (0.1V resolution)
#define POWER_REG_BATTERY_VOLTAGE        0x0000  // 0.1V units

// Battery current (0.1A resolution)
#define POWER_REG_BATTERY_CURRENT        0x0001  // 0.1A units

// Battery temperature (0.1°C resolution)
#define POWER_REG_BATTERY_TEMP           0x0002  // 0.1°C units

// State of Charge (0-100%)
#define POWER_REG_BATTERY_SOC            0x0003  // 0-100%

// Battery health (0-100%)
#define POWER_REG_BATTERY_HEALTH         0x0004  // 0-100%

// Cycle count
#define POWER_REG_BATTERY_CYCLES         0x0005  // Number of cycles

// Time to empty (minutes)
#define POWER_REG_TIME_TO_EMPTY          0x0006  // Minutes

// Time to full (minutes)
#define POWER_REG_TIME_TO_FULL           0x0007  // Minutes
```

#### **Charging Control (0x0010-0x001F):**
```c
// Charge status
#define POWER_REG_CHARGE_STATUS          0x0010  // 0=Idle, 1=Charging, 2=Full

// Charge current setting (0.1A resolution)
#define POWER_REG_CHARGE_CURRENT_SET     0x0011  // 0.1A units

// Charge voltage setting (0.1V resolution)
#define POWER_REG_CHARGE_VOLTAGE_SET     0x0012  // 0.1V units

// Charge enable/disable
#define POWER_REG_CHARGE_ENABLE          0x0013  // 0=Disable, 1=Enable

// Charge mode
#define POWER_REG_CHARGE_MODE            0x0014  // 0=CC, 1=CV, 2=Float

// Charge time remaining (minutes)
#define POWER_REG_CHARGE_TIME_REMAINING  0x0015  // Minutes
```

#### **Power Distribution (0x0020-0x002F):**
```c
// 5V output status
#define POWER_REG_5V_STATUS              0x0020  // 0=Off, 1=On, 2=Fault

// 5V output current (0.1A resolution)
#define POWER_REG_5V_CURRENT             0x0021  // 0.1A units

// 12V output status
#define POWER_REG_12V_STATUS             0x0022  // 0=Off, 1=On, 2=Fault

// 12V output current (0.1A resolution)
#define POWER_REG_12V_CURRENT            0x0023  // 0.1A units

// 24V output status
#define POWER_REG_24V_STATUS             0x0024  // 0=Off, 1=On, 2=Fault

// 24V output current (0.1A resolution)
#define POWER_REG_24V_CURRENT            0x0025  // 0.1A units

// 48V output status
#define POWER_REG_48V_STATUS             0x0026  // 0=Off, 1=On, 2=Fault

// 48V output current (0.1A resolution)
#define POWER_REG_48V_CURRENT            0x0027  // 0.1A units
```

#### **Fault Status (0x0030-0x003F):**
```c
// Fault status register
#define POWER_REG_FAULT_STATUS           0x0030  // Bit field

// Overvoltage fault
#define POWER_REG_OVERVOLTAGE_FAULT      0x0031  // 0=OK, 1=Fault

// Overcurrent fault
#define POWER_REG_OVERCURRENT_FAULT      0x0032  // 0=OK, 1=Fault

// Overtemperature fault
#define POWER_REG_OVERTEMP_FAULT         0x0033  // 0=OK, 1=Fault

// Short circuit fault
#define POWER_REG_SHORT_CIRCUIT_FAULT    0x0034  // 0=OK, 1=Fault

// Communication fault
#define POWER_REG_COMM_FAULT             0x0035  // 0=OK, 1=Fault
```

#### **System Registers (0x00F0-0x00FF):**
```c
// Device ID (Modbus slave address)
#define POWER_REG_DEVICE_ID              0x00F0  // 16-bit unsigned

// Firmware version (e.g. 0x0101 = v1.01)
#define POWER_REG_FIRMWARE_VERSION       0x00F1  // 16-bit unsigned

// System status (bit field)
#define POWER_REG_SYSTEM_STATUS          0x00F2  // 16-bit unsigned

// System error (global error code)
#define POWER_REG_SYSTEM_ERROR           0x00F3  // 16-bit unsigned

// Reset error command (write 1 to reset all error flags)
#define POWER_REG_RESET_ERROR_CMD        0x00F4  // 16-bit unsigned

// Config baudrate (1=9600, 2=19200, 3=38400,...)
#define POWER_REG_CONFIG_BAUDRATE        0x00F5  // 16-bit unsigned

// Config parity (0=None, 1=Even, 2=Odd)
#define POWER_REG_CONFIG_PARITY          0x00F6  // 16-bit unsigned

// Module type (0x0002 = Power Module)
#define POWER_REG_MODULE_TYPE            0x00F7  // 16-bit unsigned

// Module name (low word)
#define POWER_REG_MODULE_NAME_LOW        0x00F8  // 16-bit unsigned

// Module name (high word)
#define POWER_REG_MODULE_NAME_HIGH       0x00F9  // 16-bit unsigned

// Hardware version
#define POWER_REG_HARDWARE_VERSION       0x00FA  // 16-bit unsigned

// Serial number (low word)
#define POWER_REG_SERIAL_NUMBER_LOW      0x00FB  // 16-bit unsigned

// Serial number (high word)
#define POWER_REG_SERIAL_NUMBER_HIGH     0x00FC  // 16-bit unsigned

// Build date (low word)
#define POWER_REG_BUILD_DATE_LOW         0x00FD  // 16-bit unsigned

// Build date (high word)
#define POWER_REG_BUILD_DATE_HIGH        0x00FE  // 16-bit unsigned

// Register checksum
#define POWER_REG_CHECKSUM               0x00FF  // 16-bit unsigned
```

---

## 🔧 **CONFIGURATION**

### **1. Power Module Configuration:**
```json
{
  "power_module": {
    "address": "0x02",
    "name": "Power Management Module",
    "type": "power",
    "version": "1.0",
    "enabled": true,
    "battery_config": {
      "capacity": 100,
      "nominal_voltage": 48.0,
      "max_voltage": 54.0,
      "min_voltage": 40.0,
      "max_current": 50.0,
      "charge_current": 10.0,
      "charge_voltage": 54.0
    },
    "output_config": {
      "5v_enabled": true,
      "5v_current_limit": 5.0,
      "12v_enabled": true,
      "12v_current_limit": 3.0,
      "24v_enabled": true,
      "24v_current_limit": 2.0,
      "48v_enabled": true,
      "48v_current_limit": 1.0
    },
    "safety_config": {
      "overvoltage_threshold": 55.0,
      "overcurrent_threshold": 55.0,
      "overtemp_threshold": 85.0,
      "undervoltage_threshold": 38.0
    },
    "system_config": {
      "baudrate": 3,
      "parity": 0,
      "device_id": 2
    }
  }
}
```

### **2. HAL Configuration:**
```c
// Power module HAL configuration
typedef struct {
    uint8_t address;
    uint32_t timeout_ms;
    uint8_t retry_count;
    float battery_capacity;
    float nominal_voltage;
    float max_voltage;
    float min_voltage;
    float max_current;
    float charge_current;
    float charge_voltage;
    bool outputs_enabled[4];
    float current_limits[4];
    float safety_thresholds[4];
} power_module_config_t;
```

---

## 🧪 **TESTING**

### **1. Unit Tests:**
```c
// Power module unit tests
void test_power_module_init(void);
void test_power_module_read_battery_voltage(void);
void test_power_module_read_battery_current(void);
void test_power_module_read_battery_temperature(void);
void test_power_module_read_battery_soc(void);
void test_power_module_control_charging(void);
void test_power_module_power_distribution(void);
void test_power_module_fault_handling(void);
```

### **2. Integration Tests:**
```c
// Power module integration tests
void test_power_module_full_cycle(void);
void test_power_module_charging_cycle(void);
void test_power_module_fault_recovery(void);
void test_power_module_power_distribution(void);
void test_power_module_communication_fault(void);
```

### **3. Performance Tests:**
```c
// Power module performance tests
void test_power_module_response_time(void);
void test_power_module_accuracy(void);
void test_power_module_stability(void);
void test_power_module_efficiency(void);
```

---

## 📊 **MONITORING VÀ DIAGNOSTICS**

### **1. Real-time Monitoring:**
```c
// Power module monitoring data
typedef struct {
    float battery_voltage;
    float battery_current;
    float battery_temperature;
    uint8_t battery_soc;
    uint8_t battery_health;
    uint16_t battery_cycles;
    uint16_t time_to_empty;
    uint16_t time_to_full;
    power_charge_status_t charge_status;
    power_output_status_t outputs[4];
    power_fault_status_t faults;
    uint32_t uptime;
    uint32_t communication_errors;
} power_module_status_t;
```