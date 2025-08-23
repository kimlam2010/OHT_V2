# Interface Definitions - OHT-50 Master Module (Phiên bản 2.0)

**Phiên bản:** v2.0  
**Ngày tạo:** 2025-01-28  
**Team:** FW Team  
**Trạng thái:** 📋 PRODUCTION READY

---

## 🎯 **TỔNG QUAN INTERFACE**

### **Mục tiêu:**
- Định nghĩa rõ ràng các interface giữa các module và services
- Chuẩn hóa giao tiếp nội bộ và bên ngoài
- Đảm bảo tính nhất quán và khả năng mở rộng
- Hỗ trợ PM lên task cho các team

---

## 🔌 **HAL INTERFACES**

### **1. HAL GPIO Interface**
```c
// GPIO Control Interface
typedef struct {
    uint8_t pin;           // GPIO pin number
    uint8_t direction;     // INPUT/OUTPUT
    uint8_t value;         // Current value
    uint8_t pull_up_down;  // Pull-up/down configuration
} hal_gpio_config_t;

// Function prototypes
int hal_gpio_init(void);
int hal_gpio_set_direction(uint8_t pin, uint8_t direction);
int hal_gpio_set_value(uint8_t pin, uint8_t value);
int hal_gpio_get_value(uint8_t pin, uint8_t *value);
int hal_gpio_set_pull_up_down(uint8_t pin, uint8_t pull_up_down);
```

### **2. HAL UART/RS485 Interface**
```c
// RS485 Configuration
typedef struct {
    uint32_t baudrate;     // Baudrate (115200)
    uint8_t data_bits;     // Data bits (8)
    uint8_t stop_bits;     // Stop bits (1)
    uint8_t parity;        // Parity (NONE)
    uint8_t flow_control;  // Flow control (NONE)
    uint8_t de_pin;        // DE pin for RS485
    uint8_t re_pin;        // RE pin for RS485
} hal_rs485_config_t;

// Function prototypes
int hal_rs485_init(hal_rs485_config_t *config);
int hal_rs485_send(uint8_t *data, uint16_t length);
int hal_rs485_receive(uint8_t *data, uint16_t *length);
int hal_rs485_set_de(uint8_t state);
int hal_rs485_set_re(uint8_t state);
```

### **3. HAL Network Interface**
```c
// Network Configuration
typedef struct {
    char ip_address[16];   // IP address
    char netmask[16];      // Netmask
    char gateway[16];      // Gateway
    char dns[16];          // DNS server
    uint8_t dhcp_enabled;  // DHCP enabled
} hal_network_config_t;

// Function prototypes
int hal_network_init(void);
int hal_network_set_config(hal_network_config_t *config);
int hal_network_get_config(hal_network_config_t *config);
int hal_network_connect(void);
int hal_network_disconnect(void);
int hal_network_get_status(uint8_t *status);
```

### **4. HAL Safety Interface**
```c
// Safety Configuration
typedef struct {
    uint8_t estop_pin1;    // E-Stop pin 1
    uint8_t estop_pin2;    // E-Stop pin 2
    uint8_t interlock_pin; // Interlock pin
    uint32_t timeout_ms;   // Safety timeout
} hal_safety_config_t;

// Function prototypes
int hal_safety_init(hal_safety_config_t *config);
int hal_safety_get_estop_status(uint8_t *status);
int hal_safety_get_interlock_status(uint8_t *status);
int hal_safety_set_timeout(uint32_t timeout_ms);
int hal_safety_reset(void);
```

---

## 🔄 **CORE INTERFACES**

### **1. State Machine Interface**
```c
// State Machine States
typedef enum {
    STATE_INIT = 0,
    STATE_IDLE = 1,
    STATE_MOVE = 2,
    STATE_DOCK = 3,
    STATE_FAULT = 4,
    STATE_ESTOP = 5
} system_state_t;

// State Machine Events
typedef enum {
    EVENT_INIT_COMPLETE = 0,
    EVENT_MOVE_COMMAND = 1,
    EVENT_DOCK_COMMAND = 2,
    EVENT_FAULT_DETECTED = 3,
    EVENT_ESTOP_TRIGGERED = 4,
    EVENT_FAULT_CLEARED = 5,
    EVENT_ESTOP_RESET = 6
} system_event_t;

// Function prototypes
int state_machine_init(void);
int state_machine_set_state(system_state_t state);
int state_machine_get_state(system_state_t *state);
int state_machine_trigger_event(system_event_t event);
int state_machine_register_callback(system_state_t state, void (*callback)(void));
```

### **2. Module Manager Interface**
```c
// Module Information
typedef struct {
    uint8_t address;           // Module address
    uint8_t type;              // Module type
    uint8_t status;            // Module status
    uint8_t version[4];        // Firmware version
    uint32_t last_seen;        // Last seen timestamp
    uint16_t error_count;      // Error count
} module_info_t;

// Function prototypes
int module_manager_init(void);
int module_manager_discover_modules(void);
int module_manager_get_module_count(uint8_t *count);
int module_manager_get_module_info(uint8_t address, module_info_t *info);
int module_manager_send_command(uint8_t address, uint8_t *command, uint16_t length);
int module_manager_register_callback(uint8_t address, void (*callback)(module_info_t *));
```

### **3. Communication Manager Interface**
```c
// Communication Protocol
typedef enum {
    PROTOCOL_RS485 = 0,
    PROTOCOL_ETHERNET = 1,
    PROTOCOL_WIFI = 2,
    PROTOCOL_USB = 3
} comm_protocol_t;

// Function prototypes
int comm_manager_init(void);
int comm_manager_send(comm_protocol_t protocol, uint8_t *data, uint16_t length);
int comm_manager_receive(comm_protocol_t protocol, uint8_t *data, uint16_t *length);
int comm_manager_register_handler(comm_protocol_t protocol, void (*handler)(uint8_t *, uint16_t));
int comm_manager_set_config(comm_protocol_t protocol, void *config);
```

---

## 🌐 **API INTERFACES**

### **1. REST API Interface**
```c
// API Endpoints
#define API_STATUS "/api/v1/status"
#define API_MODULES "/api/v1/modules"
#define API_COMMAND "/api/v1/command"
#define API_CONFIG "/api/v1/config"
#define API_LOGS "/api/v1/logs"

// API Response Structure
typedef struct {
    uint16_t status_code;      // HTTP status code
    char *content_type;        // Content type
    char *data;               // Response data
    uint32_t data_length;     // Data length
} api_response_t;

// Function prototypes
int api_init(void);
int api_handle_request(char *method, char *path, char *data, api_response_t *response);
int api_register_handler(char *path, void (*handler)(char *, api_response_t *));
int api_start_server(uint16_t port);
int api_stop_server(void);
```

### **2. WebSocket Interface**
```c
// WebSocket Message Types
typedef enum {
    WS_MSG_TELEMETRY = 0,
    WS_MSG_STATUS = 1,
    WS_MSG_COMMAND = 2,
    WS_MSG_ERROR = 3
} ws_message_type_t;

// WebSocket Message
typedef struct {
    ws_message_type_t type;    // Message type
    char *data;               // Message data
    uint32_t data_length;     // Data length
    uint32_t timestamp;       // Timestamp
} ws_message_t;

// Function prototypes
int websocket_init(void);
int websocket_send_message(ws_message_t *message);
int websocket_broadcast_message(ws_message_t *message);
int websocket_register_handler(ws_message_type_t type, void (*handler)(ws_message_t *));
int websocket_start_server(uint16_t port);
int websocket_stop_server(void);
```

---

## 📡 **MODBUS INTERFACES**

### **1. Modbus RTU Interface**
```c
// Modbus Function Codes
#define MODBUS_READ_COILS          0x01
#define MODBUS_READ_DISCRETE       0x02
#define MODBUS_READ_HOLDING        0x03
#define MODBUS_READ_INPUT          0x04
#define MODBUS_WRITE_SINGLE_COIL   0x05
#define MODBUS_WRITE_SINGLE_REG    0x06
#define MODBUS_WRITE_MULTIPLE_COILS 0x0F
#define MODBUS_WRITE_MULTIPLE_REGS 0x10

// Modbus Request/Response
typedef struct {
    uint8_t address;          // Slave address
    uint8_t function_code;    // Function code
    uint16_t start_address;   // Start address
    uint16_t quantity;        // Quantity
    uint8_t *data;           // Data buffer
    uint16_t data_length;    // Data length
} modbus_frame_t;

// Function prototypes
int modbus_init(void);
int modbus_send_request(modbus_frame_t *request);
int modbus_receive_response(modbus_frame_t *response);
int modbus_register_handler(uint8_t function_code, void (*handler)(modbus_frame_t *));
int modbus_set_slave_address(uint8_t address);
```

---

## 🔧 **CONFIGURATION INTERFACES**

### **1. Configuration Manager Interface**
```c
// Configuration Types
typedef enum {
    CONFIG_SYSTEM = 0,
    CONFIG_NETWORK = 1,
    CONFIG_SAFETY = 2,
    CONFIG_MODULES = 3,
    CONFIG_COMMUNICATION = 4
} config_type_t;

// Function prototypes
int config_init(void);
int config_load(config_type_t type, void *config);
int config_save(config_type_t type, void *config);
int config_validate(config_type_t type, void *config);
int config_backup(void);
int config_restore(void);
```

### **2. Logging Interface**
```c
// Log Levels
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_CRITICAL = 4
} log_level_t;

// Function prototypes
int log_init(void);
int log_write(log_level_t level, const char *module, const char *message);
int log_set_level(log_level_t level);
int log_get_logs(char *buffer, uint32_t buffer_size);
int log_clear_logs(void);
```

---

## 📊 **TELEMETRY INTERFACES**

### **1. Telemetry Manager Interface**
```c
// Telemetry Data
typedef struct {
    uint32_t timestamp;       // Timestamp
    system_state_t state;     // System state
    uint8_t module_count;     // Module count
    uint8_t error_count;      // Error count
    float cpu_usage;          // CPU usage
    float memory_usage;       // Memory usage
    float temperature;        // Temperature
} telemetry_data_t;

// Function prototypes
int telemetry_init(void);
int telemetry_start_collection(void);
int telemetry_stop_collection(void);
int telemetry_get_data(telemetry_data_t *data);
int telemetry_set_interval(uint32_t interval_ms);
int telemetry_register_callback(void (*callback)(telemetry_data_t *));
```

---

## 🚀 **DEPLOYMENT INTERFACES**

### **1. OTA Update Interface**
```c
// OTA Update Status
typedef enum {
    OTA_STATUS_IDLE = 0,
    OTA_STATUS_DOWNLOADING = 1,
    OTA_STATUS_VERIFYING = 2,
    OTA_STATUS_UPDATING = 3,
    OTA_STATUS_COMPLETE = 4,
    OTA_STATUS_ERROR = 5
} ota_status_t;

// Function prototypes
int ota_init(void);
int ota_start_update(const char *url);
int ota_get_status(ota_status_t *status);
int ota_get_progress(uint8_t *progress);
int ota_rollback(void);
int ota_verify_firmware(void);
```

---

## 📋 **TASK MAPPING CHO PM**

### **EMBED Team Tasks:**
- [ ] Implement HAL GPIO interface
- [ ] Implement HAL UART/RS485 interface  
- [ ] Implement HAL Network interface
- [ ] Implement HAL Safety interface
- [ ] Hardware validation tests

### **Firmware Team Tasks:**
- [ ] Implement State Machine interface
- [ ] Implement Module Manager interface
- [ ] Implement Communication Manager interface
- [ ] Implement Modbus RTU interface
- [ ] Unit tests cho tất cả interfaces

### **Backend Team Tasks:**
- [ ] Implement REST API interface
- [ ] Implement WebSocket interface
- [ ] Implement Configuration Manager interface
- [ ] Implement Logging interface
- [ ] API documentation

### **Frontend Team Tasks:**
- [ ] Implement WebSocket client
- [ ] Implement REST API client
- [ ] Dashboard integration
- [ ] Real-time monitoring interface

### **System Integration Tasks:**
- [ ] Interface integration testing
- [ ] End-to-end testing
- [ ] Performance validation
- [ ] Security validation

---

## 📚 **REFERENCES**

### **Related Documents:**
- `architecture.md` - Overall firmware architecture
- `state_machine.md` - State machine design
- `safety_architecture.md` - Safety system design
- `system_design.md` - System design details

### **Standards:**
- Modbus RTU Protocol Specification
- REST API Design Guidelines
- WebSocket Protocol Specification
- HAL Design Patterns

---

## 📝 **CHANGELOG**

### **v2.0 (2025-01-28):**
- ✅ Added comprehensive interface definitions
- ✅ Added HAL interfaces
- ✅ Added Core interfaces
- ✅ Added API interfaces
- ✅ Added Modbus interfaces
- ✅ Added Configuration interfaces
- ✅ Added Telemetry interfaces
- ✅ Added OTA interfaces
- ✅ Added task mapping for PM

### **v1.0 (2025-01-27):**
- ✅ Initial interface document
- ✅ Basic HAL interface definitions

---

**🚨 Lưu ý:** Tất cả interfaces phải tuân thủ chuẩn và được test đầy đủ trước khi tích hợp.
