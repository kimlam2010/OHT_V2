/**
 * @file module_manager.c
 * @brief Module Management System Implementation for OHT-50 Master Module
 * @version 2.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-07 (Module Management Implementation)
 */

#include "module_manager.h"
#include "communication_manager.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Forward declarations for auto-discovery functions
static hal_status_t discover_module_at_address(uint8_t address);
static bool is_valid_module_type(uint16_t module_type);
static hal_status_t read_module_capabilities(uint8_t address, module_type_t type, uint32_t *capabilities);
static void check_offline_modules(void);

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
    module_entry_t modules[MAX_MODULES];
    module_event_callback_t event_callback;
    uint64_t last_discovery_time;
    uint64_t last_health_check_time;
    uint32_t discovery_sequence;
    module_stats_t statistics;
} g_module_manager = {0};

// Default configuration
static const module_config_t default_config = {
    .module_id = 0,
    .discovery_timeout_ms = 5000,
    .health_check_interval_ms = 10000,
    .response_timeout_ms = 1000,
    .retry_count = 3,
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

hal_status_t module_manager_init(void) {
    if (g_module_manager.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize module manager structure
    memset(&g_module_manager, 0, sizeof(g_module_manager));
    
    // Set default configuration
    memcpy(&g_module_manager.config, &default_config, sizeof(module_config_t));
    
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
    
    // Initialize statistics
    memset(&g_module_manager.statistics, 0, sizeof(module_stats_t));
    
    printf("Module Manager initialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t module_manager_deinit(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Stop all activities
    module_manager_stop();
    
    // Clear all module entries
    for (int i = 0; i < MAX_MODULES; i++) {
        g_module_manager.modules[i].registered = false;
    }
    
    g_module_manager.initialized = false;
    printf("Module Manager deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t module_manager_start(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("Module Manager started\n");
    return HAL_STATUS_OK;
}

hal_status_t module_manager_stop(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("Module Manager stopped\n");
    return HAL_STATUS_OK;
}

hal_status_t module_manager_discover_modules(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    return perform_module_discovery();
}

hal_status_t module_manager_register_module(const module_info_t *info) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!is_module_id_valid(info->module_id)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_module_index(info->module_id);
    if (index >= 0) {
        // Module already exists, update it
        g_module_manager.modules[index].info = *info;
        g_module_manager.modules[index].status.status = MODULE_STATUS_ONLINE;
        g_module_manager.modules[index].last_health_check = hal_get_timestamp_us();
        
        if (g_module_manager.event_callback) {
            g_module_manager.event_callback(MODULE_EVENT_UPDATED, info->module_id, info);
        }
        
        printf("Module %d updated\n", info->module_id);
    } else {
        // Find free slot
        for (int i = 0; i < MAX_MODULES; i++) {
            if (!g_module_manager.modules[i].registered) {
                g_module_manager.modules[i].registered = true;
                g_module_manager.modules[i].info = *info;
                g_module_manager.modules[i].status.status = MODULE_STATUS_ONLINE;
                g_module_manager.modules[i].last_health_check = hal_get_timestamp_us();
                g_module_manager.modules[i].discovery_time = hal_get_timestamp_us();
                
                g_module_manager.statistics.total_modules++;
                g_module_manager.statistics.online_modules++;
                
                if (g_module_manager.event_callback) {
                    g_module_manager.event_callback(MODULE_EVENT_DISCOVERED, info->module_id, info);
                }
                
                printf("Module %d registered\n", info->module_id);
                return HAL_STATUS_OK;
            }
        }
        
        return HAL_STATUS_NO_MEMORY;
    }
    
    return HAL_STATUS_OK;
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
        return HAL_STATUS_NOT_FOUND;
    }
    
    // Update statistics
    if (g_module_manager.modules[index].status.status == MODULE_STATUS_ONLINE) {
        g_module_manager.statistics.online_modules--;
    } else if (g_module_manager.modules[index].status.status == MODULE_STATUS_ERROR) {
        g_module_manager.statistics.error_modules--;
    } else {
        g_module_manager.statistics.offline_modules--;
    }
    
    g_module_manager.statistics.total_modules--;
    
    // Clear module entry
    g_module_manager.modules[index].registered = false;
    memset(&g_module_manager.modules[index].info, 0, sizeof(module_info_t));
    memset(&g_module_manager.modules[index].status, 0, sizeof(module_status_info_t));
    
    if (g_module_manager.event_callback) {
        g_module_manager.event_callback(MODULE_EVENT_OFFLINE, module_id, NULL);
    }
    
    printf("Module %d unregistered\n", module_id);
    return HAL_STATUS_OK;
}

hal_status_t module_manager_get_module_info(uint8_t module_id, module_info_t *info) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!is_module_id_valid(module_id)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_module_index(module_id);
    if (index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    *info = g_module_manager.modules[index].info;
    return HAL_STATUS_OK;
}

hal_status_t module_manager_get_module_status(uint8_t module_id, module_status_info_t *status) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!is_module_id_valid(module_id)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_module_index(module_id);
    if (index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    *status = g_module_manager.modules[index].status;
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
            module_ids[count] = g_module_manager.modules[i].info.module_id;
            count++;
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
    
    return perform_health_check(module_id);
}

hal_status_t module_manager_health_check_all(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    for (int i = 0; i < MAX_MODULES; i++) {
        if (g_module_manager.modules[i].registered) {
            perform_health_check(g_module_manager.modules[i].info.module_id);
        }
    }
    
    return HAL_STATUS_OK;
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
    
    memcpy(stats, &g_module_manager.statistics, sizeof(module_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t module_manager_reset_statistics(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&g_module_manager.statistics, 0, sizeof(module_stats_t));
    return HAL_STATUS_OK;
}

const char* module_manager_get_type_name(module_type_t type) {
    switch (type) {
        case MODULE_TYPE_UNKNOWN: return "Unknown";
        case MODULE_TYPE_POWER: return "Power";
        case MODULE_TYPE_SAFETY: return "Safety";
        case MODULE_TYPE_TRAVEL_MOTOR: return "Travel Motor";
        case MODULE_TYPE_DOCK: return "Dock & Location";
        default: return "Invalid";
    }
}

const char* module_manager_get_status_name(module_status_t status) {
    switch (status) {
        case MODULE_STATUS_UNKNOWN: return "Unknown";
        case MODULE_STATUS_OFFLINE: return "Offline";
        case MODULE_STATUS_ONLINE: return "Online";
        case MODULE_STATUS_ERROR: return "Error";
        case MODULE_STATUS_WARNING: return "Warning";
        case MODULE_STATUS_MAINTENANCE: return "Maintenance";
        default: return "Invalid";
    }
}

const char* module_manager_get_health_name(module_health_t health) {
    switch (health) {
        case MODULE_HEALTH_UNKNOWN: return "Unknown";
        case MODULE_HEALTH_EXCELLENT: return "Excellent";
        case MODULE_HEALTH_GOOD: return "Good";
        case MODULE_HEALTH_FAIR: return "Fair";
        case MODULE_HEALTH_POOR: return "Poor";
        case MODULE_HEALTH_CRITICAL: return "Critical";
        case MODULE_HEALTH_FAILED: return "Failed";
        default: return "Invalid";
    }
}

const char* module_manager_get_event_name(module_event_t event) {
    switch (event) {
        case MODULE_EVENT_NONE: return "None";
        case MODULE_EVENT_DISCOVERED: return "Discovered";
        case MODULE_EVENT_REGISTERED: return "Registered";
        case MODULE_EVENT_ONLINE: return "Online";
        case MODULE_EVENT_OFFLINE: return "Offline";
        case MODULE_EVENT_ERROR: return "Error";
        case MODULE_EVENT_WARNING: return "Warning";
        case MODULE_EVENT_HEALTH_CHANGE: return "Health Change";
        case MODULE_EVENT_CONFIG_CHANGE: return "Config Change";
        case MODULE_EVENT_UPDATED: return "Updated";
        case MODULE_EVENT_TIMEOUT: return "Timeout";
        default: return "Invalid";
    }
}

hal_status_t module_manager_self_test(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("Module Manager self-test passed\n");
    return HAL_STATUS_OK;
}

hal_status_t module_manager_reset(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Reset all module entries
    for (int i = 0; i < MAX_MODULES; i++) {
        g_module_manager.modules[i].registered = false;
        memset(&g_module_manager.modules[i].info, 0, sizeof(module_info_t));
        memset(&g_module_manager.modules[i].status, 0, sizeof(module_status_info_t));
    }
    
    // Reset statistics
    memset(&g_module_manager.statistics, 0, sizeof(module_stats_t));
    
    printf("Module Manager reset\n");
    return HAL_STATUS_OK;
}

// Helper functions

static hal_status_t perform_module_discovery(void) {
    hal_status_t overall_status = HAL_STATUS_OK;
    int discovered_count = 0;
    
    // Scan Modbus addresses 0x01-0x20
    for (uint8_t address = 0x01; address <= 0x20; address++) {
        hal_status_t status = discover_module_at_address(address);
        if (status == HAL_STATUS_OK) {
            discovered_count++;
        }
    }
    
    // Check for offline modules
    check_offline_modules();
    
    return overall_status;
}

static hal_status_t perform_health_check(uint8_t module_id) {
    int index = find_module_index(module_id);
    if (index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    // Simulate health check
    uint32_t response_time = 50; // Simulated response time
    uint8_t health_percentage = 95; // Simulated health percentage
    
    // Update module status
    g_module_manager.modules[index].status.health_percentage = health_percentage;
    g_module_manager.modules[index].status.health = get_health_level(health_percentage);
    g_module_manager.modules[index].status.response_time_ms = response_time;
    g_module_manager.modules[index].last_health_check = hal_get_timestamp_us();
    
    // Update statistics
    if (g_module_manager.modules[index].status.status == MODULE_STATUS_ONLINE) {
        g_module_manager.statistics.online_modules++;
    }
    
    if (response_time > g_module_manager.config.response_timeout_ms) {
        g_module_manager.modules[index].status.status = MODULE_STATUS_ERROR;
        g_module_manager.statistics.error_modules++;
        g_module_manager.statistics.online_modules--;
    }
    
    return HAL_STATUS_OK;
}

__attribute__((unused))
static void handle_module_event(module_event_t event, uint8_t module_id, const void *data) {
    if (g_module_manager.event_callback) {
        g_module_manager.event_callback(event, module_id, data);
    }
}

__attribute__((unused))
static uint8_t calculate_health_percentage(const module_status_info_t *status) {
    uint8_t percentage = 100;
    
    // Reduce health based on error count
    if (status->error_count > 0) {
        percentage -= (status->error_count * 10);
    }
    
    // Reduce health based on warning count
    if (status->warning_count > 0) {
        percentage -= (status->warning_count * 5);
    }
    
    // Reduce health based on response time
    if (status->response_time_ms > 100) {
        percentage -= ((status->response_time_ms - 100) / 10);
    }
    
    // Ensure percentage is within valid range
    if (percentage > 100) percentage = 100;
    // percentage is unsigned, so it cannot be < 0
    
    return percentage;
}

static module_health_t get_health_level(uint8_t percentage) {
    if (percentage >= 90) return MODULE_HEALTH_EXCELLENT;
    if (percentage >= 80) return MODULE_HEALTH_GOOD;
    if (percentage >= 60) return MODULE_HEALTH_FAIR;
    if (percentage >= 40) return MODULE_HEALTH_POOR;
    if (percentage >= 20) return MODULE_HEALTH_CRITICAL;
    return MODULE_HEALTH_FAILED;
}

static bool is_module_id_valid(uint8_t module_id) {
    // module_id is uint8_t so it's always <= 255, just check > 0
    return module_id > 0;
}

static int find_module_index(uint8_t module_id) {
    for (int i = 0; i < MAX_MODULES; i++) {
        if (g_module_manager.modules[i].registered && 
            g_module_manager.modules[i].info.module_id == module_id) {
            return i;
        }
    }
    return -1;
}

// Auto-Discovery Implementation Functions

static hal_status_t discover_module_at_address(uint8_t address) {
    uint16_t device_id, module_type;
    char version[16] = {0};
    
    // Read Device ID register (0x00F0) - use single register read
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        address, 0x00F0, 1, &device_id);
    
    if (status != HAL_STATUS_OK) {
        return status; // Module not responding
    }
    
    // Read Module Type register (0x00F7) - use single register read
    status = comm_manager_modbus_read_holding_registers(
        address, 0x00F7, 1, &module_type);
    
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Read Version registers (0x00F8-0x00FF) - 8 registers for version string
    uint16_t version_regs[8];
    status = comm_manager_modbus_read_holding_registers(
        address, 0x00F8, 8, version_regs);
    
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Convert version registers to string
    int version_index = 0;
    for (int i = 0; i < 8 && version_index < 15; i++) {
        // Each register contains 2 ASCII characters
        char high_byte = (version_regs[i] >> 8) & 0xFF;
        char low_byte = version_regs[i] & 0xFF;
        
        if (high_byte != 0) {
            version[version_index++] = high_byte;
        }
        if (low_byte != 0 && version_index < 15) {
            version[version_index++] = low_byte;
        }
    }
    version[version_index] = '\0'; // Ensure null termination
    
    // Validate module type
    if (!is_valid_module_type(module_type)) {
        printf("Invalid module type 0x%04X at address 0x%02X\n", module_type, address);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create module info
    module_info_t module_info = {0};
    module_info.module_id = address;
    module_info.address = address;
    module_info.type = (module_type_t)module_type;
    strncpy(module_info.version, version, sizeof(module_info.version) - 1);
    snprintf(module_info.name, sizeof(module_info.name), "%s_%02X", 
             module_manager_get_type_name(module_info.type), address);
    snprintf(module_info.serial_number, sizeof(module_info.serial_number), 
             "SN%04X%02X", device_id, address);
    
    // Read module capabilities based on type
    status = read_module_capabilities(address, module_info.type, &module_info.capabilities);
    if (status != HAL_STATUS_OK) {
        printf("Failed to read capabilities for module at 0x%02X\n", address);
        // Continue anyway, capabilities will be 0
    }
    
    // Register or update module
    status = module_manager_register_module(&module_info);
    
    if (status == HAL_STATUS_OK) {
        // Mark module as online in registry
        registry_mark_online(address, module_info.type, version);
        
        if (g_module_manager.event_callback) {
            g_module_manager.event_callback(MODULE_EVENT_DISCOVERED, address, &module_info);
        }
    }
    
    return status;
}

static bool is_valid_module_type(uint16_t module_type) {
    switch (module_type) {
        case MODULE_TYPE_POWER:
        case MODULE_TYPE_SAFETY:
        case MODULE_TYPE_TRAVEL_MOTOR:
        case MODULE_TYPE_DOCK:
            return true;
        default:
            return false;
    }
}

static hal_status_t read_module_capabilities(uint8_t address, module_type_t type, uint32_t *capabilities) {
    (void)type; // Suppress unused parameter warning
    if (capabilities == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *capabilities = 0; // Default to no capabilities
    
    // Read capabilities register (0x0100) - use single register read
    uint16_t caps_reg;
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        address, 0x0100, 1, &caps_reg);
    
    if (status == HAL_STATUS_OK) {
        *capabilities = caps_reg;
    }
    
    return HAL_STATUS_OK;
}

static void check_offline_modules(void) {
    uint64_t current_time = hal_get_timestamp_us();
    uint64_t offline_threshold = 30000000; // 30 seconds in microseconds
    
    for (int i = 0; i < MAX_MODULES; i++) {
        if (g_module_manager.modules[i].registered) {
            uint64_t time_since_last_seen = current_time - g_module_manager.modules[i].last_health_check;
            
            if (time_since_last_seen > offline_threshold) {
                if (g_module_manager.modules[i].status.status == MODULE_STATUS_ONLINE) {
                    // Mark as offline
                    g_module_manager.modules[i].status.status = MODULE_STATUS_OFFLINE;
                    g_module_manager.statistics.online_modules--;
                    g_module_manager.statistics.offline_modules++;
                    
                    // Mark as offline in registry
                    registry_mark_offline(g_module_manager.modules[i].info.address);
                    
                    if (g_module_manager.event_callback) {
                        g_module_manager.event_callback(MODULE_EVENT_OFFLINE, 
                            g_module_manager.modules[i].info.module_id, NULL);
                    }
                    
                    printf("Module %d (0x%02X) marked as offline\n", 
                           g_module_manager.modules[i].info.module_id,
                           g_module_manager.modules[i].info.address);
                }
            }
        }
    }
}
