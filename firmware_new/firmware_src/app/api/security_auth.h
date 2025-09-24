/**
 * @file security_auth.h
 * @brief Security Authentication & Authorization Header - Issue #123 Fix
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task Issue #123 Fix - Security Implementation
 */

#ifndef SECURITY_AUTH_H
#define SECURITY_AUTH_H

#include "hal_common.h"
#include "api_endpoints.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Security configuration structure
typedef struct {
    char jwt_secret[256];
    uint32_t jwt_expiry_seconds;
    bool api_key_enabled;
    bool rate_limit_enabled;
    uint32_t max_requests_per_minute;
} security_config_t;

// API key structure
typedef struct {
    char key[64];
    char client_type[32];
    char permissions[32];
    bool enabled;
} api_key_t;

// Client information structure
typedef struct {
    char client_type[32];
    char permissions[32];
} client_info_t;

// Rate limiting entry
typedef struct {
    char client_ip[64];
    time_t timestamp;
} rate_limit_entry_t;

// Security status codes
#define HAL_STATUS_UNAUTHORIZED           -100
#define HAL_STATUS_FORBIDDEN             -101
#define HAL_STATUS_RATE_LIMIT_EXCEEDED   -102

// API Response codes
#define API_MGR_RESPONSE_UNAUTHORIZED        401
#define API_MGR_RESPONSE_FORBIDDEN           403
#define API_MGR_RESPONSE_TOO_MANY_REQUESTS   429

// Function declarations

/**
 * @brief Initialize security authentication system
 * @return hal_status_t HAL_STATUS_OK on success
 */
hal_status_t security_auth_init(void);

/**
 * @brief Deinitialize security authentication system
 * @return hal_status_t HAL_STATUS_OK on success
 */
hal_status_t security_auth_deinit(void);

/**
 * @brief Validate API key
 * @param api_key API key string
 * @param client_info Client information output
 * @return hal_status_t HAL_STATUS_OK if valid
 */
hal_status_t security_auth_validate_api_key(const char *api_key, client_info_t *client_info);

/**
 * @brief Validate JWT token
 * @param token JWT token string
 * @param client_info Client information output
 * @return hal_status_t HAL_STATUS_OK if valid
 */
hal_status_t security_auth_validate_jwt(const char *token, client_info_t *client_info);

/**
 * @brief Check rate limit for client
 * @param client_ip Client IP address
 * @return hal_status_t HAL_STATUS_OK if within limit
 */
hal_status_t security_auth_check_rate_limit(const char *client_ip);

/**
 * @brief Check if client has required permission
 * @param client_info Client information
 * @param required_permission Required permission
 * @return hal_status_t HAL_STATUS_OK if authorized
 */
hal_status_t security_auth_check_permission(const client_info_t *client_info, const char *required_permission);

/**
 * @brief Generate JWT token for client
 * @param client_info Client information
 * @param token_buffer Buffer to store token
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success
 */
hal_status_t security_auth_generate_jwt(const client_info_t *client_info, char *token_buffer, size_t buffer_size);

/**
 * @brief Extract client IP from request
 * @param request HTTP request
 * @param client_ip Buffer for client IP
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success
 */
hal_status_t security_auth_extract_client_ip(const api_mgr_http_request_t *request, char *client_ip, size_t buffer_size);

/**
 * @brief Security middleware for API requests
 * @param request HTTP request
 * @param response HTTP response
 * @param required_permission Required permission
 * @return hal_status_t HAL_STATUS_OK if authorized
 */
hal_status_t security_auth_middleware(const api_mgr_http_request_t *request, api_mgr_http_response_t *response, const char *required_permission);

#ifdef __cplusplus
}
#endif

#endif // SECURITY_AUTH_H
