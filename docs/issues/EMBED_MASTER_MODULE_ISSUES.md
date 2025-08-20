# EMBED Team Issues - Master Module Hardware Implementation

## 📋 **Tổng quan**
Issues cho EMBED team để implement Master Module HARDWARE và low-level setup.

**Ngày tạo:** 2025-01-27  
**Priority:** High  
**Team:** EMBED  
**Sprint:** Master Module v3.0
**Phạm vi:** Hardware design, PCB, wiring, circuits, device tree, basic hardware testing

---

## 🎯 **Issue #EMBED-001: Master Module Hardware Design & PCB**

### **Mô tả:**
Design và implement hardware cho Master Module với các interface mới.

### **Requirements (HARDWARE ONLY):**
- [ ] PCB design cho Master Module
- [ ] USB Debug circuit (USB 2.0 Type-C connector)
- [ ] RS485 transceiver circuit (MAX485/SN75176)
- [ ] Ethernet PHY circuit (LAN8720A/RTL8201F)
- [ ] WiFi module circuit (ESP32-WROOM/ESP8266)
- [ ] LED driver circuits (5 LEDs với current limiting)
- [ ] E-Stop safety circuit (dual-channel, 24V)
- [ ] Relay driver circuit (solid-state relay, 24V/2A)

### **Tasks (HARDWARE FOCUS):**
1. **PCB Design:**
   - [ ] Schematic design cho tất cả circuits
   - [ ] PCB layout design
   - [ ] Component placement
   - [ ] Routing và signal integrity
   - [ ] Power distribution design
   - [ ] Ground plane design

2. **Circuit Implementation:**
   - [ ] USB debug circuit với protection
   - [ ] RS485 transceiver với termination
   - [ ] Ethernet PHY với magnetics
   - [ ] WiFi module với antenna
   - [ ] LED circuits với current limiting
   - [ ] E-Stop circuit với safety features
   - [ ] Relay circuit với protection

3. **Hardware Assembly:**
   - [ ] PCB fabrication
   - [ ] Component soldering
   - [ ] Hardware assembly
   - [ ] Connector installation
   - [ ] Enclosure design

### **Acceptance Criteria:**
- [ ] PCB design completed
- [ ] All circuits implemented
- [ ] Hardware assembled
- [ ] Basic connectivity tested
- [ ] Power distribution working
- [ ] Documentation updated

### **Estimate:** 2 weeks
### **Priority:** High

---

## 🎯 **Issue #EMBED-002: LED Hardware Implementation**

### **Mô tả:**
Implement LED hardware circuits cho 5 status LEDs.

### **Requirements (HARDWARE ONLY):**
- [ ] Power LED circuit (Green, GPIO 54 - GPIO1_D6)
- [ ] System LED circuit (Blue, GPIO 35 - GPIO1_A3)
- [ ] Communication LED circuit (Yellow, GPIO 28 - GPIO0_D4)
- [ ] Network LED circuit (Green, GPIO 29 - GPIO0_D5)
- [ ] Error LED circuit (Red, GPIO 58 - GPIO1_D2)
- [ ] Current limiting resistors
- [ ] LED driver circuits (nếu cần)

### **Tasks (HARDWARE FOCUS):**
1. **Circuit Design:**
   - [ ] LED circuit schematics
   - [ ] Current limiting calculations
   - [ ] Resistor selection
   - [ ] Power supply design
   - [ ] GPIO connection design

2. **Hardware Implementation:**
   - [ ] LED mounting design
   - [ ] PCB footprint design
   - [ ] Wiring design
   - [ ] Connector selection
   - [ ] Enclosure integration

3. **Hardware Testing:**
   - [ ] LED functionality test
   - [ ] Current measurement
   - [ ] Voltage measurement
   - [ ] GPIO connectivity test
   - [ ] Visual inspection

### **Acceptance Criteria:**
- [ ] All 5 LED circuits implemented
- [ ] LEDs light up correctly
- [ ] Current limiting working
- [ ] GPIO connections verified
- [ ] Hardware documentation complete

### **Estimate:** 1 week
### **Priority:** High

---

## 🎯 **Issue #EMBED-003: E-Stop Hardware Safety Circuit**

### **Mô tả:**
Implement E-Stop hardware safety circuit theo chuẩn SIL2.

### **Requirements (HARDWARE ONLY):**
- [ ] Single-channel E-Stop input circuit
- [ ] E-Stop input: GPIO 59 (GPIO1_D3)
- [ ] 24V DC safety circuit
- [ ] Safety isolation circuits
- [ ] Fault detection circuits
- [ ] E-Stop button mounting

### **Tasks (HARDWARE FOCUS):**
1. **Safety Circuit Design:**
   - [ ] Dual-channel input circuit
   - [ ] 24V safety circuit design
   - [ ] Isolation circuit design
   - [ ] Fault detection circuit
   - [ ] Safety compliance design

2. **Hardware Implementation:**
   - [ ] E-Stop button selection
   - [ ] Safety relay selection
   - [ ] Circuit board design
   - [ ] Wiring design
   - [ ] Enclosure mounting

3. **Safety Testing:**
   - [ ] Dual-channel functionality test
   - [ ] Safety circuit test
   - [ ] Fault detection test
   - [ ] Response time measurement
   - [ ] Safety validation

### **Acceptance Criteria:**
- [ ] Dual-channel E-Stop circuit working
- [ ] Safety isolation working
- [ ] Fault detection working
- [ ] Response time < 100ms
- [ ] Safety validation passed
- [ ] Hardware documentation complete

### **Estimate:** 1.5 weeks
### **Priority:** Critical

---

## 🎯 **Issue #EMBED-004: Relay Hardware Circuit**

### **Mô tả:**
Implement relay hardware circuit cho external device control.

### **Requirements (HARDWARE ONLY):**
- [ ] Solid-state relay circuit
- [ ] Relay 1: GPIO 131 (GPIO4_A3) control
- [ ] Relay 2: GPIO 132 (GPIO4_A4) control
- [ ] 24V DC output circuit
- [ ] 2A current capacity
- [ ] Overcurrent protection
- [ ] Output connector

### **Tasks (HARDWARE FOCUS):**
1. **Relay Circuit Design:**
   - [ ] Solid-state relay selection
   - [ ] Driver circuit design
   - [ ] Power supply design
   - [ ] Protection circuit design
   - [ ] Output connector design

2. **Hardware Implementation:**
   - [ ] Relay mounting design
   - [ ] Heat sink design (nếu cần)
   - [ ] PCB footprint design
   - [ ] Wiring design
   - [ ] Connector installation

3. **Hardware Testing:**
   - [ ] Relay functionality test
   - [ ] Current capacity test
   - [ ] Protection circuit test
   - [ ] Temperature measurement
   - [ ] Electrical safety test

### **Acceptance Criteria:**
- [ ] Relay circuit working
- [ ] 24V DC output available
- [ ] 2A current capacity verified
- [ ] Protection circuits working
- [ ] Hardware documentation complete

### **Estimate:** 1 week
### **Priority:** Medium

---

## 🎯 **Issue #EMBED-005: Network Hardware Implementation**

### **Mô tả:**
Implement Ethernet và WiFi hardware circuits.

### **Requirements (HARDWARE ONLY):**
- [ ] Ethernet PHY circuit (10/100/1000 Mbps)
- [ ] WiFi module circuit (5G/2.4G dual-band)
- [ ] Network connectors (RJ45, antenna)
- [ ] Power supply cho network modules
- [ ] Signal integrity design

### **Tasks (HARDWARE FOCUS):**
1. **Ethernet Circuit Design:**
   - [ ] Ethernet PHY selection
   - [ ] Magnetics circuit design
   - [ ] RJ45 connector design
   - [ ] Signal routing design
   - [ ] Power supply design

2. **WiFi Circuit Design:**
   - [ ] WiFi module selection
   - [ ] Antenna design
   - [ ] Power supply design
   - [ ] Signal routing design
   - [ ] Enclosure integration

3. **Hardware Testing:**
   - [ ] Ethernet connectivity test
   - [ ] WiFi connectivity test
   - [ ] Signal integrity test
   - [ ] Power consumption test
   - [ ] Environmental test

### **Acceptance Criteria:**
- [ ] Ethernet circuit working
- [ ] WiFi circuit working
- [ ] Network connectivity verified
- [ ] Signal integrity good
- [ ] Hardware documentation complete

### **Estimate:** 1.5 weeks
### **Priority:** High

---

## 📊 **Sprint Planning**

### **Sprint 1 (Week 1-2):**
- [ ] Issue #EMBED-001: Hardware design & PCB
- [ ] Issue #EMBED-002: LED hardware implementation

### **Sprint 2 (Week 3-4):**
- [ ] Issue #EMBED-003: E-Stop hardware safety circuit
- [ ] Issue #EMBED-004: Relay hardware circuit

### **Sprint 3 (Week 5-6):**
- [ ] Issue #EMBED-005: Network hardware implementation
- [ ] Integration testing

### **Total Estimate:** 6 weeks
### **Team Size:** 2-3 engineers

---

## 🔧 **Resources Required**

### **Hardware:**
- PCB design software (KiCad/Eagle)
- Soldering equipment
- Multimeter, oscilloscope
- Power supplies
- Test equipment

### **Components:**
- Orange Pi 5B development board
- RS485 transceiver modules
- LED indicators
- E-Stop buttons
- Solid-state relays
- Ethernet PHY chips
- WiFi modules

### **Documentation:**
- Hardware schematics
- PCB layout files
- Bill of materials
- Assembly instructions
- Test procedures

---

## 📋 **Definition of Done**

### **Hardware:**
- [ ] PCB design completed
- [ ] Prototype assembled
- [ ] Hardware tests passed
- [ ] Safety validation completed

### **Documentation:**
- [ ] Schematics updated
- [ ] PCB layout files ready
- [ ] BOM completed
- [ ] Assembly instructions written

### **Quality:**
- [ ] Hardware review completed
- [ ] Safety validation passed
- [ ] Performance requirements met
- [ ] Reliability requirements met
