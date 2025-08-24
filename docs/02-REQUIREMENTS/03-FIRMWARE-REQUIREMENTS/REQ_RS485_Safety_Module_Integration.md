# HƯỚNG DẪN KẾT NỐI VÀ ĐIỀU KHIỂN SAFETY MODULE QUA RS485
## OHT-50 Master Module - Safety Module Integration

**Phiên bản:** v2.0  
**Ngày tạo:** 2025-01-28  
**Team:** FW Team  
**Trạng thái:** Implementation Guide - Updated for Real Requirements  

---

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với Safety Module qua RS485
- Giám sát an toàn bằng 4 cảm biến khoảng cách (Analog)
- Phát hiện vật cản bằng 4 cảm biến tiệm cận (Digital)
- Kích hoạt các chức năng an toàn qua 2-4 relay outputs
- Tích hợp với hệ thống E-Stop và safety logic

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| **Tổng quan** | Hiểu kiến trúc Safety Module | FW Developer | Safety Integration | ✅ Hoàn thành |
| **Phần cứng và kết nối** | Cấu hình phần cứng | HW Engineer | Wiring, Pinout | ✅ Hoàn thành |
| **Thông số kỹ thuật RS485** | Định nghĩa protocol | Network Engineer | RS485, Modbus | ✅ Hoàn thành |
| **Kết nối phần cứng** | Wiring diagram | HW Engineer | Cable Connection | ✅ Hoàn thành |
| **Cấu hình GPIO** | Pin assignment | HW Engineer | GPIO Setup | ✅ Hoàn thành |
| **Implementation Software** | Code implementation | FW Developer | HAL, Drivers | ✅ Hoàn thành |
| **HAL RS485 Driver** | Driver functions | FW Developer | HAL API | ✅ Hoàn thành |
| **Safety Manager** | Safety control logic | FW Developer | Safety Logic | ✅ Hoàn thành |
| **Analog Inputs** | Cảm biến khoảng cách | FW Developer | Distance Sensors | ✅ Hoàn thành |
| **Digital Inputs** | Cảm biến tiệm cận | FW Developer | Proximity Sensors | ✅ Hoàn thành |
| **Relay Outputs** | Kích hoạt chức năng an toàn | FW Developer | Safety Actions | ✅ Hoàn thành |
| **Safety Logic** | Logic an toàn | Safety Engineer | Safety Rules | ✅ Hoàn thành |
| **Error Handling** | Error handling logic | FW Developer | Error Recovery | ✅ Hoàn thành |
| **Testing Procedures** | Testing guidelines | Test Engineer | Test Cases | ✅ Hoàn thành |
| **Troubleshooting** | Debug procedures | Support Engineer | Debug Guide | ✅ Hoàn thành |
| **Performance Optimization** | Performance tuning | Performance Engineer | Optimization | ✅ Hoàn thành |
| **Calibration** | Calibration procedures | FW Developer | Calibration | ✅ Hoàn thành |

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │  Safety Module  │
│  (Orange Pi 5B) │             │   (Controller)  │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ Safety      │ │
│ │ Driver      │ │             │ │ Controller  │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ Safety Mgr  │ │             │ │ 4x Analog   │ │
│ │ Application │ │             │ │ Distance    │ │
│ └─────────────┘ │             │ │ Sensors     │ │
└─────────────────┘             │ └─────────────┘ │
                                │ ┌─────────────┐ │
                                │ │ 4x Digital  │ │
                                │ │ Proximity   │ │
                                │ │ Sensors     │ │
                                │ └─────────────┘ │
                                │ ┌─────────────┐ │
                                │ │ 2-4x Relay  │ │
                                │ │ Outputs     │ │
                                └─────────────────┘
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

#### **Safety Module:**
- **Protocol:** Modbus RTU
- **Slave Address:** 0x03 (cố định cho Safety Module)
- **Response Time:** < 50ms (critical for safety)
- **Error Detection:** CRC-16

### **2. Kết nối phần cứng:**

#### **Wiring Diagram:**
```
Orange Pi 5B          RS485 Bus          Safety Module
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

## 📊 **BẢNG MAPPING REGISTER MODBUS - SAFETY MODULE:**

| **Register Address** | **Tên Register** | **Loại** | **Đơn vị** | **Mô tả** | **Access** | **Trạng thái** |
|---------------------|------------------|----------|------------|-----------|------------|----------------|
| **0x0000** | Safety System Status | Input | - | Trạng thái hệ thống an toàn | R | ✅ Hoàn thành |
| **0x0001** | Emergency Stop Status | Input | - | Trạng thái E-Stop (0=Normal, 1=Pressed) | R | ✅ Hoàn thành |
| **0x0002** | Safety Zone Status | Input | - | Trạng thái vùng an toàn (bit field) | R | ✅ Hoàn thành |
| **0x0003** | Proximity Alert Status | Input | - | Trạng thái cảnh báo tiệm cận (bit field) | R | ✅ Hoàn thành |
| **0x0004** | Relay Output Status | Input | - | Trạng thái relay outputs (bit field) | R | ✅ Hoàn thành |
| **0x0005** | Safety Error Code | Input | - | Mã lỗi an toàn | R | ✅ Hoàn thành |
| **0x0006** | System Temperature | Input | °C × 10 | Nhiệt độ hệ thống | R | ✅ Hoàn thành |
| **0x0007** | System Voltage | Input | V × 10 | Điện áp hệ thống | R | ✅ Hoàn thành |
| **0x0010** | Analog Input 1 | Input | mm | Khoảng cách cảm biến 1 (mm) | R | ✅ Hoàn thành |
| **0x0011** | Analog Input 2 | Input | mm | Khoảng cách cảm biến 2 (mm) | R | ✅ Hoàn thành |
| **0x0012** | Analog Input 3 | Input | mm | Khoảng cách cảm biến 3 (mm) | R | ✅ Hoàn thành |
| **0x0013** | Analog Input 4 | Input | mm | Khoảng cách cảm biến 4 (mm) | R | ✅ Hoàn thành |
| **0x0014** | Analog Input 1 Raw | Input | ADC | Giá trị ADC thô cảm biến 1 | R | ✅ Hoàn thành |
| **0x0015** | Analog Input 2 Raw | Input | ADC | Giá trị ADC thô cảm biến 2 | R | ✅ Hoàn thành |
| **0x0016** | Analog Input 3 Raw | Input | ADC | Giá trị ADC thô cảm biến 3 | R | ✅ Hoàn thành |
| **0x0017** | Analog Input 4 Raw | Input | ADC | Giá trị ADC thô cảm biến 4 | R | ✅ Hoàn thành |
| **0x0020** | Digital Input Status | Input | - | Trạng thái 4 digital inputs (bit field) | R | ✅ Hoàn thành |
| **0x0021** | DI1 Status | Input | - | Trạng thái DI1 (0=Clear, 1=Detected) | R | ✅ Hoàn thành |
| **0x0022** | DI2 Status | Input | - | Trạng thái DI2 (0=Clear, 1=Detected) | R | ✅ Hoàn thành |
| **0x0023** | DI3 Status | Input | - | Trạng thái DI3 (0=Clear, 1=Detected) | R | ✅ Hoàn thành |
| **0x0024** | DI4 Status | Input | - | Trạng thái DI4 (0=Clear, 1=Detected) | R | ✅ Hoàn thành |
| **0x0030** | Relay Output Control | Holding | - | Điều khiển 4 relay outputs (bit field) | R/W | ✅ Hoàn thành |
| **0x0031** | Relay 1 Control | Holding | - | Điều khiển Relay 1 (0=Off, 1=On) | R/W | ✅ Hoàn thành |
| **0x0032** | Relay 2 Control | Holding | - | Điều khiển Relay 2 (0=Off, 1=On) | R/W | ✅ Hoàn thành |
| **0x0033** | Relay 3 Control | Holding | - | Điều khiển Relay 3 (0=Off, 1=On) | R/W | ✅ Hoàn thành |
| **0x0034** | Relay 4 Control | Holding | - | Điều khiển Relay 4 (0=Off, 1=On) | R/W | ✅ Hoàn thành |
| **0x0040** | Safety Zone 1 Threshold | Holding | mm | Ngưỡng vùng an toàn 1 (mm) | R/W | ✅ Hoàn thành |
| **0x0041** | Safety Zone 2 Threshold | Holding | mm | Ngưỡng vùng an toàn 2 (mm) | R/W | ✅ Hoàn thành |
| **0x0042** | Safety Zone 3 Threshold | Holding | mm | Ngưỡng vùng an toàn 3 (mm) | R/W | ✅ Hoàn thành |
| **0x0043** | Safety Zone 4 Threshold | Holding | mm | Ngưỡng vùng an toàn 4 (mm) | R/W | ✅ Hoàn thành |
| **0x0050** | Proximity Threshold | Holding | - | Ngưỡng cảm biến tiệm cận | R/W | ✅ Hoàn thành |
| **0x0051** | Safety Response Time | Holding | ms | Thời gian phản ứng an toàn (ms) | R/W | ✅ Hoàn thành |
| **0x0052** | Auto Reset Enable | Holding | - | Bật/tắt tự động reset (0=Disable, 1=Enable) | R/W | ✅ Hoàn thành |
| **0x0053** | Safety Mode | Holding | - | Chế độ an toàn (0=Normal, 1=High, 2=Critical) | R/W | ✅ Hoàn thành |
| **0x0060** | Analog Sensor 1 Type | Holding | - | Loại cảm biến analog 1 | R/W | ✅ Hoàn thành |
| **0x0061** | Analog Sensor 2 Type | Holding | - | Loại cảm biến analog 2 | R/W | ✅ Hoàn thành |
| **0x0062** | Analog Sensor 3 Type | Holding | - | Loại cảm biến analog 3 | R/W | ✅ Hoàn thành |
| **0x0063** | Analog Sensor 4 Type | Holding | - | Loại cảm biến analog 4 | R/W | ✅ Hoàn thành |
| **0x0070** | Digital Sensor 1 Type | Holding | - | Loại cảm biến digital 1 | R/W | ✅ Hoàn thành |
| **0x0071** | Digital Sensor 2 Type | Holding | - | Loại cảm biến digital 2 | R/W | ✅ Hoàn thành |
| **0x0072** | Digital Sensor 3 Type | Holding | - | Loại cảm biến digital 3 | R/W | ✅ Hoàn thành |
| **0x0073** | Digital Sensor 4 Type | Holding | - | Loại cảm biến digital 4 | R/W | ✅ Hoàn thành |
| **0x0080** | Relay 1 Function | Holding | - | Chức năng Relay 1 | R/W | ✅ Hoàn thành |
| **0x0081** | Relay 2 Function | Holding | - | Chức năng Relay 2 | R/W | ✅ Hoàn thành |
| **0x0082** | Relay 3 Function | Holding | - | Chức năng Relay 3 | R/W | ✅ Hoàn thành |
| **0x0083** | Relay 4 Function | Holding | - | Chức năng Relay 4 | R/W | ✅ Hoàn thành |
| **0x00F0** | Device ID | Input | - | ID thiết bị (Modbus slave address) | R | ✅ Hoàn thành |
| **0x00F1** | Firmware Version | Input | - | Phiên bản firmware | R | ✅ Hoàn thành |
| **0x00F2** | System Status | Input | - | Trạng thái hệ thống (bit field) | R | ✅ Hoàn thành |
| **0x00F3** | System Error | Input | - | Lỗi hệ thống (global error code) | R | ✅ Hoàn thành |
| **0x00F4** | Reset Error Command | Holding | - | Lệnh reset lỗi | W | ✅ Hoàn thành |
| **0x00F5** | Config Baudrate | Holding | - | Cấu hình baudrate | R/W | ✅ Hoàn thành |
| **0x00F6** | Config Parity | Holding | - | Cấu hình parity | R/W | ✅ Hoàn thành |
| **0x00F7** | Module Type | Input | - | Loại module (0x0003 = Safety Module) | R | ✅ Hoàn thành |
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

## 🔧 **CHỨC NĂNG CHI TIẾT**

### **1. Analog Inputs (Cảm biến khoảng cách):**
- **4 kênh analog** (0x0010-0x0013): Đo khoảng cách từ 0-5000mm
- **Raw ADC values** (0x0014-0x0017): Giá trị ADC thô để calibration
- **Configurable thresholds** (0x0040-0x0043): Ngưỡng cảnh báo cho từng zone
- **Sensor types** (0x0060-0x0063): Loại cảm biến (ultrasonic, LiDAR, etc.)

### **2. Digital Inputs (Cảm biến tiệm cận):**
- **4 kênh digital** (0x0020-0x0024): Phát hiện vật cản gần
- **Proximity threshold** (0x0050): Ngưỡng phát hiện tiệm cận
- **Sensor types** (0x0070-0x0073): Loại cảm biến (inductive, capacitive, etc.)

### **3. Relay Outputs (Kích hoạt chức năng an toàn):**
- **4 relay outputs** (0x0030-0x0034): Kích hoạt các chức năng an toàn
- **Function assignment** (0x0080-0x0083): Gán chức năng cho từng relay
- **Safety actions:** Phanh khẩn cấp, đèn cảnh báo, E-Stop, etc.

### **4. Safety Logic:**
- **Zone monitoring:** Giám sát 4 vùng an toàn bằng cảm biến khoảng cách
- **Proximity detection:** Phát hiện vật cản gần bằng cảm biến tiệm cận
- **Automatic response:** Tự động kích hoạt relay khi phát hiện nguy hiểm
- **Configurable response time** (0x0051): Thời gian phản ứng an toàn

---

## 🧪 **TESTING**

### **1. Hardware Testing:**
```bash
# Test RS485 communication
python3 test_safety_module.py --address 0x03 --test communication

# Test analog inputs
python3 test_safety_module.py --address 0x03 --test analog

# Test digital inputs
python3 test_safety_module.py --address 0x03 --test digital

# Test relay outputs
python3 test_safety_module.py --address 0x03 --test relay
```

### **2. Safety Testing:**
```bash
# Test safety zone monitoring
python3 test_safety_module.py --address 0x03 --test safety_zones

# Test proximity detection
python3 test_safety_module.py --address 0x03 --test proximity

# Test emergency response
python3 test_safety_module.py --address 0x03 --test emergency
```

---

## 🔧 **CONFIGURATION**

### **1. Safety Module Configuration:**
```json
{
  "safety_module": {
    "address": 3,
    "baudrate": 115200,
    "parity": "none",
    "safety_zones": {
      "zone_1": {
        "threshold_mm": 1000,
        "sensor_type": "ultrasonic",
        "enabled": true
      },
      "zone_2": {
        "threshold_mm": 500,
        "sensor_type": "lidar",
        "enabled": true
      },
      "zone_3": {
        "threshold_mm": 200,
        "sensor_type": "ultrasonic",
        "enabled": true
      },
      "zone_4": {
        "threshold_mm": 100,
        "sensor_type": "lidar",
        "enabled": true
      }
    },
    "proximity_sensors": {
      "sensor_1": {
        "type": "inductive",
        "threshold": 50,
        "enabled": true
      },
      "sensor_2": {
        "type": "capacitive",
        "threshold": 30,
        "enabled": true
      },
      "sensor_3": {
        "type": "inductive",
        "threshold": 50,
        "enabled": true
      },
      "sensor_4": {
        "type": "capacitive",
        "threshold": 30,
        "enabled": true
      }
    },
    "relay_outputs": {
      "relay_1": {
        "function": "emergency_brake",
        "enabled": true
      },
      "relay_2": {
        "function": "warning_light",
        "enabled": true
      },
      "relay_3": {
        "function": "horn",
        "enabled": true
      },
      "relay_4": {
        "function": "estop",
        "enabled": true
      }
    },
    "safety_settings": {
      "response_time_ms": 50,
      "auto_reset_enabled": false,
      "safety_mode": 1
    }
  }
}
```

---

## 🚨 **SAFETY CONSIDERATIONS**

### **1. Critical Safety Features:**
- **Fast response time:** < 50ms cho safety actions
- **Redundant monitoring:** Multiple sensors cho critical zones
- **Fail-safe operation:** Default to safe state khi có lỗi
- **Manual override:** Khả năng override trong trường hợp khẩn cấp

### **2. Safety Zones:**
- **Zone 1 (1000mm):** Early warning zone
- **Zone 2 (500mm):** Caution zone
- **Zone 3 (200mm):** Warning zone
- **Zone 4 (100mm):** Critical zone

### **3. Emergency Actions:**
- **Emergency brake:** Dừng khẩn cấp
- **Warning lights:** Đèn cảnh báo
- **Horn:** Còi cảnh báo
- **E-Stop:** Dừng khẩn cấp toàn hệ thống

---

**Changelog:**
- ✅ v1.0 (2025-01-27): Initial Safety Module integration guide
- ✅ v2.0 (2025-01-28): Updated for real requirements (4 analog + 4 digital + 4 relay)
- ✅ Added detailed register mapping for all I/O channels
- ✅ Added safety logic and configuration examples
- ✅ Added testing procedures and safety considerations
