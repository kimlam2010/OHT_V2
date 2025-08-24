# ⚙️ REQ_FW_Architecture - Yêu cầu kiến trúc Firmware

**Mục đích:** Định nghĩa yêu cầu kiến trúc Firmware cho OHT-50 Master Module

---

## 📋 **Thông tin tài liệu:**
- **Phiên bản:** v1.0
- **Ngày tạo:** 2025-01-24
- **Người tạo:** PM Team
- **Trạng thái:** Draft

---

## 🎯 **Mục tiêu:**

### **Yêu cầu chính:**
1. **Real-time Operation** - Hoạt động thời gian thực
2. **Safety Compliance** - Tuân thủ tiêu chuẩn an toàn
3. **Hardware Abstraction** - Trừu tượng hóa phần cứng
4. **Reliable Communication** - Giao tiếp tin cậy
5. **Fault Tolerance** - Chịu lỗi cao

---

## 📊 **BẢNG MAPPING - TÀI LIỆU NÀY:**

| **Phần** | **Mục đích** | **Đối tượng đọc** | **Liên quan đến** | **Trạng thái** |
|----------|--------------|-------------------|-------------------|----------------|
| **Kiến trúc tổng thể** | Định nghĩa 4 layer chính | Architect, FW Lead | System Design | ✅ Hoàn thành |
| **Application Layer** | Yêu cầu ứng dụng cao cấp | FW Developer | Mission, Safety | ✅ Hoàn thành |
| **HAL Layer** | Trừu tượng hóa phần cứng | HAL Developer | Hardware | ✅ Hoàn thành |
| **Driver Layer** | Driver cho thiết bị | Driver Developer | Sensors, Motors | ✅ Hoàn thành |
| **Hardware Layer** | Thông số phần cứng | HW Engineer | Orange Pi 5B | ✅ Hoàn thành |
| **Real-time Requirements** | Yêu cầu thời gian thực | System Engineer | Performance | ✅ Hoàn thành |
| **Safety Requirements** | Yêu cầu an toàn | Safety Engineer | SIL2, E-Stop | ✅ Hoàn thành |
| **Communication Requirements** | Yêu cầu giao tiếp | Network Engineer | RS485, Modbus | ✅ Hoàn thành |
| **Performance Requirements** | Yêu cầu hiệu suất | Performance Engineer | CPU, Memory | ✅ Hoàn thành |
| **Data Flow** | Luồng dữ liệu | System Architect | Data Architecture | ✅ Hoàn thành |
| **Safety & Reliability** | An toàn và tin cậy | Safety Engineer | Fault Tolerance | ✅ Hoàn thành |
| **Scalability** | Khả năng mở rộng | System Architect | Future Growth | ✅ Hoàn thành |
| **Integration** | Tích hợp hệ thống | Integration Engineer | BE, FE, HW | ✅ Hoàn thành |
| **Acceptance Criteria** | Tiêu chí chấp nhận | QA Engineer | Testing | ✅ Hoàn thành |

---

## 🏗️ **Kiến trúc tổng thể:**

### **1. Application Layer:**
```
┌─────────────────────────────────────┐
│         Application Layer           │
│  ┌─────────────┬─────────────────┐  │
│  │ State Mgmt  │  Mission Ctrl   │  │
│  ├─────────────┼─────────────────┤  │
│  │ Safety Ctrl │  Movement Ctrl  │  │
│  ├─────────────┼─────────────────┤  │
│  │ Config Mgmt │  Telemetry      │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Yêu cầu:**
- ✅ **State Management** - Quản lý trạng thái hệ thống
- ✅ **Mission Control** - Điều khiển mission
- ✅ **Safety Control** - Điều khiển an toàn
- ✅ **Movement Control** - Điều khiển chuyển động
- ✅ **Configuration Management** - Quản lý cấu hình
- ✅ **Telemetry Collection** - Thu thập telemetry

### **2. HAL (Hardware Abstraction Layer):**
```
┌─────────────────────────────────────┐
│         HAL Layer                   │
│  ┌─────────────┬─────────────────┐  │
│  │ UART HAL    │  GPIO HAL       │  │
│  ├─────────────┼─────────────────┤  │
│  │ Timer HAL   │  ADC HAL        │  │
│  ├─────────────┼─────────────────┤  │
│  │ PWM HAL     │  I2C HAL        │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Yêu cầu:**
- ✅ **UART HAL** - Giao tiếp serial (RS485)
- ✅ **GPIO HAL** - Điều khiển GPIO (E-Stop, LED)
- ✅ **Timer HAL** - Timer và watchdog
- ✅ **ADC HAL** - Analog-to-Digital conversion
- ✅ **PWM HAL** - Pulse Width Modulation
- ✅ **I2C HAL** - I2C communication

### **3. Driver Layer:**
```
┌─────────────────────────────────────┐
│         Driver Layer                │
│  ┌─────────────┬─────────────────┐  │
│  │ RS485 Drv   │  Motor Drv      │  │
│  ├─────────────┼─────────────────┤  │
│  │ Sensor Drv  │  Relay Drv      │  │
│  ├─────────────┼─────────────────┤  │
│  │ LED Drv     │  E-Stop Drv     │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Yêu cầu:**
- ✅ **RS485 Driver** - Driver cho RS485 communication
- ✅ **Motor Driver** - Driver cho motor control
- ✅ **Sensor Driver** - Driver cho sensors
- ✅ **Relay Driver** - Driver cho relay control
- ✅ **LED Driver** - Driver cho LED indicators
- ✅ **E-Stop Driver** - Driver cho E-Stop system

### **4. Hardware Layer:**
```
┌─────────────────────────────────────┐
│         Hardware Layer              │
│  ┌─────────────┬─────────────────┐  │
│  │ Orange Pi   │  RS485 Trans    │  │
│  ├─────────────┼─────────────────┤  │
│  │ Motor Ctrl  │  Sensors        │  │
│  ├─────────────┼─────────────────┤  │
│  │ E-Stop      │  LED Indicators │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

**Yêu cầu:**
- ✅ **Orange Pi 5B** - Main processor (RK3588)
- ✅ **RS485 Transceiver** - RS485 communication
- ✅ **Motor Controller** - Motor control hardware
- ✅ **Sensors** - Various sensors
- ✅ **E-Stop System** - Emergency stop hardware
- ✅ **LED Indicators** - Status indicators

---

## 🔧 **Yêu cầu kỹ thuật:**

### **1. Real-time Requirements:**
- ✅ **Response Time** < 10ms cho critical operations
- ✅ **Control Loop** 100Hz cho motor control
- ✅ **Safety Response** < 5ms cho E-Stop
- ✅ **Communication Latency** < 1ms cho RS485
- ✅ **Watchdog Timeout** 1 second

### **2. Safety Requirements:**
- ✅ **SIL2 Compliance** - Safety Integrity Level 2
- ✅ **Dual-channel E-Stop** - E-Stop kênh kép
- ✅ **Watchdog Timer** - Hardware watchdog
- ✅ **Fault Detection** - Phát hiện lỗi tự động
- ✅ **Safe State** - Trạng thái an toàn khi lỗi

### **3. Communication Requirements:**
- ✅ **RS485 Protocol** - Modbus RTU
- ✅ **Baud Rate** 115200 bps
- ✅ **Data Format** 8N1 (8 data, no parity, 1 stop)
- ✅ **Error Detection** CRC-16
- ✅ **Retry Mechanism** - Tự động thử lại khi lỗi

### **4. Performance Requirements:**
- ✅ **CPU Usage** < 70% under normal load
- ✅ **Memory Usage** < 256MB
- ✅ **Flash Usage** < 128MB
- ✅ **Boot Time** < 30 seconds
- ✅ **Update Time** < 5 minutes

---

## 🔄 **Data Flow Architecture:**

### **1. Command Flow:**
```
Backend → RS485 → HAL → Driver → Hardware
```

### **2. Status Flow:**
```
Hardware → Driver → HAL → Application → Backend
```

### **3. Safety Flow:**
```
E-Stop → Driver → HAL → Safety Control → State Management
```

---

## 🛡️ **Safety & Reliability:**

### **1. Fault Tolerance:**
- ✅ **Redundant Systems** - Hệ thống dự phòng
- ✅ **Error Recovery** - Khôi phục lỗi tự động
- ✅ **Graceful Degradation** - Suy giảm từ từ
- ✅ **Fail-safe Operation** - Hoạt động an toàn khi lỗi

### **2. Monitoring & Diagnostics:**
- ✅ **Self-diagnostics** - Tự chẩn đoán
- ✅ **Health Monitoring** - Giám sát sức khỏe
- ✅ **Error Logging** - Ghi log lỗi
- ✅ **Performance Metrics** - Metrics hiệu năng

### **3. Update & Maintenance:**
- ✅ **OTA Updates** - Cập nhật qua không khí
- ✅ **Rollback Capability** - Khả năng rollback
- ✅ **Configuration Backup** - Sao lưu cấu hình
- ✅ **Factory Reset** - Reset về trạng thái ban đầu

---

## 📊 **Scalability Requirements:**

### **1. Modular Design:**
- ✅ **Plugin Architecture** - Kiến trúc plugin
- ✅ **Dynamic Loading** - Tải động
- ✅ **Hot-swappable Modules** - Module thay nóng
- ✅ **Version Compatibility** - Tương thích phiên bản

### **2. Resource Management:**
- ✅ **Memory Pool** - Pool bộ nhớ
- ✅ **Task Scheduling** - Lập lịch tác vụ
- ✅ **Priority Management** - Quản lý ưu tiên
- ✅ **Resource Allocation** - Phân bổ tài nguyên

---

## 🔗 **Integration Requirements:**

### **1. Hardware Integration:**
- ✅ **Pin Configuration** - Cấu hình pin
- ✅ **Interrupt Handling** - Xử lý ngắt
- ✅ **DMA Support** - Hỗ trợ DMA
- ✅ **Power Management** - Quản lý nguồn

### **2. Software Integration:**
- ✅ **API Interface** - Giao diện API
- ✅ **Protocol Support** - Hỗ trợ protocol
- ✅ **Data Format** - Định dạng dữ liệu
- ✅ **Error Handling** - Xử lý lỗi

---

## 📋 **Acceptance Criteria:**

### **✅ Functional Requirements:**
- [ ] Tất cả HAL functions hoạt động đúng
- [ ] Driver communication ổn định
- [ ] Safety system hoạt động đúng
- [ ] Real-time requirements đạt chuẩn
- [ ] Communication protocols hoạt động

### **✅ Non-Functional Requirements:**
- [ ] Response time < 10ms
- [ ] CPU usage < 70%
- [ ] Memory usage < 256MB
- [ ] Boot time < 30 seconds
- [ ] Safety compliance đạt chuẩn

---

## 📚 **References:**
- [Hardware Requirements](../02-HARDWARE-REQUIREMENTS/)
- [Communication Protocols](REQ_FW_Communication.md)
- [Safety System](REQ_FW_Safety_System.md)
- [State Management](REQ_FW_State_Management.md)

---

**Cập nhật lần cuối:** 2025-01-24
**Phiên bản:** v1.0
