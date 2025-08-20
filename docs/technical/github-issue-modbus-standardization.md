# 🚀 [ENHANCEMENT] Standardize Modbus Register Map for OHT-50 Industrial Robot System

## 📋 **Issue Overview**
**Type:** Enhancement Request  
**Priority:** High  
**Component:** Modbus Register Map  
**Target:** `Docs/modbus_map.md`

---

## 🎯 **Objective**
Standardize the DC Motor module Modbus register map to support **auto-detection** and **multi-module systems** for the OHT-50 industrial robot platform.

---

## 🔍 **Current Analysis**

### **Existing Register Map:**
- ✅ **Motor Control:** Well-structured control registers (0x0000-0x000F)
- ✅ **Digital I/O:** 4 DI / 2 DO configuration
- ✅ **PID Control:** Basic PID parameters
- ❌ **Missing:** Auto-detection registers
- ❌ **Missing:** Module identification
- ❌ **Missing:** System configuration

### **OHT-50 Requirements:**
- **Multi-module support:** Up to 8 modules on single RS485 bus
- **Auto-discovery:** Automatic module detection and identification
- **Standardized interface:** Consistent register layout across all modules
- **Industrial safety:** Fault detection and system status

---

## 📊 **Proposed Changes**

### **1. Add System Registers (0x00F0-0x00FF)**

| Address | Name | Type | R/W | Description | Default | Range |
|---------|------|------|-----|-------------|---------|-------|
| 0x00F0 | Device_ID | uint16 | R/W | Modbus slave address | 4 | 1-247 |
| 0x00F1 | Firmware_Version | uint16 | R | Firmware version (e.g. 0x0101 = v1.01) | 0x0101 | 0x0000-0xFFFF |
| 0x00F2 | System_Status | uint16 | R | Bitfield: system status | 0x0000 | 0x0000-0xFFFF |
| 0x00F3 | System_Error | uint16 | R | Global error code | 0 | 0-65535 |
| 0x00F4 | Reset_Error_Command | uint16 | W | Write 1 to reset all error flags | 0 | 0-1 |
| 0x00F5 | Config_Baudrate | uint16 | R/W | 1=9600, 2=19200, 3=38400,... | 1 | 1-10 |
| 0x00F6 | Config_Parity | uint16 | R/W | 0=None, 1=Even, 2=Odd | 0 | 0-2 |
| 0x00F7 | Module_Type | uint16 | R | Module type (0x0004 = DC Motor) | 0x0004 | 0x0000-0xFFFF |
| 0x00F8 | Module_Name_Low | uint16 | R | Module name (low word) | 0x4443 | "DC" |
| 0x00F9 | Module_Name_High | uint16 | R | Module name (high word) | 0x4D4F | "MO" |
| 0x00FA | Hardware_Version | uint16 | R | Hardware version | 0x0100 | 0x0000-0xFFFF |
| 0x00FB | Serial_Number_Low | uint16 | R | Serial number (low word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FC | Serial_Number_High | uint16 | R | Serial number (high word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FD | Build_Date_Low | uint16 | R | Build date (low word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FE | Build_Date_High | uint16 | R | Build date (high word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FF | Checksum | uint16 | R | Register checksum | 0x0000 | 0x0000-0xFFFF |

### **2. Auto-Detection Sequence**

```c
// Master Module auto-detection logic
1. Scan address range 0x02-0x0F
2. Read 0x00F0 (Device_ID) - confirm address
3. Read 0x00F7 (Module_Type) - identify module type:
   - 0x0002 = Power Module
   - 0x0003 = Stepper Motor  
   - 0x0004 = DC Motor
   - 0x0005 = Sensor Module
   - 0x0006 = DI/DO Module
   - 0x0007 = Analog Input Module
4. Read 0x00F8-0x00F9 (Module_Name) - confirm module name
5. Read 0x00F1 (Firmware_Version) - check compatibility
6. Read 0x00F2 (System_Status) - check health
7. Register module vào system configuration
```

### **3. Enhanced Configuration**

```json
{
  "module_config": {
    "device_id": 4,
    "baudrate": 3,
    "parity": 0,
    "module_type": "DC_MOTOR",
    "module_name": "DCMO",
    "hardware_version": "1.0.0",
    "firmware_version": "1.0.1"
  },
  "motor_config": {
    "motor1": {
      "control_mode": "PID",
      "max_speed": 255,
      "pid_kp": 100,
      "pid_ki": 50,
      "pid_kd": 25
    },
    "motor2": {
      "control_mode": "PID", 
      "max_speed": 255,
      "pid_kp": 100,
      "pid_ki": 50,
      "pid_kd": 25
    }
  },
  "digital_io": {
    "inputs": 4,
    "outputs": 2,
    "logic_level": "24V",
    "isolation": true
  }
}
```

---

## 🎯 **Benefits for OHT-50**

### **1. Auto-Discovery**
- **Plug-and-play:** Modules tự động được nhận diện
- **Dynamic configuration:** Không cần cấu hình thủ công
- **Hot-swapping:** Thay module mà không cần restart

### **2. Multi-Module Support**
- **Scalable:** Hỗ trợ tối đa 8 modules trên 1 bus
- **Address management:** Tự động quản lý địa chỉ
- **Load balancing:** Phân tải tối ưu

### **3. Industrial Safety**
- **Fault detection:** Phát hiện lỗi real-time
- **System monitoring:** Giám sát trạng thái hệ thống
- **Error recovery:** Khôi phục tự động

### **4. Standardization**
- **Consistent interface:** Tất cả modules cùng format
- **Easy integration:** Dễ dàng tích hợp module mới
- **Maintenance friendly:** Bảo trì và debug dễ dàng

---

## 🔧 **Implementation Details**

### **1. Register Definitions**
```c
// System Registers (0x00F0-0x00FF)
#define DC_MOTOR_REG_DEVICE_ID           0x00F0  // 16-bit unsigned
#define DC_MOTOR_REG_FIRMWARE_VERSION    0x00F1  // 16-bit unsigned
#define DC_MOTOR_REG_SYSTEM_STATUS       0x00F2  // 16-bit unsigned
#define DC_MOTOR_REG_SYSTEM_ERROR        0x00F3  // 16-bit unsigned
#define DC_MOTOR_REG_RESET_ERROR_CMD     0x00F4  // 16-bit unsigned
#define DC_MOTOR_REG_CONFIG_BAUDRATE     0x00F5  // 16-bit unsigned
#define DC_MOTOR_REG_CONFIG_PARITY       0x00F6  // 16-bit unsigned
#define DC_MOTOR_REG_MODULE_TYPE         0x00F7  // 16-bit unsigned
#define DC_MOTOR_REG_MODULE_NAME_LOW     0x00F8  // 16-bit unsigned
#define DC_MOTOR_REG_MODULE_NAME_HIGH    0x00F9  // 16-bit unsigned
#define DC_MOTOR_REG_HARDWARE_VERSION    0x00FA  // 16-bit unsigned
#define DC_MOTOR_REG_SERIAL_NUMBER_LOW   0x00FB  // 16-bit unsigned
#define DC_MOTOR_REG_SERIAL_NUMBER_HIGH  0x00FC  // 16-bit unsigned
#define DC_MOTOR_REG_BUILD_DATE_LOW      0x00FD  // 16-bit unsigned
#define DC_MOTOR_REG_BUILD_DATE_HIGH     0x00FE  // 16-bit unsigned
#define DC_MOTOR_REG_CHECKSUM            0x00FF  // 16-bit unsigned
```

### **2. Module Type Codes**
```c
// Module type definitions
#define MODULE_TYPE_POWER        0x0002
#define MODULE_TYPE_STEPPER      0x0003
#define MODULE_TYPE_DC_MOTOR     0x0004
#define MODULE_TYPE_SENSOR       0x0005
#define MODULE_TYPE_DIDO         0x0006
#define MODULE_TYPE_ANALOG       0x0007
```

### **3. System Status Bits**
```c
// System status bit definitions
#define SYSTEM_STATUS_READY      0x0001
#define SYSTEM_STATUS_FAULT      0x0002
#define SYSTEM_STATUS_BUSY       0x0004
#define SYSTEM_STATUS_COMM_ERR   0x0008
#define SYSTEM_STATUS_CALIBRATED 0x0010
#define SYSTEM_STATUS_SAFE       0x0020
```

---

## 📋 **Testing Requirements**

### **1. Auto-Detection Tests**
- [ ] Single module detection
- [ ] Multiple modules detection
- [ ] Address conflict resolution
- [ ] Module type validation
- [ ] Firmware version check

### **2. Communication Tests**
- [ ] Register read/write operations
- [ ] Error handling
- [ ] Timeout handling
- [ ] CRC validation
- [ ] Baudrate configuration

### **3. Integration Tests**
- [ ] OHT-50 system integration
- [ ] Multi-module communication
- [ ] Fault tolerance
- [ ] Performance validation
- [ ] Safety compliance

---

## 🚀 **Deployment Plan**

### **Phase 1: Development (Week 1)**
- [ ] Implement System Registers (0x00F0-0x00FF)
- [ ] Add auto-detection logic
- [ ] Update firmware
- [ ] Unit testing

### **Phase 2: Integration (Week 2)**
- [ ] OHT-50 system integration
- [ ] Multi-module testing
- [ ] Performance optimization
- [ ] Documentation update

### **Phase 3: Validation (Week 3)**
- [ ] Field testing
- [ ] Safety validation
- [ ] User acceptance testing
- [ ] Final documentation

---

## 📚 **Documentation Updates**

### **Files to Update:**
1. `Docs/modbus_map.md` - Main register map
2. `README.md` - Auto-detection guide
3. `examples/` - Code examples
4. `tests/` - Test procedures

### **New Documentation:**
1. `Docs/auto_detection.md` - Auto-detection guide
2. `Docs/multi_module_setup.md` - Multi-module configuration
3. `Docs/troubleshooting.md` - Common issues and solutions

---

## 🤝 **Collaboration**

### **OHT-50 Team:**
- **Hardware:** Orange Pi 5B (RK3588)
- **Communication:** RS485 (UART1)
- **Protocol:** Modbus RTU
- **Modules:** 6 types (Power, Stepper, DC Motor, Sensor, DI/DO, Analog)

### **Integration Points:**
- **Master Module:** Auto-discovery và configuration
- **Firmware:** HAL drivers và communication
- **Backend:** API services và telemetry
- **Frontend:** Configuration UI

---

## 📞 **Contact Information**

**Project:** OHT-50 Industrial Robot System  
**Repository:** https://github.com/KhaLin0401/Driver_2_Motor  
**Issue:** #[Issue Number]  
**Status:** Open for discussion

---

**Labels:** `enhancement`, `modbus`, `auto-detection`, `industrial-robot`, `OHT-50`  
**Milestone:** OHT-50 Integration  
**Assignees:** @KhaLin0401
