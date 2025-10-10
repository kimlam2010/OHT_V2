/**
 * @file wifi_manager.h
 * @brief WiFi Manager - OHT-50 Robot WiFi Connectivity Management
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * WiFi Manager cung cấp quản lý kết nối WiFi chi tiết cho OHT-50 robots
 * bao gồm scanning, connection, roaming, và signal monitoring.
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "hal_common.h"
#include "network_manager.h"
#include <stdint.h>
#include <stdbool.h>

// WiFi Manager Error Codes
typedef enum {
    WIFI_SUCCESS = 0,
    WIFI_ERROR_INVALID_PARAM = -1,
    WIFI_ERROR_INIT_FAILED = -2,
    WIFI_ERROR_CONNECTION_FAILED = -3,
    WIFI_ERROR_TIMEOUT = -4,
    WIFI_ERROR_AUTH_FAILED = -5,
    WIFI_ERROR_SCAN_FAILED = -6,
    WIFI_ERROR_ROAMING_FAILED = -7,
    WIFI_ERROR_SIGNAL_WEAK = -8,
    WIFI_ERROR_NOT_INITIALIZED = -9
} wifi_error_t;

// WiFi Signal Quality Levels
typedef enum {
    WIFI_SIGNAL_NONE = 0,
    WIFI_SIGNAL_WEAK = 1,
    WIFI_SIGNAL_FAIR = 2,
    WIFI_SIGNAL_GOOD = 3,
    WIFI_SIGNAL_EXCELLENT = 4
} wifi_signal_quality_t;

// WiFi Band Types
typedef enum {
    WIFI_BAND_2_4G = 2400,
    WIFI_BAND_5G = 5000
} wifi_band_t;

// WiFi Roaming Configuration
typedef struct {
    bool enabled;
    int signal_threshold_dbm;
    uint32_t scan_interval_ms;
    uint32_t handover_timeout_ms;
    bool aggressive_roaming;
} wifi_roaming_config_t;

// WiFi Connection Parameters
typedef struct {
    char ssid[32];
    char password[64];
    int security_type;
    wifi_band_t preferred_band;
    bool auto_connect;
    uint32_t connection_timeout_ms;
} wifi_connection_params_t;

// WiFi Network Scan Result
typedef struct {
    char ssid[32];
    char bssid[18];
    int signal_strength_dbm;
    wifi_signal_quality_t signal_quality;
    int frequency_mhz;
    int channel;
    int security_type;
    wifi_band_t band;
    bool hidden;
    bool connected;
} wifi_scan_result_t;

// WiFi Statistics
typedef struct {
    uint32_t connection_attempts;
    uint32_t successful_connections;
    uint32_t failed_connections;
    uint32_t roaming_events;
    uint32_t disconnection_events;
    float connection_success_rate;
    uint32_t total_uptime_seconds;
    uint32_t average_signal_strength_dbm;
} wifi_statistics_t;

// WiFi Manager API Functions

/**
 * @brief Initialize WiFi Manager
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_init(void);

/**
 * @brief Deinitialize WiFi Manager
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_deinit(void);

/**
 * @brief Scan for available WiFi networks
 * @param results Array to store scan results
 * @param max_count Maximum number of networks to scan
 * @param timeout_ms Scan timeout in milliseconds
 * @return Number of networks found, or error code on failure
 */
int wifi_manager_scan_networks(wifi_scan_result_t *results, int max_count, uint32_t timeout_ms);

/**
 * @brief Connect to WiFi network
 * @param params Connection parameters
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_connect(const wifi_connection_params_t *params);

/**
 * @brief Disconnect from current WiFi network
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_disconnect(void);

/**
 * @brief Get current WiFi connection status
 * @return Connection status
 */
wifi_connection_status_t wifi_manager_get_connection_status(void);

/**
 * @brief Update WiFi status from system (realtime)
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_update_status_from_system(void);

/**
 * @brief Get current signal strength
 * @return Signal strength in dBm, or 0 if not connected
 */
int wifi_manager_get_signal_strength(void);

/**
 * @brief Get signal quality level
 * @return Signal quality level
 */
wifi_signal_quality_t wifi_manager_get_signal_quality(void);

/**
 * @brief Enable/disable WiFi roaming
 * @param config Roaming configuration
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_enable_roaming(const wifi_roaming_config_t *config);

/**
 * @brief Handle WiFi roaming (call periodically)
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_handle_roaming(void);

/**
 * @brief Get WiFi statistics
 * @param stats Pointer to statistics structure
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_get_statistics(wifi_statistics_t *stats);

/**
 * @brief Reset WiFi statistics
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_reset_statistics(void);

/**
 * @brief Get WiFi configuration
 * @param config Pointer to configuration structure
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_get_config(wifi_roaming_config_t *config);

/**
 * @brief Set WiFi configuration
 * @param config Pointer to configuration structure
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_set_config(const wifi_roaming_config_t *config);

/**
 * @brief Get error message for error code
 * @param error_code Error code
 * @return Error message string
 */
const char* wifi_manager_get_error_message(int error_code);

/**
 * @brief Check if WiFi is connected
 * @return true if connected, false otherwise
 */
bool wifi_manager_is_connected(void);

/**
 * @brief Get current SSID
 * @param ssid Buffer to store SSID (must be at least 32 bytes)
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_get_current_ssid(char *ssid);

/**
 * @brief Force WiFi reconnection
 * @return WIFI_SUCCESS on success, error code on failure
 */
int wifi_manager_reconnect(void);

#endif // WIFI_MANAGER_H
