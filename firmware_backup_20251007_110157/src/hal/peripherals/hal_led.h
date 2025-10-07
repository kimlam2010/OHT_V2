#ifndef HAL_LED_H
#define HAL_LED_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// LED GPIO Pin Definitions (updated for available pins) - REAL HARDWARE
#define LED_POWER_PIN          54  // GPIO1_D6 - Power LED (Green) - REAL HARDWARE
#define LED_SYSTEM_PIN         35  // GPIO1_A3 - System LED (Blue) - REAL HARDWARE
#define LED_COMM_PIN           28  // GPIO0_D4 - Communication LED (Yellow) - REAL HARDWARE
#define LED_NETWORK_PIN        29  // GPIO0_D5 - Network LED (Green) - REAL HARDWARE
#define LED_ERROR_PIN          58  // GPIO1_D2 - Error LED (Red) - REAL HARDWARE

// LED States
typedef enum {
    LED_STATE_OFF = 0,
    LED_STATE_ON,
    LED_STATE_BLINK_SLOW,    // 1Hz blink
    LED_STATE_BLINK_FAST,    // 5Hz blink
    LED_STATE_PULSE          // PWM pulse
} led_state_t;

// LED Patterns
typedef enum {
    LED_PATTERN_NONE = 0,
    LED_PATTERN_SOLID,
    LED_PATTERN_BLINK_SLOW,
    LED_PATTERN_BLINK_FAST,
    LED_PATTERN_PULSE,
    LED_PATTERN_ERROR,       // Fast blink for errors
    LED_PATTERN_WARNING      // Medium blink for warnings
} led_pattern_t;

// LED Configuration Structure
typedef struct {
    uint8_t pin;
    led_state_t default_state;
    led_pattern_t pattern;
    uint32_t blink_period_ms;
    uint32_t pulse_period_ms;
} led_config_t;

// LED Status Structure
typedef struct {
    bool initialized;
    led_state_t current_state;
    led_pattern_t current_pattern;
    uint64_t last_toggle_time;
    bool led_value;
} led_status_t;

// Function Prototypes

/**
 * @brief Initialize LED system
 * @return HAL status
 */
hal_status_t hal_led_init(void);

/**
 * @brief Deinitialize LED system
 * @return HAL status
 */
hal_status_t hal_led_deinit(void);

/**
 * @brief Set LED state
 * @param pin LED GPIO pin
 * @param state LED state
 * @return HAL status
 */
hal_status_t hal_led_set_state(uint8_t pin, led_state_t state);

/**
 * @brief Get LED state
 * @param pin LED GPIO pin
 * @param state Pointer to store LED state
 * @return HAL status
 */
hal_status_t hal_led_get_state(uint8_t pin, led_state_t *state);

/**
 * @brief Set LED pattern
 * @param pin LED GPIO pin
 * @param pattern LED pattern
 * @return HAL status
 */
hal_status_t hal_led_set_pattern(uint8_t pin, led_pattern_t pattern);

/**
 * @brief Get LED pattern
 * @param pin LED GPIO pin
 * @param pattern Pointer to store LED pattern
 * @return HAL status
 */
hal_status_t hal_led_get_pattern(uint8_t pin, led_pattern_t *pattern);

/**
 * @brief Turn LED on
 * @param pin LED GPIO pin
 * @return HAL status
 */
hal_status_t hal_led_on(uint8_t pin);

/**
 * @brief Turn LED off
 * @param pin LED GPIO pin
 * @return HAL status
 */
hal_status_t hal_led_off(uint8_t pin);

/**
 * @brief Toggle LED
 * @param pin LED GPIO pin
 * @return HAL status
 */
hal_status_t hal_led_toggle(uint8_t pin);

/**
 * @brief Set LED brightness (PWM)
 * @param pin LED GPIO pin
 * @param brightness Brightness value (0-100)
 * @return HAL status
 */
hal_status_t hal_led_set_brightness(uint8_t pin, uint8_t brightness);

/**
 * @brief Get LED brightness
 * @param pin LED GPIO pin
 * @param brightness Pointer to store brightness value
 * @return HAL status
 */
hal_status_t hal_led_get_brightness(uint8_t pin, uint8_t *brightness);

/**
 * @brief Update LED patterns (call periodically)
 * @return HAL status
 */
hal_status_t hal_led_update(void);

/**
 * @brief Get LED status
 * @param pin LED GPIO pin
 * @param status Pointer to store LED status
 * @return HAL status
 */
hal_status_t hal_led_get_status(uint8_t pin, led_status_t *status);

// Convenience functions for specific LEDs

/**
 * @brief Set Power LED state
 * @param state LED state
 * @return HAL status
 */
hal_status_t hal_led_power_set(led_state_t state);

/**
 * @brief Set System LED state
 * @param state LED state
 * @return HAL status
 */
hal_status_t hal_led_system_set(led_state_t state);

/**
 * @brief Set Communication LED state
 * @param state LED state
 * @return HAL status
 */
hal_status_t hal_led_comm_set(led_state_t state);

/**
 * @brief Set Network LED state
 * @param state LED state
 * @return HAL status
 */
hal_status_t hal_led_network_set(led_state_t state);

/**
 * @brief Set Error LED state
 * @param state LED state
 * @return HAL status
 */
hal_status_t hal_led_error_set(led_state_t state);

// LED Pattern Functions

/**
 * @brief Set all LEDs to system ready pattern
 * @return HAL status
 */
hal_status_t hal_led_system_ready(void);

/**
 * @brief Set all LEDs to system error pattern
 * @return HAL status
 */
hal_status_t hal_led_system_error(void);

/**
 * @brief Set all LEDs to system warning pattern
 * @return HAL status
 */
hal_status_t hal_led_system_warning(void);

/**
 * @brief Set all LEDs to system shutdown pattern
 * @return HAL status
 */
hal_status_t hal_led_system_shutdown(void);

#endif // HAL_LED_H
