/**
 * @file api_auth.h
 * @brief API Authentication & Authorization for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.1
 */

#ifndef API_AUTH_H
#define API_AUTH_H

#include "hal_common.h"
#include "api_manager.h"

// Authentication Management Version
#define API_AUTH_VERSION_MAJOR 1
#define API_AUTH_VERSION_MINOR 0
#define API_AUTH_VERSION_PATCH 0
#define API_AUTH_VERSION_STRING "1.0.0"

// Authentication Limits
#define API_AUTH_MAX_USERS 32
#define API_AUTH_MAX_SESSIONS 64
#define API_AUTH_MAX_USERNAME_LEN 32
#define API_AUTH_MAX_PASSWORD_LEN 128
#define API_AUTH_MAX_SESSION_ID_LEN 64
#define API_AUTH_MAX_TOKEN_LEN 256
#define API_AUTH_MAX_MESSAGE_LEN 256

// Authentication Types
typedef enum {
    API_AUTH_TYPE_NONE = 0,
    API_AUTH_TYPE_BASIC,
    API_AUTH_TYPE_TOKEN,
    API_AUTH_TYPE_JWT,
    API_AUTH_TYPE_MAX
} api_auth_type_t;

// User Roles
typedef enum {
    API_USER_ROLE_GUEST = 0,
    API_USER_ROLE_OPERATOR,
    API_USER_ROLE_ADMIN,
    API_USER_ROLE_MAINTENANCE,
    API_USER_ROLE_MAX
} api_user_role_t;

// Permission Levels
typedef enum {
    API_PERMISSION_READ = 0x01,
    API_PERMISSION_WRITE = 0x02,
    API_PERMISSION_ADMIN = 0x04,
    API_PERMISSION_MAINTENANCE = 0x08,
    API_PERMISSION_ALL = 0xFF
} api_permission_t;

// Password Policy Structure
typedef struct {
    uint32_t min_length;
    bool require_uppercase;
    bool require_lowercase;
    bool require_numbers;
    bool require_special;
} api_password_policy_t;

// User Structure
typedef struct {
    char username[32];                // Username
    char password_hash[64];           // Password hash (SHA-256)
    api_user_role_t role;             // User role
    uint32_t permissions;             // Permission bitmap
    bool active;                      // Active status
    uint64_t last_login;              // Last login timestamp
    uint64_t created_at;              // Creation timestamp
    uint32_t failed_attempts;         // Failed login attempts
    uint64_t locked_until;            // Lockout timestamp
} api_user_t;

// Session Structure
typedef struct {
    char session_id[64];              // Session identifier
    api_user_t user;                  // User information
    uint64_t created_at;              // Session creation time
    uint64_t expires_at;              // Session expiration time
    uint64_t last_activity;           // Last activity timestamp
    char client_ip[16];               // Client IP address
    bool active;                      // Session active status
} api_session_t;

// Authentication Configuration
typedef struct {
    bool auth_enabled;                // Enable authentication
    api_auth_type_t auth_type;        // Authentication type
    uint32_t session_timeout_ms;      // Session timeout in milliseconds
    uint32_t max_failed_attempts;     // Maximum failed login attempts
    uint32_t lockout_duration_ms;     // Lockout duration in milliseconds
    bool require_https;               // Require HTTPS
    char jwt_secret[64];              // JWT secret key
    uint32_t jwt_expiry_ms;           // JWT expiration time
} api_auth_config_t;

// Authentication Request Structure
typedef struct {
    char username[32];                // Username
    char password[64];                // Password
    char client_ip[16];               // Client IP address
    char user_agent[128];             // User agent string
} api_auth_request_t;

// Authentication Response Structure
typedef struct {
    bool success;                     // Authentication success
    char message[256];                // Response message
    char session_id[64];              // Session ID (if successful)
    char token[256];                  // JWT token (if using JWT)
    uint64_t expires_at;              // Expiration timestamp
    api_user_role_t role;             // User role
    uint32_t permissions;             // User permissions
} api_auth_response_t;

// Authorization Request Structure
typedef struct {
    char session_id[64];              // Session ID
    char resource[64];                // Resource being accessed
    api_permission_t required_permission; // Required permission
    char client_ip[16];               // Client IP address
} api_authz_request_t;

// Authorization Response Structure
typedef struct {
    bool authorized;                  // Authorization result
    char message[256];                // Response message
    api_user_role_t role;             // User role
    uint32_t permissions;             // User permissions
} api_authz_response_t;

// Login request structure
typedef struct {
    char username[64];
    char password[128];
    bool remember_me;
} api_login_request_t;

// Login response structure
typedef struct {
    bool success;
    char session_id[65];
    api_user_t user;
    uint64_t expires_at;
    char message[256];
} api_login_response_t;

// Change password request structure
typedef struct {
    char current_password[128];
    char new_password[128];
    char confirm_password[128];
} api_change_password_request_t;

// Change password response structure
typedef struct {
    bool success;
    char message[256];
} api_change_password_response_t;

// Users list structure
typedef struct {
    api_user_t users[API_AUTH_MAX_USERS];
    uint32_t user_count;
} api_users_list_t;

// Sessions list structure
typedef struct {
    api_session_t sessions[API_AUTH_MAX_SESSIONS];
    uint32_t session_count;
} api_sessions_list_t;

// Authentication status structure
typedef struct {
    bool authenticated;
    uint32_t session_count;
    uint32_t active_users;
    uint32_t max_sessions;
    uint32_t session_timeout;
    api_password_policy_t password_policy;
} api_auth_status_t;

// Authentication Statistics
typedef struct {
    uint32_t total_users;             // Total number of users
    uint32_t active_sessions;         // Active sessions
    uint32_t total_logins;            // Total login attempts
    uint32_t failed_logins;           // Failed login attempts
    uint32_t locked_accounts;         // Locked accounts
    uint64_t last_activity;           // Last activity timestamp
} api_auth_stats_t;

// Authentication Management Functions

/**
 * @brief Initialize authentication system
 * @param config Authentication configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_init(const api_auth_config_t *config);

/**
 * @brief Deinitialize authentication system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_deinit(void);

/**
 * @brief Authenticate user
 * @param request Authentication request
 * @param response Authentication response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_authenticate(const api_auth_request_t *request, api_auth_response_t *response);

/**
 * @brief Authorize user access
 * @param request Authorization request
 * @param response Authorization response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_authorize(const api_authz_request_t *request, api_authz_response_t *response);

/**
 * @brief Create user session
 * @param username Username
 * @param client_ip Client IP address
 * @param session_id Session ID (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_create_session(const char *username, const char *client_ip, char *session_id);

/**
 * @brief Validate session
 * @param session_id Session ID
 * @param session Session information (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_validate_session(const char *session_id, api_session_t *session);

/**
 * @brief Destroy session
 * @param session_id Session ID
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_destroy_session(const char *session_id);

/**
 * @brief Refresh session
 * @param session_id Session ID
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_refresh_session(const char *session_id);

/**
 * @brief Create user
 * @param user User information
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_create_user(const api_user_t *user);

/**
 * @brief Update user
 * @param username Username
 * @param user Updated user information
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_update_user(const char *username, const api_user_t *user);

/**
 * @brief Delete user
 * @param username Username
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_delete_user(const char *username);

/**
 * @brief Get user information
 * @param username Username
 * @param user User information (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_get_user(const char *username, api_user_t *user);

/**
 * @brief List all users
 * @param users Array of user information
 * @param max_users Maximum number of users
 * @param actual_count Actual number of users returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_list_users(api_user_t *users, uint32_t max_users, uint32_t *actual_count);

/**
 * @brief Get authentication statistics
 * @param stats Authentication statistics
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_get_stats(api_auth_stats_t *stats);

/**
 * @brief Get authentication configuration
 * @param config Authentication configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_get_config(api_auth_config_t *config);

/**
 * @brief Set authentication configuration
 * @param config Authentication configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_set_config(const api_auth_config_t *config);

// Authentication Endpoint Handlers

/**
 * @brief Handle POST /api/v1/auth/login - User login
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_login(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/auth/logout - User logout
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_logout(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/auth/session - Get session information
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_session(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/auth/refresh - Refresh session
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_refresh(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/auth/users - List users
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_users(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/auth/users - Create user
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_create_user(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle PUT /api/v1/auth/users/{username} - Update user
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_update_user(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle DELETE /api/v1/auth/users/{username} - Delete user
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_delete_user(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/auth/stats - Get authentication statistics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_auth_stats(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

// Utility Functions

/**
 * @brief Hash password
 * @param password Plain text password
 * @param hash Password hash (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_hash_password(const char *password, char *hash);

/**
 * @brief Verify password
 * @param password Plain text password
 * @param hash Password hash
 * @return hal_status_t HAL_STATUS_OK if valid, error code if invalid
 */
hal_status_t api_auth_verify_password(const char *password, const char *hash);

/**
 * @brief Generate session ID
 * @param session_id Session ID (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_generate_session_id(char *session_id);

/**
 * @brief Generate JWT token
 * @param username Username
 * @param role User role
 * @param token JWT token (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_generate_jwt_token(const char *username, api_user_role_t role, char *token);

/**
 * @brief Validate JWT token
 * @param token JWT token
 * @param username Username (output)
 * @param role User role (output)
 * @return hal_status_t HAL_STATUS_OK if valid, error code if invalid
 */
hal_status_t api_auth_validate_jwt_token(const char *token, char *username, api_user_role_t *role);

/**
 * @brief Get user role name
 * @param role User role
 * @return const char* Role name
 */
const char* api_auth_get_role_name(api_user_role_t role);

/**
 * @brief Get permission name
 * @param permission Permission
 * @return const char* Permission name
 */
const char* api_auth_get_permission_name(api_permission_t permission);

/**
 * @brief Check if user has permission
 * @param user_permissions User permissions bitmap
 * @param required_permission Required permission
 * @return bool true if user has permission, false otherwise
 */
bool api_auth_has_permission(uint32_t user_permissions, api_permission_t required_permission);

// Middleware Functions

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

// JSON Serialization Functions

/**
 * @brief Serialize authentication response to JSON
 * @param response Authentication response
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_auth_response_json(const api_auth_response_t *response, char *buffer, size_t buffer_size);

/**
 * @brief Serialize authorization response to JSON
 * @param response Authorization response
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_authz_response_json(const api_authz_response_t *response, char *buffer, size_t buffer_size);

/**
 * @brief Serialize user information to JSON
 * @param user User information
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_user_json(const api_user_t *user, char *buffer, size_t buffer_size);

/**
 * @brief Serialize session information to JSON
 * @param session Session information
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_session_json(const api_session_t *session, char *buffer, size_t buffer_size);

/**
 * @brief Serialize authentication statistics to JSON
 * @param stats Authentication statistics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_auth_stats_json(const api_auth_stats_t *stats, char *buffer, size_t buffer_size);

#endif // API_AUTH_H
