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
#include "power_module_handler.h"
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
static hal_status_t perform_health_check_all(void);
static hal_status_t poll_registered_modules_and_push_telemetry(void);
static void handle_module_event(module_event_t event, uint8_t module_id, const void *data);
static uint8_t calculate_health_percentage(const module_status_info_t *status);
static module_health_t get_health_level(uint8_t percentage);
static bool is_module_id_valid(uint8_t module_id);
static int find_module_index(uint8_t module_id);
static uint32_t get_default_capabilities(module_type_t type);

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
    // Ensure communication manager is initialized before discovery
    comm_mgr_status_info_t comm_status;
    hal_status_t comm_status_result = comm_manager_get_status(&comm_status);
    printf("[MODULE] Communication manager status check: result=%d, status=%d\n", 
           comm_status_result, comm_status.status);
    
    if (comm_status_result != HAL_STATUS_OK ||
        comm_status.status == COMM_MGR_STATUS_DISCONNECTED) {
        printf("[MODULE] Module discovery blocked: communication manager not ready\n");
        return HAL_STATUS_INVALID_STATE;
    }
    
    printf("[MODULE] Starting module discovery...\n");
    return perform_module_discovery();
}

hal_status_t module_manager_update(void) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    // Health check pass (respect default interval)
    uint64_t now = hal_get_timestamp_us();
    if (now - g_module_manager.last_health_check_time >= (uint64_t)g_module_manager.config.health_check_interval_ms * 1000ULL) {
        perform_health_check_all();
        g_module_manager.last_health_check_time = now;
    }
    // Poll data and push telemetry (non-blocking per module)
    return poll_registered_modules_and_push_telemetry();
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
    
    if (max_count == 0) {
        *actual_count = 0;
        return HAL_STATUS_OK;
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
            (void)perform_health_check(g_module_manager.modules[i].info.module_id);
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
    
    // Scan Modbus addresses 0x01-0x20 per spec
    uint64_t start_scan = hal_get_timestamp_us();
    uint32_t addresses_scanned = 0;
    uint32_t per_addr_ms[0x20] = {0};
    for (uint8_t address = 0x01; address <= 0x20; address++) {
        uint64_t t0 = hal_get_timestamp_us();
        hal_status_t status = discover_module_at_address(address);
        if (status == HAL_STATUS_OK) {
            discovered_count++;
            g_module_manager.statistics.discovery_success++;
        } else {
            g_module_manager.statistics.discovery_fail++;
        }
        addresses_scanned++;
        uint64_t t1 = hal_get_timestamp_us();
        per_addr_ms[address - 1] = (uint32_t)((t1 - t0) / 1000ULL);
    }
    
    // Check for offline modules
    check_offline_modules();
    uint64_t end_scan = hal_get_timestamp_us();
    uint32_t total_ms = (uint32_t)((end_scan - start_scan) / 1000ULL);
    printf("[MODULE] Discovery scan complete: discovered=%d, scanned=%u, total_ms=%u\n", discovered_count, addresses_scanned, total_ms);
    g_module_manager.statistics.discovery_total_ms = total_ms;
    g_module_manager.statistics.discovery_count++;
    // Compute p95/p99 from per-address durations (non-zero entries)
    // Simple selection approach: copy to temp and sort
    uint32_t tmp[0x20];
    uint32_t n = 0;
    for (uint32_t i = 0; i < 0x20; i++) {
        if (per_addr_ms[i] > 0) tmp[n++] = per_addr_ms[i];
    }
    for (uint32_t i = 0; i + 1 < n; i++) {
        for (uint32_t j = i + 1; j < n; j++) {
            if (tmp[j] < tmp[i]) { uint32_t v = tmp[i]; tmp[i] = tmp[j]; tmp[j] = v; }
        }
    }
    if (n > 0) {
        uint32_t idx95 = (n * 95) / 100; if (idx95 >= n) idx95 = n - 1;
        uint32_t idx99 = (n * 99) / 100; if (idx99 >= n) idx99 = n - 1;
        g_module_manager.statistics.discovery_p95_ms = tmp[idx95];
        g_module_manager.statistics.discovery_p99_ms = tmp[idx99];
    } else {
        g_module_manager.statistics.discovery_p95_ms = 0;
        g_module_manager.statistics.discovery_p99_ms = 0;
    }
    
    return overall_status;
}

static hal_status_t perform_health_check(uint8_t module_id) {
    int index = find_module_index(module_id);
    if (index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    // Track previous health for change detection
    uint8_t prev_health = g_module_manager.modules[index].status.health_percentage;
    
    // Real health check: measure response time and check module status
    uint64_t start_time = hal_get_timestamp_us();
    
    // Try to read Device ID register (0x00F0) to check if module is responsive
    uint16_t device_id;
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        module_id, 0x00F0, 1, &device_id);
    
    uint64_t end_time = hal_get_timestamp_us();
    uint32_t response_time = (uint32_t)((end_time - start_time) / 1000ULL); // Convert to ms
    
    if (status == HAL_STATUS_OK) {
        // Module is responsive, calculate health percentage
        // Spec formula: 100 − (response_time_ms / 1000) − (error_count * 10)
        uint32_t error_count = g_module_manager.modules[index].status.error_count;
        int32_t computed = 100 - (int32_t)(response_time / 1000U) - (int32_t)(error_count * 10U);
        if (computed < 0) computed = 0;
        if (computed > 100) computed = 100;
        uint8_t health_percentage = (uint8_t)computed;
        
        // Update module status
        g_module_manager.modules[index].status.health_percentage = health_percentage;
        g_module_manager.modules[index].status.health = get_health_level(health_percentage);
        g_module_manager.modules[index].status.response_time_ms = response_time;
        g_module_manager.modules[index].status.status = MODULE_STATUS_ONLINE;
        g_module_manager.modules[index].last_health_check = hal_get_timestamp_us();
        g_module_manager.modules[index].status.last_communication = g_module_manager.modules[index].last_health_check;
        
        // Emit health change event if health changed
        if (prev_health != health_percentage) {
            if (g_module_manager.event_callback) {
                g_module_manager.event_callback(MODULE_EVENT_HEALTH_CHANGE, module_id, &health_percentage);
            }
        }
        
        g_module_manager.statistics.health_checks++;
        printf("[MODULE] Health check for module %u: response_time=%ums, health=%u%%\n", 
               module_id, response_time, health_percentage);
    } else {
        // Module not responsive
        g_module_manager.modules[index].status.health_percentage = 0;
        g_module_manager.modules[index].status.health = MODULE_HEALTH_FAILED;
        g_module_manager.modules[index].status.response_time_ms = response_time;
        g_module_manager.modules[index].status.status = MODULE_STATUS_ERROR;
        g_module_manager.modules[index].last_health_check = hal_get_timestamp_us();
        g_module_manager.modules[index].status.error_count++;
        g_module_manager.statistics.health_timeouts++;
        
        // Emit health change event (health went to 0)
        if (prev_health > 0) {
            if (g_module_manager.event_callback) {
                g_module_manager.event_callback(MODULE_EVENT_HEALTH_CHANGE, module_id, &g_module_manager.modules[index].status.health_percentage);
            }
        }
        
        printf("[MODULE] Health check failed for module %u: status=%d\n", module_id, status);
    }
    
    return status;
}

static hal_status_t perform_health_check_all(void) {
    for (int i = 0; i < MAX_MODULES; i++) {
        if (g_module_manager.modules[i].registered) {
            (void)perform_health_check(g_module_manager.modules[i].info.module_id);
        }
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
	if (address < 0x01 || address > 0x20) {
		return HAL_STATUS_INVALID_PARAMETER;
	}
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
    
    // Try to read Version registers (0x00F8-0x00FF) - optional, not all modules support this
    uint16_t version_regs[8];
    status = comm_manager_modbus_read_holding_registers(
        address, 0x00F8, 8, version_regs);
    
    if (status == HAL_STATUS_OK) {
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
        printf("[MODULE] Version read successfully: '%s'\n", version);
    } else {
        // Version registers not supported, use default version
        strcpy(version, "v1.0.0");
        printf("[MODULE] Version registers not supported, using default: '%s'\n", version);
    }
    
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
    
    // Try to read module capabilities based on type (optional)
    status = read_module_capabilities(address, module_info.type, &module_info.capabilities);
    if (status == HAL_STATUS_OK) {
        printf("[MODULE] Capabilities read successfully for module 0x%02X\n", address);
    } else {
        printf("[MODULE] Capabilities not supported for module 0x%02X, using defaults\n", address);
        // Use default capabilities based on module type
        module_info.capabilities = get_default_capabilities(module_info.type);
    }
    
    printf("[MODULE] Attempting to register module: address=0x%02X, type=0x%04X, device_id=0x%04X\n", 
           address, module_info.type, device_id);
    
    // Register or update module
    status = module_manager_register_module(&module_info);
    printf("[MODULE] module_manager_register_module result: %d\n", status);
    
    if (status == HAL_STATUS_OK) {
        // Mark module as online in registry
        registry_mark_online(address, module_info.type, version);
        printf("[MODULE] Module registered successfully: address=0x%02X, type=0x%04X\n", address, module_info.type);
        
        // Call module-specific auto-detect for power modules
        if (module_info.type == MODULE_TYPE_POWER) {
            printf("[MODULE] Calling power module auto-detect for address 0x%02X\n", address);
            hal_status_t auto_detect_status = power_module_handler_auto_detect(address, 1000);
            printf("[MODULE] Power auto-detect result: %d\n", auto_detect_status);
        }
        
        if (g_module_manager.event_callback) {
            g_module_manager.event_callback(MODULE_EVENT_DISCOVERED, address, &module_info);
        }
    } else {
        printf("[MODULE] Failed to register module: address=0x%02X, status=%d\n", address, status);
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
	(void)type;
	if (capabilities == NULL) {
		return HAL_STATUS_INVALID_PARAMETER;
	}
	if (address < 0x01 || address > 0x20) {
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

static uint32_t get_default_capabilities(module_type_t type) {
    uint32_t capabilities = 0;
    
    switch (type) {
        case MODULE_TYPE_POWER:
            capabilities = 0x07;  // FC3|FC6|FC16 supported
            break;
            
        case MODULE_TYPE_SAFETY:
            capabilities = 0x03;  // FC3|FC6 supported
            break;
            
        case MODULE_TYPE_TRAVEL_MOTOR:
            capabilities = 0x07;  // FC3|FC6|FC16 supported
            break;
            
        case MODULE_TYPE_DOCK:
            capabilities = 0x03;  // FC3|FC6 supported
            break;
            
        default:
            capabilities = 0x03;  // Default to FC3|FC6
            break;
    }
    
    return capabilities;
}

// Polling and Telemetry Hook (basic)
static hal_status_t poll_registered_modules_and_push_telemetry(void) {
    // Per-module minimal polling without blocking
    for (int i = 0; i < MAX_MODULES; i++) {
        if (!g_module_manager.modules[i].registered) continue;
        uint8_t addr = g_module_manager.modules[i].info.address;
        
        // Read status registers based on module type
        uint16_t buf[4] = {0};
        hal_status_t status = comm_manager_modbus_read_holding_registers(addr, 0x0000, 2, buf);
        
        if (status == HAL_STATUS_OK) {
            // Emit telemetry event for successful data read
            if (g_module_manager.event_callback) {
                g_module_manager.event_callback(MODULE_EVENT_UPDATED, addr, buf);
            }
        }
    }
    return HAL_STATUS_OK;
}
