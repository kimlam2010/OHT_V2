/**
 * @file led_manager.h
 * @brief LED Manager for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-03 (LED Manager Implementation)
 */

#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include "hal_common.h"
#include "hal_led.h"
#include "system_state_machine.h"
#include "safety_manager.h"
#include <stdint.h>
#include <stdbool.h>

// LED Manager Pattern Types
typedef enum {
    LED_MGR_PATTERN_TYPE_NONE = 0,
    LED_MGR_PATTERN_TYPE_SYSTEM_STATE,    // System state patterns
    LED_MGR_PATTERN_TYPE_SAFETY_CONDITION, // Safety condition patterns
    LED_MGR_PATTERN_TYPE_COMMUNICATION,   // Communication patterns
    LED_MGR_PATTERN_TYPE_ERROR,           // Error patterns
    LED_MGR_PATTERN_TYPE_CUSTOM           // Custom patterns
} led_mgr_pattern_type_t;

// LED Manager Animation Types
typedef enum {
    LED_MGR_ANIMATION_NONE = 0,
    LED_MGR_ANIMATION_SOLID,              // Solid on/off
    LED_MGR_ANIMATION_BLINK_SLOW,         // Slow blink (1Hz)
    LED_MGR_ANIMATION_BLINK_FAST,         // Fast blink (5Hz)
    LED_MGR_ANIMATION_PULSE,              // Pulse effect
    LED_MGR_ANIMATION_FADE,               // Fade in/out
    LED_MGR_ANIMATION_CHASE,              // Chase effect
    LED_MGR_ANIMATION_BREATH              // Breathing effect
} led_mgr_animation_t;

// LED Manager Priority Levels
typedef enum {
    LED_MGR_PRIORITY_LOW = 0,
    LED_MGR_PRIORITY_NORMAL,
    LED_MGR_PRIORITY_HIGH,
    LED_MGR_PRIORITY_CRITICAL,
    LED_MGR_PRIORITY_EMERGENCY
} led_mgr_priority_t;

// LED Manager Pattern Structure
typedef struct {
    led_mgr_pattern_type_t type;
    led_mgr_animation_t animation;
    led_mgr_priority_t priority;
    uint32_t duration_ms;             // Pattern duration (0 = infinite)
    uint32_t blink_period_ms;         // Blink period
    uint32_t fade_period_ms;          // Fade period
    uint8_t brightness;               // Brightness (0-100)
    bool power_led_enabled;
    bool system_led_enabled;
    bool comm_led_enabled;
    bool network_led_enabled;
    bool error_led_enabled;
} led_mgr_pattern_t;

// LED Manager Status Structure
typedef struct {
    led_mgr_pattern_t current_pattern;
    led_mgr_pattern_t previous_pattern;
    led_mgr_priority_t current_priority;
    uint64_t pattern_start_time;
    uint64_t last_update_time;
    bool pattern_active;
    uint32_t pattern_count;
    uint32_t animation_frame;
} led_mgr_status_t;

// LED Manager Configuration Structure
typedef struct {
    uint32_t update_interval_ms;      // LED update interval
    uint32_t animation_speed_ms;      // Animation speed
    bool enable_animations;           // Enable LED animations
    bool enable_transitions;          // Enable pattern transitions
    uint8_t default_brightness;       // Default brightness
    bool auto_dimming;                // Auto dimming feature
} led_mgr_config_t;

// LED Manager Event Types
typedef enum {
    LED_MGR_EVENT_NONE = 0,
    LED_MGR_EVENT_SYSTEM_STATE_CHANGED,   // System state changed
    LED_MGR_EVENT_SAFETY_LEVEL_CHANGED,   // Safety level changed
    LED_MGR_EVENT_COMMUNICATION_CHANGED,  // Communication status changed
    LED_MGR_EVENT_ERROR_DETECTED,         // Error detected
    LED_MGR_EVENT_PATTERN_TIMEOUT,        // Pattern timeout
    LED_MGR_EVENT_ANIMATION_COMPLETE      // Animation complete
} led_mgr_event_t;

// LED Manager Event Callback
typedef void (*led_mgr_event_callback_t)(led_mgr_event_t event, led_mgr_pattern_t pattern);

// Function Prototypes

/**
 * @brief Initialize LED manager
 * @param config LED configuration
 * @return HAL status
 */
hal_status_t led_manager_init(const led_mgr_config_t *config);

/**
 * @brief Deinitialize LED manager
 * @return HAL status
 */
hal_status_t led_manager_deinit(void);

/**
 * @brief Update LED manager (call periodically)
 * @return HAL status
 */
hal_status_t led_manager_update(void);

/**
 * @brief Set LED pattern
 * @param pattern LED pattern to set
 * @return HAL status
 */
hal_status_t led_manager_set_pattern(const led_mgr_pattern_t *pattern);

/**
 * @brief Get current LED pattern
 * @param pattern Pointer to store current pattern
 * @return HAL status
 */
hal_status_t led_manager_get_pattern(led_mgr_pattern_t *pattern);

/**
 * @brief Clear LED pattern
 * @return HAL status
 */
hal_status_t led_manager_clear_pattern(void);

/**
 * @brief Set LED event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t led_manager_set_callback(led_mgr_event_callback_t callback);

/**
 * @brief Get LED status
 * @param status Pointer to store LED status
 * @return HAL status
 */
hal_status_t led_manager_get_status(led_mgr_status_t *status);

// System State LED Functions

/**
 * @brief Set LED pattern for system state
 * @param state System state
 * @return HAL status
 */
hal_status_t led_manager_set_system_state_pattern(system_state_t state);

/**
 * @brief Update LED pattern based on system state
 * @param state Current system state
 * @return HAL status
 */
hal_status_t led_manager_update_system_state(system_state_t state);

// Safety Condition LED Functions

/**
 * @brief Set LED pattern for safety level
 * @param level Safety level
 * @return HAL status
 */
hal_status_t led_manager_set_safety_pattern(safety_level_t level);

/**
 * @brief Update LED pattern based on safety level
 * @param level Current safety level
 * @return HAL status
 */
hal_status_t led_manager_update_safety_level(safety_level_t level);

// Communication LED Functions

/**
 * @brief Set LED pattern for communication status
 * @param connected Communication connected status
 * @param error Communication error status
 * @return HAL status
 */
hal_status_t led_manager_set_communication_pattern(bool connected, bool error);

/**
 * @brief Update LED pattern based on communication status
 * @param connected Communication connected status
 * @param error Communication error status
 * @return HAL status
 */
hal_status_t led_manager_update_communication_status(bool connected, bool error);

// Error LED Functions

/**
 * @brief Set LED pattern for error condition
 * @param error_type Error type
 * @param error_severity Error severity
 * @return HAL status
 */
hal_status_t led_manager_set_error_pattern(uint32_t error_type, uint8_t error_severity);

/**
 * @brief Clear error LED pattern
 * @return HAL status
 */
hal_status_t led_manager_clear_error_pattern(void);

// Animation Functions

/**
 * @brief Start LED animation
 * @param animation Animation type
 * @param duration_ms Animation duration
 * @return HAL status
 */
hal_status_t led_manager_start_animation(led_mgr_animation_t animation, uint32_t duration_ms);

/**
 * @brief Stop LED animation
 * @return HAL status
 */
hal_status_t led_manager_stop_animation(void);

/**
 * @brief Set LED brightness
 * @param brightness Brightness value (0-100)
 * @return HAL status
 */
hal_status_t led_manager_set_brightness(uint8_t brightness);

/**
 * @brief Get LED brightness
 * @param brightness Pointer to store brightness value
 * @return HAL status
 */
hal_status_t led_manager_get_brightness(uint8_t *brightness);

// Configuration Functions

/**
 * @brief Set LED configuration
 * @param config LED configuration
 * @return HAL status
 */
hal_status_t led_manager_set_config(const led_mgr_config_t *config);

/**
 * @brief Get LED configuration
 * @param config Pointer to store LED configuration
 * @return HAL status
 */
hal_status_t led_manager_get_config(led_mgr_config_t *config);

// Utility Functions

/**
 * @brief Get pattern type name as string
 * @param type Pattern type
 * @return Pattern type name string
 */
const char* led_manager_get_pattern_type_name(led_mgr_pattern_type_t type);

/**
 * @brief Get animation name as string
 * @param animation Animation type
 * @return Animation name string
 */
const char* led_manager_get_animation_name(led_mgr_animation_t animation);

/**
 * @brief Get priority name as string
 * @param priority Priority level
 * @return Priority name string
 */
const char* led_manager_get_priority_name(led_mgr_priority_t priority);

/**
 * @brief Get LED diagnostic information
 * @param info Pointer to store diagnostic info
 * @param max_len Maximum length of info string
 * @return HAL status
 */
hal_status_t led_manager_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Perform LED self-test
 * @return HAL status
 */
hal_status_t led_manager_self_test(void);

/**
 * @brief Reset LED manager
 * @return HAL status
 */
hal_status_t led_manager_reset(void);

// Predefined Pattern Functions

/**
 * @brief Set system ready pattern
 * @return HAL status
 */
hal_status_t led_manager_system_ready(void);

/**
 * @brief Set system error pattern
 * @return HAL status
 */
hal_status_t led_manager_system_error(void);

/**
 * @brief Set system warning pattern
 * @return HAL status
 */
hal_status_t led_manager_system_warning(void);

/**
 * @brief Set system shutdown pattern
 * @return HAL status
 */
hal_status_t led_manager_system_shutdown(void);

/**
 * @brief Set emergency stop pattern
 * @return HAL status
 */
hal_status_t led_manager_emergency_stop(void);

#endif // LED_MANAGER_H
