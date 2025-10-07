/**
 * @file safety_rs485_integration.h
 * @brief Safety RS485 Integration for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 1.2 - Data Structure Design (Safety Monitor RS485 Integration)
 * 
 * 🚨 SAFETY CRITICAL: This module integrates RS485 communication with existing safety monitor
 * 🔒 SECURITY: All functions use "safety_rs485_" prefix to avoid conflicts
 * ⚠️  WARNING: NEVER modify existing safety_monitor.h - only ADD new functionality
 */

#ifndef SAFETY_RS485_INTEGRATION_H
#define SAFETY_RS485_INTEGRATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

// Include existing headers - NO MODIFICATIONS to existing files
// Updated paths for Domain-Driven Architecture v1.0.1
#include "../../infrastructure/communication/communication_manager.h"
#include "../../domain/module_management/module_manager.h"
#include "safety_monitor.h"

#ifdef __cplusplus
extern "C" {
#endif

// 🔧 **SAFETY MODULE REGISTER MAP (từ SAFETY_MONITOR_IMPROVEMENT_PLAN.md)**
#define SAFETY_RS485_REG_STATUS           0x0000  // Safety status (0=SAFE, 1=WARNING, 2=CRITICAL, 3=ESTOP, 4=FAULT)
#define SAFETY_RS485_REG_ESTOP            0x0001  // E-Stop active (0=inactive, 1=active)
#define SAFETY_RS485_REG_SENSOR1_DIST     0x0010  // Sensor 1 distance (mm)
#define SAFETY_RS485_REG_SENSOR2_DIST     0x0011  // Sensor 2 distance (mm)
#define SAFETY_RS485_REG_DIGITAL_INPUTS   0x0020  // Digital inputs bitmap
#define SAFETY_RS485_REG_ERROR_CODES      0x0005  // Error codes bitmap

// 🎯 **SAFETY MODULE ADDRESSES**
#define SAFETY_RS485_POWER_MODULE_ADDR    0x02    // Power Module
#define SAFETY_RS485_SAFETY_MODULE_ADDR   0x03    // Safety Module (CRITICAL)
#define SAFETY_RS485_TRAVEL_MODULE_ADDR   0x04    // Travel Motor Module
#define SAFETY_RS485_LIFTER_MODULE_ADDR   0x05    // Lifter Module  
#define SAFETY_RS485_DOCK_MODULE_ADDR     0x06    // Dock Module

// 🚨 **SAFETY THRESHOLDS & TIMEOUTS**
#define SAFETY_RS485_MAX_RESPONSE_TIME_MS 50      // Maximum acceptable response time
#define SAFETY_RS485_CRITICAL_TIMEOUT_MS  0       // Safety Module timeout = immediate E-Stop
#define SAFETY_RS485_POWER_TIMEOUT_MS     5000    // Power Module timeout = delayed E-Stop
#define SAFETY_RS485_TRAVEL_TIMEOUT_MS    1000    // Travel Module timeout = warning
#define SAFETY_RS485_DOCK_TIMEOUT_MS      10000   // Dock Module timeout = log only

#define SAFETY_RS485_MIN_DISTANCE_MM      0       // Minimum valid distance
#define SAFETY_RS485_MAX_DISTANCE_MM      10000   // Maximum valid distance
#define SAFETY_RS485_MAX_STATUS_VALUE     4       // Maximum valid status value

// 📊 **SAFETY MODULE RESPONSE STRUCTURE**
typedef struct {
    // Connection status
    bool connection_online;                       // Module responding to communication
    
    // Safety status data
    uint8_t safety_status;                        // 0=SAFE, 1=WARNING, 2=CRITICAL, 3=ESTOP, 4=FAULT
    bool estop_active;                            // E-Stop button pressed
    
    // Critical sensor values
    struct {
        uint16_t sensor1_distance_mm;             // Sensor 1 distance in mm
        uint16_t sensor2_distance_mm;             // Sensor 2 distance in mm
        uint16_t digital_inputs;                  // Digital input bitmap
    } critical_values;
    
    // Error information
    uint8_t error_codes;                          // Error codes bitmap
    
    // Timing information
    struct {
        uint64_t request_timestamp_ms;            // When request was sent
        uint64_t response_timestamp_ms;           // When response was received
        uint32_t response_time_ms;                // Response time in ms
        uint64_t last_valid_response_ms;          // Last valid response timestamp
    } timestamps;
    
    // Communication quality
    struct {
        uint32_t successful_reads;                // Successful register reads
        uint32_t failed_reads;                    // Failed register reads
        uint32_t timeout_count;                   // Communication timeouts
        uint32_t crc_error_count;                 // CRC errors
    } comm_stats;
    
} safety_module_response_t;

// ⚙️ **MODULE SAFETY CONFIGURATION STRUCTURE**
typedef struct {
    uint8_t module_address;                       // RS485 address (0x02-0x06)
    module_type_t module_type;                    // Module type from module_manager.h
    bool is_critical;                             // Critical for safety operation
    uint32_t timeout_ms;                          // Timeout before action
    uint8_t safety_action;                        // Action to take on loss (safety_action_t)
    
    // Health monitoring
    uint32_t health_check_interval_ms;            // How often to check health
    uint8_t max_consecutive_failures;             // Max failures before action
    
    // Response time monitoring  
    uint32_t expected_response_time_ms;           // Expected response time
    uint32_t max_acceptable_response_time_ms;     // Maximum acceptable response time
    
} module_safety_config_t;

// 🚨 **SAFETY ACTION TYPES**
typedef enum {
    SAFETY_RS485_ACTION_LOG_ONLY = 0,             // Just log the event
    SAFETY_RS485_ACTION_WARNING,                  // Set warning mode
    SAFETY_RS485_ACTION_DEGRADED,                 // Degraded operation mode
    SAFETY_RS485_ACTION_ESTOP_DELAYED,            // Delayed E-Stop (countdown)
    SAFETY_RS485_ACTION_ESTOP_IMMEDIATE           // Immediate E-Stop
} safety_action_t;

// 📈 **SAFETY INTEGRATION STATISTICS**
typedef struct {
    // Overall statistics
    uint64_t total_checks;                        // Total safety checks performed
    uint64_t successful_checks;                   // Successful checks
    uint64_t failed_checks;                       // Failed checks
    
    // Per-module statistics
    struct {
        uint32_t power_module_checks;             // Power module checks
        uint32_t safety_module_checks;            // Safety module checks  
        uint32_t travel_module_checks;            // Travel module checks
        uint32_t lifter_module_checks;            // Lifter module checks
        uint32_t dock_module_checks;              // Dock module checks
    } module_checks;
    
    // Response time statistics
    uint32_t min_response_time_ms;                // Minimum response time
    uint32_t max_response_time_ms;                // Maximum response time
    uint32_t avg_response_time_ms;                // Average response time
    
    // Safety actions taken
    uint32_t warnings_triggered;                  // Warnings triggered
    uint32_t degraded_modes_entered;              // Degraded modes entered
    uint32_t delayed_estops_triggered;            // Delayed E-Stops triggered
    uint32_t immediate_estops_triggered;          // Immediate E-Stops triggered
    
    // Timing information
    uint64_t last_check_timestamp_ms;             // Last check timestamp
    uint64_t last_successful_check_ms;            // Last successful check
    uint64_t integration_start_time_ms;           // When integration started
    
} safety_rs485_stats_t;

// 🔧 **SAFETY INTEGRATION STATUS**
typedef struct {
    bool integration_active;                      // Integration is active
    bool safety_module_online;                    // Safety module responding
    uint8_t critical_modules_offline;             // Number of critical modules offline
    uint8_t total_modules_monitored;              // Total modules being monitored
    
    safety_monitor_state_t current_safety_state;  // Current safety state
    uint32_t time_in_current_state_ms;            // Time in current state
    
    // Last safety action
    safety_action_t last_action_taken;            // Last safety action taken
    uint64_t last_action_timestamp_ms;            // When last action was taken
    uint8_t last_action_module_addr;              // Which module triggered last action
    
} safety_rs485_status_t;

// 📋 **FUNCTION PROTOTYPES - Phase 1.2**

/**
 * @brief Initialize Safety RS485 Integration
 * @return HAL status
 * 
 * 🚨 SAFETY: Must be called before any other safety_rs485_ functions
 * 🔒 THREAD-SAFE: Yes
 */
hal_status_t safety_rs485_init(void);

/**
 * @brief Deinitialize Safety RS485 Integration  
 * @return HAL status
 * 
 * 🚨 SAFETY: Cleanly shuts down integration
 * 🔒 THREAD-SAFE: Yes
 */
hal_status_t safety_rs485_deinit(void);

/**
 * @brief Read module data from RS485 (Phase 1.3 implementation)
 * @param module_addr Module address (0x02-0x06)
 * @param response Response structure to populate
 * @return HAL status
 * 
 * 🚨 SAFETY: Uses existing comm_manager_modbus_read_holding_registers()
 * ⏱️  PERFORMANCE: Must complete within SAFETY_RS485_MAX_RESPONSE_TIME_MS
 * 🔒 THREAD-SAFE: Yes
 * 
 * Registers read:
 * - 0x0000: Safety status
 * - 0x0001: E-Stop status  
 * - 0x0010-0x0011: Sensor distances
 * - 0x0020: Digital inputs
 * - 0x0005: Error codes
 */
hal_status_t safety_rs485_read_module_data(uint8_t module_addr, safety_module_response_t *response);

/**
 * @brief Validate module response data
 * @param response Response structure to validate
 * @return HAL status (HAL_OK if valid, HAL_ERROR if invalid)
 * 
 * 🚨 SAFETY: Validates all data ranges and timestamps
 * 🔍 VALIDATION: Checks distances 0-10000mm, status 0-4, timestamps
 */
hal_status_t safety_rs485_validate_response(const safety_module_response_t *response);

/**
 * @brief Check critical modules health (Phase 2.1 implementation)
 * @return HAL status
 * 
 * 🚨 SAFETY: Determines safety actions based on module timeouts
 * ⚠️  CRITICAL: Safety Module timeout = immediate E-Stop
 * 📊 MONITORING: Updates module health statistics
 */
hal_status_t safety_rs485_check_critical_modules(void);

/**
 * @brief Trigger immediate E-Stop due to safety module failure
 * @param module_addr Module that triggered E-Stop
 * @param reason Reason for E-Stop
 * @return HAL status
 * 
 * 🚨 SAFETY CRITICAL: Must execute within 100ms
 * 🔗 INTEGRATION: Calls existing safety_monitor_trigger_emergency_stop()
 */
hal_status_t safety_rs485_trigger_immediate_estop(uint8_t module_addr, const char *reason);

/**
 * @brief Trigger delayed E-Stop with countdown
 * @param module_addr Module that triggered delayed E-Stop
 * @param delay_ms Delay before E-Stop (max 10000ms)
 * @param reason Reason for delayed E-Stop
 * @return HAL status
 * 
 * ⏰ DELAYED: Provides time for recovery or operator intervention
 * 🚨 SAFETY: Still triggers immediate E-Stop if safety module fails
 */
hal_status_t safety_rs485_trigger_delayed_estop(uint8_t module_addr, uint32_t delay_ms, const char *reason);

/**
 * @brief Set system to warning mode
 * @param module_addr Module that triggered warning
 * @param reason Reason for warning
 * @return HAL status
 * 
 * ⚠️  WARNING: Reduces system performance but maintains operation
 * 📊 MONITORING: Updates warning statistics
 */
hal_status_t safety_rs485_set_warning_mode(uint8_t module_addr, const char *reason);

/**
 * @brief Log module loss event
 * @param module_addr Module that was lost
 * @param reason Reason for loss
 * @return HAL status
 * 
 * 📝 LOGGING: Records event for diagnostics
 * 📊 STATISTICS: Updates loss statistics
 */
hal_status_t safety_rs485_log_module_loss(uint8_t module_addr, const char *reason);

/**
 * @brief Get safety integration statistics
 * @param stats Statistics structure to populate
 * @return HAL status
 * 
 * 📊 MONITORING: Provides comprehensive statistics for diagnostics
 */
hal_status_t safety_rs485_get_statistics(safety_rs485_stats_t *stats);

/**
 * @brief Get safety integration status
 * @param status Status structure to populate  
 * @return HAL status
 * 
 * 📊 STATUS: Provides current integration status
 */
hal_status_t safety_rs485_get_status(safety_rs485_status_t *status);

/**
 * @brief Reset safety integration statistics
 * @return HAL status
 * 
 * 🔄 RESET: Clears all statistics counters
 */
hal_status_t safety_rs485_reset_statistics(void);

// 🔧 **UTILITY FUNCTIONS**

/**
 * @brief Get safety action name string
 * @param action Safety action enum
 * @return Action name string
 */
const char* safety_rs485_get_action_name(safety_action_t action);

/**
 * @brief Get module name from address
 * @param module_addr Module address
 * @return Module name string
 */
const char* safety_rs485_get_module_name(uint8_t module_addr);

/**
 * @brief Check if module is critical for safety
 * @param module_addr Module address
 * @return true if critical, false otherwise
 * 
 * 🚨 CRITICAL MODULES: Safety Module (0x03) is always critical
 */
bool safety_rs485_is_module_critical(uint8_t module_addr);

/**
 * @brief Get timeout for module
 * @param module_addr Module address
 * @return Timeout in ms
 */
uint32_t safety_rs485_get_module_timeout(uint8_t module_addr);

/**
 * @brief Get safety action for module timeout
 * @param module_addr Module address
 * @return Safety action to take
 */
safety_action_t safety_rs485_get_module_action(uint8_t module_addr);

// 🧪 **TESTING & DIAGNOSTICS FUNCTIONS**

/**
 * @brief Perform self-test of safety integration
 * @return HAL status
 * 
 * 🧪 TESTING: Tests all safety integration functions
 * 🔍 DIAGNOSTICS: Validates communication with all modules
 */
hal_status_t safety_rs485_self_test(void);

/**
 * @brief Get diagnostic information
 * @param info Buffer for diagnostic info
 * @param max_len Maximum buffer length
 * @return HAL status
 * 
 * 🔍 DIAGNOSTICS: Provides detailed diagnostic information
 */
hal_status_t safety_rs485_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Enable/disable debug logging
 * @param enable Enable debug logging
 * @return HAL status
 * 
 * 🐛 DEBUG: Controls debug output level
 */
hal_status_t safety_rs485_set_debug_logging(bool enable);

// 🔄 **CONFIGURATION FUNCTIONS (Phase 3 implementation)**

/**
 * @brief Load configuration from YAML file
 * @param config_path Path to YAML configuration file
 * @return HAL status
 * 
 * ⚙️ CONFIGURATION: Loads module safety configuration from YAML
 */
hal_status_t safety_rs485_load_config(const char *config_path);

/**
 * @brief Save current configuration to YAML file
 * @param config_path Path to save YAML configuration
 * @return HAL status
 * 
 * ⚙️ CONFIGURATION: Saves current configuration to YAML
 */
hal_status_t safety_rs485_save_config(const char *config_path);

/**
 * @brief Get module safety configuration
 * @param module_addr Module address
 * @param config Configuration structure to populate
 * @return HAL status
 */
hal_status_t safety_rs485_get_module_config(uint8_t module_addr, module_safety_config_t *config);

/**
 * @brief Set module safety configuration
 * @param module_addr Module address
 * @param config Configuration structure
 * @return HAL status
 * 
 * ⚠️  SAFETY: Validates configuration before applying
 */
hal_status_t safety_rs485_set_module_config(uint8_t module_addr, const module_safety_config_t *config);

#ifdef __cplusplus
}
#endif

#endif // SAFETY_RS485_INTEGRATION_H

/**
 * 📋 **IMPLEMENTATION NOTES:**
 * 
 * 🚨 **SAFETY COMPLIANCE:**
 * - All functions follow safety-first approach
 * - E-Stop response time must be < 100ms
 * - Safety Module (0x03) timeout = immediate E-Stop
 * - All data validation includes range checking
 * 
 * 🔒 **SECURITY:**
 * - All functions use "safety_rs485_" prefix
 * - No modifications to existing safety_monitor.h
 * - Thread-safe design for multi-threaded environment
 * 
 * ⚡ **PERFORMANCE:**
 * - Register reading must complete < 50ms
 * - Comprehensive error handling
 * - Efficient data structures
 * - Minimal memory allocation
 * 
 * 🔗 **INTEGRATION:**
 * - Uses existing communication_manager functions
 * - Compatible with existing safety_monitor
 * - Works with module_manager registry
 * - Follows existing HAL patterns
 * 
 * 📊 **MONITORING:**
 * - Comprehensive statistics collection
 * - Real-time status reporting
 * - Diagnostic information available
 * - Debug logging support
 * 
 * 🧪 **TESTING:**
 * - Self-test functionality
 * - Standalone testing support
 * - Hardware-in-the-loop ready
 * - Performance benchmarking
 */
