/**
 * @file safety_config_manager.h
 * @brief Safety Configuration Manager for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 3.1 - YAML Configuration System
 * 
 * ⚙️ CONFIGURATION: This module manages safety system configuration
 * 📄 YAML SUPPORT: Load/save configurations from/to YAML files
 * 🔧 RUNTIME UPDATES: Hot-reload configurations without restart
 * 🛡️ VALIDATION: Comprehensive configuration validation
 */

#ifndef SAFETY_CONFIG_MANAGER_H
#define SAFETY_CONFIG_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

// Include existing headers
#include "../core/safety_integration/safety_rs485_integration.h"
#include "../core/safety_integration/critical_module_detector.h"
#include "../core/safety_integration/graduated_response_system.h"
#include "../../hal/common/hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// 📁 **CONFIGURATION FILE PATHS**
#define SAFETY_CONFIG_DEFAULT_PATH        "/etc/oht50/safety_config.yaml"
#define SAFETY_CONFIG_BACKUP_PATH         "/etc/oht50/safety_config.backup.yaml"
#define SAFETY_CONFIG_FACTORY_PATH        "/etc/oht50/safety_config.factory.yaml"
#define SAFETY_CONFIG_TEMP_PATH           "/tmp/safety_config.temp.yaml"

// 🔧 **CONFIGURATION VALIDATION LIMITS**
#define SAFETY_CONFIG_MIN_TIMEOUT_MS      0
#define SAFETY_CONFIG_MAX_TIMEOUT_MS      60000    // 60 seconds max
#define SAFETY_CONFIG_MIN_POLLING_MS      10       // 10ms minimum polling
#define SAFETY_CONFIG_MAX_POLLING_MS      30000    // 30 seconds max polling
#define SAFETY_CONFIG_MIN_SUCCESS_RATE    0.5f     // 50% minimum success rate
#define SAFETY_CONFIG_MAX_SUCCESS_RATE    1.0f     // 100% maximum success rate

// ⚙️ **SAFETY SYSTEM CONFIGURATION STRUCTURE**
typedef struct {
    // System information
    char config_version[16];                      // Configuration version
    char system_name[64];                         // System name
    uint64_t config_timestamp;                    // Configuration timestamp
    char config_author[64];                       // Who created this config
    
    // Global safety settings
    struct {
        bool enable_module_monitoring;            // Enable module monitoring
        bool enable_graduated_response;           // Enable graduated response
        bool enable_adaptive_polling;             // Enable adaptive polling
        bool enable_auto_recovery;                // Enable automatic recovery
        uint32_t system_update_interval_ms;       // System update interval
        uint32_t emergency_response_timeout_ms;   // Emergency response timeout
        float global_safety_margin;               // Global safety margin (1.0 = 100%)
    } global_settings;
    
    // Module configurations
    struct {
        uint8_t num_modules;                      // Number of configured modules
        module_safety_config_t modules[8];        // Module configurations (max 8)
    } module_config;
    
    // Critical module detector settings
    struct {
        bool enable_critical_detection;          // Enable critical module detection
        uint32_t detection_interval_ms;          // Detection interval
        uint32_t max_detection_time_ms;          // Maximum detection time
        uint8_t max_consecutive_failures;        // Max consecutive failures
        float failure_rate_threshold;            // Failure rate threshold
    } detector_config;
    
    // Graduated response settings
    struct {
        bool enable_led_patterns;                // Enable LED patterns
        bool enable_adaptive_polling;            // Enable adaptive polling
        uint32_t led_update_interval_ms;         // LED update interval
        uint32_t polling_update_interval_ms;     // Polling update interval
        uint32_t transition_delay_ms;            // Level transition delay
        uint32_t recovery_timeout_ms;            // Auto recovery timeout
    } response_config;
    
    // Advanced settings
    struct {
        bool enable_debug_logging;               // Enable debug logging
        bool enable_performance_monitoring;      // Enable performance monitoring
        bool enable_statistics_collection;      // Enable statistics collection
        uint32_t log_buffer_size;                // Log buffer size
        uint32_t stats_retention_time_ms;       // Statistics retention time
    } advanced_settings;
    
} safety_system_config_t;

/**
 * @brief Configuration Validation Results
 */
typedef struct {
    bool is_valid;                               // Overall validation result
    uint32_t error_count;                        // Number of validation errors
    uint32_t warning_count;                      // Number of validation warnings
    
    // Specific validation results
    struct {
        bool global_settings_valid;              // Global settings validation
        bool module_config_valid;                // Module config validation
        bool detector_config_valid;              // Detector config validation
        bool response_config_valid;              // Response config validation
        bool advanced_settings_valid;            // Advanced settings validation
    } section_results;
    
    // Error details
    char error_messages[10][256];                // Error messages (max 10)
    char warning_messages[10][256];              // Warning messages (max 10)
    
} config_validation_result_t;

/**
 * @brief Configuration Manager Status
 */
typedef struct {
    bool initialized;                            // Config manager initialized
    bool config_loaded;                          // Configuration loaded
    bool config_valid;                           // Current configuration is valid
    
    // File information
    char current_config_file[256];               // Current config file path
    uint64_t config_load_time;                   // When config was loaded
    uint64_t config_file_timestamp;              // Config file modification time
    uint32_t config_file_size;                   // Config file size
    
    // Runtime information
    uint32_t hot_reload_count;                   // Number of hot reloads
    uint32_t validation_count;                   // Number of validations
    uint32_t backup_count;                       // Number of backups created
    uint64_t last_validation_time;               // Last validation time
    
    // Status
    bool hot_reload_in_progress;                 // Hot reload in progress
    bool backup_available;                       // Backup file available
    bool factory_reset_available;                // Factory reset available
    
} safety_config_manager_status_t;

/**
 * @brief Configuration Change Event
 */
typedef struct {
    uint64_t timestamp;                          // When change occurred
    char section[64];                            // Which section changed
    char field[64];                              // Which field changed
    char old_value[256];                         // Old value
    char new_value[256];                         // New value
    char changed_by[64];                         // Who made the change
    bool requires_restart;                       // Requires system restart
} config_change_event_t;

// Configuration change callback
typedef void (*config_change_callback_t)(const config_change_event_t *event);

// 🔧 **FUNCTION PROTOTYPES**

// System Management Functions

/**
 * @brief Initialize Safety Configuration Manager
 * @return HAL status
 * 
 * ⚙️ INITIALIZATION: Sets up configuration management system
 */
hal_status_t safety_config_manager_init(void);

/**
 * @brief Deinitialize Safety Configuration Manager
 * @return HAL status
 */
hal_status_t safety_config_manager_deinit(void);

// Configuration Loading Functions

/**
 * @brief Load configuration from YAML file
 * @param config_path Path to YAML configuration file
 * @return HAL status
 * 
 * 📄 YAML LOADING: Loads complete safety configuration from YAML
 * 🛡️ VALIDATION: Validates configuration before applying
 * 💾 BACKUP: Creates backup of current config before loading
 */
hal_status_t safety_config_load_from_yaml(const char *config_path);

/**
 * @brief Save current configuration to YAML file
 * @param config_path Path to save YAML configuration
 * @return HAL status
 * 
 * 📄 YAML SAVING: Saves current configuration to YAML format
 */
hal_status_t safety_config_save_to_yaml(const char *config_path);

/**
 * @brief Load factory default configuration
 * @return HAL status
 * 
 * 🏭 FACTORY RESET: Loads factory default configuration
 */
hal_status_t safety_config_load_factory_defaults(void);

/**
 * @brief Create backup of current configuration
 * @param backup_path Path for backup file (optional, uses default if NULL)
 * @return HAL status
 * 
 * 💾 BACKUP: Creates timestamped backup of current configuration
 */
hal_status_t safety_config_create_backup(const char *backup_path);

/**
 * @brief Restore configuration from backup
 * @param backup_path Path to backup file
 * @return HAL status
 * 
 * 🔄 RESTORE: Restores configuration from backup file
 */
hal_status_t safety_config_restore_from_backup(const char *backup_path);

// Configuration Access Functions

/**
 * @brief Get current safety system configuration
 * @param config Output configuration structure
 * @return HAL status
 */
hal_status_t safety_config_get_current(safety_system_config_t *config);

/**
 * @brief Set safety system configuration
 * @param config New configuration structure
 * @param validate_only If true, only validate without applying
 * @return HAL status
 * 
 * ⚙️ HOT RELOAD: Applies configuration changes without restart
 * 🛡️ VALIDATION: Validates configuration before applying
 * 🔄 ROLLBACK: Automatic rollback on validation failure
 */
hal_status_t safety_config_set_current(const safety_system_config_t *config, bool validate_only);

/**
 * @brief Get module configuration
 * @param module_addr Module address
 * @param module_config Output module configuration
 * @return HAL status
 */
hal_status_t safety_config_get_module(uint8_t module_addr, module_safety_config_t *module_config);

/**
 * @brief Set module configuration
 * @param module_addr Module address
 * @param module_config New module configuration
 * @return HAL status
 * 
 * 🔧 MODULE CONFIG: Updates individual module configuration
 * 🛡️ VALIDATION: Validates module config before applying
 */
hal_status_t safety_config_set_module(uint8_t module_addr, const module_safety_config_t *module_config);

// Configuration Validation Functions

/**
 * @brief Validate safety system configuration
 * @param config Configuration to validate
 * @param result Validation result details
 * @return HAL status
 * 
 * 🛡️ COMPREHENSIVE VALIDATION: Validates all configuration sections
 */
hal_status_t safety_config_validate(const safety_system_config_t *config, 
                                   config_validation_result_t *result);

/**
 * @brief Validate module configuration
 * @param module_config Module configuration to validate
 * @param result Validation result details
 * @return HAL status
 */
hal_status_t safety_config_validate_module(const module_safety_config_t *module_config,
                                          config_validation_result_t *result);

/**
 * @brief Check configuration compatibility
 * @param config Configuration to check
 * @return HAL status
 * 
 * 🔧 COMPATIBILITY: Checks if config is compatible with current system
 */
hal_status_t safety_config_check_compatibility(const safety_system_config_t *config);

// Status and Information Functions

/**
 * @brief Get configuration manager status
 * @param status Output status structure
 * @return HAL status
 */
hal_status_t safety_config_get_status(safety_config_manager_status_t *status);

/**
 * @brief Get configuration file information
 * @param config_path Configuration file path
 * @param file_size Output file size
 * @param file_timestamp Output file timestamp
 * @return HAL status
 */
hal_status_t safety_config_get_file_info(const char *config_path, 
                                         uint32_t *file_size, 
                                         uint64_t *file_timestamp);

/**
 * @brief Get configuration change history
 * @param events Output array for change events
 * @param max_events Maximum number of events
 * @param actual_events Output actual number of events
 * @return HAL status
 */
hal_status_t safety_config_get_change_history(config_change_event_t *events, 
                                             uint32_t max_events, 
                                             uint32_t *actual_events);

// Event Handling Functions

/**
 * @brief Set configuration change callback
 * @param callback Configuration change callback function
 * @return HAL status
 */
hal_status_t safety_config_set_change_callback(config_change_callback_t callback);

/**
 * @brief Trigger configuration reload from file
 * @return HAL status
 * 
 * 🔄 HOT RELOAD: Reloads configuration from current file
 */
hal_status_t safety_config_trigger_reload(void);

/**
 * @brief Apply configuration changes immediately
 * @return HAL status
 * 
 * ⚡ IMMEDIATE APPLY: Applies pending configuration changes
 */
hal_status_t safety_config_apply_changes(void);

// Utility Functions

/**
 * @brief Get configuration schema version
 * @return Configuration schema version string
 */
const char* safety_config_get_schema_version(void);

/**
 * @brief Check if configuration file exists
 * @param config_path Configuration file path
 * @return true if file exists and is readable
 */
bool safety_config_file_exists(const char *config_path);

/**
 * @brief Get default configuration
 * @param config Output default configuration
 * @return HAL status
 * 
 * 🏭 DEFAULTS: Returns factory default configuration
 */
hal_status_t safety_config_get_defaults(safety_system_config_t *config);

/**
 * @brief Compare two configurations
 * @param config1 First configuration
 * @param config2 Second configuration
 * @param differences Output differences description
 * @param max_len Maximum description length
 * @return HAL status
 */
hal_status_t safety_config_compare(const safety_system_config_t *config1,
                                  const safety_system_config_t *config2,
                                  char *differences, size_t max_len);

// Testing and Diagnostics Functions

/**
 * @brief Perform self-test of configuration system
 * @return HAL status
 * 
 * 🧪 TESTING: Tests configuration loading, validation, and saving
 */
hal_status_t safety_config_self_test(void);

/**
 * @brief Get diagnostic information
 * @param info Buffer for diagnostic info
 * @param max_len Maximum buffer length
 * @return HAL status
 */
hal_status_t safety_config_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Enable/disable debug logging
 * @param enable Enable debug logging
 * @return HAL status
 */
hal_status_t safety_config_set_debug_logging(bool enable);

/**
 * @brief Test configuration file format
 * @param config_path Configuration file to test
 * @param result Validation result
 * @return HAL status
 * 
 * 🧪 TESTING: Tests YAML file format and structure
 */
hal_status_t safety_config_test_file_format(const char *config_path,
                                           config_validation_result_t *result);

#ifdef __cplusplus
}
#endif

#endif // SAFETY_CONFIG_MANAGER_H

/**
 * 📋 **CONFIGURATION SCHEMA EXAMPLE:**
 * 
 * ```yaml
 * # OHT-50 Safety System Configuration
 * config_version: "1.0.0"
 * system_name: "OHT-50 Master Module"
 * config_timestamp: 1695123456789
 * config_author: "FW Team"
 * 
 * global_settings:
 *   enable_module_monitoring: true
 *   enable_graduated_response: true
 *   enable_adaptive_polling: true
 *   enable_auto_recovery: true
 *   system_update_interval_ms: 100
 *   emergency_response_timeout_ms: 100
 *   global_safety_margin: 1.2
 * 
 * modules:
 *   power_module:
 *     address: 0x02
 *     type: MODULE_TYPE_POWER
 *     is_critical: false
 *     timeout_ms: 5000
 *     safety_action: ESTOP_DELAYED
 *     health_check_interval_ms: 500
 *     max_consecutive_failures: 3
 *     expected_response_time_ms: 30
 *     max_acceptable_response_time_ms: 50
 *     
 *   safety_module:
 *     address: 0x03
 *     type: MODULE_TYPE_SAFETY
 *     is_critical: true
 *     timeout_ms: 0
 *     safety_action: ESTOP_IMMEDIATE
 *     health_check_interval_ms: 50
 *     max_consecutive_failures: 1
 *     expected_response_time_ms: 20
 *     max_acceptable_response_time_ms: 50
 *     
 *   travel_module:
 *     address: 0x04
 *     type: MODULE_TYPE_TRAVEL_MOTOR
 *     is_critical: false
 *     timeout_ms: 1000
 *     safety_action: WARNING
 *     health_check_interval_ms: 1000
 *     max_consecutive_failures: 5
 *     expected_response_time_ms: 25
 *     max_acceptable_response_time_ms: 100
 *     
 *   dock_module:
 *     address: 0x06
 *     type: MODULE_TYPE_DOCK
 *     is_critical: false
 *     timeout_ms: 10000
 *     safety_action: LOG_ONLY
 *     health_check_interval_ms: 5000
 *     max_consecutive_failures: 10
 *     expected_response_time_ms: 40
 *     max_acceptable_response_time_ms: 200
 * 
 * detector_config:
 *   enable_critical_detection: true
 *   detection_interval_ms: 100
 *   max_detection_time_ms: 1000
 *   max_consecutive_failures: 5
 *   failure_rate_threshold: 0.1
 * 
 * response_config:
 *   enable_led_patterns: true
 *   enable_adaptive_polling: true
 *   led_update_interval_ms: 100
 *   polling_update_interval_ms: 1000
 *   transition_delay_ms: 100
 *   recovery_timeout_ms: 5000
 * 
 * advanced_settings:
 *   enable_debug_logging: false
 *   enable_performance_monitoring: true
 *   enable_statistics_collection: true
 *   log_buffer_size: 4096
 *   stats_retention_time_ms: 86400000
 * ```
 * 
 * 📋 **IMPLEMENTATION NOTES:**
 * 
 * 🔧 **CONFIGURATION MANAGEMENT:**
 * - YAML format for human readability
 * - Comprehensive validation before applying
 * - Hot-reload capability for runtime updates
 * - Automatic backup before changes
 * - Factory reset capability
 * 
 * 🛡️ **SAFETY FEATURES:**
 * - Validation prevents invalid configurations
 * - Backup system for rollback capability
 * - Factory reset for emergency situations
 * - Configuration change logging
 * - Compatibility checking
 * 
 * ⚡ **PERFORMANCE FEATURES:**
 * - Efficient YAML parsing
 * - Minimal memory usage
 * - Fast validation algorithms
 * - Optimized hot-reload process
 * - Statistics collection
 * 
 * 🧪 **TESTING FEATURES:**
 * - Self-test capability
 * - File format validation
 * - Configuration comparison
 * - Diagnostic information
 * - Debug logging support
 */
