/**
 * @file mock_led.h
 * @brief Mock LED HAL for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 */

#ifndef MOCK_LED_H
#define MOCK_LED_H

#include "hal_led.h"
#include <stdbool.h>
#include <stdint.h>

// Mock LED state structure
typedef struct {
    bool initialized;
    bool power_led_on;
    bool system_led_on;
    bool comm_led_on;
    bool network_led_on;
    bool error_led_on;
    led_config_t config;
    led_status_t status;
    uint64_t timestamp_us;
} mock_led_state_t;

extern mock_led_state_t mock_led_state;

// Mock LED control functions
void mock_led_reset(void);
void mock_led_set_initialized(bool initialized);
void mock_led_set_power_led(bool on);
void mock_led_set_system_led(bool on);
void mock_led_set_comm_led(bool on);
void mock_led_set_network_led(bool on);
void mock_led_set_error_led(bool on);
void mock_led_set_config(const led_config_t *config);
void mock_led_set_status(const led_status_t *status);

// Mock LED validation
hal_status_t mock_led_validate_config(const led_config_t *config);

#endif // MOCK_LED_H
