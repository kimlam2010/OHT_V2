#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/hal_led.h"
#include "../include/hal_common.h"

// Test configuration
#define TEST_DELAY_MS 1000
#define TEST_ITERATIONS 5

// Test results
static int tests_passed = 0;
static int tests_failed = 0;

// Test function prototypes
static void test_led_init(void);
static void test_led_basic_control(void);
static void test_led_patterns(void);
static void test_led_brightness(void);
static void test_led_status(void);
static void test_led_convenience_functions(void);
static void test_led_system_patterns(void);

// Helper functions
static void print_test_result(const char *test_name, bool passed);
static void delay_ms(uint32_t ms);

int main(void) {
    printf("=== LED System Test Program ===\n");
    printf("Testing Master Module LED system...\n\n");

    // Run tests
    test_led_init();
    test_led_basic_control();
    test_led_patterns();
    test_led_brightness();
    test_led_status();
    test_led_convenience_functions();
    test_led_system_patterns();

    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    printf("Success rate: %.1f%%\n", 
           (float)tests_passed / (tests_passed + tests_failed) * 100.0);

    // Cleanup
    hal_led_deinit();

    return (tests_failed == 0) ? 0 : 1;
}

static void test_led_init(void) {
    printf("Testing LED initialization...\n");
    
    hal_status_t status = hal_led_init();
    bool passed = (status == HAL_STATUS_OK);
    
    print_test_result("LED Init", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_led_basic_control(void) {
    printf("Testing LED basic control...\n");
    
    bool passed = true;
    
    // Test Power LED
    hal_status_t status = hal_led_on(LED_POWER_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_off(LED_POWER_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    // Test System LED
    status = hal_led_on(LED_SYSTEM_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_off(LED_SYSTEM_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    // Test Communication LED
    status = hal_led_on(LED_COMM_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_off(LED_COMM_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    // Test Network LED
    status = hal_led_on(LED_NETWORK_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_off(LED_NETWORK_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    // Test Error LED
    status = hal_led_on(LED_ERROR_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_off(LED_ERROR_PIN);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    print_test_result("LED Basic Control", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_led_patterns(void) {
    printf("Testing LED patterns...\n");
    
    bool passed = true;
    
    // Test slow blink pattern
    hal_status_t status = hal_led_set_pattern(LED_POWER_PIN, LED_PATTERN_BLINK_SLOW);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(2000);
    
    // Test fast blink pattern
    status = hal_led_set_pattern(LED_SYSTEM_PIN, LED_PATTERN_BLINK_FAST);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(2000);
    
    // Test pulse pattern
    status = hal_led_set_pattern(LED_COMM_PIN, LED_PATTERN_PULSE);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(2000);
    
    // Turn off all patterns
    hal_led_off(LED_POWER_PIN);
    hal_led_off(LED_SYSTEM_PIN);
    hal_led_off(LED_COMM_PIN);
    
    print_test_result("LED Patterns", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_led_brightness(void) {
    printf("Testing LED brightness control...\n");
    
    bool passed = true;
    
    // Test brightness levels
    for (int brightness = 0; brightness <= 100; brightness += 25) {
        hal_status_t status = hal_led_set_brightness(LED_POWER_PIN, brightness);
        if (status != HAL_STATUS_OK) passed = false;
        delay_ms(500);
    }
    
    // Turn off LED
    hal_led_off(LED_POWER_PIN);
    
    print_test_result("LED Brightness", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_led_status(void) {
    printf("Testing LED status functions...\n");
    
    bool passed = true;
    
    // Test status functions
    led_status_t status;
    hal_status_t hal_status = hal_led_get_status(LED_POWER_PIN, &status);
    if (hal_status != HAL_STATUS_OK) passed = false;
    
    // Test state functions
    led_state_t state;
    hal_status = hal_led_get_state(LED_POWER_PIN, &state);
    if (hal_status != HAL_STATUS_OK) passed = false;
    
    // Test pattern functions
    led_pattern_t pattern;
    hal_status = hal_led_get_pattern(LED_POWER_PIN, &pattern);
    if (hal_status != HAL_STATUS_OK) passed = false;
    
    print_test_result("LED Status", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_led_convenience_functions(void) {
    printf("Testing LED convenience functions...\n");
    
    bool passed = true;
    
    // Test convenience functions
    hal_status_t status = hal_led_power_set(LED_STATE_ON);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_system_set(LED_STATE_ON);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_comm_set(LED_STATE_ON);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_network_set(LED_STATE_ON);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    status = hal_led_error_set(LED_STATE_ON);
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(500);
    
    // Turn off all LEDs
    hal_led_power_set(LED_STATE_OFF);
    hal_led_system_set(LED_STATE_OFF);
    hal_led_comm_set(LED_STATE_OFF);
    hal_led_network_set(LED_STATE_OFF);
    hal_led_error_set(LED_STATE_OFF);
    
    print_test_result("LED Convenience Functions", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_led_system_patterns(void) {
    printf("Testing LED system patterns...\n");
    
    bool passed = true;
    
    // Test system ready pattern
    hal_status_t status = hal_led_system_ready();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(2000);
    
    // Test system warning pattern
    status = hal_led_system_warning();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(2000);
    
    // Test system error pattern
    status = hal_led_system_error();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(2000);
    
    // Test system shutdown pattern
    status = hal_led_system_shutdown();
    if (status != HAL_STATUS_OK) passed = false;
    delay_ms(2000);
    
    // Turn off all LEDs
    hal_led_power_set(LED_STATE_OFF);
    hal_led_system_set(LED_STATE_OFF);
    hal_led_comm_set(LED_STATE_OFF);
    hal_led_network_set(LED_STATE_OFF);
    hal_led_error_set(LED_STATE_OFF);
    
    print_test_result("LED System Patterns", passed);
    
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
