/**
 * @file websocket_server.c
 * @brief WebSocket Server Foundation Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#include "websocket_server.h"
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
#include <pthread.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

// Global WebSocket Server Instance
ws_server_instance_t g_ws_server = {0};

// Private function declarations
static hal_status_t ws_server_initialize_defaults(ws_server_config_t *config);
static hal_status_t ws_server_setup_signal_handlers(void);
static hal_status_t ws_server_cleanup_resources(void);
static hal_status_t ws_server_validate_config(const ws_server_config_t *config);
static hal_status_t ws_server_find_client(int socket_fd, ws_client_t **client);
static hal_status_t ws_server_parse_http_headers(const char *request, char *websocket_key, size_t key_size);
static hal_status_t ws_server_calculate_accept_key(const char *websocket_key, char *accept_key, size_t accept_key_size);
static hal_status_t ws_server_send_http_response(int socket_fd, const char *response);
static hal_status_t ws_server_read_data(int socket_fd, uint8_t *buffer, size_t buffer_size, size_t *received_length);
static hal_status_t ws_server_write_data(int socket_fd, const uint8_t *data, size_t data_length);

// Signal handler for graceful shutdown
static volatile bool g_ws_shutdown_requested = false;
static void ws_server_signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Shutdown signal received");
        g_ws_shutdown_requested = true;
    }
}

/**
 * @brief Initialize WebSocket Server with configuration
 * @param config Pointer to configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_init(const ws_server_config_t *config) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Initializing...");
    
    // Check if already initialized
    if (g_ws_server.initialized) {
        hal_log_error("WS_SERVER", "ws_server_init", __LINE__, 
                     HAL_STATUS_ERROR, "WebSocket Server already initialized");
        return HAL_STATUS_ERROR;
    }
    
    // Validate input parameters
    if (config == NULL) {
        hal_log_error("WS_SERVER", "ws_server_init", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Config parameter is NULL");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize default values for any missing fields
    ws_server_config_t config_copy = *config; // Make a copy to modify
    hal_status_t defaults_result = ws_server_initialize_defaults(&config_copy);
    if (defaults_result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_init", __LINE__, 
                     defaults_result, "Failed to initialize default values");
        return defaults_result;
    }
    
    // Validate configuration
    hal_status_t validation_result = ws_server_validate_config(&config_copy);
    if (validation_result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_init", __LINE__, 
                     validation_result, "Configuration validation failed");
        return validation_result;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&g_ws_server.mutex, NULL) != 0) {
        hal_log_error("WS_SERVER", "ws_server_init", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to initialize mutex");
        return HAL_STATUS_ERROR;
    }
    
    // Copy configuration (with defaults applied)
    memcpy(&g_ws_server.config, &config_copy, sizeof(ws_server_config_t));
    
    // Initialize status
    memset(&g_ws_server.status, 0, sizeof(ws_server_status_t));
    g_ws_server.status.overall_status = HAL_DEVICE_STATUS_INITIALIZING;
    
    // Initialize clients array
    memset(g_ws_server.clients, 0, sizeof(g_ws_server.clients));
    g_ws_server.client_count = 0;
    
    // Initialize message handler
    g_ws_server.message_handler = NULL;
    
    // Initialize telemetry
    g_ws_server.telemetry_callback = NULL;
    g_ws_server.telemetry_streaming = false;
    g_ws_server.telemetry_interval_ms = 1000; // Default 1 second
    
    // Initialize server socket
    g_ws_server.server_socket = -1;
    
    // Set initialization flags
    g_ws_server.initialized = true;
    g_ws_server.running = false;
    
    // Setup signal handlers
    hal_status_t signal_result = ws_server_setup_signal_handlers();
    if (signal_result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_init", __LINE__, 
                     signal_result, "Failed to setup signal handlers");
        ws_server_deinit();
        return signal_result;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Initialized successfully");
    g_ws_server.status.overall_status = HAL_DEVICE_STATUS_OK;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start WebSocket Server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_start(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Starting on port %d", g_ws_server.config.port);
    
    if (!g_ws_server.initialized) {
        hal_log_error("WS_SERVER", "ws_server_start", __LINE__, 
                     HAL_STATUS_NOT_INITIALIZED, "WebSocket Server not initialized");
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_ws_server.running) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "WebSocket Server: Already running");
        return HAL_STATUS_ALREADY_ACTIVE;
    }
    
    // Create socket
    g_ws_server.server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_ws_server.server_socket < 0) {
        hal_log_error("WS_SERVER", "ws_server_start", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to create server socket: %s", strerror(errno));
        return HAL_STATUS_IO_ERROR;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(g_ws_server.server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        hal_log_error("WS_SERVER", "ws_server_start", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to set socket options: %s", strerror(errno));
        close(g_ws_server.server_socket);
        g_ws_server.server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Set accept timeout to prevent blocking (Issue #113 Fix)
    struct timeval accept_timeout;
    accept_timeout.tv_sec = 1;   // 1 second timeout for accept
    accept_timeout.tv_usec = 0;
    
    if (setsockopt(g_ws_server.server_socket, SOL_SOCKET, SO_RCVTIMEO, &accept_timeout, sizeof(accept_timeout)) < 0) {
        hal_log_error("WS_SERVER", "ws_server_start", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to set accept timeout: %s", strerror(errno));
        // Continue anyway - this is not critical
    }
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(g_ws_server.config.port);
    
    if (bind(g_ws_server.server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        hal_log_error("WS_SERVER", "ws_server_start", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to bind server socket: %s", strerror(errno));
        close(g_ws_server.server_socket);
        g_ws_server.server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Listen for connections
    if (listen(g_ws_server.server_socket, g_ws_server.config.max_clients) < 0) {
        hal_log_error("WS_SERVER", "ws_server_start", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to listen on server socket: %s", strerror(errno));
        close(g_ws_server.server_socket);
        g_ws_server.server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Start server thread
    if (pthread_create(&g_ws_server.server_thread, NULL, ws_server_thread, NULL) != 0) {
        hal_log_error("WS_SERVER", "ws_server_start", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to create server thread: %s", strerror(errno));
        close(g_ws_server.server_socket);
        g_ws_server.server_socket = -1;
        return HAL_STATUS_ERROR;
    }
    
    // Start telemetry streaming thread
    if (pthread_create(&g_ws_server.telemetry_thread, NULL, ws_server_telemetry_thread, NULL) != 0) {
        hal_log_error("WS_SERVER", "ws_server_start", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to create telemetry thread: %s", strerror(errno));
        // Clean up server thread
        g_ws_server.running = false;
        pthread_join(g_ws_server.server_thread, NULL);
        close(g_ws_server.server_socket);
        g_ws_server.server_socket = -1;
        return HAL_STATUS_ERROR;
    }
    
    g_ws_server.running = true;
    g_ws_server.status.running = true;
    g_ws_server.status.listening = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Started successfully on port %d", g_ws_server.config.port);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Stop WebSocket Server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_stop(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Stopping...");
    
    if (!g_ws_server.running) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "WebSocket Server: Not running");
        return HAL_STATUS_OK;
    }
    
    g_ws_server.running = false;
    g_ws_server.status.running = false;
    g_ws_server.status.listening = false;
    
    // Close all client connections
    pthread_mutex_lock(&g_ws_server.mutex);
    for (uint32_t i = 0; i < g_ws_server.client_count; i++) {
        if (g_ws_server.clients[i].connected) {
            ws_server_send_close(g_ws_server.clients[i].socket_fd, WS_CLOSE_GOING_AWAY, "Server shutting down");
            close(g_ws_server.clients[i].socket_fd);
            g_ws_server.clients[i].connected = false;
        }
    }
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    // Close server socket
    if (g_ws_server.server_socket >= 0) {
        close(g_ws_server.server_socket);
        g_ws_server.server_socket = -1;
    }
    
    // Wait for telemetry thread to finish
    if (g_ws_server.telemetry_thread) {
        pthread_join(g_ws_server.telemetry_thread, NULL);
        g_ws_server.telemetry_thread = 0;
    }
    
    // Wait for server thread to finish
    if (g_ws_server.server_thread) {
        pthread_join(g_ws_server.server_thread, NULL);
        g_ws_server.server_thread = 0;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Stopped successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize WebSocket Server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Deinitializing...");
    
    // Stop server if running
    if (g_ws_server.running) {
        ws_server_stop();
    }
    
    // Cleanup resources
    hal_status_t cleanup_result = ws_server_cleanup_resources();
    if (cleanup_result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_deinit", __LINE__, 
                     cleanup_result, "Failed to cleanup resources");
    }
    
    // Destroy mutex
    if (g_ws_server.initialized) {
        pthread_mutex_destroy(&g_ws_server.mutex);
    }
    
    // Reset instance
    memset(&g_ws_server, 0, sizeof(ws_server_instance_t));
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Deinitialized successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get WebSocket Server status
 * @param status Pointer to status structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_get_status(ws_server_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    
    // Update statistics
    g_ws_server.status.statistics.uptime_ms = hal_get_timestamp_ms();
    g_ws_server.status.statistics.active_connections = g_ws_server.client_count;
    
    // Copy status
    memcpy(status, &g_ws_server.status, sizeof(ws_server_status_t));
    
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Register message handler
 * @param handler Message handler function
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_register_message_handler(ws_message_handler_t handler) {
    if (handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_ws_server.message_handler = handler;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Message handler registered");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Add WebSocket client
 * @param socket_fd Client socket file descriptor
 * @param client_ip Client IP address
 * @param client_port Client port
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_add_client(int socket_fd, const char *client_ip, uint16_t client_port) {
    if (socket_fd < 0 || client_ip == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    
    // Check if we have space for more clients
    if (g_ws_server.client_count >= g_ws_server.config.max_clients) {
        pthread_mutex_unlock(&g_ws_server.mutex);
        hal_log_error("WS_SERVER", "ws_server_add_client", __LINE__, 
                     HAL_STATUS_NO_MEMORY, "Maximum number of clients reached");
        return HAL_STATUS_NO_MEMORY;
    }
    
    // Find empty slot
    uint32_t client_index = g_ws_server.client_count;
    
    // Initialize client
    memset(&g_ws_server.clients[client_index], 0, sizeof(ws_client_t));
    g_ws_server.clients[client_index].socket_fd = socket_fd;
    g_ws_server.clients[client_index].connected = true;
    g_ws_server.clients[client_index].handshake_complete = false;
    gettimeofday(&g_ws_server.clients[client_index].last_activity, NULL);
    gettimeofday(&g_ws_server.clients[client_index].last_ping, NULL);
    
    strncpy(g_ws_server.clients[client_index].client_ip, client_ip, 15);
    g_ws_server.clients[client_index].client_ip[15] = '\0';
    g_ws_server.clients[client_index].client_port = client_port;
    
    snprintf(g_ws_server.clients[client_index].client_id, sizeof(g_ws_server.clients[client_index].client_id),
             "%s:%d", client_ip, client_port);
    
    // Initialize client mutex
    if (pthread_mutex_init(&g_ws_server.clients[client_index].mutex, NULL) != 0) {
        pthread_mutex_unlock(&g_ws_server.mutex);
        hal_log_error("WS_SERVER", "ws_server_add_client", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to initialize client mutex");
        return HAL_STATUS_ERROR;
    }
    
    g_ws_server.client_count++;
    
    // Update statistics
    g_ws_server.status.statistics.total_connections++;
    g_ws_server.status.statistics.active_connections = g_ws_server.client_count;
    
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Client added %s", 
                   g_ws_server.clients[client_index].client_id);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Remove WebSocket client
 * @param socket_fd Client socket file descriptor
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_remove_client(int socket_fd) {
    if (socket_fd < 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    
    // Find client
    for (uint32_t i = 0; i < g_ws_server.client_count; i++) {
        if (g_ws_server.clients[i].socket_fd == socket_fd) {
            // Close socket
            close(socket_fd);
            
            // Destroy client mutex
            pthread_mutex_destroy(&g_ws_server.clients[i].mutex);
            
            // Remove client by shifting array
            for (uint32_t j = i; j < g_ws_server.client_count - 1; j++) {
                g_ws_server.clients[j] = g_ws_server.clients[j + 1];
            }
            
            g_ws_server.client_count--;
            
            // Update statistics
            g_ws_server.status.statistics.active_connections = g_ws_server.client_count;
            
            pthread_mutex_unlock(&g_ws_server.mutex);
            
            hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Client removed %d", socket_fd);
            
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Send WebSocket message
 * @param socket_fd Client socket file descriptor
 * @param message Message to send
 * @param message_length Message length
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_send_message(int socket_fd, const char *message, size_t message_length) {
    if (socket_fd < 0 || message == NULL || message_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Find client
    ws_client_t *client;
    hal_status_t find_result = ws_server_find_client(socket_fd, &client);
    if (find_result != HAL_STATUS_OK) {
        return find_result;
    }
    
    // Create text frame
    ws_frame_t frame;
    hal_status_t frame_result = ws_server_create_frame(WS_FRAME_TEXT, (const uint8_t*)message, 
                                                      message_length, false, &frame);
    if (frame_result != HAL_STATUS_OK) {
        return frame_result;
    }
    
    // Send frame
    hal_status_t send_result = ws_server_send_frame(socket_fd, &frame);
    
    // Update client statistics
    if (send_result == HAL_STATUS_OK) {
        pthread_mutex_lock(&client->mutex);
        client->messages_sent++;
        client->bytes_sent += message_length;
        gettimeofday(&client->last_activity, NULL);
        pthread_mutex_unlock(&client->mutex);
        
        // Update server statistics
        pthread_mutex_lock(&g_ws_server.mutex);
        g_ws_server.status.statistics.total_messages_sent++;
        g_ws_server.status.statistics.total_bytes_sent += message_length;
        pthread_mutex_unlock(&g_ws_server.mutex);
    }
    
    return send_result;
}

/**
 * @brief Send WebSocket text message
 * @param socket_fd Client socket file descriptor
 * @param text Text to send
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_send_text(int socket_fd, const char *text) {
    if (text == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return ws_server_send_message(socket_fd, text, strlen(text));
}

/**
 * @brief Broadcast WebSocket message to all connected clients
 * @param message Message to broadcast
 * @param message_length Message length
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_broadcast_message(const char *message, size_t message_length) {
    if (message == NULL || message_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    
    uint32_t sent_count = 0;
    for (uint32_t i = 0; i < g_ws_server.client_count; i++) {
        if (g_ws_server.clients[i].connected && g_ws_server.clients[i].handshake_complete) {
            hal_status_t send_result = ws_server_send_message(g_ws_server.clients[i].socket_fd, message, message_length);
            if (send_result == HAL_STATUS_OK) {
                sent_count++;
            }
        }
    }
    
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Broadcast message to %d clients", sent_count);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Broadcast WebSocket text message to all connected clients
 * @param text Text to broadcast
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_broadcast_text(const char *text) {
    if (text == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return ws_server_broadcast_message(text, strlen(text));
}

/**
 * @brief Send WebSocket binary message
 * @param socket_fd Client socket file descriptor
 * @param data Binary data to send
 * @param data_length Data length
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_send_binary(int socket_fd, const uint8_t *data, size_t data_length) {
    if (socket_fd < 0 || data == NULL || data_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Find client
    ws_client_t *client;
    hal_status_t find_result = ws_server_find_client(socket_fd, &client);
    if (find_result != HAL_STATUS_OK) {
        return find_result;
    }
    
    // Create binary frame
    ws_frame_t frame;
    hal_status_t frame_result = ws_server_create_frame(WS_FRAME_BINARY, data, 
                                                      data_length, false, &frame);
    if (frame_result != HAL_STATUS_OK) {
        return frame_result;
    }
    
    // Send frame
    hal_status_t send_result = ws_server_send_frame(socket_fd, &frame);
    
    // Update client statistics
    if (send_result == HAL_STATUS_OK) {
        pthread_mutex_lock(&client->mutex);
        client->messages_sent++;
        client->bytes_sent += data_length;
        gettimeofday(&client->last_activity, NULL);
        pthread_mutex_unlock(&client->mutex);
        
        // Update server statistics
        pthread_mutex_lock(&g_ws_server.mutex);
        g_ws_server.status.statistics.total_messages_sent++;
        g_ws_server.status.statistics.total_bytes_sent += data_length;
        pthread_mutex_unlock(&g_ws_server.mutex);
    }
    
    return send_result;
}

/**
 * @brief Broadcast WebSocket binary message to all connected clients
 * @param data Binary data to broadcast
 * @param data_length Data length
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_broadcast_binary(const uint8_t *data, size_t data_length) {
    if (data == NULL || data_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    
    uint32_t sent_count = 0;
    for (uint32_t i = 0; i < g_ws_server.client_count; i++) {
        if (g_ws_server.clients[i].connected && g_ws_server.clients[i].handshake_complete) {
            hal_status_t send_result = ws_server_send_binary(g_ws_server.clients[i].socket_fd, data, data_length);
            if (send_result == HAL_STATUS_OK) {
                sent_count++;
            }
        }
    }
    
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Broadcast binary message to %d clients", sent_count);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Send WebSocket frame
 * @param socket_fd Client socket file descriptor
 * @param frame Frame to send
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_send_frame(int socket_fd, const ws_frame_t *frame) {
    if (socket_fd < 0 || frame == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Serialize frame
    uint8_t frame_buffer[WS_SERVER_MAX_FRAME_SIZE];
    size_t serialized_length;
    
    hal_status_t serialize_result = ws_server_serialize_frame(frame, frame_buffer, 
                                                             sizeof(frame_buffer), &serialized_length);
    if (serialize_result != HAL_STATUS_OK) {
        return serialize_result;
    }
    
    // Send frame data
    hal_status_t send_result = ws_server_write_data(socket_fd, frame_buffer, serialized_length);
    
    return send_result;
}

/**
 * @brief Send WebSocket ping frame
 * @param socket_fd Client socket file descriptor
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_send_ping(int socket_fd) {
    if (socket_fd < 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create ping frame
    ws_frame_t frame;
    hal_status_t frame_result = ws_server_create_frame(WS_FRAME_PING, NULL, 0, false, &frame);
    if (frame_result != HAL_STATUS_OK) {
        return frame_result;
    }
    
    // Send frame
    hal_status_t send_result = ws_server_send_frame(socket_fd, &frame);
    
    // Update statistics
    if (send_result == HAL_STATUS_OK) {
        pthread_mutex_lock(&g_ws_server.mutex);
        g_ws_server.status.statistics.ping_frames_sent++;
        pthread_mutex_unlock(&g_ws_server.mutex);
    }
    
    return send_result;
}

/**
 * @brief Send WebSocket pong frame
 * @param socket_fd Client socket file descriptor
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_send_pong(int socket_fd) {
    if (socket_fd < 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create pong frame
    ws_frame_t frame;
    hal_status_t frame_result = ws_server_create_frame(WS_FRAME_PONG, NULL, 0, false, &frame);
    if (frame_result != HAL_STATUS_OK) {
        return frame_result;
    }
    
    // Send frame
    return ws_server_send_frame(socket_fd, &frame);
}

/**
 * @brief Send WebSocket close frame
 * @param socket_fd Client socket file descriptor
 * @param code Close code
 * @param reason Close reason
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_send_close(int socket_fd, ws_close_code_t code, const char *reason) {
    if (socket_fd < 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Prepare close payload
    size_t reason_length = reason ? strlen(reason) : 0;
    size_t payload_length = 2 + reason_length; // 2 bytes for code + reason
    
    uint8_t *payload = malloc(payload_length);
    if (payload == NULL) {
        return HAL_STATUS_NO_MEMORY;
    }
    
    // Add close code (big-endian)
    payload[0] = (code >> 8) & 0xFF;
    payload[1] = code & 0xFF;
    
    // Add reason if provided
    if (reason && reason_length > 0) {
        memcpy(payload + 2, reason, reason_length);
    }
    
    // Create close frame
    ws_frame_t frame;
    hal_status_t frame_result = ws_server_create_frame(WS_FRAME_CLOSE, payload, payload_length, false, &frame);
    
    free(payload);
    
    if (frame_result != HAL_STATUS_OK) {
        return frame_result;
    }
    
    // Send frame
    hal_status_t send_result = ws_server_send_frame(socket_fd, &frame);
    
    // Update statistics
    if (send_result == HAL_STATUS_OK) {
        pthread_mutex_lock(&g_ws_server.mutex);
        g_ws_server.status.statistics.close_frames_sent++;
        pthread_mutex_unlock(&g_ws_server.mutex);
    }
    
    return send_result;
}

/**
 * @brief Get WebSocket Server version
 * @param major Major version number
 * @param minor Minor version number
 * @param patch Patch version number
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch) {
    if (major == NULL || minor == NULL || patch == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *major = WS_SERVER_VERSION_MAJOR;
    *minor = WS_SERVER_VERSION_MINOR;
    *patch = WS_SERVER_VERSION_PATCH;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get WebSocket Server version string
 * @return const char* Version string
 */
const char* ws_server_get_version_string(void) {
    return WS_SERVER_VERSION_STRING;
}

/**
 * @brief Handle HTTP request on WebSocket port (Issue #113 Fix)
 * @param socket_fd Client socket file descriptor
 * @param request HTTP request string
 * @param request_length Length of request
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_handle_http_request(int socket_fd, const char *request, size_t request_length) {
    (void)request_length; // Unused parameter
    
    // Parse the HTTP request to extract path
    char method[16] = {0};
    char path[256] = {0};
    char version[16] = {0};
    
    if (sscanf(request, "%15s %255s %15s", method, path, version) != 3) {
        // Invalid HTTP request format
        const char *bad_request_response = 
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"error\":true,\"message\":\"Invalid HTTP request format\"}";
        return ws_server_send_http_response(socket_fd, bad_request_response);
    }
    
    // Handle specific endpoints that Backend expects on port 8081
    if (strcmp(path, "/health") == 0) {
        const char *health_response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"success\":true,\"status\":\"healthy\",\"service\":\"websocket\",\"port\":8081}";
        return ws_server_send_http_response(socket_fd, health_response);
    }
    
    if (strcmp(path, "/api/v1/status") == 0) {
        const char *status_response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"success\":true,\"data\":{\"service\":\"websocket\",\"port\":8081,\"clients_connected\":%u}}";
        
        char response_buffer[512];
        snprintf(response_buffer, sizeof(response_buffer), status_response, g_ws_server.client_count);
        return ws_server_send_http_response(socket_fd, response_buffer);
    }
    
    if (strcmp(path, "/api/v1/robot/status") == 0) {
        const char *robot_status_response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"success\":true,\"data\":{\"robot_id\":\"OHT-50-001\",\"status\":\"idle\",\"websocket_service\":true,\"port\":8081}}";
        return ws_server_send_http_response(socket_fd, robot_status_response);
    }
    
    // For any other path, redirect to port 8080 (HTTP API server)
    const char *redirect_response = 
        "HTTP/1.1 302 Found\r\n"
        "Location: http://localhost:8080%s\r\n"
        "Content-Type: application/json\r\n"
        "Connection: close\r\n"
        "\r\n"
        "{\"error\":false,\"message\":\"Redirecting to HTTP API server on port 8080\",\"redirect_url\":\"http://localhost:8080%s\"}";
    
    char redirect_buffer[512];
    snprintf(redirect_buffer, sizeof(redirect_buffer), redirect_response, path, path);
    return ws_server_send_http_response(socket_fd, redirect_buffer);
}

/**
 * @brief Broadcast RS485 module telemetry (Issue #90)
 * @param module_addr Module address (0x02, 0x03, 0x04, 0x05)
 * @return HAL status
 */
hal_status_t ws_server_broadcast_rs485_telemetry(uint8_t module_addr) {
    if (!g_ws_server.initialized || !g_ws_server.running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Include telemetry manager header
    #include "managers/telemetry_manager.h"
    
    // Create JSON telemetry data for this module
    char telemetry_buffer[4096];
    int json_length = telemetry_manager_serialize_rs485_telemetry(
        module_addr, telemetry_buffer, sizeof(telemetry_buffer));
    
    if (json_length <= 0) {
        hal_log_error("WS_SERVER", "ws_server_broadcast_rs485_telemetry", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to serialize RS485 telemetry");
        return HAL_STATUS_ERROR;
    }
    
    // Broadcast to all connected clients
    hal_status_t result = ws_server_broadcast_telemetry(telemetry_buffer, json_length);
    
    if (result == HAL_STATUS_OK) {
        hal_log_message(HAL_LOG_LEVEL_DEBUG, 
                       "WebSocket Server: Broadcasted RS485 telemetry for module 0x%02X (%d bytes)",
                       module_addr, json_length);
    }
    
    return result;
}

/**
 * @brief Start RS485 telemetry streaming for all modules (Issue #90)
 * @param interval_ms Streaming interval in milliseconds (1000-2000ms recommended)
 * @return HAL status
 */
hal_status_t ws_server_start_rs485_telemetry_streaming(uint32_t interval_ms) {
    if (!g_ws_server.initialized || !g_ws_server.running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (interval_ms < 500 || interval_ms > 10000) {
        hal_log_error("WS_SERVER", "ws_server_start_rs485_telemetry_streaming", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid interval (must be 500-10000ms)");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Set telemetry streaming parameters
    g_ws_server.telemetry_streaming = true;
    g_ws_server.telemetry_interval_ms = interval_ms;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, 
                   "WebSocket Server: Started RS485 telemetry streaming (interval: %ums)", 
                   interval_ms);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set WebSocket Server configuration
 * @param config Pointer to configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_set_config(const ws_server_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_ws_server.running) {
        hal_log_error("WS_SERVER", "ws_server_set_config", __LINE__, 
                     HAL_STATUS_INVALID_STATE, "Cannot change config while server is running");
        return HAL_STATUS_INVALID_STATE;
    }
    
    // Initialize defaults and validate
    ws_server_config_t config_copy = *config;
    hal_status_t defaults_result = ws_server_initialize_defaults(&config_copy);
    if (defaults_result != HAL_STATUS_OK) {
        return defaults_result;
    }
    
    hal_status_t validation_result = ws_server_validate_config(&config_copy);
    if (validation_result != HAL_STATUS_OK) {
        return validation_result;
    }
    
    // Update configuration
    pthread_mutex_lock(&g_ws_server.mutex);
    memcpy(&g_ws_server.config, &config_copy, sizeof(ws_server_config_t));
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Configuration updated");
    return HAL_STATUS_OK;
}

/**
 * @brief Get WebSocket Server configuration
 * @param config Pointer to configuration structure to fill
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_get_config(ws_server_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    memcpy(config, &g_ws_server.config, sizeof(ws_server_config_t));
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set default WebSocket Server configuration
 * @param config Pointer to configuration structure to fill with defaults
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_set_default_config(ws_server_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize with zeros
    memset(config, 0, sizeof(ws_server_config_t));
    
    // Set default values
    return ws_server_initialize_defaults(config);
}

/**
 * @brief Unregister message handler
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_unregister_message_handler(void) {
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_ws_server.message_handler = NULL;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Message handler unregistered");
    return HAL_STATUS_OK;
}

/**
 * @brief Get all connected clients
 * @param clients Pointer to client array to fill
 * @param count Pointer to count variable (input: max count, output: actual count)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_get_clients(ws_client_t *clients, uint32_t *count) {
    if (clients == NULL || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    
    uint32_t max_count = *count;
    uint32_t actual_count = g_ws_server.client_count;
    
    if (actual_count > max_count) {
        actual_count = max_count;
    }
    
    for (uint32_t i = 0; i < actual_count; i++) {
        clients[i] = g_ws_server.clients[i];
    }
    
    *count = actual_count;
    
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get specific client by socket file descriptor
 * @param socket_fd Client socket file descriptor
 * @param client Pointer to client structure pointer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_get_client(int socket_fd, ws_client_t **client) {
    if (socket_fd < 0 || client == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    return ws_server_find_client(socket_fd, client);
}

/**
 * @brief Parse HTTP headers to extract WebSocket key
 * @param request HTTP request string
 * @param websocket_key Buffer to store WebSocket key
 * @param key_size Size of the key buffer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
static hal_status_t ws_server_parse_http_headers(const char *request, char *websocket_key, size_t key_size) {
    if (request == NULL || websocket_key == NULL || key_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Extract Sec-WebSocket-Key (case-insensitive)
    const char *needle = "sec-websocket-key:";
    const char *kpos = NULL;
    size_t nlen = strlen(needle);
    
    // Search for the key header
    const char *p = request;
    while (*p) {
        if (strncasecmp(p, needle, nlen) == 0) {
            kpos = p;
            break;
        }
        p++;
    }
    
    if (!kpos) {
        hal_log_error("WS_SERVER", "ws_server_parse_http_headers", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Sec-WebSocket-Key header not found");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Skip past the header name
    kpos += nlen;
    
    // Skip whitespace
    while (*kpos == ' ' || *kpos == '\t') {
        kpos++;
    }
    
    // Extract the key value
    size_t i = 0;
    while (i < key_size - 1 && *kpos && *kpos != '\r' && *kpos != '\n') {
        websocket_key[i++] = *kpos++;
    }
    websocket_key[i] = '\0';
    
    if (i == 0) {
        hal_log_error("WS_SERVER", "ws_server_parse_http_headers", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Empty WebSocket key");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Extracted key: %s", websocket_key);
    return HAL_STATUS_OK;
}

/**
 * @brief Calculate WebSocket accept key from client key
 * @param websocket_key Client WebSocket key
 * @param accept_key Buffer to store accept key
 * @param accept_key_size Size of the accept key buffer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
static hal_status_t ws_server_calculate_accept_key(const char *websocket_key, char *accept_key, size_t accept_key_size) {
    if (websocket_key == NULL || accept_key == NULL || accept_key_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // WebSocket GUID
    static const char *GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    
    // Combine key with GUID
    char combined[256];
    int combined_len = snprintf(combined, sizeof(combined), "%s%s", websocket_key, GUID);
    if (combined_len < 0 || (size_t)combined_len >= sizeof(combined)) {
        hal_log_error("WS_SERVER", "ws_server_calculate_accept_key", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to combine key with GUID");
        return HAL_STATUS_ERROR;
    }
    
    // Compute SHA1 hash
    uint8_t sha[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)combined, strlen(combined), sha);
    
    // Base64 encode
    int outlen = EVP_EncodeBlock((unsigned char*)accept_key, sha, SHA_DIGEST_LENGTH);
    if (outlen <= 0 || (size_t)outlen >= accept_key_size) {
        hal_log_error("WS_SERVER", "ws_server_calculate_accept_key", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to base64 encode accept key");
        return HAL_STATUS_ERROR;
    }
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Calculated accept key: %s", accept_key);
    return HAL_STATUS_OK;
}

/**
 * @brief Handle WebSocket ping frame
 * @param client Client that sent the ping
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_handle_ping(ws_client_t *client) {
    if (client == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Send pong response
    hal_status_t result = ws_server_send_pong(client->socket_fd);
    if (result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_handle_ping", __LINE__, 
                     result, "Failed to send pong response");
        return result;
    }
    
    // Update client statistics
    pthread_mutex_lock(&client->mutex);
    gettimeofday(&client->last_activity, NULL);
    client->messages_received++;
    pthread_mutex_unlock(&client->mutex);
    
    // Update server statistics
    pthread_mutex_lock(&g_ws_server.mutex);
    g_ws_server.status.statistics.pong_frames_received++;
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Handled ping from client %s", client->client_id);
    return HAL_STATUS_OK;
}

/**
 * @brief Handle WebSocket pong frame
 * @param client Client that sent the pong
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_handle_pong(ws_client_t *client) {
    if (client == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Update client activity
    pthread_mutex_lock(&client->mutex);
    gettimeofday(&client->last_activity, NULL);
    gettimeofday(&client->last_ping, NULL);
    client->messages_received++;
    pthread_mutex_unlock(&client->mutex);
    
    // Update server statistics
    pthread_mutex_lock(&g_ws_server.mutex);
    g_ws_server.status.statistics.pong_frames_received++;
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Handled pong from client %s", client->client_id);
    return HAL_STATUS_OK;
}

/**
 * @brief Handle WebSocket close frame
 * @param client Client that sent the close
 * @param code Close code
 * @param reason Close reason
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_handle_close(ws_client_t *client, ws_close_code_t code, const char *reason) {
    if (client == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Client %s closing connection (code: %d, reason: %s)", 
                   client->client_id, code, reason ? reason : "none");
    
    // Send close response if not already sent
    if (client->connected) {
        hal_status_t result = ws_server_send_close(client->socket_fd, code, reason);
        if (result != HAL_STATUS_OK) {
            hal_log_error("WS_SERVER", "ws_server_handle_close", __LINE__, 
                         result, "Failed to send close response");
        }
    }
    
    // Update server statistics
    pthread_mutex_lock(&g_ws_server.mutex);
    g_ws_server.status.statistics.close_frames_received++;
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    return HAL_STATUS_OK;
}

// Private function implementations
static hal_status_t ws_server_validate_config(const ws_server_config_t *config) {
    // Validate port
    if (config->port == 0) {
        hal_log_error("WS_SERVER", "ws_server_validate_config", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid port: %d (must be 1-65535)", config->port);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate max_clients
    if (config->max_clients == 0 || config->max_clients > WS_SERVER_MAX_CLIENTS) {
        hal_log_error("WS_SERVER", "ws_server_validate_config", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid max_clients: %d (must be 1-%d)", 
                     config->max_clients, WS_SERVER_MAX_CLIENTS);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate message sizes
    if (config->max_message_size == 0 || config->max_message_size > WS_SERVER_MAX_MESSAGE_SIZE) {
        hal_log_error("WS_SERVER", "ws_server_validate_config", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid max_message_size: %d (must be 1-%d)", 
                     config->max_message_size, WS_SERVER_MAX_MESSAGE_SIZE);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_frame_size == 0 || config->max_frame_size > WS_SERVER_MAX_FRAME_SIZE) {
        hal_log_error("WS_SERVER", "ws_server_validate_config", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid max_frame_size: %d (must be 1-%d)", 
                     config->max_frame_size, WS_SERVER_MAX_FRAME_SIZE);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate timeouts
    if (config->timeout_ms == 0 || config->timeout_ms > 300000) { // Max 5 minutes
        hal_log_error("WS_SERVER", "ws_server_validate_config", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid timeout_ms: %d (must be 1-300000)", config->timeout_ms);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->ping_interval_ms == 0 || config->ping_interval_ms > 300000) {
        hal_log_error("WS_SERVER", "ws_server_validate_config", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid ping_interval_ms: %d (must be 1-300000)", config->ping_interval_ms);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->pong_timeout_ms == 0 || config->pong_timeout_ms > 60000) { // Max 1 minute
        hal_log_error("WS_SERVER", "ws_server_validate_config", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid pong_timeout_ms: %d (must be 1-60000)", config->pong_timeout_ms);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate server name
    if (config->server_name[0] == '\0') {
        hal_log_error("WS_SERVER", "ws_server_validate_config", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Server name cannot be empty");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Configuration validation passed");
    return HAL_STATUS_OK;
}

static hal_status_t ws_server_initialize_defaults(ws_server_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Set default values for any uninitialized fields
    if (config->port == 0) {
        config->port = WS_SERVER_DEFAULT_PORT;
    }
    
    if (config->max_clients == 0) {
        config->max_clients = WS_SERVER_MAX_CLIENTS;
    }
    
    if (config->timeout_ms == 0) {
        config->timeout_ms = WS_SERVER_DEFAULT_TIMEOUT_MS;
    }
    
    if (config->max_message_size == 0) {
        config->max_message_size = WS_SERVER_MAX_MESSAGE_SIZE;
    }
    
    if (config->max_frame_size == 0) {
        config->max_frame_size = WS_SERVER_MAX_FRAME_SIZE;
    }
    
    if (config->ping_interval_ms == 0) {
        config->ping_interval_ms = WS_SERVER_PING_INTERVAL_MS;
    }
    
    if (config->pong_timeout_ms == 0) {
        config->pong_timeout_ms = WS_SERVER_PONG_TIMEOUT_MS;
    }
    
    if (config->server_name[0] == '\0') {
        strncpy(config->server_name, "OHT-50-WebSocket", sizeof(config->server_name) - 1);
        config->server_name[sizeof(config->server_name) - 1] = '\0';
    }
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Default values initialized");
    return HAL_STATUS_OK;
}

static hal_status_t ws_server_setup_signal_handlers(void) {
    signal(SIGINT, ws_server_signal_handler);
    signal(SIGTERM, ws_server_signal_handler);
    return HAL_STATUS_OK;
}

static hal_status_t ws_server_cleanup_resources(void) {
    // Close server socket
    if (g_ws_server.server_socket >= 0) {
        close(g_ws_server.server_socket);
        g_ws_server.server_socket = -1;
    }
    
    // Close all client connections
    for (uint32_t i = 0; i < g_ws_server.client_count; i++) {
        if (g_ws_server.clients[i].socket_fd >= 0) {
            close(g_ws_server.clients[i].socket_fd);
            g_ws_server.clients[i].socket_fd = -1;
        }
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t ws_server_find_client(int socket_fd, ws_client_t **client) {
    for (uint32_t i = 0; i < g_ws_server.client_count; i++) {
        if (g_ws_server.clients[i].socket_fd == socket_fd) {
            *client = &g_ws_server.clients[i];
            return HAL_STATUS_OK;
        }
    }
    return HAL_STATUS_NOT_FOUND;
}

// WebSocket Server Thread Function
void* ws_server_thread(void *arg) {
    (void)arg; // Unused parameter
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Server thread started");
    
    while (!g_ws_shutdown_requested && g_ws_server.running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(g_ws_server.server_socket, 
                                  (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by signal
            }
            hal_log_error("WS_SERVER", "ws_server_thread", __LINE__, 
                         HAL_STATUS_IO_ERROR, "Failed to accept client connection: %s", strerror(errno));
            continue;
        }
        
        // Get client IP and port
        char client_ip[16];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        uint16_t client_port = ntohs(client_addr.sin_port);
        
        // Add client
        hal_status_t add_result = ws_server_add_client(client_socket, client_ip, client_port);
        if (add_result != HAL_STATUS_OK) {
            hal_log_error("WS_SERVER", "ws_server_thread", __LINE__, 
                         add_result, "Failed to add client");
            close(client_socket);
            continue;
        }
        
        // Handle client in a separate thread
        pthread_t client_thread;
        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;
        
        if (pthread_create(&client_thread, NULL, ws_server_client_thread, client_socket_ptr) != 0) {
            hal_log_error("WS_SERVER", "ws_server_thread", __LINE__, 
                         HAL_STATUS_ERROR, "Failed to create client thread: %s", strerror(errno));
            ws_server_remove_client(client_socket);
            free(client_socket_ptr);
        } else {
            pthread_detach(client_thread);
        }
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Server thread stopped");
    return NULL;
}

// WebSocket Client Thread Function
void* ws_server_client_thread(void *arg) {
    int client_socket = *(int*)arg;
    free(arg);
    
    char request_buffer[4096];
    size_t received_length;
    
    // Read initial request (could be HTTP or WebSocket handshake)
    hal_status_t read_result = ws_server_read_data(client_socket, (uint8_t*)request_buffer, 
                                                  sizeof(request_buffer), &received_length);
    if (read_result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_client_thread", __LINE__, 
                     read_result, "Failed to read request");
        ws_server_remove_client(client_socket);
        return NULL;
    }
    
    // NULL-terminate the request buffer for string operations
    if (received_length < sizeof(request_buffer)) {
        request_buffer[received_length] = '\0';
    } else {
        request_buffer[sizeof(request_buffer) - 1] = '\0';
    }
    
    // Check if this is an HTTP request (not WebSocket handshake)
    // Look for HTTP methods and check if it's NOT a WebSocket upgrade
    bool is_http_request = false;
    if (strncmp(request_buffer, "GET ", 4) == 0 || 
        strncmp(request_buffer, "POST ", 5) == 0 ||
        strncmp(request_buffer, "PUT ", 4) == 0 ||
        strncmp(request_buffer, "DELETE ", 7) == 0) {
        
        // Check if it's NOT a WebSocket upgrade request
        if (strstr(request_buffer, "Upgrade:") == NULL || 
            strstr(request_buffer, "websocket") == NULL ||
            strstr(request_buffer, "Sec-WebSocket-Key:") == NULL) {
            is_http_request = true;
        }
    }
    
    if (is_http_request) {
        // This is a regular HTTP request, handle it
        hal_status_t http_result = ws_server_handle_http_request(client_socket, request_buffer, received_length);
        if (http_result != HAL_STATUS_OK) {
            hal_log_error("WS_SERVER", "ws_server_client_thread", __LINE__, 
                         http_result, "Failed to handle HTTP request");
        }
        ws_server_remove_client(client_socket);
        return NULL;
    }
    
    // Handle WebSocket handshake
    hal_status_t handshake_result = ws_server_handle_handshake(client_socket, request_buffer, received_length);
    if (handshake_result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_client_thread", __LINE__, 
                     handshake_result, "Failed to handle WebSocket handshake");
        ws_server_remove_client(client_socket);
        return NULL;
    }
    
    // Mark handshake as complete
    ws_client_t *client = NULL;
    if (ws_server_find_client(client_socket, &client) == HAL_STATUS_OK) {
        client->handshake_complete = true;
        // Send a ready message immediately after handshake
        ws_server_send_text(client_socket, "{\"type\":\"ready\"}");
    }
    
    // Main message loop
    while (g_ws_server.running) {
        ws_frame_t frame;
        hal_status_t receive_result = ws_server_receive_frame(client_socket, &frame);
        
        if (receive_result != HAL_STATUS_OK) {
            hal_log_error("WS_SERVER", "ws_server_client_thread", __LINE__, 
                         receive_result, "Failed to receive frame");
            break;
        }
        
        // Handle frame based on type
        // Ensure we have a valid client pointer (may change after array shifts)
        if (client == NULL) {
            ws_server_find_client(client_socket, &client);
        }
        switch (frame.opcode) {
            case WS_FRAME_TEXT:
            case WS_FRAME_BINARY:
                // Handle message
                if (g_ws_server.message_handler != NULL) {
                    g_ws_server.message_handler((const char*)frame.payload, frame.payload_length, client);
                }
                break;
                
            case WS_FRAME_PING:
                ws_server_send_pong(client_socket);
                break;
                
            case WS_FRAME_PONG:
                // Update last pong time
                if (client != NULL) {
                    gettimeofday(&client->last_activity, NULL);
                }
                break;
                
            case WS_FRAME_CLOSE: {
                // Handle close
                ws_close_code_t close_code = WS_CLOSE_NORMAL;
                if (frame.payload_length >= 2) {
                    close_code = (frame.payload[0] << 8) | frame.payload[1];
                }
                ws_server_send_close(client_socket, close_code, "Goodbye");
                goto cleanup;
            }
                
            default:
                break;
        }
        
        // Free frame payload
        if (frame.payload != NULL) {
            free(frame.payload);
        }
    }
    
cleanup:
    ws_server_remove_client(client_socket);
    return NULL;
}

// Placeholder implementations (minimal)
hal_status_t ws_server_handle_handshake(int socket_fd, const char *request, size_t request_length) {
    (void)request_length;
    // Extract Sec-WebSocket-Key (case-insensitive)
    char key[128]={0};
    const char *kpos = NULL; {
        const char *p = request; const char *needle = "sec-websocket-key:"; size_t nlen=strlen(needle);
        while(*p){ if(strncasecmp(p, needle, nlen)==0){ kpos=p; break; } p++; }
    }
    if(!kpos){
        ws_server_send_http_response(socket_fd, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    kpos += strlen("Sec-WebSocket-Key:");
    while(*kpos==' '||*kpos=='\t') kpos++;
    size_t i=0; while(i<sizeof(key)-1 && *kpos && *kpos!='\r' && *kpos!='\n'){ key[i++]=*kpos++; }
    key[i]='\0';

    // Compute Sec-WebSocket-Accept = base64( SHA1(key + GUID) )
    static const char *GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", key, GUID);
    uint8_t sha[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)combined, strlen(combined), sha);

    // base64 encode
    char accept_b64[128];
    int outlen = EVP_EncodeBlock((unsigned char*)accept_b64, sha, SHA_DIGEST_LENGTH);
    if(outlen <= 0){
        ws_server_send_http_response(socket_fd, "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n");
        return HAL_STATUS_ERROR;
    }

    char response[512];
    int n = snprintf(response, sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n", accept_b64);
    if(n<0 || (size_t)n>=sizeof(response)) return HAL_STATUS_ERROR;
    return ws_server_send_http_response(socket_fd, response);
}

hal_status_t ws_server_receive_frame(int socket_fd, ws_frame_t *frame) {
    if(!frame) return HAL_STATUS_INVALID_PARAMETER;
    uint8_t hdr[2]; size_t rec=0; hal_status_t r = ws_server_read_data(socket_fd, hdr, 2, &rec); if(r!=HAL_STATUS_OK||rec<2) return HAL_STATUS_IO_ERROR;
    frame->fin = (hdr[0] & 0x80)!=0;
    frame->opcode = (ws_frame_type_t)(hdr[0] & 0x0F);
    bool masked = (hdr[1] & 0x80)!=0; frame->masked = masked;
    uint64_t plen = (hdr[1] & 0x7F);
    if(plen==126){ uint8_t ext[2]; size_t rr; if(ws_server_read_data(socket_fd, ext, 2, &rr)!=HAL_STATUS_OK||rr<2) return HAL_STATUS_IO_ERROR; plen = (ext[0]<<8)|ext[1]; }
    else if(plen==127){ uint8_t ext[8]; size_t rr; if(ws_server_read_data(socket_fd, ext, 8, &rr)!=HAL_STATUS_OK||rr<8) return HAL_STATUS_IO_ERROR; plen=0; for(int i=0;i<8;i++){ plen=(plen<<8)|ext[i]; } }
    uint32_t mask=0; if(masked){ uint8_t m[4]; size_t rr; if(ws_server_read_data(socket_fd, m, 4, &rr)!=HAL_STATUS_OK||rr<4) return HAL_STATUS_IO_ERROR; mask = (m[0]<<24)|(m[1]<<16)|(m[2]<<8)|m[3]; }
    if(plen>WS_SERVER_MAX_MESSAGE_SIZE) return HAL_STATUS_INVALID_PARAMETER;
    uint8_t *payload = NULL; if(plen>0){ payload = (uint8_t*)malloc(plen); if(!payload) return HAL_STATUS_NO_MEMORY; size_t rr; if(ws_server_read_data(socket_fd, payload, plen, &rr)!=HAL_STATUS_OK||rr<plen){ free(payload); return HAL_STATUS_IO_ERROR; } if(masked){ for(uint64_t i=0;i<plen;i++){ payload[i] ^= ((uint8_t*)&mask)[i%4]; } } }
    frame->payload = payload; frame->payload_length = plen; frame->payload_size = plen; frame->masking_key = mask; return HAL_STATUS_OK;
}

hal_status_t ws_server_create_frame(ws_frame_type_t opcode, const uint8_t *payload, size_t payload_length, 
                                   bool masked, ws_frame_t *frame) {
    if(!frame) return HAL_STATUS_INVALID_PARAMETER;
    (void)masked; // server-to-client must be unmasked
    frame->fin=true; frame->rsv1=frame->rsv2=frame->rsv3=false; frame->opcode=opcode; frame->masked=false; frame->payload_length=payload_length; frame->payload_size=payload_length; frame->payload=NULL; frame->masking_key=0;
    if(payload_length>0){ frame->payload=(uint8_t*)malloc(payload_length); if(!frame->payload) return HAL_STATUS_NO_MEMORY; memcpy(frame->payload,payload,payload_length); }
    return HAL_STATUS_OK;
}

hal_status_t ws_server_serialize_frame(const ws_frame_t *frame, uint8_t *buffer, size_t buffer_size, size_t *serialized_length) {
    if(!frame||!buffer||!serialized_length) return HAL_STATUS_INVALID_PARAMETER;
    size_t need = 2; uint64_t plen=frame->payload_length; if(plen>=126 && plen<=0xFFFF) need+=2; else if(plen>0xFFFF) need+=8; need+=plen; if(need>buffer_size) return HAL_STATUS_NO_MEMORY;
    size_t off=0; buffer[off++] = 0x80 | (frame->opcode & 0x0F); // FIN + opcode
    if(plen<126){ buffer[off++] = (uint8_t)plen; }
    else if(plen<=0xFFFF){ buffer[off++] = 126; buffer[off++] = (uint8_t)((plen>>8)&0xFF); buffer[off++] = (uint8_t)(plen&0xFF); }
    else { buffer[off++] = 127; for(int i=7;i>=0;i--){ buffer[off++] = (uint8_t)((plen>>(8*i))&0xFF); } }
    if(plen>0 && frame->payload){ memcpy(buffer+off, frame->payload, plen); off+=plen; }
    *serialized_length = off; return HAL_STATUS_OK;
}

hal_status_t ws_server_read_data(int socket_fd, uint8_t *buffer, size_t buffer_size, size_t *received_length) {
    if(socket_fd<0||!buffer||buffer_size==0||!received_length) return HAL_STATUS_INVALID_PARAMETER;
    
    // Set socket timeout to prevent blocking (Issue #113 Fix)
    struct timeval timeout;
    timeout.tv_sec = 5;   // 5 second timeout
    timeout.tv_usec = 0;
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        hal_log_error("WS_SERVER", "ws_server_read_data", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to set socket timeout: %s", strerror(errno));
    }
    
    ssize_t n = recv(socket_fd, buffer, buffer_size, 0);
    if(n<=0){ 
        *received_length=0; 
        if (n == 0) {
            // Connection closed by client
            return HAL_STATUS_IO_ERROR;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Timeout occurred
            hal_log_error("WS_SERVER", "ws_server_read_data", __LINE__, 
                         HAL_STATUS_TIMEOUT, "Socket read timeout after 5 seconds");
            return HAL_STATUS_TIMEOUT;
        }
        return HAL_STATUS_IO_ERROR; 
    }
    *received_length=(size_t)n; 
    return HAL_STATUS_OK;
}

hal_status_t ws_server_write_data(int socket_fd, const uint8_t *data, size_t data_length) {
    if(socket_fd<0||!data||data_length==0) return HAL_STATUS_INVALID_PARAMETER;
    size_t off=0; while(off<data_length){ ssize_t n=send(socket_fd, data+off, data_length-off, 0); if(n<0){ if(errno==EINTR) continue; return HAL_STATUS_IO_ERROR;} if(n==0) break; off+=n; }
    return HAL_STATUS_OK;
}
static hal_status_t ws_server_send_http_response(int socket_fd, const char *response) {
    if (socket_fd < 0 || !response) return HAL_STATUS_INVALID_PARAMETER;
    size_t len = strlen(response);
    return ws_server_write_data(socket_fd, (const uint8_t*)response, len);
}

/**
 * @brief Create WebSocket handshake response
 * @param request HTTP request string
 * @param response Buffer to store response
 * @param response_size Size of response buffer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_create_handshake_response(const char *request, char *response, size_t response_size) {
    if (request == NULL || response == NULL || response_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Extract WebSocket key
    char websocket_key[128];
    hal_status_t key_result = ws_server_parse_http_headers(request, websocket_key, sizeof(websocket_key));
    if (key_result != HAL_STATUS_OK) {
        return key_result;
    }
    
    // Calculate accept key
    char accept_key[128];
    hal_status_t accept_result = ws_server_calculate_accept_key(websocket_key, accept_key, sizeof(accept_key));
    if (accept_result != HAL_STATUS_OK) {
        return accept_result;
    }
    
    // Create response
    int n = snprintf(response, response_size,
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n", accept_key);
    
    if (n < 0 || (size_t)n >= response_size) {
        hal_log_error("WS_SERVER", "ws_server_create_handshake_response", __LINE__, 
                     HAL_STATUS_ERROR, "Response too large for buffer");
        return HAL_STATUS_ERROR;
    }
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Created handshake response");
    return HAL_STATUS_OK;
}

/**
 * @brief Extract WebSocket key from HTTP request
 * @param request HTTP request string
 * @param key Buffer to store key
 * @param key_size Size of key buffer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_extract_websocket_key(const char *request, char *key, size_t key_size) {
    return ws_server_parse_http_headers(request, key, key_size);
}

/**
 * @brief Generate WebSocket accept key
 * @param websocket_key Client WebSocket key
 * @param accept_key Buffer to store accept key
 * @param accept_key_size Size of accept key buffer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_generate_accept_key(const char *websocket_key, char *accept_key, size_t accept_key_size) {
    return ws_server_calculate_accept_key(websocket_key, accept_key, accept_key_size);
}

/**
 * @brief Parse WebSocket frame from data
 * @param data Raw frame data
 * @param data_length Length of data
 * @param frame Frame structure to fill
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_parse_frame(const uint8_t *data, size_t data_length, ws_frame_t *frame) {
    if (data == NULL || frame == NULL || data_length < 2) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Parse frame header
    frame->fin = (data[0] & 0x80) != 0;
    frame->rsv1 = (data[0] & 0x40) != 0;
    frame->rsv2 = (data[0] & 0x20) != 0;
    frame->rsv3 = (data[0] & 0x10) != 0;
    frame->opcode = (ws_frame_type_t)(data[0] & 0x0F);
    
    frame->masked = (data[1] & 0x80) != 0;
    uint64_t payload_length = (data[1] & 0x7F);
    
    size_t header_size = 2;
    
    // Parse extended payload length
    if (payload_length == 126) {
        if (data_length < 4) {
            return HAL_STATUS_INVALID_PARAMETER;
        }
        payload_length = (data[2] << 8) | data[3];
        header_size = 4;
    } else if (payload_length == 127) {
        if (data_length < 10) {
            return HAL_STATUS_INVALID_PARAMETER;
        }
        payload_length = 0;
        for (int i = 0; i < 8; i++) {
            payload_length = (payload_length << 8) | data[2 + i];
        }
        header_size = 10;
    }
    
    // Parse masking key
    frame->masking_key = 0;
    if (frame->masked) {
        if (data_length < header_size + 4) {
            return HAL_STATUS_INVALID_PARAMETER;
        }
        frame->masking_key = (data[header_size] << 24) | 
                           (data[header_size + 1] << 16) | 
                           (data[header_size + 2] << 8) | 
                           data[header_size + 3];
        header_size += 4;
    }
    
    // Check if we have enough data for payload
    if (data_length < header_size + payload_length) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Allocate and copy payload
    frame->payload_length = payload_length;
    frame->payload_size = payload_length;
    if (payload_length > 0) {
        frame->payload = (uint8_t*)malloc(payload_length);
        if (frame->payload == NULL) {
            return HAL_STATUS_NO_MEMORY;
        }
        
        memcpy(frame->payload, data + header_size, payload_length);
        
        // Unmask payload if masked
        if (frame->masked) {
            ws_server_mask_payload(frame->payload, payload_length, frame->masking_key);
        }
    } else {
        frame->payload = NULL;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Mask WebSocket payload
 * @param payload Payload data
 * @param payload_length Payload length
 * @param masking_key Masking key
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_mask_payload(uint8_t *payload, size_t payload_length, uint32_t masking_key) {
    if (payload == NULL || payload_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    uint8_t *mask = (uint8_t*)&masking_key;
    for (size_t i = 0; i < payload_length; i++) {
        payload[i] ^= mask[i % 4];
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get WebSocket server statistics
 * @param statistics Pointer to statistics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_get_statistics(ws_server_statistics_t *statistics) {
    if (statistics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    
    // Update uptime
    g_ws_server.status.statistics.uptime_ms = hal_get_timestamp_ms();
    g_ws_server.status.statistics.active_connections = g_ws_server.client_count;
    
    // Copy statistics
    memcpy(statistics, &g_ws_server.status.statistics, sizeof(ws_server_statistics_t));
    
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Reset WebSocket server statistics
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_reset_statistics(void) {
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    
    // Reset all statistics
    memset(&g_ws_server.status.statistics, 0, sizeof(ws_server_statistics_t));
    g_ws_server.status.statistics.uptime_ms = hal_get_timestamp_ms();
    
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Statistics reset");
    return HAL_STATUS_OK;
}

/**
 * @brief Update WebSocket server statistics
 * @param stats Statistics structure to update
 * @param sent True if message was sent, false if received
 * @param bytes Number of bytes
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_update_statistics(ws_server_statistics_t *stats, bool sent, size_t bytes) {
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (sent) {
        stats->total_messages_sent++;
        stats->total_bytes_sent += bytes;
    } else {
        stats->total_messages_received++;
        stats->total_bytes_received += bytes;
    }
    
    stats->last_activity = hal_get_timestamp_ms();
    
    return HAL_STATUS_OK;
}

/**
 * @brief Log WebSocket connection event
 * @param client_ip Client IP address
 * @param client_port Client port
 * @param connected True if connected, false if disconnected
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_log_connection(const char *client_ip, uint16_t client_port, bool connected) {
    if (client_ip == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    const char *action = connected ? "connected" : "disconnected";
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Client %s:%d %s", 
                   client_ip, client_port, action);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Log WebSocket message event
 * @param client_id Client identifier
 * @param message Message content
 * @param message_length Message length
 * @param sent True if message was sent, false if received
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_log_message(const char *client_id, const char *message, size_t message_length, bool sent) {
    if (client_id == NULL || message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    const char *direction = sent ? "sent to" : "received from";
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Message %s %s (%zu bytes): %.100s%s", 
                   direction, client_id, message_length, message, 
                   message_length > 100 ? "..." : "");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Log WebSocket error event
 * @param error_message Error message
 * @param context Error context
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_log_error(const char *error_message, const char *context) {
    if (error_message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (context != NULL) {
        hal_log_error("WS_SERVER", context, __LINE__, HAL_STATUS_ERROR, "%s", error_message);
    } else {
        hal_log_error("WS_SERVER", "ws_server_log_error", __LINE__, HAL_STATUS_ERROR, "%s", error_message);
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Base64 encode data
 * @param input Input data
 * @param input_length Input data length
 * @param output Output buffer
 * @param output_size Output buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_base64_encode(const uint8_t *input, size_t input_length, char *output, size_t output_size) {
    if (input == NULL || output == NULL || output_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int outlen = EVP_EncodeBlock((unsigned char*)output, input, input_length);
    if (outlen <= 0 || (size_t)outlen >= output_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Compute SHA1 hash of input string
 * @param input Input string
 * @param output Output buffer for hash
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_sha1_hash(const char *input, uint8_t *output) {
    if (input == NULL || output == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    SHA1((const unsigned char*)input, strlen(input), output);
    return HAL_STATUS_OK;
}

/**
 * @brief Broadcast telemetry data to all connected clients
 * @param telemetry_data JSON telemetry data
 * @param data_length Length of telemetry data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_broadcast_telemetry(const char *telemetry_data, size_t data_length) {
    if (telemetry_data == NULL || data_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized || !g_ws_server.running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "WebSocket Server: Broadcasting telemetry to %d clients", g_ws_server.client_count);
    
    // Broadcast to all connected clients
    hal_status_t result = ws_server_broadcast_message(telemetry_data, data_length);
    if (result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_broadcast_telemetry", __LINE__, 
                     result, "Failed to broadcast telemetry data");
        return result;
    }
    
    // Update statistics
    pthread_mutex_lock(&g_ws_server.mutex);
    g_ws_server.status.statistics.total_messages_sent += g_ws_server.client_count;
    g_ws_server.status.statistics.total_bytes_sent += data_length * g_ws_server.client_count;
    g_ws_server.status.statistics.last_activity = hal_get_timestamp_ms();
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Broadcast robot status to all connected clients
 * @param robot_status Robot status data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_broadcast_robot_status(const ws_robot_status_t *robot_status) {
    if (robot_status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized || !g_ws_server.running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Create JSON message
    char json_message[1024];
    int n = snprintf(json_message, sizeof(json_message),
        "{"
        "\"type\":\"robot_status\","
        "\"timestamp\":%lu,"
        "\"data\":{"
        "\"robot_id\":\"%s\","
        "\"status\":\"%s\","
        "\"position\":{\"x\":%.3f,\"y\":%.3f},"
        "\"battery_level\":%d,"
        "\"temperature\":%.1f,"
        "\"speed\":%.2f,"
        "\"connection_status\":\"%s\""
        "}"
        "}",
        hal_get_timestamp_ms(),
        robot_status->robot_id,
        robot_status->status,
        robot_status->position.x,
        robot_status->position.y,
        robot_status->battery_level,
        robot_status->temperature,
        robot_status->speed,
        robot_status->connection_status
    );
    
    if (n < 0 || (size_t)n >= sizeof(json_message)) {
        hal_log_error("WS_SERVER", "ws_server_broadcast_robot_status", __LINE__, 
                     HAL_STATUS_ERROR, "JSON message too large");
        return HAL_STATUS_ERROR;
    }
    
    return ws_server_broadcast_telemetry(json_message, strlen(json_message));
}

/**
 * @brief Broadcast system alert to all connected clients
 * @param alert_type Alert type
 * @param alert_message Alert message
 * @param severity Alert severity
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_broadcast_alert(const char *alert_type, const char *alert_message, ws_alert_severity_t severity) {
    if (alert_type == NULL || alert_message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_server.initialized || !g_ws_server.running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Create JSON alert message
    char json_message[1024];
    int n = snprintf(json_message, sizeof(json_message),
        "{"
        "\"type\":\"alert\","
        "\"timestamp\":%lu,"
        "\"data\":{"
        "\"alert_type\":\"%s\","
        "\"message\":\"%s\","
        "\"severity\":\"%s\","
        "\"acknowledged\":false"
        "}"
        "}",
        hal_get_timestamp_ms(),
        alert_type,
        alert_message,
        (severity == WS_ALERT_CRITICAL) ? "critical" :
        (severity == WS_ALERT_WARNING) ? "warning" : "info"
    );
    
    if (n < 0 || (size_t)n >= sizeof(json_message)) {
        hal_log_error("WS_SERVER", "ws_server_broadcast_alert", __LINE__, 
                     HAL_STATUS_ERROR, "JSON alert message too large");
        return HAL_STATUS_ERROR;
    }
    
    hal_log_message(HAL_LOG_LEVEL_WARNING, "WebSocket Server: Broadcasting alert: %s - %s", alert_type, alert_message);
    
    return ws_server_broadcast_telemetry(json_message, strlen(json_message));
}

/**
 * @brief Broadcast system heartbeat to all connected clients
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_broadcast_heartbeat(void) {
    if (!g_ws_server.initialized || !g_ws_server.running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Create JSON heartbeat message
    char json_message[256];
    int n = snprintf(json_message, sizeof(json_message),
        "{"
        "\"type\":\"heartbeat\","
        "\"timestamp\":%lu,"
        "\"data\":{"
        "\"server_status\":\"running\","
        "\"active_connections\":%d,"
        "\"uptime_ms\":%lu"
        "}"
        "}",
        hal_get_timestamp_ms(),
        g_ws_server.client_count,
        hal_get_timestamp_ms() - g_ws_server.status.statistics.uptime_ms
    );
    
    if (n < 0 || (size_t)n >= sizeof(json_message)) {
        hal_log_error("WS_SERVER", "ws_server_broadcast_heartbeat", __LINE__, 
                     HAL_STATUS_ERROR, "JSON heartbeat message too large");
        return HAL_STATUS_ERROR;
    }
    
    return ws_server_broadcast_telemetry(json_message, strlen(json_message));
}

/**
 * @brief Start telemetry streaming thread
 * @param interval_ms Telemetry interval in milliseconds
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_start_telemetry_streaming(uint32_t interval_ms) {
    if (!g_ws_server.initialized || !g_ws_server.running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (interval_ms == 0 || interval_ms > 60000) { // Max 1 minute
        hal_log_error("WS_SERVER", "ws_server_start_telemetry_streaming", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Invalid telemetry interval: %d ms", interval_ms);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Set telemetry interval
    pthread_mutex_lock(&g_ws_server.mutex);
    g_ws_server.telemetry_interval_ms = interval_ms;
    g_ws_server.telemetry_streaming = true;
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Started telemetry streaming with interval %d ms", interval_ms);
    return HAL_STATUS_OK;
}

/**
 * @brief Stop telemetry streaming
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_stop_telemetry_streaming(void) {
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    g_ws_server.telemetry_streaming = false;
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Stopped telemetry streaming");
    return HAL_STATUS_OK;
}

/**
 * @brief Set telemetry callback function
 * @param callback Telemetry data callback function
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_set_telemetry_callback(ws_telemetry_callback_t callback) {
    if (!g_ws_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_server.mutex);
    g_ws_server.telemetry_callback = callback;
    pthread_mutex_unlock(&g_ws_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Telemetry callback set");
    return HAL_STATUS_OK;
}

/**
 * @brief Telemetry streaming thread function
 * @param arg Thread argument (unused)
 * @return void* Thread return value
 */
void* ws_server_telemetry_thread(void *arg) {
    (void)arg;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Telemetry streaming thread started");
    
    while (g_ws_server.running) {
        pthread_mutex_lock(&g_ws_server.mutex);
        bool streaming = g_ws_server.telemetry_streaming;
        uint32_t interval = g_ws_server.telemetry_interval_ms;
        ws_telemetry_callback_t callback = g_ws_server.telemetry_callback;
        pthread_mutex_unlock(&g_ws_server.mutex);
        
        if (streaming && callback != NULL) {
            // Call telemetry callback to get current data
            char telemetry_data[2048];
            size_t data_length = sizeof(telemetry_data);
            
            hal_status_t result = callback(telemetry_data, &data_length);
            if (result == HAL_STATUS_OK && data_length > 0) {
                // Broadcast telemetry data
                ws_server_broadcast_telemetry(telemetry_data, data_length);
            }
        } else if (streaming) {
            // Send heartbeat if no callback is set
            ws_server_broadcast_heartbeat();
        }
        
        // Sleep for the specified interval
        usleep(interval * 1000);
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "WebSocket Server: Telemetry streaming thread stopped");
    return NULL;
}

/**
 * @brief Convert WebSocket frame type to string
 * @param frame_type WebSocket frame type
 * @return const char* String representation of frame type
 */
const char* ws_frame_type_to_string(ws_frame_type_t frame_type) {
    switch (frame_type) {
        case WS_FRAME_CONTINUATION: return "CONTINUATION";
        case WS_FRAME_TEXT: return "TEXT";
        case WS_FRAME_BINARY: return "BINARY";
        case WS_FRAME_CLOSE: return "CLOSE";
        case WS_FRAME_PING: return "PING";
        case WS_FRAME_PONG: return "PONG";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Convert WebSocket close code to string
 * @param close_code WebSocket close code
 * @return const char* String representation of close code
 */
const char* ws_close_code_to_string(ws_close_code_t close_code) {
    switch (close_code) {
        case WS_CLOSE_NORMAL: return "NORMAL";
        case WS_CLOSE_GOING_AWAY: return "GOING_AWAY";
        case WS_CLOSE_PROTOCOL_ERROR: return "PROTOCOL_ERROR";
        case WS_CLOSE_UNSUPPORTED_DATA: return "UNSUPPORTED_DATA";
        case WS_CLOSE_NO_STATUS: return "NO_STATUS";
        case WS_CLOSE_ABNORMAL: return "ABNORMAL";
        case WS_CLOSE_INVALID_DATA: return "INVALID_DATA";
        case WS_CLOSE_POLICY_VIOLATION: return "POLICY_VIOLATION";
        case WS_CLOSE_MESSAGE_TOO_BIG: return "MESSAGE_TOO_BIG";
        case WS_CLOSE_MANDATORY_EXTENSION: return "MANDATORY_EXTENSION";
        case WS_CLOSE_INTERNAL_ERROR: return "INTERNAL_ERROR";
        case WS_CLOSE_SERVICE_RESTART: return "SERVICE_RESTART";
        case WS_CLOSE_TRY_AGAIN_LATER: return "TRY_AGAIN_LATER";
        case WS_CLOSE_BAD_GATEWAY: return "BAD_GATEWAY";
        case WS_CLOSE_TLS_HANDSHAKE: return "TLS_HANDSHAKE";
        default: return "UNKNOWN";
    }
}

