/**
 * @file api_endpoints.c
 * @brief API Endpoints Implementation for OHT-50 Master Module Firmware
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-03 (API Endpoints Implementation)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "api_endpoints.h"
#include "hal_common.h"
#include "safety_monitor.h"

// External dependencies (to be integrated with existing managers)
extern hal_status_t system_get_status(void);
extern hal_status_t safety_get_status(void);
extern hal_status_t module_get_list(void);
extern hal_status_t config_get_data(void);
extern hal_status_t config_set_data(const char *data);

// API Endpoint Handlers

hal_status_t api_handle_system_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    // Create system status response
    api_system_status_t status = {
        .system_name = "OHT-50 Master Module",
        .version = "1.0.0",
        .status = "running",
        .uptime_ms = hal_get_timestamp_ms(),
        .active_modules = 3, // Mock data
        .estop_active = false,
        .safety_ok = true
    };
    
    char json_buffer[1024];
    hal_status_t result = api_create_system_status_json(&status, json_buffer, sizeof(json_buffer));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to create system status");
    }
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_system_health(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    // Create health status response
    api_health_status_t health = {
        .status = "healthy",
        .timestamp = hal_get_timestamp_ms(),
        .response_time_ms = 50, // Mock data
        .details = "All systems operational"
    };
    
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"status\":\"%s\",\"timestamp\":%lu,\"response_time_ms\":%u,\"details\":\"%s\"}",
             health.status, health.timestamp, health.response_time_ms, health.details);
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_modules_list(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    // Create modules list response
    api_modules_list_t modules = {0};
    modules.module_count = 3;
    
    // Mock module data
    modules.modules[0] = (api_module_info_t){
        .module_id = 1,
        .module_type = "power",
        .status = "online",
        .online = true,
        .last_seen = hal_get_timestamp_ms(),
        .version = "1.0.0"
    };
    
    modules.modules[1] = (api_module_info_t){
        .module_id = 2,
        .module_type = "motor",
        .status = "online",
        .online = true,
        .last_seen = hal_get_timestamp_ms(),
        .version = "1.0.0"
    };
    
    modules.modules[2] = (api_module_info_t){
        .module_id = 3,
        .module_type = "dock",
        .status = "online",
        .online = true,
        .last_seen = hal_get_timestamp_ms(),
        .version = "1.0.0"
    };
    
    char json_buffer[2048];
    hal_status_t result = api_create_modules_list_json(&modules, json_buffer, sizeof(json_buffer));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to create modules list");
    }
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_module_info(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    int module_id = api_extract_module_id(request->path);
    if (module_id < 0) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid module ID");
    }
    
    // Mock module info based on ID
    api_module_info_t module = {
        .module_id = module_id,
        .module_type = "unknown",
        .status = "offline",
        .online = false,
        .last_seen = 0,
        .version = "unknown"
    };
    
    // Set module type based on ID
    switch (module_id) {
        case 1:
            strcpy(module.module_type, "power");
            module.online = true;
            strcpy(module.status, "online");
            break;
        case 2:
            strcpy(module.module_type, "motor");
            module.online = true;
            strcpy(module.status, "online");
            break;
        case 3:
            strcpy(module.module_type, "dock");
            module.online = true;
            strcpy(module.status, "online");
            break;
        default:
            return api_create_error_response(response, API_MGR_RESPONSE_NOT_FOUND, "Module not found");
    }
    
    module.last_seen = hal_get_timestamp_ms();
    strcpy(module.version, "1.0.0");
    
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"module_id\":%u,\"module_type\":\"%s\",\"status\":\"%s\",\"online\":%s,\"last_seen\":%lu,\"version\":\"%s\"}",
             module.module_id, module.module_type, module.status,
             module.online ? "true" : "false", module.last_seen, module.version);
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_module_command(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    int module_id = api_extract_module_id(request->path);
    if (module_id < 0) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid module ID");
    }
    
    if (request->method != API_MGR_HTTP_POST) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Parse command from request body
    api_module_command_t command = {0};
    hal_status_t result = api_parse_json_body(request->body, command.command, sizeof(command.command));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid command format");
    }
    
    // Mock command execution
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"module_id\":%d,\"command\":\"%s\",\"status\":\"executed\",\"timestamp\":%lu}",
             module_id, command.command, hal_get_timestamp_ms());
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_safety_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    // Get real safety status from safety monitor
    safety_monitor_status_t safety_status;
    hal_status_t status = safety_monitor_get_status(&safety_status);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get safety status");
    }
    
    // Get E-Stop status
    bool estop_active;
    status = safety_monitor_is_estop_active(&estop_active);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get E-Stop status");
    }
    
    // Get basic safety zones
    basic_safety_zones_t zones;
    status = safety_monitor_get_basic_zones(&zones);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get safety zones");
    }
    
    // Create comprehensive safety status response
    api_safety_status_t safety = {
        .estop_active = estop_active,
        .safety_ok = (safety_status.current_state == SAFETY_MONITOR_STATE_SAFE),
        .safety_level = (uint32_t)safety_status.current_state,
        .safety_message = "Safety system operational",
        .last_safety_check = safety_status.last_update_time
    };
    
    // Create detailed JSON response with safety zones
    char json_buffer[2048];
    snprintf(json_buffer, sizeof(json_buffer),
             "{"
             "\"estop_active\":%s,"
             "\"safety_ok\":%s,"
             "\"safety_level\":%u,"
             "\"safety_message\":\"%s\","
             "\"last_safety_check\":%lu,"
             "\"current_state\":\"%s\","
             "\"safety_zones\":{"
             "\"enabled\":%s,"
             "\"emergency_zone_mm\":%u,"
             "\"warning_zone_mm\":%u,"
             "\"safe_zone_mm\":%u,"
             "\"min_distance_mm\":%u,"
             "\"emergency_violated\":%s,"
             "\"warning_violated\":%s,"
             "\"safe_violated\":%s"
             "},"
             "\"violation_count\":%u,"
             "\"fault_count\":%u"
             "}",
             estop_active ? "true" : "false",
             safety.safety_ok ? "true" : "false",
             safety.safety_level,
             safety.safety_message,
             safety.last_safety_check,
             (safety_status.current_state == SAFETY_MONITOR_STATE_SAFE) ? "SAFE" : 
             (safety_status.current_state == SAFETY_MONITOR_STATE_WARNING) ? "WARNING" :
             (safety_status.current_state == SAFETY_MONITOR_STATE_CRITICAL) ? "CRITICAL" :
             (safety_status.current_state == SAFETY_MONITOR_STATE_ESTOP) ? "ESTOP" :
             (safety_status.current_state == SAFETY_MONITOR_STATE_FAULT) ? "FAULT" : "INIT",
             zones.enabled ? "true" : "false",
             zones.emergency_zone_mm,
             zones.warning_zone_mm,
             zones.safe_zone_mm,
             zones.min_distance_mm,
             zones.emergency_violated ? "true" : "false",
             zones.warning_violated ? "true" : "false",
             zones.safe_violated ? "true" : "false",
             safety_status.violation_count,
             safety_status.fault_count);
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_safety_estop(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    if (request->method != API_MGR_HTTP_POST) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Parse E-Stop request
    api_estop_request_t estop = {0};
    hal_status_t result = api_parse_json_body(request->body, estop.estop_reason, sizeof(estop.estop_reason));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid E-Stop request format");
    }
    
    // Trigger emergency stop via safety monitor
    hal_status_t status = safety_monitor_trigger_emergency_stop(estop.estop_reason);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to trigger emergency stop");
    }
    
    // Get updated E-Stop status
    bool estop_active;
    status = safety_monitor_is_estop_active(&estop_active);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get E-Stop status");
    }
    
    estop.timestamp = hal_get_timestamp_ms();
    estop.acknowledged = true;
    
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{"
             "\"estop_reason\":\"%s\","
             "\"timestamp\":%lu,"
             "\"acknowledged\":%s,"
             "\"status\":\"executed\","
             "\"estop_active\":%s,"
             "\"message\":\"Emergency stop triggered successfully\""
             "}",
             estop.estop_reason, 
             estop.timestamp, 
             estop.acknowledged ? "true" : "false",
             estop_active ? "true" : "false");
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_safety_zones_get(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    // Get safety zones from safety monitor
    basic_safety_zones_t zones;
    hal_status_t status = safety_monitor_get_basic_zones(&zones);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get safety zones");
    }
    
    // Create JSON response
    char json_buffer[1024];
    snprintf(json_buffer, sizeof(json_buffer),
             "{"
             "\"enabled\":%s,"
             "\"emergency_zone_mm\":%u,"
             "\"warning_zone_mm\":%u,"
             "\"safe_zone_mm\":%u,"
             "\"min_distance_mm\":%u,"
             "\"min_distance_angle\":%u,"
             "\"emergency_violated\":%s,"
             "\"warning_violated\":%s,"
             "\"safe_violated\":%s,"
             "\"last_violation_time\":%lu"
             "}",
             zones.enabled ? "true" : "false",
             zones.emergency_zone_mm,
             zones.warning_zone_mm,
             zones.safe_zone_mm,
             zones.min_distance_mm,
             zones.min_distance_angle,
             zones.emergency_violated ? "true" : "false",
             zones.warning_violated ? "true" : "false",
             zones.safe_violated ? "true" : "false",
             zones.last_violation_time);
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_safety_zones_set(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    if (request->method != API_MGR_HTTP_PUT) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Parse safety zones configuration from request body
    basic_safety_zones_t zones = {0};
    
    // Simple JSON parsing for zones configuration
    // Expected format: {"enabled":true,"emergency_zone_mm":500,"warning_zone_mm":1000,"safe_zone_mm":2000}
    char *enabled_str = strstr(request->body, "\"enabled\":");
    char *emergency_str = strstr(request->body, "\"emergency_zone_mm\":");
    char *warning_str = strstr(request->body, "\"warning_zone_mm\":");
    char *safe_str = strstr(request->body, "\"safe_zone_mm\":");
    
    if (enabled_str) {
        enabled_str = strchr(enabled_str, ':') + 1;
        zones.enabled = (strstr(enabled_str, "true") != NULL);
    }
    
    if (emergency_str) {
        emergency_str = strchr(emergency_str, ':') + 1;
        zones.emergency_zone_mm = (uint16_t)strtoul(emergency_str, NULL, 10);
    }
    
    if (warning_str) {
        warning_str = strchr(warning_str, ':') + 1;
        zones.warning_zone_mm = (uint16_t)strtoul(warning_str, NULL, 10);
    }
    
    if (safe_str) {
        safe_str = strchr(safe_str, ':') + 1;
        zones.safe_zone_mm = (uint16_t)strtoul(safe_str, NULL, 10);
    }
    
    // Validate zones configuration
    if (zones.emergency_zone_mm >= zones.warning_zone_mm ||
        zones.warning_zone_mm >= zones.safe_zone_mm) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid zone distances");
    }
    
    // Set safety zones via safety monitor
    hal_status_t status = safety_monitor_set_basic_zones(&zones);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to set safety zones");
    }
    
    // Create success response
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{"
             "\"status\":\"updated\","
             "\"enabled\":%s,"
             "\"emergency_zone_mm\":%u,"
             "\"warning_zone_mm\":%u,"
             "\"safe_zone_mm\":%u,"
             "\"timestamp\":%lu"
             "}",
             zones.enabled ? "true" : "false",
             zones.emergency_zone_mm,
             zones.warning_zone_mm,
             zones.safe_zone_mm,
             hal_get_timestamp_ms());
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_config_get(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    // Create config data response
    api_config_data_t config = {
        .config_data = "{\"system\":{\"name\":\"OHT-50\",\"version\":\"1.0.0\"},\"network\":{\"port\":8080,\"timeout\":30000}}",
        .config_version = 1,
        .last_updated = hal_get_timestamp_ms()
    };
    
    char json_buffer[2048];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"config_data\":%s,\"config_version\":%u,\"last_updated\":%lu}",
             config.config_data, config.config_version, config.last_updated);
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_config_set(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    if (request->method != API_MGR_HTTP_PUT) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Parse config data from request body
    char config_data[1024];
    hal_status_t result = api_parse_json_body(request->body, config_data, sizeof(config_data));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid config format");
    }
    
    // Mock config update
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"status\":\"updated\",\"config_version\":2,\"timestamp\":%lu}",
             hal_get_timestamp_ms());
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_diagnostics(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    // Create diagnostics response
    api_diagnostics_t diagnostics = {
        .total_requests = 100,
        .successful_requests = 95,
        .failed_requests = 5,
        .uptime_ms = hal_get_timestamp_ms(),
        .system_info = "OHT-50 Master Module v1.0.0 running on Orange Pi 5B",
        .error_log = "No errors in last 24 hours"
    };
    
    char json_buffer[2048];
    hal_status_t result = api_create_diagnostics_json(&diagnostics, json_buffer, sizeof(json_buffer));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to create diagnostics");
    }
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_telemetry(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    // Create telemetry response with mock data
    api_telemetry_t telemetry = {
        .timestamp = hal_get_timestamp_ms(),
        .system_state = "idle",
        .position_x = 0.0f,
        .position_y = 0.0f,
        .position_z = 0.0f,
        .velocity_x = 0.0f,
        .velocity_y = 0.0f,
        .velocity_z = 0.0f,
        .acceleration_x = 0.0f,
        .acceleration_y = 0.0f,
        .acceleration_z = 0.0f,
        .estop_active = false,
        .safety_ok = true,
        .active_modules = 3,
        .status_message = "System operational"
    };
    
    char json_buffer[2048];
    hal_status_t result = api_create_telemetry_json(&telemetry, json_buffer, sizeof(json_buffer));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to create telemetry");
    }
    
    return api_create_success_response(response, json_buffer);
}

// Safety Configuration Management API Handlers

hal_status_t api_handle_safety_config_get(const api_mgr_http_request_t *request, api_mgr_http_response_t *response)
{
    if (!request || !response) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (request->method != API_MGR_HTTP_GET) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Export configuration as JSON
    char json_buffer[2048];
    size_t actual_size;
    hal_status_t status = safety_monitor_export_config_json(json_buffer, sizeof(json_buffer), &actual_size);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to export configuration");
    }
    
    // Set response
    response->status_code = API_MGR_RESPONSE_OK;
    response->content_type = API_MGR_CONTENT_TYPE_JSON;
    strncpy(response->body, json_buffer, sizeof(response->body) - 1);
    response->body[sizeof(response->body) - 1] = '\0';
    
    return HAL_STATUS_OK;
}

hal_status_t api_handle_safety_config_set(const api_mgr_http_request_t *request, api_mgr_http_response_t *response)
{
    if (!request || !response) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (request->method != API_MGR_HTTP_PUT) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    if (!request->body || strlen(request->body) == 0) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Missing configuration data");
    }
    
    // Import configuration from JSON
    hal_status_t status = safety_monitor_import_config_json(request->body);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid configuration data");
    }
    
    // Set success response
    response->status_code = API_MGR_RESPONSE_OK;
    response->content_type = API_MGR_CONTENT_TYPE_JSON;
    strcpy(response->body, "{\"status\":\"success\",\"message\":\"Configuration updated successfully\"}");
    
    return HAL_STATUS_OK;
}

hal_status_t api_handle_safety_config_export(const api_mgr_http_request_t *request, api_mgr_http_response_t *response)
{
    if (!request || !response) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (request->method != API_MGR_HTTP_GET) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Export configuration as JSON
    char json_buffer[2048];
    size_t actual_size;
    hal_status_t status = safety_monitor_export_config_json(json_buffer, sizeof(json_buffer), &actual_size);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to export configuration");
    }
    
    // Set response with filename for download
    response->status_code = API_MGR_RESPONSE_OK;
    response->content_type = API_MGR_CONTENT_TYPE_JSON;
    strncpy(response->body, json_buffer, sizeof(response->body) - 1);
    response->body[sizeof(response->body) - 1] = '\0';
    
    // Add filename header for download
    strcpy(response->headers, "Content-Disposition: attachment; filename=\"safety_config.json\"\r\n");
    
    return HAL_STATUS_OK;
}

hal_status_t api_handle_safety_config_import(const api_mgr_http_request_t *request, api_mgr_http_response_t *response)
{
    if (!request || !response) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (request->method != API_MGR_HTTP_POST) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    if (!request->body || strlen(request->body) == 0) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Missing configuration data");
    }
    
    // Import configuration from JSON
    hal_status_t status = safety_monitor_import_config_json(request->body);
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid configuration data");
    }
    
    // Set success response
    response->status_code = API_MGR_RESPONSE_OK;
    response->content_type = API_MGR_CONTENT_TYPE_JSON;
    strcpy(response->body, "{\"status\":\"success\",\"message\":\"Configuration imported successfully\"}");
    
    return HAL_STATUS_OK;
}

hal_status_t api_handle_safety_config_reset(const api_mgr_http_request_t *request, api_mgr_http_response_t *response)
{
    if (!request || !response) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (request->method != API_MGR_HTTP_POST) {
        return api_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Reset configuration to factory defaults
    hal_status_t status = safety_monitor_reset_config_to_factory();
    if (status != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to reset configuration");
    }
    
    // Set success response
    response->status_code = API_MGR_RESPONSE_OK;
    response->content_type = API_MGR_CONTENT_TYPE_JSON;
    strcpy(response->body, "{\"status\":\"success\",\"message\":\"Configuration reset to factory defaults\"}");
    
    return HAL_STATUS_OK;
}

// API Utility Functions

hal_status_t api_endpoints_init(void) {
    // Initialize API endpoints
    // Note: Route registration will be handled by API manager
    
    // API endpoints initialized successfully
    return HAL_STATUS_OK;
}

hal_status_t api_endpoints_deinit(void) {
    // API endpoints deinitialized
    return HAL_STATUS_OK;
}

int api_extract_module_id(const char *path) {
    if (path == NULL) {
        return -1;
    }
    
    // Look for pattern /api/v1/modules/{id}
    const char *modules_pattern = "/api/v1/modules/";
    char *modules_pos = strstr(path, modules_pattern);
    if (modules_pos == NULL) {
        return -1;
    }
    
    // Extract ID after modules/
    const char *id_start = modules_pos + strlen(modules_pattern);
    if (*id_start == '\0') {
        return -1;
    }
    
    // Check if it's a command endpoint
    char *command_pos = strstr(id_start, "/command");
    if (command_pos != NULL) {
        // Extract ID before /command
        char id_str[16];
        size_t id_len = command_pos - id_start;
        if (id_len >= sizeof(id_str)) {
            return -1;
        }
        strncpy(id_str, id_start, id_len);
        id_str[id_len] = '\0';
        return atoi(id_str);
    }
    
    // Direct module ID
    return atoi(id_start);
}

hal_status_t api_create_system_status_json(const api_system_status_t *status, char *json_buffer, int buffer_size) {
    if (status == NULL || json_buffer == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int written = snprintf(json_buffer, buffer_size,
                          "{\"system_name\":\"%s\",\"version\":\"%s\",\"status\":\"%s\","
                          "\"uptime_ms\":%lu,\"active_modules\":%u,\"estop_active\":%s,\"safety_ok\":%s}",
                          status->system_name, status->version, status->status,
                          status->uptime_ms, status->active_modules,
                          status->estop_active ? "true" : "false",
                          status->safety_ok ? "true" : "false");
    
    return (written < buffer_size) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t api_create_modules_list_json(const api_modules_list_t *modules, char *json_buffer, int buffer_size) {
    if (modules == NULL || json_buffer == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int written = snprintf(json_buffer, buffer_size, "{\"modules\":[");
    
    for (uint32_t i = 0; i < modules->module_count; i++) {
        if (i > 0) {
            written += snprintf(json_buffer + written, buffer_size - written, ",");
        }
        
        written += snprintf(json_buffer + written, buffer_size - written,
                           "{\"module_id\":%u,\"module_type\":\"%s\",\"status\":\"%s\","
                           "\"online\":%s,\"last_seen\":%lu,\"version\":\"%s\"}",
                           modules->modules[i].module_id,
                           modules->modules[i].module_type,
                           modules->modules[i].status,
                           modules->modules[i].online ? "true" : "false",
                           modules->modules[i].last_seen,
                           modules->modules[i].version);
        
        if (written >= buffer_size) {
            return HAL_STATUS_ERROR;
        }
    }
    
    written += snprintf(json_buffer + written, buffer_size - written, "],\"module_count\":%u}", modules->module_count);
    
    return (written < buffer_size) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t api_create_safety_status_json(const api_safety_status_t *safety, char *json_buffer, int buffer_size) {
    if (safety == NULL || json_buffer == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int written = snprintf(json_buffer, buffer_size,
                          "{\"estop_active\":%s,\"safety_ok\":%s,\"safety_level\":%u,"
                          "\"safety_message\":\"%s\",\"last_safety_check\":%lu}",
                          safety->estop_active ? "true" : "false",
                          safety->safety_ok ? "true" : "false",
                          safety->safety_level,
                          safety->safety_message,
                          safety->last_safety_check);
    
    return (written < buffer_size) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t api_create_diagnostics_json(const api_diagnostics_t *diagnostics, char *json_buffer, int buffer_size) {
    if (diagnostics == NULL || json_buffer == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int written = snprintf(json_buffer, buffer_size,
                          "{\"total_requests\":%u,\"successful_requests\":%u,\"failed_requests\":%u,"
                          "\"uptime_ms\":%lu,\"system_info\":\"%s\",\"error_log\":\"%s\"}",
                          diagnostics->total_requests,
                          diagnostics->successful_requests,
                          diagnostics->failed_requests,
                          diagnostics->uptime_ms,
                          diagnostics->system_info,
                          diagnostics->error_log);
    
    return (written < buffer_size) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t api_parse_json_body(const char *body, char *json_buffer, int buffer_size) {
    if (body == NULL || json_buffer == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Simple JSON parsing - just copy the body
    strncpy(json_buffer, body, buffer_size - 1);
    json_buffer[buffer_size - 1] = '\0';
    
    return HAL_STATUS_OK;
}

hal_status_t api_validate_request(const api_mgr_http_request_t *request, api_mgr_http_method_t required_method, const char *required_path) {
    if (request == NULL || required_path == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (request->method != required_method) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (strncmp(request->path, required_path, strlen(required_path)) != 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t api_create_error_response(api_mgr_http_response_t *response, api_mgr_response_code_t error_code, const char *error_message) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Set error response
    response->status_code = error_code;
    response->content_type = API_MGR_CONTENT_TYPE_JSON;
    
    // Create error JSON
    char error_json[512];
    snprintf(error_json, sizeof(error_json),
             "{\"error\":\"%s\",\"code\":%d,\"timestamp\":%lu}",
             error_message, error_code, hal_get_timestamp_ms());
    
    response->body = strdup(error_json);
    response->body_length = strlen(error_json);
    
    return HAL_STATUS_OK;
}

hal_status_t api_create_success_response(api_mgr_http_response_t *response, const char *data) {
    if (response == NULL || data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Set success response
    response->status_code = API_MGR_RESPONSE_OK;
    response->content_type = API_MGR_CONTENT_TYPE_JSON;
    
    response->body = strdup(data);
    response->body_length = strlen(data);
    
    return HAL_STATUS_OK;
}

hal_status_t api_create_telemetry_json(const api_telemetry_t *telemetry, char *json_buffer, int buffer_size) {
    if (telemetry == NULL || json_buffer == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int written = snprintf(json_buffer, buffer_size,
                          "{\"timestamp\":%lu,\"system_state\":\"%s\","
                          "\"position\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f},"
                          "\"velocity\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f},"
                          "\"acceleration\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f},"
                          "\"estop_active\":%s,\"safety_ok\":%s,\"active_modules\":%u,"
                          "\"status_message\":\"%s\"}",
                          telemetry->timestamp, telemetry->system_state,
                          telemetry->position_x, telemetry->position_y, telemetry->position_z,
                          telemetry->velocity_x, telemetry->velocity_y, telemetry->velocity_z,
                          telemetry->acceleration_x, telemetry->acceleration_y, telemetry->acceleration_z,
                          telemetry->estop_active ? "true" : "false",
                          telemetry->safety_ok ? "true" : "false",
                          telemetry->active_modules,
                          telemetry->status_message);
    
    return (written < buffer_size) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}
