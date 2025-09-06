/**
 * @file api_auth_middleware.c
 * @brief API Authentication Middleware for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.1
 */

#include "api_auth.h"
#include "api_endpoints.h"
#include <stdio.h>
#include <string.h>

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
 * @brief Authentication middleware
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK if authenticated, error code if not
 */
hal_status_t api_auth_middleware(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    if (request == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Skip authentication for certain endpoints
    if (strstr(request->path, "/api/v1/auth/login") != NULL ||
        strstr(request->path, "/api/v1/auth/logout") != NULL ||
        strstr(request->path, "/api/v1/health") != NULL ||
        strstr(request->path, "/api/v1/status") != NULL) {
        return HAL_STATUS_OK;
    }
    
    // Extract session ID from headers
    const char *session_header = get_header_value(request, "Authorization");
    if (session_header == NULL) {
        api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Missing authorization header");
        return HAL_STATUS_ERROR;
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
        api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Invalid or expired session");
        return result;
    }
    
    // Store session information in request context (if available)
    // This would typically be done by adding session info to request structure
    
    return HAL_STATUS_OK;
}

/**
 * @brief Authorization middleware
 * @param request HTTP request
 * @param response HTTP response
 * @param required_permission Required permission
 * @return hal_status_t HAL_STATUS_OK if authorized, error code if not
 */
hal_status_t api_authz_middleware(const api_mgr_http_request_t *request, api_mgr_http_response_t *response, 
                                 api_permission_t required_permission) {
    if (request == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // First check authentication
    hal_status_t auth_result = api_auth_middleware(request, response);
    if (auth_result != HAL_STATUS_OK) {
        return auth_result;
    }
    
    // Extract session ID from headers
    const char *session_header = get_header_value(request, "Authorization");
    if (session_header == NULL) {
        api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Missing authorization header");
        return HAL_STATUS_ERROR;
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
    
    // Validate session and get permissions
    api_session_t session;
    hal_status_t session_result = api_auth_validate_session(session_id, &session);
    
    if (session_result != HAL_STATUS_OK) {
        api_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Invalid or expired session");
        return session_result;
    }
    
    // Check permissions
    bool has_permission = api_auth_has_permission(session.user.permissions, required_permission);
    
    if (!has_permission) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Insufficient permissions. Required: %s, User role: %s", 
                api_auth_get_permission_name(required_permission),
                api_auth_get_role_name(session.user.role));
        api_create_error_response(response, API_MGR_RESPONSE_FORBIDDEN, error_msg);
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Check if endpoint requires authentication
 * @param path Request path
 * @return bool true if authentication required, false otherwise
 */
bool api_auth_endpoint_requires_auth(const char *path) {
    if (path == NULL) {
        return true; // Default to requiring auth
    }
    
    // Public endpoints that don't require authentication
    const char *public_endpoints[] = {
        "/api/v1/auth/login",
        "/api/v1/auth/logout",
        "/api/v1/health",
        "/api/v1/status",
        "/api/v1/info"
    };
    
    for (size_t i = 0; i < sizeof(public_endpoints) / sizeof(public_endpoints[0]); i++) {
        if (strstr(path, public_endpoints[i]) != NULL) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Get required permission for endpoint
 * @param path Request path
 * @param method HTTP method
 * @return api_permission_t Required permission
 */
api_permission_t api_auth_get_endpoint_permission(const char *path, api_mgr_http_method_t method) {
    if (path == NULL) {
        return API_PERMISSION_READ; // Default to read permission
    }
    
    // Admin endpoints
    if (strstr(path, "/api/v1/auth/users") != NULL ||
        strstr(path, "/api/v1/config") != NULL ||
        strstr(path, "/api/v1/diagnostics") != NULL) {
        return API_PERMISSION_ADMIN;
    }
    
    // Write endpoints
    if (method == API_MGR_HTTP_POST || method == API_MGR_HTTP_PUT || method == API_MGR_HTTP_DELETE) {
        if (strstr(path, "/api/v1/robot/control") != NULL ||
            strstr(path, "/api/v1/safety") != NULL ||
            strstr(path, "/api/v1/config") != NULL) {
            return API_PERMISSION_WRITE;
        }
    }
    
    // Maintenance endpoints
    if (strstr(path, "/api/v1/maintenance") != NULL ||
        strstr(path, "/api/v1/system/restart") != NULL ||
        strstr(path, "/api/v1/system/shutdown") != NULL) {
        return API_PERMISSION_MAINTENANCE;
    }
    
    // Default to read permission
    return API_PERMISSION_READ;
}

/**
 * @brief Apply authentication and authorization to request
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK if authorized, error code if not
 */
hal_status_t api_auth_apply_middleware(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    if (request == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check if endpoint requires authentication
    if (!api_auth_endpoint_requires_auth(request->path)) {
        return HAL_STATUS_OK;
    }
    
    // Get required permission for endpoint
    api_permission_t required_permission = api_auth_get_endpoint_permission(request->path, request->method);
    
    // Apply authorization middleware
    return api_authz_middleware(request, response, required_permission);
}

/**
 * @brief Extract session information from request
 * @param request HTTP request
 * @param session Session information (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_extract_session(const api_mgr_http_request_t *request, api_session_t *session) {
    if (request == NULL || session == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Extract session ID from headers
    const char *session_header = get_header_value(request, "Authorization");
    if (session_header == NULL) {
        return HAL_STATUS_ERROR;
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
    return api_auth_validate_session(session_id, session);
}

/**
 * @brief Check if user has permission for resource
 * @param request HTTP request
 * @param resource Resource name
 * @param permission Required permission
 * @return bool true if user has permission, false otherwise
 */
bool api_auth_check_resource_permission(const api_mgr_http_request_t *request, const char *resource, api_permission_t permission) {
    if (request == NULL || resource == NULL) {
        return false;
    }
    
    api_session_t session;
    hal_status_t result = api_auth_extract_session(request, &session);
    
    if (result != HAL_STATUS_OK) {
        return false;
    }
    
    return api_auth_has_permission(session.user.permissions, permission);
}

/**
 * @brief Log authentication event
 * @param event Event type
 * @param username Username
 * @param client_ip Client IP address
 * @param success Success status
 * @param details Additional details
 */
void api_auth_log_event(const char *event, const char *username, const char *client_ip, bool success, const char *details) {
    if (event == NULL || username == NULL) {
        return;
    }
    
    const char *status = success ? "SUCCESS" : "FAILED";
    hal_log_message(HAL_LOG_LEVEL_INFO, "AUTH_EVENT: %s - User: %s, IP: %s, Status: %s, Details: %s", 
                   event, username, client_ip ? client_ip : "unknown", status, details ? details : "none");
}
