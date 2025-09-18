/**
 * @file telemetry_manager.c
 * @brief Telemetry Manager implementation for OHT-50 system
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "telemetry_manager.h"
#include "hal_common.h"
#include "system_state_machine.h"
#include "module_manager.h"
#include "safety_monitor.h"
#include "control_loop.h"

// Telemetry Manager instance
static struct {
    bool initialized;
    telemetry_config_t config;
    telemetry_data_t current_data;
    telemetry_stats_t statistics;
    telemetry_event_callback_t event_callback;
    
    // Ring buffer for telemetry data (reserved for future use)
    telemetry_data_t *ring_buffer;
    uint32_t buffer_head;
    uint32_t buffer_tail;
    uint32_t buffer_count;
    
    // Timing
    uint64_t last_update_time;
    uint64_t last_location_update;
    uint64_t last_navigation_update;
    uint64_t last_dock_update;
    uint64_t last_safety_update;
    uint64_t last_system_update;
} g_telemetry_manager;

// Default configuration
static const telemetry_config_t default_config = {
    .update_rate_ms = 100,      // 100ms update rate
    .buffer_size = 100,         // 100 entries ring buffer
    .enable_location = true,
    .enable_navigation = true,
    .enable_dock = true,
    .enable_system = true,
    .enable_events = true
};

// Forward declarations
static void initialize_telemetry_data(telemetry_data_t *data);
static void update_timestamp(telemetry_data_t *data);
static void collect_system_data(telemetry_data_t *data);
static void collect_control_data(telemetry_data_t *data);
static void collect_location_data(telemetry_data_t *data);
static void collect_navigation_data(telemetry_data_t *data);
static void collect_dock_data(telemetry_data_t *data);
static void collect_safety_data(telemetry_data_t *data);
static void broadcast_event(telemetry_event_t event, const telemetry_data_t *data);
static int serialize_location_json(const telemetry_location_t *location, char *buffer, size_t buffer_size);
static int serialize_navigation_json(const telemetry_navigation_t *navigation, char *buffer, size_t buffer_size);
static int serialize_dock_json(const telemetry_dock_t *dock, char *buffer, size_t buffer_size);
static int serialize_safety_json(const telemetry_safety_t *safety, char *buffer, size_t buffer_size);
static int serialize_status_json(const telemetry_status_t *status, char *buffer, size_t buffer_size);

// Telemetry Manager implementation

hal_status_t telemetry_manager_init(const telemetry_config_t *config) {
    printf("[TELEMETRY] telemetry_manager_init called\n");
    
    if (g_telemetry_manager.initialized) {
        printf("[TELEMETRY] Already initialized, returning\n");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize telemetry manager structure
    memset(&g_telemetry_manager, 0, sizeof(g_telemetry_manager));
    
    // Set configuration
    if (config != NULL) {
        printf("[TELEMETRY] Using provided config\n");
        memcpy(&g_telemetry_manager.config, config, sizeof(telemetry_config_t));
    } else {
        printf("[TELEMETRY] Using default config\n");
        memcpy(&g_telemetry_manager.config, &default_config, sizeof(telemetry_config_t));
    }
    
    // Initialize ring buffer
    if (g_telemetry_manager.config.buffer_size > 0) {
        g_telemetry_manager.ring_buffer = (telemetry_data_t*)malloc(
            g_telemetry_manager.config.buffer_size * sizeof(telemetry_data_t));
        if (g_telemetry_manager.ring_buffer == NULL) {
            printf("[TELEMETRY] Failed to allocate ring buffer\n");
            return HAL_STATUS_ERROR;
        }
        memset(g_telemetry_manager.ring_buffer, 0, 
               g_telemetry_manager.config.buffer_size * sizeof(telemetry_data_t));
    }
    
    // Initialize current telemetry data
    initialize_telemetry_data(&g_telemetry_manager.current_data);
    
    // Initialize timing
    g_telemetry_manager.last_update_time = hal_get_timestamp_us();
    g_telemetry_manager.last_location_update = g_telemetry_manager.last_update_time;
    g_telemetry_manager.last_navigation_update = g_telemetry_manager.last_update_time;
    g_telemetry_manager.last_dock_update = g_telemetry_manager.last_update_time;
    g_telemetry_manager.last_safety_update = g_telemetry_manager.last_update_time;
    g_telemetry_manager.last_system_update = g_telemetry_manager.last_update_time;
    
    g_telemetry_manager.initialized = true;
    printf("[TELEMETRY] Initialization complete\n");
    
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_deinit(void) {
    printf("[TELEMETRY] telemetry_manager_deinit called\n");
    
    if (!g_telemetry_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Free ring buffer
    if (g_telemetry_manager.ring_buffer != NULL) {
        free(g_telemetry_manager.ring_buffer);
        g_telemetry_manager.ring_buffer = NULL;
    }
    
    // Clear callback
    g_telemetry_manager.event_callback = NULL;
    
    g_telemetry_manager.initialized = false;
    printf("[TELEMETRY] Deinitialization complete\n");
    
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_update(void) {
    if (!g_telemetry_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = hal_get_timestamp_us();
    
    // Check if it's time to update
    if ((current_time - g_telemetry_manager.last_update_time) < 
        (g_telemetry_manager.config.update_rate_ms * 1000)) {
        return HAL_STATUS_OK;
    }
    
    // Update timestamp
    update_timestamp(&g_telemetry_manager.current_data);
    
    // Collect data from various sources
    if (g_telemetry_manager.config.enable_system) {
        collect_system_data(&g_telemetry_manager.current_data);
    }

    // Always collect control status for control.status publishing
    collect_control_data(&g_telemetry_manager.current_data);
    
    if (g_telemetry_manager.config.enable_location) {
        collect_location_data(&g_telemetry_manager.current_data);
    }
    
    if (g_telemetry_manager.config.enable_navigation) {
        collect_navigation_data(&g_telemetry_manager.current_data);
    }
    
    if (g_telemetry_manager.config.enable_dock) {
        collect_dock_data(&g_telemetry_manager.current_data);
    }
    
    collect_safety_data(&g_telemetry_manager.current_data);
    
    // Update statistics
    g_telemetry_manager.statistics.total_updates++;
    g_telemetry_manager.last_update_time = current_time;
    
    // Broadcast system status event
    if (g_telemetry_manager.config.enable_events) {
        broadcast_event(TELEMETRY_EVENT_SYSTEM_STATUS, &g_telemetry_manager.current_data);
    }
    
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_set_callback(telemetry_event_callback_t callback) {
    if (!g_telemetry_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_telemetry_manager.event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_get_data(telemetry_data_t *data) {
    if (!g_telemetry_manager.initialized || data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(data, &g_telemetry_manager.current_data, sizeof(telemetry_data_t));
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_update_location(const telemetry_location_t *location) {
    if (!g_telemetry_manager.initialized || location == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_telemetry_manager.current_data.location, location, sizeof(telemetry_location_t));
    g_telemetry_manager.statistics.location_updates++;
    g_telemetry_manager.last_location_update = hal_get_timestamp_us();
    
    if (g_telemetry_manager.config.enable_events) {
        broadcast_event(TELEMETRY_EVENT_LOCATION_UPDATE, &g_telemetry_manager.current_data);
    }
    
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_update_navigation(const telemetry_navigation_t *navigation) {
    if (!g_telemetry_manager.initialized || navigation == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_telemetry_manager.current_data.navigation, navigation, sizeof(telemetry_navigation_t));
    g_telemetry_manager.statistics.navigation_updates++;
    g_telemetry_manager.last_navigation_update = hal_get_timestamp_us();
    
    if (g_telemetry_manager.config.enable_events) {
        broadcast_event(TELEMETRY_EVENT_NAVIGATION_UPDATE, &g_telemetry_manager.current_data);
    }
    
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_update_dock(const telemetry_dock_t *dock) {
    if (!g_telemetry_manager.initialized || dock == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_telemetry_manager.current_data.dock, dock, sizeof(telemetry_dock_t));
    g_telemetry_manager.statistics.dock_updates++;
    g_telemetry_manager.last_dock_update = hal_get_timestamp_us();
    
    if (g_telemetry_manager.config.enable_events) {
        broadcast_event(TELEMETRY_EVENT_DOCK_UPDATE, &g_telemetry_manager.current_data);
    }
    
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_update_safety(const telemetry_safety_t *safety) {
    if (!g_telemetry_manager.initialized || safety == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_telemetry_manager.current_data.status.safety, safety, sizeof(telemetry_safety_t));
    g_telemetry_manager.statistics.safety_updates++;
    g_telemetry_manager.last_safety_update = hal_get_timestamp_us();
    
    // Check for safety alerts
    if (safety->estop || safety->emergency_stop || !safety->location_safe) {
        if (g_telemetry_manager.config.enable_events) {
            broadcast_event(TELEMETRY_EVENT_SAFETY_ALERT, &g_telemetry_manager.current_data);
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_update_status(const telemetry_status_t *status) {
    if (!g_telemetry_manager.initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_telemetry_manager.current_data.status, status, sizeof(telemetry_status_t));
    g_telemetry_manager.statistics.system_updates++;
    g_telemetry_manager.last_system_update = hal_get_timestamp_us();
    
    return HAL_STATUS_OK;
}

int telemetry_manager_serialize_json(const telemetry_data_t *data, char *json_buffer, size_t buffer_size) {
    if (data == NULL || json_buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = 0;
    int result;
    
    // Start JSON object
    result = snprintf(json_buffer + written, buffer_size - written,
        "{\n"
        "  \"ts\": %lu,\n"
        "  \"status\": ",
        (unsigned long)data->ts);
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Serialize status (partial, ends with '"safety": ')
    result = serialize_status_json(&data->status, json_buffer + written, buffer_size - written);
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Append safety object
    result = serialize_safety_json(&data->status.safety, json_buffer + written, buffer_size - written);
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Close status object
    result = snprintf(json_buffer + written, buffer_size - written, "\n  }");
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Serialize location
    result = snprintf(json_buffer + written, buffer_size - written, ",\n  \"location\": ");
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    result = serialize_location_json(&data->location, json_buffer + written, buffer_size - written);
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Serialize navigation
    result = snprintf(json_buffer + written, buffer_size - written, ",\n  \"navigation\": ");
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    result = serialize_navigation_json(&data->navigation, json_buffer + written, buffer_size - written);
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Serialize dock
    result = snprintf(json_buffer + written, buffer_size - written, ",\n  \"dock\": ");
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    result = serialize_dock_json(&data->dock, json_buffer + written, buffer_size - written);
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Add system fields
    result = snprintf(json_buffer + written, buffer_size - written,
        ",\n"
        "  \"mission_id\": \"%s\",\n"
        "  \"station_id\": \"%s\",\n"
        "  \"cargo_present\": %s,\n"
        "  \"cargo_locked\": %s,\n"
        "  \"cpu_usage\": %.2f,\n"
        "  \"memory_usage\": %.2f,\n"
        "  \"temperature\": %.2f,\n"
        "  \"center_connected\": %s,\n"
        "  \"rs485_connected\": %s,\n"
        "  \"tag_id\": \"%s\",\n"
        "  \"rssi\": %d\n"
        "}",
        data->mission_id,
        data->station_id,
        data->cargo_present ? "true" : "false",
        data->cargo_locked ? "true" : "false",
        data->cpu_usage,
        data->memory_usage,
        data->temperature,
        data->center_connected ? "true" : "false",
        data->rs485_connected ? "true" : "false",
        data->tag_id,
        data->rssi);
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    g_telemetry_manager.statistics.json_serializations++;
    return written;
}

hal_status_t telemetry_manager_get_statistics(telemetry_stats_t *stats) {
    if (!g_telemetry_manager.initialized || stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(stats, &g_telemetry_manager.statistics, sizeof(telemetry_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t telemetry_manager_reset_statistics(void) {
    if (!g_telemetry_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&g_telemetry_manager.statistics, 0, sizeof(telemetry_stats_t));
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize RS485 module telemetry to JSON (Issue #90 format)
 * @param module_addr Module address (0x02, 0x03, 0x04, 0x05)
 * @param json_buffer Output JSON buffer
 * @param buffer_size Buffer size
 * @return Number of bytes written, -1 on error
 */
int telemetry_manager_serialize_rs485_telemetry(uint8_t module_addr, char *json_buffer, size_t buffer_size) {
    if (json_buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    // Get current timestamp
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", gmtime(&ts.tv_sec));
    snprintf(timestamp + strlen(timestamp), sizeof(timestamp) - strlen(timestamp), 
             ".%06ldZ", ts.tv_nsec / 1000);
    
    // Get module name based on address
    const char *module_name;
    switch (module_addr) {
        case 0x02: module_name = "Power"; break;
        case 0x03: module_name = "Safety"; break;
        case 0x04: module_name = "Travel Motor"; break;
        case 0x05: module_name = "Dock & Location"; break;
        default: module_name = "Unknown"; break;
    }
    
    int written = 0;
    int result;
    
    // Start JSON object with issue #90 format
    result = snprintf(json_buffer + written, buffer_size - written,
        "{\n"
        "  \"type\": \"telemetry\",\n"
        "  \"data\": {\n"
        "    \"module_addr\": \"0x%02X\",\n"
        "    \"module_name\": \"%s\",\n"
        "    \"registers\": [",
        module_addr, module_name);
    
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Add module-specific registers based on address
    if (module_addr == 0x02) {
        // Power module registers (DalyBMS + SK60X + INA219)
        result = serialize_power_module_registers(json_buffer + written, buffer_size - written);
    } else if (module_addr == 0x03) {
        // Safety module registers
        result = serialize_safety_module_registers(json_buffer + written, buffer_size - written);
    } else if (module_addr == 0x04) {
        // Travel Motor module registers
        result = serialize_motor_module_registers(json_buffer + written, buffer_size - written);
    } else if (module_addr == 0x05) {
        // Dock & Location module registers
        result = serialize_dock_module_registers(json_buffer + written, buffer_size - written);
    } else {
        // Unknown module - basic registers
        result = snprintf(json_buffer + written, buffer_size - written,
            "\n      {\"addr\": \"0x0100\", \"name\": \"Device_ID\", \"value\": 0, \"unit\": \"hex\", \"mode\": \"R\"}");
    }
    
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    // Close JSON object
    result = snprintf(json_buffer + written, buffer_size - written,
        "\n    ],\n"
        "    \"timestamp\": \"%s\"\n"
        "  }\n"
        "}",
        timestamp);
    
    if (result < 0 || result >= (int)(buffer_size - written)) {
        return -1;
    }
    written += result;
    
    return written;
}

// Helper functions

static void initialize_telemetry_data(telemetry_data_t *data) {
    memset(data, 0, sizeof(telemetry_data_t));
    
    // Initialize with default values
    data->ts = hal_get_timestamp_us();
    data->status.state = SYSTEM_STATE_INIT;
    data->location.system_status = LOCATION_SYSTEM_OK;
    data->location.imu_status = IMU_STATUS_OK;
    data->location.magnetic_status = MAGNETIC_STATUS_OK;
    data->navigation.status = NAV_STATUS_IDLE;
    data->navigation.zone_type = ZONE_TYPE_SAFE;
    data->dock.status = DOCK_STATUS_NOT_DOCKING;
    data->dock.alignment_status = ALIGNMENT_NOT_ALIGNED;
    data->dock.charging_status = CHARGING_NOT_CHARGING;
    data->dock.safety_status = DOCK_SAFETY_SAFE;
    data->status.enc.health = ENCODER_HEALTH_OK;
    
    strcpy(data->mission_id, "NONE");
    strcpy(data->station_id, "NONE");
    strcpy(data->tag_id, "NONE");
}

static void update_timestamp(telemetry_data_t *data) {
    data->ts = hal_get_timestamp_us();
}

static void collect_system_data(telemetry_data_t *data) {
    // Get system state from state machine
    system_status_t system_status;
    if (system_state_machine_get_status(&system_status) == HAL_STATUS_OK) {
        data->status.state = system_status.current_state;
    }
    
    // Get real system metrics from HAL (when available)
    // For now, use basic system calls to get real data
    data->cpu_usage = 0.0f;  // TODO: Implement real CPU usage measurement
    data->memory_usage = 0.0f; // TODO: Implement real memory usage measurement
    data->temperature = 0.0f;  // TODO: Implement real temperature measurement
    
    // Get real connection status from communication manager
    comm_mgr_status_info_t comm_status;
    if (comm_manager_get_status(&comm_status) == HAL_STATUS_OK) {
        data->rs485_connected = (comm_status.status == COMM_MGR_STATUS_CONNECTED);
    } else {
        data->rs485_connected = false;
    }
    
    // TODO: Implement real center connection status
    data->center_connected = false;
}

static void collect_control_data(telemetry_data_t *data) {
    control_status_t cs;
    if (control_loop_get_status(&cs) == HAL_STATUS_OK) {
        // Velocity-only architecture: position from estimator_1d, velocity from control loop
        #include "../core/estimator_1d.h"
        est1d_state_t est = {0};
        (void)estimator_1d_get_state(&est);
        data->status.pos_mm = est.x_est_mm;
        data->status.vel_mms = cs.current_velocity;
        // Targets (position target removed)
        data->status.target.pos_mm = est.x_est_mm;
        data->status.target.vel_mms = cs.target_velocity;
    }
}

static void collect_location_data(telemetry_data_t *data) {
    // TODO: Get real location data from location system/HAL
    // For now, use zero values to indicate no real data available
    data->location.position.x = 0.0f;
    data->location.position.y = 0.0f;
    data->location.position.z = 0.0f;
    
    data->location.orientation.pitch = 0.0f;
    data->location.orientation.roll = 0.0f;
    data->location.orientation.yaw = 0.0f;
    
    data->location.velocity.x = 0.0f;
    data->location.velocity.y = 0.0f;
    data->location.velocity.z = 0.0f;
    
    data->location.acceleration.x = 0.0f;
    data->location.acceleration.y = 0.0f;
    data->location.acceleration.z = 0.0f;
    
    data->location.accuracy = 0.0f;
    
    // TODO: Implement real location system integration
}

static void collect_navigation_data(telemetry_data_t *data) {
    // TODO: Get real navigation data from navigation system/HAL
    // For now, use zero values to indicate no real data available
    data->navigation.target.x = 0.0f;
    data->navigation.target.y = 0.0f;
    data->navigation.target.z = 0.0f;
    
    data->navigation.distance_to_target = 0.0f;
    data->navigation.progress = 0.0f;
    data->navigation.estimated_time = 0.0f;
    data->navigation.path_clear = false;
    data->navigation.obstacle_detected = false;
    data->navigation.speed_limit = 0.0f;
    
    // TODO: Implement real navigation system integration
}

static void collect_dock_data(telemetry_data_t *data) {
    // TODO: Get real dock data from dock system/HAL
    // For now, use empty values to indicate no real data available
    strcpy(data->dock.station_id, "");
    strcpy(data->dock.target_station_id, "");
    data->dock.distance = 0.0f;
    data->dock.angle = 0.0f;
    data->dock.charging_current = 0.0f;
    data->dock.charging_voltage = 0.0f;
    strcpy(data->dock.rfid_tag_id, "");
    data->dock.rfid_signal_strength = 0;
    
    // TODO: Implement real dock system integration
}

static void collect_safety_data(telemetry_data_t *data) {
    // Populate from Safety Monitor
    bool estop_active = false;
    (void)safety_monitor_is_estop_active(&estop_active);
    data->status.safety.estop = estop_active;
    data->status.safety.emergency_stop = estop_active;

    safety_monitor_status_t ss;
    if (safety_monitor_get_status(&ss) == HAL_STATUS_OK) {
        data->status.safety.zone_blocked = ss.zone_violation;
        data->status.safety.interlock_active = ss.interlock_open;
        // Derive location safety as inverse of zone violation for now
        data->status.safety.location_safe = !ss.zone_violation;
        // Obstacle detected if any zone violation or LiDAR min distance within safe range
        data->status.safety.obstacle_detected = ss.zone_violation || ss.safety_zones.emergency_violated || ss.safety_zones.warning_violated;
        data->status.safety.safety_zone_clear = !ss.zone_violation;
    }
}

static void broadcast_event(telemetry_event_t event, const telemetry_data_t *data) {
    if (g_telemetry_manager.event_callback != NULL) {
        g_telemetry_manager.event_callback(event, data);
        g_telemetry_manager.statistics.events_sent++;
    }
}

// JSON serialization helpers

static int serialize_location_json(const telemetry_location_t *location, char *buffer, size_t buffer_size) {
    return snprintf(buffer, buffer_size,
        "{\n"
        "    \"position\": {\"x\": %.2f, \"y\": %.2f, \"z\": %.2f},\n"
        "    \"orientation\": {\"pitch\": %.2f, \"roll\": %.2f, \"yaw\": %.2f},\n"
        "    \"velocity\": {\"x\": %.2f, \"y\": %.2f, \"z\": %.2f},\n"
        "    \"acceleration\": {\"x\": %.2f, \"y\": %.2f, \"z\": %.2f},\n"
        "    \"accuracy\": %.2f,\n"
        "    \"system_status\": \"%s\",\n"
        "    \"imu_status\": \"%s\",\n"
        "    \"magnetic_status\": \"%s\"\n"
        "  }",
        location->position.x, location->position.y, location->position.z,
        location->orientation.pitch, location->orientation.roll, location->orientation.yaw,
        location->velocity.x, location->velocity.y, location->velocity.z,
        location->acceleration.x, location->acceleration.y, location->acceleration.z,
        location->accuracy,
        location->system_status == LOCATION_SYSTEM_OK ? "ok" : 
        location->system_status == LOCATION_SYSTEM_ERROR ? "error" : "calibrating",
        location->imu_status == IMU_STATUS_OK ? "ok" : "error",
        location->magnetic_status == MAGNETIC_STATUS_OK ? "ok" : "error");
}

static int serialize_navigation_json(const telemetry_navigation_t *navigation, char *buffer, size_t buffer_size) {
    const char *status_str = "idle";
    switch (navigation->status) {
        case NAV_STATUS_NAVIGATING: status_str = "navigating"; break;
        case NAV_STATUS_POSITIONING: status_str = "positioning"; break;
        case NAV_STATUS_ARRIVED: status_str = "arrived"; break;
        case NAV_STATUS_ERROR: status_str = "error"; break;
        default: status_str = "idle"; break;
    }
    
    const char *zone_str = "safe";
    switch (navigation->zone_type) {
        case ZONE_TYPE_WARNING: zone_str = "warning"; break;
        case ZONE_TYPE_RESTRICTED: zone_str = "restricted"; break;
        case ZONE_TYPE_EMERGENCY: zone_str = "emergency"; break;
        default: zone_str = "safe"; break;
    }
    
    return snprintf(buffer, buffer_size,
        "{\n"
        "    \"status\": \"%s\",\n"
        "    \"target\": {\"x\": %.2f, \"y\": %.2f, \"z\": %.2f},\n"
        "    \"progress\": %.2f,\n"
        "    \"distance_to_target\": %.2f,\n"
        "    \"estimated_time\": %.2f,\n"
        "    \"path_clear\": %s,\n"
        "    \"obstacle_detected\": %s,\n"
        "    \"obstacle_distance\": %.2f,\n"
        "    \"obstacle_angle\": %.2f,\n"
        "    \"speed_limit\": %.2f,\n"
        "    \"zone_type\": \"%s\"\n"
        "  }",
        status_str,
        navigation->target.x, navigation->target.y, navigation->target.z,
        navigation->progress,
        navigation->distance_to_target,
        navigation->estimated_time,
        navigation->path_clear ? "true" : "false",
        navigation->obstacle_detected ? "true" : "false",
        navigation->obstacle_distance,
        navigation->obstacle_angle,
        navigation->speed_limit,
        zone_str);
}

static int serialize_dock_json(const telemetry_dock_t *dock, char *buffer, size_t buffer_size) {
    const char *status_str = "not_docking";
    switch (dock->status) {
        case DOCK_STATUS_APPROACHING: status_str = "approaching"; break;
        case DOCK_STATUS_ALIGNING: status_str = "aligning"; break;
        case DOCK_STATUS_DOCKED: status_str = "docked"; break;
        case DOCK_STATUS_UNDOCKING: status_str = "undocking"; break;
        case DOCK_STATUS_ERROR: status_str = "error"; break;
        default: status_str = "not_docking"; break;
    }
    
    const char *alignment_str = "not_aligned";
    switch (dock->alignment_status) {
        case ALIGNMENT_ALIGNING: alignment_str = "aligning"; break;
        case ALIGNMENT_ALIGNED: alignment_str = "aligned"; break;
        case ALIGNMENT_ERROR: alignment_str = "error"; break;
        default: alignment_str = "not_aligned"; break;
    }
    
    const char *charging_str = "not_charging";
    switch (dock->charging_status) {
        case CHARGING_CHARGING: charging_str = "charging"; break;
        case CHARGING_FULL: charging_str = "full"; break;
        case CHARGING_ERROR: charging_str = "error"; break;
        default: charging_str = "not_charging"; break;
    }
    
    const char *safety_str = "safe";
    switch (dock->safety_status) {
        case DOCK_SAFETY_WARNING: safety_str = "warning"; break;
        case DOCK_SAFETY_DANGER: safety_str = "danger"; break;
        default: safety_str = "safe"; break;
    }
    
    return snprintf(buffer, buffer_size,
        "{\n"
        "    \"status\": \"%s\",\n"
        "    \"station_id\": \"%s\",\n"
        "    \"target_station_id\": \"%s\",\n"
        "    \"alignment_status\": \"%s\",\n"
        "    \"distance\": %.2f,\n"
        "    \"angle\": %.2f,\n"
        "    \"charging_status\": \"%s\",\n"
        "    \"charging_current\": %.2f,\n"
        "    \"charging_voltage\": %.2f,\n"
        "    \"rfid_tag_id\": \"%s\",\n"
        "    \"rfid_signal_strength\": %d,\n"
        "    \"safety_status\": \"%s\"\n"
        "  }",
        status_str,
        dock->station_id,
        dock->target_station_id,
        alignment_str,
        dock->distance,
        dock->angle,
        charging_str,
        dock->charging_current,
        dock->charging_voltage,
        dock->rfid_tag_id,
        dock->rfid_signal_strength,
        safety_str);
}

static int serialize_safety_json(const telemetry_safety_t *safety, char *buffer, size_t buffer_size) {
    return snprintf(buffer, buffer_size,
        "{\n"
        "    \"estop\": %s,\n"
        "    \"zone_blocked\": %s,\n"
        "    \"interlock_active\": %s,\n"
        "    \"location_safe\": %s,\n"
        "    \"obstacle_detected\": %s,\n"
        "    \"safety_zone_clear\": %s,\n"
        "    \"emergency_stop\": %s\n"
        "  }",
        safety->estop ? "true" : "false",
        safety->zone_blocked ? "true" : "false",
        safety->interlock_active ? "true" : "false",
        safety->location_safe ? "true" : "false",
        safety->obstacle_detected ? "true" : "false",
        safety->safety_zone_clear ? "true" : "false",
        safety->emergency_stop ? "true" : "false");
}

// Forward declarations for RS485 module register serializers
static int serialize_power_module_registers(char *buffer, size_t buffer_size);
static int serialize_safety_module_registers(char *buffer, size_t buffer_size);
static int serialize_motor_module_registers(char *buffer, size_t buffer_size);
static int serialize_dock_module_registers(char *buffer, size_t buffer_size);

static int serialize_status_json(const telemetry_status_t *status, char *buffer, size_t buffer_size) {
    const char *state_str = "idle";
    switch (status->state) {
        case SYSTEM_STATE_MOVE: state_str = "move"; break;
        case SYSTEM_STATE_DOCK: state_str = "dock"; break;
        case SYSTEM_STATE_FAULT: state_str = "fault"; break;
        case SYSTEM_STATE_ESTOP: state_str = "estop"; break;
        default: state_str = "idle"; break;
    }
    
    const char *health_str = "ok";
    (void)health_str; // Suppress unused warning
    switch (status->enc.health) {
        case ENCODER_HEALTH_WARNING: health_str = "warning"; break;
        case ENCODER_HEALTH_ERROR: health_str = "error"; break;
        default: health_str = "ok"; break;
    }
    
    return snprintf(buffer, buffer_size,
        "{\n"
        "    \"state\": \"%s\",\n"
        "    \"pos_mm\": %.2f,\n"
        "    \"vel_mms\": %.2f,\n"
        "    \"acc_mms2\": %.2f,\n"
        "    \"target\": {\"pos_mm\": %.2f, \"vel_mms\": %.2f},\n"
        "    \"safety\": ",
        state_str,
        status->pos_mm,
        status->vel_mms,
        status->acc_mms2,
        status->target.pos_mm,
        status->target.vel_mms);
}

// RS485 Module Register Serializers for Issue #90

/**
 * @brief Serialize Power Module (0x02) registers - DalyBMS + SK60X + INA219
 */
static int serialize_power_module_registers(char *buffer, size_t buffer_size) {
    // Read actual data from Power Module via communication manager
    uint16_t battery_data[11] = {0};  // Battery registers 0x0000-0x000A
    uint16_t charging_data[8] = {0};  // SK60X registers 0x0030-0x0037
    uint16_t power_data[12] = {0};    // INA219 registers 0x0040-0x004B
    
    // Read battery data (best effort)
    comm_manager_modbus_read_holding_registers(0x02, 0x0000, 11, battery_data);
    comm_manager_modbus_read_holding_registers(0x02, 0x0032, 4, &charging_data[2]); // SK60X subset
    comm_manager_modbus_read_holding_registers(0x02, 0x0040, 12, power_data);
    
    return snprintf(buffer, buffer_size,
        "\n      // DalyBMS Status Registers\n"
        "      {\"addr\": \"0x0000\", \"name\": \"Battery_Voltage\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0001\", \"name\": \"Battery_Current\", \"value\": %.1f, \"unit\": \"A\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0002\", \"name\": \"Battery_SOC\", \"value\": %.1f, \"unit\": \"%%\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0003\", \"name\": \"Max_Cell_Voltage\", \"value\": %d, \"unit\": \"mV\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0004\", \"name\": \"Min_Cell_Voltage\", \"value\": %d, \"unit\": \"mV\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0008\", \"name\": \"Temperature\", \"value\": %d, \"unit\": \"°C\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0009\", \"name\": \"Connection_Status\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x000B\", \"name\": \"Charge_MOS\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x000C\", \"name\": \"Discharge_MOS\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0011\", \"name\": \"Charge_Status\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"R\"},\n"
        "      \n"
        "      // SK60X Power Supply Registers\n"
        "      {\"addr\": \"0x0032\", \"name\": \"Output_Voltage\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0033\", \"name\": \"Output_Current\", \"value\": %.1f, \"unit\": \"A\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0034\", \"name\": \"Output_Power\", \"value\": %.1f, \"unit\": \"W\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x003C\", \"name\": \"Output_ON_OFF\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"RW\"},\n"
        "      \n"
        "      // INA219 Multi-rail Monitoring\n"
        "      {\"addr\": \"0x0040\", \"name\": \"12V_Output_Voltage\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0041\", \"name\": \"12V_Output_Current\", \"value\": %.1f, \"unit\": \"A\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0043\", \"name\": \"5V_Output_Voltage\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0044\", \"name\": \"5V_Output_Current\", \"value\": %.1f, \"unit\": \"A\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0046\", \"name\": \"3V3_Output_Voltage\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x0047\", \"name\": \"3V3_Output_Current\", \"value\": %.1f, \"unit\": \"A\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      \n"
        "      // Relay Controls\n"
        "      {\"addr\": \"0x0049\", \"name\": \"12V_Relay\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x004A\", \"name\": \"5V_Relay\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x004B\", \"name\": \"3V3_Relay\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"RW\"}",
        
        // DalyBMS values
        battery_data[0] * 0.1f,  // Battery_Voltage
        (int16_t)battery_data[1] * 0.1f,  // Battery_Current (signed)
        battery_data[2] * 0.1f,  // Battery_SOC
        battery_data[3],         // Max_Cell_Voltage
        battery_data[4],         // Min_Cell_Voltage
        (int16_t)battery_data[8], // Temperature (signed)
        battery_data[9],         // Connection_Status
        battery_data[10] & 0x01, // Charge_MOS (bit 0)
        (battery_data[10] >> 1) & 0x01, // Discharge_MOS (bit 1)
        battery_data[10] >> 2,   // Charge_Status
        
        // SK60X values
        charging_data[2] * 0.1f, // Output_Voltage
        charging_data[3] * 0.1f, // Output_Current
        charging_data[4] * 0.1f, // Output_Power
        charging_data[7] & 0x01, // Output_ON_OFF
        
        // INA219 values
        power_data[0] * 0.1f,    // 12V_Output_Voltage
        power_data[1] * 0.1f,    // 12V_Output_Current
        power_data[3] * 0.1f,    // 5V_Output_Voltage
        power_data[4] * 0.1f,    // 5V_Output_Current
        power_data[6] * 0.1f,    // 3V3_Output_Voltage
        power_data[7] * 0.1f,    // 3V3_Output_Current
        
        // Relay states
        power_data[9],           // 12V_Relay
        power_data[10],          // 5V_Relay
        power_data[11]           // 3V3_Relay
    );
}

/**
 * @brief Serialize Safety Module (0x03) registers - Proximity Sensors + Relays
 */
static int serialize_safety_module_registers(char *buffer, size_t buffer_size) {
    // Read actual data from Safety Module
    uint16_t safety_data[8] = {0};   // Safety status registers 0x0000-0x0007
    uint16_t analog_data[4] = {0};   // Analog inputs 0x0010-0x0013
    uint16_t digital_data[1] = {0};  // Digital inputs 0x0020
    uint16_t relay_data[1] = {0};    // Relay control 0x0030
    
    // Read safety data (best effort)
    comm_manager_modbus_read_holding_registers(0x03, 0x0000, 8, safety_data);
    comm_manager_modbus_read_holding_registers(0x03, 0x0010, 4, analog_data);
    comm_manager_modbus_read_holding_registers(0x03, 0x0020, 1, digital_data);
    comm_manager_modbus_read_holding_registers(0x03, 0x0030, 1, relay_data);
    
    return snprintf(buffer, buffer_size,
        "\n      {\"addr\": \"0x0000\", \"name\": \"Safety_Status\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0001\", \"name\": \"Emergency_Stop\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0002\", \"name\": \"Safety_Zone\", \"value\": %d, \"unit\": \"enum\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0003\", \"name\": \"Proximity_Alert\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0010\", \"name\": \"Analog_Input_1\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0011\", \"name\": \"Analog_Input_2\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0012\", \"name\": \"Analog_Input_3\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0013\", \"name\": \"Analog_Input_4\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0020\", \"name\": \"Digital_Inputs\", \"value\": %d, \"unit\": \"bitmask\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0030\", \"name\": \"Relay_Control\", \"value\": %d, \"unit\": \"bitmask\", \"mode\": \"RW\"}",
        
        safety_data[0],          // Safety_Status
        safety_data[1],          // Emergency_Stop
        safety_data[2],          // Safety_Zone
        safety_data[3],          // Proximity_Alert
        analog_data[0] * 0.1f,   // Analog_Input_1 (scaled to V)
        analog_data[1] * 0.1f,   // Analog_Input_2
        analog_data[2] * 0.1f,   // Analog_Input_3
        analog_data[3] * 0.1f,   // Analog_Input_4
        digital_data[0],         // Digital_Inputs
        relay_data[0]            // Relay_Control
    );
}

/**
 * @brief Serialize Travel Motor Module (0x04) registers
 */
static int serialize_motor_module_registers(char *buffer, size_t buffer_size) {
    // Read actual data from Travel Motor Module
    uint16_t control_data[16] = {0}; // Control registers 0x0000-0x000F
    uint16_t status_data[16] = {0};  // Status registers 0x0010-0x001F
    
    // Read motor data (best effort)
    comm_manager_modbus_read_holding_registers(0x04, 0x0000, 16, control_data);
    comm_manager_modbus_read_holding_registers(0x04, 0x0010, 16, status_data);
    
    return snprintf(buffer, buffer_size,
        "\n      {\"addr\": \"0x0000\", \"name\": \"Motor_Enable\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x0001\", \"name\": \"Operation_Mode\", \"value\": %d, \"unit\": \"enum\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x0002\", \"name\": \"Speed_Target\", \"value\": %d, \"unit\": \"rpm\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x0003\", \"name\": \"Speed_Actual\", \"value\": %d, \"unit\": \"rpm\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0004\", \"name\": \"Position_Target\", \"value\": %d, \"unit\": \"mm\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x0005\", \"name\": \"Position_Actual\", \"value\": %d, \"unit\": \"mm\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x0006\", \"name\": \"Direction\", \"value\": %d, \"unit\": \"enum\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x000B\", \"name\": \"Temperature\", \"value\": %d, \"unit\": \"°C\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x000C\", \"name\": \"Voltage\", \"value\": %.1f, \"unit\": \"V\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x000D\", \"name\": \"Current\", \"value\": %.1f, \"unit\": \"A\", \"mode\": \"R\", \"scaling\": 0.1},\n"
        "      {\"addr\": \"0x000E\", \"name\": \"Fault_Status\", \"value\": %d, \"unit\": \"bitmask\", \"mode\": \"R\"}",
        
        control_data[0],         // Motor_Enable
        control_data[1],         // Operation_Mode
        control_data[2],         // Speed_Target
        control_data[3],         // Speed_Actual
        control_data[4],         // Position_Target
        control_data[5],         // Position_Actual
        control_data[6],         // Direction
        (int16_t)control_data[11], // Temperature (signed)
        control_data[12] * 0.1f, // Voltage
        control_data[13] * 0.1f, // Current
        control_data[14]         // Fault_Status
    );
}

/**
 * @brief Serialize Dock & Location Module (0x05) registers
 */
static int serialize_dock_module_registers(char *buffer, size_t buffer_size) {
    // Read actual data from Dock Module
    uint16_t position_data[8] = {0}; // Position registers 0x7000-0x7007
    uint16_t control_data[2] = {0};  // Control registers 0x8000-0x8001
    uint16_t fault_data[1] = {0};    // Fault register 0x9000
    
    // Read dock data (best effort)
    comm_manager_modbus_read_holding_registers(0x05, 0x7000, 8, position_data);
    comm_manager_modbus_read_holding_registers(0x05, 0x8000, 2, control_data);
    comm_manager_modbus_read_holding_registers(0x05, 0x9000, 1, fault_data);
    
    return snprintf(buffer, buffer_size,
        "\n      {\"addr\": \"0x7000\", \"name\": \"Position_Target\", \"value\": %d, \"unit\": \"mm\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x7001\", \"name\": \"Current_Position\", \"value\": %d, \"unit\": \"mm\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x7002\", \"name\": \"Approach_Speed\", \"value\": %d, \"unit\": \"mm/s\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x7005\", \"name\": \"Dock_Status\", \"value\": %d, \"unit\": \"enum\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x7006\", \"name\": \"Distance_to_Dock\", \"value\": %d, \"unit\": \"mm\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x7007\", \"name\": \"Alignment_Angle\", \"value\": %d, \"unit\": \"deg*10\", \"mode\": \"R\"},\n"
        "      {\"addr\": \"0x8000\", \"name\": \"Dock_Enable\", \"value\": %d, \"unit\": \"bool\", \"mode\": \"RW\"},\n"
        "      {\"addr\": \"0x8001\", \"name\": \"Start_Docking\", \"value\": %d, \"unit\": \"cmd\", \"mode\": \"W\"},\n"
        "      {\"addr\": \"0x9000\", \"name\": \"Fault_Status\", \"value\": %d, \"unit\": \"bitmask\", \"mode\": \"R\"}",
        
        position_data[0],        // Position_Target
        position_data[1],        // Current_Position
        position_data[2],        // Approach_Speed
        position_data[5],        // Dock_Status
        position_data[6],        // Distance_to_Dock
        position_data[7],        // Alignment_Angle
        control_data[0],         // Dock_Enable
        control_data[1],         // Start_Docking
        fault_data[0]            // Fault_Status
    );
}
