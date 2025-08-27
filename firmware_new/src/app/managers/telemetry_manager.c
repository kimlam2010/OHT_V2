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
    
    // Serialize status
    result = serialize_status_json(&data->status, json_buffer + written, buffer_size - written);
    if (result < 0) {
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
    if (result < 0) {
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
    if (result < 0) {
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
    if (result < 0) {
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
    
    // Simulate system metrics (in real implementation, these would come from HAL)
    data->cpu_usage = 25.0f;  // 25% CPU usage
    data->memory_usage = 45.0f; // 45% memory usage
    data->temperature = 35.0f;  // 35°C temperature
    
    // Connection status (in real implementation, these would come from HAL)
    data->center_connected = true;
    data->rs485_connected = true;
}

static void collect_location_data(telemetry_data_t *data) {
    // In real implementation, this would come from location system
    // For now, simulate some basic data
    static float position_x = 1000.0f;
    static float position_y = 2000.0f;
    static float velocity_x = 100.0f;
    
    data->location.position.x = position_x;
    data->location.position.y = position_y;
    data->location.position.z = 0.0f;
    
    data->location.orientation.pitch = 0.0f;
    data->location.orientation.roll = 0.0f;
    data->location.orientation.yaw = 45.0f;
    
    data->location.velocity.x = velocity_x;
    data->location.velocity.y = 0.0f;
    data->location.velocity.z = 0.0f;
    
    data->location.acceleration.x = 0.0f;
    data->location.acceleration.y = 0.0f;
    data->location.acceleration.z = 0.0f;
    
    data->location.accuracy = 10.0f;
    
    // Update position for next cycle
    position_x += velocity_x * 0.1f; // 100ms update
}

static void collect_navigation_data(telemetry_data_t *data) {
    // In real implementation, this would come from navigation system
    data->navigation.target.x = 5000.0f;
    data->navigation.target.y = 3000.0f;
    data->navigation.target.z = 0.0f;
    
    // Calculate progress based on current position
    float dx = data->navigation.target.x - data->location.position.x;
    float dy = data->navigation.target.y - data->location.position.y;
    data->navigation.distance_to_target = sqrtf(dx*dx + dy*dy);
    
    // Calculate progress percentage
    float total_distance = sqrtf(4000.0f*4000.0f + 1000.0f*1000.0f); // Distance from start to target
    data->navigation.progress = ((total_distance - data->navigation.distance_to_target) / total_distance) * 100.0f;
    if (data->navigation.progress < 0.0f) data->navigation.progress = 0.0f;
    if (data->navigation.progress > 100.0f) data->navigation.progress = 100.0f;
    
    data->navigation.estimated_time = data->navigation.distance_to_target / 100.0f; // Assuming 100mm/s
    data->navigation.path_clear = true;
    data->navigation.obstacle_detected = false;
    data->navigation.speed_limit = 200.0f; // 200mm/s speed limit
}

static void collect_dock_data(telemetry_data_t *data) {
    // In real implementation, this would come from dock system
    strcpy(data->dock.station_id, "STATION_001");
    strcpy(data->dock.target_station_id, "STATION_001");
    data->dock.distance = 500.0f;
    data->dock.angle = 5.0f;
    data->dock.charging_current = 0.0f;
    data->dock.charging_voltage = 0.0f;
    strcpy(data->dock.rfid_tag_id, "TAG_001");
    data->dock.rfid_signal_strength = -45;
}

static void collect_safety_data(telemetry_data_t *data) {
    // In real implementation, this would come from safety system
    data->status.safety.estop = false;
    data->status.safety.zone_blocked = false;
    data->status.safety.interlock_active = false;
    data->status.safety.location_safe = true;
    data->status.safety.obstacle_detected = false;
    data->status.safety.safety_zone_clear = true;
    data->status.safety.emergency_stop = false;
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
