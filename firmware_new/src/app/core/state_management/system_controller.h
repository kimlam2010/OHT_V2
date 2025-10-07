/**
 * @file system_controller.h
 * @brief System Controller for OHT-50 Master Module - Main System Coordination
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (System Controller Implementation)
 */

#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include "hal_common.h"
#include "system_state_machine.h"
#include "../safety/safety_monitor.h"
#include "../control/control_loop.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// System Controller States
typedef enum {
    SYSTEM_CONTROLLER_STATE_INIT = 0,        // Initialization state
    SYSTEM_CONTROLLER_STATE_IDLE,            // Idle state
    SYSTEM_CONTROLLER_STATE_ACTIVE,          // Active state
    SYSTEM_CONTROLLER_STATE_FAULT,           // Fault state
    SYSTEM_CONTROLLER_STATE_EMERGENCY,       // Emergency state
    SYSTEM_CONTROLLER_STATE_SHUTDOWN         // Shutdown state
} system_controller_state_t;

// System Controller Events
typedef enum {
    SYSTEM_CONTROLLER_EVENT_NONE = 0,
    SYSTEM_CONTROLLER_EVENT_INIT_COMPLETE,   // Initialization complete
    SYSTEM_CONTROLLER_EVENT_ACTIVATE,        // Activate system
    SYSTEM_CONTROLLER_EVENT_DEACTIVATE,      // Deactivate system
    SYSTEM_CONTROLLER_EVENT_FAULT_DETECTED,  // Fault detected
    SYSTEM_CONTROLLER_EVENT_FAULT_CLEARED,   // Fault cleared
    SYSTEM_CONTROLLER_EVENT_EMERGENCY,       // Emergency event
    SYSTEM_CONTROLLER_EVENT_SHUTDOWN,        // Shutdown command
    SYSTEM_CONTROLLER_EVENT_ERROR            // Error event
} system_controller_event_t;

// System Controller Error Types
typedef enum {
    SYSTEM_CONTROLLER_ERROR_NONE = 0,
    SYSTEM_CONTROLLER_ERROR_INIT_FAILED,     // Initialization failed
    SYSTEM_CONTROLLER_ERROR_SAFETY_VIOLATION, // Safety violation
    SYSTEM_CONTROLLER_ERROR_COMMUNICATION,   // Communication error
    SYSTEM_CONTROLLER_ERROR_CONTROL,         // Control error
    SYSTEM_CONTROLLER_ERROR_MEMORY,          // Memory error
    SYSTEM_CONTROLLER_ERROR_TIMEOUT,         // Timeout error
    SYSTEM_CONTROLLER_ERROR_HARDWARE         // Hardware error
} system_controller_error_t;

// System Controller Configuration
typedef struct {
    uint32_t update_period_ms;               // Controller update period
    uint32_t timeout_ms;                     // General timeout
    uint32_t error_retry_count;              // Error retry count
    uint32_t error_retry_delay_ms;           // Error retry delay
    bool enable_auto_recovery;               // Enable auto recovery
    bool enable_error_logging;               // Enable error logging
    bool enable_performance_monitoring;      // Enable performance monitoring
    bool enable_diagnostics;                 // Enable diagnostics
    uint32_t max_error_count;                // Maximum error count
    uint32_t error_reset_timeout_ms;         // Error reset timeout
} system_controller_config_t;

// System Controller Status
typedef struct {
    system_controller_state_t current_state; // Current state
    system_controller_state_t previous_state; // Previous state
    system_controller_event_t last_event;    // Last event
    system_controller_error_t current_error; // Current error
    uint64_t state_entry_time;               // State entry time
    uint64_t last_update_time;               // Last update time
    uint32_t state_transition_count;         // State transition count
    bool system_ready;                       // System ready
    bool safety_ok;                          // Safety OK
    bool communication_ok;                   // Communication OK
    bool control_ok;                         // Control OK
    uint32_t error_count;                    // Error count
    uint32_t recovery_count;                 // Recovery count
    uint64_t uptime_ms;                      // System uptime
} system_controller_status_t;

// System Controller Statistics
typedef struct {
    uint32_t total_events;                   // Total events
    uint32_t error_events;                   // Error events
    uint32_t recovery_events;                // Recovery events
    uint32_t state_transitions;              // State transitions
    uint64_t total_uptime_ms;                // Total uptime
    uint64_t active_uptime_ms;               // Active uptime
    uint32_t performance_metrics[16];        // Performance metrics
    uint32_t error_metrics[16];              // Error metrics
} system_controller_stats_t;

// System Controller Performance Metrics
typedef struct {
    uint32_t cpu_usage_percent;              // CPU usage percentage
    uint32_t memory_usage_percent;           // Memory usage percentage
    uint32_t update_frequency_hz;            // Update frequency
    uint32_t response_time_ms;               // Response time
    uint32_t error_rate_percent;             // Error rate
    uint32_t recovery_time_ms;               // Recovery time
} system_controller_performance_t;

// System Controller Error Information
typedef struct {
    system_controller_error_t error_type;    // Error type
    uint32_t error_code;                     // Error code
    uint64_t error_time;                     // Error time
    char error_message[256];                 // Error message
    char error_context[256];                 // Error context
    uint32_t error_count;                    // Error count
    bool error_resolved;                     // Error resolved
} system_controller_error_info_t;

// System Controller Event Callback
typedef void (*system_controller_event_callback_t)(system_controller_state_t state, 
                                                  system_controller_event_t event, 
                                                  const char* details);

// System Controller Error Callback
typedef void (*system_controller_error_callback_t)(system_controller_error_t error, 
                                                  const char* message, 
                                                  const char* context);

// Function Prototypes

/**
 * @brief Initialize system controller
 * @param config System controller configuration
 * @return HAL status
 */
hal_status_t system_controller_init(const system_controller_config_t *config);

/**
 * @brief Deinitialize system controller
 * @return HAL status
 */
hal_status_t system_controller_deinit(void);

/**
 * @brief Update system controller (call periodically)
 * @return HAL status
 */
hal_status_t system_controller_update(void);

/**
 * @brief Process system controller event
 * @param event Event to process
 * @param details Event details
 * @return HAL status
 */
hal_status_t system_controller_process_event(system_controller_event_t event, const char* details);

/**
 * @brief Get system controller status
 * @param status Pointer to store status
 * @return HAL status
 */
hal_status_t system_controller_get_status(system_controller_status_t *status);

/**
 * @brief Get system controller statistics
 * @param stats Pointer to store statistics
 * @return HAL status
 */
hal_status_t system_controller_get_stats(system_controller_stats_t *stats);

/**
 * @brief Get system controller performance metrics
 * @param performance Pointer to store performance metrics
 * @return HAL status
 */
hal_status_t system_controller_get_performance(system_controller_performance_t *performance);

/**
 * @brief Get system controller error information
 * @param error_info Pointer to store error information
 * @return HAL status
 */
hal_status_t system_controller_get_error_info(system_controller_error_info_t *error_info);

/**
 * @brief Check if system is ready
 * @param ready Pointer to store ready status
 * @return HAL status
 */
hal_status_t system_controller_is_ready(bool *ready);

/**
 * @brief Activate system
 * @return HAL status
 */
hal_status_t system_controller_activate(void);

/**
 * @brief Deactivate system
 * @return HAL status
 */
hal_status_t system_controller_deactivate(void);

/**
 * @brief Trigger system fault
 * @param error_type Error type
 * @param message Error message
 * @param context Error context
 * @return HAL status
 */
hal_status_t system_controller_trigger_fault(system_controller_error_t error_type, 
                                            const char* message, 
                                            const char* context);

/**
 * @brief Clear system fault
 * @return HAL status
 */
hal_status_t system_controller_clear_fault(void);

/**
 * @brief Emergency shutdown
 * @param reason Shutdown reason
 * @return HAL status
 */
hal_status_t system_controller_emergency_shutdown(const char* reason);

/**
 * @brief Set system controller configuration
 * @param config System controller configuration
 * @return HAL status
 */
hal_status_t system_controller_set_config(const system_controller_config_t *config);

/**
 * @brief Get system controller configuration
 * @param config Pointer to store configuration
 * @return HAL status
 */
hal_status_t system_controller_get_config(system_controller_config_t *config);

/**
 * @brief Set system controller event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t system_controller_set_event_callback(system_controller_event_callback_t callback);

/**
 * @brief Set system controller error callback
 * @param callback Error callback function
 * @return HAL status
 */
hal_status_t system_controller_set_error_callback(system_controller_error_callback_t callback);

/**
 * @brief Run system diagnostics
 * @param result Diagnostic result
 * @param max_size Maximum result size
 * @return HAL status
 */
hal_status_t system_controller_run_diagnostics(char* result, size_t max_size);

/**
 * @brief Clear system statistics
 * @return HAL status
 */
hal_status_t system_controller_clear_stats(void);

/**
 * @brief Reset system controller
 * @return HAL status
 */
hal_status_t system_controller_reset(void);

/**
 * @brief Get system controller version
 * @return Version string
 */
const char* system_controller_get_version(void);

/**
 * @brief Handle error with recovery mechanism
 * @param error_code Error code
 * @param context Error context
 * @return HAL status
 */
hal_status_t system_controller_handle_error(hal_status_t error_code, const char* context);

/**
 * @brief Log system event
 * @param event Event type
 * @param details Event details
 * @return HAL status
 */
hal_status_t system_controller_log_event(system_controller_event_t event, const char* details);

/**
 * @brief Log system error
 * @param error Error type
 * @param message Error message
 * @param context Error context
 * @return HAL status
 */
hal_status_t system_controller_log_error(system_controller_error_t error, 
                                        const char* message, 
                                        const char* context);

/**
 * @brief Recover from error
 * @param error_code Error code
 * @return HAL status
 */
hal_status_t system_controller_recover_from_error(hal_status_t error_code);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_CONTROLLER_H
