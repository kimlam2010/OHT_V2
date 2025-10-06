#pragma once
#include "api_manager.h"

// CRITICAL: Module Data Access API Structures - Issue #140
typedef struct {
    float voltage;
    float current;
    float power;
    float temperature;
    float efficiency;
    float load_percentage;
} api_module_telemetry_data_t;

typedef struct {
    int module_id;
    char module_name[32];
    float voltage;
    float current;
    float power;
    float temperature;
    float efficiency;
    float load_percentage;
    unsigned long timestamp;
    int data_freshness_ms;
} api_module_telemetry_t;

// Enhanced telemetry field with value ranges (Issue #143)
typedef struct {
    float value;
    float min_value;
    float max_value;
    char unit[16];
    char description[64];
} telemetry_field_t;

// Enhanced module telemetry with ranges (Issue #143)
typedef struct {
    int module_id;
    char module_name[32];
    telemetry_field_t voltage;
    telemetry_field_t current;
    telemetry_field_t power;
    telemetry_field_t temperature;
    telemetry_field_t efficiency;
    telemetry_field_t load_percentage;
    unsigned long timestamp;
    unsigned int data_freshness_ms;
} api_module_telemetry_with_range_t;

typedef struct {
    bool emergency_stop_enabled;
    int response_time_ms;
    bool auto_recovery;
} api_module_config_data_t;

typedef struct {
    int module_id;
    char module_name[32];
    bool emergency_stop_enabled;
    int response_time_ms;
    bool auto_recovery;
    char config_version[16];
    unsigned long last_updated;
} api_module_config_t;

typedef struct {
    char command[32];
    char parameters[256];
    char reason[128];
} api_module_command_t;

typedef struct {
    char health_status[32];
    float health_score;
    unsigned long uptime_seconds;
    int error_count;
    int warning_count;
} api_module_health_t;

int api_register_minimal_endpoints(void);

int api_handle_system_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_safety_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_safety_estop(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_modules_list(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_module_status_by_id(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_system_state(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_control_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
// New: Module Manager extended endpoints
int api_handle_modules_stats(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_modules_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_modules_config_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// Motion API (EXEC PLAN Gate E)
int api_handle_motion_segment_start(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_motion_segment_stop(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_motion_state(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// LiDAR API (CRITICAL - Production Integration)
int api_register_lidar_endpoints(void);
int api_handle_lidar_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_scan_data(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_start_scanning(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_stop_scanning(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_config_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_config_set(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_safety_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_health_check(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
// Full-frame LiDAR data (no point limit)
int api_handle_lidar_scan_frame_full(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
// Angle-filtered frame via GET query (?min_deg=&max_deg=&normalize=1&limit=)
int api_handle_lidar_scan_frame(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
// 360-bin frame (0..359 degrees) via GET query (?reducer=max&min_q=0&max_range=0)
int api_handle_lidar_scan_frame_360(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// CRITICAL ENDPOINTS - Issue #112 Fix
int api_handle_health_check(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_rs485_modules(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// CRITICAL ENDPOINTS - Phase 1 Implementation
int api_handle_robot_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_robot_command(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_battery_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_temperature_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// STATE MACHINE CONTROL APIs - NEW IMPLEMENTATION
int api_handle_state_move(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_state_pause(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_state_resume(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_state_stop(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_state_dock(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_state_emergency(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_state_reset(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// CONFIGURATION APIs - NEW IMPLEMENTATION
int api_handle_config_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_config_set(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_config_timeouts(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// STATISTICS APIs - NEW IMPLEMENTATION  
int api_handle_state_statistics(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// CRITICAL: Module Data Access APIs - Issue #140
int api_handle_module_telemetry(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_module_config_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_module_config_set(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_module_command(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_module_history(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_module_health(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// Deprecated: WebSocket helper functions removed (Firmware is HTTP-only on port 8080)
const char* get_module_name_by_id(int module_id);
int get_module_telemetry_data(int module_id, api_module_telemetry_t *telemetry);
int get_module_telemetry_data_with_ranges(int module_id, api_module_telemetry_with_range_t *telemetry);
int get_module_config_data(int module_id, api_module_config_t *config);

// Network Management API Handlers
int api_handle_network_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_network_wifi_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_network_wifi_connect(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_network_wifi_disconnect(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_network_performance(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_network_health(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
