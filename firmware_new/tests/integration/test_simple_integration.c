/**
 * @file test_simple_integration.c
 * @brief Simple integration tests for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team EMBED
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "unity.h"
#include "hal_common.h"
#include "hal_gpio.h"
#include "hal_led.h"
#include "hal_estop.h"
#include "hal_relay.h"

// Test configuration
#define TEST_DURATION_MS 2000
#define TEST_GPIO_PIN 54  // LED_POWER_PIN

// Test results
static struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} test_results = {0};

void setUp(void) {
    test_results.total_tests++;
}

void tearDown(void) {
    // Cleanup
}

// Test 1: HAL Common Integration
void test_hal_common_integration(void) {
    printf("\n=== HAL Common Integration Test ===\n");
    
    // Test timestamp functions
    uint64_t timestamp_us = hal_get_timestamp_us();
    uint64_t timestamp_ms = hal_get_timestamp_ms();
    
    if (timestamp_us > 0 && timestamp_ms > 0) {
        printf("✅ Timestamp functions working: %lu us, %lu ms\n", timestamp_us, timestamp_ms);
        test_results.passed_tests++;
    } else {
        printf("❌ Timestamp functions failed\n");
        test_results.failed_tests++;
        TEST_FAIL();
    }
    
    // Test status string conversion
    const char *status_str = hal_status_to_string(HAL_STATUS_OK);
    if (strcmp(status_str, "OK") == 0) {
        printf("✅ Status string conversion working\n");
        test_results.passed_tests++;
    } else {
        printf("❌ Status string conversion failed\n");
        test_results.failed_tests++;
        TEST_FAIL();
    }
}

// Test 2: GPIO Integration
void test_gpio_integration(void) {
    printf("\n=== GPIO Integration Test ===\n");
    
    // Initialize GPIO
    hal_status_t status = hal_gpio_init();
    if (status == HAL_STATUS_OK) {
        printf("✅ GPIO initialization successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ GPIO initialization failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
        TEST_FAIL();
    }
    
    // Configure test pin as output
    gpio_config_t config = {
        .pin_number = TEST_GPIO_PIN,
        .direction = GPIO_DIRECTION_OUT,
        .edge = GPIO_EDGE_NONE,
        .bias = GPIO_BIAS_DISABLE,
        .drive = GPIO_DRIVE_2MA,
        .active_low = false,
        .debounce_ms = 0
    };
    
    status = hal_gpio_configure_pin(&config);
    if (status == HAL_STATUS_OK) {
        printf("✅ GPIO pin configuration successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ GPIO pin configuration failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    // Test GPIO write/read
    status = hal_gpio_set_value(TEST_GPIO_PIN, true);
    if (status == HAL_STATUS_OK) {
        printf("✅ GPIO write successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ GPIO write failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    usleep(100000); // 100ms delay
    
    bool value;
    status = hal_gpio_get_value(TEST_GPIO_PIN, &value);
    if (status == HAL_STATUS_OK) {
        printf("✅ GPIO read successful: %s\n", value ? "HIGH" : "LOW");
        test_results.passed_tests++;
    } else {
        printf("❌ GPIO read failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    // Cleanup
    hal_gpio_set_value(TEST_GPIO_PIN, false);
    hal_gpio_deinit();
}

// Test 3: LED Integration
void test_led_integration(void) {
    printf("\n=== LED Integration Test ===\n");
    
    // Initialize LED system
    hal_status_t status = hal_led_init();
    if (status == HAL_STATUS_OK) {
        printf("✅ LED initialization successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ LED initialization failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
        TEST_FAIL();
    }
    
    // Test LED control
    status = hal_led_power_set(LED_STATE_ON);
    if (status == HAL_STATUS_OK) {
        printf("✅ Power LED ON successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ Power LED ON failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    usleep(500000); // 500ms delay
    
    status = hal_led_power_set(LED_STATE_OFF);
    if (status == HAL_STATUS_OK) {
        printf("✅ Power LED OFF successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ Power LED OFF failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    // Test LED pattern
    status = hal_led_system_set(LED_STATE_BLINK_SLOW);
    if (status == HAL_STATUS_OK) {
        printf("✅ System LED pattern successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ System LED pattern failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    usleep(1000000); // 1 second delay
    
    // Cleanup
    hal_led_system_set(LED_STATE_OFF);
    hal_led_deinit();
}

// Test 4: E-Stop Integration
void test_estop_integration(void) {
    printf("\n=== E-Stop Integration Test ===\n");
    
    // Initialize E-Stop
    estop_config_t config = {
        .pin = ESTOP_PIN,
        .response_timeout_ms = 100,
        .debounce_time_ms = 50,
        .auto_reset_enabled = false
    };
    
    hal_status_t status = hal_estop_init(&config);
    if (status == HAL_STATUS_OK) {
        printf("✅ E-Stop initialization successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ E-Stop initialization failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
        TEST_FAIL();
    }
    
    // Test E-Stop status
    estop_state_t state;
    status = hal_estop_get_state(&state);
    if (status == HAL_STATUS_OK) {
        printf("✅ E-Stop status read successful: %d\n", state);
        test_results.passed_tests++;
    } else {
        printf("❌ E-Stop status read failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    // Test E-Stop update
    status = hal_estop_update();
    if (status == HAL_STATUS_OK) {
        printf("✅ E-Stop update successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ E-Stop update failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    // Cleanup
    hal_estop_deinit();
}

// Test 5: Relay Integration
void test_relay_integration(void) {
    printf("\n=== Relay Integration Test ===\n");
    
    // Initialize relay
    relay_config_t config = {
        .output_pin = RELAY1_OUTPUT_PIN,
        .voltage_v = 24,
        .current_max_a = 2,
        .pulse_duration_ms = 100,
        .pulse_interval_ms = 1000,
        .overcurrent_protection = true,
        .overtemperature_protection = true
    };
    
    hal_status_t status = hal_relay_init(&config);
    if (status == HAL_STATUS_OK) {
        printf("✅ Relay initialization successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ Relay initialization failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
        TEST_FAIL();
    }
    
    // Test relay control
    status = hal_relay1_on();
    if (status == HAL_STATUS_OK) {
        printf("✅ Relay 1 ON successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ Relay 1 ON failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    usleep(500000); // 500ms delay
    
    status = hal_relay1_off();
    if (status == HAL_STATUS_OK) {
        printf("✅ Relay 1 OFF successful\n");
        test_results.passed_tests++;
    } else {
        printf("❌ Relay 1 OFF failed: %s\n", hal_status_to_string(status));
        test_results.failed_tests++;
    }
    
    // Cleanup
    hal_relay_deinit();
}

// Test 6: System Integration
void test_system_integration(void) {
    printf("\n=== System Integration Test ===\n");
    
    // Test multiple HAL modules working together
    hal_status_t status;
    
    // Initialize all systems
    status = hal_gpio_init();
    if (status != HAL_STATUS_OK) {
        printf("❌ GPIO init failed\n");
        test_results.failed_tests++;
        TEST_FAIL();
    }
    
    status = hal_led_init();
    if (status != HAL_STATUS_OK) {
        printf("❌ LED init failed\n");
        test_results.failed_tests++;
        TEST_FAIL();
    }
    
    status = hal_relay_init(NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Relay init failed\n");
        test_results.failed_tests++;
        TEST_FAIL();
    }
    
    printf("✅ All HAL modules initialized successfully\n");
    test_results.passed_tests++;
    
    // Test coordinated operation
    hal_led_power_set(LED_STATE_ON);
    hal_relay1_on();
    usleep(200000); // 200ms
    
    hal_led_power_set(LED_STATE_OFF);
    hal_relay1_off();
    
    printf("✅ Coordinated operation successful\n");
    test_results.passed_tests++;
    
    // Cleanup
    hal_gpio_deinit();
    hal_led_deinit();
    hal_relay_deinit();
}

// Main test runner
int main(void) {
    printf("=== OHT-50 Master Module Simple Integration Tests ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    
    UNITY_BEGIN();
    
    // Run integration tests
    RUN_TEST(test_hal_common_integration);
    RUN_TEST(test_gpio_integration);
    RUN_TEST(test_led_integration);
    RUN_TEST(test_estop_integration);
    RUN_TEST(test_relay_integration);
    RUN_TEST(test_system_integration);
    
    // Print test summary
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", test_results.total_tests);
    printf("Passed: %d\n", test_results.passed_tests);
    printf("Failed: %d\n", test_results.failed_tests);
    printf("Success rate: %.1f%%\n", 
           (float)test_results.passed_tests / test_results.total_tests * 100.0);
    
    if (test_results.failed_tests == 0) {
        printf("✅ All integration tests PASSED!\n");
    } else {
        printf("❌ Some integration tests FAILED!\n");
    }
    
    return UNITY_END();
}
