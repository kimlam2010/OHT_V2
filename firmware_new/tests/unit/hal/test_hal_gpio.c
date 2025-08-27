/**
 * @file test_hal_gpio.c
 * @brief Unit tests for HAL GPIO functionality
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "unity.h"
#include "hal_gpio.h"
#include "mock_gpio.h"
#include <stdint.h>
#include <stdbool.h>

// Test fixtures
static gpio_config_t test_config;
static uint32_t test_pin;

void setUp(void) {
    // Reset mock GPIO state
    mock_gpio_reset();
    
    // Initialize test configuration
    test_config.pin_number = 1;
    test_config.direction = GPIO_DIRECTION_OUT;
    test_config.edge = GPIO_EDGE_NONE;
    test_config.bias = GPIO_BIAS_DISABLE;
    test_config.drive = GPIO_DRIVE_2MA;
    test_config.active_low = false;
    test_config.debounce_ms = 0;
    
    test_pin = 1;
}

void tearDown(void) {
    // Clean up after each test
    hal_gpio_deinit();
}

// Test GPIO initialization
void test_hal_gpio_init_returns_success(void) {
    hal_status_t status = hal_gpio_init();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_gpio_init_already_initialized_returns_already_initialized(void) {
    hal_gpio_init();
    hal_status_t status = hal_gpio_init();
    TEST_ASSERT_EQUAL(HAL_STATUS_ALREADY_INITIALIZED, status);
}

// Test GPIO configuration
void test_hal_gpio_configure_pin_returns_success(void) {
    hal_gpio_init();
    hal_status_t status = hal_gpio_configure_pin(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_gpio_configure_pin_not_initialized_returns_error(void) {
    setUp(); // Ensure setup is called
    hal_status_t status = hal_gpio_configure_pin(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_hal_gpio_configure_pin_null_config_returns_error(void) {
    hal_gpio_init();
    hal_status_t status = hal_gpio_configure_pin(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

// Test GPIO read/write operations
void test_hal_gpio_write_pin_returns_success(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    
    hal_status_t status = hal_gpio_set_value(test_pin, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_gpio_read_pin_returns_valid_level(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    hal_gpio_set_value(test_pin, true);
    
    bool value;
    hal_status_t status = hal_gpio_get_value(test_pin, &value);
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_TRUE(value);
}

void test_hal_gpio_read_pin_null_level_returns_error(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    
    hal_status_t status = hal_gpio_get_value(test_pin, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

// Test GPIO interrupt functionality
void test_hal_gpio_set_interrupt_returns_success(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    
    hal_status_t status = hal_gpio_set_edge(test_pin, GPIO_EDGE_RISING);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_gpio_clear_interrupt_returns_success(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    
    hal_status_t status = hal_gpio_set_edge(test_pin, GPIO_EDGE_NONE);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

// Test GPIO status functions
void test_hal_gpio_get_statistics_returns_valid_stats(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    
    gpio_statistics_t stats;
    hal_status_t result = hal_gpio_get_statistics(&stats);
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_GREATER_THAN_OR_EQUAL(0, stats.reads);
    TEST_ASSERT_GREATER_THAN_OR_EQUAL(0, stats.writes);
}

void test_hal_gpio_get_statistics_null_stats_returns_error(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    
    hal_status_t result = hal_gpio_get_statistics(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, result);
}

// Test GPIO deinitialization
void test_hal_gpio_deinit_returns_success(void) {
    hal_gpio_init();
    hal_status_t status = hal_gpio_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_gpio_deinit_not_initialized_returns_error(void) {
    hal_status_t status = hal_gpio_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Test GPIO error handling
void test_hal_gpio_invalid_pin_returns_error(void) {
    hal_gpio_init();
    gpio_config_t invalid_config = test_config;
    invalid_config.pin_number = 999;  // Invalid pin
    hal_status_t status = hal_gpio_configure_pin(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_hal_gpio_operations_after_deinit_return_error(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    hal_gpio_deinit();
    
    hal_status_t status = hal_gpio_set_value(test_pin, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Test GPIO performance
void test_hal_gpio_write_performance(void) {
    hal_gpio_init();
    hal_gpio_configure_pin(&test_config);
    
    uint64_t start_time = hal_get_timestamp_us();
    
    for (int i = 0; i < 100; i++) {
        hal_gpio_set_value(test_pin, true);
        hal_gpio_set_value(test_pin, false);
    }
    
    uint64_t end_time = hal_get_timestamp_us();
    uint64_t duration = end_time - start_time;
    
    // Should complete 200 operations in reasonable time (< 1ms)
    TEST_ASSERT_LESS_THAN(1000, duration);
}

// Test GPIO edge cases
void test_hal_gpio_multiple_pins_simultaneous(void) {
    hal_gpio_init();
    
    gpio_config_t config1 = {.pin_number = 1, .direction = GPIO_DIRECTION_OUT};
    gpio_config_t config2 = {.pin_number = 2, .direction = GPIO_DIRECTION_OUT};
    
    hal_status_t status1 = hal_gpio_configure_pin(&config1);
    hal_status_t status2 = hal_gpio_configure_pin(&config2);
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
    
    hal_gpio_set_value(1, true);
    hal_gpio_set_value(2, false);
    
    bool value1, value2;
    hal_gpio_get_value(1, &value1);
    hal_gpio_get_value(2, &value2);
    
    TEST_ASSERT_TRUE(value1);
    TEST_ASSERT_FALSE(value2);
}

// Main test runner
int main(void) {
    UNITY_BEGIN();
    
    // GPIO initialization tests
    RUN_TEST(test_hal_gpio_init_returns_success);
    RUN_TEST(test_hal_gpio_init_already_initialized_returns_already_initialized);
    
    // GPIO configuration tests
    RUN_TEST(test_hal_gpio_configure_pin_returns_success);
    RUN_TEST(test_hal_gpio_configure_pin_not_initialized_returns_error);
    RUN_TEST(test_hal_gpio_configure_pin_null_config_returns_error);
    
    // GPIO read/write tests
    RUN_TEST(test_hal_gpio_write_pin_returns_success);
    RUN_TEST(test_hal_gpio_read_pin_returns_valid_level);
    RUN_TEST(test_hal_gpio_read_pin_null_level_returns_error);
    
    // GPIO interrupt tests
    RUN_TEST(test_hal_gpio_set_interrupt_returns_success);
    RUN_TEST(test_hal_gpio_clear_interrupt_returns_success);
    
    // GPIO statistics tests
    RUN_TEST(test_hal_gpio_get_statistics_returns_valid_stats);
    RUN_TEST(test_hal_gpio_get_statistics_null_stats_returns_error);
    
    // GPIO deinitialization tests
    RUN_TEST(test_hal_gpio_deinit_returns_success);
    RUN_TEST(test_hal_gpio_deinit_not_initialized_returns_error);
    
    // GPIO error handling tests
    RUN_TEST(test_hal_gpio_invalid_pin_returns_error);
    RUN_TEST(test_hal_gpio_operations_after_deinit_return_error);
    
    // GPIO performance tests
    RUN_TEST(test_hal_gpio_write_performance);
    
    // GPIO edge case tests
    RUN_TEST(test_hal_gpio_multiple_pins_simultaneous);
    
    UNITY_END();
    return 0;
}
