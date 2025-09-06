/**
 * @file api_auth_middleware.h
 * @brief API Authentication Middleware for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.1
 */

#ifndef API_AUTH_MIDDLEWARE_H
#define API_AUTH_MIDDLEWARE_H

#include "api_auth.h"
#include "api_endpoints.h"
#include "hal_common.h"

/**
 * @brief Authentication middleware
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK if authenticated, error code if not
 */
hal_status_t api_auth_middleware(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Authorization middleware
 * @param request HTTP request
 * @param response HTTP response
 * @param required_permission Required permission
 * @return hal_status_t HAL_STATUS_OK if authorized, error code if not
 */
hal_status_t api_authz_middleware(const api_mgr_http_request_t *request, api_mgr_http_response_t *response, 
                                 api_permission_t required_permission);

/**
 * @brief Check if endpoint requires authentication
 * @param path Request path
 * @return bool true if authentication required, false otherwise
 */
bool api_auth_endpoint_requires_auth(const char *path);

/**
 * @brief Get required permission for endpoint
 * @param path Request path
 * @param method HTTP method
 * @return api_permission_t Required permission
 */
api_permission_t api_auth_get_endpoint_permission(const char *path, api_mgr_http_method_t method);

/**
 * @brief Apply authentication and authorization to request
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK if authorized, error code if not
 */
hal_status_t api_auth_apply_middleware(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Extract session information from request
 * @param request HTTP request
 * @param session Session information (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_extract_session(const api_mgr_http_request_t *request, api_session_t *session);

/**
 * @brief Check if user has permission for resource
 * @param request HTTP request
 * @param resource Resource name
 * @param permission Required permission
 * @return bool true if user has permission, false otherwise
 */
bool api_auth_check_resource_permission(const api_mgr_http_request_t *request, const char *resource, api_permission_t permission);

/**
 * @brief Log authentication event
 * @param event Event type
 * @param username Username
 * @param client_ip Client IP address
 * @param success Success status
 * @param details Additional details
 */
void api_auth_log_event(const char *event, const char *username, const char *client_ip, bool success, const char *details);

#endif // API_AUTH_MIDDLEWARE_H
