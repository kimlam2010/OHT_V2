/**
 * @file test_hal_led_wrapper.c
 * @brief Mock LED HAL wrapper for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 */

#include "mock_led.h"
#include "hal_led.h"
#include <string.h>
#include <stdio.h>

hal_status_t hal_led_init(void)
{
    if (mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_initialized(true);
    return HAL_STATUS_OK;
}

hal_status_t hal_led_deinit(void)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_initialized(false);
    return HAL_STATUS_OK;
}

hal_status_t hal_led_update(void)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Update mock timestamp
    mock_led_state.timestamp_us += 1000;
    return HAL_STATUS_OK;
}

hal_status_t hal_led_power_set(led_state_t state)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_power_led(state == LED_STATE_ON);
    return HAL_STATUS_OK;
}

hal_status_t hal_led_system_set(led_state_t state)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_system_led(state == LED_STATE_ON);
    return HAL_STATUS_OK;
}

hal_status_t hal_led_comm_set(led_state_t state)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_comm_led(state == LED_STATE_ON);
    return HAL_STATUS_OK;
}

hal_status_t hal_led_network_set(led_state_t state)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_network_led(state == LED_STATE_ON);
    return HAL_STATUS_OK;
}

hal_status_t hal_led_error_set(led_state_t state)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_error_led(state == LED_STATE_ON);
    return HAL_STATUS_OK;
}

hal_status_t hal_led_get_status(uint8_t pin, led_status_t *status)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    if (status == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    // Set status based on pin (simplified mapping)
    switch (pin) {
        case 0: // Power LED
            status->led_value = mock_led_state.power_led_on;
            break;
        case 1: // System LED
            status->led_value = mock_led_state.system_led_on;
            break;
        case 2: // Comm LED
            status->led_value = mock_led_state.comm_led_on;
            break;
        case 3: // Network LED
            status->led_value = mock_led_state.network_led_on;
            break;
        case 4: // Error LED
            status->led_value = mock_led_state.error_led_on;
            break;
        default:
            status->led_value = false;
            break;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t hal_led_get_device_info(hal_device_info_t *device_info)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    if (device_info == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    device_info->device_type = HAL_DEVICE_TYPE_RELAY; // Use existing device type
    device_info->status = HAL_DEVICE_STATUS_OK;
    device_info->device_id = 0;
    strcpy(device_info->device_name, "Mock_LED");
    strcpy(device_info->device_version, "1.0.0");
    device_info->timestamp_us = mock_led_state.timestamp_us;
    device_info->error_count = 0;
    device_info->warning_count = 0;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_led_get_diagnostics(char *info, size_t max_len)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    if (info == NULL || max_len == 0) {
        return HAL_STATUS_ERROR;
    }
    
    snprintf(info, max_len, "Mock LED Diagnostics: Power=%s, System=%s, Comm=%s, Network=%s, Error=%s",
             mock_led_state.power_led_on ? "ON" : "OFF",
             mock_led_state.system_led_on ? "ON" : "OFF",
             mock_led_state.comm_led_on ? "ON" : "OFF",
             mock_led_state.network_led_on ? "ON" : "OFF",
             mock_led_state.error_led_on ? "ON" : "OFF");
    
    return HAL_STATUS_OK;
}

// Additional convenience functions
hal_status_t hal_led_system_ready(void)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_power_led(true);
    mock_led_set_system_led(true);
    mock_led_set_comm_led(true);
    mock_led_set_network_led(true);
    mock_led_set_error_led(false);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_led_system_error(void)
{
    if (!mock_led_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_led_set_power_led(true);
    mock_led_set_system_led(false);
    mock_led_set_comm_led(false);
    mock_led_set_network_led(false);
    mock_led_set_error_led(true);
    
    return HAL_STATUS_OK;
}
