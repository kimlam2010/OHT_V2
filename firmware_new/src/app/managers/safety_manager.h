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
#include "safety_types.h"
#include <stdint.h>
#include <stdbool.h>

// Safety Levels - Using unified safety_level_t from safety_types.h
// Safety Events - Using unified safety_event_t from safety_types.h
// Safety Fault Types - Using unified safety_fault_t from safety_types.h

// Safety Status Structure - Using unified safety_status_info_t from safety_types.h
// Safety Configuration Structure - Using unified safety_config_t from safety_types.h

// Safety Event Callback - Using unified safety_event_callback_t from safety_types.h

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
hal_status_t safety_manager_get_status(safety_status_info_t *status);

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
