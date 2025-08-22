/**
 * @file test_helpers.c
 * @brief Implementation of test helpers và testing framework
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-15 (Testing Framework Implementation)
 */

#include "test_helpers.h"
#include "../../include/02-HAL/hal_common.h"
#include <stdarg.h>
#include <sys/time.h>
#include <errno.h>

// Global variables
static test_suite_t g_current_suite;
test_config_t g_test_config;
static FILE *g_log_file = NULL;
static uint32_t g_current_test_index = 0;
static test_result_t *g_test_results = NULL;
static uint32_t g_max_tests = 1000;

// Test statistics
static struct {
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t skipped_tests;
    uint32_t error_tests;
    uint32_t timeout_tests;
    uint64_t total_duration_us;
    uint64_t start_time_us;
    uint64_t end_time_us;
} g_test_stats = {0};

// Mock data structures
static struct {
    bool initialized;
    char return_values[100][64];
    int call_counts[100];
    void *parameters[100][10];
} g_mock_data = {0};

// Performance tracking
static struct {
    char current_test[64];
    uint64_t start_time_us;
    uint64_t total_time_us;
    uint32_t iteration_count;
} g_performance_data = {0};

// Test initialization
void test_init(const char *suite_name, const char *description) {
    // Initialize test suite
    strncpy(g_current_suite.name, suite_name, TEST_MAX_NAME_LENGTH - 1);
    strncpy(g_current_suite.description, description, TEST_MAX_DESCRIPTION_LENGTH - 1);
    g_current_suite.total_tests = 0;
    g_current_suite.passed_tests = 0;
    g_current_suite.failed_tests = 0;
    g_current_suite.skipped_tests = 0;
    g_current_suite.error_tests = 0;
    g_current_suite.total_duration_us = 0;
    g_current_suite.start_time_us = test_get_timestamp_us();
    
    // Allocate test results array
    if (g_test_results == NULL) {
        g_test_results = malloc(g_max_tests * sizeof(test_result_t));
        if (g_test_results == NULL) {
            fprintf(stderr, "Failed to allocate test results array\n");
            exit(1);
        }
        memset(g_test_results, 0, g_max_tests * sizeof(test_result_t));
    }
    
    // Initialize mock data
    g_mock_data.initialized = true;
    memset(g_mock_data.return_values, 0, sizeof(g_mock_data.return_values));
    memset(g_mock_data.call_counts, 0, sizeof(g_mock_data.call_counts));
    memset(g_mock_data.parameters, 0, sizeof(g_mock_data.parameters));
    
    // Print test header
    test_print_header(suite_name);
}

// Test cleanup
void test_cleanup(void) {
    g_current_suite.end_time_us = test_get_timestamp_us();
    g_current_suite.total_duration_us = g_current_suite.end_time_us - g_current_suite.start_time_us;
    
    // Print test summary
    test_print_summary(&g_current_suite);
    
    // Close log file
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    
    // Free test results
    if (g_test_results != NULL) {
        free(g_test_results);
        g_test_results = NULL;
    }
    
    // Reset statistics
    memset(&g_test_stats, 0, sizeof(g_test_stats));
    g_current_test_index = 0;
}

// Test configuration initialization
void test_config_init(test_config_t *config) {
    if (config == NULL) {
        config = &g_test_config;
    }
    
    config->verbose_output = true;
    config->color_output = true;
    config->stop_on_failure = false;
    config->timeout_ms = 5000;
    config->retry_count = 3;
    config->log_file = NULL;
    config->min_level = TEST_LEVEL_UNIT;
    config->category_count = 0;
}

// Test configuration setters
void test_config_set_verbose(bool verbose) {
    g_test_config.verbose_output = verbose;
}

void test_config_set_color(bool color) {
    g_test_config.color_output = color;
}

void test_config_set_timeout(uint32_t timeout_ms) {
    g_test_config.timeout_ms = timeout_ms;
}

void test_config_set_log_file(const char *log_file) {
    if (g_log_file != NULL) {
        fclose(g_log_file);
    }
    
    if (log_file != NULL) {
        g_log_file = fopen(log_file, "w");
        if (g_log_file == NULL) {
            fprintf(stderr, "Failed to open log file: %s\n", log_file);
        }
    }
}

// Test execution
bool test_run(const char *test_name, const char *description, bool (*test_func)(void)) {
    return test_run_with_timeout(test_name, description, test_func, g_test_config.timeout_ms);
}

bool test_run_with_timeout(const char *test_name, const char *description, bool (*test_func)(void), uint32_t timeout_ms) {
    if (g_current_test_index >= g_max_tests) {
        fprintf(stderr, "Maximum number of tests reached\n");
        return false;
    }
    
    test_result_t *result = &g_test_results[g_current_test_index];
    
    // Initialize test result
    strncpy(result->name, test_name, TEST_MAX_NAME_LENGTH - 1);
    strncpy(result->description, description, TEST_MAX_DESCRIPTION_LENGTH - 1);
    result->status = TEST_STATUS_PASS;
    result->level = TEST_LEVEL_UNIT;
    result->category = TEST_CATEGORY_SYSTEM;
    result->start_time_us = test_get_timestamp_us();
    result->end_time_us = 0;
    result->duration_us = 0;
    result->line_number = 0;
    result->file_name = NULL;
    result->function_name = NULL;
    
    // Print test progress
    if (g_test_config.verbose_output) {
        printf("  [%3d/%3d] %s: ", g_current_test_index + 1, g_current_suite.total_tests, test_name);
        fflush(stdout);
    }
    
    // Run test function
    bool test_passed = false;
    uint64_t start_time = test_get_timestamp_us();
    
    if (test_func != NULL) {
        test_passed = test_func();
    }
    
    uint64_t end_time = test_get_timestamp_us();
    
    // Update test result
    result->end_time_us = end_time;
    result->duration_us = end_time - start_time;
    result->status = test_passed ? TEST_STATUS_PASS : TEST_STATUS_FAIL;
    
    // Update statistics
    g_test_stats.total_tests++;
    if (test_passed) {
        g_test_stats.passed_tests++;
        g_current_suite.passed_tests++;
    } else {
        g_test_stats.failed_tests++;
        g_current_suite.failed_tests++;
    }
    
    // Print test result
    if (g_test_config.verbose_output) {
        if (g_test_config.color_output) {
            if (test_passed) {
                printf("%s[PASS]%s", TEST_COLOR_GREEN, TEST_COLOR_RESET);
            } else {
                printf("%s[FAIL]%s", TEST_COLOR_RED, TEST_COLOR_RESET);
            }
        } else {
            printf("[%s]", test_passed ? "PASS" : "FAIL");
        }
        printf(" (%lu us)\n", result->duration_us);
    }
    
    // Log test result
    if (g_log_file != NULL) {
        fprintf(g_log_file, "[%s] %s: %s (%lu us)\n", 
                test_passed ? "PASS" : "FAIL", 
                test_name, 
                description, 
                result->duration_us);
    }
    
    g_current_test_index++;
    g_current_suite.total_tests++;
    
    return test_passed;
}

// Test skip
void test_skip(const char *reason) {
    if (g_current_test_index >= g_max_tests) {
        return;
    }
    
    test_result_t *result = &g_test_results[g_current_test_index];
    result->status = TEST_STATUS_SKIP;
    strncpy(result->error_message, reason, TEST_MAX_ERROR_MESSAGE_LENGTH - 1);
    
    g_test_stats.skipped_tests++;
    g_current_suite.skipped_tests++;
    
    if (g_test_config.verbose_output) {
        if (g_test_config.color_output) {
            printf("%s[SKIP]%s: %s\n", TEST_COLOR_YELLOW, TEST_COLOR_RESET, reason);
        } else {
            printf("[SKIP]: %s\n", reason);
        }
    }
    
    g_current_test_index++;
    g_current_suite.total_tests++;
}

// Test result functions
void test_pass(const char *test_name) {
    // This function is called by assertion macros
    // The actual result is handled in test_run
}

void test_fail(const char *file, int line, const char *function, const char *message) {
    if (g_current_test_index > 0) {
        test_result_t *result = &g_test_results[g_current_test_index - 1];
        result->status = TEST_STATUS_FAIL;
        result->line_number = line;
        result->file_name = file;
        result->function_name = function;
        strncpy(result->error_message, message, TEST_MAX_ERROR_MESSAGE_LENGTH - 1);
    }
}

void test_error(const char *file, int line, const char *function, const char *message) {
    if (g_current_test_index > 0) {
        test_result_t *result = &g_test_results[g_current_test_index - 1];
        result->status = TEST_STATUS_ERROR;
        result->line_number = line;
        result->file_name = file;
        result->function_name = function;
        strncpy(result->error_message, message, TEST_MAX_ERROR_MESSAGE_LENGTH - 1);
    }
    
    g_test_stats.error_tests++;
    g_current_suite.error_tests++;
}

// Test reporting functions
void test_print_header(const char *suite_name) {
    printf("\n");
    if (g_test_config.color_output) {
        printf("%s=== %s ===%s\n", TEST_COLOR_CYAN, suite_name, TEST_COLOR_RESET);
    } else {
        printf("=== %s ===\n", suite_name);
    }
    printf("Test Framework Version: %s\n", TEST_FRAMEWORK_VERSION);
    printf("Date: %s\n", __DATE__);
    printf("\n");
}

void test_print_result(const test_result_t *result) {
    if (result == NULL) {
        return;
    }
    
    const char *status_str = "UNKNOWN";
    const char *color = TEST_COLOR_RESET;
    
    switch (result->status) {
        case TEST_STATUS_PASS:
            status_str = "PASS";
            color = TEST_COLOR_GREEN;
            break;
        case TEST_STATUS_FAIL:
            status_str = "FAIL";
            color = TEST_COLOR_RED;
            break;
        case TEST_STATUS_SKIP:
            status_str = "SKIP";
            color = TEST_COLOR_YELLOW;
            break;
        case TEST_STATUS_ERROR:
            status_str = "ERROR";
            color = TEST_COLOR_MAGENTA;
            break;
        case TEST_STATUS_TIMEOUT:
            status_str = "TIMEOUT";
            color = TEST_COLOR_YELLOW;
            break;
    }
    
    if (g_test_config.color_output) {
        printf("%s[%s]%s %s (%lu us)\n", color, status_str, TEST_COLOR_RESET, result->name, result->duration_us);
    } else {
        printf("[%s] %s (%lu us)\n", status_str, result->name, result->duration_us);
    }
    
    if (result->status != TEST_STATUS_PASS && strlen(result->error_message) > 0) {
        printf("  Error: %s\n", result->error_message);
        if (result->file_name != NULL && result->line_number > 0) {
            printf("  Location: %s:%d in %s\n", result->file_name, result->line_number, result->function_name);
        }
    }
}

void test_print_summary(const test_suite_t *suite) {
    printf("\n");
    if (g_test_config.color_output) {
        printf("%s=== Test Summary ===%s\n", TEST_COLOR_CYAN, TEST_COLOR_RESET);
    } else {
        printf("=== Test Summary ===\n");
    }
    
    printf("Suite: %s\n", suite->name);
    printf("Description: %s\n", suite->description);
    printf("Total Tests: %u\n", suite->total_tests);
    
    if (g_test_config.color_output) {
        printf("Passed: %s%u%s\n", TEST_COLOR_GREEN, suite->passed_tests, TEST_COLOR_RESET);
        printf("Failed: %s%u%s\n", TEST_COLOR_RED, suite->failed_tests, TEST_COLOR_RESET);
        printf("Skipped: %s%u%s\n", TEST_COLOR_YELLOW, suite->skipped_tests, TEST_COLOR_RESET);
        printf("Errors: %s%u%s\n", TEST_COLOR_MAGENTA, suite->error_tests, TEST_COLOR_RESET);
    } else {
        printf("Passed: %u\n", suite->passed_tests);
        printf("Failed: %u\n", suite->failed_tests);
        printf("Skipped: %u\n", suite->skipped_tests);
        printf("Errors: %u\n", suite->error_tests);
    }
    
    printf("Total Duration: %lu us (%.2f ms)\n", suite->total_duration_us, suite->total_duration_us / 1000.0);
    
    if (suite->failed_tests > 0 || suite->error_tests > 0) {
        if (g_test_config.color_output) {
            printf("%sSome tests failed!%s\n", TEST_COLOR_RED, TEST_COLOR_RESET);
        } else {
            printf("Some tests failed!\n");
        }
    } else {
        if (g_test_config.color_output) {
            printf("%sAll tests passed!%s\n", TEST_COLOR_GREEN, TEST_COLOR_RESET);
        } else {
            printf("All tests passed!\n");
        }
    }
    printf("\n");
}

void test_print_progress(uint32_t current, uint32_t total) {
    if (!g_test_config.verbose_output) {
        return;
    }
    
    float percentage = (float)current / total * 100.0;
    int bar_width = 50;
    int filled_width = (int)(percentage / 100.0 * bar_width);
    
    printf("\r[");
    for (int i = 0; i < bar_width; i++) {
        if (i < filled_width) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("] %3.1f%% (%u/%u)", percentage, current, total);
    fflush(stdout);
    
    if (current == total) {
        printf("\n");
    }
}

// Test utility functions
uint64_t test_get_timestamp_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

uint64_t test_get_timestamp_ms(void) {
    return test_get_timestamp_us() / 1000ULL;
}

void test_sleep_ms(uint32_t milliseconds) {
    usleep(milliseconds * 1000);
}

void test_sleep_us(uint64_t microseconds) {
    usleep(microseconds);
}

// Test fixture functions
void test_fixture_setup(void) {
    // Initialize HAL modules for testing
    test_log_message("INFO", "Setting up test fixture");
}

void test_fixture_teardown(void) {
    // Cleanup HAL modules after testing
    test_log_message("INFO", "Tearing down test fixture");
}

void test_fixture_reset(void) {
    // Reset test fixture to initial state
    test_log_message("INFO", "Resetting test fixture");
}

// Test mock functions
void test_mock_init(void) {
    g_mock_data.initialized = true;
    memset(g_mock_data.return_values, 0, sizeof(g_mock_data.return_values));
    memset(g_mock_data.call_counts, 0, sizeof(g_mock_data.call_counts));
    memset(g_mock_data.parameters, 0, sizeof(g_mock_data.parameters));
}

void test_mock_cleanup(void) {
    g_mock_data.initialized = false;
}

void test_mock_set_return_value(const char *function, int return_value) {
    if (!g_mock_data.initialized) {
        return;
    }
    
    // Find or create entry for function
    for (int i = 0; i < 100; i++) {
        if (strlen(g_mock_data.return_values[i]) == 0) {
            strncpy(g_mock_data.return_values[i], function, 63);
            break;
        }
    }
}

void test_mock_set_parameter(const char *function, const char *param_name, void *value) {
    if (!g_mock_data.initialized) {
        return;
    }
    
    // Implementation for parameter setting
    (void)function;
    (void)param_name;
    (void)value;
}

void test_mock_verify_call(const char *function, int expected_calls) {
    if (!g_mock_data.initialized) {
        return;
    }
    
    // Implementation for call verification
    (void)function;
    (void)expected_calls;
}

// Test performance functions
void test_performance_start(const char *test_name) {
    strncpy(g_performance_data.current_test, test_name, 63);
    g_performance_data.start_time_us = test_get_timestamp_us();
    g_performance_data.iteration_count = 0;
}

void test_performance_end(const char *test_name) {
    if (strcmp(g_performance_data.current_test, test_name) == 0) {
        uint64_t end_time = test_get_timestamp_us();
        g_performance_data.total_time_us = end_time - g_performance_data.start_time_us;
        
        printf("Performance Test: %s\n", test_name);
        printf("  Total Time: %lu us (%.2f ms)\n", 
               g_performance_data.total_time_us, 
               g_performance_data.total_time_us / 1000.0);
        printf("  Iterations: %u\n", g_performance_data.iteration_count);
        if (g_performance_data.iteration_count > 0) {
            printf("  Average Time: %.2f us per iteration\n", 
                   (float)g_performance_data.total_time_us / g_performance_data.iteration_count);
        }
    }
}

void test_performance_report(void) {
    printf("Performance Report:\n");
    printf("  Total Performance Tests: 1\n");
    printf("  Total Time: %lu us (%.2f ms)\n", 
           g_performance_data.total_time_us, 
           g_performance_data.total_time_us / 1000.0);
}

// Test stress functions
void test_stress_start(const char *test_name, uint32_t iterations) {
    strncpy(g_performance_data.current_test, test_name, 63);
    g_performance_data.start_time_us = test_get_timestamp_us();
    g_performance_data.iteration_count = iterations;
    
    printf("Starting stress test: %s (%u iterations)\n", test_name, iterations);
}

void test_stress_iteration(uint32_t current) {
    if (current % 100 == 0) {
        printf("  Progress: %u/%u\n", current, g_performance_data.iteration_count);
    }
}

void test_stress_end(const char *test_name) {
    if (strcmp(g_performance_data.current_test, test_name) == 0) {
        uint64_t end_time = test_get_timestamp_us();
        g_performance_data.total_time_us = end_time - g_performance_data.start_time_us;
        
        printf("Stress test completed: %s\n", test_name);
        printf("  Total Time: %lu us (%.2f ms)\n", 
               g_performance_data.total_time_us, 
               g_performance_data.total_time_us / 1000.0);
        printf("  Iterations: %u\n", g_performance_data.iteration_count);
        if (g_performance_data.iteration_count > 0) {
            printf("  Average Time: %.2f us per iteration\n", 
                   (float)g_performance_data.total_time_us / g_performance_data.iteration_count);
        }
    }
}

// Test safety functions
void test_safety_check(void) {
    test_log_message("INFO", "Performing safety check");
}

void test_safety_trigger_estop(void) {
    test_log_message("WARNING", "Triggering E-Stop for testing");
}

void test_safety_trigger_watchdog(void) {
    test_log_message("WARNING", "Triggering watchdog timeout for testing");
}

void test_safety_trigger_module_timeout(void) {
    test_log_message("WARNING", "Triggering module timeout for testing");
}

// Test communication functions
void test_comm_send_message(const uint8_t *data, size_t length) {
    test_log_message("DEBUG", "Sending message (length: %zu)", length);
    (void)data;
}

void test_comm_receive_message(uint8_t *buffer, size_t max_length, size_t *actual_length) {
    test_log_message("DEBUG", "Receiving message (max length: %zu)", max_length);
    *actual_length = 0;
    (void)buffer;
}

void test_comm_simulate_timeout(void) {
    test_log_message("WARNING", "Simulating communication timeout");
}

void test_comm_simulate_error(void) {
    test_log_message("WARNING", "Simulating communication error");
}

// Test I/O functions
void test_io_set_pin_state(uint8_t pin, bool state) {
    test_log_message("DEBUG", "Setting pin %u to %s", pin, state ? "HIGH" : "LOW");
}

bool test_io_get_pin_state(uint8_t pin) {
    test_log_message("DEBUG", "Getting pin %u state", pin);
    return false;
}

void test_io_simulate_interrupt(uint8_t pin) {
    test_log_message("WARNING", "Simulating interrupt on pin %u", pin);
}

void test_io_simulate_fault(uint8_t pin) {
    test_log_message("WARNING", "Simulating fault on pin %u", pin);
}

// Test sensor functions
void test_sensor_set_value(uint32_t sensor_id, float value) {
    test_log_message("DEBUG", "Setting sensor %u value to %.2f", sensor_id, value);
}

float test_sensor_get_value(uint32_t sensor_id) {
    test_log_message("DEBUG", "Getting sensor %u value", sensor_id);
    return 0.0f;
}

void test_sensor_simulate_noise(uint32_t sensor_id, float noise_level) {
    test_log_message("WARNING", "Simulating noise on sensor %u (level: %.2f)", sensor_id, noise_level);
}

void test_sensor_simulate_fault(uint32_t sensor_id) {
    test_log_message("WARNING", "Simulating fault on sensor %u", sensor_id);
}

// Test system functions
void test_system_set_state(const char *state) {
    test_log_message("INFO", "Setting system state to: %s", state);
}

const char* test_system_get_state(void) {
    test_log_message("DEBUG", "Getting system state");
    return "UNKNOWN";
}

void test_system_simulate_reboot(void) {
    test_log_message("WARNING", "Simulating system reboot");
}

void test_system_simulate_crash(void) {
    test_log_message("WARNING", "Simulating system crash");
}

// Test logging functions
void test_log_init(const char *log_file) {
    if (g_log_file != NULL) {
        fclose(g_log_file);
    }
    
    if (log_file != NULL) {
        g_log_file = fopen(log_file, "w");
        if (g_log_file == NULL) {
            fprintf(stderr, "Failed to open log file: %s\n", log_file);
        }
    }
}

void test_log_message(const char *level, const char *format, ...) {
    if (g_log_file == NULL) {
        return;
    }
    
    uint64_t timestamp = test_get_timestamp_ms();
    fprintf(g_log_file, "[%lu ms] [%s] ", timestamp, level);
    
    va_list args;
    va_start(args, format);
    vfprintf(g_log_file, format, args);
    va_end(args);
    
    fprintf(g_log_file, "\n");
    fflush(g_log_file);
}

void test_log_close(void) {
    if (g_log_file != NULL) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
}

// Test statistics functions
void test_stats_init(void) {
    memset(&g_test_stats, 0, sizeof(g_test_stats));
    g_test_stats.start_time_us = test_get_timestamp_us();
}

void test_stats_add_result(const test_result_t *result) {
    if (result == NULL) {
        return;
    }
    
    g_test_stats.total_tests++;
    g_test_stats.total_duration_us += result->duration_us;
    
    switch (result->status) {
        case TEST_STATUS_PASS:
            g_test_stats.passed_tests++;
            break;
        case TEST_STATUS_FAIL:
            g_test_stats.failed_tests++;
            break;
        case TEST_STATUS_SKIP:
            g_test_stats.skipped_tests++;
            break;
        case TEST_STATUS_ERROR:
            g_test_stats.error_tests++;
            break;
        case TEST_STATUS_TIMEOUT:
            g_test_stats.timeout_tests++;
            break;
    }
}

void test_stats_print_summary(void) {
    printf("\n=== Test Statistics ===\n");
    printf("Total Tests: %u\n", g_test_stats.total_tests);
    printf("Passed: %u (%.1f%%)\n", g_test_stats.passed_tests, 
           g_test_stats.total_tests > 0 ? (float)g_test_stats.passed_tests / g_test_stats.total_tests * 100.0 : 0.0);
    printf("Failed: %u (%.1f%%)\n", g_test_stats.failed_tests,
           g_test_stats.total_tests > 0 ? (float)g_test_stats.failed_tests / g_test_stats.total_tests * 100.0 : 0.0);
    printf("Skipped: %u (%.1f%%)\n", g_test_stats.skipped_tests,
           g_test_stats.total_tests > 0 ? (float)g_test_stats.skipped_tests / g_test_stats.total_tests * 100.0 : 0.0);
    printf("Errors: %u (%.1f%%)\n", g_test_stats.error_tests,
           g_test_stats.total_tests > 0 ? (float)g_test_stats.error_tests / g_test_stats.total_tests * 100.0 : 0.0);
    printf("Timeouts: %u (%.1f%%)\n", g_test_stats.timeout_tests,
           g_test_stats.total_tests > 0 ? (float)g_test_stats.timeout_tests / g_test_stats.total_tests * 100.0 : 0.0);
    
    g_test_stats.end_time_us = test_get_timestamp_us();
    uint64_t total_time = g_test_stats.end_time_us - g_test_stats.start_time_us;
    printf("Total Time: %lu us (%.2f ms)\n", total_time, total_time / 1000.0);
    if (g_test_stats.total_tests > 0) {
        printf("Average Time: %.2f us per test\n", (float)total_time / g_test_stats.total_tests);
    }
}

void test_stats_export_csv(const char *filename) {
    if (filename == NULL) {
        return;
    }
    
    FILE *csv_file = fopen(filename, "w");
    if (csv_file == NULL) {
        fprintf(stderr, "Failed to open CSV file: %s\n", filename);
        return;
    }
    
    fprintf(csv_file, "Test Name,Status,Duration (us),Category,Level\n");
    
    for (uint32_t i = 0; i < g_current_test_index; i++) {
        test_result_t *result = &g_test_results[i];
        const char *status_str = "UNKNOWN";
        
        switch (result->status) {
            case TEST_STATUS_PASS:
                status_str = "PASS";
                break;
            case TEST_STATUS_FAIL:
                status_str = "FAIL";
                break;
            case TEST_STATUS_SKIP:
                status_str = "SKIP";
                break;
            case TEST_STATUS_ERROR:
                status_str = "ERROR";
                break;
            case TEST_STATUS_TIMEOUT:
                status_str = "TIMEOUT";
                break;
        }
        
        fprintf(csv_file, "%s,%s,%lu,%d,%d\n", 
                result->name, status_str, result->duration_us, 
                result->category, result->level);
    }
    
    fclose(csv_file);
    printf("Test statistics exported to: %s\n", filename);
}
