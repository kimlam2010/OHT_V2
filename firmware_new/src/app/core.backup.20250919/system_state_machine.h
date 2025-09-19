/**
 * @file system_state_machine.h
 * @brief System State Machine for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-01 (System State Machine Implementation)
 */

#ifndef SYSTEM_STATE_MACHINE_H
#define SYSTEM_STATE_MACHINE_H

#include "hal_common.h"
#include "hal_estop.h"
#include "hal_led.h"
#include "hal_relay.h"
#include "hal_rs485.h"
#include "hal_network.h"
#include <stdint.h>
#include <stdbool.h>

// System States
typedef enum {
    SYSTEM_STATE_INIT = 0,        // Initialization state
    SYSTEM_STATE_IDLE,            // Idle state - ready for commands
    SYSTEM_STATE_MOVE,            // Moving state
    SYSTEM_STATE_DOCK,            // Docking state
    SYSTEM_STATE_FAULT,           // Fault state
    SYSTEM_STATE_ESTOP,           // Emergency stop state
    SYSTEM_STATE_SHUTDOWN,        // Shutdown state
    
    // ✅ NEW STATES ADDED (DO NOT MODIFY ABOVE)
    SYSTEM_STATE_BOOT,            // Boot/startup state
    SYSTEM_STATE_PAUSED,          // Paused state (can resume)
    SYSTEM_STATE_CONFIG,          // Configuration state
    SYSTEM_STATE_SAFE,            // Safe recovery state
    SYSTEM_STATE_MAX              // Maximum state count (for validation)
} system_state_t;

// System Events
typedef enum {
    SYSTEM_EVENT_NONE = 0,
    SYSTEM_EVENT_INIT_COMPLETE,   // Initialization complete
    SYSTEM_EVENT_MOVE_COMMAND,    // Move command received
    SYSTEM_EVENT_DOCK_COMMAND,    // Dock command received
    SYSTEM_EVENT_STOP_COMMAND,    // Stop command received
    SYSTEM_EVENT_ESTOP_TRIGGERED, // E-Stop triggered
    SYSTEM_EVENT_FAULT_DETECTED,  // Fault detected
    SYSTEM_EVENT_FAULT_CLEARED,   // Fault cleared
    SYSTEM_EVENT_ESTOP_RESET,     // E-Stop reset
    SYSTEM_EVENT_SHUTDOWN,        // Shutdown command
    SYSTEM_EVENT_TIMEOUT,         // Timeout event
    SYSTEM_EVENT_ERROR,           // Error event
    
    // ✅ NEW EVENTS ADDED (DO NOT MODIFY ABOVE)
    SYSTEM_EVENT_BOOT_COMPLETE,   // Boot sequence complete
    SYSTEM_EVENT_PAUSE_COMMAND,   // Pause command received
    SYSTEM_EVENT_RESUME_COMMAND,  // Resume command received
    SYSTEM_EVENT_CONFIG_COMMAND,  // Configuration command
    SYSTEM_EVENT_CONFIG_COMPLETE, // Configuration complete
    SYSTEM_EVENT_CONFIG_FAILED,   // Configuration failed
    SYSTEM_EVENT_SAFE_RESET,      // Safe reset command
    SYSTEM_EVENT_MAX              // Maximum event count
} system_event_t;

// System Fault Types
typedef enum {
    SYSTEM_FAULT_NONE = 0,
    SYSTEM_FAULT_ESTOP,           // E-Stop fault
    SYSTEM_FAULT_COMMUNICATION,   // Communication fault
    SYSTEM_FAULT_SENSOR,          // Sensor fault
    SYSTEM_FAULT_MOTOR,           // Motor fault
    SYSTEM_FAULT_POWER,           // Power fault
    SYSTEM_FAULT_SOFTWARE,        // Software fault
    SYSTEM_FAULT_HARDWARE         // Hardware fault
} system_fault_t;

// System Status Structure
typedef struct {
    system_state_t current_state;
    system_state_t previous_state;
    system_event_t last_event;
    system_fault_t current_fault;
    uint64_t state_entry_time;
    uint64_t last_update_time;
    uint32_t state_transition_count;
    bool system_ready;
    bool safety_ok;
    bool communication_ok;
    bool sensors_ok;
    bool location_ok;
    bool target_valid;
    
    // ✅ NEW FIELDS ADDED (DO NOT MODIFY ABOVE)
    uint64_t state_entry_timestamp;     // When current state was entered
    uint64_t state_duration_ms;         // How long in current state
    uint32_t state_timeout_ms;          // Timeout for current state (0 = no timeout)
    uint32_t total_state_transitions;   // Total number of transitions
    bool state_timeout_enabled;        // Is timeout monitoring enabled
} system_status_t;

// System Configuration Structure
typedef struct {
    uint32_t state_timeout_ms;
    uint32_t update_period_ms;
    bool auto_recovery_enabled;
    bool safety_monitoring_enabled;
    bool communication_monitoring_enabled;
    bool sensor_monitoring_enabled;
} system_config_t;

// System Event Callback
typedef void (*system_event_callback_t)(system_state_t state, system_event_t event);

// Function Prototypes

/**
 * @brief Initialize system state machine
 * @param config System configuration
 * @return HAL status
 */
hal_status_t system_state_machine_init(const system_config_t *config);

/**
 * @brief Deinitialize system state machine
 * @return HAL status
 */
hal_status_t system_state_machine_deinit(void);

/**
 * @brief Process system event
 * @param event System event to process
 * @return HAL status
 */
hal_status_t system_state_machine_process_event(system_event_t event);

/**
 * @brief Update system state machine (call periodically)
 * @return HAL status
 */
hal_status_t system_state_machine_update(void);

/**
 * @brief Get current system state
 * @param state Pointer to store current state
 * @return HAL status
 */
hal_status_t system_state_machine_get_state(system_state_t *state);

/**
 * @brief Get system status
 * @param status Pointer to store system status
 * @return HAL status
 */
hal_status_t system_state_machine_get_status(system_status_t *status);

/**
 * @brief Set system event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t system_state_machine_set_callback(system_event_callback_t callback);

/**
 * @brief Check if system is ready
 * @param ready Pointer to store ready status
 * @return HAL status
 */
hal_status_t system_state_machine_is_ready(bool *ready);

/**
 * @brief Check if system is safe
 * @param safe Pointer to store safety status
 * @return HAL status
 */
hal_status_t system_state_machine_is_safe(bool *safe);

/**
 * @brief Trigger system fault
 * @param fault Fault type to trigger
 * @return HAL status
 */
hal_status_t system_state_machine_trigger_fault(system_fault_t fault);

/**
 * @brief Clear system fault
 * @return HAL status
 */
hal_status_t system_state_machine_clear_fault(void);

/**
 * @brief Get current fault
 * @param fault Pointer to store current fault
 * @return HAL status
 */
hal_status_t system_state_machine_get_fault(system_fault_t *fault);

/**
 * @brief Set system configuration
 * @param config System configuration
 * @return HAL status
 */
hal_status_t system_state_machine_set_config(const system_config_t *config);

/**
 * @brief Get system configuration
 * @param config Pointer to store system configuration
 * @return HAL status
 */
hal_status_t system_state_machine_get_config(system_config_t *config);

/**
 * @brief Set location system status
 * @param ok true if location system is healthy/ready
 * @return HAL status
 */
hal_status_t system_state_machine_set_location_ok(bool ok);

/**
 * @brief Set target validity status (e.g., navigation target validated)
 * @param valid true if target is valid
 * @return HAL status
 */
hal_status_t system_state_machine_set_target_valid(bool valid);

// State-specific Functions

/**
 * @brief Enter IDLE state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_idle(void);

/**
 * @brief Enter MOVE state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_move(void);

/**
 * @brief Enter DOCK state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_dock(void);

/**
 * @brief Enter FAULT state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_fault(void);

/**
 * @brief Enter E-STOP state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_estop(void);

/**
 * @brief Enter SHUTDOWN state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_shutdown(void);

// Utility Functions

/**
 * @brief Get state name as string
 * @param state System state
 * @return State name string
 */
const char* system_state_machine_get_state_name(system_state_t state);

/**
 * @brief Get event name as string
 * @param event System event
 * @return Event name string
 */
const char* system_state_machine_get_event_name(system_event_t event);

/**
 * @brief Get fault name as string
 * @param fault System fault
 * @return Fault name string
 */
const char* system_state_machine_get_fault_name(system_fault_t fault);

/**
 * @brief Validate state transition
 * @param from_state Current state
 * @param to_state Target state
 * @return true if transition is valid
 */
bool system_state_machine_validate_transition(system_state_t from_state, system_state_t to_state);

/**
 * @brief Get system diagnostic information
 * @param info Pointer to store diagnostic info
 * @param max_len Maximum length of info string
 * @return HAL status
 */
hal_status_t system_state_machine_get_diagnostics(char *info, size_t max_len);

// ✅ NEW FUNCTION DECLARATIONS AND STRUCTURES ADDED

// State timeout management structure
typedef struct {
    system_state_t state;
    uint32_t timeout_ms;
    uint64_t entry_time;
    bool enabled;
} state_timeout_config_t;

// State statistics structure
typedef struct {
    uint64_t time_in_boot_ms;
    uint64_t time_in_idle_ms;
    uint64_t time_in_moving_ms;
    uint64_t time_in_paused_ms;
    uint64_t time_in_docking_ms;
    uint64_t time_in_config_ms;
    uint64_t time_in_fault_ms;
    uint64_t time_in_estop_ms;
    uint64_t time_in_safe_ms;
    uint32_t total_transitions;
    uint32_t emergency_count;
    uint32_t fault_count;
} system_state_statistics_t;

/**
 * @brief Enter BOOT state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_boot(void);

/**
 * @brief Enter PAUSED state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_paused(void);

/**
 * @brief Enter CONFIG state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_config(void);

/**
 * @brief Enter SAFE state
 * @return HAL status
 */
hal_status_t system_state_machine_enter_safe(void);

/**
 * @brief Resume from PAUSED state
 * @return HAL status
 */
hal_status_t system_state_machine_resume_from_pause(void);

/**
 * @brief Set state timeout for specific state
 * @param state System state to set timeout for
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return HAL status
 */
hal_status_t system_state_machine_set_state_timeout(system_state_t state, uint32_t timeout_ms);

/**
 * @brief Get state statistics
 * @param stats Pointer to store state statistics
 * @return HAL status
 */
hal_status_t system_state_machine_get_state_statistics(system_state_statistics_t *stats);

#endif // SYSTEM_STATE_MACHINE_H
