# LiDAR Test Procedures - OHT-50 (v2.0)

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated để tích hợp với Dock & Location module qua USB

---

## 🎯 **MỤC TIÊU**

Quy trình kiểm thử LiDAR RPLIDAR A1M8 qua USB để tích hợp với Dock & Location module cho hệ thống OHT-50.

---

## 🧪 **TEST CATEGORIES**

### **Test Categories:**
```
1. Hardware Tests
   - USB Connection Test
   - Power Supply Test
   - Mechanical Mounting Test
   - Environmental Test

2. Software Tests
   - Driver Installation Test
   - USB Communication Test
   - SDK Integration Test
   - Performance Test

3. Functional Tests
   - Mapping Test
   - Localization Test
   - Obstacle Detection Test
   - Path Planning Test

4. Integration Tests
   - Dock & Location Module Integration
   - Multi-Sensor Fusion Test
   - Safety Integration Test
   - System Performance Test
```

---

## 🔧 **HARDWARE TESTS**

### **1. USB Connection Test**
```
Test ID: HW-USB-001
Objective: Verify USB connection và device detection
Equipment: RPLIDAR A1M8, USB cable, Orange Pi 5B

Procedure:
1. Connect LiDAR to Orange Pi 5B via USB
2. Check device detection: lsusb | grep RPLIDAR
3. Verify device path: ls -la /dev/ttyUSB*
4. Test USB communication: sudo stty -F /dev/ttyUSB0 115200
5. Check device permissions

Expected Results:
- Device detected: RPLIDAR A1M8
- Device path: /dev/ttyUSB0 hoặc /dev/ttyACM0
- Permissions: 666
- Communication: No errors

Pass Criteria: All checks pass
```

### **2. Power Supply Test**
```
Test ID: HW-PWR-001
Objective: Verify USB power supply cho LiDAR
Equipment: RPLIDAR A1M8, USB cable, Power meter

Procedure:
1. Connect LiDAR to powered USB port
2. Measure voltage: 5V ±5%
3. Measure current: 1.5A maximum
4. Monitor temperature: < 50°C
5. Check power stability

Expected Results:
- Voltage: 4.75V - 5.25V
- Current: < 1.5A
- Temperature: < 50°C
- Power stability: ±2%

Pass Criteria: All measurements within limits
```

### **3. Mechanical Mounting Test**
```
Test ID: HW-MECH-001
Objective: Verify mounting stability và vibration isolation
Equipment: Mounted LiDAR, Vibration sensor

Procedure:
1. Check mounting height: 300-500mm
2. Verify horizontal orientation
3. Test vibration isolation
4. Check clearance: ≥100mm
5. Verify protection cover

Expected Results:
- Height: 300-500mm
- Orientation: Horizontal ±2°
- Vibration: < 0.5g RMS
- Clearance: ≥100mm
- Protection: Dust/moisture resistant

Pass Criteria: All mechanical requirements met
```

---

## 💻 **SOFTWARE TESTS**

### **1. Driver Installation Test**
```
Test ID: SW-DRV-001
Objective: Verify RPLIDAR SDK installation
Equipment: Orange Pi 5B, RPLIDAR SDK

Procedure:
1. Install RPLIDAR SDK
2. Compile sample programs
3. Test driver functionality
4. Verify library linking
5. Check error handling

Expected Results:
- SDK installed successfully
- Sample programs compile
- Driver functions correctly
- Libraries linked properly
- Error handling works

Pass Criteria: All software components work
```

### **2. USB Communication Test**
```
Test ID: SW-USB-001
Objective: Verify USB communication với LiDAR
Equipment: RPLIDAR A1M8, USB driver

Procedure:
1. Initialize USB connection
2. Test data transfer
3. Verify scan data format
4. Check communication speed
5. Test error recovery

Expected Results:
- Connection established
- Data transfer successful
- Scan data format correct
- Speed: 115200 bps
- Error recovery works

Pass Criteria: USB communication stable
```

### **3. SDK Integration Test**
```
Test ID: SW-SDK-001
Objective: Verify SDK integration với application
Equipment: RPLIDAR SDK, Test application

Procedure:
1. Initialize SDK
2. Start LiDAR scanning
3. Process scan data
4. Test data structures
5. Verify API functions

Expected Results:
- SDK initializes correctly
- Scanning starts successfully
- Data processing works
- Structures defined correctly
- API functions work

Pass Criteria: SDK integration complete
```

---

## 🔍 **FUNCTIONAL TESTS**

### **1. Mapping Test**
```
Test ID: FUN-MAP-001
Objective: Verify map generation functionality
Equipment: LiDAR, Mapping software

Procedure:
1. Start SLAM algorithm
2. Generate map of test area
3. Verify map accuracy
4. Test map storage
5. Check map loading

Expected Results:
- Map generated successfully
- Accuracy: ±5cm
- Storage: Binary format
- Loading: Fast và reliable
- Resolution: 50mm/cell

Pass Criteria: Mapping functionality works
```

### **2. Localization Test**
```
Test ID: FUN-LOC-001
Objective: Verify position estimation
Equipment: LiDAR, Localization software

Procedure:
1. Initialize localization
2. Estimate position
3. Verify accuracy
4. Test stability
5. Check confidence

Expected Results:
- Position estimated correctly
- Accuracy: ±5cm position, ±2° orientation
- Stability: Consistent results
- Confidence: > 80%
- Update rate: 10Hz

Pass Criteria: Localization accurate và stable
```

### **3. Obstacle Detection Test**
```
Test ID: FUN-OBS-001
Objective: Verify obstacle detection
Equipment: LiDAR, Test obstacles

Procedure:
1. Place test obstacles
2. Detect obstacles
3. Measure distances
4. Verify accuracy
5. Test response time

Expected Results:
- Obstacles detected correctly
- Distance accuracy: ±2cm
- Response time: < 100ms
- Range: 0.15m - 12m
- False positives: < 1%

Pass Criteria: Obstacle detection reliable
```

---

## 🔗 **INTEGRATION TESTS**

### **1. Dock & Location Module Integration**
```
Test ID: INT-DOCK-001
Objective: Verify integration với Dock & Location module
Equipment: LiDAR, Dock & Location module

Procedure:
1. Connect LiDAR to Dock & Location module
2. Test data flow
3. Verify register mapping
4. Check synchronization
5. Test error handling

Expected Results:
- Data flow established
- Registers mapped correctly
- Synchronization working
- Error handling functional
- Performance acceptable

Pass Criteria: Integration successful
```

### **2. Multi-Sensor Fusion Test**
```
Test ID: INT-FUSION-001
Objective: Verify multi-sensor fusion
Equipment: LiDAR, IMU, Magnetic sensors

Procedure:
1. Collect data from all sensors
2. Fuse sensor data
3. Verify fusion algorithm
4. Test accuracy improvement
5. Check robustness

Expected Results:
- Data fusion successful
- Accuracy improved
- Algorithm stable
- Robust to sensor failures
- Performance enhanced

Pass Criteria: Multi-sensor fusion works
```

### **3. Safety Integration Test**
```
Test ID: INT-SAFETY-001
Objective: Verify safety integration
Equipment: LiDAR, Safety system

Procedure:
1. Test emergency stop
2. Verify speed control
3. Test path replanning
4. Check zone monitoring
5. Verify collision prevention

Expected Results:
- Emergency stop: < 10ms
- Speed control: Responsive
- Path replanning: Automatic
- Zone monitoring: Active
- Collision prevention: Effective

Pass Criteria: Safety integration functional
```

---

## 📊 **PERFORMANCE TESTS**

### **1. Scan Performance Test**
```
Test ID: PERF-SCAN-001
Objective: Verify scan performance
Equipment: LiDAR, Performance monitor

Procedure:
1. Monitor scan frequency
2. Check data quality
3. Test update rate
4. Verify bandwidth usage
5. Check CPU usage

Expected Results:
- Scan frequency: 5.5Hz
- Data quality: > 95%
- Update rate: 5Hz
- Bandwidth: < 10% USB 2.0
- CPU usage: < 20%

Pass Criteria: Performance within limits
```

### **2. Memory Usage Test**
```
Test ID: PERF-MEM-001
Objective: Verify memory usage
Equipment: LiDAR, Memory monitor

Procedure:
1. Monitor memory usage
2. Check memory leaks
3. Test map storage
4. Verify buffer management
5. Check garbage collection

Expected Results:
- Memory usage: < 1MB
- No memory leaks
- Map storage: Efficient
- Buffer management: Proper
- Garbage collection: Working

Pass Criteria: Memory usage acceptable
```

---

## 🔧 **TROUBLESHOOTING TESTS**

### **1. Error Recovery Test**
```
Test ID: TROUB-001
Objective: Verify error recovery
Equipment: LiDAR, Error simulator

Procedure:
1. Simulate USB disconnection
2. Test reconnection
3. Simulate data corruption
4. Test error correction
5. Verify system recovery

Expected Results:
- Reconnection: Automatic
- Error correction: Working
- System recovery: Fast
- Data integrity: Maintained
- Stability: Preserved

Pass Criteria: Error recovery robust
```

### **2. Stress Test**
```
Test ID: TROUB-002
Objective: Verify system under stress
Equipment: LiDAR, Stress test software

Procedure:
1. High data rate test
2. Continuous operation test
3. Temperature stress test
4. Vibration stress test
5. Power fluctuation test

Expected Results:
- High data rate: Stable
- Continuous operation: Reliable
- Temperature: Within limits
- Vibration: Tolerant
- Power: Stable

Pass Criteria: System stress tolerant
```

---

## 📋 **TEST REPORTING**

### **Test Report Format:**
```
Test Report Template:
1. Test Information
   - Test ID
   - Test Name
   - Test Date
   - Tester Name
   - Equipment Used

2. Test Results
   - Pass/Fail Status
   - Measured Values
   - Expected Values
   - Deviations
   - Comments

3. Issues Found
   - Issue Description
   - Severity Level
   - Root Cause
   - Corrective Action
   - Verification

4. Recommendations
   - Improvement Suggestions
   - Additional Tests
   - Configuration Changes
   - Documentation Updates
```

### **Test Metrics:**
```
Performance Metrics:
- Test Coverage: > 95%
- Pass Rate: > 90%
- Issue Resolution: < 24 hours
- Documentation: 100% updated
- Training: All team members
```

---

**Changelog:**
- v2.0 (2025-01-27): Updated để tích hợp với Dock & Location module qua USB
- v1.0 (2025-01-27): Initial LiDAR test procedures

**Status:** Updated để tích hợp với Dock & Location module qua USB  
**Next Steps:** Implementation theo test procedures
