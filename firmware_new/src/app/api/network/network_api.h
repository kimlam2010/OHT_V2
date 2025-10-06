/**
 * @file network_api.h
 * @brief Network API - OHT-50 Robot Network Management REST API
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Network API cung cấp REST endpoints cho Network Management
 * bao gồm WiFi connection, scanning, roaming, và performance monitoring.
 */

#ifndef NETWORK_API_H
#define NETWORK_API_H

#include "hal_common.h"
#include "network_manager.h"
#include "wifi_manager.h"
#include "api_manager.h"
#include <stdint.h>
#include <stdbool.h>

// Authentication Levels
typedef enum {
    AUTH_NONE = 0,      // Public access
    AUTH_OPERATOR = 1,  // Operator token required
    AUTH_ADMIN = 2      // Admin token required
} auth_level_t;

// HTTP Request Structure
typedef struct {
    char method[8];
    char path[256];
    char headers[1024];
    char body[2048];
    char auth_token[128];
    auth_level_t required_auth;
} http_request_t;

// HTTP Response Structure
typedef struct {
    int status_code;
    char headers[1024];
    char body[4096];
    uint32_t content_length;
} http_response_t;

// API Route Structure
typedef struct {
    const char *path;
    const char *method;
    int (*handler)(http_request_t *req, http_response_t *resp);
    auth_level_t required_auth;
} api_route_t;

// Network API Functions

/**
 * @brief Initialize Network API
 * @return 0 on success, -1 on failure
 */
int network_api_init(void);

/**
 * @brief Deinitialize Network API
 * @return 0 on success, -1 on failure
 */
int network_api_deinit(void);

/**
 * @brief Handle HTTP request
 * @param req HTTP request structure
 * @param resp HTTP response structure
 * @return 0 on success, -1 on failure
 */
int network_api_handle_request(http_request_t *req, http_response_t *resp);

/**
 * @brief Register Network API routes
 * @return 0 on success, -1 on failure
 */
int network_api_register_routes(void);

/**
 * @brief Wrapper handler for API Manager integration
 * @param req API Manager request
 * @param res API Manager response
 * @return 0 on success, -1 on failure
 */
int network_api_wrapper_handler(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Validate authentication token
 * @param token Authentication token
 * @param required_level Required authentication level
 * @return true if valid, false otherwise
 */
bool network_api_validate_auth(const char *token, auth_level_t required_level);

/**
 * @brief Send JSON response
 * @param resp HTTP response structure
 * @param status_code HTTP status code
 * @param json_data JSON data string
 * @return 0 on success, -1 on failure
 */
int network_api_send_json_response(http_response_t *resp, int status_code, const char *json_data);

/**
 * @brief Send error response
 * @param resp HTTP response structure
 * @param status_code HTTP status code
 * @param error_message Error message
 * @return 0 on success, -1 on failure
 */
int network_api_send_error_response(http_response_t *resp, int status_code, const char *error_message);

/**
 * @brief Parse JSON request body
 * @param body JSON body string
 * @param key Key to extract
 * @param value Buffer to store value
 * @param max_len Maximum length of value buffer
 * @return 0 on success, -1 on failure
 */
int network_api_parse_json_string(const char *body, const char *key, char *value, int max_len);

/**
 * @brief Parse JSON request body for boolean
 * @param body JSON body string
 * @param key Key to extract
 * @param value Pointer to boolean value
 * @return 0 on success, -1 on failure
 */
int network_api_parse_json_bool(const char *body, const char *key, bool *value);

/**
 * @brief Parse JSON request body for integer
 * @param body JSON body string
 * @param key Key to extract
 * @param value Pointer to integer value
 * @return 0 on success, -1 on failure
 */
int network_api_parse_json_int(const char *body, const char *key, int *value);

// API Endpoint Handlers

/**
 * @brief Handle GET /api/v1/network/status
 */
int handle_get_network_status(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/wifi/connect
 */
int handle_post_wifi_connect(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/wifi/disconnect
 */
int handle_post_wifi_disconnect(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/wifi/scan
 */
int handle_get_wifi_scan(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/performance
 */
int handle_get_network_performance(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/health
 */
int handle_get_network_health(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/roaming/enable
 */
int handle_post_roaming_enable(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/mobile/enable
 */
int handle_post_mobile_enable(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/config
 */
int handle_get_network_config(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/config
 */
int handle_post_network_config(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle GET /api/v1/network/statistics
 */
int handle_get_network_statistics(http_request_t *req, http_response_t *resp);

/**
 * @brief Handle POST /api/v1/network/statistics/reset
 */
int handle_post_statistics_reset(http_request_t *req, http_response_t *resp);

#endif // NETWORK_API_H
