/**
 * @file safety_manager.c
 * @brief Safety Manager Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-02 (Safety Integration)
 */

#include "safety_manager.h"
#include <string.h>
#include <stdio.h>

// Internal safety manager structure
typedef struct {
    safety_config_t config;
    safety_status_t status;
    safety_event_callback_t event_callback;
    bool initialized;
    uint64_t last_update_time;
    uint64_t start_time;
} safety_manager_t;

// Global safety manager instance
static safety_manager_t g_safety_manager = {0};

// Forward declarations
static hal_status_t update_safety_status(void);
static hal_status_t check_safety_conditions(void);
static hal_status_t handle_safety_event(safety_event_t event);
static void estop_callback(estop_state_t state, estop_fault_t fault);

// Safety manager implementation
hal_status_t safety_manager_init(const safety_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize safety manager structure
    memset(&g_safety_manager, 0, sizeof(safety_manager_t));
    
    // Set configuration
    memcpy(&g_safety_manager.config, config, sizeof(safety_config_t));
    
    // Initialize status
    g_safety_manager.status.current_level = SAFETY_LEVEL_NORMAL;
    g_safety_manager.status.last_event = SAFETY_EVENT_NONE;
    g_safety_manager.status.current_fault = SAFETY_FAULT_NONE;
    g_safety_manager.status.estop_triggered = false;
    g_safety_manager.status.interlock_triggered = false;
    g_safety_manager.status.safety_circuit_ok = true;
    g_safety_manager.status.sensors_ok = true;
    g_safety_manager.status.communication_ok = true;
    g_safety_manager.status.power_ok = true;
    g_safety_manager.status.last_safety_check = hal_get_timestamp_us();
    g_safety_manager.status.fault_count = 0;
    g_safety_manager.status.estop_count = 0;
    g_safety_manager.status.interlock_count = 0;
    g_safety_manager.status.uptime_seconds = 0;
    
    // Set start time
    g_safety_manager.start_time = hal_get_timestamp_us();
    g_safety_manager.last_update_time = g_safety_manager.start_time;
    g_safety_manager.initialized = true;
    
    // Set E-Stop callback
    hal_estop_set_callback(estop_callback);
    
    // Set initial LED pattern for safety
    hal_led_error_set(LED_STATE_OFF);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_deinit(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Clear E-Stop callback
    hal_estop_set_callback(NULL);
    
    // Clear safety manager
    memset(&g_safety_manager, 0, sizeof(safety_manager_t));
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_update(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = hal_get_timestamp_us();
    g_safety_manager.last_update_time = current_time;
    
    // Update uptime
    g_safety_manager.status.uptime_seconds = (current_time - g_safety_manager.start_time) / 1000000ULL;
    
    // Check if it's time for safety check
    uint64_t time_since_last_check = (current_time - g_safety_manager.status.last_safety_check) / 1000ULL;
    if (time_since_last_check >= g_safety_manager.config.safety_check_interval_ms) {
        update_safety_status();
        check_safety_conditions();
        g_safety_manager.status.last_safety_check = current_time;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_process_event(safety_event_t event) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (event == SAFETY_EVENT_NONE) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return handle_safety_event(event);
}

hal_status_t safety_manager_get_status(safety_status_t *status) {
    if (!g_safety_manager.initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(status, &g_safety_manager.status, sizeof(safety_status_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_is_safe(bool *safe) {
    if (!g_safety_manager.initialized || safe == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *safe = (g_safety_manager.status.current_level == SAFETY_LEVEL_NORMAL) &&
            !g_safety_manager.status.estop_triggered &&
            (g_safety_manager.status.current_fault == SAFETY_FAULT_NONE);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_trigger_fault(safety_fault_t fault) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.current_fault = fault;
    g_safety_manager.status.fault_count++;
    
    return safety_manager_process_event(SAFETY_EVENT_SAFETY_FAULT);
}

hal_status_t safety_manager_clear_fault(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.current_fault = SAFETY_FAULT_NONE;
    
    return safety_manager_process_event(SAFETY_EVENT_SAFETY_CLEARED);
}

hal_status_t safety_manager_get_fault(safety_fault_t *fault) {
    if (!g_safety_manager.initialized || fault == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *fault = g_safety_manager.status.current_fault;
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_set_callback(safety_event_callback_t callback) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_set_config(const safety_config_t *config) {
    if (!g_safety_manager.initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_safety_manager.config, config, sizeof(safety_config_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_get_config(safety_config_t *config) {
    if (!g_safety_manager.initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_safety_manager.config, sizeof(safety_config_t));
    return HAL_STATUS_OK;
}

// E-Stop Integration Functions
hal_status_t safety_manager_handle_estop_trigger(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.estop_triggered = true;
    g_safety_manager.status.estop_count++;
    g_safety_manager.status.current_level = SAFETY_LEVEL_EMERGENCY;
    
    // Set error LED
    hal_led_error_set(LED_STATE_ON);
    
    // Trigger emergency stop
    safety_manager_emergency_stop();
    
    return safety_manager_process_event(SAFETY_EVENT_ESTOP_TRIGGERED);
}

hal_status_t safety_manager_handle_estop_reset(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.estop_triggered = false;
    g_safety_manager.status.last_event = SAFETY_EVENT_ESTOP_RESET;
    
    // Clear emergency stop
    handle_safety_event(SAFETY_EVENT_SAFETY_CLEARED);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_trigger_interlock(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.interlock_triggered = true;
    g_safety_manager.status.interlock_count++;
    g_safety_manager.status.last_event = SAFETY_EVENT_INTERLOCK_TRIGGERED;
    
    // Trigger safety fault
    handle_safety_event(SAFETY_EVENT_SAFETY_FAULT);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_reset_interlock(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.interlock_triggered = false;
    g_safety_manager.status.last_event = SAFETY_EVENT_INTERLOCK_RELEASED;
    
    // Clear safety fault
    handle_safety_event(SAFETY_EVENT_SAFETY_CLEARED);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_check_estop(bool *triggered) {
    if (!g_safety_manager.initialized || triggered == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *triggered = g_safety_manager.status.estop_triggered;
    return HAL_STATUS_OK;
}

// Safety Circuit Functions
hal_status_t safety_manager_check_safety_circuit(bool *ok) {
    if (!g_safety_manager.initialized || ok == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // TODO: Implement actual safety circuit check
    // For now, assume safety circuit is OK
    g_safety_manager.status.safety_circuit_ok = true;
    *ok = g_safety_manager.status.safety_circuit_ok;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_validate_safety_circuit(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    bool circuit_ok;
    hal_status_t status = safety_manager_check_safety_circuit(&circuit_ok);
    
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (!circuit_ok) {
        return safety_manager_trigger_fault(SAFETY_FAULT_SAFETY_CIRCUIT);
    }
    
    return HAL_STATUS_OK;
}

// Sensor Monitoring Functions
hal_status_t safety_manager_check_sensors(bool *ok) {
    if (!g_safety_manager.initialized || ok == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // TODO: Implement actual sensor check
    // For now, assume sensors are OK
    g_safety_manager.status.sensors_ok = true;
    *ok = g_safety_manager.status.sensors_ok;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_validate_sensors(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    bool sensors_ok;
    hal_status_t status = safety_manager_check_sensors(&sensors_ok);
    
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (!sensors_ok) {
        return safety_manager_trigger_fault(SAFETY_FAULT_SENSOR_FAILURE);
    }
    
    return HAL_STATUS_OK;
}

// Emergency Procedures
hal_status_t safety_manager_emergency_stop(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Set emergency LED pattern
    hal_led_error_set(LED_STATE_BLINK_FAST);
    hal_led_system_set(LED_STATE_OFF);
    hal_led_comm_set(LED_STATE_OFF);
    hal_led_network_set(LED_STATE_OFF);
    
    // TODO: Implement actual emergency stop procedures
    // - Stop all motors
    // - Disable power outputs
    // - Activate brakes
    // - Send emergency stop signal to all modules
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_safety_shutdown(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Set shutdown LED pattern
    hal_led_error_set(LED_STATE_ON);
    hal_led_system_set(LED_STATE_BLINK_SLOW);
    hal_led_comm_set(LED_STATE_OFF);
    hal_led_network_set(LED_STATE_OFF);
    
    // TODO: Implement actual safety shutdown procedures
    // - Graceful shutdown of all systems
    // - Save critical data
    // - Disable all outputs
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_safety_recovery(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Clear all safety faults
    g_safety_manager.status.current_fault = SAFETY_FAULT_NONE;
    g_safety_manager.status.current_level = SAFETY_LEVEL_NORMAL;
    
    // Reset LED patterns
    hal_led_error_set(LED_STATE_OFF);
    hal_led_system_set(LED_STATE_ON);
    hal_led_comm_set(LED_STATE_BLINK_SLOW);
    hal_led_network_set(LED_STATE_ON);
    
    // TODO: Implement actual safety recovery procedures
    // - Re-enable systems
    // - Perform self-tests
    // - Restore normal operation
    
    return HAL_STATUS_OK;
}

// Utility Functions
const char* safety_manager_get_level_name(safety_level_t level) {
    switch (level) {
        case SAFETY_LEVEL_NORMAL: return "NORMAL";
        case SAFETY_LEVEL_WARNING: return "WARNING";
        case SAFETY_LEVEL_CRITICAL: return "CRITICAL";
        case SAFETY_LEVEL_EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

const char* safety_manager_get_event_name(safety_event_t event) {
    switch (event) {
        case SAFETY_EVENT_NONE: return "NONE";
        case SAFETY_EVENT_ESTOP_TRIGGERED: return "ESTOP_TRIGGERED";
        case SAFETY_EVENT_ESTOP_RESET: return "ESTOP_RESET";
        case SAFETY_EVENT_SAFETY_FAULT: return "SAFETY_FAULT";
        case SAFETY_EVENT_SAFETY_CLEARED: return "SAFETY_CLEARED";
        case SAFETY_EVENT_EMERGENCY_STOP: return "EMERGENCY_STOP";
        case SAFETY_EVENT_SAFETY_TIMEOUT: return "SAFETY_TIMEOUT";
        case SAFETY_EVENT_SYSTEM_FAULT: return "SYSTEM_FAULT";
        case SAFETY_EVENT_INTERLOCK_TRIGGERED: return "INTERLOCK_TRIGGERED";
        case SAFETY_EVENT_INTERLOCK_RELEASED: return "INTERLOCK_RELEASED";
        default: return "UNKNOWN";
    }
}

const char* safety_manager_get_fault_name(safety_fault_t fault) {
    switch (fault) {
        case SAFETY_FAULT_NONE: return "NONE";
        case SAFETY_FAULT_ESTOP_HARDWARE: return "ESTOP_HARDWARE";
        case SAFETY_FAULT_ESTOP_SOFTWARE: return "ESTOP_SOFTWARE";
        case SAFETY_FAULT_SAFETY_CIRCUIT: return "SAFETY_CIRCUIT";
        case SAFETY_FAULT_SENSOR_FAILURE: return "SENSOR_FAILURE";
        case SAFETY_FAULT_COMMUNICATION: return "COMMUNICATION";
        case SAFETY_FAULT_POWER_FAILURE: return "POWER_FAILURE";
        case SAFETY_FAULT_OVERTEMPERATURE: return "OVERTEMPERATURE";
        case SAFETY_FAULT_OVERCURRENT: return "OVERCURRENT";
        case SAFETY_FAULT_MECHANICAL_FAULT: return "MECHANICAL_FAULT";
        default: return "UNKNOWN";
    }
}

hal_status_t safety_manager_get_diagnostics(char *info, size_t max_len) {
    if (!g_safety_manager.initialized || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(info, max_len,
        "Safety Manager Diagnostics:\n"
        "Current Level: %s\n"
        "Last Event: %s\n"
        "Current Fault: %s\n"
        "E-Stop Triggered: %s\n"
        "Safety Circuit OK: %s\n"
        "Sensors OK: %s\n"
        "Communication OK: %s\n"
        "Power OK: %s\n"
        "Last Safety Check: %lu ms ago\n"
        "Fault Count: %u\n"
        "E-Stop Count: %u\n"
        "Uptime: %lu seconds\n",
        safety_manager_get_level_name(g_safety_manager.status.current_level),
        safety_manager_get_event_name(g_safety_manager.status.last_event),
        safety_manager_get_fault_name(g_safety_manager.status.current_fault),
        g_safety_manager.status.estop_triggered ? "YES" : "NO",
        g_safety_manager.status.safety_circuit_ok ? "YES" : "NO",
        g_safety_manager.status.sensors_ok ? "YES" : "NO",
        g_safety_manager.status.communication_ok ? "YES" : "NO",
        g_safety_manager.status.power_ok ? "YES" : "NO",
        (unsigned long)((hal_get_timestamp_us() - g_safety_manager.status.last_safety_check) / 1000),
        g_safety_manager.status.fault_count,
        g_safety_manager.status.estop_count,
        (unsigned long)g_safety_manager.status.uptime_seconds
    );
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_self_test(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Perform safety self-test
    hal_status_t status = HAL_STATUS_OK;
    
    // Test E-Stop
    bool estop_ok;
    hal_estop_is_triggered(&estop_ok);
    if (estop_ok) {
        status = HAL_STATUS_ERROR;
    }
    
    // Test safety circuit
    bool circuit_ok;
    safety_manager_check_safety_circuit(&circuit_ok);
    if (!circuit_ok) {
        status = HAL_STATUS_ERROR;
    }
    
    // Test sensors
    bool sensors_ok;
    safety_manager_check_sensors(&sensors_ok);
    if (!sensors_ok) {
        status = HAL_STATUS_ERROR;
    }
    
    return status;
}

hal_status_t safety_manager_validate_compliance(bool *compliant) {
    if (!g_safety_manager.initialized || compliant == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check safety compliance
    *compliant = (g_safety_manager.status.current_level == SAFETY_LEVEL_NORMAL) &&
                 !g_safety_manager.status.estop_triggered &&
                 (g_safety_manager.status.current_fault == SAFETY_FAULT_NONE) &&
                 g_safety_manager.status.safety_circuit_ok &&
                 g_safety_manager.status.sensors_ok;
    
    return HAL_STATUS_OK;
}

// Internal helper functions
static hal_status_t update_safety_status(void) {
    // Update E-Stop status
    bool estop_triggered;
    hal_estop_is_triggered(&estop_triggered);
    g_safety_manager.status.estop_triggered = estop_triggered;
    
    // Update safety circuit status
    bool circuit_ok;
    safety_manager_check_safety_circuit(&circuit_ok);
    g_safety_manager.status.safety_circuit_ok = circuit_ok;
    
    // Update sensor status
    bool sensors_ok;
    safety_manager_check_sensors(&sensors_ok);
    g_safety_manager.status.sensors_ok = sensors_ok;
    
    return HAL_STATUS_OK;
}

static hal_status_t check_safety_conditions(void) {
    // Check for safety violations
    if (g_safety_manager.status.estop_triggered) {
        g_safety_manager.status.current_level = SAFETY_LEVEL_EMERGENCY;
        return safety_manager_process_event(SAFETY_EVENT_ESTOP_TRIGGERED);
    }
    
    if (g_safety_manager.status.current_fault != SAFETY_FAULT_NONE) {
        g_safety_manager.status.current_level = SAFETY_LEVEL_CRITICAL;
        return safety_manager_process_event(SAFETY_EVENT_SAFETY_FAULT);
    }
    
    if (!g_safety_manager.status.safety_circuit_ok || 
        !g_safety_manager.status.sensors_ok) {
        g_safety_manager.status.current_level = SAFETY_LEVEL_WARNING;
        return safety_manager_process_event(SAFETY_EVENT_SAFETY_FAULT);
    }
    
    // All conditions OK
    g_safety_manager.status.current_level = SAFETY_LEVEL_NORMAL;
    return HAL_STATUS_OK;
}

static hal_status_t handle_safety_event(safety_event_t event) {
    // Store last event
    g_safety_manager.status.last_event = event;
    
    // Handle event based on type
    switch (event) {
        case SAFETY_EVENT_ESTOP_TRIGGERED:
            safety_manager_emergency_stop();
            break;
            
        case SAFETY_EVENT_ESTOP_RESET:
            safety_manager_safety_recovery();
            break;
            
        case SAFETY_EVENT_SAFETY_FAULT:
            g_safety_manager.status.current_level = SAFETY_LEVEL_CRITICAL;
            safety_manager_safety_shutdown();
            break;
            
        case SAFETY_EVENT_SAFETY_CLEARED:
            safety_manager_safety_recovery();
            break;
            
        case SAFETY_EVENT_EMERGENCY_STOP:
            safety_manager_emergency_stop();
            break;
            
        case SAFETY_EVENT_SAFETY_TIMEOUT:
            // Handle safety timeout
            break;
            
        case SAFETY_EVENT_SYSTEM_FAULT:
            g_safety_manager.status.current_level = SAFETY_LEVEL_CRITICAL;
            safety_manager_safety_shutdown();
            break;
            
        case SAFETY_EVENT_INTERLOCK_TRIGGERED:
            g_safety_manager.status.current_level = SAFETY_LEVEL_CRITICAL;
            safety_manager_safety_shutdown();
            break;
            
        case SAFETY_EVENT_INTERLOCK_RELEASED:
            safety_manager_safety_recovery();
            break;
            
        default:
            break;
    }
    
    // Call event callback if set
    if (g_safety_manager.event_callback != NULL) {
        g_safety_manager.event_callback(event, g_safety_manager.status.current_fault);
    }
    
    return HAL_STATUS_OK;
}

// E-Stop callback function
static void estop_callback(estop_state_t state, estop_fault_t fault) {
    (void)fault; // Unused parameter
    if (!g_safety_manager.initialized) {
        return;
    }
    
    switch (state) {
        case ESTOP_STATE_SAFE:
            if (g_safety_manager.status.estop_triggered) {
                safety_manager_handle_estop_reset();
            }
            break;
            
        case ESTOP_STATE_TRIGGERED:
            if (!g_safety_manager.status.estop_triggered) {
                safety_manager_handle_estop_trigger();
            }
            break;
            
        case ESTOP_STATE_FAULT:
            safety_manager_trigger_fault(SAFETY_FAULT_ESTOP_HARDWARE);
            break;
            
        case ESTOP_STATE_RESETTING:
            // E-Stop is being reset
            break;
            
        default:
            break;
    }
}
