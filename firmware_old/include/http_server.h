/**
 * @file http_server.h
 * @brief HTTP Server for OHT-50 Master Module Firmware
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-01 (HTTP Server Implementation)
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "hal_common.h"

// HTTP Server Configuration
#define HTTP_SERVER_MAX_CONNECTIONS    10
#define HTTP_SERVER_BUFFER_SIZE        4096
#define HTTP_SERVER_DEFAULT_PORT       8080
#define HTTP_SERVER_MAX_HEADERS        20
#define HTTP_SERVER_TIMEOUT_MS         30000

// HTTP Methods
typedef enum {
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_UNKNOWN
} http_method_t;

// HTTP Status Codes
typedef enum {
    HTTP_STATUS_OK = 200,
    HTTP_STATUS_CREATED = 201,
    HTTP_STATUS_NO_CONTENT = 204,
    HTTP_STATUS_BAD_REQUEST = 400,
    HTTP_STATUS_UNAUTHORIZED = 401,
    HTTP_STATUS_FORBIDDEN = 403,
    HTTP_STATUS_NOT_FOUND = 404,
    HTTP_STATUS_METHOD_NOT_ALLOWED = 405,
    HTTP_STATUS_INTERNAL_SERVER_ERROR = 500,
    HTTP_STATUS_SERVICE_UNAVAILABLE = 503
} http_status_t;

// HTTP Request Structure
typedef struct {
    http_method_t method;
    char path[256];
    char query_string[256];
    char headers[HTTP_SERVER_MAX_HEADERS][2][128]; // [header][name][value]
    int header_count;
    char body[HTTP_SERVER_BUFFER_SIZE];
    int body_length;
    char content_type[64];
    char authorization[128];
} http_request_t;

// HTTP Response Structure
typedef struct {
    http_status_t status;
    char content_type[64];
    char headers[HTTP_SERVER_MAX_HEADERS][2][128]; // [header][name][value]
    int header_count;
    char body[HTTP_SERVER_BUFFER_SIZE];
    int body_length;
} http_response_t;

// HTTP Server Configuration
typedef struct {
    uint16_t port;
    uint32_t max_connections;
    uint32_t timeout_ms;
    bool enable_cors;
    char cors_origin[128];
    bool enable_auth;
    char auth_token[64];
    bool enable_logging;
    char log_file[256];
} http_server_config_t;

// HTTP Server Status
typedef struct {
    bool is_running;
    uint32_t active_connections;
    uint32_t total_requests;
    uint32_t successful_requests;
    uint32_t failed_requests;
    uint64_t uptime_ms;
    uint64_t last_request_time;
} http_server_status_t;

// HTTP Route Handler Function Type
typedef hal_status_t (*http_route_handler_t)(const http_request_t *request, http_response_t *response);

// HTTP Route Structure
typedef struct {
    http_method_t method;
    char path[128];
    http_route_handler_t handler;
    bool require_auth;
} http_route_t;

// HTTP Server Instance
typedef struct {
    http_server_config_t config;
    http_server_status_t status;
    http_route_t routes[50];
    int route_count;
    int server_socket;
    bool should_stop;
    pthread_t server_thread;
    pthread_mutex_t mutex;
} http_server_t;

// HTTP Server API Functions

/**
 * @brief Initialize HTTP server
 * @param server HTTP server instance
 * @param config Server configuration
 * @return HAL status
 */
hal_status_t http_server_init(http_server_t *server, const http_server_config_t *config);

/**
 * @brief Deinitialize HTTP server
 * @param server HTTP server instance
 * @return HAL status
 */
hal_status_t http_server_deinit(http_server_t *server);

/**
 * @brief Start HTTP server
 * @param server HTTP server instance
 * @return HAL status
 */
hal_status_t http_server_start(http_server_t *server);

/**
 * @brief Stop HTTP server
 * @param server HTTP server instance
 * @return HAL status
 */
hal_status_t http_server_stop(http_server_t *server);

/**
 * @brief Add HTTP route
 * @param server HTTP server instance
 * @param method HTTP method
 * @param path Route path
 * @param handler Route handler function
 * @param require_auth Whether authentication is required
 * @return HAL status
 */
hal_status_t http_server_add_route(http_server_t *server, 
                                  http_method_t method, 
                                  const char *path, 
                                  http_route_handler_t handler, 
                                  bool require_auth);

/**
 * @brief Get HTTP server status
 * @param server HTTP server instance
 * @param status Status structure to fill
 * @return HAL status
 */
hal_status_t http_server_get_status(http_server_t *server, http_server_status_t *status);

/**
 * @brief Set HTTP server configuration
 * @param server HTTP server instance
 * @param config New configuration
 * @return HAL status
 */
hal_status_t http_server_set_config(http_server_t *server, const http_server_config_t *config);

/**
 * @brief Get HTTP server configuration
 * @param server HTTP server instance
 * @param config Configuration structure to fill
 * @return HAL status
 */
hal_status_t http_server_get_config(http_server_t *server, http_server_config_t *config);

// HTTP Utility Functions

/**
 * @brief Parse HTTP method from string
 * @param method_str Method string
 * @return HTTP method
 */
http_method_t http_parse_method(const char *method_str);

/**
 * @brief Get HTTP method string
 * @param method HTTP method
 * @return Method string
 */
const char* http_get_method_string(http_method_t method);

/**
 * @brief Get HTTP status string
 * @param status HTTP status
 * @return Status string
 */
const char* http_get_status_string(http_status_t status);

/**
 * @brief Parse HTTP request
 * @param request_data Raw request data
 * @param request_length Request data length
 * @param request Parsed request structure
 * @return HAL status
 */
hal_status_t http_parse_request(const char *request_data, int request_length, http_request_t *request);

/**
 * @brief Build HTTP response
 * @param response Response structure
 * @param response_data Output buffer for response data
 * @param max_length Maximum buffer length
 * @param actual_length Actual response length
 * @return HAL status
 */
hal_status_t http_build_response(const http_response_t *response, 
                                char *response_data, 
                                int max_length, 
                                int *actual_length);

/**
 * @brief Add HTTP response header
 * @param response Response structure
 * @param name Header name
 * @param value Header value
 * @return HAL status
 */
hal_status_t http_response_add_header(http_response_t *response, const char *name, const char *value);

/**
 * @brief Set HTTP response JSON body
 * @param response Response structure
 * @param json_data JSON data
 * @return HAL status
 */
hal_status_t http_response_set_json(http_response_t *response, const char *json_data);

/**
 * @brief Set HTTP response error
 * @param response Response structure
 * @param status HTTP status code
 * @param error_message Error message
 * @return HAL status
 */
hal_status_t http_response_set_error(http_response_t *response, http_status_t status, const char *error_message);

/**
 * @brief Validate HTTP request authentication
 * @param request HTTP request
 * @param auth_token Expected auth token
 * @return true if authenticated, false otherwise
 */
bool http_validate_auth(const http_request_t *request, const char *auth_token);

/**
 * @brief Get HTTP request header value
 * @param request HTTP request
 * @param header_name Header name
 * @return Header value or NULL if not found
 */
const char* http_get_header(const http_request_t *request, const char *header_name);

/**
 * @brief Get HTTP request query parameter
 * @param request HTTP request
 * @param param_name Parameter name
 * @return Parameter value or NULL if not found
 */
const char* http_get_query_param(const http_request_t *request, const char *param_name);

// Default HTTP Server Configuration
extern const http_server_config_t HTTP_SERVER_DEFAULT_CONFIG;

#endif // HTTP_SERVER_H
