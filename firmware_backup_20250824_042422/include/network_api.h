/**
 * @file network_api.h
 * @brief Network Communication Layer for OHT-50 Master Module Firmware
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-04 (Network Communication Layer)
 */

#ifndef NETWORK_API_H
#define NETWORK_API_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// Network API Configuration
#define NETWORK_MAX_CONNECTIONS      5
#define NETWORK_BUFFER_SIZE          4096
#define NETWORK_DEFAULT_TIMEOUT_MS   5000
#define NETWORK_MAX_RETRIES          3
#define NETWORK_RETRY_DELAY_MS       1000

// Network Connection Types
typedef enum {
    NETWORK_TYPE_TCP = 0,
    NETWORK_TYPE_UDP,
    NETWORK_TYPE_HTTP,
    NETWORK_TYPE_HTTPS
} network_type_t;

// Network Connection States
typedef enum {
    NETWORK_STATE_DISCONNECTED = 0,
    NETWORK_STATE_CONNECTING,
    NETWORK_STATE_CONNECTED,
    NETWORK_STATE_ERROR
} network_state_t;

// Network Connection Structure
typedef struct {
    int socket_fd;
    network_type_t type;
    network_state_t state;
    char remote_host[256];
    uint16_t remote_port;
    uint64_t connection_id;
    uint64_t last_activity;
    uint32_t retry_count;
    uint32_t bytes_sent;
    uint32_t bytes_received;
    bool auto_reconnect;
    uint32_t reconnect_interval_ms;
} network_connection_t;

// Network API Configuration
typedef struct {
    uint32_t max_connections;
    uint32_t default_timeout_ms;
    uint32_t max_retries;
    uint32_t retry_delay_ms;
    bool enable_auto_reconnect;
    uint32_t reconnect_interval_ms;
    bool enable_logging;
    char log_file[256];
} network_api_config_t;

// Network API Status
typedef struct {
    bool is_initialized;
    uint32_t active_connections;
    uint32_t total_connections;
    uint32_t failed_connections;
    uint64_t total_bytes_sent;
    uint64_t total_bytes_received;
    uint64_t uptime_ms;
} network_api_status_t;

// Network Message Structure
typedef struct {
    uint32_t message_id;
    uint32_t message_type;
    uint32_t payload_length;
    uint8_t payload[NETWORK_BUFFER_SIZE];
    uint64_t timestamp;
} network_message_t;

// Network API Functions

/**
 * @brief Initialize network API
 * @param config Network API configuration
 * @return HAL status
 */
hal_status_t network_api_init(const network_api_config_t *config);

/**
 * @brief Deinitialize network API
 * @return HAL status
 */
hal_status_t network_api_deinit(void);

/**
 * @brief Create TCP connection
 * @param host Remote host
 * @param port Remote port
 * @param timeout_ms Connection timeout
 * @param connection_id Output connection ID
 * @return HAL status
 */
hal_status_t network_create_tcp_connection(const char *host, uint16_t port, 
                                          uint32_t timeout_ms, uint64_t *connection_id);

/**
 * @brief Create UDP connection
 * @param host Remote host
 * @param port Remote port
 * @param connection_id Output connection ID
 * @return HAL status
 */
hal_status_t network_create_udp_connection(const char *host, uint16_t port, 
                                          uint64_t *connection_id);

/**
 * @brief Close connection
 * @param connection_id Connection ID
 * @return HAL status
 */
hal_status_t network_close_connection(uint64_t connection_id);

/**
 * @brief Send data
 * @param connection_id Connection ID
 * @param data Data to send
 * @param length Data length
 * @return HAL status
 */
hal_status_t network_send_data(uint64_t connection_id, const uint8_t *data, size_t length);

/**
 * @brief Receive data
 * @param connection_id Connection ID
 * @param data Output buffer
 * @param max_length Maximum buffer length
 * @param actual_length Actual received length
 * @return HAL status
 */
hal_status_t network_receive_data(uint64_t connection_id, uint8_t *data, 
                                 size_t max_length, size_t *actual_length);

/**
 * @brief Send HTTP request
 * @param host Remote host
 * @param port Remote port
 * @param method HTTP method
 * @param path Request path
 * @param headers Request headers
 * @param body Request body
 * @param response Output response buffer
 * @param max_response_length Maximum response buffer length
 * @param actual_response_length Actual response length
 * @return HAL status
 */
hal_status_t network_send_http_request(const char *host, uint16_t port,
                                      const char *method, const char *path,
                                      const char *headers, const char *body,
                                      char *response, size_t max_response_length,
                                      size_t *actual_response_length);

/**
 * @brief Get connection status
 * @param connection_id Connection ID
 * @param state Output connection state
 * @return HAL status
 */
hal_status_t network_get_connection_status(uint64_t connection_id, network_state_t *state);

/**
 * @brief Get network API status
 * @param status Output status structure
 * @return HAL status
 */
hal_status_t network_get_api_status(network_api_status_t *status);

/**
 * @brief Set connection auto-reconnect
 * @param connection_id Connection ID
 * @param enable Enable/disable auto-reconnect
 * @param interval_ms Reconnect interval
 * @return HAL status
 */
hal_status_t network_set_auto_reconnect(uint64_t connection_id, bool enable, uint32_t interval_ms);

/**
 * @brief Monitor network connections
 * @param timeout_ms Monitoring timeout
 * @return HAL status
 */
hal_status_t network_monitor_connections(uint32_t timeout_ms);

// Network Utility Functions

/**
 * @brief Resolve hostname to IP address
 * @param hostname Hostname to resolve
 * @param ip_address Output IP address
 * @return HAL status
 */
hal_status_t network_resolve_hostname(const char *hostname, char *ip_address);

/**
 * @brief Check if host is reachable
 * @param host Remote host
 * @param port Remote port
 * @param timeout_ms Timeout
 * @return HAL status
 */
hal_status_t network_check_connectivity(const char *host, uint16_t port, uint32_t timeout_ms);

/**
 * @brief Get local IP address
 * @param interface_name Network interface name
 * @param ip_address Output IP address
 * @return HAL status
 */
hal_status_t network_get_local_ip(const char *interface_name, char *ip_address);

/**
 * @brief Create network message
 * @param message_type Message type
 * @param payload Message payload
 * @param payload_length Payload length
 * @param message Output message structure
 * @return HAL status
 */
hal_status_t network_create_message(uint32_t message_type, const uint8_t *payload,
                                   uint32_t payload_length, network_message_t *message);

/**
 * @brief Parse network message
 * @param data Raw message data
 * @param length Data length
 * @param message Output parsed message
 * @return HAL status
 */
hal_status_t network_parse_message(const uint8_t *data, size_t length, network_message_t *message);

/**
 * @brief Send network message
 * @param connection_id Connection ID
 * @param message Message to send
 * @return HAL status
 */
hal_status_t network_send_message(uint64_t connection_id, const network_message_t *message);

/**
 * @brief Receive network message
 * @param connection_id Connection ID
 * @param message Output received message
 * @return HAL status
 */
hal_status_t network_receive_message(uint64_t connection_id, network_message_t *message);

// Network Configuration Functions

/**
 * @brief Set network API configuration
 * @param config New configuration
 * @return HAL status
 */
hal_status_t network_set_config(const network_api_config_t *config);

/**
 * @brief Get network API configuration
 * @param config Output configuration structure
 * @return HAL status
 */
hal_status_t network_get_config(network_api_config_t *config);

// Network Monitoring Functions

/**
 * @brief Get connection statistics
 * @param connection_id Connection ID
 * @param bytes_sent Output bytes sent
 * @param bytes_received Output bytes received
 * @return HAL status
 */
hal_status_t network_get_connection_stats(uint64_t connection_id, 
                                         uint32_t *bytes_sent, uint32_t *bytes_received);

/**
 * @brief Reset connection statistics
 * @param connection_id Connection ID
 * @return HAL status
 */
hal_status_t network_reset_connection_stats(uint64_t connection_id);

// Default Network API Configuration
extern const network_api_config_t NETWORK_API_DEFAULT_CONFIG;

#endif // NETWORK_API_H
