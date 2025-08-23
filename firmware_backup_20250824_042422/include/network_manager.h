/**
 * @file network_manager.h
 * @brief Network Manager for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-04 (Network Redundancy System)
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "hal_common.h"
#include "hal_network.h"
#include <stdint.h>
#include <stdbool.h>

// Network Manager Configuration
#define NETWORK_MGR_MAX_INTERFACES     4
#define NETWORK_MGR_MAX_SSIDS          10
#define NETWORK_MGR_DEFAULT_TIMEOUT    5000    // 5 seconds
#define NETWORK_MGR_FAILOVER_TIMEOUT   3000    // 3 seconds
#define NETWORK_MGR_HEALTH_CHECK_INTERVAL 10000 // 10 seconds

// Network Manager Interface Types
typedef enum {
    NETWORK_MGR_INTERFACE_ETHERNET = 0,
    NETWORK_MGR_INTERFACE_WIFI,
    NETWORK_MGR_INTERFACE_CELLULAR,
    NETWORK_MGR_INTERFACE_VPN
} network_mgr_interface_t;

// Network Manager Interface States
typedef enum {
    NETWORK_MGR_STATE_DISCONNECTED = 0,
    NETWORK_MGR_STATE_CONNECTING,
    NETWORK_MGR_STATE_CONNECTED,
    NETWORK_MGR_STATE_FAILED,
    NETWORK_MGR_STATE_DISABLED,
    NETWORK_MGR_STATE_MAINTENANCE
} network_mgr_state_t;

// Network Manager Interface Priority
typedef enum {
    NETWORK_MGR_PRIORITY_PRIMARY = 0,
    NETWORK_MGR_PRIORITY_SECONDARY,
    NETWORK_MGR_PRIORITY_BACKUP,
    NETWORK_MGR_PRIORITY_EMERGENCY
} network_mgr_priority_t;

// Network Manager Failover Modes
typedef enum {
    NETWORK_MGR_FAILOVER_MODE_AUTO = 0,
    NETWORK_MGR_FAILOVER_MODE_MANUAL,
    NETWORK_MGR_FAILOVER_MODE_DISABLED
} network_mgr_failover_mode_t;

// Network Manager Interface Configuration
typedef struct {
    network_mgr_interface_t type;
    network_mgr_priority_t priority;
    bool enabled;
    char interface_name[16];
    network_config_t network_config;
    wifi_config_t wifi_config;
    uint32_t health_check_interval_ms;
    uint32_t failover_timeout_ms;
    bool auto_reconnect;
} network_mgr_interface_config_t;

// Network Manager Interface Status
typedef struct {
    network_mgr_interface_t type;
    network_mgr_state_t state;
    network_mgr_priority_t priority;
    bool active;
    network_status_t network_status;
    uint32_t health_score;          // 0-100
    uint32_t last_health_check;
    uint32_t failover_count;
    uint32_t total_uptime;
    uint32_t last_failover_time;
} network_mgr_interface_status_t;

// Network Manager Configuration
typedef struct {
    network_mgr_failover_mode_t failover_mode;
    uint32_t failover_timeout_ms;
    uint32_t health_check_interval_ms;
    bool auto_failover_enabled;
    bool load_balancing_enabled;
    uint32_t max_failover_attempts;
    bool monitoring_enabled;
    uint32_t statistics_interval_ms;
} network_mgr_config_t;

// Network Manager Status
typedef struct {
    network_mgr_state_t overall_state;
    network_mgr_interface_t active_interface;
    network_mgr_interface_t primary_interface;
    uint32_t total_interfaces;
    uint32_t active_interfaces;
    uint32_t failed_interfaces;
    uint32_t last_failover_time;
    uint32_t failover_count;
    uint32_t total_uptime;
    bool failover_in_progress;
} network_mgr_status_t;

// Network Manager Statistics
typedef struct {
    uint32_t total_connections;
    uint32_t successful_connections;
    uint32_t failed_connections;
    uint32_t total_failovers;
    uint32_t successful_failovers;
    uint32_t failed_failovers;
    uint32_t health_check_count;
    uint32_t health_check_failures;
    uint64_t total_bytes_sent;
    uint64_t total_bytes_received;
    uint32_t average_latency_ms;
    uint32_t packet_loss_percentage;
} network_mgr_stats_t;

// Network Manager Events
typedef enum {
    NETWORK_MGR_EVENT_NONE = 0,
    NETWORK_MGR_EVENT_INTERFACE_CONNECTED,
    NETWORK_MGR_EVENT_INTERFACE_DISCONNECTED,
    NETWORK_MGR_EVENT_INTERFACE_FAILED,
    NETWORK_MGR_EVENT_FAILOVER_STARTED,
    NETWORK_MGR_EVENT_FAILOVER_COMPLETED,
    NETWORK_MGR_EVENT_FAILOVER_FAILED,
    NETWORK_MGR_EVENT_HEALTH_CHECK_FAILED,
    NETWORK_MGR_EVENT_PRIMARY_RESTORED,
    NETWORK_MGR_EVENT_CONFIGURATION_CHANGED
} network_mgr_event_t;

// Network Manager Event Callback
typedef void (*network_mgr_event_callback_t)(network_mgr_event_t event, 
                                           network_mgr_interface_t interface, 
                                           const void *data);

// Function Prototypes

/**
 * @brief Initialize Network Manager
 * @param config Network Manager configuration
 * @return HAL status
 */
hal_status_t network_manager_init(const network_mgr_config_t *config);

/**
 * @brief Deinitialize Network Manager
 * @return HAL status
 */
hal_status_t network_manager_deinit(void);

/**
 * @brief Get Network Manager configuration
 * @param config Pointer to store configuration
 * @return HAL status
 */
hal_status_t network_manager_get_config(network_mgr_config_t *config);

/**
 * @brief Set Network Manager configuration
 * @param config Network Manager configuration
 * @return HAL status
 */
hal_status_t network_manager_set_config(const network_mgr_config_t *config);

/**
 * @brief Get Network Manager status
 * @param status Pointer to store status
 * @return HAL status
 */
hal_status_t network_manager_get_status(network_mgr_status_t *status);

/**
 * @brief Get Network Manager statistics
 * @param stats Pointer to store statistics
 * @return HAL status
 */
hal_status_t network_manager_get_statistics(network_mgr_stats_t *stats);

/**
 * @brief Reset Network Manager statistics
 * @return HAL status
 */
hal_status_t network_manager_reset_statistics(void);

/**
 * @brief Add network interface
 * @param interface_id Interface ID
 * @param config Interface configuration
 * @return HAL status
 */
hal_status_t network_manager_add_interface(uint8_t interface_id, 
                                         const network_mgr_interface_config_t *config);

/**
 * @brief Remove network interface
 * @param interface_id Interface ID
 * @return HAL status
 */
hal_status_t network_manager_remove_interface(uint8_t interface_id);

/**
 * @brief Get interface status
 * @param interface_id Interface ID
 * @param status Pointer to store interface status
 * @return HAL status
 */
hal_status_t network_manager_get_interface_status(uint8_t interface_id, 
                                                 network_mgr_interface_status_t *status);

/**
 * @brief Enable interface
 * @param interface_id Interface ID
 * @return HAL status
 */
hal_status_t network_manager_enable_interface(uint8_t interface_id);

/**
 * @brief Disable interface
 * @param interface_id Interface ID
 * @return HAL status
 */
hal_status_t network_manager_disable_interface(uint8_t interface_id);

/**
 * @brief Connect interface
 * @param interface_id Interface ID
 * @return HAL status
 */
hal_status_t network_manager_connect_interface(uint8_t interface_id);

/**
 * @brief Disconnect interface
 * @param interface_id Interface ID
 * @return HAL status
 */
hal_status_t network_manager_disconnect_interface(uint8_t interface_id);

/**
 * @brief Perform manual failover
 * @param target_interface_id Target interface ID
 * @return HAL status
 */
hal_status_t network_manager_manual_failover(uint8_t target_interface_id);

/**
 * @brief Restore primary interface
 * @return HAL status
 */
hal_status_t network_manager_restore_primary(void);

/**
 * @brief Perform health check on interface
 * @param interface_id Interface ID
 * @return HAL status
 */
hal_status_t network_manager_health_check_interface(uint8_t interface_id);

/**
 * @brief Perform health check on all interfaces
 * @return HAL status
 */
hal_status_t network_manager_health_check_all(void);

/**
 * @brief Set event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t network_manager_set_callback(network_mgr_event_callback_t callback);

/**
 * @brief Get Network Manager diagnostics
 * @param diagnostics Buffer to store diagnostics string
 * @param max_length Maximum length of diagnostics string
 * @return HAL status
 */
hal_status_t network_manager_get_diagnostics(char *diagnostics, uint32_t max_length);

/**
 * @brief Perform Network Manager self test
 * @return HAL status
 */
hal_status_t network_manager_self_test(void);

/**
 * @brief Reset Network Manager
 * @return HAL status
 */
hal_status_t network_manager_reset(void);

/**
 * @brief Update Network Manager (call periodically)
 * @return HAL status
 */
hal_status_t network_manager_update(void);

// Utility Functions

/**
 * @brief Get interface type name
 * @param type Interface type
 * @return Interface type name
 */
const char* network_manager_get_interface_type_name(network_mgr_interface_t type);

/**
 * @brief Get interface state name
 * @param state Interface state
 * @return Interface state name
 */
const char* network_manager_get_interface_state_name(network_mgr_state_t state);

/**
 * @brief Get priority name
 * @param priority Priority level
 * @return Priority name
 */
const char* network_manager_get_priority_name(network_mgr_priority_t priority);

/**
 * @brief Get failover mode name
 * @param mode Failover mode
 * @return Failover mode name
 */
const char* network_manager_get_failover_mode_name(network_mgr_failover_mode_t mode);

/**
 * @brief Get event name
 * @param event Event type
 * @return Event name
 */
const char* network_manager_get_event_name(network_mgr_event_t event);

#endif // NETWORK_MANAGER_H
