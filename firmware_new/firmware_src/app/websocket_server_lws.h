/**
 * @file websocket_server_lws.h
 * @brief WebSocket Server Implementation using libwebsockets for OHT-50 Master Module
 * @version 2.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket System Overhaul - Issue #153)
 * 
 * This file implements a robust WebSocket server using libwebsockets library
 * to fix critical issues in the original custom implementation:
 * - Thread memory leaks
 * - Connection hangs
 * - Protocol violations
 * - Poor error recovery
 */

#ifndef WEBSOCKET_SERVER_LWS_H
#define WEBSOCKET_SERVER_LWS_H

#include <libwebsockets.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "hal/hal_common.h"
#include "api/api_endpoints.h"
#include "api/security_auth.h"

// WebSocket Server Configuration
#define WS_LWS_MAX_CLIENTS 10
#define WS_LWS_MAX_MESSAGE_SIZE 4096
#define WS_LWS_PING_INTERVAL_SEC 30
#define WS_LWS_PONG_TIMEOUT_SEC 10
#define WS_LWS_HANDSHAKE_TIMEOUT_SEC 5

// WebSocket Message Types
typedef enum {
    WS_MSG_TYPE_TELEMETRY = 1,
    WS_MSG_TYPE_STATUS = 2,
    WS_MSG_TYPE_ALERT = 3,
    WS_MSG_TYPE_HEARTBEAT = 4,
    WS_MSG_TYPE_MODULE_DISCOVERY = 5,
    WS_MSG_TYPE_COMMAND_RESPONSE = 6
} ws_lws_message_type_t;

// WebSocket Client State
typedef enum {
    WS_CLIENT_STATE_CONNECTING = 0,
    WS_CLIENT_STATE_HANDSHAKE = 1,
    WS_CLIENT_STATE_CONNECTED = 2,
    WS_CLIENT_STATE_DISCONNECTING = 3,
    WS_CLIENT_STATE_DISCONNECTED = 4
} ws_lws_client_state_t;

// WebSocket Client Information
typedef struct {
    struct lws *wsi;                    // libwebsockets wsi
    ws_lws_client_state_t state;        // Client state
    time_t last_ping_time;              // Last ping time
    time_t last_pong_time;              // Last pong time
    uint32_t message_count;             // Message counter
    uint32_t error_count;               // Error counter
    bool authenticated;                 // Authentication status
    char client_ip[16];                 // Client IP address
    uint16_t client_port;               // Client port
    time_t connect_time;                // Connection time
} ws_lws_client_t;

// WebSocket Server Configuration
typedef struct {
    uint16_t port;                      // Server port
    uint32_t max_clients;               // Maximum clients
    uint32_t max_message_size;          // Maximum message size
    uint32_t ping_interval_sec;         // Ping interval
    uint32_t pong_timeout_sec;          // Pong timeout
    uint32_t handshake_timeout_sec;     // Handshake timeout
    bool enable_ssl;                    // Enable SSL/TLS
    char ssl_cert_path[256];            // SSL certificate path
    char ssl_key_path[256];             // SSL private key path
    bool enable_compression;            // Enable compression
    bool enable_authentication;         // Enable authentication
} ws_lws_config_t;

// WebSocket Server Instance
typedef struct {
    struct lws_context *context;        // libwebsockets context
    struct lws_context_creation_info info; // Context creation info
    ws_lws_config_t config;             // Server configuration
    ws_lws_client_t clients[WS_LWS_MAX_CLIENTS]; // Client array
    uint32_t client_count;              // Current client count
    pthread_t server_thread;            // Server thread
    pthread_mutex_t clients_mutex;      // Clients mutex
    bool running;                       // Server running flag
    bool initialized;                   // Initialization flag
    time_t start_time;                  // Server start time
    uint64_t total_connections;         // Total connections
    uint64_t total_messages;            // Total messages
    uint64_t total_errors;              // Total errors
} ws_lws_server_t;

// WebSocket Message Structure
typedef struct {
    ws_lws_message_type_t type;         // Message type
    uint32_t size;                      // Message size
    uint8_t data[WS_LWS_MAX_MESSAGE_SIZE]; // Message data
    time_t timestamp;                   // Message timestamp
    uint32_t client_id;                 // Client ID (0 = broadcast)
} ws_lws_message_t;

// Global WebSocket Server Instance
extern ws_lws_server_t g_ws_lws_server;

// WebSocket Server API Functions

/**
 * @brief Initialize WebSocket server with libwebsockets
 * @param config Pointer to configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_server_init(const ws_lws_config_t *config);

/**
 * @brief Start WebSocket server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_server_start(void);

/**
 * @brief Stop WebSocket server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_server_stop(void);

/**
 * @brief Cleanup WebSocket server resources
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_server_cleanup(void);

/**
 * @brief Send message to specific client
 * @param client_id Client ID
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_send_message(uint32_t client_id, const ws_lws_message_t *message);

/**
 * @brief Broadcast message to all connected clients
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_broadcast_message(const ws_lws_message_t *message);

/**
 * @brief Get server statistics
 * @param stats Pointer to statistics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_get_server_stats(ws_lws_server_t *stats);

/**
 * @brief Check if server is running
 * @return bool true if running, false otherwise
 */
bool ws_lws_server_is_running(void);

/**
 * @brief Get connected client count
 * @return uint32_t Number of connected clients
 */
uint32_t ws_lws_get_client_count(void);

// libwebsockets Callback Functions

/**
 * @brief WebSocket connection callback
 * @param wsi WebSocket instance
 * @param reason Connection reason
 * @param user User data
 * @param in Input data
 * @param len Input data length
 * @return int libwebsockets return code
 */
int ws_lws_callback_http(struct lws *wsi, enum lws_callback_reasons reason,
                        void *user, void *in, size_t len);

/**
 * @brief WebSocket protocol callback
 * @param wsi WebSocket instance
 * @param reason Protocol reason
 * @param user User data
 * @param in Input data
 * @param len Input data length
 * @return int libwebsockets return code
 */
int ws_lws_callback_protocol(struct lws *wsi, enum lws_callback_reasons reason,
                            void *user, void *in, size_t len);

// Utility Functions

/**
 * @brief Initialize default configuration
 * @param config Pointer to configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_init_default_config(ws_lws_config_t *config);

/**
 * @brief Validate configuration
 * @param config Pointer to configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_validate_config(const ws_lws_config_t *config);

/**
 * @brief Find client by wsi
 * @param wsi WebSocket instance
 * @param client Pointer to client pointer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_find_client_by_wsi(struct lws *wsi, ws_lws_client_t **client);

/**
 * @brief Add client to server
 * @param wsi WebSocket instance
 * @param client Pointer to client pointer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_add_client(struct lws *wsi, ws_lws_client_t **client);

/**
 * @brief Remove client from server
 * @param wsi WebSocket instance
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_remove_client(struct lws *wsi);

/**
 * @brief Cleanup disconnected clients
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_cleanup_disconnected_clients(void);

/**
 * @brief Send ping to client
 * @param client Pointer to client
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_send_ping(ws_lws_client_t *client);

/**
 * @brief Check client health
 * @param client Pointer to client
 * @return bool true if healthy, false otherwise
 */
bool ws_lws_is_client_healthy(ws_lws_client_t *client);

/**
 * @brief Create telemetry message
 * @param telemetry_data Pointer to telemetry data
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_create_telemetry_message(const void *telemetry_data, ws_lws_message_t *message);

/**
 * @brief Create status message
 * @param status_data Pointer to status data
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_create_status_message(const void *status_data, ws_lws_message_t *message);

/**
 * @brief Create alert message
 * @param alert_data Pointer to alert data
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_create_alert_message(const void *alert_data, ws_lws_message_t *message);

/**
 * @brief Create heartbeat message
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_create_heartbeat_message(ws_lws_message_t *message);

// Thread Functions

/**
 * @brief Server thread function
 * @param arg Thread argument
 * @return void* Thread return value
 */
void* ws_lws_server_thread(void *arg);

/**
 * @brief Telemetry streaming thread function
 * @param arg Thread argument
 * @return void* Thread return value
 */
void* ws_lws_telemetry_thread(void *arg);

/**
 * @brief Client health monitoring thread function
 * @param arg Thread argument
 * @return void* Thread return value
 */
void* ws_lws_health_monitor_thread(void *arg);

// Error Handling

/**
 * @brief Handle WebSocket error
 * @param wsi WebSocket instance
 * @param error_code Error code
 * @param error_message Error message
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_lws_handle_error(struct lws *wsi, int error_code, const char *error_message);

/**
 * @brief Log WebSocket event
 * @param level Log level
 * @param format Log format string
 * @param ... Variable arguments
 */
void ws_lws_log_event(hal_log_level_t level, const char *format, ...);

// Protocol Definitions
#define WS_LWS_PROTOCOL_NAME "oht50-protocol"
#define WS_LWS_PROTOCOL_COUNT 1

// libwebsockets Protocol Structure
extern struct lws_protocols ws_lws_protocols[];

#endif // WEBSOCKET_SERVER_LWS_H
