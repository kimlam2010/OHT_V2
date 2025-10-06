/**
 * @file network_manager.h
 * @brief Network Manager - OHT-50 Robot WiFi Connectivity Management
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Network Manager cung cấp quản lý kết nối WiFi cho OHT-50 robots
 * di chuyển trên overhead rails trong môi trường công nghiệp.
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// Network Manager Error Codes
typedef enum {
    NETWORK_SUCCESS = 0,
    NETWORK_ERROR_INVALID_PARAM = -1,
    NETWORK_ERROR_INIT_FAILED = -2,
    NETWORK_ERROR_CONNECTION_FAILED = -3,
    NETWORK_ERROR_TIMEOUT = -4,
    NETWORK_ERROR_AUTH_FAILED = -5,
    NETWORK_ERROR_SCAN_FAILED = -6,
    NETWORK_ERROR_CONFIG_FAILED = -7,
    NETWORK_ERROR_NOT_INITIALIZED = -8,
    NETWORK_ERROR_INVALID_SSID = -9,
    NETWORK_ERROR_WEAK_PASSWORD = -10,
    NETWORK_ERROR_INVALID_SECURITY = -11,
    NETWORK_ERROR_ROAMING_FAILED = -12,
    NETWORK_ERROR_MOBILE_APP_FAILED = -13
} network_error_t;

// WiFi Security Types (OHT-50 specific)
typedef enum {
    OHT_WIFI_SECURITY_OPEN = 0,
    OHT_WIFI_SECURITY_WEP = 1,
    OHT_WIFI_SECURITY_WPA = 2,
    OHT_WIFI_SECURITY_WPA2 = 3,
    OHT_WIFI_SECURITY_WPA3 = 4
} oht_wifi_security_type_t;

// WiFi Connection Status
typedef enum {
    WIFI_STATUS_DISCONNECTED = 0,
    WIFI_STATUS_CONNECTING = 1,
    WIFI_STATUS_CONNECTED = 2,
    WIFI_STATUS_FAILED = 3,
    WIFI_STATUS_ROAMING = 4
} wifi_connection_status_t;

// OHT-50 Network Configuration Structure
typedef struct {
    bool wifi_enabled;
    char wifi_ssid[32];
    char wifi_password[64];
    int wifi_security_type;
    int signal_strength;
    bool roaming_enabled;
    bool mobile_app_enabled;
    uint32_t last_update_time;
} oht_network_config_t;

// WiFi Network Information
typedef struct {
    char ssid[32];
    char bssid[18];
    int signal_strength;
    int frequency;
    int channel;
    char security[16];
    bool hidden;
} wifi_network_t;

// OHT-50 Network Status Structure
typedef struct {
    bool connected;
    char current_ssid[32];
    int signal_strength;
    char ip_address[16];
    char gateway[16];
    char dns[16];
    uint32_t bytes_sent;
    uint32_t bytes_received;
    float latency_ms;
    bool roaming_active;
} oht_network_status_t;

// Performance Metrics
typedef struct {
    uint32_t response_time_ms;
    uint32_t request_count;
    uint32_t error_count;
    float success_rate;
} performance_metrics_t;

// Network Manager API Functions

/**
 * @brief Initialize Network Manager
 * @return NETWORK_SUCCESS on success, error code on failure
 * @note This function must be called before using any network functions
 */
int network_manager_init(void);

/**
 * @brief Deinitialize Network Manager
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_deinit(void);

/**
 * @brief Connect to WiFi network
 * @param ssid WiFi network SSID
 * @param password WiFi network password
 * @return NETWORK_SUCCESS on success, error code on failure
 * @note Password must be at least 8 characters long
 */
int network_manager_connect_wifi(const char *ssid, const char *password);

/**
 * @brief Disconnect from current WiFi network
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_disconnect_wifi(void);

/**
 * @brief Scan for available WiFi networks
 * @param networks Array to store network information
 * @param max_count Maximum number of networks to scan
 * @return Number of networks found, or error code on failure
 */
int network_manager_scan_networks(wifi_network_t *networks, int max_count);

/**
 * @brief Get current network status
 * @param status Pointer to network status structure
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_get_status(oht_network_status_t *status);

/**
 * @brief Enable/disable WiFi roaming
 * @param enable true to enable roaming, false to disable
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_enable_roaming(bool enable);

/**
 * @brief Enable/disable mobile app support
 * @param enable true to enable mobile app, false to disable
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_enable_mobile_app(bool enable);

/**
 * @brief Get network configuration
 * @param config Pointer to network configuration structure
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_get_config(oht_network_config_t *config);

/**
 * @brief Set network configuration
 * @param config Pointer to network configuration structure
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_set_config(const oht_network_config_t *config);

/**
 * @brief Get performance metrics
 * @param metrics Pointer to performance metrics structure
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_get_performance(performance_metrics_t *metrics);

/**
 * @brief Reset network configuration to defaults
 * @return NETWORK_SUCCESS on success, error code on failure
 */
int network_manager_reset_config(void);

/**
 * @brief Validate network configuration
 * @param config Pointer to network configuration structure
 * @return true if valid, false if invalid
 */
bool network_manager_validate_config(const oht_network_config_t *config);

/**
 * @brief Get error message for error code
 * @param error_code Error code
 * @return Error message string
 */
const char* network_manager_get_error_message(int error_code);

#endif // NETWORK_MANAGER_H
