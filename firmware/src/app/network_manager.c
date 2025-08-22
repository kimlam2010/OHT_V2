/**
 * @file network_manager.c
 * @brief Network Manager Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-04 (Network Redundancy System)
 */

#include "network_manager.h"
#include <string.h>
#include <stdio.h>

// Network Manager internal structure
typedef struct {
    network_mgr_config_t config;
    network_mgr_status_t status;
    network_mgr_stats_t statistics;
    network_mgr_event_callback_t event_callback;
    bool initialized;
    uint64_t last_update_time;
    uint64_t last_health_check_time;
    uint64_t last_statistics_time;
    
    // Interface management
    network_mgr_interface_config_t interfaces[NETWORK_MGR_MAX_INTERFACES];
    network_mgr_interface_status_t interface_status[NETWORK_MGR_MAX_INTERFACES];
    bool interface_registered[NETWORK_MGR_MAX_INTERFACES];
    
    // Failover management
    uint8_t current_primary_interface;
    uint8_t current_active_interface;
    bool failover_in_progress;
    uint64_t failover_start_time;
    uint8_t failover_target_interface;
    
    // Health monitoring
    uint32_t health_check_counter;
    bool health_monitoring_active;
} network_manager_t;

// Global Network Manager instance
static network_manager_t g_network_manager = {0};

// Default configuration
static const network_mgr_config_t default_config = {
    .failover_mode = NETWORK_MGR_FAILOVER_MODE_AUTO,
    .failover_timeout_ms = NETWORK_MGR_FAILOVER_TIMEOUT,
    .health_check_interval_ms = NETWORK_MGR_HEALTH_CHECK_INTERVAL,
    .auto_failover_enabled = true,
    .load_balancing_enabled = false,
    .max_failover_attempts = 3,
    .monitoring_enabled = true,
    .statistics_interval_ms = 60000  // 1 minute
};

// Forward declarations
static hal_status_t perform_health_check(uint8_t interface_id);
static hal_status_t perform_failover(uint8_t target_interface_id);
static hal_status_t restore_primary_interface(void);
static hal_status_t update_interface_status(uint8_t interface_id);
static hal_status_t handle_network_event(network_mgr_event_t event, 
                                       network_mgr_interface_t interface, 
                                       const void *data);
static uint8_t find_best_available_interface(void);
static bool is_interface_healthy(uint8_t interface_id);
static void update_statistics(void);

// Network Manager implementation

hal_status_t network_manager_init(const network_mgr_config_t *config) {
    if (g_network_manager.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize Network Manager structure
    memset(&g_network_manager, 0, sizeof(network_manager_t));
    
    // Set configuration
    if (config != NULL) {
        memcpy(&g_network_manager.config, config, sizeof(network_mgr_config_t));
    } else {
        memcpy(&g_network_manager.config, &default_config, sizeof(network_mgr_config_t));
    }
    
    // Initialize HAL Network
    network_config_t hal_network_config = {
        .type = NETWORK_TYPE_BOTH,
        .dhcp_enabled = true,
        .timeout_ms = g_network_manager.config.failover_timeout_ms,
        .auto_reconnect = true
    };
    
    hal_status_t status = hal_network_init(&hal_network_config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Initialize status
    g_network_manager.status.overall_state = NETWORK_MGR_STATE_DISCONNECTED;
    g_network_manager.status.active_interface = NETWORK_MGR_INTERFACE_ETHERNET;
    g_network_manager.status.primary_interface = NETWORK_MGR_INTERFACE_ETHERNET;
    g_network_manager.status.total_interfaces = 0;
    g_network_manager.status.active_interfaces = 0;
    g_network_manager.status.failed_interfaces = 0;
    g_network_manager.status.failover_count = 0;
    g_network_manager.status.total_uptime = 0;
    g_network_manager.status.failover_in_progress = false;
    
    // Initialize timing
    g_network_manager.last_update_time = hal_get_timestamp_us();
    g_network_manager.last_health_check_time = hal_get_timestamp_us();
    g_network_manager.last_statistics_time = hal_get_timestamp_us();
    
    // Initialize interface arrays
    for (int i = 0; i < NETWORK_MGR_MAX_INTERFACES; i++) {
        g_network_manager.interface_registered[i] = false;
        g_network_manager.interface_status[i].state = NETWORK_MGR_STATE_DISCONNECTED;
        g_network_manager.interface_status[i].active = false;
        g_network_manager.interface_status[i].health_score = 0;
        g_network_manager.interface_status[i].failover_count = 0;
        g_network_manager.interface_status[i].total_uptime = 0;
    }
    
    g_network_manager.initialized = true;
    g_network_manager.health_monitoring_active = g_network_manager.config.monitoring_enabled;
    
    handle_network_event(NETWORK_MGR_EVENT_CONFIGURATION_CHANGED, 
                        NETWORK_MGR_INTERFACE_ETHERNET, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t network_manager_deinit(void) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Disconnect all interfaces
    for (int i = 0; i < NETWORK_MGR_MAX_INTERFACES; i++) {
        if (g_network_manager.interface_registered[i]) {
            network_manager_disconnect_interface(i);
        }
    }
    
    // Deinitialize HAL Network
    hal_network_deinit();
    
    // Clear Network Manager
    memset(&g_network_manager, 0, sizeof(network_manager_t));
    
    return HAL_STATUS_OK;
}

hal_status_t network_manager_get_config(network_mgr_config_t *config) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_network_manager.config, sizeof(network_mgr_config_t));
    return HAL_STATUS_OK;
}

hal_status_t network_manager_set_config(const network_mgr_config_t *config) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_network_manager.config, config, sizeof(network_mgr_config_t));
    
    // Update monitoring state
    g_network_manager.health_monitoring_active = g_network_manager.config.monitoring_enabled;
    
    handle_network_event(NETWORK_MGR_EVENT_CONFIGURATION_CHANGED, 
                        NETWORK_MGR_INTERFACE_ETHERNET, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t network_manager_get_status(network_mgr_status_t *status) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(status, &g_network_manager.status, sizeof(network_mgr_status_t));
    return HAL_STATUS_OK;
}

hal_status_t network_manager_get_statistics(network_mgr_stats_t *stats) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(stats, &g_network_manager.statistics, sizeof(network_mgr_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t network_manager_reset_statistics(void) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&g_network_manager.statistics, 0, sizeof(network_mgr_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t network_manager_add_interface(uint8_t interface_id, 
                                         const network_mgr_interface_config_t *config) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Register interface
    memcpy(&g_network_manager.interfaces[interface_id], config, sizeof(network_mgr_interface_config_t));
    g_network_manager.interface_registered[interface_id] = true;
    
    // Initialize interface status
    g_network_manager.interface_status[interface_id].type = config->type;
    g_network_manager.interface_status[interface_id].priority = config->priority;
    g_network_manager.interface_status[interface_id].state = NETWORK_MGR_STATE_DISCONNECTED;
    g_network_manager.interface_status[interface_id].active = false;
    g_network_manager.interface_status[interface_id].health_score = 0;
    g_network_manager.interface_status[interface_id].failover_count = 0;
    g_network_manager.interface_status[interface_id].total_uptime = 0;
    g_network_manager.interface_status[interface_id].last_failover_time = 0;
    
    // Update status
    g_network_manager.status.total_interfaces++;
    
    // Set as primary if this is the first interface
    if (g_network_manager.status.total_interfaces == 1) {
        g_network_manager.current_primary_interface = interface_id;
        g_network_manager.current_active_interface = interface_id;
        g_network_manager.status.primary_interface = config->type;
        g_network_manager.status.active_interface = config->type;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t network_manager_remove_interface(uint8_t interface_id) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Disconnect interface if active
    if (g_network_manager.interface_status[interface_id].active) {
        network_manager_disconnect_interface(interface_id);
    }
    
    // Unregister interface
    g_network_manager.interface_registered[interface_id] = false;
    g_network_manager.status.total_interfaces--;
    
    // Update active interface if this was the active one
    if (g_network_manager.current_active_interface == interface_id) {
        uint8_t new_active = find_best_available_interface();
        if (new_active < NETWORK_MGR_MAX_INTERFACES) {
            g_network_manager.current_active_interface = new_active;
            g_network_manager.status.active_interface = g_network_manager.interfaces[new_active].type;
        } else {
            g_network_manager.status.overall_state = NETWORK_MGR_STATE_DISCONNECTED;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t network_manager_get_interface_status(uint8_t interface_id, 
                                                 network_mgr_interface_status_t *status) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(status, &g_network_manager.interface_status[interface_id], 
           sizeof(network_mgr_interface_status_t));
    return HAL_STATUS_OK;
}

hal_status_t network_manager_enable_interface(uint8_t interface_id) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    g_network_manager.interfaces[interface_id].enabled = true;
    return HAL_STATUS_OK;
}

hal_status_t network_manager_disable_interface(uint8_t interface_id) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Disconnect if active
    if (g_network_manager.interface_status[interface_id].active) {
        network_manager_disconnect_interface(interface_id);
    }
    
    g_network_manager.interfaces[interface_id].enabled = false;
    return HAL_STATUS_OK;
}

hal_status_t network_manager_connect_interface(uint8_t interface_id) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_network_manager.interface_registered[interface_id] || 
        !g_network_manager.interfaces[interface_id].enabled) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Update interface status
    g_network_manager.interface_status[interface_id].state = NETWORK_MGR_STATE_CONNECTING;
    
    // Connect using HAL Network
    hal_status_t status = hal_network_connect(g_network_manager.interfaces[interface_id].network_config.type);
    if (status == HAL_STATUS_OK) {
        g_network_manager.interface_status[interface_id].state = NETWORK_MGR_STATE_CONNECTED;
        g_network_manager.interface_status[interface_id].active = true;
        g_network_manager.status.active_interfaces++;
        
        // Update overall status
        if (g_network_manager.status.overall_state == NETWORK_MGR_STATE_DISCONNECTED) {
            g_network_manager.status.overall_state = NETWORK_MGR_STATE_CONNECTED;
        }
        
        // Set as active interface if this is the primary
        if (g_network_manager.interfaces[interface_id].priority == NETWORK_MGR_PRIORITY_PRIMARY) {
            g_network_manager.current_active_interface = interface_id;
            g_network_manager.status.active_interface = g_network_manager.interfaces[interface_id].type;
        }
        
        g_network_manager.statistics.successful_connections++;
        handle_network_event(NETWORK_MGR_EVENT_INTERFACE_CONNECTED, 
                           g_network_manager.interfaces[interface_id].type, NULL);
    } else {
        g_network_manager.interface_status[interface_id].state = NETWORK_MGR_STATE_FAILED;
        g_network_manager.statistics.failed_connections++;
        handle_network_event(NETWORK_MGR_EVENT_INTERFACE_FAILED, 
                           g_network_manager.interfaces[interface_id].type, NULL);
    }
    
    g_network_manager.statistics.total_connections++;
    return status;
}

hal_status_t network_manager_disconnect_interface(uint8_t interface_id) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Disconnect using HAL Network
    hal_status_t status = hal_network_disconnect(g_network_manager.interfaces[interface_id].network_config.type);
    
    // Update interface status
    g_network_manager.interface_status[interface_id].state = NETWORK_MGR_STATE_DISCONNECTED;
    g_network_manager.interface_status[interface_id].active = false;
    
    if (g_network_manager.interface_status[interface_id].active) {
        g_network_manager.status.active_interfaces--;
    }
    
    // Update overall status if no active interfaces
    if (g_network_manager.status.active_interfaces == 0) {
        g_network_manager.status.overall_state = NETWORK_MGR_STATE_DISCONNECTED;
    }
    
    handle_network_event(NETWORK_MGR_EVENT_INTERFACE_DISCONNECTED, 
                       g_network_manager.interfaces[interface_id].type, NULL);
    
    return status;
}

hal_status_t network_manager_manual_failover(uint8_t target_interface_id) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (target_interface_id >= NETWORK_MGR_MAX_INTERFACES) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_network_manager.interface_registered[target_interface_id] || 
        !g_network_manager.interfaces[target_interface_id].enabled) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return perform_failover(target_interface_id);
}

hal_status_t network_manager_restore_primary(void) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    return restore_primary_interface();
}

hal_status_t network_manager_health_check_interface(uint8_t interface_id) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return perform_health_check(interface_id);
}

hal_status_t network_manager_health_check_all(void) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    hal_status_t overall_status = HAL_STATUS_OK;
    
    for (int i = 0; i < NETWORK_MGR_MAX_INTERFACES; i++) {
        if (g_network_manager.interface_registered[i] && 
            g_network_manager.interfaces[i].enabled) {
            hal_status_t status = perform_health_check(i);
            if (status != HAL_STATUS_OK) {
                overall_status = status;
            }
        }
    }
    
    return overall_status;
}

hal_status_t network_manager_set_callback(network_mgr_event_callback_t callback) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_network_manager.event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t network_manager_get_diagnostics(char *diagnostics, uint32_t max_length) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (diagnostics == NULL || max_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(diagnostics, max_length,
             "Network Manager Diagnostics:\n"
             "Overall State: %s\n"
             "Active Interface: %s\n"
             "Primary Interface: %s\n"
             "Total Interfaces: %d\n"
             "Active Interfaces: %d\n"
             "Failed Interfaces: %d\n"
             "Failover Count: %d\n"
             "Total Uptime: %d ms\n"
             "Failover In Progress: %s\n"
             "Health Monitoring: %s\n"
             "Auto Failover: %s\n"
             "Load Balancing: %s\n"
             "Last Health Check: %d ms ago\n"
             "Last Statistics Update: %d ms ago",
             network_manager_get_interface_state_name(g_network_manager.status.overall_state),
             network_manager_get_interface_type_name(g_network_manager.status.active_interface),
             network_manager_get_interface_type_name(g_network_manager.status.primary_interface),
             g_network_manager.status.total_interfaces,
             g_network_manager.status.active_interfaces,
             g_network_manager.status.failed_interfaces,
             g_network_manager.status.failover_count,
             g_network_manager.status.total_uptime,
             g_network_manager.status.failover_in_progress ? "YES" : "NO",
             g_network_manager.health_monitoring_active ? "ACTIVE" : "INACTIVE",
             g_network_manager.config.auto_failover_enabled ? "ENABLED" : "DISABLED",
             g_network_manager.config.load_balancing_enabled ? "ENABLED" : "DISABLED",
             (uint32_t)((hal_get_timestamp_us() - g_network_manager.last_health_check_time) / 1000ULL),
             (uint32_t)((hal_get_timestamp_us() - g_network_manager.last_statistics_time) / 1000ULL));
    
    return HAL_STATUS_OK;
}

hal_status_t network_manager_self_test(void) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Test interface registration
    network_mgr_interface_config_t test_config = {
        .type = NETWORK_MGR_INTERFACE_ETHERNET,
        .priority = NETWORK_MGR_PRIORITY_PRIMARY,
        .enabled = true,
        .health_check_interval_ms = 5000,
        .failover_timeout_ms = 3000,
        .auto_reconnect = true
    };
    strcpy(test_config.interface_name, "eth0");
    
    hal_status_t status = network_manager_add_interface(0, &test_config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Test interface status retrieval
    network_mgr_interface_status_t test_status;
    status = network_manager_get_interface_status(0, &test_status);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Test interface removal
    status = network_manager_remove_interface(0);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t network_manager_reset(void) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Disconnect all interfaces
    for (int i = 0; i < NETWORK_MGR_MAX_INTERFACES; i++) {
        if (g_network_manager.interface_registered[i]) {
            network_manager_disconnect_interface(i);
            g_network_manager.interface_registered[i] = false;
        }
    }
    
    // Reset statistics
    network_manager_reset_statistics();
    
    // Reset status
    g_network_manager.status.overall_state = NETWORK_MGR_STATE_DISCONNECTED;
    g_network_manager.status.active_interfaces = 0;
    g_network_manager.status.failed_interfaces = 0;
    g_network_manager.status.total_interfaces = 0;
    g_network_manager.status.failover_in_progress = false;
    g_network_manager.status.last_failover_time = 0;
    
    // Reset timing
    g_network_manager.last_update_time = hal_get_timestamp_us();
    g_network_manager.last_health_check_time = hal_get_timestamp_us();
    g_network_manager.last_statistics_time = hal_get_timestamp_us();
    
    return HAL_STATUS_OK;
}

hal_status_t network_manager_update(void) {
    if (!g_network_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = hal_get_timestamp_us();
    
    // Update statistics periodically
    if ((current_time - g_network_manager.last_statistics_time) >= 
        (g_network_manager.config.statistics_interval_ms * 1000ULL)) {
        update_statistics();
        g_network_manager.last_statistics_time = current_time;
    }
    
    // Perform health checks periodically
    if (g_network_manager.health_monitoring_active && 
        (current_time - g_network_manager.last_health_check_time) >= 
        (g_network_manager.config.health_check_interval_ms * 1000ULL)) {
        network_manager_health_check_all();
        g_network_manager.last_health_check_time = current_time;
    }
    
    // Handle failover in progress
    if (g_network_manager.failover_in_progress) {
        if ((current_time - g_network_manager.failover_start_time) >= 
            (g_network_manager.config.failover_timeout_ms * 1000ULL)) {
            // Failover timeout
            g_network_manager.failover_in_progress = false;
            g_network_manager.statistics.failed_failovers++;
            handle_network_event(NETWORK_MGR_EVENT_FAILOVER_FAILED, 
                               NETWORK_MGR_INTERFACE_ETHERNET, NULL);
        }
    }
    
    // Auto-failover logic
    if (g_network_manager.config.auto_failover_enabled && 
        !g_network_manager.failover_in_progress) {
        
        // Check if current active interface is healthy
        if (g_network_manager.current_active_interface < NETWORK_MGR_MAX_INTERFACES) {
            if (!is_interface_healthy(g_network_manager.current_active_interface)) {
                // Find best available interface
                uint8_t best_interface = find_best_available_interface();
                if (best_interface < NETWORK_MGR_MAX_INTERFACES && 
                    best_interface != g_network_manager.current_active_interface) {
                    perform_failover(best_interface);
                }
            }
        }
    }
    
    g_network_manager.last_update_time = current_time;
    return HAL_STATUS_OK;
}

// Utility Functions

const char* network_manager_get_interface_type_name(network_mgr_interface_t type) {
    switch (type) {
        case NETWORK_MGR_INTERFACE_ETHERNET: return "ETHERNET";
        case NETWORK_MGR_INTERFACE_WIFI: return "WIFI";
        case NETWORK_MGR_INTERFACE_CELLULAR: return "CELLULAR";
        case NETWORK_MGR_INTERFACE_VPN: return "VPN";
        default: return "UNKNOWN";
    }
}

const char* network_manager_get_interface_state_name(network_mgr_state_t state) {
    switch (state) {
        case NETWORK_MGR_STATE_DISCONNECTED: return "DISCONNECTED";
        case NETWORK_MGR_STATE_CONNECTING: return "CONNECTING";
        case NETWORK_MGR_STATE_CONNECTED: return "CONNECTED";
        case NETWORK_MGR_STATE_FAILED: return "FAILED";
        case NETWORK_MGR_STATE_DISABLED: return "DISABLED";
        case NETWORK_MGR_STATE_MAINTENANCE: return "MAINTENANCE";
        default: return "UNKNOWN";
    }
}

const char* network_manager_get_priority_name(network_mgr_priority_t priority) {
    switch (priority) {
        case NETWORK_MGR_PRIORITY_PRIMARY: return "PRIMARY";
        case NETWORK_MGR_PRIORITY_SECONDARY: return "SECONDARY";
        case NETWORK_MGR_PRIORITY_BACKUP: return "BACKUP";
        case NETWORK_MGR_PRIORITY_EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

const char* network_manager_get_failover_mode_name(network_mgr_failover_mode_t mode) {
    switch (mode) {
        case NETWORK_MGR_FAILOVER_MODE_AUTO: return "AUTO";
        case NETWORK_MGR_FAILOVER_MODE_MANUAL: return "MANUAL";
        case NETWORK_MGR_FAILOVER_MODE_DISABLED: return "DISABLED";
        default: return "UNKNOWN";
    }
}

const char* network_manager_get_event_name(network_mgr_event_t event) {
    switch (event) {
        case NETWORK_MGR_EVENT_NONE: return "NONE";
        case NETWORK_MGR_EVENT_INTERFACE_CONNECTED: return "INTERFACE_CONNECTED";
        case NETWORK_MGR_EVENT_INTERFACE_DISCONNECTED: return "INTERFACE_DISCONNECTED";
        case NETWORK_MGR_EVENT_INTERFACE_FAILED: return "INTERFACE_FAILED";
        case NETWORK_MGR_EVENT_FAILOVER_STARTED: return "FAILOVER_STARTED";
        case NETWORK_MGR_EVENT_FAILOVER_COMPLETED: return "FAILOVER_COMPLETED";
        case NETWORK_MGR_EVENT_FAILOVER_FAILED: return "FAILOVER_FAILED";
        case NETWORK_MGR_EVENT_HEALTH_CHECK_FAILED: return "HEALTH_CHECK_FAILED";
        case NETWORK_MGR_EVENT_PRIMARY_RESTORED: return "PRIMARY_RESTORED";
        case NETWORK_MGR_EVENT_CONFIGURATION_CHANGED: return "CONFIGURATION_CHANGED";
        default: return "UNKNOWN";
    }
}

// Internal helper functions

static hal_status_t perform_health_check(uint8_t interface_id) {
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES || 
        !g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    g_network_manager.statistics.health_check_count++;
    
    // Get network status from HAL
    network_status_t hal_status;
    hal_status_t status = hal_network_get_status(&hal_status);
    
    if (status == HAL_STATUS_OK && hal_status.state == NETWORK_STATE_CONNECTED) {
        // Interface is healthy
        g_network_manager.interface_status[interface_id].health_score = 100;
        g_network_manager.interface_status[interface_id].last_health_check = hal_get_timestamp_us();
        
        // Update interface status
        memcpy(&g_network_manager.interface_status[interface_id].network_status, 
               &hal_status, sizeof(network_status_t));
        
    } else {
        // Interface is unhealthy
        g_network_manager.interface_status[interface_id].health_score = 0;
        g_network_manager.statistics.health_check_failures++;
        
        if (g_network_manager.interface_status[interface_id].active) {
            g_network_manager.interface_status[interface_id].state = NETWORK_MGR_STATE_FAILED;
            handle_network_event(NETWORK_MGR_EVENT_HEALTH_CHECK_FAILED, 
                               g_network_manager.interfaces[interface_id].type, NULL);
        }
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t perform_failover(uint8_t target_interface_id) {
    if (target_interface_id >= NETWORK_MGR_MAX_INTERFACES || 
        !g_network_manager.interface_registered[target_interface_id] ||
        !g_network_manager.interfaces[target_interface_id].enabled) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (g_network_manager.failover_in_progress) {
        return HAL_STATUS_BUSY;
    }
    
    // Start failover
    g_network_manager.failover_in_progress = true;
    g_network_manager.failover_start_time = hal_get_timestamp_us();
    g_network_manager.failover_target_interface = target_interface_id;
    g_network_manager.status.failover_in_progress = true;
    
    handle_network_event(NETWORK_MGR_EVENT_FAILOVER_STARTED, 
                       g_network_manager.interfaces[target_interface_id].type, NULL);
    
    // Disconnect current active interface
    if (g_network_manager.current_active_interface < NETWORK_MGR_MAX_INTERFACES) {
        network_manager_disconnect_interface(g_network_manager.current_active_interface);
    }
    
    // Connect target interface
    hal_status_t status = network_manager_connect_interface(target_interface_id);
    
    if (status == HAL_STATUS_OK) {
        // Failover successful
        g_network_manager.current_active_interface = target_interface_id;
        g_network_manager.status.active_interface = g_network_manager.interfaces[target_interface_id].type;
        g_network_manager.status.failover_count++;
        g_network_manager.statistics.successful_failovers++;
        
        g_network_manager.interface_status[target_interface_id].failover_count++;
        g_network_manager.interface_status[target_interface_id].last_failover_time = hal_get_timestamp_us();
        
        handle_network_event(NETWORK_MGR_EVENT_FAILOVER_COMPLETED, 
                           g_network_manager.interfaces[target_interface_id].type, NULL);
    } else {
        // Failover failed
        g_network_manager.statistics.failed_failovers++;
        handle_network_event(NETWORK_MGR_EVENT_FAILOVER_FAILED, 
                           g_network_manager.interfaces[target_interface_id].type, NULL);
    }
    
    g_network_manager.failover_in_progress = false;
    g_network_manager.status.failover_in_progress = false;
    g_network_manager.status.last_failover_time = hal_get_timestamp_us();
    
    return status;
}

static hal_status_t restore_primary_interface(void) {
    if (g_network_manager.current_primary_interface >= NETWORK_MGR_MAX_INTERFACES) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check if primary interface is registered and enabled
    if (!g_network_manager.interface_registered[g_network_manager.current_primary_interface] ||
        !g_network_manager.interfaces[g_network_manager.current_primary_interface].enabled) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Always try to restore primary interface, even if not currently healthy
    // The failover process will handle reconnection
    return perform_failover(g_network_manager.current_primary_interface);
}

__attribute__((unused))
static hal_status_t update_interface_status(uint8_t interface_id) {
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES || 
        !g_network_manager.interface_registered[interface_id]) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Get current network status from HAL
    network_status_t hal_status;
    hal_status_t status = hal_network_get_status(&hal_status);
    
    if (status == HAL_STATUS_OK) {
        memcpy(&g_network_manager.interface_status[interface_id].network_status, 
               &hal_status, sizeof(network_status_t));
    }
    
    return status;
}

static hal_status_t handle_network_event(network_mgr_event_t event, 
                                       network_mgr_interface_t interface, 
                                       const void *data) {
    if (g_network_manager.event_callback != NULL) {
        g_network_manager.event_callback(event, interface, data);
    }
    
    return HAL_STATUS_OK;
}

static uint8_t find_best_available_interface(void) {
    uint8_t best_interface = NETWORK_MGR_MAX_INTERFACES;
    network_mgr_priority_t best_priority = NETWORK_MGR_PRIORITY_EMERGENCY;
    
    for (int i = 0; i < NETWORK_MGR_MAX_INTERFACES; i++) {
        if (g_network_manager.interface_registered[i] && 
            g_network_manager.interfaces[i].enabled &&
            is_interface_healthy(i)) {
            
            if (g_network_manager.interfaces[i].priority < best_priority) {
                best_priority = g_network_manager.interfaces[i].priority;
                best_interface = i;
            }
        }
    }
    
    return best_interface;
}

static bool is_interface_healthy(uint8_t interface_id) {
    if (interface_id >= NETWORK_MGR_MAX_INTERFACES || 
        !g_network_manager.interface_registered[interface_id]) {
        return false;
    }
    
    return (g_network_manager.interface_status[interface_id].state == NETWORK_MGR_STATE_CONNECTED &&
            g_network_manager.interface_status[interface_id].health_score > 50);
}

static void update_statistics(void) {
    // Update total uptime
    uint64_t current_time = hal_get_timestamp_us();
    uint32_t elapsed_ms = (uint32_t)((current_time - g_network_manager.last_statistics_time) / 1000ULL);
    
    for (int i = 0; i < NETWORK_MGR_MAX_INTERFACES; i++) {
        if (g_network_manager.interface_registered[i] && 
            g_network_manager.interface_status[i].active) {
            g_network_manager.interface_status[i].total_uptime += elapsed_ms;
        }
    }
    
    g_network_manager.status.total_uptime += elapsed_ms;
    
    // Update average latency and packet loss
    uint32_t total_latency = 0;
    uint32_t total_packet_loss = 0;
    uint32_t active_count = 0;
    
    for (int i = 0; i < NETWORK_MGR_MAX_INTERFACES; i++) {
        if (g_network_manager.interface_registered[i] && 
            g_network_manager.interface_status[i].active) {
            total_latency += g_network_manager.interface_status[i].network_status.latency_ms;
            total_packet_loss += g_network_manager.interface_status[i].network_status.packet_loss;
            active_count++;
        }
    }
    
    if (active_count > 0) {
        g_network_manager.statistics.average_latency_ms = total_latency / active_count;
        g_network_manager.statistics.packet_loss_percentage = total_packet_loss / active_count;
    }
}
