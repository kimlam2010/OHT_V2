# Master Module Hardware Setup Plan - EMBED Team

## 📋 **Tổng quan**
Kế hoạch implement hardware cho OHT-50 Master Module theo specification mới.

**Date:** 2025-01-27  
**Team:** EMBED  
**Issue:** #EMBED-001  
**Priority:** HIGH  
**Timeline:** 2 weeks

---

## 🎯 **Hardware Requirements**

### **1. Communication Interfaces**

#### **USB Debug (1x)**
- **Type:** USB 2.0 Type-C
- **Function:** Debug console, firmware update, configuration
- **Speed:** 480 Mbps
- **Power:** 5V, 500mA
- **Device:** `/dev/ttyACM0` (USB CDC)
- **Status:** ✅ Available on Orange Pi 5B

#### **RS485 (1x)**
- **Device:** `/dev/ttyS1` → `/dev/ttyOHT485`
- **Baudrate:** 115200
- **TX:** Pin 46 (GPIO1_D1)
- **RX:** Pin 47 (GPIO1_D0)
- **Protocol:** Modbus RTU
- **Termination:** 120Ω resistors
- **Status:** ✅ Already implemented

#### **LAN (1x)**
- **Type:** Ethernet 10/100/1000 Mbps
- **Interface:** eth0
- **Function:** Center connection, web interface, OTA update
- **Protocol:** HTTP/HTTPS, WebSocket, MQTT
- **Status:** ✅ Available on Orange Pi 5B

#### **Wireless 5G/2.4G**
- **Type:** WiFi 6 (802.11ax), dual-band
- **Interface:** wlan0
- **Function:** Backup connection, mobile app, remote access
- **Speed:** Up to 1.2 Gbps
- **Security:** WPA3 encryption
- **Status:** ✅ Available on Orange Pi 5B

### **2. Status LEDs (5x)**

#### **LED Configuration**
- **Power LED:** GPIO1_A0 (Pin 32) - Green (24V status)
- **System LED:** GPIO1_A1 (Pin 33) - Blue (system status)
- **Comm LED:** GPIO1_A2 (Pin 34) - Yellow (RS485 traffic)
- **Network LED:** GPIO1_A3 (Pin 35) - Green (LAN/WiFi status)
- **Error LED:** GPIO1_A4 (Pin 36) - Red (error status)

#### **LED Hardware Requirements**
- **Type:** 3mm LED indicators
- **Current:** 20mA per LED
- **Voltage:** 3.3V (GPIO level)
- **Current Limiting:** 150Ω resistors
- **Status:** 🔄 Need to implement

### **3. Emergency Stop (E-Stop)**

#### **Hardware E-Stop**
- **Type:** Dual-channel safety input
- **GPIO:** GPIO1_B0 (Pin 48), GPIO1_B1 (Pin 49)
- **Voltage:** 24V DC safety circuit
- **Safety Level:** SIL2
- **Response Time:** < 100ms
- **Status:** 🔄 Need to implement

#### **E-Stop Hardware Requirements**
- **E-Stop Button:** Industrial safety button
- **Voltage Divider:** 24V → 3.3V for GPIO
- **Isolation:** Optocoupler for safety
- **Dual Channel:** Redundant safety circuit

### **4. Relay Output (1x)**

#### **Relay Specifications**
- **GPIO:** GPIO1_D3 (Pin 51)
- **Type:** Solid-state relay
- **Voltage:** 24V DC
- **Current:** 2A
- **Function:** External device control, safety circuit
- **Status:** 🔄 Need to implement

#### **Relay Hardware Requirements**
- **SSR Module:** 24V DC, 2A solid-state relay
- **Heat Sink:** For thermal management
- **Protection:** Overcurrent protection circuit
- **Indication:** LED status indicator

---

## 🔧 **Implementation Plan**

### **Phase 1: Hardware Setup (Week 1)**

#### **Day 1-2: PCB Design Planning**
- [ ] Review Orange Pi 5B pinout
- [ ] Design LED driver circuits
- [ ] Design E-Stop safety circuits
- [ ] Design relay driver circuit
- [ ] Create component list

#### **Day 3-4: Component Procurement**
- [ ] Order LED indicators và resistors
- [ ] Order E-Stop button và safety components
- [ ] Order solid-state relay module
- [ ] Order connectors và wiring

#### **Day 5-7: Prototype Assembly**
- [ ] Assemble LED circuits trên breadboard
- [ ] Assemble E-Stop safety circuit
- [ ] Assemble relay driver circuit
- [ ] Test individual circuits

### **Phase 2: Software Implementation (Week 2)**

#### **Day 8-10: HAL Driver Development**
- [ ] Implement LED HAL driver
- [ ] Implement E-Stop HAL driver
- [ ] Implement relay HAL driver
- [ ] Update existing RS485 HAL

#### **Day 11-12: Device Tree Configuration**
- [ ] Update UART1 overlay
- [ ] Configure GPIO pins
- [ ] Configure network interfaces
- [ ] Test device tree overlays

#### **Day 13-14: Integration Testing**
- [ ] Test all hardware interfaces
- [ ] Test safety systems
- [ ] Test communication protocols
- [ ] Validate performance

---

## 📋 **Component List**

### **LED System Components**
- [ ] 5x 3mm LED indicators (Green, Blue, Yellow, Green, Red)
- [ ] 5x 150Ω current limiting resistors
- [ ] 1x LED mounting bracket
- [ ] 1x LED wiring harness

### **E-Stop Safety Components**
- [ ] 1x Industrial E-Stop button (24V, dual-channel)
- [ ] 2x Optocoupler modules (24V input, 3.3V output)
- [ ] 2x Voltage divider circuits (24V → 3.3V)
- [ ] 1x Safety circuit PCB
- [ ] 1x E-Stop wiring harness

### **Relay System Components**
- [ ] 1x Solid-state relay module (24V DC, 2A)
- [ ] 1x Heat sink for SSR
- [ ] 1x Overcurrent protection circuit
- [ ] 1x Relay status LED
- [ ] 1x Relay wiring harness

### **General Components**
- [ ] 1x Prototype breadboard
- [ ] 1x Jumper wire kit
- [ ] 1x Multimeter for testing
- [ ] 1x Oscilloscope (if available)
- [ ] 1x 24V DC power supply

---

## 🧪 **Test Procedures**

### **LED System Testing**
```bash
# Test LED GPIO pins
echo 32 | sudo tee /sys/class/gpio/export
echo out | sudo tee /sys/class/gpio/gpio32/direction
echo 1 | sudo tee /sys/class/gpio/gpio32/value  # Turn on
echo 0 | sudo tee /sys/class/gpio/gpio32/value  # Turn off
```

### **E-Stop System Testing**
```bash
# Test E-Stop GPIO pins
echo 48 | sudo tee /sys/class/gpio/export
echo in | sudo tee /sys/class/gpio/gpio48/direction
cat /sys/class/gpio/gpio48/value  # Read status
```

### **Relay System Testing**
```bash
# Test relay GPIO pin
echo 51 | sudo tee /sys/class/gpio/export
echo out | sudo tee /sys/class/gpio/gpio51/direction
echo 1 | sudo tee /sys/class/gpio/gpio51/value  # Turn on
echo 0 | sudo tee /sys/class/gpio/gpio51/value  # Turn off
```

### **Communication Testing**
```bash
# Test RS485
sudo python3 test_uart1_pins_46_47.py

# Test Ethernet
ping 8.8.8.8

# Test WiFi
iwconfig wlan0
```

---

## 📊 **Success Criteria**

### **Hardware Criteria**
- [ ] All LED circuits working
- [ ] E-Stop safety circuit functional
- [ ] Relay driver circuit operational
- [ ] All GPIO pins accessible
- [ ] Power supply stable

### **Software Criteria**
- [ ] HAL drivers implemented
- [ ] Device tree overlays working
- [ ] GPIO control functional
- [ ] Safety systems validated
- [ ] Communication protocols working

### **Integration Criteria**
- [ ] All systems integrated
- [ ] Performance requirements met
- [ ] Safety requirements met
- [ ] Documentation complete
- [ ] Test procedures validated

---

## 🚨 **Risk Assessment**

### **Technical Risks**
- **PCB Design Complexity:** Medium risk
- **Safety Circuit Design:** High risk (SIL2 compliance)
- **Component Availability:** Low risk
- **Integration Issues:** Medium risk

### **Mitigation Strategies**
- **PCB Design:** Start with breadboard prototype
- **Safety Circuit:** Follow industry standards
- **Components:** Order early, have alternatives
- **Integration:** Test incrementally

---

## 📞 **Next Steps**

### **Immediate (Next 24 hours):**
1. **📋 Review Plan** - Finalize hardware setup plan
2. **🔧 Component List** - Complete component procurement list
3. **📐 Circuit Design** - Start LED và E-Stop circuit design
4. **📚 Documentation** - Update documentation

### **Short-term (Next 48 hours):**
1. **🛒 Order Components** - Procure required components
2. **🔧 Breadboard Setup** - Prepare breadboard for prototyping
3. **📐 Circuit Design** - Complete circuit designs
4. **🧪 Test Setup** - Prepare test procedures

**EMBED Team sẵn sàng bắt đầu hardware implementation!** 🚀
