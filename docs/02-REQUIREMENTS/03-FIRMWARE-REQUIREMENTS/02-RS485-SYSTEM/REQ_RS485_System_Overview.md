# TỔNG QUAN HỆ THỐNG RS485 OHT-50
## OHT-50 Master Module - System Overview

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-27  
**Team:** FW Team  
**Trạng thái:** System Overview  

---

## 🎯 **TỔNG QUAN HỆ THỐNG**

### **Mục tiêu:**
- Tổng quan kiến trúc hệ thống RS485 OHT-50
- Mô tả các module và chức năng chính
- Hướng dẫn tích hợp và cấu hình hệ thống
- Thống nhất giao thức giao tiếp giữa các module

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| **Tổng quan hệ thống** | Hiểu kiến trúc tổng thể | System Architect | System Design | ✅ Hoàn thành |
| **Danh sách module** | Liệt kê tất cả module | FW Developer | Module Integration | ✅ Hoàn thành |
| **Thông số kỹ thuật** | Định nghĩa protocol | Network Engineer | RS485, Modbus | ✅ Hoàn thành |
| **Hardware Interface** | Giao diện phần cứng | HW Engineer | UART, GPIO | ✅ Hoàn thành |
| **Software Architecture** | Kiến trúc phần mềm | FW Developer | HAL, Drivers | ✅ Hoàn thành |
| **Communication Protocol** | Giao thức giao tiếp | Protocol Engineer | Modbus RTU | ✅ Hoàn thành |
| **Error Handling** | Xử lý lỗi | FW Developer | Error Recovery | ✅ Hoàn thành |
| **Performance Metrics** | Chỉ số hiệu suất | Performance Engineer | Latency, Throughput | ✅ Hoàn thành |
| **Safety Considerations** | Yếu tố an toàn | Safety Engineer | E-Stop, Fault | ✅ Hoàn thành |
| **Integration Guide** | Hướng dẫn tích hợp | Integration Engineer | Module Setup | ✅ Hoàn thành |
| **Configuration** | Cấu hình hệ thống | System Admin | Settings, Parameters | ✅ Hoàn thành |
| **Testing Strategy** | Chiến lược kiểm thử | QA Engineer | Test Cases | ✅ Hoàn thành |
| **Maintenance** | Bảo trì hệ thống | Maintenance Engineer | Troubleshooting | ✅ Hoàn thành |

### **Kiến trúc tổng thể:**
```
┌─────────────────────────────────────────────────────────┐
│                    OHT-50 SYSTEM                        │
├─────────────────────────────────────────────────────────┤
│  🎨 Frontend (Dashboard)                                │
│  └─ Real-time monitoring, configuration, control        │
├─────────────────────────────────────────────────────────┤
│  🌐 Backend (API Services)                              │
│  └─ REST API, WebSocket, module management, security    │
├─────────────────────────────────────────────────────────┤
│  ⚙️ Master Module (Orange Pi 5B)                        │
│  └─ State machine, safety, communication, control       │
├─────────────────────────────────────────────────────────┤
│  📡 RS485 Bus Network                                   │
│  ├─ Power Module (0x02)                                 │
│  ├─ Safety Module (0x03)                                │
│  ├─ Travel Motor Module (0x04)                          │
│  ├─ Dock & Location Module (0x05)                       │
│  ├─ Lifter Motor Module (0x06)                          │
│  ├─ Cargo Door Module (0x07)                            │
│  ├─ Safety Extended Module (0x08)                       │
│  ├─ RFID Reader Module (0x09)                           │
│  ├─ Camera Module (0x0A)                                │
│  ├─ Environmental Sensor Module (0x0B)                  │
│  └─ Custom Modules (0x0C+)                              │
└─────────────────────────────────────────────────────────┘
```

---

## 📋 **DANH SÁCH MODULE**

### **1. POWER MODULE (0x02) - BẮT BUỘC**
- **Chức năng:** Quản lý nguồn điện, pin, charger
- **Tính năng:** Battery monitoring, power distribution, safety protection
- **Tài liệu:** `REQ_RS485_Power_Module_Integration.md`

### **2. SAFETY MODULE (0x03) - BẮT BUỘC**
- **Chức năng:** Hệ thống an toàn, E-Stop, light curtain
- **Tính năng:** E-Stop monitoring, safety sensors, emergency brake
- **Tài liệu:** `REQ_RS485_Safety_Module_Integration.md`

### **3. TRAVEL MOTOR MODULE (0x04) - BẮT BUỘC**
- **Chức năng:** Điều khiển động cơ DC di chuyển
- **Tính năng:** Differential drive, speed control, torque control
- **Tài liệu:** `REQ_RS485_Travel_Motor_Integration.md`

### **4. DOCK & LOCATION MODULE (0x05) - BẮT BUỘC**
- **Chức năng:** Định vị và docking
- **Tính năng:** IMU, Magnetic sensors, RFID, GPS, LiDAR
- **Tài liệu:** `REQ_RS485_Dock_Location_Integration.md`

### **5. LIFTER MOTOR MODULE (0x06) - TÙY CHỌN**
- **Chức năng:** Điều khiển động cơ nâng hạ
- **Tính năng:** Linear actuator, load cell, position control
- **Tài liệu:** `REQ_RS485_Lifter_Motor_Integration.md`

### **6. CARGO DOOR MODULE (0x07) - TÙY CHỌN**
- **Chức năng:** Điều khiển cửa hàng hóa
- **Tính năng:** Door actuator, position sensor, cargo detection
- **Tài liệu:** `REQ_RS485_Cargo_Door_Integration.md`

### **7. SAFETY EXTENDED MODULE (0x08) - TÙY CHỌN**
- **Chức năng:** Mở rộng hệ thống an toàn
- **Tính năng:** Additional safety sensors, zone monitoring
- **Tài liệu:** `REQ_RS485_Safety_Extended_Integration.md`

### **8. RFID READER MODULE (0x09) - PLUG-AND-PLAY**
- **Chức năng:** Đọc RFID tags
- **Tính năng:** Tag reading, station identification, cargo tracking
- **Tài liệu:** `REQ_RS485_RFID_Reader_Integration.md`

### **9. CAMERA MODULE (0x0A) - PLUG-AND-PLAY**
- **Chức năng:** Hệ thống camera
- **Tính năng:** Vision system, object detection, documentation
- **Tài liệu:** `REQ_RS485_Camera_Integration.md`

### **10. ENVIRONMENTAL SENSOR MODULE (0x0B) - PLUG-AND-PLAY**
- **Chức năng:** Cảm biến môi trường
- **Tính năng:** Temperature, humidity, air quality, environmental monitoring
- **Tài liệu:** `REQ_RS485_Environmental_Sensor_Integration.md`

### **11. CUSTOM MODULES (0x0C-0xFF) - PLUG-AND-PLAY**
- **Chức năng:** Module tùy chỉnh theo yêu cầu
- **Tính năng:** Flexible configuration, custom protocols
- **Tài liệu:** `REQ_RS485_Custom_Module_Integration.md`

---

## 🔧 **THÔNG SỐ KỸ THUẬT CHUNG**

### **1. RS485 Communication:**
```
Protocol: Modbus RTU
Baud Rate: 115200 bps
Data Bits: 8
Stop Bits: 1
Parity: None
Flow Control: None
Address Range: 0x02 - 0xFF
Response Time: < 100ms
Error Detection: CRC-16
```

### **2. Hardware Interface:**
```
Master Module: Orange Pi 5B
UART Interface: UART1 (/dev/ttyS1)
RS485 Transceiver: MAX485
Pin Assignment:
├── UART1_TX: GPIO1_A0 (Pin 8)
├── UART1_RX: GPIO1_A1 (Pin 10)

└── GND:      Common ground
```

### **3. Network Topology:**
```
Star Topology with RS485 Bus
├── Master Module (Orange Pi 5B)
├── Power Module (0x02) - BẮT BUỘC
├── Safety Module (0x03) - BẮT BUỘC
├── Travel Motor Module (0x04) - BẮT BUỘC
├── Dock & Location Module (0x05) - BẮT BUỘC
├── Lifter Motor Module (0x06) - TÙY CHỌN
├── Cargo Door Module (0x07) - TÙY CHỌN
├── Safety Extended Module (0x08) - TÙY CHỌN
├── RFID Reader Module (0x09) - PLUG-AND-PLAY
├── Camera Module (0x0A) - PLUG-AND-PLAY
├── Environmental Sensor Module (0x0B) - PLUG-AND-PLAY
└── Custom Modules (0x0C-0xFF) - PLUG-AND-PLAY
```

---

## 📡 **GIAO THỨC GIAO TIẾP**

### **1. Modbus RTU Frame Format:**
```
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┐
│ Address │ Function│ Data    │ Data    │ ...     │ CRC     │
│ (1 byte)│ (1 byte)│ (N bytes)│ (N bytes)│ (N bytes)│ (2 bytes)│
└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┘
```

### **2. Common Functions:**
```
0x03: Read Holding Registers
0x04: Read Input Registers
0x06: Write Single Register
0x10: Write Multiple Registers
0x14: Read File Record
0x15: Write File Record
0x16: Mask Write Register
0x17: Read/Write Multiple Registers
```

### **3. Register Map Structure:**
```
Each Module has dedicated register ranges:
├── 0x0000-0x00FF: Module Information
├── 0x0100-0x01FF: Control Registers
├── 0x0200-0x02FF: Status Registers
├── 0x0300-0x03FF: Configuration
├── 0x0400-0x04FF: Fault Status
└── 0x0500-0x05FF: Reserved
```

---

## 🔧 **CẤU HÌNH HỆ THỐNG**

### **1. Master Module Configuration:**
```json
{
  "master_module": {
    "name": "OHT-50 Master Module",
    "version": "1.0",
    "rs485_config": {
      "device": "/dev/ttyS1",
      "baud_rate": 115200,
      "data_bits": 8,
      "stop_bits": 1,
      "parity": "none",
      "timeout_ms": 1000,
      "retry_count": 3
    },
    "auto_discovery": {
      "enabled": true,
      "scan_interval": 30,
      "address_range": [2, 255]
    },
    "modules": [
      {
        "address": "0x02",
        "type": "power",
        "name": "Power Module",
        "enabled": true
      },
      {
        "address": "0x03",
        "type": "stepper",
        "name": "Stepper Motor Module",
        "enabled": true
      },
      {
        "address": "0x04",
        "type": "dc_motor",
        "name": "DC Motor Module",
        "enabled": true
      },
      {
        "address": "0x05",
        "type": "sensor",
        "name": "Sensor Module",
        "enabled": true
      }
    ]
  }
}
```

### **2. System Integration:**
```c
// System initialization
hal_status_t system_init(void);
hal_status_t system_discover_modules(void);
hal_status_t system_configure_modules(void);
hal_status_t system_start_operation(void);
hal_status_t system_stop_operation(void);

// Module management
hal_status_t module_register(uint8_t address, module_type_t type);
hal_status_t module_unregister(uint8_t address);
hal_status_t module_get_status(uint8_t address, module_status_t *status);
hal_status_t module_send_command(uint8_t address, module_command_t *cmd);
```

---

## 🧪 **TESTING STRATEGY**

### **1. Unit Testing:**
```
├── Module Communication Tests
├── Register Read/Write Tests
├── Error Handling Tests
├── Timeout Tests
├── CRC Validation Tests
└── Protocol Compliance Tests
```

### **2. Integration Testing:**
```
├── Multi-module Communication
├── System Initialization
├── Auto-discovery Testing
├── Fault Recovery Testing
├── Performance Testing
└── Stress Testing
```

### **3. System Testing:**
```
├── End-to-end Functionality
├── Real-world Scenarios
├── Safety System Testing
├── Performance Validation
├── Reliability Testing
└── User Acceptance Testing
```

---

## 📊 **MONITORING VÀ DIAGNOSTICS**

### **1. System Health Monitoring:**
```c
typedef struct {
    uint32_t total_modules;
    uint32_t online_modules;
    uint32_t offline_modules;
    uint32_t communication_errors;
    uint32_t system_uptime;
    system_status_t system_status;
    module_status_t module_status[MAX_MODULES];
} system_health_t;
```

### **2. Diagnostic Functions:**
```c
// System diagnostics
hal_status_t system_self_test(void);
hal_status_t system_health_check(void);
hal_status_t system_diagnostics_report(void);
hal_status_t system_log_export(void);
hal_status_t system_config_backup(void);
hal_status_t system_config_restore(void);
```

---

## 🚨 **SAFETY VÀ FAULT HANDLING**

### **1. Safety Features:**
- **Communication Monitoring:** Real-time communication health check
- **Module Health Monitoring:** Individual module status monitoring
- **Fault Detection:** Automatic fault detection and reporting
- **Emergency Stop:** System-wide emergency stop capability
- **Fail-safe Operation:** Graceful degradation on module failure

### **2. Fault Handling:**
```c
typedef enum {
    SYSTEM_FAULT_NONE = 0,
    SYSTEM_FAULT_COMMUNICATION,
    SYSTEM_FAULT_MODULE_FAILURE,
    SYSTEM_FAULT_CONFIGURATION,
    SYSTEM_FAULT_POWER,
    SYSTEM_FAULT_SAFETY,
    SYSTEM_FAULT_EMERGENCY_STOP
} system_fault_type_t;

hal_status_t system_handle_fault(system_fault_type_t fault);
hal_status_t system_clear_fault(system_fault_type_t fault);
hal_status_t system_get_fault_status(system_fault_status_t *status);
```

---

## 📈 **PERFORMANCE METRICS**

### **1. System Performance:**
- **Communication Speed:** 115200 bps
- **Response Time:** < 100ms per module
- **Update Rate:** 10Hz system-wide
- **Reliability:** > 99.9% uptime
- **Scalability:** Up to 254 modules
- **Fault Recovery:** < 1 second

### **2. Monitoring Metrics:**
- **System Uptime:** Continuous monitoring
- **Module Status:** Real-time status tracking
- **Communication Quality:** Error rate monitoring
- **Performance Metrics:** Response time tracking
- **Fault Statistics:** Fault frequency analysis
- **Resource Utilization:** System resource monitoring

---

## 🔄 **MAINTENANCE**

### **1. Regular Maintenance:**
- **Daily:** System health check và module status verification
- **Weekly:** Communication quality assessment
- **Monthly:** System performance analysis
- **Quarterly:** Full system diagnostics
- **Annually:** System upgrade và optimization

### **2. Troubleshooting:**
- **Communication Issues:** Check RS485 wiring, termination, và configuration
- **Module Issues:** Verify module connections, power, và configuration
- **Performance Issues:** Analyze communication patterns và optimize settings
- **Fault Issues:** Review fault logs và implement corrective actions
- **Integration Issues:** Verify system configuration và module compatibility

---

## 📚 **TÀI LIỆU THAM KHẢO**

### **1. Module Integration Guides:**
- `01-rs485-power-module-integration.md` - Power Module integration
- `02-rs485-stepper-motor-integration.md` - Stepper Motor Module integration
- `03-rs485-dc-motor-integration.md` - DC Motor Module integration
- `04-rs485-sensor-module-integration.md` - Sensor Module integration
- `05-rs485-dido-module-integration.md` - DI/DO Module integration
- `06-rs485-ai-module-integration.md` - AI Module integration
- `07-rs485-custom-module-integration.md` - Custom Module integration

### **2. System Documentation:**
- `rs485-system-connection-guide.md` - System connection guide
- `rs485-protocol-specification.md` - Protocol specification
- `rs485-troubleshooting-guide.md` - Troubleshooting guide

---

## 🎯 **KẾT LUẬN**

### **Hệ thống OHT-50 RS485 cung cấp:**
- ✅ **Modular Architecture:** Kiến trúc module linh hoạt
- ✅ **Scalable Design:** Thiết kế có thể mở rộng
- ✅ **Standard Protocol:** Giao thức chuẩn Modbus RTU
- ✅ **Auto-discovery:** Tự động phát hiện module
- ✅ **Safety Features:** Tính năng an toàn đầy đủ
- ✅ **Monitoring:** Giám sát real-time
- ✅ **Diagnostics:** Chẩn đoán hệ thống
- ✅ **Maintenance:** Bảo trì dễ dàng

### **Ứng dụng:**
- 🚗 **Automated Guided Vehicles (AGV)**
- 🏭 **Industrial Automation**
- 📦 **Material Handling Systems**
- 🔧 **Robotic Systems**
- 🏢 **Smart Building Systems**

---

**Changelog:**
- v1.0 (2025-01-27): Initial system overview document
- v1.1 (2025-01-27): Removed GPIO_DE pin (not needed for RS485)
