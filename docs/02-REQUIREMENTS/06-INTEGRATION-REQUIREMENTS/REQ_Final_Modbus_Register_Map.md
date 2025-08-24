# **FINAL MODBUS REGISTER MAP - OHT-50 TRAVEL MOTOR MODULE**

**📋 Dựa trên System Architecture v2.0 - CHUẨN HÓA CHO AUTO-DETECT**

---

## **🔵 Motor 1 Registers (0x0000-0x000C):**

| Address | Name | Type | R/W | Description | Default | Range |
|---------|------|------|-----|-------------|---------|-------|
| 0x0000 | M1_Control_Mode | uint8 | R/W | 1=ONOFF, 2=LINEAR, 3=PID | 1 | 1-3 |
| 0x0001 | M1_Enable | uint8 | R/W | 0=DISABLE, 1=ENABLE | 0 | 0-1 |
| 0x0002 | M1_Command_Speed | uint8 | R/W | Speed setpoint | 0 | 0-255 |
| 0x0003 | M1_Linear_Input | uint8 | R/W | Linear control input (0-100%) | 0 | 0-100 |
| 0x0004 | M1_Linear_Unit | uint8 | R/W | Linear unit (0-20%) | 5 | 0-20 |
| 0x0005 | M1_Linear_State | uint8 | R/W | 0=DECELERATION, 1=ACCELERATION | 0 | 0-1 |
| 0x0006 | M1_Actual_Speed | uint8 | R | Measured speed | 0 | 0-255 |
| 0x0007 | M1_Direction | uint8 | R/W | 0=Idle, 1=Forward, 2=Reverse | 0 | 0-2 |
| 0x0008 | M1_PID_Kp | uint8 | R/W | PID Kp gain (×100) | 100 | 0-255 |
| 0x0009 | M1_PID_Ki | uint8 | R/W | PID Ki gain (×100) | 10 | 0-255 |
| 0x000A | M1_PID_Kd | uint8 | R/W | PID Kd gain (×100) | 5 | 0-255 |
| 0x000B | M1_Status_Word | uint8 | R | Motor status flags | 0x00 | 0x00-0xFF |
| 0x000C | M1_Error_Code | uint8 | R | Error code if any | 0 | 0-255 |

## **🟢 Motor 2 Registers (0x0010-0x001C):**

| Address | Name | Type | R/W | Description | Default | Range |
|---------|------|------|-----|-------------|---------|-------|
| 0x0010 | M2_Control_Mode | uint8 | R/W | 1=ONOFF, 2=LINEAR, 3=PID | 1 | 1-3 |
| 0x0011 | M2_Enable | uint8 | R/W | 0=OFF, 1=ON | 0 | 0-1 |
| 0x0012 | M2_Command_Speed | uint8 | R/W | Speed setpoint | 0 | 0-255 |
| 0x0013 | M2_Linear_Input | uint8 | R/W | Linear control input (0-100%) | 0 | 0-100 |
| 0x0014 | M2_Linear_Unit | uint8 | R/W | Linear unit (0-20%) | 5 | 0-20 |
| 0x0015 | M2_Linear_State | uint8 | R/W | 0=DECELERATION, 1=ACCELERATION | 0 | 0-1 |
| 0x0016 | M2_Actual_Speed | uint8 | R | Measured speed | 0 | 0-255 |
| 0x0017 | M2_Direction | uint8 | R/W | 0=Idle, 1=Forward, 2=Reverse | 0 | 0-2 |
| 0x0018 | M2_PID_Kp | uint8 | R/W | PID Kp gain (×100) | 100 | 0-255 |
| 0x0019 | M2_PID_Ki | uint8 | R/W | PID Ki gain (×100) | 10 | 0-255 |
| 0x001A | M2_PID_Kd | uint8 | R/W | PID Kd gain (×100) | 5 | 0-255 |
| 0x001B | M2_Status_Word | uint8 | R | Motor status flags | 0x00 | 0x00-0xFF |
| 0x001C | M2_Error_Code | uint8 | R | Error code if any | 0 | 0-255 |

## **🟡 Digital Input Registers (0x0020-0x0024):**

| Address | Name | Type | R/W | Description | Default | Range |
|---------|------|------|-----|-------------|---------|-------|
| 0x0020 | DI_Status_Word | uint16 | R | Bitfield for 4 digital inputs (bit 0: DI1, bit 1: DI2, bit 2: DI3, bit 3: DI4; 1=active) | 0x0000 | 0-0x000F |
| 0x0021 | DI1_Assignment | uint8 | R/W | Function assignment for DI1 (0=none, 1=start M1, 2=stop M1, 3=reverse M1, 4=fault reset, 5=mode switch) | 0 | 0-10 |
| 0x0022 | DI2_Assignment | uint8 | R/W | Function assignment for DI2 (same options as DI1) | 0 | 0-10 |
| 0x0023 | DI3_Assignment | uint8 | R/W | Function assignment for DI3 (same options as DI1) | 0 | 0-10 |
| 0x0024 | DI4_Assignment | uint8 | R/W | Function assignment for DI4 (same options as DI1) | 0 | 0-10 |

## **🟠 Digital Output Registers (0x0030-0x0034):**

| Address | Name | Type | R/W | Description | Default | Range |
|---------|------|------|-----|-------------|---------|-------|
| 0x0030 | DO_Status_Word | uint16 | R | Bitfield for 2 digital outputs (bit 0: DO1, bit 1: DO2; 1=active) | 0x0000 | 0-0x0003 |
| 0x0031 | DO1_Control | uint8 | R/W | Control DO1 (0=off, 1=on) | 0 | 0-1 |
| 0x0032 | DO1_Assignment | uint8 | R/W | Function assignment for DO1 (0=none, 1=running M1, 2=fault M1, 3=speed reached M1, 4=ready) | 0 | 0-10 |
| 0x0033 | DO2_Control | uint8 | R/W | Control DO2 (0=off, 1=on) | 0 | 0-1 |
| 0x0034 | DO2_Assignment | uint8 | R/W | Function assignment for DO2 (same options as DO1) | 0 | 0-10 |

## **🟣 System Registers (0x00F0-0x00FF):**

| Address | Name | Type | R/W | Description | Default | Range |
|---------|------|------|-----|-------------|---------|-------|
| 0x00F0 | Device_ID | uint16 | R/W | Modbus slave address | 4 | 1-247 |
| 0x00F1 | Firmware_Version | uint16 | R | Firmware version (e.g. 0x0101 = v1.01) | 0x0101 | 0x0000-0xFFFF |
| 0x00F2 | System_Status | uint16 | R | Bitfield: system status | 0x0000 | 0x0000-0xFFFF |
| 0x00F3 | System_Error | uint16 | R | Global error code | 0 | 0-65535 |
| 0x00F4 | Reset_Error_Command | uint16 | W | Write 1 to reset all error flags | 0 | 0-1 |
| 0x00F5 | Config_Baudrate | uint16 | R/W | 1=9600, 2=19200, 3=38400,... | 1 | 1-10 |
| 0x00F6 | Config_Parity | uint16 | R/W | 0=None, 1=Even, 2=Odd | 0 | 0-2 |
| 0x00F7 | Module_Type | uint16 | R | Module type (0x0004 = Travel Motor) | 0x0004 | 0x0000-0xFFFF |
| 0x00F8 | Module_Name_Low | uint16 | R | Module name (low word) | 0x5452 | "TR" |
| 0x00F9 | Module_Name_High | uint16 | R | Module name (high word) | 0x564C | "VL" |
| 0x00FA | Hardware_Version | uint16 | R | Hardware version | 0x0100 | 0x0000-0xFFFF |
| 0x00FB | Serial_Number_Low | uint16 | R | Serial number (low word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FC | Serial_Number_High | uint16 | R | Serial number (high word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FD | Build_Date_Low | uint16 | R | Build date (low word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FE | Build_Date_High | uint16 | R | Build date (high word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FF | Checksum | uint16 | R | Register checksum | 0x0000 | 0x0000-0xFFFF |

## **🔵 RESERVED REGISTERS (0x0035-0x00EF):**

| Address Range | Purpose | Status |
|---------------|---------|--------|
| 0x0035-0x003F | Reserved for future DI/DO expansion | Reserved |
| 0x0040-0x004F | Reserved for motor status monitoring | Reserved |
| 0x0050-0x005F | Reserved for configuration parameters | Reserved |
| 0x0060-0x006F | Reserved for fault status | Reserved |
| 0x0070-0x00EF | Reserved for future features | Reserved |
| 0x00F7-0x00FF | Reserved for system expansion | Reserved |

---

## **📋 FUNCTION ASSIGNMENT CODES:**

### **Digital Input Functions (DI1-DI4):**
| Code | Function | Description |
|------|----------|-------------|
| 0 | None | No function assigned |
| 1 | Start_M1 | Start Motor 1 |
| 2 | Stop_M1 | Stop Motor 1 |
| 3 | Reverse_M1 | Reverse Motor 1 direction |
| 4 | Fault_Reset | Reset all fault conditions |
| 5 | Mode_Switch | Switch control mode |

### **Digital Output Functions (DO1-DO2):**
| Code | Function | Description |
|------|----------|-------------|
| 0 | None | No function assigned |
| 1 | Running_M1 | Motor 1 running indicator |
| 2 | Fault_M1 | Motor 1 fault indicator |
| 3 | Speed_Reached_M1 | Motor 1 speed target reached |
| 4 | Ready | System ready indicator |

---

## **📊 TỔNG KẾT REGISTERS:**

- **🔵 Motor 1:** 13 registers (0x0000-0x000C)
- **🟢 Motor 2:** 13 registers (0x0010-0x001C)  
- **🟡 Digital Input:** 5 registers (0x0020-0x0024) - **4 DI channels**
- **🟠 Digital Output:** 5 registers (0x0030-0x0034) - **2 DO channels**
- **🟣 System:** 16 registers (0x00F0-0x00FF) - **FULL AUTO-DETECT SUPPORT**
- **🔵 Reserved:** 204 registers (0x0035-0x00EF)

**🚀 Tổng cộng: 256 registers (0x0000-0x00FF) - CHUẨN CHO AUTO-DETECT**

---

## **🔍 AUTO-DETECT SEQUENCE:**

### **Quét register đầu tiên cho auto-detect:**
1. **0x00F0** - Device_ID (xác nhận địa chỉ)
2. **0x00F7** - Module_Type (xác định loại module = 0x0004)
3. **0x00F8-0x00F9** - Module_Name (xác nhận "DCMO")
4. **0x00F1** - Firmware_Version (kiểm tra version)
5. **0x00F2** - System_Status (kiểm tra trạng thái)

### **Module Type Codes:**
| Code | Module Type | Description |
|------|-------------|-------------|
| 0x0002 | Power Module | Power management |
| 0x0003 | Stepper Motor | Stepper motor control |
| 0x0004 | DC Motor | DC motor control |
| 0x0005 | Sensor Module | Sensor interface |
| 0x0006 | DI/DO Module | Digital I/O |
| 0x0007 | AI Module | Analog input |
| 0x0008+ | Custom Module | Custom modules |

---

**📝 Changelog:**
- v1.0 (2025-01-27): Initial register map
- v1.1 (2025-01-27): Enhanced with GitHub compatibility and extended features
- v1.2 (2025-01-27): Simplified to match actual module specs (4 DI / 2 DO)
- v1.3 (2025-01-27): Standardized to 256 registers (0x0000-0x00FF) for auto-detect compatibility
- v1.4 (2025-01-27): Added complete auto-detect registers (0x00F7-0x00FF) for module identification
