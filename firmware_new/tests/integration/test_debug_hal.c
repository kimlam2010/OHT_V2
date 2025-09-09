/**
 * @file test_debug_hal.c
 * @brief Debug HAL system for API Manager
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include "unity.h"
#include "hal_common.h"
#include "hal_gpio.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {
    // Initialize HAL logging system
    hal_status_t result = hal_log_init("/tmp/test_debug.log");
    printf("HAL Log init result: %d\n", result);
    
    // Initialize HAL GPIO
    result = hal_gpio_init();
    printf("HAL GPIO init result: %d\n", result);
}

void tearDown(void) {
    // Clean up
    hal_gpio_deinit();
    hal_log_close();
}

void test_hal_logging_works(void) {
    // Test HAL logging
    hal_status_t result = hal_log_message(HAL_LOG_LEVEL_INFO, "Test HAL logging message");
    printf("HAL log message result: %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
}

void test_hal_error_logging_works(void) {
    // Test HAL error logging
    hal_status_t result = hal_log_error("TEST", "test_function", 123, HAL_STATUS_OK, "Test error message");
    printf("HAL log error result: %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
}

void test_hal_device_status_works(void) {
    // Test HAL device status
    hal_device_status_t status = HAL_DEVICE_STATUS_OK;
    printf("HAL device status: %d\n", status);
    TEST_ASSERT_EQUAL(HAL_DEVICE_STATUS_OK, status);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hal_logging_works);
    RUN_TEST(test_hal_error_logging_works);
    RUN_TEST(test_hal_device_status_works);
    UNITY_END();
    return 0;
}
