/**
 * @file module_manager.h
 * @brief Module Management System for OHT-50 Master Module
 * @version 2.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-07 (Module Management Implementation)
 */

#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include "hal_common.h"
#include "communication_manager.h"
#include "system_state_machine.h"

#ifdef __cplusplus
extern "C" {
#endif

// Max modules supported in registry
#define MODULE_REGISTRY_MAX_MODULES 16

// Module Types (Unified from module_registry.h and module_manager.h)
typedef enum {
    MODULE_TYPE_UNKNOWN = 0,
    MODULE_TYPE_POWER = 0x02,        // Power module (0x02)
    MODULE_TYPE_SAFETY = 0x03,       // Safety module (0x03)
    MODULE_TYPE_TRAVEL_MOTOR = 0x04, // Travel Motor module (0x04)
    MODULE_TYPE_DOCK = 0x05,         // Dock & Location module (0x05)
    MODULE_TYPE_MAX
} module_type_t;

// Module Status (Unified)
typedef enum {
    MODULE_STATUS_UNKNOWN = 0,
    MODULE_STATUS_OFFLINE,         // Module not responding
    MODULE_STATUS_ONLINE,          // Module responding normally
    MODULE_STATUS_ERROR,           // Module in error state
    MODULE_STATUS_WARNING,         // Module in warning state
    MODULE_STATUS_MAINTENANCE,     // Module in maintenance mode
    MODULE_STATUS_MAX
} module_status_t;

// Module Health Levels
typedef enum {
    MODULE_HEALTH_UNKNOWN = 0,
    MODULE_HEALTH_EXCELLENT,       // 100% healthy
    MODULE_HEALTH_GOOD,            // 80-99% healthy
    MODULE_HEALTH_FAIR,            // 60-79% healthy
    MODULE_HEALTH_POOR,            // 40-59% healthy
    MODULE_HEALTH_CRITICAL,        // 20-39% healthy
    MODULE_HEALTH_FAILED,          // 0-19% healthy
    MODULE_HEALTH_MAX
} module_health_t;

// Module Discovery Events (Unified)
typedef enum {
    MODULE_EVENT_NONE = 0,
    MODULE_EVENT_DISCOVERED,       // New module discovered
    MODULE_EVENT_REGISTERED,       // Module registered successfully
    MODULE_EVENT_ONLINE,           // Module came online
    MODULE_EVENT_OFFLINE,          // Module went offline
    MODULE_EVENT_ERROR,            // Module error detected
    MODULE_EVENT_WARNING,          // Module warning detected
    MODULE_EVENT_HEALTH_CHANGE,    // Module health changed
    MODULE_EVENT_CONFIG_CHANGE,    // Module configuration changed
    MODULE_EVENT_UPDATED,          // Module updated
    MODULE_EVENT_TIMEOUT,          // Module timeout
    MODULE_EVENT_MAX
} module_event_t;

// Power module capabilities
#define POWER_CAP_VOLTAGE_MONITOR     (1 << 0)
#define POWER_CAP_CURRENT_MONITOR     (1 << 1)
#define POWER_CAP_TEMP_MONITOR        (1 << 2)
#define POWER_CAP_RELAY_CONTROL       (1 << 3)
#define POWER_CAP_OVERVOLTAGE_PROTECT (1 << 4)
#define POWER_CAP_OVERCURRENT_PROTECT (1 << 5)
#define POWER_CAP_OVERTEMP_PROTECT    (1 << 6)

// Power module registers (Modbus)
#define POWER_REG_VOLTAGE_MAIN        0x0000  // Main voltage (V * 10)
#define POWER_REG_CURRENT_MAIN        0x0001  // Main current (A * 10)
#define POWER_REG_TEMP_MAIN           0x0002  // Main temperature (°C * 10)
#define POWER_REG_RELAY1_STATUS       0x0004  // Relay 1 status (0=OFF, 1=ON)
#define POWER_REG_RELAY2_STATUS       0x0005  // Relay 2 status (0=OFF, 1=ON)
#define POWER_REG_RELAY1_CONTROL      0x0006  // Relay 1 control (0=OFF, 1=ON)
#define POWER_REG_RELAY2_CONTROL      0x0007  // Relay 2 control (0=OFF, 1=ON)
#define POWER_REG_ALARM_STATUS        0x0008  // Alarm status (bitmap)
#define POWER_REG_DEVICE_ID           0x00F0  // Device ID
#define POWER_REG_MODULE_TYPE         0x00F7  // Module type (0x0002 = Power)

// Module Information Structure (Unified)
typedef struct {
    uint8_t module_id;             // Unique module ID
    uint8_t address;               // Modbus/RS485 address
    module_type_t type;            // Module type
    char name[32];                 // Module name
    char version[16];              // Module version
    char serial_number[32];        // Module serial number
    uint32_t capabilities;         // Module capabilities bitmap
    uint32_t config_flags;         // Configuration flags
    module_status_t status;        // Online/Offline
    uint64_t last_seen_ms;         // Last seen timestamp (ms)
} module_info_t;

// Module Status Structure
typedef struct {
    module_status_t status;        // Current status
    module_health_t health;        // Health level (0-100%)
    uint8_t health_percentage;     // Health percentage
    uint32_t uptime_seconds;       // Module uptime
    uint32_t error_count;          // Error count
    uint32_t warning_count;        // Warning count
    uint64_t last_communication;   // Last communication time
    uint32_t response_time_ms;     // Average response time
    uint32_t timeout_count;        // Timeout count
    uint32_t crc_error_count;      // CRC error count
} module_status_info_t;

// Module Configuration Structure
typedef struct {
    uint8_t module_id;             // Module ID
    uint32_t discovery_timeout_ms; // Discovery timeout
    uint32_t health_check_interval_ms; // Health check interval
    uint32_t response_timeout_ms;  // Response timeout
    uint32_t retry_count;          // Retry count
    uint32_t config_flags;         // Configuration flags
} module_config_t;

// Module Statistics Structure
typedef struct {
    uint32_t total_modules;        // Total modules discovered
    uint32_t online_modules;       // Online modules
    uint32_t offline_modules;      // Offline modules
    uint32_t error_modules;        // Modules in error
    uint32_t discovery_count;      // Discovery attempts
    uint32_t timeout_count;        // Timeout count
    uint32_t error_count;          // Error count
    uint64_t total_uptime;         // Total uptime
    // Week1 metrics additions
    uint32_t discovery_success;     // Successful discoveries
    uint32_t discovery_fail;        // Failed discoveries
    uint32_t discovery_total_ms;    // Total time for last full-bus scan
    uint32_t discovery_p95_ms;      // p95 per-address discovery duration (last scan)
    uint32_t discovery_p99_ms;      // p99 per-address discovery duration (last scan)
    uint32_t health_checks;         // Number of health checks performed
    uint32_t health_timeouts;       // Health check timeouts
} module_stats_t;

// Event callback function type
typedef void (*module_event_callback_t)(module_event_t event, uint8_t module_id, const void *data);

// Module Manager API
hal_status_t module_manager_init(void);
hal_status_t module_manager_deinit(void);
hal_status_t module_manager_start(void);
hal_status_t module_manager_stop(void);

// Module Discovery
hal_status_t module_manager_discover_modules(void);
// Periodic polling/update for registered modules (data polling + telemetry hook)
hal_status_t module_manager_update(void);
hal_status_t module_manager_register_module(const module_info_t *info);
hal_status_t module_manager_unregister_module(uint8_t module_id);

// Module Information
hal_status_t module_manager_get_module_info(uint8_t module_id, module_info_t *info);
hal_status_t module_manager_get_module_status(uint8_t module_id, module_status_info_t *status);
hal_status_t module_manager_get_registered_modules(uint8_t *module_ids, uint32_t max_count, uint32_t *actual_count);

// Module Health
hal_status_t module_manager_health_check_module(uint8_t module_id);
hal_status_t module_manager_health_check_all(void);

// Event Handling
hal_status_t module_manager_set_callback(module_event_callback_t callback);

// Statistics
hal_status_t module_manager_get_statistics(module_stats_t *stats);
hal_status_t module_manager_reset_statistics(void);

// Utility Functions
const char* module_manager_get_type_name(module_type_t type);
const char* module_manager_get_status_name(module_status_t status);
const char* module_manager_get_health_name(module_health_t health);
const char* module_manager_get_event_name(module_event_t event);

// Self Test
hal_status_t module_manager_self_test(void);
hal_status_t module_manager_reset(void);

// Registry API (from module_registry.h)
int registry_init(void);
int registry_deinit(void);
int registry_clear(void);
int registry_add_or_update(const module_info_t *info);
int registry_mark_online(uint8_t address, module_type_t type, const char *version);
int registry_mark_offline(uint8_t address);
int registry_set_meta(uint8_t address, const char *name, module_type_t type);
int registry_get(uint8_t address, module_info_t *out);
int registry_get_all(module_info_t *out_array, size_t max_items, size_t *actual_count);
size_t registry_list(module_info_t *out_array, size_t max_items);
size_t registry_count_online(void);
bool registry_has_offline_saved(void);
void registry_set_event_callback(module_event_callback_t cb);
int registry_load_yaml(const char *path);
int registry_save_yaml(const char *path);
void registry_set_scanning(bool scanning);
bool registry_is_scanning(void);

#ifdef __cplusplus
}
#endif

#endif // MODULE_MANAGER_H
