---
title: "Lidar Setup Guide"
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

# 🔧 LiDAR Setup Guide - OHT-50

**Version:** 1.0.0  
**Date:** 2025-01-27  
**Team:** EMBED  
**Task:** EM-12 (LiDAR Driver & USB Integration)

---

## 📋 **Prerequisites**

### **Hardware Requirements:**
- Orange Pi 5B với USB 2.0 port
- RPLIDAR A1M8 sensor
- USB Type-A to Type-C cable
- Mounting bracket cho LiDAR

### **Software Requirements:**
- Ubuntu 22.04 LTS hoặc tương đương
- GCC compiler
- Git
- libudev-dev package

---

## 🛠️ **Step-by-Step Setup**

### **Step 1: Hardware Installation**

#### **1.1 Physical Mounting:**
```bash
# Mount LiDAR trên OHT-50 platform
# Vị trí: Front-facing, height 300mm from ground
# Orientation: Forward direction (0° angle)
```

#### **1.2 USB Connection:**
```bash
# Kết nối USB cable
# Port: Bất kỳ USB 2.0 port nào trên Orange Pi 5B
# Cable: USB Type-A to Type-C (included với LiDAR)
```

#### **1.3 Power Verification:**
```bash
# Kiểm tra power supply
# Voltage: 5V DC
# Current: ~500mA peak
# Power: ~2.5W
```

### **Step 2: Software Installation**

#### **2.1 System Update:**
```bash
sudo apt-get update
sudo apt-get upgrade -y
```

#### **2.2 Install Dependencies:**
```bash
sudo apt-get install -y \
    build-essential \
    git \
    libudev-dev \
    cmake \
    pkg-config
```

#### **2.3 Install RPLIDAR SDK:**
```bash
# Clone RPLIDAR SDK
cd ~
git clone https://github.com/Slamtec/rplidar_sdk.git
cd rplidar_sdk

# Build SDK
mkdir build && cd build
cmake ..
make -j4

# Install SDK
sudo make install
sudo ldconfig
```

#### **2.4 Configure udev Rules:**
```bash
# Tạo udev rules file
sudo nano /etc/udev/rules.d/99-rplidar.rules
```

**Nội dung file `/etc/udev/rules.d/99-rplidar.rules`:**
```
# RPLIDAR A1M8 udev rules
SUBSYSTEM=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666"
SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666"
```

```bash
# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### **Step 3: Device Verification**

#### **3.1 Check USB Detection:**
```bash
# Kiểm tra USB device
lsusb | grep 10c4
# Expected output: Bus XXX Device XXX: ID 10c4:ea60 Silicon Labs CP210x UART Bridge
```

#### **3.2 Check Serial Device:**
```bash
# Kiểm tra serial device
ls -la /dev/ttyUSB*
# Expected: /dev/ttyUSB0

# Check device permissions
ls -la /dev/ttyUSB0
# Expected: crw-rw-rw- 1 root dialout 188, 0 Jan 27 10:00 /dev/ttyUSB0
```

#### **3.3 Test Serial Communication:**
```bash
# Test basic serial communication
sudo chmod 666 /dev/ttyUSB0
stty -F /dev/ttyUSB0 115200

# Send test command (Get device info)
echo -e "\xA5\x25" > /dev/ttyUSB0

# Check response (optional)
hexdump -C /dev/ttyUSB0
```

### **Step 4: SDK Testing**

#### **4.1 Test with RPLIDAR SDK:**
```bash
# Navigate to SDK examples
cd ~/rplidar_sdk/sdk/output/Linux/Release/

# Test basic functionality
./ultra_simple /dev/ttyUSB0
# Expected: LiDAR starts scanning và hiển thị distance data
```

#### **4.2 Test with SDK Examples:**
```bash
# Test với các examples khác
./simple_grabber /dev/ttyUSB0
./frame_grabber /dev/ttyUSB0
```

---

## 🔧 **Configuration**

### **Serial Port Configuration:**
```bash
# Configure serial port parameters
stty -F /dev/ttyUSB0 \
    baudrate 115200 \
    cs8 \
    -cstopb \
    -parenb \
    -echo \
    -echoe \
    -echok \
    -icanon \
    -isig \
    -iexten \
    -opost
```

### **Performance Tuning:**
```bash
# Optimize USB performance
echo 'ACTION=="add", SUBSYSTEM=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", ATTR{power/autosuspend}="-1"' | sudo tee -a /etc/udev/rules.d/99-rplidar.rules
```

---

## 🧪 **Validation Tests**

### **Test 1: Basic Connectivity**
```bash
#!/bin/bash
# Test script: test_lidar_basic.sh

echo "=== LiDAR Basic Connectivity Test ==="

# Check USB device
echo "1. Checking USB device..."
if lsusb | grep -q "10c4:ea60"; then
    echo "✅ USB device detected"
else
    echo "❌ USB device not found"
    exit 1
fi

# Check serial device
echo "2. Checking serial device..."
if [ -e /dev/ttyUSB0 ]; then
    echo "✅ Serial device found: /dev/ttyUSB0"
else
    echo "❌ Serial device not found"
    exit 1
fi

# Test permissions
echo "3. Checking permissions..."
if [ -r /dev/ttyUSB0 ] && [ -w /dev/ttyUSB0 ]; then
    echo "✅ Device permissions OK"
else
    echo "❌ Permission issues"
    exit 1
fi

echo "✅ Basic connectivity test PASSED"
```

### **Test 2: SDK Functionality**
```bash
#!/bin/bash
# Test script: test_lidar_sdk.sh

echo "=== LiDAR SDK Functionality Test ==="

# Test SDK installation
echo "1. Testing SDK installation..."
if [ -f /usr/local/lib/librplidar_sdk.so ]; then
    echo "✅ SDK library found"
else
    echo "❌ SDK library not found"
    exit 1
fi

# Test SDK examples
echo "2. Testing SDK examples..."
cd ~/rplidar_sdk/sdk/output/Linux/Release/

if [ -f ./ultra_simple ]; then
    echo "✅ SDK examples compiled"
else
    echo "❌ SDK examples not compiled"
    exit 1
fi

echo "✅ SDK functionality test PASSED"
```

### **Test 3: Data Acquisition**
```bash
#!/bin/bash
# Test script: test_lidar_data.sh

echo "=== LiDAR Data Acquisition Test ==="

# Test data acquisition (timeout 10 seconds)
echo "1. Testing data acquisition..."
timeout 10s ./ultra_simple /dev/ttyUSB0 > /tmp/lidar_test.log 2>&1

# Check if data was received
if [ -s /tmp/lidar_test.log ]; then
    echo "✅ Data acquisition successful"
    echo "Sample data:"
    head -5 /tmp/lidar_test.log
else
    echo "❌ No data received"
    exit 1
fi

echo "✅ Data acquisition test PASSED"
```

---

## 🚨 **Troubleshooting**

### **Issue 1: Device Not Detected**
```bash
# Symptoms: lsusb không hiển thị 10c4:ea60
# Solution:
sudo dmesg | grep -i usb
sudo dmesg | grep -i tty
# Check USB cable connection
# Try different USB port
```

### **Issue 2: Permission Denied**
```bash
# Symptoms: Cannot access /dev/ttyUSB0
# Solution:
sudo usermod -a -G dialout $USER
# Logout và login lại
# Hoặc:
sudo chmod 666 /dev/ttyUSB0
```

### **Issue 3: No Data from LiDAR**
```bash
# Symptoms: SDK không nhận được data
# Solution:
# 1. Check baud rate
stty -F /dev/ttyUSB0 115200

# 2. Check cable connection
# 3. Restart LiDAR power
# 4. Check for interference
```

### **Issue 4: Inaccurate Readings**
```bash
# Symptoms: Distance readings không chính xác
# Solution:
# 1. Check mounting angle
# 2. Check for reflective surfaces
# 3. Calibrate sensor
# 4. Check for vibration
```

---

## 📊 **Performance Monitoring**

### **System Resources:**
```bash
# Monitor USB bandwidth
sudo cat /sys/kernel/debug/usb/devices | grep -A 10 "10c4:ea60"

# Monitor serial port
sudo cat /proc/tty/driver/usbserial

# Monitor system load
htop
```

### **LiDAR Performance:**
```bash
# Monitor scan rate
# Expected: 8Hz (125ms per scan)

# Monitor data quality
# Expected: 400 points per scan

# Monitor range accuracy
# Expected: ±2cm
```

---

## 📝 **Documentation**

### **Files Created:**
- ✅ `docs/specs/lidar_wiring_guide.md`
- ✅ `docs/specs/lidar_setup_guide.md`
- 🔄 `docs/specs/lidar_test_procedures.md` (next)
- 🔄 `docs/specs/lidar_integration_guide.md` (next)

### **Next Steps:**
1. Tạo test procedures
2. Tạo integration guide
3. Implement HAL driver
4. Tích hợp với safety system

---

**🎯 Setup Complete!**
LiDAR sensor đã được cài đặt và cấu hình thành công. Sẵn sàng cho bước tiếp theo: test procedures và HAL driver implementation.
