/**
 * @file safety_manager.c
 * @brief Safety Manager Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-02 (Safety Integration)
 */

#include "safety_manager.h"
#include "safety_types.h"
#include <string.h>
#include <stdio.h>

// Internal safety manager structure
typedef struct {
    safety_config_t config;
    safety_status_info_t status;
    safety_event_callback_t event_callback;
    bool initialized;
    uint64_t last_update_time;
    uint64_t start_time;
    // Additional internal state
    bool estop_triggered;
    bool safety_circuit_ok;
    bool sensors_ok;
    uint32_t estop_count;
    uint32_t interlock_count;
    uint64_t last_safety_check;
    uint64_t uptime_seconds;
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
    g_safety_manager.status.initialized = true;
    g_safety_manager.status.status = SAFETY_STATUS_OK;
    g_safety_manager.status.level = SAFETY_LEVEL_NORMAL;
    g_safety_manager.status.last_event = SAFETY_EVENT_NONE;
    g_safety_manager.status.current_fault = SAFETY_FAULT_NONE;
    g_safety_manager.status.last_event_time = hal_get_timestamp_us();
    g_safety_manager.status.fault_count = 0;
    g_safety_manager.status.event_count = 0;
    
    // Initialize internal state
    g_safety_manager.estop_triggered = false;
    g_safety_manager.safety_circuit_ok = true;
    g_safety_manager.sensors_ok = true;
    g_safety_manager.estop_count = 0;
    g_safety_manager.interlock_count = 0;
    g_safety_manager.last_safety_check = hal_get_timestamp_us();
    g_safety_manager.uptime_seconds = 0;
    
    // Set start time
    g_safety_manager.start_time = hal_get_timestamp_us();
    g_safety_manager.last_update_time = g_safety_manager.start_time;
    g_safety_manager.initialized = true;
    
    // Set E-Stop callback (guard return code)
    if (hal_estop_set_callback(estop_callback) != HAL_STATUS_OK) {
        printf("[SAFETY] WARNING: hal_estop_set_callback failed\n");
    }
    
    // Set initial LED pattern for safety (guard return code)
    if (hal_led_error_set(LED_STATE_OFF) != HAL_STATUS_OK) {
        printf("[SAFETY] WARNING: hal_led_error_set failed\n");
    }
    
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
    g_safety_manager.uptime_seconds = (current_time - g_safety_manager.start_time) / 1000000ULL;
    
    // Check if it's time for safety check
    uint64_t time_since_last_check = (current_time - g_safety_manager.last_safety_check) / 1000ULL;
    
    if (time_since_last_check >= g_safety_manager.config.safety_check_interval_ms) {
        g_safety_manager.last_safety_check = current_time;
        
        // Perform safety checks
        update_safety_status();
        check_safety_conditions();
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_get_status(safety_status_info_t *status) {
    if (!g_safety_manager.initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *status = g_safety_manager.status;
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_is_safe(bool *safe) {
    if (!g_safety_manager.initialized || safe == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *safe = (g_safety_manager.status.level == SAFETY_LEVEL_NORMAL) &&
            !g_safety_manager.estop_triggered &&
            (g_safety_manager.status.current_fault == SAFETY_FAULT_NONE);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_trigger_fault(safety_fault_t fault) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.current_fault = fault;
    g_safety_manager.status.fault_count++;
    
    return safety_manager_process_event(SAFETY_EVENT_FAULT_DETECTED);
}

hal_status_t safety_manager_clear_fault(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.current_fault = SAFETY_FAULT_NONE;
    
    return safety_manager_process_event(SAFETY_EVENT_FAULT_CLEARED);
}

hal_status_t safety_manager_get_fault(safety_fault_t *fault) {
    if (!g_safety_manager.initialized || fault == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *fault = g_safety_manager.status.current_fault;
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_process_event(safety_event_t event) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    return handle_safety_event(event);
}

hal_status_t safety_manager_set_event_callback(safety_event_callback_t callback) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_handle_estop_trigger(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.estop_triggered = true;
    g_safety_manager.estop_count++;
    g_safety_manager.status.level = SAFETY_LEVEL_EMERGENCY;
    
    // Update LED pattern (guard return codes)
    if (hal_led_error_set(LED_STATE_ON) != HAL_STATUS_OK) {
        printf("[SAFETY] WARNING: hal_led_error_set failed\n");
    }
    if (hal_led_system_set(LED_STATE_BLINK_FAST) != HAL_STATUS_OK) {
        printf("[SAFETY] WARNING: hal_led_system_set failed\n");
    }
    
    return safety_manager_process_event(SAFETY_EVENT_EMERGENCY_STOP);
}

hal_status_t safety_manager_handle_estop_reset(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.estop_triggered = false;
    g_safety_manager.status.last_event = SAFETY_EVENT_ESTOP_RELEASED;
    
    // Update LED pattern (guard return codes)
    if (hal_led_error_set(LED_STATE_OFF) != HAL_STATUS_OK) {
        printf("[SAFETY] WARNING: hal_led_error_set failed\n");
    }
    if (hal_led_system_set(LED_STATE_ON) != HAL_STATUS_OK) {
        printf("[SAFETY] WARNING: hal_led_system_set failed\n");
    }
    
    handle_safety_event(SAFETY_EVENT_FAULT_CLEARED);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_trigger_interlock(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.interlock_count++;
    g_safety_manager.status.last_event = SAFETY_EVENT_SAFETY_ZONE_VIOLATION;
    
    handle_safety_event(SAFETY_EVENT_FAULT_DETECTED);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_reset_interlock(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_manager.status.last_event = SAFETY_EVENT_SAFETY_ZONE_CLEAR;
    
    handle_safety_event(SAFETY_EVENT_FAULT_CLEARED);
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_check_estop(bool *triggered) {
    if (!g_safety_manager.initialized || triggered == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *triggered = g_safety_manager.estop_triggered;
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_check_safety_circuit(bool *ok) {
    if (!g_safety_manager.initialized || ok == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Simulate safety circuit check
    g_safety_manager.safety_circuit_ok = true;
    *ok = g_safety_manager.safety_circuit_ok;
    
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
        return safety_manager_trigger_fault(SAFETY_FAULT_SYSTEM_FAILURE);
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_check_sensors(bool *ok) {
    if (!g_safety_manager.initialized || ok == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Simulate sensor check
    g_safety_manager.sensors_ok = true;
    *ok = g_safety_manager.sensors_ok;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_safety_recovery(void) {
    if (!g_safety_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Clear all safety faults
    g_safety_manager.status.current_fault = SAFETY_FAULT_NONE;
    g_safety_manager.status.level = SAFETY_LEVEL_NORMAL;
    
    // Reset LED patterns
    hal_led_error_set(LED_STATE_OFF);
    hal_led_system_set(LED_STATE_ON);
    hal_led_comm_set(LED_STATE_BLINK_SLOW);
    hal_led_network_set(LED_STATE_ON);
    
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
        case SAFETY_EVENT_ESTOP_PRESSED: return "ESTOP_PRESSED";
        case SAFETY_EVENT_ESTOP_RELEASED: return "ESTOP_RELEASED";
        case SAFETY_EVENT_SAFETY_ZONE_VIOLATION: return "SAFETY_ZONE_VIOLATION";
        case SAFETY_EVENT_SAFETY_ZONE_CLEAR: return "SAFETY_ZONE_CLEAR";
        case SAFETY_EVENT_FAULT_DETECTED: return "FAULT_DETECTED";
        case SAFETY_EVENT_FAULT_CLEARED: return "FAULT_CLEARED";
        case SAFETY_EVENT_EMERGENCY_STOP: return "EMERGENCY_STOP";
        case SAFETY_EVENT_SAFETY_RESET: return "SAFETY_RESET";
        default: return "UNKNOWN";
    }
}

const char* safety_manager_get_fault_name(safety_fault_t fault) {
    switch (fault) {
        case SAFETY_FAULT_NONE: return "NONE";
        case SAFETY_FAULT_ESTOP_ACTIVATED: return "ESTOP_ACTIVATED";
        case SAFETY_FAULT_SAFETY_ZONE_VIOLATION: return "SAFETY_ZONE_VIOLATION";
        case SAFETY_FAULT_OVERTEMPERATURE: return "OVERTEMPERATURE";
        case SAFETY_FAULT_OVERCURRENT: return "OVERCURRENT";
        case SAFETY_FAULT_OVERVOLTAGE: return "OVERVOLTAGE";
        case SAFETY_FAULT_UNDERVOLTAGE: return "UNDERVOLTAGE";
        case SAFETY_FAULT_COMMUNICATION_LOSS: return "COMMUNICATION_LOSS";
        case SAFETY_FAULT_SENSOR_FAILURE: return "SENSOR_FAILURE";
        case SAFETY_FAULT_ACTUATOR_FAILURE: return "ACTUATOR_FAILURE";
        case SAFETY_FAULT_SYSTEM_FAILURE: return "SYSTEM_FAILURE";
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
        "Last Safety Check: %lu ms ago\n"
        "Fault Count: %u\n"
        "E-Stop Count: %u\n"
        "Uptime: %lu seconds\n",
        safety_manager_get_level_name(g_safety_manager.status.level),
        safety_manager_get_event_name(g_safety_manager.status.last_event),
        safety_manager_get_fault_name(g_safety_manager.status.current_fault),
        g_safety_manager.estop_triggered ? "YES" : "NO",
        g_safety_manager.safety_circuit_ok ? "YES" : "NO",
        g_safety_manager.sensors_ok ? "YES" : "NO",
        (unsigned long)((hal_get_timestamp_us() - g_safety_manager.last_safety_check) / 1000),
        g_safety_manager.status.fault_count,
        g_safety_manager.estop_count,
        (unsigned long)g_safety_manager.uptime_seconds
    );
    
    return HAL_STATUS_OK;
}

hal_status_t safety_manager_validate_compliance(bool *compliant) {
    if (!g_safety_manager.initialized || compliant == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *compliant = (g_safety_manager.status.level == SAFETY_LEVEL_NORMAL) &&
                 !g_safety_manager.estop_triggered &&
                 (g_safety_manager.status.current_fault == SAFETY_FAULT_NONE) &&
                 g_safety_manager.safety_circuit_ok &&
                 g_safety_manager.sensors_ok;
    
    return HAL_STATUS_OK;
}

// Private Functions
static hal_status_t update_safety_status(void) {
    // Update safety status based on current conditions
    estop_state_t estop_state;
    bool circuit_ok = true;
    bool sensors_ok = true;
    
    // Check E-Stop status
    hal_estop_get_state(&estop_state);
    g_safety_manager.estop_triggered = (estop_state == ESTOP_STATE_TRIGGERED);
    
    // Update safety circuit status
    g_safety_manager.safety_circuit_ok = circuit_ok;
    
    // Update sensor status
    g_safety_manager.sensors_ok = sensors_ok;
    
    return HAL_STATUS_OK;
}

static hal_status_t check_safety_conditions(void) {
    // Check for safety violations
    if (g_safety_manager.estop_triggered) {
        g_safety_manager.status.level = SAFETY_LEVEL_EMERGENCY;
        return safety_manager_process_event(SAFETY_EVENT_EMERGENCY_STOP);
    }
    
    // Check for faults
    if (g_safety_manager.status.current_fault != SAFETY_FAULT_NONE) {
        g_safety_manager.status.level = SAFETY_LEVEL_CRITICAL;
        return safety_manager_process_event(SAFETY_EVENT_FAULT_DETECTED);
    }
    
    // Check for warnings
    if (!g_safety_manager.safety_circuit_ok ||
        !g_safety_manager.sensors_ok) {
        g_safety_manager.status.level = SAFETY_LEVEL_WARNING;
        return HAL_STATUS_OK;
    }
    
    // All conditions normal
    g_safety_manager.status.level = SAFETY_LEVEL_NORMAL;
    
    return HAL_STATUS_OK;
}

static hal_status_t handle_safety_event(safety_event_t event) {
    g_safety_manager.status.last_event = event;
    g_safety_manager.status.last_event_time = hal_get_timestamp_us();
    g_safety_manager.status.event_count++;
    
    switch (event) {
        case SAFETY_EVENT_EMERGENCY_STOP:
            // Emergency stop event
            break;
            
        case SAFETY_EVENT_ESTOP_RELEASED:
            // E-Stop released event
            break;
            
        case SAFETY_EVENT_FAULT_DETECTED:
            g_safety_manager.status.level = SAFETY_LEVEL_CRITICAL;
            break;
            
        case SAFETY_EVENT_FAULT_CLEARED:
            // Fault cleared event
            break;
            
        case SAFETY_EVENT_SAFETY_RESET:
            // Safety reset event
            break;
            
        default:
            // Unknown event
            break;
    }
    
    // Call event callback if set
    if (g_safety_manager.event_callback) {
        g_safety_manager.event_callback(event, g_safety_manager.status.current_fault);
    }
    
    return HAL_STATUS_OK;
}

static void estop_callback(estop_state_t state, estop_fault_t fault) {
    (void)fault; // Unused parameter
    
    if (!g_safety_manager.initialized) {
        return;
    }
    
    switch (state) {
        case ESTOP_STATE_TRIGGERED:
            if (g_safety_manager.estop_triggered) {
                // E-Stop already triggered, ignore
                return;
            }
            safety_manager_handle_estop_trigger();
            break;
            
        case ESTOP_STATE_SAFE:
            if (!g_safety_manager.estop_triggered) {
                // E-Stop already released, ignore
                return;
            }
            safety_manager_handle_estop_reset();
            break;
            
        case ESTOP_STATE_FAULT:
            safety_manager_trigger_fault(SAFETY_FAULT_ESTOP_ACTIVATED);
            break;
            
        default:
            // Unknown state
            break;
    }
}
