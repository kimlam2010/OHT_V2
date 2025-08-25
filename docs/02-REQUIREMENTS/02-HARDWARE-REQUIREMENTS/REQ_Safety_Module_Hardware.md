# Safety Module Hardware Requirements - OHT-50 (v1.0)

**Phiên bản:** 1.0  
**Ngày tạo:** 2025-01-28  
**Team:** Hardware Engineering  
**Trạng thái:** Hardware Specification - Based on RS485 Integration Requirements  

---

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Thiết kế phần cứng Safety Module với 4 cảm biến khoảng cách analog
- Tích hợp 4 cảm biến tiệm cận digital
- Cung cấp 4 relay outputs cho chức năng an toàn
- Giao tiếp qua RS485 Modbus RTU với Master Module
- Đảm bảo tuân thủ tiêu chuẩn an toàn SIL2

---

## 🔧 **PHẦN CỨNG CHÍNH**

### **1. Microcontroller:**
- **Model:** STM32F407VGT6 hoặc tương đương
- **Core:** ARM Cortex-M4 32-bit RISC
- **Clock:** 168 MHz
- **Flash:** 1MB
- **RAM:** 192KB
- **Operating Voltage:** 3.3V
- **Temperature Range:** -40°C to +85°C

### **2. RS485 Communication:**
- **Transceiver:** MAX485 hoặc SN65HVD72
- **Baud Rate:** 115200 bps (configurable)
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Termination:** 120Ω resistors (jumper selectable)
- **Bias:** 560Ω pull-up/pull-down resistors
- **Protection:** TVS diodes, ESD protection

### **3. Power Supply:**
- **Input Voltage:** 24V DC ±10%
- **Output Voltages:**
  - 3.3V @ 500mA (Logic)
  - 5V @ 200mA (Sensors)
  - 12V @ 100mA (Optional)
- **Efficiency:** >85%
- **Protection:** Over-voltage, under-voltage, over-current
- **Isolation:** 1500V isolation cho safety circuits

---

## 📡 **ANALOG INPUTS (4 KÊNH)**

### **Cảm biến khoảng cách:**
- **Type:** Ultrasonic hoặc LiDAR distance sensors
- **Range:** 0-5000mm
- **Accuracy:** ±1% hoặc ±10mm (whichever is greater)
- **Update Rate:** 10Hz minimum
- **Interface:** 4-20mA hoặc 0-5V analog
- **Resolution:** 12-bit ADC (4096 levels)
- **Input Protection:** Over-voltage, reverse polarity

### **ADC Specifications:**
- **ADC Type:** 12-bit SAR ADC
- **Channels:** 4 differential channels
- **Sample Rate:** 1000 samples/second
- **Input Range:** 0-5V
- **Accuracy:** ±1 LSB
- **Temperature Drift:** <50ppm/°C

### **Calibration:**
- **Factory Calibration:** Stored in EEPROM
- **Field Calibration:** Via Modbus registers
- **Temperature Compensation:** Built-in
- **Drift Compensation:** Auto-calibration feature

---

## 🔍 **DIGITAL INPUTS (4 KÊNH)**

### **Cảm biến tiệm cận:**
- **Type:** Inductive hoặc Capacitive proximity sensors
- **Detection Range:** 5-50mm (configurable)
- **Response Time:** <10ms
- **Output:** PNP/NPN configurable
- **Operating Voltage:** 24V DC
- **Current Rating:** 200mA max
- **Protection:** Reverse polarity, over-current

### **Digital Input Circuit:**
- **Input Voltage:** 24V DC
- **Logic Level:** 3.3V CMOS
- **Input Impedance:** 10kΩ
- **Hysteresis:** 2V typical
- **Filtering:** Hardware debounce (10ms)
- **Isolation:** Optocoupler isolation

---

## 🔌 **RELAY OUTPUTS (4 KÊNH)**

### **Relay Specifications:**
- **Type:** Electromechanical relays
- **Contact Rating:** 24V DC @ 2A
- **Contact Configuration:** SPDT (Single Pole Double Throw)
- **Coil Voltage:** 5V DC
- **Coil Current:** 50mA max
- **Switching Time:** <10ms
- **Life:** 100,000 cycles minimum
- **Protection:** Flyback diode, snubber circuit

### **Relay Driver Circuit:**
- **Driver Type:** ULN2003A hoặc tương đương
- **Output Current:** 500mA per channel
- **Input Logic:** 3.3V CMOS compatible
- **Protection:** Over-current, thermal shutdown
- **Status Feedback:** Contact status monitoring

---

## 🔒 **SAFETY FEATURES**

### **E-Stop Integration:**
- **Dual-Channel E-Stop:** 2x independent safety inputs
- **Response Time:** <10ms
- **Contact Rating:** 24V/2A
- **Safety Level:** SIL2 compliant
- **Fail-Safe:** Normally closed contacts
- **Monitoring:** Continuous contact monitoring

### **Safety Logic:**
- **Hardware Safety:** Dedicated safety microcontroller
- **Watchdog Timer:** Independent watchdog
- **Fault Detection:** Continuous self-diagnosis
- **Safe State:** Defined safe states cho all outputs
- **Emergency Stop:** Immediate relay deactivation

---

## 🔧 **MECHANICAL DESIGN**

### **Enclosure:**
- **Material:** Aluminum hoặc polycarbonate
- **Protection:** IP65 (dust and water resistant)
- **Mounting:** DIN rail mounting
- **Dimensions:** 120mm x 90mm x 60mm (approximate)
- **Weight:** <500g

### **Connectors:**
- **Power:** Phoenix Contact hoặc tương đương
- **RS485:** Screw terminal hoặc RJ45
- **Analog Inputs:** Screw terminal (4-pin)
- **Digital Inputs:** Screw terminal (4-pin)
- **Relay Outputs:** Screw terminal (8-pin)
- **E-Stop:** Screw terminal (4-pin)

### **Cable Management:**
- **Cable Glands:** IP65 rated
- **Strain Relief:** Integrated strain relief
- **Grounding:** Dedicated ground terminal
- **Shielding:** Shielded cables for analog signals

---

## 🔌 **INTERFACE SPECIFICATIONS**

### **Power Interface:**
```
Pin 1: 24V+ (Power Input)
Pin 2: 24V- (Power Return)
Pin 3: GND (Signal Ground)
Pin 4: Shield (Cable Shield)
```

### **RS485 Interface:**
```
Pin 1: A+ (RS485 Positive)
Pin 2: B- (RS485 Negative)
Pin 3: GND (Signal Ground)
Pin 4: Shield (Cable Shield)
```

### **Analog Input Interface:**
```
Channel 1: AI1+, AI1-, GND
Channel 2: AI2+, AI2-, GND
Channel 3: AI3+, AI3-, GND
Channel 4: AI4+, AI4-, GND
```

### **Digital Input Interface:**
```
Channel 1: DI1+, DI1-, GND
Channel 2: DI2+, DI2-, GND
Channel 3: DI3+, DI3-, GND
Channel 4: DI4+, DI4-, GND
```

### **Relay Output Interface:**
```
Relay 1: COM1, NO1, NC1
Relay 2: COM2, NO2, NC2
Relay 3: COM3, NO3, NC3
Relay 4: COM4, NO4, NC4
```

### **E-Stop Interface:**
```
Channel 1: ES1+, ES1-, GND
Channel 2: ES2+, ES2-, GND
```

---

## 📊 **ELECTRICAL SPECIFICATIONS**

### **Power Requirements:**
- **Input Voltage:** 24V DC ±10%
- **Input Current:** 200mA typical, 500mA max
- **Power Consumption:** 5W typical, 12W max
- **Efficiency:** >85% at rated load

### **Signal Levels:**
- **Logic High:** 3.3V ±0.3V
- **Logic Low:** 0V ±0.3V
- **Analog Input:** 0-5V, 4-20mA
- **Digital Input:** 24V DC
- **Relay Output:** 24V DC @ 2A

### **Timing Specifications:**
- **RS485 Response Time:** <50ms
- **Analog Update Rate:** 10Hz minimum
- **Digital Response Time:** <10ms
- **Relay Switching Time:** <10ms
- **E-Stop Response Time:** <10ms

---

## 🔒 **SAFETY COMPLIANCE**

### **Standards Compliance:**
- **Functional Safety:** IEC 61508 SIL2
- **Machine Safety:** ISO 13849-1, Performance Level d
- **Electrical Safety:** IEC 61010-1
- **EMC:** EN 61000-6-2, EN 61000-6-4
- **RoHS:** 2011/65/EU

### **Safety Features:**
- **Fail-Safe Design:** Safe state khi power loss
- **Redundant Monitoring:** Dual-channel safety
- **Fault Detection:** Continuous self-diagnosis
- **Emergency Stop:** Immediate response
- **Isolation:** Galvanic isolation cho safety circuits

---

## 📋 **TESTING REQUIREMENTS**

### **Electrical Testing:**
- **Insulation Resistance:** >100MΩ @ 500V DC
- **Dielectric Strength:** 1500V AC, 1 minute
- **Ground Continuity:** <0.1Ω
- **Voltage Drop:** <2% at rated current

### **Functional Testing:**
- **RS485 Communication:** Modbus RTU test
- **Analog Inputs:** Accuracy và linearity test
- **Digital Inputs:** Response time test
- **Relay Outputs:** Contact resistance test
- **E-Stop Function:** Response time test

### **Environmental Testing:**
- **Temperature:** -20°C to +50°C
- **Humidity:** 10% to 90% RH (non-condensing)
- **Vibration:** 5-500Hz, 2g RMS
- **Shock:** 10g, 11ms half-sine

---

## 📦 **PACKAGING & DOCUMENTATION**

### **Packaging:**
- **Anti-static bag:** ESD protection
- **User Manual:** Installation và operation guide
- **Test Report:** Factory test results
- **Calibration Certificate:** Analog input calibration
- **Safety Certificate:** Safety compliance certificate

### **Documentation:**
- **Hardware Manual:** Detailed hardware specifications
- **Installation Guide:** Step-by-step installation
- **Wiring Diagram:** Connection diagrams
- **Troubleshooting Guide:** Common problems và solutions
- **Maintenance Guide:** Preventive maintenance procedures

---

**Changelog:**
- ✅ v1.0 (2025-01-28): Initial Safety Module hardware requirements
- ✅ Based on REQ_RS485_Safety_Module_Integration.md
- ✅ Added detailed hardware specifications
- ✅ Added safety compliance requirements
- ✅ Added testing procedures

**Status:** Complete hardware specification  
**Next Steps:** Create GitHub issues for implementation
