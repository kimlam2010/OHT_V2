# 🔧 **02-HAL - Hardware Abstraction Layer**

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-27  
**Tuân thủ:** MISRA C:2012, AUTOSAR  
**Safety Level:** SIL2

---

## 🎯 **MỤC TIÊU**

Thư mục này chứa Hardware Abstraction Layer (HAL) cung cấp interface thống nhất cho:
- **Communication:** RS485, Network, USB Debug
- **IO Devices:** GPIO, LED, Relay
- **Sensors:** LiDAR, Encoders
- **Safety:** E-Stop, Safety mechanisms
- **System:** Common utilities, OTA, Configuration persistence

---

## 📁 **CẤU TRÚC THƯ MỤC**

```
02-HAL/
├── 02-01-Communication/            # Communication interfaces
│   ├── hal_rs485.c                 # RS485 communication
│   ├── hal_network.c               # Network communication
│   └── hal_usb_debug.c             # USB debug interface
├── 02-02-IO-Devices/               # Input/Output devices
│   ├── hal_gpio.c                  # GPIO control
│   ├── hal_led.c                   # LED control
│   └── hal_relay.c                 # Relay control
├── 02-03-Sensors/                  # Sensor interfaces
│   └── hal_lidar.c                 # LiDAR sensor
├── 02-04-Safety/                   # Safety systems
│   └── hal_estop.c                 # Emergency stop
└── 02-05-System/                   # System utilities
    ├── hal_common.c                # Common HAL functions
    ├── hal_config_persistence.c    # Configuration persistence
    └── hal_ota_update.c            # Over-the-air updates
```

---

## 🔧 **CODING STANDARDS**

### **Naming Convention:**
- **Files:** `hal_module_name.c`
- **Functions:** `HAL_Module_Verb_Noun()`
- **Variables:** `hal_module_verb_noun`

### **MISRA C:2012 Compliance:**
- Rule 8.1: Types shall be explicitly specified
- Rule 8.2: Function types shall be in prototype form
- Rule 8.3: All declarations of the same object or function shall have compatible types

### **Safety Requirements:**
- SIL2 compliance for safety-critical functions
- Error handling for all hardware operations
- Timeout mechanisms for all communications
- Validation of all input parameters

---

## 📋 **DEPENDENCIES**

### **Internal Dependencies:**
- `01-CORE/` - Core system components
- Platform-specific drivers

### **External Dependencies:**
- Hardware datasheets
- Platform SDK
- Communication protocols

---

## 🚨 **CRITICAL FUNCTIONS**

### **Communication:**
- `HAL_RS485_Init()` - Initialize RS485
- `HAL_RS485_Send_Data()` - Send data via RS485
- `HAL_Network_Connect()` - Establish network connection
- `HAL_USB_Debug_Init()` - Initialize USB debug

### **IO Devices:**
- `HAL_GPIO_Set_Pin()` - Set GPIO pin state
- `HAL_LED_Set_State()` - Set LED state
- `HAL_Relay_Set_State()` - Set relay state

### **Safety:**
- `HAL_EStop_Get_Status()` - Get E-Stop status
- `HAL_EStop_Init()` - Initialize E-Stop system

### **System:**
- `HAL_Common_Init()` - Initialize common HAL
- `HAL_Config_Save()` - Save configuration
- `HAL_OTA_Update()` - Perform OTA update

---

## 🔍 **TESTING REQUIREMENTS**

### **Unit Tests:**
- Hardware initialization
- Communication protocols
- Error handling scenarios
- Timeout mechanisms

### **Integration Tests:**
- Hardware integration
- Communication reliability
- Safety system validation

### **Hardware Tests:**
- Loopback tests
- Signal integrity
- EMI/EMC compliance

---

## 📚 **DOCUMENTATION**

### **Required Documentation:**
- Hardware interface specifications
- Communication protocol documentation
- Error codes và handling procedures
- Hardware timing requirements

---

## ⚠️ **SAFETY CONSIDERATIONS**

### **Critical Safety Functions:**
- E-Stop handling must be interrupt-driven
- All safety functions must have watchdog protection
- Hardware failures must be detected và reported

### **Communication Safety:**
- CRC validation for all communications
- Timeout mechanisms for all protocols
- Error recovery procedures

---

**Changelog v1.0.0:**
- ✅ Created HAL structure
- ✅ Defined coding standards
- ✅ Specified dependencies
- ✅ Listed critical functions
- ✅ Added safety considerations

**🚨 Lưu ý:** Tất cả HAL functions phải tuân thủ MISRA C:2012 và có error handling đầy đủ.
