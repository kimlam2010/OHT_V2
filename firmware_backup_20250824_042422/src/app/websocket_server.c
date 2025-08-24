/**
 * @file websocket_server.c
 * @brief WebSocket Server Implementation for OHT-50 Master Module Firmware
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-02 (WebSocket Server Implementation)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include "websocket_server.h"
#include "hal_common.h"

// WebSocket Constants
const char* WS_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
const char* WS_ACCEPT_MAGIC = "HTTP/1.1 101 Switching Protocols\r\n"
                             "Upgrade: websocket\r\n"
                             "Connection: Upgrade\r\n"
                             "Sec-WebSocket-Accept: ";

// Default WebSocket server configuration
const ws_server_config_t WS_SERVER_DEFAULT_CONFIG = {
    .port = WS_SERVER_DEFAULT_PORT,
    .max_connections = WS_SERVER_MAX_CONNECTIONS,
    .heartbeat_interval_ms = WS_SERVER_HEARTBEAT_INTERVAL,
    .frame_timeout_ms = 5000,
    .enable_compression = false,
    .enable_logging = true,
    .log_file = "/var/log/oht50/websocket_server.log"
};

// Internal function declarations
static void* ws_server_thread(void *arg);
static hal_status_t ws_handle_connection(ws_server_t *server, int client_socket);
static hal_status_t ws_process_frame(ws_server_t *server, ws_connection_t *connection, const uint8_t *data, size_t length);
static ws_connection_t* ws_find_connection(ws_server_t *server, uint64_t connection_id);
static ws_connection_t* ws_add_connection(ws_server_t *server, int client_socket);
static void ws_remove_connection(ws_server_t *server, uint64_t connection_id);
static void ws_cleanup_connection(ws_connection_t *connection);
static void ws_log_event(ws_server_t *server, const char *event, const char *details);

// Base64 encoding function
static char* base64_encode(const unsigned char *input, int length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    char *b64text = (char *)malloc(bufferPtr->length + 1);
    memcpy(b64text, bufferPtr->data, bufferPtr->length);
    b64text[bufferPtr->length] = '\0';

    BIO_free_all(bio);
    return b64text;
}

hal_status_t ws_server_init(ws_server_t *server, const ws_server_config_t *config) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Initialize server structure
    memset(server, 0, sizeof(ws_server_t));
    
    // Set configuration
    if (config != NULL) {
        server->config = *config;
    } else {
        server->config = WS_SERVER_DEFAULT_CONFIG;
    }

    // Initialize mutex
    if (pthread_mutex_init(&server->mutex, NULL) != 0) {
        return HAL_STATUS_ERROR;
    }

    // Initialize status
    server->status.is_running = false;
    server->status.active_connections = 0;
    server->status.total_connections = 0;
    server->status.messages_sent = 0;
    server->status.messages_received = 0;
    server->status.bytes_sent = 0;
    server->status.bytes_received = 0;
    server->status.uptime_ms = 0;

    // Initialize connections
    for (int i = 0; i < WS_SERVER_MAX_CONNECTIONS; i++) {
        server->connections[i].socket_fd = -1;
        server->connections[i].state = WS_STATE_CLOSED;
    }

    server->should_stop = false;
    server->server_socket = -1;
    server->next_connection_id = 1;

    hal_log_info("[WS] Server initialized on port %d", server->config.port);
    return HAL_STATUS_OK;
}

hal_status_t ws_server_deinit(ws_server_t *server) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Stop server if running
    if (server->status.is_running) {
        ws_server_stop(server);
    }

    // Close server socket
    if (server->server_socket >= 0) {
        close(server->server_socket);
        server->server_socket = -1;
    }

    // Cleanup connections
    for (int i = 0; i < WS_SERVER_MAX_CONNECTIONS; i++) {
        if (server->connections[i].socket_fd >= 0) {
            ws_cleanup_connection(&server->connections[i]);
        }
    }

    // Destroy mutex
    pthread_mutex_destroy(&server->mutex);

    hal_log_info("[WS] Server deinitialized");
    return HAL_STATUS_OK;
}

hal_status_t ws_server_start(ws_server_t *server) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    
    if (server->status.is_running) {
        pthread_mutex_unlock(&server->mutex);
        return HAL_STATUS_ALREADY_ACTIVE;
    }

    // Create server socket
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_socket < 0) {
        pthread_mutex_unlock(&server->mutex);
        hal_log_error("[WS] Failed to create socket: %s", strerror(errno));
        return HAL_STATUS_ERROR;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(server->server_socket);
        server->server_socket = -1;
        pthread_mutex_unlock(&server->mutex);
        hal_log_error("[WS] Failed to set socket options: %s", strerror(errno));
        return HAL_STATUS_ERROR;
    }

    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server->config.port);

    if (bind(server->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server->server_socket);
        server->server_socket = -1;
        pthread_mutex_unlock(&server->mutex);
        hal_log_error("[WS] Failed to bind socket: %s", strerror(errno));
        return HAL_STATUS_ERROR;
    }

    // Listen for connections
    if (listen(server->server_socket, server->config.max_connections) < 0) {
        close(server->server_socket);
        server->server_socket = -1;
        pthread_mutex_unlock(&server->mutex);
        hal_log_error("[WS] Failed to listen: %s", strerror(errno));
        return HAL_STATUS_ERROR;
    }

    // Start server thread
    server->should_stop = false;
    if (pthread_create(&server->server_thread, NULL, ws_server_thread, server) != 0) {
        close(server->server_socket);
        server->server_socket = -1;
        pthread_mutex_unlock(&server->mutex);
        hal_log_error("[WS] Failed to create server thread");
        return HAL_STATUS_ERROR;
    }

    server->status.is_running = true;
    server->status.uptime_ms = hal_get_timestamp_ms();
    
    pthread_mutex_unlock(&server->mutex);

    hal_log_info("[WS] Server started on port %d", server->config.port);
    return HAL_STATUS_OK;
}

hal_status_t ws_server_stop(ws_server_t *server) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    
    if (!server->status.is_running) {
        pthread_mutex_unlock(&server->mutex);
        return HAL_STATUS_OK;
    }

    // Signal thread to stop
    server->should_stop = true;

    // Close server socket to wake up accept()
    if (server->server_socket >= 0) {
        close(server->server_socket);
        server->server_socket = -1;
    }

    pthread_mutex_unlock(&server->mutex);

    // Wait for thread to finish
    pthread_join(server->server_thread, NULL);

    pthread_mutex_lock(&server->mutex);
    server->status.is_running = false;
    pthread_mutex_unlock(&server->mutex);

    hal_log_info("[WS] Server stopped");
    return HAL_STATUS_OK;
}

hal_status_t ws_server_set_message_handler(ws_server_t *server, ws_message_handler_t handler) {
    if (server == NULL || handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    server->message_handler = handler;
    pthread_mutex_unlock(&server->mutex);

    return HAL_STATUS_OK;
}

hal_status_t ws_server_set_event_handler(ws_server_t *server, ws_event_handler_t handler) {
    if (server == NULL || handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    server->event_handler = handler;
    pthread_mutex_unlock(&server->mutex);

    return HAL_STATUS_OK;
}

hal_status_t ws_server_get_status(ws_server_t *server, ws_server_status_t *status) {
    if (server == NULL || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    *status = server->status;
    pthread_mutex_unlock(&server->mutex);

    return HAL_STATUS_OK;
}

hal_status_t ws_server_broadcast(ws_server_t *server, const char *message, size_t length) {
    if (server == NULL || message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    
    hal_status_t result = HAL_STATUS_OK;
    for (int i = 0; i < WS_SERVER_MAX_CONNECTIONS; i++) {
        if (server->connections[i].socket_fd >= 0 && 
            server->connections[i].state == WS_STATE_OPEN) {
            
            hal_status_t send_result = ws_send_text_frame(server->connections[i].socket_fd, message, length);
            if (send_result == HAL_STATUS_OK) {
                server->connections[i].messages_sent++;
                server->connections[i].bytes_sent += length;
                server->status.messages_sent++;
                server->status.bytes_sent += length;
            } else {
                result = send_result;
            }
        }
    }
    
    pthread_mutex_unlock(&server->mutex);
    return result;
}

hal_status_t ws_server_send(ws_server_t *server, uint64_t connection_id, const char *message, size_t length) {
    if (server == NULL || message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    
    ws_connection_t *connection = ws_find_connection(server, connection_id);
    if (connection == NULL || connection->state != WS_STATE_OPEN) {
        pthread_mutex_unlock(&server->mutex);
        return HAL_STATUS_NOT_FOUND;
    }

    hal_status_t result = ws_send_text_frame(connection->socket_fd, message, length);
    if (result == HAL_STATUS_OK) {
        connection->messages_sent++;
        connection->bytes_sent += length;
        server->status.messages_sent++;
        server->status.bytes_sent += length;
    }
    
    pthread_mutex_unlock(&server->mutex);
    return result;
}

hal_status_t ws_server_close_connection(ws_server_t *server, uint64_t connection_id) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    
    ws_connection_t *connection = ws_find_connection(server, connection_id);
    if (connection == NULL) {
        pthread_mutex_unlock(&server->mutex);
        return HAL_STATUS_NOT_FOUND;
    }

    // Send close frame
    ws_send_close_frame(connection->socket_fd, 1000, "Normal closure");
    
    // Update state
    connection->state = WS_STATE_CLOSING;
    
    pthread_mutex_unlock(&server->mutex);
    return HAL_STATUS_OK;
}

// WebSocket Server Thread Function
static void* ws_server_thread(void *arg) {
    ws_server_t *server = (ws_server_t*)arg;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    hal_log_info("[WS] Server thread started");

    while (!server->should_stop) {
        // Accept connection
        int client_socket = accept(server->server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            if (server->should_stop) {
                break;
            }
            hal_log_error("[WS] Accept failed: %s", strerror(errno));
            continue;
        }

        // Handle connection
        hal_status_t status = ws_handle_connection(server, client_socket);
        if (status != HAL_STATUS_OK) {
            hal_log_error("[WS] Connection handling failed: %d", status);
            close(client_socket);
        }
    }

    hal_log_info("[WS] Server thread stopped");
    return NULL;
}

// Handle individual connection
static hal_status_t ws_handle_connection(ws_server_t *server, int client_socket) {
    char buffer[WS_SERVER_BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        return HAL_STATUS_ERROR;
    }

    buffer[bytes_received] = '\0';

    // Check if this is a WebSocket handshake
    if (strstr(buffer, "GET") && strstr(buffer, "Upgrade: websocket")) {
        // Handle WebSocket handshake
        hal_status_t status = ws_handle_handshake(client_socket, buffer, bytes_received);
        if (status != HAL_STATUS_OK) {
            return status;
        }

        // Add connection to server
        pthread_mutex_lock(&server->mutex);
        ws_connection_t *connection = ws_add_connection(server, client_socket);
        if (connection == NULL) {
            pthread_mutex_unlock(&server->mutex);
            close(client_socket);
            return HAL_STATUS_NO_MEMORY;
        }

        // Get client IP
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        getpeername(client_socket, (struct sockaddr*)&client_addr, &addr_len);
        inet_ntop(AF_INET, &client_addr.sin_addr, connection->client_ip, sizeof(connection->client_ip));
        connection->client_port = ntohs(client_addr.sin_port);

        server->status.active_connections++;
        server->status.total_connections++;
        pthread_mutex_unlock(&server->mutex);

        // Call event handler
        if (server->event_handler) {
            server->event_handler(connection, "connect");
        }

        hal_log_info("[WS] New connection from %s:%d (ID: %llu)", 
                     connection->client_ip, connection->client_port, connection->connection_id);

        // Handle WebSocket frames
        while (connection->state == WS_STATE_OPEN) {
            bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
                break;
            }

            status = ws_process_frame(server, connection, (uint8_t*)buffer, bytes_received);
            if (status != HAL_STATUS_OK) {
                break;
            }
        }

        // Cleanup connection
        pthread_mutex_lock(&server->mutex);
        ws_remove_connection(server, connection->connection_id);
        server->status.active_connections--;
        pthread_mutex_unlock(&server->mutex);

        // Call event handler
        if (server->event_handler) {
            server->event_handler(connection, "disconnect");
        }

        ws_cleanup_connection(connection);
        close(client_socket);

        hal_log_info("[WS] Connection closed (ID: %llu)", connection->connection_id);
    } else {
        // Not a WebSocket request, close connection
        close(client_socket);
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

// Process WebSocket frame
static hal_status_t ws_process_frame(ws_server_t *server, ws_connection_t *connection, const uint8_t *data, size_t length) {
    ws_frame_t frame;
    hal_status_t status = ws_parse_frame(data, length, &frame);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    // Update connection stats
    connection->bytes_received += length;
    server->status.bytes_received += length;

    // Handle different frame types
    switch (frame.opcode) {
        case WS_FRAME_TEXT:
            connection->messages_received++;
            server->status.messages_received++;
            
            // Call message handler
            if (server->message_handler) {
                server->message_handler(connection, (char*)frame.payload, frame.payload_length);
            }
            break;

        case WS_FRAME_BINARY:
            connection->messages_received++;
            server->status.messages_received++;
            
            // Call message handler for binary data
            if (server->message_handler) {
                server->message_handler(connection, (char*)frame.payload, frame.payload_length);
            }
            break;

        case WS_FRAME_PING:
            // Send pong response
            ws_send_pong_frame(connection->socket_fd, (char*)frame.payload, frame.payload_length);
            break;

        case WS_FRAME_CLOSE:
            // Send close frame and close connection
            ws_send_close_frame(connection->socket_fd, 1000, "Normal closure");
            connection->state = WS_STATE_CLOSING;
            break;

        case WS_FRAME_CONTINUATION:
            // Handle fragmented frames (simplified implementation)
            break;

        default:
            // Unknown frame type, ignore
            break;
    }

    return HAL_STATUS_OK;
}

// Find connection by ID
static ws_connection_t* ws_find_connection(ws_server_t *server, uint64_t connection_id) {
    for (int i = 0; i < WS_SERVER_MAX_CONNECTIONS; i++) {
        if (server->connections[i].connection_id == connection_id) {
            return &server->connections[i];
        }
    }
    return NULL;
}

// Add new connection
static ws_connection_t* ws_add_connection(ws_server_t *server, int client_socket) {
    for (int i = 0; i < WS_SERVER_MAX_CONNECTIONS; i++) {
        if (server->connections[i].socket_fd == -1) {
            server->connections[i].socket_fd = client_socket;
            server->connections[i].state = WS_STATE_OPEN;
            server->connections[i].connection_id = server->next_connection_id++;
            server->connections[i].last_heartbeat = hal_get_timestamp_ms();
            server->connections[i].authenticated = false;
            server->connections[i].bytes_sent = 0;
            server->connections[i].bytes_received = 0;
            server->connections[i].messages_sent = 0;
            server->connections[i].messages_received = 0;
            return &server->connections[i];
        }
    }
    return NULL;
}

// Remove connection
static void ws_remove_connection(ws_server_t *server, uint64_t connection_id) {
    for (int i = 0; i < WS_SERVER_MAX_CONNECTIONS; i++) {
        if (server->connections[i].connection_id == connection_id) {
            server->connections[i].socket_fd = -1;
            server->connections[i].state = WS_STATE_CLOSED;
            break;
        }
    }
}

// Cleanup connection
static void ws_cleanup_connection(ws_connection_t *connection) {
    if (connection->socket_fd >= 0) {
        close(connection->socket_fd);
        connection->socket_fd = -1;
    }
    connection->state = WS_STATE_CLOSED;
}

// Log event
static void ws_log_event(ws_server_t *server, const char *event, const char *details) {
    if (server->config.enable_logging) {
        hal_log_info("[WS] %s: %s", event, details ? details : "");
    }
}

// WebSocket Utility Functions

hal_status_t ws_parse_frame(const uint8_t *data, size_t length, ws_frame_t *frame) {
    if (data == NULL || frame == NULL || length < 2) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    memset(frame, 0, sizeof(ws_frame_t));

    // Parse first byte
    frame->fin = (data[0] & 0x80) != 0;
    frame->rsv1 = (data[0] & 0x40) != 0;
    frame->rsv2 = (data[0] & 0x20) != 0;
    frame->rsv3 = (data[0] & 0x10) != 0;
    frame->opcode = (ws_frame_type_t)(data[0] & 0x0F);

    // Parse second byte
    frame->masked = (data[1] & 0x80) != 0;
    uint64_t payload_length = data[1] & 0x7F;

    size_t header_length = 2;

    // Extended payload length
    if (payload_length == 126) {
        if (length < 4) return HAL_STATUS_ERROR;
        payload_length = (data[2] << 8) | data[3];
        header_length = 4;
    } else if (payload_length == 127) {
        if (length < 10) return HAL_STATUS_ERROR;
        payload_length = 0;
        for (int i = 0; i < 8; i++) {
            payload_length = (payload_length << 8) | data[2 + i];
        }
        header_length = 10;
    }

    frame->payload_length = payload_length;

    // Masking key
    if (frame->masked) {
        if (length < header_length + 4) return HAL_STATUS_ERROR;
        frame->masking_key = (data[header_length] << 24) |
                           (data[header_length + 1] << 16) |
                           (data[header_length + 2] << 8) |
                           data[header_length + 3];
        header_length += 4;
    }

    // Payload
    if (length < header_length + payload_length) return HAL_STATUS_ERROR;
    
    if (payload_length > 0) {
        memcpy(frame->payload, data + header_length, payload_length);
        
        // Unmask payload if masked
        if (frame->masked) {
            for (size_t i = 0; i < payload_length; i++) {
                frame->payload[i] ^= ((frame->masking_key >> ((3 - (i % 4)) * 8)) & 0xFF);
            }
        }
    }

    return HAL_STATUS_OK;
}

hal_status_t ws_build_frame(const ws_frame_t *frame, uint8_t *data, size_t max_length, size_t *actual_length) {
    if (frame == NULL || data == NULL || actual_length == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    size_t header_length = 2;
    size_t total_length = header_length + frame->payload_length;

    if (total_length > max_length) {
        return HAL_STATUS_ERROR;
    }

    // First byte
    data[0] = (frame->fin ? 0x80 : 0x00) |
              (frame->rsv1 ? 0x40 : 0x00) |
              (frame->rsv2 ? 0x20 : 0x00) |
              (frame->rsv3 ? 0x10 : 0x00) |
              (frame->opcode & 0x0F);

    // Second byte (no masking for server frames)
    if (frame->payload_length < 126) {
        data[1] = frame->payload_length;
    } else if (frame->payload_length < 65536) {
        data[1] = 126;
        data[2] = (frame->payload_length >> 8) & 0xFF;
        data[3] = frame->payload_length & 0xFF;
        header_length = 4;
    } else {
        data[1] = 127;
        for (int i = 0; i < 8; i++) {
            data[2 + i] = (frame->payload_length >> ((7 - i) * 8)) & 0xFF;
        }
        header_length = 10;
    }

    // Copy payload
    if (frame->payload_length > 0) {
        memcpy(data + header_length, frame->payload, frame->payload_length);
    }

    *actual_length = header_length + frame->payload_length;
    return HAL_STATUS_OK;
}

hal_status_t ws_generate_accept_key(const char *client_key, char *accept_key) {
    if (client_key == NULL || accept_key == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Concatenate client key with GUID
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", client_key, WS_GUID);

    // Calculate SHA1 hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)combined, strlen(combined), hash);

    // Base64 encode
    char *b64 = base64_encode(hash, SHA_DIGEST_LENGTH);
    if (b64 == NULL) {
        return HAL_STATUS_ERROR;
    }

    strcpy(accept_key, b64);
    free(b64);

    return HAL_STATUS_OK;
}

hal_status_t ws_handle_handshake(int client_socket, const char *request, size_t request_length) {
    // Parse Sec-WebSocket-Key
    char *key_start = strstr(request, "Sec-WebSocket-Key: ");
    if (key_start == NULL) {
        return HAL_STATUS_ERROR;
    }

    key_start += 19; // Skip "Sec-WebSocket-Key: "
    char *key_end = strstr(key_start, "\r\n");
    if (key_end == NULL) {
        return HAL_STATUS_ERROR;
    }

    char client_key[64];
    size_t key_length = key_end - key_start;
    strncpy(client_key, key_start, key_length);
    client_key[key_length] = '\0';

    // Generate accept key
    char accept_key[64];
    hal_status_t status = ws_generate_accept_key(client_key, accept_key);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    // Send handshake response
    char response[512];
    snprintf(response, sizeof(response),
             "%s%s\r\n\r\n",
             WS_ACCEPT_MAGIC, accept_key);

    int sent = send(client_socket, response, strlen(response), 0);
    if (sent != strlen(response)) {
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

hal_status_t ws_send_close_frame(int client_socket, uint16_t code, const char *reason) {
    ws_frame_t frame = {0};
    frame.fin = true;
    frame.opcode = WS_FRAME_CLOSE;
    
    size_t reason_length = reason ? strlen(reason) : 0;
    frame.payload_length = 2 + reason_length; // 2 bytes for code + reason
    
    // Add close code
    frame.payload[0] = (code >> 8) & 0xFF;
    frame.payload[1] = code & 0xFF;
    
    // Add reason if provided
    if (reason && reason_length > 0) {
        memcpy(frame.payload + 2, reason, reason_length);
    }

    uint8_t frame_data[128];
    size_t frame_length;
    hal_status_t status = ws_build_frame(&frame, frame_data, sizeof(frame_data), &frame_length);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    int sent = send(client_socket, frame_data, frame_length, 0);
    return (sent == frame_length) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t ws_send_ping_frame(int client_socket, const char *payload, size_t payload_length) {
    ws_frame_t frame = {0};
    frame.fin = true;
    frame.opcode = WS_FRAME_PING;
    frame.payload_length = payload_length;
    
    if (payload && payload_length > 0) {
        memcpy(frame.payload, payload, payload_length);
    }

    uint8_t frame_data[128];
    size_t frame_length;
    hal_status_t status = ws_build_frame(&frame, frame_data, sizeof(frame_data), &frame_length);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    int sent = send(client_socket, frame_data, frame_length, 0);
    return (sent == frame_length) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t ws_send_pong_frame(int client_socket, const char *payload, size_t payload_length) {
    ws_frame_t frame = {0};
    frame.fin = true;
    frame.opcode = WS_FRAME_PONG;
    frame.payload_length = payload_length;
    
    if (payload && payload_length > 0) {
        memcpy(frame.payload, payload, payload_length);
    }

    uint8_t frame_data[128];
    size_t frame_length;
    hal_status_t status = ws_build_frame(&frame, frame_data, sizeof(frame_data), &frame_length);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    int sent = send(client_socket, frame_data, frame_length, 0);
    return (sent == frame_length) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t ws_send_text_frame(int client_socket, const char *text, size_t length) {
    ws_frame_t frame = {0};
    frame.fin = true;
    frame.opcode = WS_FRAME_TEXT;
    frame.payload_length = length;
    
    if (text && length > 0) {
        memcpy(frame.payload, text, length);
    }

    uint8_t frame_data[WS_SERVER_MAX_FRAME_SIZE];
    size_t frame_length;
    hal_status_t status = ws_build_frame(&frame, frame_data, sizeof(frame_data), &frame_length);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    int sent = send(client_socket, frame_data, frame_length, 0);
    return (sent == frame_length) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t ws_send_binary_frame(int client_socket, const uint8_t *data, size_t length) {
    ws_frame_t frame = {0};
    frame.fin = true;
    frame.opcode = WS_FRAME_BINARY;
    frame.payload_length = length;
    
    if (data && length > 0) {
        memcpy(frame.payload, data, length);
    }

    uint8_t frame_data[WS_SERVER_MAX_FRAME_SIZE];
    size_t frame_length;
    hal_status_t status = ws_build_frame(&frame, frame_data, sizeof(frame_data), &frame_length);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    int sent = send(client_socket, frame_data, frame_length, 0);
    return (sent == frame_length) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}
