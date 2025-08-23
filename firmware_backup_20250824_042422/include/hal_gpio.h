/**
 * @file hal_gpio.h
 * @brief HAL interface for GPIO and Relay control
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-05 (IO Drivers)
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// GPIO Configuration
#define GPIO_MAX_PINS             64
#define GPIO_CHIP_NAME            "gpiochip1"

// Relay Configuration
#define RELAY_CHANNEL_1           "GPIO1_D3"  // Relay 1
#define RELAY_CHANNEL_2           "GPIO1_D2"  // Relay 2
#define RELAY_MAX_CHANNELS        2

// GPIO direction
typedef enum {
    GPIO_DIRECTION_IN = 0,
    GPIO_DIRECTION_OUT = 1
} gpio_direction_t;

// GPIO edge detection
typedef enum {
    GPIO_EDGE_NONE = 0,
    GPIO_EDGE_RISING = 1,
    GPIO_EDGE_FALLING = 2,
    GPIO_EDGE_BOTH = 3
} gpio_edge_t;

// GPIO bias
typedef enum {
    GPIO_BIAS_DISABLE = 0,
    GPIO_BIAS_PULL_UP = 1,
    GPIO_BIAS_PULL_DOWN = 2
} gpio_bias_t;

// GPIO drive strength
typedef enum {
    GPIO_DRIVE_2MA = 0,
    GPIO_DRIVE_4MA = 1,
    GPIO_DRIVE_8MA = 2,
    GPIO_DRIVE_12MA = 3
} gpio_drive_t;

// GPIO configuration structure
typedef struct {
    uint32_t pin_number;
    gpio_direction_t direction;
    gpio_edge_t edge;
    gpio_bias_t bias;
    gpio_drive_t drive;
    bool active_low;
    uint32_t debounce_ms;
} gpio_config_t;

// Relay configuration structure
typedef struct {
    uint8_t channel;
    char gpio_pin[16];
    bool active_high;
    uint32_t pulse_duration_ms;
    bool auto_off;
} relay_config_t;

// GPIO event structure
typedef struct {
    uint32_t pin_number;
    bool value;
    uint64_t timestamp_us;
    gpio_edge_t edge;
} gpio_event_t;

// GPIO statistics
typedef struct {
    uint64_t reads;
    uint64_t writes;
    uint64_t events;
    uint64_t errors;
    uint64_t timestamp_us;
} gpio_statistics_t;

// Relay statistics
typedef struct {
    uint64_t activations;
    uint64_t deactivations;
    uint64_t total_on_time_ms;
    uint64_t errors;
    uint64_t timestamp_us;
} relay_statistics_t;

// GPIO device info
typedef struct {
    hal_device_type_t device_type;
    hal_device_status_t status;
    char device_name[64];
    char device_version[32];
    uint32_t pin_count;
    uint64_t timestamp_us;
    uint32_t error_count;
    uint32_t warning_count;
} gpio_device_info_t;

// HAL function prototypes
hal_status_t hal_gpio_init(void);
hal_status_t hal_gpio_deinit(void);
hal_status_t hal_gpio_configure_pin(const gpio_config_t *config);
hal_status_t hal_gpio_set_direction(uint32_t pin, gpio_direction_t direction);
hal_status_t hal_gpio_set_value(uint32_t pin, bool value);
hal_status_t hal_gpio_get_value(uint32_t pin, bool *value);
hal_status_t hal_gpio_set_edge(uint32_t pin, gpio_edge_t edge);
hal_status_t hal_gpio_set_bias(uint32_t pin, gpio_bias_t bias);
hal_status_t hal_gpio_set_drive(uint32_t pin, gpio_drive_t drive);
hal_status_t hal_gpio_wait_for_event(uint32_t pin, gpio_event_t *event, uint32_t timeout_ms);
hal_status_t hal_gpio_get_statistics(gpio_statistics_t *statistics);
hal_status_t hal_gpio_reset_statistics(void);
hal_status_t hal_gpio_health_check(void);

// Relay function prototypes
hal_status_t hal_relay_init(const relay_config_t *config);
hal_status_t hal_relay_deinit(void);
hal_status_t hal_relay_set(uint8_t channel, bool state);
hal_status_t hal_relay_get(uint8_t channel, bool *state);
hal_status_t hal_relay_pulse(uint8_t channel, uint32_t duration_ms);
hal_status_t hal_relay_toggle(uint8_t channel);
hal_status_t hal_relay_get_statistics(relay_statistics_t *statistics);
hal_status_t hal_relay_reset_statistics(void);
hal_status_t hal_relay_health_check(void);

// Utility functions
hal_status_t gpio_validate_config(const gpio_config_t *config);
hal_status_t relay_validate_config(const relay_config_t *config);
uint32_t gpio_get_pin_offset(const char *pin_name);
bool gpio_is_pin_valid(uint32_t pin);
hal_status_t gpio_export_pin(uint32_t pin);
hal_status_t gpio_unexport_pin(uint32_t pin);

#endif // HAL_GPIO_H
