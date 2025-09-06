/**
 * @file api_diagnostics_endpoints.c
 * @brief API Diagnostics Endpoint Handlers for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.3
 */

#include "api_diagnostics.h"
#include "api_endpoints.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Handle GET /api/v1/diagnostics/system - Get system diagnostics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_system(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get system diagnostics
    api_system_diagnostics_t diagnostics;
    hal_status_t result = api_diagnostics_get_system(&diagnostics);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get system diagnostics: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[1024];
    hal_status_t json_result = api_serialize_system_diagnostics_json(&diagnostics, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/diagnostics/modules - Get all module diagnostics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_modules(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get all module diagnostics
    api_module_diagnostics_t diagnostics[16];
    uint32_t actual_count;
    hal_status_t result = api_diagnostics_get_all_modules(diagnostics, 16, &actual_count);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get module diagnostics: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[4096];
    int offset = 0;
    
    offset += snprintf(json_buffer + offset, sizeof(json_buffer) - offset,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Module diagnostics retrieved successfully\",\n"
        "  \"module_count\": %u,\n"
        "  \"modules\": [\n",
        actual_count
    );
    
    for (uint32_t i = 0; i < actual_count; i++) {
        if (i > 0) {
            offset += snprintf(json_buffer + offset, sizeof(json_buffer) - offset, ",\n");
        }
        
        offset += snprintf(json_buffer + offset, sizeof(json_buffer) - offset,
            "    {\n"
            "      \"module_name\": \"%s\",\n"
            "      \"online\": %s,\n"
            "      \"last_heartbeat\": %lu,\n"
            "      \"error_count\": %u,\n"
            "      \"warning_count\": %u,\n"
            "      \"message_count\": %u,\n"
            "      \"message_errors\": %u,\n"
            "      \"response_time_avg\": %.2f,\n"
            "      \"status_message\": \"%s\"\n"
            "    }",
            diagnostics[i].module_name,
            diagnostics[i].online ? "true" : "false",
            diagnostics[i].last_heartbeat,
            diagnostics[i].error_count,
            diagnostics[i].warning_count,
            diagnostics[i].message_count,
            diagnostics[i].message_errors,
            diagnostics[i].response_time_avg,
            diagnostics[i].status_message
        );
        
        if (offset < 0 || (size_t)offset >= sizeof(json_buffer)) {
            return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
        }
    }
    
    offset += snprintf(json_buffer + offset, sizeof(json_buffer) - offset,
        "\n  ]\n"
        "}\n"
    );
    
    if (offset < 0 || (size_t)offset >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/diagnostics/module/{name} - Get specific module diagnostics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_module(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Extract module name from path
    char module_name[32];
    if (sscanf(request->path, "/api/v1/diagnostics/module/%31s", module_name) != 1) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid module name in path");
    }
    
    // Get module diagnostics
    api_module_diagnostics_t diagnostics;
    hal_status_t result = api_diagnostics_get_module(module_name, &diagnostics);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get module diagnostics: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[1024];
    hal_status_t json_result = api_serialize_module_diagnostics_json(&diagnostics, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/diagnostics/logs - Get log entries
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_logs(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Parse query parameters
    api_log_level_t level = API_LOG_LEVEL_MAX;
    api_log_category_t category = API_LOG_CATEGORY_MAX;
    uint32_t max_entries = 100;
    
    // Extract level from query string
    const char *level_str = strstr(request->query_string, "level=");
    if (level_str != NULL) {
        level_str += 6; // Skip "level="
        char level_name[16];
        if (sscanf(level_str, "%15s", level_name) == 1) {
            level = api_diagnostics_get_log_level_from_string(level_name);
        }
    }
    
    // Extract category from query string
    const char *category_str = strstr(request->query_string, "category=");
    if (category_str != NULL) {
        category_str += 9; // Skip "category="
        char category_name[16];
        if (sscanf(category_str, "%15s", category_name) == 1) {
            category = api_diagnostics_get_log_category_from_string(category_name);
        }
    }
    
    // Extract max_entries from query string
    const char *max_str = strstr(request->query_string, "max=");
    if (max_str != NULL) {
        max_str += 4; // Skip "max="
        max_entries = (uint32_t)atoi(max_str);
        if (max_entries > 1000) max_entries = 1000; // Limit to 1000 entries
    }
    
    // Get log entries
    api_log_entry_t entries[1000];
    uint32_t actual_count;
    hal_status_t result = api_diagnostics_get_logs(level, category, max_entries, entries, &actual_count);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get log entries: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[8192];
    int offset = 0;
    
    offset += snprintf(json_buffer + offset, sizeof(json_buffer) - offset,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Log entries retrieved successfully\",\n"
        "  \"entry_count\": %u,\n"
        "  \"entries\": [\n",
        actual_count
    );
    
    for (uint32_t i = 0; i < actual_count; i++) {
        if (i > 0) {
            offset += snprintf(json_buffer + offset, sizeof(json_buffer) - offset, ",\n");
        }
        
        offset += snprintf(json_buffer + offset, sizeof(json_buffer) - offset,
            "    {\n"
            "      \"timestamp\": %lu,\n"
            "      \"level\": \"%s\",\n"
            "      \"category\": \"%s\",\n"
            "      \"source\": \"%s\",\n"
            "      \"line_number\": %u,\n"
            "      \"message\": \"%s\",\n"
            "      \"details\": \"%s\"\n"
            "    }",
            entries[i].timestamp,
            api_diagnostics_get_log_level_name(entries[i].level),
            api_diagnostics_get_log_category_name(entries[i].category),
            entries[i].source,
            entries[i].line_number,
            entries[i].message,
            entries[i].details
        );
        
        if (offset < 0 || (size_t)offset >= sizeof(json_buffer)) {
            return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
        }
    }
    
    offset += snprintf(json_buffer + offset, sizeof(json_buffer) - offset,
        "\n  ]\n"
        "}\n"
    );
    
    if (offset < 0 || (size_t)offset >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle DELETE /api/v1/diagnostics/logs - Clear log entries
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_clear_logs(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_DELETE);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Parse query parameters
    api_log_level_t level = API_LOG_LEVEL_MAX;
    api_log_category_t category = API_LOG_CATEGORY_MAX;
    
    // Extract level from query string
    const char *level_str = strstr(request->query_string, "level=");
    if (level_str != NULL) {
        level_str += 6; // Skip "level="
        char level_name[16];
        if (sscanf(level_str, "%15s", level_name) == 1) {
            level = api_diagnostics_get_log_level_from_string(level_name);
        }
    }
    
    // Extract category from query string
    const char *category_str = strstr(request->query_string, "category=");
    if (category_str != NULL) {
        category_str += 9; // Skip "category="
        char category_name[16];
        if (sscanf(category_str, "%15s", category_name) == 1) {
            category = api_diagnostics_get_log_category_from_string(category_name);
        }
    }
    
    // Clear log entries
    hal_status_t result = api_diagnostics_clear_logs(level, category);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to clear log entries: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Log entries cleared successfully\"\n"
        "}"
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/diagnostics/stats - Get diagnostics statistics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_stats(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get diagnostics statistics
    api_diagnostics_stats_t stats;
    hal_status_t result = api_diagnostics_get_stats(&stats);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get diagnostics statistics: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[1024];
    hal_status_t json_result = api_serialize_diagnostics_stats_json(&stats, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/diagnostics/config - Get diagnostics configuration
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_config(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get diagnostics configuration
    api_diagnostics_config_t config;
    hal_status_t result = api_diagnostics_get_config(&config);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get diagnostics configuration: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Diagnostics configuration retrieved successfully\",\n"
        "  \"config\": {\n"
        "    \"diagnostics_enabled\": %s,\n"
        "    \"log_buffer_size\": %u,\n"
        "    \"max_log_entries\": %u,\n"
        "    \"min_log_level\": \"%s\",\n"
        "    \"log_to_file\": %s,\n"
        "    \"log_to_console\": %s,\n"
        "    \"log_file_path\": \"%s\",\n"
        "    \"log_rotation_size\": %u\n"
        "  }\n"
        "}",
        config.diagnostics_enabled ? "true" : "false",
        config.log_buffer_size,
        config.max_log_entries,
        api_diagnostics_get_log_level_name(config.min_log_level),
        config.log_to_file ? "true" : "false",
        config.log_to_console ? "true" : "false",
        config.log_file_path,
        config.log_rotation_size
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/diagnostics/config - Set diagnostics configuration
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_set_config(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Parse request body (simplified JSON parsing)
    api_diagnostics_config_t config;
    
    // Initialize with current config
    hal_status_t result = api_diagnostics_get_config(&config);
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get current config");
    }
    
    // Parse configuration from request body (simplified)
    if (strstr(request->body, "\"diagnostics_enabled\":true") != NULL) {
        config.diagnostics_enabled = true;
    } else if (strstr(request->body, "\"diagnostics_enabled\":false") != NULL) {
        config.diagnostics_enabled = false;
    }
    
    if (strstr(request->body, "\"log_to_console\":true") != NULL) {
        config.log_to_console = true;
    } else if (strstr(request->body, "\"log_to_console\":false") != NULL) {
        config.log_to_console = false;
    }
    
    // Set diagnostics configuration
    result = api_diagnostics_set_config(&config);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to set diagnostics configuration: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Diagnostics configuration updated successfully\"\n"
        "}"
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}
