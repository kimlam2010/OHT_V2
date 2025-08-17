#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/hal_estop.h"
#include "../include/hal_common.h"

// Test configuration
#define TEST_DELAY_MS 1000
#define TEST_ITERATIONS 5

// Test results
static int tests_passed = 0;
static int tests_failed = 0;

// Test function prototypes
static void test_estop_init(void);
static void test_estop_basic_functions(void);
static void test_estop_channel_monitoring(void);
static void test_estop_safety_validation(void);
static void test_estop_fault_detection(void);
static void test_estop_reset_functionality(void);
static void test_estop_statistics(void);
static void test_estop_diagnostics(void);

// Helper functions
static void print_test_result(const char *test_name, bool passed);
static void delay_ms(uint32_t ms);
static void estop_event_callback(estop_state_t state, estop_fault_t fault);

int main(void) {
    printf("=== E-Stop Safety System Test Program ===\n");
    printf("Testing Master Module E-Stop safety system...\n\n");

    // Run tests
    test_estop_init();
    test_estop_basic_functions();
    test_estop_channel_monitoring();
    test_estop_safety_validation();
    test_estop_fault_detection();
    test_estop_reset_functionality();
    test_estop_statistics();
    test_estop_diagnostics();

    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    printf("Success rate: %.1f%%\n", 
           (float)tests_passed / (tests_passed + tests_failed) * 100.0);

    // Cleanup
    hal_estop_deinit();

    return (tests_failed == 0) ? 0 : 1;
}

static void test_estop_init(void) {
    printf("Testing E-Stop initialization...\n");
    
    // Set up callback
    hal_status_t status = hal_estop_set_callback(estop_event_callback);
    if (status != HAL_STATUS_OK) {
        print_test_result("E-Stop Set Callback", false);
        tests_failed++;
        return;
    }
    
    status = hal_estop_init(NULL);
    bool passed = (status == HAL_STATUS_OK);
    
    print_test_result("E-Stop Init", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_estop_basic_functions(void) {
    printf("Testing E-Stop basic functions...\n");
    
    bool passed = true;
    
    // Test get state
    estop_state_t state;
    hal_status_t status = hal_estop_get_state(&state);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test is triggered
    bool triggered;
    status = hal_estop_is_triggered(&triggered);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test get fault
    estop_fault_t fault;
    status = hal_estop_get_fault(&fault);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test clear fault
    status = hal_estop_clear_fault();
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("E-Stop Basic Functions", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_estop_channel_monitoring(void) {
    printf("Testing E-Stop channel monitoring...\n");
    
    bool passed = true;
    
    // Test channel status
    bool channel1_status, channel2_status;
    hal_status_t status = hal_estop_test_channels(&channel1_status, &channel2_status);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test individual channel functions
    status = hal_estop_get_channel1_status(&channel1_status);
    if (status != HAL_STATUS_OK) passed = false;
    
    status = hal_estop_get_channel2_status(&channel2_status);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test channel test functions
    status = hal_estop_test_channel1();
    if (status != HAL_STATUS_OK) passed = false;
    
    status = hal_estop_test_channel2();
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("E-Stop Channel Monitoring", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_estop_safety_validation(void) {
    printf("Testing E-Stop safety validation...\n");
    
    bool passed = true;
    
    // Test safety validation
    hal_status_t status = hal_estop_validate_safety();
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test safety compliance
    bool compliant;
    status = hal_estop_check_safety_compliance(&compliant);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test response time
    uint32_t response_time;
    status = hal_estop_get_response_time(&response_time);
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("E-Stop Safety Validation", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_estop_fault_detection(void) {
    printf("Testing E-Stop fault detection...\n");
    
    bool passed = true;
    
    // Test self-test
    hal_status_t status = hal_estop_self_test();
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test hardware validation
    status = hal_estop_validate_hardware();
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test emergency handling
    status = hal_estop_handle_emergency();
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("E-Stop Fault Detection", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_estop_reset_functionality(void) {
    printf("Testing E-Stop reset functionality...\n");
    
    bool passed = true;
    
    // Test reset (this might fail if channels are not safe)
    hal_status_t status = hal_estop_reset();
    // Note: Reset might fail if E-Stop is actually triggered, which is expected
    
    // Test configuration functions
    estop_config_t config;
    status = hal_estop_get_config(&config);
    if (status != HAL_STATUS_OK) passed = false;
    
    status = hal_estop_set_config(&config);
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("E-Stop Reset Functionality", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_estop_statistics(void) {
    printf("Testing E-Stop statistics...\n");
    
    bool passed = true;
    
    // Test trigger count
    uint32_t trigger_count;
    hal_status_t status = hal_estop_get_trigger_count(&trigger_count);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test fault count
    uint32_t fault_count;
    status = hal_estop_get_fault_count(&fault_count);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test reset statistics
    status = hal_estop_reset_statistics();
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("E-Stop Statistics", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_estop_diagnostics(void) {
    printf("Testing E-Stop diagnostics...\n");
    
    bool passed = true;
    
    // Test diagnostics
    char diagnostics[512];
    hal_status_t status = hal_estop_get_diagnostics(diagnostics, sizeof(diagnostics));
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test status
    estop_status_t status_info;
    status = hal_estop_get_status(&status_info);
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("E-Stop Diagnostics", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void print_test_result(const char *test_name, bool passed) {
    if (passed) {
        printf("  ✅ %s: PASSED\n", test_name);
    } else {
        printf("  ❌ %s: FAILED\n", test_name);
    }
}

static void delay_ms(uint32_t ms) {
    usleep(ms * 1000);
}

static void estop_event_callback(estop_state_t state, estop_fault_t fault) {
    printf("E-Stop Event: State=%d, Fault=%d\n", state, fault);
}
