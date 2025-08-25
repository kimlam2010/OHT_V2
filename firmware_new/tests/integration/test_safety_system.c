#include "unity.h"
#include "safety_module_handler.h"
#include "travel_motor_module_handler.h"
#include "hal_estop.h"
#include "hal_common.h"
#include <string.h>

// Test fixtures
static safety_module_handler_t test_safety_handler;
static travel_motor_module_handler_t test_motor_handler;
static hal_estop_t test_estop;
static safety_module_config_t test_safety_config;
static travel_motor_module_config_t test_motor_config;
static hal_estop_config_t test_estop_config;

void setUp(void) {
    memset(&test_safety_handler, 0, sizeof(test_safety_handler));
    memset(&test_motor_handler, 0, sizeof(test_motor_handler));
    memset(&test_estop, 0, sizeof(test_estop));
    memset(&test_safety_config, 0, sizeof(test_safety_config));
    memset(&test_motor_config, 0, sizeof(test_motor_config));
    memset(&test_estop_config, 0, sizeof(test_estop_config));
    
    // Configure safety module
    test_safety_config.address = 0x03;
    test_safety_config.update_rate_hz = 10;
    test_safety_config.enable_auto_stop = true;
    test_safety_config.enable_relay_control = true;
    test_safety_config.thresholds.warning_distance = 1000;
    test_safety_config.thresholds.critical_distance = 500;
    test_safety_config.thresholds.emergency_distance = 200;
    for (int i = 0; i < 4; i++) {
        test_safety_config.thresholds.analog_thresholds[i] = 800;
    }
    
    // Configure motor module
    test_motor_config.address = 0x04;
    test_motor_config.update_rate_hz = 50;
    test_motor_config.max_speed_mmps = 1000;
    test_motor_config.max_acceleration_mmps2 = 500;
    test_motor_config.enable_current_protection = true;
    test_motor_config.current_limit_ma = 5000;
    test_motor_config.enable_brake_control = true;
    
    // Configure E-Stop
    test_estop_config.pin = 0;
    test_estop_config.debounce_time_ms = 50;
    test_estop_config.timeout_ms = 1000;
}

void tearDown(void) {
    safety_module_deinit(&test_safety_handler);
    travel_motor_module_deinit(&test_motor_handler);
    hal_estop_deinit(&test_estop);
}

// Test safety system initialization
void test_safety_system_initialization_works_correctly(void) {
    // Initialize E-Stop first
    hal_status_t estop_result = hal_estop_init(&test_estop, &test_estop_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, estop_result);
    
    // Initialize safety module
    hal_status_t safety_result = safety_module_init(&test_safety_handler, &test_safety_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, safety_result);
    
    // Initialize motor module
    hal_status_t motor_result = travel_motor_module_init(&test_motor_handler, &test_motor_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, motor_result);
    
    TEST_ASSERT_TRUE(test_estop.initialized);
    TEST_ASSERT_TRUE(test_safety_handler.initialized);
    TEST_ASSERT_TRUE(test_motor_handler.initialized);
}

// Test E-Stop functionality
void test_estop_functionality_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Check E-Stop status
    bool triggered;
    hal_status_t result = hal_estop_is_triggered(&test_estop, &triggered);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Test E-Stop channels
    bool channel1_status, channel2_status;
    result = hal_estop_get_channel1_status(&test_estop, &channel1_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    result = hal_estop_get_channel2_status(&test_estop, &channel2_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test safety sensor monitoring
void test_safety_sensor_monitoring_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Read analog sensors
    uint16_t distance;
    hal_status_t result = safety_module_get_analog_sensor(&test_safety_handler, 0, &distance);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Read digital sensors
    uint8_t sensors;
    result = safety_module_get_digital_sensors(&test_safety_handler, &sensors);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Check proximity detection
    bool detected;
    result = safety_module_is_proximity_detected(&test_safety_handler, 0, &detected);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test safety zone monitoring
void test_safety_zone_monitoring_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Check safety status
    bool safe;
    hal_status_t result = safety_module_check_safety(&test_safety_handler, &safe);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Get violation info
    bool violation;
    uint8_t violation_type;
    result = safety_module_get_violation_info(&test_safety_handler, &violation, &violation_type);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test motor emergency stop
void test_motor_emergency_stop_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Set motor speed
    hal_status_t result = travel_motor_module_set_speed(&test_motor_handler, 0, 500);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Trigger emergency stop
    result = travel_motor_module_emergency_stop(&test_motor_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Check if emergency stop is active
    bool active;
    result = travel_motor_module_is_emergency_stop_active(&test_motor_handler, &active);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(active);
    
    // Clear emergency stop
    result = travel_motor_module_clear_emergency_stop(&test_motor_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test safety relay control
void test_safety_relay_control_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Set relay
    hal_status_t result = safety_module_set_relay(&test_safety_handler, 0, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Get relay status
    bool state;
    result = safety_module_get_relay(&test_safety_handler, 0, &state);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(state);
    
    // Clear relay
    result = safety_module_set_relay(&test_safety_handler, 0, false);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    result = safety_module_get_relay(&test_safety_handler, 0, &state);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_FALSE(state);
}

// Test motor brake control
void test_motor_brake_control_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Set brake
    hal_status_t result = travel_motor_module_set_brake(&test_motor_handler, 0, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Get brake status
    bool brake_active;
    result = travel_motor_module_get_brake_status(&test_motor_handler, 0, &brake_active);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(brake_active);
    
    // Release brake
    result = travel_motor_module_set_brake(&test_motor_handler, 0, false);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    result = travel_motor_module_get_brake_status(&test_motor_handler, 0, &brake_active);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_FALSE(brake_active);
}

// Test current protection
void test_current_protection_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Check current protection status
    bool active;
    hal_status_t result = travel_motor_module_is_current_protection_active(&test_motor_handler, &active);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Get motor current
    uint16_t current;
    result = travel_motor_module_get_current(&test_motor_handler, 0, &current);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test safety threshold management
void test_safety_threshold_management_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Get current thresholds
    safety_thresholds_t thresholds;
    hal_status_t result = safety_module_get_thresholds(&test_safety_handler, &thresholds);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL(1000, thresholds.warning_distance);
    TEST_ASSERT_EQUAL(500, thresholds.critical_distance);
    TEST_ASSERT_EQUAL(200, thresholds.emergency_distance);
    
    // Set new thresholds
    safety_thresholds_t new_thresholds = thresholds;
    new_thresholds.warning_distance = 1200;
    new_thresholds.critical_distance = 600;
    new_thresholds.emergency_distance = 250;
    
    result = safety_module_set_thresholds(&test_safety_handler, &new_thresholds);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Verify new thresholds
    safety_thresholds_t updated_thresholds;
    result = safety_module_get_thresholds(&test_safety_handler, &updated_thresholds);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL(1200, updated_thresholds.warning_distance);
    TEST_ASSERT_EQUAL(600, updated_thresholds.critical_distance);
    TEST_ASSERT_EQUAL(250, updated_thresholds.emergency_distance);
}

// Test safety violation handling
void test_safety_violation_handling_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Clear any existing violations
    hal_status_t result = safety_module_clear_violation(&test_safety_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Check violation status
    bool violation;
    uint8_t violation_type;
    result = safety_module_get_violation_info(&test_safety_handler, &violation, &violation_type);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test safety system integration
void test_safety_system_integration_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Start motor motion
    hal_status_t result = travel_motor_module_set_speed(&test_motor_handler, 0, 300);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Check safety status
    bool safe;
    result = safety_module_check_safety(&test_safety_handler, &safe);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // If safety violation detected, motor should stop
    if (!safe) {
        bool estop_active;
        result = travel_motor_module_is_emergency_stop_active(&test_motor_handler, &estop_active);
        TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    }
}

// Test safety system performance
void test_safety_system_performance_is_acceptable(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Measure safety check response time
    uint64_t start_time = hal_get_timestamp_us();
    bool safe;
    hal_status_t result = safety_module_check_safety(&test_safety_handler, &safe);
    uint64_t end_time = hal_get_timestamp_us();
    uint64_t response_time = end_time - start_time;
    
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_LESS_THAN(1000, response_time); // Should respond within 1ms
}

// Test safety system fault handling
void test_safety_system_fault_handling_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Get fault status
    uint8_t safety_fault;
    hal_status_t result = safety_module_get_fault_status(&test_safety_handler, &safety_fault);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    uint8_t motor_fault;
    result = travel_motor_module_get_fault_status(&test_motor_handler, &motor_fault);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Clear faults
    result = safety_module_clear_faults(&test_safety_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    result = travel_motor_module_clear_faults(&test_motor_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test safety system diagnostics
void test_safety_system_diagnostics_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Get safety module diagnostics
    char safety_info[256];
    hal_status_t result = safety_module_get_diagnostics(&test_safety_handler, safety_info, sizeof(safety_info));
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_GREATER_THAN(0, strlen(safety_info));
    
    // Get motor module diagnostics
    char motor_info[256];
    result = travel_motor_module_get_diagnostics(&test_motor_handler, motor_info, sizeof(motor_info));
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_GREATER_THAN(0, strlen(motor_info));
}

// Test safety system enable/disable
void test_safety_system_enable_disable_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Enable safety system
    hal_status_t result = safety_module_enable(&test_safety_handler, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(test_safety_handler.enabled);
    
    // Enable motor system
    result = travel_motor_module_enable(&test_motor_handler, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(test_motor_handler.enabled);
    
    // Disable safety system
    result = safety_module_enable(&test_safety_handler, false);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_FALSE(test_safety_handler.enabled);
    
    // Disable motor system
    result = travel_motor_module_enable(&test_motor_handler, false);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_FALSE(test_motor_handler.enabled);
}

// Test safety system update
void test_safety_system_update_works_correctly(void) {
    hal_estop_init(&test_estop, &test_estop_config);
    safety_module_init(&test_safety_handler, &test_safety_config);
    travel_motor_module_init(&test_motor_handler, &test_motor_config);
    
    // Update safety module
    hal_status_t result = safety_module_update(&test_safety_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Update motor module
    result = travel_motor_module_update(&test_motor_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

int main(void) {
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_safety_system_initialization_works_correctly);
    
    // E-Stop tests
    RUN_TEST(test_estop_functionality_works_correctly);
    
    // Safety sensor tests
    RUN_TEST(test_safety_sensor_monitoring_works_correctly);
    RUN_TEST(test_safety_zone_monitoring_works_correctly);
    
    // Motor control tests
    RUN_TEST(test_motor_emergency_stop_works_correctly);
    RUN_TEST(test_motor_brake_control_works_correctly);
    RUN_TEST(test_current_protection_works_correctly);
    
    // Safety control tests
    RUN_TEST(test_safety_relay_control_works_correctly);
    RUN_TEST(test_safety_threshold_management_works_correctly);
    RUN_TEST(test_safety_violation_handling_works_correctly);
    
    // Integration tests
    RUN_TEST(test_safety_system_integration_works_correctly);
    
    // Performance tests
    RUN_TEST(test_safety_system_performance_is_acceptable);
    
    // Fault handling tests
    RUN_TEST(test_safety_system_fault_handling_works_correctly);
    
    // Diagnostics tests
    RUN_TEST(test_safety_system_diagnostics_works_correctly);
    
    // Enable/disable tests
    RUN_TEST(test_safety_system_enable_disable_works_correctly);
    
    // Update tests
    RUN_TEST(test_safety_system_update_works_correctly);
    
    return UNITY_END();
}
