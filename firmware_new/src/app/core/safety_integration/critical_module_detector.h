/**
 * @file critical_module_detector.h
 * @brief Critical Module Detection System for OHT-50 Safety Monitor
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 2.1 - Critical Module Detection Architecture
 * 
 * 🚨 SAFETY CRITICAL: This module detects critical module failures and triggers appropriate safety responses
 * 🔒 SECURITY: All functions use "critical_module_" prefix to avoid conflicts
 * ⚠️  WARNING: This system can trigger E-Stop - extreme care required
 */

#ifndef CRITICAL_MODULE_DETECTOR_H
#define CRITICAL_MODULE_DETECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

// Include existing headers
#include "safety_rs485_integration.h"
#include "../../managers/module_manager.h"
#include "../../../hal/common/hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// 🎯 **MODULE PRIORITY CLASSIFICATION**

/**
 * @brief Module Criticality Levels
 * 
 * Defines how critical each module is for system safety operation.
 * Higher criticality = faster response to failures.
 */
typedef enum {
    MODULE_CRITICALITY_OPTIONAL = 0,     // Non-essential modules (Dock)
    MODULE_CRITICALITY_IMPORTANT,        // Important but not safety-critical (Travel, Lifter)
    MODULE_CRITICALITY_ESSENTIAL,        // Essential for operation (Power)
    MODULE_CRITICALITY_SAFETY_CRITICAL   // Safety-critical modules (Safety Module)
} module_criticality_t;

/**
 * @brief Safety Response Levels
 * 
 * Defines the graduated response system based on module status and criticality.
 */
typedef enum {
    RESPONSE_LEVEL_NORMAL = 0,           // Normal operation - green LED
    RESPONSE_LEVEL_MONITORING,           // Increased monitoring - green blink
    RESPONSE_LEVEL_WARNING,              // Warning condition - yellow LED
    RESPONSE_LEVEL_CRITICAL,             // Critical condition - red blink
    RESPONSE_LEVEL_EMERGENCY             // Emergency condition - red solid + E-Stop
} safety_response_level_t;

/**
 * @brief Critical Module Health Status
 * 
 * Comprehensive health assessment for each module.
 * Uses different enum name to avoid conflicts with module_manager.h
 */
typedef enum {
    CRITICAL_MODULE_HEALTH_UNKNOWN = 0,
    CRITICAL_MODULE_HEALTH_HEALTHY,      // All systems normal
    CRITICAL_MODULE_HEALTH_DEGRADED,     // Some issues but operational
    CRITICAL_MODULE_HEALTH_FAILING,      // Significant issues, may fail soon
    CRITICAL_MODULE_HEALTH_FAILED,       // Module has failed
    CRITICAL_MODULE_HEALTH_OFFLINE       // Module not responding
} critical_module_health_status_t;

// 📊 **MODULE PRIORITY MATRIX STRUCTURE**

/**
 * @brief Critical Module Configuration
 * 
 * Defines how each module should be monitored and what actions to take on failure.
 */
typedef struct {
    uint8_t module_address;              // RS485 address (0x02-0x06)
    module_type_t module_type;           // Module type from module_manager.h
    module_criticality_t criticality;   // How critical this module is
    
    // Timeout configuration
    uint32_t offline_timeout_ms;         // Time before considering offline
    uint32_t failure_timeout_ms;         // Time before triggering failure action
    uint32_t recovery_timeout_ms;        // Time to wait for recovery
    
    // Safety actions
    safety_action_t failure_action;     // Action to take on failure
    safety_response_level_t max_response_level; // Maximum response level for this module
    
    // Monitoring configuration
    uint32_t health_check_interval_ms;   // How often to check health
    uint32_t adaptive_polling_threshold; // When to increase polling frequency
    uint8_t consecutive_failure_threshold; // Failures before action
    
    // Performance requirements
    uint32_t max_response_time_ms;       // Maximum acceptable response time
    float min_success_rate;              // Minimum acceptable success rate (0.0-1.0)
    
} critical_module_config_t;

/**
 * @brief Module Health Assessment
 * 
 * Real-time health assessment for a module.
 */
typedef struct {
    uint8_t module_address;              // Module address
    critical_module_health_status_t health_status; // Current health status
    safety_response_level_t response_level; // Current response level
    
    // Timing information
    uint64_t last_seen_ms;               // Last successful communication
    uint64_t last_health_check_ms;       // Last health check
    uint64_t offline_since_ms;           // When module went offline (0 if online)
    uint64_t failure_detected_ms;        // When failure was first detected
    
    // Communication statistics
    uint32_t total_checks;               // Total health checks performed
    uint32_t successful_checks;          // Successful health checks
    uint32_t failed_checks;              // Failed health checks
    uint32_t consecutive_failures;       // Current consecutive failures
    float current_success_rate;          // Current success rate
    
    // Performance metrics
    uint32_t last_response_time_ms;      // Last response time
    uint32_t avg_response_time_ms;       // Average response time
    uint32_t max_response_time_ms;       // Maximum response time seen
    
    // Safety status
    bool safety_action_triggered;       // Has safety action been triggered
    safety_action_t last_action_taken;  // Last safety action taken
    uint64_t last_action_time_ms;       // When last action was taken
    
} module_health_assessment_t;

/**
 * @brief Critical Module Detector Status
 * 
 * Overall status of the critical module detection system.
 */
typedef struct {
    bool system_active;                  // Detection system is active
    safety_response_level_t overall_response_level; // System-wide response level
    uint8_t modules_monitored;           // Number of modules being monitored
    uint8_t modules_online;              // Number of modules currently online
    uint8_t modules_degraded;            // Number of modules in degraded state
    uint8_t modules_failed;              // Number of modules that have failed
    uint8_t critical_modules_offline;    // Number of critical modules offline
    
    // System timing
    uint64_t system_start_time_ms;       // When detection system started
    uint64_t last_system_check_ms;       // Last system-wide check
    uint32_t system_check_interval_ms;   // Current system check interval
    
    // Safety status
    bool emergency_triggered;            // Has emergency response been triggered
    uint64_t last_emergency_time_ms;     // Last emergency trigger time
    uint32_t total_emergencies;          // Total emergency responses
    
} critical_module_detector_status_t;

/**
 * @brief Module Detection Statistics
 * 
 * Comprehensive statistics for the detection system.
 */
typedef struct {
    // Overall statistics
    uint64_t total_health_checks;        // Total health checks across all modules
    uint64_t total_failures_detected;    // Total failures detected
    uint64_t total_recoveries;           // Total recoveries detected
    uint64_t total_safety_actions;       // Total safety actions triggered
    
    // Response time statistics
    uint32_t min_detection_time_ms;      // Minimum failure detection time
    uint32_t max_detection_time_ms;      // Maximum failure detection time
    uint32_t avg_detection_time_ms;      // Average failure detection time
    
    // Per-criticality statistics
    struct {
        uint32_t safety_critical_checks; // Safety critical module checks
        uint32_t essential_checks;       // Essential module checks
        uint32_t important_checks;       // Important module checks
        uint32_t optional_checks;        // Optional module checks
    } criticality_stats;
    
    // Action statistics
    uint32_t log_only_actions;           // LOG_ONLY actions taken
    uint32_t warning_actions;            // WARNING actions taken
    uint32_t degraded_actions;           // DEGRADED actions taken
    uint32_t delayed_estop_actions;      // ESTOP_DELAYED actions taken
    uint32_t immediate_estop_actions;    // ESTOP_IMMEDIATE actions taken
    
    // System performance
    uint32_t min_system_check_time_ms;   // Minimum system check time
    uint32_t max_system_check_time_ms;   // Maximum system check time
    uint32_t avg_system_check_time_ms;   // Average system check time
    
} critical_module_detector_stats_t;

// 🔧 **FUNCTION PROTOTYPES**

// System Management Functions

/**
 * @brief Initialize Critical Module Detector
 * @return HAL status
 * 
 * 🚨 SAFETY: Must be called after safety_rs485_init()
 * 🔒 THREAD-SAFE: Yes
 */
hal_status_t critical_module_detector_init(void);

/**
 * @brief Deinitialize Critical Module Detector
 * @return HAL status
 * 
 * 🚨 SAFETY: Cleanly shuts down detection system
 * 🔒 THREAD-SAFE: Yes
 */
hal_status_t critical_module_detector_deinit(void);

/**
 * @brief Start Critical Module Detection
 * @return HAL status
 * 
 * 🚨 SAFETY: Begins monitoring all configured modules
 */
hal_status_t critical_module_detector_start(void);

/**
 * @brief Stop Critical Module Detection
 * @return HAL status
 * 
 * 🚨 SAFETY: Stops monitoring but preserves current safety state
 */
hal_status_t critical_module_detector_stop(void);

// Core Detection Functions

/**
 * @brief Check all critical modules health
 * @return HAL status
 * 
 * 🚨 SAFETY CRITICAL: This is the main detection function
 * ⏱️  PERFORMANCE: Must complete within 1ms
 * 🔒 THREAD-SAFE: Yes
 * 
 * This function:
 * - Checks health of all configured modules
 * - Calculates timeout conditions
 * - Determines appropriate safety responses
 * - Triggers safety actions if necessary
 */
hal_status_t critical_module_check_all_modules(void);

/**
 * @brief Check specific module health
 * @param module_addr Module address to check
 * @param assessment Output health assessment
 * @return HAL status
 * 
 * 🚨 SAFETY: Performs detailed health assessment of single module
 * ⏱️  PERFORMANCE: Must complete within 200μs per module
 */
hal_status_t critical_module_check_single_module(uint8_t module_addr, 
                                                module_health_assessment_t *assessment);

/**
 * @brief Determine system response level
 * @param response_level Output system response level
 * @return HAL status
 * 
 * 🚨 SAFETY: Analyzes all module health to determine overall system response
 */
hal_status_t critical_module_determine_response_level(safety_response_level_t *response_level);

// Safety Action Functions

/**
 * @brief Execute safety action for module
 * @param module_addr Module that triggered action
 * @param action Safety action to execute
 * @param reason Reason for action (for logging)
 * @return HAL status
 * 
 * 🚨 SAFETY CRITICAL: Executes appropriate safety response
 * ⚡ IMMEDIATE: E-Stop actions must execute within 100ms
 */
hal_status_t critical_module_execute_safety_action(uint8_t module_addr, 
                                                  safety_action_t action, 
                                                  const char *reason);

/**
 * @brief Handle module recovery
 * @param module_addr Module that has recovered
 * @return HAL status
 * 
 * 🔄 RECOVERY: Handles module coming back online
 */
hal_status_t critical_module_handle_recovery(uint8_t module_addr);

/**
 * @brief Update system response level
 * @param new_level New response level
 * @return HAL status
 * 
 * 🎛️ CONTROL: Updates system-wide response level and associated behaviors
 */
hal_status_t critical_module_update_response_level(safety_response_level_t new_level);

// Configuration Functions

/**
 * @brief Get module configuration
 * @param module_addr Module address
 * @param config Output configuration
 * @return HAL status
 */
hal_status_t critical_module_get_config(uint8_t module_addr, critical_module_config_t *config);

/**
 * @brief Set module configuration
 * @param module_addr Module address
 * @param config New configuration
 * @return HAL status
 * 
 * ⚠️  SAFETY: Validates configuration before applying
 */
hal_status_t critical_module_set_config(uint8_t module_addr, const critical_module_config_t *config);

/**
 * @brief Load configuration from default settings
 * @return HAL status
 */
hal_status_t critical_module_load_default_config(void);

// Status and Statistics Functions

/**
 * @brief Get detector system status
 * @param status Output status structure
 * @return HAL status
 */
hal_status_t critical_module_get_status(critical_module_detector_status_t *status);

/**
 * @brief Get detection statistics
 * @param stats Output statistics structure
 * @return HAL status
 */
hal_status_t critical_module_get_statistics(critical_module_detector_stats_t *stats);

/**
 * @brief Reset detection statistics
 * @return HAL status
 */
hal_status_t critical_module_reset_statistics(void);

/**
 * @brief Get module health assessment
 * @param module_addr Module address
 * @param assessment Output health assessment
 * @return HAL status
 */
hal_status_t critical_module_get_health_assessment(uint8_t module_addr, 
                                                  module_health_assessment_t *assessment);

// Utility Functions

/**
 * @brief Get criticality name string
 * @param criticality Module criticality
 * @return Criticality name string
 */
const char* critical_module_get_criticality_name(module_criticality_t criticality);

/**
 * @brief Get response level name string
 * @param level Response level
 * @return Response level name string
 */
const char* critical_module_get_response_level_name(safety_response_level_t level);

/**
 * @brief Get health status name string
 * @param health Health status
 * @return Health status name string
 */
const char* critical_module_get_health_name(critical_module_health_status_t health);

/**
 * @brief Check if module is critical
 * @param module_addr Module address
 * @return true if critical, false otherwise
 */
bool critical_module_is_critical(uint8_t module_addr);

/**
 * @brief Get module timeout
 * @param module_addr Module address
 * @return Timeout in milliseconds
 */
uint32_t critical_module_get_timeout(uint8_t module_addr);

/**
 * @brief Calculate time since last seen
 * @param module_addr Module address
 * @return Time in milliseconds since last seen
 */
uint32_t critical_module_time_since_last_seen(uint8_t module_addr);

// Testing and Diagnostics Functions

/**
 * @brief Perform self-test of detection system
 * @return HAL status
 * 
 * 🧪 TESTING: Tests all detection functions
 */
hal_status_t critical_module_self_test(void);

/**
 * @brief Get diagnostic information
 * @param info Buffer for diagnostic info
 * @param max_len Maximum buffer length
 * @return HAL status
 */
hal_status_t critical_module_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Enable/disable debug logging
 * @param enable Enable debug logging
 * @return HAL status
 */
hal_status_t critical_module_set_debug_logging(bool enable);

/**
 * @brief Simulate module failure for testing
 * @param module_addr Module to simulate failure
 * @param failure_duration_ms How long to simulate failure
 * @return HAL status
 * 
 * 🧪 TESTING: For testing safety responses
 * ⚠️  WARNING: Only use in test environment
 */
hal_status_t critical_module_simulate_failure(uint8_t module_addr, uint32_t failure_duration_ms);

// Integration Functions (Phase 2.2)

/**
 * @brief Integrate with safety monitor update cycle
 * @return HAL status
 * 
 * 🔗 INTEGRATION: Called from safety_monitor_update()
 * ⏱️  PERFORMANCE: Must complete within 1ms
 */
hal_status_t critical_module_safety_monitor_integration(void);

/**
 * @brief Get adaptive polling interval
 * @param module_addr Module address
 * @return Polling interval in milliseconds
 * 
 * 🎛️ ADAPTIVE: Returns appropriate polling interval based on module health
 */
uint32_t critical_module_get_adaptive_polling_interval(uint8_t module_addr);

/**
 * @brief Update LED patterns based on response level
 * @param level Current response level
 * @return HAL status
 * 
 * 💡 LED CONTROL: Updates LED patterns to reflect system status
 */
hal_status_t critical_module_update_led_patterns(safety_response_level_t level);

#ifdef __cplusplus
}
#endif

#endif // CRITICAL_MODULE_DETECTOR_H

/**
 * 📋 **ARCHITECTURE NOTES:**
 * 
 * 🎯 **MODULE PRIORITY MATRIX:**
 * 
 * | Module | Address | Criticality | Timeout | Action | Response Level |
 * |--------|---------|-------------|---------|--------|----------------|
 * | Safety | 0x03 | SAFETY_CRITICAL | 0ms | ESTOP_IMMEDIATE | EMERGENCY |
 * | Power | 0x02 | ESSENTIAL | 5000ms | ESTOP_DELAYED | CRITICAL |
 * | Travel | 0x04 | IMPORTANT | 1000ms | WARNING | WARNING |
 * | Lifter | 0x05 | IMPORTANT | 1000ms | WARNING | WARNING |
 * | Dock | 0x06 | OPTIONAL | 10000ms | LOG_ONLY | MONITORING |
 * 
 * 🚨 **SAFETY DESIGN PRINCIPLES:**
 * 
 * 1. **Fail-Safe Default:** Always default to higher safety level when uncertain
 * 2. **Graduated Response:** Provide appropriate response based on criticality
 * 3. **Fast Detection:** Critical modules checked every 50ms, others adaptively
 * 4. **Immediate Action:** Safety-critical failures trigger E-Stop within 100ms
 * 5. **Recovery Support:** Automatic recovery when modules come back online
 * 
 * 📊 **PERFORMANCE REQUIREMENTS:**
 * 
 * - **System Check Time:** < 1ms total for all modules
 * - **Individual Module Check:** < 200μs per module
 * - **E-Stop Trigger Time:** < 100ms for safety-critical failures
 * - **Detection Latency:** < 50ms for offline detection
 * - **Memory Usage:** < 4KB additional for all data structures
 * 
 * 🔗 **INTEGRATION STRATEGY:**
 * 
 * - **Phase 2.1:** Standalone implementation and testing
 * - **Phase 2.2:** Integration with safety_monitor_update()
 * - **Phase 2.3:** LED pattern coordination and graduated response
 * - **Phase 3:** Configuration system and HTTP API integration
 * 
 * 🧪 **TESTING STRATEGY:**
 * 
 * - **Unit Tests:** Each function tested individually
 * - **Integration Tests:** Full system testing with mock modules
 * - **Hardware Tests:** Real module disconnection testing
 * - **Performance Tests:** Response time and throughput validation
 * - **Safety Tests:** E-Stop trigger time validation
 */
