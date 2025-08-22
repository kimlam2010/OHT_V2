---
title: "Lidar Wiring Guide"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "CTO Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['system-architecture', 'lidar', 'sensors', 'guide', 'documentation']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# 🔧 LiDAR Wiring Guide - OHT-50

**Version:** 1.1.0  
**Date:** 2025-01-27  
**Team:** EMBED  
**Task:** EM-12 (LiDAR Driver & USB Integration)

---

## 📋 **Tổng quan**

### **Mục tiêu:**
- Tích hợp LiDAR sensor qua USB để phát hiện chướng ngại vật
- Đảm bảo an toàn khi có vật cản trong quá trình di chuyển
- Tích hợp với hệ thống E-Stop hiện có

### **Thiết bị được chọn:**
- **LiDAR Model:** RPLIDAR C1M1 360° Laser Range Scanner
- **Khoảng cách:** 0.05m - 12m (white object), 0.05m - 6m (black object)
- **Góc quét:** 360°
- **Tần suất:** 8-12Hz (typical 10Hz)
- **Interface:** TTL UART (3.3V level) qua USB bridge

---

## 🔌 **Wiring Diagram**

### **USB Connection:**
```
RPLIDAR C1M1 ── USB Cable ── Orange Pi 5B USB Port
     │
     ├── VCC (5V)
     ├── GND
     ├── D+ (Data+)
     └── D- (Data-)
```

### **Power Requirements:**
- **Voltage:** 5V DC
- **Current:** ~500mA peak
- **Power:** ~2.5W

### **USB Port Assignment:**
- **Port:** USB 2.0 (bất kỳ port nào trên Orange Pi 5B)
- **Device:** `/dev/ttyUSB0` (sau khi cài driver)
- **Baud Rate:** 460800 (khác với A1M8: 115200)

---

## 🛠️ **Hardware Setup**

### **1. Physical Mounting:**
```
OHT-50 Platform
    │
    ├── LiDAR Mount (front-facing)
    │   ├── Height: 300mm from ground
    │   ├── Angle: 0° (horizontal)
    │   └── Orientation: Forward direction
    │
    └── Cable Management
        ├── USB cable routing
        ├── Strain relief
        └── EMI protection
```

### **2. Safety Considerations:**
- **EMI Shielding:** USB cable có shield
- **Strain Relief:** Cable tie để tránh kéo
- **Water Protection:** IP54 protection (built-in)
- **Vibration:** Mounting bracket chống rung

---

## 🔧 **Software Setup**

### **1. Driver Installation:**
```bash
# Cài đặt RPLIDAR SDK
sudo apt-get update
sudo apt-get install libudev-dev
git clone https://github.com/Slamtec/rplidar_sdk.git
cd rplidar_sdk
make
sudo make install
```

### **2. udev Rules:**
```bash
# Tạo file /etc/udev/rules.d/99-rplidar.rules
SUBSYSTEM=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666"
```

### **3. Device Detection:**
```bash
# Kiểm tra device
ls -la /dev/ttyUSB*
dmesg | grep ttyUSB
```

---

## 🧪 **Testing Procedures**

### **1. Basic Connectivity Test:**
```bash
# Test USB connection
lsusb | grep 10c4
# Expected: Bus XXX Device XXX: ID 10c4:ea60 Silicon Labs CP210x UART Bridge
```

### **2. Serial Communication Test:**
```bash
# Test serial communication với baud rate đúng
sudo chmod 666 /dev/ttyUSB0
stty -F /dev/ttyUSB0 460800
echo -e "\xA5\x25" > /dev/ttyUSB0  # Get device info command
```

### **3. SDK Test:**
```bash
# Test với RPLIDAR SDK
cd rplidar_sdk/sdk/output/Linux/Release/
./ultra_simple /dev/ttyUSB0
```

---

## 📊 **Performance Specifications**

### **Scanning Performance:**
- **Scan Rate:** 8-12Hz (typical 10Hz)
- **Sample Rate:** 5KHz
- **Angular Resolution:** 0.72° (typical)
- **Range Accuracy:** ±30mm
- **Range Resolution:** 15mm

### **Safety Thresholds:**
- **Emergency Stop Distance:** 0.5m
- **Warning Distance:** 1.0m
- **Safe Distance:** 2.0m

### **Integration Requirements:**
- **E-Stop Trigger:** Khi phát hiện vật < 0.5m
- **Warning Alert:** Khi phát hiện vật < 1.0m
- **Normal Operation:** Khi không có vật < 2.0m

---

## 🔒 **Safety Integration**

### **E-Stop Integration:**
```
LiDAR Detection ── Distance Check ── E-Stop Trigger
     │                    │                │
     ├── < 0.5m ──┐      │                │
     │             ├── Emergency Stop     │
     ├── < 1.0m ──┤      │                │
     │             └── Warning Alert      │
     └── > 2.0m ──┘      │                │
                         │                │
                    ┌────┴────┐      ┌────┴────┐
                    │ Warning │      │ E-Stop  │
                    │ System  │      │ System  │
                    └─────────┘      └─────────┘
```

### **Safety Logic:**
1. **Continuous Monitoring:** LiDAR scan liên tục
2. **Distance Calculation:** Tính khoảng cách ngắn nhất
3. **Threshold Check:** So sánh với ngưỡng an toàn
4. **Action Trigger:** Kích hoạt E-Stop hoặc warning

---

## 📝 **Documentation Requirements**

### **Files to Create:**
- `docs/specs/lidar_setup_guide.md`
- `docs/specs/lidar_test_procedures.md`
- `docs/specs/lidar_integration_guide.md`
- `firmware/src/hal/hal_lidar.c`
- `firmware/include/hal_lidar.h`

### **Integration Points:**
- HAL interface cho LiDAR
- Safety system integration
- E-Stop trigger mechanism
- Warning system integration

---

## 🚨 **Troubleshooting**

### **Common Issues:**
1. **Device not detected:** Kiểm tra USB cable, power supply
2. **Permission denied:** Kiểm tra udev rules, user permissions
3. **No data:** Kiểm tra baud rate (phải là 460800), cable connection
4. **Inaccurate readings:** Kiểm tra mounting, calibration

### **Debug Commands:**
```bash
# Check USB devices
lsusb -v | grep -A 10 "10c4:ea60"

# Check serial port với baud rate đúng
stty -F /dev/ttyUSB0 -a

# Monitor serial data
hexdump -C /dev/ttyUSB0
```

---

**📋 Next Steps:**
1. Tạo `lidar_setup_guide.md`
2. Tạo `lidar_test_procedures.md`
3. Tạo `lidar_integration_guide.md`
4. Implement HAL LiDAR driver
5. Tích hợp với safety system
