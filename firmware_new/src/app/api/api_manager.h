/**
 * @file api_manager.h
 * @brief API Manager Core Structure for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#ifndef API_MANAGER_H
#define API_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/time.h>

// Include HAL dependencies
#include "../../hal/common/hal_common.h"

// API Manager Configuration
#define API_MANAGER_MAX_CLIENTS          10
#define API_MANAGER_MAX_MESSAGE_SIZE     4096
#define API_MANAGER_MAX_PATH_LENGTH      256
#define API_MANAGER_MAX_HEADERS          32
#define API_MANAGER_DEFAULT_HTTP_PORT    8080
#define API_MANAGER_DEFAULT_WS_PORT      8081
#define API_MANAGER_DEFAULT_TIMEOUT_MS   5000

// HTTP Methods
typedef enum {
    API_MGR_HTTP_GET = 0,
    API_MGR_HTTP_POST,
    API_MGR_HTTP_PUT,
    API_MGR_HTTP_DELETE,
    API_MGR_HTTP_PATCH,
    API_MGR_HTTP_HEAD,
    API_MGR_HTTP_OPTIONS
} api_mgr_http_method_t;

// HTTP Response Codes
typedef enum {
    API_MGR_RESPONSE_OK = 200,
    API_MGR_RESPONSE_CREATED = 201,
    API_MGR_RESPONSE_NO_CONTENT = 204,
    API_MGR_RESPONSE_BAD_REQUEST = 400,
    API_MGR_RESPONSE_UNAUTHORIZED = 401,
    API_MGR_RESPONSE_FORBIDDEN = 403,
    API_MGR_RESPONSE_NOT_FOUND = 404,
    API_MGR_RESPONSE_METHOD_NOT_ALLOWED = 405,
    API_MGR_RESPONSE_INTERNAL_SERVER_ERROR = 500,
    API_MGR_RESPONSE_NOT_IMPLEMENTED = 501,
    API_MGR_RESPONSE_SERVICE_UNAVAILABLE = 503
} api_mgr_http_response_code_t;

// HTTP Header Structure
typedef struct {
    char name[64];
    char value[256];
} api_mgr_http_header_t;

// HTTP Request Structure
typedef struct {
    api_mgr_http_method_t method;
    char path[API_MANAGER_MAX_PATH_LENGTH];
    char query_string[256];
    api_mgr_http_header_t headers[API_MANAGER_MAX_HEADERS];
    uint32_t header_count;
    char *body;
    size_t body_length;
    struct timeval timestamp;
} api_mgr_http_request_t;

// HTTP Response Structure
typedef struct {
    api_mgr_http_response_code_t status_code;
    api_mgr_http_header_t headers[API_MANAGER_MAX_HEADERS];
    uint32_t header_count;
    char *body;
    size_t body_length;
    struct timeval timestamp;
} api_mgr_http_response_t;

// WebSocket Client Structure
typedef struct {
    int socket_fd;
    bool connected;
    bool authenticated;
    struct timeval last_activity;
    char client_id[64];
    pthread_t thread_id;
} api_mgr_websocket_client_t;

// API Manager Configuration Structure
typedef struct {
    uint16_t http_port;
    uint16_t websocket_port;
    uint32_t timeout_ms;
    uint32_t max_clients;
    bool enable_cors;
    bool enable_authentication;
    char cors_origin[256];
    char api_version[16];
} api_mgr_config_t;

// API Manager Statistics Structure
typedef struct {
    uint64_t total_requests;
    uint64_t successful_requests;
    uint64_t failed_requests;
    uint64_t websocket_connections;
    uint64_t websocket_messages_sent;
    uint64_t websocket_messages_received;
    uint64_t uptime_ms;
    uint64_t last_request_time;
    uint64_t last_websocket_activity;
} api_mgr_statistics_t;

// API Manager Status Structure
typedef struct {
    bool http_server_running;
    bool websocket_server_running;
    uint32_t active_http_connections;
    uint32_t active_websocket_connections;
    api_mgr_statistics_t statistics;
    hal_device_status_t overall_status;
} api_mgr_status_t;

// API Endpoint Handler Function Type
typedef hal_status_t (*api_mgr_endpoint_handler_t)(const api_mgr_http_request_t *request, 
                                                   api_mgr_http_response_t *response);

// API Endpoint Structure
typedef struct {
    api_mgr_http_method_t method;
    char path[API_MANAGER_MAX_PATH_LENGTH];
    api_mgr_endpoint_handler_t handler;
    bool requires_authentication;
    char description[256];
} api_mgr_endpoint_t;

// WebSocket Message Handler Function Type
typedef hal_status_t (*api_mgr_websocket_handler_t)(const char *message, size_t message_length, 
                                                    api_mgr_websocket_client_t *client);

// API Manager Instance Structure
typedef struct {
    api_mgr_config_t config;
    api_mgr_status_t status;
    api_mgr_endpoint_t endpoints[64];
    uint32_t endpoint_count;
    api_mgr_websocket_client_t clients[API_MANAGER_MAX_CLIENTS];
    uint32_t client_count;
    pthread_mutex_t mutex;
    pthread_t http_server_thread;
    pthread_t websocket_server_thread;
    int http_server_socket;
    int websocket_server_socket;
    bool initialized;
    bool running;
} api_mgr_instance_t;

// API Manager Core Functions
hal_status_t api_manager_init(const api_mgr_config_t *config);
hal_status_t api_manager_start(void);
hal_status_t api_manager_stop(void);
hal_status_t api_manager_deinit(void);
hal_status_t api_manager_get_status(api_mgr_status_t *status);

// API Manager Configuration Functions
hal_status_t api_manager_set_config(const api_mgr_config_t *config);
hal_status_t api_manager_get_config(api_mgr_config_t *config);
hal_status_t api_manager_set_default_config(api_mgr_config_t *config);

// API Manager Endpoint Management
hal_status_t api_manager_register_endpoint(const api_mgr_endpoint_t *endpoint);
hal_status_t api_manager_unregister_endpoint(const char *path, api_mgr_http_method_t method);
hal_status_t api_manager_get_endpoints(api_mgr_endpoint_t *endpoints, uint32_t *count);

// API Manager HTTP Server Functions
hal_status_t api_manager_start_http_server(void);
hal_status_t api_manager_stop_http_server(void);
hal_status_t api_manager_handle_http_request(const api_mgr_http_request_t *request, 
                                            api_mgr_http_response_t *response);

// API Manager WebSocket Server Functions
hal_status_t api_manager_start_websocket_server(void);
hal_status_t api_manager_stop_websocket_server(void);
hal_status_t api_manager_broadcast_websocket_message(const char *message, size_t message_length);
hal_status_t api_manager_send_websocket_message(int client_socket, const char *message, size_t message_length);
hal_status_t api_manager_register_websocket_handler(api_mgr_websocket_handler_t handler);

// API Manager Client Management
hal_status_t api_manager_add_websocket_client(int socket_fd);
hal_status_t api_manager_remove_websocket_client(int socket_fd);
hal_status_t api_manager_get_websocket_clients(api_mgr_websocket_client_t *clients, uint32_t *count);

// API Manager Utility Functions
hal_status_t api_manager_create_success_response(api_mgr_http_response_t *response, const char *json_data);
hal_status_t api_manager_create_error_response(api_mgr_http_response_t *response, 
                                               api_mgr_http_response_code_t status_code, 
                                               const char *error_message);
hal_status_t api_manager_parse_http_request(const char *raw_request, api_mgr_http_request_t *request);
hal_status_t api_manager_serialize_http_response(const api_mgr_http_response_t *response, char *buffer, size_t buffer_size);

// API Manager Statistics Functions
hal_status_t api_manager_get_statistics(api_mgr_statistics_t *statistics);
hal_status_t api_manager_reset_statistics(void);
hal_status_t api_manager_update_statistics(api_mgr_statistics_t *stats, bool success);

// API Manager Logging Functions
hal_status_t api_manager_log_request(const api_mgr_http_request_t *request, 
                                    const api_mgr_http_response_t *response);
hal_status_t api_manager_log_websocket_activity(const char *activity, const char *client_id);

// API Manager Error Handling Functions
hal_status_t api_manager_handle_error(hal_status_t error_code, const char *context);
const char* api_manager_error_to_string(hal_status_t error_code);

// API Manager Thread Functions
void* api_manager_http_server_thread(void *arg);
void* api_manager_websocket_server_thread(void *arg);
void* api_manager_websocket_client_thread(void *arg);
void* api_manager_handle_http_client_thread(void *arg);

// API Manager Global Instance
extern api_mgr_instance_t g_api_manager;

// API Manager Version Information
#define API_MANAGER_VERSION_MAJOR 1
#define API_MANAGER_VERSION_MINOR 0
#define API_MANAGER_VERSION_PATCH 0
#define API_MANAGER_VERSION_STRING "1.0.0"

hal_status_t api_manager_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch);
const char* api_manager_get_version_string(void);

#endif // API_MANAGER_H
