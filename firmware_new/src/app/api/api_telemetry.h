/**
 * @file api_telemetry.h
 * @brief API Telemetry & Real-time Data Management for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.2
 */

#ifndef API_TELEMETRY_H
#define API_TELEMETRY_H

#include "hal_common.h"
#include "api_manager.h"

// Telemetry Management Version
#define API_TELEMETRY_VERSION_MAJOR 1
#define API_TELEMETRY_VERSION_MINOR 0
#define API_TELEMETRY_VERSION_PATCH 0
#define API_TELEMETRY_VERSION_STRING "1.0.0"

// Telemetry Data Types
typedef enum {
    API_TELEMETRY_TYPE_SYSTEM = 0,
    API_TELEMETRY_TYPE_SAFETY,
    API_TELEMETRY_TYPE_MOTOR,
    API_TELEMETRY_TYPE_POWER,
    API_TELEMETRY_TYPE_DOCK,
    API_TELEMETRY_TYPE_NETWORK,
    API_TELEMETRY_TYPE_MAX
} api_telemetry_type_t;

// Telemetry Data Structure
typedef struct {
    uint64_t timestamp;              // Timestamp in milliseconds
    api_telemetry_type_t type;       // Data type
    char source[32];                 // Data source identifier
    uint32_t data_size;              // Size of data payload
    char data[512];                  // JSON data payload
} api_telemetry_data_t;

// Telemetry Stream Configuration
typedef struct {
    bool enabled;                    // Stream enabled flag
    uint32_t interval_ms;            // Update interval in milliseconds
    uint32_t max_clients;            // Maximum number of clients
    bool compression_enabled;        // Enable data compression
    uint32_t buffer_size;            // Buffer size for telemetry data
} api_telemetry_stream_config_t;

// Telemetry Client Information
typedef struct {
    uint32_t client_id;              // Unique client identifier
    int socket_fd;                   // Socket file descriptor
    bool connected;                  // Connection status
    uint64_t last_activity;          // Last activity timestamp
    uint32_t messages_sent;          // Number of messages sent
    uint32_t messages_failed;        // Number of failed messages
    api_telemetry_type_t subscribed_types[API_TELEMETRY_TYPE_MAX]; // Subscribed data types
    uint32_t subscribed_count;       // Number of subscribed types
} api_telemetry_client_t;

// Telemetry Statistics
typedef struct {
    uint32_t total_clients;          // Total number of clients
    uint32_t active_clients;         // Number of active clients
    uint32_t total_messages_sent;    // Total messages sent
    uint32_t total_messages_failed;  // Total failed messages
    uint64_t total_bytes_sent;       // Total bytes sent
    uint64_t uptime_ms;              // Telemetry service uptime
    uint32_t error_count;            // Number of errors
} api_telemetry_stats_t;

// Telemetry Management Functions

/**
 * @brief Initialize telemetry management
 * @param config Telemetry stream configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_init(const api_telemetry_stream_config_t *config);

/**
 * @brief Deinitialize telemetry management
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_deinit(void);

/**
 * @brief Start telemetry streaming
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_start_streaming(void);

/**
 * @brief Stop telemetry streaming
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_stop_streaming(void);

/**
 * @brief Add telemetry client
 * @param socket_fd Client socket file descriptor
 * @param client_id Pointer to client ID (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_add_client(int socket_fd, uint32_t *client_id);

/**
 * @brief Remove telemetry client
 * @param client_id Client identifier
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_remove_client(uint32_t client_id);

/**
 * @brief Subscribe client to telemetry types
 * @param client_id Client identifier
 * @param types Array of telemetry types
 * @param count Number of types
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_subscribe_client(uint32_t client_id, const api_telemetry_type_t *types, uint32_t count);

/**
 * @brief Unsubscribe client from telemetry types
 * @param client_id Client identifier
 * @param types Array of telemetry types
 * @param count Number of types
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_unsubscribe_client(uint32_t client_id, const api_telemetry_type_t *types, uint32_t count);

/**
 * @brief Broadcast telemetry data to all subscribed clients
 * @param data Telemetry data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_broadcast_data(const api_telemetry_data_t *data);

/**
 * @brief Send telemetry data to specific client
 * @param client_id Client identifier
 * @param data Telemetry data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_send_to_client(uint32_t client_id, const api_telemetry_data_t *data);

/**
 * @brief Get telemetry statistics
 * @param stats Telemetry statistics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_get_stats(api_telemetry_stats_t *stats);

/**
 * @brief Get telemetry configuration
 * @param config Telemetry stream configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_get_config(api_telemetry_stream_config_t *config);

/**
 * @brief Set telemetry configuration
 * @param config Telemetry stream configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_set_config(const api_telemetry_stream_config_t *config);

/**
 * @brief Get telemetry client information
 * @param client_id Client identifier
 * @param client_info Client information structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_get_client_info(uint32_t client_id, api_telemetry_client_t *client_info);

/**
 * @brief Get all telemetry clients
 * @param clients Array of client information structures
 * @param max_clients Maximum number of clients
 * @param actual_count Actual number of clients returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_get_all_clients(api_telemetry_client_t *clients, uint32_t max_clients, uint32_t *actual_count);

// Telemetry Data Collection Functions

/**
 * @brief Collect system telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_system_data(api_telemetry_data_t *data);

/**
 * @brief Collect safety telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_safety_data(api_telemetry_data_t *data);

/**
 * @brief Collect motor telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_motor_data(api_telemetry_data_t *data);

/**
 * @brief Collect power telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_power_data(api_telemetry_data_t *data);

/**
 * @brief Collect dock telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_dock_data(api_telemetry_data_t *data);

/**
 * @brief Collect network telemetry data
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_collect_network_data(api_telemetry_data_t *data);

// Telemetry Endpoint Handlers

/**
 * @brief Handle GET /api/v1/telemetry/stats - Get telemetry statistics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_stats(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/telemetry/clients - Get telemetry clients
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_clients(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/telemetry/subscribe - Subscribe to telemetry types
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_subscribe(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/telemetry/unsubscribe - Unsubscribe from telemetry types
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_unsubscribe(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/telemetry/start - Start telemetry streaming
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_start(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/telemetry/stop - Stop telemetry streaming
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_telemetry_stop(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

// JSON Serialization Functions

/**
 * @brief Serialize telemetry data to JSON
 * @param data Telemetry data
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_telemetry_data_json(const api_telemetry_data_t *data, char *buffer, size_t buffer_size);

/**
 * @brief Serialize telemetry statistics to JSON
 * @param stats Telemetry statistics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_telemetry_stats_json(const api_telemetry_stats_t *stats, char *buffer, size_t buffer_size);

/**
 * @brief Serialize telemetry clients to JSON
 * @param clients Telemetry clients
 * @param count Number of clients
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_telemetry_clients_json(const api_telemetry_client_t *clients, uint32_t count, char *buffer, size_t buffer_size);

// Utility Functions

/**
 * @brief Get telemetry type name
 * @param type Telemetry type
 * @return const char* Type name
 */
const char* api_telemetry_get_type_name(api_telemetry_type_t type);

/**
 * @brief Get telemetry type from string
 * @param type_name Type name string
 * @return api_telemetry_type_t Telemetry type
 */
api_telemetry_type_t api_telemetry_get_type_from_string(const char *type_name);

/**
 * @brief Create telemetry data structure
 * @param type Telemetry type
 * @param source Data source
 * @param json_data JSON data payload
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_create_data(api_telemetry_type_t type, const char *source, const char *json_data, api_telemetry_data_t *data);

/**
 * @brief Validate telemetry data
 * @param data Telemetry data
 * @return hal_status_t HAL_STATUS_OK if valid, error code if invalid
 */
hal_status_t api_telemetry_validate_data(const api_telemetry_data_t *data);

#endif // API_TELEMETRY_H
