# Hardware Implementation Plan - OHT-50 (v2.0)

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated với Dock & Location module

---

## 🎯 **MỤC TIÊU**

Lập kế hoạch triển khai phần cứng cho hệ thống OHT-50 với 5 module bắt buộc, bao gồm Dock & Location module cho positioning và navigation.

---

## 📋 **PHASE 1: MASTER MODULE SETUP**

### **Week 1-2: Platform Setup**
```
Day 1-3: Orange Pi 5B Setup
- Install Ubuntu 22.04 LTS
- Configure network interfaces (Ethernet + WiFi)
- Install development tools và dependencies
- Configure UART1 cho RS485 communication

Day 4-7: GPIO Configuration
- Configure GPIO pins cho E-Stop inputs
- Configure GPIO pins cho relay outputs
- Configure GPIO pins cho status LEDs
- Test GPIO functionality

Day 8-10: Communication Setup
- Configure UART1 RS485 với termination
- Test Modbus RTU communication
- Configure Ethernet và WiFi failover
- Test network connectivity

Day 11-14: Safety System Integration
- Install E-Stop buttons và wiring
- Configure dual-channel safety monitoring
- Test emergency stop functionality
- Configure emergency brake control
```

### **Deliverables:**
- ✅ Orange Pi 5B với Ubuntu 22.04 LTS
- ✅ UART1 RS485 communication working
- ✅ GPIO control cho E-Stop và relays
- ✅ Network connectivity (Ethernet + WiFi)
- ✅ Basic safety system functionality

---

## 📋 **PHASE 2: CORE MODULES IMPLEMENTATION**

### **Week 3-4: Power Module**
```
Day 15-17: Battery System
- Install LiFePO4 battery pack (50Ah @ 24V)
- Configure BMS với cell balancing
- Test charging/discharging cycles
- Validate safety protections

Day 18-21: Power Distribution
- Install power distribution board
- Configure 24V, 12V, 5V, 3.3V rails
- Test power monitoring và protection
- Validate UPS functionality

Day 22-28: Charging System
- Install AC/DC converter (220V → 24V)
- Configure MPPT charger (optional)
- Install magnetic charging connector
- Test charging interface
```

### **Week 5-6: Safety Module**
```
Day 29-31: E-Stop System
- Install dual-channel E-Stop buttons
- Configure safety monitoring circuits
- Test response time (< 10ms)
- Validate SIL2 compliance

Day 32-35: Safety Sensors
- Install safety light curtain
- Install 4x safety mats
- Install 2x door sensors
- Test all safety sensors

Day 36-42: Emergency Brake
- Install electromagnetic brake
- Configure brake control circuit
- Test brake response time
- Validate fail-safe operation
```

### **Week 7-8: Travel Motor Module**
```
Day 43-45: Motor Installation
- Install 2x DC motors (500W each)
- Configure motor drivers (H-bridge)
- Test motor control functionality
- Validate power ratings

Day 46-49: Encoder Integration
- Install quadrature encoders (2000 PPR)
- Configure encoder interface
- Test position feedback
- Validate accuracy (±0.1°)

Day 50-56: Gearbox & Wheels
- Install planetary gearbox (12:1 ratio)
- Install omni-directional wheels
- Test differential drive
- Validate load capacity
```

---

## 📋 **PHASE 3: DOCK & LOCATION MODULE IMPLEMENTATION**

### **Week 9-10: IMU & Sensors**
```
Day 57-59: IMU Installation
- Install MPU6050 IMU module
- Configure I2C/SPI interface
- Test accelerometer và gyroscope
- Validate sensor accuracy

Day 60-63: Magnetic Sensors
- Install 2x Hall effect sensors
- Configure analog interface
- Test magnetic field detection
- Validate position tracking

Day 64-70: Sensor Calibration
- Calibrate IMU sensors
- Calibrate magnetic sensors
- Test sensor fusion algorithms
- Validate positioning accuracy (±10mm)
```

### **Week 11-12: RFID & Dock Sensors**
```
Day 71-73: RFID Reader
- Install RFID reader module
- Configure UART/SPI interface
- Test tag reading functionality
- Validate read range (10-50cm)

Day 74-77: Dock Alignment Sensors
- Install 4x infrared proximity sensors
- Configure sensor array
- Test alignment detection
- Validate accuracy (±1mm)

Day 78-84: Charging Interface
- Install magnetic charging connector
- Configure charging circuit
- Test charging functionality
- Validate alignment tolerance (±5mm)
```

### **Week 13-14: GPS & Integration**
```
Day 85-87: GPS Module (Optional)
- Install multi-constellation GNSS receiver
- Configure UART interface
- Test GPS functionality
- Validate accuracy (±2.5m)

Day 88-91: Module Integration
- Integrate all sensors vào single module
- Configure power distribution
- Test communication interface
- Validate overall functionality

Day 92-98: Testing & Validation
- Test positioning accuracy
- Test navigation functionality
- Test docking procedures
- Validate safety integration
```

---

## 📋 **PHASE 4: OPTIONAL MODULES**

### **Week 15-16: Lifter Motor Module**
```
Day 99-101: Linear Actuator
- Install ball screw linear actuator
- Configure motor driver
- Test stroke và load capacity
- Validate position accuracy

Day 102-105: Load Cell
- Install strain gauge load cell
- Configure load monitoring
- Test weight measurement
- Validate accuracy (±0.1% FS)

Day 106-112: Integration
- Integrate actuator và load cell
- Configure safety limits
- Test complete lifter system
- Validate safety functionality
```

### **Week 17-18: Cargo Door Module**
```
Day 113-115: Door Actuator
- Install electric linear actuator
- Configure door control
- Test door movement
- Validate position feedback

Day 116-119: Sensors
- Install door position sensor
- Install cargo presence sensor
- Test sensor functionality
- Validate detection accuracy

Day 120-126: Integration
- Integrate door system
- Configure safety interlocks
- Test complete door system
- Validate safety functionality
```

---

## 📋 **PHASE 5: SYSTEM INTEGRATION**

### **Week 19-20: Mechanical Integration**
```
Day 127-129: Chassis Assembly
- Assemble robot chassis
- Install all modules
- Configure cable management
- Test mechanical fit

Day 130-133: Power Integration
- Connect power distribution
- Test all power rails
- Validate power monitoring
- Test UPS functionality

Day 134-140: Safety Integration
- Connect all safety sensors
- Test safety monitoring
- Validate emergency procedures
- Test fail-safe operation
```

### **Week 21-22: Communication Integration**
```
Day 141-143: RS485 Bus
- Connect all modules to RS485 bus
- Configure Modbus RTU addresses
- Test communication
- Validate auto-discovery

Day 144-147: Network Integration
- Configure Ethernet connectivity
- Configure WiFi connectivity
- Test network failover
- Validate communication reliability

Day 148-154: System Testing
- Test complete system functionality
- Validate all safety features
- Test navigation system
- Validate performance requirements
```

---

## 📋 **PHASE 6: TESTING & VALIDATION**

### **Week 23-24: Functional Testing**
```
Day 155-157: Module Testing
- Test each module individually
- Validate module specifications
- Test communication protocols
- Validate safety features

Day 158-161: Integration Testing
- Test module interactions
- Validate system coordination
- Test safety integration
- Validate performance

Day 162-168: System Testing
- Test complete system functionality
- Validate all requirements
- Test edge cases
- Validate reliability
```

### **Week 25-26: Performance Testing**
```
Day 169-171: Performance Testing
- Test positioning accuracy (±10mm)
- Test navigation speed (0.5-2.0 m/s)
- Test docking accuracy (±5mm)
- Validate response times

Day 172-175: Safety Testing
- Test E-Stop response (< 10ms)
- Test safety monitoring
- Test emergency procedures
- Validate safety compliance

Day 176-182: Environmental Testing
- Test temperature range (-20°C to +50°C)
- Test humidity resistance
- Test vibration resistance
- Validate environmental compliance
```

---

## 📊 **TIMELINE TỔNG THỂ**

### **Phase Breakdown:**
```
Phase 1 (Week 1-2):   Master Module Setup
Phase 2 (Week 3-8):   Core Modules Implementation
Phase 3 (Week 9-14):  Dock & Location Module
Phase 4 (Week 15-18): Optional Modules
Phase 5 (Week 19-22): System Integration
Phase 6 (Week 23-26): Testing & Validation
```

### **Critical Path:**
```
Week 1-2:   Master Module (Critical)
Week 3-4:   Power Module (Critical)
Week 5-6:   Safety Module (Critical)
Week 7-8:   Travel Motor Module (Critical)
Week 9-14:  Dock & Location Module (Critical)
Week 19-22: System Integration (Critical)
Week 23-26: Testing & Validation (Critical)
```

---

## 🔧 **RESOURCE REQUIREMENTS**

### **Hardware Components:**
- **Master Module:** Orange Pi 5B, accessories
- **Power Module:** LiFePO4 battery, BMS, chargers
- **Safety Module:** E-Stop buttons, sensors, brake
- **Travel Motor Module:** DC motors, drivers, encoders
- **Dock & Location Module:** IMU, sensors, RFID, GPS
- **Optional Modules:** Actuators, sensors, controllers

### **Tools & Equipment:**
- **Development Tools:** Oscilloscope, multimeter, power supply
- **Testing Equipment:** Load tester, environmental chamber
- **Software Tools:** Development IDE, testing frameworks
- **Documentation:** Technical manuals, datasheets

### **Personnel:**
- **Hardware Engineer:** 1 person, full-time
- **Embedded Engineer:** 1 person, full-time
- **Test Engineer:** 1 person, part-time
- **Project Manager:** 1 person, part-time

---

## 🚨 **RISK MANAGEMENT**

### **Technical Risks:**
- **Component Availability:** Supply chain issues
- **Integration Complexity:** Module interaction issues
- **Performance Requirements:** Accuracy và speed requirements
- **Safety Compliance:** SIL2 compliance challenges

### **Mitigation Strategies:**
- **Early Procurement:** Order components early
- **Modular Design:** Independent module development
- **Extensive Testing:** Comprehensive testing program
- **Expert Consultation:** Safety expert review

---

## 📋 **DELIVERABLES**

### **Hardware Deliverables:**
- ✅ Complete robot system với 5 core modules
- ✅ All optional modules (if required)
- ✅ Complete documentation
- ✅ Test reports và validation

### **Documentation Deliverables:**
- ✅ Hardware specifications
- ✅ Assembly instructions
- ✅ Test procedures
- ✅ Maintenance manuals

---

**Changelog:**
- v2.0 (2025-01-27): Added Dock & Location module implementation
- v1.0 (2025-01-27): Initial implementation plan

**Status:** Updated với Dock & Location module  
**Next Steps:** Motor Control Module update
