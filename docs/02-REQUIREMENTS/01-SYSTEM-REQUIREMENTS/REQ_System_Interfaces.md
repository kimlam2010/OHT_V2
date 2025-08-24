# Interfaces hệ thống OHT-50 (Phiên bản 2.1)

**Phiên bản:** 2.1  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated với RS485 cho tất cả modules

---

## 🎯 **MỤC TIÊU**

### **Yêu cầu hệ thống:**
- **Chuẩn hóa RS485:** Tất cả modules đều sử dụng RS485/Modbus RTU
- **Master-Slave Architecture:** Master Module ↔ Slave Modules qua RS485
- **Auto-discovery:** Tự động phát hiện modules qua RS485
- **Location-based Safety:** Tích hợp positioning vào safety system
- **Backend Integration:** Master ↔ Center qua HTTP/WebSocket

---

## 🔌 **RS485 COMMUNICATION STANDARD**

### **Bus Configuration (Tất cả Modules):**
```
Protocol:           Modbus RTU
Baud Rate:          115200 bps (có thể cấu hình)
Data Bits:          8
Parity:             None
Stop Bits:          1
Address Range:      1-32 (auto-discovery)
Termination:        120Ω resistors at both ends
Bias:               560Ω pull-up/pull-down resistors
Cable:              Shielded twisted pair, 24AWG
Max Distance:       1200m
Max Nodes:          32 modules
```

### **RS485 Bus Topology:**
```
Master Module (Orange Pi 5B)
    │
    ├── UART1 RS485 (115200 bps)
    │
    ├── Power Module (ID: 0x02)
    ├── Safety Module (ID: 0x03)
    ├── Travel Motor Module (ID: 0x04)
    ├── Dock & Location Module (ID: 0x05)
    ├── Lifter Motor Module (ID: 0x06) [Optional]
    ├── Cargo Door Module (ID: 0x07) [Optional]
    ├── Safety Extended Module (ID: 0x08) [Optional]
    ├── RFID Reader Module (ID: 0x09) [Plug-and-Play]
    ├── Camera Module (ID: 0x0A) [Plug-and-Play]
    ├── Environmental Sensor Module (ID: 0x0B) [Plug-and-Play]
    └── Custom Modules (ID: 0x0C-0xFF) [Plug-and-Play]
```

---

## 🏗️ **MASTER MODULE INTERFACES**

### **RS485 Master Interface:**
```
Hardware:           UART1 RS485 transceiver
Driver:             Linux serial driver
Device:             /dev/ttyS1 → /dev/ttyOHT485 (udev symlink)
Protocol:           Modbus RTU Master
Address Scanning:   1-32 auto-discovery
Timeout:            100ms per module
Retry:              3 attempts per command
CRC:                Modbus CRC-16
```

### **Network Interfaces:**
```
Ethernet:           1Gbps RJ45, PoE support
WiFi:               2.4GHz/5GHz 802.11ac
Protocol:           HTTP/WebSocket cho Center communication
Security:           SSL/TLS encryption
Authentication:     Token-based authentication
```

### **Local Interfaces:**
```
USB-C:              Debug interface, firmware update
GPIO:               40-pin header với 5V/3.3V logic
E-Stop:             2x dual-channel safety inputs
Relay Outputs:      2x 24V/2A relay outputs
Status LEDs:        5x RGB LEDs
```

---

## 🔋 **POWER MODULE INTERFACE (ID: 0x02) - RS485**

### **Module Type:** 0x0002 (Power)
### **Module Name:** "POWR" (Power)

### **Register Map (0x0000-0x00FF):**
```
// Battery System (0x0000-0x000F)
0x0000: Battery Status (0=OK, 1=Low, 2=Critical, 3=Error)
0x0001: Battery Voltage (mV)
0x0002: Battery Current (mA)
0x0003: Battery Temperature (°C)
0x0004: State of Charge (%)
0x0005: Remaining Capacity (mAh)
0x0006: Full Capacity (mAh)
0x0007: Cycle Count
0x0008: Cell Voltage 1 (mV)
0x0009: Cell Voltage 2 (mV)
0x000A: Cell Voltage 3 (mV)
0x000B: Cell Voltage 4 (mV)
0x000C: Cell Balance Status (16-bit bitfield)
0x000D: BMS Status (16-bit bitfield)
0x000E: Fault Code
0x000F: Battery Health (%)

// Power Distribution (0x0010-0x001F)
0x0010: 24V Bus Voltage (mV)
0x0011: 24V Bus Current (mA)
0x0012: 12V Bus Voltage (mV)
0x0013: 12V Bus Current (mA)
0x0014: 5V Bus Voltage (mV)
0x0015: 5V Bus Current (mA)
0x0016: 3.3V Bus Voltage (mV)
0x0017: 3.3V Bus Current (mA)
0x0018: Power Status (16-bit bitfield)
0x0019: Over-current Status (16-bit bitfield)
0x001A: Over-voltage Status (16-bit bitfield)
0x001B: Under-voltage Status (16-bit bitfield)
0x001C: Temperature Status (16-bit bitfield)
0x001D: Fault Status (16-bit bitfield)
0x001E: Power Fault Code
0x001F: Power Health (%)

// Charging System (0x0020-0x002F)
0x0020: Charging Status (0=Not Charging, 1=Charging, 2=Full, 3=Error)
0x0021: Charging Current (mA)
0x0022: Charging Voltage (mV)
0x0023: Charging Power (W × 10)
0x0024: Charging Temperature (°C)
0x0025: Charging Time (minutes)
0x0026: Estimated Time to Full (minutes)
0x0027: Charging Efficiency (%)
0x0028: AC Input Voltage (V × 10)
0x0029: AC Input Current (A × 100)
0x002A: AC Input Power (W)
0x002B: AC Input Frequency (Hz × 10)
0x002C: Charger Status (16-bit bitfield)
0x002D: Charger Fault Code
0x002E: Charger Temperature (°C)
0x002F: Charger Health (%)

// Configuration (0x0030-0x003F)
0x0030: Max Charging Current (mA)
0x0031: Max Discharging Current (mA)
0x0032: Low Voltage Cutoff (mV)
0x0033: High Voltage Cutoff (mV)
0x0034: Temperature Cutoff (°C)
0x0035: Cell Balance Threshold (mV)
0x0036: Update Rate (Hz)
0x0037: Module Status
```

---

## 🔒 **SAFETY MODULE INTERFACE (ID: 0x03) - RS485**

### **Module Type:** 0x0003 (Safety)
### **Module Name:** "SAFE" (Safety)

### **Register Map (0x0000-0x00FF):**
```
// E-Stop System (0x0000-0x000F)
0x0000: E-Stop Status (0=Normal, 1=Activated, 2=Error)
0x0001: E-Stop Channel 1 Status (0=Normal, 1=Activated)
0x0002: E-Stop Channel 2 Status (0=Normal, 1=Activated)
0x0003: E-Stop Response Time (ms)
0x0004: E-Stop Fault Code
0x0005: E-Stop Health (%)

// Light Curtain (0x0010-0x001F)
0x0010: Light Curtain Status (0=Clear, 1=Blocked, 2=Error)
0x0011: Beam Status (16-bit bitfield)
0x0012: Detection Range (mm)
0x0013: Response Time (ms)
0x0014: Fault Code
0x0015: Health (%)

// Safety Mats (0x0020-0x002F)
0x0020: Safety Mat Status (0=Clear, 1=Activated, 2=Error)
0x0021: Mat 1 Status (0=Clear, 1=Activated)
0x0022: Mat 2 Status (0=Clear, 1=Activated)
0x0023: Mat 3 Status (0=Clear, 1=Activated)
0x0024: Mat 4 Status (0=Clear, 1=Activated)
0x0025: Activation Force (kg × 10)
0x0026: Response Time (ms)
0x0027: Fault Code
0x0028: Health (%)

// Door Sensors (0x0030-0x003F)
0x0030: Door Status (0=Closed, 1=Open, 2=Error)
0x0031: Door 1 Status (0=Closed, 1=Open)
0x0032: Door 2 Status (0=Closed, 1=Open)
0x0033: Response Time (ms)
0x0034: Fault Code
0x0035: Health (%)

// Emergency Brake (0x0040-0x004F)
0x0040: Brake Status (0=Released, 1=Applied, 2=Error)
0x0041: Brake Voltage (mV)
0x0042: Brake Current (mA)
0x0043: Brake Temperature (°C)
0x0044: Response Time (ms)
0x0045: Fault Code
0x0046: Health (%)

// Safety System (0x0050-0x005F)
0x0050: Overall Safety Status (0=Safe, 1=Warning, 2=Danger, 3=Emergency)
0x0051: Safety Zone Status (16-bit bitfield)
0x0052: Fault Status (16-bit bitfield)
0x0053: Emergency Status (16-bit bitfield)
0x0054: Safety Response Time (ms)
0x0055: System Health (%)

// Configuration (0x0060-0x006F)
0x0060: E-Stop Response Time (ms)
0x0061: Light Curtain Response Time (ms)
0x0062: Safety Mat Response Time (ms)
0x0063: Door Response Time (ms)
0x0064: Brake Response Time (ms)
0x0065: Update Rate (Hz)
0x0066: Module Status
0x0067: Configuration Status
```

---

## 🚗 **TRAVEL MOTOR MODULE INTERFACE (ID: 0x04) - RS485**

### **Module Type:** 0x0004 (Travel Motor)
### **Module Name:** "TRAV" (Travel)

### **Register Map (0x0000-0x00FF):**
```
// Motor Control (0x0000-0x000F)
0x0000: Motor 1 Control Mode (1=Manual, 2=Auto, 3=Backend)
0x0001: Motor 1 Enable (0=Disable, 1=Enable)
0x0002: Motor 1 Target Speed (% × 10)
0x0003: Motor 1 Actual Speed (% × 10)
0x0004: Motor 1 PWM Output (% × 10)
0x0005: Motor 1 Current (mA)
0x0006: Motor 1 Temperature (°C)
0x0007: Motor 1 Status

0x0008: Motor 2 Control Mode (1=Manual, 2=Auto, 3=Backend)
0x0009: Motor 2 Enable (0=Disable, 1=Enable)
0x000A: Motor 2 Target Speed (% × 10)
0x000B: Motor 2 Actual Speed (% × 10)
0x000C: Motor 2 PWM Output (% × 10)
0x000D: Motor 2 Current (mA)
0x000E: Motor 2 Temperature (°C)
0x000F: Motor 2 Status

// Velocity Control (0x0010-0x001F)
0x0010: Target Velocity (m/s × 100)
0x0011: Current Velocity (m/s × 100)
0x0012: Speed Percentage (% × 10)
0x0013: Velocity Error (m/s × 100)
0x0014: Max Velocity Limit (m/s × 100)
0x0015: Acceleration Limit (m/s² × 100)
0x0016: Deceleration Limit (m/s² × 100)
0x0017: Velocity Status

// PID Control (0x0020-0x002F)
0x0020: Speed PID Kp (× 1000)
0x0021: Speed PID Ki (× 1000)
0x0022: Speed PID Kd (× 1000)
0x0023: PID Update Rate (Hz)
0x0024: PID Output (% × 10)
0x0025: PID Error (% × 10)
0x0026: PID Status
0x0027: PID Mode (0=Manual, 1=Auto)

// Backend Integration (0x0030-0x003F)
0x0030: Backend Target Velocity (m/s × 100)
0x0031: Backend Max Velocity (m/s × 100)
0x0032: Backend Acceleration (m/s² × 100)
0x0033: Backend Deceleration (m/s² × 100)
0x0034: Backend Control Mode (0=Manual, 1=Auto)
0x0035: Backend Safety Enabled (0=Disabled, 1=Enabled)
0x0036: Backend Status
0x0037: Backend Error Code

// Safety Control (0x0040-0x004F)
0x0040: Emergency Stop Status (0=Normal, 1=Activated)
0x0041: Safety Zone Status (16-bit bitfield)
0x0042: Speed Limit (% × 10)
0x0043: Fault Status (16-bit bitfield)
0x0044: Safety Status (0=Safe, 1=Warning, 2=Danger)
0x0045: Fault Code
0x0046: Safety Mode
0x0047: Safety Response Time (ms)

// Configuration (0x0050-0x005F)
0x0050: Wheel Radius (mm × 10)
0x0051: Gear Ratio (× 100)
0x0052: Max Motor RPM
0x0053: Speed Sensor PPR
0x0054: Max Speed (% × 10)
0x0055: Min Speed (% × 10)
0x0056: Speed Deadband (% × 10)
0x0057: Control Update Rate (Hz)
```

---

## 🧭 **DOCK & LOCATION MODULE INTERFACE (ID: 0x05) - RS485**

### **Module Type:** 0x0005 (Dock & Location)
### **Module Name:** "DCLO" (Dock & Location)

### **Register Map (0x0000-0x00FF):**
```
// Location System (0x0000-0x000F)
0x0000: Location System Status (0=OK, 1=Error, 2=Calibrating)
0x0001: Current Position X (mm × 10)
0x0002: Current Position Y (mm × 10)
0x0003: Current Position Z (mm × 10)
0x0004: Target Position X (mm × 10)
0x0005: Target Position Y (mm × 10)
0x0006: Target Position Z (mm × 10)
0x0007: Position Accuracy (mm × 10)
0x0008: Navigation Status (0=Idle, 1=Navigating, 2=Arrived)
0x0009: Path Progress (%)
0x000A: Distance to Target (mm)
0x000B: Estimated Time to Target (seconds)
0x000C: Obstacle Detected (0=No, 1=Yes)
0x000D: Obstacle Distance (mm)
0x000E: Obstacle Angle (deg × 100)
0x000F: Navigation Error Code

// IMU Data (0x0010-0x001F)
0x0010: IMU Status (0=OK, 1=Error)
0x0011: Accelerometer X (mg × 10)
0x0012: Accelerometer Y (mg × 10)
0x0013: Accelerometer Z (mg × 10)
0x0014: Gyroscope X (deg/s × 10)
0x0015: Gyroscope Y (deg/s × 10)
0x0016: Gyroscope Z (deg/s × 10)
0x0017: Temperature (°C)
0x0018: Orientation Pitch (deg × 100)
0x0019: Orientation Roll (deg × 100)
0x001A: Orientation Yaw (deg × 100)
0x001B: Velocity X (mm/s)
0x001C: Velocity Y (mm/s)
0x001D: Velocity Z (mm/s)
0x001E: Acceleration X (mm/s²)
0x001F: Acceleration Y (mm/s²)

// Magnetic Sensors (0x0020-0x002F)
0x0020: Magnetic Status (0=OK, 1=Error)
0x0021: Magnetic X (mT × 100)
0x0022: Magnetic Y (mT × 100)
0x0023: Magnetic Z (mT × 100)
0x0024: Magnetic Heading (deg × 100)
0x0025: Magnetic Strength (mT × 100)
0x0026: Magnetic Quality (%)

// RFID System (0x0030-0x003F)
0x0030: RFID Status (0=OK, 1=Error)
0x0031: RFID Tag ID (32-bit)
0x0032: RFID Tag ID (32-bit)
0x0033: RFID Signal Strength (RSSI)
0x0034: RFID Read Count
0x0035: RFID Last Read Time (seconds)

// Dock Alignment (0x0040-0x004F)
0x0040: Dock Status (0=Not Docking, 1=Approaching, 2=Aligning, 3=Docked)
0x0041: Alignment Status (0=Not Aligned, 1=Aligning, 2=Aligned, 3=Error)
0x0042: Distance to Dock (mm)
0x0043: Alignment Angle (deg × 100)
0x0044: Station ID (16-bit)
0x0045: Charging Status (0=Not Charging, 1=Charging, 2=Full, 3=Error)
0x0046: Charging Current (mA)
0x0047: Charging Voltage (mV)
0x0048: Dock Sensor 1 (mm)
0x0049: Dock Sensor 2 (mm)
0x004A: Dock Sensor 3 (mm)
0x004B: Dock Sensor 4 (mm)
0x004C: Dock Safety Status (0=Safe, 1=Warning, 2=Danger)
0x004D: Dock Fault Code
0x004E: Dock Health (%)

// GPS/GNSS (Optional) (0x0050-0x005F)
0x0050: GPS Status (0=OK, 1=Error, 2=No Signal)
0x0051: GPS Latitude (deg × 1000000)
0x0052: GPS Longitude (deg × 1000000)
0x0053: GPS Altitude (m × 10)
0x0054: GPS Speed (m/s × 10)
0x0055: GPS Heading (deg × 100)
0x0056: GPS Satellites Count
0x0057: GPS HDOP (× 100)
0x0058: GPS VDOP (× 100)
0x0059: GPS Time (Unix timestamp)
0x005A: GPS Quality (%)

// Configuration (0x0060-0x006F)
0x0060: Update Rate (Hz)
0x0061: IMU Update Rate (Hz)
0x0062: Magnetic Update Rate (Hz)
0x0063: RFID Update Rate (Hz)
0x0064: GPS Update Rate (Hz)
0x0065: Module Status
0x0066: Configuration Status
0x0067: System Health (%)
```

---

## 🔧 **OPTIONAL MODULES - RS485**

### **Lifter Motor Module (ID: 0x06) - RS485:**
```
Module Type: 0x0006 (Lifter Motor)
Module Name: "LIFT" (Lifter)
Register Map: 0x0000-0x00FF
- Linear actuator control
- Load cell monitoring
- Position control
- Safety limits
```

### **Cargo Door Module (ID: 0x07) - RS485:**
```
Module Type: 0x0007 (Cargo Door)
Module Name: "DOOR" (Door)
Register Map: 0x0000-0x00FF
- Door actuator control
- Door position sensor
- Cargo presence sensor
- Safety interlocks
```

### **Safety Extended Module (ID: 0x08) - RS485:**
```
Module Type: 0x0008 (Safety Extended)
Module Name: "SAFX" (Safety Extended)
Register Map: 0x0000-0x00FF
- Additional safety sensors
- Advanced safety logic
- Safety zone management
- Emergency procedures
```

---

## 🔌 **PLUG-AND-PLAY MODULES - RS485**

### **RFID Reader Module (ID: 0x09) - RS485:**
```
Module Type: 0x0009 (RFID Reader)
Module Name: "RFID" (RFID)
Register Map: 0x0000-0x00FF
- RFID tag reading
- Signal strength monitoring
- Tag database management
- Communication protocol
```

### **Camera Module (ID: 0x0A) - RS485:**
```
Module Type: 0x000A (Camera)
Module Name: "CAME" (Camera)
Register Map: 0x0000-0x00FF
- Camera control
- Image capture
- Video streaming
- Image processing
```

### **Environmental Sensor Module (ID: 0x0B) - RS485:**
```
Module Type: 0x000B (Environmental)
Module Name: "ENVI" (Environmental)
Register Map: 0x0000-0x00FF
- Temperature sensors
- Humidity sensors
- Pressure sensors
- Air quality sensors
```

---

## 🔄 **AUTO-DISCOVERY PROTOCOL**

### **Discovery Process:**
```
1. Master sends discovery broadcast (Address 0x00)
2. All modules respond với their information
3. Master builds module registry
4. Master assigns addresses (1-32)
5. Master validates module compatibility
6. Master starts normal communication
```

### **Module Information Response:**
```
Register 0x00FE: Module Type (16-bit)
Register 0x00FF: Module Name (4 characters)
Register 0x00FD: Module Version (16-bit)
Register 0x00FC: Module Capabilities (16-bit)
Register 0x00FB: Module Status (16-bit)
```

### **Address Assignment:**
```
Mandatory Modules:
- 0x01: Master Control Module
- 0x02: Power Module
- 0x03: Safety Module
- 0x04: Travel Motor Module
- 0x05: Dock & Location Module

Optional Modules:
- 0x06: Lifter Motor Module
- 0x07: Cargo Door Module
- 0x08: Safety Extended Module

Plug-and-Play Modules:
- 0x09-0xFF: Dynamic assignment
```

---

## 📡 **MASTER ↔ CENTER COMMUNICATION**

### **HTTP API:**
```
Base URL: http://localhost:8080/api/v1
Authentication: Bearer token
Content-Type: application/json
Rate Limiting: 100 requests/minute
Timeout: 30 seconds
```

### **WebSocket Real-time:**
```
URL: ws://localhost:8080/ws
Protocol: JSON messages
Authentication: Token in handshake
Heartbeat: 30 seconds
Reconnection: Automatic
```

### **Data Flow:**
```
Slave Modules (RS485) → Master Module → Center (HTTP/WS)
Center (HTTP/WS) → Master Module → Slave Modules (RS485)
```

---

## 🔒 **SECURITY & RELIABILITY**

### **RS485 Security:**
```
CRC Validation: Modbus CRC-16
Timeout Handling: 100ms per module
Retry Logic: 3 attempts per command
Error Detection: Frame errors, CRC errors
Fault Recovery: Automatic reconnection
```

### **Network Security:**
```
SSL/TLS: HTTPS và WSS
Authentication: Token-based
Authorization: Role-based access
Encryption: AES-256
Certificate: Self-signed hoặc CA-signed
```

---

**Changelog:**
- v2.1 (2025-01-27): Updated với RS485 cho tất cả modules
- v2.0 (2025-01-27): Added Dock & Location module interface
- v1.0 (2025-01-27): Initial interface specification

**Status:** Updated với RS485 cho tất cả modules  
**Next Steps:** Implementation theo RS485 standard


