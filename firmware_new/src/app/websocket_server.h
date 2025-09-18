/**
 * @file websocket_server.h
 * @brief WebSocket Server Foundation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/time.h>

// Include HAL dependencies
#include "../../hal/common/hal_common.h"

// WebSocket Server Configuration
#define WS_SERVER_MAX_CLIENTS              10
#define WS_SERVER_MAX_MESSAGE_SIZE         4096
#define WS_SERVER_MAX_FRAME_SIZE           8192
#define WS_SERVER_DEFAULT_PORT             8081
#define WS_SERVER_DEFAULT_TIMEOUT_MS       5000
#define WS_SERVER_DEFAULT_BACKLOG          10
#define WS_SERVER_PING_INTERVAL_MS         30000
#define WS_SERVER_PONG_TIMEOUT_MS          10000

// WebSocket Frame Types
typedef enum {
    WS_FRAME_CONTINUATION = 0x0,
    WS_FRAME_TEXT = 0x1,
    WS_FRAME_BINARY = 0x2,
    WS_FRAME_CLOSE = 0x8,
    WS_FRAME_PING = 0x9,
    WS_FRAME_PONG = 0xA
} ws_frame_type_t;

// WebSocket Close Codes
typedef enum {
    WS_CLOSE_NORMAL = 1000,
    WS_CLOSE_GOING_AWAY = 1001,
    WS_CLOSE_PROTOCOL_ERROR = 1002,
    WS_CLOSE_UNSUPPORTED_DATA = 1003,
    WS_CLOSE_NO_STATUS = 1005,
    WS_CLOSE_ABNORMAL = 1006,
    WS_CLOSE_INVALID_DATA = 1007,
    WS_CLOSE_POLICY_VIOLATION = 1008,
    WS_CLOSE_MESSAGE_TOO_BIG = 1009,
    WS_CLOSE_MANDATORY_EXTENSION = 1010,
    WS_CLOSE_INTERNAL_ERROR = 1011,
    WS_CLOSE_SERVICE_RESTART = 1012,
    WS_CLOSE_TRY_AGAIN_LATER = 1013,
    WS_CLOSE_BAD_GATEWAY = 1014,
    WS_CLOSE_TLS_HANDSHAKE = 1015
} ws_close_code_t;

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
    uint8_t *payload;
    uint64_t payload_size;
} ws_frame_t;

// WebSocket Client Structure
typedef struct {
    int socket_fd;
    bool connected;
    bool authenticated;
    bool handshake_complete;
    struct timeval last_activity;
    struct timeval last_ping;
    char client_id[64];
    char client_ip[16];
    uint16_t client_port;
    pthread_t thread_id;
    pthread_mutex_t mutex;
    uint64_t messages_sent;
    uint64_t messages_received;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} ws_client_t;

// WebSocket Message Handler Function Type
typedef hal_status_t (*ws_message_handler_t)(const char *message, size_t message_length, ws_client_t *client);

// WebSocket Telemetry Callback Function Type
typedef hal_status_t (*ws_telemetry_callback_t)(char *telemetry_data, size_t *data_length);

// WebSocket Alert Severity
typedef enum {
    WS_ALERT_INFO = 0,
    WS_ALERT_WARNING = 1,
    WS_ALERT_CRITICAL = 2
} ws_alert_severity_t;

// WebSocket Robot Status Structure
typedef struct {
    char robot_id[64];
    char status[32];
    struct {
        float x;
        float y;
    } position;
    int battery_level;
    float temperature;
    float speed;
    char connection_status[32];
} ws_robot_status_t;

// WebSocket Server Configuration Structure
typedef struct {
    uint16_t port;
    uint32_t timeout_ms;
    uint32_t max_clients;
    uint32_t max_message_size;
    uint32_t max_frame_size;
    uint32_t ping_interval_ms;
    uint32_t pong_timeout_ms;
    bool enable_compression;
    bool enable_authentication;
    char server_name[64];
} ws_server_config_t;

// WebSocket Server Statistics Structure
typedef struct {
    uint64_t total_connections;
    uint64_t active_connections;
    uint64_t total_messages_sent;
    uint64_t total_messages_received;
    uint64_t total_bytes_sent;
    uint64_t total_bytes_received;
    uint64_t ping_frames_sent;
    uint64_t pong_frames_received;
    uint64_t close_frames_sent;
    uint64_t close_frames_received;
    uint64_t uptime_ms;
    uint64_t last_activity;
} ws_server_statistics_t;

// WebSocket Server Status Structure
typedef struct {
    bool running;
    bool listening;
    uint32_t active_connections;
    ws_server_statistics_t statistics;
    hal_device_status_t overall_status;
} ws_server_status_t;

// WebSocket Server Instance Structure
typedef struct {
    ws_server_config_t config;
    ws_server_status_t status;
    ws_client_t clients[WS_SERVER_MAX_CLIENTS];
    uint32_t client_count;
    ws_message_handler_t message_handler;
    ws_telemetry_callback_t telemetry_callback;
    int server_socket;
    pthread_t server_thread;
    pthread_t telemetry_thread;
    pthread_mutex_t mutex;
    bool initialized;
    bool running;
    bool telemetry_streaming;
    uint32_t telemetry_interval_ms;
} ws_server_instance_t;

// WebSocket Server Core Functions
hal_status_t ws_server_init(const ws_server_config_t *config);
hal_status_t ws_server_start(void);
hal_status_t ws_server_stop(void);
hal_status_t ws_server_deinit(void);
hal_status_t ws_server_get_status(ws_server_status_t *status);

// WebSocket Server Configuration Functions
hal_status_t ws_server_set_config(const ws_server_config_t *config);
hal_status_t ws_server_get_config(ws_server_config_t *config);
hal_status_t ws_server_set_default_config(ws_server_config_t *config);

// WebSocket Server Message Handler Functions
hal_status_t ws_server_register_message_handler(ws_message_handler_t handler);
hal_status_t ws_server_unregister_message_handler(void);

// WebSocket Server Client Management Functions
hal_status_t ws_server_add_client(int socket_fd, const char *client_ip, uint16_t client_port);
hal_status_t ws_server_remove_client(int socket_fd);
hal_status_t ws_server_get_clients(ws_client_t *clients, uint32_t *count);
hal_status_t ws_server_get_client(int socket_fd, ws_client_t **client);

// WebSocket Server Message Functions
hal_status_t ws_server_send_message(int socket_fd, const char *message, size_t message_length);
hal_status_t ws_server_send_text(int socket_fd, const char *text);
hal_status_t ws_server_send_binary(int socket_fd, const uint8_t *data, size_t data_length);
hal_status_t ws_server_broadcast_message(const char *message, size_t message_length);
hal_status_t ws_server_broadcast_text(const char *text);
hal_status_t ws_server_broadcast_binary(const uint8_t *data, size_t data_length);

// WebSocket Server Frame Functions
hal_status_t ws_server_send_frame(int socket_fd, const ws_frame_t *frame);
hal_status_t ws_server_receive_frame(int socket_fd, ws_frame_t *frame);
hal_status_t ws_server_send_ping(int socket_fd);
hal_status_t ws_server_send_pong(int socket_fd);
hal_status_t ws_server_send_close(int socket_fd, ws_close_code_t code, const char *reason);

// WebSocket Server Handshake Functions
hal_status_t ws_server_handle_handshake(int socket_fd, const char *request, size_t request_length);
hal_status_t ws_server_create_handshake_response(const char *request, char *response, size_t response_size);
hal_status_t ws_server_extract_websocket_key(const char *request, char *key, size_t key_size);
hal_status_t ws_server_generate_accept_key(const char *websocket_key, char *accept_key, size_t accept_key_size);

// WebSocket Server Utility Functions
hal_status_t ws_server_parse_frame(const uint8_t *data, size_t data_length, ws_frame_t *frame);
hal_status_t ws_server_create_frame(ws_frame_type_t opcode, const uint8_t *payload, size_t payload_length, 
                                   bool masked, ws_frame_t *frame);
hal_status_t ws_server_serialize_frame(const ws_frame_t *frame, uint8_t *buffer, size_t buffer_size, size_t *serialized_length);
hal_status_t ws_server_mask_payload(uint8_t *payload, size_t payload_length, uint32_t masking_key);

// WebSocket Server Statistics Functions
hal_status_t ws_server_get_statistics(ws_server_statistics_t *statistics);
hal_status_t ws_server_reset_statistics(void);
hal_status_t ws_server_update_statistics(ws_server_statistics_t *stats, bool sent, size_t bytes);

// WebSocket Server Logging Functions
hal_status_t ws_server_log_connection(const char *client_ip, uint16_t client_port, bool connected);
hal_status_t ws_server_log_message(const char *client_id, const char *message, size_t message_length, bool sent);
hal_status_t ws_server_log_error(const char *error_message, const char *context);

// WebSocket Server Telemetry Functions
hal_status_t ws_server_broadcast_telemetry(const char *telemetry_data, size_t data_length);
hal_status_t ws_server_broadcast_robot_status(const ws_robot_status_t *robot_status);
hal_status_t ws_server_broadcast_alert(const char *alert_type, const char *alert_message, ws_alert_severity_t severity);
hal_status_t ws_server_broadcast_heartbeat(void);
hal_status_t ws_server_start_telemetry_streaming(uint32_t interval_ms);
hal_status_t ws_server_stop_telemetry_streaming(void);
hal_status_t ws_server_set_telemetry_callback(ws_telemetry_callback_t callback);

// RS485 Telemetry Functions (Issue #90)
hal_status_t ws_server_broadcast_rs485_telemetry(uint8_t module_addr);
hal_status_t ws_server_start_rs485_telemetry_streaming(uint32_t interval_ms);

// WebSocket Server Thread Functions
void* ws_server_thread(void *arg);
void* ws_server_client_thread(void *arg);
void* ws_server_telemetry_thread(void *arg);

// WebSocket Server Utility Functions
const char* ws_frame_type_to_string(ws_frame_type_t frame_type);
const char* ws_close_code_to_string(ws_close_code_t close_code);
hal_status_t ws_server_base64_encode(const uint8_t *input, size_t input_length, char *output, size_t output_size);
hal_status_t ws_server_sha1_hash(const char *input, uint8_t *output);

// WebSocket Server Protocol Handlers (for internal use)
hal_status_t ws_server_handle_ping(ws_client_t *client);
hal_status_t ws_server_handle_pong(ws_client_t *client);
hal_status_t ws_server_handle_close(ws_client_t *client, ws_close_code_t code, const char *reason);

// WebSocket Server Global Instance
extern ws_server_instance_t g_ws_server;

// WebSocket Server Version Information
#define WS_SERVER_VERSION_MAJOR 1
#define WS_SERVER_VERSION_MINOR 0
#define WS_SERVER_VERSION_PATCH 0
#define WS_SERVER_VERSION_STRING "1.0.0"

hal_status_t ws_server_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch);
const char* ws_server_get_version_string(void);

#endif // WEBSOCKET_SERVER_H
