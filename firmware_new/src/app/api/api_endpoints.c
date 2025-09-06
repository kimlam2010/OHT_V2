/**
 * @file api_endpoints.c
 * @brief API Endpoints Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#include "api_endpoints.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global variables
static bool g_api_endpoints_initialized = false;
static uint32_t g_api_endpoints_count = 0;

/**
 * @brief Initialize API endpoints
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_endpoints_init(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Endpoints: Initializing...");
    
    if (g_api_endpoints_initialized) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Endpoints: Already initialized");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Register all endpoints
    hal_status_t register_result = api_register_all_endpoints();
    if (register_result != HAL_STATUS_OK) {
        hal_log_error("API_ENDPOINTS", "api_endpoints_init", __LINE__, 
                     register_result, "Failed to register endpoints");
        return register_result;
    }
    
    g_api_endpoints_initialized = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Endpoints: Initialized successfully with %d endpoints", g_api_endpoints_count);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize API endpoints
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_endpoints_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Endpoints: Deinitializing...");
    
    g_api_endpoints_initialized = false;
    g_api_endpoints_count = 0;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Endpoints: Deinitialized successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Register all API endpoints
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_register_all_endpoints(void) {
    hal_status_t result = HAL_STATUS_OK;
    
    // Register system endpoints
    result = api_register_system_endpoints();
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Register safety endpoints
    result = api_register_safety_endpoints();
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Register module endpoints
    result = api_register_module_endpoints();
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Register system endpoints
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_register_system_endpoints(void) {
    // System status endpoint
    api_mgr_endpoint_t system_status_endpoint = {
        .method = API_MGR_HTTP_GET,
        .path = "/api/v1/system/status",
        .handler = api_handle_system_status,
        .requires_authentication = false,
        .description = "Get system status and information"
    };
    
    hal_status_t result = api_manager_register_endpoint(&system_status_endpoint);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    g_api_endpoints_count++;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Register safety endpoints
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_register_safety_endpoints(void) {
    // Safety status endpoint
    api_mgr_endpoint_t safety_status_endpoint = {
        .method = API_MGR_HTTP_GET,
        .path = "/api/v1/safety/status",
        .handler = api_handle_safety_status,
        .requires_authentication = false,
        .description = "Get safety system status"
    };
    
    hal_status_t result = api_manager_register_endpoint(&safety_status_endpoint);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    g_api_endpoints_count++;
    
    // Safety sensors endpoint
    api_mgr_endpoint_t safety_sensors_endpoint = {
        .method = API_MGR_HTTP_GET,
        .path = "/api/v1/safety/sensors",
        .handler = api_handle_safety_sensors,
        .requires_authentication = false,
        .description = "Get safety sensor data"
    };
    
    result = api_manager_register_endpoint(&safety_sensors_endpoint);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    g_api_endpoints_count++;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Register module endpoints
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_register_module_endpoints(void) {
    // Modules list endpoint
    api_mgr_endpoint_t modules_list_endpoint = {
        .method = API_MGR_HTTP_GET,
        .path = "/api/v1/modules",
        .handler = api_handle_modules_list,
        .requires_authentication = false,
        .description = "Get list of registered modules"
    };
    
    hal_status_t result = api_manager_register_endpoint(&modules_list_endpoint);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    g_api_endpoints_count++;
    
    // Power module status endpoint
    api_mgr_endpoint_t power_status_endpoint = {
        .method = API_MGR_HTTP_GET,
        .path = "/api/v1/modules/power/status",
        .handler = api_handle_power_status,
        .requires_authentication = false,
        .description = "Get power module status"
    };
    
    result = api_manager_register_endpoint(&power_status_endpoint);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    g_api_endpoints_count++;
    
    // Motor module status endpoint
    api_mgr_endpoint_t motor_status_endpoint = {
        .method = API_MGR_HTTP_GET,
        .path = "/api/v1/modules/motor/status",
        .handler = api_handle_motor_status,
        .requires_authentication = false,
        .description = "Get motor module status"
    };
    
    result = api_manager_register_endpoint(&motor_status_endpoint);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    g_api_endpoints_count++;
    
    // Dock module status endpoint
    api_mgr_endpoint_t dock_status_endpoint = {
        .method = API_MGR_HTTP_GET,
        .path = "/api/v1/modules/dock/status",
        .handler = api_handle_dock_status,
        .requires_authentication = false,
        .description = "Get dock module status"
    };
    
    result = api_manager_register_endpoint(&dock_status_endpoint);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    g_api_endpoints_count++;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Handle system status endpoint
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_system_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get system status
    api_system_status_t system_status;
    hal_status_t result = api_get_system_status(&system_status);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get system status: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    hal_status_t json_result = api_serialize_system_status_json(&system_status, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle safety status endpoint
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_safety_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get safety status
    api_safety_status_t safety_status;
    hal_status_t result = api_get_safety_status(&safety_status);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get safety status: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    hal_status_t json_result = api_serialize_safety_status_json(&safety_status, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle modules list endpoint
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_modules_list(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get modules list
    api_modules_list_t modules_list;
    hal_status_t result = api_get_modules_list(&modules_list);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get modules list: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[1024];
    hal_status_t json_result = api_serialize_modules_list_json(&modules_list, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle power status endpoint
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_power_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get power status
    api_power_status_t power_status;
    hal_status_t result = api_get_power_status(&power_status);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get power status: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    hal_status_t json_result = api_serialize_power_status_json(&power_status, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle motor status endpoint
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_motor_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get motor status
    api_motor_status_t motor_status;
    hal_status_t result = api_get_motor_status(&motor_status);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get motor status: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    hal_status_t json_result = api_serialize_motor_status_json(&motor_status, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle dock status endpoint
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_dock_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get dock status
    api_dock_status_t dock_status;
    hal_status_t result = api_get_dock_status(&dock_status);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get dock status: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    hal_status_t json_result = api_serialize_dock_status_json(&dock_status, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle safety sensors endpoint
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_safety_sensors(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get safety sensors
    api_safety_sensors_t safety_sensors;
    hal_status_t result = api_get_safety_sensors(&safety_sensors);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get safety sensors: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    hal_status_t json_result = api_serialize_safety_sensors_json(&safety_sensors, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Get system status from system controller
 * @param status Pointer to system status structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_get_system_status(api_system_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get system controller status
    system_controller_status_t sys_status;
    hal_status_t result = system_controller_get_status(&sys_status);
    
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_ENDPOINTS", "api_get_system_status", __LINE__, 
                     result, "Failed to get system controller status");
        return result;
    }
    
    // Convert to API format
    strncpy(status->system_name, "OHT-50", sizeof(status->system_name) - 1);
    status->system_name[sizeof(status->system_name) - 1] = '\0';
    
    strncpy(status->version, "1.0.0", sizeof(status->version) - 1);
    status->version[sizeof(status->version) - 1] = '\0';
    
    strncpy(status->status, api_get_system_state_name(sys_status.current_state), sizeof(status->status) - 1);
    status->status[sizeof(status->status) - 1] = '\0';
    
    status->uptime_ms = sys_status.uptime_ms;
    status->active_modules = sys_status.system_ready ? 1 : 0;
    status->estop_active = !sys_status.safety_ok;
    status->safety_ok = sys_status.safety_ok;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get safety status from safety manager
 * @param status Pointer to safety status structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_get_safety_status(api_safety_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get safety monitor status
    safety_monitor_status_t safety_status;
    hal_status_t result = safety_monitor_get_status(&safety_status);
    
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_ENDPOINTS", "api_get_safety_status", __LINE__, 
                     result, "Failed to get safety monitor status");
        return result;
    }
    
    // Convert to API format
    status->estop_active = safety_status.estop_active;
    status->safety_ok = safety_status.current_state == SAFETY_MONITOR_STATE_SAFE;
    status->safety_level = (uint32_t)safety_status.current_state;
    status->last_safety_check = safety_status.last_update_time;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get modules list from module manager
 * @param modules Pointer to modules list structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_get_modules_list(api_modules_list_t *modules) {
    if (modules == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get registered modules from module manager
    uint8_t module_ids[16];
    uint32_t actual_count;
    hal_status_t result = module_manager_get_registered_modules(module_ids, 16, &actual_count);
    
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_ENDPOINTS", "api_get_modules_list", __LINE__, 
                     result, "Failed to get registered modules");
        return result;
    }
    
    // Convert to API format
    modules->module_count = actual_count;
    
    for (uint32_t i = 0; i < actual_count && i < 16; i++) {
        module_info_t module_info;
        if (module_manager_get_module_info(module_ids[i], &module_info) == HAL_STATUS_OK) {
            modules->modules[i].module_id = module_ids[i];
            strncpy(modules->modules[i].module_type, 
                   api_get_module_type_name(module_info.type), 31);
            modules->modules[i].module_type[31] = '\0';
            modules->modules[i].online = module_info.status == MODULE_STATUS_ONLINE;
            modules->modules[i].last_seen = module_info.last_seen_ms;
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get power status from power module handler
 * @param status Pointer to power status structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_get_power_status(api_power_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get power module data
    power_module_data_t power_data;
    hal_status_t result = power_module_handler_read_data(&power_data);
    
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_ENDPOINTS", "api_get_power_status", __LINE__, 
                     result, "Failed to read power module data");
        return result;
    }
    
    // Convert to API format
    status->battery_voltage = power_data.battery_voltage;
    status->battery_current = power_data.battery_current;
    status->battery_soc = power_data.battery_soc;
    status->charge_status = power_data.charge_status;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get motor status from motor module handler
 * @param status Pointer to motor status structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_get_motor_status(api_motor_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get motor module data
    motor_module_data_t motor_data;
    motor_module_handler_t handler;
    
    // Initialize handler with default config
    motor_module_config_t config = {0};
    config.address = 0x01; // Motor module address
    motor_module_init(&handler, &config);
    
    hal_status_t result = motor_module_get_data(&handler, &motor_data);
    
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_ENDPOINTS", "api_get_motor_status", __LINE__, 
                     result, "Failed to read motor module data");
        return result;
    }
    
    // Convert to API format
    status->position = motor_data.position_actual;
    status->velocity = motor_data.speed_actual;
    status->temperature = motor_data.temperature;
    status->voltage = motor_data.voltage;
    status->current = motor_data.current;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get dock status from dock module handler
 * @param status Pointer to dock status structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_get_dock_status(api_dock_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get dock module data
    dock_module_handler_t handler;
    
    // Initialize handler with default config
    dock_module_init(&handler, 0x04); // Dock module address
    
    uint8_t dock_status;
    uint16_t dock_position;
    hal_status_t result = dock_module_get_status(&handler, &dock_status);
    if (result == HAL_STATUS_OK) {
        result = dock_module_get_position(&handler, &dock_position);
    }
    
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_ENDPOINTS", "api_get_dock_status", __LINE__, 
                     result, "Failed to read dock module data");
        return result;
    }
    
    // Convert to API format
    status->dock_status = dock_status;
    status->dock_position = dock_position;
    status->dock_ready = (dock_status == DOCK_STATUS_DOCKED);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get safety sensors from safety module handler
 * @param sensors Pointer to safety sensors structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_get_safety_sensors(api_safety_sensors_t *sensors) {
    if (sensors == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get safety module data
    safety_module_data_t safety_data;
    safety_module_handler_t handler;
    
    // Initialize handler with default config
    safety_module_config_t config = {0};
    config.address = 0x03; // Safety module address
    safety_module_init(&handler, &config);
    
    hal_status_t result = safety_module_get_data(&handler, &safety_data);
    
    if (result != HAL_STATUS_OK) {
        hal_log_error("API_ENDPOINTS", "api_get_safety_sensors", __LINE__, 
                     result, "Failed to read safety module data");
        return result;
    }
    
    // Convert to API format
    for (int i = 0; i < 4; i++) {
        sensors->analog_sensors[i] = safety_data.analog_sensors[i];
    }
    sensors->digital_sensors = safety_data.digital_sensors;
    sensors->proximity_alert = safety_data.proximity_alert;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize system status to JSON
 * @param status System status structure
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_system_status_json(const api_system_status_t *status, char *buffer, size_t buffer_size) {
    if (status == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int result = snprintf(buffer, buffer_size,
        "{\n"
        "  \"system_name\": \"%s\",\n"
        "  \"version\": \"%s\",\n"
        "  \"status\": \"%s\",\n"
        "  \"uptime_ms\": %lu,\n"
        "  \"active_modules\": %u,\n"
        "  \"estop_active\": %s,\n"
        "  \"safety_ok\": %s\n"
        "}",
        status->system_name,
        status->version,
        status->status,
        status->uptime_ms,
        status->active_modules,
        status->estop_active ? "true" : "false",
        status->safety_ok ? "true" : "false"
    );
    
    if (result < 0 || (size_t)result >= buffer_size) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize safety status to JSON
 * @param status Safety status structure
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_safety_status_json(const api_safety_status_t *status, char *buffer, size_t buffer_size) {
    if (status == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int result = snprintf(buffer, buffer_size,
        "{\n"
        "  \"estop_active\": %s,\n"
        "  \"safety_ok\": %s,\n"
        "  \"safety_level\": %u,\n"
        "  \"last_safety_check\": %lu\n"
        "}",
        status->estop_active ? "true" : "false",
        status->safety_ok ? "true" : "false",
        status->safety_level,
        status->last_safety_check
    );
    
    if (result < 0 || (size_t)result >= buffer_size) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize modules list to JSON
 * @param modules Modules list structure
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_modules_list_json(const api_modules_list_t *modules, char *buffer, size_t buffer_size) {
    if (modules == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = snprintf(buffer, buffer_size,
        "{\n"
        "  \"module_count\": %u,\n"
        "  \"modules\": [\n",
        (unsigned int)modules->module_count
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    for (uint32_t i = 0; i < modules->module_count; i++) {
        int result = snprintf(buffer + offset, buffer_size - offset,
            "    {\n"
            "      \"module_id\": %u,\n"
            "      \"module_type\": \"%s\",\n"
            "      \"online\": %s,\n"
            "      \"last_seen\": %lu\n"
            "    }%s\n",
            modules->modules[i].module_id,
            modules->modules[i].module_type,
            modules->modules[i].online ? "true" : "false",
            modules->modules[i].last_seen,
            (i < modules->module_count - 1) ? "," : ""
        );
        
        if (result < 0 || (size_t)(offset + result) >= buffer_size) {
            return HAL_STATUS_INVALID_PARAMETER;
        }
        
        offset += result;
    }
    
    int result = snprintf(buffer + offset, buffer_size - offset,
        "  ]\n"
        "}\n"
    );
    
    if (result < 0 || (size_t)(offset + result) >= buffer_size) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize power status to JSON
 * @param status Power status structure
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_power_status_json(const api_power_status_t *status, char *buffer, size_t buffer_size) {
    if (status == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int result = snprintf(buffer, buffer_size,
        "{\n"
        "  \"battery_voltage\": %.2f,\n"
        "  \"battery_current\": %.2f,\n"
        "  \"battery_soc\": %u,\n"
        "  \"charge_status\": %u\n"
        "}",
        status->battery_voltage,
        status->battery_current,
        status->battery_soc,
        status->charge_status
    );
    
    if (result < 0 || (size_t)result >= buffer_size) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize motor status to JSON
 * @param status Motor status structure
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_motor_status_json(const api_motor_status_t *status, char *buffer, size_t buffer_size) {
    if (status == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int result = snprintf(buffer, buffer_size,
        "{\n"
        "  \"position\": %d,\n"
        "  \"velocity\": %d,\n"
        "  \"temperature\": %d,\n"
        "  \"voltage\": %u,\n"
        "  \"current\": %d\n"
        "}",
        status->position,
        status->velocity,
        status->temperature,
        status->voltage,
        status->current
    );
    
    if (result < 0 || (size_t)result >= buffer_size) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize dock status to JSON
 * @param status Dock status structure
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_dock_status_json(const api_dock_status_t *status, char *buffer, size_t buffer_size) {
    if (status == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int result = snprintf(buffer, buffer_size,
        "{\n"
        "  \"dock_status\": %u,\n"
        "  \"dock_position\": %d,\n"
        "  \"dock_ready\": %s\n"
        "}",
        status->dock_status,
        status->dock_position,
        status->dock_ready ? "true" : "false"
    );
    
    if (result < 0 || (size_t)result >= buffer_size) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize safety sensors to JSON
 * @param sensors Safety sensors structure
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_safety_sensors_json(const api_safety_sensors_t *sensors, char *buffer, size_t buffer_size) {
    if (sensors == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int result = snprintf(buffer, buffer_size,
        "{\n"
        "  \"analog_sensors\": [%u, %u, %u, %u],\n"
        "  \"digital_sensors\": %u,\n"
        "  \"proximity_alert\": %s\n"
        "}",
        sensors->analog_sensors[0],
        sensors->analog_sensors[1],
        sensors->analog_sensors[2],
        sensors->analog_sensors[3],
        sensors->digital_sensors,
        sensors->proximity_alert ? "true" : "false"
    );
    
    if (result < 0 || (size_t)result >= buffer_size) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get system state name
 * @param state System state
 * @return const char* State name
 */
const char* api_get_system_state_name(system_controller_state_t state) {
    switch (state) {
        case SYSTEM_CONTROLLER_STATE_INIT: return "init";
        case SYSTEM_CONTROLLER_STATE_IDLE: return "idle";
        case SYSTEM_CONTROLLER_STATE_ACTIVE: return "active";
        case SYSTEM_CONTROLLER_STATE_FAULT: return "fault";
        case SYSTEM_CONTROLLER_STATE_EMERGENCY: return "emergency";
        case SYSTEM_CONTROLLER_STATE_SHUTDOWN: return "shutdown";
        default: return "unknown";
    }
}

/**
 * @brief Get safety state name
 * @param state Safety state
 * @return const char* State name
 */
const char* api_get_safety_state_name(safety_monitor_state_t state) {
    switch (state) {
        case SAFETY_MONITOR_STATE_SAFE: return "safe";
        case SAFETY_MONITOR_STATE_WARNING: return "warning";
        case SAFETY_MONITOR_STATE_FAULT: return "fault";
        case SAFETY_MONITOR_STATE_ESTOP: return "estop";
        default: return "unknown";
    }
}

/**
 * @brief Get module type name
 * @param type Module type
 * @return const char* Type name
 */
const char* api_get_module_type_name(module_type_t type) {
    switch (type) {
        case MODULE_TYPE_POWER: return "power";
        case MODULE_TYPE_TRAVEL_MOTOR: return "motor";
        case MODULE_TYPE_SAFETY: return "safety";
        case MODULE_TYPE_DOCK: return "dock";
        case MODULE_TYPE_UNKNOWN: return "unknown";
        default: return "unknown";
    }
}

/**
 * @brief Validate request method
 * @param request HTTP request
 * @param expected_method Expected method
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validate_request_method(const api_mgr_http_request_t *request, api_mgr_http_method_t expected_method) {
    if (request == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (request->method != expected_method) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate request path
 * @param request HTTP request
 * @param expected_path Expected path
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validate_request_path(const api_mgr_http_request_t *request, const char *expected_path) {
    if (request == NULL || expected_path == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (strcmp(request->path, expected_path) != 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create error response
 * @param response HTTP response
 * @param status_code HTTP status code
 * @param error_message Error message
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_create_error_response(api_mgr_http_response_t *response, api_mgr_http_response_code_t status_code, const char *error_message) {
    return api_manager_create_error_response(response, status_code, error_message);
}

/**
 * @brief Get API endpoints status
 * @param initialized Pointer to initialized flag
 * @param endpoint_count Pointer to endpoint count
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_endpoints_get_status(bool *initialized, uint32_t *endpoint_count) {
    if (initialized == NULL || endpoint_count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *initialized = g_api_endpoints_initialized;
    *endpoint_count = g_api_endpoints_count;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get API endpoints version
 * @param major Major version number
 * @param minor Minor version number
 * @param patch Patch version number
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_endpoints_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch) {
    if (major == NULL || minor == NULL || patch == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *major = API_ENDPOINTS_VERSION_MAJOR;
    *minor = API_ENDPOINTS_VERSION_MINOR;
    *patch = API_ENDPOINTS_VERSION_PATCH;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get API endpoints version string
 * @return const char* Version string
 */
const char* api_endpoints_get_version_string(void) {
    return API_ENDPOINTS_VERSION_STRING;
}
