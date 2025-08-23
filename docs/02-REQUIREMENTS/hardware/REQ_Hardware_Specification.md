# Hardware Specification - OHT-50 (v2.0)

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated với Dock & Location module

---

## 🎯 **MỤC TIÊU**

Định nghĩa chi tiết phần cứng cho hệ thống OHT-50 với 5 module bắt buộc, bao gồm Dock & Location module cho positioning và navigation.

---

## 🏗️ **MASTER MODULE (ORANGE PI 5B)**

### **Platform Specifications:**
- **SoC:** Rockchip RK3588 (4x Cortex-A76 + 4x Cortex-A55)
- **RAM:** 8GB LPDDR4X
- **Storage:** 32GB eMMC + MicroSD slot
- **Power:** 5V/4A via USB-C hoặc 12V/2A via DC jack
- **Operating System:** Ubuntu 22.04 LTS

### **Communication Interfaces:**
- **UART1 RS485:** Modbus RTU Master, 115200 baud, termination 120Ω
- **Ethernet:** 1Gbps RJ45, PoE support
- **WiFi:** 2.4GHz/5GHz 802.11ac, dual-band
- **USB-C:** Debug interface, firmware update
- **GPIO:** 40-pin header với 5V/3.3V logic

### **Safety & Control:**
- **E-Stop:** 2x dual-channel safety inputs
- **Relay Outputs:** 2x 24V/2A relay outputs
- **Status LEDs:** 5x RGB LEDs (Power, System, Communication, Network, Error)
- **Emergency Brake:** 24V control output

---

## 🔋 **POWER MODULE (ID: 0x02) - BẮT BUỘC**

### **Battery System:**
- **Chemistry:** LiFePO4 (Lithium Iron Phosphate)
- **Capacity:** 50Ah @ 24V nominal
- **Voltage Range:** 20V - 28.8V
- **Cycle Life:** >2000 cycles @ 80% DoD
- **Charging Current:** 10A max (0.2C)
- **Discharge Current:** 50A continuous, 100A peak (2C)

### **Power Management:**
- **BMS (Battery Management System):**
  - Cell balancing
  - Over-voltage protection
  - Under-voltage protection
  - Over-current protection
  - Temperature monitoring
  - State of Charge (SoC) calculation

### **Power Distribution:**
- **24V Main Bus:** Motor drives, actuators
- **12V Secondary:** Sensors, communication modules
- **5V Logic:** Microcontrollers, digital circuits
- **3.3V Digital:** Low-power sensors, communication

### **Charging System:**
- **AC/DC Converter:** 220V AC → 24V DC, 15A
- **MPPT Charger:** Solar panel support (optional)
- **UPS Function:** Battery backup cho critical systems
- **Charging Interface:** Magnetic connector, IP67 rated

### **Monitoring & Protection:**
- **Voltage Monitoring:** ±0.1% accuracy
- **Current Monitoring:** ±1% accuracy
- **Temperature Monitoring:** -40°C to +85°C
- **Fault Detection:** Short circuit, over-temperature, cell imbalance

---

## 🔒 **SAFETY MODULE (ID: 0x03) - BẮT BUỘC**

### **E-Stop System:**
- **Dual-Channel E-Stop:** 2x independent safety circuits
- **Response Time:** < 10ms
- **Contact Rating:** 24V/2A
- **Safety Level:** SIL2 compliant
- **Fail-Safe:** Normally closed contacts

### **Light Curtain:**
- **Type:** Safety light curtain, Type 4
- **Range:** 0.5m - 3m
- **Resolution:** 14mm beam spacing
- **Response Time:** < 20ms
- **Protection Height:** 300mm
- **Operating Voltage:** 24V DC

### **Safety Mats:**
- **Quantity:** 4x pressure-sensitive safety mats
- **Size:** 600mm x 400mm each
- **Activation Force:** 5kg minimum
- **Response Time:** < 50ms
- **Operating Voltage:** 24V DC
- **IP Rating:** IP65

### **Door Sensors:**
- **Type:** Magnetic reed switches
- **Quantity:** 2x door sensors
- **Contact Rating:** 24V/1A
- **Operating Temperature:** -40°C to +85°C
- **IP Rating:** IP67

### **Emergency Brake:**
- **Type:** Electromagnetic brake
- **Voltage:** 24V DC
- **Holding Torque:** 50Nm
- **Response Time:** < 100ms
- **Fail-Safe:** Spring-applied, electrically released

---

## 🚗 **TRAVEL MOTOR MODULE (ID: 0x04) - BẮT BUỘC**

### **Motor Specifications:**
- **Type:** 2x DC brushed motors
- **Power Rating:** 500W each (1000W total)
- **Voltage:** 24V DC
- **Speed Range:** 0-3000 RPM
- **Torque:** 2.4Nm continuous, 7.2Nm peak
- **Efficiency:** >85% at rated load

### **Motor Drivers:**
- **Type:** H-bridge motor drivers
- **Current Rating:** 30A continuous, 60A peak
- **PWM Frequency:** 20kHz
- **Control Mode:** Current control với PID
- **Protection:** Over-current, over-temperature, short-circuit

### **Encoders:**
- **Type:** Quadrature incremental encoders
- **Resolution:** 2000 PPR (8000 counts/revolution)
- **Accuracy:** ±0.1°
- **Operating Temperature:** -40°C to +85°C
- **IP Rating:** IP65

### **Gearbox:**
- **Type:** Planetary gearbox
- **Ratio:** 12:1
- **Efficiency:** >90%
- **Backlash:** < 1°
- **Lubrication:** Grease, maintenance-free

### **Wheels:**
- **Type:** Omni-directional wheels
- **Diameter:** 200mm
- **Material:** Polyurethane tread
- **Load Capacity:** 100kg per wheel
- **Grip:** High traction, low noise

---

## 🧭 **DOCK & LOCATION MODULE (ID: 0x05) - BẮT BUỘC**

### **IMU (Inertial Measurement Unit):**
- **Model:** MPU6050 hoặc tương đương
- **Accelerometer:**
  - Range: ±2g, ±4g, ±8g, ±16g
  - Resolution: 16-bit
  - Accuracy: ±0.1mg
- **Gyroscope:**
  - Range: ±250°/s, ±500°/s, ±1000°/s, ±2000°/s
  - Resolution: 16-bit
  - Accuracy: ±0.1°/s
- **Operating Temperature:** -40°C to +85°C
- **Update Rate:** 100Hz

### **Magnetic Sensors:**
- **Type:** 2x Hall effect sensors
- **Range:** ±100mT
- **Resolution:** 16-bit
- **Accuracy:** ±0.1mT
- **Response Time:** < 1ms
- **Operating Temperature:** -40°C to +125°C

### **RFID Reader:**
- **Frequency:** 13.56MHz (HF) hoặc 860-960MHz (UHF)
- **Read Range:** 10-50cm
- **Read Rate:** 100 tags/second
- **Memory:** 64KB
- **Interface:** SPI/UART
- **Power:** 3.3V, 100mA

### **Dock Alignment Sensors:**
- **Type:** 4x infrared proximity sensors
- **Range:** 5-100mm
- **Accuracy:** ±1mm
- **Response Time:** < 10ms
- **Operating Temperature:** -20°C to +70°C

### **Charging Interface:**
- **Type:** Magnetic charging connector
- **Voltage:** 24V DC
- **Current:** 10A max
- **Contact Rating:** 100,000 cycles
- **IP Rating:** IP67
- **Alignment Tolerance:** ±5mm

### **GPS/GNSS (Optional):**
- **Type:** Multi-constellation GNSS receiver
- **Constellations:** GPS, GLONASS, Galileo, BeiDou
- **Accuracy:** ±2.5m CEP
- **Update Rate:** 10Hz
- **Time to First Fix:** < 30s cold start
- **Operating Temperature:** -40°C to +85°C

---

## 🔧 **LIFTER MOTOR MODULE (ID: 0x06) - TÙY CHỌN**

### **Linear Actuator:**
- **Type:** Ball screw linear actuator
- **Stroke:** 500mm
- **Load Capacity:** 50kg
- **Speed:** 50mm/s
- **Position Accuracy:** ±2mm
- **Power:** 200W, 24V DC

### **Load Cell:**
- **Type:** Strain gauge load cell
- **Capacity:** 100kg
- **Accuracy:** ±0.1% FS
- **Output:** 2mV/V
- **Operating Temperature:** -10°C to +50°C

### **Position Sensor:**
- **Type:** Linear potentiometer
- **Range:** 0-500mm
- **Resolution:** 0.1mm
- **Accuracy:** ±0.5mm
- **Life:** 1,000,000 cycles

---

## 🚪 **CARGO DOOR MODULE (ID: 0x07) - TÙY CHỌN**

### **Door Actuator:**
- **Type:** Electric linear actuator
- **Stroke:** 300mm
- **Force:** 200N
- **Speed:** 30mm/s
- **Power:** 100W, 24V DC

### **Door Position Sensor:**
- **Type:** Rotary potentiometer
- **Range:** 0-90°
- **Resolution:** 0.1°
- **Accuracy:** ±1°

### **Cargo Presence Sensor:**
- **Type:** Capacitive proximity sensor
- **Range:** 0-50mm
- **Detection:** Metal và non-metal objects
- **Response Time:** < 10ms

---

## 🔌 **MODULE INTERFACES**

### **RS485 Bus:**
- **Standard:** Modbus RTU
- **Baud Rate:** 115200 (configurable)
- **Data Bits:** 8
- **Parity:** None
- **Stop Bits:** 1
- **Termination:** 120Ω resistors at both ends
- **Bias:** 560Ω pull-up/pull-down resistors
- **Cable:** Shielded twisted pair, 24AWG
- **Max Distance:** 1200m
- **Max Nodes:** 32

### **Power Distribution:**
- **Main Bus:** 24V DC, 50A max
- **Secondary Bus:** 12V DC, 10A max
- **Logic Bus:** 5V DC, 5A max
- **Digital Bus:** 3.3V DC, 2A max

### **Safety Network:**
- **Protocol:** Safety over Modbus
- **Response Time:** < 100ms
- **Redundancy:** Dual-channel
- **Diagnostics:** Continuous monitoring

---

## 📊 **ENVIRONMENTAL SPECIFICATIONS**

### **Operating Conditions:**
- **Temperature:** -20°C to +50°C
- **Humidity:** 10% to 90% RH (non-condensing)
- **Altitude:** 0-2000m above sea level
- **Vibration:** 5-500Hz, 2g RMS
- **Shock:** 10g, 11ms half-sine

### **Protection Ratings:**
- **Master Module:** IP20 (indoor use)
- **Power Module:** IP65
- **Safety Module:** IP67
- **Travel Motor Module:** IP65
- **Dock & Location Module:** IP67
- **Lifter Module:** IP65
- **Cargo Door Module:** IP65

### **EMC Compliance:**
- **Emissions:** EN 61000-6-3
- **Immunity:** EN 61000-6-2
- **Industrial:** EN 61000-6-4 (emissions), EN 61000-6-2 (immunity)

---

## 🔧 **MECHANICAL SPECIFICATIONS**

### **Overall Dimensions:**
- **Length:** 800mm
- **Width:** 600mm
- **Height:** 400mm (without lifter)
- **Weight:** 50kg (without cargo)

### **Module Mounting:**
- **Master Module:** DIN rail mounting
- **Power Module:** Rack mounting, 2U height
- **Safety Module:** Panel mounting
- **Travel Motor Module:** Chassis mounting
- **Dock & Location Module:** Chassis mounting
- **Lifter Module:** Chassis mounting
- **Cargo Door Module:** Chassis mounting

### **Cable Management:**
- **Cable Trays:** Integrated cable management
- **Strain Relief:** Cable glands, IP67 rated
- **Grounding:** Dedicated ground bus
- **Shielding:** Shielded cables for sensitive signals

---

## 🔒 **SAFETY COMPLIANCE**

### **Standards Compliance:**
- **Functional Safety:** IEC 61508 SIL2
- **Machine Safety:** ISO 13849-1, Performance Level d
- **Electrical Safety:** IEC 61010-1
- **EMC:** EN 61000 series
- **RoHS:** 2011/65/EU

### **Safety Features:**
- **Emergency Stop:** Dual-channel, fail-safe
- **Safety Monitoring:** Continuous diagnostics
- **Fault Detection:** Automatic fault detection và reporting
- **Safe State:** Defined safe states cho all modules

---

## 📋 **TESTING REQUIREMENTS**

### **Electrical Testing:**
- **Insulation Resistance:** >100MΩ @ 500V DC
- **Dielectric Strength:** 1500V AC, 1 minute
- **Ground Continuity:** < 0.1Ω
- **Voltage Drop:** < 2% at rated current

### **Functional Testing:**
- **Module Communication:** Modbus RTU communication test
- **Safety Functions:** E-Stop, light curtain, safety mat test
- **Motor Control:** Speed, position, current control test
- **Location System:** IMU, magnetic sensor, RFID test

### **Environmental Testing:**
- **Temperature Cycling:** -20°C to +50°C, 10 cycles
- **Humidity:** 40°C, 93% RH, 96 hours
- **Vibration:** 5-500Hz, 2g RMS, 2 hours each axis
- **Shock:** 10g, 11ms, 3 shocks each direction

---

**Changelog:**
- v2.0 (2025-01-27): Added Dock & Location module specifications
- v1.0 (2025-01-27): Initial hardware specification

**Status:** Updated với Dock & Location module  
**Next Steps:** Hardware Implementation Plan update
