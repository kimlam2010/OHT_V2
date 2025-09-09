# Safety Monitor API Reference

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-28  
**Module:** Safety Monitor  
**Team:** FIRMWARE  
**Task:** LIDAR Safety Integration

---

## 📋 **TỔNG QUAN**

Safety Monitor API cung cấp interface để quản lý safety system của OHT-50 Master Module, bao gồm:
- Safety zones monitoring với LiDAR integration
- E-Stop handling (hardware & software)
- LED status indication
- Configuration management
- Real-time safety status

---

## 🔧 **CORE FUNCTIONS**

### **Initialization & Management**

#### `safety_monitor_init()`
```c
hal_status_t safety_monitor_init(void);
```
**Mô tả:** Khởi tạo safety monitor system  
**Returns:** `HAL_STATUS_OK` nếu thành công  
**Lưu ý:** Phải gọi trước khi sử dụng các functions khác

#### `safety_monitor_deinit()`
```c
hal_status_t safety_monitor_deinit(void);
```
**Mô tả:** Deinitialize safety monitor system  
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_update()`
```c
hal_status_t safety_monitor_update(void);
```
**Mô tả:** Update safety monitor state (gọi định kỳ)  
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_update_with_lidar()`
```c
hal_status_t safety_monitor_update_with_lidar(const lidar_scan_data_t *scan_data);
```
**Mô tả:** Update safety monitor với LiDAR data  
**Parameters:**
- `scan_data`: LiDAR scan data pointer
**Returns:** `HAL_STATUS_OK` nếu thành công

---

### **Safety Zones Management**

#### `safety_monitor_set_basic_zones()`
```c
hal_status_t safety_monitor_set_basic_zones(const basic_safety_zones_t *zones);
```
**Mô tả:** Set safety zones configuration  
**Parameters:**
- `zones`: Safety zones configuration pointer
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_get_basic_zones()`
```c
hal_status_t safety_monitor_get_basic_zones(basic_safety_zones_t *zones);
```
**Mô tả:** Get current safety zones configuration  
**Parameters:**
- `zones`: Pointer để store safety zones data
**Returns:** `HAL_STATUS_OK` nếu thành công

---

### **E-Stop Management**

#### `safety_monitor_trigger_emergency_stop()`
```c
hal_status_t safety_monitor_trigger_emergency_stop(const char* reason);
```
**Mô tả:** Trigger software emergency stop  
**Parameters:**
- `reason`: Lý do emergency stop
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_trigger_lidar_emergency_stop()`
```c
hal_status_t safety_monitor_trigger_lidar_emergency_stop(const lidar_scan_data_t *scan_data, const char* reason);
```
**Mô tả:** Trigger emergency stop từ LiDAR data  
**Parameters:**
- `scan_data`: LiDAR scan data
- `reason`: Lý do emergency stop
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_is_estop_active()`
```c
hal_status_t safety_monitor_is_estop_active(bool* estop_active);
```
**Mô tả:** Check if E-Stop is active  
**Parameters:**
- `estop_active`: Pointer để store E-Stop status
**Returns:** `HAL_STATUS_OK` nếu thành công

---

### **Status & Information**

#### `safety_monitor_get_status()`
```c
hal_status_t safety_monitor_get_status(safety_monitor_status_t *status);
```
**Mô tả:** Get safety monitor status  
**Parameters:**
- `status`: Pointer để store status data
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_get_config()`
```c
hal_status_t safety_monitor_get_config(safety_monitor_config_t *config);
```
**Mô tả:** Get safety monitor configuration  
**Parameters:**
- `config`: Pointer để store configuration data
**Returns:** `HAL_STATUS_OK` nếu thành công

---

### **LED Status Management**

#### `safety_monitor_set_communication_led_pattern()`
```c
hal_status_t safety_monitor_set_communication_led_pattern(bool modules_online, uint32_t online_count);
```
**Mô tả:** Set communication LED pattern based on module status  
**Parameters:**
- `modules_online`: Whether modules are online
- `online_count`: Number of online modules
**Returns:** `HAL_STATUS_OK` nếu thành công

---

## ⚙️ **CONFIGURATION MANAGEMENT**

### **Configuration Persistence**

#### `safety_monitor_load_config()`
```c
hal_status_t safety_monitor_load_config(void);
```
**Mô tả:** Load configuration từ persistent storage  
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_save_config()`
```c
hal_status_t safety_monitor_save_config(void);
```
**Mô tả:** Save configuration vào persistent storage  
**Returns:** `HAL_STATUS_OK` nếu thành công

### **JSON Import/Export**

#### `safety_monitor_export_config_json()`
```c
hal_status_t safety_monitor_export_config_json(char *buffer, size_t buffer_size, size_t *actual_size);
```
**Mô tả:** Export configuration ra JSON string  
**Parameters:**
- `buffer`: Buffer để store JSON string
- `buffer_size`: Size của buffer
- `actual_size`: Actual size của JSON string
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_import_config_json()`
```c
hal_status_t safety_monitor_import_config_json(const char *json_string);
```
**Mô tả:** Import configuration từ JSON string  
**Parameters:**
- `json_string`: JSON configuration string
**Returns:** `HAL_STATUS_OK` nếu thành công

### **Factory Reset & Validation**

#### `safety_monitor_reset_config_to_factory()`
```c
hal_status_t safety_monitor_reset_config_to_factory(void);
```
**Mô tả:** Reset configuration về factory defaults  
**Returns:** `HAL_STATUS_OK` nếu thành công

#### `safety_monitor_validate_config()`
```c
hal_status_t safety_monitor_validate_config(bool *valid);
```
**Mô tả:** Validate configuration  
**Parameters:**
- `valid`: Pointer để store validation result
**Returns:** `HAL_STATUS_OK` nếu thành công

---

## 📊 **DATA STRUCTURES**

### **Safety Monitor Status**
```c
typedef struct {
    safety_monitor_state_t current_state;
    safety_monitor_state_t previous_state;
    uint64_t state_entry_time;
    uint32_t state_transition_count;
    uint64_t last_update_time;
    uint64_t last_safety_check;
    uint32_t violation_count;
    uint32_t fault_count;
    bool zone_violation;
    basic_safety_zones_t safety_zones;
} safety_monitor_status_t;
```

### **Safety Monitor Configuration**
```c
typedef struct {
    uint32_t estop_timeout_ms;
    uint32_t zone_check_period_ms;
    uint32_t interlock_check_period_ms;
    uint32_t sensor_check_period_ms;
    uint32_t watchdog_timeout_ms;
    bool enable_zone_monitoring;
    bool enable_interlock_monitoring;
    bool enable_sensor_monitoring;
    bool enable_watchdog_monitoring;
} safety_monitor_config_t;
```

### **Basic Safety Zones**
```c
typedef struct {
    bool enabled;
    uint16_t emergency_zone_mm;
    uint16_t warning_zone_mm;
    uint16_t safe_zone_mm;
    uint16_t min_distance_mm;
    uint16_t min_distance_angle;
    bool emergency_violated;
    bool warning_violated;
    bool safe_violated;
    uint64_t last_violation_time;
} basic_safety_zones_t;
```

---

## 🚨 **SAFETY STATES**

### **State Definitions**
```c
typedef enum {
    SAFETY_MONITOR_STATE_INIT = 0,
    SAFETY_MONITOR_STATE_SAFE,
    SAFETY_MONITOR_STATE_WARNING,
    SAFETY_MONITOR_STATE_CRITICAL,
    SAFETY_MONITOR_STATE_ESTOP,
    SAFETY_MONITOR_STATE_FAULT
} safety_monitor_state_t;
```

### **State Transitions**
- **INIT → SAFE:** System initialized successfully
- **SAFE → WARNING:** Warning zone violated
- **SAFE → CRITICAL:** Critical condition detected
- **ANY → ESTOP:** Emergency stop triggered
- **ANY → FAULT:** System fault detected
- **WARNING → SAFE:** Conditions returned to safe
- **CRITICAL → WARNING:** Conditions improved
- **ESTOP → SAFE:** E-Stop cleared (manual reset required)
- **FAULT → SAFE:** Fault cleared (manual reset required)

---

## 🔌 **API ENDPOINTS**

### **Safety Status**
```
GET /api/v1/safety/status
```
**Response:** Current safety status và zone information

### **E-Stop Control**
```
POST /api/v1/safety/estop
```
**Request:** Trigger software E-Stop  
**Response:** E-Stop status

### **Safety Zones**
```
GET /api/v1/safety/zones
```
**Response:** Current safety zones configuration

```
PUT /api/v1/safety/zones
```
**Request:** Update safety zones configuration  
**Response:** Update status

### **Configuration Management**
```
GET /api/v1/safety/config
```
**Response:** Current safety configuration

```
PUT /api/v1/safety/config
```
**Request:** Update safety configuration  
**Response:** Update status

```
GET /api/v1/safety/config/export
```
**Response:** Configuration file download

```
POST /api/v1/safety/config/import
```
**Request:** Import configuration file  
**Response:** Import status

```
POST /api/v1/safety/config/reset
```
**Response:** Factory reset status

---

## ⚠️ **ERROR CODES**

| Code | Description |
|------|-------------|
| `HAL_STATUS_OK` | Operation successful |
| `HAL_STATUS_INVALID_PARAMETER` | Invalid parameter |
| `HAL_STATUS_NOT_INITIALIZED` | Safety monitor not initialized |
| `HAL_STATUS_INVALID_STATE` | Invalid state transition |
| `HAL_STATUS_ERROR` | General error |

---

## 📝 **USAGE EXAMPLES**

### **Basic Initialization**
```c
// Initialize safety monitor
hal_status_t status = safety_monitor_init();
if (status != HAL_STATUS_OK) {
    printf("Failed to initialize safety monitor\n");
    return status;
}

// Set safety zones
basic_safety_zones_t zones = {
    .enabled = true,
    .emergency_zone_mm = 500,
    .warning_zone_mm = 1000,
    .safe_zone_mm = 2000
};
safety_monitor_set_basic_zones(&zones);

// Main loop
while (running) {
    safety_monitor_update();
    usleep(50000); // 50ms
}
```

### **LiDAR Integration**
```c
// Update with LiDAR data
lidar_scan_data_t scan_data;
// ... populate scan_data ...
safety_monitor_update_with_lidar(&scan_data);

// Check for violations
basic_safety_zones_t zones;
safety_monitor_get_basic_zones(&zones);
if (zones.emergency_violated) {
    printf("Emergency zone violated!\n");
}
```

### **Configuration Management**
```c
// Load configuration
safety_monitor_load_config();

// Export configuration
char json_buffer[2048];
size_t actual_size;
safety_monitor_export_config_json(json_buffer, sizeof(json_buffer), &actual_size);
printf("Configuration: %s\n", json_buffer);

// Reset to factory defaults
safety_monitor_reset_config_to_factory();
```

---

## 🔒 **SAFETY CONSIDERATIONS**

### **Critical Safety Rules**
1. **E-Stop Priority:** E-Stop có priority cao nhất
2. **Zone Validation:** Emergency < Warning < Safe zones
3. **State Transitions:** Chỉ allow valid state transitions
4. **Configuration Validation:** Validate trước khi apply
5. **LED Feedback:** Visual feedback cho mọi safety events

### **Error Handling**
- **Initialization Failures:** System không start nếu safety monitor fail
- **Configuration Errors:** Fallback to factory defaults
- **LiDAR Failures:** Fallback to basic safety monitoring
- **State Errors:** Transition to FAULT state

---

**Changelog v1.0:**
- ✅ Created comprehensive API reference
- ✅ Added all function descriptions
- ✅ Added data structures documentation
- ✅ Added API endpoints reference
- ✅ Added usage examples
- ✅ Added safety considerations

**🚨 Lưu ý:** Luôn tuân thủ safety rules và error handling guidelines.
