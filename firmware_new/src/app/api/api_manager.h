/**
 * @file api_manager.h
 * @brief API Manager for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-06 (Advanced Communication)
 */

#ifndef API_MANAGER_H
#define API_MANAGER_H

#include "hal_common.h"
// #include "security_manager.h" // Not needed for now
#include <stdint.h>
#include <stdbool.h>

// API Manager Configuration
#define API_MGR_MAX_ENDPOINTS        20
#define API_MGR_MAX_WEBSOCKET_CLIENTS 10
#define API_MGR_MAX_REQUEST_SIZE     4096
#define API_MGR_MAX_RESPONSE_SIZE    8192
#define API_MGR_DEFAULT_PORT         8080
#define API_MGR_WEBSOCKET_PORT       8081
#define API_MGR_MAX_HEADERS          10
#define API_MGR_MAX_URL_LENGTH       256
#define API_MGR_MAX_METHOD_LENGTH    10

// API Manager HTTP Methods
typedef enum {
    API_MGR_HTTP_GET = 0,
    API_MGR_HTTP_POST,
    API_MGR_HTTP_PUT,
    API_MGR_HTTP_DELETE,
    API_MGR_HTTP_PATCH,
    API_MGR_HTTP_HEAD,
    API_MGR_HTTP_OPTIONS
} api_mgr_http_method_t;

// API Manager Response Codes
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
    API_MGR_RESPONSE_SERVICE_UNAVAILABLE = 503
} api_mgr_response_code_t;

// API Manager Content Types
typedef enum {
    API_MGR_CONTENT_TYPE_JSON = 0,
    API_MGR_CONTENT_TYPE_XML,
    API_MGR_CONTENT_TYPE_TEXT,
    API_MGR_CONTENT_TYPE_BINARY,
    API_MGR_CONTENT_TYPE_FORM_DATA
} api_mgr_content_type_t;

// API Manager WebSocket States
typedef enum {
    API_MGR_WS_STATE_DISCONNECTED = 0,
    API_MGR_WS_STATE_CONNECTING,
    API_MGR_WS_STATE_CONNECTED,
    API_MGR_WS_STATE_CLOSING,
    API_MGR_WS_STATE_ERROR
} api_mgr_ws_state_t;

// API Manager WebSocket Frame Types
typedef enum {
    API_MGR_WS_FRAME_CONTINUATION = 0x0,
    API_MGR_WS_FRAME_TEXT = 0x1,
    API_MGR_WS_FRAME_BINARY = 0x2,
    API_MGR_WS_FRAME_CLOSE = 0x8,
    API_MGR_WS_FRAME_PING = 0x9,
    API_MGR_WS_FRAME_PONG = 0xA
} api_mgr_ws_frame_type_t;

// API Manager HTTP Header
typedef struct {
    char name[64];
    char value[256];
} api_mgr_http_header_t;

// API Manager HTTP Request
typedef struct {
    api_mgr_http_method_t method;
    char url[API_MGR_MAX_URL_LENGTH];
    char path[API_MGR_MAX_URL_LENGTH];
    api_mgr_http_header_t headers[API_MGR_MAX_HEADERS];
    uint32_t header_count;
    char *body;
    uint32_t body_length;
    api_mgr_content_type_t content_type;
    char client_ip[16];
    char session_id[64]; // Temporary size, was SECURITY_MGR_TOKEN_LENGTH
} api_mgr_http_request_t;

// API Manager HTTP Response
typedef struct {
    api_mgr_response_code_t status_code;
    api_mgr_http_header_t headers[API_MGR_MAX_HEADERS];
    uint32_t header_count;
    char *body;
    uint32_t body_length;
    api_mgr_content_type_t content_type;
    bool keep_alive;
} api_mgr_http_response_t;

// API Manager WebSocket Client
typedef struct {
    uint32_t client_id;
    api_mgr_ws_state_t state;
    char client_ip[16];
    uint64_t connected_time;
    uint64_t last_activity_time;
    bool authenticated;
    char session_id[64]; // Temporary size, was SECURITY_MGR_TOKEN_LENGTH
    uint8_t user_level; // Temporary type, was security_mgr_level_t
} api_mgr_ws_client_t;

// API Manager WebSocket Message
typedef struct {
    api_mgr_ws_frame_type_t frame_type;
    char *payload;
    uint32_t payload_length;
    bool fin;
    bool masked;
    uint32_t client_id;
} api_mgr_ws_message_t;

// API Manager Endpoint Handler
typedef hal_status_t (*api_mgr_endpoint_handler_t)(const api_mgr_http_request_t *request, 
                                                  api_mgr_http_response_t *response);

// API Manager WebSocket Message Handler
typedef hal_status_t (*api_mgr_ws_message_handler_t)(const api_mgr_ws_message_t *message);

// API Manager Configuration
typedef struct {
    uint16_t http_port;
    uint16_t websocket_port;
    bool http_enabled;
    bool websocket_enabled;
    bool cors_enabled;
    char cors_origin[64];
    uint32_t max_request_size;
    uint32_t max_response_size;
    uint32_t request_timeout_ms;
    uint32_t websocket_timeout_ms;
    bool authentication_required;
    bool ssl_enabled;
    char ssl_certificate_path[128];
    char ssl_private_key_path[128];
} api_mgr_config_t;

// API Manager Status
typedef struct {
    bool initialized;
    bool http_server_active;
    bool websocket_server_active;
    uint32_t active_http_connections;
    uint32_t active_websocket_connections;
    uint32_t total_requests_processed;
    uint32_t total_websocket_messages;
    uint64_t server_uptime;
    uint32_t error_count;
} api_mgr_status_t;

// API Manager Statistics
typedef struct {
    uint32_t http_requests_received;
    uint32_t http_responses_sent;
    uint32_t websocket_connections;
    uint32_t websocket_messages_sent;
    uint32_t websocket_messages_received;
    uint32_t authentication_failures;
    uint32_t authorization_failures;
    uint32_t request_timeouts;
    uint64_t total_response_time;
    uint32_t average_response_time;
} api_mgr_stats_t;

// API Manager Event Types
typedef enum {
    API_MGR_EVENT_NONE = 0,
    API_MGR_EVENT_HTTP_REQUEST,
    API_MGR_EVENT_HTTP_RESPONSE,
    API_MGR_EVENT_WEBSOCKET_CONNECT,
    API_MGR_EVENT_WEBSOCKET_DISCONNECT,
    API_MGR_EVENT_WEBSOCKET_MESSAGE,
    API_MGR_EVENT_AUTHENTICATION_FAILED,
    API_MGR_EVENT_AUTHORIZATION_FAILED,
    API_MGR_EVENT_SERVER_STARTED,
    API_MGR_EVENT_SERVER_STOPPED
} api_mgr_event_t;

// API Manager Event Callback
typedef void (*api_mgr_event_callback_t)(api_mgr_event_t event, 
                                        const void *data);

// API Manager Endpoint Registration
typedef struct {
    char path[API_MGR_MAX_URL_LENGTH];
    api_mgr_http_method_t method;
    api_mgr_endpoint_handler_t handler;
    uint8_t required_resource; // Temporary type, was security_mgr_resource_t
    uint8_t required_permission; // Temporary type, was security_mgr_permission_t
    bool authentication_required;
    bool enabled;
} api_mgr_endpoint_t;

// API Manager API Functions

// Initialization and Configuration
hal_status_t api_manager_init(const api_mgr_config_t *config);
hal_status_t api_manager_deinit(void);
hal_status_t api_manager_get_config(api_mgr_config_t *config);
hal_status_t api_manager_set_config(const api_mgr_config_t *config);

// Status and Statistics
hal_status_t api_manager_get_status(api_mgr_status_t *status);
hal_status_t api_manager_get_statistics(api_mgr_stats_t *stats);
hal_status_t api_manager_reset_statistics(void);

// HTTP Server Management
hal_status_t api_manager_start_http_server(void);
hal_status_t api_manager_stop_http_server(void);
hal_status_t api_manager_register_endpoint(const api_mgr_endpoint_t *endpoint);
hal_status_t api_manager_unregister_endpoint(const char *path, api_mgr_http_method_t method);
hal_status_t api_manager_process_http_request(const api_mgr_http_request_t *request, 
                                             api_mgr_http_response_t *response);

// WebSocket Server Management
hal_status_t api_manager_start_websocket_server(void);
hal_status_t api_manager_stop_websocket_server(void);
hal_status_t api_manager_set_websocket_message_handler(api_mgr_ws_message_handler_t handler);
hal_status_t api_manager_broadcast_websocket_message(const char *message, uint32_t length);
hal_status_t api_manager_send_websocket_message(uint32_t client_id, const char *message, uint32_t length);
hal_status_t api_manager_disconnect_websocket_client(uint32_t client_id);

// Authentication and Authorization
hal_status_t api_manager_authenticate_request(const api_mgr_http_request_t *request, 
                                             uint8_t *user_level); // was security_mgr_level_t
hal_status_t api_manager_authorize_request(const api_mgr_http_request_t *request, 
                                          uint8_t resource,  // was security_mgr_resource_t
                                          uint8_t permission); // was security_mgr_permission_t

// Response Helpers
hal_status_t api_manager_create_json_response(api_mgr_http_response_t *response, 
                                             api_mgr_response_code_t status_code, 
                                             const char *json_data);
hal_status_t api_manager_create_error_response(api_mgr_http_response_t *response, 
                                              api_mgr_response_code_t status_code, 
                                              const char *error_message);
hal_status_t api_manager_create_success_response(api_mgr_http_response_t *response, 
                                                const char *message);

// Event Handling
hal_status_t api_manager_set_callback(api_mgr_event_callback_t callback);
hal_status_t api_manager_get_diagnostics(char *diagnostics, uint32_t max_length);

// Built-in Endpoints
hal_status_t api_manager_register_system_endpoints(void);
hal_status_t api_manager_register_network_endpoints(void);
hal_status_t api_manager_register_communication_endpoints(void);
hal_status_t api_manager_register_safety_endpoints(void);
hal_status_t api_manager_register_configuration_endpoints(void);

// Utility Functions
const char* api_manager_get_http_method_name(api_mgr_http_method_t method);
const char* api_manager_get_response_code_name(api_mgr_response_code_t code);
const char* api_manager_get_content_type_name(api_mgr_content_type_t type);
const char* api_manager_get_ws_frame_type_name(api_mgr_ws_frame_type_t frame_type);
const char* api_manager_get_event_name(api_mgr_event_t event);

// Built-in Endpoint Handlers
hal_status_t api_manager_handle_system_status(const api_mgr_http_request_t *request, 
                                             api_mgr_http_response_t *response);
hal_status_t api_manager_handle_system_health(const api_mgr_http_request_t *request, 
                                             api_mgr_http_response_t *response);
hal_status_t api_manager_handle_network_status(const api_mgr_http_request_t *request, 
                                              api_mgr_http_response_t *response);
hal_status_t api_manager_handle_network_config(const api_mgr_http_request_t *request, 
                                              api_mgr_http_response_t *response);
hal_status_t api_manager_handle_communication_status(const api_mgr_http_request_t *request, 
                                                    api_mgr_http_response_t *response);
hal_status_t api_manager_handle_safety_status(const api_mgr_http_request_t *request, 
                                             api_mgr_http_response_t *response);
hal_status_t api_manager_handle_configuration_get(const api_mgr_http_request_t *request, 
                                                 api_mgr_http_response_t *response);
hal_status_t api_manager_handle_configuration_set(const api_mgr_http_request_t *request, 
                                                 api_mgr_http_response_t *response);

#endif // API_MANAGER_H
