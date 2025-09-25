/**
 * @file websocket_integration.c
 * @brief WebSocket Integration Wrapper Implementation for OHT-50 Master Module
 * @version 2.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket System Overhaul - Issue #153)
 * 
 * This implementation provides seamless integration between the old and new WebSocket systems,
 * allowing for gradual migration and fallback capabilities.
 */

#include "websocket_integration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// Global Integration Instance
ws_integration_config_t g_ws_integration_config = {0};
ws_integration_stats_t g_ws_integration_stats = {0};

// Private variables
static bool g_ws_integration_initialized = false;
static bool g_ws_integration_running = false;
static pthread_mutex_t g_ws_integration_mutex = PTHREAD_MUTEX_INITIALIZER;
static ws_integration_mode_t g_ws_current_mode = WS_INTEGRATION_MODE_ORIGINAL;

// Private function declarations
static hal_status_t ws_integration_initialize_defaults(ws_integration_config_t *config);
static hal_status_t ws_integration_validate_config(const ws_integration_config_t *config);
static hal_status_t ws_integration_start_original_system(void);
static hal_status_t ws_integration_start_libwebsockets_system(void);
static hal_status_t ws_integration_stop_original_system(void);
static hal_status_t ws_integration_stop_libwebsockets_system(void);
static hal_status_t ws_integration_cleanup_original_system(void);
static hal_status_t ws_integration_cleanup_libwebsockets_system(void);
static void ws_integration_log_event(hal_log_level_t level, const char *format, ...);
static hal_status_t ws_integration_handle_error(int error_code, const char *error_message);

/**
 * @brief Initialize WebSocket integration system
 */
hal_status_t ws_integration_init(const ws_integration_config_t *config) {
    ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Initializing...");
    
    // Check if already initialized
    if (g_ws_integration_initialized) {
        ws_integration_log_event(HAL_LOG_LEVEL_WARNING, "WebSocket Integration: Already initialized");
        return HAL_STATUS_OK;
    }
    
    // Validate input parameters
    if (config == NULL) {
        ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: Config parameter is NULL");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize default values for any missing fields
    ws_integration_config_t config_copy = *config;
    hal_status_t defaults_result = ws_integration_initialize_defaults(&config_copy);
    if (defaults_result != HAL_STATUS_OK) {
        ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: Failed to initialize default values");
        return defaults_result;
    }
    
    // Validate configuration
    hal_status_t validation_result = ws_integration_validate_config(&config_copy);
    if (validation_result != HAL_STATUS_OK) {
        ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: Configuration validation failed");
        return validation_result;
    }
    
    // Copy configuration
    g_ws_integration_config = config_copy;
    
    // Initialize statistics
    memset(&g_ws_integration_stats, 0, sizeof(g_ws_integration_stats));
    g_ws_integration_stats.last_activity = time(NULL);
    g_ws_integration_stats.system_healthy = true;
    
    // Set current mode
    g_ws_current_mode = g_ws_integration_config.mode;
    
    g_ws_integration_initialized = true;
    ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Initialization completed successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start WebSocket integration system
 */
hal_status_t ws_integration_start(void) {
    ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Starting...");
    
    // Check if initialized
    if (!g_ws_integration_initialized) {
        ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: Not initialized");
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check if already running
    if (g_ws_integration_running) {
        ws_integration_log_event(HAL_LOG_LEVEL_WARNING, "WebSocket Integration: Already running");
        return HAL_STATUS_OK;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    hal_status_t result = HAL_STATUS_OK;
    
    // Start appropriate system based on mode
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_ORIGINAL:
            result = ws_integration_start_original_system();
            if (result == HAL_STATUS_OK) {
                g_ws_integration_stats.original_connections++;
            }
            break;
            
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            result = ws_integration_start_libwebsockets_system();
            if (result == HAL_STATUS_OK) {
                g_ws_integration_stats.libwebsockets_connections++;
            }
            break;
            
        case WS_INTEGRATION_MODE_AUTO:
            // Try libwebsockets first, fallback to original
            result = ws_integration_start_libwebsockets_system();
            if (result == HAL_STATUS_OK) {
                g_ws_current_mode = WS_INTEGRATION_MODE_LIBWEBSOCKETS;
                g_ws_integration_stats.libwebsockets_connections++;
            } else if (g_ws_integration_config.enable_fallback) {
                ws_integration_log_event(HAL_LOG_LEVEL_WARNING, "WebSocket Integration: libwebsockets failed, falling back to original system");
                result = ws_integration_start_original_system();
                if (result == HAL_STATUS_OK) {
                    g_ws_current_mode = WS_INTEGRATION_MODE_ORIGINAL;
                    g_ws_integration_stats.original_connections++;
                    g_ws_integration_stats.fallback_events++;
                }
            }
            break;
            
        default:
            ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: Invalid mode: %d", g_ws_current_mode);
            result = HAL_STATUS_INVALID_PARAMETER;
            break;
    }
    
    if (result == HAL_STATUS_OK) {
        g_ws_integration_running = true;
        g_ws_integration_stats.last_activity = time(NULL);
        ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Started successfully in mode %d", g_ws_current_mode);
    } else {
        ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: Failed to start");
        g_ws_integration_stats.error_events++;
    }
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    return result;
}

/**
 * @brief Stop WebSocket integration system
 */
hal_status_t ws_integration_stop(void) {
    ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Stopping...");
    
    // Check if running
    if (!g_ws_integration_running) {
        ws_integration_log_event(HAL_LOG_LEVEL_WARNING, "WebSocket Integration: Not running");
        return HAL_STATUS_OK;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    hal_status_t result = HAL_STATUS_OK;
    
    // Stop appropriate system based on current mode
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_ORIGINAL:
            result = ws_integration_stop_original_system();
            break;
            
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            result = ws_integration_stop_libwebsockets_system();
            break;
            
        default:
            ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: Invalid mode: %d", g_ws_current_mode);
            result = HAL_STATUS_INVALID_PARAMETER;
            break;
    }
    
    if (result == HAL_STATUS_OK) {
        g_ws_integration_running = false;
        g_ws_integration_stats.last_activity = time(NULL);
        ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Stopped successfully");
    } else {
        ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: Failed to stop");
        g_ws_integration_stats.error_events++;
    }
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    return result;
}

/**
 * @brief Cleanup WebSocket integration system
 */
hal_status_t ws_integration_cleanup(void) {
    ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Cleaning up...");
    
    // Stop system if running
    if (g_ws_integration_running) {
        ws_integration_stop();
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    // Cleanup appropriate system based on current mode
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_ORIGINAL:
            ws_integration_cleanup_original_system();
            break;
            
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            ws_integration_cleanup_libwebsockets_system();
            break;
            
        default:
            break;
    }
    
    // Reset state
    g_ws_integration_initialized = false;
    g_ws_integration_running = false;
    g_ws_current_mode = WS_INTEGRATION_MODE_ORIGINAL;
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    
    ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Cleanup completed");
    return HAL_STATUS_OK;
}

/**
 * @brief Send message using integration system
 */
hal_status_t ws_integration_send_message(uint32_t client_id, const void *message) {
    if (message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_integration_running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    hal_status_t result = HAL_STATUS_OK;
    
    // Send message using appropriate system
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_ORIGINAL:
            result = ws_server_send_message(client_id, (const ws_message_t *)message);
            break;
            
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            result = ws_lws_send_message(client_id, (const ws_lws_message_t *)message);
            break;
            
        default:
            result = HAL_STATUS_INVALID_PARAMETER;
            break;
    }
    
    if (result != HAL_STATUS_OK) {
        g_ws_integration_stats.error_events++;
    }
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    return result;
}

/**
 * @brief Broadcast message using integration system
 */
hal_status_t ws_integration_broadcast_message(const void *message) {
    if (message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_ws_integration_running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    hal_status_t result = HAL_STATUS_OK;
    
    // Broadcast message using appropriate system
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_ORIGINAL:
            result = ws_server_broadcast_message((const ws_message_t *)message);
            break;
            
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            result = ws_lws_broadcast_message((const ws_lws_message_t *)message);
            break;
            
        default:
            result = HAL_STATUS_INVALID_PARAMETER;
            break;
    }
    
    if (result != HAL_STATUS_OK) {
        g_ws_integration_stats.error_events++;
    }
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    return result;
}

/**
 * @brief Get integration statistics
 */
hal_status_t ws_integration_get_stats(ws_integration_stats_t *stats) {
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    *stats = g_ws_integration_stats;
    pthread_mutex_unlock(&g_ws_integration_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Check if integration system is running
 */
bool ws_integration_is_running(void) {
    return g_ws_integration_running;
}

/**
 * @brief Get connected client count
 */
uint32_t ws_integration_get_client_count(void) {
    if (!g_ws_integration_running) {
        return 0;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    uint32_t count = 0;
    
    // Get client count from appropriate system
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_ORIGINAL:
            count = ws_server_get_client_count();
            break;
            
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            count = ws_lws_get_client_count();
            break;
            
        default:
            count = 0;
            break;
    }
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    return count;
}

/**
 * @brief Switch to libwebsockets system
 */
hal_status_t ws_integration_switch_to_libwebsockets(void) {
    ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Switching to libwebsockets system...");
    
    if (!g_ws_integration_running) {
        ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: System not running");
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    hal_status_t result = HAL_STATUS_OK;
    
    // Stop current system
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_ORIGINAL:
            result = ws_integration_stop_original_system();
            break;
            
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            // Already using libwebsockets
            pthread_mutex_unlock(&g_ws_integration_mutex);
            return HAL_STATUS_OK;
            
        default:
            result = HAL_STATUS_INVALID_PARAMETER;
            break;
    }
    
    if (result == HAL_STATUS_OK) {
        // Start libwebsockets system
        result = ws_integration_start_libwebsockets_system();
        if (result == HAL_STATUS_OK) {
            g_ws_current_mode = WS_INTEGRATION_MODE_LIBWEBSOCKETS;
            g_ws_integration_stats.libwebsockets_connections++;
            g_ws_integration_stats.migration_events++;
            ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Successfully switched to libwebsockets system");
        } else {
            // Fallback to original system if enabled
            if (g_ws_integration_config.enable_fallback) {
                ws_integration_log_event(HAL_LOG_LEVEL_WARNING, "WebSocket Integration: libwebsockets failed, falling back to original system");
                result = ws_integration_start_original_system();
                if (result == HAL_STATUS_OK) {
                    g_ws_current_mode = WS_INTEGRATION_MODE_ORIGINAL;
                    g_ws_integration_stats.original_connections++;
                    g_ws_integration_stats.fallback_events++;
                }
            }
        }
    }
    
    if (result != HAL_STATUS_OK) {
        g_ws_integration_stats.error_events++;
    }
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    return result;
}

/**
 * @brief Switch to original system
 */
hal_status_t ws_integration_switch_to_original(void) {
    ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Switching to original system...");
    
    if (!g_ws_integration_running) {
        ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "WebSocket Integration: System not running");
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    hal_status_t result = HAL_STATUS_OK;
    
    // Stop current system
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            result = ws_integration_stop_libwebsockets_system();
            break;
            
        case WS_INTEGRATION_MODE_ORIGINAL:
            // Already using original system
            pthread_mutex_unlock(&g_ws_integration_mutex);
            return HAL_STATUS_OK;
            
        default:
            result = HAL_STATUS_INVALID_PARAMETER;
            break;
    }
    
    if (result == HAL_STATUS_OK) {
        // Start original system
        result = ws_integration_start_original_system();
        if (result == HAL_STATUS_OK) {
            g_ws_current_mode = WS_INTEGRATION_MODE_ORIGINAL;
            g_ws_integration_stats.original_connections++;
            g_ws_integration_stats.migration_events++;
            ws_integration_log_event(HAL_LOG_LEVEL_INFO, "WebSocket Integration: Successfully switched to original system");
        }
    }
    
    if (result != HAL_STATUS_OK) {
        g_ws_integration_stats.error_events++;
    }
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    return result;
}

/**
 * @brief Perform system health check
 */
hal_status_t ws_integration_health_check(void) {
    if (!g_ws_integration_running) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_ws_integration_mutex);
    
    hal_status_t result = HAL_STATUS_OK;
    
    // Perform health check on appropriate system
    switch (g_ws_current_mode) {
        case WS_INTEGRATION_MODE_ORIGINAL:
            // Check if original system is running
            if (!ws_server_is_running()) {
                result = HAL_STATUS_ERROR;
            }
            break;
            
        case WS_INTEGRATION_MODE_LIBWEBSOCKETS:
            // Check if libwebsockets system is running
            if (!ws_lws_server_is_running()) {
                result = HAL_STATUS_ERROR;
            }
            break;
            
        default:
            result = HAL_STATUS_INVALID_PARAMETER;
            break;
    }
    
    g_ws_integration_stats.system_healthy = (result == HAL_STATUS_OK);
    g_ws_integration_stats.last_activity = time(NULL);
    
    if (result != HAL_STATUS_OK) {
        g_ws_integration_stats.error_events++;
    }
    
    pthread_mutex_unlock(&g_ws_integration_mutex);
    return result;
}

// Private function implementations

/**
 * @brief Initialize default configuration
 */
static hal_status_t ws_integration_initialize_defaults(ws_integration_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Set defaults for missing fields
    if (config->mode == 0) { // Assuming 0 is uninitialized
        config->mode = WS_INTEGRATION_MODE_AUTO;
    }
    if (config->migration_timeout_sec == 0) {
        config->migration_timeout_sec = 30;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate configuration
 */
static hal_status_t ws_integration_validate_config(const ws_integration_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->mode > WS_INTEGRATION_MODE_AUTO) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start original WebSocket system
 */
static hal_status_t ws_integration_start_original_system(void) {
    // Initialize original WebSocket system with default config
    ws_server_config_t original_config = {0};
    original_config.port = 8080;
    original_config.max_clients = 10;
    original_config.max_message_size = 4096;
    
    hal_status_t result = ws_server_init(&original_config);
    if (result == HAL_STATUS_OK) {
        result = ws_server_start();
    }
    
    return result;
}

/**
 * @brief Start libwebsockets system
 */
static hal_status_t ws_integration_start_libwebsockets_system(void) {
    // Initialize libwebsockets system with default config
    ws_lws_config_t lws_config = {0};
    lws_config.port = 8080;
    lws_config.max_clients = 10;
    lws_config.max_message_size = 4096;
    lws_config.ping_interval_sec = 30;
    lws_config.pong_timeout_sec = 10;
    lws_config.handshake_timeout_sec = 5;
    
    hal_status_t result = ws_lws_server_init(&lws_config);
    if (result == HAL_STATUS_OK) {
        result = ws_lws_server_start();
    }
    
    return result;
}

/**
 * @brief Stop original WebSocket system
 */
static hal_status_t ws_integration_stop_original_system(void) {
    hal_status_t result = ws_server_stop();
    if (result == HAL_STATUS_OK) {
        ws_server_cleanup();
    }
    return result;
}

/**
 * @brief Stop libwebsockets system
 */
static hal_status_t ws_integration_stop_libwebsockets_system(void) {
    hal_status_t result = ws_lws_server_stop();
    if (result == HAL_STATUS_OK) {
        ws_lws_server_cleanup();
    }
    return result;
}

/**
 * @brief Cleanup original WebSocket system
 */
static hal_status_t ws_integration_cleanup_original_system(void) {
    return ws_server_cleanup();
}

/**
 * @brief Cleanup libwebsockets system
 */
static hal_status_t ws_integration_cleanup_libwebsockets_system(void) {
    return ws_lws_server_cleanup();
}

/**
 * @brief Log integration event
 */
static void ws_integration_log_event(hal_log_level_t level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    char message[512];
    vsnprintf(message, sizeof(message), format, args);
    
    hal_log_message(level, "[WS_INTEGRATION] %s", message);
    
    va_end(args);
}

/**
 * @brief Handle integration error
 */
static hal_status_t ws_integration_handle_error(int error_code, const char *error_message) {
    ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "Error %d: %s", error_code, error_message);
    g_ws_integration_stats.error_events++;
    return HAL_STATUS_ERROR;
}

// Compatibility function implementations

hal_status_t ws_server_init_compat(const ws_server_config_t *config) {
    ws_integration_config_t integration_config = {0};
    integration_config.mode = WS_INTEGRATION_MODE_ORIGINAL;
    integration_config.enable_fallback = true;
    
    hal_status_t result = ws_integration_init(&integration_config);
    if (result == HAL_STATUS_OK) {
        result = ws_integration_start();
    }
    
    return result;
}

hal_status_t ws_server_start_compat(void) {
    return ws_integration_start();
}

hal_status_t ws_server_stop_compat(void) {
    return ws_integration_stop();
}

hal_status_t ws_server_cleanup_compat(void) {
    return ws_integration_cleanup();
}

hal_status_t ws_server_send_message_compat(uint32_t client_id, const ws_message_t *message) {
    return ws_integration_send_message(client_id, message);
}

hal_status_t ws_server_broadcast_message_compat(const ws_message_t *message) {
    return ws_integration_broadcast_message(message);
}

// Stub implementations for remaining functions
hal_status_t ws_integration_get_performance_metrics(void *metrics) {
    (void)metrics;
    return HAL_STATUS_NOT_IMPLEMENTED;
}

hal_status_t ws_integration_start_migration(void) {
    return ws_integration_switch_to_libwebsockets();
}

hal_status_t ws_integration_complete_migration(void) {
    return ws_integration_switch_to_libwebsockets();
}

hal_status_t ws_integration_rollback_migration(void) {
    return ws_integration_switch_to_original();
}

ws_integration_mode_t ws_integration_get_migration_status(void) {
    return g_ws_current_mode;
}

hal_status_t ws_integration_init_default_config(ws_integration_config_t *config) {
    return ws_integration_initialize_defaults(config);
}

hal_status_t ws_integration_validate_config(const ws_integration_config_t *config) {
    return ws_integration_validate_config(config);
}

void ws_integration_log_event(hal_log_level_t level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    char message[512];
    vsnprintf(message, sizeof(message), format, args);
    
    hal_log_message(level, "[WS_INTEGRATION] %s", message);
    
    va_end(args);
}

hal_status_t ws_integration_handle_error(int error_code, const char *error_message) {
    ws_integration_log_event(HAL_LOG_LEVEL_ERROR, "Error %d: %s", error_code, error_message);
    g_ws_integration_stats.error_events++;
    return HAL_STATUS_ERROR;
}

hal_status_t ws_integration_start_performance_monitoring(void) {
    return HAL_STATUS_NOT_IMPLEMENTED;
}

hal_status_t ws_integration_stop_performance_monitoring(void) {
    return HAL_STATUS_NOT_IMPLEMENTED;
}

hal_status_t ws_integration_get_performance_report(void *report) {
    (void)report;
    return HAL_STATUS_NOT_IMPLEMENTED;
}
