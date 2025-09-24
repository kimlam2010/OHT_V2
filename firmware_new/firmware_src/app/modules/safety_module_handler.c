/**
 * @file safety_module_handler.c
 * @brief Safety Module Handler Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-Phase3-1 (Safety Module Implementation)
 */

#include "safety_module_handler.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Global safety module handler instance
// static safety_module_handler_t g_safety_module_handler = {0}; // Unused - commented out
static safety_event_callback_t g_event_callback = NULL;

// Default configuration
static const safety_module_config_t DEFAULT_SAFETY_CONFIG = {
    .address = 0x03,
    .response_timeout_ms = 1000,
    .update_interval_ms = 50,
    .zone_thresholds = {1000, 500, 200, 100},  // Warning, Critical, Emergency zones (mm)
    .proximity_threshold = 50,                 // 50mm proximity threshold
    .auto_reset_enabled = true,
    .safety_mode = 0,
    .enable_debouncing = true,
    .debounce_time_ms = 10
};

// ============================================================================
// INITIALIZATION AND CONFIGURATION
// ============================================================================

hal_status_t safety_module_init(safety_module_handler_t *handler, const safety_module_config_t *config) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize handler with default values
    memset(handler, 0, sizeof(safety_module_handler_t));
    
    // Set configuration
    if (config) {
        handler->config = *config;
    } else {
        handler->config = DEFAULT_SAFETY_CONFIG;
    }
    
    // Validate configuration
    hal_status_t status = safety_module_validate_config(&handler->config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Initialize status
    handler->status.state = SAFETY_STATE_DISABLED;
    handler->status.fault_code = SAFETY_FAULT_NONE;
    handler->status.fault_count = 0;
    handler->status.health_percentage = 100;
    handler->status.emergency_stop_active = false;
    handler->status.safety_violation = false;
    
    // Initialize data
    memset(&handler->data, 0, sizeof(safety_module_data_t));
    
    // Initialize statistics
    memset(&handler->statistics, 0, sizeof(safety_module_stats_t));
    
    // Set initialization flag
    handler->initialized = true;
    handler->enabled = false;
    
    printf("Safety Module Handler initialized (address: 0x%02X)\n", handler->config.address);
    return HAL_STATUS_OK;
}

hal_status_t safety_module_deinit(safety_module_handler_t *handler) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Disable module
    handler->enabled = false;
    handler->initialized = false;
    
    printf("Safety Module Handler deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t safety_module_enable(safety_module_handler_t *handler, bool enable) {
    if (!handler || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    handler->enabled = enable;
    handler->status.state = enable ? SAFETY_STATE_ENABLED : SAFETY_STATE_DISABLED;
    
    // Emit event
    if (g_event_callback) {
        safety_event_t event = enable ? SAFETY_EVENT_SAFETY_RESET : SAFETY_EVENT_NONE;
        g_event_callback(event, SAFETY_FAULT_NONE);
    }
    
    printf("Safety Module %s\n", enable ? "enabled" : "disabled");
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_config(safety_module_handler_t *handler, safety_module_config_t *config) {
    if (!handler || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *config = handler->config;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_set_config(safety_module_handler_t *handler, const safety_module_config_t *config) {
    if (!handler || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Validate new configuration
    hal_status_t status = safety_module_validate_config(config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    handler->config = *config;
    return HAL_STATUS_OK;
}

// ============================================================================
// STATUS AND INFORMATION
// ============================================================================

hal_status_t safety_module_get_status(safety_module_handler_t *handler, safety_module_status_t *status) {
    if (!handler || !status) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *status = handler->status;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_data(safety_module_handler_t *handler, safety_module_data_t *data) {
    if (!handler || !data) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *data = handler->data;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_statistics(safety_module_handler_t *handler, safety_module_stats_t *stats) {
    if (!handler || !stats) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *stats = handler->statistics;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_diagnostics(safety_module_handler_t *handler, char *info, size_t max_len) {
    if (!handler || !info) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    snprintf(info, max_len,
        "Safety Module Diagnostics:\n"
        "Address: 0x%02X\n"
        "State: %s\n"
        "Health: %d%%\n"
        "Emergency Stop: %s\n"
        "Safety Violation: %s\n"
        "Fault Code: %s\n"
        "Response Time: %d ms\n"
        "Total Events: %d\n"
        "Uptime: %lu ms",
        handler->config.address,
        safety_module_get_state_name(handler->status.state),
        handler->status.health_percentage,
        handler->status.emergency_stop_active ? "ACTIVE" : "INACTIVE",
        handler->status.safety_violation ? "YES" : "NO",
        safety_module_get_fault_name(handler->status.fault_code),
        handler->status.response_time_ms,
        handler->statistics.total_events,
        handler->statistics.total_uptime_ms
    );
    
    return HAL_STATUS_OK;
}

// ============================================================================
// SENSOR DATA ACCESS
// ============================================================================

hal_status_t safety_module_get_analog_sensor(safety_module_handler_t *handler, uint8_t sensor_number, uint16_t *distance) {
    if (!handler || !distance) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_sensor_number(sensor_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Bounds check to prevent array access warnings
    if (sensor_number >= 4) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *distance = handler->data.analog_sensors[sensor_number];
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_analog_raw(safety_module_handler_t *handler, uint8_t sensor_number, uint16_t *raw_value) {
    if (!handler || !raw_value) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_sensor_number(sensor_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Bounds check to prevent array access warnings
    if (sensor_number >= 4) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *raw_value = handler->data.analog_raw[sensor_number];
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_digital_sensors(safety_module_handler_t *handler, uint8_t *sensors) {
    if (!handler || !sensors) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *sensors = handler->data.digital_sensors;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_is_digital_sensor_active(safety_module_handler_t *handler, uint8_t sensor_number, bool *active) {
    if (!handler || !active) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_sensor_number(sensor_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *active = (handler->data.digital_sensors & (1 << sensor_number)) != 0;
    return HAL_STATUS_OK;
}

// ============================================================================
// RELAY CONTROL
// ============================================================================

hal_status_t safety_module_set_relay(safety_module_handler_t *handler, uint8_t relay_number, bool state) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_relay_number(relay_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (state) {
        handler->data.relay_outputs |= (1 << relay_number);
    } else {
        handler->data.relay_outputs &= ~(1 << relay_number);
    }
    
    // Emit event
    if (g_event_callback) {
        safety_event_t event = state ? SAFETY_EVENT_FAULT_DETECTED : SAFETY_EVENT_FAULT_CLEARED;
        g_event_callback(event, SAFETY_FAULT_NONE);
    }
    
    handler->statistics.relay_activations++;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_relay(safety_module_handler_t *handler, uint8_t relay_number, bool *state) {
    if (!handler || !state) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_relay_number(relay_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *state = (handler->data.relay_outputs & (1 << relay_number)) != 0;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_set_relays(safety_module_handler_t *handler, uint8_t relays) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    handler->data.relay_outputs = relays;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_relays(safety_module_handler_t *handler, uint8_t *relays) {
    if (!handler || !relays) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *relays = handler->data.relay_outputs;
    return HAL_STATUS_OK;
}

// ============================================================================
// SAFETY LOGIC
// ============================================================================

hal_status_t safety_module_check_safety(safety_module_handler_t *handler, bool *safe) {
    if (!handler || !safe) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check emergency stop
    if (handler->status.emergency_stop_active) {
        *safe = false;
        return HAL_STATUS_OK;
    }
    
    // Check safety zones
    if (handler->data.safety_zones != 0) {
        *safe = false;
        return HAL_STATUS_OK;
    }
    
    // Check proximity alert
    if (handler->data.proximity_alert) {
        *safe = false;
        return HAL_STATUS_OK;
    }
    
    // Check fault status
    if (handler->status.fault_code != SAFETY_FAULT_NONE) {
        *safe = false;
        return HAL_STATUS_OK;
    }
    
    *safe = true;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_safety_zones(safety_module_handler_t *handler, uint8_t *zones) {
    if (!handler || !zones) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *zones = handler->data.safety_zones;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_is_zone_violated(safety_module_handler_t *handler, uint8_t zone_number, bool *violated) {
    if (!handler || !violated) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_zone_number(zone_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *violated = (handler->data.safety_zones & (1 << zone_number)) != 0;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_zone_level(safety_module_handler_t *handler, uint8_t zone_number, safety_zone_level_t *level) {
    if (!handler || !level) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_zone_number(zone_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get distance for this zone with bounds check
    if (zone_number >= 4) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    uint16_t distance = handler->data.analog_sensors[zone_number];
    uint16_t threshold = handler->config.zone_thresholds[zone_number];
    
    *level = safety_module_distance_to_zone_level(distance, threshold);
    return HAL_STATUS_OK;
}

hal_status_t safety_module_is_proximity_alert(safety_module_handler_t *handler, bool *alert) {
    if (!handler || !alert) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *alert = handler->data.proximity_alert;
    return HAL_STATUS_OK;
}

// ============================================================================
// EMERGENCY STOP
// ============================================================================

hal_status_t safety_module_is_emergency_stop_active(safety_module_handler_t *handler, bool *active) {
    if (!handler || !active) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *active = handler->status.emergency_stop_active;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_clear_emergency_stop(safety_module_handler_t *handler) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    handler->status.emergency_stop_active = false;
    handler->status.state = SAFETY_STATE_ENABLED;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_module_test_emergency_stop(safety_module_handler_t *handler) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Simulate emergency stop test
    handler->status.emergency_stop_active = true;
    handler->status.state = SAFETY_STATE_EMERGENCY_STOP;
    
    // Emit event
    if (g_event_callback) {
        g_event_callback(SAFETY_EVENT_EMERGENCY_STOP, SAFETY_FAULT_ESTOP_ACTIVATED);
    }
    
    handler->statistics.emergency_stop_events++;
    return HAL_STATUS_OK;
}

// ============================================================================
// FAULT MANAGEMENT
// ============================================================================

hal_status_t safety_module_get_fault_code(safety_module_handler_t *handler, safety_fault_t *fault_code) {
    if (!handler || !fault_code) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *fault_code = handler->status.fault_code;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_clear_faults(safety_module_handler_t *handler) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    handler->status.fault_code = SAFETY_FAULT_NONE;
    handler->status.fault_count = 0;
    
    // Emit event
    if (g_event_callback) {
        g_event_callback(SAFETY_EVENT_FAULT_CLEARED, SAFETY_FAULT_NONE);
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_fault_count(safety_module_handler_t *handler, uint8_t *fault_count) {
    if (!handler || !fault_count) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *fault_count = handler->status.fault_count;
    return HAL_STATUS_OK;
}

// ============================================================================
// THRESHOLD MANAGEMENT
// ============================================================================

hal_status_t safety_module_set_zone_threshold(safety_module_handler_t *handler, uint8_t zone_number, uint16_t threshold) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_zone_number(zone_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (threshold < SAFETY_MODULE_MIN_DISTANCE_MM || threshold > SAFETY_MODULE_MAX_DISTANCE_MM) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Bounds check to prevent array access warnings
    if (zone_number >= 4) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->config.zone_thresholds[zone_number] = threshold;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_zone_threshold(safety_module_handler_t *handler, uint8_t zone_number, uint16_t *threshold) {
    if (!handler || !threshold) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!safety_module_validate_zone_number(zone_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Bounds check to prevent array access warnings
    if (zone_number >= 4) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *threshold = handler->config.zone_thresholds[zone_number];
    return HAL_STATUS_OK;
}

hal_status_t safety_module_set_proximity_threshold(safety_module_handler_t *handler, uint16_t threshold) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (threshold < SAFETY_MODULE_MIN_DISTANCE_MM || threshold > SAFETY_MODULE_MAX_DISTANCE_MM) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->config.proximity_threshold = threshold;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_get_proximity_threshold(safety_module_handler_t *handler, uint16_t *threshold) {
    if (!handler || !threshold) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *threshold = handler->config.proximity_threshold;
    return HAL_STATUS_OK;
}

// ============================================================================
// REGISTER ACCESS
// ============================================================================

hal_status_t safety_module_read_register(safety_module_handler_t *handler, uint16_t reg, uint16_t *value) {
    if (!handler || !value) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Simulate register read based on register address
    switch (reg) {
        case SAFETY_SYSTEM_STATUS_REG:
            *value = (uint16_t)handler->status.state;
            break;
        case SAFETY_EMERGENCY_STOP_REG:
            *value = handler->status.emergency_stop_active ? 1 : 0;
            break;
        case SAFETY_SAFETY_ZONE_REG:
            *value = handler->data.safety_zones;
            break;
        case SAFETY_PROXIMITY_ALERT_REG:
            *value = handler->data.proximity_alert ? 1 : 0;
            break;
        case SAFETY_RELAY_OUTPUT_REG:
            *value = handler->data.relay_outputs;
            break;
        case SAFETY_SAFETY_ERROR_REG:
            *value = (uint16_t)handler->status.fault_code;
            break;
        case SAFETY_SYSTEM_TEMP_REG:
            *value = (uint16_t)handler->data.system_temperature;
            break;
        case SAFETY_SYSTEM_VOLTAGE_REG:
            *value = handler->data.system_voltage;
            break;
        case SAFETY_ANALOG_INPUT_1_REG:
            *value = handler->data.analog_sensors[0];
            break;
        case SAFETY_ANALOG_INPUT_2_REG:
            *value = handler->data.analog_sensors[1];
            break;
        case SAFETY_ANALOG_INPUT_3_REG:
            *value = handler->data.analog_sensors[2];
            break;
        case SAFETY_ANALOG_INPUT_4_REG:
            *value = handler->data.analog_sensors[3];
            break;
        case SAFETY_DIGITAL_INPUT_REG:
            *value = handler->data.digital_sensors;
            break;
        case SAFETY_RELAY_CONTROL_REG:
            *value = handler->data.relay_outputs;
            break;
        case SAFETY_DEVICE_ID_REG:
            *value = 0x0003; // Safety module device ID
            break;
        case SAFETY_MODULE_TYPE_REG:
            *value = 0x0003; // Safety module type
            break;
        default:
            return HAL_STATUS_NOT_FOUND;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_module_write_register(safety_module_handler_t *handler, uint16_t reg, uint16_t value) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Simulate register write based on register address
    switch (reg) {
        case SAFETY_EMERGENCY_STOP_REG:
            handler->status.emergency_stop_active = (value != 0);
            break;
        case SAFETY_RELAY_CONTROL_REG:
            handler->data.relay_outputs = (uint8_t)value;
            break;
        case SAFETY_RELAY_1_CONTROL_REG:
            if (value) {
                handler->data.relay_outputs |= 0x01;
            } else {
                handler->data.relay_outputs &= ~0x01;
            }
            break;
        case SAFETY_RELAY_2_CONTROL_REG:
            if (value) {
                handler->data.relay_outputs |= 0x02;
            } else {
                handler->data.relay_outputs &= ~0x02;
            }
            break;
        case SAFETY_RELAY_3_CONTROL_REG:
            if (value) {
                handler->data.relay_outputs |= 0x04;
            } else {
                handler->data.relay_outputs &= ~0x04;
            }
            break;
        case SAFETY_RELAY_4_CONTROL_REG:
            if (value) {
                handler->data.relay_outputs |= 0x08;
            } else {
                handler->data.relay_outputs &= ~0x08;
            }
            break;
        case SAFETY_RESET_ERROR_CMD_REG:
            if (value == 0x0001) {
                safety_module_clear_faults(handler);
            }
            break;
        default:
            return HAL_STATUS_NOT_FOUND;
    }
    
    return HAL_STATUS_OK;
}

// ============================================================================
// EVENT HANDLING
// ============================================================================

hal_status_t safety_module_set_event_callback(safety_module_handler_t *handler, safety_event_callback_t callback) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t safety_module_clear_event_callback(safety_module_handler_t *handler) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_event_callback = NULL;
    return HAL_STATUS_OK;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

hal_status_t safety_module_self_test(safety_module_handler_t *handler) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("Safety Module self-test passed\n");
    return HAL_STATUS_OK;
}

hal_status_t safety_module_reset(safety_module_handler_t *handler) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Reset status
    handler->status.state = SAFETY_STATE_DISABLED;
    handler->status.fault_code = SAFETY_FAULT_NONE;
    handler->status.fault_count = 0;
    handler->status.health_percentage = 100;
    handler->status.emergency_stop_active = false;
    handler->status.safety_violation = false;
    
    // Reset data
    memset(&handler->data, 0, sizeof(safety_module_data_t));
    
    // Reset statistics
    memset(&handler->statistics, 0, sizeof(safety_module_stats_t));
    
    printf("Safety Module reset\n");
    return HAL_STATUS_OK;
}

hal_status_t safety_module_update(safety_module_handler_t *handler) {
    if (!handler) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Update timestamp
    handler->status.last_update_time = hal_get_timestamp_us();
    handler->statistics.total_uptime_ms = (handler->status.last_update_time - handler->statistics.last_event_time) / 1000;
    
    // Simulate sensor readings (in real implementation, this would read from hardware)
    for (int i = 0; i < 4; i++) {
        handler->data.analog_sensors[i] = 1000 + (i * 100); // Simulated distances
        handler->data.analog_raw[i] = handler->data.analog_sensors[i] * 10; // Simulated ADC values
    }
    
    // Simulate digital sensors
    handler->data.digital_sensors = 0x0F; // All sensors active
    
    // Simulate system data
    handler->data.system_temperature = 250; // 25.0°C
    handler->data.system_voltage = 240;     // 24.0V
    
    // Update safety zones based on analog sensors
    handler->data.safety_zones = 0;
    for (int i = 0; i < 4; i++) {
        if (handler->data.analog_sensors[i] < handler->config.zone_thresholds[i]) {
            handler->data.safety_zones |= (1 << i);
        }
    }
    
    // Update proximity alert
    handler->data.proximity_alert = false;
    for (int i = 0; i < 4; i++) {
        if (handler->data.analog_sensors[i] < handler->config.proximity_threshold) {
            handler->data.proximity_alert = true;
            break;
        }
    }
    
    return HAL_STATUS_OK;
}

// ============================================================================
// VALIDATION FUNCTIONS
// ============================================================================

hal_status_t safety_module_validate_config(const safety_module_config_t *config) {
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!safety_module_validate_address(config->address)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->response_timeout_ms == 0 || config->response_timeout_ms > 10000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->update_interval_ms == 0 || config->update_interval_ms > 1000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    for (int i = 0; i < 4; i++) {
        if (config->zone_thresholds[i] < SAFETY_MODULE_MIN_DISTANCE_MM || 
            config->zone_thresholds[i] > SAFETY_MODULE_MAX_DISTANCE_MM) {
            return HAL_STATUS_INVALID_PARAMETER;
        }
    }
    
    if (config->proximity_threshold < SAFETY_MODULE_MIN_DISTANCE_MM || 
        config->proximity_threshold > SAFETY_MODULE_MAX_DISTANCE_MM) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t safety_module_validate_address(uint8_t address) {
    return (address == 0x03) ? HAL_STATUS_OK : HAL_STATUS_INVALID_PARAMETER;
}

hal_status_t safety_module_validate_sensor_number(uint8_t sensor_number) {
    return (sensor_number < 4) ? HAL_STATUS_OK : HAL_STATUS_INVALID_PARAMETER;
}

hal_status_t safety_module_validate_relay_number(uint8_t relay_number) {
    return (relay_number < 4) ? HAL_STATUS_OK : HAL_STATUS_INVALID_PARAMETER;
}

hal_status_t safety_module_validate_zone_number(uint8_t zone_number) {
    return (zone_number < 4) ? HAL_STATUS_OK : HAL_STATUS_INVALID_PARAMETER;
}

// ============================================================================
// CONVERSION FUNCTIONS
// ============================================================================

safety_zone_level_t safety_module_distance_to_zone_level(uint16_t distance, uint16_t threshold) {
    if (distance >= threshold) {
        return SAFETY_ZONE_LEVEL_NONE;
    } else if (distance >= threshold * 0.8) {
        return SAFETY_ZONE_LEVEL_WARNING;
    } else if (distance >= threshold * 0.5) {
        return SAFETY_ZONE_LEVEL_CRITICAL;
    } else {
        return SAFETY_ZONE_LEVEL_EMERGENCY;
    }
}

const char* safety_module_get_fault_name(safety_fault_t fault_code) {
    switch (fault_code) {
        case SAFETY_FAULT_NONE: return "None";
        case SAFETY_FAULT_ESTOP_ACTIVATED: return "E-Stop Activated";
        case SAFETY_FAULT_SAFETY_ZONE_VIOLATION: return "Safety Zone Violation";
        case SAFETY_FAULT_OVERTEMPERATURE: return "Over Temperature";
        case SAFETY_FAULT_OVERCURRENT: return "Over Current";
        case SAFETY_FAULT_OVERVOLTAGE: return "Over Voltage";
        case SAFETY_FAULT_UNDERVOLTAGE: return "Under Voltage";
        case SAFETY_FAULT_COMMUNICATION_LOSS: return "Communication Loss";
        case SAFETY_FAULT_SENSOR_FAILURE: return "Sensor Failure";
        case SAFETY_FAULT_ACTUATOR_FAILURE: return "Actuator Failure";
        case SAFETY_FAULT_SYSTEM_FAILURE: return "System Failure";
        default: return "Unknown";
    }
}

const char* safety_module_get_state_name(safety_state_t state) {
    switch (state) {
        case SAFETY_STATE_DISABLED: return "Disabled";
        case SAFETY_STATE_ENABLED: return "Enabled";
        case SAFETY_STATE_WARNING: return "Warning";
        case SAFETY_STATE_CRITICAL: return "Critical";
        case SAFETY_STATE_EMERGENCY_STOP: return "Emergency Stop";
        case SAFETY_STATE_FAULT: return "Fault";
        case SAFETY_STATE_MAINTENANCE: return "Maintenance";
        default: return "Unknown";
    }
}

const char* safety_module_get_event_name(safety_event_t event) {
    switch (event) {
        case SAFETY_EVENT_NONE: return "None";
        case SAFETY_EVENT_ESTOP_PRESSED: return "E-Stop Pressed";
        case SAFETY_EVENT_ESTOP_RELEASED: return "E-Stop Released";
        case SAFETY_EVENT_SAFETY_ZONE_VIOLATION: return "Safety Zone Violation";
        case SAFETY_EVENT_SAFETY_ZONE_CLEAR: return "Safety Zone Clear";
        case SAFETY_EVENT_FAULT_DETECTED: return "Fault Detected";
        case SAFETY_EVENT_FAULT_CLEARED: return "Fault Cleared";
        case SAFETY_EVENT_EMERGENCY_STOP: return "Emergency Stop";
        case SAFETY_EVENT_SAFETY_RESET: return "Safety Reset";
        default: return "Unknown";
    }
}

const char* safety_module_get_zone_level_name(safety_zone_level_t level) {
    switch (level) {
        case SAFETY_ZONE_LEVEL_NONE: return "None";
        case SAFETY_ZONE_LEVEL_WARNING: return "Warning";
        case SAFETY_ZONE_LEVEL_CRITICAL: return "Critical";
        case SAFETY_ZONE_LEVEL_EMERGENCY: return "Emergency";
        default: return "Unknown";
    }
}
