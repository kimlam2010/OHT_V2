#include "unity.h"
#include "dock_module_handler.h"
#include "hal_common.h"
#include <string.h>

// Test fixtures
static dock_module_handler_t test_handler;
static dock_module_config_t test_config;

void setUp(void) {
    memset(&test_handler, 0, sizeof(test_handler));
    memset(&test_config, 0, sizeof(test_config));
    
    test_config.address = 0x05;
    test_config.update_rate_hz = 100;
    test_config.enable_imu_calibration = true;
    test_config.enable_magnetic_sensors = true;
    test_config.enable_limit_switches = true;
    test_config.enable_position_tracking = true;
}

void tearDown(void) {
    dock_module_deinit(&test_handler);
}

// Test initialization
void test_dock_module_init_returns_success(void) {
    hal_status_t result = dock_module_init(&test_handler, &test_config);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_TRUE(test_handler.initialized);
    TEST_ASSERT_EQUAL(0x05, test_handler.address);
}

void test_dock_module_init_null_handler_returns_error(void) {
    hal_status_t result = dock_module_init(NULL, &test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_dock_module_init_null_config_returns_error(void) {
    hal_status_t result = dock_module_init(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_dock_module_init_invalid_address_returns_error(void) {
    test_config.address = 0xFF; // Invalid address
    hal_status_t result = dock_module_init(&test_handler, &test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test deinitialization
void test_dock_module_deinit_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_deinit(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_FALSE(test_handler.initialized);
}

void test_dock_module_deinit_not_initialized_returns_error(void) {
    hal_status_t result = dock_module_deinit(&test_handler);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test IMU data reading
void test_dock_module_read_imu_data_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    imu_data_t imu_data;
    hal_status_t result = dock_module_read_imu_data(&test_handler, &imu_data);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_read_imu_data_null_data_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_read_imu_data(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_dock_module_read_imu_data_not_initialized_returns_error(void) {
    imu_data_t imu_data;
    hal_status_t result = dock_module_read_imu_data(&test_handler, &imu_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test accelerometer reading
void test_dock_module_get_accelerometer_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    int16_t x, y, z;
    hal_status_t result = dock_module_get_accelerometer(&test_handler, &x, &y, &z);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_accelerometer_null_pointers_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_get_accelerometer(&test_handler, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test gyroscope reading
void test_dock_module_get_gyroscope_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    int16_t x, y, z;
    hal_status_t result = dock_module_get_gyroscope(&test_handler, &x, &y, &z);
    TEST_ASSERT_EQUAL(0, result);
}

// Test magnetometer reading
void test_dock_module_get_magnetometer_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    int16_t x, y, z;
    hal_status_t result = dock_module_get_magnetometer(&test_handler, &x, &y, &z);
    TEST_ASSERT_EQUAL(0, result);
}

// Test temperature reading
void test_dock_module_get_temperature_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    int16_t temperature;
    hal_status_t result = dock_module_get_temperature(&test_handler, &temperature);
    TEST_ASSERT_EQUAL(0, result);
}

// Test magnetic sensor reading
void test_dock_module_get_magnetic_sensor_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    uint16_t magnetic_value;
    hal_status_t result = dock_module_get_magnetic_sensor(&test_handler, 0, &magnetic_value);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_magnetic_sensor_invalid_sensor_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    uint16_t magnetic_value;
    hal_status_t result = dock_module_get_magnetic_sensor(&test_handler, 4, &magnetic_value);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test limit switch reading
void test_dock_module_get_limit_switch_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    bool switch_state;
    hal_status_t result = dock_module_get_limit_switch(&test_handler, 0, &switch_state);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_limit_switch_invalid_switch_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    bool switch_state;
    hal_status_t result = dock_module_get_limit_switch(&test_handler, 4, &switch_state);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test navigation data calculation
void test_dock_module_calculate_navigation_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    navigation_data_t nav_data;
    hal_status_t result = dock_module_calculate_navigation(&test_handler, &nav_data);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_calculate_navigation_null_data_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_calculate_navigation(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test orientation reading
void test_dock_module_get_orientation_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    float roll, pitch, yaw;
    hal_status_t result = dock_module_get_orientation(&test_handler, &roll, &pitch, &yaw);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_orientation_null_pointers_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_get_orientation(&test_handler, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test heading reading
void test_dock_module_get_heading_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    float heading;
    hal_status_t result = dock_module_get_heading(&test_handler, &heading);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_heading_null_pointer_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_get_heading(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test position tracking
void test_dock_module_get_position_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    float x, y, z;
    hal_status_t result = dock_module_get_position(&test_handler, &x, &y, &z);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_position_null_pointers_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_get_position(&test_handler, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test velocity reading
void test_dock_module_get_velocity_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    float vx, vy, vz;
    hal_status_t result = dock_module_get_velocity(&test_handler, &vx, &vy, &vz);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_velocity_null_pointers_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_get_velocity(&test_handler, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test angular velocity reading
void test_dock_module_get_angular_velocity_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    float wx, wy, wz;
    hal_status_t result = dock_module_get_angular_velocity(&test_handler, &wx, &wy, &wz);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_angular_velocity_null_pointers_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_get_angular_velocity(&test_handler, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test calibration functions
void test_dock_module_start_calibration_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_start_calibration(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_stop_calibration_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    dock_module_start_calibration(&test_handler);
    hal_status_t result = dock_module_stop_calibration(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_calibration_status_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    bool calibrated;
    hal_status_t result = dock_module_get_calibration_status(&test_handler, &calibrated);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_calibration_status_null_pointer_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_get_calibration_status(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test docking control
void test_dock_module_start_docking_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_start_docking(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_stop_docking_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    dock_module_start_docking(&test_handler);
    hal_status_t result = dock_module_stop_docking(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_is_docking_active_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    bool active;
    hal_status_t result = dock_module_is_docking_active(&test_handler, &active);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_is_docking_active_null_pointer_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_is_docking_active(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_dock_module_is_docked_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    bool docked;
    hal_status_t result = dock_module_is_docked(&test_handler, &docked);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_is_docked_null_pointer_returns_error(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_is_docked(&test_handler, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test register access
void test_dock_module_read_register_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    uint16_t value;
    hal_status_t result = dock_module_read_register(&test_handler, DOCK_REG_IMU_STATUS, &value);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_write_register_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_write_register(&test_handler, DOCK_REG_LIMIT_SWITCH_STATUS, 0x01);
    TEST_ASSERT_EQUAL(0, result);
}

// Test utility functions
void test_dock_module_get_fault_status_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    uint8_t fault_status;
    hal_status_t result = dock_module_get_fault_status(&test_handler, &fault_status);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_clear_faults_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_clear_faults(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

void test_dock_module_get_diagnostics_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    char info[256];
    hal_status_t result = dock_module_get_diagnostics(&test_handler, info, sizeof(info));
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_GREATER_THAN(0, strlen(info));
}

// Test enable/disable
void test_dock_module_enable_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_enable(&test_handler, true);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_TRUE(test_handler.enabled);
}

void test_dock_module_disable_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    dock_module_enable(&test_handler, true);
    hal_status_t result = dock_module_enable(&test_handler, false);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_FALSE(test_handler.enabled);
}

// Test update function
void test_dock_module_update_returns_success(void) {
    dock_module_init(&test_handler, &test_config);
    hal_status_t result = dock_module_update(&test_handler);
    TEST_ASSERT_EQUAL(0, result);
}

// Test constants
void test_dock_module_constants_are_defined(void) {
    TEST_ASSERT_EQUAL(0x05, DOCK_MODULE_ADDRESS);
    TEST_ASSERT_EQUAL(0x0000, DOCK_REG_IMU_ACCEL_X);
    TEST_ASSERT_EQUAL(0x0001, DOCK_REG_IMU_ACCEL_Y);
    TEST_ASSERT_EQUAL(0x0002, DOCK_REG_IMU_ACCEL_Z);
    TEST_ASSERT_EQUAL(0x0003, DOCK_REG_IMU_GYRO_X);
    TEST_ASSERT_EQUAL(0x0004, DOCK_REG_IMU_GYRO_Y);
    TEST_ASSERT_EQUAL(0x0005, DOCK_REG_IMU_GYRO_Z);
    TEST_ASSERT_EQUAL(0x0006, DOCK_REG_IMU_MAG_X);
    TEST_ASSERT_EQUAL(0x0007, DOCK_REG_IMU_MAG_Y);
    TEST_ASSERT_EQUAL(0x0008, DOCK_REG_IMU_MAG_Z);
    TEST_ASSERT_EQUAL(0x0009, DOCK_REG_IMU_TEMP);
    TEST_ASSERT_EQUAL(0x000A, DOCK_REG_IMU_STATUS);
    TEST_ASSERT_EQUAL(0x0010, DOCK_REG_MAGNETIC_1);
    TEST_ASSERT_EQUAL(0x0020, DOCK_REG_LIMIT_SWITCH_1);
    TEST_ASSERT_EQUAL(0x0030, DOCK_REG_POSITION_X);
    TEST_ASSERT_EQUAL(0x0040, DOCK_REG_VELOCITY_X);
}

// Test data structures
void test_dock_module_data_structures_are_valid(void) {
    // Test basic data structure size
    TEST_ASSERT_EQUAL(sizeof(dock_module_data_t), sizeof(test_handler.data));
    
    // Test config structure size
    TEST_ASSERT_EQUAL(sizeof(dock_config_t), sizeof(test_handler.config));
    
    // Test handler structure has required members
    TEST_ASSERT_TRUE(test_handler.initialized == false);
    TEST_ASSERT_TRUE(test_handler.enabled == false);
    TEST_ASSERT_TRUE(test_handler.address == 0);
}

int main(void) {
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_dock_module_init_returns_success);
    RUN_TEST(test_dock_module_init_null_handler_returns_error);
    RUN_TEST(test_dock_module_init_null_config_returns_error);
    RUN_TEST(test_dock_module_init_invalid_address_returns_error);
    
    // Deinitialization tests
    RUN_TEST(test_dock_module_deinit_returns_success);
    RUN_TEST(test_dock_module_deinit_not_initialized_returns_error);
    
    // IMU data reading tests
    RUN_TEST(test_dock_module_read_imu_data_returns_success);
    RUN_TEST(test_dock_module_read_imu_data_null_data_returns_error);
    RUN_TEST(test_dock_module_read_imu_data_not_initialized_returns_error);
    RUN_TEST(test_dock_module_get_accelerometer_returns_success);
    RUN_TEST(test_dock_module_get_accelerometer_null_pointers_returns_error);
    RUN_TEST(test_dock_module_get_gyroscope_returns_success);
    RUN_TEST(test_dock_module_get_magnetometer_returns_success);
    RUN_TEST(test_dock_module_get_temperature_returns_success);
    
    // Magnetic sensor tests
    RUN_TEST(test_dock_module_get_magnetic_sensor_returns_success);
    RUN_TEST(test_dock_module_get_magnetic_sensor_invalid_sensor_returns_error);
    
    // Limit switch tests
    RUN_TEST(test_dock_module_get_limit_switch_returns_success);
    RUN_TEST(test_dock_module_get_limit_switch_invalid_switch_returns_error);
    
    // Navigation tests
    RUN_TEST(test_dock_module_calculate_navigation_returns_success);
    RUN_TEST(test_dock_module_calculate_navigation_null_data_returns_error);
    RUN_TEST(test_dock_module_get_orientation_returns_success);
    RUN_TEST(test_dock_module_get_orientation_null_pointers_returns_error);
    RUN_TEST(test_dock_module_get_heading_returns_success);
    RUN_TEST(test_dock_module_get_heading_null_pointer_returns_error);
    
    // Position and velocity tests
    RUN_TEST(test_dock_module_get_position_returns_success);
    RUN_TEST(test_dock_module_get_position_null_pointers_returns_error);
    RUN_TEST(test_dock_module_get_velocity_returns_success);
    RUN_TEST(test_dock_module_get_velocity_null_pointers_returns_error);
    RUN_TEST(test_dock_module_get_angular_velocity_returns_success);
    RUN_TEST(test_dock_module_get_angular_velocity_null_pointers_returns_error);
    
    // Calibration tests
    RUN_TEST(test_dock_module_start_calibration_returns_success);
    RUN_TEST(test_dock_module_stop_calibration_returns_success);
    RUN_TEST(test_dock_module_get_calibration_status_returns_success);
    RUN_TEST(test_dock_module_get_calibration_status_null_pointer_returns_error);
    
    // Docking control tests
    RUN_TEST(test_dock_module_start_docking_returns_success);
    RUN_TEST(test_dock_module_stop_docking_returns_success);
    RUN_TEST(test_dock_module_is_docking_active_returns_success);
    RUN_TEST(test_dock_module_is_docking_active_null_pointer_returns_error);
    RUN_TEST(test_dock_module_is_docked_returns_success);
    RUN_TEST(test_dock_module_is_docked_null_pointer_returns_error);
    
    // Register access tests
    RUN_TEST(test_dock_module_read_register_returns_success);
    RUN_TEST(test_dock_module_write_register_returns_success);
    
    // Utility function tests
    RUN_TEST(test_dock_module_get_fault_status_returns_success);
    RUN_TEST(test_dock_module_clear_faults_returns_success);
    RUN_TEST(test_dock_module_get_diagnostics_returns_success);
    
    // Enable/disable tests
    RUN_TEST(test_dock_module_enable_returns_success);
    RUN_TEST(test_dock_module_disable_returns_success);
    
    // Update test
    RUN_TEST(test_dock_module_update_returns_success);
    
    // Constants and data structure tests
    RUN_TEST(test_dock_module_constants_are_defined);
    RUN_TEST(test_dock_module_data_structures_are_valid);
    
    UNITY_END();
    return 0;
}
