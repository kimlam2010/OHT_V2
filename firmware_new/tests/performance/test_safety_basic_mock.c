#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include "../unity/unity.h"
#include "../../src/hal/common/hal_common.h"
#include "../../src/hal/safety/hal_estop.h"
#include "../../src/hal/peripherals/hal_relay.h"
#include "../../src/app/core/system_controller.h"
#include "../../src/app/core/safety_monitor.h"

// Mock functions for testing
hal_status_t mock_hal_gpio_export(uint8_t pin) {
    (void)pin; // Suppress unused parameter warning
    return HAL_STATUS_OK;
}

hal_status_t mock_hal_gpio_set_direction(uint8_t pin, int direction) {
    (void)pin;
    (void)direction;
    return HAL_STATUS_OK;
}

hal_status_t mock_hal_gpio_set_value(uint8_t pin, int value) {
    (void)pin;
    (void)value;
    return HAL_STATUS_OK;
}

hal_status_t mock_hal_gpio_get_value(uint8_t pin, int* value) {
    (void)pin;
    *value = 1; // Mock high value
    return HAL_STATUS_OK;
}

void test_estop_mock_basic(void) {
    printf("Testing E-Stop HAL mock functions...\n");
    
    estop_config_t config = {
        .pin = 59,
        .response_timeout_ms = 100,
        .debounce_time_ms = 50,
        .auto_reset_enabled = false
    };
    
    hal_status_t status = hal_estop_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    printf("E-Stop initialized successfully\n");
    
    // Test E-Stop state
    estop_state_t state;
    status = hal_estop_get_state(&state);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    printf("E-Stop state: %d\n", state);
    
    // Test E-Stop trigger check
    bool triggered;
    status = hal_estop_is_triggered(&triggered);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    printf("E-Stop triggered: %s\n", triggered ? "true" : "false");
    
    hal_estop_deinit();
    printf("E-Stop deinitialized\n");
}

void test_system_controller_mock_basic(void) {
    printf("Testing System Controller HAL mock functions...\n");
    
    system_controller_config_t config = {
        .timeout_ms = 5000
    };
    
    hal_status_t status = system_controller_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    printf("System controller initialized successfully\n");
    
    system_controller_status_t status_info;
    status = system_controller_get_status(&status_info);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    printf("System controller status retrieved\n");
    
    system_controller_deinit();
    printf("System controller deinitialized\n");
}

void test_safety_monitor_mock_basic(void) {
    printf("Testing Safety Monitor HAL mock functions...\n");
    
    safety_monitor_config_t config = {
        .watchdog_timeout_ms = 200
    };
    
    hal_status_t status = safety_monitor_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    printf("Safety monitor initialized successfully\n");
    
    safety_monitor_status_t safety_status;
    status = safety_monitor_get_status(&safety_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    printf("Safety monitor status retrieved\n");
    
    safety_monitor_deinit();
    printf("Safety monitor deinitialized\n");
}

int main(void) {
    printf("Starting safety basic mock tests...\n");
    
    UNITY_BEGIN();
    
    RUN_TEST(test_estop_mock_basic);
    RUN_TEST(test_system_controller_mock_basic);
    RUN_TEST(test_safety_monitor_mock_basic);
    
    UNITY_END();
    
    printf("Safety basic mock tests completed.\n");
    return 0;
}
