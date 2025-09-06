/**
 * @file api_telemetry.c
 * @brief API Telemetry & Real-time Data Management Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.2
 */

#include "api_telemetry.h"
#include "api_endpoints.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// Global variables
static bool g_telemetry_initialized = false;
static bool g_telemetry_streaming = false;
static api_telemetry_stream_config_t g_telemetry_config;
static api_telemetry_client_t g_telemetry_clients[32];  // Max 32 clients
static uint32_t g_telemetry_client_count = 0;
static uint32_t g_next_client_id = 1;
static api_telemetry_stats_t g_telemetry_stats = {0};
static uint64_t g_telemetry_start_time = 0;
static pthread_mutex_t g_telemetry_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Initialize telemetry management
 * @param config Telemetry stream configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_init(const api_telemetry_stream_config_t *config) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Initializing...");
    
    if (g_telemetry_initialized) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Telemetry: Already initialized");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize configuration
    g_telemetry_config = *config;
    
    // Initialize client array
    memset(g_telemetry_clients, 0, sizeof(g_telemetry_clients));
    g_telemetry_client_count = 0;
    g_next_client_id = 1;
    
    // Initialize statistics
    memset(&g_telemetry_stats, 0, sizeof(g_telemetry_stats));
    g_telemetry_start_time = hal_get_timestamp_ms();
    
    g_telemetry_initialized = true;
    g_telemetry_streaming = false;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Initialized with interval %u ms", g_telemetry_config.interval_ms);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize telemetry management
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Deinitializing...");
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Stop streaming
    api_telemetry_stop_streaming();
    
    // Disconnect all clients
    for (uint32_t i = 0; i < g_telemetry_client_count; i++) {
        if (g_telemetry_clients[i].connected) {
            api_telemetry_remove_client(g_telemetry_clients[i].client_id);
        }
    }
    
    g_telemetry_initialized = false;
    g_telemetry_client_count = 0;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Deinitialized");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start telemetry streaming
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_start_streaming(void) {
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_telemetry_streaming) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Telemetry: Already streaming");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    g_telemetry_streaming = true;
    g_telemetry_start_time = hal_get_timestamp_ms();
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Streaming started");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Stop telemetry streaming
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_stop_streaming(void) {
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!g_telemetry_streaming) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Telemetry: Not streaming");
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_telemetry_streaming = false;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Streaming stopped");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Add telemetry client
 * @param socket_fd Client socket file descriptor
 * @param client_id Pointer to client ID (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_add_client(int socket_fd, uint32_t *client_id) {
    if (socket_fd < 0 || client_id == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_telemetry_client_count >= g_telemetry_config.max_clients) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Telemetry: Maximum clients reached");
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    // Find free slot
    uint32_t slot = 0;
    for (uint32_t i = 0; i < 32; i++) {
        if (!g_telemetry_clients[i].connected) {
            slot = i;
            break;
        }
    }
    
    // Initialize client
    g_telemetry_clients[slot].client_id = g_next_client_id++;
    g_telemetry_clients[slot].socket_fd = socket_fd;
    g_telemetry_clients[slot].connected = true;
    g_telemetry_clients[slot].last_activity = hal_get_timestamp_ms();
    g_telemetry_clients[slot].messages_sent = 0;
    g_telemetry_clients[slot].messages_failed = 0;
    g_telemetry_clients[slot].subscribed_count = 0;
    
    *client_id = g_telemetry_clients[slot].client_id;
    g_telemetry_client_count++;
    
    // Update statistics
    g_telemetry_stats.total_clients++;
    g_telemetry_stats.active_clients = g_telemetry_client_count;
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Client %u added", *client_id);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Remove telemetry client
 * @param client_id Client identifier
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_remove_client(uint32_t client_id) {
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    // Find client
    for (uint32_t i = 0; i < 32; i++) {
        if (g_telemetry_clients[i].client_id == client_id && g_telemetry_clients[i].connected) {
            g_telemetry_clients[i].connected = false;
            g_telemetry_clients[i].socket_fd = -1;
            g_telemetry_clients[i].subscribed_count = 0;
            
            g_telemetry_client_count--;
            g_telemetry_stats.active_clients = g_telemetry_client_count;
            
            pthread_mutex_unlock(&g_telemetry_mutex);
            
            hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Client %u removed", client_id);
            
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Subscribe client to telemetry types
 * @param client_id Client identifier
 * @param types Array of telemetry types
 * @param count Number of types
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_subscribe_client(uint32_t client_id, const api_telemetry_type_t *types, uint32_t count) {
    if (types == NULL || count == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    // Find client
    for (uint32_t i = 0; i < 32; i++) {
        if (g_telemetry_clients[i].client_id == client_id && g_telemetry_clients[i].connected) {
            // Add subscription types
            for (uint32_t j = 0; j < count && g_telemetry_clients[i].subscribed_count < API_TELEMETRY_TYPE_MAX; j++) {
                g_telemetry_clients[i].subscribed_types[g_telemetry_clients[i].subscribed_count] = types[j];
                g_telemetry_clients[i].subscribed_count++;
            }
            
            pthread_mutex_unlock(&g_telemetry_mutex);
            
            hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Client %u subscribed to %u types", client_id, count);
            
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Unsubscribe client from telemetry types
 * @param client_id Client identifier
 * @param types Array of telemetry types
 * @param count Number of types
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_unsubscribe_client(uint32_t client_id, const api_telemetry_type_t *types, uint32_t count) {
    if (types == NULL || count == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    // Find client
    for (uint32_t i = 0; i < 32; i++) {
        if (g_telemetry_clients[i].client_id == client_id && g_telemetry_clients[i].connected) {
            // Remove subscription types
            for (uint32_t j = 0; j < count; j++) {
                for (uint32_t k = 0; k < g_telemetry_clients[i].subscribed_count; k++) {
                    if (g_telemetry_clients[i].subscribed_types[k] == types[j]) {
                        // Shift remaining types
                        for (uint32_t l = k; l < g_telemetry_clients[i].subscribed_count - 1; l++) {
                            g_telemetry_clients[i].subscribed_types[l] = g_telemetry_clients[i].subscribed_types[l + 1];
                        }
                        g_telemetry_clients[i].subscribed_count--;
                        break;
                    }
                }
            }
            
            pthread_mutex_unlock(&g_telemetry_mutex);
            
            hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Client %u unsubscribed from %u types", client_id, count);
            
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Broadcast telemetry data to all subscribed clients
 * @param data Telemetry data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_broadcast_data(const api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized || !g_telemetry_streaming) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    uint32_t sent_count = 0;
    uint32_t failed_count = 0;
    
    // Send to all subscribed clients
    for (uint32_t i = 0; i < 32; i++) {
        if (g_telemetry_clients[i].connected) {
            // Check if client is subscribed to this data type
            bool subscribed = false;
            for (uint32_t j = 0; j < g_telemetry_clients[i].subscribed_count; j++) {
                if (g_telemetry_clients[i].subscribed_types[j] == data->type) {
                    subscribed = true;
                    break;
                }
            }
            
            if (subscribed) {
                hal_status_t result = api_telemetry_send_to_client(g_telemetry_clients[i].client_id, data);
                if (result == HAL_STATUS_OK) {
                    sent_count++;
                    g_telemetry_clients[i].messages_sent++;
                } else {
                    failed_count++;
                    g_telemetry_clients[i].messages_failed++;
                }
            }
        }
    }
    
    // Update statistics
    g_telemetry_stats.total_messages_sent += sent_count;
    g_telemetry_stats.total_messages_failed += failed_count;
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    if (sent_count > 0) {
        hal_log_message(HAL_LOG_LEVEL_DEBUG, "API Telemetry: Broadcasted to %u clients (%u failed)", sent_count, failed_count);
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Send telemetry data to specific client
 * @param client_id Client identifier
 * @param data Telemetry data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_send_to_client(uint32_t client_id, const api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    // Find client
    for (uint32_t i = 0; i < 32; i++) {
        if (g_telemetry_clients[i].client_id == client_id && g_telemetry_clients[i].connected) {
            // Serialize data to JSON
            char json_buffer[1024];
            hal_status_t json_result = api_serialize_telemetry_data_json(data, json_buffer, sizeof(json_buffer));
            
            if (json_result != HAL_STATUS_OK) {
                pthread_mutex_unlock(&g_telemetry_mutex);
                return json_result;
            }
            
            // Send via WebSocket
            hal_status_t ws_result = api_manager_broadcast_websocket_message(json_buffer, strlen(json_buffer));
            
            if (ws_result == HAL_STATUS_OK) {
                g_telemetry_clients[i].last_activity = hal_get_timestamp_ms();
                g_telemetry_stats.total_bytes_sent += strlen(json_buffer);
            }
            
            pthread_mutex_unlock(&g_telemetry_mutex);
            
            return ws_result;
        }
    }
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Get telemetry statistics
 * @param stats Telemetry statistics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_get_stats(api_telemetry_stats_t *stats) {
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    *stats = g_telemetry_stats;
    stats->uptime_ms = hal_get_timestamp_ms() - g_telemetry_start_time;
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get telemetry configuration
 * @param config Telemetry stream configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_get_config(api_telemetry_stream_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *config = g_telemetry_config;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set telemetry configuration
 * @param config Telemetry stream configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_set_config(const api_telemetry_stream_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_telemetry_config = *config;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Telemetry: Configuration updated");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get telemetry client information
 * @param client_id Client identifier
 * @param client_info Client information structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_get_client_info(uint32_t client_id, api_telemetry_client_t *client_info) {
    if (client_info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    // Find client
    for (uint32_t i = 0; i < 32; i++) {
        if (g_telemetry_clients[i].client_id == client_id && g_telemetry_clients[i].connected) {
            *client_info = g_telemetry_clients[i];
            pthread_mutex_unlock(&g_telemetry_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Get all telemetry clients
 * @param clients Array of client information structures
 * @param max_clients Maximum number of clients
 * @param actual_count Actual number of clients returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_get_all_clients(api_telemetry_client_t *clients, uint32_t max_clients, uint32_t *actual_count) {
    if (clients == NULL || actual_count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_telemetry_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_telemetry_mutex);
    
    *actual_count = 0;
    
    for (uint32_t i = 0; i < 32 && *actual_count < max_clients; i++) {
        if (g_telemetry_clients[i].connected) {
            clients[*actual_count] = g_telemetry_clients[i];
            (*actual_count)++;
        }
    }
    
    pthread_mutex_unlock(&g_telemetry_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get telemetry type name
 * @param type Telemetry type
 * @return const char* Type name
 */
const char* api_telemetry_get_type_name(api_telemetry_type_t type) {
    switch (type) {
        case API_TELEMETRY_TYPE_SYSTEM: return "system";
        case API_TELEMETRY_TYPE_SAFETY: return "safety";
        case API_TELEMETRY_TYPE_MOTOR: return "motor";
        case API_TELEMETRY_TYPE_POWER: return "power";
        case API_TELEMETRY_TYPE_DOCK: return "dock";
        case API_TELEMETRY_TYPE_NETWORK: return "network";
        default: return "unknown";
    }
}

/**
 * @brief Get telemetry type from string
 * @param type_name Type name string
 * @return api_telemetry_type_t Telemetry type
 */
api_telemetry_type_t api_telemetry_get_type_from_string(const char *type_name) {
    if (type_name == NULL) {
        return API_TELEMETRY_TYPE_MAX;
    }
    
    for (int i = 0; i < API_TELEMETRY_TYPE_MAX; i++) {
        if (strcmp(api_telemetry_get_type_name((api_telemetry_type_t)i), type_name) == 0) {
            return (api_telemetry_type_t)i;
        }
    }
    
    return API_TELEMETRY_TYPE_MAX;
}

/**
 * @brief Create telemetry data structure
 * @param type Telemetry type
 * @param source Data source
 * @param json_data JSON data payload
 * @param data Telemetry data structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_telemetry_create_data(api_telemetry_type_t type, const char *source, const char *json_data, api_telemetry_data_t *data) {
    if (source == NULL || json_data == NULL || data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (type >= API_TELEMETRY_TYPE_MAX) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    data->timestamp = hal_get_timestamp_ms();
    data->type = type;
    strncpy(data->source, source, sizeof(data->source) - 1);
    data->source[sizeof(data->source) - 1] = '\0';
    
    size_t json_len = strlen(json_data);
    if (json_len >= sizeof(data->data)) {
        return HAL_STATUS_ERROR;
    }
    
    strncpy(data->data, json_data, sizeof(data->data) - 1);
    data->data[sizeof(data->data) - 1] = '\0';
    data->data_size = json_len;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate telemetry data
 * @param data Telemetry data
 * @return hal_status_t HAL_STATUS_OK if valid, error code if invalid
 */
hal_status_t api_telemetry_validate_data(const api_telemetry_data_t *data) {
    if (data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (data->type >= API_TELEMETRY_TYPE_MAX) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (strlen(data->source) == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (strlen(data->data) == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (data->data_size == 0 || data->data_size >= sizeof(data->data)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}
