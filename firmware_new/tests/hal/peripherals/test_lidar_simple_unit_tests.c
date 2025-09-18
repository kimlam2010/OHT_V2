/**
 * @file test_lidar_simple_unit_tests.c
 * @brief Simple Unit Tests for LiDAR HAL v2.3.0
 * @version 2.3.0
 * @date 2025-01-28
 * @team EMBED
 * 
 * This file contains simple unit tests for LiDAR HAL features:
 * - Basic functionality tests
 * - Configuration tests
 * - Error handling tests
 * - Performance tests
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "hal_lidar.h"
#include "hal_common.h"

// Test configuration
#define MAX_TEST_ITERATIONS    1000
#define PERFORMANCE_TEST_ITERATIONS 10000

// Test statistics
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    double total_time_ms;
} test_stats_t;

// Global test statistics
static test_stats_t g_test_stats = {0};

// Test utilities
static void test_start(const char* test_name);
static void test_end(const char* test_name, bool passed);
static double get_time_ms(void);
static void print_test_summary(void);

// Test categories
static void test_basic_functionality(void);
static void test_configuration(void);
static void test_error_handling(void);
static void test_performance(void);

// Individual test functions
static bool test_lidar_initialization(void);
static bool test_lidar_configuration(void);
static bool test_lidar_status(void);
static bool test_lidar_reset(void);
static bool test_invalid_parameters(void);
static bool test_performance_basic(void);

int main(void) {
    printf("🧪 LIDAR HAL SIMPLE UNIT TESTS v2.3.0\n");
    printf("======================================\n");
    printf("Testing basic LiDAR HAL functionality\n");
    printf("Date: 2025-01-28\n");
    printf("Team: EMBED\n\n");

    // Run all test categories
    printf("🚀 STARTING SIMPLE UNIT TESTS\n");
    printf("==============================\n\n");

    test_basic_functionality();
    test_configuration();
    test_error_handling();
    test_performance();

    // Print final results
    printf("\n🏁 UNIT TEST RESULTS SUMMARY\n");
    printf("============================\n");
    print_test_summary();

    printf("\n🎯 LiDAR HAL v2.3.0 Simple Unit Tests Complete!\n");
    return (g_test_stats.failed_tests > 0) ? -1 : 0;
}

// Test utilities implementation
static void test_start(const char* test_name) {
    printf("🧪 Testing: %s\n", test_name);
    g_test_stats.total_tests++;
}

static void test_end(const char* test_name, bool passed) {
    if (passed) {
        printf("   ✅ PASSED: %s\n", test_name);
        g_test_stats.passed_tests++;
    } else {
        printf("   ❌ FAILED: %s\n", test_name);
        g_test_stats.failed_tests++;
    }
}

static double get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

static void print_test_summary(void) {
    printf("📊 Test Statistics:\n");
    printf("   • Total Tests: %d\n", g_test_stats.total_tests);
    printf("   • Passed: %d (%.1f%%)\n", g_test_stats.passed_tests, 
           (double)g_test_stats.passed_tests / g_test_stats.total_tests * 100.0);
    printf("   • Failed: %d (%.1f%%)\n", g_test_stats.failed_tests,
           (double)g_test_stats.failed_tests / g_test_stats.total_tests * 100.0);
    printf("   • Success Rate: %.1f%%\n", 
           (double)g_test_stats.passed_tests / g_test_stats.total_tests * 100.0);
}

// Basic Functionality Tests
static void test_basic_functionality(void) {
    printf("🔧 BASIC FUNCTIONALITY TESTS\n");
    printf("============================\n");

    test_start("LiDAR Initialization");
    bool result = test_lidar_initialization();
    test_end("LiDAR Initialization", result);

    test_start("LiDAR Status");
    result = test_lidar_status();
    test_end("LiDAR Status", result);

    test_start("LiDAR Reset");
    result = test_lidar_reset();
    test_end("LiDAR Reset", result);

    printf("\n");
}

static bool test_lidar_initialization(void) {
    // Test basic initialization
    lidar_config_t config = {
        .device_path = "/dev/ttyUSB0",
        .baud_rate = 460800,
        .scan_rate_hz = 10,
        .emergency_stop_mm = 500,
        .warning_mm = 1000,
        .safe_mm = 2000,
        .sample_rate_hz = 5000,
        .angular_resolution = 0.72f
    };

    hal_status_t status = hal_lidar_init(&config);
    if (status != HAL_STATUS_OK) {
        printf("      ❌ Initialization failed: %d\n", status);
        return false;
    }

    printf("      ✅ LiDAR initialized successfully\n");
    return true;
}

static bool test_lidar_status(void) {
    lidar_device_info_t device_info;
    hal_status_t result = hal_lidar_get_device_info(&device_info);
    
    if (result != HAL_STATUS_OK) {
        printf("      ❌ Device info retrieval failed: %d\n", result);
        return false;
    }

    printf("      ✅ Device info retrieved: model=%d, firmware=%d, hardware=%d, healthy=%d\n", 
           device_info.model, device_info.firmware_version, device_info.hardware_version, device_info.device_healthy);
    return true;
}

static bool test_lidar_reset(void) {
    hal_status_t status = hal_lidar_reset();
    if (status != HAL_STATUS_OK) {
        printf("      ❌ Reset failed: %d\n", status);
        return false;
    }

    printf("      ✅ LiDAR reset successfully\n");
    return true;
}

// Configuration Tests
static void test_configuration(void) {
    printf("⚙️ CONFIGURATION TESTS\n");
    printf("======================\n");

    test_start("LiDAR Configuration");
    bool result = test_lidar_configuration();
    test_end("LiDAR Configuration", result);

    printf("\n");
}

static bool test_lidar_configuration(void) {
    // Test basic configuration validation
    lidar_config_t config = {
        .device_path = "/dev/ttyUSB0",
        .baud_rate = 460800,
        .scan_rate_hz = 10,
        .emergency_stop_mm = 500,
        .warning_mm = 1000,
        .safe_mm = 2000,
        .sample_rate_hz = 5000,
        .angular_resolution = 0.72f
    };

    // Test configuration validation
    hal_status_t status = lidar_validate_config(&config);
    if (status != HAL_STATUS_OK) {
        printf("      ❌ Configuration validation failed: %d\n", status);
        return false;
    }

    printf("      ✅ Configuration validated: baud=%d, scan_rate=%dHz, resolution=%.2f°\n",
           config.baud_rate, config.scan_rate_hz, config.angular_resolution);
    return true;
}

// Error Handling Tests
static void test_error_handling(void) {
    printf("⚠️ ERROR HANDLING TESTS\n");
    printf("=======================\n");

    test_start("Invalid Parameter Handling");
    bool result = test_invalid_parameters();
    test_end("Invalid Parameter Handling", result);

    printf("\n");
}

static bool test_invalid_parameters(void) {
    bool all_passed = true;

    // Test NULL parameter handling
    hal_status_t status = lidar_validate_config(NULL);
    if (status != HAL_STATUS_INVALID_PARAMETER) {
        printf("      ❌ NULL config not handled correctly: %d\n", status);
        all_passed = false;
    }

    status = hal_lidar_get_device_info(NULL);
    if (status != HAL_STATUS_INVALID_PARAMETER) {
        printf("      ❌ NULL device info not handled correctly: %d\n", status);
        all_passed = false;
    }

    // Test invalid configuration values
    lidar_config_t invalid_config = {
        .device_path = "/dev/ttyUSB0",
        .baud_rate = 0,  // Invalid baud rate
        .scan_rate_hz = 0,  // Invalid scan rate
        .emergency_stop_mm = 0,
        .warning_mm = 0,
        .safe_mm = 0,
        .sample_rate_hz = 0,
        .angular_resolution = 0.0f
    };

    status = lidar_validate_config(&invalid_config);
    if (status != HAL_STATUS_INVALID_PARAMETER) {
        printf("      ❌ Invalid config not handled correctly: %d\n", status);
        all_passed = false;
    }

    if (all_passed) {
        printf("      ✅ All error handling tests passed\n");
    }

    return all_passed;
}

// Performance Tests
static void test_performance(void) {
    printf("⚡ PERFORMANCE TESTS\n");
    printf("===================\n");

    test_start("Basic Performance");
    bool result = test_performance_basic();
    test_end("Basic Performance", result);

    printf("\n");
}

static bool test_performance_basic(void) {
    double total_time = 0.0;
    int successful_operations = 0;

    // Test multiple operations
    for (int i = 0; i < PERFORMANCE_TEST_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        lidar_device_info_t device_info;
        hal_status_t result = hal_lidar_get_device_info(&device_info);
        
        double end_time = get_time_ms();
        
        if (result == HAL_STATUS_OK) {
            total_time += (end_time - start_time);
            successful_operations++;
        }
    }

    if (successful_operations == 0) {
        printf("      ❌ No successful operations\n");
        return false;
    }

    double avg_time = total_time / successful_operations;
    printf("      ✅ Performance: %d operations, avg=%.3fms, total=%.3fms\n",
           successful_operations, avg_time, total_time);

    // Check if performance is reasonable (should be < 10ms per operation)
    if (avg_time > 10.0) {
        printf("      ⚠️ Performance warning: avg time %.3fms > 10ms\n", avg_time);
    }

    return true;
}
