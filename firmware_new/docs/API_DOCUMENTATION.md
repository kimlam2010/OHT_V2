# OHT-50 Firmware API Documentation

**Phiên bản:** 3.0.0  
**Ngày cập nhật:** 2025-01-28  
**Team:** FW Team  
**Trạng thái:** Complete

---

## 📋 **TỔNG QUAN API**

### **Module Handlers API**
- **Power Module Handler:** Battery management, charging control
- **Safety Module Handler:** E-Stop, safety zones, proximity detection
- **Travel Motor Module Handler:** Differential drive, speed control
- **Dock Module Handler:** IMU integration, navigation

### **HAL Layer API**
- **Communication:** RS485, Modbus RTU
- **GPIO:** Digital I/O control
- **Safety:** E-Stop handling
- **Storage:** Configuration persistence

---

## 🔋 **POWER MODULE HANDLER API**

### **Initialization**
```c
hal_status_t power_module_init(power_module_handler_t *handler, 
                              const power_module_config_t *config);
```
**Mô tả:** Khởi tạo power module handler  
**Parameters:**
- `handler`: Pointer to power module handler
- `config`: Power module configuration

**Returns:** `HAL_STATUS_OK` (0) on success, error code on failure

### **Configuration Structure**
```c
typedef struct {
    uint8_t address;                // Module address (0x02)
    uint32_t update_rate_hz;        // Update rate in Hz
    uint16_t voltage_threshold_mv;  // Voltage threshold (mV)
    uint16_t current_threshold_ma;  // Current threshold (mA)
    bool enable_charging;           // Enable charging
    bool enable_discharging;        // Enable discharging
} power_module_config_t;
```

### **Data Reading Functions**
```c
hal_status_t power_module_read_battery_voltage(power_module_handler_t *handler, 
                                              uint16_t *voltage_mv);
hal_status_t power_module_read_battery_current(power_module_handler_t *handler, 
                                              int16_t *current_ma);
hal_status_t power_module_read_battery_soc(power_module_handler_t *handler, 
                                          uint8_t *soc_percent);
hal_status_t power_module_read_battery_temp(power_module_handler_t *handler, 
                                           int16_t *temp_celsius);
```

### **Control Functions**
```c
hal_status_t power_module_enable_charging(power_module_handler_t *handler, bool enable);
hal_status_t power_module_enable_discharging(power_module_handler_t *handler, bool enable);
hal_status_t power_module_set_charge_current(power_module_handler_t *handler, uint16_t current_ma);
hal_status_t power_module_set_charge_voltage(power_module_handler_t *handler, uint16_t voltage_mv);
```

---

## 🛡️ **SAFETY MODULE HANDLER API**

### **Initialization**
```c
hal_status_t safety_module_init(safety_module_handler_t *handler, 
                               const safety_module_config_t *config);
```
**Mô tả:** Khởi tạo safety module handler  
**Parameters:**
- `handler`: Pointer to safety module handler
- `config`: Safety module configuration

**Returns:** `HAL_STATUS_OK` (0) on success, error code on failure

### **Configuration Structure**
```c
typedef struct {
    uint8_t address;                // Module address (0x03)
    uint32_t update_rate_hz;        // Update rate in Hz
    safety_thresholds_t thresholds; // Safety thresholds
    bool enable_auto_stop;          // Enable automatic stop
    bool enable_relay_control;      // Enable relay control
} safety_module_config_t;
```

### **Sensor Reading Functions**
```c
hal_status_t safety_module_read_sensors(safety_module_handler_t *handler, 
                                       safety_sensor_data_t *sensor_data);
hal_status_t safety_module_get_analog_sensor(safety_module_handler_t *handler, 
                                            uint8_t sensor_number, uint16_t *distance);
hal_status_t safety_module_get_digital_sensors(safety_module_handler_t *handler, 
                                              uint8_t *sensors);
hal_status_t safety_module_is_proximity_detected(safety_module_handler_t *handler, 
                                                uint8_t sensor_number, bool *detected);
```

### **Safety Control Functions**
```c
hal_status_t safety_module_check_safety(safety_module_handler_t *handler, bool *safe);
hal_status_t safety_module_get_violation_info(safety_module_handler_t *handler, 
                                             bool *violation, uint8_t *type);
hal_status_t safety_module_clear_violation(safety_module_handler_t *handler);
```

### **Relay Control Functions**
```c
hal_status_t safety_module_set_relay(safety_module_handler_t *handler, 
                                    uint8_t relay_number, bool state);
hal_status_t safety_module_get_relay(safety_module_handler_t *handler, 
                                    uint8_t relay_number, bool *state);
hal_status_t safety_module_set_relays(safety_module_handler_t *handler, uint8_t relays);
hal_status_t safety_module_get_relays(safety_module_handler_t *handler, uint8_t *relays);
```

---

## 🚗 **TRAVEL MOTOR MODULE HANDLER API**

### **Initialization**
```c
hal_status_t travel_motor_module_init(travel_motor_module_handler_t *handler, 
                                     const travel_motor_module_config_t *config);
```
**Mô tả:** Khởi tạo travel motor module handler  
**Parameters:**
- `handler`: Pointer to travel motor module handler
- `config`: Travel motor module configuration

**Returns:** `HAL_STATUS_OK` (0) on success, error code on failure

### **Configuration Structure**
```c
typedef struct {
    uint8_t address;                    // Module address (0x04)
    uint32_t update_rate_hz;            // Update rate in Hz
    uint16_t max_speed_mmps;            // Maximum speed (mm/s)
    uint16_t max_acceleration_mmps2;    // Maximum acceleration (mm/s²)
    bool enable_current_protection;     // Enable current protection
    uint16_t current_limit_ma;          // Current limit (mA)
    bool enable_brake_control;          // Enable brake control
} travel_motor_module_config_t;
```

### **Motor Control Functions**
```c
hal_status_t travel_motor_module_set_speed(travel_motor_module_handler_t *handler, 
                                          int16_t speed_mmps);
hal_status_t travel_motor_module_get_speed(travel_motor_module_handler_t *handler, 
                                          int16_t *speed_mmps);
hal_status_t travel_motor_module_set_position(travel_motor_module_handler_t *handler, 
                                             int32_t position_mm);
hal_status_t travel_motor_module_get_position(travel_motor_module_handler_t *handler, 
                                             int32_t *position_mm);
```

### **Differential Drive Functions**
```c
hal_status_t travel_motor_module_set_differential_speed(travel_motor_module_handler_t *handler, 
                                                       int16_t left_speed, int16_t right_speed);
hal_status_t travel_motor_module_get_differential_speed(travel_motor_module_handler_t *handler, 
                                                       int16_t *left_speed, int16_t *right_speed);
```

### **Brake Control Functions**
```c
hal_status_t travel_motor_module_set_brake(travel_motor_module_handler_t *handler, bool engaged);
hal_status_t travel_motor_module_get_brake(travel_motor_module_handler_t *handler, bool *engaged);
hal_status_t travel_motor_module_emergency_stop(travel_motor_module_handler_t *handler);
```

---

## 🎯 **DOCK MODULE HANDLER API**

### **Initialization**
```c
hal_status_t dock_module_init(dock_module_handler_t *handler, 
                             const dock_module_config_t *config);
```
**Mô tả:** Khởi tạo dock module handler  
**Parameters:**
- `handler`: Pointer to dock module handler
- `config`: Dock module configuration

**Returns:** `HAL_STATUS_OK` (0) on success, error code on failure

### **Configuration Structure**
```c
typedef struct {
    uint8_t address;                // Module address (0x05)
    uint32_t update_rate_hz;        // Update rate in Hz
    bool enable_imu;                // Enable IMU
    bool enable_magnetic_sensors;   // Enable magnetic sensors
    bool enable_limit_switches;     // Enable limit switches
    float position_threshold_mm;    // Position threshold (mm)
    float orientation_threshold_deg; // Orientation threshold (degrees)
} dock_module_config_t;
```

### **IMU Data Functions**
```c
hal_status_t dock_module_read_imu_data(dock_module_handler_t *handler, imu_data_t *imu_data);
hal_status_t dock_module_get_accelerometer(dock_module_handler_t *handler, 
                                          int16_t *x, int16_t *y, int16_t *z);
hal_status_t dock_module_get_gyroscope(dock_module_handler_t *handler, 
                                      int16_t *x, int16_t *y, int16_t *z);
hal_status_t dock_module_get_magnetometer(dock_module_handler_t *handler, 
                                         int16_t *x, int16_t *y, int16_t *z);
```

### **Navigation Functions**
```c
hal_status_t dock_module_calculate_navigation(dock_module_handler_t *handler, 
                                             navigation_data_t *nav_data);
hal_status_t dock_module_get_orientation(dock_module_handler_t *handler, 
                                        float *roll, float *pitch, float *yaw);
hal_status_t dock_module_get_heading(dock_module_handler_t *handler, float *heading);
```

### **Docking Control Functions**
```c
hal_status_t dock_module_start_docking_sequence(dock_module_handler_t *handler);
hal_status_t dock_module_stop_docking_sequence(dock_module_handler_t *handler);
hal_status_t dock_module_get_docking_status(dock_module_handler_t *handler, 
                                           uint8_t *status);
```

---

## 🔧 **HAL LAYER API**

### **Communication HAL**
```c
// RS485 Communication
hal_status_t hal_rs485_init(const rs485_config_t *config);
hal_status_t hal_rs485_send(const uint8_t *data, size_t length);
hal_status_t hal_rs485_receive(uint8_t *data, size_t max_length, size_t *actual_length);

// Modbus RTU
hal_status_t hal_modbus_rtu_init(const modbus_rtu_config_t *config);
hal_status_t hal_modbus_rtu_read_holding_registers(uint8_t slave_addr, 
                                                  uint16_t start_addr, 
                                                  uint16_t count, 
                                                  uint16_t *registers);
hal_status_t hal_modbus_rtu_write_holding_registers(uint8_t slave_addr, 
                                                   uint16_t start_addr, 
                                                   uint16_t count, 
                                                   const uint16_t *registers);
```

### **GPIO HAL**
```c
// GPIO Control
hal_status_t hal_gpio_init(const gpio_config_t *config);
hal_status_t hal_gpio_set_pin(uint8_t pin, bool state);
hal_status_t hal_gpio_get_pin(uint8_t pin, bool *state);
hal_status_t hal_gpio_set_direction(uint8_t pin, gpio_direction_t direction);
```

### **Safety HAL**
```c
// E-Stop Control
hal_status_t hal_estop_init(const estop_config_t *config);
hal_status_t hal_estop_is_triggered(bool *triggered);
hal_status_t hal_estop_get_channel1_status(bool *status);
hal_status_t hal_estop_get_channel2_status(bool *status);
```

### **Storage HAL**
```c
// Configuration Persistence
hal_status_t hal_config_save(const char *key, const void *data, size_t size);
hal_status_t hal_config_load(const char *key, void *data, size_t size);
hal_status_t hal_config_delete(const char *key);
hal_status_t hal_config_exists(const char *key, bool *exists);
```

---

## 📊 **ERROR CODES**

### **HAL Status Codes**
```c
#define HAL_STATUS_OK                   0   // Success
#define HAL_STATUS_ERROR               -1   // General error
#define HAL_STATUS_INVALID_PARAMETER   -2   // Invalid parameter
#define HAL_STATUS_NOT_INITIALIZED     -3   // Not initialized
#define HAL_STATUS_TIMEOUT             -4   // Operation timeout
#define HAL_STATUS_BUSY                -5   // Device busy
#define HAL_STATUS_NOT_SUPPORTED       -6   // Operation not supported
#define HAL_STATUS_NO_MEMORY           -7   // Out of memory
#define HAL_STATUS_IO_ERROR            -8   // I/O error
```

### **Module-Specific Error Codes**
```c
// Power Module Errors
#define POWER_ERROR_OVERVOLTAGE        0x01
#define POWER_ERROR_UNDERVOLTAGE       0x02
#define POWER_ERROR_OVERCURRENT        0x03
#define POWER_ERROR_OVERTEMPERATURE    0x04

// Safety Module Errors
#define SAFETY_ERROR_ESTOP_TRIGGERED   0x01
#define SAFETY_ERROR_ZONE_VIOLATION    0x02
#define SAFETY_ERROR_SENSOR_FAILURE    0x03
#define SAFETY_ERROR_RELAY_FAILURE     0x04

// Motor Module Errors
#define MOTOR_ERROR_OVERCURRENT        0x01
#define MOTOR_ERROR_OVERTEMPERATURE    0x02
#define MOTOR_ERROR_ENCODER_FAILURE    0x03
#define MOTOR_ERROR_DRIVER_FAULT       0x04

// Dock Module Errors
#define DOCK_ERROR_IMU_FAILURE         0x01
#define DOCK_ERROR_MAGNETIC_FAILURE    0x02
#define DOCK_ERROR_LIMIT_SWITCH_FAILURE 0x03
#define DOCK_ERROR_NAVIGATION_FAILURE  0x04
```

---

## 📋 **USAGE EXAMPLES**

### **Power Module Example**
```c
#include "power_module_handler.h"

power_module_handler_t power_handler;
power_module_config_t power_config = {
    .address = 0x02,
    .update_rate_hz = 10,
    .voltage_threshold_mv = 42000,  // 42V
    .current_threshold_ma = 5000,   // 5A
    .enable_charging = true,
    .enable_discharging = true
};

// Initialize power module
hal_status_t status = power_module_init(&power_handler, &power_config);
if (status == HAL_STATUS_OK) {
    // Read battery voltage
    uint16_t voltage_mv;
    status = power_module_read_battery_voltage(&power_handler, &voltage_mv);
    if (status == HAL_STATUS_OK) {
        printf("Battery voltage: %d mV\n", voltage_mv);
    }
}
```

### **Safety Module Example**
```c
#include "safety_module_handler.h"

safety_module_handler_t safety_handler;
safety_module_config_t safety_config = {
    .address = 0x03,
    .update_rate_hz = 50,
    .enable_auto_stop = true,
    .enable_relay_control = true
};

// Initialize safety module
hal_status_t status = safety_module_init(&safety_handler, &safety_config);
if (status == HAL_STATUS_OK) {
    // Check safety status
    bool safe;
    status = safety_module_check_safety(&safety_handler, &safe);
    if (status == HAL_STATUS_OK && !safe) {
        printf("Safety violation detected!\n");
    }
}
```

### **Motor Module Example**
```c
#include "travel_motor_module_handler.h"

travel_motor_module_handler_t motor_handler;
travel_motor_module_config_t motor_config = {
    .address = 0x04,
    .update_rate_hz = 100,
    .max_speed_mmps = 1000,
    .max_acceleration_mmps2 = 500,
    .enable_current_protection = true,
    .current_limit_ma = 5000,
    .enable_brake_control = true
};

// Initialize motor module
hal_status_t status = travel_motor_module_init(&motor_handler, &motor_config);
if (status == HAL_STATUS_OK) {
    // Set motor speed
    status = travel_motor_module_set_speed(&motor_handler, 500);  // 500 mm/s
    if (status == HAL_STATUS_OK) {
        printf("Motor speed set to 500 mm/s\n");
    }
}
```

---

## 🔄 **VERSION HISTORY**

### **v3.0.0 (2025-01-28)**
- ✅ Complete API documentation for all module handlers
- ✅ HAL layer API documentation
- ✅ Error codes and usage examples
- ✅ Configuration structures documented
- ✅ Function signatures updated

### **v2.0.0 (2024-12-19)**
- 🔧 Initial API documentation
- 🔧 Basic function descriptions
- ⚠️ Incomplete documentation

---

**📞 Liên hệ:** FW Team  
**📧 Email:** firmware@oht50.com  
**🔗 Repository:** `firmware_new/`
