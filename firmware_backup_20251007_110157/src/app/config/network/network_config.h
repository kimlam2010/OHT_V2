/**
 * @file network_config.h
 * @brief Network Configuration - OHT-50 Robot Network Management Configuration
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Network Configuration cung cấp quản lý cấu hình network
 * bao gồm WiFi settings, roaming config, và security settings.
 */

#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include "hal_common.h"
#include "network_manager.h"
#include "wifi_manager.h"
#include <stdint.h>
#include <stdbool.h>

// Configuration File Paths
#define NETWORK_CONFIG_FILE "/etc/oht50/network.conf"
#define WIFI_CONFIG_FILE "/etc/oht50/wifi.conf"
#define ROAMING_CONFIG_FILE "/etc/oht50/roaming.conf"

// Default Configuration Values
#define DEFAULT_WIFI_SSID "OHT-50-Network"
#define DEFAULT_WIFI_PASSWORD "secure_password_2025"
#define DEFAULT_WIFI_SECURITY WIFI_SECURITY_WPA2
#define DEFAULT_SIGNAL_THRESHOLD -70
#define DEFAULT_ROAMING_ENABLED true
#define DEFAULT_MOBILE_APP_ENABLED true
#define DEFAULT_SCAN_TIMEOUT_MS 10000
#define DEFAULT_CONNECTION_TIMEOUT_MS 5000
#define DEFAULT_HANDOVER_TIMEOUT_MS 2000

// Configuration Validation
typedef enum {
    CONFIG_VALID = 0,
    CONFIG_INVALID_SSID = -1,
    CONFIG_INVALID_PASSWORD = -2,
    CONFIG_INVALID_SECURITY = -3,
    CONFIG_INVALID_THRESHOLD = -4,
    CONFIG_INVALID_TIMEOUT = -5,
    CONFIG_FILE_ERROR = -6
} config_validation_result_t;

// Network Configuration Functions

/**
 * @brief Load network configuration from file
 * @param config Pointer to network configuration structure
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_load(oht_network_config_t *config);

/**
 * @brief Save network configuration to file
 * @param config Pointer to network configuration structure
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_save(const oht_network_config_t *config);

/**
 * @brief Reset network configuration to defaults
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_reset(void);

/**
 * @brief Validate network configuration
 * @param config Pointer to network configuration structure
 * @return CONFIG_VALID if valid, error code if invalid
 */
config_validation_result_t network_config_validate(const oht_network_config_t *config);

/**
 * @brief Apply network configuration
 * @param config Pointer to network configuration structure
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_apply(const oht_network_config_t *config);

/**
 * @brief Load WiFi configuration from file
 * @param config Pointer to WiFi configuration structure
 * @return CONFIG_VALID on success, error code on failure
 */
int wifi_config_load(wifi_roaming_config_t *config);

/**
 * @brief Save WiFi configuration to file
 * @param config Pointer to WiFi configuration structure
 * @return CONFIG_VALID on success, error code on failure
 */
int wifi_config_save(const wifi_roaming_config_t *config);

/**
 * @brief Reset WiFi configuration to defaults
 * @return CONFIG_VALID on success, error code on failure
 */
int wifi_config_reset(void);

/**
 * @brief Validate WiFi configuration
 * @param config Pointer to WiFi configuration structure
 * @return CONFIG_VALID if valid, error code if invalid
 */
config_validation_result_t wifi_config_validate(const wifi_roaming_config_t *config);

/**
 * @brief Apply WiFi configuration
 * @param config Pointer to WiFi configuration structure
 * @return CONFIG_VALID on success, error code on failure
 */
int wifi_config_apply(const wifi_roaming_config_t *config);

/**
 * @brief Create default network configuration
 * @param config Pointer to network configuration structure
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_create_default(oht_network_config_t *config);

/**
 * @brief Create default WiFi configuration
 * @param config Pointer to WiFi configuration structure
 * @return CONFIG_VALID on success, error code on failure
 */
int wifi_config_create_default(wifi_roaming_config_t *config);

/**
 * @brief Backup current configuration
 * @param backup_path Path to backup file
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_backup(const char *backup_path);

/**
 * @brief Restore configuration from backup
 * @param backup_path Path to backup file
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_restore(const char *backup_path);

/**
 * @brief Get configuration validation error message
 * @param result Validation result
 * @return Error message string
 */
const char* network_config_get_validation_error(config_validation_result_t result);

/**
 * @brief Check if configuration file exists
 * @param file_path Path to configuration file
 * @return true if exists, false otherwise
 */
bool network_config_file_exists(const char *file_path);

/**
 * @brief Create configuration directory if not exists
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_create_directory(void);

/**
 * @brief Set configuration file permissions
 * @param file_path Path to configuration file
 * @return CONFIG_VALID on success, error code on failure
 */
int network_config_set_permissions(const char *file_path);

#endif // NETWORK_CONFIG_H
