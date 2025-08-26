/**
 * @file test_hal_gpio_wrapper.c
 * @brief Test wrapper for HAL GPIO with mock functions
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "hal_gpio.h"
#include "mock_gpio.h"
#include <stdio.h>
#include <string.h>

// Override GPIO functions with mock implementations for testing
hal_status_t hal_gpio_configure_pin(const gpio_config_t *config) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->pin_number >= 256) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Mock the configuration
    mock_gpio_set_pin_direction(config->pin_number, config->direction == GPIO_DIRECTION_OUT);
    mock_gpio_set_pin_edge(config->pin_number, config->edge);
    mock_gpio_set_pin_bias(config->pin_number, config->bias);
    mock_gpio_set_pin_drive(config->pin_number, config->drive);
    mock_gpio_set_pin_active_low(config->pin_number, config->active_low);
    mock_gpio_set_pin_debounce(config->pin_number, config->debounce_ms);
    
    // Mark pin as configured
    mock_gpio_state.pins_configured[config->pin_number] = true;
    mock_gpio_state.configure_count++;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_value(uint32_t pin, bool value) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (pin >= 256) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!mock_gpio_is_pin_configured(pin)) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock the write operation
    mock_gpio_set_pin_value(pin, value);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_get_value(uint32_t pin, bool *value) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (pin >= 256 || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!mock_gpio_is_pin_configured(pin)) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock the read operation
    *value = mock_gpio_get_pin_value(pin);
    mock_gpio_state.read_count++;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_edge(uint32_t pin, gpio_edge_t edge) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (pin >= 256) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!mock_gpio_is_pin_configured(pin)) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock the edge setting
    mock_gpio_set_pin_edge(pin, edge);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_bias(uint32_t pin, gpio_bias_t bias) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (pin >= 256) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!mock_gpio_is_pin_configured(pin)) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock the bias setting
    mock_gpio_set_pin_bias(pin, bias);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_drive(uint32_t pin, gpio_drive_t drive) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (pin >= 256) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!mock_gpio_is_pin_configured(pin)) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock the drive setting
    mock_gpio_set_pin_drive(pin, drive);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_get_statistics(gpio_statistics_t *stats) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Mock statistics using correct structure
    stats->reads = mock_gpio_get_read_count();
    stats->writes = mock_gpio_get_write_count();
    stats->events = 0;
    stats->errors = 0;
    stats->timestamp_us = 0;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_reset_statistics(void) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Reset mock statistics
    mock_gpio_state.write_count = 0;
    mock_gpio_state.read_count = 0;
    mock_gpio_state.configure_count = 0;
    mock_gpio_state.set_edge_count = 0;
    mock_gpio_state.set_bias_count = 0;
    mock_gpio_state.set_drive_count = 0;
    
    return HAL_STATUS_OK;
}

// Override init/deinit to use mock state
hal_status_t hal_gpio_init(void) {
    if (mock_gpio_is_initialized()) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    mock_gpio_state.initialized = true;
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_deinit(void) {
    if (!mock_gpio_is_initialized()) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    mock_gpio_state.initialized = false;
    return HAL_STATUS_OK;
}
