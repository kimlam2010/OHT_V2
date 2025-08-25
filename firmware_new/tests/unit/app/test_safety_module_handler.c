#include "unity.h"
#include "safety_module_handler.h"
#include "hal_common.h"
#include <string.h>

// Test fixtures
static safety_module_handler_t test_handler;
static safety_module_config_t test_config;

void setUp(void) {
    memset(&test_handler, 0, sizeof(test_handler));
    memset(&test_config, 0, sizeof(test_config));
    
    test_config.address = 0x03;
    test_config.update_rate_hz = 10;
    test_config.enable_auto_stop = true;
    test_config.enable_relay_control = true;
    
    // Set default thresholds
    test_config.thresholds.warning_distance = 1000;
    test_config.thresholds.critical_distance = 500;
    test_config.thresholds.emergency_distance = 200;
    for (int i = 0; i < 4; i++) {
        test_config.thresholds.analog_thresholds[i] = 800;
    }
}

void tearDown(void) {
    safety_module_deinit(&test_handler);
}

// Test initialization
void test_safety_module_init_returns_success(void) {
    hal_status_t result = safety_module_init(&test_handler, &test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(test_handler.initialized);
    TEST_ASSERT_EQUAL(0x03, test_handler.address);
}

void test_safety_module_init_null_handler_returns_error(void) {
    hal_status_t result = safety_module_init(NULL, &test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_safety_module_init_null_config_returns_error(void) {
    hal_status_t result = safety_module_init(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_safety_module_init_invalid_address_returns_error(void) {
    test_config.address = 0xFF; // Invalid address
    hal_status_t result = safety_module_init(&test_handler, &test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test deinitialization
void test_safety_module_deinit_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_deinit(&test_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_FALSE(test_handler.initialized);
}

void test_safety_module_deinit_not_initialized_returns_error(void) {
    hal_status_t result = safety_module_deinit(&test_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test sensor reading
void test_safety_module_read_sensors_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    safety_sensor_data_t sensor_data;
    hal_status_t result = safety_module_read_sensors(&test_handler, &sensor_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_read_sensors_null_data_returns_error(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_read_sensors(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_safety_module_read_sensors_not_initialized_returns_error(void) {
    safety_sensor_data_t sensor_data;
    hal_status_t result = safety_module_read_sensors(&test_handler, &sensor_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test analog sensor reading
void test_safety_module_get_analog_sensor_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    uint16_t distance;
    hal_status_t result = safety_module_get_analog_sensor(&test_handler, 0, &distance);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_get_analog_sensor_invalid_sensor_returns_error(void) {
    safety_module_init(&test_handler, &test_config);
    uint16_t distance;
    hal_status_t result = safety_module_get_analog_sensor(&test_handler, 4, &distance);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test digital sensor reading
void test_safety_module_get_digital_sensors_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    uint8_t sensors;
    hal_status_t result = safety_module_get_digital_sensors(&test_handler, &sensors);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test relay control
void test_safety_module_set_relay_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_set_relay(&test_handler, 0, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_set_relay_invalid_relay_returns_error(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_set_relay(&test_handler, 4, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_safety_module_get_relay_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    bool state;
    hal_status_t result = safety_module_get_relay(&test_handler, 0, &state);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test safety checking
void test_safety_module_check_safety_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    bool safe;
    hal_status_t result = safety_module_check_safety(&test_handler, &safe);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_check_safety_null_safe_returns_error(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_check_safety(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test violation handling
void test_safety_module_get_violation_info_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    bool violation;
    uint8_t type;
    hal_status_t result = safety_module_get_violation_info(&test_handler, &violation, &type);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_clear_violation_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_clear_violation(&test_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test threshold management
void test_safety_module_set_thresholds_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    safety_thresholds_t new_thresholds = test_config.thresholds;
    new_thresholds.warning_distance = 1200;
    hal_status_t result = safety_module_set_thresholds(&test_handler, &new_thresholds);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_get_thresholds_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    safety_thresholds_t thresholds;
    hal_status_t result = safety_module_get_thresholds(&test_handler, &thresholds);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL(1000, thresholds.warning_distance);
}

// Test register access
void test_safety_module_read_register_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    uint16_t value;
    hal_status_t result = safety_module_read_register(&test_handler, SAFETY_REG_SYSTEM_STATUS, &value);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_write_register_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_write_register(&test_handler, SAFETY_REG_RELAY_CONTROL, 0x01);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test utility functions
void test_safety_module_get_fault_status_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    uint8_t fault_status;
    hal_status_t result = safety_module_get_fault_status(&test_handler, &fault_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_clear_faults_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_clear_faults(&test_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

void test_safety_module_get_diagnostics_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    char info[256];
    hal_status_t result = safety_module_get_diagnostics(&test_handler, info, sizeof(info));
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_GREATER_THAN(0, strlen(info));
}

// Test enable/disable
void test_safety_module_enable_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_enable(&test_handler, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_TRUE(test_handler.enabled);
}

void test_safety_module_disable_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    safety_module_enable(&test_handler, true);
    hal_status_t result = safety_module_enable(&test_handler, false);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_FALSE(test_handler.enabled);
}

// Test update function
void test_safety_module_update_returns_success(void) {
    safety_module_init(&test_handler, &test_config);
    hal_status_t result = safety_module_update(&test_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test constants
void test_safety_module_constants_are_defined(void) {
    TEST_ASSERT_EQUAL(0x03, SAFETY_MODULE_ADDRESS);
    TEST_ASSERT_EQUAL(0x0000, SAFETY_REG_SYSTEM_STATUS);
    TEST_ASSERT_EQUAL(0x0001, SAFETY_REG_EMERGENCY_STOP);
    TEST_ASSERT_EQUAL(0x0010, SAFETY_REG_ANALOG_INPUT_1);
    TEST_ASSERT_EQUAL(0x0020, SAFETY_REG_DIGITAL_INPUT);
    TEST_ASSERT_EQUAL(0x0030, SAFETY_REG_RELAY_CONTROL);
}

// Test data structures
void test_safety_module_data_structures_are_valid(void) {
    TEST_ASSERT_EQUAL(4, sizeof(test_handler.data.sensor_data.analog_sensors) / sizeof(uint16_t));
    TEST_ASSERT_EQUAL(1, sizeof(test_handler.data.sensor_data.digital_sensors));
    TEST_ASSERT_EQUAL(1, sizeof(test_handler.data.sensor_data.relay_outputs));
    TEST_ASSERT_EQUAL(1, sizeof(test_handler.data.sensor_data.safety_status));
    TEST_ASSERT_EQUAL(1, sizeof(test_handler.data.sensor_data.fault_status));
}

int main(void) {
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_safety_module_init_returns_success);
    RUN_TEST(test_safety_module_init_null_handler_returns_error);
    RUN_TEST(test_safety_module_init_null_config_returns_error);
    RUN_TEST(test_safety_module_init_invalid_address_returns_error);
    
    // Deinitialization tests
    RUN_TEST(test_safety_module_deinit_returns_success);
    RUN_TEST(test_safety_module_deinit_not_initialized_returns_error);
    
    // Sensor reading tests
    RUN_TEST(test_safety_module_read_sensors_returns_success);
    RUN_TEST(test_safety_module_read_sensors_null_data_returns_error);
    RUN_TEST(test_safety_module_read_sensors_not_initialized_returns_error);
    RUN_TEST(test_safety_module_get_analog_sensor_returns_success);
    RUN_TEST(test_safety_module_get_analog_sensor_invalid_sensor_returns_error);
    RUN_TEST(test_safety_module_get_digital_sensors_returns_success);
    
    // Relay control tests
    RUN_TEST(test_safety_module_set_relay_returns_success);
    RUN_TEST(test_safety_module_set_relay_invalid_relay_returns_error);
    RUN_TEST(test_safety_module_get_relay_returns_success);
    
    // Safety checking tests
    RUN_TEST(test_safety_module_check_safety_returns_success);
    RUN_TEST(test_safety_module_check_safety_null_safe_returns_error);
    
    // Violation handling tests
    RUN_TEST(test_safety_module_get_violation_info_returns_success);
    RUN_TEST(test_safety_module_clear_violation_returns_success);
    
    // Threshold management tests
    RUN_TEST(test_safety_module_set_thresholds_returns_success);
    RUN_TEST(test_safety_module_get_thresholds_returns_success);
    
    // Register access tests
    RUN_TEST(test_safety_module_read_register_returns_success);
    RUN_TEST(test_safety_module_write_register_returns_success);
    
    // Utility function tests
    RUN_TEST(test_safety_module_get_fault_status_returns_success);
    RUN_TEST(test_safety_module_clear_faults_returns_success);
    RUN_TEST(test_safety_module_get_diagnostics_returns_success);
    
    // Enable/disable tests
    RUN_TEST(test_safety_module_enable_returns_success);
    RUN_TEST(test_safety_module_disable_returns_success);
    
    // Update test
    RUN_TEST(test_safety_module_update_returns_success);
    
    // Constants and data structure tests
    RUN_TEST(test_safety_module_constants_are_defined);
    RUN_TEST(test_safety_module_data_structures_are_valid);
    
    return UNITY_END();
}
