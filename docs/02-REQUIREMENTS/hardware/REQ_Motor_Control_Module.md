# Motor Control Module Specification - OHT-50 (v2.1)

**Phiên bản:** 2.1  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated với DC brushed motor 12V và PID speed control

---

## 🎯 **MỤC TIÊU**

Định nghĩa chi tiết module điều khiển động cơ DC chổi than 12V cho hệ thống OHT-50 với PID control theo % speed và tích hợp location module cho velocity control (m/s).

---

## 🚗 **TRAVEL MOTOR MODULE OVERVIEW**

### **Module Type:** Travel Motor Control (ID: 0x04)
### **Module Name:** "TRAV" (Travel)

### **Chức năng chính:**
- **DC Brushed Motors:** 2x DC chổi than 12V cho differential drive
- **PID Speed Control:** Điều khiển tốc độ theo % với PID control
- **Velocity Control:** Tích hợp với location module để đạt tốc độ m/s mong muốn
- **Backend Integration:** Backend setting kết hợp location và motor control
- **Safety Integration:** Tích hợp với Safety module cho emergency stop

---

## 🔧 **HARDWARE SPECIFICATIONS**

### **Motor Specifications:**
```
Motor Type:          2x DC Brushed Motors (chổi than)
Voltage:             12V DC
Power Rating:        100W each (200W total)
Speed Range:         0-3000 RPM
Torque:              0.5Nm continuous, 1.5Nm peak
Current Rating:      8A continuous, 15A peak
Efficiency:          >80% at rated load
Operating Temp:      -20°C to +60°C
Protection:          IP65
```

### **Motor Driver Specifications:**
```
Driver Type:         H-bridge motor drivers
Voltage:             12V DC
Current Rating:      15A continuous, 20A peak
PWM Frequency:       20kHz
Control Mode:        PWM speed control với PID
Protection:          Over-current, over-temperature, short-circuit
Interface:           SPI/UART cho configuration
```

### **Speed Sensor Specifications:**
```
Type:                Hall effect speed sensor
Resolution:          60 pulses per revolution
Accuracy:            ±1% speed accuracy
Interface:           Digital output
Operating Temp:      -40°C to +85°C
Protection:          IP65
```

### **Gearbox Specifications:**
```
Type:                Planetary gearbox
Ratio:               20:1
Efficiency:          >85%
Backlash:            < 2°
Lubrication:         Grease, maintenance-free
Life:                5,000 hours
```

### **Wheel Specifications:**
```
Type:                Omni-directional wheels
Diameter:            150mm
Material:            Polyurethane tread
Load Capacity:       50kg per wheel
Grip:                High traction, low noise
```

---

## 🧭 **SPEED CONTROL INTEGRATION**

### **PID Speed Control:**
```c
// PID speed control theo % speed
typedef struct {
    float Kp;        // Proportional gain
    float Ki;        // Integral gain
    float Kd;        // Derivative gain
    float setpoint;  // Target speed (%)
    float feedback;  // Current speed (%)
    float output;    // PWM output (0-100%)
    float integral;  // Integral term
    float prev_error; // Previous error
} pid_speed_controller_t;

// Speed control parameters
#define SPEED_MIN_PERCENT    0.0f    // Minimum speed 0%
#define SPEED_MAX_PERCENT    100.0f  // Maximum speed 100%
#define SPEED_DEADBAND       2.0f    // Speed deadband 2%
#define PID_UPDATE_RATE      100     // PID update rate 100Hz
```

### **Velocity Control (m/s):**
```c
// Velocity control với location feedback
typedef struct {
    float target_velocity;   // Target velocity (m/s)
    float current_velocity;  // Current velocity (m/s)
    float speed_percent;     // Calculated speed percentage
    float wheel_radius;      // Wheel radius (m)
    float gear_ratio;        // Gear ratio
    float max_velocity;      // Maximum velocity (m/s)
} velocity_control_t;

// Velocity to speed conversion
float velocity_to_speed_percent(float target_velocity_mps) {
    float wheel_circumference = 2 * PI * WHEEL_RADIUS;
    float target_rpm = (target_velocity_mps * 60) / wheel_circumference;
    float motor_rpm = target_rpm * GEAR_RATIO;
    float speed_percent = (motor_rpm / MAX_MOTOR_RPM) * 100.0f;
    return constrain(speed_percent, 0.0f, 100.0f);
}
```

### **Backend Integration:**
```c
// Backend setting cho motor control
typedef struct {
    float target_velocity_mps;    // Target velocity from backend (m/s)
    float max_velocity_mps;       // Maximum velocity limit (m/s)
    float acceleration_mps2;      // Acceleration limit (m/s²)
    float deceleration_mps2;      // Deceleration limit (m/s²)
    uint8_t control_mode;         // Control mode (0=Manual, 1=Auto)
    uint8_t safety_enabled;       // Safety control enabled
} backend_motor_settings_t;
```

---

## 🔌 **COMMUNICATION INTERFACE**

### **Modbus RTU Register Map:**
```
// Motor Control Registers (0x0000-0x00FF)
0x0000: Motor 1 Control Mode (1=Manual, 2=Auto, 3=Backend)
0x0001: Motor 1 Enable (0=Disable, 1=Enable)
0x0002: Motor 1 Target Speed (% × 10)
0x0003: Motor 1 Actual Speed (% × 10)
0x0004: Motor 1 PWM Output (% × 10)
0x0005: Motor 1 Current (mA)
0x0006: Motor 1 Temperature (°C)
0x0007: Motor 1 Status

0x0010-0x0017: Motor 2 (tương tự)

// Velocity Control Registers
0x0020: Target Velocity (m/s × 100)
0x0021: Current Velocity (m/s × 100)
0x0022: Speed Percentage (% × 10)
0x0023: Velocity Error (m/s × 100)
0x0024: Max Velocity Limit (m/s × 100)
0x0025: Acceleration Limit (m/s² × 100)
0x0026: Deceleration Limit (m/s² × 100)
0x0027: Velocity Status

// PID Control Registers
0x0030: Speed PID Kp (× 1000)
0x0031: Speed PID Ki (× 1000)
0x0032: Speed PID Kd (× 1000)
0x0033: PID Update Rate (Hz)
0x0034: PID Output (% × 10)
0x0035: PID Error (% × 10)
0x0036: PID Status
0x0037: PID Mode (0=Manual, 1=Auto)

// Backend Integration Registers
0x0040: Backend Target Velocity (m/s × 100)
0x0041: Backend Max Velocity (m/s × 100)
0x0042: Backend Acceleration (m/s² × 100)
0x0043: Backend Deceleration (m/s² × 100)
0x0044: Backend Control Mode (0=Manual, 1=Auto)
0x0045: Backend Safety Enabled (0=Disabled, 1=Enabled)
0x0046: Backend Status
0x0047: Backend Error Code

// Safety Control Registers
0x0050: Emergency Stop Status (0=Normal, 1=Activated)
0x0051: Safety Zone Status (16-bit bitfield)
0x0052: Speed Limit (% × 10)
0x0053: Fault Status (16-bit bitfield)
0x0054: Safety Status (0=Safe, 1=Warning, 2=Danger)
0x0055: Fault Code
0x0056: Safety Mode
0x0057: Safety Response Time (ms)

// Configuration Registers
0x0060: Wheel Radius (mm × 10)
0x0061: Gear Ratio (× 100)
0x0062: Max Motor RPM
0x0063: Speed Sensor PPR
0x0064: Max Speed (% × 10)
0x0065: Min Speed (% × 10)
0x0066: Speed Deadband (% × 10)
0x0067: Control Update Rate (Hz)
```

---

## 🔒 **SAFETY INTEGRATION**

### **Safety Features:**
```c
// Safety monitoring và control
- Emergency Stop: Immediate motor stop khi E-Stop activated
- Speed Limits: Zone-based speed limits từ location system
- Obstacle Detection: Stop/reduce speed khi obstacle detected
- Fault Detection: Motor fault detection và reporting
- Fail-safe Operation: Safe state khi fault detected
- Safety Zones: Speed limits based on location zones
```

### **Safety Response Times:**
```c
// Safety response time requirements
- Emergency Stop Response: < 10ms
- Safety Zone Response: < 50ms
- Obstacle Response: < 100ms
- Fault Response: < 200ms
- Normal Stop Response: < 500ms
```

---

## 📡 **BACKEND INTEGRATION**

### **Backend Control Flow:**
```c
// Backend control flow
Backend → Location Module → Motor Control Module
1. Backend sets target velocity (m/s)
2. Location module provides current velocity (m/s)
3. Motor control calculates speed percentage
4. PID control adjusts PWM output
5. Motor achieves target velocity
```

### **Backend API Integration:**
```c
// Backend API cho motor control
POST /api/v1/motor/set-velocity
{
    "target_velocity": 1.5,      // Target velocity in m/s
    "max_velocity": 2.0,         // Maximum velocity limit
    "acceleration": 0.5,         // Acceleration limit m/s²
    "deceleration": 0.5,         // Deceleration limit m/s²
    "control_mode": "auto",      // Control mode
    "safety_enabled": true       // Safety control
}

GET /api/v1/motor/status
{
    "current_velocity": 1.45,    // Current velocity in m/s
    "target_velocity": 1.5,      // Target velocity in m/s
    "speed_percentage": 72.5,    // Current speed percentage
    "motor_status": "running",   // Motor status
    "safety_status": "safe"      // Safety status
}
```

### **Location Module Integration:**
```c
// Location module integration
typedef struct {
    float current_velocity_mps;  // Current velocity từ location module
    float target_velocity_mps;   // Target velocity từ backend
    float velocity_error;        // Velocity error
    float acceleration_mps2;     // Current acceleration
    uint8_t velocity_status;     // Velocity status
} location_velocity_data_t;
```

---

## 🔧 **CONTROL MODES**

### **Manual Control Mode:**
```c
// Manual control mode
- Direct speed control via joystick/keyboard
- Speed percentage control (0-100%)
- Velocity feedback display
- Safety limits enforced
- Emergency stop available
```

### **Auto Control Mode:**
```c
// Auto control mode
- Target velocity specified (m/s)
- Automatic speed percentage calculation
- PID control for smooth operation
- Safety zone compliance
- Obstacle avoidance
```

### **Backend Control Mode:**
```c
// Backend control mode
- Backend sets target velocity (m/s)
- Location module provides velocity feedback
- Automatic speed percentage calculation
- PID control for precise velocity control
- Safety monitoring và control
```

---

## 📊 **PERFORMANCE SPECIFICATIONS**

### **Speed Control Performance:**
```c
// Speed control performance
- Speed Range: 0-100% PWM output
- Speed Accuracy: ±2% speed accuracy
- Speed Response: < 50ms speed change
- Speed Stability: ±1% speed stability
- Speed Deadband: 2% minimum speed
```

### **Velocity Control Performance:**
```c
// Velocity control performance
- Velocity Range: 0-2.0 m/s
- Velocity Accuracy: ±0.1 m/s
- Velocity Response: < 100ms velocity change
- Velocity Stability: ±0.05 m/s
- Acceleration: 0-1.0 m/s²
- Deceleration: 0-1.0 m/s²
```

### **PID Control Performance:**
```c
// PID control performance
- Settling Time: < 1s for velocity changes
- Overshoot: < 5% maximum overshoot
- Steady State Error: < 1% steady state error
- Update Rate: 100Hz PID update rate
```

---

## 🔧 **IMPLEMENTATION REQUIREMENTS**

### **Hardware Implementation:**
```c
// Hardware components
- 2x DC brushed motors (12V, 100W each)
- 2x motor drivers (H-bridge, 15A)
- 2x Hall effect speed sensors
- 1x planetary gearbox (20:1 ratio)
- 2x omni-directional wheels (150mm)
- 1x microcontroller (STM32F407)
- 1x power distribution board
- 1x communication interface (RS485)
```

### **Software Implementation:**
```c
// Software components
- Motor control firmware
- PID speed control algorithms
- Velocity to speed conversion
- Backend integration
- Safety monitoring
- Communication protocol (Modbus RTU)
```

### **Testing Requirements:**
```c
// Testing requirements
- Motor performance testing
- Speed control testing
- Velocity control testing
- PID control testing
- Backend integration testing
- Safety system testing
- Environmental testing
- Reliability testing
```

---

## 📋 **TESTING PROCEDURES**

### **Functional Testing:**
```c
// Functional test procedures
1. Motor Control Testing
   - Test motor enable/disable
   - Test speed control (0-100%)
   - Test direction control
   - Test current monitoring

2. Speed Control Testing
   - Test speed accuracy
   - Test speed response time
   - Test speed stability
   - Test speed deadband

3. Velocity Control Testing
   - Test velocity accuracy (m/s)
   - Test velocity response time
   - Test velocity stability
   - Test acceleration/deceleration

4. PID Control Testing
   - Test PID parameters
   - Test settling time
   - Test overshoot
   - Test steady state error

5. Backend Integration Testing
   - Test backend velocity setting
   - Test location module integration
   - Test safety integration
   - Test communication protocol
```

### **Performance Testing:**
```c
// Performance test procedures
1. Accuracy Testing
   - Speed accuracy measurement
   - Velocity accuracy measurement
   - PID control accuracy

2. Response Time Testing
   - Motor response time
   - Speed response time
   - Velocity response time
   - Safety response time

3. Reliability Testing
   - Continuous operation test
   - Environmental test
   - Stress test
   - Life cycle test
```

---

**Changelog:**
- v2.1 (2025-01-27): Updated với DC brushed motor 12V và PID speed control
- v2.0 (2025-01-27): Added location-based control và navigation requirements
- v1.0 (2025-01-27): Initial motor control specification

**Status:** Updated với DC brushed motor 12V và PID speed control  
**Next Steps:** Implementation theo new requirements
