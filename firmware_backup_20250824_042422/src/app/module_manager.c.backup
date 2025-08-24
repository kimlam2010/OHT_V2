/**
 * @file module_manager.c
 * @brief Module Management System Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-07 (Module Management Implementation)
 */

#include "module_manager.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Maximum number of modules that can be managed
#define MAX_MODULES 32

// Module entry structure
typedef struct {
    bool registered;
    module_info_t info;
    module_status_info_t status;
    uint64_t last_health_check;
    uint64_t discovery_time;
} module_entry_t;

// Module Manager internal state
static struct {
    bool initialized;
    module_config_t config;
    module_mgr_status_t status;
    module_entry_t modules[MAX_MODULES];
    module_event_callback_t event_callback;
    uint64_t last_discovery_time;
    uint64_t last_health_check_time;
    uint32_t discovery_sequence;
} g_module_manager = {0};

// Default configuration
static const module_config_t default_config = {
    .module_id = 0,
    .discovery_timeout_ms = 5000,
    .health_check_interval_ms = 10000,
    .max_response_time_ms = 1000,
    .retry_count = 3,
    .auto_discovery_enabled = true,
    .health_monitoring_enabled = true,
    .config_flags = 0
};

// Forward declarations
static hal_status_t perform_module_discovery(void);
static hal_status_t perform_health_check(uint8_t module_id);
static void handle_module_event(module_event_t event, uint8_t module_id, const void *data);
static uint8_t calculate_health_percentage(const module_status_info_t *status);
static module_health_t get_health_level(uint8_t percentage);
static bool is_module_id_valid(uint8_t module_id);
static int find_module_index(uint8_t module_id);

// Module Manager Implementation

hal_status_t module_manager_init(const module_config_t *config) {
    if (g_module_manager.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize module manager structure
    memset(&g_module_manager, 0, sizeof(g_module_manager));
    
    // Set configuration
    if (config != NULL) {
        memcpy(&g_module_manager.config, config, sizeof(module_config_t));
    } else {
        memcpy(&g_module_manager.config, &default_config, sizeof(module_config_t));
    }
    
    // Initialize status
    g_module_manager.status.initialized = true;
    g_module_manager.status.discovery_active = false;
    g_module_manager.status.health_monitoring_active = false;
    g_module_manager.status.total_registered_modules = 0;
    g_module_manager.status.last_update_time = hal_get_timestamp_us();
    
    // Initialize statistics
    memset(&g_module_manager.status.statistics, 0, sizeof(module_stats_t));
    
    // Initialize module entries
    for (int i = 0; i < MAX_MODULES; i++) {
        g_module_manager.modules[i].registered = false;
        memset(&g_module_manager.modules[i].info, 0, sizeof(module_info_t));
        memset(&g_module_manager.modules[i].status, 0, sizeof(module_status_info_t));
        g_module_manager.modules[i].last_health_check = 0;
        g_module_manager.modules[i].discovery_time = 0;
    }
    
    g_module_manager.initialized = true;
    g_module_manager.event_callback = NULL;
    g_module_manager.last_discovery_time = 0;
    g_module_manager.last_health_check_time = 0;
    g_module_manager.discovery_sequence = 0;
    
    // Trigger initialization event
    handle_module_event(MODULE_EVENT_NONE, 0, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t module_manager_deinit(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Stop discovery and health monitoring
    module_manager_stop_discovery();
    
    // Clear all modules
    for (int i = 0; i < MAX_MODULES; i++) {
        if (g_module_manager.modules[i].registered) {
            handle_module_event(MODULE_EVENT_OFFLINE, g_module_manager.modules[i].info.module_id, NULL);
        }
    }
    
    // Clear module manager
    memset(&g_module_manager, 0, sizeof(g_module_manager));
    
    return HAL_STATUS_OK;
}

hal_status_t module_manager_update(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = hal_get_timestamp_us();
    g_module_manager.status.last_update_time = current_time;
    
    // Perform auto-discovery if enabled
    if (g_module_manager.config.auto_discovery_enabled && 
        g_module_manager.status.discovery_active) {
        
        if (current_time - g_module_manager.last_discovery_time >= 
            (g_module_manager.config.discovery_timeout_ms * 1000ULL)) {
            
            perform_module_discovery();
            g_module_manager.last_discovery_time = current_time;
        }
    }
    
    // Perform health monitoring if enabled
    if (g_module_manager.config.health_monitoring_enabled && 
        g_module_manager.status.health_monitoring_active) {
        
        if (current_time - g_module_manager.last_health_check_time >= 
            (g_module_manager.config.health_check_interval_ms * 1000ULL)) {
            
            module_manager_health_check_all();
            g_module_manager.last_health_check_time = current_time;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t module_manager_get_status(module_mgr_status_t *status) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(status, &g_module_manager.status, sizeof(module_mgr_status_t));
    return HAL_STATUS_OK;
}

hal_status_t module_manager_get_config(module_config_t *config) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_module_manager.config, sizeof(module_config_t));
    return HAL_STATUS_OK;
}

hal_status_t module_manager_set_config(const module_config_t *config) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_module_manager.config, config, sizeof(module_config_t));
    handle_module_event(MODULE_EVENT_CONFIG_CHANGE, 0, config);
    
    return HAL_STATUS_OK;
}

hal_status_t module_manager_start_discovery(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_module_manager.status.discovery_active = true;
    g_module_manager.last_discovery_time = hal_get_timestamp_us();
    
    return HAL_STATUS_OK;
}

hal_status_t module_manager_stop_discovery(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_module_manager.status.discovery_active = false;
    
    return HAL_STATUS_OK;
}

hal_status_t module_manager_register_module(uint8_t module_id, const module_info_t *info) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (!is_module_id_valid(module_id) || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_module_index(module_id);
    if (index >= 0) {
        return HAL_STATUS_ALREADY_ACTIVE; // Module already registered
    }
    
    // Find free slot
    for (int i = 0; i < MAX_MODULES; i++) {
        if (!g_module_manager.modules[i].registered) {
            g_module_manager.modules[i].registered = true;
            memcpy(&g_module_manager.modules[i].info, info, sizeof(module_info_t));
            
            // Initialize status
            g_module_manager.modules[i].status.status = MODULE_STATUS_ONLINE;
            g_module_manager.modules[i].status.health = MODULE_HEALTH_GOOD;
            g_module_manager.modules[i].status.health_percentage = 90;
            g_module_manager.modules[i].status.uptime_seconds = 0;
            g_module_manager.modules[i].status.error_count = 0;
            g_module_manager.modules[i].status.warning_count = 0;
            g_module_manager.modules[i].status.last_communication = hal_get_timestamp_us();
            g_module_manager.modules[i].status.response_time_ms = 0;
            g_module_manager.modules[i].status.timeout_count = 0;
            g_module_manager.modules[i].status.crc_error_count = 0;
            
            g_module_manager.modules[i].last_health_check = hal_get_timestamp_us();
            g_module_manager.modules[i].discovery_time = hal_get_timestamp_us();
            
            g_module_manager.status.total_registered_modules++;
            g_module_manager.status.statistics.total_modules++;
            g_module_manager.status.statistics.online_modules++;
            g_module_manager.status.statistics.successful_discoveries++;
            
            handle_module_event(MODULE_EVENT_REGISTERED, module_id, info);
            
            return HAL_STATUS_OK;
        }
    }
    
    return HAL_STATUS_ERROR; // No free slots
}

hal_status_t module_manager_unregister_module(uint8_t module_id) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (!is_module_id_valid(module_id)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_module_index(module_id);
    if (index < 0) {
        return HAL_STATUS_ERROR; // Module not found
    }
    
    // Update statistics
    if (g_module_manager.modules[index].status.status == MODULE_STATUS_ONLINE) {
        g_module_manager.status.statistics.online_modules--;
    } else if (g_module_manager.modules[index].status.status == MODULE_STATUS_ERROR) {
        g_module_manager.status.statistics.error_modules--;
    } else if (g_module_manager.modules[index].status.status == MODULE_STATUS_OFFLINE) {
        g_module_manager.status.statistics.offline_modules--;
    }
    
    g_module_manager.status.total_registered_modules--;
    g_module_manager.status.statistics.total_modules--;
    
    // Clear module entry
    g_module_manager.modules[index].registered = false;
    memset(&g_module_manager.modules[index].info, 0, sizeof(module_info_t));
    memset(&g_module_manager.modules[index].status, 0, sizeof(module_status_info_t));
    g_module_manager.modules[index].last_health_check = 0;
    g_module_manager.modules[index].discovery_time = 0;
    
    handle_module_event(MODULE_EVENT_OFFLINE, module_id, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t module_manager_get_module_info(uint8_t module_id, module_info_t *info) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (!is_module_id_valid(module_id) || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_module_index(module_id);
    if (index < 0) {
        return HAL_STATUS_ERROR; // Module not found
    }
    
    memcpy(info, &g_module_manager.modules[index].info, sizeof(module_info_t));
    return HAL_STATUS_OK;
}

hal_status_t module_manager_get_module_status(uint8_t module_id, module_status_info_t *status) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (!is_module_id_valid(module_id) || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_module_index(module_id);
    if (index < 0) {
        return HAL_STATUS_ERROR; // Module not found
    }
    
    memcpy(status, &g_module_manager.modules[index].status, sizeof(module_status_info_t));
    return HAL_STATUS_OK;
}

hal_status_t module_manager_get_registered_modules(uint8_t *module_ids, uint32_t max_count, uint32_t *actual_count) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (module_ids == NULL || actual_count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    uint32_t count = 0;
    for (int i = 0; i < MAX_MODULES && count < max_count; i++) {
        if (g_module_manager.modules[i].registered) {
            module_ids[count++] = g_module_manager.modules[i].info.module_id;
        }
    }
    
    *actual_count = count;
    return HAL_STATUS_OK;
}

hal_status_t module_manager_health_check_module(uint8_t module_id) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (!is_module_id_valid(module_id)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check if module is registered
    int module_index = find_module_index(module_id);
    if (module_index == -1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return perform_health_check(module_id);
}

hal_status_t module_manager_health_check_all(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    hal_status_t overall_status = HAL_STATUS_OK;
    
    for (int i = 0; i < MAX_MODULES; i++) {
        if (g_module_manager.modules[i].registered) {
            hal_status_t status = perform_health_check(g_module_manager.modules[i].info.module_id);
            if (status != HAL_STATUS_OK) {
                overall_status = status;
            }
        }
    }
    
    return overall_status;
}

hal_status_t module_manager_set_callback(module_event_callback_t callback) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_module_manager.event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t module_manager_get_statistics(module_stats_t *stats) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(stats, &g_module_manager.status.statistics, sizeof(module_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t module_manager_reset_statistics(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&g_module_manager.status.statistics, 0, sizeof(module_stats_t));
    return HAL_STATUS_OK;
}

const char* module_manager_get_type_name(module_type_t type) {
    switch (type) {
        case MODULE_TYPE_UNKNOWN: return "UNKNOWN";
        case MODULE_TYPE_SENSOR: return "SENSOR";
        case MODULE_TYPE_ACTUATOR: return "ACTUATOR";
        case MODULE_TYPE_CONTROLLER: return "CONTROLLER";
        case MODULE_TYPE_SAFETY: return "SAFETY";
        case MODULE_TYPE_COMMUNICATION: return "COMMUNICATION";
        case MODULE_TYPE_STORAGE: return "STORAGE";
        case MODULE_TYPE_DISPLAY: return "DISPLAY";
        default: return "INVALID";
    }
}

const char* module_manager_get_status_name(module_status_t status) {
    switch (status) {
        case MODULE_STATUS_UNKNOWN: return "UNKNOWN";
        case MODULE_STATUS_OFFLINE: return "OFFLINE";
        case MODULE_STATUS_ONLINE: return "ONLINE";
        case MODULE_STATUS_ERROR: return "ERROR";
        case MODULE_STATUS_WARNING: return "WARNING";
        case MODULE_STATUS_MAINTENANCE: return "MAINTENANCE";
        default: return "INVALID";
    }
}

const char* module_manager_get_health_name(module_health_t health) {
    switch (health) {
        case MODULE_HEALTH_UNKNOWN: return "UNKNOWN";
        case MODULE_HEALTH_EXCELLENT: return "EXCELLENT";
        case MODULE_HEALTH_GOOD: return "GOOD";
        case MODULE_HEALTH_FAIR: return "FAIR";
        case MODULE_HEALTH_POOR: return "POOR";
        case MODULE_HEALTH_CRITICAL: return "CRITICAL";
        case MODULE_HEALTH_FAILED: return "FAILED";
        default: return "INVALID";
    }
}

const char* module_manager_get_event_name(module_event_t event) {
    switch (event) {
        case MODULE_EVENT_NONE: return "NONE";
        case MODULE_EVENT_DISCOVERED: return "DISCOVERED";
        case MODULE_EVENT_REGISTERED: return "REGISTERED";
        case MODULE_EVENT_ONLINE: return "ONLINE";
        case MODULE_EVENT_OFFLINE: return "OFFLINE";
        case MODULE_EVENT_ERROR: return "ERROR";
        case MODULE_EVENT_WARNING: return "WARNING";
        case MODULE_EVENT_HEALTH_CHANGE: return "HEALTH_CHANGE";
        case MODULE_EVENT_CONFIG_CHANGE: return "CONFIG_CHANGE";
        default: return "INVALID";
    }
}

hal_status_t module_manager_self_test(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Test module registration
    module_info_t test_info = {
        .module_id = 100, // Test module ID (within valid range 1-247)
        .type = MODULE_TYPE_SENSOR,
        .capabilities = 0x12345678,
        .config_flags = 0
    };
    strcpy(test_info.name, "TEST_MODULE");
    strcpy(test_info.version, "1.0.0");
    strcpy(test_info.serial_number, "TEST123456");
    
    hal_status_t status = module_manager_register_module(100, &test_info);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Test module retrieval
    module_info_t retrieved_info;
    status = module_manager_get_module_info(100, &retrieved_info);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Test module unregistration
    status = module_manager_unregister_module(100);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t module_manager_reset(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Unregister all modules
    for (int i = 0; i < MAX_MODULES; i++) {
        if (g_module_manager.modules[i].registered) {
            module_manager_unregister_module(g_module_manager.modules[i].info.module_id);
        }
    }
    
    // Reset statistics
    module_manager_reset_statistics();
    
    // Reset status
    g_module_manager.status.discovery_active = false;
    g_module_manager.status.health_monitoring_active = false;
    g_module_manager.status.total_registered_modules = 0;
    g_module_manager.status.last_update_time = hal_get_timestamp_us();
    
    return HAL_STATUS_OK;
}

// Internal helper functions

static hal_status_t perform_module_discovery(void) {
    g_module_manager.status.statistics.discovery_attempts++;
    
    // Use Communication Manager to discover modules via Modbus
    // Scan common Modbus slave addresses (1-247)
    for (uint8_t slave_id = 1; slave_id <= 10; slave_id++) { // Limit to first 10 for testing
        uint16_t data[1];
        hal_status_t status = comm_manager_modbus_read_holding_registers(slave_id, 0, 1, data);
        
        if (status == HAL_STATUS_OK) {
            // Module found - try to register it
            module_info_t module_info = {
                .module_id = slave_id,
                .type = MODULE_TYPE_SENSOR, // Default type
                .capabilities = 0,
                .config_flags = 0
            };
            
            sprintf(module_info.name, "MODULE_%d", slave_id);
            strcpy(module_info.version, "1.0.0");
            sprintf(module_info.serial_number, "SN%06d", slave_id);
            
            // Check if module is already registered
            int index = find_module_index(slave_id);
            if (index < 0) {
                // New module discovered
                handle_module_event(MODULE_EVENT_DISCOVERED, slave_id, &module_info);
                module_manager_register_module(slave_id, &module_info);
            }
        }
    }
    
    g_module_manager.status.statistics.last_discovery_time = hal_get_timestamp_us();
    
    return HAL_STATUS_OK;
}

static hal_status_t perform_health_check(uint8_t module_id) {
    int index = find_module_index(module_id);
    if (index < 0) {
        return HAL_STATUS_ERROR;
    }
    
    module_entry_t *module = &g_module_manager.modules[index];
    module_status_info_t old_status = module->status;
    
    // Perform health check via Communication Manager
    uint16_t data[1];
    uint64_t start_time = hal_get_timestamp_us();
    
    hal_status_t status = comm_manager_modbus_read_holding_registers(module_id, 0, 1, data);
    
    uint64_t end_time = hal_get_timestamp_us();
    uint32_t response_time = (uint32_t)((end_time - start_time) / 1000ULL);
    
    // Update module status
    module->status.last_communication = hal_get_timestamp_us();
    module->status.response_time_ms = response_time;
    module->last_health_check = hal_get_timestamp_us();
    
    if (status == HAL_STATUS_OK) {
        // Module is healthy
        if (module->status.status != MODULE_STATUS_ONLINE) {
            module->status.status = MODULE_STATUS_ONLINE;
            g_module_manager.status.statistics.online_modules++;
            
            if (old_status.status == MODULE_STATUS_ERROR) {
                g_module_manager.status.statistics.error_modules--;
            } else if (old_status.status == MODULE_STATUS_OFFLINE) {
                g_module_manager.status.statistics.offline_modules--;
            }
            
            handle_module_event(MODULE_EVENT_ONLINE, module_id, NULL);
        }
        
        // Calculate health percentage based on response time
        uint8_t health_percentage = 100;
        if (response_time > g_module_manager.config.max_response_time_ms) {
            health_percentage = 100 - ((response_time - g_module_manager.config.max_response_time_ms) / 10);
            if (health_percentage < 20) health_percentage = 20;
        }
        
        module->status.health_percentage = health_percentage;
        module->status.health = get_health_level(health_percentage);
        
        if (module->status.health != old_status.health) {
            handle_module_event(MODULE_EVENT_HEALTH_CHANGE, module_id, &health_percentage);
        }
        
    } else {
        // Module is not responding
        module->status.timeout_count++;
        
        if (module->status.status == MODULE_STATUS_ONLINE) {
            module->status.status = MODULE_STATUS_OFFLINE;
            g_module_manager.status.statistics.online_modules--;
            g_module_manager.status.statistics.offline_modules++;
            handle_module_event(MODULE_EVENT_OFFLINE, module_id, NULL);
        }
        
        module->status.health_percentage = 0;
        module->status.health = MODULE_HEALTH_FAILED;
    }
    
    return HAL_STATUS_OK;
}

static void handle_module_event(module_event_t event, uint8_t module_id, const void *data) {
    if (g_module_manager.event_callback != NULL) {
        g_module_manager.event_callback(event, module_id, data);
    }
}

static uint8_t calculate_health_percentage(const module_status_info_t *status) {
    uint8_t percentage = 100;
    
    // Reduce health based on error count
    percentage -= (status->error_count * 10);
    if (percentage < 0) percentage = 0;
    
    // Reduce health based on warning count
    percentage -= (status->warning_count * 5);
    if (percentage < 0) percentage = 0;
    
    // Reduce health based on timeout count
    percentage -= (status->timeout_count * 5);
    if (percentage < 0) percentage = 0;
    
    // Reduce health based on CRC error count
    percentage -= (status->crc_error_count * 3);
    if (percentage < 0) percentage = 0;
    
    return percentage;
}

static module_health_t get_health_level(uint8_t percentage) {
    if (percentage >= 100) return MODULE_HEALTH_EXCELLENT;
    if (percentage >= 80) return MODULE_HEALTH_GOOD;
    if (percentage >= 60) return MODULE_HEALTH_FAIR;
    if (percentage >= 40) return MODULE_HEALTH_POOR;
    if (percentage >= 20) return MODULE_HEALTH_CRITICAL;
    return MODULE_HEALTH_FAILED;
}

static bool is_module_id_valid(uint8_t module_id) {
    return module_id > 0 && module_id <= 247; // Valid Modbus slave ID range
}

static int find_module_index(uint8_t module_id) {
    for (int i = 0; i < MAX_MODULES; i++) {
        if (g_module_manager.modules[i].registered && 
            g_module_manager.modules[i].info.module_id == module_id) {
            return i;
        }
    }
    return -1; // Module not found
}
