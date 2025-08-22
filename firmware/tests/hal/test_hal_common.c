/**
 * @file test_hal_common.c
 * @brief Unit tests for hal_common functions
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-15 (HAL Common Testing)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include "../utils/test_helpers.h"
#include "../include/02-HAL/hal_common.h"

// Test configuration
#define TEST_COMMON_TIMEOUT_MS    1000
#define TEST_SLEEP_TIME_MS        100

// Test helper functions
static void print_test_header(const char *test_name) {
    printf("\n=== %s ===\n", test_name);
}

static void print_test_result(const char *test_name, bool passed) {
    printf("  [%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

// Test error callback function
static void test_error_callback(hal_status_t error_code, void *context) {
    bool *called = (bool*)context;
    hal_status_t *error = (hal_status_t*)((char*)context + sizeof(bool));
    *called = true;
    *error = error_code;
}

// Test cases

/**
 * @brief Test HAL status string conversion
 */
static bool test_hal_status_to_string(void) {
    print_test_header("Test HAL Status String Conversion");
    
    // Test all status codes
    TEST_ASSERT_STR_EQUAL("OK", hal_status_to_string(HAL_STATUS_OK));
    TEST_ASSERT_STR_EQUAL("ERROR", hal_status_to_string(HAL_STATUS_ERROR));
    TEST_ASSERT_STR_EQUAL("INVALID_PARAMETER", hal_status_to_string(HAL_STATUS_INVALID_PARAMETER));
    TEST_ASSERT_STR_EQUAL("NOT_INITIALIZED", hal_status_to_string(HAL_STATUS_NOT_INITIALIZED));
    TEST_ASSERT_STR_EQUAL("TIMEOUT", hal_status_to_string(HAL_STATUS_TIMEOUT));
    TEST_ASSERT_STR_EQUAL("BUSY", hal_status_to_string(HAL_STATUS_BUSY));
    TEST_ASSERT_STR_EQUAL("NOT_SUPPORTED", hal_status_to_string(HAL_STATUS_NOT_SUPPORTED));
    TEST_ASSERT_STR_EQUAL("ALREADY_INITIALIZED", hal_status_to_string(HAL_STATUS_ALREADY_INITIALIZED));
    TEST_ASSERT_STR_EQUAL("ALREADY_ACTIVE", hal_status_to_string(HAL_STATUS_ALREADY_ACTIVE));
    TEST_ASSERT_STR_EQUAL("IO_ERROR", hal_status_to_string(HAL_STATUS_IO_ERROR));
    
    // Test unknown status
    TEST_ASSERT_STR_EQUAL("UNKNOWN", hal_status_to_string(999));
    
    return true;
}

/**
 * @brief Test HAL device status string conversion
 */
static bool test_hal_device_status_to_string(void) {
    print_test_header("Test HAL Device Status String Conversion");
    
    // Test all device status codes
    TEST_ASSERT_STR_EQUAL("UNKNOWN", hal_device_status_to_string(HAL_DEVICE_STATUS_UNKNOWN));
    TEST_ASSERT_STR_EQUAL("OK", hal_device_status_to_string(HAL_DEVICE_STATUS_OK));
    TEST_ASSERT_STR_EQUAL("WARNING", hal_device_status_to_string(HAL_DEVICE_STATUS_WARNING));
    TEST_ASSERT_STR_EQUAL("ERROR", hal_device_status_to_string(HAL_DEVICE_STATUS_ERROR));
    TEST_ASSERT_STR_EQUAL("FAULT", hal_device_status_to_string(HAL_DEVICE_STATUS_FAULT));
    TEST_ASSERT_STR_EQUAL("OFFLINE", hal_device_status_to_string(HAL_DEVICE_STATUS_OFFLINE));
    TEST_ASSERT_STR_EQUAL("CALIBRATING", hal_device_status_to_string(HAL_DEVICE_STATUS_CALIBRATING));
    TEST_ASSERT_STR_EQUAL("INITIALIZING", hal_device_status_to_string(HAL_DEVICE_STATUS_INITIALIZING));
    
    // Test unknown status
    TEST_ASSERT_STR_EQUAL("UNKNOWN", hal_device_status_to_string(999));
    
    return true;
}

/**
 * @brief Test HAL device type string conversion
 */
static bool test_hal_device_type_to_string(void) {
    print_test_header("Test HAL Device Type String Conversion");
    
    // Test all device types
    TEST_ASSERT_STR_EQUAL("UNKNOWN", hal_device_type_to_string(HAL_DEVICE_TYPE_UNKNOWN));
    TEST_ASSERT_STR_EQUAL("LIDAR", hal_device_type_to_string(HAL_DEVICE_TYPE_LIDAR));
    TEST_ASSERT_STR_EQUAL("ENCODER", hal_device_type_to_string(HAL_DEVICE_TYPE_ENCODER));
    TEST_ASSERT_STR_EQUAL("MOTOR", hal_device_type_to_string(HAL_DEVICE_TYPE_MOTOR));
    TEST_ASSERT_STR_EQUAL("RELAY", hal_device_type_to_string(HAL_DEVICE_TYPE_RELAY));
    TEST_ASSERT_STR_EQUAL("LIMIT_SWITCH", hal_device_type_to_string(HAL_DEVICE_TYPE_LIMIT_SWITCH));
    TEST_ASSERT_STR_EQUAL("ESTOP", hal_device_type_to_string(HAL_DEVICE_TYPE_ESTOP));
    TEST_ASSERT_STR_EQUAL("UART", hal_device_type_to_string(HAL_DEVICE_TYPE_UART));
    TEST_ASSERT_STR_EQUAL("GPIO", hal_device_type_to_string(HAL_DEVICE_TYPE_GPIO));
    TEST_ASSERT_STR_EQUAL("CAN", hal_device_type_to_string(HAL_DEVICE_TYPE_CAN));
    TEST_ASSERT_STR_EQUAL("USB", hal_device_type_to_string(HAL_DEVICE_TYPE_USB));
    
    // Test unknown type
    TEST_ASSERT_STR_EQUAL("UNKNOWN", hal_device_type_to_string(999));
    
    return true;
}

/**
 * @brief Test HAL timestamp functions
 */
static bool test_hal_timestamp_functions(void) {
    print_test_header("Test HAL Timestamp Functions");
    
    // Test timestamp functions
    uint64_t timestamp_us1 = hal_get_timestamp_us();
    uint64_t timestamp_ms1 = hal_get_timestamp_ms();
    
    TEST_ASSERT(timestamp_us1 > 0);
    TEST_ASSERT(timestamp_ms1 > 0);
    TEST_ASSERT(timestamp_us1 >= timestamp_ms1 * 1000);
    
    // Test sleep functions
    uint64_t start_time = hal_get_timestamp_us();
    hal_sleep_ms(TEST_SLEEP_TIME_MS);
    uint64_t end_time = hal_get_timestamp_us();
    uint64_t sleep_duration = end_time - start_time;
    
    // Sleep duration should be at least the requested time
    TEST_ASSERT(sleep_duration >= TEST_SLEEP_TIME_MS * 1000);
    
    // Test microsecond sleep
    start_time = hal_get_timestamp_us();
    hal_sleep_us(1000); // 1ms
    end_time = hal_get_timestamp_us();
    sleep_duration = end_time - start_time;
    
    TEST_ASSERT(sleep_duration >= 1000);
    
    return true;
}

/**
 * @brief Test HAL error handling functions
 */
static bool test_hal_error_handling(void) {
    print_test_header("Test HAL Error Handling");
    
    // Test error callback
    bool callback_called = false;
    hal_status_t callback_error = HAL_STATUS_OK;
    
    hal_error_callback_t test_callback = test_error_callback;
    
    void *context = malloc(sizeof(bool) + sizeof(hal_status_t));
    bool *called_ptr = (bool*)context;
    hal_status_t *error_ptr = (hal_status_t*)((char*)context + sizeof(bool));
    *called_ptr = false;
    *error_ptr = HAL_STATUS_OK;
    
    hal_status_t status = hal_set_error_callback(test_callback, context);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test get last error (should be OK initially)
    hal_error_info_t error_info;
    status = hal_get_last_error(&error_info);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, error_info.error_code);
    
    // Test clear errors
    status = hal_clear_errors();
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    free(context);
    
    return true;
}

/**
 * @brief Test HAL statistics functions
 */
static bool test_hal_statistics(void) {
    print_test_header("Test HAL Statistics");
    
    // Test get statistics
    hal_statistics_t stats;
    hal_status_t status = hal_get_statistics(&stats);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test reset statistics
    status = hal_reset_statistics();
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Verify reset worked
    status = hal_get_statistics(&stats);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(0, stats.total_operations);
    TEST_ASSERT_EQUAL(0, stats.successful_operations);
    TEST_ASSERT_EQUAL(0, stats.failed_operations);
    
    return true;
}

/**
 * @brief Test HAL configuration functions
 */
static bool test_hal_configuration(void) {
    print_test_header("Test HAL Configuration");
    
    // Test get configuration
    hal_config_t config;
    hal_status_t status = hal_get_configuration(&config);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test set configuration
    hal_config_t new_config = {
        .config_id = 123,
        .version = 1,
        .timestamp_us = hal_get_timestamp_us(),
        .enabled = true,
        .timeout_ms = 5000,
        .retry_count = 3
    };
    
    status = hal_set_configuration(&new_config);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Verify configuration was set
    hal_config_t verify_config;
    status = hal_get_configuration(&verify_config);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(new_config.config_id, verify_config.config_id);
    TEST_ASSERT_EQUAL(new_config.version, verify_config.version);
    TEST_ASSERT_EQUAL(new_config.enabled, verify_config.enabled);
    TEST_ASSERT_EQUAL(new_config.timeout_ms, verify_config.timeout_ms);
    TEST_ASSERT_EQUAL(new_config.retry_count, verify_config.retry_count);
    
    return true;
}

/**
 * @brief Test HAL device management functions
 */
static bool test_hal_device_management(void) {
    print_test_header("Test HAL Device Management");
    
    // Test register device
    hal_status_t status = hal_register_device(HAL_DEVICE_TYPE_LIDAR, "Test LiDAR");
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test get device list
    hal_device_info_t device_list[10];
    uint32_t device_count = 0;
    status = hal_get_device_list(device_list, &device_count);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    TEST_ASSERT(device_count > 0);
    
    // Test get device info
    if (device_count > 0) {
        hal_device_info_t device_info;
        status = hal_get_device_info(device_list[0].device_id, &device_info);
        TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
        TEST_ASSERT_EQUAL(device_list[0].device_id, device_info.device_id);
        TEST_ASSERT_EQUAL(device_list[0].device_type, device_info.device_type);
    }
    
    // Test unregister device
    if (device_count > 0) {
        status = hal_unregister_device(device_list[0].device_id);
        TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    }
    
    return true;
}

/**
 * @brief Test HAL safety functions
 */
static bool test_hal_safety_functions(void) {
    print_test_header("Test HAL Safety Functions");
    
    // Test safety check
    hal_status_t status = hal_safety_check();
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test safety enable/disable
    status = hal_safety_enable();
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    bool enabled = hal_safety_is_enabled();
    TEST_ASSERT(enabled == true);
    
    status = hal_safety_disable();
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    enabled = hal_safety_is_enabled();
    TEST_ASSERT(enabled == false);
    
    // Test get safety status
    hal_safety_status_t safety_status = hal_safety_get_status();
    TEST_ASSERT(safety_status == HAL_SAFETY_OK || safety_status == HAL_SAFETY_ESTOP_TRIGGERED);
    
    return true;
}

/**
 * @brief Test HAL module monitoring functions
 */
static bool test_hal_module_monitoring(void) {
    print_test_header("Test HAL Module Monitoring");
    
    // Test register heartbeat
    hal_status_t status = hal_module_heartbeat_register(1);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test check heartbeat
    status = hal_module_heartbeat_check(1);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test health check
    hal_module_health_t health;
    status = hal_module_health_check(1, &health);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    TEST_ASSERT(health == HAL_MODULE_HEALTH_OK || health == HAL_MODULE_HEALTH_TIMEOUT);
    
    // Test timeout detection
    status = hal_module_timeout_detect();
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    return true;
}

/**
 * @brief Test HAL logging functions
 */
static bool test_hal_logging(void) {
    print_test_header("Test HAL Logging");
    
    // Test log initialization
    hal_status_t status = hal_log_init("test_hal_common.log");
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test log level setting
    status = hal_log_set_level(HAL_LOG_LEVEL_INFO);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test log message
    status = hal_log_message(HAL_LOG_LEVEL_INFO, "Test log message from hal_common test");
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test different log levels
    status = hal_log_message(HAL_LOG_LEVEL_DEBUG, "Debug message");
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    status = hal_log_message(HAL_LOG_LEVEL_WARNING, "Warning message");
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    status = hal_log_message(HAL_LOG_LEVEL_ERROR, "Error message");
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    // Test log close
    status = hal_log_close();
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    
    return true;
}

/**
 * @brief Test HAL version functions
 */
static bool test_hal_version_functions(void) {
    print_test_header("Test HAL Version Functions");
    
    // Test get version
    uint32_t major, minor, patch;
    hal_status_t status = hal_get_version(&major, &minor, &patch);
    TEST_ASSERT_STATUS(HAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(HAL_VERSION_MAJOR, major);
    TEST_ASSERT_EQUAL(HAL_VERSION_MINOR, minor);
    TEST_ASSERT_EQUAL(HAL_VERSION_PATCH, patch);
    
    // Test get version string
    const char *version_string = hal_get_version_string();
    TEST_ASSERT_NOT_NULL(version_string);
    TEST_ASSERT_STR_EQUAL(HAL_VERSION_STRING, version_string);
    
    return true;
}

/**
 * @brief Test HAL constants and definitions
 */
static bool test_hal_constants(void) {
    print_test_header("Test HAL Constants and Definitions");
    
    // Test timeout constants
    TEST_ASSERT(HAL_TIMEOUT_MS > 0);
    TEST_ASSERT(HAL_DEBOUNCE_MS > 0);
    TEST_ASSERT(HAL_SAFETY_TIMEOUT_MS > 0);
    TEST_ASSERT(HAL_WATCHDOG_TIMEOUT_MS > 0);
    TEST_ASSERT(HAL_SAFETY_CHECK_INTERVAL_MS > 0);
    TEST_ASSERT(HAL_MODULE_HEARTBEAT_MS > 0);
    TEST_ASSERT(HAL_ESTOP_RESPONSE_TIME_MS > 0);
    TEST_ASSERT(HAL_MODULE_COMM_TIMEOUT_MS > 0);
    TEST_ASSERT(HAL_MODULE_RETRY_COUNT > 0);
    
    // Test buffer sizes
    TEST_ASSERT(HAL_BUFFER_SIZE > 0);
    TEST_ASSERT(HAL_STRING_SIZE > 0);
    
    // Test LED pin definitions
    TEST_ASSERT(LED_POWER_PIN > 0);
    TEST_ASSERT(LED_SYSTEM_PIN > 0);
    TEST_ASSERT(LED_COMM_PIN > 0);
    TEST_ASSERT(LED_NETWORK_PIN > 0);
    TEST_ASSERT(LED_ERROR_PIN > 0);
    
    // Test E-Stop pin definitions
    TEST_ASSERT(ESTOP_CHANNEL1_PIN > 0);
    TEST_ASSERT(ESTOP_CHANNEL2_PIN > 0);
    
    // Test relay pin definitions
    TEST_ASSERT(RELAY_OUTPUT_PIN > 0);
    
    // Test network interface definitions
    TEST_ASSERT_NOT_NULL(NETWORK_ETH_INTERFACE);
    TEST_ASSERT_NOT_NULL(NETWORK_WIFI_INTERFACE);
    
    // Test device paths
    TEST_ASSERT_NOT_NULL(LIDAR_DEVICE_PATH);
    TEST_ASSERT_NOT_NULL(RS485_DEVICE_PATH);
    
    // Test baud rates
    TEST_ASSERT(LIDAR_BAUD_RATE > 0);
    TEST_ASSERT(RS485_BAUD_RATE > 0);
    
    return true;
}

/**
 * @brief Test HAL error conditions
 */
static bool test_hal_error_conditions(void) {
    print_test_header("Test HAL Error Conditions");
    
    // Test invalid parameters
    hal_status_t status = hal_get_last_error(NULL);
    TEST_ASSERT_STATUS(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = hal_get_statistics(NULL);
    TEST_ASSERT_STATUS(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = hal_get_configuration(NULL);
    TEST_ASSERT_STATUS(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = hal_set_configuration(NULL);
    TEST_ASSERT_STATUS(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = hal_get_device_info(0, NULL);
    TEST_ASSERT_STATUS(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = hal_get_device_list(NULL, NULL);
    TEST_ASSERT_STATUS(HAL_STATUS_INVALID_PARAMETER, status);
    
    hal_module_health_t health;
    status = hal_module_health_check(0, NULL);
    TEST_ASSERT_STATUS(HAL_STATUS_INVALID_PARAMETER, status);
    
    return true;
}

/**
 * @brief Main test function
 */
int main(void) {
    printf("=== HAL Common Unit Tests ===\n");
    printf("Version: %s\n", HAL_VERSION_STRING);
    printf("Date: %s\n", __DATE__);
    
    // Initialize test framework
    test_init("HAL Common Tests", "Unit tests for hal_common functions");
    test_config_init(NULL);
    test_config_set_verbose(true);
    test_config_set_color(true);
    test_config_set_timeout(TEST_COMMON_TIMEOUT_MS);
    
    bool all_tests_passed = true;
    
    // Run all tests
    all_tests_passed &= test_run("Status String Conversion", "Test HAL status to string conversion", test_hal_status_to_string);
    all_tests_passed &= test_run("Device Status String Conversion", "Test HAL device status to string conversion", test_hal_device_status_to_string);
    all_tests_passed &= test_run("Device Type String Conversion", "Test HAL device type to string conversion", test_hal_device_type_to_string);
    all_tests_passed &= test_run("Timestamp Functions", "Test HAL timestamp functions", test_hal_timestamp_functions);
    all_tests_passed &= test_run("Error Handling", "Test HAL error handling functions", test_hal_error_handling);
    all_tests_passed &= test_run("Statistics", "Test HAL statistics functions", test_hal_statistics);
    all_tests_passed &= test_run("Configuration", "Test HAL configuration functions", test_hal_configuration);
    all_tests_passed &= test_run("Device Management", "Test HAL device management functions", test_hal_device_management);
    all_tests_passed &= test_run("Safety Functions", "Test HAL safety functions", test_hal_safety_functions);
    all_tests_passed &= test_run("Module Monitoring", "Test HAL module monitoring functions", test_hal_module_monitoring);
    all_tests_passed &= test_run("Logging", "Test HAL logging functions", test_hal_logging);
    all_tests_passed &= test_run("Version Functions", "Test HAL version functions", test_hal_version_functions);
    all_tests_passed &= test_run("Constants", "Test HAL constants and definitions", test_hal_constants);
    all_tests_passed &= test_run("Error Conditions", "Test HAL error conditions", test_hal_error_conditions);
    
    // Cleanup
    test_cleanup();
    
    printf("\n=== Test Summary ===\n");
    printf("All tests passed: %s\n", all_tests_passed ? "YES" : "NO");
    
    return all_tests_passed ? 0 : 1;
}
