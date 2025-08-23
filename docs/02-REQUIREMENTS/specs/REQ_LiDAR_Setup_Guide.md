# LiDAR Setup Guide - OHT-50 (v2.0)

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated để tích hợp với Dock & Location module qua USB

---

## 🎯 **MỤC TIÊU**

Hướng dẫn cài đặt và cấu hình LiDAR RPLIDAR A1M8 qua USB để tích hợp với Dock & Location module cho hệ thống OHT-50.

---

## 🔧 **HARDWARE SETUP**

### **LiDAR Components:**
```
1. RPLIDAR A1M8 Sensor
2. USB 2.0 Cable (Type-A to Type-B)
3. Power Supply via USB (5V, 1.5A)
4. Mounting Bracket
5. Protection Cover
6. Cable Management
7. Vibration Isolation Mounts
```

### **Mounting Requirements:**
```
Height:              300-500mm từ mặt đất
Orientation:         Horizontal scan plane
Clearance:           ≥100mm clearance xung quanh
Stability:           Vibration isolation recommended
Protection:          Dust và moisture protection
Mounting Surface:    Flat, stable surface
```

### **Power Requirements:**
```
Voltage:             5V DC via USB
Current:             1.5A continuous
Power:               7.5W
Protection:          USB over-current protection
Filtering:           USB power filtering
```

---

## 🔌 **WIRING DIAGRAM**

### **USB Connection:**
```
LiDAR RPLIDAR A1M8
    │
    ├── USB Type-B → USB Type-A Cable
    │
    └── USB Type-A → Orange Pi 5B USB Port
```

### **Power Connection:**
```
USB Port (Orange Pi 5B)
    │
    ├── 5V → LiDAR Power Input (via USB)
    ├── GND → LiDAR Ground (via USB)
    └── Data+ → LiDAR Data+ (via USB)
    └── Data- → LiDAR Data- (via USB)
```

### **Integration với Dock & Location Module:**
```
LiDAR (USB) → Orange Pi 5B → Dock & Location Module (RS485)
    │              │                    │
    ├── Scan Data  ├── Process Data     ├── Register Map
    ├── Status     ├── Localization     ├── Safety Integration
    └── Health     └── Mapping          └── Navigation
```

---

## ⚙️ **SOFTWARE SETUP**

### **Driver Installation:**
```bash
# Install RPLIDAR SDK
sudo apt-get update
sudo apt-get install build-essential cmake
git clone https://github.com/Slamtec/rplidar_sdk.git
cd rplidar_sdk
mkdir build && cd build
cmake ..
make
sudo make install
```

### **USB Configuration:**
```bash
# Check USB device
lsusb | grep RPLIDAR

# Check device permissions
ls -la /dev/ttyUSB*

# Add user to dialout group
sudo usermod -a -G dialout $USER

# Configure USB permissions
sudo nano /etc/udev/rules.d/99-rplidar.rules
```

### **Udev Rules:**
```
# /etc/udev/rules.d/99-rplidar.rules
KERNEL=="ttyUSB*", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666", GROUP="dialout"
KERNEL=="ttyACM*", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666", GROUP="dialout"
SUBSYSTEM=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666"
```

---

## 🔧 **CONFIGURATION**

### **LiDAR Configuration:**
```c
// LiDAR configuration parameters
typedef struct {
    char device_path[64];       // "/dev/ttyUSB0"
    uint32_t baudrate;          // 115200 bps
    uint32_t scan_frequency;    // 5.5 Hz
    uint32_t rotation_speed;    // 330 RPM
    float detection_range;      // 12.0 m
    float angular_resolution;   // 0.9 degrees
    uint32_t update_rate;       // 5 Hz
} lidar_config_t;
```

### **USB Configuration:**
```c
// USB configuration
typedef struct {
    char device_path[64];       // USB device path
    uint32_t timeout_ms;        // 1000 ms
    uint8_t retry_count;        // 3
    uint32_t buffer_size;       // 4096 bytes
    uint8_t auto_reconnect;     // 1 = enabled
} usb_config_t;
```

### **Integration Configuration:**
```c
// Integration với Dock & Location module
typedef struct {
    uint8_t module_address;     // 0x05 (Dock & Location)
    uint32_t register_offset;   // 0x0070 (LiDAR registers)
    uint32_t update_rate;       // 5 Hz
    uint8_t priority;           // High priority
    char usb_device[64];        // USB device path
} integration_config_t;
```

---

## 🧪 **TESTING PROCEDURES**

### **Hardware Testing:**
```
1. USB Connection Test
   - Verify USB connection
   - Check device detection
   - Test data transfer

2. Power Test
   - Verify USB power supply
   - Check current consumption
   - Monitor temperature

3. Mechanical Test
   - Check mounting stability
   - Verify vibration isolation
   - Test protection cover
```

### **Software Testing:**
```
1. Driver Test
   - Test LiDAR driver
   - Verify USB communication
   - Check scan data

2. Integration Test
   - Test USB integration
   - Verify register mapping
   - Check data flow

3. Performance Test
   - Test scan frequency
   - Verify data accuracy
   - Check update rate
```

### **Functional Testing:**
```
1. Mapping Test
   - Test map generation
   - Verify map accuracy
   - Check map storage

2. Localization Test
   - Test position estimation
   - Verify accuracy
   - Check stability

3. Obstacle Detection Test
   - Test detection range
   - Verify accuracy
   - Check response time
```

---

## 🔧 **TROUBLESHOOTING**

### **Common Issues:**
```
1. USB Connection Issues
   - Symptom: LiDAR không được detect
   - Cause: USB cable hoặc port problem
   - Solution: Check USB connection và cable

2. Power Issues
   - Symptom: LiDAR không khởi động
   - Cause: Insufficient USB power
   - Solution: Use powered USB hub hoặc external power

3. Permission Issues
   - Symptom: Cannot access device
   - Cause: USB permissions
   - Solution: Check udev rules và user groups

4. Driver Issues
   - Symptom: No scan data
   - Cause: Driver installation problem
   - Solution: Reinstall RPLIDAR SDK
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

## 📊 **CALIBRATION**

### **Mechanical Calibration:**
```
1. Mounting Calibration
   - Ensure horizontal mounting
   - Check height requirements
   - Verify clearance

2. Orientation Calibration
   - Align với robot coordinate system
   - Check scan plane orientation
   - Verify coordinate transformation
```

### **Software Calibration:**
```
1. USB Calibration
   - Calibrate USB communication
   - Adjust timeout settings
   - Set retry parameters

2. Sensor Calibration
   - Calibrate scan frequency
   - Adjust angular resolution
   - Set detection range

3. Integration Calibration
   - Calibrate với IMU
   - Align với magnetic sensors
   - Verify coordinate system
```

---

## 📋 **MAINTENANCE**

### **Regular Maintenance:**
```
1. Visual Inspection
   - Check USB cable connection
   - Inspect protection cover
   - Verify mounting stability

2. Performance Check
   - Monitor scan quality
   - Check update rate
   - Verify accuracy

3. Software Update
   - Update LiDAR driver
   - Update firmware
   - Check configuration
```

### **Preventive Maintenance:**
```
1. Monthly
   - Clean USB connectors
   - Check cable integrity
   - Verify power supply

2. Quarterly
   - Calibrate sensor
   - Update software
   - Performance test

3. Annually
   - Complete system check
   - Replace worn parts
   - Update documentation
```

---

## 📚 **DOCUMENTATION**

### **Required Documentation:**
```
1. Hardware Setup
   - USB connection diagram
   - Mounting diagram
   - Parts list

2. Software Setup
   - Driver installation guide
   - Configuration guide
   - Testing procedures

3. Integration Guide
   - USB integration
   - Register mapping
   - Data flow

4. Maintenance Guide
   - Maintenance schedule
   - Troubleshooting guide
   - Calibration procedures
```

---

**Changelog:**
- v2.0 (2025-01-27): Updated để tích hợp với Dock & Location module qua USB
- v1.0 (2025-01-27): Initial LiDAR setup guide

**Status:** Updated để tích hợp với Dock & Location module qua USB  
**Next Steps:** Implementation theo USB setup guide
