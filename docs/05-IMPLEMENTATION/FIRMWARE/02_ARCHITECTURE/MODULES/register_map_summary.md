# Register Map Summary - OHT-50 Master Module (Phiên bản 2.0)

**Phiên bản:** v2.0  
**Ngày tạo:** 2025-01-28  
**Team:** FW Team  
**Trạng thái:** ⚡ PRODUCTION READY  
**Reference:** https://github.com/hiimshyy/stm32-power-management/blob/main/Docs/modbus_register_map.md

---

## 📊 **TỔNG HỢP REGISTER MAP TẤT CẢ MODULES**

### **Bảng Register Map Tổng Quan:**

| Module | Address | Type | Register Range | Description |
|--------|---------|------|----------------|-------------|
| Power | 0x02 | MODULE_TYPE_POWER (0x0002) | 0x0000-0x00FF | Power management & monitoring (STM32 Power Management) |
| Motor | 0x03 | MODULE_TYPE_MOTOR | 0x0000-0x0011 | Motor control & position |
| IO | 0x04 | MODULE_TYPE_IO | 0x0000-0x0015 | Digital/Analog I/O |
| Dock | 0x05 | MODULE_TYPE_DOCK | 0x0000-0x0012 | Docking mechanism |
| AI | 0x07 | MODULE_TYPE_AI | 0x0000-0x0018 | AI processing & vision |

---

## ⚡ **POWER MODULE (0x02) - REGISTER MAP**

### **Register Map Overview (STM32 Power Management):**

| Register Range | Description | Access | Scale |
|----------------|-------------|--------|-------|
| **0x0100-0x0106** | Modbus Configuration | Read/Write | - |
| **0x0000-0x002B** | DalyBMS Status | Read | V×10, A×10, %×10, mV |
| **0x0030-0x003F** | SK60X Data | Read/Write | V×10, A×10, W×10 |
| **0x0040-0x0048** | INA219 Sensor Values | Read | V×10, A×10, W×10 |
| **0x0049-0x004D** | Relay State | Read/Write | - |
| **0x00F0-0x00FF** | System Registers | Read/Write | - |

### **Key Registers:**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| **Battery Management (DalyBMS)** | | | | |
| BATTERY_VOLTAGE | 0x0000 | uint16_t | Battery pack voltage (V) | 0.1V |
| BATTERY_CURRENT | 0x0001 | uint16_t | Battery pack current (A) | 0.1A |
| BATTERY_SOC | 0x0002 | uint16_t | State of Charge (%) | 0.1% |
| MAX_CELL_V | 0x0003 | uint16_t | Maximum cell voltage (mV) | 1mV |
| MIN_CELL_V | 0x0004 | uint16_t | Minimum cell voltage (mV) | 1mV |
| CELL_DIFF | 0x0007 | uint8_t | Voltage difference between cells (mV) | 1mV |
| TEMPERATURE | 0x0008 | uint8_t | Average temperature (°C) | 1°C |
| CONNECTION_STATUS | 0x0009 | bool | BMS Connection Status | - |
| CHARGE_MOS | 0x000B | bool | Charge MOSFET status | - |
| DISCHARGE_MOS | 0x000C | bool | Discharge MOSFET status | - |
| CELL_VOLTAGE_0-5 | 0x0014-0x0019 | uint16_t[6] | Cell voltages (mV) | 1mV |
| TEMPERATURE_0-1 | 0x001A-0x001B | uint8_t[2] | Temperature sensors (°C) | 1°C |
| CELL_BALANCE_0-5 | 0x001C-0x0021 | bool[6] | Cell balancing status | - |
| CELL_BALANCE_ACTIVE | 0x0022 | bool | Global balancing status | - |
| FAULT_FLAGS | 0x0023 | uint8_t | Fault status flags | - |
| **Charging Control (SK60X)** | | | | |
| V_SET | 0x0030 | uint16_t | Voltage setpoint (V) | 0.1V |
| I_SET | 0x0031 | uint16_t | Current setpoint (A) | 0.1A |
| V_OUT | 0x0032 | uint16_t | Output voltage (V) | 0.1V |
| I_OUT | 0x0033 | uint16_t | Output current (A) | 0.1A |
| P_OUT | 0x0034 | uint16_t | Output power (W) | 0.1W |
| V_IN | 0x0035 | uint16_t | Input voltage (V) | 0.1V |
| I_IN | 0x0036 | uint16_t | Input current (A) | 0.1A |
| TEMP | 0x0037 | uint8_t | Temperature of SK60X (°C) | 1°C |
| STATUS | 0x003B | bool | Operational status | - |
| ON_OFF | 0x003C | bool | Output ON/OFF state | - |
| CHARGE_RELAY | 0x003D | bool | Charge relay status | - |
| CHARGE_STATE | 0x003E | uint16_t | Charge state (0=IDLE, 1=WAITING, 2=CHARGE) | - |
| CHARGE_REQUEST | 0x003F | bool | Charge request | - |
| **Power Distribution (INA219)** | | | | |
| V_OUT_12V | 0x0040 | uint16_t | 12V output voltage (V) | 0.1V |
| I_OUT_12V | 0x0041 | uint16_t | 12V output current (A) | 0.1A |
| P_OUT_12V | 0x0042 | uint16_t | 12V output power (W) | 0.1W |
| V_OUT_5V | 0x0043 | uint16_t | 5V output voltage (V) | 0.1V |
| I_OUT_5V | 0x0044 | uint16_t | 5V output current (A) | 0.1A |
| P_OUT_5V | 0x0045 | uint16_t | 5V output power (W) | 0.1W |
| V_OUT_3V3 | 0x0046 | uint16_t | 3.3V output voltage (V) | 0.1V |
| I_OUT_3V3 | 0x0047 | uint16_t | 3.3V output current (A) | 0.1A |
| P_OUT_3V3 | 0x0048 | uint16_t | 3.3V output power (W) | 0.1W |
| **Relay Control** | | | | |
| RL_12V | 0x0049 | uint8_t | State of 12V relay | - |
| RL_5V | 0x004A | uint8_t | State of 5V relay | - |
| RL_3V3 | 0x004B | uint8_t | State of 3V3 relay | - |
| RL_FAUL | 0x004C | uint8_t | State of Faul relay | - |
| USE_V_THS | 0x004D | uint16_t | Voltage usage threshold | 0.01 |
| **System Information** | | | | |
| DEVICE_ID | 0x00F0 | uint16_t | Device ID (Modbus slave address) | - |
| FIRMWARE_VERSION | 0x00F1 | uint16_t | Firmware version (e.g. 0x0101 = v1.01) | - |
| SYSTEM_STATUS | 0x00F2 | uint16_t | System status (bit field) | - |
| SYSTEM_ERROR | 0x00F3 | uint16_t | System error (global error code) | - |
| RESET_ERROR_CMD | 0x00F4 | uint16_t | Reset error command | - |
| CONFIG_BAUDRATE | 0x00F5 | uint16_t | Config baudrate | - |
| CONFIG_PARITY | 0x00F6 | uint16_t | Config parity | - |
| MODULE_TYPE | 0x00F7 | uint16_t | Module type (0x0002 = Power Module) | - |
| MODULE_NAME_LOW | 0x00F8 | uint16_t | Module name (low word) | - |
| MODULE_NAME_HIGH | 0x00F9 | uint16_t | Module name (high word) | - |
| HARDWARE_VERSION | 0x00FA | uint16_t | Hardware version | - |
| SERIAL_NUMBER_LOW | 0x00FB | uint16_t | Serial number (low word) | - |
| SERIAL_NUMBER_HIGH | 0x00FC | uint16_t | Serial number (high word) | - |
| BUILD_DATE_LOW | 0x00FD | uint16_t | Build date (low word) | - |
| BUILD_DATE_HIGH | 0x00FE | uint16_t | Build date (high word) | - |
| CHECKSUM | 0x00FF | uint16_t | Register checksum | - |

---

## 🚗 **MOTOR MODULE (0x03) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| POSITION | 0x0001 | int32_t | Current position (steps) | 1 step |
| TARGET_POSITION | 0x0003 | int32_t | Target position (steps) | 1 step |
| VELOCITY | 0x0005 | int16_t | Current velocity (steps/s) | 1 step/s |
| TARGET_VELOCITY | 0x0006 | int16_t | Target velocity (steps/s) | 1 step/s |
| ACCELERATION | 0x0007 | uint16_t | Acceleration (steps/s²) | 1 step/s² |
| DECELERATION | 0x0008 | uint16_t | Deceleration (steps/s²) | 1 step/s² |
| CURRENT_LIMIT | 0x0009 | uint16_t | Current limit (mA) | 1mA |
| TEMPERATURE | 0x000A | int16_t | Motor temperature (°C) | 0.1°C |
| FAULT_STATUS | 0x000B | uint16_t | Fault status | - |
| FAULT_CODE | 0x000C | uint16_t | Fault code | - |
| UPTIME | 0x000D | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x000F | uint32_t | Firmware version | - |

---

## 🔌 **IO MODULE (0x04) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| DIGITAL_INPUTS | 0x0001 | uint16_t | Digital inputs state | - |
| DIGITAL_OUTPUTS | 0x0002 | uint16_t | Digital outputs state | - |
| ANALOG_INPUT_0 | 0x0003 | uint16_t | Analog input 0 (mV) | 1mV |
| ANALOG_INPUT_1 | 0x0004 | uint16_t | Analog input 1 (mV) | 1mV |
| ANALOG_INPUT_2 | 0x0005 | uint16_t | Analog input 2 (mV) | 1mV |
| ANALOG_INPUT_3 | 0x0006 | uint16_t | Analog input 3 (mV) | 1mV |
| ANALOG_OUTPUT_0 | 0x0007 | uint16_t | Analog output 0 (mV) | 1mV |
| ANALOG_OUTPUT_1 | 0x0008 | uint16_t | Analog output 1 (mV) | 1mV |
| PWM_FREQUENCY | 0x0009 | uint16_t | PWM frequency (Hz) | 1Hz |
| PWM_DUTY_CYCLE | 0x000A | uint16_t | PWM duty cycle (%) | 0.1% |
| TEMPERATURE | 0x000B | int16_t | Module temperature (°C) | 0.1°C |
| FAULT_STATUS | 0x000C | uint16_t | Fault status | - |
| FAULT_CODE | 0x000D | uint16_t | Fault code | - |
| UPTIME | 0x000E | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x0010 | uint32_t | Firmware version | - |

---

## 🎯 **DOCK MODULE (0x05) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| POSITION | 0x0001 | int32_t | Current position (mm) | 0.1mm |
| TARGET_POSITION | 0x0003 | int32_t | Target position (mm) | 0.1mm |
| VELOCITY | 0x0005 | int16_t | Current velocity (mm/s) | 0.1mm/s |
| TARGET_VELOCITY | 0x0006 | int16_t | Target velocity (mm/s) | 0.1mm/s |
| FORCE_SENSOR | 0x0007 | uint16_t | Force sensor reading (N) | 0.1N |
| PROXIMITY_SENSOR | 0x0008 | uint16_t | Proximity sensor (mm) | 0.1mm |
| DOCKING_STATE | 0x0009 | uint8_t | Docking state (0=IDLE, 1=APPROACHING, 2=DOCKED, 3=UNDOCKING) | - |
| LOCK_STATUS | 0x000A | bool | Lock mechanism status | - |
| TEMPERATURE | 0x000B | int16_t | Module temperature (°C) | 0.1°C |
| FAULT_STATUS | 0x000C | uint16_t | Fault status | - |
| FAULT_CODE | 0x000D | uint16_t | Fault code | - |
| UPTIME | 0x000E | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x0010 | uint32_t | Firmware version | - |

---

## 🤖 **AI MODULE (0x07) - REGISTER MAP**

| Register | Address | Type | Description | Resolution |
|----------|---------|------|-------------|------------|
| STATUS | 0x0000 | uint16_t | Module status | - |
| PROCESSING_STATE | 0x0001 | uint8_t | Processing state (0=IDLE, 1=PROCESSING, 2=COMPLETE, 3=ERROR) | - |
| OBJECT_COUNT | 0x0002 | uint8_t | Number of detected objects | - |
| OBJECT_0_X | 0x0003 | uint16_t | Object 0 X position (pixels) | 1px |
| OBJECT_0_Y | 0x0004 | uint16_t | Object 0 Y position (pixels) | 1px |
| OBJECT_0_CONFIDENCE | 0x0005 | uint16_t | Object 0 confidence (%) | 0.1% |
| OBJECT_1_X | 0x0006 | uint16_t | Object 1 X position (pixels) | 1px |
| OBJECT_1_Y | 0x0007 | uint16_t | Object 1 Y position (pixels) | 1px |
| OBJECT_1_CONFIDENCE | 0x0008 | uint16_t | Object 1 confidence (%) | 0.1% |
| OBJECT_2_X | 0x0009 | uint16_t | Object 2 X position (pixels) | 1px |
| OBJECT_2_Y | 0x000A | uint16_t | Object 2 Y position (pixels) | 1px |
| OBJECT_2_CONFIDENCE | 0x000B | uint16_t | Object 2 confidence (%) | 0.1% |
| PROCESSING_TIME | 0x000C | uint16_t | Processing time (ms) | 1ms |
| FRAME_RATE | 0x000D | uint16_t | Frame rate (fps) | 0.1fps |
| TEMPERATURE | 0x000E | int16_t | Module temperature (°C) | 0.1°C |
| FAULT_STATUS | 0x000F | uint16_t | Fault status | - |
| FAULT_CODE | 0x0010 | uint16_t | Fault code | - |
| UPTIME | 0x0011 | uint32_t | Module uptime (hours) | 1h |
| VERSION | 0x0013 | uint32_t | Firmware version | - |

---

## 🔧 **COMMON REGISTER PATTERNS**

### **System Registers (0x00F0-0x00FF):**
Tất cả modules đều có system registers để auto-detection:

| Register | Address | Description | Access |
|----------|---------|-------------|--------|
| DEVICE_ID | 0x00F0 | Device ID (Modbus slave address) | Read |
| FIRMWARE_VERSION | 0x00F1 | Firmware version | Read |
| SYSTEM_STATUS | 0x00F2 | System status (bit field) | Read |
| SYSTEM_ERROR | 0x00F3 | System error (global error code) | Read |
| RESET_ERROR_CMD | 0x00F4 | Reset error command | Write |
| CONFIG_BAUDRATE | 0x00F5 | Config baudrate | Read/Write |
| CONFIG_PARITY | 0x00F6 | Config parity | Read/Write |
| MODULE_TYPE | 0x00F7 | Module type | Read |
| MODULE_NAME_LOW | 0x00F8 | Module name (low word) | Read |
| MODULE_NAME_HIGH | 0x00F9 | Module name (high word) | Read |
| HARDWARE_VERSION | 0x00FA | Hardware version | Read |
| SERIAL_NUMBER_LOW | 0x00FB | Serial number (low word) | Read |
| SERIAL_NUMBER_HIGH | 0x00FC | Serial number (high word) | Read |
| BUILD_DATE_LOW | 0x00FD | Build date (low word) | Read |
| BUILD_DATE_HIGH | 0x00FE | Build date (high word) | Read |
| CHECKSUM | 0x00FF | Register checksum | Read |

### **Module Types:**
```c
#define MODULE_TYPE_POWER    0x0002  // Power Module
#define MODULE_TYPE_MOTOR    0x0003  // Motor Module
#define MODULE_TYPE_IO       0x0004  // IO Module
#define MODULE_TYPE_DOCK     0x0005  // Dock Module
#define MODULE_TYPE_AI       0x0007  // AI Module
```

---

## 📊 **REGISTER ACCESS PATTERNS**

### **Read Access:**
- **Single Register:** Function Code 0x03 (Read Holding Registers)
- **Multiple Registers:** Function Code 0x03 with count > 1
- **Response:** Register values + CRC

### **Write Access:**
- **Single Register:** Function Code 0x06 (Write Single Register)
- **Multiple Registers:** Function Code 0x10 (Write Multiple Registers)
- **Response:** Echo of written values + CRC

### **Error Handling:**
- **Exception Code 0x01:** Illegal Function
- **Exception Code 0x02:** Illegal Data Address
- **Exception Code 0x03:** Illegal Data Value
- **Exception Code 0x04:** Slave Device Failure

---

## 📚 **REFERENCES**

### **Implementation Files:**
- `firmware/include/power_module_handler.h` - Power module header
- `firmware/src/app/power_module_handler.c` - Power module implementation
- `firmware/tests/app/test_power_module_handler.c` - Power module tests

### **External References:**
- [STM32 Power Management](https://github.com/hiimshyy/stm32-power-management) - Reference implementation
- Modbus RTU Protocol Specification
- DalyBMS Protocol Documentation
- SK60X Charger Documentation
- INA219 Sensor Documentation

---

## 📝 **CHANGELOG**

### **v2.0 (2025-01-28):**
- ✅ Updated Power Module register map to STM32 Power Management
- ✅ Added DalyBMS battery management registers
- ✅ Added SK60X charging control registers
- ✅ Added INA219 power distribution registers
- ✅ Added comprehensive system registers
- ✅ Updated register access patterns
- ✅ Added module type definitions
- ✅ Added error handling patterns

### **v1.0 (2025-01-28):**
- ✅ Added basic register map summary
- ✅ Added Power Module registers
- ✅ Added Motor Module registers
- ✅ Added IO Module registers
- ✅ Added Dock Module registers
- ✅ Added AI Module registers

---

**🚨 Lưu ý:** Register map phải tuân thủ đúng protocol và scaling factors để đảm bảo compatibility.
