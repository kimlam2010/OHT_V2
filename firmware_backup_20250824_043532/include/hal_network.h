#ifndef HAL_NETWORK_H
#define HAL_NETWORK_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// Network Interface Definitions
#define NETWORK_ETH_INTERFACE     "eth0"
#define NETWORK_WIFI_INTERFACE    "wlan0"
#define NETWORK_ETH_SPEED_10      10      // 10 Mbps
#define NETWORK_ETH_SPEED_100     100     // 100 Mbps
#define NETWORK_ETH_SPEED_1000    1000    // 1000 Mbps
#define NETWORK_WIFI_BAND_2_4G    2400    // 2.4 GHz
#define NETWORK_WIFI_BAND_5G      5000    // 5 GHz

// Network States
typedef enum {
    NETWORK_STATE_DISCONNECTED = 0,
    NETWORK_STATE_CONNECTING,
    NETWORK_STATE_CONNECTED,
    NETWORK_STATE_FAILED,
    NETWORK_STATE_TIMEOUT
} network_state_t;

// Network Types
typedef enum {
    NETWORK_TYPE_ETHERNET = 0,
    NETWORK_TYPE_WIFI,
    NETWORK_TYPE_BOTH
} network_type_t;

// WiFi Security Types
typedef enum {
    WIFI_SECURITY_NONE = 0,
    WIFI_SECURITY_WEP,
    WIFI_SECURITY_WPA,
    WIFI_SECURITY_WPA2,
    WIFI_SECURITY_WPA3
} wifi_security_t;

// Network Configuration Structure
typedef struct {
    network_type_t type;
    bool dhcp_enabled;
    char ip_address[16];
    char gateway[16];
    char netmask[16];
    char dns1[16];
    char dns2[16];
    uint32_t timeout_ms;
    bool auto_reconnect;
} network_config_t;

// WiFi Configuration Structure
typedef struct {
    char ssid[32];
    char password[64];
    wifi_security_t security;
    uint32_t band;              // 2400 for 2.4G, 5000 for 5G
    bool backup_only;           // Use as backup to Ethernet
    uint32_t scan_timeout_ms;
} wifi_config_t;

// Network Status Structure
typedef struct {
    network_state_t state;
    network_type_t active_type;
    char ip_address[16];
    char mac_address[18];
    uint32_t speed_mbps;
    uint32_t signal_strength;   // For WiFi (0-100)
    uint32_t packet_loss;
    uint32_t latency_ms;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint32_t connection_time;
    uint32_t disconnect_count;
} network_status_t;

// Network Event Callback
typedef void (*network_event_callback_t)(network_state_t state, network_type_t type);

// Function Prototypes

/**
 * @brief Initialize network system
 * @param config Network configuration
 * @return HAL status
 */
hal_status_t hal_network_init(const network_config_t *config);

/**
 * @brief Deinitialize network system
 * @return HAL status
 */
hal_status_t hal_network_deinit(void);

/**
 * @brief Connect to network
 * @param type Network type to connect
 * @return HAL status
 */
hal_status_t hal_network_connect(network_type_t type);

/**
 * @brief Disconnect from network
 * @param type Network type to disconnect
 * @return HAL status
 */
hal_status_t hal_network_disconnect(network_type_t type);

/**
 * @brief Get network status
 * @param status Pointer to store network status
 * @return HAL status
 */
hal_status_t hal_network_get_status(network_status_t *status);

/**
 * @brief Check if network is connected
 * @param type Network type to check
 * @param connected Pointer to store connection status
 * @return HAL status
 */
hal_status_t hal_network_is_connected(network_type_t type, bool *connected);

/**
 * @brief Set network configuration
 * @param config Network configuration
 * @return HAL status
 */
hal_status_t hal_network_set_config(const network_config_t *config);

/**
 * @brief Get network configuration
 * @param config Pointer to store network configuration
 * @return HAL status
 */
hal_status_t hal_network_get_config(network_config_t *config);

/**
 * @brief Set WiFi configuration
 * @param config WiFi configuration
 * @return HAL status
 */
hal_status_t hal_network_set_wifi_config(const wifi_config_t *config);

/**
 * @brief Get WiFi configuration
 * @param config Pointer to store WiFi configuration
 * @return HAL status
 */
hal_status_t hal_network_get_wifi_config(wifi_config_t *config);

/**
 * @brief Set network event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t hal_network_set_callback(network_event_callback_t callback);

/**
 * @brief Update network system (call periodically)
 * @return HAL status
 */
hal_status_t hal_network_update(void);

// Ethernet Functions

/**
 * @brief Initialize Ethernet interface
 * @return HAL status
 */
hal_status_t hal_ethernet_init(void);

/**
 * @brief Connect Ethernet
 * @return HAL status
 */
hal_status_t hal_ethernet_connect(void);

/**
 * @brief Disconnect Ethernet
 * @return HAL status
 */
hal_status_t hal_ethernet_disconnect(void);

/**
 * @brief Get Ethernet status
 * @param status Pointer to store Ethernet status
 * @return HAL status
 */
hal_status_t hal_ethernet_get_status(network_status_t *status);

/**
 * @brief Get Ethernet speed
 * @param speed_mbps Pointer to store speed in Mbps
 * @return HAL status
 */
hal_status_t hal_ethernet_get_speed(uint32_t *speed_mbps);

/**
 * @brief Test Ethernet connectivity
 * @param target_ip Target IP for ping test
 * @param timeout_ms Timeout in milliseconds
 * @return HAL status
 */
hal_status_t hal_ethernet_test_connectivity(const char *target_ip, uint32_t timeout_ms);

// WiFi Functions

/**
 * @brief Initialize WiFi interface
 * @return HAL status
 */
hal_status_t hal_wifi_init(void);

/**
 * @brief Connect WiFi
 * @return HAL status
 */
hal_status_t hal_wifi_connect(void);

/**
 * @brief Disconnect WiFi
 * @return HAL status
 */
hal_status_t hal_wifi_disconnect(void);

/**
 * @brief Get WiFi status
 * @param status Pointer to store WiFi status
 * @return HAL status
 */
hal_status_t hal_wifi_get_status(network_status_t *status);

/**
 * @brief Scan for WiFi networks
 * @param networks Pointer to store network list
 * @param max_networks Maximum number of networks to scan
 * @param count Pointer to store number of networks found
 * @return HAL status
 */
hal_status_t hal_wifi_scan(char (*networks)[32], uint32_t max_networks, uint32_t *count);

/**
 * @brief Get WiFi signal strength
 * @param strength Pointer to store signal strength (0-100)
 * @return HAL status
 */
hal_status_t hal_wifi_get_signal_strength(uint32_t *strength);

/**
 * @brief Get WiFi channel
 * @param channel Pointer to store channel number
 * @return HAL status
 */
hal_status_t hal_wifi_get_channel(uint32_t *channel);

/**
 * @brief Test WiFi connectivity
 * @param target_ip Target IP for ping test
 * @param timeout_ms Timeout in milliseconds
 * @return HAL status
 */
hal_status_t hal_wifi_test_connectivity(const char *target_ip, uint32_t timeout_ms);

// Network Utility Functions

/**
 * @brief Ping network host
 * @param host Host IP address or hostname
 * @param timeout_ms Timeout in milliseconds
 * @param latency_ms Pointer to store latency in milliseconds
 * @return HAL status
 */
hal_status_t hal_network_ping(const char *host, uint32_t timeout_ms, uint32_t *latency_ms);

/**
 * @brief Get network statistics
 * @param bytes_sent Pointer to store bytes sent
 * @param bytes_received Pointer to store bytes received
 * @return HAL status
 */
hal_status_t hal_network_get_statistics(uint64_t *bytes_sent, uint64_t *bytes_received);

/**
 * @brief Reset network statistics
 * @return HAL status
 */
hal_status_t hal_network_reset_statistics(void);

/**
 * @brief Get network diagnostic information
 * @param info Pointer to store diagnostic info
 * @param max_len Maximum length of info string
 * @return HAL status
 */
hal_status_t hal_network_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Perform network self-test
 * @return HAL status
 */
hal_status_t hal_network_self_test(void);

/**
 * @brief Validate network hardware
 * @return HAL status
 */
hal_status_t hal_network_validate_hardware(void);

// Failover Functions

/**
 * @brief Enable network failover
 * @param primary_type Primary network type
 * @param backup_type Backup network type
 * @return HAL status
 */
hal_status_t hal_network_enable_failover(network_type_t primary_type, network_type_t backup_type);

/**
 * @brief Disable network failover
 * @return HAL status
 */
hal_status_t hal_network_disable_failover(void);

/**
 * @brief Get failover status
 * @param active_type Pointer to store active network type
 * @param failover_enabled Pointer to store failover status
 * @return HAL status
 */
hal_status_t hal_network_get_failover_status(network_type_t *active_type, bool *failover_enabled);

/**
 * @brief Switch to backup network
 * @return HAL status
 */
hal_status_t hal_network_switch_to_backup(void);

/**
 * @brief Switch to primary network
 * @return HAL status
 */
hal_status_t hal_network_switch_to_primary(void);

#endif // HAL_NETWORK_H
