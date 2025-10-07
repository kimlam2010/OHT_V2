/**
 * @file safety_types.h
 * @brief Unified safety type definitions for OHT-50 Firmware
 * @brief MISRA C:2012 compliant safety types
 */

#ifndef OHT50_SAFETY_TYPES_H
#define OHT50_SAFETY_TYPES_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// SAFETY EVENT TYPES
// ============================================================================

/**
 * @brief Safety event types
 */
typedef enum {
    SAFETY_EVENT_NONE = 0,
    SAFETY_EVENT_ESTOP_PRESSED,
    SAFETY_EVENT_ESTOP_RELEASED,
    SAFETY_EVENT_SAFETY_ZONE_VIOLATION,
    SAFETY_EVENT_SAFETY_ZONE_CLEAR,
    SAFETY_EVENT_FAULT_DETECTED,
    SAFETY_EVENT_FAULT_CLEARED,
    SAFETY_EVENT_EMERGENCY_STOP,
    SAFETY_EVENT_SAFETY_RESET,
    SAFETY_EVENT_MAX
} safety_event_t;

// ============================================================================
// SAFETY FAULT TYPES
// ============================================================================

/**
 * @brief Safety fault types
 */
typedef enum {
    SAFETY_FAULT_NONE = 0,
    SAFETY_FAULT_ESTOP_ACTIVATED,
    SAFETY_FAULT_SAFETY_ZONE_VIOLATION,
    SAFETY_FAULT_OVERTEMPERATURE,
    SAFETY_FAULT_OVERCURRENT,
    SAFETY_FAULT_OVERVOLTAGE,
    SAFETY_FAULT_UNDERVOLTAGE,
    SAFETY_FAULT_COMMUNICATION_LOSS,
    SAFETY_FAULT_SENSOR_FAILURE,
    SAFETY_FAULT_ACTUATOR_FAILURE,
    SAFETY_FAULT_SYSTEM_FAILURE,
    SAFETY_FAULT_MAX
} safety_fault_t;

// ============================================================================
// SAFETY LEVEL TYPES
// ============================================================================

/**
 * @brief Safety level types
 */
typedef enum {
    SAFETY_LEVEL_NORMAL = 0,
    SAFETY_LEVEL_WARNING,
    SAFETY_LEVEL_CRITICAL,
    SAFETY_LEVEL_EMERGENCY,
    SAFETY_LEVEL_MAX
} safety_level_t;

// ============================================================================
// SAFETY STATUS TYPES
// ============================================================================

/**
 * @brief Safety status types
 */
typedef enum {
    SAFETY_STATUS_OK = 0,
    SAFETY_STATUS_WARNING,
    SAFETY_STATUS_CRITICAL,
    SAFETY_STATUS_EMERGENCY,
    SAFETY_STATUS_FAULT,
    SAFETY_STATUS_MAX
} safety_status_t;

// ============================================================================
// SAFETY CALLBACK TYPES
// ============================================================================

/**
 * @brief Safety event callback function type
 */
typedef void (*safety_event_callback_t)(safety_event_t event, safety_fault_t fault);

// ============================================================================
// SAFETY CONFIGURATION TYPES
// ============================================================================

/**
 * @brief Safety configuration structure
 */
typedef struct {
    uint8_t estop_pin;
    uint32_t response_time_ms;
    uint32_t debounce_time_ms;
    uint32_t safety_check_interval_ms;
    uint32_t fault_clear_timeout_ms;
    safety_event_callback_t event_callback;
} safety_config_t;

/**
 * @brief Safety status structure
 */
typedef struct {
    bool initialized;
    safety_status_t status;
    safety_level_t level;
    safety_fault_t current_fault;
    safety_event_t last_event;
    uint64_t last_event_time;
    uint32_t fault_count;
    uint32_t event_count;
} safety_status_info_t;

// ============================================================================
// SAFETY UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Get safety event name
 * @param event Safety event
 * @return Event name string
 */
const char* safety_event_get_name(safety_event_t event);

/**
 * @brief Get safety fault name
 * @param fault Safety fault
 * @return Fault name string
 */
const char* safety_fault_get_name(safety_fault_t fault);

/**
 * @brief Get safety level name
 * @param level Safety level
 * @return Level name string
 */
const char* safety_level_get_name(safety_level_t level);

/**
 * @brief Get safety status name
 * @param status Safety status
 * @return Status name string
 */
const char* safety_status_get_name(safety_status_t status);

/**
 * @brief Check if safety event is valid
 * @param event Safety event
 * @return true if valid, false otherwise
 */
bool safety_event_is_valid(safety_event_t event);

/**
 * @brief Check if safety fault is valid
 * @param fault Safety fault
 * @return true if valid, false otherwise
 */
bool safety_fault_is_valid(safety_fault_t fault);

/**
 * @brief Check if safety level is valid
 * @param level Safety level
 * @return true if valid, false otherwise
 */
bool safety_level_is_valid(safety_level_t level);

/**
 * @brief Check if safety status is valid
 * @param status Safety status
 * @return true if valid, false otherwise
 */
bool safety_status_is_valid(safety_status_t status);

#endif // OHT50_SAFETY_TYPES_H
