/**
 * @file api_auth.c
 * @brief API Authentication & Authorization Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.1
 */

#include "api_auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

// Global variables
static bool g_auth_initialized = false;
static api_auth_config_t g_auth_config;
static api_user_t g_users[16];  // Max 16 users
static uint32_t g_user_count = 0;
static api_session_t g_sessions[32];  // Max 32 sessions
static uint32_t g_session_count = 0;
static api_auth_stats_t g_auth_stats = {0};
static pthread_mutex_t g_auth_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint64_t g_auth_start_time = 0;

// Default users
static const api_user_t g_default_users[] = {
    {
        .username = "admin",
        .password_hash = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", // "password"
        .role = API_USER_ROLE_ADMIN,
        .permissions = API_PERMISSION_ALL,
        .active = true,
        .last_login = 0,
        .created_at = 0,
        .failed_attempts = 0,
        .locked_until = 0
    },
    {
        .username = "operator",
        .password_hash = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", // "password"
        .role = API_USER_ROLE_OPERATOR,
        .permissions = API_PERMISSION_READ | API_PERMISSION_WRITE,
        .active = true,
        .last_login = 0,
        .created_at = 0,
        .failed_attempts = 0,
        .locked_until = 0
    },
    {
        .username = "guest",
        .password_hash = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", // "password"
        .role = API_USER_ROLE_GUEST,
        .permissions = API_PERMISSION_READ,
        .active = true,
        .last_login = 0,
        .created_at = 0,
        .failed_attempts = 0,
        .locked_until = 0
    }
};

#define DEFAULT_USERS_COUNT (sizeof(g_default_users) / sizeof(g_default_users[0]))

/**
 * @brief Initialize authentication system
 * @param config Authentication configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_init(const api_auth_config_t *config) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Auth: Initializing...");
    
    if (g_auth_initialized) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Auth: Already initialized");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize configuration
    g_auth_config = *config;
    
    // Initialize users with defaults
    g_user_count = DEFAULT_USERS_COUNT;
    memcpy(g_users, g_default_users, sizeof(g_default_users));
    
    // Set creation timestamps
    uint64_t current_time = hal_get_timestamp_ms();
    for (uint32_t i = 0; i < g_user_count; i++) {
        g_users[i].created_at = current_time;
    }
    
    // Initialize sessions
    memset(g_sessions, 0, sizeof(g_sessions));
    g_session_count = 0;
    
    // Initialize statistics
    memset(&g_auth_stats, 0, sizeof(g_auth_stats));
    g_auth_stats.total_users = g_user_count;
    g_auth_start_time = current_time;
    
    g_auth_initialized = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Auth: Initialized with %u users", g_user_count);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize authentication system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Auth: Deinitializing...");
    
    if (!g_auth_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Clear all sessions
    memset(g_sessions, 0, sizeof(g_sessions));
    g_session_count = 0;
    
    g_auth_initialized = false;
    g_user_count = 0;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Auth: Deinitialized");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Hash password
 * @param password Plain text password
 * @param hash Password hash (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_hash_password(const char *password, char *hash) {
    if (password == NULL || hash == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    
    if (SHA256_Init(&ctx) != 1) {
        return HAL_STATUS_ERROR;
    }
    
    if (SHA256_Update(&ctx, password, strlen(password)) != 1) {
        return HAL_STATUS_ERROR;
    }
    
    if (SHA256_Final(digest, &ctx) != 1) {
        return HAL_STATUS_ERROR;
    }
    
    // Convert to hex string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hash + (i * 2), "%02x", digest[i]);
    }
    hash[SHA256_DIGEST_LENGTH * 2] = '\0';
    
    return HAL_STATUS_OK;
}

/**
 * @brief Verify password
 * @param password Plain text password
 * @param hash Password hash
 * @return hal_status_t HAL_STATUS_OK if valid, error code if invalid
 */
hal_status_t api_auth_verify_password(const char *password, const char *hash) {
    if (password == NULL || hash == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    char computed_hash[65];
    hal_status_t result = api_auth_hash_password(password, computed_hash);
    
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    return (strcmp(computed_hash, hash) == 0) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

/**
 * @brief Generate session ID
 * @param session_id Session ID (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_generate_session_id(char *session_id) {
    if (session_id == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    unsigned char random_bytes[32];
    if (RAND_bytes(random_bytes, sizeof(random_bytes)) != 1) {
        return HAL_STATUS_ERROR;
    }
    
    // Convert to hex string
    for (int i = 0; i < 32; i++) {
        sprintf(session_id + (i * 2), "%02x", random_bytes[i]);
    }
    session_id[64] = '\0';
    
    return HAL_STATUS_OK;
}

/**
 * @brief Authenticate user
 * @param request Authentication request
 * @param response Authentication response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_authenticate(const api_auth_request_t *request, api_auth_response_t *response) {
    if (request == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_auth_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_auth_mutex);
    
    // Find user
    api_user_t *user = NULL;
    for (uint32_t i = 0; i < g_user_count; i++) {
        if (strcmp(g_users[i].username, request->username) == 0) {
            user = &g_users[i];
            break;
        }
    }
    
    if (user == NULL) {
        response->success = false;
        strcpy(response->message, "Invalid username or password");
        g_auth_stats.failed_logins++;
        pthread_mutex_unlock(&g_auth_mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Check if account is locked
    uint64_t current_time = hal_get_timestamp_ms();
    if (user->locked_until > current_time) {
        response->success = false;
        strcpy(response->message, "Account is locked");
        g_auth_stats.failed_logins++;
        pthread_mutex_unlock(&g_auth_mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Check if account is active
    if (!user->active) {
        response->success = false;
        strcpy(response->message, "Account is disabled");
        g_auth_stats.failed_logins++;
        pthread_mutex_unlock(&g_auth_mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Verify password
    hal_status_t password_result = api_auth_verify_password(request->password, user->password_hash);
    
    if (password_result != HAL_STATUS_OK) {
        user->failed_attempts++;
        if (user->failed_attempts >= g_auth_config.max_failed_attempts) {
            user->locked_until = current_time + g_auth_config.lockout_duration_ms;
            strcpy(response->message, "Account locked due to too many failed attempts");
        } else {
            strcpy(response->message, "Invalid username or password");
        }
        response->success = false;
        g_auth_stats.failed_logins++;
        pthread_mutex_unlock(&g_auth_mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Authentication successful
    user->failed_attempts = 0;
    user->locked_until = 0;
    user->last_login = current_time;
    
    // Create session
    char session_id[65];
    hal_status_t session_result = api_auth_create_session(request->username, request->client_ip, session_id);
    
    if (session_result != HAL_STATUS_OK) {
        response->success = false;
        strcpy(response->message, "Failed to create session");
        pthread_mutex_unlock(&g_auth_mutex);
        return session_result;
    }
    
    // Update statistics
    g_auth_stats.total_logins++;
    g_auth_stats.last_activity = current_time;
    
    // Fill response
    response->success = true;
    strcpy(response->message, "Authentication successful");
    strcpy(response->session_id, session_id);
    response->expires_at = current_time + g_auth_config.session_timeout_ms;
    response->role = user->role;
    response->permissions = user->permissions;
    
    pthread_mutex_unlock(&g_auth_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Auth: User '%s' authenticated successfully", request->username);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create user session
 * @param username Username
 * @param client_ip Client IP address
 * @param session_id Session ID (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_create_session(const char *username, const char *client_ip, char *session_id) {
    if (username == NULL || client_ip == NULL || session_id == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_auth_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_session_count >= 32) {
        return HAL_STATUS_ERROR; // Max sessions reached
    }
    
    // Find user
    api_user_t *user = NULL;
    for (uint32_t i = 0; i < g_user_count; i++) {
        if (strcmp(g_users[i].username, username) == 0) {
            user = &g_users[i];
            break;
        }
    }
    
    if (user == NULL) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    // Generate session ID
    hal_status_t result = api_auth_generate_session_id(session_id);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Create session
    uint64_t current_time = hal_get_timestamp_ms();
    api_session_t *session = &g_sessions[g_session_count];
    
    strcpy(session->session_id, session_id);
    strcpy(session->user.username, username);
    session->user.role = user->role;
    session->user.permissions = user->permissions;
    session->created_at = current_time;
    session->expires_at = current_time + g_auth_config.session_timeout_ms;
    session->last_activity = current_time;
    strcpy(session->client_ip, client_ip);
    session->active = true;
    
    g_session_count++;
    g_auth_stats.active_sessions = g_session_count;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate session
 * @param session_id Session ID
 * @param session Session information (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_validate_session(const char *session_id, api_session_t *session) {
    if (session_id == NULL || session == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_auth_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_auth_mutex);
    
    // Find session
    for (uint32_t i = 0; i < g_session_count; i++) {
        if (strcmp(g_sessions[i].session_id, session_id) == 0 && g_sessions[i].active) {
            uint64_t current_time = hal_get_timestamp_ms();
            
            // Check if session is expired
            if (g_sessions[i].expires_at <= current_time) {
                g_sessions[i].active = false;
                g_auth_stats.active_sessions--;
                pthread_mutex_unlock(&g_auth_mutex);
                return HAL_STATUS_ERROR; // Session expired
            }
            
            // Update last activity
            g_sessions[i].last_activity = current_time;
            *session = g_sessions[i];
            
            pthread_mutex_unlock(&g_auth_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&g_auth_mutex);
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Authorize user access
 * @param request Authorization request
 * @param response Authorization response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_authorize(const api_authz_request_t *request, api_authz_response_t *response) {
    if (request == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_auth_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Validate session
    api_session_t session;
    hal_status_t session_result = api_auth_validate_session(request->session_id, &session);
    
    if (session_result != HAL_STATUS_OK) {
        response->authorized = false;
        strcpy(response->message, "Invalid or expired session");
        return session_result;
    }
    
    // Check permissions
    bool has_permission = api_auth_has_permission(session.user.permissions, request->required_permission);
    
    if (!has_permission) {
        response->authorized = false;
        strcpy(response->message, "Insufficient permissions");
        response->role = session.user.role;
        response->permissions = session.user.permissions;
        return HAL_STATUS_ERROR;
    }
    
    // Authorization successful
    response->authorized = true;
    strcpy(response->message, "Access authorized");
    response->role = session.user.role;
    response->permissions = session.user.permissions;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Destroy session
 * @param session_id Session ID
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_destroy_session(const char *session_id) {
    if (session_id == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_auth_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_auth_mutex);
    
    // Find and deactivate session
    for (uint32_t i = 0; i < g_session_count; i++) {
        if (strcmp(g_sessions[i].session_id, session_id) == 0 && g_sessions[i].active) {
            g_sessions[i].active = false;
            g_auth_stats.active_sessions--;
            pthread_mutex_unlock(&g_auth_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&g_auth_mutex);
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Get user role name
 * @param role User role
 * @return const char* Role name
 */
const char* api_auth_get_role_name(api_user_role_t role) {
    switch (role) {
        case API_USER_ROLE_GUEST: return "guest";
        case API_USER_ROLE_OPERATOR: return "operator";
        case API_USER_ROLE_ADMIN: return "admin";
        case API_USER_ROLE_MAINTENANCE: return "maintenance";
        default: return "unknown";
    }
}

/**
 * @brief Get permission name
 * @param permission Permission
 * @return const char* Permission name
 */
const char* api_auth_get_permission_name(api_permission_t permission) {
    switch (permission) {
        case API_PERMISSION_READ: return "read";
        case API_PERMISSION_WRITE: return "write";
        case API_PERMISSION_ADMIN: return "admin";
        case API_PERMISSION_MAINTENANCE: return "maintenance";
        case API_PERMISSION_ALL: return "all";
        default: return "unknown";
    }
}

/**
 * @brief Check if user has permission
 * @param user_permissions User permissions bitmap
 * @param required_permission Required permission
 * @return bool true if user has permission, false otherwise
 */
bool api_auth_has_permission(uint32_t user_permissions, api_permission_t required_permission) {
    return (user_permissions & required_permission) != 0;
}

/**
 * @brief Get authentication statistics
 * @param stats Authentication statistics
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_get_stats(api_auth_stats_t *stats) {
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_auth_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_auth_mutex);
    
    *stats = g_auth_stats;
    
    // Count locked accounts
    stats->locked_accounts = 0;
    uint64_t current_time = hal_get_timestamp_ms();
    for (uint32_t i = 0; i < g_user_count; i++) {
        if (g_users[i].locked_until > current_time) {
            stats->locked_accounts++;
        }
    }
    
    pthread_mutex_unlock(&g_auth_mutex);
    
    return HAL_STATUS_OK;
}

// Placeholder functions for future implementation
hal_status_t api_auth_refresh_session(const char *session_id) {
    (void)session_id; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_auth_create_user(const api_user_t *user) {
    (void)user; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_auth_update_user(const char *username, const api_user_t *user) {
    (void)username; // Unused parameter
    (void)user; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_auth_delete_user(const char *username) {
    (void)username; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_auth_get_user(const char *username, api_user_t *user) {
    (void)username; // Unused parameter
    (void)user; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_auth_list_users(api_user_t *users, uint32_t max_users, uint32_t *actual_count) {
    (void)users; // Unused parameter
    (void)max_users; // Unused parameter
    (void)actual_count; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_auth_get_config(api_auth_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *config = g_auth_config;
    return HAL_STATUS_OK;
}

hal_status_t api_auth_set_config(const api_auth_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    g_auth_config = *config;
    return HAL_STATUS_OK;
}

hal_status_t api_auth_generate_jwt_token(const char *username, api_user_role_t role, char *token) {
    (void)username; // Unused parameter
    (void)role; // Unused parameter
    (void)token; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_auth_validate_jwt_token(const char *token, char *username, api_user_role_t *role) {
    (void)token; // Unused parameter
    (void)username; // Unused parameter
    (void)role; // Unused parameter
    return HAL_STATUS_OK;
}
