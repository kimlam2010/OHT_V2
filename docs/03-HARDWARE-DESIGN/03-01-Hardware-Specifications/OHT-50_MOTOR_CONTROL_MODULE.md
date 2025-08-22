---
title: "Oht 50 Motor Control Module"
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

# MODULE ĐIỀU KHIỂN ĐỘNG CƠ OHT-50
## Motor Control Module - Dual Mode Operation

**Phiên bản:** v2.0  
**Ngày tạo:** $(date +%Y-%m-%d)  
**PM:** [Tên PM]  
**Trạng thái:** Technical Specification  

---

## 📋 **TỔNG QUAN MODULE**

### **🎯 Mục tiêu:**
Module điều khiển động cơ OHT-50 cung cấp 2 chế độ hoạt động linh hoạt:
- **Mode 1:** Điều khiển 2 động cơ DC (differential drive)
- **Mode 2:** Điều khiển 1 động cơ Stepper (precision positioning)

### **🏗️ Kiến trúc:**
```
Motor Control Module Architecture
├── Microcontroller: STM32F407VGT6
├── Motor Drivers: 2x DRV8833 (Dual H-Bridge)
├── Operating Modes:
│   ├── Mode 1: 2x DC Motors (2 H-Bridges)
│   └── Mode 2: 1x Stepper Motor (2 H-Bridges)
├── Encoders: 2x Quadrature Encoders
├── Communication: RS485/Modbus RTU
├── Power: 12-24V DC Input
├── Safety: Overcurrent, Overvoltage Protection
└── Status: Real-time feedback
```

---

## ⚙️ **3 OPTION THÔNG SỐ KỸ THUẬT**

### **🔧 OPTION 1: CƠ BẢN (12V)**
```
Option 1: Basic Configuration (12V)
├── Power Supply: 12V DC ±10%
├── Current Rating: 2A per channel
├── DC Motors: 2x 12V, 24W each
├── Stepper Motor: 1x 12V, 12W
├── Encoder Resolution: 1000 PPR
├── Position Accuracy: ±2mm
├── Speed Range: 0-1500 RPM (DC), 0-1000 steps/s (Stepper)
├── Torque: 0.5 Nm (DC), 0.3 Nm (Stepper)
├── Operating Temperature: -10°C to +50°C
├── Protection: IP54
├── Communication: RS485/Modbus RTU
├── Auto-register: Basic info only
└── Cost: Low
```

### **🔧 OPTION 2: TIÊU CHUẨN (24V)**
```
Option 2: Standard Configuration (24V)
├── Power Supply: 24V DC ±10%
├── Current Rating: 3A per channel
├── DC Motors: 2x 24V, 72W each
├── Stepper Motor: 1x 24V, 36W
├── Encoder Resolution: 2000 PPR
├── Position Accuracy: ±1mm
├── Speed Range: 0-3000 RPM (DC), 0-2000 steps/s (Stepper)
├── Torque: 1.5 Nm (DC), 0.8 Nm (Stepper)
├── Operating Temperature: -20°C to +60°C
├── Protection: IP65
├── Communication: RS485/Modbus RTU
├── Auto-register: Full info + diagnostics
├── Safety: SIL2 certified
└── Cost: Medium
```

### **🔧 OPTION 3: CAO CẤP (24V)**
```
Option 3: Premium Configuration (24V)
├── Power Supply: 24V DC ±5%
├── Current Rating: 5A per channel
├── DC Motors: 2x 24V, 120W each
├── Stepper Motor: 1x 24V, 60W
├── Encoder Resolution: 4000 PPR
├── Position Accuracy: ±0.5mm
├── Speed Range: 0-5000 RPM (DC), 0-4000 steps/s (Stepper)
├── Torque: 3.0 Nm (DC), 1.5 Nm (Stepper)
├── Operating Temperature: -40°C to +85°C
├── Protection: IP67
├── Communication: RS485/Modbus RTU + CAN
├── Auto-register: Full info + diagnostics + predictive
├── Safety: SIL3 certified
├── Advanced Features: Predictive maintenance, AI optimization
└── Cost: High
```

---

## 📡 **REGISTER MAP CHO AUTO-REGISTER**

### **1. MODULE INFORMATION REGISTERS (0x0000-0x00FF)**

#### **Module Identity (0x0000-0x000F):**
```
Module Identity Registers
├── 0x0000: Module Type (0x0001 = Motor Control Module)
├── 0x0001: Hardware Version (0x0100 = v1.0)
├── 0x0002: Firmware Version (0x0100 = v1.0)
├── 0x0003: Module ID (Unique 16-bit identifier)
├── 0x0004: Module ID High (Unique 32-bit identifier)
├── 0x0005: Serial Number Low (32-bit)
├── 0x0006: Serial Number High (32-bit)
├── 0x0007: Manufacturing Date (YYYYMMDD)
├── 0x0008: Operating Mode (0=Stop, 1=DC Mode, 2=Stepper Mode)
├── 0x0009: Power Supply Voltage (V × 10)
├── 0x000A: Current Rating (A × 10)
├── 0x000B: Temperature (°C)
├── 0x000C: Status (0=OK, 1=Warning, 2=Error, 3=Emergency)
├── 0x000D: Error Code (0=No Error, 1=Overcurrent, 2=Overtemperature, 3=Communication)
├── 0x000E: Uptime Low (seconds)
└── 0x000F: Uptime High (seconds)
```

#### **Capability Information (0x0010-0x001F):**
```
Capability Registers
├── 0x0010: Max DC Motors (2)
├── 0x0011: Max Stepper Motors (1)
├── 0x0012: Max Encoders (2)
├── 0x0013: Max Current per Channel (A × 10)
├── 0x0014: Max Voltage (V × 10)
├── 0x0015: Max Speed DC (RPM)
├── 0x0016: Max Speed Stepper (steps/s)
├── 0x0017: Max Torque DC (Nm × 100)
├── 0x0018: Max Torque Stepper (Nm × 100)
├── 0x0019: Position Accuracy (mm × 100)
├── 0x001A: Encoder Resolution (PPR)
├── 0x001B: Communication Protocols (0x0001 = Modbus RTU)
├── 0x001C: Safety Level (0=Basic, 1=SIL2, 2=SIL3)
├── 0x001D: Protection Level (0x0054 = IP54, 0x0065 = IP65, 0x0067 = IP67)
├── 0x001E: Operating Temperature Min (°C)
└── 0x001F: Operating Temperature Max (°C)
```

#### **Auto-Discovery Registers (0x0020-0x002F):**
```
Auto-Discovery Registers
├── 0x0020: Auto-Discovery Status (0=Idle, 1=Running, 2=Complete, 3=Error)
├── 0x0021: Auto-Discovery Progress (0-100%)
├── 0x0022: Detected Motor 1 Type (0=Unknown, 1=DC, 2=Stepper)
├── 0x0023: Detected Motor 2 Type (0=Unknown, 1=DC, 2=Stepper)
├── 0x0024: Detected Encoder 1 Type (0=Unknown, 1=Quadrature, 2=Absolute)
├── 0x0025: Detected Encoder 2 Type (0=Unknown, 1=Quadrature, 2=Absolute)
├── 0x0026: Motor 1 Parameters Valid (0=No, 1=Yes)
├── 0x0027: Motor 2 Parameters Valid (0=No, 1=Yes)
├── 0x0028: Encoder 1 Parameters Valid (0=No, 1=Yes)
├── 0x0029: Encoder 2 Parameters Valid (0=No, 1=Yes)
├── 0x002A: Calibration Status (0=Not Calibrated, 1=Calibrated)
├── 0x002B: Calibration Progress (0-100%)
├── 0x002C: Configuration Status (0=Default, 1=Custom)
├── 0x002D: Test Status (0=Not Tested, 1=Passed, 2=Failed)
├── 0x002E: Ready Status (0=Not Ready, 1=Ready)
└── 0x002F: Reserved
```

### **2. CONTROL REGISTERS (0x0100-0x01FF)**

#### **Mode Control (0x0100-0x010F):**
```
Mode Control Registers
├── 0x0100: Operating Mode Set (0=Stop, 1=DC Mode, 2=Stepper Mode)
├── 0x0101: Mode Transition Status (0=Idle, 1=In Progress, 2=Complete, 3=Error)
├── 0x0102: Emergency Stop (0=Normal, 1=Emergency, 2=Reset)
├── 0x0103: System Reset (0=Idle, 1=Reset, 2=Factory Reset)
├── 0x0104: Configuration Save (0=Idle, 1=Save, 2=Load Default)
├── 0x0105: Auto-Discovery Start (0=Idle, 1=Start, 2=Stop)
├── 0x0106: Calibration Start (0=Idle, 1=Start, 2=Stop)
├── 0x0107: Test Start (0=Idle, 1=Start, 2=Stop)
├── 0x0108: Motor 1 Enable (0=Disable, 1=Enable)
├── 0x0109: Motor 2 Enable (0=Disable, 1=Enable)
├── 0x010A: Stepper Enable (0=Disable, 1=Enable)
├── 0x010B: Safety Mode (0=Normal, 1=Safe, 2=Ultra Safe)
├── 0x010C: Motion Profile (0=Linear, 1=S-Curve, 2=Trapezoid)
├── 0x010D: Communication Mode (0=Modbus, 1=CAN, 2=Both)
├── 0x010E: Diagnostic Level (0=Basic, 1=Standard, 2=Advanced)
└── 0x010F: Reserved
```

#### **DC Motor Control (0x0110-0x011F):**
```
DC Motor Control Registers
├── 0x0110: DC Motor 1 Target Speed (RPM)
├── 0x0111: DC Motor 2 Target Speed (RPM)
├── 0x0112: DC Motor 1 Target Position (mm × 100)
├── 0x0113: DC Motor 2 Target Position (mm × 100)
├── 0x0114: DC Motor 1 Acceleration (RPM/s)
├── 0x0115: DC Motor 2 Acceleration (RPM/s)
├── 0x0116: DC Motor 1 Deceleration (RPM/s)
├── 0x0117: DC Motor 2 Deceleration (RPM/s)
├── 0x0118: DC Motor 1 Torque Limit (Nm × 100)
├── 0x0119: DC Motor 2 Torque Limit (Nm × 100)
├── 0x011A: DC Motor 1 Current Limit (mA)
├── 0x011B: DC Motor 2 Current Limit (mA)
├── 0x011C: DC Motor 1 PID P Gain (× 1000)
├── 0x011D: DC Motor 1 PID I Gain (× 1000)
├── 0x011E: DC Motor 1 PID D Gain (× 1000)
└── 0x011F: DC Motor 2 PID P Gain (× 1000)
```

#### **Stepper Motor Control (0x0120-0x012F):**
```
Stepper Motor Control Registers
├── 0x0120: Stepper Target Position (mm × 100)
├── 0x0121: Stepper Target Speed (steps/s)
├── 0x0122: Stepper Acceleration (steps/s²)
├── 0x0123: Stepper Deceleration (steps/s²)
├── 0x0124: Stepper Microstepping (1, 2, 4, 8, 16, 32)
├── 0x0125: Stepper Torque Limit (Nm × 100)
├── 0x0126: Stepper Current Limit (mA)
├── 0x0127: Stepper PID P Gain (× 1000)
├── 0x0128: Stepper PID I Gain (× 1000)
├── 0x0129: Stepper PID D Gain (× 1000)
├── 0x012A: Stepper Homing Speed (steps/s)
├── 0x012B: Stepper Homing Direction (0=Forward, 1=Reverse)
├── 0x012C: Stepper Homing Offset (steps)
├── 0x012D: Stepper Soft Limit Min (steps)
├── 0x012E: Stepper Soft Limit Max (steps)
└── 0x012F: Reserved
```

### **3. STATUS REGISTERS (0x0200-0x02FF)**

#### **System Status (0x0200-0x020F):**
```
System Status Registers
├── 0x0200: System Status (0=OK, 1=Warning, 2=Error, 3=Emergency)
├── 0x0201: Operating Mode (0=Stop, 1=DC Mode, 2=Stepper Mode)
├── 0x0202: Power Supply Voltage (V × 10)
├── 0x0203: System Current (mA)
├── 0x0204: Temperature (°C)
├── 0x0205: Error Code (0=No Error, 1=Overcurrent, 2=Overtemperature, 3=Communication)
├── 0x0206: Warning Code (0=No Warning, 1=High Temperature, 2=High Current)
├── 0x0207: Uptime Low (seconds)
├── 0x0208: Uptime High (seconds)
├── 0x0209: Communication Quality (0-100%)
├── 0x020A: Safety Status (0=Safe, 1=Warning, 2=Danger)
├── 0x020B: Emergency Stop Status (0=Normal, 1=Active)
├── 0x020C: System Health Score (0-100%)
├── 0x020D: Maintenance Required (0=No, 1=Yes)
├── 0x020E: Predictive Maintenance Days (days)
└── 0x020F: Reserved
```

#### **Motor Status (0x0210-0x021F):**
```
Motor Status Registers
├── 0x0210: DC Motor 1 Status (0=Stopped, 1=Running, 2=Error, 3=Calibrating)
├── 0x0211: DC Motor 2 Status (0=Stopped, 1=Running, 2=Error, 3=Calibrating)
├── 0x0212: Stepper Status (0=Stopped, 1=Running, 2=Error, 3=Calibrating)
├── 0x0213: DC Motor 1 Current Speed (RPM)
├── 0x0214: DC Motor 2 Current Speed (RPM)
├── 0x0215: Stepper Current Speed (steps/s)
├── 0x0216: DC Motor 1 Current Position (mm × 100)
├── 0x0217: DC Motor 2 Current Position (mm × 100)
├── 0x0218: Stepper Current Position (mm × 100)
├── 0x0219: DC Motor 1 Current (mA)
├── 0x021A: DC Motor 2 Current (mA)
├── 0x021B: Stepper Current (mA)
├── 0x021C: DC Motor 1 Torque (Nm × 100)
├── 0x021D: DC Motor 2 Torque (Nm × 100)
├── 0x021E: Stepper Torque (Nm × 100)
└── 0x021F: Reserved
```

#### **Encoder Status (0x0220-0x022F):**
```
Encoder Status Registers
├── 0x0220: Encoder 1 Status (0=OK, 1=Error, 2=No Signal)
├── 0x0221: Encoder 2 Status (0=OK, 1=Error, 2=No Signal)
├── 0x0222: Encoder 1 Counts Low (32-bit)
├── 0x0223: Encoder 1 Counts High (32-bit)
├── 0x0224: Encoder 2 Counts Low (32-bit)
├── 0x0225: Encoder 2 Counts High (32-bit)
├── 0x0226: Encoder 1 Speed (counts/s)
├── 0x0227: Encoder 2 Speed (counts/s)
├── 0x0228: Encoder 1 Resolution (PPR)
├── 0x0229: Encoder 2 Resolution (PPR)
├── 0x022A: Encoder 1 Type (0=Unknown, 1=Quadrature, 2=Absolute)
├── 0x022B: Encoder 2 Type (0=Unknown, 1=Quadrature, 2=Absolute)
├── 0x022C: Encoder 1 Health (0-100%)
├── 0x022D: Encoder 2 Health (0-100%)
├── 0x022E: Encoder 1 Temperature (°C)
└── 0x022F: Encoder 2 Temperature (°C)
```

---

## 🔧 **HƯỚNG DẪN SỬ DỤNG CHO CÁC TEAM**

### **1. EMBED TEAM - PHẦN CỨNG**

#### **Yêu cầu phần cứng:**
```
Hardware Requirements
├── Microcontroller: STM32F407VGT6
├── Motor Drivers: 2x DRV8833 (Dual H-Bridge)
├── Power Supply: 12-24V DC, 5A minimum
├── Encoders: 2x Quadrature Encoders
├── Communication: RS485 transceiver
├── Protection: Overcurrent, overvoltage circuits
├── Connectors: Screw terminals for power and motors
└── PCB: 4-layer design with proper grounding
```

#### **Pin assignments:**
```
Pin Assignments
├── STM32F407 GPIO:
│   ├── PA0-PA1: Encoder 1 (A, B)
│   ├── PA2-PA3: Encoder 2 (A, B)
│   ├── PA4-PA5: DRV8833_1 (PWM1, PWM2)
│   ├── PA6-PA7: DRV8833_2 (PWM1, PWM2)
│   ├── PA8-PA9: DRV8833_1 (IN1, IN2)
│   ├── PA10-PA11: DRV8833_2 (IN1, IN2)
│   ├── PA12: DRV8833_1 Enable
│   ├── PA13: DRV8833_2 Enable
│   ├── PA14-PA15: UART1 (RS485)
│   └── PB0-PB1: Current sensing
├── Power:
│   ├── VIN: 12-24V DC input
│   ├── 3.3V: MCU power
│   ├── 5V: Logic power
│   └── GND: Common ground
└── Motor Connections:
    ├── MOTOR1_A, MOTOR1_B: DC Motor 1
    ├── MOTOR2_A, MOTOR2_B: DC Motor 2
    └── STEPPER_A, STEPPER_B: Stepper Motor
```

#### **Test procedures:**
```
Hardware Test Procedures
├── Power-up Test:
│   ├── Check 3.3V and 5V rails
│   ├── Verify MCU boot sequence
│   └── Check communication interface
├── Motor Driver Test:
│   ├── Test H-Bridge functionality
│   ├── Verify PWM generation
│   └── Check current sensing
├── Encoder Test:
│   ├── Verify quadrature signals
│   ├── Test position counting
│   └── Check speed calculation
├── Communication Test:
│   ├── Test RS485 communication
│   ├── Verify Modbus protocol
│   └── Check register access
└── Integration Test:
    ├── Test mode switching
    ├── Verify safety features
    └── Check thermal management
```

### **2. FIRMWARE TEAM - PHẦN MỀM**

#### **HAL Functions:**
```c
// Module initialization
hal_status_t motor_module_init(const motor_config_t *config);
hal_status_t motor_module_deinit(void);

// Mode control
hal_status_t motor_set_mode(motor_mode_t mode);
hal_status_t motor_get_mode(motor_mode_t *mode);

// DC motor control
hal_status_t dc_motor_set_speed(uint8_t motor_id, float speed_rpm);
hal_status_t dc_motor_set_position(uint8_t motor_id, float position_mm);
hal_status_t dc_motor_get_status(uint8_t motor_id, dc_motor_status_t *status);

// Stepper motor control
hal_status_t stepper_set_position(float position_mm);
hal_status_t stepper_set_speed(float speed_steps_per_sec);
hal_status_t stepper_get_status(stepper_status_t *status);

// Auto-discovery
hal_status_t motor_auto_discovery_start(void);
hal_status_t motor_auto_discovery_get_progress(uint8_t *progress);
hal_status_t motor_auto_discovery_get_results(motor_discovery_t *results);

// Safety and diagnostics
hal_status_t motor_emergency_stop(void);
hal_status_t motor_get_system_status(system_status_t *status);
hal_status_t motor_get_health_status(health_status_t *status);
```

#### **Data structures:**
```c
typedef struct {
    uint16_t module_type;
    uint16_t hardware_version;
    uint16_t firmware_version;
    uint32_t module_id;
    uint32_t serial_number;
    uint32_t manufacturing_date;
    motor_mode_t operating_mode;
    uint16_t power_voltage;
    uint16_t current_rating;
    int16_t temperature;
    uint8_t status;
    uint8_t error_code;
    uint32_t uptime;
} module_info_t;

typedef struct {
    uint8_t motor_id;
    uint8_t status;
    float current_speed;
    float current_position;
    uint16_t current;
    uint16_t torque;
    uint32_t encoder_counts;
} dc_motor_status_t;

typedef struct {
    uint8_t status;
    float current_speed;
    float current_position;
    uint16_t current;
    uint16_t torque;
    uint32_t encoder_counts;
    uint8_t microstepping;
} stepper_status_t;
```

#### **Integration guide:**
```
Firmware Integration Steps
├── 1. Include HAL headers
├── 2. Initialize module with configuration
├── 3. Set operating mode (DC or Stepper)
├── 4. Configure motor parameters
├── 5. Enable motors
├── 6. Start control loop
├── 7. Monitor status and handle errors
└── 8. Implement safety features
```

### **3. BACKEND TEAM - API SERVICES**

#### **REST API Endpoints:**
```http
# Module information
GET /api/v1/motor-module/info
GET /api/v1/motor-module/capabilities
GET /api/v1/motor-module/status

# Mode control
POST /api/v1/motor-module/mode
{
    "mode": "dc|stepper|stop"
}

# DC motor control
POST /api/v1/motor-module/dc/{motor_id}/speed
{
    "speed": 1000.0,
    "unit": "rpm"
}

POST /api/v1/motor-module/dc/{motor_id}/position
{
    "position": 500.0,
    "unit": "mm"
}

# Stepper motor control
POST /api/v1/motor-module/stepper/position
{
    "position": 100.0,
    "unit": "mm"
}

POST /api/v1/motor-module/stepper/speed
{
    "speed": 1000.0,
    "unit": "steps_per_sec"
}

# Auto-discovery
POST /api/v1/motor-module/auto-discovery/start
GET /api/v1/motor-module/auto-discovery/progress
GET /api/v1/motor-module/auto-discovery/results

# Safety and diagnostics
POST /api/v1/motor-module/emergency-stop
GET /api/v1/motor-module/health
GET /api/v1/motor-module/diagnostics
```

#### **WebSocket Events:**
```javascript
// Module status updates
{
    "event": "module_status_update",
    "data": {
        "module_id": 1,
        "mode": "dc|stepper|stop",
        "status": "ok|warning|error|emergency",
        "temperature": 45,
        "voltage": 24.5,
        "uptime": 3600
    }
}

// Motor status updates
{
    "event": "motor_status_update",
    "data": {
        "motor_id": 1,
        "type": "dc|stepper",
        "status": "stopped|running|error|calibrating",
        "speed": 1000.0,
        "position": 500.0,
        "current": 1500,
        "torque": 2.5
    }
}

// Auto-discovery events
{
    "event": "auto_discovery_event",
    "data": {
        "status": "started|progress|completed|error",
        "progress": 75,
        "detected_motors": [
            {"motor_id": 1, "type": "dc", "encoder": "quadrature"},
            {"motor_id": 2, "type": "stepper", "encoder": "quadrature"}
        ]
    }
}
```

#### **Database schema:**
```sql
-- Module information
CREATE TABLE motor_modules (
    id INTEGER PRIMARY KEY,
    module_type VARCHAR(50),
    hardware_version VARCHAR(20),
    firmware_version VARCHAR(20),
    serial_number VARCHAR(50),
    manufacturing_date DATE,
    power_voltage INTEGER,
    current_rating INTEGER,
    status VARCHAR(20),
    created_at TIMESTAMP,
    updated_at TIMESTAMP
);

-- Motor configurations
CREATE TABLE motor_configs (
    id INTEGER PRIMARY KEY,
    module_id INTEGER,
    motor_id INTEGER,
    motor_type VARCHAR(20),
    max_speed INTEGER,
    max_torque INTEGER,
    encoder_resolution INTEGER,
    pid_p_gain INTEGER,
    pid_i_gain INTEGER,
    pid_d_gain INTEGER,
    created_at TIMESTAMP,
    updated_at TIMESTAMP
);

-- Motor status history
CREATE TABLE motor_status_history (
    id INTEGER PRIMARY KEY,
    module_id INTEGER,
    motor_id INTEGER,
    status VARCHAR(20),
    speed REAL,
    position REAL,
    current INTEGER,
    torque INTEGER,
    temperature INTEGER,
    timestamp TIMESTAMP
);
```

### **4. FRONTEND TEAM - GIAO DIỆN**

#### **Dashboard Components:**
```javascript
// Module Status Card
const ModuleStatusCard = ({ moduleId }) => {
    const [moduleInfo, setModuleInfo] = useState(null);
    
    return (
        <Card>
            <CardHeader>
                <h3>Motor Module {moduleId}</h3>
                <StatusBadge status={moduleInfo?.status} />
            </CardHeader>
            <CardBody>
                <div className="module-info">
                    <InfoRow label="Mode" value={moduleInfo?.mode} />
                    <InfoRow label="Voltage" value={`${moduleInfo?.voltage}V`} />
                    <InfoRow label="Temperature" value={`${moduleInfo?.temperature}°C`} />
                    <InfoRow label="Uptime" value={formatUptime(moduleInfo?.uptime)} />
                </div>
                <div className="module-controls">
                    <Button onClick={() => handleEmergencyStop(moduleId)}>
                        Emergency Stop
                    </Button>
                    <Button onClick={() => handleReset(moduleId)}>
                        Reset
                    </Button>
                </div>
            </CardBody>
        </Card>
    );
};

// Mode Selection Panel
const ModeSelectionPanel = ({ moduleId, currentMode, onModeChange }) => {
    return (
        <Card>
            <CardHeader>
                <h3>Operating Mode</h3>
            </CardHeader>
            <CardBody>
                <RadioGroup value={currentMode} onChange={onModeChange}>
                    <Radio value="stop">Stop</Radio>
                    <Radio value="dc">DC Mode (2 Motors)</Radio>
                    <Radio value="stepper">Stepper Mode (1 Motor)</Radio>
                </RadioGroup>
                <div className="mode-description">
                    {currentMode === 'dc' && (
                        <p>Control 2 DC motors for differential drive</p>
                    )}
                    {currentMode === 'stepper' && (
                        <p>Control 1 stepper motor for precision positioning</p>
                    )}
                </div>
            </CardBody>
        </Card>
    );
};

// Motor Control Panel
const MotorControlPanel = ({ moduleId, mode, motors }) => {
    return (
        <Card>
            <CardHeader>
                <h3>Motor Control</h3>
            </CardHeader>
            <CardBody>
                {mode === 'dc' && (
                    <div className="dc-motors">
                        <DCMotorControl motorId={1} motor={motors[0]} />
                        <DCMotorControl motorId={2} motor={motors[1]} />
                    </div>
                )}
                {mode === 'stepper' && (
                    <div className="stepper-motor">
                        <StepperMotorControl motor={motors[0]} />
                    </div>
                )}
            </CardBody>
        </Card>
    );
};

// Auto-Discovery Panel
const AutoDiscoveryPanel = ({ moduleId }) => {
    const [discoveryStatus, setDiscoveryStatus] = useState('idle');
    const [progress, setProgress] = useState(0);
    
    return (
        <Card>
            <CardHeader>
                <h3>Auto-Discovery</h3>
            </CardHeader>
            <CardBody>
                <div className="discovery-status">
                    <StatusBadge status={discoveryStatus} />
                    <ProgressBar value={progress} max={100} />
                </div>
                <div className="discovery-controls">
                    <Button onClick={handleStartDiscovery} disabled={discoveryStatus === 'running'}>
                        Start Discovery
                    </Button>
                    <Button onClick={handleStopDiscovery} disabled={discoveryStatus !== 'running'}>
                        Stop Discovery
                    </Button>
                </div>
            </CardBody>
        </Card>
    );
};
```

#### **State Management:**
```javascript
// Redux slice for motor module
const motorModuleSlice = createSlice({
    name: 'motorModule',
    initialState: {
        moduleInfo: null,
        mode: 'stop',
        motors: [],
        discoveryStatus: 'idle',
        discoveryProgress: 0,
        systemStatus: 'ok',
        error: null
    },
    reducers: {
        setModuleInfo: (state, action) => {
            state.moduleInfo = action.payload;
        },
        setMode: (state, action) => {
            state.mode = action.payload;
        },
        setMotors: (state, action) => {
            state.motors = action.payload;
        },
        setDiscoveryStatus: (state, action) => {
            state.discoveryStatus = action.payload;
        },
        setDiscoveryProgress: (state, action) => {
            state.discoveryProgress = action.payload;
        },
        setSystemStatus: (state, action) => {
            state.systemStatus = action.payload;
        },
        setError: (state, action) => {
            state.error = action.payload;
        }
    }
});
```

---

## 🎯 **KẾT LUẬN**

Module điều khiển động cơ OHT-50 cung cấp:

### **✅ Tính năng chính:**
- **2 chế độ hoạt động:** DC Mode (2 motors) và Stepper Mode (1 motor)
- **3 option thông số:** Cơ bản (12V), Tiêu chuẩn (24V), Cao cấp (24V)
- **Auto-register:** Register map chi tiết cho auto-discovery
- **Safety systems:** Overcurrent, overtemperature protection
- **Real-time monitoring:** Encoder feedback và diagnostics

### **✅ Hướng dẫn team:**
- **EMBED:** Pin assignments, test procedures, hardware requirements
- **FIRMWARE:** HAL functions, data structures, integration guide
- **BACKEND:** REST API, WebSocket events, database schema
- **FRONTEND:** Dashboard components, state management, UI controls

### **✅ Register Map:**
- **Module Information (0x0000-0x00FF):** Identity, capabilities, auto-discovery
- **Control Registers (0x0100-0x01FF):** Mode control, motor control
- **Status Registers (0x0200-0x02FF):** System status, motor status, encoder status

**Status:** Ready for implementation  
**Next Steps:** Team development based on selected option

---

**📄 Tài liệu này sẽ được cập nhật khi có thay đổi trong thiết kế hoặc yêu cầu mới.**

**Ngày cập nhật:** $(date +%Y-%m-%d)  
**Phiên bản:** v2.0  
**Trạng thái:** Technical Specification
