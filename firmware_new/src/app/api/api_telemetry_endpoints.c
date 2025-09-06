/**
 * @file api_telemetry_endpoints.c
 * @brief API Telemetry Endpoint Handlers for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.2
 */

#include "api_telemetry.h"
#include "api_endpoints.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Handle GET /api/v1/telemetry/stats - Get telemetry statistics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_stats(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get telemetry statistics
    api_telemetry_stats_t stats;
    hal_status_t result = api_telemetry_get_stats(&stats);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get telemetry statistics: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[1024];
    hal_status_t json_result = api_serialize_telemetry_stats_json(&stats, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/telemetry/clients - Get telemetry clients
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_clients(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get all telemetry clients
    api_telemetry_client_t clients[32];
    uint32_t actual_count;
    hal_status_t result = api_telemetry_get_all_clients(clients, 32, &actual_count);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get telemetry clients: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[4096];
    hal_status_t json_result = api_serialize_telemetry_clients_json(clients, actual_count, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/telemetry/subscribe - Subscribe to telemetry types
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_subscribe(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Parse request body
    uint32_t client_id = 0;
    api_telemetry_type_t types[API_TELEMETRY_TYPE_MAX];
    uint32_t type_count = 0;
    
    // Extract client_id from request body
    const char *client_id_start = strstr(request->body, "\"client_id\":");
    if (client_id_start != NULL) {
        client_id_start += 11; // Skip "client_id":
        while (*client_id_start == ' ' || *client_id_start == '\t') client_id_start++; // Skip whitespace
        client_id = (uint32_t)atoi(client_id_start);
    }
    
    // Extract types from request body
    const char *types_start = strstr(request->body, "\"types\":[");
    if (types_start != NULL) {
        types_start += 9; // Skip "types":[
        
        while (*types_start != ']' && type_count < API_TELEMETRY_TYPE_MAX) {
            // Skip whitespace and commas
            while (*types_start == ' ' || *types_start == '\t' || *types_start == ',') types_start++;
            
            if (*types_start == '"') {
                types_start++; // Skip opening quote
                const char *type_end = strchr(types_start, '"');
                if (type_end != NULL) {
                    char type_name[32];
                    size_t type_len = type_end - types_start;
                    if (type_len < sizeof(type_name)) {
                        strncpy(type_name, types_start, type_len);
                        type_name[type_len] = '\0';
                        
                        api_telemetry_type_t type = api_telemetry_get_type_from_string(type_name);
                        if (type < API_TELEMETRY_TYPE_MAX) {
                            types[type_count] = type;
                            type_count++;
                        }
                    }
                    types_start = type_end + 1;
                }
            } else {
                break;
            }
        }
    }
    
    if (client_id == 0 || type_count == 0) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid client_id or types");
    }
    
    // Subscribe client to telemetry types
    hal_status_t result = api_telemetry_subscribe_client(client_id, types, type_count);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to subscribe client: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, error_msg);
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Client subscribed successfully\",\n"
        "  \"client_id\": %u,\n"
        "  \"subscribed_types\": %u\n"
        "}",
        client_id,
        type_count
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/telemetry/unsubscribe - Unsubscribe from telemetry types
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_unsubscribe(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Parse request body (similar to subscribe)
    uint32_t client_id = 0;
    api_telemetry_type_t types[API_TELEMETRY_TYPE_MAX];
    uint32_t type_count = 0;
    
    // Extract client_id from request body
    const char *client_id_start = strstr(request->body, "\"client_id\":");
    if (client_id_start != NULL) {
        client_id_start += 11; // Skip "client_id":
        while (*client_id_start == ' ' || *client_id_start == '\t') client_id_start++; // Skip whitespace
        client_id = (uint32_t)atoi(client_id_start);
    }
    
    // Extract types from request body
    const char *types_start = strstr(request->body, "\"types\":[");
    if (types_start != NULL) {
        types_start += 9; // Skip "types":[
        
        while (*types_start != ']' && type_count < API_TELEMETRY_TYPE_MAX) {
            // Skip whitespace and commas
            while (*types_start == ' ' || *types_start == '\t' || *types_start == ',') types_start++;
            
            if (*types_start == '"') {
                types_start++; // Skip opening quote
                const char *type_end = strchr(types_start, '"');
                if (type_end != NULL) {
                    char type_name[32];
                    size_t type_len = type_end - types_start;
                    if (type_len < sizeof(type_name)) {
                        strncpy(type_name, types_start, type_len);
                        type_name[type_len] = '\0';
                        
                        api_telemetry_type_t type = api_telemetry_get_type_from_string(type_name);
                        if (type < API_TELEMETRY_TYPE_MAX) {
                            types[type_count] = type;
                            type_count++;
                        }
                    }
                    types_start = type_end + 1;
                }
            } else {
                break;
            }
        }
    }
    
    if (client_id == 0 || type_count == 0) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid client_id or types");
    }
    
    // Unsubscribe client from telemetry types
    hal_status_t result = api_telemetry_unsubscribe_client(client_id, types, type_count);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to unsubscribe client: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, error_msg);
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Client unsubscribed successfully\",\n"
        "  \"client_id\": %u,\n"
        "  \"unsubscribed_types\": %u\n"
        "}",
        client_id,
        type_count
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/telemetry/start - Start telemetry streaming
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_start(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Start telemetry streaming
    hal_status_t result = api_telemetry_start_streaming();
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to start telemetry streaming: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Telemetry streaming started successfully\"\n"
        "}"
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/telemetry/stop - Stop telemetry streaming
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_stop(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Stop telemetry streaming
    hal_status_t result = api_telemetry_stop_streaming();
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to stop telemetry streaming: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Telemetry streaming stopped successfully\"\n"
        "}"
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}
