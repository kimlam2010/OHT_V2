/**
 * @file graduated_response_system.h
 * @brief Graduated Response System for OHT-50 Safety Monitor
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 2.3 - Graduated Response System Implementation
 * 
 * 🎛️ GRADUATED RESPONSE: This module implements the graduated response system
 * 💡 LED COORDINATION: Manages LED patterns based on system response level
 * ⚡ ADAPTIVE POLLING: Implements adaptive polling based on module health
 * 🔄 RECOVERY LOGIC: Handles automatic escalation and de-escalation
 */

#ifndef GRADUATED_RESPONSE_SYSTEM_H
#define GRADUATED_RESPONSE_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

// Include existing headers
#include "critical_module_detector.h"
#include "../../../hal/peripherals/hal_led.h"
#include "../../../hal/common/hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// 💡 **LED PATTERN DEFINITIONS**

/**
 * @brief Safety Response LED Patterns
 * 
 * Defines LED patterns for each safety response level.
 */
typedef struct {
    // System Status LED (Blue - GPIO1_A3)
    led_pattern_t system_led_pattern;
    uint32_t system_blink_period_ms;
    
    // Communication LED (Yellow - GPIO0_D4)  
    led_pattern_t comm_led_pattern;
    uint32_t comm_blink_period_ms;
    
    // Network LED (Green - GPIO0_D5)
    led_pattern_t network_led_pattern;
    uint32_t network_blink_period_ms;
    
    // Error LED (Red - GPIO1_D2)
    led_pattern_t error_led_pattern;
    uint32_t error_blink_period_ms;
    
    // Power LED (Green - GPIO1_D6)
    led_pattern_t power_led_pattern;
    uint32_t power_blink_period_ms;
    
} safety_response_led_pattern_t;

/**
 * @brief Response Level Transition
 * 
 * Defines transition behavior between response levels.
 */
typedef struct {
    safety_response_level_t from_level;      // Source response level
    safety_response_level_t to_level;        // Target response level
    uint32_t transition_delay_ms;            // Delay before transition
    bool requires_confirmation;              // Requires manual confirmation
    bool auto_recovery_enabled;              // Automatic recovery allowed
    uint32_t recovery_timeout_ms;            // Time before auto-recovery
} response_level_transition_t;

/**
 * @brief Adaptive Polling Configuration
 * 
 * Defines adaptive polling intervals for different module states.
 */
typedef struct {
    uint8_t module_address;                  // Module address
    
    // Polling intervals by health status (in milliseconds)
    uint32_t healthy_interval_ms;            // Normal polling interval
    uint32_t degraded_interval_ms;           // Degraded state polling
    uint32_t failing_interval_ms;            // Failing state polling
    uint32_t failed_interval_ms;             // Failed state polling
    uint32_t offline_interval_ms;            // Offline state polling
    
    // Adaptive scaling factors
    float performance_scale_factor;          // Scale based on performance
    float success_rate_scale_factor;         // Scale based on success rate
    uint32_t min_interval_ms;                // Minimum polling interval
    uint32_t max_interval_ms;                // Maximum polling interval
    
} adaptive_polling_config_t;

/**
 * @brief Graduated Response System Status
 * 
 * Current status of the graduated response system.
 */
typedef struct {
    bool system_active;                      // System is active
    safety_response_level_t current_level;   // Current response level
    safety_response_level_t previous_level;  // Previous response level
    uint64_t level_entry_time_ms;            // When current level was entered
    uint64_t last_level_change_ms;           // Last level change timestamp
    
    // LED status
    bool led_patterns_active;                // LED patterns are active
    uint64_t last_led_update_ms;             // Last LED update timestamp
    uint32_t led_update_interval_ms;         // LED update interval
    
    // Polling status
    bool adaptive_polling_active;            // Adaptive polling is active
    uint64_t last_polling_update_ms;         // Last polling update
    uint32_t polling_update_interval_ms;     // Polling update interval
    
    // Transition status
    bool transition_in_progress;             // Level transition in progress
    safety_response_level_t transition_target; // Target level for transition
    uint64_t transition_start_time_ms;       // When transition started
    uint32_t transition_timeout_ms;          // Transition timeout
    
} graduated_response_status_t;

/**
 * @brief Response System Statistics
 * 
 * Statistics for the graduated response system.
 */
typedef struct {
    // Level statistics
    uint32_t normal_level_count;             // Times in normal level
    uint32_t monitoring_level_count;         // Times in monitoring level
    uint32_t warning_level_count;            // Times in warning level
    uint32_t critical_level_count;           // Times in critical level
    uint32_t emergency_level_count;          // Times in emergency level
    
    // Transition statistics
    uint32_t total_transitions;              // Total level transitions
    uint32_t escalation_count;               // Escalations (level up)
    uint32_t de_escalation_count;            // De-escalations (level down)
    uint32_t auto_recoveries;                // Automatic recoveries
    uint32_t manual_recoveries;              // Manual recoveries
    
    // Time statistics
    uint64_t total_time_normal_ms;           // Total time in normal
    uint64_t total_time_monitoring_ms;       // Total time in monitoring
    uint64_t total_time_warning_ms;          // Total time in warning
    uint64_t total_time_critical_ms;         // Total time in critical
    uint64_t total_time_emergency_ms;        // Total time in emergency
    
    // Performance statistics
    uint32_t led_update_count;               // LED updates performed
    uint32_t polling_adjustments;            // Polling interval adjustments
    uint32_t avg_transition_time_ms;         // Average transition time
    
} graduated_response_stats_t;

// 🔧 **FUNCTION PROTOTYPES**

// System Management Functions

/**
 * @brief Initialize Graduated Response System
 * @return HAL status
 * 
 * 🎛️ INITIALIZATION: Sets up LED patterns and adaptive polling
 */
hal_status_t graduated_response_init(void);

/**
 * @brief Deinitialize Graduated Response System
 * @return HAL status
 */
hal_status_t graduated_response_deinit(void);

/**
 * @brief Start Graduated Response System
 * @return HAL status
 * 
 * 🚀 ACTIVATION: Begins LED pattern management and adaptive polling
 */
hal_status_t graduated_response_start(void);

/**
 * @brief Stop Graduated Response System
 * @return HAL status
 */
hal_status_t graduated_response_stop(void);

/**
 * @brief Update Graduated Response System
 * @return HAL status
 * 
 * 🔄 UPDATE: Main update function called from safety monitor
 * This function:
 * - Updates LED patterns based on current response level
 * - Adjusts polling intervals based on module health
 * - Handles level transitions
 * - Manages recovery logic
 */
hal_status_t graduated_response_update(void);

// Response Level Management

/**
 * @brief Set system response level
 * @param new_level New response level
 * @param reason Reason for level change
 * @return HAL status
 * 
 * 🎛️ LEVEL CONTROL: Changes system response level with proper validation
 */
hal_status_t graduated_response_set_level(safety_response_level_t new_level, const char *reason);

/**
 * @brief Get current system response level
 * @param current_level Output current response level
 * @return HAL status
 */
hal_status_t graduated_response_get_level(safety_response_level_t *current_level);

/**
 * @brief Check if level transition is allowed
 * @param from_level Source level
 * @param to_level Target level
 * @return true if transition is allowed
 */
bool graduated_response_is_transition_allowed(safety_response_level_t from_level, 
                                             safety_response_level_t to_level);

/**
 * @brief Handle automatic recovery
 * @return HAL status
 * 
 * 🔄 RECOVERY: Handles automatic de-escalation when conditions improve
 */
hal_status_t graduated_response_handle_auto_recovery(void);

// LED Pattern Management

/**
 * @brief Update LED patterns based on response level
 * @param response_level Current response level
 * @return HAL status
 * 
 * 💡 LED CONTROL: Updates all LED patterns to reflect system status
 */
hal_status_t graduated_response_update_led_patterns(safety_response_level_t response_level);

/**
 * @brief Get LED pattern for response level
 * @param response_level Response level
 * @param led_pattern Output LED pattern configuration
 * @return HAL status
 */
hal_status_t graduated_response_get_led_pattern(safety_response_level_t response_level, 
                                               safety_response_led_pattern_t *led_pattern);

/**
 * @brief Set custom LED pattern
 * @param response_level Response level
 * @param led_pattern LED pattern configuration
 * @return HAL status
 */
hal_status_t graduated_response_set_led_pattern(safety_response_level_t response_level, 
                                               const safety_response_led_pattern_t *led_pattern);

// Adaptive Polling Management

/**
 * @brief Get adaptive polling interval for module
 * @param module_addr Module address
 * @param polling_interval Output polling interval in milliseconds
 * @return HAL status
 * 
 * ⚡ ADAPTIVE POLLING: Returns appropriate polling interval based on module health
 */
hal_status_t graduated_response_get_polling_interval(uint8_t module_addr, uint32_t *polling_interval);

/**
 * @brief Update polling intervals for all modules
 * @return HAL status
 * 
 * ⚡ POLLING UPDATE: Adjusts polling intervals based on current module health
 */
hal_status_t graduated_response_update_polling_intervals(void);

/**
 * @brief Set adaptive polling configuration
 * @param module_addr Module address
 * @param config Polling configuration
 * @return HAL status
 */
hal_status_t graduated_response_set_polling_config(uint8_t module_addr, 
                                                  const adaptive_polling_config_t *config);

// Status and Statistics

/**
 * @brief Get graduated response system status
 * @param status Output status structure
 * @return HAL status
 */
hal_status_t graduated_response_get_status(graduated_response_status_t *status);

/**
 * @brief Get graduated response system statistics
 * @param stats Output statistics structure
 * @return HAL status
 */
hal_status_t graduated_response_get_statistics(graduated_response_stats_t *stats);

/**
 * @brief Reset graduated response system statistics
 * @return HAL status
 */
hal_status_t graduated_response_reset_statistics(void);

// Utility Functions

/**
 * @brief Get response level name string
 * @param level Response level
 * @return Response level name string
 */
const char* graduated_response_get_level_name(safety_response_level_t level);

/**
 * @brief Get LED pattern name string
 * @param pattern LED pattern
 * @return LED pattern name string
 */
const char* graduated_response_get_led_pattern_name(led_pattern_t pattern);

/**
 * @brief Calculate time in current level
 * @return Time in current level (milliseconds)
 */
uint32_t graduated_response_get_time_in_level(void);

/**
 * @brief Check if system is in safe state
 * @return true if in normal or monitoring level
 */
bool graduated_response_is_safe_state(void);

/**
 * @brief Check if system requires attention
 * @return true if in warning, critical, or emergency level
 */
bool graduated_response_requires_attention(void);

// Testing and Diagnostics

/**
 * @brief Perform self-test of graduated response system
 * @return HAL status
 * 
 * 🧪 TESTING: Tests LED patterns and polling adjustments
 */
hal_status_t graduated_response_self_test(void);

/**
 * @brief Get diagnostic information
 * @param info Buffer for diagnostic info
 * @param max_len Maximum buffer length
 * @return HAL status
 */
hal_status_t graduated_response_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Enable/disable debug logging
 * @param enable Enable debug logging
 * @return HAL status
 */
hal_status_t graduated_response_set_debug_logging(bool enable);

/**
 * @brief Test LED patterns manually
 * @param level Response level to test
 * @param duration_ms Test duration in milliseconds
 * @return HAL status
 * 
 * 🧪 TESTING: Manually test LED patterns for specific response level
 */
hal_status_t graduated_response_test_led_patterns(safety_response_level_t level, uint32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif // GRADUATED_RESPONSE_SYSTEM_H

/**
 * 📋 **IMPLEMENTATION NOTES:**
 * 
 * 🎛️ **GRADUATED RESPONSE LEVELS:**
 * 
 * | Level | LED Pattern | Polling | Description |
 * |-------|-------------|---------|-------------|
 * | NORMAL | Green steady | Normal intervals | All systems healthy |
 * | MONITORING | Green blink | Normal intervals | Minor issues detected |
 * | WARNING | Yellow steady | Increased intervals | Attention required |
 * | CRITICAL | Red blink | High frequency | Critical issues |
 * | EMERGENCY | Red steady + alarm | Maximum frequency | Emergency response |
 * 
 * 💡 **LED PATTERN MAPPING:**
 * 
 * - **System LED (Blue):** Overall system status
 * - **Communication LED (Yellow):** RS485 communication status
 * - **Network LED (Green):** Network connectivity status
 * - **Error LED (Red):** Error and emergency status
 * - **Power LED (Green):** Power system status
 * 
 * ⚡ **ADAPTIVE POLLING STRATEGY:**
 * 
 * - **Healthy modules:** Standard polling intervals
 * - **Degraded modules:** 1.5x faster polling
 * - **Failing modules:** 2x faster polling
 * - **Failed modules:** 4x faster polling (for recovery detection)
 * - **Offline modules:** Reduced polling to save resources
 * 
 * 🔄 **RECOVERY LOGIC:**
 * 
 * - **Automatic de-escalation:** When all modules return to healthy state
 * - **Hysteresis:** Prevents rapid level changes
 * - **Confirmation delays:** Ensures stable transitions
 * - **Manual override:** Operator can force level changes
 * 
 * 🧪 **TESTING STRATEGY:**
 * 
 * - **LED pattern testing:** Manual testing of all LED patterns
 * - **Polling validation:** Verify adaptive polling adjustments
 * - **Transition testing:** Test all level transitions
 * - **Recovery testing:** Test automatic recovery scenarios
 * - **Performance testing:** Ensure minimal system overhead
 */
