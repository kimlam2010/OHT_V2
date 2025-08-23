#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/hal_relay.h"
#include "../include/hal_common.h"

// Test configuration
#define TEST_DELAY_MS 1000
#define TEST_ITERATIONS 5

// Test results
static int tests_passed = 0;
static int tests_failed = 0;

// Test function prototypes
static void test_relay_init(void);
static void test_relay_basic_control(void);
static void test_relay_pulse_functionality(void);
static void test_relay_monitoring(void);
static void test_relay_protection(void);
static void test_relay_configuration(void);
static void test_relay_statistics(void);
static void test_relay_diagnostics(void);

// Helper functions
static void print_test_result(const char *test_name, bool passed);
static void delay_ms(uint32_t ms);
static void relay_event_callback(relay_state_t state, relay_fault_t fault);

int main(void) {
    printf("=== Relay System Test Program ===\n");
    printf("Testing Master Module relay system...\n\n");

    // Run tests
    test_relay_init();
    test_relay_basic_control();
    test_relay_pulse_functionality();
    test_relay_monitoring();
    test_relay_protection();
    test_relay_configuration();
    test_relay_statistics();
    test_relay_diagnostics();

    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    printf("Success rate: %.1f%%\n", 
           (float)tests_passed / (tests_passed + tests_failed) * 100.0);

    // Cleanup
    hal_relay_deinit();

    return (tests_failed == 0) ? 0 : 1;
}

static void test_relay_init(void) {
    printf("Testing relay initialization...\n");
    
    // Set up callback
    hal_status_t status = hal_relay_set_callback(relay_event_callback);
    if (status != HAL_STATUS_OK) {
        print_test_result("Relay Set Callback", false);
        tests_failed++;
        return;
    }
    
    status = hal_relay_init(NULL);
    bool passed = (status == HAL_STATUS_OK);
    
    print_test_result("Relay Init", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_relay_basic_control(void) {
    printf("Testing relay basic control...\n");
    
    bool passed = true;
    
    // Test ON
    hal_status_t status = hal_relay_on();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    // Test OFF
    status = hal_relay_off();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    // Test toggle
    status = hal_relay_toggle();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_relay_toggle();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    // Test get state
    relay_state_t state;
    status = hal_relay_get_state(&state);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test get output status
    bool output_status;
    status = hal_relay_get_output_status(&output_status);
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("Relay Basic Control", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_relay_pulse_functionality(void) {
    printf("Testing relay pulse functionality...\n");
    
    bool passed = true;
    
    // Test pulse
    hal_status_t status = hal_relay_pulse(100);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(200);
    
    // Test pulse with different duration
    status = hal_relay_pulse(200);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(300);
    
    // Test pulse test function
    status = hal_relay_test_pulse(150);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(200);
    
    print_test_result("Relay Pulse Functionality", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_relay_monitoring(void) {
    printf("Testing relay monitoring...\n");
    
    bool passed = true;
    
    // Test current monitoring
    uint32_t current_ma;
    hal_status_t status = hal_relay_get_current(&current_ma);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test voltage monitoring
    uint32_t voltage_mv;
    status = hal_relay_get_voltage(&voltage_mv);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test temperature monitoring
    uint32_t temperature_c;
    status = hal_relay_get_temperature(&temperature_c);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test brightness (for LED-like functionality)
    uint8_t brightness;
    // status = hal_relay_get_brightness(RELAY_OUTPUT_PIN, &brightness); // Function not implemented
    // if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("Relay Monitoring", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_relay_protection(void) {
    printf("Testing relay protection...\n");
    
    bool passed = true;
    
    // Test overcurrent protection
    bool overcurrent;
    hal_status_t status = hal_relay_check_overcurrent(&overcurrent);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test overtemperature protection
    bool overtemperature;
    status = hal_relay_check_overtemperature(&overtemperature);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test safety status
    bool safe;
    status = hal_relay_check_safety(&safe);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test protection enable/disable
    status = hal_relay_set_overcurrent_protection(true);
    if (status != HAL_STATUS_OK) passed = false;
    
    status = hal_relay_set_overtemperature_protection(true);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test threshold setting
    status = hal_relay_set_overcurrent_threshold(2000);
    if (status != HAL_STATUS_OK) passed = false;
    
    status = hal_relay_set_overtemperature_threshold(85);
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("Relay Protection", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_relay_configuration(void) {
    printf("Testing relay configuration...\n");
    
    bool passed = true;
    
    // Test get configuration
    relay_config_t config;
    hal_status_t status = hal_relay_get_config(&config);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test set configuration
    status = hal_relay_set_config(&config);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test hardware validation
    status = hal_relay_validate_hardware();
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test self-test
    status = hal_relay_self_test();
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("Relay Configuration", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_relay_statistics(void) {
    printf("Testing relay statistics...\n");
    
    bool passed = true;
    
    // Test switch count
    uint32_t switch_count;
    hal_status_t status = hal_relay_get_switch_count(&switch_count);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test fault count
    uint32_t fault_count;
    status = hal_relay_get_fault_count(&fault_count);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test reset statistics
    status = hal_relay_reset_statistics();
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test get fault
    relay_fault_t fault;
    status = hal_relay_get_fault(&fault);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test clear fault
    status = hal_relay_clear_fault();
    if (status != HAL_STATUS_OK) passed = false;
    
    print_test_result("Relay Statistics", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_relay_diagnostics(void) {
    printf("Testing relay diagnostics...\n");
    
    bool passed = true;
    
    // Test diagnostics
    char diagnostics[512];
    hal_status_t status = hal_relay_get_diagnostics(diagnostics, sizeof(diagnostics));
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test status
    relay_status_t status_info;
    status = hal_relay_get_status(&status_info);
    if (status != HAL_STATUS_OK) passed = false;
    
    // Test emergency shutdown
    status = hal_relay_emergency_shutdown();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    print_test_result("Relay Diagnostics", passed);
    
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

static void relay_event_callback(relay_state_t state, relay_fault_t fault) {
    printf("Relay Event: State=%d, Fault=%d\n", state, fault);
}
