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
- Thu thập dữ liệu từ cảm biến gia tốc 3 trục (MMA7660FC)
- Xử lý dữ liệu từ 4x DI công tắc từ
- Xử lý dữ liệu RFID/NFC reader
- **Lưu ý:** LiDAR kết nối qua USB trực tiếp Master Module, không qua RS485

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
| **0x0000** | Accelerometer X | Input | mg | Gia tốc X (mg) | R | ✅ Hoàn thành |
| **0x0001** | Accelerometer Y | Input | mg | Gia tốc Y (mg) | R | ✅ Hoàn thành |
| **0x0002** | Accelerometer Z | Input | mg | Gia tốc Z (mg) | R | ✅ Hoàn thành |
| **0x0003** | Accelerometer Status | Input | - | Trạng thái cảm biến gia tốc | R | ✅ Hoàn thành |
| **0x0004** | Accelerometer Error Code | Input | - | Mã lỗi cảm biến gia tốc | R | ✅ Hoàn thành |
| **0x0010** | DI 1 (Magnetic Switch) | Input | - | Trạng thái công tắc từ 1 | R | ✅ Hoàn thành |
| **0x0011** | DI 2 (Magnetic Switch) | Input | - | Trạng thái công tắc từ 2 | R | ✅ Hoàn thành |
| **0x0012** | DI 3 (Magnetic Switch) | Input | - | Trạng thái công tắc từ 3 | R | ✅ Hoàn thành |
| **0x0013** | DI 4 (Magnetic Switch) | Input | - | Trạng thái công tắc từ 4 | R | ✅ Hoàn thành |
| **0x0014** | DI Status | Input | - | Trạng thái tổng hợp DI | R | ✅ Hoàn thành |
| **0x0015** | DI Error Code | Input | - | Mã lỗi DI | R | ✅ Hoàn thành |
| **0x0020** | RFID/NFC Data Low | Input | - | Dữ liệu RFID/NFC (word thấp) | R | ✅ Hoàn thành |
| **0x0021** | RFID/NFC Data High | Input | - | Dữ liệu RFID/NFC (word cao) | R | ✅ Hoàn thành |
| **0x0022** | RFID/NFC Status | Input | - | Trạng thái RFID/NFC | R | ✅ Hoàn thành |
| **0x0023** | RFID/NFC Error Code | Input | - | Mã lỗi RFID/NFC | R | ✅ Hoàn thành |
| **0x0024** | RFID/NFC Card Type | Input | - | Loại thẻ RFID/NFC | R | ✅ Hoàn thành |
| **0x0025** | RFID/NFC Card ID | Input | - | ID thẻ RFID/NFC | R | ✅ Hoàn thành |
| **0x0030** | System Status | Input | - | Trạng thái hệ thống | R | ✅ Hoàn thành |
| **0x0031** | System Error | Input | - | Mã lỗi hệ thống | R | ✅ Hoàn thành |
| **0x0060** | Accelerometer Sample Rate | Holding | Hz | Tần suất lấy mẫu gia tốc | R/W | ✅ Hoàn thành |
| **0x0061** | DI Debounce Time | Holding | ms | Thời gian debounce DI | R/W | ✅ Hoàn thành |
| **0x0062** | RFID Read Timeout | Holding | ms | Timeout đọc RFID | R/W | ✅ Hoàn thành |
| **0x0063** | Data Validation | Holding | - | Xác thực dữ liệu | R/W | ✅ Hoàn thành |
| **0x0064** | Fault Reporting | Holding | - | Báo cáo lỗi | R/W | ✅ Hoàn thành |
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
// Sensor Module specific functions (Accelerometer + DI + RFID/NFC)
hal_status_t sensor_module_init(void);
hal_status_t sensor_module_read_accelerometer_data(accelerometer_data_t *data);
hal_status_t sensor_module_read_di_data(di_data_t *data);
hal_status_t sensor_module_read_rfid_data(rfid_data_t *data);
hal_status_t sensor_module_read_status(sensor_status_t *status);

// Note: LiDAR data is handled via USB interface directly to Master Module, not RS485
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
    "accelerometer_config": {
      "enabled": true,
      "sample_rate": 100,
      "range": 1.5,
      "sensitivity": 21,
      "ic": "MMA7660FC"
    },
    "di_config": {
      "enabled": true,
      "inputs": [
        {"id": 1, "enabled": true, "type": "magnetic_switch", "function": "docking_detect"},
        {"id": 2, "enabled": true, "type": "magnetic_switch", "function": "safety_stop"},
        {"id": 3, "enabled": true, "type": "magnetic_switch", "function": "emergency_stop"},
        {"id": 4, "enabled": true, "type": "magnetic_switch", "function": "manual_override"}
      ]
    },
    "rfid_config": {
      "enabled": true,
      "reader_type": "rfid_nfc",
      "supported_cards": ["ISO14443A", "ISO14443B", "ISO15693"],
      "read_timeout": 1000,
      "auto_read": true
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
    bool accelerometer_enabled;
    uint16_t accelerometer_sample_rate;
    uint8_t accelerometer_range;
    uint8_t accelerometer_sensitivity;
    bool di_enabled;
    uint8_t di_input_count;
    bool rfid_enabled;
    uint8_t rfid_reader_type;
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
- v1.4 (2025-01-27): Updated hardware requirements - IMU + 4x DI magnetic switches + RFID/NFC reader (LiDAR via USB direct to Master)
- v1.5 (2025-01-27): Simplified to 1x Accelerometer (MMA7660FC) + 4x DI magnetic switches + 1x RFID/NFC reader
- v1.6 (2025-01-28): Removed System Temperature register (0x0030) - not needed for real-time operation
