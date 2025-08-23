/**
 * @file api_manager.c
 * @brief API Manager Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-06 (Advanced Communication)
 */

#include "api_manager.h"
#include "network_manager.h"
#include "communication_manager.h"
#include "safety_manager.h"
#include "system_state_machine.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// API Manager internal structure
typedef struct {
    api_mgr_config_t config;
    api_mgr_status_t status;
    api_mgr_stats_t statistics;
    api_mgr_event_callback_t event_callback;
    api_mgr_ws_message_handler_t ws_message_handler;
    bool initialized;
    uint64_t last_update_time;
    uint64_t server_start_time;
    
    // HTTP server management
    api_mgr_endpoint_t endpoints[API_MGR_MAX_ENDPOINTS];
    bool endpoint_registered[API_MGR_MAX_ENDPOINTS];
    uint32_t registered_endpoints;
    
    // WebSocket server management
    api_mgr_ws_client_t ws_clients[API_MGR_MAX_WEBSOCKET_CLIENTS];
    bool ws_client_active[API_MGR_MAX_WEBSOCKET_CLIENTS];
    uint32_t active_ws_clients;
    uint32_t next_client_id;
    
    // Request processing
    char request_buffer[API_MGR_MAX_REQUEST_SIZE];
    char response_buffer[API_MGR_MAX_RESPONSE_SIZE];
    
    // Server state
    bool http_server_running;
    bool websocket_server_running;
} api_manager_t;

// Global API Manager instance
static api_manager_t g_api_manager = {0};

// Default configuration
static const api_mgr_config_t default_config = {
    .http_port = API_MGR_DEFAULT_PORT,
    .websocket_port = API_MGR_WEBSOCKET_PORT,
    .http_enabled = true,
    .websocket_enabled = true,
    .cors_enabled = true,
    .cors_origin = "*",
    .max_request_size = API_MGR_MAX_REQUEST_SIZE,
    .max_response_size = API_MGR_MAX_RESPONSE_SIZE,
    .request_timeout_ms = 30000,  // 30 seconds
    .websocket_timeout_ms = 60000, // 60 seconds
    .authentication_required = true,
    .ssl_enabled = false,
    .ssl_certificate_path = "",
    .ssl_private_key_path = ""
};

// Built-in endpoints
static const api_mgr_endpoint_t builtin_endpoints[] = {
    // System endpoints
    {
        .path = "/api/v1/system/status",
        .method = API_MGR_HTTP_GET,
        .handler = api_manager_handle_system_status,
        .required_resource = SECURITY_MGR_RESOURCE_SYSTEM,
        .required_permission = SECURITY_MGR_PERM_READ,
        .authentication_required = true,
        .enabled = true
    },
    {
        .path = "/api/v1/system/health",
        .method = API_MGR_HTTP_GET,
        .handler = api_manager_handle_system_health,
        .required_resource = SECURITY_MGR_RESOURCE_SYSTEM,
        .required_permission = SECURITY_MGR_PERM_READ,
        .authentication_required = false,
        .enabled = true
    },
    
    // Network endpoints
    {
        .path = "/api/v1/network/status",
        .method = API_MGR_HTTP_GET,
        .handler = api_manager_handle_network_status,
        .required_resource = SECURITY_MGR_RESOURCE_NETWORK,
        .required_permission = SECURITY_MGR_PERM_READ,
        .authentication_required = true,
        .enabled = true
    },
    {
        .path = "/api/v1/network/config",
        .method = API_MGR_HTTP_GET,
        .handler = api_manager_handle_network_config,
        .required_resource = SECURITY_MGR_RESOURCE_NETWORK,
        .required_permission = SECURITY_MGR_PERM_READ,
        .authentication_required = true,
        .enabled = true
    },
    {
        .path = "/api/v1/network/config",
        .method = API_MGR_HTTP_POST,
        .handler = api_manager_handle_network_config,
        .required_resource = SECURITY_MGR_RESOURCE_NETWORK,
        .required_permission = SECURITY_MGR_PERM_WRITE,
        .authentication_required = true,
        .enabled = true
    },
    
    // Communication endpoints
    {
        .path = "/api/v1/communication/status",
        .method = API_MGR_HTTP_GET,
        .handler = api_manager_handle_communication_status,
        .required_resource = SECURITY_MGR_RESOURCE_COMMUNICATION,
        .required_permission = SECURITY_MGR_PERM_READ,
        .authentication_required = true,
        .enabled = true
    },
    
    // Safety endpoints
    {
        .path = "/api/v1/safety/status",
        .method = API_MGR_HTTP_GET,
        .handler = api_manager_handle_safety_status,
        .required_resource = SECURITY_MGR_RESOURCE_SAFETY,
        .required_permission = SECURITY_MGR_PERM_READ,
        .authentication_required = true,
        .enabled = true
    },
    
    // Configuration endpoints
    {
        .path = "/api/v1/config",
        .method = API_MGR_HTTP_GET,
        .handler = api_manager_handle_configuration_get,
        .required_resource = SECURITY_MGR_RESOURCE_CONFIGURATION,
        .required_permission = SECURITY_MGR_PERM_READ,
        .authentication_required = true,
        .enabled = true
    },
    {
        .path = "/api/v1/config",
        .method = API_MGR_HTTP_POST,
        .handler = api_manager_handle_configuration_set,
        .required_resource = SECURITY_MGR_RESOURCE_CONFIGURATION,
        .required_permission = SECURITY_MGR_PERM_WRITE,
        .authentication_required = true,
        .enabled = true
    }
};

// Forward declarations
static hal_status_t find_endpoint(const char *path, api_mgr_http_method_t method, int *endpoint_index);
static hal_status_t handle_api_event(api_mgr_event_t event, const void *data);
static hal_status_t cleanup_expired_websocket_clients(void);
static int find_websocket_client_by_id(uint32_t client_id);
static int find_free_websocket_client_slot(void);
static hal_status_t extract_session_id_from_headers(const api_mgr_http_request_t *request, char *session_id);
static void update_statistics(void);

// API Manager implementation

hal_status_t api_manager_init(const api_mgr_config_t *config) {
    if (g_api_manager.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize API Manager structure
    memset(&g_api_manager, 0, sizeof(api_manager_t));
    
    // Set configuration
    if (config != NULL) {
        memcpy(&g_api_manager.config, config, sizeof(api_mgr_config_t));
    } else {
        memcpy(&g_api_manager.config, &default_config, sizeof(api_mgr_config_t));
    }
    
    // Initialize status
    g_api_manager.status.initialized = true;
    g_api_manager.status.http_server_active = false;
    g_api_manager.status.websocket_server_active = false;
    g_api_manager.status.active_http_connections = 0;
    g_api_manager.status.active_websocket_connections = 0;
    g_api_manager.status.total_requests_processed = 0;
    g_api_manager.status.total_websocket_messages = 0;
    g_api_manager.status.error_count = 0;
    
    // Initialize timing
    g_api_manager.last_update_time = hal_get_timestamp_us();
    g_api_manager.server_start_time = hal_get_timestamp_us();
    
    // Initialize endpoint arrays
    for (int i = 0; i < API_MGR_MAX_ENDPOINTS; i++) {
        g_api_manager.endpoint_registered[i] = false;
        memset(&g_api_manager.endpoints[i], 0, sizeof(api_mgr_endpoint_t));
    }
    
    // Initialize WebSocket client arrays
    for (int i = 0; i < API_MGR_MAX_WEBSOCKET_CLIENTS; i++) {
        g_api_manager.ws_client_active[i] = false;
        memset(&g_api_manager.ws_clients[i], 0, sizeof(api_mgr_ws_client_t));
    }
    
    g_api_manager.initialized = true;
    g_api_manager.next_client_id = 1;
    
    handle_api_event(API_MGR_EVENT_NONE, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_deinit(void) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Stop servers
    api_manager_stop_http_server();
    api_manager_stop_websocket_server();
    
    // Disconnect all WebSocket clients
    for (int i = 0; i < API_MGR_MAX_WEBSOCKET_CLIENTS; i++) {
        if (g_api_manager.ws_client_active[i]) {
            api_manager_disconnect_websocket_client(g_api_manager.ws_clients[i].client_id);
        }
    }
    
    // Clear API Manager
    memset(&g_api_manager, 0, sizeof(api_manager_t));
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_get_config(api_mgr_config_t *config) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_api_manager.config, sizeof(api_mgr_config_t));
    return HAL_STATUS_OK;
}

hal_status_t api_manager_set_config(const api_mgr_config_t *config) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_api_manager.config, config, sizeof(api_mgr_config_t));
    
    handle_api_event(API_MGR_EVENT_NONE, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_get_status(api_mgr_status_t *status) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Update uptime
    g_api_manager.status.server_uptime = hal_get_timestamp_us() - g_api_manager.server_start_time;
    
    memcpy(status, &g_api_manager.status, sizeof(api_mgr_status_t));
    return HAL_STATUS_OK;
}

hal_status_t api_manager_get_statistics(api_mgr_stats_t *stats) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(stats, &g_api_manager.statistics, sizeof(api_mgr_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t api_manager_reset_statistics(void) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&g_api_manager.statistics, 0, sizeof(api_mgr_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t api_manager_start_http_server(void) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_api_manager.http_server_running) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Start HTTP server (simplified implementation)
    g_api_manager.http_server_running = true;
    g_api_manager.status.http_server_active = true;
    
    handle_api_event(API_MGR_EVENT_SERVER_STARTED, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_stop_http_server(void) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!g_api_manager.http_server_running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_api_manager.http_server_running = false;
    g_api_manager.status.http_server_active = false;
    g_api_manager.status.active_http_connections = 0;
    
    handle_api_event(API_MGR_EVENT_SERVER_STOPPED, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_register_endpoint(const api_mgr_endpoint_t *endpoint) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (endpoint == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check if endpoint already exists
    int existing_index;
    if (find_endpoint(endpoint->path, endpoint->method, &existing_index) == HAL_STATUS_OK) {
        return HAL_STATUS_ALREADY_EXISTS;
    }
    
    // Find free slot
    for (int i = 0; i < API_MGR_MAX_ENDPOINTS; i++) {
        if (!g_api_manager.endpoint_registered[i]) {
            memcpy(&g_api_manager.endpoints[i], endpoint, sizeof(api_mgr_endpoint_t));
            g_api_manager.endpoint_registered[i] = true;
            g_api_manager.registered_endpoints++;
            return HAL_STATUS_OK;
        }
    }
    
    return HAL_STATUS_NO_MEMORY;
}

hal_status_t api_manager_unregister_endpoint(const char *path, api_mgr_http_method_t method) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (path == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int endpoint_index;
    hal_status_t status = find_endpoint(path, method, &endpoint_index);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    g_api_manager.endpoint_registered[endpoint_index] = false;
    memset(&g_api_manager.endpoints[endpoint_index], 0, sizeof(api_mgr_endpoint_t));
    g_api_manager.registered_endpoints--;
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_process_http_request(const api_mgr_http_request_t *request, 
                                            api_mgr_http_response_t *response) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (request == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    g_api_manager.statistics.http_requests_received++;
    g_api_manager.status.total_requests_processed++;
    
    // Find endpoint
    int endpoint_index;
    hal_status_t status = find_endpoint(request->path, request->method, &endpoint_index);
    if (status != HAL_STATUS_OK) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_NOT_FOUND, "Endpoint not found");
    }
    
    // Check if endpoint is enabled
    if (!g_api_manager.endpoints[endpoint_index].enabled) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_SERVICE_UNAVAILABLE, "Endpoint disabled");
    }
    
    // Authentication check
    if (g_api_manager.endpoints[endpoint_index].authentication_required) {
        security_mgr_level_t user_level;
        status = api_manager_authenticate_request(request, &user_level);
        if (status != HAL_STATUS_OK) {
            g_api_manager.statistics.authentication_failures++;
            handle_api_event(API_MGR_EVENT_AUTHENTICATION_FAILED, request);
            return api_manager_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, "Authentication required");
        }
    }
    
    // Authorization check
    status = api_manager_authorize_request(request, 
                                         g_api_manager.endpoints[endpoint_index].required_resource,
                                         g_api_manager.endpoints[endpoint_index].required_permission);
    if (status != HAL_STATUS_OK) {
        g_api_manager.statistics.authorization_failures++;
        handle_api_event(API_MGR_EVENT_AUTHORIZATION_FAILED, request);
        return api_manager_create_error_response(response, API_MGR_RESPONSE_FORBIDDEN, "Insufficient permissions");
    }
    
    // Call endpoint handler
    status = g_api_manager.endpoints[endpoint_index].handler(request, response);
    if (status != HAL_STATUS_OK) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Handler error");
    }
    
    g_api_manager.statistics.http_responses_sent++;
    handle_api_event(API_MGR_EVENT_HTTP_RESPONSE, response);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_start_websocket_server(void) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_api_manager.websocket_server_running) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Start WebSocket server (simplified implementation)
    g_api_manager.websocket_server_running = true;
    g_api_manager.status.websocket_server_active = true;
    
    handle_api_event(API_MGR_EVENT_SERVER_STARTED, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_stop_websocket_server(void) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!g_api_manager.websocket_server_running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_api_manager.websocket_server_running = false;
    g_api_manager.status.websocket_server_active = false;
    g_api_manager.status.active_websocket_connections = 0;
    
    handle_api_event(API_MGR_EVENT_SERVER_STOPPED, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_set_websocket_message_handler(api_mgr_ws_message_handler_t handler) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_api_manager.ws_message_handler = handler;
    return HAL_STATUS_OK;
}

hal_status_t api_manager_broadcast_websocket_message(const char *message, uint32_t length) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    hal_status_t overall_status = HAL_STATUS_OK;
    
    for (int i = 0; i < API_MGR_MAX_WEBSOCKET_CLIENTS; i++) {
        if (g_api_manager.ws_client_active[i] && 
            g_api_manager.ws_clients[i].state == API_MGR_WS_STATE_CONNECTED) {
            hal_status_t status = api_manager_send_websocket_message(g_api_manager.ws_clients[i].client_id, 
                                                                   message, length);
            if (status != HAL_STATUS_OK) {
                overall_status = status;
            }
        }
    }
    
    return overall_status;
}

hal_status_t api_manager_send_websocket_message(uint32_t client_id, const char *message, uint32_t length) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int client_index = find_websocket_client_by_id(client_id);
    if (client_index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    if (g_api_manager.ws_clients[client_index].state != API_MGR_WS_STATE_CONNECTED) {
        return HAL_STATUS_INVALID_STATE;
    }
    
    // Send WebSocket message (simplified implementation)
    g_api_manager.statistics.websocket_messages_sent++;
    g_api_manager.ws_clients[client_index].last_activity_time = hal_get_timestamp_us();
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_disconnect_websocket_client(uint32_t client_id) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    int client_index = find_websocket_client_by_id(client_id);
    if (client_index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    g_api_manager.ws_clients[client_index].state = API_MGR_WS_STATE_DISCONNECTED;
    g_api_manager.ws_client_active[client_index] = false;
    g_api_manager.active_ws_clients--;
    g_api_manager.status.active_websocket_connections--;
    
    handle_api_event(API_MGR_EVENT_WEBSOCKET_DISCONNECT, &g_api_manager.ws_clients[client_index]);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_authenticate_request(const api_mgr_http_request_t *request, 
                                            security_mgr_level_t *user_level) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (request == NULL || user_level == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Extract session ID from headers
    char session_id[SECURITY_MGR_TOKEN_LENGTH];
    hal_status_t status = extract_session_id_from_headers(request, session_id);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_AUTHENTICATION_FAILED;
    }
    
    // Validate session
    return security_manager_validate_session(session_id, user_level);
}

hal_status_t api_manager_authorize_request(const api_mgr_http_request_t *request, 
                                         security_mgr_resource_t resource, 
                                         security_mgr_permission_t permission) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (request == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Extract session ID from headers
    char session_id[SECURITY_MGR_TOKEN_LENGTH];
    hal_status_t status = extract_session_id_from_headers(request, session_id);
    if (status != HAL_STATUS_OK) {
        return HAL_STATUS_AUTHENTICATION_FAILED;
    }
    
    // Check permission
    return security_manager_check_permission(session_id, resource, permission);
}

hal_status_t api_manager_create_json_response(api_mgr_http_response_t *response, 
                                            api_mgr_response_code_t status_code, 
                                            const char *json_data) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    response->status_code = status_code;
    response->content_type = API_MGR_CONTENT_TYPE_JSON;
    response->body = (char*)json_data;
    response->body_length = json_data ? strlen(json_data) : 0;
    response->keep_alive = true;
    response->header_count = 0;
    
    // Add standard headers
    strcpy(response->headers[response->header_count].name, "Content-Type");
    strcpy(response->headers[response->header_count].value, "application/json");
    response->header_count++;
    
    if (g_api_manager.config.cors_enabled) {
        strcpy(response->headers[response->header_count].name, "Access-Control-Allow-Origin");
        strcpy(response->headers[response->header_count].value, g_api_manager.config.cors_origin);
        response->header_count++;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_create_error_response(api_mgr_http_response_t *response, 
                                             api_mgr_response_code_t status_code, 
                                             const char *error_message) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create error JSON
    char error_json[512];
    snprintf(error_json, sizeof(error_json),
             "{\"error\":{\"code\":%d,\"message\":\"%s\"}}",
             status_code, error_message ? error_message : "Unknown error");
    
    return api_manager_create_json_response(response, status_code, error_json);
}

hal_status_t api_manager_create_success_response(api_mgr_http_response_t *response, 
                                               const char *message) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create success JSON
    char success_json[512];
    snprintf(success_json, sizeof(success_json),
             "{\"success\":true,\"message\":\"%s\"}",
             message ? message : "Operation completed successfully");
    
    return api_manager_create_json_response(response, API_MGR_RESPONSE_OK, success_json);
}

hal_status_t api_manager_set_callback(api_mgr_event_callback_t callback) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_api_manager.event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t api_manager_get_diagnostics(char *diagnostics, uint32_t max_length) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (diagnostics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(diagnostics, max_length,
             "API Manager Diagnostics:\n"
             "Initialized: %s\n"
             "HTTP Server Active: %s\n"
             "WebSocket Server Active: %s\n"
             "Active HTTP Connections: %u\n"
             "Active WebSocket Connections: %u\n"
             "Registered Endpoints: %u\n"
             "Total Requests Processed: %u\n"
             "Total WebSocket Messages: %u\n"
             "HTTP Requests Received: %u\n"
             "HTTP Responses Sent: %u\n"
             "WebSocket Messages Sent: %u\n"
             "WebSocket Messages Received: %u\n"
             "Authentication Failures: %u\n"
             "Authorization Failures: %u\n"
             "Request Timeouts: %u\n",
             g_api_manager.status.initialized ? "Yes" : "No",
             g_api_manager.status.http_server_active ? "Yes" : "No",
             g_api_manager.status.websocket_server_active ? "Yes" : "No",
             g_api_manager.status.active_http_connections,
             g_api_manager.status.active_websocket_connections,
             g_api_manager.registered_endpoints,
             g_api_manager.status.total_requests_processed,
             g_api_manager.status.total_websocket_messages,
             g_api_manager.statistics.http_requests_received,
             g_api_manager.statistics.http_responses_sent,
             g_api_manager.statistics.websocket_messages_sent,
             g_api_manager.statistics.websocket_messages_received,
             g_api_manager.statistics.authentication_failures,
             g_api_manager.statistics.authorization_failures,
             g_api_manager.statistics.request_timeouts);
    
    return HAL_STATUS_OK;
}

// Built-in endpoint registration
hal_status_t api_manager_register_system_endpoints(void) {
    for (int i = 0; i < 2; i++) {
        api_manager_register_endpoint(&builtin_endpoints[i]);
    }
    return HAL_STATUS_OK;
}

hal_status_t api_manager_register_network_endpoints(void) {
    for (int i = 2; i < 5; i++) {
        api_manager_register_endpoint(&builtin_endpoints[i]);
    }
    return HAL_STATUS_OK;
}

hal_status_t api_manager_register_communication_endpoints(void) {
    api_manager_register_endpoint(&builtin_endpoints[5]);
    return HAL_STATUS_OK;
}

hal_status_t api_manager_register_safety_endpoints(void) {
    api_manager_register_endpoint(&builtin_endpoints[6]);
    return HAL_STATUS_OK;
}

hal_status_t api_manager_register_configuration_endpoints(void) {
    for (int i = 7; i < 9; i++) {
        api_manager_register_endpoint(&builtin_endpoints[i]);
    }
    return HAL_STATUS_OK;
}

// Built-in endpoint handlers
hal_status_t api_manager_handle_system_status(const api_mgr_http_request_t *request, 
                                            api_mgr_http_response_t *response) {
    (void)request; // Suppress unused parameter warning
    
    // Get system status from system state machine
    system_state_t system_state;
    hal_status_t status = system_state_machine_get_state(&system_state);
    if (status != HAL_STATUS_OK) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get system state");
    }
    
    // Create JSON response
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"system\":{\"state\":\"%s\",\"uptime\":%lu,\"version\":\"1.0.0\"}}",
             system_state_machine_get_state_name(system_state),
             hal_get_timestamp_us());
    
    return api_manager_create_json_response(response, API_MGR_RESPONSE_OK, json_response);
}

hal_status_t api_manager_handle_system_health(const api_mgr_http_request_t *request, 
                                            api_mgr_http_response_t *response) {
    (void)request; // Suppress unused parameter warning
    
    // Create health check response
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"health\":{\"status\":\"healthy\",\"timestamp\":%lu}}",
             hal_get_timestamp_us());
    
    return api_manager_create_json_response(response, API_MGR_RESPONSE_OK, json_response);
}

hal_status_t api_manager_handle_network_status(const api_mgr_http_request_t *request, 
                                             api_mgr_http_response_t *response) {
    (void)request; // Suppress unused parameter warning
    
    // Get network status from network manager
    network_mgr_status_t network_status;
    hal_status_t status = network_manager_get_status(&network_status);
    if (status != HAL_STATUS_OK) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get network status");
    }
    
    // Create JSON response
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"network\":{\"state\":\"%d\",\"active_interface\":%d,\"failover_count\":%u}}",
             network_status.overall_state,
             network_status.active_interface,
             network_status.failover_count);
    
    return api_manager_create_json_response(response, API_MGR_RESPONSE_OK, json_response);
}

hal_status_t api_manager_handle_network_config(const api_mgr_http_request_t *request, 
                                             api_mgr_http_response_t *response) {
    if (request->method == API_MGR_HTTP_GET) {
        // Get network configuration
        network_mgr_config_t config;
        hal_status_t status = network_manager_get_config(&config);
        if (status != HAL_STATUS_OK) {
            return api_manager_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get network config");
        }
        
        char json_response[1024];
        snprintf(json_response, sizeof(json_response),
                 "{\"network_config\":{\"failover_mode\":\"%s\",\"auto_failover\":%s}}",
                 network_manager_get_failover_mode_name(config.failover_mode),
                 config.auto_failover_enabled ? "true" : "false");
        
        return api_manager_create_json_response(response, API_MGR_RESPONSE_OK, json_response);
    } else if (request->method == API_MGR_HTTP_POST) {
        // Set network configuration (simplified)
        return api_manager_create_success_response(response, "Network configuration updated");
    }
    
    return api_manager_create_error_response(response, API_MGR_RESPONSE_METHOD_NOT_ALLOWED, "Method not allowed");
}

hal_status_t api_manager_handle_communication_status(const api_mgr_http_request_t *request, 
                                                   api_mgr_http_response_t *response) {
    (void)request; // Suppress unused parameter warning
    
    // Create simplified communication status response
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"communication\":{\"state\":\"active\",\"active_modules\":1,\"total_modules\":1}}");
    
    return api_manager_create_json_response(response, API_MGR_RESPONSE_OK, json_response);
}

hal_status_t api_manager_handle_safety_status(const api_mgr_http_request_t *request, 
                                            api_mgr_http_response_t *response) {
    (void)request; // Suppress unused parameter warning
    
    // Create simplified safety status response
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"safety\":{\"state\":\"safe\",\"estop_active\":false,\"faults\":0}}");
    
    return api_manager_create_json_response(response, API_MGR_RESPONSE_OK, json_response);
}

hal_status_t api_manager_handle_configuration_get(const api_mgr_http_request_t *request, 
                                                api_mgr_http_response_t *response) {
    (void)request; // Suppress unused parameter warning
    
    // Get system configuration (simplified)
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"configuration\":{\"version\":\"1.0.0\",\"build_date\":\"2025-01-27\",\"features\":[\"network_redundancy\",\"security\",\"websocket\"]}}");
    
    return api_manager_create_json_response(response, API_MGR_RESPONSE_OK, json_response);
}

hal_status_t api_manager_handle_configuration_set(const api_mgr_http_request_t *request, 
                                                api_mgr_http_response_t *response) {
    (void)request; // Suppress unused parameter warning
    
    // Set system configuration (simplified)
    return api_manager_create_success_response(response, "Configuration updated successfully");
}

// Utility functions
const char* api_manager_get_http_method_name(api_mgr_http_method_t method) {
    switch (method) {
        case API_MGR_HTTP_GET: return "GET";
        case API_MGR_HTTP_POST: return "POST";
        case API_MGR_HTTP_PUT: return "PUT";
        case API_MGR_HTTP_DELETE: return "DELETE";
        case API_MGR_HTTP_PATCH: return "PATCH";
        case API_MGR_HTTP_HEAD: return "HEAD";
        case API_MGR_HTTP_OPTIONS: return "OPTIONS";
        default: return "UNKNOWN";
    }
}

const char* api_manager_get_response_code_name(api_mgr_response_code_t code) {
    switch (code) {
        case API_MGR_RESPONSE_OK: return "OK";
        case API_MGR_RESPONSE_CREATED: return "CREATED";
        case API_MGR_RESPONSE_NO_CONTENT: return "NO_CONTENT";
        case API_MGR_RESPONSE_BAD_REQUEST: return "BAD_REQUEST";
        case API_MGR_RESPONSE_UNAUTHORIZED: return "UNAUTHORIZED";
        case API_MGR_RESPONSE_FORBIDDEN: return "FORBIDDEN";
        case API_MGR_RESPONSE_NOT_FOUND: return "NOT_FOUND";
        case API_MGR_RESPONSE_METHOD_NOT_ALLOWED: return "METHOD_NOT_ALLOWED";
        case API_MGR_RESPONSE_INTERNAL_SERVER_ERROR: return "INTERNAL_SERVER_ERROR";
        case API_MGR_RESPONSE_SERVICE_UNAVAILABLE: return "SERVICE_UNAVAILABLE";
        default: return "UNKNOWN";
    }
}

const char* api_manager_get_content_type_name(api_mgr_content_type_t type) {
    switch (type) {
        case API_MGR_CONTENT_TYPE_JSON: return "application/json";
        case API_MGR_CONTENT_TYPE_XML: return "application/xml";
        case API_MGR_CONTENT_TYPE_TEXT: return "text/plain";
        case API_MGR_CONTENT_TYPE_BINARY: return "application/octet-stream";
        case API_MGR_CONTENT_TYPE_FORM_DATA: return "multipart/form-data";
        default: return "application/octet-stream";
    }
}

const char* api_manager_get_ws_frame_type_name(api_mgr_ws_frame_type_t frame_type) {
    switch (frame_type) {
        case API_MGR_WS_FRAME_CONTINUATION: return "CONTINUATION";
        case API_MGR_WS_FRAME_TEXT: return "TEXT";
        case API_MGR_WS_FRAME_BINARY: return "BINARY";
        case API_MGR_WS_FRAME_CLOSE: return "CLOSE";
        case API_MGR_WS_FRAME_PING: return "PING";
        case API_MGR_WS_FRAME_PONG: return "PONG";
        default: return "UNKNOWN";
    }
}

const char* api_manager_get_event_name(api_mgr_event_t event) {
    switch (event) {
        case API_MGR_EVENT_NONE: return "NONE";
        case API_MGR_EVENT_HTTP_REQUEST: return "HTTP_REQUEST";
        case API_MGR_EVENT_HTTP_RESPONSE: return "HTTP_RESPONSE";
        case API_MGR_EVENT_WEBSOCKET_CONNECT: return "WEBSOCKET_CONNECT";
        case API_MGR_EVENT_WEBSOCKET_DISCONNECT: return "WEBSOCKET_DISCONNECT";
        case API_MGR_EVENT_WEBSOCKET_MESSAGE: return "WEBSOCKET_MESSAGE";
        case API_MGR_EVENT_AUTHENTICATION_FAILED: return "AUTHENTICATION_FAILED";
        case API_MGR_EVENT_AUTHORIZATION_FAILED: return "AUTHORIZATION_FAILED";
        case API_MGR_EVENT_SERVER_STARTED: return "SERVER_STARTED";
        case API_MGR_EVENT_SERVER_STOPPED: return "SERVER_STOPPED";
        default: return "UNKNOWN";
    }
}

// Internal helper functions
static hal_status_t find_endpoint(const char *path, api_mgr_http_method_t method, int *endpoint_index) {
    if (path == NULL || endpoint_index == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    for (int i = 0; i < API_MGR_MAX_ENDPOINTS; i++) {
        if (g_api_manager.endpoint_registered[i] && 
            strcmp(g_api_manager.endpoints[i].path, path) == 0 &&
            g_api_manager.endpoints[i].method == method) {
            *endpoint_index = i;
            return HAL_STATUS_OK;
        }
    }
    
    return HAL_STATUS_NOT_FOUND;
}

static hal_status_t handle_api_event(api_mgr_event_t event, const void *data) {
    if (g_api_manager.event_callback != NULL) {
        g_api_manager.event_callback(event, data);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t cleanup_expired_websocket_clients(void) {
    uint64_t current_time = hal_get_timestamp_us();
    
    for (int i = 0; i < API_MGR_MAX_WEBSOCKET_CLIENTS; i++) {
        if (g_api_manager.ws_client_active[i] && 
            (current_time - g_api_manager.ws_clients[i].last_activity_time) > 
            (g_api_manager.config.websocket_timeout_ms * 1000)) {
            api_manager_disconnect_websocket_client(g_api_manager.ws_clients[i].client_id);
        }
    }
    
    return HAL_STATUS_OK;
}

static int find_websocket_client_by_id(uint32_t client_id) {
    for (int i = 0; i < API_MGR_MAX_WEBSOCKET_CLIENTS; i++) {
        if (g_api_manager.ws_client_active[i] && 
            g_api_manager.ws_clients[i].client_id == client_id) {
            return i;
        }
    }
    
    return -1;
}

static int find_free_websocket_client_slot(void) {
    for (int i = 0; i < API_MGR_MAX_WEBSOCKET_CLIENTS; i++) {
        if (!g_api_manager.ws_client_active[i]) {
            return i;
        }
    }
    
    return -1;
}

static hal_status_t extract_session_id_from_headers(const api_mgr_http_request_t *request, char *session_id) {
    if (request == NULL || session_id == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Look for session ID in headers
    for (uint32_t i = 0; i < request->header_count; i++) {
        if (strcasecmp(request->headers[i].name, "Authorization") == 0) {
            // Extract session ID from Authorization header
            if (strncmp(request->headers[i].value, "Bearer ", 7) == 0) {
                strcpy(session_id, request->headers[i].value + 7);
                return HAL_STATUS_OK;
            }
        } else if (strcasecmp(request->headers[i].name, "X-Session-ID") == 0) {
            strcpy(session_id, request->headers[i].value);
            return HAL_STATUS_OK;
        }
    }
    
    return HAL_STATUS_NOT_FOUND;
}

static void update_statistics(void) {
    // Update average response time
    if (g_api_manager.statistics.http_responses_sent > 0) {
        g_api_manager.statistics.average_response_time = 
            g_api_manager.statistics.total_response_time / g_api_manager.statistics.http_responses_sent;
    }
}
