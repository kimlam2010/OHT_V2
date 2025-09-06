/**
 * @file api_auth_json.c
 * @brief API Authentication JSON Utilities for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.1
 */

#include "api_auth.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Serialize login request to JSON
 * @param request Login request
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_login_request_json(const api_login_request_t *request, char *buffer, size_t buffer_size) {
    if (request == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"username\":\"%s\","
        "\"password\":\"%s\","
        "\"remember_me\":%s"
        "}",
        request->username,
        request->password,
        request->remember_me ? "true" : "false"
    );
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Parse login request from JSON
 * @param json JSON string
 * @param request Login request (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_parse_login_request_json(const char *json, api_login_request_t *request) {
    if (json == NULL || request == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Simple JSON parsing (in production, use a proper JSON library)
    memset(request, 0, sizeof(api_login_request_t));
    
    // Find username field
    const char *username_start = strstr(json, "\"username\":\"");
    if (username_start != NULL) {
        username_start += 12; // Skip "\"username\":\""
        const char *username_end = strchr(username_start, '"');
        if (username_end != NULL) {
            size_t username_len = username_end - username_start;
            if (username_len < sizeof(request->username)) {
                strncpy(request->username, username_start, username_len);
                request->username[username_len] = '\0';
            }
        }
    }
    
    // Find password field
    const char *password_start = strstr(json, "\"password\":\"");
    if (password_start != NULL) {
        password_start += 12; // Skip "\"password\":\""
        const char *password_end = strchr(password_start, '"');
        if (password_end != NULL) {
            size_t password_len = password_end - password_start;
            if (password_len < sizeof(request->password)) {
                strncpy(request->password, password_start, password_len);
                request->password[password_len] = '\0';
            }
        }
    }
    
    // Find remember_me field
    const char *remember_start = strstr(json, "\"remember_me\":");
    if (remember_start != NULL) {
        remember_start += 14; // Skip "\"remember_me\":"
        if (strncmp(remember_start, "true", 4) == 0) {
            request->remember_me = true;
        } else {
            request->remember_me = false;
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize login response to JSON
 * @param response Login response
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_login_response_json(const api_login_response_t *response, char *buffer, size_t buffer_size) {
    if (response == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"success\":%s,"
        "\"session_id\":\"%s\","
        "\"user\":{"
            "\"username\":\"%s\","
            "\"role\":\"%s\","
            "\"permissions\":%u"
        "},"
        "\"expires_at\":%lu,"
        "\"message\":\"%s\""
        "}",
        response->success ? "true" : "false",
        response->session_id,
        response->user.username,
        api_auth_get_role_name(response->user.role),
        response->user.permissions,
        response->expires_at,
        response->message
    );
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Serialize user information to JSON
 * @param user User information
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_user_json(const api_user_t *user, char *buffer, size_t buffer_size) {
    if (user == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"username\":\"%s\","
        "\"role\":\"%s\","
        "\"permissions\":%u,"
        "\"created_at\":%lu,"
        "\"last_login\":%lu,"
        "\"active\":%s"
        "}",
        user->username,
        api_auth_get_role_name(user->role),
        user->permissions,
        user->created_at,
        user->last_login,
        user->active ? "true" : "false"
    );
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Serialize session information to JSON
 * @param session Session information
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_session_json(const api_session_t *session, char *buffer, size_t buffer_size) {
    if (session == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"session_id\":\"%s\","
        "\"user\":{"
            "\"username\":\"%s\","
            "\"role\":\"%s\","
            "\"permissions\":%u"
        "},"
        "\"created_at\":%lu,"
        "\"expires_at\":%lu,"
        "\"last_activity\":%lu,"
        "\"active\":%s"
        "}",
        session->session_id,
        session->user.username,
        api_auth_get_role_name(session->user.role),
        session->user.permissions,
        session->created_at,
        session->expires_at,
        session->last_activity,
        session->active ? "true" : "false"
    );
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Serialize user list to JSON
 * @param users User list
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_users_list_json(const api_users_list_t *users, char *buffer, size_t buffer_size) {
    if (users == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size, "{\"users\":[");
    
    for (uint32_t i = 0; i < users->user_count && written < (int)buffer_size; i++) {
        if (i > 0) {
            written += snprintf(buffer + written, buffer_size - written, ",");
        }
        
        char user_json[512];
        int user_written = api_auth_serialize_user_json(&users->users[i], user_json, sizeof(user_json));
        
        if (user_written > 0) {
            written += snprintf(buffer + written, buffer_size - written, "%s", user_json);
        }
    }
    
    written += snprintf(buffer + written, buffer_size - written, "],\"total\":%u}", users->user_count);
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Serialize session list to JSON
 * @param sessions Session list
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_sessions_list_json(const api_sessions_list_t *sessions, char *buffer, size_t buffer_size) {
    if (sessions == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size, "{\"sessions\":[");
    
    for (uint32_t i = 0; i < sessions->session_count && written < (int)buffer_size; i++) {
        if (i > 0) {
            written += snprintf(buffer + written, buffer_size - written, ",");
        }
        
        char session_json[1024];
        int session_written = api_auth_serialize_session_json(&sessions->sessions[i], session_json, sizeof(session_json));
        
        if (session_written > 0) {
            written += snprintf(buffer + written, buffer_size - written, "%s", session_json);
        }
    }
    
    written += snprintf(buffer + written, buffer_size - written, "],\"total\":%u}", sessions->session_count);
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Serialize authentication status to JSON
 * @param status Authentication status
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_status_json(const api_auth_status_t *status, char *buffer, size_t buffer_size) {
    if (status == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"authenticated\":%s,"
        "\"session_count\":%u,"
        "\"active_users\":%u,"
        "\"max_sessions\":%u,"
        "\"session_timeout\":%u,"
        "\"password_policy\":{"
            "\"min_length\":%u,"
            "\"require_uppercase\":%s,"
            "\"require_lowercase\":%s,"
            "\"require_numbers\":%s,"
            "\"require_special\":%s"
        "}"
        "}",
        status->authenticated ? "true" : "false",
        status->session_count,
        status->active_users,
        status->max_sessions,
        status->session_timeout,
        status->password_policy.min_length,
        status->password_policy.require_uppercase ? "true" : "false",
        status->password_policy.require_lowercase ? "true" : "false",
        status->password_policy.require_numbers ? "true" : "false",
        status->password_policy.require_special ? "true" : "false"
    );
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Parse change password request from JSON
 * @param json JSON string
 * @param request Change password request (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_parse_change_password_json(const char *json, api_change_password_request_t *request) {
    if (json == NULL || request == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Simple JSON parsing (in production, use a proper JSON library)
    memset(request, 0, sizeof(api_change_password_request_t));
    
    // Find current_password field
    const char *current_start = strstr(json, "\"current_password\":\"");
    if (current_start != NULL) {
        current_start += 20; // Skip "\"current_password\":\""
        const char *current_end = strchr(current_start, '"');
        if (current_end != NULL) {
            size_t current_len = current_end - current_start;
            if (current_len < sizeof(request->current_password)) {
                strncpy(request->current_password, current_start, current_len);
                request->current_password[current_len] = '\0';
            }
        }
    }
    
    // Find new_password field
    const char *new_start = strstr(json, "\"new_password\":\"");
    if (new_start != NULL) {
        new_start += 16; // Skip "\"new_password\":\""
        const char *new_end = strchr(new_start, '"');
        if (new_end != NULL) {
            size_t new_len = new_end - new_start;
            if (new_len < sizeof(request->new_password)) {
                strncpy(request->new_password, new_start, new_len);
                request->new_password[new_len] = '\0';
            }
        }
    }
    
    // Find confirm_password field
    const char *confirm_start = strstr(json, "\"confirm_password\":\"");
    if (confirm_start != NULL) {
        confirm_start += 20; // Skip "\"confirm_password\":\""
        const char *confirm_end = strchr(confirm_start, '"');
        if (confirm_end != NULL) {
            size_t confirm_len = confirm_end - confirm_start;
            if (confirm_len < sizeof(request->confirm_password)) {
                strncpy(request->confirm_password, confirm_start, confirm_len);
                request->confirm_password[confirm_len] = '\0';
            }
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize change password response to JSON
 * @param response Change password response
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_change_password_response_json(const api_change_password_response_t *response, char *buffer, size_t buffer_size) {
    if (response == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"success\":%s,"
        "\"message\":\"%s\""
        "}",
        response->success ? "true" : "false",
        response->message
    );
    
    return (written < (int)buffer_size) ? written : -1;
}
