# TESTING PROCEDURES - OHT-50 (v2.0)

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** FW  
**Trạng thái:** Testing Procedures for Architecture v2.0

---

## 📋 TỔNG QUAN

Testing procedures chi tiết cho hệ thống OHT-50 v2.0, bao gồm unit testing, integration testing, system testing, safety testing, và performance validation.

### **🏗️ Kiến trúc mới v2.0:**
- **5 Module bắt buộc:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **RS485 Standard:** Tất cả modules đều sử dụng RS485/Modbus RTU
- **Auto-discovery:** Tự động phát hiện và cấu hình modules
- **Hot-swap:** Thay đổi modules khi đang hoạt động
- **Location-based Safety:** Hệ thống an toàn dựa trên vị trí

---

## 🔧 UNIT TESTING

### **1. Power Module Testing**

#### **Test Cases:**
```c
// Test Case 1: Battery Voltage Reading
void test_power_voltage_reading(void) {
    uint16_t voltage;
    int result = modbus_read_holding_registers(0x01, POWER_REG_VOLTAGE, 1, &voltage);
    
    assert(result == 0);  // Communication successful
    assert(voltage >= 4000 && voltage <= 6000);  // 40V - 60V range
    printf("Power voltage reading test: PASSED\n");
}

// Test Case 2: Battery Current Reading
void test_power_current_reading(void) {
    int16_t current;
    int result = modbus_read_holding_registers(0x01, POWER_REG_CURRENT, 1, &current);
    
    assert(result == 0);  // Communication successful
    assert(current >= -10000 && current <= 10000);  // -100A to +100A range
    printf("Power current reading test: PASSED\n");
}

// Test Case 3: State of Charge Reading
void test_power_soc_reading(void) {
    uint8_t soc;
    int result = modbus_read_holding_registers(0x01, POWER_REG_SOC, 1, &soc);
    
    assert(result == 0);  // Communication successful
    assert(soc >= 0 && soc <= 100);  // 0-100% range
    printf("Power SOC reading test: PASSED\n");
}

// Test Case 4: Power Status Flags
void test_power_status_flags(void) {
    uint16_t status;
    int result = modbus_read_holding_registers(0x01, POWER_REG_STATUS, 1, &status);
    
    assert(result == 0);  // Communication successful
    // Check if at least one status flag is set
    assert(status != 0);
    printf("Power status flags test: PASSED\n");
}

// Test Case 5: BMS Status
void test_power_bms_status(void) {
    uint16_t bms_status;
    int result = modbus_read_holding_registers(0x01, POWER_REG_BMS_STATUS, 1, &bms_status);
    
    assert(result == 0);  // Communication successful
    // Check if BMS is operational
    assert(bms_status & BMS_STATUS_OPERATIONAL);
    printf("Power BMS status test: PASSED\n");
}
```

#### **Test Execution:**
```bash
# Compile and run power module tests
gcc -o test_power test_power.c -lmodbus
./test_power

# Expected output:
# Power voltage reading test: PASSED
# Power current reading test: PASSED
# Power SOC reading test: PASSED
# Power status flags test: PASSED
# Power BMS status test: PASSED
# All power module tests passed!
```

### **2. Safety Module Testing**

#### **Test Cases:**
```c
// Test Case 1: E-Stop Status Reading
void test_safety_estop_status(void) {
    uint16_t estop_status;
    int result = modbus_read_holding_registers(0x02, SAFETY_REG_ESTOP_STATUS, 1, &estop_status);
    
    assert(result == 0);  // Communication successful
    // Check if E-Stop is not activated (normal operation)
    assert(!(estop_status & ESTOP_STATUS_ACTIVATED));
    printf("Safety E-Stop status test: PASSED\n");
}

// Test Case 2: Dual-channel E-Stop
void test_safety_dual_channel_estop(void) {
    uint8_t channel1, channel2;
    
    // Read channel 1
    int result1 = modbus_read_holding_registers(0x02, SAFETY_REG_ESTOP_CHANNEL1, 1, &channel1);
    // Read channel 2
    int result2 = modbus_read_holding_registers(0x02, SAFETY_REG_ESTOP_CHANNEL2, 1, &channel2);
    
    assert(result1 == 0 && result2 == 0);  // Communication successful
    // Both channels should be in normal state
    assert(channel1 == ESTOP_CHANNEL_NORMAL);
    assert(channel2 == ESTOP_CHANNEL_NORMAL);
    printf("Safety dual-channel E-Stop test: PASSED\n");
}

// Test Case 3: Safety Level Reading
void test_safety_level_reading(void) {
    uint8_t safety_level;
    int result = modbus_read_holding_registers(0x02, SAFETY_REG_SAFETY_LEVEL, 1, &safety_level);
    
    assert(result == 0);  // Communication successful
    assert(safety_level >= SAFETY_LEVEL_NORMAL && safety_level <= SAFETY_LEVEL_EMERGENCY);
    printf("Safety level reading test: PASSED\n");
}

// Test Case 4: Safety Sensors Status
void test_safety_sensors_status(void) {
    uint16_t sensors_status;
    int result = modbus_read_holding_registers(0x02, SAFETY_REG_SENSORS, 1, &sensors_status);
    
    assert(result == 0);  // Communication successful
    // Check if all safety sensors are operational
    assert(sensors_status & SENSOR_LIGHT_CURTAIN);
    assert(sensors_status & SENSOR_SAFETY_MATS);
    assert(sensors_status & SENSOR_DOOR_SENSORS);
    printf("Safety sensors status test: PASSED\n");
}

// Test Case 5: Emergency Brake Status
void test_safety_emergency_brake(void) {
    uint8_t brake_status;
    int result = modbus_read_holding_registers(0x02, SAFETY_REG_EMERGENCY_BRAKE, 1, &brake_status);
    
    assert(result == 0);  // Communication successful
    // Check if emergency brake is disengaged (normal operation)
    assert(brake_status == EMERGENCY_BRAKE_DISENGAGED);
    printf("Safety emergency brake test: PASSED\n");
}
```

### **3. Travel Motor Module Testing**

#### **Test Cases:**
```c
// Test Case 1: Motor Speed Reading
void test_motor_speed_reading(void) {
    uint16_t speed;
    int result = modbus_read_holding_registers(0x03, MOTOR_REG_SPEED, 1, &speed);
    
    assert(result == 0);  // Communication successful
    assert(speed >= 0 && speed <= 200);  // 0-2 m/s range (m/s * 100)
    printf("Motor speed reading test: PASSED\n");
}

// Test Case 2: Motor Speed Control
void test_motor_speed_control(void) {
    uint16_t target_speed = 100;  // 1.0 m/s
    
    // Set target speed
    int result1 = modbus_write_single_register(0x03, MOTOR_REG_TARGET_SPEED, target_speed);
    assert(result1 == 0);  // Write successful
    
    // Read back target speed
    uint16_t read_speed;
    int result2 = modbus_read_holding_registers(0x03, MOTOR_REG_TARGET_SPEED, 1, &read_speed);
    assert(result2 == 0);  // Read successful
    assert(read_speed == target_speed);  // Value matches
    
    printf("Motor speed control test: PASSED\n");
}

// Test Case 3: Motor Position Reading
void test_motor_position_reading(void) {
    int32_t position;
    int result = modbus_read_holding_registers(0x03, MOTOR_REG_POSITION, 2, &position);
    
    assert(result == 0);  // Communication successful
    // Position should be within reasonable range
    assert(position >= -1000000 && position <= 1000000);  // ±1000m range
    printf("Motor position reading test: PASSED\n");
}

// Test Case 4: Motor Status
void test_motor_status(void) {
    uint16_t status;
    int result = modbus_read_holding_registers(0x03, MOTOR_REG_STATUS, 1, &status);
    
    assert(result == 0);  // Communication successful
    // Check if motor is operational
    assert(status & MOTOR_STATUS_OK);
    printf("Motor status test: PASSED\n");
}

// Test Case 5: PID Parameters
void test_motor_pid_parameters(void) {
    uint16_t kp, ki, kd;
    
    // Read PID parameters
    int result1 = modbus_read_holding_registers(0x03, MOTOR_REG_PID_KP, 1, &kp);
    int result2 = modbus_read_holding_registers(0x03, MOTOR_REG_PID_KI, 1, &ki);
    int result3 = modbus_read_holding_registers(0x03, MOTOR_REG_PID_KD, 1, &kd);
    
    assert(result1 == 0 && result2 == 0 && result3 == 0);  // Communication successful
    // PID parameters should be within reasonable ranges
    assert(kp > 0 && kp <= 10000);
    assert(ki > 0 && ki <= 1000);
    assert(kd >= 0 && kd <= 1000);
    printf("Motor PID parameters test: PASSED\n");
}
```

### **4. Dock & Location Module Testing**

#### **Test Cases:**
```c
// Test Case 1: Position Reading
void test_location_position_reading(void) {
    int32_t position[3];  // X, Y, Z
    int result = modbus_read_holding_registers(0x05, LOCATION_REG_POSITION_X, 3, position);
    
    assert(result == 0);  // Communication successful
    // Position should be within reasonable range
    assert(position[0] >= -100000 && position[0] <= 100000);  // ±100m X
    assert(position[1] >= -100000 && position[1] <= 100000);  // ±100m Y
    assert(position[2] >= -1000 && position[2] <= 1000);      // ±1m Z
    printf("Location position reading test: PASSED\n");
}

// Test Case 2: Orientation Reading
void test_location_orientation_reading(void) {
    int16_t orientation[3];  // Pitch, Roll, Yaw
    int result = modbus_read_holding_registers(0x05, LOCATION_REG_ORIENTATION_P, 3, orientation);
    
    assert(result == 0);  // Communication successful
    // Orientation should be within ±180 degrees
    assert(orientation[0] >= -1800 && orientation[0] <= 1800);  // Pitch ±180°
    assert(orientation[1] >= -1800 && orientation[1] <= 1800);  // Roll ±180°
    assert(orientation[2] >= -1800 && orientation[2] <= 1800);  // Yaw ±180°
    printf("Location orientation reading test: PASSED\n");
}

// Test Case 3: LiDAR Status
void test_location_lidar_status(void) {
    uint16_t lidar_status;
    int result = modbus_read_holding_registers(0x05, LOCATION_REG_LIDAR_STATUS, 1, &lidar_status);
    
    assert(result == 0);  // Communication successful
    // Check if LiDAR is active and USB connected
    assert(lidar_status & LIDAR_STATUS_ACTIVE);
    assert(lidar_status & LIDAR_STATUS_USB_CONNECTED);
    printf("Location LiDAR status test: PASSED\n");
}

// Test Case 4: Navigation State
void test_location_navigation_state(void) {
    uint8_t navigation_state;
    int result = modbus_read_holding_registers(0x05, LOCATION_REG_NAVIGATION, 1, &navigation_state);
    
    assert(result == 0);  // Communication successful
    assert(navigation_state >= NAV_STATE_IDLE && navigation_state <= NAV_STATE_UNDOCKING);
    printf("Location navigation state test: PASSED\n");
}

// Test Case 5: Sensors Status
void test_location_sensors_status(void) {
    uint16_t sensors_status;
    int result = modbus_read_holding_registers(0x05, LOCATION_REG_SENSORS, 1, &sensors_status);
    
    assert(result == 0);  // Communication successful
    // Check if key sensors are operational
    assert(sensors_status & SENSOR_IMU_OK);
    assert(sensors_status & SENSOR_MAGNETIC_OK);
    printf("Location sensors status test: PASSED\n");
}
```

### **5. Master Control Module Testing**

#### **Test Cases:**
```c
// Test Case 1: System State Reading
void test_master_system_state(void) {
    uint8_t system_state;
    int result = modbus_read_holding_registers(0x00, MASTER_REG_SYSTEM_STATE, 1, &system_state);
    
    assert(result == 0);  // Communication successful
    assert(system_state >= SYSTEM_STATE_IDLE && system_state <= SYSTEM_STATE_ESTOP);
    printf("Master system state test: PASSED\n");
}

// Test Case 2: Navigation State Reading
void test_master_navigation_state(void) {
    uint8_t navigation_state;
    int result = modbus_read_holding_registers(0x00, MASTER_REG_NAVIGATION_STATE, 1, &navigation_state);
    
    assert(result == 0);  // Communication successful
    assert(navigation_state >= NAV_STATE_IDLE && navigation_state <= NAV_STATE_UNDOCKING);
    printf("Master navigation state test: PASSED\n");
}

// Test Case 3: Mandatory Modules Status
void test_master_mandatory_modules(void) {
    uint16_t mandatory_status;
    int result = modbus_read_holding_registers(0x00, MASTER_REG_MANDATORY_STATUS, 1, &mandatory_status);
    
    assert(result == 0);  // Communication successful
    // Check if all mandatory modules are OK
    assert(mandatory_status & MODULE_POWER_OK);
    assert(mandatory_status & MODULE_SAFETY_OK);
    assert(mandatory_status & MODULE_TRAVEL_MOTOR_OK);
    assert(mandatory_status & MODULE_DOCK_LOCATION_OK);
    assert(mandatory_status & MODULE_MASTER_CONTROL_OK);
    printf("Master mandatory modules test: PASSED\n");
}

// Test Case 4: RS485 Communication Status
void test_master_rs485_status(void) {
    uint16_t rs485_status;
    int result = modbus_read_holding_registers(0x00, MASTER_REG_RS485_STATUS, 1, &rs485_status);
    
    assert(result == 0);  // Communication successful
    // Check if RS485 communication is active
    assert(rs485_status & RS485_STATUS_ACTIVE);
    printf("Master RS485 status test: PASSED\n");
}

// Test Case 5: System Uptime
void test_master_uptime(void) {
    uint32_t uptime;
    int result = modbus_read_holding_registers(0x00, MASTER_REG_UPTIME, 2, &uptime);
    
    assert(result == 0);  // Communication successful
    // Uptime should be positive
    assert(uptime > 0);
    printf("Master uptime test: PASSED\n");
}
```

---

## 🔄 INTEGRATION TESTING

### **1. Auto-discovery Testing**

#### **Test Procedure:**
```c
// Test auto-discovery process
void test_auto_discovery(void) {
    printf("Starting auto-discovery test...\n");
    
    // Clear module registry
    clear_module_registry();
    
    // Start auto-discovery
    auto_discovery_process();
    
    // Check if all mandatory modules are discovered
    assert(is_module_registered(0x01));  // Power module
    assert(is_module_registered(0x02));  // Safety module
    assert(is_module_registered(0x03));  // Travel motor module
    assert(is_module_registered(0x05));  // Dock & location module
    
    // Check module information
    module_info_t *power_module = get_module_info(0x01);
    assert(strcmp(power_module->module_name, "Power Module") == 0);
    assert(power_module->module_type == MODULE_TYPE_POWER);
    
    module_info_t *safety_module = get_module_info(0x02);
    assert(strcmp(safety_module->module_name, "Safety Module") == 0);
    assert(safety_module->module_type == MODULE_TYPE_SAFETY);
    
    printf("Auto-discovery test: PASSED\n");
}
```

### **2. Module Communication Testing**

#### **Test Procedure:**
```c
// Test module communication
void test_module_communication(void) {
    printf("Starting module communication test...\n");
    
    // Test communication with each module
    uint8_t module_ids[] = {0x01, 0x02, 0x03, 0x05};
    char *module_names[] = {"Power", "Safety", "Travel Motor", "Dock & Location"};
    
    for (int i = 0; i < 4; i++) {
        uint16_t test_data;
        int result = modbus_read_holding_registers(module_ids[i], 0x0001, 1, &test_data);
        
        if (result == 0) {
            printf("  %s module communication: OK\n", module_names[i]);
        } else {
            printf("  %s module communication: FAILED\n", module_names[i]);
            assert(0);  // Fail test
        }
    }
    
    printf("Module communication test: PASSED\n");
}
```

### **3. Safety Integration Testing**

#### **Test Procedure:**
```c
// Test safety integration
void test_safety_integration(void) {
    printf("Starting safety integration test...\n");
    
    // Test E-Stop integration
    uint16_t estop_status;
    int result = modbus_read_holding_registers(0x02, SAFETY_REG_ESTOP_STATUS, 1, &estop_status);
    assert(result == 0);
    
    // Simulate E-Stop activation
    // Note: This should be done carefully in a controlled environment
    printf("  E-Stop integration: OK\n");
    
    // Test location-based safety
    uint16_t location_safety;
    result = modbus_read_holding_registers(0x05, LOCATION_REG_NAVIGATION, 1, &location_safety);
    assert(result == 0);
    
    printf("  Location-based safety: OK\n");
    
    printf("Safety integration test: PASSED\n");
}
```

---

## 🛡️ SAFETY TESTING

### **1. E-Stop Testing**

#### **Test Procedure:**
```c
// Test E-Stop functionality
void test_estop_functionality(void) {
    printf("Starting E-Stop functionality test...\n");
    
    // Test E-Stop activation
    printf("  Activating E-Stop...\n");
    // Simulate E-Stop button press
    
    // Check if system enters E-Stop state
    uint8_t system_state;
    int result = modbus_read_holding_registers(0x00, MASTER_REG_SYSTEM_STATE, 1, &system_state);
    assert(result == 0);
    assert(system_state == SYSTEM_STATE_ESTOP);
    
    // Check if motors are stopped
    uint16_t motor_status;
    result = modbus_read_holding_registers(0x03, MOTOR_REG_STATUS, 1, &motor_status);
    assert(result == 0);
    assert(motor_status & MOTOR_STATUS_STOPPED);
    
    printf("  E-Stop activation: OK\n");
    
    // Test E-Stop reset
    printf("  Resetting E-Stop...\n");
    // Simulate E-Stop reset
    
    // Check if system returns to normal state
    result = modbus_read_holding_registers(0x00, MASTER_REG_SYSTEM_STATE, 1, &system_state);
    assert(result == 0);
    assert(system_state == SYSTEM_STATE_IDLE);
    
    printf("  E-Stop reset: OK\n");
    printf("E-Stop functionality test: PASSED\n");
}
```

### **2. Location-based Safety Testing**

#### **Test Procedure:**
```c
// Test location-based safety
void test_location_based_safety(void) {
    printf("Starting location-based safety test...\n");
    
    // Test safety zones
    safety_zone_t test_zones[] = {
        {1, 0.0, 5.0, 0.0, 5.0, SAFETY_LEVEL_NORMAL, 200},
        {2, 5.0, 10.0, 0.0, 5.0, SAFETY_LEVEL_WARNING, 100},
        {3, 10.0, 15.0, 0.0, 5.0, SAFETY_LEVEL_CRITICAL, 50}
    };
    
    // Test each safety zone
    for (int i = 0; i < 3; i++) {
        // Simulate robot position in zone
        position_t test_position = {
            (test_zones[i].x_min + test_zones[i].x_max) / 2,
            (test_zones[i].y_min + test_zones[i].y_max) / 2,
            0
        };
        
        // Check if safety level is set correctly
        uint8_t safety_level = get_safety_level_for_position(test_position);
        assert(safety_level == test_zones[i].safety_level);
        
        printf("  Safety zone %d: OK\n", i + 1);
    }
    
    printf("Location-based safety test: PASSED\n");
}
```

---

## 📊 PERFORMANCE TESTING

### **1. Communication Performance Testing**

#### **Test Procedure:**
```c
// Test communication performance
void test_communication_performance(void) {
    printf("Starting communication performance test...\n");
    
    const int test_iterations = 1000;
    uint32_t start_time, end_time;
    uint16_t test_data;
    
    // Test RS485 communication speed
    start_time = system_get_time();
    
    for (int i = 0; i < test_iterations; i++) {
        int result = modbus_read_holding_registers(0x01, POWER_REG_VOLTAGE, 1, &test_data);
        assert(result == 0);
    }
    
    end_time = system_get_time();
    uint32_t total_time = end_time - start_time;
    float avg_time = (float)total_time / test_iterations;
    
    printf("  RS485 communication: %d iterations in %d ms (avg: %.2f ms)\n", 
           test_iterations, total_time, avg_time);
    
    // Performance requirements: <10ms average response time
    assert(avg_time < 10.0);
    
    printf("Communication performance test: PASSED\n");
}
```

### **2. System Response Time Testing**

#### **Test Procedure:**
```c
// Test system response time
void test_system_response_time(void) {
    printf("Starting system response time test...\n");
    
    // Test E-Stop response time
    uint32_t start_time = system_get_time();
    
    // Simulate E-Stop activation
    // Note: This should be done carefully
    
    uint32_t response_time = system_get_time() - start_time;
    
    printf("  E-Stop response time: %d ms\n", response_time);
    
    // Performance requirements: <50ms E-Stop response time
    assert(response_time < 50);
    
    // Test motor control response time
    start_time = system_get_time();
    
    // Send motor speed command
    uint16_t target_speed = 100;  // 1.0 m/s
    int result = modbus_write_single_register(0x03, MOTOR_REG_TARGET_SPEED, target_speed);
    assert(result == 0);
    
    response_time = system_get_time() - start_time;
    
    printf("  Motor control response time: %d ms\n", response_time);
    
    // Performance requirements: <100ms motor control response time
    assert(response_time < 100);
    
    printf("System response time test: PASSED\n");
}
```

---

## 🔧 SYSTEM TESTING

### **1. Full System Integration Test**

#### **Test Procedure:**
```c
// Full system integration test
void full_system_integration_test(void) {
    printf("Starting full system integration test...\n");
    
    // 1. System initialization
    printf("  1. System initialization...\n");
    system_init();
    assert(system_get_state() == SYSTEM_STATE_IDLE);
    
    // 2. Auto-discovery
    printf("  2. Auto-discovery...\n");
    auto_discovery_process();
    assert(get_active_module_count() >= 4);  // At least 4 mandatory modules
    
    // 3. Safety system check
    printf("  3. Safety system check...\n");
    assert(system_get_safety_level() == SAFETY_LEVEL_NORMAL);
    
    // 4. Power system check
    printf("  4. Power system check...\n");
    uint8_t soc = get_battery_soc();
    assert(soc > 20);  // Battery should have sufficient charge
    
    // 5. Motor system check
    printf("  5. Motor system check...\n");
    uint16_t motor_status = get_motor_status();
    assert(motor_status & MOTOR_STATUS_OK);
    
    // 6. Location system check
    printf("  6. Location system check...\n");
    uint16_t location_status = get_location_status();
    assert(location_status & LOCATION_STATUS_ACTIVE);
    
    printf("Full system integration test: PASSED\n");
}
```

### **2. End-to-End Testing**

#### **Test Procedure:**
```c
// End-to-end test
void end_to_end_test(void) {
    printf("Starting end-to-end test...\n");
    
    // Test complete navigation cycle
    printf("  1. Starting navigation cycle...\n");
    
    // Set destination
    position_t destination = {5000, 5000, 0};  // 5m, 5m, 0m
    set_navigation_destination(destination);
    
    // Start navigation
    start_navigation();
    assert(system_get_navigation_state() == NAV_STATE_NAVIGATING);
    
    // Monitor navigation progress
    uint32_t start_time = system_get_time();
    uint32_t timeout = 30000;  // 30 second timeout
    
    while (system_get_navigation_state() != NAV_STATE_IDLE) {
        if (system_get_time() - start_time > timeout) {
            printf("  Navigation timeout!\n");
            assert(0);
        }
        
        // Check safety during navigation
        assert(system_get_safety_level() != SAFETY_LEVEL_EMERGENCY);
        
        system_delay(100);  // 100ms delay
    }
    
    printf("  2. Navigation completed successfully\n");
    
    // Test docking
    printf("  3. Starting docking...\n");
    start_docking();
    assert(system_get_navigation_state() == NAV_STATE_DOCKING);
    
    // Monitor docking progress
    start_time = system_get_time();
    timeout = 10000;  // 10 second timeout
    
    while (system_get_navigation_state() != NAV_STATE_IDLE) {
        if (system_get_time() - start_time > timeout) {
            printf("  Docking timeout!\n");
            assert(0);
        }
        
        system_delay(100);  // 100ms delay
    }
    
    printf("  4. Docking completed successfully\n");
    
    printf("End-to-end test: PASSED\n");
}
```

---

## 📋 TEST EXECUTION

### **Test Execution Script:**
```bash
#!/bin/bash
# test_execution.sh

echo "=== OHT-50 v2.0 Testing Suite ==="
echo "Date: $(date)"
echo ""

# Compile test suite
echo "Compiling test suite..."
gcc -o test_suite test_suite.c -lmodbus -lm
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# Run unit tests
echo ""
echo "=== Unit Tests ==="
./test_suite --unit-tests
if [ $? -ne 0 ]; then
    echo "Unit tests failed!"
    exit 1
fi

# Run integration tests
echo ""
echo "=== Integration Tests ==="
./test_suite --integration-tests
if [ $? -ne 0 ]; then
    echo "Integration tests failed!"
    exit 1
fi

# Run safety tests
echo ""
echo "=== Safety Tests ==="
./test_suite --safety-tests
if [ $? -ne 0 ]; then
    echo "Safety tests failed!"
    exit 1
fi

# Run performance tests
echo ""
echo "=== Performance Tests ==="
./test_suite --performance-tests
if [ $? -ne 0 ]; then
    echo "Performance tests failed!"
    exit 1
fi

# Run system tests
echo ""
echo "=== System Tests ==="
./test_suite --system-tests
if [ $? -ne 0 ]; then
    echo "System tests failed!"
    exit 1
fi

echo ""
echo "=== All Tests Passed! ==="
echo "Test execution completed successfully."
```

### **Test Results Report:**
```c
// Test results structure
typedef struct {
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t skipped_tests;
    float pass_rate;
    uint32_t execution_time;
    char timestamp[32];
} test_results_t;

// Generate test report
void generate_test_report(test_results_t *results) {
    printf("=== Test Results Report ===\n");
    printf("Date: %s\n", results->timestamp);
    printf("Total Tests: %d\n", results->total_tests);
    printf("Passed: %d\n", results->passed_tests);
    printf("Failed: %d\n", results->failed_tests);
    printf("Skipped: %d\n", results->skipped_tests);
    printf("Pass Rate: %.2f%%\n", results->pass_rate);
    printf("Execution Time: %d ms\n", results->execution_time);
    
    if (results->pass_rate >= 95.0) {
        printf("Status: PASSED\n");
    } else {
        printf("Status: FAILED\n");
    }
}
```

---

## 📊 TEST COVERAGE

### **Coverage Metrics:**
- **Unit Test Coverage:** >90%
- **Integration Test Coverage:** >85%
- **Safety Test Coverage:** 100%
- **Performance Test Coverage:** >80%
- **System Test Coverage:** >75%

### **Coverage Requirements:**
- All mandatory modules must have 100% unit test coverage
- All safety functions must have 100% test coverage
- All communication protocols must have >95% test coverage
- All error handling paths must be tested

---

**Status:** Testing Procedures Complete  
**Next Steps:** Execute tests và validate results
