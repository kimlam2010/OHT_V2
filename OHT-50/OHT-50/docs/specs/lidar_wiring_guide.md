# LiDAR Wiring Guide - OHT-50

## 📋 **Tổng quan**
Hướng dẫn kết nối và cấu hình LiDAR cho hệ thống OHT-50.

**Ngày tạo:** 2025-01-27  
**Phiên bản:** v1.0  
**Trạng thái:** Draft

---

## 🎯 **Mục tiêu**

### **LiDAR Integration**
- Kết nối USB LiDAR với Orange Pi 5B
- Cấu hình driver và software stack
- Test và validation LiDAR functionality
- Integration với navigation system

### **Hardware Requirements**
- USB LiDAR sensor (RPLIDAR A1/A2 hoặc tương đương)
- USB 2.0/3.0 cable
- Orange Pi 5B với USB ports
- Power supply đủ cho LiDAR

---

## 🔧 **Hardware Setup**

### **LiDAR Model Specification**
```
Model: RPLIDAR A1/A2 (hoặc tương đương)
Interface: USB 2.0
Power: 5V/500mA (USB powered)
Range: 0.15m - 12m
Accuracy: ±2cm
Scan Rate: 5.5Hz (A1) / 8Hz (A2)
Angular Resolution: 1°
```

### **USB Connection**
```
Orange Pi 5B USB Port → USB Cable → LiDAR Sensor
- USB 2.0 Type-A (Orange Pi) → USB Type-B (LiDAR)
- Power: 5V/500mA từ USB port
- Data: USB 2.0 protocol
```

### **Physical Mounting**
```
LiDAR Position: Top center của OHT-50
Height: 30-50cm từ ground
Orientation: Horizontal, 360° scan
Mounting: Vibration isolated mount
Cable Routing: Protected cable path
```

---

## 💻 **Software Setup**

### **1. Driver Installation**
```bash
# Install LiDAR dependencies
sudo apt update
sudo apt install -y python3-pip python3-dev
sudo apt install -y libusb-1.0-0-dev

# Install Python LiDAR libraries
pip3 install rplidar-roboticia
pip3 install pyusb
pip3 install numpy matplotlib
```

### **2. USB Permissions**
```bash
# Create udev rules for LiDAR
sudo nano /etc/udev/rules.d/99-lidar.rules

# Add content:
SUBSYSTEM=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="5740", MODE="0666"

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### **3. Test Connection**
```bash
# Check USB devices
lsusb | grep -i lidar

# Check device permissions
ls -la /dev/ttyUSB*
ls -la /dev/ttyACM*
```

---

## 🔌 **Connection Diagram**

```
┌─────────────────┐    USB Cable    ┌─────────────────┐
│   Orange Pi 5B  │ ──────────────→ │   LiDAR Sensor  │
│                 │                 │                 │
│ USB 2.0 Port    │                 │ USB Type-B      │
│ Power: 5V/500mA │                 │ Power: 5V/500mA │
│ Data: USB 2.0   │                 │ Data: USB 2.0   │
└─────────────────┘                 └─────────────────┘
         │                                   │
         │                                   │
         ▼                                   ▼
┌─────────────────┐                 ┌─────────────────┐
│   Software      │                 │   Hardware      │
│   - Driver      │                 │   - Sensor      │
│   - API         │                 │   - Mount       │
│   - Processing  │                 │   - Cable       │
└─────────────────┘                 └─────────────────┘
```

---

## 📊 **Pin Assignment**

### **USB Connection**
```
Orange Pi 5B USB Port:
- VCC: 5V (Red)
- D-: Data- (White)
- D+: Data+ (Green)
- GND: Ground (Black)

LiDAR USB Type-B:
- VCC: 5V (Red)
- D-: Data- (White)
- D+: Data+ (Green)
- GND: Ground (Black)
```

### **Power Requirements**
```
LiDAR Power Consumption:
- Operating: 5V/500mA
- Peak: 5V/800mA
- Standby: 5V/100mA

Orange Pi 5B USB Port:
- USB 2.0: 5V/500mA per port
- USB 3.0: 5V/900mA per port
- Total: 5V/2A (multiple ports)
```

---

## 🔧 **Configuration**

### **1. LiDAR Configuration**
```python
# LiDAR configuration parameters
LIDAR_CONFIG = {
    'port': '/dev/ttyUSB0',  # USB device port
    'baudrate': 115200,      # Communication baudrate
    'timeout': 1.0,          # Timeout in seconds
    'scan_frequency': 5.5,   # Scan frequency in Hz
    'max_distance': 12.0,    # Maximum distance in meters
    'min_distance': 0.15,    # Minimum distance in meters
    'angle_offset': 0.0,     # Angle offset in degrees
}
```

### **2. System Integration**
```python
# Integration with OHT-50 system
OHT50_LIDAR_CONFIG = {
    'update_rate': 10.0,     # Update rate in Hz
    'obstacle_threshold': 1.0, # Obstacle detection threshold
    'safety_zone': 0.5,      # Safety zone in meters
    'navigation_zone': 2.0,  # Navigation zone in meters
    'emergency_stop': 0.3,   # Emergency stop distance
}
```

---

## 🧪 **Testing Procedures**

### **1. Basic Connection Test**
```bash
# Test LiDAR connection
python3 test_lidar_connection.py

# Expected output:
# ✅ LiDAR connected successfully
# ✅ Device: /dev/ttyUSB0
# ✅ Baudrate: 115200
# ✅ Communication: OK
```

### **2. Functionality Test**
```bash
# Test LiDAR functionality
python3 test_lidar_functionality.py

# Expected output:
# ✅ Scan data received
# ✅ Distance measurements: OK
# ✅ Angular resolution: OK
# ✅ Update rate: 5.5Hz
```

### **3. Performance Test**
```bash
# Test LiDAR performance
python3 test_lidar_performance.py

# Expected output:
# ✅ Accuracy: ±2cm
# ✅ Range: 0.15m - 12m
# ✅ Latency: < 100ms
# ✅ Reliability: 99.9%
```

---

## 🚨 **Troubleshooting**

### **Common Issues**

#### **1. USB Permission Denied**
```bash
# Solution: Fix USB permissions
sudo chmod 666 /dev/ttyUSB0
sudo usermod -a -G dialout $USER
# Reboot required
```

#### **2. Device Not Found**
```bash
# Solution: Check USB connection
lsusb | grep -i lidar
dmesg | grep -i usb
# Check cable and power
```

#### **3. Communication Error**
```bash
# Solution: Check baudrate and port
python3 -c "import serial; print(serial.tools.list_ports.comports())"
# Verify correct port and baudrate
```

#### **4. Power Issues**
```bash
# Solution: Check power supply
sudo dmesg | grep -i usb
# Ensure adequate power supply
```

---

## 📋 **Installation Checklist**

### **Hardware Setup**
- [ ] LiDAR sensor received and inspected
- [ ] USB cable connected properly
- [ ] Power supply adequate (5V/500mA)
- [ ] Physical mounting completed
- [ ] Cable routing protected

### **Software Setup**
- [ ] Driver dependencies installed
- [ ] USB permissions configured
- [ ] udev rules created
- [ ] Python libraries installed
- [ ] Configuration files created

### **Testing**
- [ ] Basic connection test passed
- [ ] Functionality test passed
- [ ] Performance test passed
- [ ] Integration test passed
- [ ] Documentation completed

---

## 📚 **References**

### **Technical Documentation**
- RPLIDAR A1/A2 User Manual
- Orange Pi 5B USB Specifications
- Python RPLIDAR Library Documentation

### **Related Documents**
- `docs/specs/hardware.md` - Hardware specifications
- `docs/specs/navigation.md` - Navigation system
- `EMBED/lidar_driver.py` - LiDAR driver implementation

---

**Changelog:**
- v1.0 (2025-01-27): Initial version with USB LiDAR wiring guide
