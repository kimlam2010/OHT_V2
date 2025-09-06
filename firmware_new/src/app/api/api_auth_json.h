/**
 * @file api_auth_json.h
 * @brief API Authentication JSON Utilities for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.1
 */

#ifndef API_AUTH_JSON_H
#define API_AUTH_JSON_H

#include "api_auth.h"
#include "hal_common.h"

/**
 * @brief Serialize login request to JSON
 * @param request Login request
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_login_request_json(const api_login_request_t *request, char *buffer, size_t buffer_size);

/**
 * @brief Parse login request from JSON
 * @param json JSON string
 * @param request Login request (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_parse_login_request_json(const char *json, api_login_request_t *request);

/**
 * @brief Serialize login response to JSON
 * @param response Login response
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_login_response_json(const api_login_response_t *response, char *buffer, size_t buffer_size);

/**
 * @brief Serialize user information to JSON
 * @param user User information
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_user_json(const api_user_t *user, char *buffer, size_t buffer_size);

/**
 * @brief Serialize session information to JSON
 * @param session Session information
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_session_json(const api_session_t *session, char *buffer, size_t buffer_size);

/**
 * @brief Serialize user list to JSON
 * @param users User list
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_users_list_json(const api_users_list_t *users, char *buffer, size_t buffer_size);

/**
 * @brief Serialize session list to JSON
 * @param sessions Session list
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_sessions_list_json(const api_sessions_list_t *sessions, char *buffer, size_t buffer_size);

/**
 * @brief Serialize authentication status to JSON
 * @param status Authentication status
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_status_json(const api_auth_status_t *status, char *buffer, size_t buffer_size);

/**
 * @brief Parse change password request from JSON
 * @param json JSON string
 * @param request Change password request (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_auth_parse_change_password_json(const char *json, api_change_password_request_t *request);

/**
 * @brief Serialize change password response to JSON
 * @param response Change password response
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_auth_serialize_change_password_response_json(const api_change_password_response_t *response, char *buffer, size_t buffer_size);

#endif // API_AUTH_JSON_H
