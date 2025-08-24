/**
 * @file safety_manager.h
 * @brief Safety Manager for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-02 (Safety Integration)
 */

#ifndef SAFETY_MANAGER_H
#define SAFETY_MANAGER_H

#include "hal_common.h"
#include "hal_estop.h"
#include "hal_led.h"
#include "system_state_machine.h"
#include <stdint.h>
#include <stdbool.h>

// Safety Levels
typedef enum {
    SAFETY_LEVEL_NORMAL = 0,     // Normal operation
    SAFETY_LEVEL_WARNING,        // Warning condition
    SAFETY_LEVEL_CRITICAL,       // Critical condition
    SAFETY_LEVEL_EMERGENCY       // Emergency stop
} safety_level_t;

// Safety Events
typedef enum {
    SAFETY_EVENT_NONE = 0,
    SAFETY_EVENT_ESTOP_TRIGGERED,    // E-Stop button pressed
    SAFETY_EVENT_ESTOP_RESET,        // E-Stop reset
    SAFETY_EVENT_SAFETY_FAULT,       // Safety fault detected
    SAFETY_EVENT_SAFETY_CLEARED,     // Safety fault cleared
    SAFETY_EVENT_EMERGENCY_STOP,     // Emergency stop triggered
    SAFETY_EVENT_SAFETY_TIMEOUT,     // Safety timeout
    SAFETY_EVENT_SYSTEM_FAULT,       // System fault
    SAFETY_EVENT_INTERLOCK_TRIGGERED, // Interlock triggered
    SAFETY_EVENT_INTERLOCK_RELEASED  // Interlock released
} safety_event_t;

// Safety Fault Types
typedef enum {
    SAFETY_FAULT_NONE = 0,
    SAFETY_FAULT_ESTOP_HARDWARE,     // E-Stop hardware fault
    SAFETY_FAULT_ESTOP_SOFTWARE,     // E-Stop software fault
    SAFETY_FAULT_SAFETY_CIRCUIT,     // Safety circuit fault
    SAFETY_FAULT_SENSOR_FAILURE,     // Sensor failure
    SAFETY_FAULT_COMMUNICATION,      // Communication fault
    SAFETY_FAULT_POWER_FAILURE,      // Power failure
    SAFETY_FAULT_OVERTEMPERATURE,    // Over-temperature
    SAFETY_FAULT_OVERCURRENT,        // Over-current
    SAFETY_FAULT_MECHANICAL_FAULT    // Mechanical fault
} safety_fault_t;

// Safety Status Structure
typedef struct {
    safety_level_t current_level;
    safety_event_t last_event;
    safety_fault_t current_fault;
    bool estop_triggered;
    bool interlock_triggered;
    bool safety_circuit_ok;
    bool sensors_ok;
    bool communication_ok;
    bool power_ok;
    uint64_t last_safety_check;
    uint32_t fault_count;
    uint32_t estop_count;
    uint32_t interlock_count;
    uint64_t uptime_seconds;
} safety_status_t;

// Safety Configuration Structure
typedef struct {
    uint32_t safety_check_interval_ms;
    uint32_t estop_response_timeout_ms;
    uint32_t safety_circuit_timeout_ms;
    uint32_t sensor_timeout_ms;
    bool enable_auto_recovery;
    bool enable_safety_monitoring;
    bool enable_estop_monitoring;
    bool enable_sensor_monitoring;
} safety_config_t;

// Safety Event Callback
typedef void (*safety_event_callback_t)(safety_event_t event, safety_fault_t fault);

// Function Prototypes

/**
 * @brief Initialize safety manager
 * @param config Safety configuration
 * @return HAL status
 */
hal_status_t safety_manager_init(const safety_config_t *config);

/**
 * @brief Deinitialize safety manager
 * @return HAL status
 */
hal_status_t safety_manager_deinit(void);

/**
 * @brief Update safety manager (call periodically)
 * @return HAL status
 */
hal_status_t safety_manager_update(void);

/**
 * @brief Process safety event
 * @param event Safety event to process
 * @return HAL status
 */
hal_status_t safety_manager_process_event(safety_event_t event);

/**
 * @brief Get safety status
 * @param status Pointer to store safety status
 * @return HAL status
 */
hal_status_t safety_manager_get_status(safety_status_t *status);

/**
 * @brief Check if system is safe
 * @param safe Pointer to store safety status
 * @return HAL status
 */
hal_status_t safety_manager_is_safe(bool *safe);

/**
 * @brief Trigger safety fault
 * @param fault Fault type to trigger
 * @return HAL status
 */
hal_status_t safety_manager_trigger_fault(safety_fault_t fault);

/**
 * @brief Clear safety fault
 * @return HAL status
 */
hal_status_t safety_manager_clear_fault(void);

/**
 * @brief Get current safety fault
 * @param fault Pointer to store current fault
 * @return HAL status
 */
hal_status_t safety_manager_get_fault(safety_fault_t *fault);

/**
 * @brief Set safety event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t safety_manager_set_callback(safety_event_callback_t callback);

/**
 * @brief Set safety configuration
 * @param config Safety configuration
 * @return HAL status
 */
hal_status_t safety_manager_set_config(const safety_config_t *config);

/**
 * @brief Get safety configuration
 * @param config Pointer to store safety configuration
 * @return HAL status
 */
hal_status_t safety_manager_get_config(safety_config_t *config);

// E-Stop Integration Functions

/**
 * @brief Handle E-Stop trigger
 * @return HAL status
 */
hal_status_t safety_manager_handle_estop_trigger(void);

/**
 * @brief Handle E-Stop reset
 * @return HAL status
 */
hal_status_t safety_manager_handle_estop_reset(void);

/**
 * @brief Trigger interlock
 * @return HAL status
 */
hal_status_t safety_manager_trigger_interlock(void);

/**
 * @brief Reset interlock
 * @return HAL status
 */
hal_status_t safety_manager_reset_interlock(void);

/**
 * @brief Handle E-Stop reset
 * @return HAL status
 */
hal_status_t safety_manager_handle_estop_reset(void);

/**
 * @brief Check E-Stop status
 * @param triggered Pointer to store E-Stop status
 * @return HAL status
 */
hal_status_t safety_manager_check_estop(bool *triggered);

// Safety Circuit Functions

/**
 * @brief Check safety circuit
 * @param ok Pointer to store safety circuit status
 * @return HAL status
 */
hal_status_t safety_manager_check_safety_circuit(bool *ok);

/**
 * @brief Validate safety circuit
 * @return HAL status
 */
hal_status_t safety_manager_validate_safety_circuit(void);

// Sensor Monitoring Functions

/**
 * @brief Check sensor status
 * @param ok Pointer to store sensor status
 * @return HAL status
 */
hal_status_t safety_manager_check_sensors(bool *ok);

/**
 * @brief Validate sensors
 * @return HAL status
 */
hal_status_t safety_manager_validate_sensors(void);

// Emergency Procedures

/**
 * @brief Execute emergency stop procedure
 * @return HAL status
 */
hal_status_t safety_manager_emergency_stop(void);

/**
 * @brief Execute safety shutdown procedure
 * @return HAL status
 */
hal_status_t safety_manager_safety_shutdown(void);

/**
 * @brief Execute safety recovery procedure
 * @return HAL status
 */
hal_status_t safety_manager_safety_recovery(void);

// Utility Functions

/**
 * @brief Get safety level name as string
 * @param level Safety level
 * @return Safety level name string
 */
const char* safety_manager_get_level_name(safety_level_t level);

/**
 * @brief Get safety event name as string
 * @param event Safety event
 * @return Safety event name string
 */
const char* safety_manager_get_event_name(safety_event_t event);

/**
 * @brief Get safety fault name as string
 * @param fault Safety fault
 * @return Safety fault name string
 */
const char* safety_manager_get_fault_name(safety_fault_t fault);

/**
 * @brief Get safety diagnostic information
 * @param info Pointer to store diagnostic info
 * @param max_len Maximum length of info string
 * @return HAL status
 */
hal_status_t safety_manager_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Perform safety self-test
 * @return HAL status
 */
hal_status_t safety_manager_self_test(void);

/**
 * @brief Validate safety compliance
 * @param compliant Pointer to store compliance status
 * @return HAL status
 */
hal_status_t safety_manager_validate_compliance(bool *compliant);

#endif // SAFETY_MANAGER_H
