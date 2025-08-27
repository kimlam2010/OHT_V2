/**
 * @file safety_monitor.h
 * @brief Safety Monitor for OHT-50 Master Module - Safety-First Approach
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (Safety Monitor Implementation)
 */

#ifndef SAFETY_MONITOR_H
#define SAFETY_MONITOR_H

// Safety Monitor Constants
#define MAX_SAFETY_ZONES 8
#define MAX_SAFETY_INTERLOCKS 16
#define MAX_SAFETY_SENSORS 32

#include "hal_common.h"
#include "hal_estop.h"
#include "hal_led.h"
#include "hal_relay.h"
#include "hal_rs485.h"
#include "system_state_machine.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Safety Monitor States
typedef enum {
    SAFETY_MONITOR_STATE_INIT = 0,        // Initialization state
    SAFETY_MONITOR_STATE_SAFE,            // System is safe
    SAFETY_MONITOR_STATE_WARNING,         // Safety warning detected
    SAFETY_MONITOR_STATE_CRITICAL,        // Critical safety issue
    SAFETY_MONITOR_STATE_ESTOP,           // Emergency stop active
    SAFETY_MONITOR_STATE_FAULT            // Safety fault detected
} safety_monitor_state_t;

// Safety Monitor Events
typedef enum {
    SAFETY_MONITOR_EVENT_NONE = 0,
    SAFETY_MONITOR_EVENT_ESTOP_TRIGGERED, // E-Stop button pressed
    SAFETY_MONITOR_EVENT_ESTOP_RESET,     // E-Stop reset
    SAFETY_MONITOR_EVENT_ZONE_VIOLATION,  // Safety zone violation
    SAFETY_MONITOR_EVENT_INTERLOCK_OPEN,  // Safety interlock opened
    SAFETY_MONITOR_EVENT_SENSOR_FAULT,    // Safety sensor fault
    SAFETY_MONITOR_EVENT_COMMUNICATION_LOST, // Safety communication lost
    SAFETY_MONITOR_EVENT_WATCHDOG_TIMEOUT,   // Watchdog timeout
    SAFETY_MONITOR_EVENT_EMERGENCY_STOP,     // Emergency stop command
    SAFETY_MONITOR_EVENT_SAFETY_RESET        // Safety system reset
} safety_monitor_event_t;

// Safety Zone Types
typedef enum {
    SAFETY_ZONE_NONE = 0,
    SAFETY_ZONE_OPERATIONAL,      // Normal operational zone
    SAFETY_ZONE_RESTRICTED,       // Restricted access zone
    SAFETY_ZONE_DANGER,           // Danger zone
    SAFETY_ZONE_EMERGENCY         // Emergency stop zone
} safety_zone_t;

// Safety Interlock Types
typedef enum {
    SAFETY_INTERLOCK_NONE = 0,
    SAFETY_INTERLOCK_DOOR,        // Door interlock
    SAFETY_INTERLOCK_GATE,        // Gate interlock
    SAFETY_INTERLOCK_LIGHT_CURTAIN, // Light curtain interlock
    SAFETY_INTERLOCK_EMERGENCY_STOP, // Emergency stop interlock
    SAFETY_INTERLOCK_SENSOR       // Safety sensor interlock
} safety_interlock_t;

// Safety Sensor Types
typedef enum {
    SAFETY_SENSOR_NONE = 0,
    SAFETY_SENSOR_PROXIMITY,      // Proximity sensor
    SAFETY_SENSOR_LIDAR,          // LiDAR sensor
    SAFETY_SENSOR_CAMERA,         // Camera sensor
    SAFETY_SENSOR_PRESSURE,       // Pressure sensor
    SAFETY_SENSOR_TEMPERATURE     // Temperature sensor
} safety_sensor_t;

// Safety Monitor Configuration
typedef struct {
    uint32_t update_period_ms;            // Safety monitor update period
    uint32_t estop_timeout_ms;            // E-Stop response timeout
    uint32_t zone_check_period_ms;        // Safety zone check period
    uint32_t interlock_check_period_ms;   // Interlock check period
    uint32_t sensor_check_period_ms;      // Sensor check period
    uint32_t watchdog_timeout_ms;         // Watchdog timeout
    bool enable_zone_monitoring;          // Enable zone monitoring
    bool enable_interlock_monitoring;     // Enable interlock monitoring
    bool enable_sensor_monitoring;        // Enable sensor monitoring
    bool enable_watchdog_monitoring;      // Enable watchdog monitoring
    bool enable_emergency_procedures;     // Enable emergency procedures
    uint8_t max_retry_count;              // Maximum retry count
    uint32_t retry_delay_ms;              // Retry delay
} safety_monitor_config_t;

// Safety Zone Configuration
typedef struct {
    safety_zone_t zone_type;              // Zone type
    float min_distance_m;                 // Minimum safe distance
    float max_distance_m;                 // Maximum safe distance
    float angle_start_deg;                // Start angle in degrees
    float angle_end_deg;                  // End angle in degrees
    bool enabled;                         // Zone enabled
    uint32_t violation_timeout_ms;        // Violation timeout
} safety_zone_config_t;

// Safety Interlock Configuration
typedef struct {
    safety_interlock_t interlock_type;    // Interlock type
    uint8_t interlock_id;                 // Interlock ID
    bool normally_closed;                 // Normally closed state
    bool enabled;                         // Interlock enabled
    uint32_t timeout_ms;                  // Interlock timeout
} safety_interlock_config_t;

// Safety Sensor Configuration
typedef struct {
    safety_sensor_t sensor_type;          // Sensor type
    uint8_t sensor_id;                    // Sensor ID
    float threshold_value;                // Threshold value
    bool enabled;                         // Sensor enabled
    uint32_t timeout_ms;                  // Sensor timeout
    uint32_t check_period_ms;             // Check period
} safety_sensor_config_t;

// Safety Monitor Status
typedef struct {
    safety_monitor_state_t current_state; // Current safety state
    safety_monitor_state_t previous_state; // Previous safety state
    safety_monitor_event_t last_event;    // Last safety event
    uint64_t state_entry_time;            // State entry time
    uint64_t last_update_time;            // Last update time
    bool estop_active;                    // E-Stop active
    bool zone_violation;                  // Zone violation detected
    bool interlock_open;                  // Interlock open
    bool sensor_fault;                    // Sensor fault detected
    bool communication_ok;                // Communication OK
    bool watchdog_ok;                     // Watchdog OK
    uint32_t violation_count;             // Violation count
    uint32_t fault_count;                 // Fault count
    uint64_t last_safe_time;              // Last safe time
    uint32_t state_transition_count;      // State transition count
} safety_monitor_status_t;

// Safety Monitor Statistics
typedef struct {
    uint32_t total_events;                // Total events
    uint32_t estop_events;                // E-Stop events
    uint32_t zone_violations;             // Zone violations
    uint32_t interlock_opens;             // Interlock opens
    uint32_t sensor_faults;               // Sensor faults
    uint32_t communication_failures;      // Communication failures
    uint32_t watchdog_timeouts;           // Watchdog timeouts
    uint64_t total_uptime_ms;             // Total uptime
    uint64_t safe_uptime_ms;              // Safe uptime
    uint32_t recovery_count;              // Recovery count
} safety_monitor_stats_t;

// Safety Event Callback
typedef void (*safety_monitor_event_callback_t)(safety_monitor_state_t state, 
                                               safety_monitor_event_t event, 
                                               const char* details);

// Function Prototypes

/**
 * @brief Initialize safety monitor
 * @param config Safety monitor configuration
 * @return HAL status
 */
hal_status_t safety_monitor_init(const safety_monitor_config_t *config);

/**
 * @brief Deinitialize safety monitor
 * @return HAL status
 */
hal_status_t safety_monitor_deinit(void);

/**
 * @brief Update safety monitor (call periodically)
 * @return HAL status
 */
hal_status_t safety_monitor_update(void);

/**
 * @brief Process safety event
 * @param event Safety event to process
 * @param details Event details
 * @return HAL status
 */
hal_status_t safety_monitor_process_event(safety_monitor_event_t event, const char* details);

/**
 * @brief Get safety monitor status
 * @param status Pointer to store status
 * @return HAL status
 */
hal_status_t safety_monitor_get_status(safety_monitor_status_t *status);

/**
 * @brief Get safety monitor statistics
 * @param stats Pointer to store statistics
 * @return HAL status
 */
hal_status_t safety_monitor_get_stats(safety_monitor_stats_t *stats);

/**
 * @brief Check if system is safe
 * @param safe Pointer to store safety status
 * @return HAL status
 */
hal_status_t safety_monitor_is_safe(bool *safe);

/**
 * @brief Trigger emergency stop
 * @param reason Emergency stop reason
 * @return HAL status
 */
hal_status_t safety_monitor_trigger_emergency_stop(const char* reason);

/**
 * @brief Reset safety system
 * @return HAL status
 */
hal_status_t safety_monitor_reset(void);

/**
 * @brief Set safety zone configuration
 * @param zone_id Zone ID
 * @param config Zone configuration
 * @return HAL status
 */
hal_status_t safety_monitor_set_zone_config(uint8_t zone_id, const safety_zone_config_t *config);

/**
 * @brief Set safety interlock configuration
 * @param interlock_id Interlock ID
 * @param config Interlock configuration
 * @return HAL status
 */
hal_status_t safety_monitor_set_interlock_config(uint8_t interlock_id, const safety_interlock_config_t *config);

/**
 * @brief Set safety sensor configuration
 * @param sensor_id Sensor ID
 * @param config Sensor configuration
 * @return HAL status
 */
hal_status_t safety_monitor_set_sensor_config(uint8_t sensor_id, const safety_sensor_config_t *config);

/**
 * @brief Set safety event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t safety_monitor_set_callback(safety_monitor_event_callback_t callback);

/**
 * @brief Set safety monitor configuration
 * @param config Safety monitor configuration
 * @return HAL status
 */
hal_status_t safety_monitor_set_config(const safety_monitor_config_t *config);

/**
 * @brief Get safety monitor configuration
 * @param config Pointer to store configuration
 * @return HAL status
 */
hal_status_t safety_monitor_get_config(safety_monitor_config_t *config);

/**
 * @brief Run safety diagnostics
 * @param result Diagnostic result
 * @return HAL status
 */
hal_status_t safety_monitor_run_diagnostics(char* result, size_t max_size);

/**
 * @brief Clear safety statistics
 * @return HAL status
 */
hal_status_t safety_monitor_clear_stats(void);

/**
 * @brief Get safety monitor version
 * @return Version string
 */
const char* safety_monitor_get_version(void);

#ifdef __cplusplus
}
#endif

#endif // SAFETY_MONITOR_H
