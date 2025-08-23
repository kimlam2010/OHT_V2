# LiDAR Wiring Guide - OHT-50 (v2.0)

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated để tích hợp với Dock & Location module qua USB

---

## 🎯 **MỤC TIÊU**

Hướng dẫn wiring LiDAR RPLIDAR A1M8 qua USB để tích hợp với Dock & Location module cho hệ thống OHT-50.

---

## 🔧 **HARDWARE COMPONENTS**

### **Required Components:**
```
1. RPLIDAR A1M8 Sensor
2. USB 2.0 Cable (Type-A to Type-B)
3. Orange Pi 5B (Master Module)
4. Mounting Bracket
5. Protection Cover
6. Cable Management System
7. Vibration Isolation Mounts
```

### **Component Specifications:**
```
RPLIDAR A1M8:
- Interface: USB 2.0 Type-B
- Power: 5V DC via USB
- Current: 1.5A maximum
- Operating Temperature: -10°C to +50°C
- Protection: IP40

USB Cable:
- Type: USB 2.0 Type-A to Type-B
- Length: 1-3 meters (recommended)
- Shielding: Shielded twisted pair
- Quality: High-quality, low-noise

Mounting Bracket:
- Material: Aluminum hoặc steel
- Mounting: Vibration isolation
- Protection: Dust/moisture resistant
```

---

## 🔌 **WIRING DIAGRAM**

### **USB Connection Diagram:**
```
RPLIDAR A1M8                    Orange Pi 5B
    │                               │
    ├── USB Type-B Port            ├── USB Type-A Port
    │   ├── VCC (5V)               │   ├── VCC (5V)
    │   ├── GND                    │   ├── GND
    │   ├── D+ (Data+)             │   ├── D+ (Data+)
    │   └── D- (Data-)             │   └── D- (Data-)
    │                               │
    └── USB Cable                  └── USB Cable
        ├── Red Wire (VCC)             ├── Red Wire (VCC)
        ├── Black Wire (GND)           ├── Black Wire (GND)
        ├── Green Wire (D+)            ├── Green Wire (D+)
        └── White Wire (D-)            └── White Wire (D-)
```

### **Power Distribution:**
```
Orange Pi 5B USB Port
    │
    ├── 5V Power Supply
    │   ├── VCC → LiDAR Power Input
    │   └── Current: 1.5A maximum
    │
    ├── Ground Connection
    │   ├── GND → LiDAR Ground
    │   └── Common ground reference
    │
    └── Data Communication
        ├── D+ → LiDAR Data+
        └── D- → LiDAR Data-
```

---

## 📍 **MOUNTING DIAGRAM**

### **Top View:**
```
                    Robot Platform
    ┌─────────────────────────────────────────┐
    │                                         │
    │  ┌─────────────────────────────────────┐ │
    │  │         Mounting Bracket            │ │
    │  │  ┌─────────────────────────────┐    │ │
    │  │  │      Protection Cover       │    │ │
    │  │  │  ┌─────────────────────┐    │    │ │
    │  │  │  │   RPLIDAR A1M8      │    │    │ │
    │  │  │  │   (USB Port)        │    │    │ │
    │  │  │  └─────────────────────┘    │    │ │
    │  │  └─────────────────────────────┘    │ │
    │  └─────────────────────────────────────┘ │
    │                                         │
    └─────────────────────────────────────────┘
```

### **Side View:**
```
    Height: 300-500mm
    │
    ┌─────────────────┐
    │   LiDAR A1M8    │ ← Scan Plane
    │                 │
    ├─────────────────┤
    │ Protection Cover│
    ├─────────────────┤
    │ Mounting Bracket│
    ├─────────────────┤
    │ Vibration Mount │
    ├─────────────────┤
    │ Robot Platform  │
    └─────────────────┘
```

---

## 🔧 **INSTALLATION STEPS**

### **Step 1: Mounting Preparation**
```
1. Select mounting location
   - Height: 300-500mm from ground
   - Clearance: ≥100mm around LiDAR
   - Orientation: Horizontal scan plane
   - Stability: Vibration-free mounting

2. Install mounting bracket
   - Secure bracket to robot platform
   - Use vibration isolation mounts
   - Ensure proper alignment
   - Check mounting stability
```

### **Step 2: LiDAR Installation**
```
1. Mount LiDAR sensor
   - Place LiDAR on mounting bracket
   - Secure with provided screws
   - Check horizontal orientation
   - Verify scan plane alignment

2. Install protection cover
   - Place protection cover over LiDAR
   - Secure cover properly
   - Ensure dust/moisture protection
   - Check for interference
```

### **Step 3: USB Connection**
```
1. Connect USB cable
   - Connect Type-B end to LiDAR
   - Connect Type-A end to Orange Pi 5B
   - Ensure secure connection
   - Check cable routing

2. Cable management
   - Route cable properly
   - Avoid sharp bends
   - Secure cable with ties
   - Protect from damage
```

### **Step 4: Power Verification**
```
1. Check power supply
   - Verify 5V power from USB
   - Check current consumption
   - Monitor temperature
   - Test power stability

2. Test communication
   - Check device detection
   - Verify USB communication
   - Test data transfer
   - Confirm functionality
```

---

## 🔍 **VERIFICATION PROCEDURES**

### **Physical Verification:**
```
1. Mounting Verification
   - Height: 300-500mm ✓
   - Orientation: Horizontal ✓
   - Clearance: ≥100mm ✓
   - Stability: Vibration-free ✓

2. Connection Verification
   - USB cable: Properly connected ✓
   - Power: 5V supply ✓
   - Ground: Common reference ✓
   - Data: Communication working ✓
```

### **Functional Verification:**
```
1. Device Detection
   - lsusb | grep RPLIDAR ✓
   - Device path: /dev/ttyUSB0 ✓
   - Permissions: 666 ✓
   - Driver: Loaded ✓

2. Communication Test
   - USB communication: Working ✓
   - Data transfer: Successful ✓
   - Scan data: Valid ✓
   - Performance: Acceptable ✓
```

---

## ⚠️ **SAFETY CONSIDERATIONS**

### **Electrical Safety:**
```
1. Power Safety
   - Use proper USB power supply
   - Check current limits
   - Monitor temperature
   - Prevent overloading

2. Connection Safety
   - Secure connections
   - Protect from moisture
   - Avoid cable damage
   - Use proper grounding
```

### **Mechanical Safety:**
```
1. Mounting Safety
   - Secure mounting
   - Vibration isolation
   - Protection from impact
   - Environmental protection

2. Operation Safety
   - Clear scan area
   - Avoid interference
   - Monitor performance
   - Regular maintenance
```

---

## 🔧 **TROUBLESHOOTING**

### **Common Issues:**
```
1. USB Connection Issues
   - Problem: Device not detected
   - Cause: Loose connection
   - Solution: Check cable connection

2. Power Issues
   - Problem: Insufficient power
   - Cause: USB port limitations
   - Solution: Use powered USB hub

3. Mounting Issues
   - Problem: Unstable mounting
   - Cause: Poor mounting
   - Solution: Improve mounting stability

4. Interference Issues
   - Problem: Poor scan quality
   - Cause: Environmental interference
   - Solution: Improve protection
```

### **Diagnostic Commands:**
```bash
# Check USB devices
lsusb
dmesg | grep -i usb

# Check LiDAR device
ls -la /dev/ttyUSB*
ls -la /dev/ttyACM*

# Test USB communication
sudo stty -F /dev/ttyUSB0 115200
sudo cat /dev/ttyUSB0

# Check system resources
htop
iostat
```

---

## 📋 **MAINTENANCE**

### **Regular Maintenance:**
```
1. Visual Inspection
   - Check USB cable condition
   - Inspect mounting stability
   - Verify protection cover
   - Clean connectors

2. Performance Check
   - Monitor scan quality
   - Check communication
   - Verify accuracy
   - Test functionality

3. Preventive Maintenance
   - Clean USB connectors
   - Check cable integrity
   - Verify mounting
   - Update software
```

### **Maintenance Schedule:**
```
Monthly:
- Clean USB connectors
- Check cable condition
- Verify mounting stability
- Test communication

Quarterly:
- Complete system check
- Performance verification
- Software updates
- Documentation review

Annually:
- Full system inspection
- Component replacement
- Performance optimization
- Training update
```

---

## 📚 **DOCUMENTATION**

### **Required Documentation:**
```
1. Wiring Diagram
   - USB connection diagram
   - Power distribution
   - Grounding scheme

2. Mounting Diagram
   - Top view
   - Side view
   - Dimensions
   - Clearance requirements

3. Installation Guide
   - Step-by-step instructions
   - Verification procedures
   - Troubleshooting guide

4. Maintenance Guide
   - Maintenance schedule
   - Inspection procedures
   - Replacement procedures
```

---

**Changelog:**
- v2.0 (2025-01-27): Updated để tích hợp với Dock & Location module qua USB
- v1.0 (2025-01-27): Initial LiDAR wiring guide

**Status:** Updated để tích hợp với Dock & Location module qua USB  
**Next Steps:** Implementation theo wiring guide
