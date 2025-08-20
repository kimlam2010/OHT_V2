# ⚙️ CONFIGURATION SERVICE IMPLEMENTATION

**Version:** 1.0  
**Last Updated:** 2025-01-28  
**Team:** FW (Firmware)  
**Status:** 🚧 IN PROGRESS  
**Dependencies:** HAL Layer, Module Management

---

## 📋 **TỔNG QUAN**

Configuration Service quản lý cấu hình hệ thống OHT-50 Master Module, bao gồm:
- Cấu hình module (Power, Motor, IO)
- Cấu hình an toàn (E-Stop, fault thresholds)
- Cấu hình mạng (RS485, Ethernet, WiFi)
- Cấu hình logging và telemetry
- Version control và rollback

---

## 🏗️ **KIẾN TRÚC**

### **Service Structure:**
```
firmware/src/services/
├── config/
│   ├── config_service.c          # Main service
│   ├── config_manager.c          # Configuration management
│   ├── config_validator.c        # Schema validation
│   ├── config_storage.c          # File system storage
│   ├── config_backup.c           # Backup/restore
│   └── config_api.c              # REST API interface
├── include/
│   └── config_service.h          # Public API
└── tests/
    └── config_service_test.c     # Unit tests
```

### **Configuration Schema:**
```json
{
  "version": "1.0",
  "timestamp": "2025-01-28T10:00:00Z",
  "system": {
    "name": "OHT-50-MASTER-001",
    "location": "Production Line A",
    "description": "Master Module for OHT-50 System"
  },
  "modules": {
    "power": {
      "enabled": true,
      "voltage_monitor": {
        "min_voltage": 22.0,
        "max_voltage": 26.0,
        "alarm_threshold": 21.5
      }
    },
    "motor": {
      "enabled": true,
      "motion_profile": {
        "max_velocity": 2.0,
        "max_acceleration": 1.0,
        "max_jerk": 0.5
      },
      "safety_limits": {
        "emergency_stop_distance": 0.1,
        "soft_stop_distance": 0.5
      }
    },
    "io": {
      "enabled": true,
      "led_indicators": {
        "power_led": "GPIO1_A0",
        "system_led": "GPIO1_A1",
        "comm_led": "GPIO1_A2",
        "network_led": "GPIO1_A3",
        "error_led": "GPIO1_A4"
      },
      "relay_outputs": {
        "relay1": "GPIO1_D3",
        "relay2": "GPIO1_D2"
      }
    }
  },
  "communication": {
    "rs485": {
      "enabled": true,
      "baud_rate": 115200,
      "data_bits": 8,
      "parity": "none",
      "stop_bits": 1,
      "timeout_ms": 1000,
      "retry_count": 3
    },
    "ethernet": {
      "enabled": true,
      "ip_address": "192.168.1.100",
      "subnet_mask": "255.255.255.0",
      "gateway": "192.168.1.1",
      "port": 8080
    },
    "wifi": {
      "enabled": false,
      "ssid": "",
      "password": "",
      "backup_only": true
    }
  },
  "safety": {
    "e_stop": {
      "enabled": true,
      "dual_channel": true,
      "timeout_ms": 100,
      "auto_reset": false
    },
    "fault_detection": {
      "voltage_fault": true,
      "communication_fault": true,
      "motion_fault": true,
      "temperature_fault": true
    }
  },
  "logging": {
    "level": "INFO",
    "file_rotation": true,
    "max_file_size_mb": 10,
    "max_files": 5,
    "telemetry_interval_ms": 1000
  }
}
```

---

## 🔧 **IMPLEMENTATION DETAILS**

### **1. Configuration Service Core (config_service.c)**

```c
// Configuration service state
typedef struct {
    config_t current_config;
    config_t default_config;
    config_t backup_config;
    bool config_loaded;
    bool config_valid;
    pthread_mutex_t config_mutex;
    config_callback_t change_callback;
} config_service_t;

// Main service functions
int config_service_init(void);
int config_service_deinit(void);
int config_service_load_config(const char* filename);
int config_service_save_config(const char* filename);
int config_service_validate_config(const config_t* config);
int config_service_apply_config(const config_t* config);
int config_service_backup_config(void);
int config_service_restore_config(void);
```

### **2. Configuration Manager (config_manager.c)**

```c
// Configuration management functions
int config_get_module_config(const char* module_name, module_config_t* config);
int config_set_module_config(const char* module_name, const module_config_t* config);
int config_get_safety_config(safety_config_t* config);
int config_set_safety_config(const safety_config_t* config);
int config_get_communication_config(comm_config_t* config);
int config_set_communication_config(const comm_config_t* config);
int config_get_logging_config(logging_config_t* config);
int config_set_logging_config(const logging_config_t* config);
```

### **3. Configuration Validator (config_validator.c)**

```c
// Validation functions
bool config_validate_schema(const json_t* config);
bool config_validate_module_config(const module_config_t* config);
bool config_validate_safety_config(const safety_config_t* config);
bool config_validate_communication_config(const comm_config_t* config);
bool config_validate_logging_config(const logging_config_t* config);

// Validation rules
typedef struct {
    const char* field_name;
    config_field_type_t field_type;
    bool required;
    double min_value;
    double max_value;
    const char* pattern;
} config_validation_rule_t;
```

### **4. Configuration Storage (config_storage.c)**

```c
// File system operations
int config_save_to_file(const config_t* config, const char* filename);
int config_load_from_file(config_t* config, const char* filename);
int config_create_backup(const char* source_file, const char* backup_file);
int config_restore_from_backup(const char* backup_file, const char* target_file);

// Storage locations
#define CONFIG_FILE_PATH "/etc/oht50/config.json"
#define CONFIG_BACKUP_PATH "/etc/oht50/backup/"
#define CONFIG_TEMP_PATH "/tmp/oht50_config.json"
```

---

## 🌐 **REST API INTERFACE**

### **Configuration API Endpoints:**

```c
// GET /api/v1/config
// Lấy cấu hình hiện tại
int config_api_get_config(http_request_t* req, http_response_t* resp);

// PUT /api/v1/config
// Cập nhật cấu hình
int config_api_update_config(http_request_t* req, http_response_t* resp);

// GET /api/v1/config/modules/{module_name}
// Lấy cấu hình module cụ thể
int config_api_get_module_config(http_request_t* req, http_response_t* resp);

// PUT /api/v1/config/modules/{module_name}
// Cập nhật cấu hình module
int config_api_update_module_config(http_request_t* req, http_response_t* resp);

// POST /api/v1/config/backup
// Tạo backup cấu hình
int config_api_create_backup(http_request_t* req, http_response_t* resp);

// POST /api/v1/config/restore
// Khôi phục cấu hình từ backup
int config_api_restore_config(http_request_t* req, http_response_t* resp);

// GET /api/v1/config/validate
// Validate cấu hình hiện tại
int config_api_validate_config(http_request_t* req, http_response_t* resp);
```

### **API Response Format:**

```json
{
  "status": "success",
  "data": {
    "config": { /* configuration object */ },
    "version": "1.0",
    "timestamp": "2025-01-28T10:00:00Z",
    "checksum": "sha256_hash"
  },
  "message": "Configuration loaded successfully"
}
```

---

## 🔄 **CONFIGURATION LIFECYCLE**

### **1. Startup Sequence:**
```c
// 1. Load default configuration
config_service_load_defaults();

// 2. Try to load saved configuration
if (config_service_load_config(CONFIG_FILE_PATH) != 0) {
    // Use default if load fails
    config_service_use_defaults();
}

// 3. Validate configuration
if (!config_service_validate_config(&current_config)) {
    // Log error and use defaults
    log_error("Invalid configuration, using defaults");
    config_service_use_defaults();
}

// 4. Apply configuration to modules
config_service_apply_config(&current_config);
```

### **2. Runtime Updates:**
```c
// 1. Validate new configuration
if (!config_validate_schema(new_config)) {
    return CONFIG_ERROR_INVALID_SCHEMA;
}

// 2. Backup current configuration
config_service_backup_config();

// 3. Apply new configuration
int result = config_service_apply_config(new_config);
if (result != 0) {
    // Rollback on failure
    config_service_restore_config();
    return result;
}

// 4. Save new configuration
config_service_save_config(CONFIG_FILE_PATH);
```

### **3. Error Handling:**
```c
typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERROR_INVALID_SCHEMA = -1,
    CONFIG_ERROR_VALIDATION_FAILED = -2,
    CONFIG_ERROR_APPLY_FAILED = -3,
    CONFIG_ERROR_STORAGE_FAILED = -4,
    CONFIG_ERROR_BACKUP_FAILED = -5
} config_error_t;
```

---

## 🧪 **TESTING STRATEGY**

### **Unit Tests:**
```c
// Test configuration validation
void test_config_validation(void);
void test_module_config_validation(void);
void test_safety_config_validation(void);

// Test configuration storage
void test_config_save_load(void);
void test_config_backup_restore(void);

// Test configuration API
void test_config_api_endpoints(void);
void test_config_api_validation(void);
```

### **Integration Tests:**
```c
// Test configuration with real modules
void test_config_with_power_module(void);
void test_config_with_motor_module(void);
void test_config_with_io_module(void);

// Test configuration persistence
void test_config_persistence_across_reboots(void);
void test_config_rollback_on_failure(void);
```

---

## 📊 **MONITORING & METRICS**

### **Configuration Metrics:**
- Configuration load time
- Configuration validation time
- Configuration apply time
- Configuration error rate
- Backup/restore success rate

### **Health Checks:**
```c
typedef struct {
    bool config_loaded;
    bool config_valid;
    bool backup_available;
    time_t last_backup_time;
    uint32_t config_errors;
    uint32_t validation_failures;
} config_health_t;

int config_service_get_health(config_health_t* health);
```

---

## 🔒 **SECURITY CONSIDERATIONS**

### **Access Control:**
- Configuration changes require authentication
- Sensitive configuration encrypted at rest
- Configuration backup encrypted
- Audit trail for all configuration changes

### **Validation:**
- Schema validation for all configuration
- Range checking for numeric values
- Pattern validation for strings
- Cross-field validation

---

## 📚 **DEPENDENCIES**

### **Internal Dependencies:**
- HAL Layer (GPIO, UART, Network)
- Module Management Service
- Logging Service
- Safety System

### **External Dependencies:**
- JSON parsing library (jansson)
- File system access
- HTTP server (for API)
- Encryption library (for secure storage)

---

## 🚀 **DEPLOYMENT**

### **Build Configuration:**
```makefile
# Add to firmware Makefile
CONFIG_SERVICE_SOURCES = \
    src/services/config/config_service.c \
    src/services/config/config_manager.c \
    src/services/config/config_validator.c \
    src/services/config/config_storage.c \
    src/services/config/config_backup.c \
    src/services/config/config_api.c

CONFIG_SERVICE_HEADERS = \
    include/config_service.h
```

### **Installation:**
```bash
# Create configuration directory
sudo mkdir -p /etc/oht50
sudo mkdir -p /etc/oht50/backup

# Set permissions
sudo chown oht50:oht50 /etc/oht50
sudo chmod 755 /etc/oht50

# Install default configuration
sudo cp config/default_config.json /etc/oht50/config.json
```

---

## 📝 **CHANGELOG**

**v1.0 (2025-01-28):**
- ✅ Initial implementation
- ✅ Configuration schema definition
- ✅ Basic CRUD operations
- ✅ Validation framework
- ✅ REST API interface
- ✅ Backup/restore functionality
- ✅ Unit test framework

---

**Next Steps:**
- [ ] Implement configuration encryption
- [ ] Add configuration change notifications
- [ ] Implement configuration templates
- [ ] Add configuration import/export
- [ ] Implement configuration versioning
- [ ] Add configuration migration tools
**Version:** 1.0  
**Last Updated:** 2025-01-28  
**Team:** FW (Firmware)  
**Status:** 🚧 IN PROGRESS  
**Dependencies:** HAL Layer, Module Management

---

## 📋 **TỔNG QUAN**

Configuration Service quản lý cấu hình hệ thống OHT-50 Master Module, bao gồm:
- Cấu hình module (Power, Motor, IO)
- Cấu hình an toàn (E-Stop, fault thresholds)
- Cấu hình mạng (RS485, Ethernet, WiFi)
- Cấu hình logging và telemetry
- Version control và rollback

---

## 🏗️ **KIẾN TRÚC**

### **Service Structure:**
```
firmware/src/services/
├── config/
│   ├── config_service.c          # Main service
│   ├── config_manager.c          # Configuration management
│   ├── config_validator.c        # Schema validation
│   ├── config_storage.c          # File system storage
│   ├── config_backup.c           # Backup/restore
│   └── config_api.c              # REST API interface
├── include/
│   └── config_service.h          # Public API
└── tests/
    └── config_service_test.c     # Unit tests
```

### **Configuration Schema:**
```json
{
  "version": "1.0",
  "timestamp": "2025-01-28T10:00:00Z",
  "system": {
    "name": "OHT-50-MASTER-001",
    "location": "Production Line A",
    "description": "Master Module for OHT-50 System"
  },
  "modules": {
    "power": {
      "enabled": true,
      "voltage_monitor": {
        "min_voltage": 22.0,
        "max_voltage": 26.0,
        "alarm_threshold": 21.5
      }
    },
    "motor": {
      "enabled": true,
      "motion_profile": {
        "max_velocity": 2.0,
        "max_acceleration": 1.0,
        "max_jerk": 0.5
      },
      "safety_limits": {
        "emergency_stop_distance": 0.1,
        "soft_stop_distance": 0.5
      }
    },
    "io": {
      "enabled": true,
      "led_indicators": {
        "power_led": "GPIO1_A0",
        "system_led": "GPIO1_A1",
        "comm_led": "GPIO1_A2",
        "network_led": "GPIO1_A3",
        "error_led": "GPIO1_A4"
      },
      "relay_outputs": {
        "relay1": "GPIO1_D3",
        "relay2": "GPIO1_D2"
      }
    }
  },
  "communication": {
    "rs485": {
      "enabled": true,
      "baud_rate": 115200,
      "data_bits": 8,
      "parity": "none",
      "stop_bits": 1,
      "timeout_ms": 1000,
      "retry_count": 3
    },
    "ethernet": {
      "enabled": true,
      "ip_address": "192.168.1.100",
      "subnet_mask": "255.255.255.0",
      "gateway": "192.168.1.1",
      "port": 8080
    },
    "wifi": {
      "enabled": false,
      "ssid": "",
      "password": "",
      "backup_only": true
    }
  },
  "safety": {
    "e_stop": {
      "enabled": true,
      "dual_channel": true,
      "timeout_ms": 100,
      "auto_reset": false
    },
    "fault_detection": {
      "voltage_fault": true,
      "communication_fault": true,
      "motion_fault": true,
      "temperature_fault": true
    }
  },
  "logging": {
    "level": "INFO",
    "file_rotation": true,
    "max_file_size_mb": 10,
    "max_files": 5,
    "telemetry_interval_ms": 1000
  }
}
```

---

## 🔧 **IMPLEMENTATION DETAILS**

### **1. Configuration Service Core (config_service.c)**

```c
// Configuration service state
typedef struct {
    config_t current_config;
    config_t default_config;
    config_t backup_config;
    bool config_loaded;
    bool config_valid;
    pthread_mutex_t config_mutex;
    config_callback_t change_callback;
} config_service_t;

// Main service functions
int config_service_init(void);
int config_service_deinit(void);
int config_service_load_config(const char* filename);
int config_service_save_config(const char* filename);
int config_service_validate_config(const config_t* config);
int config_service_apply_config(const config_t* config);
int config_service_backup_config(void);
int config_service_restore_config(void);
```

### **2. Configuration Manager (config_manager.c)**

```c
// Configuration management functions
int config_get_module_config(const char* module_name, module_config_t* config);
int config_set_module_config(const char* module_name, const module_config_t* config);
int config_get_safety_config(safety_config_t* config);
int config_set_safety_config(const safety_config_t* config);
int config_get_communication_config(comm_config_t* config);
int config_set_communication_config(const comm_config_t* config);
int config_get_logging_config(logging_config_t* config);
int config_set_logging_config(const logging_config_t* config);
```

### **3. Configuration Validator (config_validator.c)**

```c
// Validation functions
bool config_validate_schema(const json_t* config);
bool config_validate_module_config(const module_config_t* config);
bool config_validate_safety_config(const safety_config_t* config);
bool config_validate_communication_config(const comm_config_t* config);
bool config_validate_logging_config(const logging_config_t* config);

// Validation rules
typedef struct {
    const char* field_name;
    config_field_type_t field_type;
    bool required;
    double min_value;
    double max_value;
    const char* pattern;
} config_validation_rule_t;
```

### **4. Configuration Storage (config_storage.c)**

```c
// File system operations
int config_save_to_file(const config_t* config, const char* filename);
int config_load_from_file(config_t* config, const char* filename);
int config_create_backup(const char* source_file, const char* backup_file);
int config_restore_from_backup(const char* backup_file, const char* target_file);

// Storage locations
#define CONFIG_FILE_PATH "/etc/oht50/config.json"
#define CONFIG_BACKUP_PATH "/etc/oht50/backup/"
#define CONFIG_TEMP_PATH "/tmp/oht50_config.json"
```

---

## 🌐 **REST API INTERFACE**

### **Configuration API Endpoints:**

```c
// GET /api/v1/config
// Lấy cấu hình hiện tại
int config_api_get_config(http_request_t* req, http_response_t* resp);

// PUT /api/v1/config
// Cập nhật cấu hình
int config_api_update_config(http_request_t* req, http_response_t* resp);

// GET /api/v1/config/modules/{module_name}
// Lấy cấu hình module cụ thể
int config_api_get_module_config(http_request_t* req, http_response_t* resp);

// PUT /api/v1/config/modules/{module_name}
// Cập nhật cấu hình module
int config_api_update_module_config(http_request_t* req, http_response_t* resp);

// POST /api/v1/config/backup
// Tạo backup cấu hình
int config_api_create_backup(http_request_t* req, http_response_t* resp);

// POST /api/v1/config/restore
// Khôi phục cấu hình từ backup
int config_api_restore_config(http_request_t* req, http_response_t* resp);

// GET /api/v1/config/validate
// Validate cấu hình hiện tại
int config_api_validate_config(http_request_t* req, http_response_t* resp);
```

### **API Response Format:**

```json
{
  "status": "success",
  "data": {
    "config": { /* configuration object */ },
    "version": "1.0",
    "timestamp": "2025-01-28T10:00:00Z",
    "checksum": "sha256_hash"
  },
  "message": "Configuration loaded successfully"
}
```

---

## 🔄 **CONFIGURATION LIFECYCLE**

### **1. Startup Sequence:**
```c
// 1. Load default configuration
config_service_load_defaults();

// 2. Try to load saved configuration
if (config_service_load_config(CONFIG_FILE_PATH) != 0) {
    // Use default if load fails
    config_service_use_defaults();
}

// 3. Validate configuration
if (!config_service_validate_config(&current_config)) {
    // Log error and use defaults
    log_error("Invalid configuration, using defaults");
    config_service_use_defaults();
}

// 4. Apply configuration to modules
config_service_apply_config(&current_config);
```

### **2. Runtime Updates:**
```c
// 1. Validate new configuration
if (!config_validate_schema(new_config)) {
    return CONFIG_ERROR_INVALID_SCHEMA;
}

// 2. Backup current configuration
config_service_backup_config();

// 3. Apply new configuration
int result = config_service_apply_config(new_config);
if (result != 0) {
    // Rollback on failure
    config_service_restore_config();
    return result;
}

// 4. Save new configuration
config_service_save_config(CONFIG_FILE_PATH);
```

### **3. Error Handling:**
```c
typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERROR_INVALID_SCHEMA = -1,
    CONFIG_ERROR_VALIDATION_FAILED = -2,
    CONFIG_ERROR_APPLY_FAILED = -3,
    CONFIG_ERROR_STORAGE_FAILED = -4,
    CONFIG_ERROR_BACKUP_FAILED = -5
} config_error_t;
```

---

## 🧪 **TESTING STRATEGY**

### **Unit Tests:**
```c
// Test configuration validation
void test_config_validation(void);
void test_module_config_validation(void);
void test_safety_config_validation(void);

// Test configuration storage
void test_config_save_load(void);
void test_config_backup_restore(void);

// Test configuration API
void test_config_api_endpoints(void);
void test_config_api_validation(void);
```

### **Integration Tests:**
```c
// Test configuration with real modules
void test_config_with_power_module(void);
void test_config_with_motor_module(void);
void test_config_with_io_module(void);

// Test configuration persistence
void test_config_persistence_across_reboots(void);
void test_config_rollback_on_failure(void);
```

---

## 📊 **MONITORING & METRICS**

### **Configuration Metrics:**
- Configuration load time
- Configuration validation time
- Configuration apply time
- Configuration error rate
- Backup/restore success rate

### **Health Checks:**
```c
typedef struct {
    bool config_loaded;
    bool config_valid;
    bool backup_available;
    time_t last_backup_time;
    uint32_t config_errors;
    uint32_t validation_failures;
} config_health_t;

int config_service_get_health(config_health_t* health);
```

---

## 🔒 **SECURITY CONSIDERATIONS**

### **Access Control:**
- Configuration changes require authentication
- Sensitive configuration encrypted at rest
- Configuration backup encrypted
- Audit trail for all configuration changes

### **Validation:**
- Schema validation for all configuration
- Range checking for numeric values
- Pattern validation for strings
- Cross-field validation

---

## 📚 **DEPENDENCIES**

### **Internal Dependencies:**
- HAL Layer (GPIO, UART, Network)
- Module Management Service
- Logging Service
- Safety System

### **External Dependencies:**
- JSON parsing library (jansson)
- File system access
- HTTP server (for API)
- Encryption library (for secure storage)

---

## 🚀 **DEPLOYMENT**

### **Build Configuration:**
```makefile
# Add to firmware Makefile
CONFIG_SERVICE_SOURCES = \
    src/services/config/config_service.c \
    src/services/config/config_manager.c \
    src/services/config/config_validator.c \
    src/services/config/config_storage.c \
    src/services/config/config_backup.c \
    src/services/config/config_api.c

CONFIG_SERVICE_HEADERS = \
    include/config_service.h
```

### **Installation:**
```bash
# Create configuration directory
sudo mkdir -p /etc/oht50
sudo mkdir -p /etc/oht50/backup

# Set permissions
sudo chown oht50:oht50 /etc/oht50
sudo chmod 755 /etc/oht50

# Install default configuration
sudo cp config/default_config.json /etc/oht50/config.json
```

---

## 📝 **CHANGELOG**

**v1.0 (2025-01-28):**
- ✅ Initial implementation
- ✅ Configuration schema definition
- ✅ Basic CRUD operations
- ✅ Validation framework
- ✅ REST API interface
- ✅ Backup/restore functionality
- ✅ Unit test framework

---

**Next Steps:**
- [ ] Implement configuration encryption
- [ ] Add configuration change notifications
- [ ] Implement configuration templates
- [ ] Add configuration import/export
- [ ] Implement configuration versioning
- [ ] Add configuration migration tools
