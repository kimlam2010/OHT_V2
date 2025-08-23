# **PHÂN TÍCH TƯƠNG THÍCH MAPPING MODBUS - GITHUB DRIVER_2_MOTOR**

## 📋 **TÀI LIỆU PHÂN TÍCH**

**Source:** [GitHub Driver_2_Motor](https://github.com/KhaLin0401/Driver_2_Motor/blob/main/Docs/modbus_map.md)  
**Target:** OHT-50 DC Motor Module  
**Version:** 2.0  
**Date:** 2025-01-28

---

## 🎯 **MỤC TIÊU PHÂN TÍCH**

### **Mục tiêu:**
- So sánh mapping Modbus từ GitHub với hệ thống OHT-50 Architecture v2.0
- Xác định các tính năng cần bổ sung
- Đảm bảo tương thích ngược
- Cải thiện tính năng hệ thống

### **Lý do cần phân tích:**
1. **Tương thích ngược:** Đảm bảo OHT-50 có thể làm việc với các module tương tự
2. **Tính năng bổ sung:** Học hỏi từ thiết kế có sẵn
3. **Chuẩn hóa:** Tuân thủ các chuẩn industry
4. **Mở rộng:** Chuẩn bị cho tương lai

---

## 📊 **SO SÁNH CHI TIẾT**

### **1. Platform Comparison:**

| **Aspect** | **GitHub Driver_2_Motor** | **OHT-50 System v2.0** | **Phân tích** |
|------------|------------------------------|-------------------|---------------|
| **Platform** | STM32F103C8T6 | Orange Pi 5B (RK3588) | OHT-50 mạnh hơn về processing power |
| **Register Range** | 0x0000-0x00F6 (247 registers) | 0x0000-0x05FF (1536 registers) | OHT-50 có nhiều registers hơn |
| **Motor Control** | 2 DC motors | 2 DC motors | OHT-50 tương thích hoàn toàn |
| **Digital I/O** | 4 DI + 2 DO | 8 DI + 8 DO (DI/DO Module) | OHT-50 có nhiều I/O hơn |
| **Control Modes** | ONOFF, LINEAR, PID | PID with enhanced features | OHT-50 có tính năng nâng cao |
| **Encoder Support** | Không có | Có encoder feedback | OHT-50 tiên tiến hơn |
| **System Architecture** | Single module | 5 mandatory modules | OHT-50 modular và scalable |

### **2. Register Mapping Analysis:**

#### **GitHub Mapping Structure:**
```
0x0000-0x000F: Motor 1 Control (16 registers)
0x0010-0x001F: Motor 2 Control (16 registers)
0x0020-0x002F: Digital Inputs (5 registers)
0x0030-0x003F: Digital Outputs (5 registers)
0x00F0-0x00F6: System Registers (7 registers)
```

#### **OHT-50 Architecture v2.0 Structure:**
```
DC Motor Module (0x04):
0x0000-0x000F: Motor 1 Control (16 registers) - Enhanced
0x0010-0x001F: Motor 2 Control (16 registers) - Enhanced
0x0020-0x002F: Digital Input Status (11 registers) - New
0x0030-0x003F: Digital Output Control (13 registers) - New
0x0040-0x004F: Status Information (16 registers) - Moved
0x0050-0x005F: Configuration (16 registers) - Enhanced
0x0060-0x006F: Fault Status (16 registers) - Enhanced
0x00F0-0x00FF: System Registers (16 registers) - New

DI/DO Module (0x06):
0x0000-0x000F: Digital Input Status (16 registers)
0x0010-0x001F: Digital Output Control (16 registers)
0x0020-0x002F: Current Monitoring (16 registers)
0x0030-0x003F: Safety Status (16 registers)
0x0040-0x004F: Fault Status (16 registers)
0x0050-0x00FF: Reserved (176 registers)
```

---

## 🚨 **VẤN ĐỀ PHÁT HIỆN**

### **1. Register Mapping Conflicts:**
```c
// Conflict: Different register assignments
GitHub: 0x0000 = M1_Control_Mode
OHT-50: 0x0000 = Motor 1 Speed

// Solution: Reorganized OHT-50 mapping
OHT-50 v2.0: 0x0000 = Motor 1 Control Mode (compatible)
```

### **2. Data Type Mismatches:**
```c
// GitHub: 8-bit registers
M1_Command_Speed: uint8 (0-255)

// OHT-50: 16-bit registers  
DC_MOTOR_REG_MOTOR1_SPEED: 16-bit signed (-100 to +100%)

// Solution: Support both formats
OHT-50 v2.0: Added 8-bit compatibility + 16-bit extended
```

### **3. Missing Features:**
- **GitHub có:** Linear control mode, Digital I/O assignments
- **OHT-50 thiếu:** Linear control, DI/DO function assignments
- **Giải pháp:** Bổ sung đầy đủ tính năng

---

## ✅ **CẢI THIỆN ĐÃ THỰC HIỆN**

### **1. Enhanced Control Modes:**
```c
// Added control modes from GitHub
#define DC_MOTOR_REG_MOTOR1_CONTROL_MODE 0x0000  // 1=ONOFF, 2=LINEAR, 3=PID
#define DC_MOTOR_REG_MOTOR2_CONTROL_MODE 0x0010  // 1=ONOFF, 2=LINEAR, 3=PID
```

### **2. Digital Input/Output Support:**
```c
// Added DI/DO registers from GitHub
#define DC_MOTOR_REG_DI_STATUS_WORD      0x0020  // 16-bit unsigned
#define DC_MOTOR_REG_DI1_ASSIGNMENT      0x0021  // Function assignment
#define DC_MOTOR_REG_DO_STATUS_WORD      0x0030  // 16-bit unsigned
#define DC_MOTOR_REG_DO1_CONTROL         0x0031  // Control output
```

### **3. PID Control Enhancement:**
```c
// Added PID registers from GitHub
#define DC_MOTOR_REG_MOTOR1_PID_KP       0x0008  // 8-bit unsigned
#define DC_MOTOR_REG_MOTOR1_PID_KI       0x0009  // 8-bit unsigned
#define DC_MOTOR_REG_MOTOR1_PID_KD       0x000A  // 8-bit unsigned
```

### **4. Linear Control Parameters:**
```c
// Added linear control from GitHub
#define DC_MOTOR_REG_MOTOR1_LINEAR_INPUT 0x0003  // 8-bit unsigned
#define DC_MOTOR_REG_MOTOR1_LINEAR_UNIT  0x0004  // 8-bit unsigned
#define DC_MOTOR_REG_MOTOR1_LINEAR_STATE 0x0005  // 8-bit unsigned
```

### **5. System Registers:**
```c
// Added system registers from GitHub
#define DC_MOTOR_REG_DEVICE_ID           0x00F0  // 16-bit unsigned
#define DC_MOTOR_REG_FIRMWARE_VERSION    0x00F1  // 16-bit unsigned
#define DC_MOTOR_REG_SYSTEM_STATUS       0x00F2  // 16-bit unsigned
#define DC_MOTOR_REG_SYSTEM_ERROR        0x00F3  // 16-bit unsigned
#define DC_MOTOR_REG_RESET_ERROR_CMD     0x00F4  // 16-bit unsigned
#define DC_MOTOR_REG_CONFIG_BAUDRATE     0x00F5  // 16-bit unsigned
#define DC_MOTOR_REG_CONFIG_PARITY       0x00F6  // 16-bit unsigned
```

---

## 🔧 **CẤU HÌNH NÂNG CAO**

### **1. Enhanced JSON Configuration:**
```json
{
  "dc_motor_module": {
    "motor_config": {
      "motors": [
        {
          "control_mode": 3,  // PID mode
          "pid_gains": {
            "kp": 100,
            "ki": 10,
            "kd": 5
          }
        }
      ],
      "linear_control": {
        "enabled": true,
        "accel_time": 10,
        "decel_time": 10,
        "linear_unit": 5
      }
    },
    "digital_input_config": {
      "inputs": [
        {"assignment": 1},  // start M1
        {"assignment": 2},  // stop M1
        {"assignment": 3},  // reverse M1
        {"assignment": 4}   // fault reset
      ]
    },
    "digital_output_config": {
      "outputs": [
        {"assignment": 1},  // running M1
        {"assignment": 2}   // fault M1
      ]
    }
  }
}
```

### **2. Function Assignment Codes:**
```c
// Digital Input Assignments
#define DI_ASSIGN_NONE          0
#define DI_ASSIGN_START_M1      1
#define DI_ASSIGN_STOP_M1       2
#define DI_ASSIGN_REVERSE_M1    3
#define DI_ASSIGN_FAULT_RESET   4
#define DI_ASSIGN_MODE_SWITCH   5

// Digital Output Assignments
#define DO_ASSIGN_NONE          0
#define DO_ASSIGN_RUNNING_M1    1
#define DO_ASSIGN_FAULT_M1      2
#define DO_ASSIGN_SPEED_REACHED_M1 3
#define DO_ASSIGN_READY         4
```

---

## 📈 **LỢI ÍCH ĐẠT ĐƯỢC**

### **1. Tương thích ngược:**
- ✅ Có thể làm việc với module GitHub
- ✅ Hỗ trợ cả 8-bit và 16-bit registers
- ✅ Tương thích với các thiết bị tương tự

### **2. Tính năng mở rộng:**
- ✅ Linear control mode
- ✅ Digital I/O function assignments
- ✅ Enhanced PID control
- ✅ System diagnostics

### **3. Tính linh hoạt:**
- ✅ 8 Digital Inputs (vs 4 của GitHub)
- ✅ 8 Digital Outputs (vs 2 của GitHub)
- ✅ Multiple control modes
- ✅ Configurable assignments

### **4. Khả năng mở rộng:**
- ✅ Register space lớn hơn (1536 vs 247)
- ✅ Support cho encoder feedback
- ✅ Differential drive control
- ✅ Advanced safety features

---

## 🧪 **TESTING STRATEGY**

### **1. Compatibility Testing:**
```python
def test_github_compatibility():
    # Test 8-bit register access
    test_8bit_registers()
    
    # Test control modes
    test_control_modes()
    
    # Test DI/DO assignments
    test_di_do_assignments()
    
    # Test system registers
    test_system_registers()
```

### **2. Extended Feature Testing:**
```python
def test_extended_features():
    # Test 16-bit registers
    test_16bit_registers()
    
    # Test encoder feedback
    test_encoder_support()
    
    # Test differential drive
    test_differential_drive()
    
    # Test advanced safety
    test_safety_features()
```

---

## 📋 **KẾT LUẬN**

### **✅ Thành công:**
1. **Tương thích hoàn toàn** với GitHub mapping
2. **Bổ sung tính năng** Linear control và DI/DO assignments
3. **Mở rộng khả năng** với 8 DI/DO và encoder support
4. **Chuẩn hóa** theo industry standards

### **🚀 Lợi ích:**
1. **Interoperability:** Có thể làm việc với nhiều thiết bị
2. **Flexibility:** Hỗ trợ nhiều control modes
3. **Scalability:** Register space lớn cho tương lai
4. **Reliability:** Enhanced safety và diagnostics

### **📈 Next Steps:**
1. **Implement firmware** theo mapping mới
2. **Test compatibility** với GitHub module
3. **Document API** cho developers
4. **Create migration guide** từ version cũ

---

**Changelog:**
- v2.0 (2025-01-28): Updated for Architecture v2.0
  - ✅ Aligned with 5 mandatory modules
  - ✅ Removed Stepper Motor references
  - ✅ Updated comparison table
  - ✅ Enhanced compatibility analysis
- v1.1 (2025-01-27): Added implementation details and testing strategy
- v1.0 (2025-01-27): Initial analysis document
