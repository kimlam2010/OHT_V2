/**
 * @file test_helpers.h
 * @brief Common test helpers và testing framework standards
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-15 (Testing Framework Standards)
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

// Testing Framework Configuration
#define TEST_FRAMEWORK_VERSION "1.0.0"
#define TEST_MAX_NAME_LENGTH 128
#define TEST_MAX_DESCRIPTION_LENGTH 256
#define TEST_MAX_ERROR_MESSAGE_LENGTH 512

// Test Result Colors (ANSI)
#define TEST_COLOR_RESET   "\033[0m"
#define TEST_COLOR_RED     "\033[31m"
#define TEST_COLOR_GREEN   "\033[32m"
#define TEST_COLOR_YELLOW  "\033[33m"
#define TEST_COLOR_BLUE    "\033[34m"
#define TEST_COLOR_MAGENTA "\033[35m"
#define TEST_COLOR_CYAN    "\033[36m"
#define TEST_COLOR_WHITE   "\033[37m"

// Test Status
typedef enum {
    TEST_STATUS_PASS = 0,
    TEST_STATUS_FAIL,
    TEST_STATUS_SKIP,
    TEST_STATUS_ERROR,
    TEST_STATUS_TIMEOUT
} test_status_t;

// Test Level
typedef enum {
    TEST_LEVEL_UNIT = 0,
    TEST_LEVEL_INTEGRATION,
    TEST_LEVEL_SYSTEM,
    TEST_LEVEL_PERFORMANCE,
    TEST_LEVEL_STRESS
} test_level_t;

// Test Category
typedef enum {
    TEST_CATEGORY_SAFETY = 0,
    TEST_CATEGORY_COMMUNICATION,
    TEST_CATEGORY_IO,
    TEST_CATEGORY_SENSORS,
    TEST_CATEGORY_SYSTEM,
    TEST_CATEGORY_PERFORMANCE,
    TEST_CATEGORY_STRESS
} test_category_t;

// Test Result Structure
typedef struct {
    char name[TEST_MAX_NAME_LENGTH];
    char description[TEST_MAX_DESCRIPTION_LENGTH];
    test_status_t status;
    test_level_t level;
    test_category_t category;
    uint64_t start_time_us;
    uint64_t end_time_us;
    uint64_t duration_us;
    char error_message[TEST_MAX_ERROR_MESSAGE_LENGTH];
    uint32_t line_number;
    const char *file_name;
    const char *function_name;
} test_result_t;

// Test Suite Structure
typedef struct {
    char name[TEST_MAX_NAME_LENGTH];
    char description[TEST_MAX_DESCRIPTION_LENGTH];
    test_result_t *results;
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t skipped_tests;
    uint32_t error_tests;
    uint64_t total_duration_us;
    uint64_t start_time_us;
    uint64_t end_time_us;
} test_suite_t;

// Test Configuration
typedef struct {
    bool verbose_output;
    bool color_output;
    bool stop_on_failure;
    uint32_t timeout_ms;
    uint32_t retry_count;
    const char *log_file;
    test_level_t min_level;
    test_category_t categories[10];
    uint32_t category_count;
} test_config_t;

// Global test configuration
extern test_config_t g_test_config;

// Test assertion macros
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            test_fail(__FILE__, __LINE__, __func__, "Assertion failed: " #condition); \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            char msg[256]; \
            snprintf(msg, sizeof(msg), "Expected %ld, got %ld", (long)(expected), (long)(actual)); \
            test_fail(__FILE__, __LINE__, __func__, msg); \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_STR_EQUAL(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            char msg[256]; \
            snprintf(msg, sizeof(msg), "Expected '%s', got '%s'", (expected), (actual)); \
            test_fail(__FILE__, __LINE__, __func__, msg); \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            test_fail(__FILE__, __LINE__, __func__, "Expected NULL pointer"); \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            test_fail(__FILE__, __LINE__, __func__, "Expected non-NULL pointer"); \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_STATUS(expected_status, actual_status) \
    do { \
        if ((expected_status) != (actual_status)) { \
            char msg[256]; \
            snprintf(msg, sizeof(msg), "Expected status %d, got %d", (int)(expected_status), (int)(actual_status)); \
            test_fail(__FILE__, __LINE__, __func__, msg); \
            return false; \
        } \
    } while(0)

// Test function prototypes
void test_init(const char *suite_name, const char *description);
void test_cleanup(void);
void test_config_init(test_config_t *config);
void test_config_set_verbose(bool verbose);
void test_config_set_color(bool color);
void test_config_set_timeout(uint32_t timeout_ms);
void test_config_set_log_file(const char *log_file);

// Test execution functions
bool test_run(const char *test_name, const char *description, bool (*test_func)(void));
bool test_run_with_timeout(const char *test_name, const char *description, bool (*test_func)(void), uint32_t timeout_ms);
void test_skip(const char *reason);

// Test result functions
void test_pass(const char *test_name);
void test_fail(const char *file, int line, const char *function, const char *message);
void test_error(const char *file, int line, const char *function, const char *message);

// Test reporting functions
void test_print_header(const char *suite_name);
void test_print_result(const test_result_t *result);
void test_print_summary(const test_suite_t *suite);
void test_print_progress(uint32_t current, uint32_t total);

// Test utility functions
uint64_t test_get_timestamp_us(void);
uint64_t test_get_timestamp_ms(void);
void test_sleep_ms(uint32_t milliseconds);
void test_sleep_us(uint64_t microseconds);

// Test fixture functions
void test_fixture_setup(void);
void test_fixture_teardown(void);
void test_fixture_reset(void);

// Test mock functions
void test_mock_init(void);
void test_mock_cleanup(void);
void test_mock_set_return_value(const char *function, int return_value);
void test_mock_set_parameter(const char *function, const char *param_name, void *value);
void test_mock_verify_call(const char *function, int expected_calls);

// Test performance functions
void test_performance_start(const char *test_name);
void test_performance_end(const char *test_name);
void test_performance_report(void);

// Test stress functions
void test_stress_start(const char *test_name, uint32_t iterations);
void test_stress_iteration(uint32_t current);
void test_stress_end(const char *test_name);

// Test safety functions
void test_safety_check(void);
void test_safety_trigger_estop(void);
void test_safety_trigger_watchdog(void);
void test_safety_trigger_module_timeout(void);

// Test communication functions
void test_comm_send_message(const uint8_t *data, size_t length);
void test_comm_receive_message(uint8_t *buffer, size_t max_length, size_t *actual_length);
void test_comm_simulate_timeout(void);
void test_comm_simulate_error(void);

// Test I/O functions
void test_io_set_pin_state(uint8_t pin, bool state);
bool test_io_get_pin_state(uint8_t pin);
void test_io_simulate_interrupt(uint8_t pin);
void test_io_simulate_fault(uint8_t pin);

// Test sensor functions
void test_sensor_set_value(uint32_t sensor_id, float value);
float test_sensor_get_value(uint32_t sensor_id);
void test_sensor_simulate_noise(uint32_t sensor_id, float noise_level);
void test_sensor_simulate_fault(uint32_t sensor_id);

// Test system functions
void test_system_set_state(const char *state);
const char* test_system_get_state(void);
void test_system_simulate_reboot(void);
void test_system_simulate_crash(void);

// Test logging functions
void test_log_init(const char *log_file);
void test_log_message(const char *level, const char *format, ...);
void test_log_close(void);

// Test statistics functions
void test_stats_init(void);
void test_stats_add_result(const test_result_t *result);
void test_stats_print_summary(void);
void test_stats_export_csv(const char *filename);

#endif // TEST_HELPERS_H
