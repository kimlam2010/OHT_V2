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

// External dependencies (to be integrated with existing managers)
extern hal_status_t system_get_status(void);
extern hal_status_t safety_get_status(void);
extern hal_status_t module_get_list(void);
extern hal_status_t config_get_data(void);
extern hal_status_t config_set_data(const char *data);

// API Endpoint Handlers

hal_status_t api_handle_system_status(const http_request_t *request, http_response_t *response) {
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
        return api_create_error_response(response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Failed to create system status");
    }
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_system_health(const http_request_t *request, http_response_t *response) {
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
             "{\"status\":\"%s\",\"timestamp\":%llu,\"response_time_ms\":%u,\"details\":\"%s\"}",
             health.status, health.timestamp, health.response_time_ms, health.details);
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_modules_list(const http_request_t *request, http_response_t *response) {
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
        return api_create_error_response(response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Failed to create modules list");
    }
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_module_info(const http_request_t *request, http_response_t *response) {
    int module_id = api_extract_module_id(request->path);
    if (module_id < 0) {
        return api_create_error_response(response, HTTP_STATUS_BAD_REQUEST, "Invalid module ID");
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
            return api_create_error_response(response, HTTP_STATUS_NOT_FOUND, "Module not found");
    }
    
    module.last_seen = hal_get_timestamp_ms();
    strcpy(module.version, "1.0.0");
    
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"module_id\":%u,\"module_type\":\"%s\",\"status\":\"%s\",\"online\":%s,\"last_seen\":%llu,\"version\":\"%s\"}",
             module.module_id, module.module_type, module.status,
             module.online ? "true" : "false", module.last_seen, module.version);
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_module_command(const http_request_t *request, http_response_t *response) {
    int module_id = api_extract_module_id(request->path);
    if (module_id < 0) {
        return api_create_error_response(response, HTTP_STATUS_BAD_REQUEST, "Invalid module ID");
    }
    
    if (request->method != HTTP_METHOD_POST) {
        return api_create_error_response(response, HTTP_STATUS_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Parse command from request body
    api_module_command_t command = {0};
    hal_status_t result = api_parse_json_body(request->body, command.command, sizeof(command.command));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, HTTP_STATUS_BAD_REQUEST, "Invalid command format");
    }
    
    // Mock command execution
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"module_id\":%d,\"command\":\"%s\",\"status\":\"executed\",\"timestamp\":%llu}",
             module_id, command.command, hal_get_timestamp_ms());
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_safety_status(const http_request_t *request, http_response_t *response) {
    (void)request;
    
    // Create safety status response
    api_safety_status_t safety = {
        .estop_active = false,
        .safety_ok = true,
        .safety_level = 1,
        .safety_message = "All safety systems operational",
        .last_safety_check = hal_get_timestamp_ms()
    };
    
    char json_buffer[1024];
    hal_status_t result = api_create_safety_status_json(&safety, json_buffer, sizeof(json_buffer));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Failed to create safety status");
    }
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_safety_estop(const http_request_t *request, http_response_t *response) {
    if (request->method != HTTP_METHOD_POST) {
        return api_create_error_response(response, HTTP_STATUS_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Parse E-Stop request
    api_estop_request_t estop = {0};
    hal_status_t result = api_parse_json_body(request->body, estop.estop_reason, sizeof(estop.estop_reason));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, HTTP_STATUS_BAD_REQUEST, "Invalid E-Stop request format");
    }
    
    // Mock E-Stop execution
    estop.timestamp = hal_get_timestamp_ms();
    estop.acknowledged = true;
    
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"estop_reason\":\"%s\",\"timestamp\":%llu,\"acknowledged\":%s,\"status\":\"executed\"}",
             estop.estop_reason, estop.timestamp, estop.acknowledged ? "true" : "false");
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_config_get(const http_request_t *request, http_response_t *response) {
    (void)request;
    
    // Create config data response
    api_config_data_t config = {
        .config_data = "{\"system\":{\"name\":\"OHT-50\",\"version\":\"1.0.0\"},\"network\":{\"port\":8080,\"timeout\":30000}}",
        .config_version = 1,
        .last_updated = hal_get_timestamp_ms()
    };
    
    char json_buffer[2048];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"config_data\":%s,\"config_version\":%u,\"last_updated\":%llu}",
             config.config_data, config.config_version, config.last_updated);
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_config_set(const http_request_t *request, http_response_t *response) {
    if (request->method != HTTP_METHOD_PUT) {
        return api_create_error_response(response, HTTP_STATUS_METHOD_NOT_ALLOWED, "Method not allowed");
    }
    
    // Parse config data from request body
    char config_data[1024];
    hal_status_t result = api_parse_json_body(request->body, config_data, sizeof(config_data));
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, HTTP_STATUS_BAD_REQUEST, "Invalid config format");
    }
    
    // Mock config update
    char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer),
             "{\"status\":\"updated\",\"config_version\":2,\"timestamp\":%llu}",
             hal_get_timestamp_ms());
    
    return api_create_success_response(response, json_buffer);
}

hal_status_t api_handle_diagnostics(const http_request_t *request, http_response_t *response) {
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
        return api_create_error_response(response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Failed to create diagnostics");
    }
    
    return api_create_success_response(response, json_buffer);
}

// API Utility Functions

hal_status_t api_endpoints_init(http_server_t *server) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Add API routes
    hal_status_t status;
    
    // System endpoints
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/system/status", api_handle_system_status, false);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/system/health", api_handle_system_health, false);
    if (status != HAL_STATUS_OK) return status;
    
    // Module endpoints
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/modules", api_handle_modules_list, false);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/modules/1", api_handle_module_info, false);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/modules/2", api_handle_module_info, false);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/modules/3", api_handle_module_info, false);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_POST, "/api/v1/modules/1/command", api_handle_module_command, true);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_POST, "/api/v1/modules/2/command", api_handle_module_command, true);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_POST, "/api/v1/modules/3/command", api_handle_module_command, true);
    if (status != HAL_STATUS_OK) return status;
    
    // Safety endpoints
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/safety/status", api_handle_safety_status, false);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_POST, "/api/v1/safety/estop", api_handle_safety_estop, true);
    if (status != HAL_STATUS_OK) return status;
    
    // Config endpoints
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/config", api_handle_config_get, false);
    if (status != HAL_STATUS_OK) return status;
    
    status = http_server_add_route(server, HTTP_METHOD_PUT, "/api/v1/config", api_handle_config_set, true);
    if (status != HAL_STATUS_OK) return status;
    
    // Diagnostics endpoint
    status = http_server_add_route(server, HTTP_METHOD_GET, "/api/v1/diagnostics", api_handle_diagnostics, false);
    if (status != HAL_STATUS_OK) return status;
    
    hal_log_info("[API] Endpoints initialized successfully");
    return HAL_STATUS_OK;
}

hal_status_t api_endpoints_deinit(http_server_t *server) {
    (void)server;
    hal_log_info("[API] Endpoints deinitialized");
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
        int id_len = command_pos - id_start;
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
                          "\"uptime_ms\":%llu,\"active_modules\":%u,\"estop_active\":%s,\"safety_ok\":%s}",
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
                           "\"online\":%s,\"last_seen\":%llu,\"version\":\"%s\"}",
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
                          "\"safety_message\":\"%s\",\"last_safety_check\":%llu}",
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
                          "\"uptime_ms\":%llu,\"system_info\":\"%s\",\"error_log\":\"%s\"}",
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

hal_status_t api_validate_request(const http_request_t *request, http_method_t required_method, const char *required_path) {
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

hal_status_t api_create_error_response(http_response_t *response, http_status_t error_code, const char *error_message) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return http_response_set_error(response, error_code, error_message);
}

hal_status_t api_create_success_response(http_response_t *response, const char *data) {
    if (response == NULL || data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return http_response_set_json(response, data);
}
