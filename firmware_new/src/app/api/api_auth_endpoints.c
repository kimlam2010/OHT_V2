/**
 * @file api_auth_endpoints.c
 * @brief API Authentication Endpoint Handlers for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.1
 */

#include "api_auth.h"
#include "api_endpoints.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Helper function to get header value
 * @param request HTTP request
 * @param header_name Header name
 * @return const char* Header value or NULL if not found
 */
static const char* get_header_value(const api_mgr_http_request_t *request, const char *header_name) {
    if (request == NULL || header_name == NULL) {
        return NULL;
    }
    
    for (uint32_t i = 0; i < request->header_count; i++) {
        if (strcasecmp(request->headers[i].name, header_name) == 0) {
            return request->headers[i].value;
        }
    }
    return NULL;
}

/**
 * @brief Handle POST /api/v1/auth/login - User login
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_login(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Parse request body
    api_auth_request_t auth_request;
    memset(&auth_request, 0, sizeof(auth_request));
    
    // Extract username
    const char *username_start = strstr(request->body, "\"username\":");
    if (username_start != NULL) {
        username_start += 11; // Skip "username":
        while (*username_start == ' ' || *username_start == '\t') username_start++; // Skip whitespace
        
        if (*username_start == '"') {
            username_start++; // Skip opening quote
            const char *username_end = strchr(username_start, '"');
            if (username_end != NULL) {
                size_t username_len = username_end - username_start;
                if (username_len < sizeof(auth_request.username)) {
                    strncpy(auth_request.username, username_start, username_len);
                    auth_request.username[username_len] = '\0';
                }
            }
        }
    }
    
    // Extract password
    const char *password_start = strstr(request->body, "\"password\":");
    if (password_start != NULL) {
        password_start += 11; // Skip "password":
        while (*password_start == ' ' || *password_start == '\t') password_start++; // Skip whitespace
        
        if (*password_start == '"') {
            password_start++; // Skip opening quote
            const char *password_end = strchr(password_start, '"');
            if (password_end != NULL) {
                size_t password_len = password_end - password_start;
                if (password_len < sizeof(auth_request.password)) {
                    strncpy(auth_request.password, password_start, password_len);
                    auth_request.password[password_len] = '\0';
                }
            }
        }
    }
    
    // Extract client IP from headers
    const char *client_ip = get_header_value(request, "X-Forwarded-For");
    if (client_ip == NULL) {
        client_ip = get_header_value(request, "X-Real-IP");
    }
    if (client_ip != NULL) {
        strncpy(auth_request.client_ip, client_ip, sizeof(auth_request.client_ip) - 1);
        auth_request.client_ip[sizeof(auth_request.client_ip) - 1] = '\0';
    } else {
        strcpy(auth_request.client_ip, "127.0.0.1"); // Default to localhost
    }
    
    // Extract user agent
    const char *user_agent = get_header_value(request, "User-Agent");
    if (user_agent != NULL) {
        strncpy(auth_request.user_agent, user_agent, sizeof(auth_request.user_agent) - 1);
        auth_request.user_agent[sizeof(auth_request.user_agent) - 1] = '\0';
    } else {
        strcpy(auth_request.user_agent, "Unknown");
    }
    
    if (strlen(auth_request.username) == 0 || strlen(auth_request.password) == 0) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Missing username or password");
    }
    
    // Authenticate user
    api_auth_response_t auth_response;
    hal_status_t result = api_auth_authenticate(&auth_request, &auth_response);
    
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, auth_response.message);
    }
    
    // Create JSON response
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": %s,\n"
        "  \"message\": \"%s\",\n"
        "  \"session_id\": \"%s\",\n"
        "  \"expires_at\": %lu,\n"
        "  \"role\": \"%s\",\n"
        "  \"permissions\": %u\n"
        "}",
        auth_response.success ? "true" : "false",
        auth_response.message,
        auth_response.session_id,
        auth_response.expires_at,
        api_auth_get_role_name(auth_response.role),
        auth_response.permissions
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/auth/logout - User logout
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_logout(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Extract session ID from headers
    const char *session_header = get_header_value(request, "Authorization");
    if (session_header == NULL) {
        return api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Missing authorization header");
    }
    
    // session_header is already the value, no need to skip prefix
    
    // Check for Bearer token format
    if (strncmp(session_header, "Bearer ", 7) == 0) {
        session_header += 7; // Skip "Bearer "
    }
    
    char session_id[65];
    strncpy(session_id, session_header, sizeof(session_id) - 1);
    session_id[sizeof(session_id) - 1] = '\0';
    
    // Remove any trailing whitespace or newlines
    char *end = session_id + strlen(session_id) - 1;
    while (end > session_id && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        *end = '\0';
        end--;
    }
    
    // Destroy session
    hal_status_t result = api_auth_destroy_session(session_id);
    
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid session");
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Logout successful\"\n"
        "}"
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/auth/session - Get session information
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_session(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Extract session ID from headers
    const char *session_header = get_header_value(request, "Authorization");
    if (session_header == NULL) {
        return api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Missing authorization header");
    }
    
    // session_header is already the value, no need to skip prefix
    
    // Check for Bearer token format
    if (strncmp(session_header, "Bearer ", 7) == 0) {
        session_header += 7; // Skip "Bearer "
    }
    
    char session_id[65];
    strncpy(session_id, session_header, sizeof(session_id) - 1);
    session_id[sizeof(session_id) - 1] = '\0';
    
    // Remove any trailing whitespace or newlines
    char *end = session_id + strlen(session_id) - 1;
    while (end > session_id && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        *end = '\0';
        end--;
    }
    
    // Validate session
    api_session_t session;
    hal_status_t result = api_auth_validate_session(session_id, &session);
    
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Invalid or expired session");
    }
    
    // Create JSON response
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Session information retrieved successfully\",\n"
        "  \"session\": {\n"
        "    \"session_id\": \"%s\",\n"
        "    \"username\": \"%s\",\n"
        "    \"role\": \"%s\",\n"
        "    \"permissions\": %u,\n"
        "    \"created_at\": %lu,\n"
        "    \"expires_at\": %lu,\n"
        "    \"last_activity\": %lu,\n"
        "    \"client_ip\": \"%s\",\n"
        "    \"active\": %s\n"
        "  }\n"
        "}",
        session.session_id,
        session.user.username,
        api_auth_get_role_name(session.user.role),
        session.user.permissions,
        session.created_at,
        session.expires_at,
        session.last_activity,
        session.client_ip,
        session.active ? "true" : "false"
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle POST /api/v1/auth/refresh - Refresh session
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_refresh(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_POST);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Extract session ID from headers
    const char *session_header = get_header_value(request, "Authorization");
    if (session_header == NULL) {
        return api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Missing authorization header");
    }
    
    // session_header is already the value, no need to skip prefix
    
    // Check for Bearer token format
    if (strncmp(session_header, "Bearer ", 7) == 0) {
        session_header += 7; // Skip "Bearer "
    }
    
    char session_id[65];
    strncpy(session_id, session_header, sizeof(session_id) - 1);
    session_id[sizeof(session_id) - 1] = '\0';
    
    // Remove any trailing whitespace or newlines
    char *end = session_id + strlen(session_id) - 1;
    while (end > session_id && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        *end = '\0';
        end--;
    }
    
    // Refresh session
    hal_status_t result = api_auth_refresh_session(session_id);
    
    if (result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Failed to refresh session");
    }
    
    // Create JSON response
    char json_buffer[256];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Session refreshed successfully\"\n"
        "}"
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

/**
 * @brief Handle GET /api/v1/auth/stats - Get authentication statistics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_stats(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request_method(request, API_MGR_HTTP_GET);
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request method");
    }
    
    // Get authentication statistics
    api_auth_stats_t stats;
    hal_status_t result = api_auth_get_stats(&stats);
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get authentication statistics: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    int json_len = snprintf(json_buffer, sizeof(json_buffer),
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Authentication statistics retrieved successfully\",\n"
        "  \"stats\": {\n"
        "    \"total_users\": %u,\n"
        "    \"active_sessions\": %u,\n"
        "    \"total_logins\": %u,\n"
        "    \"failed_logins\": %u,\n"
        "    \"locked_accounts\": %u,\n"
        "    \"last_activity\": %lu\n"
        "  }\n"
        "}",
        stats.total_users,
        stats.active_sessions,
        stats.total_logins,
        stats.failed_logins,
        stats.locked_accounts,
        stats.last_activity
    );
    
    if (json_len < 0 || (size_t)json_len >= sizeof(json_buffer)) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_manager_create_success_response(response, json_buffer);
}

// Placeholder functions for future implementation
hal_status_t api_handle_auth_users(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request; // Unused parameter
    (void)response; // Unused parameter
    return api_create_error_response(response, API_MGR_RESPONSE_NOT_IMPLEMENTED, "User management not implemented yet");
}

hal_status_t api_handle_auth_create_user(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request; // Unused parameter
    (void)response; // Unused parameter
    return api_create_error_response(response, API_MGR_RESPONSE_NOT_IMPLEMENTED, "User creation not implemented yet");
}

hal_status_t api_handle_auth_update_user(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request; // Unused parameter
    (void)response; // Unused parameter
    return api_create_error_response(response, API_MGR_RESPONSE_NOT_IMPLEMENTED, "User update not implemented yet");
}

hal_status_t api_handle_auth_delete_user(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request; // Unused parameter
    (void)response; // Unused parameter
    return api_create_error_response(response, API_MGR_RESPONSE_NOT_IMPLEMENTED, "User deletion not implemented yet");
}
