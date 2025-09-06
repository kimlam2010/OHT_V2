/**
 * @file api_telemetry_collectors.c
 * @brief API Telemetry Data Collection Functions for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.2
 */

#include "api_telemetry.h"
#include "api_endpoints.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Collect system telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_system_data(api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get system status
    api_system_status_t system_status;
    hal_status_t result = api_get_system_status(&system_status);
    
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Create JSON data
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"system_name\": \"%s\",\n"
        "  \"version\": \"%s\",\n"
        "  \"status\": \"%s\",\n"
        "  \"uptime_ms\": %lu,\n"
        "  \"active_modules\": %u,\n"
        "  \"estop_active\": %s,\n"
        "  \"safety_ok\": %s,\n"
        "  \"timestamp\": %lu\n"
        "}",
        system_status.system_name,
        system_status.version,
        system_status.status,
        system_status.uptime_ms,
        system_status.active_modules,
        system_status.estop_active ? "true" : "false",
        system_status.safety_ok ? "true" : "false",
        hal_get_timestamp_ms()
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return HAL_STATUS_ERROR;
    }
    
    // Create telemetry data
    return api_telemetry_create_data(API_TELEMETRY_TYPE_SYSTEM, "system_controller", json_buffer, data);
}

/**
 * @brief Collect safety telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_safety_data(api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get safety status
    api_safety_status_t safety_status;
    hal_status_t result = api_get_safety_status(&safety_status);
    
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Get safety sensors
    api_safety_sensors_t safety_sensors;
    result = api_get_safety_sensors(&safety_sensors);
    
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Create JSON data
    char json_buffer[1024];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"estop_active\": %s,\n"
        "  \"safety_ok\": %s,\n"
        "  \"safety_level\": %u,\n"
        "  \"last_safety_check\": %lu,\n"
        "  \"analog_sensors\": [%d, %d, %d, %d],\n"
        "  \"digital_sensors\": %d,\n"
        "  \"proximity_alert\": %s,\n"
        "  \"timestamp\": %lu\n"
        "}",
        safety_status.estop_active ? "true" : "false",
        safety_status.safety_ok ? "true" : "false",
        safety_status.safety_level,
        safety_status.last_safety_check,
        safety_sensors.analog_sensors[0],
        safety_sensors.analog_sensors[1],
        safety_sensors.analog_sensors[2],
        safety_sensors.analog_sensors[3],
        safety_sensors.digital_sensors,
        safety_sensors.proximity_alert ? "true" : "false",
        hal_get_timestamp_ms()
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return HAL_STATUS_ERROR;
    }
    
    // Create telemetry data
    return api_telemetry_create_data(API_TELEMETRY_TYPE_SAFETY, "safety_manager", json_buffer, data);
}

/**
 * @brief Collect motor telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_motor_data(api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get motor status
    api_motor_status_t motor_status;
    hal_status_t result = api_get_motor_status(&motor_status);
    
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Create JSON data
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"position\": %d,\n"
        "  \"velocity\": %d,\n"
        "  \"temperature\": %d,\n"
        "  \"voltage\": %d,\n"
        "  \"current\": %d,\n"
        "  \"timestamp\": %lu\n"
        "}",
        motor_status.position,
        motor_status.velocity,
        motor_status.temperature,
        motor_status.voltage,
        motor_status.current,
        hal_get_timestamp_ms()
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return HAL_STATUS_ERROR;
    }
    
    // Create telemetry data
    return api_telemetry_create_data(API_TELEMETRY_TYPE_MOTOR, "motor_module", json_buffer, data);
}

/**
 * @brief Collect power telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_power_data(api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get power status
    api_power_status_t power_status;
    hal_status_t result = api_get_power_status(&power_status);
    
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Create JSON data
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"battery_voltage\": %.2f,\n"
        "  \"battery_current\": %.2f,\n"
        "  \"battery_soc\": %d,\n"
        "  \"charge_status\": %d,\n"
        "  \"timestamp\": %lu\n"
        "}",
        power_status.battery_voltage,
        power_status.battery_current,
        power_status.battery_soc,
        power_status.charge_status,
        hal_get_timestamp_ms()
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return HAL_STATUS_ERROR;
    }
    
    // Create telemetry data
    return api_telemetry_create_data(API_TELEMETRY_TYPE_POWER, "power_module", json_buffer, data);
}

/**
 * @brief Collect dock telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_dock_data(api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get dock status
    api_dock_status_t dock_status;
    hal_status_t result = api_get_dock_status(&dock_status);
    
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Create JSON data
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"dock_status\": %d,\n"
        "  \"dock_position\": %d,\n"
        "  \"dock_ready\": %s,\n"
        "  \"timestamp\": %lu\n"
        "}",
        dock_status.dock_status,
        dock_status.dock_position,
        dock_status.dock_ready ? "true" : "false",
        hal_get_timestamp_ms()
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return HAL_STATUS_ERROR;
    }
    
    // Create telemetry data
    return api_telemetry_create_data(API_TELEMETRY_TYPE_DOCK, "dock_module", json_buffer, data);
}

/**
 * @brief Collect network telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_network_data(api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get network statistics (placeholder - would need network manager)
    uint32_t bytes_sent = 0;
    uint32_t bytes_received = 0;
    uint32_t packets_sent = 0;
    uint32_t packets_received = 0;
    uint32_t connection_count = 0;
    
    // Create JSON data
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"bytes_sent\": %u,\n"
        "  \"bytes_received\": %u,\n"
        "  \"packets_sent\": %u,\n"
        "  \"packets_received\": %u,\n"
        "  \"connection_count\": %u,\n"
        "  \"timestamp\": %lu\n"
        "}",
        bytes_sent,
        bytes_received,
        packets_sent,
        packets_received,
        connection_count,
        hal_get_timestamp_ms()
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return HAL_STATUS_ERROR;
    }
    
    // Create telemetry data
    return api_telemetry_create_data(API_TELEMETRY_TYPE_NETWORK, "network_manager", json_buffer, data);
}
