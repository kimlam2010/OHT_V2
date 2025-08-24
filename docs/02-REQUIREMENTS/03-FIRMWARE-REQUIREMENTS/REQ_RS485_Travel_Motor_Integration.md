# HƯỚNG DẪN KẾT NỐI VÀ ĐIỀU KHIỂN TRAVEL MOTOR MODULE QUA RS485
## OHT-50 Master Module - Travel Motor Integration

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-27  
**Team:** FW Team  
**Trạng thái:** Implementation Guide  

---

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với Travel Motor Module qua RS485
- Điều khiển động cơ DC (speed, direction, torque)
- Giám sát trạng thái động cơ (speed, current, temperature)
- Hỗ trợ differential drive cho 2 động cơ

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| **Tổng quan** | Hiểu kiến trúc DC Motor | FW Developer | Motor Control | ✅ Hoàn thành |
| **Phần cứng và kết nối** | Cấu hình phần cứng | HW Engineer | Wiring, Pinout | ✅ Hoàn thành |
| **Thông số kỹ thuật RS485** | Định nghĩa protocol | Network Engineer | RS485, Modbus | ✅ Hoàn thành |
| **Kết nối phần cứng** | Wiring diagram | HW Engineer | Cable Connection | ✅ Hoàn thành |
| **Cấu hình GPIO** | Pin assignment | HW Engineer | GPIO Setup | ✅ Hoàn thành |
| **Implementation Software** | Code implementation | FW Developer | HAL, Drivers | ✅ Hoàn thành |
| **HAL RS485 Driver** | Driver functions | FW Developer | HAL API | ✅ Hoàn thành |
| **DC Motor Manager** | Motor control logic | FW Developer | Motor Logic | ✅ Hoàn thành |
| **Modbus Register Map** | Register definitions | Protocol Engineer | Modbus Spec | ✅ Hoàn thành |
| **Control Commands** | Motor control commands | FW Developer | Control API | ✅ Hoàn thành |
| **Status Monitoring** | Motor status monitoring | FW Developer | Status API | ✅ Hoàn thành |
| **Error Handling** | Error handling logic | FW Developer | Error Recovery | ✅ Hoàn thành |
| **Testing Procedures** | Testing guidelines | Test Engineer | Test Cases | ✅ Hoàn thành |
| **Troubleshooting** | Debug procedures | Support Engineer | Debug Guide | ✅ Hoàn thành |
| **Performance Optimization** | Performance tuning | Performance Engineer | Optimization | ✅ Hoàn thành |
| **Safety Considerations** | Safety requirements | Safety Engineer | Motor Safety | ✅ Hoàn thành |

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │   Travel Motor │
│  (Orange Pi 5B) │             │    Module       │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ Travel Motor│ │
│ │ Driver      │ │             │ │ Controller  │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ Travel Motor│ │             │ │ DC Motors   │ │
│ │ Manager     │ │             │ │ (Dual)      │ │
│ └─────────────┘ │             │ └─────────────┘ │
└─────────────────┘             └─────────────────┘
```

---

## 🔧 **PHẦN CỨNG VÀ KẾT NỐI**

### **1. Thông số kỹ thuật RS485:**

#### **Master Module (Orange Pi 5B):**
- **UART Interface:** UART1 (`/dev/ttyS1`)
- **RS485 Transceiver:** MAX485 hoặc tương đương
- **Baud Rate:** 115200 bps (có thể điều chỉnh)
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Flow Control:** None

#### **Travel Motor Module:**
- **Protocol:** Modbus RTU
- **Slave Address:** 0x04 (cố định cho Travel Motor Module)
- **Response Time:** < 100ms
- **Error Detection:** CRC-16

### **2. Kết nối phần cứng:**

#### **Wiring Diagram:**
```
Orange Pi 5B          RS485 Bus          Travel Motor Module
┌─────────────┐      ┌─────────┐        ┌─────────────┐
│ UART1_TX    │──────│   A+    │────────│   A+        │
│ (GPIO)      │      │         │        │             │
│             │      │         │        │             │
│ UART1_RX    │──────│   B-    │────────│   B-        │
│ (GPIO)      │      │         │        │             │
│             │      │         │        │             │
│             │──────│         │────────│             │
│ (Control)   │      │         │        │             │
│             │      │         │        │             │
│ GND         │──────│   GND   │────────│   GND       │
└─────────────┘      └─────────┘        └─────────────┘
```

### **3. Cấu hình GPIO:**

#### **Pin Assignment:**
```bash
# UART1 Configuration
UART1_TX: GPIO1_A0 (Pin 8)
UART1_RX: GPIO1_A1 (Pin 10)

# udev rule để tạo symlink
echo 'KERNEL=="ttyS1", SYMLINK+="ttyOHT485"' > /etc/udev/rules.d/99-oht-rs485.rules
```

---

## 💻 **IMPLEMENTATION SOFTWARE**

### **1. HAL RS485 Driver (`hal_rs485.c`)**

#### **Core Functions:**
```c
// Travel Motor Module specific functions
hal_status_t travel_motor_module_init(void);
hal_status_t travel_motor_module_set_speed(uint8_t motor_id, int16_t speed);
hal_status_t travel_motor_module_get_speed(uint8_t motor_id, int16_t *speed);
hal_status_t travel_motor_module_set_direction(uint8_t motor_id, motor_direction_t direction);
hal_status_t travel_motor_module_get_direction(uint8_t motor_id, motor_direction_t *direction);
hal_status_t travel_motor_module_enable(uint8_t motor_id, bool enable);
hal_status_t travel_motor_module_is_enabled(uint8_t motor_id, bool *enabled);
hal_status_t travel_motor_module_brake(uint8_t motor_id, bool brake);
hal_status_t travel_motor_module_emergency_stop(void);
hal_status_t travel_motor_module_read_status(travel_motor_status_t *status);
```

### **2. DC Motor Manager Application (`dc_motor_manager.c`)**

#### **Core Functions:**
```c
// Travel motor management functions
travel_motor_status_t travel_motor_manager_get_status(void);
hal_status_t travel_motor_manager_differential_drive(int16_t left_speed, int16_t right_speed);
hal_status_t travel_motor_manager_set_speed_profile(speed_profile_t *profile);
hal_status_t travel_motor_manager_calibrate(void);
hal_status_t travel_motor_manager_handle_faults(void);
hal_status_t travel_motor_manager_save_config(void);
hal_status_t travel_motor_manager_load_config(void);
```

---

## 📡 **MODBUS REGISTER MAP**

---

## 📊 **BẢNG MAPPING REGISTER MODBUS - TRAVEL MOTOR MODULE:**

| **Register Address** | **Tên Register** | **Loại** | **Đơn vị** | **Mô tả** | **Access** | **Trạng thái** |
|---------------------|------------------|----------|------------|-----------|------------|----------------|
| **0x0000** | Motor 1 Control Mode | Holding | - | Chế độ điều khiển động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0001** | Motor 1 Enable | Holding | - | Bật/tắt động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0002** | Motor 1 Speed | Holding | % | Tốc độ động cơ 1 (0-100) | R/W | ✅ Hoàn thành |
| **0x0003** | Motor 1 Linear Input | Holding | % | Input tuyến tính động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0004** | Motor 1 Linear Unit | Holding | - | Đơn vị tuyến tính động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0005** | Motor 1 Linear State | Holding | - | Trạng thái tuyến tính động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0006** | Motor 1 Actual Speed | Input | % | Tốc độ thực tế động cơ 1 | R | ✅ Hoàn thành |
| **0x0007** | Motor 1 Direction | Holding | - | Hướng động cơ 1 (0=Idle, 1=Forward, 2=Reverse) | R/W | ✅ Hoàn thành |
| **0x0008** | Motor 1 PID Kp | Holding | ×100 | Hệ số Kp của PID động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0009** | Motor 1 PID Ki | Holding | ×100 | Hệ số Ki của PID động cơ 1 | R/W | ✅ Hoàn thành |
| **0x000A** | Motor 1 PID Kd | Holding | ×100 | Hệ số Kd của PID động cơ 1 | R/W | ✅ Hoàn thành |
| **0x000B** | Motor 1 Status Word | Input | - | Trạng thái động cơ 1 (bit field) | R | ✅ Hoàn thành |
| **0x000C** | Motor 1 Error Code | Input | - | Mã lỗi động cơ 1 | R | ✅ Hoàn thành |
| **0x000D** | Motor 1 Brake | Holding | - | Phanh động cơ 1 (0=Release, 1=Apply) | R/W | ✅ Hoàn thành |
| **0x000E** | Motor 1 Current Limit | Holding | mA | Giới hạn dòng động cơ 1 | R/W | ✅ Hoàn thành |
| **0x000F** | Motor 1 Acceleration | Holding | 0.1s | Gia tốc động cơ 1 | R/W | ✅ Hoàn thành |
| **0x0010** | Motor 2 Control Mode | Holding | - | Chế độ điều khiển động cơ 2 | R/W | ✅ Hoàn thành |
| **0x0011** | Motor 2 Enable | Holding | - | Bật/tắt động cơ 2 | R/W | ✅ Hoàn thành |
| **0x0012** | Motor 2 Speed | Holding | % | Tốc độ động cơ 2 (0-100) | R/W | ✅ Hoàn thành |
| **0x0013** | Motor 2 Linear Input | Holding | % | Input tuyến tính động cơ 2 | R/W | ✅ Hoàn thành |
| **0x0014** | Motor 2 Linear Unit | Holding | - | Đơn vị tuyến tính động cơ 2 | R/W | ✅ Hoàn thành |
| **0x0015** | Motor 2 Linear State | Holding | - | Trạng thái tuyến tính động cơ 2 | R/W | ✅ Hoàn thành |
| **0x0016** | Motor 2 Actual Speed | Input | % | Tốc độ thực tế động cơ 2 | R | ✅ Hoàn thành |
| **0x0017** | Motor 2 Direction | Holding | - | Hướng động cơ 2 (0=Idle, 1=Forward, 2=Reverse) | R/W | ✅ Hoàn thành |
| **0x0018** | Motor 2 PID Kp | Holding | ×100 | Hệ số Kp của PID động cơ 2 | R/W | ✅ Hoàn thành |
| **0x0019** | Motor 2 PID Ki | Holding | ×100 | Hệ số Ki của PID động cơ 2 | R/W | ✅ Hoàn thành |
| **0x001A** | Motor 2 PID Kd | Holding | ×100 | Hệ số Kd của PID động cơ 2 | R/W | ✅ Hoàn thành |
| **0x001B** | Motor 2 Status Word | Input | - | Trạng thái động cơ 2 (bit field) | R | ✅ Hoàn thành |
| **0x001C** | Motor 2 Error Code | Input | - | Mã lỗi động cơ 2 | R | ✅ Hoàn thành |
| **0x001D** | Motor 2 Brake | Holding | - | Phanh động cơ 2 (0=Release, 1=Apply) | R/W | ✅ Hoàn thành |
| **0x001E** | Motor 2 Current Limit | Holding | mA | Giới hạn dòng động cơ 2 | R/W | ✅ Hoàn thành |
| **0x001F** | Motor 2 Acceleration | Holding | 0.1s | Gia tốc động cơ 2 | R/W | ✅ Hoàn thành |
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
| **0x00F0** | Device ID | Input | - | ID thiết bị (Modbus slave address) | R | ✅ Hoàn thành |
| **0x00F1** | Firmware Version | Input | - | Phiên bản firmware | R | ✅ Hoàn thành |
| **0x00F2** | System Status | Input | - | Trạng thái hệ thống (bit field) | R | ✅ Hoàn thành |
| **0x00F3** | System Error | Input | - | Lỗi hệ thống (global error code) | R | ✅ Hoàn thành |
| **0x00F4** | Reset Error Command | Holding | - | Lệnh reset lỗi | W | ✅ Hoàn thành |
| **0x00F5** | Config Baudrate | Holding | - | Cấu hình baudrate | R/W | ✅ Hoàn thành |
| **0x00F6** | Config Parity | Holding | - | Cấu hình parity | R/W | ✅ Hoàn thành |
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

---

### **1. Control Registers (0x0000-0x00FF):**

#### **Motor 1 Control (0x0000-0x000F):**
```c
// Motor 1 control mode (1=ONOFF, 2=LINEAR, 3=PID)
#define DC_MOTOR_REG_MOTOR1_CONTROL_MODE 0x0000  // 8-bit unsigned

// Motor 1 enable/disable
#define DC_MOTOR_REG_MOTOR1_ENABLE       0x0001  // 0=Disable, 1=Enable

// Motor 1 speed setpoint
#define DC_MOTOR_REG_MOTOR1_SPEED        0x0002  // 8-bit unsigned

// Motor 1 linear input (0-100%)
#define DC_MOTOR_REG_MOTOR1_LINEAR_INPUT 0x0003  // 8-bit unsigned

// Motor 1 linear unit (0-20%)
#define DC_MOTOR_REG_MOTOR1_LINEAR_UNIT  0x0004  // 8-bit unsigned

// Motor 1 linear state (0=DECELERATION, 1=ACCELERATION)
#define DC_MOTOR_REG_MOTOR1_LINEAR_STATE 0x0005  // 8-bit unsigned

// Motor 1 actual speed
#define DC_MOTOR_REG_MOTOR1_ACTUAL_SPEED 0x0006  // 8-bit unsigned

// Motor 1 direction (0=Idle, 1=Forward, 2=Reverse)
#define DC_MOTOR_REG_MOTOR1_DIRECTION    0x0007  // 8-bit unsigned

// Motor 1 PID Kp gain (×100)
#define DC_MOTOR_REG_MOTOR1_PID_KP       0x0008  // 8-bit unsigned

// Motor 1 PID Ki gain (×100)
#define DC_MOTOR_REG_MOTOR1_PID_KI       0x0009  // 8-bit unsigned

// Motor 1 PID Kd gain (×100)
#define DC_MOTOR_REG_MOTOR1_PID_KD       0x000A  // 8-bit unsigned

// Motor 1 status word (bit field)
#define DC_MOTOR_REG_MOTOR1_STATUS_WORD  0x000B  // 8-bit unsigned

// Motor 1 error code
#define DC_MOTOR_REG_MOTOR1_ERROR_CODE   0x000C  // 8-bit unsigned

// Motor 1 brake
#define DC_MOTOR_REG_MOTOR1_BRAKE        0x000D  // 0=Release, 1=Apply

// Motor 1 current limit (mA)
#define DC_MOTOR_REG_MOTOR1_CURRENT_LIMIT 0x000E  // 16-bit unsigned

// Motor 1 acceleration (0.1s)
#define DC_MOTOR_REG_MOTOR1_ACCEL        0x000F  // 16-bit unsigned
```

#### **Motor 2 Control (0x0010-0x001F):**
```c
// Motor 2 control mode (1=ONOFF, 2=LINEAR, 3=PID)
#define DC_MOTOR_REG_MOTOR2_CONTROL_MODE 0x0010  // 8-bit unsigned

// Motor 2 enable/disable
#define DC_MOTOR_REG_MOTOR2_ENABLE       0x0011  // 0=Disable, 1=Enable

// Motor 2 speed setpoint
#define DC_MOTOR_REG_MOTOR2_SPEED        0x0012  // 8-bit unsigned

// Motor 2 linear input (0-100%)
#define DC_MOTOR_REG_MOTOR2_LINEAR_INPUT 0x0013  // 8-bit unsigned

// Motor 2 linear unit (0-20%)
#define DC_MOTOR_REG_MOTOR2_LINEAR_UNIT  0x0014  // 8-bit unsigned

// Motor 2 linear state (0=DECELERATION, 1=ACCELERATION)
#define DC_MOTOR_REG_MOTOR2_LINEAR_STATE 0x0015  // 8-bit unsigned

// Motor 2 actual speed
#define DC_MOTOR_REG_MOTOR2_ACTUAL_SPEED 0x0016  // 8-bit unsigned

// Motor 2 direction (0=Idle, 1=Forward, 2=Reverse)
#define DC_MOTOR_REG_MOTOR2_DIRECTION    0x0017  // 8-bit unsigned

// Motor 2 PID Kp gain (×100)
#define DC_MOTOR_REG_MOTOR2_PID_KP       0x0018  // 8-bit unsigned

// Motor 2 PID Ki gain (×100)
#define DC_MOTOR_REG_MOTOR2_PID_KI       0x0019  // 8-bit unsigned

// Motor 2 PID Kd gain (×100)
#define DC_MOTOR_REG_MOTOR2_PID_KD       0x001A  // 8-bit unsigned

// Motor 2 status word (bit field)
#define DC_MOTOR_REG_MOTOR2_STATUS_WORD  0x001B  // 8-bit unsigned

// Motor 2 error code
#define DC_MOTOR_REG_MOTOR2_ERROR_CODE   0x001C  // 8-bit unsigned

// Motor 2 brake
#define DC_MOTOR_REG_MOTOR2_BRAKE        0x001D  // 0=Release, 1=Apply

// Motor 2 current limit (mA)
#define DC_MOTOR_REG_MOTOR2_CURRENT_LIMIT 0x001E  // 16-bit unsigned

// Motor 2 acceleration (0.1s)
#define DC_MOTOR_REG_MOTOR2_ACCEL        0x001F  // 16-bit unsigned
```

#### **Digital Input Status (0x0020-0x0024):**
```c
// Digital input status word (4 inputs)
#define DC_MOTOR_REG_DI_STATUS_WORD      0x0020  // 16-bit unsigned

// DI1 assignment (0=none, 1=start M1, 2=stop M1, 3=reverse M1, 4=fault reset, 5=mode switch)
#define DC_MOTOR_REG_DI1_ASSIGNMENT      0x0021  // 8-bit unsigned

// DI2 assignment (same options as DI1)
#define DC_MOTOR_REG_DI2_ASSIGNMENT      0x0022  // 8-bit unsigned

// DI3 assignment (same options as DI1)
#define DC_MOTOR_REG_DI3_ASSIGNMENT      0x0023  // 8-bit unsigned

// DI4 assignment (same options as DI1)
#define DC_MOTOR_REG_DI4_ASSIGNMENT      0x0024  // 8-bit unsigned
```

#### **Digital Output Control (0x0030-0x0034):**
```c
// Digital output status word (2 outputs)
#define DC_MOTOR_REG_DO_STATUS_WORD      0x0030  // 16-bit unsigned

// DO1 control (0=off, 1=on)
#define DC_MOTOR_REG_DO1_CONTROL         0x0031  // 8-bit unsigned

// DO1 assignment (0=none, 1=running M1, 2=fault M1, 3=speed reached M1, 4=ready)
#define DC_MOTOR_REG_DO1_ASSIGNMENT      0x0032  // 8-bit unsigned

// DO2 control (0=off, 1=on)
#define DC_MOTOR_REG_DO2_CONTROL         0x0033  // 8-bit unsigned

// DO2 assignment (same options as DO1)
#define DC_MOTOR_REG_DO2_ASSIGNMENT      0x0034  // 8-bit unsigned
```







#### **System Registers (0x00F0-0x00FF):**
```c
// Device ID (Modbus slave address)
#define DC_MOTOR_REG_DEVICE_ID           0x00F0  // 16-bit unsigned

// Firmware version (e.g. 0x0101 = v1.01)
#define DC_MOTOR_REG_FIRMWARE_VERSION    0x00F1  // 16-bit unsigned

// System status (bit field)
#define DC_MOTOR_REG_SYSTEM_STATUS       0x00F2  // 16-bit unsigned

// System error (global error code)
#define DC_MOTOR_REG_SYSTEM_ERROR        0x00F3  // 16-bit unsigned

// Reset error command (write 1 to reset all error flags)
#define DC_MOTOR_REG_RESET_ERROR_CMD     0x00F4  // 16-bit unsigned

// Config baudrate (1=9600, 2=19200, 3=38400,...)
#define DC_MOTOR_REG_CONFIG_BAUDRATE     0x00F5  // 16-bit unsigned

// Config parity (0=None, 1=Even, 2=Odd)
#define DC_MOTOR_REG_CONFIG_PARITY       0x00F6  // 16-bit unsigned

// Module type (0x0004 = DC Motor)
#define DC_MOTOR_REG_MODULE_TYPE         0x00F7  // 16-bit unsigned

// Module name (low word)
#define DC_MOTOR_REG_MODULE_NAME_LOW     0x00F8  // 16-bit unsigned

// Module name (high word)
#define DC_MOTOR_REG_MODULE_NAME_HIGH    0x00F9  // 16-bit unsigned

// Hardware version
#define DC_MOTOR_REG_HARDWARE_VERSION    0x00FA  // 16-bit unsigned

// Serial number (low word)
#define DC_MOTOR_REG_SERIAL_NUMBER_LOW   0x00FB  // 16-bit unsigned

// Serial number (high word)
#define DC_MOTOR_REG_SERIAL_NUMBER_HIGH  0x00FC  // 16-bit unsigned

// Build date (low word)
#define DC_MOTOR_REG_BUILD_DATE_LOW      0x00FD  // 16-bit unsigned

// Build date (high word)
#define DC_MOTOR_REG_BUILD_DATE_HIGH     0x00FE  // 16-bit unsigned

// Register checksum
#define DC_MOTOR_REG_CHECKSUM            0x00FF  // 16-bit unsigned
```

---

## 🔧 **CONFIGURATION**

### **1. DC Motor Module Configuration:**
```json
{
  "dc_motor_module": {
    "address": "0x04",
    "name": "DC Motor Module",
    "type": "dc_motor",
    "version": "1.0",
    "enabled": true,
    "motor_config": {
      "enabled": true,
      "motors": [
        {
          "id": 1,
          "enabled": true,
          "type": "dc_motor",
          "control_mode": 3,
          "max_speed": 3000,
          "max_current": 5000,
          "wheel_diameter": 100,
          "gear_ratio": 20,
          "pid_gains": {
            "kp": 100,
            "ki": 10,
            "kd": 5
          }
        },
        {
          "id": 2,
          "enabled": true,
          "type": "dc_motor",
          "control_mode": 3,
          "max_speed": 3000,
          "max_current": 5000,
          "wheel_diameter": 100,
          "gear_ratio": 20,
          "pid_gains": {
            "kp": 100,
            "ki": 10,
            "kd": 5
          }
        }
      ],
      "differential_drive": {
        "enabled": true,
        "wheelbase": 300,
        "track_width": 250
      },
      "linear_control": {
        "enabled": true,
        "accel_time": 10,
        "decel_time": 10,
        "linear_unit": 5
      }
    },
    "digital_input_config": {
      "enabled": true,
      "inputs": [
        {"id": 1, "enabled": true, "assignment": 1, "debounce_time": 10},
        {"id": 2, "enabled": true, "assignment": 2, "debounce_time": 10},
        {"id": 3, "enabled": true, "assignment": 3, "debounce_time": 10},
        {"id": 4, "enabled": true, "assignment": 4, "debounce_time": 10}
      ]
    },
    "digital_output_config": {
      "enabled": true,
      "outputs": [
        {"id": 1, "enabled": true, "assignment": 1, "control": 0},
        {"id": 2, "enabled": true, "assignment": 2, "control": 0}
      ]
    },
    "safety_config": {
      "enabled": true,
      "emergency_stop": true,
      "overcurrent_protection": true,
      "overtemperature_protection": true,
      "stall_detection": true
    },
    "system_config": {
      "baudrate": 3,
      "parity": 0,
      "device_id": 4
    }
  }
}
```

### **2. HAL Configuration:**
```c
// DC motor module HAL configuration
typedef struct {
    uint8_t address;
    uint32_t timeout_ms;
    uint8_t retry_count;
    uint16_t max_speed;
    uint16_t min_speed;
    uint16_t default_speed;
    uint8_t speed_units;
    uint16_t wheel_diameter;
    uint16_t wheel_distance;
    uint16_t gear_ratio;
    uint16_t current_limits[2];
    uint16_t accelerations[2];
    uint16_t decelerations[2];
    uint16_t safety_thresholds[4];
} dc_motor_module_config_t;
```

---

## 🧪 **TESTING**

### **1. Unit Tests:**
```c
// DC motor module unit tests
void test_dc_motor_module_init(void);
void test_dc_motor_module_set_speed(void);
void test_dc_motor_module_get_speed(void);
void test_dc_motor_module_set_direction(void);
void test_dc_motor_module_enable_disable(void);
void test_dc_motor_module_brake(void);
void test_dc_motor_module_emergency_stop(void);
void test_dc_motor_module_read_status(void);
void test_dc_motor_module_differential_drive(void);
void test_dc_motor_module_fault_handling(void);
```

### **2. Integration Tests:**
```c
// DC motor module integration tests
void test_dc_motor_module_speed_control(void);
void test_dc_motor_module_direction_control(void);
void test_dc_motor_module_differential_drive(void);
void test_dc_motor_module_acceleration_control(void);
void test_dc_motor_module_brake_control(void);
void test_dc_motor_module_fault_recovery(void);
void test_dc_motor_module_communication_fault(void);
```

### **3. Performance Tests:**
```c
// DC motor module performance tests
void test_dc_motor_module_response_time(void);
void test_dc_motor_module_speed_accuracy(void);
void test_dc_motor_module_torque_control(void);
void test_dc_motor_module_stability(void);
void test_dc_motor_module_efficiency(void);
void test_dc_motor_module_thermal_performance(void);
```

---

## 📊 **MONITORING VÀ DIAGNOSTICS**

### **1. Real-time Monitoring:**
```c
// DC motor module monitoring data
typedef struct {
    int16_t motor1_speed;
    int16_t motor2_speed;
    motor_direction_t motor1_direction;
    motor_direction_t motor2_direction;
    bool motor1_enabled;
    bool motor2_enabled;
    bool motor1_brake;
    bool motor2_brake;
    uint16_t motor1_current_speed;
    uint16_t motor2_current_speed;
    uint16_t motor1_current;
    uint16_t motor2_current;
    int16_t motor1_temperature;
    int16_t motor2_temperature;
    uint16_t motor1_voltage;
    uint16_t motor2_voltage;
    dc_motor_fault_status_t faults;
    uint32_t uptime;
    uint32_t communication_errors;
} dc_motor_module_status_t;
```

### **2. Diagnostic Functions:**
```c
// DC motor module diagnostics
hal_status_t dc_motor_module_self_test(void);
hal_status_t dc_motor_module_calibrate_speed(void);
hal_status_t dc_motor_module_calibrate_current(void);
hal_status_t dc_motor_module_reset_faults(void);
hal_status_t dc_motor_module_get_diagnostics(dc_motor_diagnostics_t *diag);
hal_status_t dc_motor_module_export_logs(void);
```

---

## 🚨 **SAFETY VÀ FAULT HANDLING**

### **1. Safety Features:**
- **Overcurrent Protection:** Tự động ngắt khi current vượt ngưỡng
- **Overtemperature Protection:** Tự động ngắt khi nhiệt độ quá cao
- **Stall Detection:** Phát hiện động cơ bị kẹt
- **Emergency Stop:** Dừng khẩn cấp cả 2 động cơ
- **Brake Control:** Điều khiển phanh điện từ
- **Speed Limiting:** Giới hạn tốc độ tối đa

### **2. Fault Handling:**
```c
// DC motor module fault handling
typedef enum {
    DC_MOTOR_FAULT_NONE = 0,
    DC_MOTOR_FAULT_OVERCURRENT,
    DC_MOTOR_FAULT_OVERTEMP,
    DC_MOTOR_FAULT_STALL,
    DC_MOTOR_FAULT_COMMUNICATION,
    DC_MOTOR_FAULT_EMERGENCY_STOP,
    DC_MOTOR_FAULT_BRAKE_FAULT
} dc_motor_fault_type_t;

hal_status_t dc_motor_module_handle_fault(dc_motor_fault_type_t fault);
hal_status_t dc_motor_module_clear_fault(dc_motor_fault_type_t fault);
hal_status_t dc_motor_module_get_fault_status(dc_motor_fault_status_t *status);
```

---

## 📈 **PERFORMANCE METRICS**

### **1. Key Performance Indicators:**
- **Speed Range:** 0-5000 RPM
- **Torque:** 0.5-3.0 Nm per motor
- **Response Time:** < 100ms for speed commands
- **Accuracy:** ±1% speed accuracy
- **Efficiency:** > 85% motor efficiency
- **Reliability:** > 99.9% uptime

### **2. Monitoring Metrics:**
- **Speed:** Real-time speed monitoring
- **Current:** Motor current monitoring
- **Temperature:** Motor temperature monitoring
- **Voltage:** Motor voltage monitoring
- **Fault Detection:** Real-time fault monitoring
- **Communication:** RS485 communication status

---

## 🔄 **MAINTENANCE**

### **1. Regular Maintenance:**
- **Daily:** Check motor status và speed accuracy
- **Weekly:** Verify current và temperature readings
- **Monthly:** Calibrate speed sensors
- **Quarterly:** Full system diagnostics
- **Annually:** Motor performance assessment

### **2. Troubleshooting:**
- **Communication Issues:** Check RS485 wiring và termination
- **Speed Issues:** Check motor parameters và settings
- **Current Issues:** Verify current sensors và calibration
- **Fault Issues:** Review fault logs và clear faults
- **Performance Issues:** Check mechanical alignment và bearings

---

**Changelog:**
- v1.0 (2025-01-27): Initial DC Motor Module integration guide
- v1.1 (2025-01-27): Enhanced with GitHub Driver_2_Motor mapping compatibility
  - Added control modes (ONOFF, LINEAR, PID)
  - Added digital input/output registers (8 DI, 8 DO)
  - Added PID control registers
  - Added linear control parameters
  - Added system registers (0x00F0-0x00FF)
  - Extended configuration for DI/DO assignments
- v1.2 (2025-01-27): Removed GPIO_DE pin (not needed for RS485)
- v1.3 (2025-01-27): Simplified to match actual module specs (4 DI / 2 DO)
  - Reduced DI channels from 8 to 4 (0x0020-0x0024)
  - Reduced DO channels from 8 to 2 (0x0030-0x0034)
  - Changed speed registers from 16-bit signed to 8-bit unsigned
  - Removed extended registers (status, configuration, fault)
  - Updated system registers to 7 registers (0x00F0-0x00F6)
  - Updated configuration JSON to match actual I/O count
- v1.4 (2025-01-27): Added complete auto-detect registers (0x00F7-0x00FF) for module identification
