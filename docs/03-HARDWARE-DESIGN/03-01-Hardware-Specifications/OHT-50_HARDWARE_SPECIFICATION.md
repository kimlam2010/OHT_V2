---
title: "Oht 50 Hardware Specification"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "Hardware Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['hardware-design']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# ĐẶC TẢ PHẦN CỨNG OHT-50
## Overhead Hoist Transfer System - Hardware Specification

**Phiên bản:** v1.0  
**Ngày tạo:** $(date +%Y-%m-%d)  
**PM:** [Tên PM]  
**Trạng thái:** Draft Specification  

---

**Changelog:**
- v1.0 (2025-01-27): Initial hardware specification
- v1.1 (2025-01-27): Updated LED pin mapping to match EMBED Team implementation
  - Power LED: GPIO 54 (GPIO1_D6)
  - System LED: GPIO 35 (GPIO1_A3)
  - Comm LED: GPIO 28 (GPIO0_D4)
  - Network LED: GPIO 29 (GPIO0_D5)
  - Error LED: GPIO 58 (GPIO1_D2)
  - E-Stop: GPIO 59 (GPIO1_D3)
  - Relay 1: GPIO 131 (GPIO4_A3)
  - Relay 2: GPIO 132 (GPIO4_A4)

---

## 📋 **TỔNG QUAN HỆ THỐNG**

### **🎯 Mục tiêu hệ thống:**
OHT-50 là hệ thống vận chuyển tự động trên cao (Overhead Hoist Transfer) với khả năng:
- Di chuyển tự động theo đường ray định sẵn
- Nâng/hạ tải trọng lên đến 50kg
- Tránh chướng ngại vật thông minh
- An toàn tuyệt đối với người vận hành
- Giao diện điều khiển thân thiện

### **🏗️ Kiến trúc tổng thể:**
```
OHT-50 System Architecture
├── Master Module (Orange Pi 5B) - Central Control
├── Motor Drive Module - Movement Control
├── Power Management Module - Power Supply
├── Sensor Module - Environment Sensing
├── Safety Module - Safety Systems
├── HMI Module - User Interface
└── Mechanical System - Physical Structure
```

---

## 🔧 **CHI TIẾT CÁC MODULE**

### **1. MASTER MODULE (Orange Pi 5B)**

#### **Thông số kỹ thuật:**
```
📱 Master Module Specifications
├── Processor: RK3588 ARM Cortex-A76 (4x) + A55 (4x)
├── Memory: 8GB LPDDR4
├── Storage: 32GB eMMC
├── GPU: Mali-G610 MP4
├── Network: 10/100/1000 Mbps Ethernet
├── WiFi: 802.11ac 2.4G/5G dual-band
├── USB: 1x USB 3.0, 2x USB 2.0
├── GPIO: 40-pin header
├── Power: 5V DC, 3A
└── Operating System: Ubuntu 22.04 LTS
```

#### **Kết nối phần cứng:**
```
🔌 Master Module Connections
├── RS485 Bus: /dev/ttyS1 (115200 baud)
├── Ethernet: eth0 (10/100/1000 Mbps)
├── WiFi: wlan0 (802.11ac)
├── USB Debug: /dev/ttyACM0
├── GPIO LEDs: 5x Status LEDs
├── GPIO E-Stop: 2x Emergency Stop inputs
├── GPIO Relay: 1x 24V relay output
└── Power: 5V DC input
```

#### **Pin Assignment:**
```
📌 GPIO Pin Mapping (Updated - EMBED Team Implementation)
├── LED_POWER_PIN: GPIO 54 (GPIO1_D6) - Green LED
├── LED_SYSTEM_PIN: GPIO 35 (GPIO1_A3) - Blue LED
├── LED_COMM_PIN: GPIO 28 (GPIO0_D4) - Yellow LED
├── LED_NETWORK_PIN: GPIO 29 (GPIO0_D5) - Green LED
├── LED_ERROR_PIN: GPIO 58 (GPIO1_D2) - Red LED
├── ESTOP_PIN: GPIO 59 (GPIO1_D3) - E-Stop safety input
├── RELAY1_OUTPUT_PIN: GPIO 131 (GPIO4_A3) - Relay 1 (24V DC)
├── RELAY2_OUTPUT_PIN: GPIO 132 (GPIO4_A4) - Relay 2 (24V DC)
└── UART1_TX/RX: GPIO1_C0/C1 (Pin 40/41) - RS485
```

---

### **2. MOTOR DRIVE MODULE**

#### **Thông số kỹ thuật:**
```
⚙️ Motor Drive Module Specifications
├── Microcontroller: STM32F407VGT6
├── Motor Drivers: 2x DRV8833 (H-bridge)
├── Motors: 2x DC Brushless 24V/200W
├── Encoders: 2x Quadrature 2000 PPR
├── Communication: RS485/Modbus RTU
├── Power Input: 24V DC, 10A
├── Operating Temperature: -20°C to +70°C
├── Protection: IP54 enclosure
└── Dimensions: 150x100x50mm
```

#### **Chức năng chính:**
```
🎯 Motor Drive Functions
├── Motor Control: PWM speed control
├── Direction Control: Forward/Reverse
├── Position Control: Encoder feedback
├── Speed Control: 0-3000 RPM
├── Torque Control: Current limiting
├── Safety: Overcurrent protection
├── Diagnostics: Motor health monitoring
└── Communication: Modbus RTU slave
```

#### **Kết nối điện:**
```
⚡ Motor Drive Connections
├── Power Input: 24V DC (Motor power)
├── Logic Power: 5V DC (MCU power)
├── Motor A: 24V DC, 10A max
├── Motor B: 24V DC, 10A max
├── Encoder A: 5V, A+/A-, B+/B-, Z+/Z-
├── Encoder B: 5V, A+/A-, B+/B-, Z+/Z-
├── RS485: A+/A- (Modbus communication)
└── Status LEDs: Power, Error, Communication
```

---

### **3. POWER MANAGEMENT MODULE**

#### **Thông số kỹ thuật:**
```
⚡ Power Management Specifications
├── Input Voltage: 220V AC hoặc 24V DC
├── Output Voltages: 24V DC (Motor), 12V DC (Logic), 5V DC (Sensors)
├── Battery Backup: LiFePO4 24V/50Ah
├── Charging Circuit: MPPT 20A
├── Power Monitoring: Voltage/Current sensors
├── Protection: Overcurrent, overvoltage, short circuit
├── Communication: RS485/Modbus RTU
├── Operating Temperature: -10°C to +60°C
└── Dimensions: 200x150x80mm
```

#### **Chức năng chính:**
```
🔋 Power Management Functions
├── AC/DC Conversion: 220V AC to 24V DC
├── Battery Charging: MPPT solar charger
├── Battery Management: BMS protection
├── Power Distribution: Multiple voltage rails
├── Power Monitoring: Real-time monitoring
├── UPS Function: Battery backup
├── Safety Protection: Multiple protection circuits
└── Communication: Power status reporting
```

#### **Sơ đồ kết nối:**
```
🔌 Power Management Connections
├── AC Input: 220V AC, 10A (with fuse)
├── DC Input: 24V DC, 20A (backup)
├── Battery: LiFePO4 24V/50Ah
├── Motor Output: 24V DC, 20A
├── Logic Output: 12V DC, 5A
├── Sensor Output: 5V DC, 3A
├── RS485: A+/A- (Modbus communication)
└── Status LEDs: Power, Battery, Fault
```

---

### **4. SENSOR MODULE**

#### **Thông số kỹ thuật:**
```
📡 Sensor Module Specifications
├── Microcontroller: STM32F103C8T6
├── LiDAR: RPLIDAR A1M8 (360° scan)
├── Ultrasonic: 4x HC-SR04 (2-400cm)
├── IMU: MPU6050 (gyroscope + accelerometer)
├── Magnetic Sensors: 2x A3144 Hall sensors
├── Limit Switches: 4x normally open
├── Communication: RS485/Modbus RTU
├── Power: 5V DC, 2A
└── Dimensions: 120x100x40mm
```

#### **Chức năng chính:**
```
🎯 Sensor Functions
├── Obstacle Detection: LiDAR + Ultrasonic
├── Position Tracking: Magnetic sensors
├── Orientation: IMU (pitch, roll, yaw)
├── End-of-Travel: Limit switches
├── Distance Measurement: Ultrasonic sensors
├── Mapping: LiDAR SLAM capability
├── Safety Monitoring: Obstacle avoidance
└── Data Fusion: Multi-sensor integration
```

#### **Kết nối cảm biến:**
```
🔌 Sensor Connections
├── LiDAR: UART (TX/RX)
├── Ultrasonic 1-4: Trigger/Echo pins
├── IMU: I2C (SDA/SCL)
├── Hall Sensors: Digital inputs
├── Limit Switches: Digital inputs
├── RS485: A+/A- (Modbus communication)
├── Power: 5V DC input
└── Status LEDs: Sensor health indicators
```

---

### **5. SAFETY MODULE**

#### **Thông số kỹ thuật:**
```
🛡️ Safety Module Specifications
├── Microcontroller: STM32F103C8T6
├── E-Stop Buttons: 2x normally closed
├── Safety Light Curtain: 2x (entry/exit)
├── Safety Mats: 4x pressure sensors
├── Door Sensors: 2x magnetic switches
├── Emergency Brake: Electromagnetic brake
├── Communication: RS485/Modbus RTU
├── Safety Level: SIL2 certified
└── Dimensions: 100x80x30mm
```

#### **Chức năng an toàn:**
```
🛡️ Safety Functions
├── Emergency Stop: Dual-channel E-Stop
├── Light Curtain: Entry/exit protection
├── Safety Mats: Floor pressure detection
├── Door Monitoring: Access door status
├── Emergency Brake: Automatic braking
├── Safety Logic: Redundant safety circuits
├── Status Monitoring: Safety system health
└── Communication: Safety status reporting
```

#### **Kết nối an toàn:**
```
🔌 Safety Connections
├── E-Stop 1: Normally closed input
├── E-Stop 2: Normally closed input
├── Light Curtain 1: Digital input
├── Light Curtain 2: Digital input
├── Safety Mats: 4x analog inputs
├── Door Sensors: 2x digital inputs
├── Emergency Brake: Relay output
├── RS485: A+/A- (Modbus communication)
└── Status LEDs: Safety status indicators
```

---

### **6. HMI MODULE (Human Machine Interface)**

#### **Thông số kỹ thuật:**
```
🖥️ HMI Module Specifications
├── Display: 7" TFT LCD (800x480 resolution)
├── Touch: Capacitive touch screen
├── Microcontroller: STM32F407VGT6
├── Buttons: 6x physical buttons
├── Buzzer: Audio feedback
├── Communication: RS485/Modbus RTU
├── Power: 12V DC, 1A
├── Operating Temperature: -10°C to +60°C
└── Dimensions: 200x150x30mm
```

#### **Chức năng giao diện:**
```
🎮 HMI Functions
├── Touch Interface: Capacitive touch
├── Display: Status information
├── Physical Buttons: Start, Stop, Reset, etc.
├── Audio Feedback: Buzzer alerts
├── Status Display: System status
├── Configuration: System settings
├── Alarms: Visual/audio alarms
└── Communication: Modbus RTU master
```

#### **Kết nối giao diện:**
```
🔌 HMI Connections
├── Display: SPI interface
├── Touch: I2C interface
├── Buttons: 6x digital inputs
├── Buzzer: PWM output
├── RS485: A+/A- (Modbus communication)
├── Power: 12V DC input
└── Status LEDs: HMI status indicators
```

---

## 🔗 **SƠ ĐỒ KẾT NỐI HỆ THỐNG**

### **Sơ đồ tổng thể:**
```
OHT-50 System Connection Diagram

[220V AC] → [Power Management Module]
                    ↓
            [24V DC] [12V DC] [5V DC]
                ↓         ↓        ↓
        [Motor Drive] [HMI] [Sensor Module]
                ↓         ↓        ↓
            [RS485 Bus] ← → [Master Module]
                ↓         ↓        ↓
        [Safety Module] ← → [Ethernet/WiFi]
```

### **RS485 Bus Topology:**
```
RS485 Bus Configuration
Master Module (Modbus Master)
├── Motor Drive Module (Slave ID: 0x02)
├── Power Management Module (Slave ID: 0x03)
├── Sensor Module (Slave ID: 0x04)
├── Safety Module (Slave ID: 0x05)
└── HMI Module (Slave ID: 0x06)

Bus Parameters:
├── Baud Rate: 115200
├── Data Bits: 8
├── Parity: None
├── Stop Bits: 1
├── Protocol: Modbus RTU
└── Termination: 120Ω resistors
```

---

## 📐 **THIẾT KẾ CƠ KHÍ**

### **Khung cơ bản:**
```
🏗️ Mechanical Frame
├── Material: Aluminum extrusion 40x40mm
├── Frame Size: 800x600x400mm
├── Weight Capacity: 50kg payload
├── Wheel Base: 600mm
├── Track Width: 400mm
├── Ground Clearance: 50mm
├── Protection: IP54 enclosure
└── Mounting: Ceiling mount brackets
```

### **Hệ thống bánh xe:**
```
🛞 Wheel System
├── Drive Wheels: 2x 100mm diameter
├── Caster Wheels: 2x 80mm diameter
├── Wheel Material: Polyurethane
├── Drive System: Belt drive
├── Gear Ratio: 20:1
├── Brake System: Electromagnetic brake
└── Encoder Mounting: Direct shaft coupling
```

### **Hệ thống nâng:**
```
🔧 Lifting System
├── Type: Linear actuator
├── Capacity: 50kg
├── Stroke: 500mm
├── Speed: 50mm/s
├── Motor: 24V DC, 100W
├── Encoder: Linear encoder
├── Limit Switches: Upper/Lower limits
└── Safety: Load cell monitoring
```

---

## ⚡ **THIẾT KẾ ĐIỆN**

### **Sơ đồ điện tổng thể:**
```
Electrical System Design

AC Power (220V) → [Power Management Module]
                        ↓
                [DC Power Distribution]
                        ↓
    ┌─────────┬─────────┬─────────┬─────────┐
    ↓         ↓         ↓         ↓         ↓
[Motor]   [Logic]   [Sensors] [HMI]   [Safety]
24V DC    12V DC    5V DC    12V DC   24V DC
```

### **Bảo vệ điện:**
```
🛡️ Electrical Protection
├── AC Input: 10A circuit breaker
├── DC Outputs: Fuse protection
├── Motor Protection: Overcurrent relays
├── Logic Protection: Reverse polarity protection
├── Sensor Protection: ESD protection
├── Communication: Surge protection
└── Grounding: Proper grounding system
```

---

## 🔧 **PHƯƠNG ÁN TRIỂN KHAI**

### **Phase 1: Core Hardware (Tuần 1-2)**
```
🎯 Phase 1 Objectives
├── Master Module setup và configuration
├── Motor Drive Module development
├── Basic power management
├── Simple safety system
└── Basic mechanical structure
```

### **Phase 2: Sensing & Control (Tuần 3-4)**
```
🎯 Phase 2 Objectives
├── Sensor module integration
├── Advanced safety systems
├── Motion control algorithms
├── Obstacle detection
└── Position tracking
```

### **Phase 3: User Interface (Tuần 5-6)**
```
🎯 Phase 3 Objectives
├── HMI module development
├── User interface design
├── System integration testing
├── Performance optimization
└── Documentation completion
```

---

## 💰 **PHÂN TÍCH CHI PHÍ**

### **Chi phí module:**
```
💰 Cost Breakdown
├── Master Module (Orange Pi 5B): $100
├── Motor Drive Module: $300
├── Power Management Module: $400
├── Sensor Module: $250
├── Safety Module: $300
├── HMI Module: $150
├── Mechanical Parts: $800
├── Cables & Connectors: $150
├── Enclosures & Mounting: $300
└── Development Tools: $200

Total Estimated Cost: $2,950
```

### **Chi phí phát triển:**
```
💼 Development Costs
├── Hardware Development: $1,500
├── Software Development: $2,000
├── Testing & Validation: $1,000
├── Documentation: $500
└── Project Management: $1,000

Total Development Cost: $6,000
```

---

## 📋 **KẾ HOẠCH MUA SẮM**

### **Danh sách mua sắm:**
```
🛒 Procurement List

1. Master Module:
   ├── Orange Pi 5B (8GB RAM, 32GB eMMC)
   ├── Power supply 5V/3A
   ├── MicroSD card 32GB
   └── Case/enclosure

2. Motor Drive Module:
   ├── STM32F407VGT6 development board
   ├── DRV8833 motor drivers (2x)
   ├── DC motors 24V/200W (2x)
   ├── Encoders 2000 PPR (2x)
   └── Enclosure IP54

3. Power Management Module:
   ├── AC/DC converter 220V to 24V
   ├── LiFePO4 battery 24V/50Ah
   ├── MPPT charger 20A
   ├── DC/DC converters
   └── Protection circuits

4. Sensor Module:
   ├── RPLIDAR A1M8
   ├── HC-SR04 ultrasonic sensors (4x)
   ├── MPU6050 IMU
   ├── Hall sensors A3144 (2x)
   └── Limit switches (4x)

5. Safety Module:
   ├── E-Stop buttons (2x)
   ├── Safety light curtains (2x)
   ├── Pressure sensors (4x)
   ├── Magnetic door switches (2x)
   └── Electromagnetic brake

6. HMI Module:
   ├── 7" TFT LCD display
   ├── Capacitive touch controller
   ├── Physical buttons (6x)
   ├── Buzzer
   └── Enclosure

7. Mechanical Parts:
   ├── Aluminum extrusion 40x40mm
   ├── Wheels và bearings
   ├── Linear actuator
   ├── Mounting brackets
   └── Cables và connectors
```

---

## 🎯 **KẾT LUẬN**

### **Tóm tắt phương án:**
OHT-50 sử dụng kiến trúc module hóa với 6 module chính:
1. **Master Module** - Điều khiển trung tâm
2. **Motor Drive Module** - Điều khiển chuyển động
3. **Power Management Module** - Quản lý nguồn
4. **Sensor Module** - Cảm biến môi trường
5. **Safety Module** - Hệ thống an toàn
6. **HMI Module** - Giao diện người dùng

### **Ưu điểm phương án:**
- ✅ **Modular Design:** Dễ bảo trì và nâng cấp
- ✅ **Scalable:** Có thể mở rộng thêm module
- ✅ **Reliable:** Hệ thống an toàn đa lớp
- ✅ **Cost-effective:** Chi phí hợp lý
- ✅ **Standardized:** Sử dụng chuẩn công nghiệp

### **Rủi ro và biện pháp:**
- ⚠️ **Integration Complexity:** Cần testing kỹ lưỡng
- ⚠️ **Power Management:** Cần thiết kế cẩn thận
- ⚠️ **Safety Compliance:** Cần certification
- ⚠️ **Cost Control:** Cần quản lý chi phí chặt chẽ

---

**📄 Tài liệu này sẽ được cập nhật khi có thay đổi trong thiết kế hoặc yêu cầu mới.**

**Ngày cập nhật:** $(date +%Y-%m-%d)  
**Phiên bản:** v1.0  
**Trạng thái:** Draft Specification
