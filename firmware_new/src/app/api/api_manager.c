/**
 * @file api_manager.c
 * @brief API Manager Core Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#include "api_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

// Global API Manager Instance
api_mgr_instance_t g_api_manager = {0};

// Private function declarations
static hal_status_t api_manager_initialize_defaults(void);
static hal_status_t api_manager_setup_signal_handlers(void);
static hal_status_t api_manager_cleanup_resources(void);
static hal_status_t api_manager_validate_config(const api_mgr_config_t *config);
static hal_status_t api_manager_find_endpoint(const char *path, api_mgr_http_method_t method, 
                                             api_mgr_endpoint_t **endpoint);
static hal_status_t api_manager_parse_http_method(const char *method_str, api_mgr_http_method_t *method);
static hal_status_t api_manager_parse_http_path(const char *raw_path, char *path, char *query_string);
static hal_status_t api_manager_parse_http_headers(const char *raw_headers, api_mgr_http_header_t *headers, 
                                                  uint32_t *header_count);
static hal_status_t api_manager_create_http_response_header(const api_mgr_http_response_t *response, 
                                                           char *header_buffer, size_t buffer_size);
static hal_status_t api_manager_handle_websocket_handshake(const api_mgr_http_request_t *request, 
                                                          api_mgr_http_response_t *response);
static hal_status_t api_manager_websocket_send_frame(int socket_fd, const char *data, size_t data_length, 
                                                    bool is_text);
static hal_status_t api_manager_websocket_receive_frame(int socket_fd, char *buffer, size_t buffer_size, 
                                                       size_t *received_length);

// Signal handler for graceful shutdown
static volatile bool g_shutdown_requested = false;
static void api_manager_signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Shutdown signal received");
        g_shutdown_requested = true;
    }
}

/**
 * @brief Initialize API Manager with default configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_init(const api_mgr_config_t *config) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Initializing...");
    
    // Validate input parameters
    if (config == NULL) {
        hal_log_error("API_MANAGER", "api_manager_init", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Config parameter is NULL");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    hal_status_t validation_result = api_manager_validate_config(config);
    if (validation_result != HAL_STATUS_OK) {
        hal_log_error("API_MANAGER", "api_manager_init", __LINE__, 
                     validation_result, "Configuration validation failed");
        return validation_result;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&g_api_manager.mutex, NULL) != 0) {
        hal_log_error("API_MANAGER", "api_manager_init", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to initialize mutex");
        return HAL_STATUS_ERROR;
    }
    
    // Copy configuration
    memcpy(&g_api_manager.config, config, sizeof(api_mgr_config_t));
    
    // Initialize status
    memset(&g_api_manager.status, 0, sizeof(api_mgr_status_t));
    g_api_manager.status.overall_status = HAL_DEVICE_STATUS_INITIALIZING;
    
    // Initialize endpoints array
    memset(g_api_manager.endpoints, 0, sizeof(g_api_manager.endpoints));
    g_api_manager.endpoint_count = 0;
    
    // Initialize clients array
    memset(g_api_manager.clients, 0, sizeof(g_api_manager.clients));
    g_api_manager.client_count = 0;
    
    // Initialize server sockets
    g_api_manager.http_server_socket = -1;
    g_api_manager.websocket_server_socket = -1;
    
    // Set initialization flags
    g_api_manager.initialized = true;
    g_api_manager.running = false;
    
    // Setup signal handlers
    hal_status_t signal_result = api_manager_setup_signal_handlers();
    if (signal_result != HAL_STATUS_OK) {
        hal_log_error("API_MANAGER", "api_manager_init", __LINE__, 
                     signal_result, "Failed to setup signal handlers");
        api_manager_deinit();
        return signal_result;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Initialized successfully");
    g_api_manager.status.overall_status = HAL_DEVICE_STATUS_OK;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start API Manager services
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_start(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Starting services...");
    
    if (!g_api_manager.initialized) {
        hal_log_error("API_MANAGER", "api_manager_start", __LINE__, 
                     HAL_STATUS_NOT_INITIALIZED, "API Manager not initialized");
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_api_manager.running) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Manager: Already running");
        return HAL_STATUS_ALREADY_ACTIVE;
    }
    
    // Start HTTP server
    hal_status_t http_result = api_manager_start_http_server();
    if (http_result != HAL_STATUS_OK) {
        hal_log_error("API_MANAGER", "api_manager_start", __LINE__, 
                     http_result, "Failed to start HTTP server");
        return http_result;
    }
    
    // Start WebSocket server
    hal_status_t ws_result = api_manager_start_websocket_server();
    if (ws_result != HAL_STATUS_OK) {
        hal_log_error("API_MANAGER", "api_manager_start", __LINE__, 
                     ws_result, "Failed to start WebSocket server");
        api_manager_stop_http_server();
        return ws_result;
    }
    
    g_api_manager.running = true;
    g_api_manager.status.http_server_running = true;
    g_api_manager.status.websocket_server_running = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Services started successfully");
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: HTTP server on port %d", g_api_manager.config.http_port);
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: WebSocket server on port %d", g_api_manager.config.websocket_port);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Stop API Manager services
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_stop(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Stopping services...");
    
    if (!g_api_manager.running) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Manager: Not running");
        return HAL_STATUS_OK;
    }
    
    // Stop HTTP server
    hal_status_t http_result = api_manager_stop_http_server();
    if (http_result != HAL_STATUS_OK) {
        hal_log_error("API_MANAGER", "api_manager_stop", __LINE__, 
                     http_result, "Failed to stop HTTP server");
    }
    
    // Stop WebSocket server
    hal_status_t ws_result = api_manager_stop_websocket_server();
    if (ws_result != HAL_STATUS_OK) {
        hal_log_error("API_MANAGER", "api_manager_stop", __LINE__, 
                     ws_result, "Failed to stop WebSocket server");
    }
    
    g_api_manager.running = false;
    g_api_manager.status.http_server_running = false;
    g_api_manager.status.websocket_server_running = false;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Services stopped successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize API Manager
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Deinitializing...");
    
    // Stop services if running
    if (g_api_manager.running) {
        api_manager_stop();
    }
    
    // Cleanup resources
    hal_status_t cleanup_result = api_manager_cleanup_resources();
    if (cleanup_result != HAL_STATUS_OK) {
        hal_log_error("API_MANAGER", "api_manager_deinit", __LINE__, 
                     cleanup_result, "Failed to cleanup resources");
    }
    
    // Destroy mutex
    if (g_api_manager.initialized) {
        pthread_mutex_destroy(&g_api_manager.mutex);
    }
    
    // Reset instance
    memset(&g_api_manager, 0, sizeof(api_mgr_instance_t));
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Deinitialized successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get API Manager status
 * @param status Pointer to status structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_get_status(api_mgr_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_api_manager.mutex);
    
    // Update statistics
    g_api_manager.status.statistics.uptime_ms = hal_get_timestamp_ms();
    
    // Copy status
    memcpy(status, &g_api_manager.status, sizeof(api_mgr_status_t));
    
    pthread_mutex_unlock(&g_api_manager.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Register API endpoint
 * @param endpoint Pointer to endpoint structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_register_endpoint(const api_mgr_endpoint_t *endpoint) {
    if (endpoint == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_api_manager.mutex);
    
    // Check if endpoint already exists
    api_mgr_endpoint_t *existing_endpoint;
    hal_status_t find_result = api_manager_find_endpoint(endpoint->path, endpoint->method, &existing_endpoint);
    if (find_result == HAL_STATUS_OK) {
        pthread_mutex_unlock(&g_api_manager.mutex);
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Manager: Endpoint %s %s already exists", 
                       endpoint->path, (endpoint->method == API_MGR_HTTP_GET) ? "GET" : "POST");
        return HAL_STATUS_ALREADY_EXISTS;
    }
    
    // Check if we have space for more endpoints
    if (g_api_manager.endpoint_count >= 64) {
        pthread_mutex_unlock(&g_api_manager.mutex);
        hal_log_error("API_MANAGER", "api_manager_register_endpoint", __LINE__, 
                     HAL_STATUS_NO_MEMORY, "Maximum number of endpoints reached");
        return HAL_STATUS_NO_MEMORY;
    }
    
    // Add endpoint
    memcpy(&g_api_manager.endpoints[g_api_manager.endpoint_count], endpoint, sizeof(api_mgr_endpoint_t));
    g_api_manager.endpoint_count++;
    
    pthread_mutex_unlock(&g_api_manager.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Registered endpoint %s %s", 
                   endpoint->path, (endpoint->method == API_MGR_HTTP_GET) ? "GET" : "POST");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start HTTP server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_start_http_server(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Starting HTTP server on port %d", 
                   g_api_manager.config.http_port);
    
    // Create socket
    g_api_manager.http_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_api_manager.http_server_socket < 0) {
        hal_log_error("API_MANAGER", "api_manager_start_http_server", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to create HTTP server socket: %s", strerror(errno));
        return HAL_STATUS_IO_ERROR;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(g_api_manager.http_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        hal_log_error("API_MANAGER", "api_manager_start_http_server", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to set socket options: %s", strerror(errno));
        close(g_api_manager.http_server_socket);
        g_api_manager.http_server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(g_api_manager.config.http_port);
    
    if (bind(g_api_manager.http_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        hal_log_error("API_MANAGER", "api_manager_start_http_server", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to bind HTTP server socket: %s", strerror(errno));
        close(g_api_manager.http_server_socket);
        g_api_manager.http_server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Listen for connections
    if (listen(g_api_manager.http_server_socket, 10) < 0) {
        hal_log_error("API_MANAGER", "api_manager_start_http_server", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to listen on HTTP server socket: %s", strerror(errno));
        close(g_api_manager.http_server_socket);
        g_api_manager.http_server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Start HTTP server thread
    if (pthread_create(&g_api_manager.http_server_thread, NULL, api_manager_http_server_thread, NULL) != 0) {
        hal_log_error("API_MANAGER", "api_manager_start_http_server", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to create HTTP server thread: %s", strerror(errno));
        close(g_api_manager.http_server_socket);
        g_api_manager.http_server_socket = -1;
        return HAL_STATUS_ERROR;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: HTTP server started successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Stop HTTP server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_stop_http_server(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Stopping HTTP server...");
    
    if (g_api_manager.http_server_socket >= 0) {
        close(g_api_manager.http_server_socket);
        g_api_manager.http_server_socket = -1;
    }
    
    // Wait for thread to finish
    if (g_api_manager.http_server_thread) {
        pthread_join(g_api_manager.http_server_thread, NULL);
        g_api_manager.http_server_thread = 0;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: HTTP server stopped");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start WebSocket server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_start_websocket_server(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Starting WebSocket server on port %d", 
                   g_api_manager.config.websocket_port);
    
    // Create socket
    g_api_manager.websocket_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_api_manager.websocket_server_socket < 0) {
        hal_log_error("API_MANAGER", "api_manager_start_websocket_server", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to create WebSocket server socket: %s", strerror(errno));
        return HAL_STATUS_IO_ERROR;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(g_api_manager.websocket_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        hal_log_error("API_MANAGER", "api_manager_start_websocket_server", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to set socket options: %s", strerror(errno));
        close(g_api_manager.websocket_server_socket);
        g_api_manager.websocket_server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(g_api_manager.config.websocket_port);
    
    if (bind(g_api_manager.websocket_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        hal_log_error("API_MANAGER", "api_manager_start_websocket_server", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to bind WebSocket server socket: %s", strerror(errno));
        close(g_api_manager.websocket_server_socket);
        g_api_manager.websocket_server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Listen for connections
    if (listen(g_api_manager.websocket_server_socket, 10) < 0) {
        hal_log_error("API_MANAGER", "api_manager_start_websocket_server", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to listen on WebSocket server socket: %s", strerror(errno));
        close(g_api_manager.websocket_server_socket);
        g_api_manager.websocket_server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Start WebSocket server thread
    if (pthread_create(&g_api_manager.websocket_server_thread, NULL, api_manager_websocket_server_thread, NULL) != 0) {
        hal_log_error("API_MANAGER", "api_manager_start_websocket_server", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to create WebSocket server thread: %s", strerror(errno));
        close(g_api_manager.websocket_server_socket);
        g_api_manager.websocket_server_socket = -1;
        return HAL_STATUS_ERROR;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: WebSocket server started successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Stop WebSocket server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_stop_websocket_server(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: Stopping WebSocket server...");
    
    if (g_api_manager.websocket_server_socket >= 0) {
        close(g_api_manager.websocket_server_socket);
        g_api_manager.websocket_server_socket = -1;
    }
    
    // Wait for thread to finish
    if (g_api_manager.websocket_server_thread) {
        pthread_join(g_api_manager.websocket_server_thread, NULL);
        g_api_manager.websocket_server_thread = 0;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: WebSocket server stopped");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Broadcast WebSocket message to all connected clients
 * @param message Message to broadcast
 * @param message_length Message length
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_broadcast_websocket_message(const char *message, size_t message_length) {
    if (message == NULL || message_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_api_manager.mutex);
    
    uint32_t sent_count = 0;
    for (uint32_t i = 0; i < g_api_manager.client_count; i++) {
        if (g_api_manager.clients[i].connected) {
            hal_status_t send_result = api_manager_websocket_send_frame(
                g_api_manager.clients[i].socket_fd, message, message_length, true);
            if (send_result == HAL_STATUS_OK) {
                sent_count++;
            }
        }
    }
    
    // Update statistics
    g_api_manager.status.statistics.websocket_messages_sent += sent_count;
    
    pthread_mutex_unlock(&g_api_manager.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "API Manager: Broadcast message to %d clients", sent_count);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create success response
 * @param response Response structure
 * @param json_data JSON data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_create_success_response(api_mgr_http_response_t *response, const char *json_data) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize response
    memset(response, 0, sizeof(api_mgr_http_response_t));
    response->status_code = API_MGR_RESPONSE_OK;
    gettimeofday(&response->timestamp, NULL);
    
    // Add CORS headers if enabled
    if (g_api_manager.config.enable_cors) {
        strncpy(response->headers[response->header_count].name, "Access-Control-Allow-Origin", 63);
        strncpy(response->headers[response->header_count].value, g_api_manager.config.cors_origin, 255);
        response->header_count++;
        
        strncpy(response->headers[response->header_count].name, "Access-Control-Allow-Methods", 63);
        strncpy(response->headers[response->header_count].value, "GET, POST, PUT, DELETE, OPTIONS", 255);
        response->header_count++;
        
        strncpy(response->headers[response->header_count].name, "Access-Control-Allow-Headers", 63);
        strncpy(response->headers[response->header_count].value, "Content-Type, Authorization", 255);
        response->header_count++;
    }
    
    // Add content type header
    strncpy(response->headers[response->header_count].name, "Content-Type", 63);
    strncpy(response->headers[response->header_count].value, "application/json", 255);
    response->header_count++;
    
    // Set body
    if (json_data != NULL) {
        response->body_length = strlen(json_data);
        response->body = malloc(response->body_length + 1);
        if (response->body != NULL) {
            strcpy(response->body, json_data);
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create error response
 * @param response Response structure
 * @param status_code HTTP status code
 * @param error_message Error message
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_create_error_response(api_mgr_http_response_t *response, 
                                               api_mgr_http_response_code_t status_code, 
                                               const char *error_message) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize response
    memset(response, 0, sizeof(api_mgr_http_response_t));
    response->status_code = status_code;
    gettimeofday(&response->timestamp, NULL);
    
    // Add CORS headers if enabled
    if (g_api_manager.config.enable_cors) {
        strncpy(response->headers[response->header_count].name, "Access-Control-Allow-Origin", 63);
        strncpy(response->headers[response->header_count].value, g_api_manager.config.cors_origin, 255);
        response->header_count++;
    }
    
    // Add content type header
    strncpy(response->headers[response->header_count].name, "Content-Type", 63);
    strncpy(response->headers[response->header_count].value, "application/json", 255);
    response->header_count++;
    
    // Create error JSON
    char error_json[512];
    snprintf(error_json, sizeof(error_json), 
             "{\"error\":true,\"status_code\":%d,\"message\":\"%s\"}", 
             status_code, error_message ? error_message : "Unknown error");
    
    response->body_length = strlen(error_json);
    response->body = malloc(response->body_length + 1);
    if (response->body != NULL) {
        strcpy(response->body, error_json);
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get API Manager version
 * @param major Major version number
 * @param minor Minor version number
 * @param patch Patch version number
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_manager_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch) {
    if (major == NULL || minor == NULL || patch == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *major = API_MANAGER_VERSION_MAJOR;
    *minor = API_MANAGER_VERSION_MINOR;
    *patch = API_MANAGER_VERSION_PATCH;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get API Manager version string
 * @return const char* Version string
 */
const char* api_manager_get_version_string(void) {
    return API_MANAGER_VERSION_STRING;
}

// Private function implementations
static hal_status_t api_manager_validate_config(const api_mgr_config_t *config) {
    if (config->http_port == 0 || config->http_port > 65535) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->websocket_port == 0 || config->websocket_port > 65535) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_clients == 0 || config->max_clients > API_MANAGER_MAX_CLIENTS) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t api_manager_setup_signal_handlers(void) {
    signal(SIGINT, api_manager_signal_handler);
    signal(SIGTERM, api_manager_signal_handler);
    return HAL_STATUS_OK;
}

static hal_status_t api_manager_cleanup_resources(void) {
    // Close server sockets
    if (g_api_manager.http_server_socket >= 0) {
        close(g_api_manager.http_server_socket);
        g_api_manager.http_server_socket = -1;
    }
    
    if (g_api_manager.websocket_server_socket >= 0) {
        close(g_api_manager.websocket_server_socket);
        g_api_manager.websocket_server_socket = -1;
    }
    
    // Close client connections
    for (uint32_t i = 0; i < g_api_manager.client_count; i++) {
        if (g_api_manager.clients[i].socket_fd >= 0) {
            close(g_api_manager.clients[i].socket_fd);
            g_api_manager.clients[i].socket_fd = -1;
        }
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t api_manager_find_endpoint(const char *path, api_mgr_http_method_t method, 
                                             api_mgr_endpoint_t **endpoint) {
    for (uint32_t i = 0; i < g_api_manager.endpoint_count; i++) {
        if (strcmp(g_api_manager.endpoints[i].path, path) == 0 && 
            g_api_manager.endpoints[i].method == method) {
            *endpoint = &g_api_manager.endpoints[i];
            return HAL_STATUS_OK;
        }
    }
    return HAL_STATUS_NOT_FOUND;
}

// HTTP Server Thread Function
void* api_manager_http_server_thread(void *arg) {
    (void)arg; // Unused parameter
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: HTTP server thread started");
    
    while (!g_shutdown_requested && g_api_manager.running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(g_api_manager.http_server_socket, 
                                  (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by signal
            }
            hal_log_error("API_MANAGER", "api_manager_http_server_thread", __LINE__, 
                         HAL_STATUS_IO_ERROR, "Failed to accept client connection: %s", strerror(errno));
            continue;
        }
        
        // Handle client request in a separate thread
        pthread_t client_thread;
        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;
        
        if (pthread_create(&client_thread, NULL, api_manager_handle_http_client_thread, client_socket_ptr) != 0) {
            hal_log_error("API_MANAGER", "api_manager_http_server_thread", __LINE__, 
                         HAL_STATUS_ERROR, "Failed to create client thread: %s", strerror(errno));
            close(client_socket);
            free(client_socket_ptr);
        } else {
            pthread_detach(client_thread);
        }
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: HTTP server thread stopped");
    return NULL;
}

// WebSocket Server Thread Function
void* api_manager_websocket_server_thread(void *arg) {
    (void)arg; // Unused parameter
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: WebSocket server thread started");
    
    while (!g_shutdown_requested && g_api_manager.running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(g_api_manager.websocket_server_socket, 
                                  (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by signal
            }
            hal_log_error("API_MANAGER", "api_manager_websocket_server_thread", __LINE__, 
                         HAL_STATUS_IO_ERROR, "Failed to accept WebSocket client connection: %s", strerror(errno));
            continue;
        }
        
        // Add WebSocket client
        hal_status_t add_result = api_manager_add_websocket_client(client_socket);
        if (add_result != HAL_STATUS_OK) {
            hal_log_error("API_MANAGER", "api_manager_websocket_server_thread", __LINE__, 
                         add_result, "Failed to add WebSocket client");
            close(client_socket);
        }
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Manager: WebSocket server thread stopped");
    return NULL;
}

// Placeholder functions for future implementation
void* api_manager_handle_http_client_thread(void *arg) {
    // TODO: Implement HTTP client handling
    (void)arg; // Unused parameter
    return NULL;
}

hal_status_t api_manager_add_websocket_client(int socket_fd) {
    // TODO: Implement WebSocket client addition
    (void)socket_fd; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_manager_websocket_send_frame(int socket_fd, const char *data, size_t data_length, bool is_text) {
    // TODO: Implement WebSocket frame sending
    (void)socket_fd; // Unused parameter
    (void)data; // Unused parameter
    (void)data_length; // Unused parameter
    (void)is_text; // Unused parameter
    return HAL_STATUS_OK;
}
