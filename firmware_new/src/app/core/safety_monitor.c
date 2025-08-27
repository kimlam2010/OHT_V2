/**
 * @file safety_monitor.c
 * @brief Safety Monitor Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (Safety Monitor Implementation)
 */

#include "safety_monitor.h"
#include "hal_common.h"
#include "hal_estop.h"
#include "hal_led.h"
#include "hal_relay.h"
#include "hal_rs485.h"
#include "system_state_machine.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Safety Monitor Instance
static struct {
    safety_monitor_config_t config;
    safety_monitor_status_t status;
    safety_monitor_stats_t stats;
    safety_monitor_event_callback_t event_callback;
    
    // Safety zones
    safety_zone_config_t zones[MAX_SAFETY_ZONES];
    uint8_t zone_count;
    
    // Safety interlocks
    safety_interlock_config_t interlocks[MAX_SAFETY_INTERLOCKS];
    uint8_t interlock_count;
    
    // Safety sensors
    safety_sensor_config_t sensors[MAX_SAFETY_SENSORS];
    uint8_t sensor_count;
    
    // Timing
    uint64_t last_zone_check;
    uint64_t last_interlock_check;
    uint64_t last_sensor_check;
    uint64_t last_watchdog_check;
    uint64_t last_estop_check;
    
    // State
    bool initialized;
    bool estop_hardware_active;
    bool estop_software_active;
    
    // Error handling
    uint32_t error_count;
    uint64_t last_error_time;
    char last_error_message[256];
} safety_monitor_instance = {0};

// Constants
#define MAX_SAFETY_ZONES 8
#define MAX_SAFETY_INTERLOCKS 16
#define MAX_SAFETY_SENSORS 32
#define SAFETY_MONITOR_VERSION "1.0.0"

// Default configuration
static const safety_monitor_config_t default_config = {
    .update_period_ms = 10,           // 10ms update period
    .estop_timeout_ms = 100,          // 100ms E-Stop timeout
    .zone_check_period_ms = 50,       // 50ms zone check
    .interlock_check_period_ms = 20,  // 20ms interlock check
    .sensor_check_period_ms = 100,    // 100ms sensor check
    .watchdog_timeout_ms = 1000,      // 1s watchdog timeout
    .enable_zone_monitoring = true,
    .enable_interlock_monitoring = true,
    .enable_sensor_monitoring = true,
    .enable_watchdog_monitoring = true,
    .enable_emergency_procedures = true,
    .max_retry_count = 3,
    .retry_delay_ms = 10
};

// Internal function prototypes
static hal_status_t safety_monitor_check_estop(void);
static hal_status_t safety_monitor_check_zones(void);
static hal_status_t safety_monitor_check_interlocks(void);
static hal_status_t safety_monitor_check_sensors(void);
static hal_status_t safety_monitor_check_watchdog(void);
static hal_status_t safety_monitor_transition_state(safety_monitor_state_t new_state);
static hal_status_t safety_monitor_handle_estop_event(void);
static hal_status_t safety_monitor_handle_zone_violation(void);
static hal_status_t safety_monitor_handle_interlock_open(void);
static hal_status_t safety_monitor_handle_sensor_fault(void);
static hal_status_t safety_monitor_handle_communication_lost(void);
static hal_status_t safety_monitor_handle_watchdog_timeout(void);
static hal_status_t safety_monitor_execute_emergency_procedures(const char* reason);
static void safety_monitor_log_event(safety_monitor_event_t event, const char* details);
static uint64_t safety_monitor_get_timestamp_ms(void);

// Implementation

hal_status_t safety_monitor_init(const safety_monitor_config_t *config)
{
    hal_status_t status = HAL_STATUS_OK;
    
    // Check if already initialized
    if (safety_monitor_instance.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize instance
    memset(&safety_monitor_instance, 0, sizeof(safety_monitor_instance));
    
    // Set configuration
    if (config != NULL) {
        safety_monitor_instance.config = *config;
    } else {
        safety_monitor_instance.config = default_config;
    }
    
    // Initialize HAL components
    estop_config_t estop_config = {0};
    status = hal_estop_init(&estop_config);
    if (status != HAL_STATUS_OK) {
        safety_monitor_instance.last_error_time = safety_monitor_get_timestamp_ms();
        strncpy(safety_monitor_instance.last_error_message, "E-Stop HAL init failed", sizeof(safety_monitor_instance.last_error_message) - 1);
        return status;
    }
    
    status = hal_led_init();
    if (status != HAL_STATUS_OK) {
        safety_monitor_instance.last_error_time = safety_monitor_get_timestamp_ms();
        strncpy(safety_monitor_instance.last_error_message, "LED HAL init failed", sizeof(safety_monitor_instance.last_error_message) - 1);
        return status;
    }
    
    relay_config_t relay_config = {0};
    status = hal_relay_init(&relay_config);
    if (status != HAL_STATUS_OK) {
        safety_monitor_instance.last_error_time = safety_monitor_get_timestamp_ms();
        strncpy(safety_monitor_instance.last_error_message, "Relay HAL init failed", sizeof(safety_monitor_instance.last_error_message) - 1);
        return status;
    }
    
    // Initialize status
    safety_monitor_instance.status.current_state = SAFETY_MONITOR_STATE_INIT;
    safety_monitor_instance.status.previous_state = SAFETY_MONITOR_STATE_INIT;
    safety_monitor_instance.status.last_event = SAFETY_MONITOR_EVENT_NONE;
    safety_monitor_instance.status.state_entry_time = safety_monitor_get_timestamp_ms();
    safety_monitor_instance.status.last_update_time = safety_monitor_get_timestamp_ms();
    
    // Initialize timing
    safety_monitor_instance.last_zone_check = safety_monitor_get_timestamp_ms();
    safety_monitor_instance.last_interlock_check = safety_monitor_get_timestamp_ms();
    safety_monitor_instance.last_sensor_check = safety_monitor_get_timestamp_ms();
    safety_monitor_instance.last_watchdog_check = safety_monitor_get_timestamp_ms();
    safety_monitor_instance.last_estop_check = safety_monitor_get_timestamp_ms();
    
    // Set initialized flag
    safety_monitor_instance.initialized = true;
    
    // Log initialization
    safety_monitor_log_event(SAFETY_MONITOR_EVENT_NONE, "Safety monitor initialized");
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_deinit(void)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Execute emergency procedures if needed
    if (safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_ESTOP ||
        safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_CRITICAL) {
        safety_monitor_execute_emergency_procedures("Safety monitor deinit");
    }
    
    // Deinitialize HAL components
    hal_estop_deinit();
    hal_led_deinit();
    hal_relay_deinit();
    
    // Clear instance
    memset(&safety_monitor_instance, 0, sizeof(safety_monitor_instance));
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_update(void)
{
    hal_status_t status = HAL_STATUS_OK;
    uint64_t current_time = safety_monitor_get_timestamp_ms();
    
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Update last update time
    safety_monitor_instance.status.last_update_time = current_time;
    
    // Check E-Stop (highest priority)
    if (current_time - safety_monitor_instance.last_estop_check >= safety_monitor_instance.config.estop_timeout_ms) {
        status = safety_monitor_check_estop();
        if (status != HAL_STATUS_OK) {
            safety_monitor_instance.error_count++;
            safety_monitor_instance.last_error_time = current_time;
        }
        safety_monitor_instance.last_estop_check = current_time;
    }
    
    // Check safety zones
    if (safety_monitor_instance.config.enable_zone_monitoring &&
        current_time - safety_monitor_instance.last_zone_check >= safety_monitor_instance.config.zone_check_period_ms) {
        status = safety_monitor_check_zones();
        if (status != HAL_STATUS_OK) {
            safety_monitor_instance.error_count++;
            safety_monitor_instance.last_error_time = current_time;
        }
        safety_monitor_instance.last_zone_check = current_time;
    }
    
    // Check safety interlocks
    if (safety_monitor_instance.config.enable_interlock_monitoring &&
        current_time - safety_monitor_instance.last_interlock_check >= safety_monitor_instance.config.interlock_check_period_ms) {
        status = safety_monitor_check_interlocks();
        if (status != HAL_STATUS_OK) {
            safety_monitor_instance.error_count++;
            safety_monitor_instance.last_error_time = current_time;
        }
        safety_monitor_instance.last_interlock_check = current_time;
    }
    
    // Check safety sensors
    if (safety_monitor_instance.config.enable_sensor_monitoring &&
        current_time - safety_monitor_instance.last_sensor_check >= safety_monitor_instance.config.sensor_check_period_ms) {
        status = safety_monitor_check_sensors();
        if (status != HAL_STATUS_OK) {
            safety_monitor_instance.error_count++;
            safety_monitor_instance.last_error_time = current_time;
        }
        safety_monitor_instance.last_sensor_check = current_time;
    }
    
    // Check watchdog
    if (safety_monitor_instance.config.enable_watchdog_monitoring &&
        current_time - safety_monitor_instance.last_watchdog_check >= safety_monitor_instance.config.watchdog_timeout_ms) {
        status = safety_monitor_check_watchdog();
        if (status != HAL_STATUS_OK) {
            safety_monitor_instance.error_count++;
            safety_monitor_instance.last_error_time = current_time;
        }
        safety_monitor_instance.last_watchdog_check = current_time;
    }
    
    // Update statistics
    safety_monitor_instance.stats.total_uptime_ms = current_time - safety_monitor_instance.status.state_entry_time;
    if (safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_SAFE) {
        safety_monitor_instance.stats.safe_uptime_ms = current_time - safety_monitor_instance.status.last_safe_time;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_process_event(safety_monitor_event_t event, const char* details)
{
    hal_status_t status = HAL_STATUS_OK;
    
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Log event
    safety_monitor_log_event(event, details);
    
    // Update statistics
    safety_monitor_instance.stats.total_events++;
    
    // Process event based on type
    switch (event) {
        case SAFETY_MONITOR_EVENT_ESTOP_TRIGGERED:
            status = safety_monitor_handle_estop_event();
            safety_monitor_instance.stats.estop_events++;
            break;
            
        case SAFETY_MONITOR_EVENT_ZONE_VIOLATION:
            status = safety_monitor_handle_zone_violation();
            safety_monitor_instance.stats.zone_violations++;
            break;
            
        case SAFETY_MONITOR_EVENT_INTERLOCK_OPEN:
            status = safety_monitor_handle_interlock_open();
            safety_monitor_instance.stats.interlock_opens++;
            break;
            
        case SAFETY_MONITOR_EVENT_SENSOR_FAULT:
            status = safety_monitor_handle_sensor_fault();
            safety_monitor_instance.stats.sensor_faults++;
            break;
            
        case SAFETY_MONITOR_EVENT_COMMUNICATION_LOST:
            status = safety_monitor_handle_communication_lost();
            safety_monitor_instance.stats.communication_failures++;
            break;
            
        case SAFETY_MONITOR_EVENT_WATCHDOG_TIMEOUT:
            status = safety_monitor_handle_watchdog_timeout();
            safety_monitor_instance.stats.watchdog_timeouts++;
            break;
            
        case SAFETY_MONITOR_EVENT_EMERGENCY_STOP:
            status = safety_monitor_trigger_emergency_stop(details);
            break;
            
        case SAFETY_MONITOR_EVENT_SAFETY_RESET:
            status = safety_monitor_reset();
            break;
            
        default:
            // Unknown event - log but don't change state
            break;
    }
    
    // Update last event
    safety_monitor_instance.status.last_event = event;
    
    return status;
}

hal_status_t safety_monitor_get_status(safety_monitor_status_t *status)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *status = safety_monitor_instance.status;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_get_stats(safety_monitor_stats_t *stats)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *stats = safety_monitor_instance.stats;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_is_safe(bool *safe)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (safe == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *safe = (safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_SAFE);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_trigger_emergency_stop(const char* reason)
{
    hal_status_t status = HAL_STATUS_OK;
    
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Execute emergency procedures
    status = safety_monitor_execute_emergency_procedures(reason);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Transition to E-Stop state
    status = safety_monitor_transition_state(SAFETY_MONITOR_STATE_ESTOP);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Set software E-Stop flag
    safety_monitor_instance.estop_software_active = true;
    
    // Log event
    safety_monitor_log_event(SAFETY_MONITOR_EVENT_EMERGENCY_STOP, reason);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_reset(void)
{
    hal_status_t status = HAL_STATUS_OK;
    
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check if reset is allowed
    if (safety_monitor_instance.estop_hardware_active) {
        return HAL_STATUS_ERROR;
    }
    
    // Clear software E-Stop
    safety_monitor_instance.estop_software_active = false;
    
    // Reset safety flags
    safety_monitor_instance.status.zone_violation = false;
    safety_monitor_instance.status.interlock_open = false;
    safety_monitor_instance.status.sensor_fault = false;
    safety_monitor_instance.status.communication_ok = true;
    safety_monitor_instance.status.watchdog_ok = true;
    
    // Transition to safe state
    status = safety_monitor_transition_state(SAFETY_MONITOR_STATE_SAFE);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Update last safe time
    safety_monitor_instance.status.last_safe_time = safety_monitor_get_timestamp_ms();
    
    // Increment recovery count
    safety_monitor_instance.stats.recovery_count++;
    
    // Log event
    safety_monitor_log_event(SAFETY_MONITOR_EVENT_SAFETY_RESET, "Safety system reset");
    
    return HAL_STATUS_OK;
}

// Internal functions

static hal_status_t safety_monitor_check_estop(void)
{
    hal_status_t status = HAL_STATUS_OK;
    bool estop_active = false;
    
    // Check hardware E-Stop
    estop_status_t estop_status;
    status = hal_estop_get_status(&estop_status);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Update hardware E-Stop state
    safety_monitor_instance.estop_hardware_active = (estop_status.state == ESTOP_STATE_TRIGGERED);
    
    // Check for E-Stop activation
    if (estop_active && !safety_monitor_instance.estop_software_active) {
        safety_monitor_process_event(SAFETY_MONITOR_EVENT_ESTOP_TRIGGERED, "Hardware E-Stop activated");
    }
    
    // Check for E-Stop reset
    if (!estop_active && safety_monitor_instance.estop_hardware_active) {
        safety_monitor_process_event(SAFETY_MONITOR_EVENT_ESTOP_RESET, "Hardware E-Stop reset");
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_check_zones(void)
{
    // TODO: Implement zone checking logic
    // This would involve checking LiDAR data, proximity sensors, etc.
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_check_interlocks(void)
{
    // TODO: Implement interlock checking logic
    // This would involve checking door sensors, light curtains, etc.
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_check_sensors(void)
{
    // TODO: Implement sensor checking logic
    // This would involve checking all safety sensors
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_check_watchdog(void)
{
    // TODO: Implement watchdog checking logic
    // This would involve checking system watchdog status
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_transition_state(safety_monitor_state_t new_state)
{
    safety_monitor_state_t old_state = safety_monitor_instance.status.current_state;
    
    // Validate state transition
    switch (old_state) {
        case SAFETY_MONITOR_STATE_INIT:
            if (new_state != SAFETY_MONITOR_STATE_SAFE && 
                new_state != SAFETY_MONITOR_STATE_FAULT) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SAFETY_MONITOR_STATE_SAFE:
            // Can transition to any state
            break;
            
        case SAFETY_MONITOR_STATE_WARNING:
            if (new_state == SAFETY_MONITOR_STATE_INIT) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SAFETY_MONITOR_STATE_CRITICAL:
            if (new_state == SAFETY_MONITOR_STATE_INIT || 
                new_state == SAFETY_MONITOR_STATE_SAFE) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SAFETY_MONITOR_STATE_ESTOP:
            if (new_state == SAFETY_MONITOR_STATE_INIT || 
                new_state == SAFETY_MONITOR_STATE_SAFE) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SAFETY_MONITOR_STATE_FAULT:
            if (new_state == SAFETY_MONITOR_STATE_INIT) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        default:
            return HAL_STATUS_INVALID_STATE;
    }
    
    // Update state
    safety_monitor_instance.status.previous_state = old_state;
    safety_monitor_instance.status.current_state = new_state;
    safety_monitor_instance.status.state_entry_time = safety_monitor_get_timestamp_ms();
    safety_monitor_instance.status.state_transition_count++;
    
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_handle_estop_event(void)
{
    return safety_monitor_transition_state(SAFETY_MONITOR_STATE_ESTOP);
}

static hal_status_t safety_monitor_handle_zone_violation(void)
{
    safety_monitor_instance.status.zone_violation = true;
    safety_monitor_instance.status.violation_count++;
    
    if (safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_SAFE) {
        return safety_monitor_transition_state(SAFETY_MONITOR_STATE_WARNING);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_handle_interlock_open(void)
{
    safety_monitor_instance.status.interlock_open = true;
    
    if (safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_SAFE ||
        safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_WARNING) {
        return safety_monitor_transition_state(SAFETY_MONITOR_STATE_CRITICAL);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_handle_sensor_fault(void)
{
    safety_monitor_instance.status.sensor_fault = true;
    safety_monitor_instance.status.fault_count++;
    
    if (safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_SAFE ||
        safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_WARNING) {
        return safety_monitor_transition_state(SAFETY_MONITOR_STATE_CRITICAL);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_handle_communication_lost(void)
{
    safety_monitor_instance.status.communication_ok = false;
    
    if (safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_SAFE ||
        safety_monitor_instance.status.current_state == SAFETY_MONITOR_STATE_WARNING) {
        return safety_monitor_transition_state(SAFETY_MONITOR_STATE_CRITICAL);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t safety_monitor_handle_watchdog_timeout(void)
{
    safety_monitor_instance.status.watchdog_ok = false;
    
    return safety_monitor_transition_state(SAFETY_MONITOR_STATE_FAULT);
}

static hal_status_t safety_monitor_execute_emergency_procedures(const char* reason)
{
    hal_status_t status = HAL_STATUS_OK;
    
    if (!safety_monitor_instance.config.enable_emergency_procedures) {
        return HAL_STATUS_OK;
    }
    
    // Set emergency LED
    status = hal_led_on(LED_ERROR_PIN);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Set error LED
    status = hal_led_on(LED_ERROR_PIN);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Turn off all relays (fail-safe)
    status = hal_relay1_off();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = hal_relay2_off();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Log emergency procedures
    safety_monitor_log_event(SAFETY_MONITOR_EVENT_EMERGENCY_STOP, reason);
    
    return HAL_STATUS_OK;
}

static void safety_monitor_log_event(safety_monitor_event_t event, const char* details)
{
    // TODO: Implement proper logging
    // This would involve writing to system log, sending to telemetry, etc.
    (void)event;
    (void)details;
}

static uint64_t safety_monitor_get_timestamp_ms(void)
{
    // TODO: Implement proper timestamp function
    // This should return system uptime in milliseconds
    return (uint64_t)time(NULL) * 1000;
}

// Additional public functions

hal_status_t safety_monitor_set_zone_config(uint8_t zone_id, const safety_zone_config_t *config)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (zone_id >= MAX_SAFETY_ZONES || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    safety_monitor_instance.zones[zone_id] = *config;
    
    if (zone_id >= safety_monitor_instance.zone_count) {
        safety_monitor_instance.zone_count = zone_id + 1;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_set_interlock_config(uint8_t interlock_id, const safety_interlock_config_t *config)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (interlock_id >= MAX_SAFETY_INTERLOCKS || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    safety_monitor_instance.interlocks[interlock_id] = *config;
    
    if (interlock_id >= safety_monitor_instance.interlock_count) {
        safety_monitor_instance.interlock_count = interlock_id + 1;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_set_sensor_config(uint8_t sensor_id, const safety_sensor_config_t *config)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (sensor_id >= MAX_SAFETY_SENSORS || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    safety_monitor_instance.sensors[sensor_id] = *config;
    
    if (sensor_id >= safety_monitor_instance.sensor_count) {
        safety_monitor_instance.sensor_count = sensor_id + 1;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_set_callback(safety_monitor_event_callback_t callback)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    safety_monitor_instance.event_callback = callback;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_set_config(const safety_monitor_config_t *config)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    safety_monitor_instance.config = *config;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_get_config(safety_monitor_config_t *config)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *config = safety_monitor_instance.config;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_run_diagnostics(char* result, size_t max_size)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (result == NULL || max_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(result, max_size,
             "Safety Monitor Diagnostics:\n"
             "State: %d\n"
             "E-Stop Active: %s\n"
             "Zone Violation: %s\n"
             "Interlock Open: %s\n"
             "Sensor Fault: %s\n"
             "Communication OK: %s\n"
             "Watchdog OK: %s\n"
             "Error Count: %u\n"
             "Violation Count: %u\n"
             "Fault Count: %u\n",
             safety_monitor_instance.status.current_state,
             safety_monitor_instance.status.estop_active ? "Yes" : "No",
             safety_monitor_instance.status.zone_violation ? "Yes" : "No",
             safety_monitor_instance.status.interlock_open ? "Yes" : "No",
             safety_monitor_instance.status.sensor_fault ? "Yes" : "No",
             safety_monitor_instance.status.communication_ok ? "Yes" : "No",
             safety_monitor_instance.status.watchdog_ok ? "Yes" : "No",
             safety_monitor_instance.error_count,
             safety_monitor_instance.status.violation_count,
             safety_monitor_instance.status.fault_count);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_monitor_clear_stats(void)
{
    if (!safety_monitor_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&safety_monitor_instance.stats, 0, sizeof(safety_monitor_instance.stats));
    
    return HAL_STATUS_OK;
}

const char* safety_monitor_get_version(void)
{
    return SAFETY_MONITOR_VERSION;
}
