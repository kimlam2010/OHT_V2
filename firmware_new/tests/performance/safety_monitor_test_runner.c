/**
 * @file safety_monitor_test_runner.c
 * @brief Safety Monitor Test Runner
 * @version 1.0.0
 * @date 2025-01-28
 * @team FIRMWARE
 * @task Task 9 (Testing & Validation)
 */

#include "safety_monitor_test.h"
#include "hal_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Test configuration
static test_config_t default_test_config = {
    .enable_unit_tests = true,
    .enable_integration_tests = true,
    .enable_safety_tests = true,
    .enable_performance_tests = true,
    .enable_stress_tests = false,
    .max_test_duration_ms = 30000,  // 30 seconds
    .safety_test_timeout_ms = 5000, // 5 seconds
    .stop_on_failure = false,
    .verbose_output = true
};

// Command line options
typedef struct {
    bool run_unit_tests;
    bool run_integration_tests;
    bool run_safety_tests;
    bool run_performance_tests;
    bool run_stress_tests;
    bool run_all_tests;
    bool verbose;
    bool generate_report;
    char report_filename[256];
} test_options_t;

// Function prototypes
void print_usage(const char *program_name);
void parse_command_line(int argc, char *argv[], test_options_t *options);
test_config_t create_test_config(const test_options_t *options);

int main(int argc, char *argv[])
{
    printf("Safety Monitor Test Runner v1.0.0\n");
    printf("==================================\n\n");
    
    // Parse command line options
    test_options_t options = {0};
    parse_command_line(argc, argv, &options);
    
    // Create test configuration
    test_config_t config = create_test_config(&options);
    
    // Initialize test framework
    hal_status_t status = safety_monitor_test_init(&config);
    if (status != HAL_STATUS_OK) {
        printf("Failed to initialize test framework\n");
        return -1;
    }
    
    // Run tests
    test_statistics_t statistics = {0};
    status = safety_monitor_test_run_all(&statistics);
    if (status != HAL_STATUS_OK) {
        printf("Failed to run tests\n");
        safety_monitor_test_deinit();
        return -1;
    }
    
    // Generate report if requested
    if (options.generate_report) {
        status = test_generate_report(&statistics, options.report_filename);
        if (status != HAL_STATUS_OK) {
            printf("Failed to generate test report\n");
        }
    }
    
    // Deinitialize test framework
    safety_monitor_test_deinit();
    
    // Return exit code based on test results
    if (statistics.failed_tests > 0 || statistics.error_tests > 0) {
        printf("\n❌ Some tests failed. Exit code: 1\n");
        return 1;
    } else {
        printf("\n✅ All tests passed. Exit code: 0\n");
        return 0;
    }
}

void print_usage(const char *program_name)
{
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  --unit              Run unit tests only\n");
    printf("  --integration       Run integration tests only\n");
    printf("  --safety            Run safety tests only\n");
    printf("  --performance       Run performance tests only\n");
    printf("  --stress            Run stress tests only\n");
    printf("  --all               Run all tests (default)\n");
    printf("  --verbose           Enable verbose output\n");
    printf("  --report <file>     Generate test report to file\n");
    printf("  --help              Show this help message\n\n");
    printf("Examples:\n");
    printf("  %s --all --verbose\n", program_name);
    printf("  %s --safety --report safety_report.txt\n", program_name);
    printf("  %s --unit --integration\n", program_name);
}

void parse_command_line(int argc, char *argv[], test_options_t *options)
{
    // Set defaults
    options->run_all_tests = true;
    options->verbose = false;
    options->generate_report = false;
    strcpy(options->report_filename, "safety_monitor_test_report.txt");
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--unit") == 0) {
            options->run_unit_tests = true;
            options->run_all_tests = false;
        } else if (strcmp(argv[i], "--integration") == 0) {
            options->run_integration_tests = true;
            options->run_all_tests = false;
        } else if (strcmp(argv[i], "--safety") == 0) {
            options->run_safety_tests = true;
            options->run_all_tests = false;
        } else if (strcmp(argv[i], "--performance") == 0) {
            options->run_performance_tests = true;
            options->run_all_tests = false;
        } else if (strcmp(argv[i], "--stress") == 0) {
            options->run_stress_tests = true;
            options->run_all_tests = false;
        } else if (strcmp(argv[i], "--all") == 0) {
            options->run_all_tests = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            options->verbose = true;
        } else if (strcmp(argv[i], "--report") == 0) {
            options->generate_report = true;
            if (i + 1 < argc) {
                strcpy(options->report_filename, argv[i + 1]);
                i++; // Skip next argument
            }
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            exit(1);
        }
    }
}

test_config_t create_test_config(const test_options_t *options)
{
    test_config_t config = default_test_config;
    
    if (!options->run_all_tests) {
        config.enable_unit_tests = options->run_unit_tests;
        config.enable_integration_tests = options->run_integration_tests;
        config.enable_safety_tests = options->run_safety_tests;
        config.enable_performance_tests = options->run_performance_tests;
        config.enable_stress_tests = options->run_stress_tests;
    }
    
    config.verbose_output = options->verbose;
    
    return config;
}
