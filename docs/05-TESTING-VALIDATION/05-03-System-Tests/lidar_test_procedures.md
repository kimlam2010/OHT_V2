---
title: "Lidar Test Procedures"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "QA Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['testing-validation', 'lidar', 'sensors', 'testing', 'validation']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# 🧪 LiDAR Test Procedures - OHT-50

**Version:** 1.0.0  
**Date:** 2025-01-27  
**Team:** EMBED  
**Task:** EM-12 (LiDAR Driver & USB Integration)

---

## 📋 **Test Overview**

### **Test Objectives:**
- Verify LiDAR hardware functionality
- Validate data accuracy và reliability
- Test safety integration
- Ensure performance requirements
- Validate USB integration

### **Test Categories:**
1. **Hardware Tests:** Physical connectivity, power, mounting
2. **Software Tests:** Driver, SDK, communication
3. **Performance Tests:** Accuracy, speed, reliability
4. **Safety Tests:** E-Stop integration, threshold validation
5. **Integration Tests:** HAL interface, system integration

---

## 🔧 **Hardware Tests**

### **Test H-01: Physical Connectivity**

#### **Objective:** Verify USB connection và power supply
#### **Equipment:** RPLIDAR A1M8, USB cable, Orange Pi 5B
#### **Duration:** 5 minutes

#### **Procedure:**
```bash
# 1. Check USB device detection
lsusb | grep 10c4
# Expected: Bus XXX Device XXX: ID 10c4:ea60 Silicon Labs CP210x UART Bridge

# 2. Check serial device creation
ls -la /dev/ttyUSB*
# Expected: /dev/ttyUSB0 exists

# 3. Check device permissions
ls -la /dev/ttyUSB0
# Expected: crw-rw-rw- 1 root dialout 188, 0

# 4. Check power consumption
sudo cat /sys/bus/usb/devices/*/power_consumption
# Expected: ~500mA peak
```

#### **Pass Criteria:**
- ✅ USB device detected
- ✅ Serial device created
- ✅ Proper permissions
- ✅ Power consumption within limits

#### **Fail Criteria:**
- ❌ Device not detected
- ❌ Permission issues
- ❌ Power consumption excessive

---

### **Test H-02: Mounting Verification**

#### **Objective:** Verify LiDAR mounting position và orientation
#### **Equipment:** RPLIDAR A1M8, mounting bracket, level, tape measure
#### **Duration:** 10 minutes

#### **Procedure:**
```bash
# 1. Check mounting height
# Measure from ground to LiDAR center
# Expected: 300mm ± 10mm

# 2. Check mounting angle
# Use level to verify horizontal orientation
# Expected: 0° ± 2°

# 3. Check mounting orientation
# Verify LiDAR faces forward direction
# Expected: Forward-facing

# 4. Check mounting stability
# Gently shake mounting bracket
# Expected: No movement, secure mounting
```

#### **Pass Criteria:**
- ✅ Height within ±10mm tolerance
- ✅ Angle within ±2° tolerance
- ✅ Correct orientation
- ✅ Stable mounting

#### **Fail Criteria:**
- ❌ Height out of tolerance
- ❌ Angle out of tolerance
- ❌ Wrong orientation
- ❌ Unstable mounting

---

## 💻 **Software Tests**

### **Test S-01: Driver Installation**

#### **Objective:** Verify RPLIDAR SDK installation
#### **Equipment:** Orange Pi 5B với Ubuntu
#### **Duration:** 5 minutes

#### **Procedure:**
```bash
# 1. Check SDK library installation
ls -la /usr/local/lib/librplidar_sdk.so
# Expected: Library file exists

# 2. Check SDK headers
ls -la /usr/local/include/rplidar.h
# Expected: Header file exists

# 3. Check pkg-config
pkg-config --libs rplidar_sdk
# Expected: Library flags returned

# 4. Check udev rules
cat /etc/udev/rules.d/99-rplidar.rules
# Expected: Rules file contains 10c4:ea60
```

#### **Pass Criteria:**
- ✅ SDK library installed
- ✅ Headers available
- ✅ pkg-config working
- ✅ udev rules configured

#### **Fail Criteria:**
- ❌ Library missing
- ❌ Headers missing
- ❌ pkg-config fails
- ❌ udev rules missing

---

### **Test S-02: Basic Communication**

#### **Objective:** Verify serial communication với LiDAR
#### **Equipment:** RPLIDAR A1M8, Orange Pi 5B
#### **Duration:** 10 minutes

#### **Procedure:**
```bash
# 1. Configure serial port
sudo chmod 666 /dev/ttyUSB0
stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb

# 2. Send device info command
echo -e "\xA5\x25" > /dev/ttyUSB0

# 3. Check response (optional)
timeout 5s hexdump -C /dev/ttyUSB0

# 4. Test SDK communication
cd ~/rplidar_sdk/sdk/output/Linux/Release/
timeout 10s ./ultra_simple /dev/ttyUSB0 > /tmp/lidar_test.log 2>&1

# 5. Check for data
if [ -s /tmp/lidar_test.log ]; then
    echo "✅ Data received"
    head -5 /tmp/lidar_test.log
else
    echo "❌ No data received"
fi
```

#### **Pass Criteria:**
- ✅ Serial port configured
- ✅ Command sent successfully
- ✅ SDK communication working
- ✅ Data received

#### **Fail Criteria:**
- ❌ Serial configuration fails
- ❌ Command not sent
- ❌ SDK communication fails
- ❌ No data received

---

## 📊 **Performance Tests**

### **Test P-01: Scan Rate Verification**

#### **Objective:** Verify LiDAR scan rate meets specifications
#### **Equipment:** RPLIDAR A1M8, Orange Pi 5B, stopwatch
#### **Duration:** 15 minutes

#### **Procedure:**
```bash
# 1. Start continuous scanning
cd ~/rplidar_sdk/sdk/output/Linux/Release/
./ultra_simple /dev/ttyUSB0 > /tmp/scan_test.log 2>&1 &

# 2. Monitor for 60 seconds
sleep 60

# 3. Stop scanning
pkill ultra_simple

# 4. Count scan cycles
grep -c "scan" /tmp/scan_test.log
# Expected: ~480 scans (8Hz * 60s)

# 5. Calculate actual scan rate
SCAN_COUNT=$(grep -c "scan" /tmp/scan_test.log)
ACTUAL_RATE=$(echo "scale=2; $SCAN_COUNT / 60" | bc)
echo "Actual scan rate: $ACTUAL_RATE Hz"
```

#### **Pass Criteria:**
- ✅ Scan rate ≥ 7.5 Hz (within 6.25% tolerance)
- ✅ Consistent scan rate
- ✅ No missing scans

#### **Fail Criteria:**
- ❌ Scan rate < 7.5 Hz
- ❌ Inconsistent scan rate
- ❌ Missing scans

---

### **Test P-02: Distance Accuracy**

#### **Objective:** Verify distance measurement accuracy
#### **Equipment:** RPLIDAR A1M8, calibration targets, tape measure
#### **Duration:** 20 minutes

#### **Procedure:**
```bash
# 1. Setup calibration targets
# Place targets at known distances: 0.5m, 1.0m, 2.0m, 5.0m

# 2. Start measurement
cd ~/rplidar_sdk/sdk/output/Linux/Release/
./ultra_simple /dev/ttyUSB0 > /tmp/accuracy_test.log 2>&1 &

# 3. Collect data for each target (30 seconds each)
sleep 30

# 4. Stop measurement
pkill ultra_simple

# 5. Analyze data for each target
# Extract measurements for each known distance
# Calculate mean và standard deviation

# 6. Calculate accuracy
for target in 0.5 1.0 2.0 5.0; do
    echo "Target distance: ${target}m"
    # Extract measurements for this target
    # Calculate accuracy
done
```

#### **Pass Criteria:**
- ✅ Accuracy within ±2cm for distances < 2m
- ✅ Accuracy within ±1% for distances ≥ 2m
- ✅ Consistent measurements

#### **Fail Criteria:**
- ❌ Accuracy worse than ±2cm
- ❌ Accuracy worse than ±1%
- ❌ Inconsistent measurements

---

### **Test P-03: Angular Resolution**

#### **Objective:** Verify angular resolution meets specifications
#### **Equipment:** RPLIDAR A1M8, calibration targets
#### **Duration:** 15 minutes

#### **Procedure:**
```bash
# 1. Setup angular calibration
# Place targets at known angles: 0°, 90°, 180°, 270°

# 2. Start measurement
cd ~/rplidar_sdk/sdk/output/Linux/Release/
./ultra_simple /dev/ttyUSB0 > /tmp/angular_test.log 2>&1 &

# 3. Collect data
sleep 30

# 4. Stop measurement
pkill ultra_simple

# 5. Analyze angular data
# Count data points per scan
# Verify angular resolution

POINTS_PER_SCAN=$(grep "scan" /tmp/angular_test.log | head -1 | awk '{print NF-1}')
echo "Points per scan: $POINTS_PER_SCAN"
echo "Angular resolution: $(echo "scale=2; 360 / $POINTS_PER_SCAN" | bc)°"
```

#### **Pass Criteria:**
- ✅ Angular resolution ≤ 1.0°
- ✅ Consistent point count per scan
- ✅ Full 360° coverage

#### **Fail Criteria:**
- ❌ Angular resolution > 1.0°
- ❌ Inconsistent point count
- ❌ Incomplete coverage

---

## 🔒 **Safety Tests**

### **Test SF-01: E-Stop Integration**

#### **Objective:** Verify E-Stop triggers when obstacle detected
#### **Equipment:** RPLIDAR A1M8, obstacle targets, E-Stop system
#### **Duration:** 15 minutes

#### **Procedure:**
```bash
# 1. Setup E-Stop monitoring
# Monitor E-Stop system status

# 2. Test emergency stop distance (0.5m)
# Place obstacle at 0.4m (within emergency stop distance)
# Expected: E-Stop triggers immediately

# 3. Test warning distance (1.0m)
# Place obstacle at 0.8m (within warning distance)
# Expected: Warning alert, no E-Stop

# 4. Test safe distance (2.0m)
# Place obstacle at 2.5m (beyond safe distance)
# Expected: No action

# 5. Test dynamic obstacle
# Move obstacle from 2.0m to 0.4m
# Expected: Warning at 1.0m, E-Stop at 0.5m
```

#### **Pass Criteria:**
- ✅ E-Stop triggers at < 0.5m
- ✅ Warning alerts at < 1.0m
- ✅ No action at > 2.0m
- ✅ Proper response time (< 100ms)

#### **Fail Criteria:**
- ❌ E-Stop doesn't trigger
- ❌ Warning doesn't alert
- ❌ False triggers
- ❌ Slow response time

---

### **Test SF-02: Threshold Validation**

#### **Objective:** Verify safety thresholds are correctly implemented
#### **Equipment:** RPLIDAR A1M8, precision targets
#### **Duration:** 20 minutes

#### **Procedure:**
```bash
# 1. Test emergency stop threshold
# Place target at exactly 0.5m
# Expected: E-Stop triggers

# 2. Test warning threshold
# Place target at exactly 1.0m
# Expected: Warning alerts

# 3. Test safe threshold
# Place target at exactly 2.0m
# Expected: No action

# 4. Test boundary conditions
# Test at 0.49m, 0.51m, 0.99m, 1.01m, 1.99m, 2.01m
# Verify correct behavior at boundaries
```

#### **Pass Criteria:**
- ✅ Correct behavior at all thresholds
- ✅ Proper boundary handling
- ✅ Consistent threshold detection

#### **Fail Criteria:**
- ❌ Incorrect threshold behavior
- ❌ Inconsistent boundary handling
- ❌ Threshold detection failures

---

## 🔗 **Integration Tests**

### **Test I-01: HAL Interface**

#### **Objective:** Verify HAL LiDAR interface functionality
#### **Equipment:** RPLIDAR A1M8, Orange Pi 5B, HAL driver
#### **Duration:** 15 minutes

#### **Procedure:**
```bash
# 1. Test HAL initialization
# Call hal_lidar_init()
# Expected: Success return code

# 2. Test HAL configuration
# Call hal_lidar_configure()
# Expected: Configuration applied

# 3. Test HAL data acquisition
# Call hal_lidar_get_scan_data()
# Expected: Valid scan data returned

# 4. Test HAL safety functions
# Call hal_lidar_check_safety()
# Expected: Safety status returned

# 5. Test HAL cleanup
# Call hal_lidar_deinit()
# Expected: Clean shutdown
```

#### **Pass Criteria:**
- ✅ HAL initialization successful
- ✅ Configuration applied correctly
- ✅ Data acquisition working
- ✅ Safety functions working
- ✅ Clean shutdown

#### **Fail Criteria:**
- ❌ HAL initialization fails
- ❌ Configuration not applied
- ❌ Data acquisition fails
- ❌ Safety functions fail
- ❌ Shutdown issues

---

### **Test I-02: System Integration**

#### **Objective:** Verify LiDAR integration với overall system
#### **Equipment:** Complete OHT-50 system
#### **Duration:** 30 minutes

#### **Procedure:**
```bash
# 1. Test system startup
# Power on complete system
# Expected: LiDAR initializes correctly

# 2. Test normal operation
# Run system in normal mode
# Expected: LiDAR monitoring active

# 3. Test safety integration
# Trigger safety scenarios
# Expected: Proper system response

# 4. Test communication
# Verify LiDAR data flows to other components
# Expected: Data accessible to all components

# 5. Test shutdown
# Power off system
# Expected: Clean shutdown
```

#### **Pass Criteria:**
- ✅ System startup successful
- ✅ Normal operation stable
- ✅ Safety integration working
- ✅ Communication established
- ✅ Clean shutdown

#### **Fail Criteria:**
- ❌ Startup issues
- ❌ Operation unstable
- ❌ Safety integration fails
- ❌ Communication issues
- ❌ Shutdown problems

---

## 📋 **Test Execution Checklist**

### **Pre-Test Setup:**
- [ ] Hardware mounted correctly
- [ ] Software installed
- [ ] udev rules configured
- [ ] Test environment prepared
- [ ] Safety equipment ready

### **Test Execution:**
- [ ] Hardware tests (H-01, H-02)
- [ ] Software tests (S-01, S-02)
- [ ] Performance tests (P-01, P-02, P-03)
- [ ] Safety tests (SF-01, SF-02)
- [ ] Integration tests (I-01, I-02)

### **Post-Test:**
- [ ] Test results documented
- [ ] Issues logged
- [ ] Performance metrics recorded
- [ ] Recommendations made

---

## 📊 **Test Results Template**

### **Test Report:**
```
Test Date: _______________
Test Engineer: _______________
Equipment: RPLIDAR A1M8, Orange Pi 5B

Hardware Tests:
- H-01: Physical Connectivity [PASS/FAIL]
- H-02: Mounting Verification [PASS/FAIL]

Software Tests:
- S-01: Driver Installation [PASS/FAIL]
- S-02: Basic Communication [PASS/FAIL]

Performance Tests:
- P-01: Scan Rate: ____ Hz [PASS/FAIL]
- P-02: Distance Accuracy: ±____ cm [PASS/FAIL]
- P-03: Angular Resolution: ____° [PASS/FAIL]

Safety Tests:
- SF-01: E-Stop Integration [PASS/FAIL]
- SF-02: Threshold Validation [PASS/FAIL]

Integration Tests:
- I-01: HAL Interface [PASS/FAIL]
- I-02: System Integration [PASS/FAIL]

Overall Result: [PASS/FAIL]
Issues Found: _______________
Recommendations: _______________
```

---

## 🚨 **Troubleshooting Guide**

### **Common Test Failures:**

#### **Hardware Issues:**
- **Device not detected:** Check USB cable, power supply
- **Permission denied:** Check udev rules, user permissions
- **Mounting problems:** Verify mounting bracket, orientation

#### **Software Issues:**
- **SDK not found:** Reinstall RPLIDAR SDK
- **Communication fails:** Check baud rate, cable connection
- **Driver issues:** Update kernel, check compatibility

#### **Performance Issues:**
- **Low scan rate:** Check USB bandwidth, system load
- **Poor accuracy:** Calibrate sensor, check mounting
- **Inconsistent data:** Check for interference, vibration

#### **Safety Issues:**
- **E-Stop not triggering:** Check threshold settings, integration
- **False triggers:** Adjust sensitivity, check environment
- **Slow response:** Optimize processing, check system load

---

**🎯 Test Procedures Complete!**
Tất cả test procedures đã được định nghĩa. Sẵn sàng cho bước tiếp theo: integration guide và HAL driver implementation.
