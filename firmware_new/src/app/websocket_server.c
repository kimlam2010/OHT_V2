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
#include <openssl/sha.h>
#include <openssl/evp.h>

// Global WebSocket Server Instance
ws_server_instance_t g_ws_server = {0};

// Private function declarations
static hal_status_t ws_server_initialize_defaults(void);
static hal_status_t ws_server_setup_signal_handlers(void);
static hal_status_t ws_server_cleanup_resources(void);
static hal_status_t ws_server_validate_config(const ws_server_config_t *config);
static hal_status_t ws_server_find_client(int socket_fd, ws_client_t **client);
static hal_status_t ws_server_parse_http_headers(const char *request, char *websocket_key, size_t key_size);
static hal_status_t ws_server_calculate_accept_key(const char *websocket_key, char *accept_key, size_t accept_key_size);
static hal_status_t ws_server_send_http_response(int socket_fd, const char *response);
static hal_status_t ws_server_read_data(int socket_fd, uint8_t *buffer, size_t buffer_size, size_t *received_length);
static hal_status_t ws_server_write_data(int socket_fd, const uint8_t *data, size_t data_length);
static hal_status_t ws_server_handle_ping(ws_client_t *client);
static hal_status_t ws_server_handle_pong(ws_client_t *client);
static hal_status_t ws_server_handle_close(ws_client_t *client, ws_close_code_t code, const char *reason);

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
    
    // Validate input parameters
    if (config == NULL) {
        hal_log_error("WS_SERVER", "ws_server_init", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Config parameter is NULL");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    hal_status_t validation_result = ws_server_validate_config(config);
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
    
    // Copy configuration
    memcpy(&g_ws_server.config, config, sizeof(ws_server_config_t));
    
    // Initialize status
    memset(&g_ws_server.status, 0, sizeof(ws_server_status_t));
    g_ws_server.status.overall_status = HAL_DEVICE_STATUS_INITIALIZING;
    
    // Initialize clients array
    memset(g_ws_server.clients, 0, sizeof(g_ws_server.clients));
    g_ws_server.client_count = 0;
    
    // Initialize message handler
    g_ws_server.message_handler = NULL;
    
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

// Private function implementations
static hal_status_t ws_server_validate_config(const ws_server_config_t *config) {
    if (config->port == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_clients == 0 || config->max_clients > WS_SERVER_MAX_CLIENTS) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_message_size == 0 || config->max_message_size > WS_SERVER_MAX_MESSAGE_SIZE) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_frame_size == 0 || config->max_frame_size > WS_SERVER_MAX_FRAME_SIZE) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
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
    
    // Read handshake request
    hal_status_t read_result = ws_server_read_data(client_socket, (uint8_t*)request_buffer, 
                                                  sizeof(request_buffer), &received_length);
    if (read_result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_client_thread", __LINE__, 
                     read_result, "Failed to read handshake request");
        ws_server_remove_client(client_socket);
        return NULL;
    }
    
    // Handle handshake
    hal_status_t handshake_result = ws_server_handle_handshake(client_socket, request_buffer, received_length);
    if (handshake_result != HAL_STATUS_OK) {
        hal_log_error("WS_SERVER", "ws_server_client_thread", __LINE__, 
                     handshake_result, "Failed to handle handshake");
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
    ssize_t n = recv(socket_fd, buffer, buffer_size, 0);
    if(n<=0){ *received_length=0; return HAL_STATUS_IO_ERROR; }
    *received_length=(size_t)n; return HAL_STATUS_OK;
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
