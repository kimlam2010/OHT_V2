# 📋 **CROSS-REFERENCE INDEX - FIRMWARE STRUCTURE**

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-27  
**Dự án:** Firmware Restructure - MISRA C:2012 + AUTOSAR

---

## 🎯 **TỔNG QUAN**

Cross-reference index cho tất cả files trong cấu trúc firmware mới, bao gồm:
- File locations
- Dependencies
- Functions
- Types
- Constants

---

## 📁 **01-CORE - CORE SYSTEM COMPONENTS**

### **01-01-Main/**
- **main.c** → Entry point, system initialization
- **system_init.c** → System initialization logic

### **01-02-State-Machine/**
- **system_state_machine.c** → Main state machine
- **state_transitions.c** → State transition logic

### **01-03-Configuration/**
- **config_system.c** → System configuration
- **system_config.c** → Configuration interface

### **Headers (include/01-CORE/)**
- **system_init.h** → System initialization interface
- **state_transitions.h** → State transition interface
- **system_config.h** → System configuration interface
- **system_state_machine.h** → State machine interface
- **config_system.h** → Configuration system interface

---

## 🔧 **02-HAL - HARDWARE ABSTRACTION LAYER**

### **02-01-Communication/**
- **hal_rs485.c** → RS485 communication
- **hal_network.c** → Network communication
- **hal_usb_debug.c** → USB debug interface

### **02-02-IO-Devices/**
- **hal_gpio.c** → GPIO control
- **hal_led.c** → LED control
- **hal_relay.c** → Relay control

### **02-03-Sensors/**
- **hal_lidar.c** → LiDAR sensor

### **02-04-Safety/**
- **hal_estop.c** → Emergency stop

### **02-05-System/**
- **hal_common.c** → Common HAL functions
- **hal_config_persistence.c** → Configuration persistence
- **hal_ota_update.c** → Over-the-air updates

### **Headers (include/02-HAL/)**
- **hal_rs485.h** → RS485 interface
- **hal_network.h** → Network interface
- **hal_usb_debug.h** → USB debug interface
- **hal_gpio.h** → GPIO interface
- **hal_led.h** → LED interface
- **hal_relay.h** → Relay interface
- **hal_lidar.h** → LiDAR interface
- **hal_estop.h** → E-Stop interface
- **hal_common.h** → Common HAL interface
- **hal_config_persistence.h** → Config persistence interface
- **hal_ota_update.h** → OTA interface

---

## 🧩 **03-MODULES - APPLICATION MODULES**

### **03-01-Motor/**
- **motor_module_handler.c** → Motor control logic

### **03-02-Dock/**
- **dock_module_handler.c** → Docking control logic

### **03-03-Power/**
- **power_module_handler.c** → Power control logic

### **03-04-IO/**
- **di_do_module_handler.c** → Digital IO control
- **io_module_handler.c** → IO management logic

### **03-05-LED/**
- **led_manager.c** → LED control logic

### **Headers (include/03-MODULES/)**
- **motor_module_handler.h** → Motor module interface
- **dock_module_handler.h** → Dock module interface
- **power_module_handler.h** → Power module interface
- **di_do_module_handler.h** → Digital IO interface
- **io_module_handler.h** → IO module interface
- **led_manager.h** → LED manager interface

---

## 🔧 **04-SERVICES - SYSTEM SERVICES**

### **04-01-API/**
- **api_manager.c** → API management
- **api_endpoints.c** → API endpoints
- **http_server.c** → HTTP server

### **04-02-Communication/**
- **communication_manager.c** → Communication management
- **network_manager.c** → Network management
- **websocket_server.c** → WebSocket server

### **04-03-Safety/**
- **safety_manager.c** → Safety management
- **safety_mechanisms.c** → Safety mechanisms

### **04-04-Security/**
- **security_manager.c** → Security management

### **04-05-Diagnostics/**
- **diagnostics_manager.c** → Diagnostics management
- **performance_manager.c** → Performance management

### **Headers (include/04-SERVICES/)**
- **api_manager.h** → API manager interface
- **api_endpoints.h** → API endpoints interface
- **http_server.h** → HTTP server interface
- **communication_manager.h** → Communication manager interface
- **network_manager.h** → Network manager interface
- **websocket_server.h** → WebSocket server interface
- **safety_manager.h** → Safety manager interface
- **safety_mechanisms.h** → Safety mechanisms interface
- **security_manager.h** → Security manager interface
- **diagnostics_manager.h** → Diagnostics manager interface
- **performance_manager.h** → Performance manager interface
- **network_api.h** → Network API interface

---

## 🎛️ **05-CONTROL - CONTROL SYSTEMS**

### **05-01-Control-Loop/**
- **control_loop.c** → Control loop logic

### **05-02-Performance/**
- **performance_metrics.c** → Performance metrics

### **05-03-Module-Management/**
- **module_manager.c** → Module management
- **module_registry.c** → Module registry

### **Headers (include/05-CONTROL/)**
- **control_loop.h** → Control loop interface
- **performance_metrics.h** → Performance metrics interface
- **module_manager.h** → Module manager interface
- **module_registry.h** → Module registry interface

---

## 🛠️ **06-UTILITIES - UTILITY FUNCTIONS**

### **06-01-Mocks/**
- **hal_log_mock.c** → HAL log mock

### **06-02-Backups/**
- **hal_relay_old.c** → Old relay implementation
- **Various .backup files** → Backup files

---

## 🔗 **DEPENDENCY MATRIX**

### **Core Dependencies:**
```
main.c → system_init.c, system_state_machine.c, config_system.c
system_init.c → hal_common.h, system_state_machine.h, config_system.h
state_transitions.c → system_state_machine.h, safety_manager.h
system_config.c → hal_config_persistence.h, config_system.h
```

### **HAL Dependencies:**
```
hal_common.c → hal_common.h
hal_rs485.c → hal_rs485.h, hal_common.h
hal_network.c → hal_network.h, hal_common.h
hal_gpio.c → hal_gpio.h, hal_common.h
hal_led.c → hal_led.h, hal_common.h
hal_relay.c → hal_relay.h, hal_common.h
hal_estop.c → hal_estop.h, hal_common.h
```

### **Module Dependencies:**
```
motor_module_handler.c → motor_module_handler.h, hal_gpio.h
dock_module_handler.c → dock_module_handler.h, hal_gpio.h
power_module_handler.c → power_module_handler.h, hal_gpio.h
io_module_handler.c → io_module_handler.h, hal_gpio.h
led_manager.c → led_manager.h, hal_led.h
```

### **Service Dependencies:**
```
api_manager.c → api_manager.h, communication_manager.h
communication_manager.c → communication_manager.h, network_manager.h
safety_manager.c → safety_manager.h, hal_estop.h
security_manager.c → security_manager.h, api_manager.h
diagnostics_manager.c → diagnostics_manager.h, performance_manager.h
```

### **Control Dependencies:**
```
control_loop.c → control_loop.h, module_manager.h
module_manager.c → module_manager.h, module_registry.h
performance_metrics.c → performance_metrics.h, diagnostics_manager.h
```

---

## 📊 **FUNCTION INDEX**

### **Core Functions:**
- `main()` → Entry point
- `System_Init()` → System initialization
- `System_State_Get()` → Get system state
- `System_State_Set()` → Set system state
- `Config_System_Load()` → Load configuration
- `Config_System_Save()` → Save configuration

### **HAL Functions:**
- `HAL_Common_Init()` → Initialize common HAL
- `HAL_RS485_Init()` → Initialize RS485
- `HAL_Network_Init()` → Initialize network
- `HAL_GPIO_Set_Pin()` → Set GPIO pin
- `HAL_LED_Set_State()` → Set LED state
- `HAL_Relay_Set_State()` → Set relay state
- `HAL_EStop_Get_Status()` → Get E-Stop status

### **Module Functions:**
- `Motor_Module_Init()` → Initialize motor module
- `Dock_Module_Init()` → Initialize dock module
- `Power_Module_Init()` → Initialize power module
- `IO_Module_Init()` → Initialize IO module
- `LED_Module_Init()` → Initialize LED module

### **Service Functions:**
- `API_Manager_Init()` → Initialize API manager
- `Communication_Manager_Init()` → Initialize communication
- `Safety_Manager_Init()` → Initialize safety manager
- `Security_Manager_Init()` → Initialize security manager
- `Diagnostics_Manager_Init()` → Initialize diagnostics

---

## 📋 **TYPE INDEX**

### **Core Types:**
- `system_state_t` → System state enumeration
- `system_event_t` → System event enumeration
- `system_config_t` → System configuration structure
- `system_result_t` → System result enumeration

### **HAL Types:**
- `hal_status_t` → HAL status enumeration
- `hal_config_t` → HAL configuration structure
- `hal_rs485_config_t` → RS485 configuration
- `hal_network_config_t` → Network configuration

### **Module Types:**
- `motor_state_t` → Motor state enumeration
- `dock_state_t` → Dock state enumeration
- `power_state_t` → Power state enumeration
- `io_state_t` → IO state enumeration
- `led_state_t` → LED state enumeration

### **Service Types:**
- `api_result_t` → API result enumeration
- `communication_result_t` → Communication result
- `safety_result_t` → Safety result enumeration
- `security_result_t` → Security result enumeration

---

## 🔢 **CONSTANT INDEX**

### **System Constants:**
- `SYSTEM_STATE_MAX` → Maximum system states
- `SYSTEM_EVENT_MAX` → Maximum system events
- `SYSTEM_CONFIG_MAX_VERSION` → Maximum config version

### **HAL Constants:**
- `HAL_RS485_TIMEOUT_MS` → RS485 timeout
- `HAL_NETWORK_TIMEOUT_MS` → Network timeout
- `HAL_GPIO_MAX_PINS` → Maximum GPIO pins

### **Module Constants:**
- `MOTOR_MAX_SPEED` → Maximum motor speed
- `DOCK_TIMEOUT_MS` → Docking timeout
- `POWER_MAX_LEVEL` → Maximum power level

### **Service Constants:**
- `API_MAX_ENDPOINTS` → Maximum API endpoints
- `COMMUNICATION_MAX_CONNECTIONS` → Max connections
- `SAFETY_TIMEOUT_MS` → Safety timeout

---

## 📚 **DOCUMENTATION INDEX**

### **Project Documentation:**
- `Firmware-Restructure-Plan.md` → Restructure plan
- `Daily-Progress-Report.md` → Progress reports
- `Firmware-Validation-Report.md` → Validation report
- `Coding-Standards.md` → Coding standards

### **README Files:**
- `src/01-CORE/README.md` → Core components guide
- `src/02-HAL/README.md` → HAL guide
- `src/03-MODULES/README.md` → Modules guide

### **Tools:**
- `tools/move_headers.sh` → Header movement script
- `tools/update_includes.sh` → Include update script
- `tools/add_misra_headers.sh` → MISRA header script

---

**Changelog v1.0.0:**
- ✅ Created cross-reference index
- ✅ Listed all files and locations
- ✅ Documented dependencies
- ✅ Indexed functions and types
- ✅ Listed constants and documentation

**🚨 Lưu ý:** Index này được cập nhật khi có thay đổi trong cấu trúc firmware.
