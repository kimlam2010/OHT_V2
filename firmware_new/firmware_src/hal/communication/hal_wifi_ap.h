/**
 * @file hal_wifi_ap.h
 * @brief HAL WiFi AP - Hardware Abstraction Layer for WiFi Access Point
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * HAL WiFi AP cung cấp hardware abstraction layer cho WiFi Access Point
 * trên Orange Pi 5B platform với RK3588 chipset.
 */

#ifndef HAL_WIFI_AP_H
#define HAL_WIFI_AP_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// HAL WiFi AP Error Codes
typedef enum {
    HAL_WIFI_AP_SUCCESS = HAL_STATUS_OK,
    HAL_WIFI_AP_ERROR_INVALID_PARAM = -1,
    HAL_WIFI_AP_ERROR_INIT_FAILED = -2,
    HAL_WIFI_AP_ERROR_START_FAILED = -3,
    HAL_WIFI_AP_ERROR_STOP_FAILED = -4,
    HAL_WIFI_AP_ERROR_NOT_INITIALIZED = -5,
    HAL_WIFI_AP_ERROR_ALREADY_RUNNING = -6,
    HAL_WIFI_AP_ERROR_NOT_RUNNING = -7,
    HAL_WIFI_AP_ERROR_SYSTEM_CALL_FAILED = -8,
    HAL_WIFI_AP_ERROR_CONFIG_FAILED = -9,
    HAL_WIFI_AP_ERROR_CLIENT_NOT_FOUND = -10
} hal_wifi_ap_error_t;

// HAL WiFi AP Security Types
typedef enum {
    HAL_AP_SECURITY_OPEN = 0,
    HAL_AP_SECURITY_WPA2 = 2,
    HAL_AP_SECURITY_WPA3 = 3,
    HAL_AP_SECURITY_WPA2_WPA3 = 4
} hal_ap_security_type_t;

// HAL WiFi AP Status
typedef enum {
    HAL_AP_STATUS_DISABLED = 0,
    HAL_AP_STATUS_STARTING = 1,
    HAL_AP_STATUS_RUNNING = 2,
    HAL_AP_STATUS_STOPPING = 3,
    HAL_AP_STATUS_ERROR = 4
} hal_ap_status_t;

// HAL WiFi AP Configuration
typedef struct {
    char ssid[32];
    char password[64];
    hal_ap_security_type_t security_type;
    char ip_address[16];
    char netmask[16];
    int channel;
    uint32_t max_clients;
} hal_wifi_ap_config_t;

// HAL WiFi AP Client Information
typedef struct {
    char mac_address[18];
    char ip_address[16];
    char hostname[64];
    int signal_strength_dbm;
    uint32_t connected_time_seconds;
    uint32_t bytes_sent;
    uint32_t bytes_received;
    bool authenticated;
} hal_wifi_ap_client_t;

// HAL WiFi AP Status Information
typedef struct {
    hal_ap_status_t status;
    bool ap_enabled;
    char ap_ssid[32];
    char ap_ip[16];
    int ap_channel;
    int connected_clients;
    uint32_t uptime_seconds;
    uint32_t total_bytes_sent;
    uint32_t total_bytes_received;
} hal_wifi_ap_status_t;

// HAL WiFi AP Statistics
typedef struct {
    uint32_t ap_start_count;
    uint32_t ap_stop_count;
    uint32_t client_connections;
    uint32_t client_disconnections;
    uint32_t total_uptime_seconds;
    uint32_t total_bytes_sent;
    uint32_t total_bytes_received;
} hal_wifi_ap_statistics_t;

// HAL WiFi AP API Functions

/**
 * @brief Initialize HAL WiFi AP
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_init(void);

/**
 * @brief Deinitialize HAL WiFi AP
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_deinit(void);

/**
 * @brief Start WiFi Access Point
 * @param config AP configuration
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_start(const hal_wifi_ap_config_t *config);

/**
 * @brief Stop WiFi Access Point
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_stop(void);

/**
 * @brief Get WiFi AP status
 * @param status Pointer to status structure
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_get_status(hal_wifi_ap_status_t *status);

/**
 * @brief Get connected clients
 * @param clients Array to store client information
 * @param max_count Maximum number of clients to retrieve
 * @return Number of clients found, or error code on failure
 */
int hal_wifi_ap_get_clients(hal_wifi_ap_client_t *clients, int max_count);

/**
 * @brief Kick client from AP
 * @param mac_address MAC address of client to kick
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_kick_client(const char *mac_address);

/**
 * @brief Set AP IP address
 * @param ip_address IP address
 * @param netmask Network mask
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_set_ip(const char *ip_address, const char *netmask);

/**
 * @brief Get AP statistics
 * @param stats Pointer to statistics structure
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_get_statistics(hal_wifi_ap_statistics_t *stats);

/**
 * @brief Reset AP statistics
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_reset_statistics(void);

/**
 * @brief Check if AP is running
 * @return true if running, false otherwise
 */
bool hal_wifi_ap_is_running(void);

/**
 * @brief Get error message for error code
 * @param error_code Error code
 * @return Error message string
 */
const char* hal_wifi_ap_get_error_message(int error_code);

/**
 * @brief Validate AP configuration
 * @param config Pointer to configuration structure
 * @return HAL_SUCCESS if valid, error code if invalid
 */
hal_status_t hal_wifi_ap_validate_config(const hal_wifi_ap_config_t *config);

/**
 * @brief Set AP channel
 * @param channel Channel number (1-13 for 2.4GHz)
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_set_channel(int channel);

/**
 * @brief Get AP channel
 * @return Channel number, or -1 on error
 */
int hal_wifi_ap_get_channel(void);

/**
 * @brief Set AP security
 * @param security_type Security type
 * @param password Password (for secured networks)
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_set_security(hal_ap_security_type_t security_type, const char *password);

/**
 * @brief Get AP security type
 * @return Security type, or -1 on error
 */
hal_ap_security_type_t hal_wifi_ap_get_security_type(void);

/**
 * @brief Enable/disable AP
 * @param enabled Enable AP
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_set_enabled(bool enabled);

/**
 * @brief Check if AP is enabled
 * @return true if enabled, false otherwise
 */
bool hal_wifi_ap_is_enabled(void);

/**
 * @brief Set maximum number of clients
 * @param max_clients Maximum number of clients
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_set_max_clients(uint32_t max_clients);

/**
 * @brief Get maximum number of clients
 * @return Maximum number of clients, or -1 on error
 */
int hal_wifi_ap_get_max_clients(void);

/**
 * @brief Get AP SSID
 * @param ssid Buffer to store SSID (must be at least 32 bytes)
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_get_ssid(char *ssid);

/**
 * @brief Set AP SSID
 * @param ssid SSID string
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_set_ssid(const char *ssid);

/**
 * @brief Get AP IP address
 * @param ip_address Buffer to store IP address (must be at least 16 bytes)
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_get_ip_address(char *ip_address);

/**
 * @brief Get AP network mask
 * @param netmask Buffer to store network mask (must be at least 16 bytes)
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_get_netmask(char *netmask);

/**
 * @brief Get AP uptime
 * @return Uptime in seconds, or 0 on error
 */
uint32_t hal_wifi_ap_get_uptime(void);

/**
 * @brief Get AP throughput statistics
 * @param bytes_sent Pointer to store bytes sent
 * @param bytes_received Pointer to store bytes received
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_get_throughput(uint32_t *bytes_sent, uint32_t *bytes_received);

/**
 * @brief Reset AP throughput statistics
 * @return HAL_SUCCESS on success, error code on failure
 */
hal_status_t hal_wifi_ap_reset_throughput(void);

#endif // HAL_WIFI_AP_H
