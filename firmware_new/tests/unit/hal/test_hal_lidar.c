/**
 * @file test_hal_lidar.c
 * @brief Unit tests for HAL LiDAR
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-12 (LiDAR Driver & USB Integration)
 */

#include "unity.h"
#include "hal_lidar.h"
#include "hal_common.h"
#include <string.h>
#include <stdbool.h>

// Test fixtures
static lidar_config_t test_config;
static lidar_scan_data_t test_scan_data;
static lidar_safety_status_t test_safety_status;
static lidar_device_info_t test_device_info;

void setUp(void)
{
    // Initialize test configuration
    memset(&test_config, 0, sizeof(lidar_config_t));
    strcpy(test_config.device_path, "/dev/ttyUSB0");
    test_config.baud_rate = LIDAR_BAUD_RATE;
    test_config.scan_rate_hz = LIDAR_SCAN_RATE_TYPICAL_HZ;
    test_config.emergency_stop_mm = LIDAR_EMERGENCY_STOP_MM;
    test_config.warning_mm = LIDAR_WARNING_MM;
    test_config.safe_mm = LIDAR_SAFE_MM;
    test_config.sample_rate_hz = LIDAR_SAMPLE_RATE_HZ;
    test_config.angular_resolution = LIDAR_ANGULAR_RESOLUTION;
    
    // Initialize test scan data
    memset(&test_scan_data, 0, sizeof(lidar_scan_data_t));
    test_scan_data.scan_complete = true;
    test_scan_data.point_count = 10;
    test_scan_data.scan_quality = 255;
    
    // Add some test points
    for (int i = 0; i < 10; i++) {
        test_scan_data.points[i].distance_mm = 1000 + (i * 100);
        test_scan_data.points[i].angle_deg = i * 36;
        test_scan_data.points[i].quality = 200 + i;
        test_scan_data.points[i].timestamp_us = i * 1000;
    }
    
    // Initialize test safety status
    memset(&test_safety_status, 0, sizeof(lidar_safety_status_t));
    test_safety_status.min_distance_mm = 1000;
    test_safety_status.max_distance_mm = 5000;
    test_safety_status.obstacle_detected = false;
    test_safety_status.emergency_stop_triggered = false;
    test_safety_status.warning_triggered = false;
    
    // Initialize test device info
    memset(&test_device_info, 0, sizeof(lidar_device_info_t));
    test_device_info.model = 0x01;
    test_device_info.firmware_version = 0x02;
    test_device_info.hardware_version = 0x03;
    test_device_info.serial_number = 0x12345678;
    test_device_info.device_healthy = true;
}

void tearDown(void)
{
    // Cleanup - deinitialize LiDAR if initialized
    hal_lidar_deinit();
}

// Test LiDAR initialization
void test_hal_lidar_init_success(void)
{
    hal_status_t status = hal_lidar_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_lidar_init_null_config(void)
{
    hal_status_t status = hal_lidar_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_hal_lidar_init_invalid_device_path(void)
{
    lidar_config_t invalid_config = test_config;
    strcpy(invalid_config.device_path, "");
    
    hal_status_t status = hal_lidar_init(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_hal_lidar_init_invalid_baud_rate(void)
{
    lidar_config_t invalid_config = test_config;
    invalid_config.baud_rate = 9600; // Invalid baud rate
    
    hal_status_t status = hal_lidar_init(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_hal_lidar_init_invalid_scan_rate(void)
{
    lidar_config_t invalid_config = test_config;
    invalid_config.scan_rate_hz = 5; // Below minimum
    
    hal_status_t status = hal_lidar_init(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_hal_lidar_init_invalid_safety_thresholds(void)
{
    lidar_config_t invalid_config = test_config;
    invalid_config.emergency_stop_mm = 1000; // Equal to warning
    invalid_config.warning_mm = 1000;
    
    hal_status_t status = hal_lidar_init(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_hal_lidar_double_init(void)
{
    hal_status_t status = hal_lidar_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_lidar_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ALREADY_INITIALIZED, status);
}

// Test LiDAR deinitialization
void test_hal_lidar_deinit_success(void)
{
    hal_status_t status = hal_lidar_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_lidar_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_lidar_deinit_not_initialized(void)
{
    hal_status_t status = hal_lidar_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Test LiDAR scanning
void test_hal_lidar_start_scanning_not_initialized(void)
{
    hal_status_t status = hal_lidar_start_scanning();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_hal_lidar_stop_scanning_not_initialized(void)
{
    hal_status_t status = hal_lidar_stop_scanning();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status); // Should return OK even if not initialized
}

// Test LiDAR scan data
void test_hal_lidar_get_scan_data_not_initialized(void)
{
    lidar_scan_data_t scan_data;
    hal_status_t status = hal_lidar_get_scan_data(&scan_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_hal_lidar_get_scan_data_null_pointer(void)
{
    hal_status_t status = hal_lidar_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_lidar_get_scan_data(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

// Test LiDAR safety
void test_hal_lidar_check_safety_not_initialized(void)
{
    lidar_safety_status_t safety_status;
    hal_status_t status = hal_lidar_check_safety(&safety_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_hal_lidar_check_safety_null_pointer(void)
{
    hal_status_t status = hal_lidar_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_lidar_check_safety(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

// Test LiDAR device info
void test_hal_lidar_get_device_info_not_initialized(void)
{
    lidar_device_info_t device_info;
    hal_status_t status = hal_lidar_get_device_info(&device_info);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_hal_lidar_get_device_info_null_pointer(void)
{
    hal_status_t status = hal_lidar_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_lidar_get_device_info(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

// Test LiDAR reset
void test_hal_lidar_reset_not_initialized(void)
{
    hal_status_t status = hal_lidar_reset();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Test LiDAR health check
void test_hal_lidar_health_check_not_initialized(void)
{
    hal_status_t status = hal_lidar_health_check();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Test utility functions
void test_lidar_calculate_min_distance(void)
{
    uint16_t min_distance = lidar_calculate_min_distance(&test_scan_data);
    TEST_ASSERT_EQUAL(1000, min_distance); // First point has distance 1000
}

void test_lidar_calculate_min_distance_null_pointer(void)
{
    uint16_t min_distance = lidar_calculate_min_distance(NULL);
    TEST_ASSERT_EQUAL(LIDAR_MAX_DISTANCE_MM, min_distance);
}

void test_lidar_calculate_min_distance_incomplete_scan(void)
{
    lidar_scan_data_t incomplete_scan = test_scan_data;
    incomplete_scan.scan_complete = false;
    
    uint16_t min_distance = lidar_calculate_min_distance(&incomplete_scan);
    TEST_ASSERT_EQUAL(LIDAR_MAX_DISTANCE_MM, min_distance);
}

void test_lidar_calculate_max_distance(void)
{
    uint16_t max_distance = lidar_calculate_max_distance(&test_scan_data);
    TEST_ASSERT_EQUAL(1900, max_distance); // Last point has distance 1900
}

void test_lidar_calculate_max_distance_null_pointer(void)
{
    uint16_t max_distance = lidar_calculate_max_distance(NULL);
    TEST_ASSERT_EQUAL(0, max_distance);
}

void test_lidar_calculate_max_distance_incomplete_scan(void)
{
    lidar_scan_data_t incomplete_scan = test_scan_data;
    incomplete_scan.scan_complete = false;
    
    uint16_t max_distance = lidar_calculate_max_distance(&incomplete_scan);
    TEST_ASSERT_EQUAL(0, max_distance);
}

void test_lidar_is_obstacle_detected_true(void)
{
    bool detected = lidar_is_obstacle_detected(&test_scan_data, 1500);
    TEST_ASSERT_TRUE(detected); // Min distance is 1000, threshold is 1500
}

void test_lidar_is_obstacle_detected_false(void)
{
    bool detected = lidar_is_obstacle_detected(&test_scan_data, 500);
    TEST_ASSERT_FALSE(detected); // Min distance is 1000, threshold is 500
}

void test_lidar_is_obstacle_detected_null_pointer(void)
{
    bool detected = lidar_is_obstacle_detected(NULL, 1000);
    TEST_ASSERT_FALSE(detected);
}

// Test configuration validation
void test_lidar_validate_config_success(void)
{
    hal_status_t status = lidar_validate_config(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_lidar_validate_config_null_pointer(void)
{
    hal_status_t status = lidar_validate_config(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_lidar_validate_config_empty_device_path(void)
{
    lidar_config_t invalid_config = test_config;
    strcpy(invalid_config.device_path, "");
    
    hal_status_t status = lidar_validate_config(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_lidar_validate_config_wrong_baud_rate(void)
{
    lidar_config_t invalid_config = test_config;
    invalid_config.baud_rate = 9600;
    
    hal_status_t status = lidar_validate_config(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_lidar_validate_config_low_scan_rate(void)
{
    lidar_config_t invalid_config = test_config;
    invalid_config.scan_rate_hz = 5;
    
    hal_status_t status = lidar_validate_config(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_lidar_validate_config_high_scan_rate(void)
{
    lidar_config_t invalid_config = test_config;
    invalid_config.scan_rate_hz = 15;
    
    hal_status_t status = lidar_validate_config(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_lidar_validate_config_invalid_safety_thresholds(void)
{
    lidar_config_t invalid_config = test_config;
    invalid_config.emergency_stop_mm = 1000;
    invalid_config.warning_mm = 1000;
    
    hal_status_t status = lidar_validate_config(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

// Test constants
void test_lidar_constants(void)
{
    TEST_ASSERT_EQUAL(12000, LIDAR_MAX_DISTANCE_MM);
    TEST_ASSERT_EQUAL(50, LIDAR_MIN_DISTANCE_MM);
    TEST_ASSERT_EQUAL(8, LIDAR_SCAN_RATE_MIN_HZ);
    TEST_ASSERT_EQUAL(12, LIDAR_SCAN_RATE_MAX_HZ);
    TEST_ASSERT_EQUAL(10, LIDAR_SCAN_RATE_TYPICAL_HZ);
    TEST_ASSERT_EQUAL(5000, LIDAR_SAMPLE_RATE_HZ);
    TEST_ASSERT_EQUAL(460800, LIDAR_BAUD_RATE);
    TEST_ASSERT_EQUAL(500, LIDAR_EMERGENCY_STOP_MM);
    TEST_ASSERT_EQUAL(1000, LIDAR_WARNING_MM);
    TEST_ASSERT_EQUAL(2000, LIDAR_SAFE_MM);
    TEST_ASSERT_EQUAL(0xA5, LIDAR_START_FLAG);
    TEST_ASSERT_EQUAL(500, LIDAR_POINTS_PER_SCAN);
    TEST_ASSERT_EQUAL(1024, LIDAR_SCAN_BUFFER_SIZE);
}

// Test data structures
void test_lidar_data_structures(void)
{
    // Test lidar_point_t
    lidar_point_t point = {0};
    point.distance_mm = 1000;
    point.angle_deg = 90;
    point.quality = 255;
    point.timestamp_us = 123456789;
    
    TEST_ASSERT_EQUAL(1000, point.distance_mm);
    TEST_ASSERT_EQUAL(90, point.angle_deg);
    TEST_ASSERT_EQUAL(255, point.quality);
    TEST_ASSERT_EQUAL(123456789, point.timestamp_us);
    
    // Test lidar_scan_data_t
    TEST_ASSERT_EQUAL(10, test_scan_data.point_count);
    TEST_ASSERT_TRUE(test_scan_data.scan_complete);
    TEST_ASSERT_EQUAL(255, test_scan_data.scan_quality);
    
    // Test lidar_safety_status_t
    TEST_ASSERT_EQUAL(1000, test_safety_status.min_distance_mm);
    TEST_ASSERT_EQUAL(5000, test_safety_status.max_distance_mm);
    TEST_ASSERT_FALSE(test_safety_status.obstacle_detected);
    TEST_ASSERT_FALSE(test_safety_status.emergency_stop_triggered);
    TEST_ASSERT_FALSE(test_safety_status.warning_triggered);
    
    // Test lidar_device_info_t
    TEST_ASSERT_EQUAL(0x01, test_device_info.model);
    TEST_ASSERT_EQUAL(0x02, test_device_info.firmware_version);
    TEST_ASSERT_EQUAL(0x03, test_device_info.hardware_version);
    TEST_ASSERT_EQUAL(0x12345678, test_device_info.serial_number);
    TEST_ASSERT_TRUE(test_device_info.device_healthy);
}

// Main test runner
int main(void)
{
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_hal_lidar_init_success);
    RUN_TEST(test_hal_lidar_init_null_config);
    RUN_TEST(test_hal_lidar_init_invalid_device_path);
    RUN_TEST(test_hal_lidar_init_invalid_baud_rate);
    RUN_TEST(test_hal_lidar_init_invalid_scan_rate);
    RUN_TEST(test_hal_lidar_init_invalid_safety_thresholds);
    RUN_TEST(test_hal_lidar_double_init);
    
    // Deinitialization tests
    RUN_TEST(test_hal_lidar_deinit_success);
    RUN_TEST(test_hal_lidar_deinit_not_initialized);
    
    // Scanning tests
    RUN_TEST(test_hal_lidar_start_scanning_not_initialized);
    RUN_TEST(test_hal_lidar_stop_scanning_not_initialized);
    
    // Scan data tests
    RUN_TEST(test_hal_lidar_get_scan_data_not_initialized);
    RUN_TEST(test_hal_lidar_get_scan_data_null_pointer);
    
    // Safety tests
    RUN_TEST(test_hal_lidar_check_safety_not_initialized);
    RUN_TEST(test_hal_lidar_check_safety_null_pointer);
    
    // Device info tests
    RUN_TEST(test_hal_lidar_get_device_info_not_initialized);
    RUN_TEST(test_hal_lidar_get_device_info_null_pointer);
    
    // Reset and health check tests
    RUN_TEST(test_hal_lidar_reset_not_initialized);
    RUN_TEST(test_hal_lidar_health_check_not_initialized);
    
    // Utility function tests
    RUN_TEST(test_lidar_calculate_min_distance);
    RUN_TEST(test_lidar_calculate_min_distance_null_pointer);
    RUN_TEST(test_lidar_calculate_min_distance_incomplete_scan);
    RUN_TEST(test_lidar_calculate_max_distance);
    RUN_TEST(test_lidar_calculate_max_distance_null_pointer);
    RUN_TEST(test_lidar_calculate_max_distance_incomplete_scan);
    RUN_TEST(test_lidar_is_obstacle_detected_true);
    RUN_TEST(test_lidar_is_obstacle_detected_false);
    RUN_TEST(test_lidar_is_obstacle_detected_null_pointer);
    
    // Configuration validation tests
    RUN_TEST(test_lidar_validate_config_success);
    RUN_TEST(test_lidar_validate_config_null_pointer);
    RUN_TEST(test_lidar_validate_config_empty_device_path);
    RUN_TEST(test_lidar_validate_config_wrong_baud_rate);
    RUN_TEST(test_lidar_validate_config_low_scan_rate);
    RUN_TEST(test_lidar_validate_config_high_scan_rate);
    RUN_TEST(test_lidar_validate_config_invalid_safety_thresholds);
    
    // Constants and data structure tests
    RUN_TEST(test_lidar_constants);
    RUN_TEST(test_lidar_data_structures);
    
    UNITY_END();
    return 0;
}
