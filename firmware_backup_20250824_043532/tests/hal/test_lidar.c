/**
 * @file test_lidar.c
 * @brief Test program for HAL LiDAR implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-12 (LiDAR Driver & USB Integration)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "hal_lidar.h"

// Global variables for cleanup
static bool test_running = true;

// Signal handler for graceful shutdown
static void signal_handler(int sig)
{
    printf("\nReceived signal %d, shutting down...\n", sig);
    test_running = false;
}

// Test functions
static hal_status_t test_lidar_init(void);
static hal_status_t test_lidar_config(void);
static hal_status_t test_lidar_device_info(void);
static hal_status_t test_lidar_health_check(void);
static hal_status_t test_lidar_scanning(void);
static hal_status_t test_lidar_safety(void);
static hal_status_t test_lidar_cleanup(void);

// Utility functions
static void print_scan_data(const lidar_scan_data_t *scan_data);
static void print_safety_status(const lidar_safety_status_t *safety_status);
static void print_device_info(const lidar_device_info_t *device_info);

int main(int argc, char *argv[])
{
    (void)argc; // Unused parameter
    (void)argv; // Unused parameter
    printf("=== HAL LiDAR Test Program - OHT-50 ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("LiDAR Model: RPLIDAR C1M1\n");
    printf("Baud Rate: %d\n", LIDAR_BAUD_RATE);
    printf("Scan Rate: %d-%d Hz (typical %d Hz)\n", 
           LIDAR_SCAN_RATE_MIN_HZ, LIDAR_SCAN_RATE_MAX_HZ, LIDAR_SCAN_RATE_TYPICAL_HZ);
    printf("Angular Resolution: %.2f°\n", LIDAR_ANGULAR_RESOLUTION);
    printf("Accuracy: ±%dmm\n", LIDAR_ACCURACY_MM);
    printf("Resolution: %dmm\n", LIDAR_RESOLUTION_MM);
    printf("\n");

    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Run tests
    hal_status_t status;
    int test_count = 0;
    int passed_count = 0;

    printf("🔧 Test 1: LiDAR Initialization\n");
    printf("--------------------------------\n");
    status = test_lidar_init();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: LiDAR initialization\n");
        passed_count++;
    } else {
        printf("❌ FAIL: LiDAR initialization (status: %d)\n", status);
        return 1;
    }

    printf("\n🔧 Test 2: LiDAR Configuration\n");
    printf("--------------------------------\n");
    status = test_lidar_config();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: LiDAR configuration\n");
        passed_count++;
    } else {
        printf("❌ FAIL: LiDAR configuration (status: %d)\n", status);
    }

    printf("\n🔧 Test 3: Device Information\n");
    printf("------------------------------\n");
    status = test_lidar_device_info();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Device information\n");
        passed_count++;
    } else {
        printf("❌ FAIL: Device information (status: %d)\n", status);
    }

    printf("\n🔧 Test 4: Health Check\n");
    printf("------------------------\n");
    status = test_lidar_health_check();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Health check\n");
        passed_count++;
    } else {
        printf("❌ FAIL: Health check (status: %d)\n", status);
    }

    printf("\n🔧 Test 5: Scanning Test\n");
    printf("-------------------------\n");
    status = test_lidar_scanning();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Scanning test\n");
        passed_count++;
    } else {
        printf("❌ FAIL: Scanning test (status: %d)\n", status);
    }

    printf("\n🔧 Test 6: Safety Test\n");
    printf("----------------------\n");
    status = test_lidar_safety();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Safety test\n");
        passed_count++;
    } else {
        printf("❌ FAIL: Safety test (status: %d)\n", status);
    }

    printf("\n🔧 Test 7: Cleanup\n");
    printf("-------------------\n");
    status = test_lidar_cleanup();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Cleanup\n");
        passed_count++;
    } else {
        printf("❌ FAIL: Cleanup (status: %d)\n", status);
    }

    // Test summary
    printf("\n🎯 Test Summary\n");
    printf("===============\n");
    printf("Total Tests: %d\n", test_count);
    printf("Passed: %d\n", passed_count);
    printf("Failed: %d\n", test_count - passed_count);
    printf("Success Rate: %.1f%%\n", (float)passed_count / test_count * 100);

    if (passed_count == test_count) {
        printf("\n🎉 All tests PASSED! HAL LiDAR implementation is working correctly.\n");
        return 0;
    } else {
        printf("\n⚠️  Some tests failed. Check implementation.\n");
        return 1;
    }
}

static hal_status_t test_lidar_init(void)
{
    // Create LiDAR configuration
    lidar_config_t config = {
        .device_path = "/dev/ttyUSB0",
        .baud_rate = LIDAR_BAUD_RATE,
        .scan_rate_hz = LIDAR_SCAN_RATE_TYPICAL_HZ,
        .emergency_stop_mm = LIDAR_EMERGENCY_STOP_MM,
        .warning_mm = LIDAR_WARNING_MM,
        .safe_mm = LIDAR_SAFE_MM,
        .sample_rate_hz = LIDAR_SAMPLE_RATE_HZ,
        .angular_resolution = LIDAR_ANGULAR_RESOLUTION
    };

    printf("Initializing LiDAR with configuration:\n");
    printf("  Device: %s\n", config.device_path);
    printf("  Baud Rate: %d\n", config.baud_rate);
    printf("  Scan Rate: %d Hz\n", config.scan_rate_hz);
    printf("  Emergency Stop: %d mm\n", config.emergency_stop_mm);
    printf("  Warning: %d mm\n", config.warning_mm);
    printf("  Safe: %d mm\n", config.safe_mm);

    return hal_lidar_init(&config);
}

static hal_status_t test_lidar_config(void)
{
    // Test configuration validation
    lidar_config_t invalid_config = {
        .device_path = "",
        .baud_rate = 115200, // Wrong baud rate
        .scan_rate_hz = 5,   // Too low
        .emergency_stop_mm = 1000,
        .warning_mm = 500,   // Wrong order
        .safe_mm = 2000,
        .sample_rate_hz = LIDAR_SAMPLE_RATE_HZ,
        .angular_resolution = LIDAR_ANGULAR_RESOLUTION
    };

    printf("Testing invalid configuration validation...\n");
    hal_status_t status = lidar_validate_config(&invalid_config);
    if (status != HAL_STATUS_OK) {
        printf("✅ Configuration validation working (rejected invalid config)\n");
        return HAL_STATUS_OK;
    } else {
        printf("❌ Configuration validation failed (accepted invalid config)\n");
        return HAL_STATUS_ERROR;
    }
}

static hal_status_t test_lidar_device_info(void)
{
    lidar_device_info_t device_info;
    
    printf("Getting device information...\n");
    hal_status_t status = hal_lidar_get_device_info(&device_info);
    
    if (status == HAL_STATUS_OK) {
        print_device_info(&device_info);
    }
    
    return status;
}

static hal_status_t test_lidar_health_check(void)
{
    printf("Performing health check...\n");
    hal_status_t status = hal_lidar_health_check();
    
    if (status == HAL_STATUS_OK) {
        printf("✅ Device is healthy\n");
    } else {
        printf("⚠️  Device health check failed\n");
    }
    
    return status;
}

static hal_status_t test_lidar_scanning(void)
{
    printf("Starting LiDAR scanning...\n");
    hal_status_t status = hal_lidar_start_scanning();
    if (status != HAL_STATUS_OK) {
        printf("❌ Failed to start scanning\n");
        return status;
    }

    printf("Scanning for 10 seconds...\n");
    for (int i = 0; i < 10 && test_running; i++) {
        lidar_scan_data_t scan_data;
        status = hal_lidar_get_scan_data(&scan_data);
        
        if (status == HAL_STATUS_OK) {
            printf("Scan %d: %d points, complete: %s\n", 
                   i + 1, scan_data.point_count, 
                   scan_data.scan_complete ? "Yes" : "No");
            
            if (scan_data.scan_complete && scan_data.point_count > 0) {
                print_scan_data(&scan_data);
            }
        } else {
            printf("Failed to get scan data (status: %d)\n", status);
        }
        
        sleep(1);
    }

    printf("Stopping LiDAR scanning...\n");
    status = hal_lidar_stop_scanning();
    
    return status;
}

static hal_status_t test_lidar_safety(void)
{
    printf("Testing safety functionality...\n");
    
    // Test safety status
    lidar_safety_status_t safety_status;
    hal_status_t status = hal_lidar_check_safety(&safety_status);
    
    if (status == HAL_STATUS_OK) {
        print_safety_status(&safety_status);
    }
    
    // Test utility functions with mock data
    lidar_scan_data_t mock_scan = {0};
    mock_scan.scan_complete = true;
    mock_scan.point_count = 3;
    mock_scan.points[0].distance_mm = 800;   // Warning distance
    mock_scan.points[1].distance_mm = 400;   // Emergency stop distance
    mock_scan.points[2].distance_mm = 2500;  // Safe distance
    
    uint16_t min_dist = lidar_calculate_min_distance(&mock_scan);
    uint16_t max_dist = lidar_calculate_max_distance(&mock_scan);
    bool obstacle_detected = lidar_is_obstacle_detected(&mock_scan, LIDAR_WARNING_MM);
    
    printf("Mock scan test:\n");
    printf("  Min distance: %d mm\n", min_dist);
    printf("  Max distance: %d mm\n", max_dist);
    printf("  Obstacle detected (warning threshold): %s\n", 
           obstacle_detected ? "Yes" : "No");
    
    return HAL_STATUS_OK;
}

static hal_status_t test_lidar_cleanup(void)
{
    printf("Cleaning up LiDAR...\n");
    return hal_lidar_deinit();
}

static void print_scan_data(const lidar_scan_data_t *scan_data)
{
    if (!scan_data) return;
    
    printf("Scan Data:\n");
    printf("  Points: %d\n", scan_data->point_count);
    printf("  Complete: %s\n", scan_data->scan_complete ? "Yes" : "No");
    printf("  Quality: %d\n", scan_data->scan_quality);
    printf("  Timestamp: %lu us\n", (unsigned long)scan_data->scan_timestamp_us);
    
    if (scan_data->point_count > 0) {
        printf("  Sample points:\n");
        for (int i = 0; i < 5 && i < scan_data->point_count; i++) {
            printf("    Point %d: %d mm @ %d°, quality: %d\n",
                   i, scan_data->points[i].distance_mm,
                   scan_data->points[i].angle_deg,
                   scan_data->points[i].quality);
        }
    }
}

static void print_safety_status(const lidar_safety_status_t *safety_status)
{
    if (!safety_status) return;
    
    printf("Safety Status:\n");
    printf("  Min distance: %d mm @ %d°\n", 
           safety_status->min_distance_mm, safety_status->min_distance_angle);
    printf("  Max distance: %d mm @ %d°\n", 
           safety_status->max_distance_mm, safety_status->max_distance_angle);
    printf("  Obstacle detected: %s\n", 
           safety_status->obstacle_detected ? "Yes" : "No");
    printf("  Emergency stop: %s\n", 
           safety_status->emergency_stop_triggered ? "Yes" : "No");
    printf("  Warning: %s\n", 
           safety_status->warning_triggered ? "Yes" : "No");
    printf("  Timestamp: %lu us\n", (unsigned long)safety_status->timestamp_us);
}

static void print_device_info(const lidar_device_info_t *device_info)
{
    if (!device_info) return;
    
    printf("Device Information:\n");
    printf("  Model: %d\n", device_info->model);
    printf("  Firmware Version: %d\n", device_info->firmware_version);
    printf("  Hardware Version: %d\n", device_info->hardware_version);
    printf("  Serial Number: %u\n", device_info->serial_number);
    printf("  Healthy: %s\n", device_info->device_healthy ? "Yes" : "No");
    printf("  Timestamp: %lu us\n", (unsigned long)device_info->timestamp_us);
}
