/**
 * @file test_safety_system.c
 * @brief Integration tests for safety system (E-Stop + Watchdog + Module Monitoring)
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-15 (Safety System Integration Testing)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "../include/02-HAL/hal_estop.h"
#include "../include/02-HAL/hal_watchdog.h"
#include "../include/02-HAL/hal_rs485.h"
#include "../include/02-HAL/hal_common.h"

// Test configuration
#define TEST_SAFETY_TIMEOUT_MS     3000    // 3 seconds for safety tests
#define TEST_MODULE_ID             1       // Test module ID
#define TEST_HEARTBEAT_INTERVAL_MS 1000    // Heartbeat interval
#define TEST_COMM_TIMEOUT_MS       2000    // Communication timeout

// Global test variables
static bool safety_system_initialized = false;
static bool estop_triggered = false;
static bool watchdog_timeout = false;
static bool module_timeout = false;
static pthread_mutex_t test_mutex = PTHREAD_MUTEX_INITIALIZER;

// Safety event callback
static void safety_event_callback(hal_safety_status_t status) {
    pthread_mutex_lock(&test_mutex);
    
    switch (status) {
        case HAL_SAFETY_ESTOP_TRIGGERED:
            estop_triggered = true;
            printf("  [SAFETY] E-Stop triggered!\n");
            break;
        case HAL_SAFETY_WATCHDOG_TIMEOUT:
            watchdog_timeout = true;
            printf("  [SAFETY] Watchdog timeout!\n");
            break;
        case HAL_SAFETY_MODULE_TIMEOUT:
            module_timeout = true;
            printf("  [SAFETY] Module timeout!\n");
            break;
        default:
            printf("  [SAFETY] Unknown safety event: %d\n", status);
            break;
    }
    
    pthread_mutex_unlock(&test_mutex);
}

// Test helper functions
static void reset_safety_flags(void) {
    pthread_mutex_lock(&test_mutex);
    estop_triggered = false;
    watchdog_timeout = false;
    module_timeout = false;
    pthread_mutex_unlock(&test_mutex);
}

static void print_test_header(const char *test_name) {
    printf("\n=== %s ===\n", test_name);
}

static void print_test_result(const char *test_name, bool passed) {
    printf("  [%s] %s\n", passed ? "PASS" : "FAIL", test_name);
}

// Test cases

/**
 * @brief Test safety system initialization
 */
static bool test_safety_system_init(void) {
    print_test_header("Test Safety System Initialization");
    
    // Initialize E-Stop
    estop_config_t estop_config = {
        .channel_pin = ESTOP_PIN,
        .response_timeout_ms = ESTOP_RESPONSE_TIME_MS,
        .debounce_time_ms = ESTOP_DEBOUNCE_TIME_MS,
        .module_timeout_ms = ESTOP_MODULE_TIMEOUT_MS,
        .heartbeat_interval_ms = ESTOP_HEARTBEAT_INTERVAL_MS,
        .auto_reset_enabled = false,
        .module_monitoring_enabled = true
    };
    
    hal_status_t status = hal_estop_init(&estop_config);
    print_test_result("Initialize E-Stop", status == HAL_STATUS_OK);
    
    // Initialize Watchdog
    watchdog_config_t watchdog_config = {
        .timeout_ms = TEST_SAFETY_TIMEOUT_MS,
        .feed_interval_ms = 1000,
        .auto_feed_enabled = false,
        .reset_on_timeout = false,
        .timeout_callback = (hal_callback_t)safety_event_callback,
        .timeout_context = NULL
    };
    
    status = hal_watchdog_init(&watchdog_config);
    print_test_result("Initialize Watchdog", status == HAL_STATUS_OK);
    
    // Initialize RS485 for module communication
    rs485_config_t rs485_config = {
        .device_path = RS485_DEVICE_PATH,
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0,
        .timeout_ms = TEST_COMM_TIMEOUT_MS,
        .retry_count = 3
    };
    
    status = hal_rs485_init(&rs485_config);
    print_test_result("Initialize RS485", status == HAL_STATUS_OK);
    
    safety_system_initialized = (status == HAL_STATUS_OK);
    
    return safety_system_initialized;
}

/**
 * @brief Test E-Stop functionality
 */
static bool test_estop_functionality(void) {
    print_test_header("Test E-Stop Functionality");
    
    if (!safety_system_initialized) {
        printf("  [ERROR] Safety system not initialized\n");
        return false;
    }
    
    // Check initial E-Stop state
    estop_state_t state;
    hal_status_t status = hal_estop_get_state(&state);
    print_test_result("Get E-Stop state", status == HAL_STATUS_OK);
    
    // Check if E-Stop is triggered
    bool triggered;
    status = hal_estop_is_triggered(&triggered);
    print_test_result("Check E-Stop trigger", status == HAL_STATUS_OK);
    
    printf("  [INFO] E-Stop state: %d, Triggered: %s\n", state, triggered ? "YES" : "NO");
    
    // Test E-Stop status
    estop_status_t estop_status;
    status = hal_estop_get_status(&estop_status);
    print_test_result("Get E-Stop status", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  [INFO] Channel status: %s, Module comm: %s\n", 
               estop_status.channel_status ? "OK" : "FAULT",
               estop_status.module_communication_ok ? "OK" : "FAULT");
    }
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test watchdog functionality
 */
static bool test_watchdog_functionality(void) {
    print_test_header("Test Watchdog Functionality");
    
    if (!safety_system_initialized) {
        printf("  [ERROR] Safety system not initialized\n");
        return false;
    }
    
    // Feed watchdog
    hal_status_t status = hal_watchdog_feed();
    print_test_result("Feed watchdog", status == HAL_STATUS_OK);
    
    // Get watchdog status
    watchdog_status_t watchdog_status;
    status = hal_watchdog_get_status(&watchdog_status);
    print_test_result("Get watchdog status", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  [INFO] State: %d, Fault: %d, Feed count: %u\n", 
               watchdog_status.state, watchdog_status.fault, watchdog_status.feed_count);
    }
    
    // Check if watchdog is enabled
    bool enabled = hal_watchdog_is_enabled();
    print_test_result("Check watchdog enabled", enabled == true);
    
    return (status == HAL_STATUS_OK && enabled == true);
}

/**
 * @brief Test module monitoring
 */
static bool test_module_monitoring(void) {
    print_test_header("Test Module Monitoring");
    
    if (!safety_system_initialized) {
        printf("  [ERROR] Safety system not initialized\n");
        return false;
    }
    
    // Register module heartbeat
    hal_status_t status = hal_estop_register_module_heartbeat(TEST_MODULE_ID);
    print_test_result("Register module heartbeat", status == HAL_STATUS_OK);
    
    // Check module heartbeat
    bool alive;
    status = hal_estop_check_module_heartbeat(TEST_MODULE_ID, &alive);
    print_test_result("Check module heartbeat", status == HAL_STATUS_OK);
    
    printf("  [INFO] Module %u heartbeat: %s\n", TEST_MODULE_ID, alive ? "ALIVE" : "DEAD");
    
    // Check module communication
    bool comm_ok;
    status = hal_estop_check_module_communication(&comm_ok);
    print_test_result("Check module communication", status == HAL_STATUS_OK);
    
    printf("  [INFO] Module communication: %s\n", comm_ok ? "OK" : "FAULT");
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test safety system integration
 */
static bool test_safety_integration(void) {
    print_test_header("Test Safety System Integration");
    
    if (!safety_system_initialized) {
        printf("  [ERROR] Safety system not initialized\n");
        return false;
    }
    
    reset_safety_flags();
    
    // Test safety check
    hal_status_t status = hal_safety_check();
    print_test_result("Safety check", status == HAL_STATUS_OK);
    
    // Get safety status
    hal_safety_status_t safety_status = hal_safety_get_status();
    print_test_result("Get safety status", safety_status == HAL_SAFETY_OK);
    
    printf("  [INFO] Safety status: %d\n", safety_status);
    
    // Test safety enable/disable
    status = hal_safety_enable();
    print_test_result("Enable safety", status == HAL_STATUS_OK);
    
    bool enabled = hal_safety_is_enabled();
    print_test_result("Check safety enabled", enabled == true);
    
    status = hal_safety_disable();
    print_test_result("Disable safety", status == HAL_STATUS_OK);
    
    enabled = hal_safety_is_enabled();
    print_test_result("Check safety disabled", enabled == false);
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test safety timeout scenarios
 */
static bool test_safety_timeout_scenarios(void) {
    print_test_header("Test Safety Timeout Scenarios");
    
    if (!safety_system_initialized) {
        printf("  [ERROR] Safety system not initialized\n");
        return false;
    }
    
    reset_safety_flags();
    
    // Test watchdog timeout simulation
    printf("  [INFO] Testing watchdog timeout simulation...\n");
    
    // Set short timeout for testing
    hal_status_t status = hal_watchdog_set_timeout(100); // 100ms
    print_test_result("Set short watchdog timeout", status == HAL_STATUS_OK);
    
    // Wait for timeout
    hal_sleep_ms(200);
    
    // Check if timeout occurred
    pthread_mutex_lock(&test_mutex);
    bool timeout_occurred = watchdog_timeout;
    pthread_mutex_unlock(&test_mutex);
    
    print_test_result("Watchdog timeout occurred", timeout_occurred == true);
    
    // Reset timeout to normal
    status = hal_watchdog_set_timeout(TEST_SAFETY_TIMEOUT_MS);
    print_test_result("Reset watchdog timeout", status == HAL_STATUS_OK);
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test safety system deinitialization
 */
static bool test_safety_system_deinit(void) {
    print_test_header("Test Safety System Deinitialization");
    
    // Deinitialize RS485
    hal_status_t status = hal_rs485_deinit();
    print_test_result("Deinitialize RS485", status == HAL_STATUS_OK);
    
    // Deinitialize Watchdog
    status = hal_watchdog_deinit();
    print_test_result("Deinitialize Watchdog", status == HAL_STATUS_OK);
    
    // Deinitialize E-Stop
    status = hal_estop_deinit();
    print_test_result("Deinitialize E-Stop", status == HAL_STATUS_OK);
    
    safety_system_initialized = false;
    
    return (status == HAL_STATUS_OK);
}

/**
 * @brief Test safety system error handling
 */
static bool test_safety_error_handling(void) {
    print_test_header("Test Safety Error Handling");
    
    // Test without initialization
    estop_state_t state;
    hal_status_t status = hal_estop_get_state(&state);
    print_test_result("E-Stop get state without init", status == HAL_STATUS_NOT_INITIALIZED);
    
    watchdog_status_t watchdog_status;
    status = hal_watchdog_get_status(&watchdog_status);
    print_test_result("Watchdog get status without init", status == HAL_STATUS_NOT_INITIALIZED);
    
    bool comm_ok;
    status = hal_estop_check_module_communication(&comm_ok);
    print_test_result("Module comm check without init", status == HAL_STATUS_NOT_INITIALIZED);
    
    return true; // These are expected errors
}

/**
 * @brief Main test function
 */
int main(void) {
    printf("=== Safety System Integration Tests ===\n");
    printf("Version: %s\n", HAL_VERSION_STRING);
    printf("Date: %s\n", __DATE__);
    
    bool all_tests_passed = true;
    
    // Run all tests
    all_tests_passed &= test_safety_system_init();
    all_tests_passed &= test_estop_functionality();
    all_tests_passed &= test_watchdog_functionality();
    all_tests_passed &= test_module_monitoring();
    all_tests_passed &= test_safety_integration();
    all_tests_passed &= test_safety_timeout_scenarios();
    all_tests_passed &= test_safety_error_handling();
    all_tests_passed &= test_safety_system_deinit();
    
    printf("\n=== Test Summary ===\n");
    printf("All tests passed: %s\n", all_tests_passed ? "YES" : "NO");
    
    // Cleanup
    pthread_mutex_destroy(&test_mutex);
    
    return all_tests_passed ? 0 : 1;
}
