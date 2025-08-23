/**
 * @file websocket_server.h
 * @brief WebSocket Server for OHT-50 Master Module Firmware
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-02 (WebSocket Server Implementation)
 */

#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// WebSocket Server Configuration
#define WS_SERVER_MAX_CONNECTIONS    10
#define WS_SERVER_BUFFER_SIZE        4096
#define WS_SERVER_DEFAULT_PORT       8081
#define WS_SERVER_MAX_FRAME_SIZE     65536
#define WS_SERVER_HEARTBEAT_INTERVAL 30000  // 30 seconds

// WebSocket Frame Types
typedef enum {
    WS_FRAME_CONTINUATION = 0x0,
    WS_FRAME_TEXT = 0x1,
    WS_FRAME_BINARY = 0x2,
    WS_FRAME_CLOSE = 0x8,
    WS_FRAME_PING = 0x9,
    WS_FRAME_PONG = 0xA
} ws_frame_type_t;

// WebSocket Connection States
typedef enum {
    WS_STATE_CONNECTING = 0,
    WS_STATE_OPEN,
    WS_STATE_CLOSING,
    WS_STATE_CLOSED
} ws_connection_state_t;

// WebSocket Frame Structure
typedef struct {
    bool fin;
    bool rsv1;
    bool rsv2;
    bool rsv3;
    ws_frame_type_t opcode;
    bool masked;
    uint64_t payload_length;
    uint32_t masking_key;
    uint8_t payload[WS_SERVER_MAX_FRAME_SIZE];
    uint64_t payload_offset;
} ws_frame_t;

// WebSocket Connection Structure
typedef struct {
    int socket_fd;
    ws_connection_state_t state;
    uint64_t connection_id;
    uint64_t last_heartbeat;
    bool authenticated;
    char client_ip[16];
    uint16_t client_port;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint32_t messages_sent;
    uint32_t messages_received;
} ws_connection_t;

// WebSocket Server Configuration
typedef struct {
    uint16_t port;
    uint32_t max_connections;
    uint32_t heartbeat_interval_ms;
    uint32_t frame_timeout_ms;
    bool enable_compression;
    bool enable_logging;
    char log_file[256];
} ws_server_config_t;

// WebSocket Server Status
typedef struct {
    bool is_running;
    uint32_t active_connections;
    uint32_t total_connections;
    uint32_t messages_sent;
    uint32_t messages_received;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t uptime_ms;
} ws_server_status_t;

// WebSocket Message Handler Function Type
typedef hal_status_t (*ws_message_handler_t)(ws_connection_t *connection, const char *message, size_t length);

// WebSocket Event Handler Function Type
typedef void (*ws_event_handler_t)(ws_connection_t *connection, const char *event_type);

// WebSocket Server Instance
typedef struct {
    ws_server_config_t config;
    ws_server_status_t status;
    ws_connection_t connections[WS_SERVER_MAX_CONNECTIONS];
    ws_message_handler_t message_handler;
    ws_event_handler_t event_handler;
    int server_socket;
    bool should_stop;
    pthread_t server_thread;
    pthread_mutex_t mutex;
    uint64_t next_connection_id;
} ws_server_t;

// WebSocket Server API Functions

/**
 * @brief Initialize WebSocket server
 * @param server WebSocket server instance
 * @param config Server configuration
 * @return HAL status
 */
hal_status_t ws_server_init(ws_server_t *server, const ws_server_config_t *config);

/**
 * @brief Deinitialize WebSocket server
 * @param server WebSocket server instance
 * @return HAL status
 */
hal_status_t ws_server_deinit(ws_server_t *server);

/**
 * @brief Start WebSocket server
 * @param server WebSocket server instance
 * @return HAL status
 */
hal_status_t ws_server_start(ws_server_t *server);

/**
 * @brief Stop WebSocket server
 * @param server WebSocket server instance
 * @return HAL status
 */
hal_status_t ws_server_stop(ws_server_t *server);

/**
 * @brief Set message handler
 * @param server WebSocket server instance
 * @param handler Message handler function
 * @return HAL status
 */
hal_status_t ws_server_set_message_handler(ws_server_t *server, ws_message_handler_t handler);

/**
 * @brief Set event handler
 * @param server WebSocket server instance
 * @param handler Event handler function
 * @return HAL status
 */
hal_status_t ws_server_set_event_handler(ws_server_t *server, ws_event_handler_t handler);

/**
 * @brief Get WebSocket server status
 * @param server WebSocket server instance
 * @param status Status structure to fill
 * @return HAL status
 */
hal_status_t ws_server_get_status(ws_server_t *server, ws_server_status_t *status);

/**
 * @brief Broadcast message to all connections
 * @param server WebSocket server instance
 * @param message Message to broadcast
 * @param length Message length
 * @return HAL status
 */
hal_status_t ws_server_broadcast(ws_server_t *server, const char *message, size_t length);

/**
 * @brief Send message to specific connection
 * @param server WebSocket server instance
 * @param connection_id Connection ID
 * @param message Message to send
 * @param length Message length
 * @return HAL status
 */
hal_status_t ws_server_send(ws_server_t *server, uint64_t connection_id, const char *message, size_t length);

/**
 * @brief Close specific connection
 * @param server WebSocket server instance
 * @param connection_id Connection ID
 * @return HAL status
 */
hal_status_t ws_server_close_connection(ws_server_t *server, uint64_t connection_id);

// WebSocket Utility Functions

/**
 * @brief Parse WebSocket frame
 * @param data Raw frame data
 * @param length Data length
 * @param frame Parsed frame structure
 * @return HAL status
 */
hal_status_t ws_parse_frame(const uint8_t *data, size_t length, ws_frame_t *frame);

/**
 * @brief Build WebSocket frame
 * @param frame Frame structure
 * @param data Output buffer for frame data
 * @param max_length Maximum buffer length
 * @param actual_length Actual frame length
 * @return HAL status
 */
hal_status_t ws_build_frame(const ws_frame_t *frame, uint8_t *data, size_t max_length, size_t *actual_length);

/**
 * @brief Generate WebSocket accept key
 * @param client_key Client key from handshake
 * @param accept_key Output accept key
 * @return HAL status
 */
hal_status_t ws_generate_accept_key(const char *client_key, char *accept_key);

/**
 * @brief Handle WebSocket handshake
 * @param client_socket Client socket
 * @param request HTTP request data
 * @param request_length Request data length
 * @return HAL status
 */
hal_status_t ws_handle_handshake(int client_socket, const char *request, size_t request_length);

/**
 * @brief Send WebSocket close frame
 * @param client_socket Client socket
 * @param code Close code
 * @param reason Close reason
 * @return HAL status
 */
hal_status_t ws_send_close_frame(int client_socket, uint16_t code, const char *reason);

/**
 * @brief Send WebSocket ping frame
 * @param client_socket Client socket
 * @param payload Ping payload
 * @param payload_length Payload length
 * @return HAL status
 */
hal_status_t ws_send_ping_frame(int client_socket, const char *payload, size_t payload_length);

/**
 * @brief Send WebSocket pong frame
 * @param client_socket Client socket
 * @param payload Pong payload
 * @param payload_length Payload length
 * @return HAL status
 */
hal_status_t ws_send_pong_frame(int client_socket, const char *payload, size_t payload_length);

/**
 * @brief Send WebSocket text frame
 * @param client_socket Client socket
 * @param text Text data
 * @param length Text length
 * @return HAL status
 */
hal_status_t ws_send_text_frame(int client_socket, const char *text, size_t length);

/**
 * @brief Send WebSocket binary frame
 * @param client_socket Client socket
 * @param data Binary data
 * @param length Data length
 * @return HAL status
 */
hal_status_t ws_send_binary_frame(int client_socket, const uint8_t *data, size_t length);

// WebSocket Constants
extern const char* WS_GUID;
extern const char* WS_ACCEPT_MAGIC;

// Default WebSocket Server Configuration
extern const ws_server_config_t WS_SERVER_DEFAULT_CONFIG;

#endif // WEBSOCKET_SERVER_H
