/**
 * @file mock_led.c
 * @brief Mock LED HAL implementation for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 */

#include "mock_led.h"
#include <string.h>

// Mock LED state
mock_led_state_t mock_led_state = {0};

void mock_led_reset(void)
{
    memset(&mock_led_state, 0, sizeof(mock_led_state_t));
    mock_led_state.timestamp_us = 1000;
    mock_led_state.power_led_on = false;
    mock_led_state.system_led_on = false;
    mock_led_state.comm_led_on = false;
    mock_led_state.network_led_on = false;
    mock_led_state.error_led_on = false;
}

void mock_led_set_initialized(bool initialized)
{
    mock_led_state.initialized = initialized;
}

void mock_led_set_power_led(bool on)
{
    mock_led_state.power_led_on = on;
}

void mock_led_set_system_led(bool on)
{
    mock_led_state.system_led_on = on;
}

void mock_led_set_comm_led(bool on)
{
    mock_led_state.comm_led_on = on;
}

void mock_led_set_network_led(bool on)
{
    mock_led_state.network_led_on = on;
}

void mock_led_set_error_led(bool on)
{
    mock_led_state.error_led_on = on;
}

void mock_led_set_config(const led_config_t *config)
{
    if (config != NULL) {
        mock_led_state.config = *config;
    }
}

void mock_led_set_status(const led_status_t *status)
{
    if (status != NULL) {
        mock_led_state.status = *status;
    }
}

hal_status_t mock_led_validate_config(const led_config_t *config)
{
    if (config == NULL) {
        return HAL_STATUS_ERROR;
    }
    return HAL_STATUS_OK;
}
