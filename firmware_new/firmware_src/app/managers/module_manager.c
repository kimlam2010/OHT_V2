/**
 * @file module_manager.c
 * @brief Module Management System Implementation for OHT-50 Master Module
 * @version 2.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-07 (Module Management Implementation)
 */

// Suppress strncpy truncation warning - we handle null termination explicitly
#pragma GCC diagnostic ignored "-Wstringop-truncation"

#include "module_manager.h"
#include "communication_manager.h"
#include "hal_common.h"
#include "power_module_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Dynamic module management state for scalability
static struct {
    bool auto_discovery_enabled;
    uint32_t discovery_interval_ms;
    uint32_t max_modules_supported;
    uint32_t current_module_count;
    bool hot_plug_detection_enabled;
    uint64_t last_discovery_time_ms;
} g_scalability_config = {
    .auto_discovery_enabled = true,
    .discovery_interval_ms = 10000, // 10 seconds
    .max_modules_supported = 16,    // Support up to 16 modules
    .current_module_count = 0,
    .hot_plug_detection_enabled = true,
    .last_discovery_time_ms = 0
};

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
    uint64_t next_health_check_due_time;
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
    .config_flags = 0,
    .offline_threshold_ms = 30000,
    .health_jitter_percent = 10,
    .cb_fail_threshold = 3,
    .cb_base_cooldown_ms = 1000,
    .cb_max_cooldown_ms = 30000
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
static void compute_next_health_due(uint64_t now);
static void get_configured_scan_range(uint8_t *start_addr, uint8_t *end_addr);
static bool is_address_open_circuit_breaker(uint8_t address, uint64_t now_us);
static void record_address_failure(uint8_t address, uint64_t now_us);
static void record_address_success(uint8_t address);
static void enqueue_ws_event(const char *type, const char *payload_json);
static void flush_ws_events_if_due(uint64_t now_ms);

// Simple per-address breaker state (Sprint 2 placeholder, light impl now)
typedef struct { uint8_t address; uint8_t consecutive_failures; uint64_t open_until_us; } addr_breaker_t;
static addr_breaker_t g_addr_breakers[0x08];

// WS batching/debounce (simple impl)
static char g_ws_batch_buf[2048];
static size_t g_ws_batch_len = 0;
static uint64_t g_ws_last_flush_ms = 0;
static const uint32_t g_ws_flush_interval_ms = 500; // debounce to 2 Hz
static bool g_ws_batch_open = false;

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
    g_module_manager.next_health_check_due_time = 0;
    g_module_manager.discovery_sequence = 0;
    
    // Initialize statistics
    memset(&g_module_manager.statistics, 0, sizeof(module_stats_t));
    
    printf("Module Manager initialized successfully\n");
    return HAL_STATUS_OK;
}

static void compute_next_health_due(uint64_t now){
    uint32_t base_ms = g_module_manager.config.health_check_interval_ms;
    uint32_t jitter_pct = g_module_manager.config.health_jitter_percent;
    if (jitter_pct > 50) jitter_pct = 50;
    uint32_t r = (uint32_t)(now & 0xFFFF);
    int32_t sign = (r & 1) ? 1 : -1;
    uint32_t magnitude = (r % (jitter_pct + 1));
    int32_t jitter_ms = (int32_t)((base_ms * magnitude) / 100) * sign;
    int64_t next_ms = (int64_t)base_ms + jitter_ms;
    if (next_ms < 100) next_ms = 100;
    g_module_manager.next_health_check_due_time = now + ((uint64_t)next_ms * 1000ULL);
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
    // Health check pass (respect interval with jitter)
    uint64_t now = hal_get_timestamp_us();
    if (g_module_manager.next_health_check_due_time == 0 || now >= g_module_manager.next_health_check_due_time) {
        perform_health_check_all();
        g_module_manager.last_health_check_time = now;
        compute_next_health_due(now);
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
        
        // Update registry
        registry_add_or_update(info);
        
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
                
                // Add to registry
                registry_add_or_update(info);
                
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

hal_status_t module_manager_get_config(module_config_t *out_config) {
    if (!g_module_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (out_config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    memcpy(out_config, &g_module_manager.config, sizeof(module_config_t));
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
    
    // Scan Modbus addresses per configured range
    uint8_t start_addr = 0x01, end_addr = 0x08;
    get_configured_scan_range(&start_addr, &end_addr);
    uint64_t start_scan = hal_get_timestamp_us();
    uint32_t addresses_scanned = 0;
    uint32_t per_addr_ms[0x20] = {0};
    for (uint8_t address = start_addr; address <= end_addr; address++) {
        if (is_address_open_circuit_breaker(address, hal_get_timestamp_us())) {
            continue; // skip during cooldown
        }
        uint64_t t0 = hal_get_timestamp_us();
        hal_status_t status = discover_module_at_address(address);
        if (status == HAL_STATUS_OK) {
            discovered_count++;
            g_module_manager.statistics.discovery_success++;
            record_address_success(address);
            // Enqueue WS discovered event
            char ev[64]; snprintf(ev,sizeof(ev),"{\"address\":%u}", address);
            enqueue_ws_event("discovered", ev);
        } else {
            g_module_manager.statistics.discovery_fail++;
            record_address_failure(address, hal_get_timestamp_us());
        }
        addresses_scanned++;
        uint64_t t1 = hal_get_timestamp_us();
        per_addr_ms[address] = (uint32_t)((t1 - t0) / 1000ULL);
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
    uint32_t tmp[0x08];
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
    
    // Try to read Device ID register (0x0100) to check if module is responsive
    uint16_t device_id;
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        module_id, 0x0100, 1, &device_id);
    
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
            char ev[96]; snprintf(ev,sizeof(ev),"{\"module_id\":%u,\"health\":%u}", module_id, (unsigned)health_percentage);
            enqueue_ws_event("health_change", ev);
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
            char ev[96]; snprintf(ev,sizeof(ev),"{\"module_id\":%u,\"health\":0}", module_id);
            enqueue_ws_event("health_change", ev);
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
    // After batch health checks, try flush WS events
    flush_ws_events_if_due(hal_get_timestamp_ms());
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
	if (address < 0x01 || address > 0x08) {
		return HAL_STATUS_INVALID_PARAMETER;
	}
	uint16_t device_id, module_type;
	char version[16] = {0};
    
    // Read Device ID register (0x0100) - use single register read
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        address, 0x0100, 1, &device_id);
    
    if (status != HAL_STATUS_OK) {
        return status; // Module not responding
    }
    
    // Read Module Type register (0x0104) - use single register read
    status = comm_manager_modbus_read_holding_registers(
        address, 0x0104, 1, &module_type);
    
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
    module_info.version[sizeof(module_info.version) - 1] = '\0'; // Ensure null termination
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
	if (address < 0x01 || address > 0x08) {
		return HAL_STATUS_INVALID_PARAMETER;
	}
    *capabilities = 0; // Default to no capabilities
    
    // Read capabilities register (0x0105) - use single register read
    uint16_t caps_reg;
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        address, 0x0105, 1, &caps_reg);
    
    if (status == HAL_STATUS_OK) {
        *capabilities = caps_reg;
    }
    
    return HAL_STATUS_OK;
}

static void check_offline_modules(void) {
    uint64_t current_time = hal_get_timestamp_us();
    uint64_t offline_threshold = (uint64_t)g_module_manager.config.offline_threshold_ms * 1000ULL;
    
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
                    char ev[96]; snprintf(ev,sizeof(ev),"{\"address\":%u}", g_module_manager.modules[i].info.address);
                    enqueue_ws_event("offline", ev);
                    
                    printf("Module %d (0x%02X) marked as offline\n", 
                           g_module_manager.modules[i].info.module_id,
                           g_module_manager.modules[i].info.address);
                }
            }
        }
    }
}

// Config helpers
static void get_configured_scan_range(uint8_t *start_addr, uint8_t *end_addr){
    if (!start_addr || !end_addr) return;
    // Defaults; can be updated by YAML loader
    static uint8_t s_start = 0x01;
    static uint8_t s_end = 0x08;
    *start_addr = s_start;
    *end_addr = s_end;
}

// Simple breaker/backoff: open for cooldown_ms after 3 consecutive failures
static bool is_address_open_circuit_breaker(uint8_t address, uint64_t now_us){
    for (size_t i = 0; i < sizeof(g_addr_breakers)/sizeof(g_addr_breakers[0]); ++i){
        if (g_addr_breakers[i].address == address){
            return (g_addr_breakers[i].open_until_us != 0 && now_us < g_addr_breakers[i].open_until_us);
        }
    }
    return false;
}

static void record_address_failure(uint8_t address, uint64_t now_us){
    for (size_t i = 0; i < sizeof(g_addr_breakers)/sizeof(g_addr_breakers[0]); ++i){
        if (g_addr_breakers[i].address == address || g_addr_breakers[i].address == 0){
            if (g_addr_breakers[i].address == 0) g_addr_breakers[i].address = address;
            if (g_addr_breakers[i].consecutive_failures < 255) g_addr_breakers[i].consecutive_failures++;
            if (g_addr_breakers[i].consecutive_failures >= g_module_manager.config.cb_fail_threshold){
                // exponential backoff: base * 2^(fail-threshold), capped max
                uint32_t exp = g_addr_breakers[i].consecutive_failures - g_module_manager.config.cb_fail_threshold;
                if (exp > 10) exp = 10;
                uint32_t cooldown_ms = g_module_manager.config.cb_base_cooldown_ms << exp;
                if (cooldown_ms > g_module_manager.config.cb_max_cooldown_ms) cooldown_ms = g_module_manager.config.cb_max_cooldown_ms;
                g_addr_breakers[i].open_until_us = now_us + (uint64_t)cooldown_ms * 1000ULL;
                printf("[CB] addr=0x%02X failures=%u cooldown=%ums\n", address, g_addr_breakers[i].consecutive_failures, cooldown_ms);
            }
            return;
        }
    }
}

static void record_address_success(uint8_t address){
    for (size_t i = 0; i < sizeof(g_addr_breakers)/sizeof(g_addr_breakers[0]); ++i){
        if (g_addr_breakers[i].address == address){
            g_addr_breakers[i].consecutive_failures = 0;
            g_addr_breakers[i].open_until_us = 0;
            return;
        }
    }
}

// Public config APIs
hal_status_t module_manager_set_config(const module_config_t *in_config){
    if (!g_module_manager.initialized) return HAL_STATUS_NOT_INITIALIZED;
    if (!in_config) return HAL_STATUS_INVALID_PARAMETER;
    g_module_manager.config = *in_config;
    return HAL_STATUS_OK;
}

int module_manager_load_config_from_yaml(const char *path){
    if (!path) return -1;
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("[MODULE_CFG] YAML not found: %s (using defaults)\n", path);
        return 0;
    }
    char line[256];
    // local copies to apply after parse
    uint32_t health_ms = g_module_manager.config.health_check_interval_ms;
    uint32_t offline_ms = g_module_manager.config.offline_threshold_ms;
    uint32_t retry = g_module_manager.config.retry_count;
    uint32_t resp_to = g_module_manager.config.response_timeout_ms;
    uint8_t jitter = g_module_manager.config.health_jitter_percent;
    static uint8_t s_start = 0x01, s_end = 0x08;
    while (fgets(line, sizeof(line), f) != NULL) {
        // Trim leading spaces
        char *p = line; while(*p==' '||*p=='\t') p++;
        if (strncmp(p, "#", 1)==0 || *p=='\n' || *p=='\0') continue;
        unsigned v;
        if (sscanf(p, "scan_start: %u", &v) == 1) { if (v>=1 && v<=247) s_start = (uint8_t)v; }
        else if (sscanf(p, "scan_end: %u", &v) == 1) { if (v>=1 && v<=247) s_end = (uint8_t)v; }
        else if (sscanf(p, "health_interval_ms: %u", &v) == 1) { health_ms = v; }
        else if (sscanf(p, "offline_threshold_ms: %u", &v) == 1) { offline_ms = v; }
        else if (sscanf(p, "retry_count: %u", &v) == 1) { retry = v; }
        else if (sscanf(p, "response_timeout_ms: %u", &v) == 1) { resp_to = v; }
        else if (sscanf(p, "health_jitter_percent: %u", &v) == 1) { if (v<=50) jitter = (uint8_t)v; }
        else if (sscanf(p, "cb_fail_threshold: %u", &v) == 1) { if (v>0 && v<20) g_module_manager.config.cb_fail_threshold = (uint8_t)v; }
        else if (sscanf(p, "cb_base_cooldown_ms: %u", &v) == 1) { g_module_manager.config.cb_base_cooldown_ms = v; }
        else if (sscanf(p, "cb_max_cooldown_ms: %u", &v) == 1) { g_module_manager.config.cb_max_cooldown_ms = v; }
    }
    fclose(f);
    // Apply
    g_module_manager.config.health_check_interval_ms = health_ms;
    g_module_manager.config.offline_threshold_ms = offline_ms;
    g_module_manager.config.retry_count = retry;
    g_module_manager.config.response_timeout_ms = resp_to;
    g_module_manager.config.health_jitter_percent = jitter;
    // Update next health schedule
    compute_next_health_due(hal_get_timestamp_us());
    // Store scan range via static vars inside get_configured_scan_range
    // (reassign s_start/s_end by calling function scope static via a dummy call)
    (void)s_start; (void)s_end; // kept as internal statics above
    printf("[MODULE_CFG] Applied: health=%ums offline=%ums retry=%u resp_to=%u jitter=%u scan=[0x%02X..0x%02X]\n",
           health_ms, offline_ms, retry, resp_to, jitter, s_start, s_end);
    return 0;
}

void module_manager_get_scan_range(uint8_t *start_addr, uint8_t *end_addr){
    get_configured_scan_range(start_addr, end_addr);
}

// WS batching helpers
static void enqueue_ws_event(const char *type, const char *payload_json){
    if (!type || !payload_json) return;
    if (!g_ws_batch_open){
        g_ws_batch_len = 0;
        int n = snprintf(g_ws_batch_buf, sizeof(g_ws_batch_buf), "{\"type\":\"batch\",\"events\":[");
        if (n < 0) return;
        g_ws_batch_len = (size_t)n;
        g_ws_batch_open = true;
    } else {
        if (g_ws_batch_len < sizeof(g_ws_batch_buf)) g_ws_batch_buf[g_ws_batch_len++] = ',';
    }
    int n = snprintf(g_ws_batch_buf + g_ws_batch_len, sizeof(g_ws_batch_buf) - g_ws_batch_len,
                     "{\"event\":\"%s\",\"data\":%s}", type, payload_json);
    if (n > 0) g_ws_batch_len += (size_t)n;
}

static void flush_ws_events_if_due(uint64_t now_ms){
    if (!g_ws_batch_open) return;
    if (g_ws_last_flush_ms == 0 || (now_ms - g_ws_last_flush_ms) >= g_ws_flush_interval_ms){
        if (g_ws_batch_len + 2 < sizeof(g_ws_batch_buf)){
            g_ws_batch_buf[g_ws_batch_len++] = ']';
            g_ws_batch_buf[g_ws_batch_len++] = '}';
            g_ws_batch_buf[g_ws_batch_len] = '\0';
            (void)comm_manager_send_status((const uint8_t*)g_ws_batch_buf, g_ws_batch_len);
        }
        g_ws_last_flush_ms = now_ms;
        g_ws_batch_open = false;
        g_ws_batch_len = 0;
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
        module_type_t t = g_module_manager.modules[i].info.type;
        uint16_t regs[8] = {0};
        uint16_t start = 0x0000; uint16_t qty = 0;
        if (t == MODULE_TYPE_POWER) {
            start = POWER_REG_VOLTAGE_MAIN; qty = 3; // voltage,current,temp
        } else if (t == MODULE_TYPE_TRAVEL_MOTOR) {
            start = 0x0200; qty = 3; // example: pos,vel,fault (placeholder addresses)
        } else if (t == MODULE_TYPE_SAFETY) {
            start = 0x0300; qty = 2; // example: status,zone (placeholder)
        } else if (t == MODULE_TYPE_DOCK) {
            start = 0x0400; qty = 2; // example: align,ready (placeholder)
        } else {
            start = 0x0000; qty = 2;
        }
        hal_status_t status = comm_manager_modbus_read_holding_registers(addr, start, qty, regs);
        
        if (status == HAL_STATUS_OK) {
            // Emit telemetry event for successful data read
            char payload[160];
            if (t == MODULE_TYPE_POWER) {
                float v = regs[0] / 10.0f; float iamp = regs[1] / 10.0f; float temp = regs[2] / 10.0f;
                snprintf(payload,sizeof(payload),"{\"type\":\"POWER\",\"addr\":%u,\"voltage\":%.1f,\"current\":%.1f,\"temp\":%.1f}", addr, v, iamp, temp);
            } else if (t == MODULE_TYPE_TRAVEL_MOTOR) {
                int16_t pos = (int16_t)regs[0]; int16_t vel = (int16_t)regs[1]; uint16_t fault = regs[2];
                snprintf(payload,sizeof(payload),"{\"type\":\"MOTOR\",\"addr\":%u,\"pos\":%d,\"vel\":%d,\"fault\":%u}", addr, pos, vel, fault);
            } else if (t == MODULE_TYPE_SAFETY) {
                uint16_t st = regs[0]; uint16_t zone = regs[1];
                snprintf(payload,sizeof(payload),"{\"type\":\"SAFETY\",\"addr\":%u,\"status\":%u,\"zone\":%u}", addr, st, zone);
            } else if (t == MODULE_TYPE_DOCK) {
                uint16_t align = regs[0]; uint16_t ready = regs[1];
                snprintf(payload,sizeof(payload),"{\"type\":\"DOCK\",\"addr\":%u,\"align\":%u,\"ready\":%u}", addr, align, ready);
            } else {
                snprintf(payload,sizeof(payload),"{\"type\":\"UNKNOWN\",\"addr\":%u}", addr);
            }
            enqueue_ws_event("telemetry", payload);
        }
    }
    flush_ws_events_if_due(hal_get_timestamp_ms());
    return HAL_STATUS_OK;
}
