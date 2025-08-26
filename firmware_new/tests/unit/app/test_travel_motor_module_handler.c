#include "unity.h"
#include "travel_motor_module_handler.h"
#include "hal_common.h"
#include <string.h>

// Test fixtures
static travel_motor_module_handler_t test_handler;
static travel_motor_module_config_t test_config;

void setUp(void) {
    memset(&test_handler, 0, sizeof(test_handler));
    memset(&test_config, 0, sizeof(test_config));
    
    test_config.address = 0x04;
    test_config.update_rate_hz = 50;
    test_config.max_speed_mmps = 1000;
    test_config.max_acceleration_mmps2 = 500;
    test_config.enable_current_protection = true;
    test_config.current_limit_ma = 5000;
    test_config.enable_brake_control = true;
}

void tearDown(void) {
    travel_motor_module_deinit(&test_handler);
}

// Test initialization
void test_travel_motor_module_init_returns_success(void) {
    hal_status_t result = travel_motor_module_init(&test_handler, &test_config);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_TRUE(test_handler.initialized);
    TEST_ASSERT_EQUAL(0x04, test_handler.address);
}

void test_travel_motor_module_init_null_handler_returns_error(void) {
    hal_status_t result = travel_motor_module_init(NULL, &test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_travel_motor_module_init_null_config_returns_error(void) {
    hal_status_t result = travel_motor_module_init(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_travel_motor_module_init_invalid_address_returns_error(void) {
    test_config.address = 0xFF; // Invalid address
    hal_status_t result = travel_motor_module_init(&test_handler, &test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test deinitialization
void test_travel_motor_module_deinit_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_deinit(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_FALSE(test_handler.initialized);
}

void test_travel_motor_module_deinit_not_initialized_returns_error(void) {
    hal_status_t result = travel_motor_module_deinit(&test_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test motor control
void test_travel_motor_module_set_speed_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_set_speed(&test_handler, 0, 500);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_set_speed_invalid_motor_returns_error(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_set_speed(&test_handler, 2, 500);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_travel_motor_module_set_speed_exceeds_limit_returns_error(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_set_speed(&test_handler, 0, 1500); // Exceeds max_speed
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_travel_motor_module_get_speed_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    int16_t speed;
    hal_status_t result = travel_motor_module_get_speed(&test_handler, 0, &speed);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_set_direction_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_set_direction(&test_handler, 0, MOTOR_DIRECTION_FORWARD);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_get_direction_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    motor_direction_t direction;
    hal_status_t result = travel_motor_module_get_direction(&test_handler, 0, &direction);
    TEST_ASSERT_EQUAL(0, result);
}

// Test differential drive control
void test_travel_motor_module_set_differential_speed_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_set_differential_speed(&test_handler, 500, 400);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_set_differential_speed_exceeds_limit_returns_error(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_set_differential_speed(&test_handler, 1500, 1400);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test current monitoring
void test_travel_motor_module_get_current_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    uint16_t current;
    hal_status_t result = travel_motor_module_get_current(&test_handler, 0, &current);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_get_current_invalid_motor_returns_error(void) {
    travel_motor_module_init(&test_handler, &test_config);
    uint16_t current;
    hal_status_t result = travel_motor_module_get_current(&test_handler, 2, &current);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_travel_motor_module_is_current_protection_active_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    bool active;
    hal_status_t result = travel_motor_module_is_current_protection_active(&test_handler, &active);
    TEST_ASSERT_EQUAL(0, result);
}

// Test brake control
void test_travel_motor_module_set_brake_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_set_brake(&test_handler, 0, true);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_get_brake_status_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    bool brake_active;
    hal_status_t result = travel_motor_module_get_brake_status(&test_handler, 0, &brake_active);
    TEST_ASSERT_EQUAL(0, result);
}

// Test emergency stop
void test_travel_motor_module_emergency_stop_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_emergency_stop(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_is_emergency_stop_active_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    bool active;
    hal_status_t result = travel_motor_module_is_emergency_stop_active(&test_handler, &active);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_clear_emergency_stop_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    travel_motor_module_emergency_stop(&test_handler);
    hal_status_t result = travel_motor_module_clear_emergency_stop(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

// Test position and velocity
void test_travel_motor_module_get_position_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    int32_t position;
    hal_status_t result = travel_motor_module_get_position(&test_handler, 0, &position);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_get_velocity_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    int16_t velocity;
    hal_status_t result = travel_motor_module_get_velocity(&test_handler, 0, &velocity);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_set_target_position_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_set_target_position(&test_handler, 0, 1000);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_is_target_reached_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    bool reached;
    hal_status_t result = travel_motor_module_is_target_reached(&test_handler, 0, &reached);
    TEST_ASSERT_EQUAL(0, result);
}

// Test motion control
void test_travel_motor_module_start_motion_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_start_motion(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_stop_motion_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_stop_motion(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_is_motion_complete_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    bool complete;
    hal_status_t result = travel_motor_module_is_motion_complete(&test_handler, &complete);
    TEST_ASSERT_EQUAL(0, result);
}

// Test fault handling
void test_travel_motor_module_get_fault_status_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    uint8_t fault_status;
    hal_status_t result = travel_motor_module_get_fault_status(&test_handler, &fault_status);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_clear_faults_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_clear_faults(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

// Test enable/disable
void test_travel_motor_module_enable_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_enable(&test_handler, true);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_TRUE(test_handler.enabled);
}

void test_travel_motor_module_disable_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    travel_motor_module_enable(&test_handler, true);
    hal_status_t result = travel_motor_module_enable(&test_handler, false);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_FALSE(test_handler.enabled);
}

// Test update function
void test_travel_motor_module_update_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_update(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

// Test register access
void test_travel_motor_module_read_register_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    uint16_t value;
    hal_status_t result = travel_motor_module_read_register(&test_handler, MOTOR_REG_ENABLE_STATUS, &value);
    TEST_ASSERT_EQUAL(0, result);
}

void test_travel_motor_module_write_register_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    hal_status_t result = travel_motor_module_write_register(&test_handler, MOTOR_REG_MOTOR1_SPEED, 500);
    TEST_ASSERT_EQUAL(0, result);
}

// Test utility functions
void test_travel_motor_module_get_diagnostics_returns_success(void) {
    travel_motor_module_init(&test_handler, &test_config);
    char info[256];
    hal_status_t result = travel_motor_module_get_diagnostics(&test_handler, info, sizeof(info));
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_GREATER_THAN(0, strlen(info));
}

// Test constants
void test_travel_motor_module_constants_are_defined(void) {
    TEST_ASSERT_EQUAL(0x04, TRAVEL_MOTOR_MODULE_ADDRESS);
    TEST_ASSERT_EQUAL(0x0000, MOTOR_REG_POSITION_TARGET);
    TEST_ASSERT_EQUAL(0x0001, MOTOR_REG_VELOCITY_TARGET);
    TEST_ASSERT_EQUAL(0x0003, MOTOR_REG_CURRENT_POSITION);
    TEST_ASSERT_EQUAL(0x0004, MOTOR_REG_CURRENT_VELOCITY);
    TEST_ASSERT_EQUAL(0x0005, MOTOR_REG_ENABLE_STATUS);
    TEST_ASSERT_EQUAL(0x0010, MOTOR_REG_MOTOR1_SPEED);
    TEST_ASSERT_EQUAL(0x0011, MOTOR_REG_MOTOR2_SPEED);
    TEST_ASSERT_EQUAL(0x0012, MOTOR_REG_MOTOR1_CURRENT);
    TEST_ASSERT_EQUAL(0x0013, MOTOR_REG_MOTOR2_CURRENT);
}

// Test data structures
void test_travel_motor_module_data_structures_are_valid(void) {
    TEST_ASSERT_EQUAL(2, sizeof(test_handler.data.motor_speeds) / sizeof(int16_t));
    TEST_ASSERT_EQUAL(2, sizeof(test_handler.data.motor_currents) / sizeof(uint16_t));
    TEST_ASSERT_EQUAL(2, sizeof(test_handler.data.motor_positions) / sizeof(int32_t));
    TEST_ASSERT_EQUAL(2, sizeof(test_handler.data.motor_velocities) / sizeof(int16_t));
    TEST_ASSERT_EQUAL(2, sizeof(test_handler.data.motor_temperatures) / sizeof(int16_t));
    TEST_ASSERT_EQUAL(2, sizeof(test_handler.data.motor_directions) / sizeof(motor_direction_t));
    TEST_ASSERT_EQUAL(2, sizeof(test_handler.data.brake_status) / sizeof(bool));
}

int main(void) {
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_travel_motor_module_init_returns_success);
    RUN_TEST(test_travel_motor_module_init_null_handler_returns_error);
    RUN_TEST(test_travel_motor_module_init_null_config_returns_error);
    RUN_TEST(test_travel_motor_module_init_invalid_address_returns_error);
    
    // Deinitialization tests
    RUN_TEST(test_travel_motor_module_deinit_returns_success);
    RUN_TEST(test_travel_motor_module_deinit_not_initialized_returns_error);
    
    // Motor control tests
    RUN_TEST(test_travel_motor_module_set_speed_returns_success);
    RUN_TEST(test_travel_motor_module_set_speed_invalid_motor_returns_error);
    RUN_TEST(test_travel_motor_module_set_speed_exceeds_limit_returns_error);
    RUN_TEST(test_travel_motor_module_get_speed_returns_success);
    RUN_TEST(test_travel_motor_module_set_direction_returns_success);
    RUN_TEST(test_travel_motor_module_get_direction_returns_success);
    
    // Differential drive tests
    RUN_TEST(test_travel_motor_module_set_differential_speed_returns_success);
    RUN_TEST(test_travel_motor_module_set_differential_speed_exceeds_limit_returns_error);
    
    // Current monitoring tests
    RUN_TEST(test_travel_motor_module_get_current_returns_success);
    RUN_TEST(test_travel_motor_module_get_current_invalid_motor_returns_error);
    RUN_TEST(test_travel_motor_module_is_current_protection_active_returns_success);
    
    // Brake control tests
    RUN_TEST(test_travel_motor_module_set_brake_returns_success);
    RUN_TEST(test_travel_motor_module_get_brake_status_returns_success);
    
    // Emergency stop tests
    RUN_TEST(test_travel_motor_module_emergency_stop_returns_success);
    RUN_TEST(test_travel_motor_module_is_emergency_stop_active_returns_success);
    RUN_TEST(test_travel_motor_module_clear_emergency_stop_returns_success);
    
    // Position and velocity tests
    RUN_TEST(test_travel_motor_module_get_position_returns_success);
    RUN_TEST(test_travel_motor_module_get_velocity_returns_success);
    RUN_TEST(test_travel_motor_module_set_target_position_returns_success);
    RUN_TEST(test_travel_motor_module_is_target_reached_returns_success);
    
    // Motion control tests
    RUN_TEST(test_travel_motor_module_start_motion_returns_success);
    RUN_TEST(test_travel_motor_module_stop_motion_returns_success);
    RUN_TEST(test_travel_motor_module_is_motion_complete_returns_success);
    
    // Fault handling tests
    RUN_TEST(test_travel_motor_module_get_fault_status_returns_success);
    RUN_TEST(test_travel_motor_module_clear_faults_returns_success);
    
    // Enable/disable tests
    RUN_TEST(test_travel_motor_module_enable_returns_success);
    RUN_TEST(test_travel_motor_module_disable_returns_success);
    
    // Update test
    RUN_TEST(test_travel_motor_module_update_returns_success);
    
    // Register access tests
    RUN_TEST(test_travel_motor_module_read_register_returns_success);
    RUN_TEST(test_travel_motor_module_write_register_returns_success);
    
    // Utility function tests
    RUN_TEST(test_travel_motor_module_get_diagnostics_returns_success);
    
    // Constants and data structure tests
    RUN_TEST(test_travel_motor_module_constants_are_defined);
    RUN_TEST(test_travel_motor_module_data_structures_are_valid);
    
    return UNITY_END();
}
