/**
 * @file security_auth.c
 * @brief Security Authentication & Authorization Implementation - Issue #123 Fix
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task Issue #123 Fix - Security Implementation
 */

#include "security_auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <jwt/jwt.h>

// Security configuration
static security_config_t g_security_config = {
    .jwt_secret = "OHT-50-SECRET-KEY-2025",
    .jwt_expiry_seconds = 3600,  // 1 hour
    .api_key_enabled = true,
    .rate_limit_enabled = true,
    .max_requests_per_minute = 1000
};

// API Keys storage (in production, use secure storage)
static api_key_t g_api_keys[] = {
    {"OHT-50-API-KEY-001", "backend", "read_write", true},
    {"OHT-50-API-KEY-002", "frontend", "read_only", true},
    {"OHT-50-API-KEY-003", "mobile", "read_only", true},
    {"OHT-50-API-KEY-004", "admin", "admin", true}
};
static const size_t g_api_keys_count = sizeof(g_api_keys) / sizeof(g_api_keys[0]);

// Rate limiting storage
static rate_limit_entry_t g_rate_limits[1000];
static size_t g_rate_limit_count = 0;

/**
 * @brief Initialize security system
 * @return hal_status_t HAL_STATUS_OK on success
 */
hal_status_t security_auth_init(void) {
    printf("[SECURITY] Initializing authentication system...\n");
    
    // Initialize rate limiting
    memset(g_rate_limits, 0, sizeof(g_rate_limits));
    g_rate_limit_count = 0;
    
    printf("[SECURITY] ✅ Authentication system initialized\n");
    return HAL_STATUS_OK;
}

/**
 * @brief Validate API key
 * @param api_key API key string
 * @param client_info Client information output
 * @return hal_status_t HAL_STATUS_OK if valid
 */
hal_status_t security_auth_validate_api_key(const char *api_key, client_info_t *client_info) {
    if (!api_key || !client_info) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check if API key exists
    for (size_t i = 0; i < g_api_keys_count; i++) {
        if (strcmp(g_api_keys[i].key, api_key) == 0 && g_api_keys[i].enabled) {
            // Copy client info
            strncpy(client_info->client_type, g_api_keys[i].client_type, sizeof(client_info->client_type) - 1);
            strncpy(client_info->permissions, g_api_keys[i].permissions, sizeof(client_info->permissions) - 1);
            client_info->permissions[sizeof(client_info->permissions) - 1] = '\0';
            client_info->client_type[sizeof(client_info->client_type) - 1] = '\0';
            
            printf("[SECURITY] ✅ API key validated for client: %s\n", g_api_keys[i].client_type);
            return HAL_STATUS_OK;
        }
    }
    
    printf("[SECURITY] ❌ Invalid API key: %s\n", api_key);
    return HAL_STATUS_UNAUTHORIZED;
}

/**
 * @brief Validate JWT token
 * @param token JWT token string
 * @param client_info Client information output
 * @return hal_status_t HAL_STATUS_OK if valid
 */
hal_status_t security_auth_validate_jwt(const char *token, client_info_t *client_info) {
    if (!token || !client_info) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    jwt_t *jwt = NULL;
    jwt_new(&jwt);
    
    // Parse JWT token
    if (jwt_decode(&jwt, token, (unsigned char*)g_security_config.jwt_secret, strlen(g_security_config.jwt_secret)) != 0) {
        jwt_free(jwt);
        printf("[SECURITY] ❌ Invalid JWT token\n");
        return HAL_STATUS_UNAUTHORIZED;
    }
    
    // Check expiration
    time_t now = time(NULL);
    time_t exp = jwt_get_grant_int(jwt, "exp");
    if (exp < now) {
        jwt_free(jwt);
        printf("[SECURITY] ❌ JWT token expired\n");
        return HAL_STATUS_UNAUTHORIZED;
    }
    
    // Extract client info
    const char *client_type = jwt_get_grant(jwt, "client_type");
    const char *permissions = jwt_get_grant(jwt, "permissions");
    
    if (client_type) {
        strncpy(client_info->client_type, client_type, sizeof(client_info->client_type) - 1);
        client_info->client_type[sizeof(client_info->client_type) - 1] = '\0';
    }
    
    if (permissions) {
        strncpy(client_info->permissions, permissions, sizeof(client_info->permissions) - 1);
        client_info->permissions[sizeof(client_info->permissions) - 1] = '\0';
    }
    
    jwt_free(jwt);
    printf("[SECURITY] ✅ JWT token validated\n");
    return HAL_STATUS_OK;
}

/**
 * @brief Check rate limit
 * @param client_ip Client IP address
 * @return hal_status_t HAL_STATUS_OK if within limit
 */
hal_status_t security_auth_check_rate_limit(const char *client_ip) {
    if (!g_security_config.rate_limit_enabled || !client_ip) {
        return HAL_STATUS_OK;
    }
    
    time_t now = time(NULL);
    int request_count = 0;
    
    // Count requests in the last minute
    for (size_t i = 0; i < g_rate_limit_count; i++) {
        if (strcmp(g_rate_limits[i].client_ip, client_ip) == 0) {
            if (now - g_rate_limits[i].timestamp < 60) {  // Within 1 minute
                request_count++;
            }
        }
    }
    
    // Check if within limit
    if (request_count >= g_security_config.max_requests_per_minute) {
        printf("[SECURITY] ❌ Rate limit exceeded for IP: %s\n", client_ip);
        return HAL_STATUS_RATE_LIMIT_EXCEEDED;
    }
    
    // Add new request record
    if (g_rate_limit_count < sizeof(g_rate_limits) / sizeof(g_rate_limits[0])) {
        strncpy(g_rate_limits[g_rate_limit_count].client_ip, client_ip, sizeof(g_rate_limits[g_rate_limit_count].client_ip) - 1);
        g_rate_limits[g_rate_limit_count].client_ip[sizeof(g_rate_limits[g_rate_limit_count].client_ip) - 1] = '\0';
        g_rate_limits[g_rate_limit_count].timestamp = now;
        g_rate_limit_count++;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Check if client has required permission
 * @param client_info Client information
 * @param required_permission Required permission
 * @return hal_status_t HAL_STATUS_OK if authorized
 */
hal_status_t security_auth_check_permission(const client_info_t *client_info, const char *required_permission) {
    if (!client_info || !required_permission) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check permissions
    if (strcmp(client_info->permissions, "admin") == 0) {
        // Admin has all permissions
        return HAL_STATUS_OK;
    }
    
    if (strcmp(required_permission, "read_only") == 0) {
        // Everyone can read
        return HAL_STATUS_OK;
    }
    
    if (strcmp(required_permission, "read_write") == 0) {
        // Only read_write and admin can write
        if (strcmp(client_info->permissions, "read_write") == 0) {
            return HAL_STATUS_OK;
        }
    }
    
    printf("[SECURITY] ❌ Permission denied: %s (required: %s)\n", client_info->permissions, required_permission);
    return HAL_STATUS_FORBIDDEN;
}

/**
 * @brief Generate JWT token for client
 * @param client_info Client information
 * @param token_buffer Buffer to store token
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success
 */
hal_status_t security_auth_generate_jwt(const client_info_t *client_info, char *token_buffer, size_t buffer_size) {
    if (!client_info || !token_buffer || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    jwt_t *jwt = NULL;
    jwt_new(&jwt);
    
    // Set claims
    jwt_set_grant(jwt, "client_type", client_info->client_type);
    jwt_set_grant(jwt, "permissions", client_info->permissions);
    jwt_set_grant_int(jwt, "iat", time(NULL));
    jwt_set_grant_int(jwt, "exp", time(NULL) + g_security_config.jwt_expiry_seconds);
    
    // Sign and encode
    char *encoded = jwt_encode_str(jwt);
    if (!encoded) {
        jwt_free(jwt);
        return HAL_STATUS_ERROR;
    }
    
    // Copy to buffer
    strncpy(token_buffer, encoded, buffer_size - 1);
    token_buffer[buffer_size - 1] = '\0';
    
    free(encoded);
    jwt_free(jwt);
    
    printf("[SECURITY] ✅ JWT token generated for client: %s\n", client_info->client_type);
    return HAL_STATUS_OK;
}

/**
 * @brief Extract client IP from request
 * @param request HTTP request
 * @param client_ip Buffer for client IP
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success
 */
hal_status_t security_auth_extract_client_ip(const api_mgr_http_request_t *request, char *client_ip, size_t buffer_size) {
    if (!request || !client_ip || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Try to get IP from X-Forwarded-For header first
    const char *forwarded_for = NULL;
    for (size_t i = 0; i < request->header_count; i++) {
        if (strcasecmp(request->headers[i].name, "X-Forwarded-For") == 0) {
            forwarded_for = request->headers[i].value;
            break;
        }
    }
    
    if (forwarded_for) {
        strncpy(client_ip, forwarded_for, buffer_size - 1);
    } else {
        // Use remote address
        strncpy(client_ip, request->remote_addr, buffer_size - 1);
    }
    
    client_ip[buffer_size - 1] = '\0';
    return HAL_STATUS_OK;
}

/**
 * @brief Security middleware for API requests
 * @param request HTTP request
 * @param response HTTP response
 * @param required_permission Required permission
 * @return hal_status_t HAL_STATUS_OK if authorized
 */
hal_status_t security_auth_middleware(const api_mgr_http_request_t *request, api_mgr_http_response_t *response, const char *required_permission) {
    if (!request || !response) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Extract client IP
    char client_ip[64];
    if (security_auth_extract_client_ip(request, client_ip, sizeof(client_ip)) != HAL_STATUS_OK) {
        strncpy(client_ip, "unknown", sizeof(client_ip) - 1);
        client_ip[sizeof(client_ip) - 1] = '\0';
    }
    
    // Check rate limit
    if (security_auth_check_rate_limit(client_ip) != HAL_STATUS_OK) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_TOO_MANY_REQUESTS,
            "Rate limit exceeded");
    }
    
    // Check for API key or JWT token
    const char *auth_header = NULL;
    const char *api_key = NULL;
    
    // Look for Authorization header
    for (size_t i = 0; i < request->header_count; i++) {
        if (strcasecmp(request->headers[i].name, "Authorization") == 0) {
            auth_header = request->headers[i].value;
            break;
        }
        if (strcasecmp(request->headers[i].name, "X-API-Key") == 0) {
            api_key = request->headers[i].value;
            break;
        }
    }
    
    client_info_t client_info = {0};
    hal_status_t auth_result = HAL_STATUS_UNAUTHORIZED;
    
    // Try API key first
    if (api_key) {
        auth_result = security_auth_validate_api_key(api_key, &client_info);
    }
    
    // Try JWT token if API key failed
    if (auth_result != HAL_STATUS_OK && auth_header) {
        if (strncmp(auth_header, "Bearer ", 7) == 0) {
            const char *token = auth_header + 7;
            auth_result = security_auth_validate_jwt(token, &client_info);
        }
    }
    
    if (auth_result != HAL_STATUS_OK) {
        printf("[SECURITY] ❌ Authentication failed for IP: %s\n", client_ip);
        return api_manager_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED,
            "Authentication required");
    }
    
    // Check permissions
    if (required_permission) {
        if (security_auth_check_permission(&client_info, required_permission) != HAL_STATUS_OK) {
            return api_manager_create_error_response(response, API_MGR_RESPONSE_FORBIDDEN,
                "Insufficient permissions");
        }
    }
    
    printf("[SECURITY] ✅ Request authorized for client: %s (IP: %s)\n", client_info.client_type, client_ip);
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize security system
 * @return hal_status_t HAL_STATUS_OK on success
 */
hal_status_t security_auth_deinit(void) {
    printf("[SECURITY] Deinitializing authentication system...\n");
    
    // Clear rate limiting data
    memset(g_rate_limits, 0, sizeof(g_rate_limits));
    g_rate_limit_count = 0;
    
    printf("[SECURITY] ✅ Authentication system deinitialized\n");
    return HAL_STATUS_OK;
}
