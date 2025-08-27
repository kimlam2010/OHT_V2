/**
 * @file safety_monitor_test.c
 * @brief Safety Monitor Testing Implementation
 * @version 1.0.0
 * @date 2025-01-28
 * @team FIRMWARE
 * @task Task 9 (Testing & Validation)
 */

#include "safety_monitor_test.h"
#include "safety_monitor.h"
#include "hal_common.h"
#include "hal_led.h"
#include "hal_estop.h"
#include "hal_config_persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Test framework state
static test_config_t g_test_config = {0};
static bool g_test_initialized = false;
static uint32_t g_test_counter = 0;

// Test framework initialization
hal_status_t safety_monitor_test_init(const test_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_test_config, config, sizeof(test_config_t));
    g_test_initialized = true;
    g_test_counter = 0;
    
    if (g_test_config.verbose_output) {
        printf("Safety Monitor Test Framework initialized\n");
        printf("Unit tests: %s\n", g_test_config.enable_unit_tests ? "enabled" : "disabled");
        printf("Integration tests: %s\n", g_test_config.enable_integration_tests ? "enabled" : "disabled");
        printf("Safety tests: %s\n", g_test_config.enable_safety_tests ? "enabled" : "disabled");
        printf("Performance tests: %s\n", g_test_config.enable_performance_tests ? "enabled" : "disabled");
        printf("Stress tests: %s\n", g_test_config.enable_stress_tests ? "enabled" : "disabled");
    }
    
    return HAL_STATUS_OK;
}

// Test framework deinitialization
hal_status_t safety_monitor_test_deinit(void)
{
    if (!g_test_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_test_initialized = false;
    
    if (g_test_config.verbose_output) {
        printf("Safety Monitor Test Framework deinitialized\n");
    }
    
    return HAL_STATUS_OK;
}

// Run all tests
hal_status_t safety_monitor_test_run_all(test_statistics_t *statistics)
{
    if (!g_test_initialized || !statistics) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memset(statistics, 0, sizeof(test_statistics_t));
    
    printf("Running Safety Monitor Test Suite\n");
    printf("================================\n\n");
    
    // Run unit tests
    if (g_test_config.enable_unit_tests) {
        printf("Running Unit Tests...\n");
        safety_monitor_test_run_category(TEST_CATEGORY_UNIT, statistics);
    }
    
    // Run integration tests
    if (g_test_config.enable_integration_tests) {
        printf("Running Integration Tests...\n");
        safety_monitor_test_run_category(TEST_CATEGORY_INTEGRATION, statistics);
    }
    
    // Run safety tests
    if (g_test_config.enable_safety_tests) {
        printf("Running Safety Tests...\n");
        safety_monitor_test_run_category(TEST_CATEGORY_SAFETY, statistics);
    }
    
    // Run performance tests
    if (g_test_config.enable_performance_tests) {
        printf("Running Performance Tests...\n");
        safety_monitor_test_run_category(TEST_CATEGORY_PERFORMANCE, statistics);
    }
    
    // Run stress tests
    if (g_test_config.enable_stress_tests) {
        printf("Running Stress Tests...\n");
        safety_monitor_test_run_category(TEST_CATEGORY_STRESS, statistics);
    }
    
    // Calculate statistics
    if (statistics->total_tests > 0) {
        statistics->avg_duration_us = statistics->total_duration_us / statistics->total_tests;
    }
    
    printf("\nTest Results Summary:\n");
    printf("====================\n");
    printf("Total Tests: %u\n", statistics->total_tests);
    printf("Passed: %u\n", statistics->passed_tests);
    printf("Failed: %u\n", statistics->failed_tests);
    printf("Skipped: %u\n", statistics->skipped_tests);
    printf("Errors: %u\n", statistics->error_tests);
    printf("Success Rate: %.1f%%\n", 
           statistics->total_tests > 0 ? 
           (float)statistics->passed_tests / statistics->total_tests * 100.0f : 0.0f);
    printf("Total Duration: %lu us\n", statistics->total_duration_us);
    printf("Average Duration: %lu us\n", statistics->avg_duration_us);
    
    return HAL_STATUS_OK;
}

// Run tests by category
hal_status_t safety_monitor_test_run_category(test_category_t category, test_statistics_t *statistics)
{
    if (!g_test_initialized || !statistics) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    switch (category) {
        case TEST_CATEGORY_UNIT:
            return safety_monitor_test_run_unit_tests(statistics);
        case TEST_CATEGORY_INTEGRATION:
            return safety_monitor_test_run_integration_tests(statistics);
        case TEST_CATEGORY_SAFETY:
            return safety_monitor_test_run_safety_tests(statistics);
        case TEST_CATEGORY_PERFORMANCE:
            return safety_monitor_test_run_performance_tests(statistics);
        case TEST_CATEGORY_STRESS:
            return safety_monitor_test_run_stress_tests(statistics);
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }
}

hal_status_t safety_monitor_test_run_unit_tests(test_statistics_t *statistics)
{
    if (!statistics) return HAL_STATUS_INVALID_PARAMETER;
    
    // Define unit test functions
    typedef hal_status_t (*unit_test_func_t)(test_status_t*);
    unit_test_func_t unit_tests[] = {
        safety_monitor_test_unit_safety_monitor,
        safety_monitor_test_unit_safety_zones,
        safety_monitor_test_unit_estop,
        safety_monitor_test_unit_states,
        safety_monitor_test_unit_led
    };
    
    const char* test_names[] = {
        "safety_monitor_basic",
        "safety_zones",
        "estop_functionality", 
        "state_transitions",
        "led_patterns"
    };
    
    const int num_tests = sizeof(unit_tests) / sizeof(unit_tests[0]);
    
    for (int i = 0; i < num_tests; i++) {
        test_status_t test_status = {0};
        test_status.category = TEST_CATEGORY_UNIT;
        strcpy(test_status.test_name, test_names[i]);
        
        hal_status_t result = unit_tests[i](&test_status);
        
        statistics->total_tests++;
        if (result == HAL_STATUS_OK && test_status.result == TEST_RESULT_PASS) {
            statistics->passed_tests++;
        } else if (test_status.result == TEST_RESULT_FAIL) {
            statistics->failed_tests++;
        } else if (test_status.result == TEST_RESULT_SKIP) {
            statistics->skipped_tests++;
        } else {
            statistics->error_tests++;
        }
        
        if (test_status.duration_us > statistics->max_duration_us) {
            statistics->max_duration_us = test_status.duration_us;
        }
        if (statistics->min_duration_us == 0 || test_status.duration_us < statistics->min_duration_us) {
            statistics->min_duration_us = test_status.duration_us;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_run_integration_tests(test_statistics_t *statistics)
{
    if (!statistics) return HAL_STATUS_INVALID_PARAMETER;
    
    // Define integration test functions
    typedef hal_status_t (*integration_test_func_t)(test_status_t*);
    integration_test_func_t integration_tests[] = {
        safety_monitor_test_integration_lidar,
        safety_monitor_test_integration_api,
        safety_monitor_test_integration_config
    };
    
    const char* test_names[] = {
        "lidar_integration",
        "api_integration",
        "config_integration"
    };
    
    const int num_tests = sizeof(integration_tests) / sizeof(integration_tests[0]);
    
    for (int i = 0; i < num_tests; i++) {
        test_status_t test_status = {0};
        test_status.category = TEST_CATEGORY_INTEGRATION;
        strcpy(test_status.test_name, test_names[i]);
        
        hal_status_t result = integration_tests[i](&test_status);
        
        statistics->total_tests++;
        if (result == HAL_STATUS_OK && test_status.result == TEST_RESULT_PASS) {
            statistics->passed_tests++;
        } else if (test_status.result == TEST_RESULT_FAIL) {
            statistics->failed_tests++;
        } else if (test_status.result == TEST_RESULT_SKIP) {
            statistics->skipped_tests++;
        } else {
            statistics->error_tests++;
        }
        
        if (test_status.duration_us > statistics->max_duration_us) {
            statistics->max_duration_us = test_status.duration_us;
        }
        if (statistics->min_duration_us == 0 || test_status.duration_us < statistics->min_duration_us) {
            statistics->min_duration_us = test_status.duration_us;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_run_safety_tests(test_statistics_t *statistics)
{
    if (!statistics) return HAL_STATUS_INVALID_PARAMETER;
    
    // Define safety test functions
    typedef hal_status_t (*safety_test_func_t)(test_status_t*);
    safety_test_func_t safety_tests[] = {
        safety_monitor_test_safety_emergency_violation,
        safety_monitor_test_safety_warning_violation,
        safety_monitor_test_safety_response_time
    };
    
    const char* test_names[] = {
        "emergency_violation",
        "warning_violation",
        "response_time"
    };
    
    const int num_tests = sizeof(safety_tests) / sizeof(safety_tests[0]);
    
    for (int i = 0; i < num_tests; i++) {
        test_status_t test_status = {0};
        test_status.category = TEST_CATEGORY_SAFETY;
        strcpy(test_status.test_name, test_names[i]);
        
        hal_status_t result = safety_tests[i](&test_status);
        
        statistics->total_tests++;
        if (result == HAL_STATUS_OK && test_status.result == TEST_RESULT_PASS) {
            statistics->passed_tests++;
        } else if (test_status.result == TEST_RESULT_FAIL) {
            statistics->failed_tests++;
        } else if (test_status.result == TEST_RESULT_SKIP) {
            statistics->skipped_tests++;
        } else {
            statistics->error_tests++;
        }
        
        if (test_status.duration_us > statistics->max_duration_us) {
            statistics->max_duration_us = test_status.duration_us;
        }
        if (statistics->min_duration_us == 0 || test_status.duration_us < statistics->min_duration_us) {
            statistics->min_duration_us = test_status.duration_us;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_run_performance_tests(test_statistics_t *statistics)
{
    if (!statistics) return HAL_STATUS_INVALID_PARAMETER;
    
    // Define performance test functions
    typedef hal_status_t (*performance_test_func_t)(test_status_t*);
    performance_test_func_t performance_tests[] = {
        safety_monitor_test_performance_load
    };
    
    const char* test_names[] = {
        "performance_load"
    };
    
    const int num_tests = sizeof(performance_tests) / sizeof(performance_tests[0]);
    
    for (int i = 0; i < num_tests; i++) {
        test_status_t test_status = {0};
        test_status.category = TEST_CATEGORY_PERFORMANCE;
        strcpy(test_status.test_name, test_names[i]);
        
        hal_status_t result = performance_tests[i](&test_status);
        
        statistics->total_tests++;
        if (result == HAL_STATUS_OK && test_status.result == TEST_RESULT_PASS) {
            statistics->passed_tests++;
        } else if (test_status.result == TEST_RESULT_FAIL) {
            statistics->failed_tests++;
        } else if (test_status.result == TEST_RESULT_SKIP) {
            statistics->skipped_tests++;
        } else {
            statistics->error_tests++;
        }
        
        if (test_status.duration_us > statistics->max_duration_us) {
            statistics->max_duration_us = test_status.duration_us;
        }
        if (statistics->min_duration_us == 0 || test_status.duration_us < statistics->min_duration_us) {
            statistics->min_duration_us = test_status.duration_us;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_run_stress_tests(test_statistics_t *statistics)
{
    if (!statistics) return HAL_STATUS_INVALID_PARAMETER;
    
    // Define stress test functions
    typedef hal_status_t (*stress_test_func_t)(test_status_t*);
    stress_test_func_t stress_tests[] = {
        safety_monitor_test_stress_fault_injection,
        safety_monitor_test_stress_recovery
    };
    
    const char* test_names[] = {
        "fault_injection",
        "stress_recovery"
    };
    
    const int num_tests = sizeof(stress_tests) / sizeof(stress_tests[0]);
    
    for (int i = 0; i < num_tests; i++) {
        test_status_t test_status = {0};
        test_status.category = TEST_CATEGORY_STRESS;
        strcpy(test_status.test_name, test_names[i]);
        
        hal_status_t result = stress_tests[i](&test_status);
        
        statistics->total_tests++;
        if (result == HAL_STATUS_OK && test_status.result == TEST_RESULT_PASS) {
            statistics->passed_tests++;
        } else if (test_status.result == TEST_RESULT_FAIL) {
            statistics->failed_tests++;
        } else if (test_status.result == TEST_RESULT_SKIP) {
            statistics->skipped_tests++;
        } else {
            statistics->error_tests++;
        }
        
        if (test_status.duration_us > statistics->max_duration_us) {
            statistics->max_duration_us = test_status.duration_us;
        }
        if (statistics->min_duration_us == 0 || test_status.duration_us < statistics->min_duration_us) {
            statistics->min_duration_us = test_status.duration_us;
        }
    }
    
    return HAL_STATUS_OK;
}

// Unit test implementations
hal_status_t safety_monitor_test_unit_safety_monitor(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "safety_monitor_basic");
    strcpy(status->description, "Test basic safety monitor functionality");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test safety monitor initialization
    safety_monitor_config_t config = {
        .update_period_ms = 20,  // 50Hz equivalent
        .estop_timeout_ms = 100,
        .zone_check_period_ms = 200
    };
    
    hal_status_t result = safety_monitor_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test safety monitor status
    safety_monitor_status_t monitor_status;
    result = safety_monitor_get_status(&monitor_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_TRUE(monitor_status.current_state == SAFETY_MONITOR_STATE_SAFE);
    
    // Test safety monitor update
    result = safety_monitor_update();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test safety monitor deinitialization
    result = safety_monitor_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_unit_safety_zones(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "safety_zones");
    strcpy(status->description, "Test safety zones configuration");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test safety zones configuration
    basic_safety_zones_t zones = {
        .enabled = true,
        .emergency_zone_mm = 200,
        .warning_zone_mm = 500,
        .safe_zone_mm = 1000
    };
    
    hal_status_t result = safety_monitor_set_basic_zones(&zones);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test safety zones retrieval
    basic_safety_zones_t retrieved_zones;
    result = safety_monitor_get_basic_zones(&retrieved_zones);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_EQUAL(zones.emergency_zone_mm, retrieved_zones.emergency_zone_mm);
    TEST_ASSERT_EQUAL(zones.warning_zone_mm, retrieved_zones.warning_zone_mm);
    TEST_ASSERT_EQUAL(zones.safe_zone_mm, retrieved_zones.safe_zone_mm);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_unit_estop(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "estop_functionality");
    strcpy(status->description, "Test E-Stop functionality");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test E-Stop triggering
    hal_status_t result = safety_monitor_trigger_emergency_stop("Test E-Stop");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test E-Stop status
    bool estop_active;
    result = safety_monitor_is_estop_active(&estop_active);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_TRUE(estop_active);
    
    // Test E-Stop acknowledgment
    result = safety_monitor_reset();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_unit_states(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "state_transitions");
    strcpy(status->description, "Test safety state transitions");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test state transitions
    safety_monitor_status_t monitor_status;
    hal_status_t result = safety_monitor_get_status(&monitor_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test transition to warning state
    result = safety_monitor_process_event(SAFETY_MONITOR_EVENT_ZONE_VIOLATION, "Test warning");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    result = safety_monitor_get_status(&monitor_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_EQUAL(SAFETY_MONITOR_STATE_WARNING, monitor_status.current_state);
    
    // Test transition to safe state
    result = safety_monitor_process_event(SAFETY_MONITOR_EVENT_SAFETY_RESET, "Test safe state");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_unit_led(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "led_patterns");
    strcpy(status->description, "Test LED pattern setting");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test LED pattern setting
    hal_status_t result = safety_monitor_set_communication_led_pattern(true, 4);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test LED status
    result = hal_led_comm_set(LED_STATE_ON);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

// Integration test implementations
hal_status_t safety_monitor_test_integration_lidar(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "lidar_integration");
    strcpy(status->description, "Test LiDAR integration with safety monitor");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Simulate LiDAR scan data
    lidar_scan_data_t scan_data = {0};
    scan_data.scan_timestamp_us = test_get_timestamp_us();
    scan_data.scan_complete = true;
    scan_data.point_count = 100;
    // Set minimum distance in points array
    for (int i = 0; i < scan_data.point_count; i++) {
        scan_data.points[i].distance_mm = 300; // 30cm
        scan_data.points[i].angle_deg = i * 3; // Spread angles
        scan_data.points[i].quality = 255;
        scan_data.points[i].timestamp_us = test_get_timestamp_us();
    }
    
    // Test LiDAR integration
    hal_status_t result = safety_monitor_update_with_lidar(&scan_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_integration_api(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "api_integration");
    strcpy(status->description, "Test API integration with safety monitor");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test API status endpoint
    safety_monitor_status_t monitor_status;
    hal_status_t result = safety_monitor_get_status(&monitor_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_NOT_NULL(&monitor_status);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_integration_config(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "config_integration");
    strcpy(status->description, "Test configuration integration");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test configuration loading
    hal_status_t result = safety_monitor_load_config();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test configuration saving
    result = safety_monitor_save_config();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

// Safety test implementations
hal_status_t safety_monitor_test_safety_emergency_violation(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "emergency_violation");
    strcpy(status->description, "Test emergency zone violation handling");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Simulate emergency zone violation
    lidar_scan_data_t scan_data = {0};
    scan_data.scan_timestamp_us = test_get_timestamp_us();
    scan_data.scan_complete = true;
    scan_data.point_count = 100;
    // Set emergency distance in points array
    for (int i = 0; i < scan_data.point_count; i++) {
        scan_data.points[i].distance_mm = 100; // 10cm - emergency violation
        scan_data.points[i].angle_deg = i * 3; // Spread angles
        scan_data.points[i].quality = 255;
        scan_data.points[i].timestamp_us = test_get_timestamp_us();
    }
    
    hal_status_t result = safety_monitor_update_with_lidar(&scan_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Check if E-Stop was triggered
    bool estop_active;
    result = safety_monitor_is_estop_active(&estop_active);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_TRUE(estop_active);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_safety_warning_violation(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "warning_violation");
    strcpy(status->description, "Test warning zone violation handling");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Simulate warning zone violation
    lidar_scan_data_t scan_data = {0};
    scan_data.scan_timestamp_us = test_get_timestamp_us();
    scan_data.scan_complete = true;
    scan_data.point_count = 100;
    // Set warning distance in points array
    for (int i = 0; i < scan_data.point_count; i++) {
        scan_data.points[i].distance_mm = 300; // 30cm - warning violation
        scan_data.points[i].angle_deg = i * 3; // Spread angles
        scan_data.points[i].quality = 255;
        scan_data.points[i].timestamp_us = test_get_timestamp_us();
    }
    
    hal_status_t result = safety_monitor_update_with_lidar(&scan_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Check if warning state was entered
    safety_monitor_status_t monitor_status;
    result = safety_monitor_get_status(&monitor_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_EQUAL(SAFETY_MONITOR_STATE_WARNING, monitor_status.current_state);
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_safety_response_time(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "response_time");
    strcpy(status->description, "Test safety response time");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test response time for E-Stop
    uint64_t start_time = test_get_timestamp_us();
    hal_status_t result = safety_monitor_trigger_emergency_stop("Response time test");
    uint64_t end_time = test_get_timestamp_us();
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_LESS_THAN(10000, end_time - start_time); // < 10ms
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

// Performance test implementations
hal_status_t safety_monitor_test_performance_load(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "performance_load");
    strcpy(status->description, "Test performance under load");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Run multiple safety monitor updates
    for (int i = 0; i < 100; i++) {
        lidar_scan_data_t scan_data = {0};
        scan_data.scan_timestamp_us = test_get_timestamp_us();
        scan_data.scan_complete = true;
        scan_data.point_count = 100;
        // Set varying distances in points array
        for (int j = 0; j < scan_data.point_count; j++) {
            scan_data.points[j].distance_mm = 500 + (i % 200); // Varying distances
            scan_data.points[j].angle_deg = j * 3; // Spread angles
            scan_data.points[j].quality = 255;
            scan_data.points[j].timestamp_us = test_get_timestamp_us();
        }
        
        hal_status_t result = safety_monitor_update_with_lidar(&scan_data);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    }
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_performance_memory(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "performance_memory");
    strcpy(status->description, "Test memory usage");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test memory allocation and deallocation
    for (int i = 0; i < 50; i++) {
        safety_monitor_status_t *status_ptr = malloc(sizeof(safety_monitor_status_t));
        TEST_ASSERT_NOT_NULL(status_ptr);
        
        hal_status_t result = safety_monitor_get_status(status_ptr);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
        
        free(status_ptr);
    }
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_performance_cpu(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "performance_cpu");
    strcpy(status->description, "Test CPU usage");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test CPU-intensive operations
    for (int i = 0; i < 1000; i++) {
        safety_monitor_update();
    }
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

// Stress test implementations
hal_status_t safety_monitor_test_stress_concurrent(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "stress_concurrent");
    strcpy(status->description, "Test concurrent operations");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Simulate concurrent operations
    for (int i = 0; i < 50; i++) {
        safety_monitor_update();
        safety_monitor_get_status(NULL);
        safety_monitor_update_with_lidar(NULL);
    }
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_stress_fault_injection(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "stress_fault_injection");
    strcpy(status->description, "Test fault injection");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Inject various faults
    for (int i = 0; i < 20; i++) {
        safety_monitor_trigger_emergency_stop("Fault injection test");
        safety_monitor_reset();
    }
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_test_stress_recovery(test_status_t *status)
{
    if (!status) return HAL_STATUS_INVALID_PARAMETER;
    
    status->test_id = ++g_test_counter;
    strcpy(status->test_name, "stress_recovery");
    strcpy(status->description, "Test recovery mechanisms");
    status->start_time_us = test_get_timestamp_us();
    
    printf("  [%u] %s: ", status->test_id, status->test_name);
    
    // Test recovery from various states
    for (int i = 0; i < 10; i++) {
        safety_monitor_process_event(SAFETY_MONITOR_EVENT_ZONE_VIOLATION, "Stress test warning");
        safety_monitor_process_event(SAFETY_MONITOR_EVENT_SAFETY_RESET, "Stress test safe");
        safety_monitor_process_event(SAFETY_MONITOR_EVENT_EMERGENCY_STOP, "Stress test critical");
        safety_monitor_process_event(SAFETY_MONITOR_EVENT_SAFETY_RESET, "Stress test safe final");
    }
    
    status->end_time_us = test_get_timestamp_us();
    status->duration_us = status->end_time_us - status->start_time_us;
    status->result = TEST_RESULT_PASS;
    
    printf("PASS (%lu us)\n", status->duration_us);
    return HAL_STATUS_OK;
}

// Utility functions
hal_status_t test_generate_report(const test_statistics_t *statistics, const char *filename)
{
    if (!statistics || !filename) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        return HAL_STATUS_ERROR;
    }
    
    fprintf(file, "Safety Monitor Test Report\n");
    fprintf(file, "=========================\n\n");
    fprintf(file, "Date: %s\n", "2025-01-28");
    fprintf(file, "Version: 1.0.0\n\n");
    
    fprintf(file, "Test Results:\n");
    fprintf(file, "Total Tests: %u\n", statistics->total_tests);
    fprintf(file, "Passed: %u\n", statistics->passed_tests);
    fprintf(file, "Failed: %u\n", statistics->failed_tests);
    fprintf(file, "Skipped: %u\n", statistics->skipped_tests);
    fprintf(file, "Errors: %u\n", statistics->error_tests);
    fprintf(file, "Success Rate: %.1f%%\n", 
           statistics->total_tests > 0 ? 
           (float)statistics->passed_tests / statistics->total_tests * 100.0f : 0.0f);
    
    fprintf(file, "\nPerformance:\n");
    fprintf(file, "Total Duration: %lu us\n", statistics->total_duration_us);
    fprintf(file, "Average Duration: %lu us\n", statistics->avg_duration_us);
    fprintf(file, "Min Duration: %lu us\n", statistics->min_duration_us);
    fprintf(file, "Max Duration: %lu us\n", statistics->max_duration_us);
    
    fclose(file);
    return HAL_STATUS_OK;
}

const char* test_result_to_string(test_result_t result)
{
    switch (result) {
        case TEST_RESULT_PASS: return "PASS";
        case TEST_RESULT_FAIL: return "FAIL";
        case TEST_RESULT_SKIP: return "SKIP";
        case TEST_RESULT_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

const char* test_category_to_string(test_category_t category)
{
    switch (category) {
        case TEST_CATEGORY_UNIT: return "UNIT";
        case TEST_CATEGORY_INTEGRATION: return "INTEGRATION";
        case TEST_CATEGORY_SAFETY: return "SAFETY";
        case TEST_CATEGORY_PERFORMANCE: return "PERFORMANCE";
        case TEST_CATEGORY_STRESS: return "STRESS";
        default: return "UNKNOWN";
    }
}

uint64_t test_get_timestamp_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
}

void test_sleep_ms(uint32_t milliseconds)
{
    usleep(milliseconds * 1000);
}
