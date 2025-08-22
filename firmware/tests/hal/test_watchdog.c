/**
 * @file test_watchdog.c
 * @brief Unit tests for watchdog system
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-15 (Watchdog System Testing)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include "../include/02-HAL/hal_watchdog.h"
#include "../include/02-HAL/hal_common.h"

// Test configuration
#define TEST_WATCHDOG_TIMEOUT_MS    2000    // 2 seconds for testing
#define TEST_FEED_INTERVAL_MS       500     // Feed every 500ms
#define TEST_TIMEOUT_CALLBACK_MS    1000    // Timeout callback after 1s

// Global test variables
static bool timeout_callback_called = false;
static uint32_t timeout_callback_count = 0;
static uint64_t timeout_callback_time = 0;

// Test timeout callback function
static void test_timeout_callback(void *context) {
    timeout_callback_called = true;
    timeout_callback_count++;
    timeout_callback_time = hal_get_timestamp_ms();
    printf("  [INFO] Watchdog timeout callback called (count: %u, time: %lu ms)\n", 
           timeout_callback_count, timeout_callback_time);
}

// Test helper functions
static void reset_test_variables(void) {
    timeout_callback_called = false;
    timeout_callback_count = 0;
    timeout_callback_time = 0;
}

static void print_test_header(const char *test_name) {
    printf("\n=== %s ===\n", test_name);
}

static void print_test_result(const char *test_name, bool passed) {
    printf("  [%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

// Test cases

/**
 * @brief Test watchdog initialization
 */
static bool test_watchdog_init(void) {
    print_test_header("Test Watchdog Initialization");
    
    watchdog_config_t config = {
        .timeout_ms = TEST_WATCHDOG_TIMEOUT_MS,
        .feed_interval_ms = TEST_FEED_INTERVAL_MS,
        .auto_feed_enabled = false,
        .reset_on_timeout = false,
        .timeout_callback = test_timeout_callback,
        .timeout_context = NULL
    };
    
    // Test initialization
    hal_status_t status = hal_watchdog_init(&config);
    print_test_result("Initialize watchdog", status == HAL_STATUS_OK);
    
    // Test if watchdog is enabled
    bool enabled = hal_watchdog_is_enabled();
    print_test_result("Check if enabled", enabled == true);
    
    // Test get timeout
    uint32_t timeout;
    status = hal_watchdog_get_timeout(&timeout);
    print_test_result("Get timeout value", status == HAL_STATUS_OK && timeout == TEST_WATCHDOG_TIMEOUT_MS);
    
    return (status == HAL_STATUS_OK && enabled == true);
}

/**
 * @brief Test watchdog feeding
 */
static bool test_watchdog_feed(void) {
    print_test_header("Test Watchdog Feeding");
    
    // Feed watchdog
    hal_status_t status = hal_watchdog_feed();
    print_test_result("Feed watchdog", status == HAL_STATUS_OK);
    
    // Get status
    watchdog_status_t status_info;
    status = hal_watchdog_get_status(&status_info);
    print_test_result("Get status", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  [INFO] State: %d, Fault: %d, Feed count: %u\n", 
               status_info.state, status_info.fault, status_info.feed_count);
        print_test_result("Check feed count", status_info.feed_count > 0);
    }
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test watchdog statistics
 */
static bool test_watchdog_statistics(void) {
    print_test_header("Test Watchdog Statistics");
    
    // Get statistics
    watchdog_statistics_t stats;
    hal_status_t status = hal_watchdog_get_statistics(&stats);
    print_test_result("Get statistics", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  [INFO] Total feeds: %lu, Total timeouts: %lu\n", 
               stats.total_feeds, stats.total_timeouts);
        print_test_result("Check total feeds", stats.total_feeds > 0);
    }
    
    // Reset statistics
    status = hal_watchdog_reset_statistics();
    print_test_result("Reset statistics", status == HAL_STATUS_OK);
    
    // Check if reset worked
    status = hal_watchdog_get_statistics(&stats);
    print_test_result("Check reset effect", status == HAL_STATUS_OK && stats.total_feeds == 0);
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test watchdog auto-feed
 */
static bool test_watchdog_auto_feed(void) {
    print_test_header("Test Watchdog Auto-Feed");
    
    // Enable auto-feed
    hal_status_t status = hal_watchdog_enable_auto_feed();
    print_test_result("Enable auto-feed", status == HAL_STATUS_OK);
    
    // Wait a bit to let auto-feed work
    hal_sleep_ms(1000);
    
    // Get status to check if auto-feed is working
    watchdog_status_t status_info;
    status = hal_watchdog_get_status(&status_info);
    print_test_result("Check auto-feed status", status == HAL_STATUS_OK);
    
    // Disable auto-feed
    status = hal_watchdog_disable_auto_feed();
    print_test_result("Disable auto-feed", status == HAL_STATUS_OK);
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test watchdog timeout (simulated)
 */
static bool test_watchdog_timeout_simulation(void) {
    print_test_header("Test Watchdog Timeout Simulation");
    
    reset_test_variables();
    
    // Set a very short timeout for testing
    hal_status_t status = hal_watchdog_set_timeout(100); // 100ms
    print_test_result("Set short timeout", status == HAL_STATUS_OK);
    
    // Wait for timeout
    hal_sleep_ms(200);
    
    // Check if timeout callback was called
    print_test_result("Check timeout callback", timeout_callback_called == true);
    
    // Get status to check timeout state
    watchdog_status_t status_info;
    status = hal_watchdog_get_status(&status_info);
    print_test_result("Check timeout state", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  [INFO] State: %d, Fault: %d, Timeout count: %u\n", 
               status_info.state, status_info.fault, status_info.timeout_count);
    }
    
    // Reset timeout to normal value
    status = hal_watchdog_set_timeout(TEST_WATCHDOG_TIMEOUT_MS);
    print_test_result("Reset timeout to normal", status == HAL_STATUS_OK);
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test watchdog force timeout
 */
static bool test_watchdog_force_timeout(void) {
    print_test_header("Test Watchdog Force Timeout");
    
    reset_test_variables();
    
    // Force timeout
    hal_status_t status = hal_watchdog_force_timeout();
    print_test_result("Force timeout", status == HAL_STATUS_OK);
    
    // Check if timeout callback was called
    print_test_result("Check force timeout callback", timeout_callback_called == true);
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test watchdog deinitialization
 */
static bool test_watchdog_deinit(void) {
    print_test_header("Test Watchdog Deinitialization");
    
    // Deinitialize watchdog
    hal_status_t status = hal_watchdog_deinit();
    print_test_result("Deinitialize watchdog", status == HAL_STATUS_OK);
    
    // Check if watchdog is disabled
    bool enabled = hal_watchdog_is_enabled();
    print_test_result("Check if disabled", enabled == false);
    
    return (status == HAL_STATUS_OK && enabled == false);
}

/**
 * @brief Test watchdog error handling
 */
static bool test_watchdog_error_handling(void) {
    print_test_header("Test Watchdog Error Handling");
    
    // Test with invalid timeout
    hal_status_t status = hal_watchdog_set_timeout(0);
    print_test_result("Set invalid timeout (0ms)", status == HAL_STATUS_INVALID_PARAMETER);
    
    // Test with very large timeout
    status = hal_watchdog_set_timeout(60000); // 60 seconds
    print_test_result("Set large timeout", status == HAL_STATUS_INVALID_PARAMETER);
    
    // Test get status without initialization
    watchdog_status_t status_info;
    status = hal_watchdog_get_status(&status_info);
    print_test_result("Get status without init", status == HAL_STATUS_NOT_INITIALIZED);
    
    return true; // These are expected errors
}

/**
 * @brief Main test function
 */
int main(void) {
    printf("=== Watchdog System Unit Tests ===\n");
    printf("Version: %s\n", HAL_VERSION_STRING);
    printf("Date: %s\n", __DATE__);
    
    bool all_tests_passed = true;
    
    // Run all tests
    all_tests_passed &= test_watchdog_init();
    all_tests_passed &= test_watchdog_feed();
    all_tests_passed &= test_watchdog_statistics();
    all_tests_passed &= test_watchdog_auto_feed();
    all_tests_passed &= test_watchdog_timeout_simulation();
    all_tests_passed &= test_watchdog_force_timeout();
    all_tests_passed &= test_watchdog_error_handling();
    all_tests_passed &= test_watchdog_deinit();
    
    printf("\n=== Test Summary ===\n");
    printf("All tests passed: %s\n", all_tests_passed ? "YES" : "NO");
    
    return all_tests_passed ? 0 : 1;
}
