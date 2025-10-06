/**
 * @file wifi_ap_api.h
 * @brief WiFi AP API - OHT-50 Robot WiFi Access Point REST API
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * WiFi AP API cung cấp REST endpoints cho WiFi Access Point management
 * bao gồm AP start/stop, client management, và fallback functionality.
 */

#ifndef WIFI_AP_API_H
#define WIFI_AP_API_H

#include "network_api.h"
#include "wifi_ap_manager.h"
#include "api_manager.h"
#include <stdint.h>
#include <stdbool.h>

// WiFi AP API Error Codes
typedef enum {
    WIFI_AP_API_SUCCESS = 0,
    WIFI_AP_API_ERROR_INVALID_PARAM = -1,
    WIFI_AP_API_ERROR_INIT_FAILED = -2,
    WIFI_AP_API_ERROR_NOT_INITIALIZED = -3,
    WIFI_AP_API_ERROR_AUTH_FAILED = -4,
    WIFI_AP_API_ERROR_JSON_PARSE_FAILED = -5,
    WIFI_AP_API_ERROR_MANAGER_FAILED = -6
} wifi_ap_api_error_t;

// WiFi AP API Functions

/**
 * @brief Initialize WiFi AP API
 * @return WIFI_AP_API_SUCCESS on success, error code on failure
 */
int wifi_ap_api_init(void);

/**
 * @brief Deinitialize WiFi AP API
 * @return WIFI_AP_API_SUCCESS on success, error code on failure
 */
int wifi_ap_api_deinit(void);

/**
 * @brief Handle HTTP request for WiFi AP API
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int wifi_ap_api_handle_request(http_request_t *req, http_response_t *resp);

/**
 * @brief Send JSON response
 * @param resp HTTP response
 * @param status_code HTTP status code
 * @param json_data JSON data string
 * @return 0 on success, error code on failure
 */
int wifi_ap_api_send_json_response(http_response_t *resp, int status_code, const char *json_data);

/**
 * @brief Send error response
 * @param resp HTTP response
 * @param status_code HTTP status code
 * @param error_message Error message
 * @return 0 on success, error code on failure
 */
int wifi_ap_api_send_error_response(http_response_t *resp, int status_code, const char *error_message);

/**
 * @brief Register WiFi AP API routes with API Manager
 * @return 0 on success, -1 on failure
 */
int wifi_ap_api_register_routes(void);

/**
 * @brief Wrapper handler for API Manager integration
 * @param req API Manager request
 * @param res API Manager response
 * @return 0 on success, -1 on failure
 */
int wifi_ap_api_wrapper_handler(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Validate authentication token
 * @param token Authentication token
 * @param required_level Required authentication level
 * @return true if valid, false otherwise
 */
bool wifi_ap_api_validate_auth(const char *token, auth_level_t required_level);

/**
 * @brief Parse JSON request body for string
 * @param body JSON body
 * @param key Key to parse
 * @param value Buffer to store value
 * @param max_len Maximum length of value buffer
 * @return 0 on success, error code on failure
 */
int wifi_ap_api_parse_json_string(const char *body, const char *key, char *value, int max_len);

/**
 * @brief Parse JSON request body for boolean
 * @param body JSON body
 * @param key Key to parse
 * @param value Pointer to store boolean value
 * @return 0 on success, error code on failure
 */
int wifi_ap_api_parse_json_bool(const char *body, const char *key, bool *value);

/**
 * @brief Parse JSON request body for integer
 * @param body JSON body
 * @param key Key to parse
 * @param value Pointer to store integer value
 * @return 0 on success, error code on failure
 */
int wifi_ap_api_parse_json_int(const char *body, const char *key, int *value);

/**
 * @brief Get current timestamp in ISO format
 * @return Timestamp string
 */
const char* wifi_ap_api_get_current_timestamp(void);

// API Endpoint Handlers

/**
 * @brief Handle POST /api/v1/network/ap/start
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_post_ap_start(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/ap/stop
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_post_ap_stop(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/ap/status
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_get_ap_status(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/ap/config
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_get_ap_config(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/ap/config
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_post_ap_config(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/ap/clients
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_get_ap_clients(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/ap/clients/kick
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_post_ap_kick_client(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/ap/statistics
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_get_ap_statistics(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/ap/statistics/reset
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_post_ap_statistics_reset(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/fallback/enable
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_post_fallback_enable(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/fallback/status
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_get_fallback_status(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/fallback/trigger
 * @param req HTTP request
 * @param resp HTTP response
 * @return 0 on success, error code on failure
 */
int handle_post_fallback_trigger(http_request_t *req, http_response_t *resp);

/**
 * @brief Get error message for error code
 * @param error_code Error code
 * @return Error message string
 */
const char* wifi_ap_api_get_error_message(int error_code);

#endif // WIFI_AP_API_H
