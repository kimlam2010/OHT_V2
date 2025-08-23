# INTEGRATION GUIDE - OHT-50 (v2.0)

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** FW  
**Trạng thái:** Integration Guide for Architecture v2.0

---

## 📋 TỔNG QUAN

Hướng dẫn integration chi tiết cho hệ thống OHT-50 v2.0, bao gồm module integration, communication setup, safety integration, và system validation.

### **🏗️ Kiến trúc mới v2.0:**
- **5 Module bắt buộc:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **RS485 Standard:** Tất cả modules đều sử dụng RS485/Modbus RTU
- **Auto-discovery:** Tự động phát hiện và cấu hình modules
- **Hot-swap:** Thay đổi modules khi đang hoạt động
- **Location-based Safety:** Hệ thống an toàn dựa trên vị trí

---

## 🔧 MODULE INTEGRATION

### **1. Master Control Module Integration**

#### **Hardware Setup:**
```bash
# Orange Pi 5B Configuration
# Enable UART1 for RS485
sudo raspi-config
# Interface Options -> Serial -> Disable Serial Console -> Enable Serial Hardware

# Configure UART1 for RS485
sudo nano /boot/config.txt
# Add: dtoverlay=uart1,ctsrts
# Add: dtoverlay=gpio-no-irq

# Create udev rule for RS485
sudo nano /etc/udev/rules.d/99-oht485.rules
# Add: KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666"

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

#### **Software Configuration:**
```c
// RS485 Configuration
#define RS485_DEVICE              "/dev/ttyOHT485"
#define RS485_BAUDRATE            115200
#define RS485_DATA_BITS           8
#define RS485_STOP_BITS           1
#define RS485_PARITY              NONE
#define RS485_TIMEOUT             1000
#define RS485_RETRY_COUNT         3

// GPIO Configuration
#define GPIO_LED_POWER            0   // Power LED
#define GPIO_LED_SYSTEM           1   // System LED
#define GPIO_LED_COMM             2   // Communication LED
#define GPIO_LED_NETWORK          3   // Network LED
#define GPIO_LED_ERROR            4   // Error LED
#define GPIO_RELAY_1              5   // Relay 1
#define GPIO_RELAY_2              6   // Relay 2
#define GPIO_ESTOP_IN             7   // E-Stop input
```

### **2. Power Module Integration**

#### **Hardware Connection:**
```
Power Module (0x01) -> RS485 Bus
├── Power Supply: 48V DC from LiFePO4 battery
├── BMS Interface: I2C communication
├── Charging Interface: 4-pin connector
└── Status LEDs: Power, Charging, Fault
```

#### **Software Integration:**
```c
// Power Module Integration
typedef struct {
    uint8_t module_id;             // 0x01
    uint8_t status;                // Module status
    power_module_data_t data;      // Power data
    uint32_t last_update;          // Last update timestamp
    uint16_t error_count;          // Error count
} power_module_integration_t;

// Power monitoring function
void power_module_monitor(void) {
    // Read power status
    if (modbus_read_holding_registers(0x01, POWER_REG_STATUS, 1, &power_status) == 0) {
        // Check battery level
        if (power_status & POWER_STATUS_LOW_BATTERY) {
            system_set_warning(SYSTEM_WARNING_LOW_BATTERY);
        }
        
        // Check charging status
        if (power_status & POWER_STATUS_CHARGING) {
            system_set_state(SYSTEM_STATE_CHARGING);
        }
        
        // Check fault status
        if (power_status & POWER_STATUS_FAULT) {
            system_set_fault(SYSTEM_FAULT_POWER);
        }
    }
}
```

### **3. Safety Module Integration**

#### **Hardware Connection:**
```
Safety Module (0x02) -> RS485 Bus
├── E-Stop: Dual-channel connection
├── Safety Sensors: Light curtain, safety mats, door sensors
├── Emergency Brake: 24V DC control
└── Status LEDs: Safety, E-Stop, Fault
```

#### **Software Integration:**
```c
// Safety Module Integration
typedef struct {
    uint8_t module_id;             // 0x02
    uint8_t status;                // Module status
    safety_module_data_t data;     // Safety data
    uint32_t last_update;          // Last update timestamp
    uint16_t error_count;          // Error count
} safety_module_integration_t;

// Safety monitoring function
void safety_module_monitor(void) {
    // Read safety status
    if (modbus_read_holding_registers(0x02, SAFETY_REG_ESTOP_STATUS, 1, &safety_status) == 0) {
        // Check E-Stop status
        if (safety_status & ESTOP_STATUS_ACTIVATED) {
            system_emergency_stop();
        }
        
        // Check safety level
        uint8_t safety_level = (safety_status >> 8) & 0xFF;
        switch (safety_level) {
            case SAFETY_LEVEL_NORMAL:
                system_set_safety_level(SAFETY_LEVEL_NORMAL);
                break;
            case SAFETY_LEVEL_WARNING:
                system_set_safety_level(SAFETY_LEVEL_WARNING);
                break;
            case SAFETY_LEVEL_CRITICAL:
                system_set_safety_level(SAFETY_LEVEL_CRITICAL);
                break;
            case SAFETY_LEVEL_EMERGENCY:
                system_emergency_stop();
                break;
        }
    }
}
```

### **4. Travel Motor Module Integration**

#### **Hardware Connection:**
```
Travel Motor Module (0x03) -> RS485 Bus
├── Motors: 2x DC brushed motors (12V, 100W each)
├── Encoders: Hall effect sensors (60 PPR)
├── Gearbox: Planetary gearbox (20:1 ratio)
└── Wheels: Omni-directional wheels (150mm)
```

#### **Software Integration:**
```c
// Travel Motor Module Integration
typedef struct {
    uint8_t module_id;             // 0x03
    uint8_t status;                // Module status
    motor_module_data_t data;      // Motor data
    uint32_t last_update;          // Last update timestamp
    uint16_t error_count;          // Error count
} motor_module_integration_t;

// Motor control function
void motor_control_set_speed(float target_speed_mps) {
    // Convert m/s to register value (m/s * 100)
    uint16_t target_speed_reg = (uint16_t)(target_speed_mps * 100);
    
    // Write target speed to motor module
    if (modbus_write_single_register(0x03, MOTOR_REG_TARGET_SPEED, target_speed_reg) == 0) {
        // Speed command sent successfully
        system_log(LOG_LEVEL_INFO, "Motor speed set to %.2f m/s", target_speed_mps);
    } else {
        // Error sending speed command
        system_log(LOG_LEVEL_ERROR, "Failed to set motor speed");
    }
}

// Motor monitoring function
void motor_module_monitor(void) {
    // Read motor status
    if (modbus_read_holding_registers(0x03, MOTOR_REG_STATUS, 1, &motor_status) == 0) {
        // Check motor fault
        if (motor_status & MOTOR_STATUS_FAULT) {
            system_set_fault(SYSTEM_FAULT_MOTOR);
        }
        
        // Check motor overload
        if (motor_status & MOTOR_STATUS_OVERLOAD) {
            system_set_warning(SYSTEM_WARNING_MOTOR_OVERLOAD);
        }
        
        // Check motor overtemperature
        if (motor_status & MOTOR_STATUS_OVERTEMP) {
            system_set_warning(SYSTEM_WARNING_MOTOR_OVERTEMP);
        }
    }
}
```

### **5. Dock & Location Module Integration**

#### **Hardware Connection:**
```
Dock & Location Module (0x05) -> RS485 Bus
├── IMU: MPU6050 (I2C)
├── Magnetic Sensors: 2x Hall effect sensors
├── RFID: RFID reader/writer
├── Dock Alignment: 4x infrared sensors
└── LiDAR: RPLIDAR A1M8 (USB 2.0)
```

#### **Software Integration:**
```c
// Dock & Location Module Integration
typedef struct {
    uint8_t module_id;             // 0x05
    uint8_t status;                // Module status
    location_module_data_t data;   // Location data
    uint32_t last_update;          // Last update timestamp
    uint16_t error_count;          // Error count
} location_module_integration_t;

// Location monitoring function
void location_module_monitor(void) {
    // Read location data
    if (modbus_read_holding_registers(0x05, LOCATION_REG_POSITION_X, 3, position_data) == 0) {
        // Update system position
        system_position.x = (int32_t)position_data[0];
        system_position.y = (int32_t)position_data[1];
        system_position.z = (int32_t)position_data[2];
        
        // Check location-based safety
        location_safety_check();
    }
}

// Location-based safety check
void location_safety_check(void) {
    // Read location safety status
    uint16_t location_safety;
    if (modbus_read_holding_registers(0x05, LOCATION_REG_NAVIGATION, 1, &location_safety) == 0) {
        // Check if robot is in restricted area
        if (location_safety & LOCATION_SAFETY_RESTRICTED_AREA) {
            system_set_safety_level(SAFETY_LEVEL_CRITICAL);
        }
        
        // Check if robot is near obstacles
        if (location_safety & LOCATION_SAFETY_OBSTACLE_DETECTED) {
            system_set_safety_level(SAFETY_LEVEL_WARNING);
        }
    }
}
```

---

## 🔄 AUTO-DISCOVERY INTEGRATION

### **Discovery Protocol Implementation:**
```c
// Auto-discovery message structure
typedef struct {
    uint8_t command;               // Discovery command
    uint8_t module_id;             // Module ID
    uint8_t module_type;           // Module type
    char module_name[32];          // Module name
    uint32_t version;              // Firmware version
    uint8_t communication;         // Communication type
} discovery_message_t;

// Discovery commands
#define DISCOVERY_CMD_DISCOVER     0x01      // Discovery command
#define DISCOVERY_CMD_RESPONSE     0x02      // Discovery response
#define DISCOVERY_CMD_REGISTER     0x03      // Register command

// Module types
#define MODULE_TYPE_POWER          0x01
#define MODULE_TYPE_SAFETY         0x02
#define MODULE_TYPE_TRAVEL_MOTOR   0x03
#define MODULE_TYPE_DOCK_LOCATION  0x05
#define MODULE_TYPE_MASTER_CONTROL 0x00
```

### **Discovery Process:**
```c
// Auto-discovery function
void auto_discovery_process(void) {
    // Send discovery broadcast
    discovery_message_t discover_msg;
    discover_msg.command = DISCOVERY_CMD_DISCOVER;
    discover_msg.module_id = 0xFF;  // Broadcast address
    
    // Send broadcast message
    modbus_broadcast_message(&discover_msg, sizeof(discovery_message_t));
    
    // Wait for responses
    uint32_t timeout = system_get_time() + 5000;  // 5 second timeout
    while (system_get_time() < timeout) {
        // Check for discovery responses
        if (modbus_receive_message(&discovery_response, sizeof(discovery_message_t)) > 0) {
            if (discovery_response.command == DISCOVERY_CMD_RESPONSE) {
                // Register discovered module
                register_module(&discovery_response);
            }
        }
    }
}

// Module registration
void register_module(discovery_message_t *module_info) {
    // Check if module is mandatory
    if (is_mandatory_module(module_info->module_type)) {
        // Register mandatory module
        mandatory_modules[module_info->module_id] = *module_info;
        system_log(LOG_LEVEL_INFO, "Mandatory module %s (ID: 0x%02X) registered", 
                  module_info->module_name, module_info->module_id);
    } else {
        // Register optional module
        optional_modules[module_info->module_id] = *module_info;
        system_log(LOG_LEVEL_INFO, "Optional module %s (ID: 0x%02X) registered", 
                  module_info->module_name, module_info->module_id);
    }
}
```

---

## 🛡️ SAFETY INTEGRATION

### **Location-based Safety System:**
```c
// Location-based safety zones
typedef struct {
    uint16_t zone_id;              // Zone ID
    float x_min, x_max;            // X boundaries
    float y_min, y_max;            // Y boundaries
    uint8_t safety_level;          // Safety level for zone
    uint16_t speed_limit;          // Speed limit (m/s * 100)
} safety_zone_t;

// Safety zones configuration
safety_zone_t safety_zones[] = {
    {1, 0.0, 5.0, 0.0, 5.0, SAFETY_LEVEL_NORMAL, 200},    // Normal zone
    {2, 5.0, 10.0, 0.0, 5.0, SAFETY_LEVEL_WARNING, 100},  // Warning zone
    {3, 10.0, 15.0, 0.0, 5.0, SAFETY_LEVEL_CRITICAL, 50}, // Critical zone
    {4, 15.0, 20.0, 0.0, 5.0, SAFETY_LEVEL_EMERGENCY, 0}  // Emergency zone
};

// Location-based safety check
void location_safety_check(void) {
    // Get current position
    position_t current_pos = system_get_position();
    
    // Check which zone robot is in
    for (int i = 0; i < sizeof(safety_zones)/sizeof(safety_zone_t); i++) {
        if (current_pos.x >= safety_zones[i].x_min && 
            current_pos.x <= safety_zones[i].x_max &&
            current_pos.y >= safety_zones[i].y_min && 
            current_pos.y <= safety_zones[i].y_max) {
            
            // Robot is in this zone
            system_set_safety_level(safety_zones[i].safety_level);
            system_set_speed_limit(safety_zones[i].speed_limit);
            break;
        }
    }
}
```

### **Dual-channel E-Stop Integration:**
```c
// E-Stop monitoring
void estop_monitor(void) {
    uint16_t estop_status;
    
    // Read E-Stop status from safety module
    if (modbus_read_holding_registers(0x02, SAFETY_REG_ESTOP_STATUS, 1, &estop_status) == 0) {
        // Check E-Stop activation
        if (estop_status & ESTOP_STATUS_ACTIVATED) {
            // E-Stop activated - emergency stop
            system_emergency_stop();
            
            // Log E-Stop event
            system_log(LOG_LEVEL_CRITICAL, "E-Stop activated");
            
            // Send E-Stop notification
            system_notify_estop();
        }
        
        // Check E-Stop fault
        if (estop_status & ESTOP_STATUS_FAULT) {
            // E-Stop fault detected
            system_set_fault(SYSTEM_FAULT_ESTOP);
            system_log(LOG_LEVEL_ERROR, "E-Stop fault detected");
        }
    }
}
```

---

## 🔧 SYSTEM INTEGRATION

### **System State Machine Integration:**
```c
// System state machine
typedef enum {
    SYSTEM_STATE_IDLE = 0,
    SYSTEM_STATE_MOVE,
    SYSTEM_STATE_DOCK,
    SYSTEM_STATE_FAULT,
    SYSTEM_STATE_ESTOP
} system_state_t;

// Navigation state machine
typedef enum {
    NAV_STATE_IDLE = 0,
    NAV_STATE_NAVIGATING,
    NAV_STATE_POSITIONING,
    NAV_STATE_DOCKING,
    NAV_STATE_UNDOCKING
} navigation_state_t;

// System state management
void system_state_manager(void) {
    // Check mandatory modules status
    uint16_t mandatory_status = check_mandatory_modules();
    
    // Check safety status
    uint8_t safety_level = system_get_safety_level();
    
    // Determine system state
    if (safety_level == SAFETY_LEVEL_EMERGENCY) {
        system_set_state(SYSTEM_STATE_ESTOP);
    } else if (mandatory_status != MANDATORY_MODULES_ALL_OK) {
        system_set_state(SYSTEM_STATE_FAULT);
    } else if (system_get_navigation_state() == NAV_STATE_DOCKING) {
        system_set_state(SYSTEM_STATE_DOCK);
    } else if (system_get_navigation_state() == NAV_STATE_NAVIGATING) {
        system_set_state(SYSTEM_STATE_MOVE);
    } else {
        system_set_state(SYSTEM_STATE_IDLE);
    }
}
```

### **Module Health Monitoring:**
```c
// Module health monitoring
void module_health_monitor(void) {
    // Check each module health
    for (int i = 0; i < MAX_MODULES; i++) {
        if (modules[i].active) {
            // Check module response time
            uint32_t response_time = system_get_time() - modules[i].last_update;
            
            if (response_time > MODULE_TIMEOUT) {
                // Module timeout
                modules[i].error_count++;
                system_log(LOG_LEVEL_WARNING, "Module 0x%02X timeout", modules[i].module_id);
                
                if (modules[i].error_count > MAX_ERROR_COUNT) {
                    // Module failed
                    modules[i].active = false;
                    system_log(LOG_LEVEL_ERROR, "Module 0x%02X failed", modules[i].module_id);
                    
                    // Check if mandatory module failed
                    if (is_mandatory_module(modules[i].module_type)) {
                        system_set_fault(SYSTEM_FAULT_MANDATORY_MODULE);
                    }
                }
            } else {
                // Module responding normally
                modules[i].error_count = 0;
            }
        }
    }
}
```

---

## 📊 INTEGRATION TESTING

### **Unit Testing:**
```c
// Module communication test
void test_module_communication(uint8_t module_id) {
    // Test register read
    uint16_t test_data;
    if (modbus_read_holding_registers(module_id, 0x0001, 1, &test_data) == 0) {
        printf("Module 0x%02X communication OK\n", module_id);
    } else {
        printf("Module 0x%02X communication FAILED\n", module_id);
    }
}

// Module functionality test
void test_module_functionality(uint8_t module_id) {
    switch (module_id) {
        case 0x01:  // Power module
            test_power_module();
            break;
        case 0x02:  // Safety module
            test_safety_module();
            break;
        case 0x03:  // Travel motor module
            test_motor_module();
            break;
        case 0x05:  // Dock & location module
            test_location_module();
            break;
        default:
            printf("Unknown module ID: 0x%02X\n", module_id);
            break;
    }
}
```

### **Integration Testing:**
```c
// System integration test
void system_integration_test(void) {
    printf("Starting system integration test...\n");
    
    // Test auto-discovery
    printf("Testing auto-discovery...\n");
    auto_discovery_process();
    
    // Test mandatory modules
    printf("Testing mandatory modules...\n");
    for (int i = 0; i < MANDATORY_MODULE_COUNT; i++) {
        test_module_communication(mandatory_modules[i].module_id);
        test_module_functionality(mandatory_modules[i].module_id);
    }
    
    // Test safety integration
    printf("Testing safety integration...\n");
    test_safety_integration();
    
    // Test location-based safety
    printf("Testing location-based safety...\n");
    test_location_safety();
    
    printf("System integration test completed.\n");
}
```

---

## 🔧 CONFIGURATION MANAGEMENT

### **System Configuration:**
```c
// System configuration structure
typedef struct {
    uint32_t version;              // Configuration version
    uint8_t system_id;             // System ID
    uint8_t module_count;          // Number of modules
    uint16_t rs485_baudrate;       // RS485 baudrate
    uint8_t rs485_timeout;         // RS485 timeout
    uint8_t rs485_retry_count;     // RS485 retry count
    safety_zone_t safety_zones[8]; // Safety zones
    uint16_t speed_limits[8];      // Speed limits
    uint32_t checksum;             // Configuration checksum
} system_config_t;

// Configuration management
void load_system_config(void) {
    // Load configuration from flash/EEPROM
    if (config_load(&system_config) == 0) {
        // Validate configuration
        if (validate_config(&system_config)) {
            // Apply configuration
            apply_system_config(&system_config);
            system_log(LOG_LEVEL_INFO, "System configuration loaded");
        } else {
            system_log(LOG_LEVEL_ERROR, "Invalid system configuration");
            // Load default configuration
            load_default_config();
        }
    } else {
        system_log(LOG_LEVEL_ERROR, "Failed to load system configuration");
        // Load default configuration
        load_default_config();
    }
}
```

---

## 📋 INTEGRATION CHECKLIST

### **Pre-integration Checklist:**
- [ ] All modules hardware assembled
- [ ] RS485 bus properly terminated
- [ ] Power supply stable
- [ ] Safety systems functional
- [ ] Communication interfaces configured

### **Integration Checklist:**
- [ ] Master Control module operational
- [ ] Auto-discovery functional
- [ ] All mandatory modules detected
- [ ] Communication established
- [ ] Safety systems integrated
- [ ] Location-based safety configured
- [ ] Motor control functional
- [ ] Power monitoring active

### **Post-integration Checklist:**
- [ ] System state machine functional
- [ ] All safety functions working
- [ ] Performance requirements met
- [ ] Error handling functional
- [ ] Logging system active
- [ ] Configuration management working
- [ ] Integration tests passed

---

**Status:** Integration Guide Complete  
**Next Steps:** Create testing procedures và validation documents
