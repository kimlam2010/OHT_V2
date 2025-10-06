/**
 * @file fallback_manager.h
 * @brief Fallback Manager - OHT-50 Robot Network Fallback Management
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Fallback Manager cung cấp quản lý fallback connectivity cho OHT-50 robots
 * bao gồm auto-fallback to AP mode khi WiFi connection fails.
 */

#ifndef FALLBACK_MANAGER_H
#define FALLBACK_MANAGER_H

#include "hal_common.h"
#include "wifi_manager.h"
#include "wifi_ap_manager.h"
#include <stdint.h>
#include <stdbool.h>

// Fallback Manager Error Codes
typedef enum {
    FALLBACK_SUCCESS = 0,
    FALLBACK_ERROR_INVALID_PARAM = -1,
    FALLBACK_ERROR_INIT_FAILED = -2,
    FALLBACK_ERROR_NOT_INITIALIZED = -3,
    FALLBACK_ERROR_ALREADY_RUNNING = -4,
    FALLBACK_ERROR_NOT_RUNNING = -5,
    FALLBACK_ERROR_WIFI_MANAGER_FAILED = -6,
    FALLBACK_ERROR_AP_MANAGER_FAILED = -7,
    FALLBACK_ERROR_TIMEOUT = -8
} fallback_error_t;

// Fallback Status
typedef enum {
    FALLBACK_STATUS_DISABLED = 0,
    FALLBACK_STATUS_MONITORING = 1,
    FALLBACK_STATUS_TRIGGERED = 2,
    FALLBACK_STATUS_RECOVERING = 3,
    FALLBACK_STATUS_ERROR = 4
} fallback_status_t;

// Fallback Configuration
typedef struct {
    bool auto_fallback_enabled;
    uint32_t connection_timeout_ms;
    uint32_t retry_interval_ms;
    int max_retry_attempts;
    bool ap_mode_enabled;
    uint32_t ap_startup_timeout_ms;
    uint32_t recovery_check_interval_ms;
    int signal_strength_threshold_dbm;
    bool aggressive_fallback;
} fallback_config_t;

// Fallback Status Information
typedef struct {
    fallback_status_t status;
    bool auto_fallback_enabled;
    bool wifi_connected;
    bool ap_running;
    int wifi_signal_strength_dbm;
    uint32_t connection_attempts;
    uint32_t fallback_triggers;
    uint32_t recovery_attempts;
    uint32_t last_fallback_time;
    uint32_t last_recovery_time;
    char current_ssid[32];
    char ap_ssid[32];
} fallback_status_info_t;

// Fallback Statistics
typedef struct {
    uint32_t total_fallback_triggers;
    uint32_t total_recovery_attempts;
    uint32_t successful_recoveries;
    uint32_t failed_recoveries;
    uint32_t total_ap_uptime_seconds;
    uint32_t total_wifi_uptime_seconds;
    float fallback_success_rate;
    float recovery_success_rate;
    uint32_t average_fallback_time_ms;
    uint32_t average_recovery_time_ms;
} fallback_statistics_t;

// Fallback Manager API Functions

/**
 * @brief Initialize Fallback Manager
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_init(void);

/**
 * @brief Deinitialize Fallback Manager
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_deinit(void);

/**
 * @brief Start fallback monitoring
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_start_monitoring(void);

/**
 * @brief Stop fallback monitoring
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_stop_monitoring(void);

/**
 * @brief Enable/disable auto-fallback
 * @param enabled Enable auto-fallback
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_enable_auto_fallback(bool enabled);

/**
 * @brief Check WiFi connection status
 * @return FALLBACK_SUCCESS if connected, error code if disconnected
 */
int fallback_manager_check_connection(void);

/**
 * @brief Trigger fallback to AP mode
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_trigger_fallback(void);

/**
 * @brief Attempt recovery to WiFi
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_attempt_recovery(void);

/**
 * @brief Get fallback status
 * @param status Pointer to status structure
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_get_status(fallback_status_info_t *status);

/**
 * @brief Get fallback configuration
 * @param config Pointer to configuration structure
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_get_config(fallback_config_t *config);

/**
 * @brief Set fallback configuration
 * @param config Pointer to configuration structure
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_set_config(const fallback_config_t *config);

/**
 * @brief Get fallback statistics
 * @param stats Pointer to statistics structure
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_get_statistics(fallback_statistics_t *stats);

/**
 * @brief Reset fallback statistics
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_reset_statistics(void);

/**
 * @brief Check if fallback is enabled
 * @return true if enabled, false otherwise
 */
bool fallback_manager_is_enabled(void);

/**
 * @brief Check if monitoring is active
 * @return true if active, false otherwise
 */
bool fallback_manager_is_monitoring(void);

/**
 * @brief Get error message for error code
 * @param error_code Error code
 * @return Error message string
 */
const char* fallback_manager_get_error_message(int error_code);

/**
 * @brief Validate fallback configuration
 * @param config Pointer to configuration structure
 * @return FALLBACK_SUCCESS if valid, error code if invalid
 */
int fallback_manager_validate_config(const fallback_config_t *config);

/**
 * @brief Set WiFi connection parameters for recovery
 * @param ssid WiFi SSID
 * @param password WiFi password
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_set_wifi_params(const char *ssid, const char *password);

/**
 * @brief Set AP configuration for fallback
 * @param ssid AP SSID
 * @param password AP password
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_set_ap_params(const char *ssid, const char *password);

/**
 * @brief Get current connection type
 * @return Connection type string
 */
const char* fallback_manager_get_connection_type(void);

/**
 * @brief Force switch to WiFi
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_force_wifi(void);

/**
 * @brief Force switch to AP mode
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_force_ap(void);

/**
 * @brief Get connection quality score
 * @return Quality score (0-100)
 */
int fallback_manager_get_connection_quality(void);

/**
 * @brief Handle fallback monitoring (call periodically)
 * @return FALLBACK_SUCCESS on success, error code on failure
 */
int fallback_manager_handle_monitoring(void);

#endif // FALLBACK_MANAGER_H
