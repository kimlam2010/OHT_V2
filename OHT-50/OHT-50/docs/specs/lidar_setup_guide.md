# LiDAR Setup Guide - OHT-50

## 📋 **Tổng quan**
Hướng dẫn cài đặt và cấu hình LiDAR cho hệ thống OHT-50.

**Ngày tạo:** 2025-01-27  
**Phiên bản:** v1.0  
**Trạng thái:** Draft

---

## 🎯 **Mục tiêu**

### **Setup Objectives**
- Cài đặt LiDAR driver và dependencies
- Cấu hình USB permissions và udev rules
- Test LiDAR functionality và performance
- Integration với OHT-50 navigation system

### **Success Criteria**
- LiDAR connected và recognized
- Driver installed và functional
- Test scripts passing
- Integration với navigation working

---

## 🔧 **Prerequisites**

### **Hardware Requirements**
- Orange Pi 5B với USB ports
- USB LiDAR sensor (RPLIDAR A1/A2)
- USB 2.0/3.0 cable
- Adequate power supply

### **Software Requirements**
- Linux kernel 6.1.43-rockchip-rk3588
- Python 3.10+
- USB libraries và drivers
- Development tools

---

## 📦 **Installation Steps**

### **Step 1: System Preparation**
```bash
# Update system packages
sudo apt update && sudo apt upgrade -y

# Install essential packages
sudo apt install -y python3-pip python3-dev
sudo apt install -y libusb-1.0-0-dev
sudo apt install -y build-essential cmake
sudo apt install -y git wget curl

# Install Python dependencies
pip3 install --upgrade pip
pip3 install setuptools wheel
```

### **Step 2: LiDAR Driver Installation**
```bash
# Install RPLIDAR Python library
pip3 install rplidar-roboticia

# Install additional libraries
pip3 install pyusb
pip3 install numpy matplotlib
pip3 install pyserial

# Verify installation
python3 -c "import rplidar; print('RPLIDAR library installed successfully')"
```

### **Step 3: USB Permissions Setup**
```bash
# Create udev rules directory
sudo mkdir -p /etc/udev/rules.d

# Create LiDAR udev rules
sudo tee /etc/udev/rules.d/99-lidar.rules > /dev/null <<EOF
# RPLIDAR A1/A2
SUBSYSTEM=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="5740", MODE="0666"

# Generic USB serial devices
SUBSYSTEM=="tty", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE="0666"
SUBSYSTEM=="tty", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="5740", MODE="0666"
EOF

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Add user to dialout group
sudo usermod -a -G dialout $USER
```

### **Step 4: Hardware Connection**
```bash
# Connect LiDAR to USB port
# Check device detection
lsusb | grep -i lidar

# Check serial ports
ls -la /dev/ttyUSB*
ls -la /dev/ttyACM*

# Test device permissions
sudo chmod 666 /dev/ttyUSB0 2>/dev/null || true
sudo chmod 666 /dev/ttyACM0 2>/dev/null || true
```

---

## 🧪 **Testing & Validation**

### **Test 1: Basic Connection**
```python
#!/usr/bin/env python3
"""
Test LiDAR basic connection
"""

import serial
import serial.tools.list_ports

def test_lidar_connection():
    """Test LiDAR USB connection"""
    print("🔧 Testing LiDAR Connection")
    print("=" * 50)
    
    # List available ports
    ports = serial.tools.list_ports.comports()
    print(f"Available ports: {len(ports)}")
    
    for port in ports:
        print(f"  - {port.device}: {port.description}")
        if 'lidar' in port.description.lower() or 'rplidar' in port.description.lower():
            print(f"    ✅ LiDAR device found: {port.device}")
            return port.device
    
    print("❌ No LiDAR device found")
    return None

if __name__ == "__main__":
    test_lidar_connection()
```

### **Test 2: LiDAR Functionality**
```python
#!/usr/bin/env python3
"""
Test LiDAR functionality
"""

import time
from rplidar import RPLidar

def test_lidar_functionality():
    """Test LiDAR basic functionality"""
    print("🔧 Testing LiDAR Functionality")
    print("=" * 50)
    
    try:
        # Initialize LiDAR
        lidar = RPLidar('/dev/ttyUSB0')
        print("✅ LiDAR initialized successfully")
        
        # Get device info
        info = lidar.get_info()
        print(f"✅ Device: {info['model']}")
        print(f"✅ Firmware: {info['firmware']}")
        print(f"✅ Hardware: {info['hardware']}")
        
        # Test scanning
        print("📊 Testing scan functionality...")
        for i, scan in enumerate(lidar.iter_scans()):
            if i >= 5:  # Test 5 scans
                break
            print(f"  Scan {i+1}: {len(scan)} points")
        
        print("✅ LiDAR functionality test passed")
        
    except Exception as e:
        print(f"❌ LiDAR test failed: {e}")
    finally:
        try:
            lidar.stop()
            lidar.disconnect()
        except:
            pass

if __name__ == "__main__":
    test_lidar_functionality()
```

### **Test 3: Performance Validation**
```python
#!/usr/bin/env python3
"""
Test LiDAR performance
"""

import time
import statistics
from rplidar import RPLidar

def test_lidar_performance():
    """Test LiDAR performance metrics"""
    print("🔧 Testing LiDAR Performance")
    print("=" * 50)
    
    try:
        lidar = RPLidar('/dev/ttyUSB0')
        
        # Performance metrics
        scan_times = []
        point_counts = []
        
        print("📊 Collecting performance data...")
        start_time = time.time()
        
        for i, scan in enumerate(lidar.iter_scans()):
            if i >= 20:  # Collect 20 scans
                break
                
            scan_time = time.time()
            scan_times.append(scan_time)
            point_counts.append(len(scan))
            
            if i % 5 == 0:
                print(f"  Scan {i+1}: {len(scan)} points")
        
        total_time = time.time() - start_time
        
        # Calculate metrics
        avg_scan_time = statistics.mean(scan_times[1:]) - statistics.mean(scan_times[:-1])
        avg_points = statistics.mean(point_counts)
        scan_rate = 1.0 / avg_scan_time if avg_scan_time > 0 else 0
        
        print(f"✅ Performance Results:")
        print(f"  - Average scan time: {avg_scan_time:.3f}s")
        print(f"  - Scan rate: {scan_rate:.1f} Hz")
        print(f"  - Average points per scan: {avg_points:.0f}")
        print(f"  - Total test time: {total_time:.1f}s")
        
        # Validate performance
        if scan_rate >= 5.0:
            print("✅ Scan rate meets requirements (≥5Hz)")
        else:
            print("❌ Scan rate below requirements")
            
        if avg_points >= 100:
            print("✅ Point density meets requirements (≥100 points)")
        else:
            print("❌ Point density below requirements")
        
    except Exception as e:
        print(f"❌ Performance test failed: {e}")
    finally:
        try:
            lidar.stop()
            lidar.disconnect()
        except:
            pass

if __name__ == "__main__":
    test_lidar_performance()
```

---

## ⚙️ **Configuration**

### **LiDAR Configuration File**
```python
# config/lidar_config.py
LIDAR_CONFIG = {
    # Hardware configuration
    'device_port': '/dev/ttyUSB0',
    'baudrate': 115200,
    'timeout': 1.0,
    
    # Performance settings
    'scan_frequency': 5.5,  # Hz
    'max_distance': 12.0,   # meters
    'min_distance': 0.15,   # meters
    'angle_offset': 0.0,    # degrees
    
    # System integration
    'update_rate': 10.0,    # Hz
    'obstacle_threshold': 1.0,  # meters
    'safety_zone': 0.5,     # meters
    'navigation_zone': 2.0, # meters
    'emergency_stop': 0.3,  # meters
    
    # Data processing
    'filter_noise': True,
    'smoothing_factor': 0.1,
    'outlier_threshold': 0.5,
}
```

### **System Integration**
```python
# integration/lidar_integration.py
from config.lidar_config import LIDAR_CONFIG
from rplidar import RPLidar
import threading
import time

class OHT50LiDAR:
    """LiDAR integration for OHT-50"""
    
    def __init__(self, config=LIDAR_CONFIG):
        self.config = config
        self.lidar = None
        self.running = False
        self.scan_data = []
        self.obstacles = []
        
    def initialize(self):
        """Initialize LiDAR connection"""
        try:
            self.lidar = RPLidar(self.config['device_port'])
            print("✅ LiDAR initialized successfully")
            return True
        except Exception as e:
            print(f"❌ LiDAR initialization failed: {e}")
            return False
    
    def start_scanning(self):
        """Start continuous scanning"""
        if not self.lidar:
            return False
            
        self.running = True
        self.scan_thread = threading.Thread(target=self._scan_loop)
        self.scan_thread.daemon = True
        self.scan_thread.start()
        print("✅ LiDAR scanning started")
        return True
    
    def _scan_loop(self):
        """Continuous scanning loop"""
        while self.running:
            try:
                for scan in self.lidar.iter_scans():
                    if not self.running:
                        break
                    self._process_scan(scan)
            except Exception as e:
                print(f"❌ Scan error: {e}")
                time.sleep(1)
    
    def _process_scan(self, scan):
        """Process scan data"""
        self.scan_data = scan
        self._detect_obstacles()
    
    def _detect_obstacles(self):
        """Detect obstacles in scan data"""
        obstacles = []
        for angle, distance in self.scan_data:
            if distance < self.config['obstacle_threshold']:
                obstacles.append((angle, distance))
        self.obstacles = obstacles
    
    def get_obstacles(self):
        """Get current obstacles"""
        return self.obstacles
    
    def stop(self):
        """Stop LiDAR scanning"""
        self.running = False
        if self.lidar:
            self.lidar.stop()
            self.lidar.disconnect()
        print("✅ LiDAR stopped")
```

---

## 🚨 **Troubleshooting**

### **Common Issues & Solutions**

#### **1. Device Not Found**
```bash
# Check USB connection
lsusb | grep -i lidar

# Check serial ports
ls -la /dev/ttyUSB*
ls -la /dev/ttyACM*

# Check kernel messages
dmesg | grep -i usb
dmesg | grep -i tty
```

#### **2. Permission Denied**
```bash
# Fix permissions
sudo chmod 666 /dev/ttyUSB0
sudo usermod -a -G dialout $USER

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Reboot if needed
sudo reboot
```

#### **3. Communication Error**
```bash
# Check baudrate
python3 -c "import serial; print(serial.tools.list_ports.comports())"

# Test with different ports
python3 test_lidar_connection.py
```

#### **4. Performance Issues**
```bash
# Check system resources
htop
iostat

# Check USB bandwidth
lsusb -t
```

---

## 📋 **Setup Checklist**

### **Pre-Installation**
- [ ] Hardware received và inspected
- [ ] System requirements verified
- [ ] USB ports available
- [ ] Power supply adequate

### **Installation**
- [ ] System packages updated
- [ ] Python dependencies installed
- [ ] LiDAR driver installed
- [ ] USB permissions configured
- [ ] udev rules created

### **Testing**
- [ ] Basic connection test passed
- [ ] Functionality test passed
- [ ] Performance test passed
- [ ] Integration test passed

### **Documentation**
- [ ] Setup guide completed
- [ ] Configuration documented
- [ ] Troubleshooting guide created
- [ ] Team training completed

---

## 📚 **References**

### **Technical Documentation**
- RPLIDAR A1/A2 User Manual
- Python RPLIDAR Library Documentation
- Orange Pi 5B USB Specifications

### **Related Documents**
- `docs/specs/lidar_wiring_guide.md` - Hardware wiring
- `docs/specs/navigation.md` - Navigation system
- `EMBED/lidar_driver.py` - Driver implementation

---

**Changelog:**
- v1.0 (2025-01-27): Initial version with complete LiDAR setup guide
