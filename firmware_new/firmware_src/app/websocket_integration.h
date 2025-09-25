/**
 * @file websocket_integration.h
 * @brief WebSocket Integration Wrapper for OHT-50 Master Module
 * @version 2.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket System Overhaul - Issue #153)
 * 
 * This file provides a seamless integration wrapper between the old WebSocket system
 * and the new libwebsockets-based system, allowing for gradual migration.
 */

#ifndef WEBSOCKET_INTEGRATION_H
#define WEBSOCKET_INTEGRATION_H

#include "hal/hal_common.h"
#include "websocket_server.h"      // Original WebSocket system
#include "websocket_server_lws.h"  // New libwebsockets system

// Integration Configuration
typedef enum {
    WS_INTEGRATION_MODE_ORIGINAL = 0,    // Use original WebSocket system
    WS_INTEGRATION_MODE_LIBWEBSOCKETS = 1, // Use libwebsockets system
    WS_INTEGRATION_MODE_AUTO = 2         // Auto-detect best system
} ws_integration_mode_t;

// Integration Configuration Structure
typedef struct {
    ws_integration_mode_t mode;          // Integration mode
    bool enable_fallback;                // Enable fallback to original system
    bool enable_gradual_migration;       // Enable gradual migration
    uint32_t migration_timeout_sec;      // Migration timeout
    bool enable_performance_monitoring;  // Enable performance monitoring
    bool enable_compatibility_mode;      // Enable compatibility mode
} ws_integration_config_t;

// Integration Statistics
typedef struct {
    uint64_t original_connections;       // Original system connections
    uint64_t libwebsockets_connections;  // libwebsockets system connections
    uint64_t fallback_events;           // Fallback events
    uint64_t migration_events;          // Migration events
    uint64_t error_events;              // Error events
    time_t last_activity;               // Last activity time
    bool system_healthy;                // System health status
} ws_integration_stats_t;

// Global Integration Instance
extern ws_integration_config_t g_ws_integration_config;
extern ws_integration_stats_t g_ws_integration_stats;

// Integration API Functions

/**
 * @brief Initialize WebSocket integration system
 * @param config Pointer to integration configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_init(const ws_integration_config_t *config);

/**
 * @brief Start WebSocket integration system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_start(void);

/**
 * @brief Stop WebSocket integration system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_stop(void);

/**
 * @brief Cleanup WebSocket integration system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_cleanup(void);

/**
 * @brief Send message using integration system
 * @param client_id Client ID
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_send_message(uint32_t client_id, const void *message);

/**
 * @brief Broadcast message using integration system
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_broadcast_message(const void *message);

/**
 * @brief Get integration statistics
 * @param stats Pointer to statistics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_get_stats(ws_integration_stats_t *stats);

/**
 * @brief Check if integration system is running
 * @return bool true if running, false otherwise
 */
bool ws_integration_is_running(void);

/**
 * @brief Get connected client count
 * @return uint32_t Number of connected clients
 */
uint32_t ws_integration_get_client_count(void);

/**
 * @brief Switch to libwebsockets system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_switch_to_libwebsockets(void);

/**
 * @brief Switch to original system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_switch_to_original(void);

/**
 * @brief Perform system health check
 * @return hal_status_t HAL_STATUS_OK if healthy, error code if unhealthy
 */
hal_status_t ws_integration_health_check(void);

/**
 * @brief Get system performance metrics
 * @param metrics Pointer to metrics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_get_performance_metrics(void *metrics);

// Compatibility Functions (for backward compatibility)

/**
 * @brief Initialize WebSocket server (compatibility wrapper)
 * @param config Pointer to original configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_init_compat(const ws_server_config_t *config);

/**
 * @brief Start WebSocket server (compatibility wrapper)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_start_compat(void);

/**
 * @brief Stop WebSocket server (compatibility wrapper)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_stop_compat(void);

/**
 * @brief Cleanup WebSocket server (compatibility wrapper)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_cleanup_compat(void);

/**
 * @brief Send message (compatibility wrapper)
 * @param client_id Client ID
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_send_message_compat(uint32_t client_id, const ws_message_t *message);

/**
 * @brief Broadcast message (compatibility wrapper)
 * @param message Pointer to message structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_server_broadcast_message_compat(const ws_message_t *message);

// Migration Functions

/**
 * @brief Start gradual migration to libwebsockets
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_start_migration(void);

/**
 * @brief Complete migration to libwebsockets
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_complete_migration(void);

/**
 * @brief Rollback migration to original system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_rollback_migration(void);

/**
 * @brief Get migration status
 * @return ws_integration_mode_t Current migration status
 */
ws_integration_mode_t ws_integration_get_migration_status(void);

// Utility Functions

/**
 * @brief Initialize default integration configuration
 * @param config Pointer to configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_init_default_config(ws_integration_config_t *config);

/**
 * @brief Validate integration configuration
 * @param config Pointer to configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_validate_config(const ws_integration_config_t *config);

/**
 * @brief Log integration event
 * @param level Log level
 * @param format Log format string
 * @param ... Variable arguments
 */
void ws_integration_log_event(hal_log_level_t level, const char *format, ...);

/**
 * @brief Handle integration error
 * @param error_code Error code
 * @param error_message Error message
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_handle_error(int error_code, const char *error_message);

// Performance Monitoring

/**
 * @brief Start performance monitoring
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_start_performance_monitoring(void);

/**
 * @brief Stop performance monitoring
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_stop_performance_monitoring(void);

/**
 * @brief Get performance report
 * @param report Pointer to report structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t ws_integration_get_performance_report(void *report);

#endif // WEBSOCKET_INTEGRATION_H
