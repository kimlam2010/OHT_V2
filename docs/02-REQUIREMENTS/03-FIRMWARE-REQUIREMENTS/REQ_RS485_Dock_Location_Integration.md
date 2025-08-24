# HƯỚNG DẪN KẾT NỐI VÀ ĐIỀU KHIỂN DOCK & LOCATION MODULE QUA RS485
## OHT-50 Master Module - Dock & Location Module Integration

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-27  
**Team:** FW Team  
**Trạng thái:** Implementation Guide  

---

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với Sensor Module qua RS485
- Thu thập dữ liệu từ cảm biến IMU và Magnetic sensors
- Xử lý dữ liệu cảm biến cho navigation và safety
- Cung cấp thông tin định hướng và vị trí cho robot
- **Lưu ý:** LiDAR đã kết nối qua USB, không qua RS485

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| **Tổng quan** | Hiểu kiến trúc Sensor Module | FW Developer | Sensor Integration | ✅ Hoàn thành |
| **Phần cứng và kết nối** | Cấu hình phần cứng | HW Engineer | Wiring, Pinout | ✅ Hoàn thành |
| **Thông số kỹ thuật RS485** | Định nghĩa protocol | Network Engineer | RS485, Modbus | ✅ Hoàn thành |
| **Kết nối phần cứng** | Wiring diagram | HW Engineer | Cable Connection | ✅ Hoàn thành |
| **Cấu hình GPIO** | Pin assignment | HW Engineer | GPIO Setup | ✅ Hoàn thành |
| **Implementation Software** | Code implementation | FW Developer | HAL, Drivers | ✅ Hoàn thành |
| **HAL RS485 Driver** | Driver functions | FW Developer | HAL API | ✅ Hoàn thành |
| **Sensor Manager** | Sensor control logic | FW Developer | Sensor Logic | ✅ Hoàn thành |
| **IMU Integration** | IMU sensor interface | FW Developer | IMU API | ✅ Hoàn thành |
| **Magnetic Sensor** | Magnetic sensor interface | FW Developer | Magnetic API | ✅ Hoàn thành |
| **LiDAR USB Integration** | LiDAR USB interface | FW Developer | LiDAR API | ✅ Hoàn thành |
| **Data Processing** | Sensor data processing | FW Developer | Data Processing | ✅ Hoàn thành |
| **Navigation Support** | Navigation algorithms | FW Developer | Navigation | ✅ Hoàn thành |
| **Safety Integration** | Safety monitoring | Safety Engineer | Safety Logic | ✅ Hoàn thành |
| **Error Handling** | Error handling logic | FW Developer | Error Recovery | ✅ Hoàn thành |
| **Testing Procedures** | Testing guidelines | Test Engineer | Test Cases | ✅ Hoàn thành |
| **Troubleshooting** | Debug procedures | Support Engineer | Debug Guide | ✅ Hoàn thành |
| **Performance Optimization** | Performance tuning | Performance Engineer | Optimization | ✅ Hoàn thành |
| **Calibration** | Sensor calibration | FW Developer | Calibration | ✅ Hoàn thành |

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │  Sensor Module  │
│  (Orange Pi 5B) │             │   (Controller)  │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ IMU &       │ │
│ │ Driver      │ │             │ │ Magnetic    │ │
│ └─────────────┘ │             │ │ Controller  │ │
│ ┌─────────────┐ │             │ └─────────────┘ │
│ │ Sensor Mgr  │ │             │ ┌─────────────┐ │
│ │ Application │ │             │ │ IMU +       │ │
│ └─────────────┘ │             │ │ Magnetic    │ │
└─────────────────┘             │ │ Sensors     │ │
                                └─────────────────┘

┌─────────────────┐    USB      ┌─────────────────┐
│   Master Module │ ──────────── │  LiDAR Module  │
│  (Orange Pi 5B) │             │   (USB)        │
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

#### **Sensor Module:**
- **Protocol:** Modbus RTU
- **Slave Address:** 0x05 (cố định cho Sensor Module)
- **Response Time:** < 100ms
- **Error Detection:** CRC-16

### **2. Kết nối phần cứng:**

#### **Wiring Diagram:**
```
Orange Pi 5B          RS485 Bus          Sensor Module
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

---

## 📊 **BẢNG MAPPING REGISTER MODBUS - SENSOR MODULE:**

| **Register Address** | **Tên Register** | **Loại** | **Đơn vị** | **Mô tả** | **Access** | **Trạng thái** |
|---------------------|------------------|----------|------------|-----------|------------|----------------|
| **0x0000** | IMU Accelerometer X | Input | mg | Gia tốc X (mg) | R | ✅ Hoàn thành |
| **0x0001** | IMU Accelerometer Y | Input | mg | Gia tốc Y (mg) | R | ✅ Hoàn thành |
| **0x0002** | IMU Accelerometer Z | Input | mg | Gia tốc Z (mg) | R | ✅ Hoàn thành |
| **0x0003** | IMU Gyroscope X | Input | mdps | Góc quay X (mdps) | R | ✅ Hoàn thành |
| **0x0004** | IMU Gyroscope Y | Input | mdps | Góc quay Y (mdps) | R | ✅ Hoàn thành |
| **0x0005** | IMU Gyroscope Z | Input | mdps | Góc quay Z (mdps) | R | ✅ Hoàn thành |
| **0x0006** | IMU Magnetometer X | Input | mgauss | Từ trường X (mgauss) | R | ✅ Hoàn thành |
| **0x0007** | IMU Magnetometer Y | Input | mgauss | Từ trường Y (mgauss) | R | ✅ Hoàn thành |
| **0x0008** | IMU Magnetometer Z | Input | mgauss | Từ trường Z (mgauss) | R | ✅ Hoàn thành |
| **0x0009** | IMU Temperature | Input | °C × 10 | Nhiệt độ IMU (°C) | R | ✅ Hoàn thành |
| **0x000A** | IMU Status | Input | - | Trạng thái IMU | R | ✅ Hoàn thành |
| **0x000B** | IMU Error Code | Input | - | Mã lỗi IMU | R | ✅ Hoàn thành |
| **0x0010** | Magnetic Sensor 1 | Input | mgauss | Cảm biến từ 1 (mgauss) | R | ✅ Hoàn thành |
| **0x0011** | Magnetic Sensor 2 | Input | mgauss | Cảm biến từ 2 (mgauss) | R | ✅ Hoàn thành |
| **0x0012** | Magnetic Sensor 3 | Input | mgauss | Cảm biến từ 3 (mgauss) | R | ✅ Hoàn thành |
| **0x0013** | Magnetic Sensor 4 | Input | mgauss | Cảm biến từ 4 (mgauss) | R | ✅ Hoàn thành |
| **0x0014** | Magnetic Status | Input | - | Trạng thái cảm biến từ | R | ✅ Hoàn thành |
| **0x0015** | Magnetic Error Code | Input | - | Mã lỗi cảm biến từ | R | ✅ Hoàn thành |
| **0x0020** | Limit Switch 1 | Input | - | Trạng thái limit switch 1 | R | ✅ Hoàn thành |
| **0x0021** | Limit Switch 2 | Input | - | Trạng thái limit switch 2 | R | ✅ Hoàn thành |
| **0x0022** | Limit Switch 3 | Input | - | Trạng thái limit switch 3 | R | ✅ Hoàn thành |
| **0x0023** | Limit Switch 4 | Input | - | Trạng thái limit switch 4 | R | ✅ Hoàn thành |
| **0x0024** | Limit Switch Status | Input | - | Trạng thái tổng hợp limit switch | R | ✅ Hoàn thành |
| **0x0025** | Limit Switch Error | Input | - | Mã lỗi limit switch | R | ✅ Hoàn thành |
| **0x0030** | Position X | Input | mm × 10 | Vị trí X (mm) | R | ✅ Hoàn thành |
| **0x0031** | Position Y | Input | mm × 10 | Vị trí Y (mm) | R | ✅ Hoàn thành |
| **0x0032** | Position Z | Input | mm × 10 | Vị trí Z (mm) | R | ✅ Hoàn thành |
| **0x0033** | Orientation Roll | Input | deg × 10 | Góc roll (độ) | R | ✅ Hoàn thành |
| **0x0034** | Orientation Pitch | Input | deg × 10 | Góc pitch (độ) | R | ✅ Hoàn thành |
| **0x0035** | Orientation Yaw | Input | deg × 10 | Góc yaw (độ) | R | ✅ Hoàn thành |
| **0x0036** | Position Status | Input | - | Trạng thái vị trí | R | ✅ Hoàn thành |
| **0x0037** | Position Accuracy | Input | mm × 10 | Độ chính xác vị trí | R | ✅ Hoàn thành |
| **0x0040** | Velocity X | Input | m/s × 100 | Vận tốc X (m/s) | R | ✅ Hoàn thành |
| **0x0041** | Velocity Y | Input | m/s × 100 | Vận tốc Y (m/s) | R | ✅ Hoàn thành |
| **0x0042** | Velocity Z | Input | m/s × 100 | Vận tốc Z (m/s) | R | ✅ Hoàn thành |
| **0x0043** | Angular Velocity X | Input | rad/s × 1000 | Vận tốc góc X (rad/s) | R | ✅ Hoàn thành |
| **0x0044** | Angular Velocity Y | Input | rad/s × 1000 | Vận tốc góc Y (rad/s) | R | ✅ Hoàn thành |
| **0x0045** | Angular Velocity Z | Input | rad/s × 1000 | Vận tốc góc Z (rad/s) | R | ✅ Hoàn thành |
| **0x0046** | Velocity Status | Input | - | Trạng thái vận tốc | R | ✅ Hoàn thành |
| **0x0047** | Velocity Accuracy | Input | m/s × 100 | Độ chính xác vận tốc | R | ✅ Hoàn thành |
| **0x0050** | System Temperature | Input | °C × 10 | Nhiệt độ hệ thống | R | ✅ Hoàn thành |
| **0x0051** | System Status | Input | - | Trạng thái hệ thống | R | ✅ Hoàn thành |
| **0x0052** | System Error | Input | - | Mã lỗi hệ thống | R | ✅ Hoàn thành |
| **0x0053** | Safety Status | Input | - | Trạng thái an toàn | R | ✅ Hoàn thành |
| **0x0054** | Emergency Stop | Input | - | Trạng thái E-Stop | R | ✅ Hoàn thành |
| **0x0060** | IMU Sample Rate | Holding | Hz | Tần suất lấy mẫu IMU | R/W | ✅ Hoàn thành |
| **0x0061** | Magnetic Threshold | Holding | mgauss | Ngưỡng cảm biến từ | R/W | ✅ Hoàn thành |
| **0x0062** | Position Update Rate | Holding | Hz | Tần suất cập nhật vị trí | R/W | ✅ Hoàn thành |
| **0x0063** | Safety Enable | Holding | - | Bật/tắt an toàn | R/W | ✅ Hoàn thành |
| **0x0064** | Data Validation | Holding | - | Xác thực dữ liệu | R/W | ✅ Hoàn thành |
| **0x0065** | Fault Reporting | Holding | - | Báo cáo lỗi | R/W | ✅ Hoàn thành |
| **0x00F0** | Device ID | Input | - | ID thiết bị (Modbus slave address) | R | ✅ Hoàn thành |
| **0x00F1** | Firmware Version | Input | - | Phiên bản firmware | R | ✅ Hoàn thành |
| **0x00F2** | System Status | Input | - | Trạng thái hệ thống (bit field) | R | ✅ Hoàn thành |
| **0x00F3** | System Error | Input | - | Lỗi hệ thống (global error code) | R | ✅ Hoàn thành |
| **0x00F4** | Reset Error Command | Holding | - | Lệnh reset lỗi | W | ✅ Hoàn thành |
| **0x00F5** | Config Baudrate | Holding | - | Cấu hình baudrate | R/W | ✅ Hoàn thành |
| **0x00F6** | Config Parity | Holding | - | Cấu hình parity | R/W | ✅ Hoàn thành |
| **0x00F7** | Module Type | Input | - | Loại module (0x0005 = Sensor Module) | R | ✅ Hoàn thành |
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
- **LiDAR:** Kết nối qua USB, không qua RS485

### **1. HAL RS485 Driver (`hal_rs485.c`)**

#### **Core Functions:**
```c
// Sensor Module specific functions (IMU + Magnetic)
hal_status_t sensor_module_init(void);
hal_status_t sensor_module_read_imu_data(imu_data_t *data);
hal_status_t sensor_module_read_magnetic_data(magnetic_data_t *data);
hal_status_t sensor_module_read_limit_switches(limit_switch_data_t *data);
hal_status_t sensor_module_get_position_data(position_data_t *data);
hal_status_t sensor_module_read_status(sensor_status_t *status);

// Note: LiDAR data is handled via USB interface, not RS485
```

### **2. Sensor Manager Application (`sensor_manager.c`)**

#### **Core Functions:**
```c
// Sensor management functions
sensor_status_t sensor_manager_get_status(void);
hal_status_t sensor_manager_process_sensor_data(void);
hal_status_t sensor_manager_detect_obstacles(void);
hal_status_t sensor_manager_calculate_position(void);
hal_status_t sensor_manager_fusion_data(void);
hal_status_t sensor_manager_handle_faults(void);
hal_status_t sensor_manager_save_config(void);
hal_status_t sensor_manager_load_config(void);
```

---

## 📡 **MODBUS REGISTER MAP**

### **1. IMU Data Registers (0x0000-0x00FF):**

#### **IMU Status (0x0000-0x000F):**
```c
// IMU status
#define SENSOR_REG_IMU_STATUS            0x0000  // Bit field

// IMU accelerometer X (0.01g resolution)
#define SENSOR_REG_IMU_ACCEL_X           0x0001  // 16-bit signed

// IMU accelerometer Y (0.01g resolution)
#define SENSOR_REG_IMU_ACCEL_Y           0x0002  // 16-bit signed

// IMU accelerometer Z (0.01g resolution)
#define SENSOR_REG_IMU_ACCEL_Z           0x0003  // 16-bit signed

// IMU gyroscope X (0.01°/s resolution)
#define SENSOR_REG_IMU_GYRO_X            0x0004  // 16-bit signed

// IMU gyroscope Y (0.01°/s resolution)
#define SENSOR_REG_IMU_GYRO_Y            0x0005  // 16-bit signed

// IMU gyroscope Z (0.01°/s resolution)
#define SENSOR_REG_IMU_GYRO_Z            0x0006  // 16-bit signed

// IMU temperature (°C)
#define SENSOR_REG_IMU_TEMPERATURE       0x0007  // 16-bit signed

// IMU error code
#define SENSOR_REG_IMU_ERROR             0x0008  // 16-bit unsigned
```

### **2. Magnetic Sensor Data Registers (0x0100-0x01FF):**

#### **Magnetic Status (0x0100-0x010F):**
```c
// Magnetic sensor status
#define SENSOR_REG_MAGNETIC_STATUS       0x0100  // Bit field

// Magnetic sensor 1 value
#define SENSOR_REG_MAGNETIC1_VALUE       0x0101  // 16-bit unsigned

// Magnetic sensor 2 value
#define SENSOR_REG_MAGNETIC2_VALUE       0x0102  // 16-bit unsigned

// Magnetic sensor 1 threshold
#define SENSOR_REG_MAGNETIC1_THRESHOLD   0x0103  // 16-bit unsigned

// Magnetic sensor 2 threshold
#define SENSOR_REG_MAGNETIC2_THRESHOLD   0x0104  // 16-bit unsigned

// Magnetic sensor error code
#define SENSOR_REG_MAGNETIC_ERROR        0x0105  // 16-bit unsigned
```

### **3. Limit Switch Data Registers (0x0200-0x02FF):**

#### **Limit Switch Status (0x0200-0x020F):**
```c
// Limit switch status
#define SENSOR_REG_LIMIT_SWITCH_STATUS   0x0200  // Bit field

// Limit switch 1 state
#define SENSOR_REG_LIMIT_SWITCH1_STATE   0x0201  // 0=Open, 1=Closed

// Limit switch 2 state
#define SENSOR_REG_LIMIT_SWITCH2_STATE   0x0202  // 0=Open, 1=Closed

// Limit switch 3 state
#define SENSOR_REG_LIMIT_SWITCH3_STATE   0x0203  // 0=Open, 1=Closed

// Limit switch 4 state
#define SENSOR_REG_LIMIT_SWITCH4_STATE   0x0204  // 0=Open, 1=Closed

// Limit switch error code
#define SENSOR_REG_LIMIT_SWITCH_ERROR    0x0205  // 16-bit unsigned
```

### **4. Position Data Registers (0x0300-0x03FF):**

#### **Position Status (0x0300-0x030F):**
```c
// Position status
#define SENSOR_REG_POSITION_STATUS       0x0300  // Bit field

// Position X (0.1mm resolution)
#define SENSOR_REG_POSITION_X            0x0301  // 32-bit signed

// Position Y (0.1mm resolution)
#define SENSOR_REG_POSITION_Y            0x0302  // 32-bit signed

// Position Z (0.1mm resolution)
#define SENSOR_REG_POSITION_Z            0x0303  // 32-bit signed

// Orientation (0.1 degree resolution)
#define SENSOR_REG_ORIENTATION           0x0305  // 16-bit signed

// Position accuracy (mm)
#define SENSOR_REG_POSITION_ACCURACY     0x0306  // 16-bit unsigned

// Position timestamp
#define SENSOR_REG_POSITION_TIMESTAMP    0x0307  // 32-bit unsigned
```

### **5. Fault Status Registers (0x0400-0x04FF):**

#### **Fault Status (0x0400-0x040F):**
```c
// Fault status register
#define SENSOR_REG_FAULT_STATUS          0x0400  // Bit field

// IMU fault
#define SENSOR_REG_IMU_FAULT             0x0401  // 0=OK, 1=Fault

// Magnetic sensor fault
#define SENSOR_REG_MAGNETIC_FAULT        0x0402  // 0=OK, 1=Fault

// Limit switch fault
#define SENSOR_REG_LIMIT_SWITCH_FAULT    0x0403  // 0=OK, 1=Fault

// Position fault
#define SENSOR_REG_POSITION_FAULT        0x0404  // 0=OK, 1=Fault

// Communication fault
#define SENSOR_REG_COMM_FAULT            0x0405  // 0=OK, 1=Fault

// Data validation fault
#define SENSOR_REG_DATA_VALIDATION_FAULT 0x0406  // 0=OK, 1=Fault
```

#### **System Registers (0x00F0-0x00FF):**
```c
// Device ID (Modbus slave address)
#define SENSOR_REG_DEVICE_ID             0x00F0  // 16-bit unsigned

// Firmware version (e.g. 0x0101 = v1.01)
#define SENSOR_REG_FIRMWARE_VERSION      0x00F1  // 16-bit unsigned

// System status (bit field)
#define SENSOR_REG_SYSTEM_STATUS         0x00F2  // 16-bit unsigned

// System error (global error code)
#define SENSOR_REG_SYSTEM_ERROR          0x00F3  // 16-bit unsigned

// Reset error command (write 1 to reset all error flags)
#define SENSOR_REG_RESET_ERROR_CMD       0x00F4  // 16-bit unsigned

// Config baudrate (1=9600, 2=19200, 3=38400,...)
#define SENSOR_REG_CONFIG_BAUDRATE       0x00F5  // 16-bit unsigned

// Config parity (0=None, 1=Even, 2=Odd)
#define SENSOR_REG_CONFIG_PARITY         0x00F6  // 16-bit unsigned

// Module type (0x0005 = Sensor Module)
#define SENSOR_REG_MODULE_TYPE           0x00F7  // 16-bit unsigned

// Module name (low word)
#define SENSOR_REG_MODULE_NAME_LOW       0x00F8  // 16-bit unsigned

// Module name (high word)
#define SENSOR_REG_MODULE_NAME_HIGH      0x00F9  // 16-bit unsigned

// Hardware version
#define SENSOR_REG_HARDWARE_VERSION      0x00FA  // 16-bit unsigned

// Serial number (low word)
#define SENSOR_REG_SERIAL_NUMBER_LOW     0x00FB  // 16-bit unsigned

// Serial number (high word)
#define SENSOR_REG_SERIAL_NUMBER_HIGH    0x00FC  // 16-bit unsigned

// Build date (low word)
#define SENSOR_REG_BUILD_DATE_LOW        0x00FD  // 16-bit unsigned

// Build date (high word)
#define SENSOR_REG_BUILD_DATE_HIGH       0x00FE  // 16-bit unsigned

// Register checksum
#define SENSOR_REG_CHECKSUM              0x00FF  // 16-bit unsigned
```

---

## 🔧 **CONFIGURATION**

### **1. Sensor Module Configuration:**
```json
{
  "sensor_module": {
    "address": "0x05",
    "name": "Sensor Module",
    "type": "sensor",
    "version": "1.0",
    "enabled": true,
    "imu_config": {
      "enabled": true,
      "sample_rate": 100,
      "accelerometer_range": 2,
      "gyroscope_range": 250,
      "temperature_monitoring": true
    },
    "magnetic_config": {
      "enabled": true,
      "sensors": [
        {"id": 1, "enabled": true, "threshold": 100, "type": "hall_effect"},
        {"id": 2, "enabled": true, "threshold": 100, "type": "hall_effect"}
      ]
    },
    "limit_switch_config": {
      "enabled": true,
      "switches": [
        {"id": 1, "enabled": true, "normally_open": true, "function": "safety_stop"},
        {"id": 2, "enabled": true, "normally_open": true, "function": "docking_detect"},
        {"id": 3, "enabled": true, "normally_open": true, "function": "emergency_stop"},
        {"id": 4, "enabled": true, "normally_open": true, "function": "manual_override"}
      ]
    },
    "position_config": {
      "enabled": true,
      "update_rate": 10,
      "accuracy_threshold": 10,
      "fusion_enabled": true
    },
    "safety_config": {
      "enabled": true,
      "data_validation": true,
      "fault_reporting": true,
      "emergency_stop": true
    },
    "system_config": {
      "baudrate": 3,
      "parity": 0,
      "device_id": 5
    },
    "note": "LiDAR is connected via USB interface, not RS485"
  }
}
```

### **2. HAL Configuration:**
```c
// Sensor module HAL configuration
typedef struct {
    uint8_t address;
    uint32_t timeout_ms;
    uint8_t retry_count;
    bool lidar_enabled;
    uint16_t lidar_scan_freq;
    uint16_t lidar_range_min;
    uint16_t lidar_range_max;
    float lidar_angle_resolution;
    bool ultrasonic_enabled;
    uint8_t ultrasonic_sensor_count;
    uint16_t ultrasonic_range_min;
    uint16_t ultrasonic_range_max;
    bool imu_enabled;
    uint16_t imu_sample_rate;
    uint8_t imu_accel_range;
    uint8_t imu_gyro_range;
    bool magnetic_enabled;
    uint8_t magnetic_sensor_count;
    bool limit_switch_enabled;
    uint8_t limit_switch_count;
    bool position_enabled;
    uint16_t position_update_rate;
    uint16_t position_accuracy_threshold;
    bool obstacle_enabled;
    uint16_t obstacle_map_resolution;
    uint16_t obstacle_map_size_x;
    uint16_t obstacle_map_size_y;
    uint16_t obstacle_threshold;
} sensor_module_config_t;
```

---

## 🧪 **TESTING**

### **1. Unit Tests:**
```c
// Sensor module unit tests
void test_sensor_module_init(void);
void test_sensor_module_read_lidar_data(void);
void test_sensor_module_read_ultrasonic_data(void);
void test_sensor_module_read_imu_data(void);
void test_sensor_module_read_magnetic_data(void);
void test_sensor_module_read_limit_switches(void);
void test_sensor_module_get_position_data(void);
void test_sensor_module_get_obstacle_map(void);
void test_sensor_module_read_status(void);
void test_sensor_module_fault_handling(void);
```

### **2. Integration Tests:**
```c
// Sensor module integration tests
void test_sensor_module_data_fusion(void);
void test_sensor_module_obstacle_detection(void);
void test_sensor_module_position_tracking(void);
void test_sensor_module_sensor_calibration(void);
void test_sensor_module_fault_recovery(void);
void test_sensor_module_communication_fault(void);
```

### **3. Performance Tests:**
```c
// Sensor module performance tests
void test_sensor_module_response_time(void);
void test_sensor_module_data_accuracy(void);
void test_sensor_module_update_rate(void);
void test_sensor_module_stability(void);
void test_sensor_module_efficiency(void);
void test_sensor_module_thermal_performance(void);
```

---

## 📊 **MONITORING VÀ DIAGNOSTICS**

### **1. Real-time Monitoring:**
```c
// Sensor module monitoring data
typedef struct {
    lidar_data_t lidar_data;
    ultrasonic_data_t ultrasonic_data;
    imu_data_t imu_data;
    magnetic_data_t magnetic_data;
    limit_switch_data_t limit_switch_data;
    position_data_t position_data;
    obstacle_map_t obstacle_map;
    sensor_fault_status_t faults;
    uint32_t uptime;
    uint32_t communication_errors;
} sensor_module_status_t;
```

### **2. Diagnostic Functions:**
```c
// Sensor module diagnostics
hal_status_t sensor_module_self_test(void);
hal_status_t sensor_module_calibrate_sensors(void);
hal_status_t sensor_module_calibrate_position(void);
hal_status_t sensor_module_reset_faults(void);
hal_status_t sensor_module_get_diagnostics(sensor_diagnostics_t *diag);
hal_status_t sensor_module_export_logs(void);
```

---

## 🚨 **SAFETY VÀ FAULT HANDLING**

### **1. Safety Features:**
- **Obstacle Detection:** Phát hiện chướng ngại vật real-time
- **Position Tracking:** Theo dõi vị trí chính xác
- **Limit Switch Protection:** Bảo vệ giới hạn hành trình
- **Sensor Health Monitoring:** Giám sát sức khỏe cảm biến
- **Data Validation:** Kiểm tra tính hợp lệ dữ liệu
- **Fault Detection:** Phát hiện lỗi cảm biến

### **2. Fault Handling:**
```c
// Sensor module fault handling
typedef enum {
    SENSOR_FAULT_NONE = 0,
    SENSOR_FAULT_LIDAR,
    SENSOR_FAULT_ULTRASONIC,
    SENSOR_FAULT_IMU,
    SENSOR_FAULT_MAGNETIC,
    SENSOR_FAULT_LIMIT_SWITCH,
    SENSOR_FAULT_COMMUNICATION,
    SENSOR_FAULT_DATA_INVALID
} sensor_fault_type_t;

hal_status_t sensor_module_handle_fault(sensor_fault_type_t fault);
hal_status_t sensor_module_clear_fault(sensor_fault_type_t fault);
hal_status_t sensor_module_get_fault_status(sensor_fault_status_t *status);
```

---

## 📈 **PERFORMANCE METRICS**

### **1. Key Performance Indicators:**
- **IMU Accuracy:** ±0.1° for orientation, ±0.01g for acceleration
- **Magnetic Sensor Range:** 0-1000 Gauss
- **Magnetic Sensor Accuracy:** ±1% full scale
- **Position Accuracy:** ±1cm (with sensor fusion)
- **Update Rate:** 100Hz for IMU, 10Hz for position
- **Response Time:** < 10ms for IMU, < 100ms for position
- **Temperature Range:** -40°C to +85°C

### **2. Monitoring Metrics:**
- **IMU Data:** Real-time accelerometer, gyroscope, temperature
- **Magnetic Data:** Real-time magnetic field strength
- **Position:** Real-time position và orientation
- **Limit Switches:** Real-time safety switch status
- **Fault Detection:** Real-time fault monitoring
- **Communication:** RS485 communication status
- **Data Quality:** Sensor data quality metrics
- **Sensor Fusion:** Combined position accuracy

---

## 🔄 **MAINTENANCE**

### **1. Regular Maintenance:**
- **Daily:** Check sensor status và data quality
- **Weekly:** Verify sensor calibration
- **Monthly:** Calibrate position sensors
- **Quarterly:** Full system diagnostics
- **Annually:** Sensor performance assessment

### **2. Troubleshooting:**
- **Communication Issues:** Check RS485 wiring và termination
- **Sensor Issues:** Check sensor connections và calibration
- **Data Issues:** Verify sensor parameters và settings
- **Fault Issues:** Review fault logs và clear faults
- **Accuracy Issues:** Check sensor alignment và calibration

---

**Changelog:**
- v1.0 (2025-01-27): Initial Sensor Module integration guide
- v1.1 (2025-01-27): Focused on IMU and Magnetic sensors (LiDAR via USB)
- v1.2 (2025-01-27): Removed GPIO_DE pin (not needed for RS485)
- v1.3 (2025-01-27): Added System Registers (0x00F0-0x00FF) for auto-detect compatibility
