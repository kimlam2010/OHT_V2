/**
 * @file wifi_ap_manager.h
 * @brief WiFi AP Manager - OHT-50 Robot WiFi Access Point Management
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * WiFi AP Manager cung cấp quản lý WiFi Access Point mode cho OHT-50 robots
 * bao gồm AP start/stop, client management, và fallback connectivity.
 */

#ifndef WIFI_AP_MANAGER_H
#define WIFI_AP_MANAGER_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// WiFi AP Manager Error Codes
typedef enum {
    WIFI_AP_SUCCESS = 0,
    WIFI_AP_ERROR_INVALID_PARAM = -1,
    WIFI_AP_ERROR_INIT_FAILED = -2,
    WIFI_AP_ERROR_START_FAILED = -3,
    WIFI_AP_ERROR_STOP_FAILED = -4,
    WIFI_AP_ERROR_NOT_INITIALIZED = -5,
    WIFI_AP_ERROR_ALREADY_RUNNING = -6,
    WIFI_AP_ERROR_NOT_RUNNING = -7,
    WIFI_AP_ERROR_INVALID_SSID = -8,
    WIFI_AP_ERROR_WEAK_PASSWORD = -9,
    WIFI_AP_ERROR_HAL_FAILED = -10,
    WIFI_AP_ERROR_MAX_CLIENTS_REACHED = -11,
    WIFI_AP_ERROR_CLIENT_NOT_FOUND = -12,
    WIFI_AP_ERROR_CONFIG_FAILED = -13
} wifi_ap_error_t;

// WiFi AP Security Types
typedef enum {
    AP_SECURITY_OPEN = 0,
    AP_SECURITY_WPA2 = 2,
    AP_SECURITY_WPA3 = 3,
    AP_SECURITY_WPA2_WPA3 = 4
} ap_security_type_t;

// WiFi AP Status
typedef enum {
    AP_STATUS_DISABLED = 0,
    AP_STATUS_STARTING = 1,
    AP_STATUS_RUNNING = 2,
    AP_STATUS_STOPPING = 3,
    AP_STATUS_ERROR = 4
} ap_status_t;

// WiFi AP Configuration
typedef struct {
    bool ap_enabled;
    char ap_ssid[32];
    char ap_password[64];
    ap_security_type_t ap_security_type;
    char ap_ip[16];
    char ap_netmask[16];
    int ap_channel;
    bool auto_fallback;
    uint32_t fallback_timeout_ms;
    uint32_t max_clients;
} wifi_ap_config_t;

// WiFi AP Client Information
typedef struct {
    char mac_address[18];
    char ip_address[16];
    char hostname[64];
    int signal_strength_dbm;
    uint32_t connected_time_seconds;
    uint32_t bytes_sent;
    uint32_t bytes_received;
    bool authenticated;
} wifi_ap_client_t;

// WiFi AP Status Information
typedef struct {
    ap_status_t status;
    bool ap_enabled;
    char ap_ssid[32];
    char ap_ip[16];
    int ap_channel;
    int connected_clients;
    int max_clients;
    uint32_t uptime_seconds;
    uint32_t total_bytes_sent;
    uint32_t total_bytes_received;
    float cpu_usage_percent;
    float memory_usage_percent;
} wifi_ap_status_t;

// WiFi AP Statistics
typedef struct {
    uint32_t ap_start_count;
    uint32_t ap_stop_count;
    uint32_t client_connections;
    uint32_t client_disconnections;
    uint32_t fallback_triggers;
    uint32_t total_uptime_seconds;
    uint32_t total_bytes_sent;
    uint32_t total_bytes_received;
    float average_cpu_usage;
    float average_memory_usage;
} wifi_ap_statistics_t;

// WiFi AP Manager API Functions

/**
 * @brief Initialize WiFi AP Manager
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_init(void);

/**
 * @brief Deinitialize WiFi AP Manager
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_deinit(void);

/**
 * @brief Start WiFi Access Point
 * @param config AP configuration
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_start(const wifi_ap_config_t *config);

/**
 * @brief Stop WiFi Access Point
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_stop(void);

/**
 * @brief Get WiFi AP status
 * @param status Pointer to status structure
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_get_status(wifi_ap_status_t *status);
int wifi_ap_manager_get_realtime_status(wifi_ap_status_t *status);

/**
 * @brief Get connected clients
 * @param clients Array to store client information
 * @param max_count Maximum number of clients to retrieve
 * @return Number of clients found, or error code on failure
 */
int wifi_ap_manager_get_clients(wifi_ap_client_t *clients, int max_count);

/**
 * @brief Kick client from AP
 * @param mac_address MAC address of client to kick
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_kick_client(const char *mac_address);

/**
 * @brief Get client information by MAC address
 * @param mac_address MAC address of client
 * @param client Pointer to client structure
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_get_client_info(const char *mac_address, wifi_ap_client_t *client);

/**
 * @brief Get client count
 * @return Number of connected clients, or error code on failure
 */
int wifi_ap_manager_get_client_count(void);

/**
 * @brief Check if client is connected
 * @param mac_address MAC address of client
 * @return true if connected, false otherwise
 */
bool wifi_ap_manager_is_client_connected(const char *mac_address);

/**
 * @brief Get AP configuration
 * @param config Pointer to configuration structure
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_get_config(wifi_ap_config_t *config);

/**
 * @brief Set AP configuration
 * @param config Pointer to configuration structure
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_set_config(const wifi_ap_config_t *config);

/**
 * @brief Get AP statistics
 * @param stats Pointer to statistics structure
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_get_statistics(wifi_ap_statistics_t *stats);

/**
 * @brief Reset AP statistics
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_reset_statistics(void);

/**
 * @brief Check if AP is running
 * @return true if running, false otherwise
 */
bool wifi_ap_manager_is_running(void);

/**
 * @brief Get error message for error code
 * @param error_code Error code
 * @return Error message string
 */
const char* wifi_ap_manager_get_error_message(int error_code);

/**
 * @brief Validate AP configuration
 * @param config Pointer to configuration structure
 * @return WIFI_AP_SUCCESS if valid, error code if invalid
 */
int wifi_ap_manager_validate_config(const wifi_ap_config_t *config);

/**
 * @brief Enable/disable auto-fallback mode
 * @param enabled Enable auto-fallback
 * @param timeout_ms Fallback timeout in milliseconds
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_set_auto_fallback(bool enabled, uint32_t timeout_ms);

/**
 * @brief Trigger fallback to AP mode
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_trigger_fallback(void);

/**
 * @brief Get fallback status
 * @param enabled Pointer to store fallback enabled status
 * @param timeout_ms Pointer to store fallback timeout
 * @return WIFI_AP_SUCCESS on success, error code on failure
 */
int wifi_ap_manager_get_fallback_status(bool *enabled, uint32_t *timeout_ms);

#endif // WIFI_AP_MANAGER_H
