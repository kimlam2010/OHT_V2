/**
 * @file http_server.h
 * @brief HTTP Server Foundation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/time.h>

// Include HAL dependencies
#include "../../hal/common/hal_common.h"

// HTTP Server Configuration
#define HTTP_SERVER_MAX_CONNECTIONS       10
#define HTTP_SERVER_MAX_REQUEST_SIZE      4096
#define HTTP_SERVER_MAX_RESPONSE_SIZE     8192
#define HTTP_SERVER_MAX_HEADERS           32
#define HTTP_SERVER_DEFAULT_PORT          8080
#define HTTP_SERVER_DEFAULT_TIMEOUT_MS    5000
#define HTTP_SERVER_DEFAULT_BACKLOG       10

// HTTP Methods
typedef enum {
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_PATCH,
    HTTP_METHOD_HEAD,
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
    HTTP_STATUS_NOT_IMPLEMENTED = 501,
    HTTP_STATUS_SERVICE_UNAVAILABLE = 503
} http_status_code_t;

// HTTP Header Structure
typedef struct {
    char name[64];
    char value[256];
} http_header_t;

// HTTP Request Structure
typedef struct {
    http_method_t method;
    char path[256];
    char query_string[256];
    char version[16];
    http_header_t headers[HTTP_SERVER_MAX_HEADERS];
    uint32_t header_count;
    char *body;
    size_t body_length;
    struct timeval timestamp;
    char client_ip[16];
    uint16_t client_port;
} http_request_t;

// HTTP Response Structure
typedef struct {
    http_status_code_t status_code;
    char version[16];
    http_header_t headers[HTTP_SERVER_MAX_HEADERS];
    uint32_t header_count;
    char *body;
    size_t body_length;
    struct timeval timestamp;
} http_response_t;

// HTTP Route Handler Function Type
typedef hal_status_t (*http_route_handler_t)(const http_request_t *request, http_response_t *response);

// HTTP Route Structure
typedef struct {
    http_method_t method;
    char path[256];
    http_route_handler_t handler;
    bool requires_authentication;
    char description[256];
} http_route_t;

// HTTP Server Configuration Structure
typedef struct {
    uint16_t port;
    uint32_t timeout_ms;
    uint32_t max_connections;
    uint32_t max_request_size;
    uint32_t max_response_size;
    bool enable_cors;
    bool enable_compression;
    bool enable_keep_alive;
    char cors_origin[256];
    char server_name[64];
    char document_root[256];
} http_server_config_t;

// HTTP Server Statistics Structure
typedef struct {
    uint64_t total_requests;
    uint64_t successful_requests;
    uint64_t failed_requests;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t uptime_ms;
    uint64_t last_request_time;
    uint32_t active_connections;
    uint32_t peak_connections;
} http_server_statistics_t;

// HTTP Server Status Structure
typedef struct {
    bool running;
    bool listening;
    uint32_t active_connections;
    http_server_statistics_t statistics;
    hal_device_status_t overall_status;
} http_server_status_t;

// HTTP Server Instance Structure
typedef struct {
    http_server_config_t config;
    http_server_status_t status;
    http_route_t routes[64];
    uint32_t route_count;
    int server_socket;
    pthread_t server_thread;
    pthread_mutex_t mutex;
    bool initialized;
    bool running;
} http_server_instance_t;

// HTTP Server Core Functions
hal_status_t http_server_init(const http_server_config_t *config);
hal_status_t http_server_start(void);
hal_status_t http_server_stop(void);
hal_status_t http_server_deinit(void);
hal_status_t http_server_get_status(http_server_status_t *status);

// HTTP Server Configuration Functions
hal_status_t http_server_set_config(const http_server_config_t *config);
hal_status_t http_server_get_config(http_server_config_t *config);
hal_status_t http_server_set_default_config(http_server_config_t *config);

// HTTP Server Route Management
hal_status_t http_server_register_route(const http_route_t *route);
hal_status_t http_server_unregister_route(const char *path, http_method_t method);
hal_status_t http_server_get_routes(http_route_t *routes, uint32_t *count);

// HTTP Server Request/Response Functions
hal_status_t http_server_handle_request(const http_request_t *request, http_response_t *response);
hal_status_t http_server_parse_request(const char *raw_request, http_request_t *request);
hal_status_t http_server_serialize_response(const http_response_t *response, char *buffer, size_t buffer_size);

// HTTP Server Utility Functions
hal_status_t http_server_create_success_response(http_response_t *response, const char *content_type, const char *body);
hal_status_t http_server_create_error_response(http_response_t *response, http_status_code_t status_code, const char *error_message);
hal_status_t http_server_create_json_response(http_response_t *response, const char *json_data);
hal_status_t http_server_create_html_response(http_response_t *response, const char *html_content);

// HTTP Server Header Functions
hal_status_t http_server_add_header(http_response_t *response, const char *name, const char *value);
hal_status_t http_server_get_header(const http_request_t *request, const char *name, char *value, size_t value_size);
hal_status_t http_server_has_header(const http_request_t *request, const char *name);

// HTTP Server CORS Functions
hal_status_t http_server_add_cors_headers(http_response_t *response);
hal_status_t http_server_handle_cors_preflight(const http_request_t *request, http_response_t *response);

// HTTP Server Authentication Functions
hal_status_t http_server_validate_authentication(const http_request_t *request);
hal_status_t http_server_require_authentication(http_response_t *response);

// HTTP Server Statistics Functions
hal_status_t http_server_get_statistics(http_server_statistics_t *statistics);
hal_status_t http_server_reset_statistics(void);
hal_status_t http_server_update_statistics(http_server_statistics_t *stats, bool success, size_t bytes_sent, size_t bytes_received);

// HTTP Server Logging Functions
hal_status_t http_server_log_request(const http_request_t *request, const http_response_t *response);
hal_status_t http_server_log_error(const char *error_message, const char *context);

// HTTP Server Thread Functions
void* http_server_thread(void *arg);
void* http_server_client_thread(void *arg);

// HTTP Server Utility Functions
const char* http_method_to_string(http_method_t method);
http_method_t http_string_to_method(const char *method_str);
const char* http_status_to_string(http_status_code_t status_code);
hal_status_t http_server_parse_http_version(const char *version_str, char *version, size_t version_size);
hal_status_t http_server_parse_query_string(const char *query_string, char *path, char *query, size_t path_size, size_t query_size);

// HTTP Server Global Instance
extern http_server_instance_t g_http_server;

// HTTP Server Version Information
#define HTTP_SERVER_VERSION_MAJOR 1
#define HTTP_SERVER_VERSION_MINOR 0
#define HTTP_SERVER_VERSION_PATCH 0
#define HTTP_SERVER_VERSION_STRING "1.0.0"

hal_status_t http_server_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch);
const char* http_server_get_version_string(void);

#endif // HTTP_SERVER_H
