/**
 * @file security_manager.h
 * @brief Security Manager for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-05 (Security Framework)
 */

#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// Security Manager Configuration
#define SECURITY_MGR_MAX_USERS          10
#define SECURITY_MGR_MAX_ROLES          5
#define SECURITY_MGR_MAX_PERMISSIONS    20
#define SECURITY_MGR_MAX_SESSIONS       5
#define SECURITY_MGR_SESSION_TIMEOUT    3600000  // 1 hour in ms
#define SECURITY_MGR_TOKEN_LENGTH       32
#define SECURITY_MGR_MAX_CERTIFICATES   5

// Security Manager Authentication Methods
typedef enum {
    SECURITY_MGR_AUTH_NONE = 0,
    SECURITY_MGR_AUTH_PASSWORD,
    SECURITY_MGR_AUTH_TOKEN,
    SECURITY_MGR_AUTH_CERTIFICATE,
    SECURITY_MGR_AUTH_MULTI_FACTOR
} security_mgr_auth_method_t;

// Security Manager Authorization Levels
typedef enum {
    SECURITY_MGR_LEVEL_GUEST = 0,
    SECURITY_MGR_LEVEL_OPERATOR,
    SECURITY_MGR_LEVEL_MAINTENANCE,
    SECURITY_MGR_LEVEL_ADMIN,
    SECURITY_MGR_LEVEL_SUPER_ADMIN
} security_mgr_level_t;

// Security Manager Permission Types
typedef enum {
    SECURITY_MGR_PERM_READ = 0,
    SECURITY_MGR_PERM_WRITE,
    SECURITY_MGR_PERM_EXECUTE,
    SECURITY_MGR_PERM_CONFIGURE,
    SECURITY_MGR_PERM_ADMIN
} security_mgr_permission_t;

// Security Manager Resource Types
typedef enum {
    SECURITY_MGR_RESOURCE_SYSTEM = 0,
    SECURITY_MGR_RESOURCE_NETWORK,
    SECURITY_MGR_RESOURCE_COMMUNICATION,
    SECURITY_MGR_RESOURCE_SAFETY,
    SECURITY_MGR_RESOURCE_CONFIGURATION
} security_mgr_resource_t;

// Security Manager Session States
typedef enum {
    SECURITY_MGR_SESSION_INACTIVE = 0,
    SECURITY_MGR_SESSION_ACTIVE,
    SECURITY_MGR_SESSION_EXPIRED,
    SECURITY_MGR_SESSION_LOCKED
} security_mgr_session_state_t;

// Security Manager SSL/TLS Configuration
typedef struct {
    bool ssl_enabled;
    bool tls_enabled;
    uint16_t tls_version;
    char certificate_path[128];
    char private_key_path[128];
    char ca_certificate_path[128];
    bool verify_peer;
    bool verify_hostname;
} security_mgr_ssl_config_t;

// Security Manager User Configuration
typedef struct {
    char username[32];
    char password_hash[64];
    security_mgr_level_t level;
    security_mgr_auth_method_t auth_method;
    bool enabled;
    uint32_t login_attempts;
    uint64_t last_login_time;
    uint64_t password_expiry_time;
} security_mgr_user_config_t;

// Security Manager Role Configuration
typedef struct {
    char role_name[32];
    security_mgr_level_t level;
    uint32_t permissions;
    bool enabled;
} security_mgr_role_config_t;

// Security Manager Session Information
typedef struct {
    char session_id[SECURITY_MGR_TOKEN_LENGTH];
    char username[32];
    security_mgr_level_t level;
    security_mgr_session_state_t state;
    uint64_t created_time;
    uint64_t last_activity_time;
    uint64_t expiry_time;
    char client_ip[16];
} security_mgr_session_t;

// Security Manager Configuration
typedef struct {
    security_mgr_ssl_config_t ssl_config;
    security_mgr_user_config_t users[SECURITY_MGR_MAX_USERS];
    security_mgr_role_config_t roles[SECURITY_MGR_MAX_ROLES];
    uint32_t session_timeout_ms;
    uint32_t max_login_attempts;
    bool auto_lock_enabled;
    uint32_t auto_lock_timeout_ms;
    bool audit_logging_enabled;
    bool encryption_enabled;
} security_mgr_config_t;

// Security Manager Status
typedef struct {
    bool initialized;
    bool ssl_active;
    bool tls_active;
    uint32_t active_sessions;
    uint32_t failed_login_attempts;
    uint32_t security_violations;
    uint64_t last_security_check;
    security_mgr_level_t current_level;
} security_mgr_status_t;

// Security Manager Statistics
typedef struct {
    uint32_t total_logins;
    uint32_t successful_logins;
    uint32_t failed_logins;
    uint32_t session_creations;
    uint32_t session_expirations;
    uint32_t permission_denials;
    uint32_t security_events;
    uint64_t uptime;
} security_mgr_stats_t;

// Security Manager Event Types
typedef enum {
    SECURITY_MGR_EVENT_NONE = 0,
    SECURITY_MGR_EVENT_LOGIN_SUCCESS,
    SECURITY_MGR_EVENT_LOGIN_FAILED,
    SECURITY_MGR_EVENT_LOGOUT,
    SECURITY_MGR_EVENT_SESSION_EXPIRED,
    SECURITY_MGR_EVENT_PERMISSION_DENIED,
    SECURITY_MGR_EVENT_SSL_HANDSHAKE,
    SECURITY_MGR_EVENT_CERTIFICATE_EXPIRED,
    SECURITY_MGR_EVENT_SECURITY_VIOLATION
} security_mgr_event_t;

// Security Manager Event Callback
typedef void (*security_mgr_event_callback_t)(security_mgr_event_t event, 
                                            const char *username, 
                                            const void *data);

// Security Manager API Functions

// Initialization and Configuration
hal_status_t security_manager_init(const security_mgr_config_t *config);
hal_status_t security_manager_deinit(void);
hal_status_t security_manager_get_config(security_mgr_config_t *config);
hal_status_t security_manager_set_config(const security_mgr_config_t *config);

// Status and Statistics
hal_status_t security_manager_get_status(security_mgr_status_t *status);
hal_status_t security_manager_get_statistics(security_mgr_stats_t *stats);
hal_status_t security_manager_reset_statistics(void);

// Authentication and Authorization
hal_status_t security_manager_authenticate_user(const char *username, 
                                               const char *password, 
                                               char *session_id);
hal_status_t security_manager_validate_session(const char *session_id, 
                                              security_mgr_level_t *level);
hal_status_t security_manager_logout_user(const char *session_id);
hal_status_t security_manager_check_permission(const char *session_id, 
                                              security_mgr_resource_t resource, 
                                              security_mgr_permission_t permission);

// User Management
hal_status_t security_manager_add_user(const security_mgr_user_config_t *user);
hal_status_t security_manager_remove_user(const char *username);
hal_status_t security_manager_update_user(const security_mgr_user_config_t *user);
hal_status_t security_manager_get_user(const char *username, 
                                      security_mgr_user_config_t *user);

// Role Management
hal_status_t security_manager_add_role(const security_mgr_role_config_t *role);
hal_status_t security_manager_remove_role(const char *role_name);
hal_status_t security_manager_update_role(const security_mgr_role_config_t *role);
hal_status_t security_manager_get_role(const char *role_name, 
                                      security_mgr_role_config_t *role);

// SSL/TLS Management
hal_status_t security_manager_enable_ssl(const security_mgr_ssl_config_t *ssl_config);
hal_status_t security_manager_disable_ssl(void);
hal_status_t security_manager_get_ssl_config(security_mgr_ssl_config_t *ssl_config);
hal_status_t security_manager_verify_certificate(const char *certificate_path);

// Session Management
hal_status_t security_manager_get_active_sessions(security_mgr_session_t *sessions, 
                                                 uint32_t *count);
hal_status_t security_manager_terminate_session(const char *session_id);
hal_status_t security_manager_terminate_all_sessions(void);

// Event Handling
hal_status_t security_manager_set_callback(security_mgr_event_callback_t callback);
hal_status_t security_manager_get_diagnostics(char *diagnostics, uint32_t max_length);

// Utility Functions
const char* security_manager_get_auth_method_name(security_mgr_auth_method_t method);
const char* security_manager_get_level_name(security_mgr_level_t level);
const char* security_manager_get_permission_name(security_mgr_permission_t permission);
const char* security_manager_get_resource_name(security_mgr_resource_t resource);
const char* security_manager_get_event_name(security_mgr_event_t event);

#endif // SECURITY_MANAGER_H
