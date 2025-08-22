---
title: "Gpio Reconfiguration Prompt"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "EMBED Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['software-development']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# **🔧 EMBED TEAM - GPIO RECONFIGURATION PROMPT**

**From:** PM Team Lead  
**To:** EMBED Team Lead  
**Date:** 2025-01-27  
**Priority:** 🔥 HIGH - IMMEDIATE ACTION REQUIRED  
**Subject:** GPIO Pin Configuration Update

---

## **📋 CURRENT GPIO STATUS CHECK**

### **✅ GPIO Configuration Test Results:**
```
🔴 LED Status Pins:
- GPIO 54 (Power LED): ✅ Configured as OUTPUT, Status: HIGH (1)
- GPIO 35 (System LED): ✅ Configured as OUTPUT, Status: HIGH (1)  
- GPIO 28 (Comm LED): ✅ Configured as OUTPUT, Status: HIGH (1)
- GPIO 29 (Network LED): ✅ Configured as OUTPUT, Status: HIGH (1)
- GPIO 58 (Error LED): ✅ Configured as OUTPUT, Status: LOW (0)

🛑 E-Stop & Relay Pins:
- GPIO 59 (E-Stop): ✅ Configured as INPUT, Status: HIGH (1)
- GPIO 131 (Relay 1): ✅ Configured as OUTPUT, Status: LOW (0)
- GPIO 132 (Relay 2): ✅ Configured as OUTPUT, Status: LOW (0)
```

---

## **🎯 EMBED TEAM ACTION REQUIRED**

### **🔥 IMMEDIATE TASK: GPIO Pin Reconfiguration**

#### **📋 Task Description:**
Cấu hình lại tất cả GPIO pins theo specification mới đã được cập nhật trong tài liệu.

#### **🎯 Required Actions:**

### **1. LED Status Pins Configuration (5x)**
```bash
# Configure LED pins as OUTPUT
gpio -g mode 54 out  # Power LED (Green)
gpio -g mode 35 out  # System LED (Blue)
gpio -g mode 28 out  # Communication LED (Yellow)
gpio -g mode 29 out  # Network LED (Green)
gpio -g mode 58 out  # Error LED (Red)

# Set initial LED states
gpio -g write 54 0   # Power LED OFF
gpio -g write 35 0   # System LED OFF
gpio -g write 28 0   # Communication LED OFF
gpio -g write 29 0   # Network LED OFF
gpio -g write 58 0   # Error LED OFF
```

### **2. E-Stop Pin Configuration (1x)**
```bash
# Configure E-Stop pin as INPUT with pull-up
gpio -g mode 59 in   # E-Stop input
gpio -g mode 59 up   # Enable pull-up resistor
```

### **3. Relay Output Pins Configuration (2x)**
```bash
# Configure Relay pins as OUTPUT
gpio -g mode 131 out # Relay 1 (24V DC)
gpio -g mode 132 out # Relay 2 (24V DC)

# Set initial relay states (OFF)
gpio -g write 131 0  # Relay 1 OFF
gpio -g write 132 0  # Relay 2 OFF
```

---

## **📊 UPDATED PIN MAPPING**

### **🔴 LED Status Pins (5x):**
| **LED** | **GPIO Pin** | **GPIO Name** | **Mode** | **Initial State** |
|---------|--------------|---------------|----------|-------------------|
| **Power LED** | GPIO 54 | GPIO1_D6 | OUTPUT | OFF (0) |
| **System LED** | GPIO 35 | GPIO1_A3 | OUTPUT | OFF (0) |
| **Communication LED** | GPIO 28 | GPIO0_D4 | OUTPUT | OFF (0) |
| **Network LED** | GPIO 29 | GPIO0_D5 | OUTPUT | OFF (0) |
| **Error LED** | GPIO 58 | GPIO1_D2 | OUTPUT | OFF (0) |

### **🛑 E-Stop & Relay Pins (3x):**
| **Component** | **GPIO Pin** | **GPIO Name** | **Mode** | **Initial State** |
|---------------|--------------|---------------|----------|-------------------|
| **E-Stop Input** | GPIO 59 | GPIO1_D3 | INPUT (Pull-up) | HIGH (1) |
| **Relay 1 Output** | GPIO 131 | GPIO4_A3 | OUTPUT | OFF (0) |
| **Relay 2 Output** | GPIO 132 | GPIO4_A4 | OUTPUT | OFF (0) |

---

## **🔧 IMPLEMENTATION STEPS**

### **📋 Step 1: GPIO Configuration Script**
```bash
#!/bin/bash
# OHT-50 GPIO Configuration Script
# EMBED Team - Updated 2025-01-27

echo "Configuring OHT-50 GPIO pins..."

# LED Status Pins (5x)
echo "Configuring LED pins..."
gpio -g mode 54 out  # Power LED
gpio -g mode 35 out  # System LED
gpio -g mode 28 out  # Communication LED
gpio -g mode 29 out  # Network LED
gpio -g mode 58 out  # Error LED

# Set LED initial states (OFF)
gpio -g write 54 0
gpio -g write 35 0
gpio -g write 28 0
gpio -g write 29 0
gpio -g write 58 0

# E-Stop Pin (1x)
echo "Configuring E-Stop pin..."
gpio -g mode 59 in   # E-Stop input
gpio -g mode 59 up   # Enable pull-up

# Relay Output Pins (2x)
echo "Configuring Relay pins..."
gpio -g mode 131 out # Relay 1
gpio -g mode 132 out # Relay 2

# Set relay initial states (OFF)
gpio -g write 131 0
gpio -g write 132 0

echo "GPIO configuration complete!"
```

### **📋 Step 2: Verification Script**
```bash
#!/bin/bash
# OHT-50 GPIO Verification Script

echo "Verifying GPIO configuration..."

# Check LED pins
echo "LED Pins Status:"
echo "Power LED (GPIO 54): $(gpio -g read 54)"
echo "System LED (GPIO 35): $(gpio -g read 35)"
echo "Comm LED (GPIO 28): $(gpio -g read 28)"
echo "Network LED (GPIO 29): $(gpio -g read 29)"
echo "Error LED (GPIO 58): $(gpio -g read 58)"

# Check E-Stop pin
echo "E-Stop Pin (GPIO 59): $(gpio -g read 59)"

# Check Relay pins
echo "Relay 1 (GPIO 131): $(gpio -g read 131)"
echo "Relay 2 (GPIO 132): $(gpio -g read 132)"

echo "Verification complete!"
```

---

## **📊 TESTING REQUIREMENTS**

### **✅ LED Testing:**
1. **Power LED Test:**
   ```bash
   gpio -g write 54 1  # Turn ON
   sleep 1
   gpio -g write 54 0  # Turn OFF
   ```

2. **System LED Test:**
   ```bash
   gpio -g write 35 1  # Turn ON
   sleep 1
   gpio -g write 35 0  # Turn OFF
   ```

3. **Communication LED Test:**
   ```bash
   gpio -g write 28 1  # Turn ON
   sleep 1
   gpio -g write 28 0  # Turn OFF
   ```

4. **Network LED Test:**
   ```bash
   gpio -g write 29 1  # Turn ON
   sleep 1
   gpio -g write 29 0  # Turn OFF
   ```

5. **Error LED Test:**
   ```bash
   gpio -g write 58 1  # Turn ON
   sleep 1
   gpio -g write 58 0  # Turn OFF
   ```

### **✅ E-Stop Testing:**
```bash
# Check E-Stop status
gpio -g read 59  # Should return 1 (HIGH) when not pressed
```

### **✅ Relay Testing:**
1. **Relay 1 Test:**
   ```bash
   gpio -g write 131 1  # Turn ON
   sleep 1
   gpio -g write 131 0  # Turn OFF
   ```

2. **Relay 2 Test:**
   ```bash
   gpio -g write 132 1  # Turn ON
   sleep 1
   gpio -g write 132 0  # Turn OFF
   ```

---

## **📋 SUCCESS CRITERIA**

### **✅ GPIO Configuration Success:**
- [ ] All 5 LED pins configured as OUTPUT
- [ ] E-Stop pin configured as INPUT with pull-up
- [ ] Both relay pins configured as OUTPUT
- [ ] All pins respond to read/write commands
- [ ] LED pins can be turned ON/OFF
- [ ] E-Stop pin reads correct state
- [ ] Relay pins can be controlled

### **✅ Documentation Update:**
- [ ] Update GPIO configuration scripts
- [ ] Update testing procedures
- [ ] Update hardware documentation
- [ ] Update HAL driver documentation

---

## **🚨 ESCALATION**

### **⚠️ If Issues Found:**
- **GPIO pin not responding:** Check physical connections
- **Permission denied:** Check user permissions
- **Pin already in use:** Check for conflicts
- **Hardware not responding:** Check power supply

### **📞 Support Required:**
- **Technical issues:** EMBED Team Lead
- **Hardware problems:** Hardware Engineer
- **Documentation updates:** Technical Writer

---

**📋 Status:** GPIO configuration ready for EMBED team implementation.

**🎯 Next Action:** EMBED team execute GPIO reconfiguration immediately.

**⏰ Timeline:** Complete by end of today (2025-01-27).

---

**From:** PM Team Lead - OHT-50 Master Module  
**Date:** 2025-01-27  
**Priority:** 🔥 HIGH  
**Status:** IMMEDIATE ACTION REQUIRED
