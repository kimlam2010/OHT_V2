/**
 * @file module_manager.h
 * @brief Module Management System for OHT-50 Master Module
 * @version 1.0.0
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

// Module Types
typedef enum {
    MODULE_TYPE_UNKNOWN = 0,
    MODULE_TYPE_SENSOR,           // Sensor modules (LiDAR, encoders, etc.)
    MODULE_TYPE_ACTUATOR,         // Actuator modules (motors, relays, etc.)
    MODULE_TYPE_CONTROLLER,       // Controller modules (PID, motion, etc.)
    MODULE_TYPE_SAFETY,           // Safety modules (E-Stop, interlocks, etc.)
    MODULE_TYPE_COMMUNICATION,    // Communication modules (RS485, CAN, etc.)
    MODULE_TYPE_STORAGE,          // Storage modules (SD card, flash, etc.)
    MODULE_TYPE_DISPLAY,          // Display modules (LCD, LED, etc.)
    MODULE_TYPE_MAX
} module_type_t;

// Module Status
typedef enum {
    MODULE_STATUS_UNKNOWN = 0,
    MODULE_STATUS_OFFLINE,        // Module not responding
    MODULE_STATUS_ONLINE,         // Module responding normally
    MODULE_STATUS_ERROR,          // Module in error state
    MODULE_STATUS_WARNING,        // Module in warning state
    MODULE_STATUS_MAINTENANCE,    // Module in maintenance mode
    MODULE_STATUS_MAX
} module_status_t;

// Module Health Levels
typedef enum {
    MODULE_HEALTH_UNKNOWN = 0,
    MODULE_HEALTH_EXCELLENT,      // 100% healthy
    MODULE_HEALTH_GOOD,           // 80-99% healthy
    MODULE_HEALTH_FAIR,           // 60-79% healthy
    MODULE_HEALTH_POOR,           // 40-59% healthy
    MODULE_HEALTH_CRITICAL,       // 20-39% healthy
    MODULE_HEALTH_FAILED,         // 0-19% healthy
    MODULE_HEALTH_MAX
} module_health_t;

// Module Discovery Events
typedef enum {
    MODULE_EVENT_NONE = 0,
    MODULE_EVENT_DISCOVERED,      // New module discovered
    MODULE_EVENT_REGISTERED,      // Module registered successfully
    MODULE_EVENT_ONLINE,          // Module came online
    MODULE_EVENT_OFFLINE,         // Module went offline
    MODULE_EVENT_ERROR,           // Module error detected
    MODULE_EVENT_WARNING,         // Module warning detected
    MODULE_EVENT_HEALTH_CHANGE,   // Module health changed
    MODULE_EVENT_CONFIG_CHANGE,   // Module configuration changed
    MODULE_EVENT_MAX
} module_event_t;

// Module Information Structure
typedef struct {
    uint8_t module_id;            // Unique module ID
    module_type_t type;           // Module type
    char name[32];                // Module name
    char version[16];             // Module version
    char serial_number[32];       // Module serial number
    uint32_t capabilities;        // Module capabilities bitmap
    uint32_t config_flags;        // Configuration flags
} module_info_t;

// Module Status Structure
typedef struct {
    module_status_t status;       // Current status
    module_health_t health;       // Health level (0-100%)
    uint8_t health_percentage;    // Health percentage
    uint32_t uptime_seconds;      // Module uptime
    uint32_t error_count;         // Error count
    uint32_t warning_count;       // Warning count
    uint64_t last_communication;  // Last communication time
    uint32_t response_time_ms;    // Average response time
    uint32_t timeout_count;       // Timeout count
    uint32_t crc_error_count;     // CRC error count
} module_status_info_t;

// Module Configuration Structure
typedef struct {
    uint8_t module_id;            // Module ID
    uint32_t discovery_timeout_ms; // Discovery timeout
    uint32_t health_check_interval_ms; // Health check interval
    uint32_t max_response_time_ms; // Maximum response time
    uint32_t retry_count;         // Retry count for failed operations
    bool auto_discovery_enabled;  // Enable auto-discovery
    bool health_monitoring_enabled; // Enable health monitoring
    uint32_t config_flags;        // Configuration flags
} module_config_t;

// Module Statistics Structure
typedef struct {
    uint32_t total_modules;       // Total modules discovered
    uint32_t online_modules;      // Online modules
    uint32_t offline_modules;     // Offline modules
    uint32_t error_modules;       // Modules in error
    uint32_t discovery_attempts;  // Discovery attempts
    uint32_t successful_discoveries; // Successful discoveries
    uint32_t failed_discoveries;  // Failed discoveries
    uint64_t last_discovery_time; // Last discovery time
    uint32_t average_discovery_time_ms; // Average discovery time
} module_stats_t;

// Module Manager Status Structure
typedef struct {
    bool initialized;             // Module manager initialized
    bool discovery_active;        // Discovery process active
    bool health_monitoring_active; // Health monitoring active
    module_stats_t statistics;    // Module statistics
    uint32_t total_registered_modules; // Total registered modules
    uint64_t last_update_time;    // Last update time
} module_mgr_status_t;

// Module Event Callback
typedef void (*module_event_callback_t)(module_event_t event, uint8_t module_id, const void *data);

// Public API Functions

/**
 * @brief Initialize Module Manager
 * @param config Configuration structure (NULL for defaults)
 * @return HAL status
 */
hal_status_t module_manager_init(const module_config_t *config);

/**
 * @brief Deinitialize Module Manager
 * @return HAL status
 */
hal_status_t module_manager_deinit(void);

/**
 * @brief Update Module Manager
 * @return HAL status
 */
hal_status_t module_manager_update(void);

/**
 * @brief Get Module Manager status
 * @param status Status structure
 * @return HAL status
 */
hal_status_t module_manager_get_status(module_mgr_status_t *status);

/**
 * @brief Get Module Manager configuration
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t module_manager_get_config(module_config_t *config);

/**
 * @brief Set Module Manager configuration
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t module_manager_set_config(const module_config_t *config);

/**
 * @brief Start module discovery process
 * @return HAL status
 */
hal_status_t module_manager_start_discovery(void);

/**
 * @brief Stop module discovery process
 * @return HAL status
 */
hal_status_t module_manager_stop_discovery(void);

/**
 * @brief Register a module manually
 * @param module_id Module ID
 * @param info Module information
 * @return HAL status
 */
hal_status_t module_manager_register_module(uint8_t module_id, const module_info_t *info);

/**
 * @brief Unregister a module
 * @param module_id Module ID
 * @return HAL status
 */
hal_status_t module_manager_unregister_module(uint8_t module_id);

/**
 * @brief Get module information
 * @param module_id Module ID
 * @param info Module information structure
 * @return HAL status
 */
hal_status_t module_manager_get_module_info(uint8_t module_id, module_info_t *info);

/**
 * @brief Get module status
 * @param module_id Module ID
 * @param status Module status structure
 * @return HAL status
 */
hal_status_t module_manager_get_module_status(uint8_t module_id, module_status_info_t *status);

/**
 * @brief Get all registered modules
 * @param module_ids Array to store module IDs
 * @param max_count Maximum number of module IDs
 * @param actual_count Actual number of module IDs
 * @return HAL status
 */
hal_status_t module_manager_get_registered_modules(uint8_t *module_ids, uint32_t max_count, uint32_t *actual_count);

/**
 * @brief Perform health check on module
 * @param module_id Module ID
 * @return HAL status
 */
hal_status_t module_manager_health_check_module(uint8_t module_id);

/**
 * @brief Perform health check on all modules
 * @return HAL status
 */
hal_status_t module_manager_health_check_all(void);

/**
 * @brief Set module event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t module_manager_set_callback(module_event_callback_t callback);

/**
 * @brief Get module statistics
 * @param stats Statistics structure
 * @return HAL status
 */
hal_status_t module_manager_get_statistics(module_stats_t *stats);

/**
 * @brief Reset module statistics
 * @return HAL status
 */
hal_status_t module_manager_reset_statistics(void);

/**
 * @brief Get module type name
 * @param type Module type
 * @return Module type name string
 */
const char* module_manager_get_type_name(module_type_t type);

/**
 * @brief Get module status name
 * @param status Module status
 * @return Module status name string
 */
const char* module_manager_get_status_name(module_status_t status);

/**
 * @brief Get module health name
 * @param health Module health
 * @return Module health name string
 */
const char* module_manager_get_health_name(module_health_t health);

/**
 * @brief Get module event name
 * @param event Module event
 * @return Module event name string
 */
const char* module_manager_get_event_name(module_event_t event);

/**
 * @brief Self test Module Manager
 * @return HAL status
 */
hal_status_t module_manager_self_test(void);

/**
 * @brief Reset Module Manager
 * @return HAL status
 */
hal_status_t module_manager_reset(void);

#ifdef __cplusplus
}
#endif

#endif // MODULE_MANAGER_H
