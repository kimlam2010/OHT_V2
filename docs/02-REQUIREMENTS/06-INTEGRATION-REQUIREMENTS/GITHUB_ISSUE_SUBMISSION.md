# 🚨 [CRITICAL] Auto-Detect Compatibility Issue - OHT-50 Integration

## 📋 **Issue Overview**
**Type:** Critical Bug  
**Priority:** High  
**Component:** Modbus Register Map  
**Target:** `Docs/modbus_map.md`  
**Repository:** [KhaLin0401/Driver_2_Motor](https://github.com/KhaLin0401/Driver_2_Motor)

---

## 🎯 **Objective**
Update the Modbus register map to support **auto-detection** and **multi-module systems** for OHT-50 industrial robot platform integration.

---

## 🔍 **Current Analysis**

### **Existing Register Map Issues:**
- ✅ **Motor Control:** Well-structured control registers (0x0010-0x002D)
- ✅ **Digital I/O:** 4 DI / 2 DO configuration
- ✅ **PID Control:** Basic PID parameters
- ❌ **Missing:** Auto-detection registers (0x00F7-0x00FF)
- ❌ **Missing:** Module identification
- ❌ **Missing:** System configuration at standard addresses
- ❌ **Address Conflict:** System registers at 0x0000-0x0006 instead of 0x0100-0x0106

### **OHT-50 Requirements:**
- **Multi-module support:** Up to 8 modules on single RS485 bus
- **Auto-discovery:** Automatic module detection and identification
- **Standardized interface:** Consistent register layout across all modules
- **Industrial safety:** Fault detection and system status

---

## 🚨 **CRITICAL ISSUES**

### **1. 🔴 Auto-Detect Failure**
**Problem:** Master module cannot automatically detect this module
**Impact:** Manual configuration required, no plug-and-play capability
**Solution:** Add auto-detect registers (0x00F7-0x00FF)

### **2. 🔴 Module Identification Missing**
**Problem:** No way to identify module type and version
**Impact:** Cannot distinguish between different module types
**Solution:** Add Module_Type and Module_Name registers

### **3. 🔴 No Validation Mechanism**
**Problem:** No checksum for data integrity validation
**Impact:** Cannot verify register data integrity
**Solution:** Add checksum register (0x00FF)

### **4. 🟡 Address Conflict**
**Problem:** System registers at wrong addresses (0x0000-0x0006)
**Impact:** Incompatible with OHT-50 standard
**Solution:** Move to standard addresses (0x0100-0x0106)

---

## 📊 **Proposed Changes**

### **1. Move System Registers (0x0100-0x0106)**

| Address | Name | Type | R/W | Description | Default | Range |
|---------|------|------|-----|-------------|---------|-------|
| 0x0100 | Device_ID | uint16 | R/W | Modbus slave address | 3 | 1-247 |
| 0x0101 | Firmware_Version | uint16 | R | Firmware version (e.g. 0x0101 = v1.01) | 0x0101 | 0x0000-0xFFFF |
| 0x0102 | System_Status | uint16 | R | Bitfield: system status | 0x0000 | 0x0000-0xFFFF |
| 0x0103 | System_Error | uint16 | R | Global error code | 0 | 0-65535 |
| 0x0104 | Reset_Error_Command | uint16 | W | Write 1 to reset all error flags | 0 | 0-1 |
| 0x0105 | Config_Baudrate | uint16 | R/W | 1=9600, 2=19200, 3=38400,... | 1 | 1-10 |
| 0x0106 | Config_Parity | uint16 | R/W | 0=None, 1=Even, 2=Odd | 0 | 0-2 |

### **2. Add Auto-Detect Registers (0x00F7-0x00FF)**

| Address | Name | Type | R/W | Description | Default | Range |
|---------|------|------|-----|-------------|---------|-------|
| 0x00F7 | Module_Type | uint16 | R | Module type (0x0004 = Travel Motor) | 0x0004 | 0x0000-0xFFFF |
| 0x00F8 | Module_Name_Low | uint16 | R | Module name (low word) | 0x5452 | "TR" |
| 0x00F9 | Module_Name_High | uint16 | R | Module name (high word) | 0x564C | "VL" |
| 0x00FA | Hardware_Version | uint16 | R | Hardware version | 0x0100 | 0x0000-0xFFFF |
| 0x00FB | Serial_Number_Low | uint16 | R | Serial number (low word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FC | Serial_Number_High | uint16 | R | Serial number (high word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FD | Build_Date_Low | uint16 | R | Build date (low word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FE | Build_Date_High | uint16 | R | Build date (high word) | 0x0000 | 0x0000-0xFFFF |
| 0x00FF | Checksum | uint16 | R | Register checksum | 0x0000 | 0x0000-0xFFFF |

### **3. Update Motor Control Registers**

#### **Add LINEAR Control Mode:**
```
Current: 1=ONOFF, 2=PID
Proposed: 1=ONOFF, 2=LINEAR, 3=PID
```

#### **Add Linear Control Registers:**
```
0x0013: M1_Linear_Input (0-100%)
0x0014: M1_Linear_Unit (0-20%)
0x0015: M1_Linear_State (0=DECELERATION, 1=ACCELERATION)
```

### **4. Standardize Register Layout**

#### **Current Layout:**
```
0x0000-0x0006: System Registers (WRONG ADDRESS)
0x0010-0x001D: Motor 1
0x0020-0x002D: Motor 2
0x0030-0x0034: Digital Input
0x0040-0x0044: Digital Output
```

#### **Proposed Layout:**
```
0x0000-0x000C: Motor 1 (13 registers)
0x0010-0x001C: Motor 2 (13 registers)
0x0020-0x0024: Digital Input (5 registers)
0x0030-0x0034: Digital Output (5 registers)
0x0035-0x00F6: Reserved (194 registers)
0x0100-0x0106: System Registers (7 registers) - BASIC SYSTEM
0x00F7-0x00FF: Auto-Detect Registers (9 registers) - MODULE IDENTIFICATION
```

---

## 🔧 **Implementation Plan**

### **Phase 1: System Registers Migration**
1. **Move System registers** from 0x0000-0x0006 to 0x0100-0x0106
2. **Update all references** to system registers
3. **Test system functionality**
4. **Update documentation**

### **Phase 2: Auto-Detect Implementation**
1. **Add registers 0x00F7-0x00FF**
2. **Implement Module_Type = 0x0004**
3. **Implement Module_Name = "TRVL"**
4. **Add checksum calculation**
5. **Test auto-detection**

### **Phase 3: Control Mode Enhancement**
1. **Add LINEAR control mode**
2. **Implement Linear Input/Unit/State**
3. **Update control logic**
4. **Test linear control**

### **Phase 4: Integration Testing**
1. **OHT-50 Master Module integration test**
2. **Auto-discovery validation**
3. **Control mode testing**
4. **Error handling validation**

---

## 📋 **Testing Requirements**

### **Auto-Detect Test:**
```c
// Test sequence
1. Read 0x0100 (Device_ID)
2. Read 0x00F7 (Module_Type) - should return 0x0004
3. Read 0x00F8-0x00F9 (Module_Name) - should return "TRVL"
4. Read 0x0101 (Firmware_Version)
5. Read 0x0102 (System_Status)
6. Calculate and verify 0x00FF (Checksum)
```

### **Compatibility Test:**
```c
// OHT-50 Master Module Test
1. Auto-discovery scan
2. Module identification
3. Register mapping validation
4. Control mode testing
5. Error handling validation
```

---

## 🎯 **Success Criteria**

### **Auto-Detect Success:**
- ✅ Master module can automatically detect this module
- ✅ Module type correctly identified as 0x0004
- ✅ Module name correctly identified as "TRVL"
- ✅ Firmware version readable
- ✅ System status accessible
- ✅ Checksum validation working

### **Control Compatibility:**
- ✅ LINEAR control mode functional
- ✅ Linear Input/Unit/State registers working
- ✅ PID control mode maintained
- ✅ Digital I/O functionality preserved
- ✅ Error handling improved

---

## 📚 **References**

### **OHT-50 Requirements:**
- [FINAL MODBUS REGISTER MAP](docs/02-REQUIREMENTS/06-INTEGRATION-REQUIREMENTS/REQ_Final_Modbus_Register_Map.md)
- [System Architecture](docs/02-REQUIREMENTS/01-SYSTEM-REQUIREMENTS/REQ_System_Architecture.md)
- [RS485 Integration](docs/02-REQUIREMENTS/03-FIRMWARE-REQUIREMENTS/REQ_RS485_System_Overview.md)

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

## 🚀 **Expected Benefits**

### **For OHT-50 Integration:**
- ✅ **Plug-and-play capability**
- ✅ **Automatic module detection**
- ✅ **Standardized interface**
- ✅ **Improved reliability**
- ✅ **Better error handling**

### **For Driver_2_Motor:**
- ✅ **Industrial compatibility**
- ✅ **Multi-module support**
- ✅ **Professional standards**
- ✅ **Market expansion**
- ✅ **Quality improvement**

---

## 📞 **Contact Information**

**OHT-50 Project Team**  
**Repository:** [OHT-50 Master Module](https://github.com/your-org/OHT-50)  
**Documentation:** [OHT-50 Docs](docs/)  
**Integration Guide:** [RS485 Integration](docs/02-REQUIREMENTS/03-FIRMWARE-REQUIREMENTS/REQ_RS485_System_Overview.md)

---

**📝 Note:** This issue is critical for OHT-50 industrial robot platform integration. Please prioritize implementation to ensure compatibility with multi-module industrial systems.

---

**🏷️ Labels:** `critical`, `auto-detect`, `modbus`, `compatibility`, `oht-50`, `industrial`

---

## 📋 **Implementation Checklist**

### **Phase 1: System Registers Migration**
- [ ] Move Device_ID from 0x0000 to 0x0100
- [ ] Move Firmware_Version from 0x0001 to 0x0101
- [ ] Move System_Status from 0x0002 to 0x0102
- [ ] Move System_Error from 0x0003 to 0x0103
- [ ] Move Reset_Error_Command from 0x0004 to 0x0104
- [ ] Move Config_Baudrate from 0x0005 to 0x0105
- [ ] Move Config_Parity from 0x0006 to 0x0106
- [ ] Update all code references
- [ ] Test system functionality
- [ ] Update documentation

### **Phase 2: Auto-Detect Implementation**
- [ ] Add Module_Type register at 0x00F7
- [ ] Add Module_Name_Low register at 0x00F8
- [ ] Add Module_Name_High register at 0x00F9
- [ ] Add Hardware_Version register at 0x00FA
- [ ] Add Serial_Number_Low register at 0x00FB
- [ ] Add Serial_Number_High register at 0x00FC
- [ ] Add Build_Date_Low register at 0x00FD
- [ ] Add Build_Date_High register at 0x00FE
- [ ] Add Checksum register at 0x00FF
- [ ] Implement checksum calculation
- [ ] Test auto-detection

### **Phase 3: Control Mode Enhancement**
- [ ] Add LINEAR control mode (mode 2)
- [ ] Update PID control mode to mode 3
- [ ] Add Linear Input registers
- [ ] Add Linear Unit registers
- [ ] Add Linear State registers
- [ ] Update control logic
- [ ] Test linear control

### **Phase 4: Integration Testing**
- [ ] OHT-50 Master Module integration test
- [ ] Auto-discovery validation
- [ ] Register mapping validation
- [ ] Control mode testing
- [ ] Error handling validation
- [ ] Performance testing
- [ ] Documentation update

---

**📅 Timeline:** 4 weeks total
- **Week 1:** System Registers Migration
- **Week 2:** Auto-Detect Implementation
- **Week 3:** Control Mode Enhancement
- **Week 4:** Integration Testing & Documentation

## 📊 **BẢNG MAPPING REGISTER MODBUS - TRAVEL MOTOR MODULE (UPDATED):**

| **Register Address** | **Tên Register** | **Loại** | **Đơn vị** | **Mô tả** | **Access** | **Trạng thái** |
|---------------------|------------------|----------|------------|-----------|------------|----------------|
| **0x0000** | Motor 1 Control Mode | Holding | - | Chế độ điều khiển động cơ 1 (1=ONOFF, 2=LINEAR, 3=PID) | R/W | ✅ Hoàn thành |
| **0x0001** | Motor 1 Enable | Holding | - | Bật/tắt động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0002** | Motor 1 Command Speed | Holding | % | Tốc độ động cơ 1 (0-255) | R/W | ✅ Hoàn thành |
| **0x0003** | Motor 1 Linear Input | Holding | % | Input tuyến tính động cơ 1 (0-100%) | R/W | ✅ Hoàn thành |
| **0x0004** | Motor 1 Linear Unit | Holding | % | Đơn vị tuyến tính động cơ 1 (0-20%) | R/W | ✅ Hoàn thành |
| **0x0005** | Motor 1 Linear State | Holding | - | Trạng thái tuyến tính động cơ 1 (0=DECELERATION, 1=ACCELERATION) | R/W | ✅ Hoàn thành |
| **0x0006** | Motor 1 Actual Speed | Input | % | Tốc độ thực tế động cơ 1 | R | ✅ Hoàn thành |
| **0x0007** | Motor 1 Direction | Holding | - | Hướng động cơ 1 (0=Idle, 1=Forward, 2=Reverse) | R/W | ✅ Hoàn thành |
| **0x0008** | Motor 1 PID Kp | Holding | ×100 | Hệ số Kp của PID động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0009** | Motor 1 PID Ki | Holding | ×100 | Hệ số Ki của PID động cơ 1 | R/W | ✅ Hoàn thành |
| **0x000A** | Motor 1 PID Kd | Holding | ×100 | Hệ số Kd của PID động cơ 1 | R/W | ✅ Hoàn thành |
| **0x000B** | Motor 1 Status Word | Input | - | Trạng thái động cơ 1 (bit field) | R | ✅ Hoàn thành |
| **0x000C** | Motor 1 Error Code | Input | - | Mã lỗi động cơ 1 | R | ✅ Hoàn thành |
| **0x0010** | Motor 2 Control Mode | Holding | - | Chế độ điều khiển động cơ 2 (1=ONOFF, 2=LINEAR, 3=PID) | R/W | ✅ Hoàn thành |
| **0x0011** | Motor 2 Enable | Holding | - | Bật/tắt động cơ 2 | R/W | ✅ Hoàn thành |
| **0x0012** | Motor 2 Command Speed | Holding | % | Tốc độ động cơ 2 (0-255) | R/W | ✅ Hoàn thành |
| **0x0013** | Motor 2 Linear Input | Holding | % | Input tuyến tính động cơ 2 (0-100%) | R/W | ✅ Hoàn thành |
| **0x0014** | Motor 2 Linear Unit | Holding | % | Đơn vị tuyến tính động cơ 2 (0-20%) | R/W | ✅ Hoàn thành |
| **0x0015** | Motor 2 Linear State | Holding | - | Trạng thái tuyến tính động cơ 2 (0=DECELERATION, 1=ACCELERATION) | R/W | ✅ Hoàn thành |
| **0x0016** | Motor 2 Actual Speed | Input | % | Tốc độ thực tế động cơ 2 | R | ✅ Hoàn thành |
| **0x0017** | Motor 2 Direction | Holding | - | Hướng động cơ 2 (0=Idle, 1=Forward, 2=Reverse) | R/W | ✅ Hoàn thành |
| **0x0018** | Motor 2 PID Kp | Holding | ×100 | Hệ số Kp của PID động cơ 2 | R/W | ✅ Hoàn thành |
| **0x0019** | Motor 2 PID Ki | Holding | ×100 | Hệ số Ki của PID động cơ 2 | R/W | ✅ Hoàn thành |
| **0x001A** | Motor 2 PID Kd | Holding | ×100 | Hệ số Kd của PID động cơ 2 | R/W | ✅ Hoàn thành |
| **0x001B** | Motor 2 Status Word | Input | - | Trạng thái động cơ 2 (bit field) | R | ✅ Hoàn thành |
| **0x001C** | Motor 2 Error Code | Input | - | Mã lỗi động cơ 2 | R | ✅ Hoàn thành |
| **0x0020** | Digital Input Status | Input | - | Trạng thái đầu vào số (bit field) | R | ✅ Hoàn thành |
| **0x0021** | DI1 Assignment | Holding | - | Gán chức năng cho DI1 | R/W | ✅ Hoàn thành |
| **0x0022** | DI2 Assignment | Holding | - | Gán chức năng cho DI2 | R/W | ✅ Hoàn thành |
| **0x0023** | DI3 Assignment | Holding | - | Gán chức năng cho DI3 | R/W | ✅ Hoàn thành |
| **0x0024** | DI4 Assignment | Holding | - | Gán chức năng cho DI4 | R/W | ✅ Hoàn thành |
| **0x0030** | Digital Output Status | Input | - | Trạng thái đầu ra số (bit field) | R | ✅ Hoàn thành |
| **0x0031** | DO1 Control | Holding | - | Điều khiển DO1 (0=Off, 1=On) | R/W | ✅ Hoàn thành |
| **0x0032** | DO1 Assignment | Holding | - | Gán chức năng cho DO1 | R/W | ✅ Hoàn thành |
| **0x0033** | DO2 Control | Holding | - | Điều khiển DO2 (0=Off, 1=On) | R/W | ✅ Hoàn thành |
| **0x0034** | DO2 Assignment | Holding | - | Gán chức năng cho DO2 | R/W | ✅ Hoàn thành |
| **0x0100** | Device ID | Input | - | ID thiết bị (Modbus slave address) | R | ✅ Hoàn thành |
| **0x0101** | Firmware Version | Input | - | Phiên bản firmware | R | ✅ Hoàn thành |
| **0x0102** | System Status | Input | - | Trạng thái hệ thống (bit field) | R | ✅ Hoàn thành |
| **0x0103** | System Error | Input | - | Lỗi hệ thống (global error code) | R | ✅ Hoàn thành |
| **0x0104** | Reset Error Command | Holding | - | Lệnh reset lỗi | W | ✅ Hoàn thành |
| **0x0105** | Config Baudrate | Holding | - | Cấu hình baudrate | R/W | ✅ Hoàn thành |
| **0x0106** | Config Parity | Holding | - | Cấu hình parity | R/W | ✅ Hoàn thành |
| **0x00F7** | Module Type | Input | - | Loại module (0x0004 = Travel Motor) | R | ✅ Hoàn thành |
| **0x00F8** | Module Name Low | Input | - | Tên module (word thấp) | R | ✅ Hoàn thành |
| **0x00F9** | Module Name High | Input | - | Tên module (word cao) | R | ✅ Hoàn thành |
| **0x00FA** | Hardware Version | Input | - | Phiên bản phần cứng | R | ✅ Hoàn thành |
| **0x00FB** | Serial Number Low | Input | - | Số serial (word thấp) | R | ✅ Hoàn thành |
| **0x00FC** | Serial Number High | Input | - | Số serial (word cao) | R | ✅ Hoàn thành |
| **0x00FD** | Build Date Low | Input | - | Ngày build (word thấp) | R | ✅ Hoàn thành |
| **0x00FE** | Build Date High | Input | - | Ngày build (word cao) | R | ✅ Hoàn thành |
| **0x00FF** | Checksum | Input | - | Checksum register | R | ✅ Hoàn thành |

**Chú thích:**
- **R/W:** Read/Write - Đọc/Ghi
- **R:** Read Only - Chỉ đọc
- **W:** Write Only - Chỉ ghi
- **Input:** Register đầu vào (từ cảm biến)
- **Holding:** Register điều khiển (có thể ghi)
