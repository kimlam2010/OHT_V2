# LiDAR Test Procedures - OHT-50

## 📋 **Tổng quan**
Quy trình test và validation LiDAR cho hệ thống OHT-50.

**Ngày tạo:** 2025-01-27  
**Phiên bản:** v1.0  
**Trạng thái:** Draft

---

## 🎯 **Mục tiêu**

### **Test Objectives**
- Validate LiDAR hardware functionality
- Verify software driver performance
- Test integration với navigation system
- Ensure safety và reliability requirements

### **Test Coverage**
- Hardware connection và communication
- Software driver functionality
- Performance metrics validation
- Integration testing
- Safety testing

---

## 🧪 **Test Categories**

### **1. Hardware Tests**
- USB connection validation
- Power consumption testing
- Physical mounting verification
- Cable routing inspection

### **2. Software Tests**
- Driver installation verification
- Communication protocol testing
- Data processing validation
- Error handling testing

### **3. Performance Tests**
- Scan rate measurement
- Accuracy validation
- Latency testing
- Reliability assessment

### **4. Integration Tests**
- Navigation system integration
- Obstacle detection testing
- Safety system integration
- Real-time performance

---

## 📋 **Test Procedures**

### **Test 1: Hardware Connection Test**

#### **Objective**
Verify LiDAR hardware connection và basic communication.

#### **Prerequisites**
- LiDAR sensor connected to Orange Pi 5B
- USB cable properly connected
- Power supply adequate

#### **Test Steps**
```bash
# Step 1: Check USB device detection
lsusb | grep -i lidar

# Expected: Device listed with vendor/product ID
# Example: Bus 001 Device 003: ID 10c4:ea60 Silicon Labs CP210x UART Bridge

# Step 2: Check serial port availability
ls -la /dev/ttyUSB*
ls -la /dev/ttyACM*

# Expected: Device file exists with proper permissions
# Example: crw-rw-rw- 1 root dialout 188, 0 Jan 27 10:00 /dev/ttyUSB0

# Step 3: Check device permissions
sudo chmod 666 /dev/ttyUSB0
ls -la /dev/ttyUSB0

# Expected: Read/write permissions for all users
```

#### **Test Script**
```python
#!/usr/bin/env python3
"""
Hardware Connection Test
"""

import serial
import serial.tools.list_ports
import subprocess

def test_hardware_connection():
    """Test LiDAR hardware connection"""
    print("🔧 Hardware Connection Test")
    print("=" * 50)
    
    # Check USB devices
    try:
        result = subprocess.run(['lsusb'], capture_output=True, text=True)
        usb_devices = result.stdout
        
        if 'lidar' in usb_devices.lower() or '10c4:ea60' in usb_devices:
            print("✅ LiDAR USB device detected")
        else:
            print("❌ LiDAR USB device not found")
            return False
    except Exception as e:
        print(f"❌ USB check failed: {e}")
        return False
    
    # Check serial ports
    ports = serial.tools.list_ports.comports()
    lidar_port = None
    
    for port in ports:
        if 'lidar' in port.description.lower() or 'rplidar' in port.description.lower():
            lidar_port = port.device
            print(f"✅ LiDAR serial port found: {port.device}")
            break
    
    if not lidar_port:
        print("❌ LiDAR serial port not found")
        return False
    
    # Test port accessibility
    try:
        with serial.Serial(lidar_port, 115200, timeout=1) as ser:
            print("✅ Serial port accessible")
            return True
    except Exception as e:
        print(f"❌ Serial port test failed: {e}")
        return False

if __name__ == "__main__":
    success = test_hardware_connection()
    print(f"\nTest Result: {'✅ PASS' if success else '❌ FAIL'}")
```

#### **Pass Criteria**
- ✅ USB device detected
- ✅ Serial port available
- ✅ Port accessible for communication

---

### **Test 2: Driver Functionality Test**

#### **Objective**
Verify LiDAR driver installation và basic functionality.

#### **Prerequisites**
- Hardware connection test passed
- Python dependencies installed
- RPLIDAR library installed

#### **Test Steps**
```bash
# Step 1: Verify Python library installation
python3 -c "import rplidar; print('RPLIDAR library OK')"

# Step 2: Run basic functionality test
python3 test_lidar_functionality.py

# Step 3: Check device information
python3 -c "
from rplidar import RPLidar
lidar = RPLidar('/dev/ttyUSB0')
info = lidar.get_info()
print(f'Device: {info[\"model\"]}')
print(f'Firmware: {info[\"firmware\"]}')
lidar.disconnect()
"
```

#### **Test Script**
```python
#!/usr/bin/env python3
"""
Driver Functionality Test
"""

import time
from rplidar import RPLidar

def test_driver_functionality():
    """Test LiDAR driver functionality"""
    print("🔧 Driver Functionality Test")
    print("=" * 50)
    
    try:
        # Initialize LiDAR
        lidar = RPLidar('/dev/ttyUSB0')
        print("✅ LiDAR driver initialized")
        
        # Get device info
        info = lidar.get_info()
        print(f"✅ Device: {info['model']}")
        print(f"✅ Firmware: {info['firmware']}")
        print(f"✅ Hardware: {info['hardware']}")
        
        # Test motor control
        lidar.motor_speed = 200
        print("✅ Motor control OK")
        
        # Test scanning
        print("📊 Testing scan functionality...")
        scan_count = 0
        start_time = time.time()
        
        for scan in lidar.iter_scans():
            scan_count += 1
            if scan_count >= 3:  # Test 3 scans
                break
            print(f"  Scan {scan_count}: {len(scan)} points")
        
        scan_time = time.time() - start_time
        print(f"✅ Scan test completed in {scan_time:.2f}s")
        
        # Stop motor
        lidar.motor_speed = 0
        lidar.stop()
        lidar.disconnect()
        
        return True
        
    except Exception as e:
        print(f"❌ Driver test failed: {e}")
        return False

if __name__ == "__main__":
    success = test_driver_functionality()
    print(f"\nTest Result: {'✅ PASS' if success else '❌ FAIL'}")
```

#### **Pass Criteria**
- ✅ Driver initialization successful
- ✅ Device information retrieved
- ✅ Motor control functional
- ✅ Scan data received

---

### **Test 3: Performance Test**

#### **Objective**
Validate LiDAR performance metrics.

#### **Prerequisites**
- Driver functionality test passed
- Stable environment conditions

#### **Test Steps**
```bash
# Run performance test
python3 test_lidar_performance.py

# Monitor system resources during test
htop
iostat
```

#### **Test Script**
```python
#!/usr/bin/env python3
"""
Performance Test
"""

import time
import statistics
from rplidar import RPLidar

def test_performance():
    """Test LiDAR performance metrics"""
    print("🔧 Performance Test")
    print("=" * 50)
    
    try:
        lidar = RPLidar('/dev/ttyUSB0')
        
        # Performance metrics
        scan_times = []
        point_counts = []
        distances = []
        
        print("📊 Collecting performance data...")
        start_time = time.time()
        
        for i, scan in enumerate(lidar.iter_scans()):
            if i >= 30:  # Collect 30 scans
                break
                
            scan_time = time.time()
            scan_times.append(scan_time)
            point_counts.append(len(scan))
            
            # Collect distance measurements
            for angle, distance in scan:
                if distance > 0:  # Valid measurement
                    distances.append(distance)
            
            if i % 10 == 0:
                print(f"  Scan {i+1}: {len(scan)} points")
        
        total_time = time.time() - start_time
        
        # Calculate metrics
        if len(scan_times) > 1:
            scan_intervals = [scan_times[i] - scan_times[i-1] for i in range(1, len(scan_times))]
            avg_scan_interval = statistics.mean(scan_intervals)
            scan_rate = 1.0 / avg_scan_interval
        else:
            scan_rate = 0
        
        avg_points = statistics.mean(point_counts) if point_counts else 0
        avg_distance = statistics.mean(distances) if distances else 0
        
        # Performance validation
        print(f"✅ Performance Results:")
        print(f"  - Scan rate: {scan_rate:.1f} Hz")
        print(f"  - Average points per scan: {avg_points:.0f}")
        print(f"  - Average distance: {avg_distance:.2f}m")
        print(f"  - Total test time: {total_time:.1f}s")
        
        # Pass criteria
        scan_rate_ok = scan_rate >= 5.0
        points_ok = avg_points >= 100
        time_ok = total_time >= 5.0
        
        print(f"\nPass Criteria:")
        print(f"  - Scan rate ≥ 5Hz: {'✅' if scan_rate_ok else '❌'}")
        print(f"  - Points ≥ 100: {'✅' if points_ok else '❌'}")
        print(f"  - Test time ≥ 5s: {'✅' if time_ok else '❌'}")
        
        lidar.stop()
        lidar.disconnect()
        
        return scan_rate_ok and points_ok and time_ok
        
    except Exception as e:
        print(f"❌ Performance test failed: {e}")
        return False

if __name__ == "__main__":
    success = test_performance()
    print(f"\nTest Result: {'✅ PASS' if success else '❌ FAIL'}")
```

#### **Pass Criteria**
- ✅ Scan rate ≥ 5Hz
- ✅ Average points per scan ≥ 100
- ✅ Test duration ≥ 5 seconds
- ✅ Stable performance

---

### **Test 4: Accuracy Test**

#### **Objective**
Validate LiDAR measurement accuracy.

#### **Prerequisites**
- Performance test passed
- Calibrated test environment

#### **Test Setup**
```
Test Environment:
- Flat wall at known distance (1m, 2m, 5m)
- Calibrated measuring tape
- Stable mounting platform
- Controlled lighting conditions
```

#### **Test Steps**
```python
#!/usr/bin/env python3
"""
Accuracy Test
"""

import time
import statistics
from rplidar import RPLidar

def test_accuracy():
    """Test LiDAR measurement accuracy"""
    print("🔧 Accuracy Test")
    print("=" * 50)
    
    # Test distances (meters)
    test_distances = [1.0, 2.0, 5.0]
    tolerance = 0.02  # 2cm tolerance
    
    try:
        lidar = RPLidar('/dev/ttyUSB0')
        
        for target_distance in test_distances:
            print(f"\n📏 Testing distance: {target_distance}m")
            print("Place flat object at this distance and press Enter...")
            input()
            
            measurements = []
            start_time = time.time()
            
            # Collect measurements for 10 seconds
            for scan in lidar.iter_scans():
                if time.time() - start_time > 10:
                    break
                    
                # Find closest measurement (should be the wall)
                if scan:
                    min_distance = min(scan, key=lambda x: x[1])[1]
                    if min_distance > 0:
                        measurements.append(min_distance)
            
            if measurements:
                avg_measured = statistics.mean(measurements)
                error = abs(avg_measured - target_distance)
                accuracy_ok = error <= tolerance
                
                print(f"  Target: {target_distance}m")
                print(f"  Measured: {avg_measured:.3f}m")
                print(f"  Error: {error:.3f}m")
                print(f"  Accuracy: {'✅ PASS' if accuracy_ok else '❌ FAIL'}")
                
                if not accuracy_ok:
                    print(f"    Error exceeds tolerance ({tolerance}m)")
            else:
                print("  ❌ No valid measurements")
        
        lidar.stop()
        lidar.disconnect()
        
    except Exception as e:
        print(f"❌ Accuracy test failed: {e}")

if __name__ == "__main__":
    test_accuracy()
```

#### **Pass Criteria**
- ✅ Measurement error ≤ 2cm at all test distances
- ✅ Consistent measurements across multiple scans
- ✅ No systematic bias in measurements

---

### **Test 5: Integration Test**

#### **Objective**
Test LiDAR integration với navigation system.

#### **Prerequisites**
- All previous tests passed
- Navigation system available

#### **Test Script**
```python
#!/usr/bin/env python3
"""
Integration Test
"""

import time
import threading
from rplidar import RPLidar

class NavigationIntegrationTest:
    """Test LiDAR integration with navigation"""
    
    def __init__(self):
        self.lidar = None
        self.obstacles = []
        self.running = False
        
    def initialize(self):
        """Initialize LiDAR for integration test"""
        try:
            self.lidar = RPLidar('/dev/ttyUSB0')
            print("✅ LiDAR initialized for integration test")
            return True
        except Exception as e:
            print(f"❌ Integration initialization failed: {e}")
            return False
    
    def start_obstacle_detection(self):
        """Start obstacle detection"""
        self.running = True
        self.detection_thread = threading.Thread(target=self._detection_loop)
        self.detection_thread.daemon = True
        self.detection_thread.start()
        print("✅ Obstacle detection started")
    
    def _detection_loop(self):
        """Obstacle detection loop"""
        while self.running:
            try:
                for scan in self.lidar.iter_scans():
                    if not self.running:
                        break
                    
                    # Detect obstacles (distance < 1m)
                    obstacles = [(angle, distance) for angle, distance in scan if distance < 1.0]
                    self.obstacles = obstacles
                    
                    if obstacles:
                        print(f"🚨 Obstacles detected: {len(obstacles)}")
                        for angle, distance in obstacles[:3]:  # Show first 3
                            print(f"  - Angle: {angle:.1f}°, Distance: {distance:.2f}m")
                    
                    time.sleep(0.1)  # 10Hz update rate
                    
            except Exception as e:
                print(f"❌ Detection error: {e}")
                time.sleep(1)
    
    def test_navigation_integration(self):
        """Test navigation system integration"""
        print("🔧 Navigation Integration Test")
        print("=" * 50)
        
        if not self.initialize():
            return False
        
        try:
            # Start obstacle detection
            self.start_obstacle_detection()
            
            print("📊 Testing for 30 seconds...")
            print("Move objects around LiDAR to test obstacle detection")
            
            start_time = time.time()
            while time.time() - start_time < 30:
                time.sleep(1)
            
            print("✅ Integration test completed")
            return True
            
        except Exception as e:
            print(f"❌ Integration test failed: {e}")
            return False
        finally:
            self.running = False
            if self.lidar:
                self.lidar.stop()
                self.lidar.disconnect()

if __name__ == "__main__":
    test = NavigationIntegrationTest()
    success = test.test_navigation_integration()
    print(f"\nTest Result: {'✅ PASS' if success else '❌ FAIL'}")
```

#### **Pass Criteria**
- ✅ LiDAR integrates with navigation system
- ✅ Obstacle detection functional
- ✅ Real-time data processing
- ✅ No system conflicts

---

## 📊 **Test Results Template**

### **Test Report**
```
LiDAR Test Report - OHT-50
Date: [Test Date]
Tester: [Tester Name]
LiDAR Model: [Model]
Serial Number: [SN]

Test Results:
1. Hardware Connection: ✅ PASS / ❌ FAIL
2. Driver Functionality: ✅ PASS / ❌ FAIL
3. Performance: ✅ PASS / ❌ FAIL
4. Accuracy: ✅ PASS / ❌ FAIL
5. Integration: ✅ PASS / ❌ FAIL

Overall Result: ✅ PASS / ❌ FAIL

Issues Found:
- [List any issues]

Recommendations:
- [List recommendations]
```

---

## 🚨 **Troubleshooting**

### **Common Test Issues**

#### **1. Device Not Found**
- Check USB connection
- Verify power supply
- Check udev rules
- Reboot system

#### **2. Permission Errors**
- Fix USB permissions
- Add user to dialout group
- Reload udev rules

#### **3. Performance Issues**
- Check system resources
- Verify USB bandwidth
- Monitor temperature
- Check for interference

#### **4. Accuracy Problems**
- Calibrate test environment
- Check mounting stability
- Verify target surfaces
- Control lighting conditions

---

## 📋 **Test Checklist**

### **Pre-Test Setup**
- [ ] Hardware connected properly
- [ ] Software installed
- [ ] Test environment prepared
- [ ] Calibration completed
- [ ] Documentation ready

### **Test Execution**
- [ ] Hardware connection test
- [ ] Driver functionality test
- [ ] Performance test
- [ ] Accuracy test
- [ ] Integration test

### **Post-Test**
- [ ] Results documented
- [ ] Issues recorded
- [ ] Recommendations made
- [ ] Test report completed
- [ ] Team notified

---

**Changelog:**
- v1.0 (2025-01-27): Initial version with comprehensive test procedures
