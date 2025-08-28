/**
 * @file safety_monitor_test.h
 * @brief Safety Monitor Testing Framework
 * @version 1.0.0
 * @date 2025-01-28
 * @team FIRMWARE
 * @task Task 9 (Testing & Validation)
 */

#ifndef SAFETY_MONITOR_TEST_H
#define SAFETY_MONITOR_TEST_H

#include "safety_monitor.h"
#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// Test result codes
typedef enum {
    TEST_RESULT_PASS = 0,
    TEST_RESULT_FAIL,
    TEST_RESULT_SKIP,
    TEST_RESULT_ERROR
} test_result_t;

// Test categories
typedef enum {
    TEST_CATEGORY_UNIT = 0,
    TEST_CATEGORY_INTEGRATION,
    TEST_CATEGORY_SAFETY,
    TEST_CATEGORY_PERFORMANCE,
    TEST_CATEGORY_STRESS
} test_category_t;

// Test status
typedef struct {
    test_result_t result;
    uint32_t test_id;
    test_category_t category;
    char test_name[64];
    char description[256];
    uint64_t start_time_us;
    uint64_t end_time_us;
    uint64_t duration_us;
    char error_message[512];
    uint32_t line_number;
    char file_name[64];
} test_status_t;

// Test configuration
typedef struct {
    bool enable_unit_tests;
    bool enable_integration_tests;
    bool enable_safety_tests;
    bool enable_performance_tests;
    bool enable_stress_tests;
    uint32_t max_test_duration_ms;
    uint32_t safety_test_timeout_ms;
    bool stop_on_failure;
    bool verbose_output;
} test_config_t;

// Test statistics
typedef struct {
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t skipped_tests;
    uint32_t error_tests;
    uint64_t total_duration_us;
    uint64_t avg_duration_us;
    uint64_t min_duration_us;
    uint64_t max_duration_us;
    uint32_t unit_tests;
    uint32_t integration_tests;
    uint32_t safety_tests;
    uint32_t performance_tests;
    uint32_t stress_tests;
} test_statistics_t;

// Test framework functions
hal_status_t safety_monitor_test_init(const test_config_t *config);
hal_status_t safety_monitor_test_deinit(void);
hal_status_t safety_monitor_test_run_all(test_statistics_t *statistics);
hal_status_t safety_monitor_test_run_category(test_category_t category, test_statistics_t *statistics);
hal_status_t safety_monitor_test_run_unit_tests(test_statistics_t *statistics);
hal_status_t safety_monitor_test_run_integration_tests(test_statistics_t *statistics);
hal_status_t safety_monitor_test_run_safety_tests(test_statistics_t *statistics);
hal_status_t safety_monitor_test_run_performance_tests(test_statistics_t *statistics);
hal_status_t safety_monitor_test_run_stress_tests(test_statistics_t *statistics);

// Individual test functions
hal_status_t safety_monitor_test_unit_safety_monitor(test_status_t *status);
hal_status_t safety_monitor_test_unit_safety_zones(test_status_t *status);
hal_status_t safety_monitor_test_unit_estop(test_status_t *status);
hal_status_t safety_monitor_test_unit_states(test_status_t *status);
hal_status_t safety_monitor_test_unit_led(test_status_t *status);

hal_status_t safety_monitor_test_integration_lidar(test_status_t *status);
hal_status_t safety_monitor_test_integration_api(test_status_t *status);
hal_status_t safety_monitor_test_integration_config(test_status_t *status);

hal_status_t safety_monitor_test_safety_emergency_violation(test_status_t *status);
hal_status_t safety_monitor_test_safety_warning_violation(test_status_t *status);
hal_status_t safety_monitor_test_safety_response_time(test_status_t *status);

hal_status_t safety_monitor_test_performance_load(test_status_t *status);
hal_status_t safety_monitor_test_performance_memory(test_status_t *status);
hal_status_t safety_monitor_test_performance_cpu(test_status_t *status);

hal_status_t safety_monitor_test_stress_concurrent(test_status_t *status);
hal_status_t safety_monitor_test_stress_fault_injection(test_status_t *status);
hal_status_t safety_monitor_test_stress_recovery(test_status_t *status);

// Utility functions
hal_status_t test_generate_report(const test_statistics_t *statistics, const char *filename);
const char* test_result_to_string(test_result_t result);
const char* test_category_to_string(test_category_t category);
uint64_t test_get_timestamp_us(void);
void test_sleep_ms(uint32_t milliseconds);

// Test assertion macros
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            status->result = TEST_RESULT_FAIL; \
            snprintf(status->error_message, sizeof(status->error_message), \
                    "Assertion failed: %s", #condition); \
            status->line_number = __LINE__; \
            strcpy(status->file_name, __FILE__); \
            return HAL_STATUS_ERROR; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            status->result = TEST_RESULT_FAIL; \
            snprintf(status->error_message, sizeof(status->error_message), \
                    "Expected %d, got %d", (int)(expected), (int)(actual)); \
            status->line_number = __LINE__; \
            strcpy(status->file_name, __FILE__); \
            return HAL_STATUS_ERROR; \
        } \
    } while(0)

#define TEST_ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            status->result = TEST_RESULT_FAIL; \
            snprintf(status->error_message, sizeof(status->error_message), \
                    "Expected true, got false"); \
            status->line_number = __LINE__; \
            strcpy(status->file_name, __FILE__); \
            return HAL_STATUS_ERROR; \
        } \
    } while(0)

#define TEST_ASSERT_FALSE(condition) \
    do { \
        if ((condition)) { \
            status->result = TEST_RESULT_FAIL; \
            snprintf(status->error_message, sizeof(status->error_message), \
                    "Expected false, got true"); \
            status->line_number = __LINE__; \
            strcpy(status->file_name, __FILE__); \
            return HAL_STATUS_ERROR; \
        } \
    } while(0)

#define TEST_ASSERT_NULL(pointer) \
    do { \
        if ((pointer) != NULL) { \
            status->result = TEST_RESULT_FAIL; \
            snprintf(status->error_message, sizeof(status->error_message), \
                    "Expected NULL pointer"); \
            status->line_number = __LINE__; \
            strcpy(status->file_name, __FILE__); \
            return HAL_STATUS_ERROR; \
        } \
    } while(0)

#define TEST_ASSERT_NOT_NULL(pointer) \
    do { \
        if ((pointer) == NULL) { \
            status->result = TEST_RESULT_FAIL; \
            snprintf(status->error_message, sizeof(status->error_message), \
                    "Expected non-NULL pointer"); \
            status->line_number = __LINE__; \
            strcpy(status->file_name, __FILE__); \
            return HAL_STATUS_ERROR; \
        } \
    } while(0)

#define TEST_ASSERT_LESS_THAN(expected, actual) \
    do { \
        if ((actual) >= (expected)) { \
            status->result = TEST_RESULT_FAIL; \
            snprintf(status->error_message, sizeof(status->error_message), \
                    "Expected less than %d, got %d", (int)(expected), (int)(actual)); \
            status->line_number = __LINE__; \
            strcpy(status->file_name, __FILE__); \
            return HAL_STATUS_ERROR; \
        } \
    } while(0)

#define TEST_ASSERT_GREATER_THAN(expected, actual) \
    do { \
        if ((actual) <= (expected)) { \
            status->result = TEST_RESULT_FAIL; \
            snprintf(status->error_message, sizeof(status->error_message), \
                    "Expected greater than %d, got %d", (int)(expected), (int)(actual)); \
            status->line_number = __LINE__; \
            strcpy(status->file_name, __FILE__); \
            return HAL_STATUS_ERROR; \
        } \
    } while(0)

#endif // SAFETY_MONITOR_TEST_H
