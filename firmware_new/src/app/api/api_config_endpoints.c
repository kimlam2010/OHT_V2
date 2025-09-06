/**
 * @file api_config_endpoints.c
 * @brief API Configuration Endpoint Handlers for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.1
 */

#include "api_config.h"
#include "api_endpoints.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Handle GET /api/v1/config - Get all configuration
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_get_all(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get all configuration
    api_config_response_t config_response;
    hal_status_t result = api_config_get_all(&config_response);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get configuration: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[8192];
    hal_status_t json_result = api_serialize_config_response_json(&config_response, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/config/{category} - Get configuration by category
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_get_category(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Extract category from path
    char category_str[32];
    if (sscanf(request->path, "/api/v1/config/%31s", category_str) != 1) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid category in path");
    }
    
    // Convert category string to enum
    api_config_category_t category = API_CONFIG_CATEGORY_MAX;
    for (int i = 0; i < API_CONFIG_CATEGORY_MAX; i++) {
        if (strcmp(api_config_get_category_name((api_config_category_t)i), category_str) == 0) {
            category = (api_config_category_t)i;
            break;
        }
    }
    
    if (category == API_CONFIG_CATEGORY_MAX) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid category");
    }
    
    // Get configuration by category
    api_config_response_t config_response;
    hal_status_t result = api_config_get_category(category, &config_response);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get category configuration: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[4096];
    hal_status_t json_result = api_serialize_config_response_json(&config_response, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/config/parameter/{key} - Get specific parameter
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_get_parameter(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Extract key from path
    char key[64];
    if (sscanf(request->path, "/api/v1/config/parameter/%63s", key) != 1) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid key in path");
    }
    
    // Get specific parameter
    api_config_parameter_t parameter;
    hal_status_t result = api_config_get_parameter(key, &parameter);
    
    if (result != HAL_STATUS_OK) {
        if (result == HAL_STATUS_NOT_FOUND) {
            return api_create_error_response(response, API_MGR_RESPONSE_NOT_FOUND, "Parameter not found");
        }
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get parameter: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[1024];
    hal_status_t json_result = api_serialize_config_parameter_json(&parameter, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/config/parameter/{key} - Set configuration parameter
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_set_parameter(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Extract key from path
    char key[64];
    if (sscanf(request->path, "/api/v1/config/parameter/%63s", key) != 1) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid key in path");
    }
    
    // Parse request body
    api_config_update_request_t update_request;
    strncpy(update_request.key, key, sizeof(update_request.key) - 1);
    update_request.key[sizeof(update_request.key) - 1] = '\0';
    
    // Extract value from request body (simple JSON parsing)
    const char *value_start = strstr(request->body, "\"value\":");
    if (value_start == NULL) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Missing value in request body");
    }
    
    value_start += 8; // Skip "value":
    while (*value_start == ' ' || *value_start == '\t') value_start++; // Skip whitespace
    
    if (*value_start != '"') {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid value format");
    }
    
    value_start++; // Skip opening quote
    const char *value_end = strchr(value_start, '"');
    if (value_end == NULL) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid value format");
    }
    
    size_t value_len = value_end - value_start;
    if (value_len >= sizeof(update_request.value)) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Value too long");
    }
    
    strncpy(update_request.value, value_start, value_len);
    update_request.value[value_len] = '\0';
    
    // Check for validate_only flag
    update_request.validate_only = (strstr(request->body, "\"validate_only\":true") != NULL);
    
    // Set parameter
    api_config_update_response_t update_response;
    hal_status_t result = api_config_set_parameter(&update_request, &update_response);
    
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, update_response.message);
    }
    
    // Create JSON response
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": %s,\n"
        "  \"message\": \"%s\",\n"
        "  \"requires_restart\": %s,\n"
        "  \"validation_errors\": \"%s\"\n"
        "}",
        update_response.success ? "true" : "false",
        update_response.message,
        update_response.requires_restart ? "true" : "false",
        update_response.validation_errors
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/config/reset - Reset configuration to defaults
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_reset(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Parse category from request body (optional)
    api_config_category_t category = API_CONFIG_CATEGORY_MAX;
    const char *category_str = strstr(request->body, "\"category\":");
    if (category_str != NULL) {
        category_str += 10; // Skip "category":
        while (*category_str == ' ' || *category_str == '\t') category_str++; // Skip whitespace
        
        if (*category_str == '"') {
            category_str++; // Skip opening quote
            const char *category_end = strchr(category_str, '"');
            if (category_end != NULL) {
                char cat_name[32];
                size_t cat_len = category_end - category_str;
                if (cat_len < sizeof(cat_name)) {
                    strncpy(cat_name, category_str, cat_len);
                    cat_name[cat_len] = '\0';
                    
                    // Convert category string to enum
                    for (int i = 0; i < API_CONFIG_CATEGORY_MAX; i++) {
                        if (strcmp(api_config_get_category_name((api_config_category_t)i), cat_name) == 0) {
                            category = (api_config_category_t)i;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Reset configuration
    hal_status_t result = api_config_reset_defaults(category);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to reset configuration: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Configuration reset to defaults successfully\",\n"
        "  \"category\": \"%s\"\n"
        "}",
        category == API_CONFIG_CATEGORY_MAX ? "all" : api_config_get_category_name(category)
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/config/categories - Get configuration categories
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_categories(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get configuration categories
    api_config_category_info_t categories[API_CONFIG_CATEGORY_MAX];
    uint32_t actual_count;
    hal_status_t result = api_config_get_categories(categories, API_CONFIG_CATEGORY_MAX, &actual_count);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get configuration categories: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[4096];
    hal_status_t json_result = api_serialize_config_categories_json(categories, actual_count, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/config/apply - Apply configuration changes
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_apply(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Apply configuration changes
    bool requires_restart;
    hal_status_t result = api_config_apply_changes(&requires_restart);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to apply configuration changes: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Configuration changes applied successfully\",\n"
        "  \"requires_restart\": %s\n"
        "}",
        requires_restart ? "true" : "false"
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

// Placeholder functions for future implementation
hal_status_t api_handle_config_backup(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request; // Unused parameter
    (void)response; // Unused parameter
    return api_create_error_response(response, API_MGR_RESPONSE_NOT_IMPLEMENTED, "Backup functionality not implemented yet");
}

hal_status_t api_handle_config_restore(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request; // Unused parameter
    (void)response; // Unused parameter
    return api_create_error_response(response, API_MGR_RESPONSE_NOT_IMPLEMENTED, "Restore functionality not implemented yet");
}
