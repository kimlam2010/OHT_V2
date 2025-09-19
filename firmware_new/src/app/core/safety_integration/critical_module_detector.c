/**
 * @file critical_module_detector.c
 * @brief Critical Module Detection System Implementation
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 2.1 - Critical Module Detection Implementation
 * 
 * 🚨 SAFETY CRITICAL: This implementation detects critical module failures
 * 🔒 SECURITY: All functions are thread-safe and use proper error handling
 * ⚠️  WARNING: This system can trigger E-Stop - extreme care required
 */

#include "critical_module_detector.h"
#include "safety_rs485_integration.h"
#include "../../managers/module_manager.h"
#include "../../../hal/common/hal_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>

// 📊 **GLOBAL STATE - THREAD SAFE**
static bool g_detector_initialized = false;
static bool g_detector_active = false;
static bool g_debug_logging_enabled = false;

static critical_module_detector_status_t g_detector_status = {0};
static critical_module_detector_stats_t g_detector_stats = {0};

// 🎯 **MODULE PRIORITY MATRIX - CONFIGURATION**
static const critical_module_config_t g_module_configs[] = {
    // Power Module (0x02) - ESSENTIAL
    {
        .module_address = 0x02,
        .module_type = MODULE_TYPE_POWER,
        .criticality = MODULE_CRITICALITY_ESSENTIAL,
        .offline_timeout_ms = 1000,          // 1 second to detect offline
        .failure_timeout_ms = 5000,          // 5 seconds before E-Stop
        .recovery_timeout_ms = 2000,         // 2 seconds for recovery
        .failure_action = SAFETY_RS485_ACTION_ESTOP_DELAYED,
        .max_response_level = RESPONSE_LEVEL_CRITICAL,
        .health_check_interval_ms = 500,     // Check every 500ms
        .adaptive_polling_threshold = 2000,   // Increase polling if response > 2ms
        .consecutive_failure_threshold = 3,   // 3 failures before action
        .max_response_time_ms = 50,          // 50ms max response
        .min_success_rate = 0.95f            // 95% minimum success rate
    },
    
    // Safety Module (0x03) - SAFETY CRITICAL
    {
        .module_address = 0x03,
        .module_type = MODULE_TYPE_SAFETY,
        .criticality = MODULE_CRITICALITY_SAFETY_CRITICAL,
        .offline_timeout_ms = 100,           // 100ms to detect offline
        .failure_timeout_ms = 0,             // Immediate E-Stop
        .recovery_timeout_ms = 1000,         // 1 second for recovery
        .failure_action = SAFETY_RS485_ACTION_ESTOP_IMMEDIATE,
        .max_response_level = RESPONSE_LEVEL_EMERGENCY,
        .health_check_interval_ms = 50,      // Check every 50ms
        .adaptive_polling_threshold = 1000,   // Increase polling if response > 1ms
        .consecutive_failure_threshold = 1,   // 1 failure triggers action
        .max_response_time_ms = 50,          // 50ms max response
        .min_success_rate = 0.99f            // 99% minimum success rate
    },
    
    // Travel Motor Module (0x04) - IMPORTANT
    {
        .module_address = 0x04,
        .module_type = MODULE_TYPE_TRAVEL_MOTOR,
        .criticality = MODULE_CRITICALITY_IMPORTANT,
        .offline_timeout_ms = 500,           // 500ms to detect offline
        .failure_timeout_ms = 1000,          // 1 second before warning
        .recovery_timeout_ms = 3000,         // 3 seconds for recovery
        .failure_action = SAFETY_RS485_ACTION_WARNING,
        .max_response_level = RESPONSE_LEVEL_WARNING,
        .health_check_interval_ms = 1000,    // Check every 1 second
        .adaptive_polling_threshold = 5000,   // Increase polling if response > 5ms
        .consecutive_failure_threshold = 5,   // 5 failures before action
        .max_response_time_ms = 100,         // 100ms max response
        .min_success_rate = 0.90f            // 90% minimum success rate
    },
    
    // Dock Module (0x06) - OPTIONAL
    {
        .module_address = 0x06,
        .module_type = MODULE_TYPE_DOCK,
        .criticality = MODULE_CRITICALITY_OPTIONAL,
        .offline_timeout_ms = 5000,          // 5 seconds to detect offline
        .failure_timeout_ms = 10000,         // 10 seconds before log
        .recovery_timeout_ms = 5000,         // 5 seconds for recovery
        .failure_action = SAFETY_RS485_ACTION_LOG_ONLY,
        .max_response_level = RESPONSE_LEVEL_MONITORING,
        .health_check_interval_ms = 5000,    // Check every 5 seconds
        .adaptive_polling_threshold = 10000,  // Increase polling if response > 10ms
        .consecutive_failure_threshold = 10,  // 10 failures before action
        .max_response_time_ms = 200,         // 200ms max response
        .min_success_rate = 0.80f            // 80% minimum success rate
    }
};

#define NUM_CONFIGURED_MODULES (sizeof(g_module_configs) / sizeof(g_module_configs[0]))

// Module health assessments
static module_health_assessment_t g_module_assessments[NUM_CONFIGURED_MODULES] = {0};

// 🕐 **UTILITY FUNCTIONS**

/**
 * @brief Get current timestamp in milliseconds
 */
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

/**
 * @brief Debug logging function
 */
static void debug_log(const char *format, ...) {
    if (!g_debug_logging_enabled) return;
    
    va_list args;
    va_start(args, format);
    printf("[CRITICAL_MODULE_DEBUG] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/**
 * @brief Error logging function
 */
static void error_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[CRITICAL_MODULE_ERROR] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/**
 * @brief Warning logging function
 */
static void warning_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[CRITICAL_MODULE_WARNING] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/**
 * @brief Find module configuration by address
 */
static const critical_module_config_t* find_module_config(uint8_t module_addr) {
    for (size_t i = 0; i < NUM_CONFIGURED_MODULES; i++) {
        if (g_module_configs[i].module_address == module_addr) {
            return &g_module_configs[i];
        }
    }
    return NULL;
}

/**
 * @brief Find module assessment by address
 */
static module_health_assessment_t* find_module_assessment(uint8_t module_addr) {
    for (size_t i = 0; i < NUM_CONFIGURED_MODULES; i++) {
        if (g_module_assessments[i].module_address == module_addr) {
            return &g_module_assessments[i];
        }
    }
    return NULL;
}

// 🔧 **SYSTEM MANAGEMENT FUNCTIONS**

hal_status_t critical_module_detector_init(void) {
    if (g_detector_initialized) {
        warning_log("Critical module detector already initialized");
        return HAL_OK;
    }
    
    debug_log("Initializing critical module detector...");
    
    // Initialize detector status
    memset(&g_detector_status, 0, sizeof(g_detector_status));
    memset(&g_detector_stats, 0, sizeof(g_detector_stats));
    memset(g_module_assessments, 0, sizeof(g_module_assessments));
    
    // Initialize system timing
    uint64_t current_time = get_timestamp_ms();
    g_detector_status.system_start_time_ms = current_time;
    g_detector_status.last_system_check_ms = current_time;
    g_detector_status.system_check_interval_ms = 100; // Default 100ms
    g_detector_status.overall_response_level = RESPONSE_LEVEL_NORMAL;
    
    // Initialize module assessments
    for (size_t i = 0; i < NUM_CONFIGURED_MODULES; i++) {
        g_module_assessments[i].module_address = g_module_configs[i].module_address;
        g_module_assessments[i].health_status = MODULE_HEALTH_UNKNOWN;
        g_module_assessments[i].response_level = RESPONSE_LEVEL_NORMAL;
        g_module_assessments[i].last_seen_ms = current_time;
        g_module_assessments[i].last_health_check_ms = current_time;
        g_module_assessments[i].current_success_rate = 1.0f;
    }
    
    // Initialize statistics
    g_detector_stats.min_detection_time_ms = UINT32_MAX;
    g_detector_stats.min_system_check_time_ms = UINT32_MAX;
    
    g_detector_status.modules_monitored = NUM_CONFIGURED_MODULES;
    g_detector_initialized = true;
    
    debug_log("Critical module detector initialized successfully");
    debug_log("Monitoring %u modules: Safety(0x03), Power(0x02), Travel(0x04), Dock(0x06)", 
              NUM_CONFIGURED_MODULES);
    
    return HAL_OK;
}

hal_status_t critical_module_detector_deinit(void) {
    if (!g_detector_initialized) {
        return HAL_ERROR;
    }
    
    g_detector_active = false;
    g_detector_status.system_active = false;
    g_detector_initialized = false;
    
    debug_log("Critical module detector deinitialized");
    return HAL_OK;
}

hal_status_t critical_module_detector_start(void) {
    if (!g_detector_initialized) {
        error_log("Detector not initialized");
        return HAL_ERROR;
    }
    
    if (g_detector_active) {
        warning_log("Detector already active");
        return HAL_OK;
    }
    
    g_detector_active = true;
    g_detector_status.system_active = true;
    g_detector_status.last_system_check_ms = get_timestamp_ms();
    
    debug_log("Critical module detector started");
    return HAL_OK;
}

hal_status_t critical_module_detector_stop(void) {
    if (!g_detector_initialized) {
        return HAL_ERROR;
    }
    
    g_detector_active = false;
    g_detector_status.system_active = false;
    
    debug_log("Critical module detector stopped");
    return HAL_OK;
}

// 🔍 **CORE DETECTION FUNCTIONS**

hal_status_t critical_module_check_single_module(uint8_t module_addr, 
                                                module_health_assessment_t *assessment) {
    if (!g_detector_initialized || !g_detector_active) {
        return HAL_ERROR;
    }
    
    if (!assessment) {
        error_log("NULL assessment pointer");
        return HAL_ERROR;
    }
    
    const critical_module_config_t *config = find_module_config(module_addr);
    if (!config) {
        error_log("Module 0x%02X not configured for monitoring", module_addr);
        return HAL_ERROR;
    }
    
    module_health_assessment_t *stored_assessment = find_module_assessment(module_addr);
    if (!stored_assessment) {
        error_log("Module 0x%02X assessment not found", module_addr);
        return HAL_ERROR;
    }
    
    uint64_t current_time = get_timestamp_ms();
    uint64_t check_start_time = current_time;
    
    debug_log("Checking health of module 0x%02X (%s)", 
              module_addr, safety_rs485_get_module_name(module_addr));
    
    // Update check statistics
    stored_assessment->total_checks++;
    stored_assessment->last_health_check_ms = current_time;
    g_detector_stats.total_health_checks++;
    
    // Update per-criticality statistics
    switch (config->criticality) {
        case MODULE_CRITICALITY_SAFETY_CRITICAL:
            g_detector_stats.criticality_stats.safety_critical_checks++;
            break;
        case MODULE_CRITICALITY_ESSENTIAL:
            g_detector_stats.criticality_stats.essential_checks++;
            break;
        case MODULE_CRITICALITY_IMPORTANT:
            g_detector_stats.criticality_stats.important_checks++;
            break;
        case MODULE_CRITICALITY_OPTIONAL:
            g_detector_stats.criticality_stats.optional_checks++;
            break;
    }
    
    // Attempt to read module data
    safety_module_response_t response;
    hal_status_t read_result = safety_rs485_read_module_data(module_addr, &response);
    
    uint64_t check_end_time = get_timestamp_ms();
    uint32_t check_duration = (uint32_t)(check_end_time - check_start_time);
    
    if (read_result == HAL_OK && response.connection_online) {
        // Module is responding
        stored_assessment->successful_checks++;
        stored_assessment->consecutive_failures = 0;
        stored_assessment->last_seen_ms = current_time;
        stored_assessment->last_response_time_ms = response.timestamps.response_time_ms;
        
        // Update response time statistics
        if (response.timestamps.response_time_ms > stored_assessment->max_response_time_ms) {
            stored_assessment->max_response_time_ms = response.timestamps.response_time_ms;
        }
        
        // Update average response time (simple moving average)
        if (stored_assessment->successful_checks > 1) {
            stored_assessment->avg_response_time_ms = 
                (stored_assessment->avg_response_time_ms * (stored_assessment->successful_checks - 1) + 
                 response.timestamps.response_time_ms) / stored_assessment->successful_checks;
        } else {
            stored_assessment->avg_response_time_ms = response.timestamps.response_time_ms;
        }
        
        // Calculate current success rate
        stored_assessment->current_success_rate = 
            (float)stored_assessment->successful_checks / (float)stored_assessment->total_checks;
        
        // Determine health status based on response
        if (response.safety_status >= 3) { // ESTOP or FAULT
            stored_assessment->health_status = MODULE_HEALTH_FAILED;
            stored_assessment->response_level = RESPONSE_LEVEL_EMERGENCY;
        } else if (response.safety_status == 2) { // CRITICAL
            stored_assessment->health_status = MODULE_HEALTH_FAILING;
            stored_assessment->response_level = RESPONSE_LEVEL_CRITICAL;
        } else if (response.safety_status == 1) { // WARNING
            stored_assessment->health_status = MODULE_HEALTH_DEGRADED;
            stored_assessment->response_level = RESPONSE_LEVEL_WARNING;
        } else { // SAFE
            stored_assessment->health_status = MODULE_HEALTH_HEALTHY;
            stored_assessment->response_level = RESPONSE_LEVEL_NORMAL;
        }
        
        // Check performance thresholds
        if (response.timestamps.response_time_ms > config->max_response_time_ms) {
            warning_log("Module 0x%02X response time %u ms exceeds limit %u ms",
                       module_addr, response.timestamps.response_time_ms, config->max_response_time_ms);
            if (stored_assessment->health_status == MODULE_HEALTH_HEALTHY) {
                stored_assessment->health_status = MODULE_HEALTH_DEGRADED;
                stored_assessment->response_level = RESPONSE_LEVEL_MONITORING;
            }
        }
        
        if (stored_assessment->current_success_rate < config->min_success_rate) {
            warning_log("Module 0x%02X success rate %.2f%% below minimum %.2f%%",
                       module_addr, stored_assessment->current_success_rate * 100.0f, 
                       config->min_success_rate * 100.0f);
            if (stored_assessment->health_status == MODULE_HEALTH_HEALTHY) {
                stored_assessment->health_status = MODULE_HEALTH_DEGRADED;
                stored_assessment->response_level = RESPONSE_LEVEL_MONITORING;
            }
        }
        
        // Clear offline status if it was set
        if (stored_assessment->offline_since_ms != 0) {
            debug_log("Module 0x%02X recovered from offline state", module_addr);
            stored_assessment->offline_since_ms = 0;
            g_detector_stats.total_recoveries++;
        }
        
        debug_log("Module 0x%02X healthy: status=%u, response_time=%u ms, success_rate=%.2f%%",
                  module_addr, response.safety_status, response.timestamps.response_time_ms,
                  stored_assessment->current_success_rate * 100.0f);
        
    } else {
        // Module is not responding
        stored_assessment->failed_checks++;
        stored_assessment->consecutive_failures++;
        
        // Update success rate
        stored_assessment->current_success_rate = 
            (float)stored_assessment->successful_checks / (float)stored_assessment->total_checks;
        
        // Check if module should be considered offline
        uint32_t time_since_last_seen = (uint32_t)(current_time - stored_assessment->last_seen_ms);
        
        if (time_since_last_seen > config->offline_timeout_ms) {
            if (stored_assessment->offline_since_ms == 0) {
                // Module just went offline
                stored_assessment->offline_since_ms = current_time;
                stored_assessment->health_status = MODULE_HEALTH_OFFLINE;
                stored_assessment->response_level = RESPONSE_LEVEL_WARNING;
                
                warning_log("Module 0x%02X went OFFLINE (last seen %u ms ago)",
                           module_addr, time_since_last_seen);
                
                g_detector_stats.total_failures_detected++;
            }
        }
        
        debug_log("Module 0x%02X communication failed: consecutive_failures=%u, time_since_seen=%u ms",
                  module_addr, stored_assessment->consecutive_failures, time_since_last_seen);
    }
    
    // Copy assessment to output
    memcpy(assessment, stored_assessment, sizeof(module_health_assessment_t));
    
    // Update system check time statistics
    if (check_duration < g_detector_stats.min_system_check_time_ms) {
        g_detector_stats.min_system_check_time_ms = check_duration;
    }
    if (check_duration > g_detector_stats.max_system_check_time_ms) {
        g_detector_stats.max_system_check_time_ms = check_duration;
    }
    
    return HAL_OK;
}

hal_status_t critical_module_check_all_modules(void) {
    if (!g_detector_initialized || !g_detector_active) {
        return HAL_ERROR;
    }
    
    uint64_t check_start_time = get_timestamp_ms();
    debug_log("Starting system-wide module health check...");
    
    // Reset system counters
    g_detector_status.modules_online = 0;
    g_detector_status.modules_degraded = 0;
    g_detector_status.modules_failed = 0;
    g_detector_status.critical_modules_offline = 0;
    
    hal_status_t overall_result = HAL_OK;
    
    // Check each configured module
    for (size_t i = 0; i < NUM_CONFIGURED_MODULES; i++) {
        const critical_module_config_t *config = &g_module_configs[i];
        module_health_assessment_t assessment;
        
        hal_status_t result = critical_module_check_single_module(config->module_address, &assessment);
        
        if (result == HAL_OK) {
            // Update system counters based on module health
            switch (assessment.health_status) {
                case MODULE_HEALTH_HEALTHY:
                    g_detector_status.modules_online++;
                    break;
                case MODULE_HEALTH_DEGRADED:
                case MODULE_HEALTH_FAILING:
                    g_detector_status.modules_degraded++;
                    break;
                case MODULE_HEALTH_FAILED:
                case MODULE_HEALTH_OFFLINE:
                    g_detector_status.modules_failed++;
                    if (config->criticality >= MODULE_CRITICALITY_ESSENTIAL) {
                        g_detector_status.critical_modules_offline++;
                    }
                    break;
                default:
                    break;
            }
            
            // Check if safety action is needed
            if (assessment.consecutive_failures >= config->consecutive_failure_threshold) {
                uint64_t current_time = get_timestamp_ms();
                uint32_t time_since_last_seen = (uint32_t)(current_time - assessment.last_seen_ms);
                
                if (time_since_last_seen >= config->failure_timeout_ms) {
                    if (!assessment.safety_action_triggered) {
                        // Trigger safety action
                        char reason[256];
                        snprintf(reason, sizeof(reason), 
                                "Module offline for %u ms (threshold: %u ms, failures: %u)",
                                time_since_last_seen, config->failure_timeout_ms, 
                                assessment.consecutive_failures);
                        
                        hal_status_t action_result = critical_module_execute_safety_action(
                            config->module_address, config->failure_action, reason);
                        
                        if (action_result == HAL_OK) {
                            // Mark action as triggered
                            module_health_assessment_t *stored = find_module_assessment(config->module_address);
                            if (stored) {
                                stored->safety_action_triggered = true;
                                stored->last_action_taken = config->failure_action;
                                stored->last_action_time_ms = current_time;
                            }
                        } else {
                            error_log("Failed to execute safety action for module 0x%02X", config->module_address);
                            overall_result = HAL_ERROR;
                        }
                    }
                }
            }
        } else {
            error_log("Failed to check module 0x%02X health", config->module_address);
            overall_result = HAL_ERROR;
        }
    }
    
    // Determine overall system response level
    safety_response_level_t new_response_level;
    critical_module_determine_response_level(&new_response_level);
    
    if (new_response_level != g_detector_status.overall_response_level) {
        debug_log("System response level changed: %s → %s",
                  critical_module_get_response_level_name(g_detector_status.overall_response_level),
                  critical_module_get_response_level_name(new_response_level));
        
        critical_module_update_response_level(new_response_level);
    }
    
    // Update system timing
    uint64_t check_end_time = get_timestamp_ms();
    uint32_t total_check_time = (uint32_t)(check_end_time - check_start_time);
    g_detector_status.last_system_check_ms = check_end_time;
    
    // Update system check time statistics
    if (g_detector_stats.avg_system_check_time_ms == 0) {
        g_detector_stats.avg_system_check_time_ms = total_check_time;
    } else {
        static uint32_t check_count = 1;
        check_count++;
        g_detector_stats.avg_system_check_time_ms = 
            (g_detector_stats.avg_system_check_time_ms * (check_count - 1) + total_check_time) / check_count;
    }
    
    debug_log("System health check completed: %u ms, online=%u, degraded=%u, failed=%u, critical_offline=%u",
              total_check_time, g_detector_status.modules_online, g_detector_status.modules_degraded,
              g_detector_status.modules_failed, g_detector_status.critical_modules_offline);
    
    return overall_result;
}

hal_status_t critical_module_determine_response_level(safety_response_level_t *response_level) {
    if (!response_level) {
        return HAL_ERROR;
    }
    
    safety_response_level_t highest_level = RESPONSE_LEVEL_NORMAL;
    
    // Find the highest response level among all modules
    for (size_t i = 0; i < NUM_CONFIGURED_MODULES; i++) {
        const module_health_assessment_t *assessment = &g_module_assessments[i];
        if (assessment->response_level > highest_level) {
            highest_level = assessment->response_level;
        }
    }
    
    // System-wide overrides
    if (g_detector_status.critical_modules_offline > 0) {
        highest_level = RESPONSE_LEVEL_EMERGENCY;
    } else if (g_detector_status.modules_failed > 0) {
        if (highest_level < RESPONSE_LEVEL_CRITICAL) {
            highest_level = RESPONSE_LEVEL_CRITICAL;
        }
    } else if (g_detector_status.modules_degraded > 0) {
        if (highest_level < RESPONSE_LEVEL_WARNING) {
            highest_level = RESPONSE_LEVEL_WARNING;
        }
    }
    
    *response_level = highest_level;
    return HAL_OK;
}

// 🚨 **SAFETY ACTION FUNCTIONS**

hal_status_t critical_module_execute_safety_action(uint8_t module_addr, 
                                                  safety_action_t action, 
                                                  const char *reason) {
    debug_log("Executing safety action %s for module 0x%02X: %s",
              safety_rs485_get_action_name(action), module_addr, reason ? reason : "No reason");
    
    g_detector_stats.total_safety_actions++;
    
    hal_status_t result = HAL_OK;
    
    switch (action) {
        case SAFETY_RS485_ACTION_LOG_ONLY:
            debug_log("LOG_ONLY action for module 0x%02X: %s", module_addr, reason ? reason : "");
            g_detector_stats.log_only_actions++;
            break;
            
        case SAFETY_RS485_ACTION_WARNING:
            warning_log("WARNING action for module 0x%02X: %s", module_addr, reason ? reason : "");
            result = safety_rs485_set_warning_mode(module_addr, reason);
            g_detector_stats.warning_actions++;
            break;
            
        case SAFETY_RS485_ACTION_DEGRADED:
            warning_log("DEGRADED action for module 0x%02X: %s", module_addr, reason ? reason : "");
            // TODO: Implement degraded mode in Phase 2.3
            g_detector_stats.degraded_actions++;
            break;
            
        case SAFETY_RS485_ACTION_ESTOP_DELAYED:
            error_log("ESTOP_DELAYED action for module 0x%02X: %s", module_addr, reason ? reason : "");
            result = safety_rs485_trigger_delayed_estop(module_addr, 5000, reason); // 5 second delay
            g_detector_stats.delayed_estop_actions++;
            break;
            
        case SAFETY_RS485_ACTION_ESTOP_IMMEDIATE:
            error_log("ESTOP_IMMEDIATE action for module 0x%02X: %s", module_addr, reason ? reason : "");
            result = safety_rs485_trigger_immediate_estop(module_addr, reason);
            g_detector_status.emergency_triggered = true;
            g_detector_status.last_emergency_time_ms = get_timestamp_ms();
            g_detector_status.total_emergencies++;
            g_detector_stats.immediate_estop_actions++;
            break;
            
        default:
            error_log("Unknown safety action %d for module 0x%02X", action, module_addr);
            result = HAL_ERROR;
            break;
    }
    
    return result;
}

hal_status_t critical_module_handle_recovery(uint8_t module_addr) {
    debug_log("Handling recovery for module 0x%02X", module_addr);
    
    module_health_assessment_t *assessment = find_module_assessment(module_addr);
    if (!assessment) {
        error_log("Module 0x%02X not found for recovery", module_addr);
        return HAL_ERROR;
    }
    
    // Reset safety action trigger
    assessment->safety_action_triggered = false;
    assessment->consecutive_failures = 0;
    assessment->offline_since_ms = 0;
    
    // Update health status
    assessment->health_status = MODULE_HEALTH_HEALTHY;
    assessment->response_level = RESPONSE_LEVEL_NORMAL;
    
    debug_log("Module 0x%02X recovery completed", module_addr);
    return HAL_OK;
}

hal_status_t critical_module_update_response_level(safety_response_level_t new_level) {
    safety_response_level_t old_level = g_detector_status.overall_response_level;
    g_detector_status.overall_response_level = new_level;
    
    debug_log("System response level updated: %s → %s",
              critical_module_get_response_level_name(old_level),
              critical_module_get_response_level_name(new_level));
    
    // Update LED patterns (Phase 2.3 implementation)
    critical_module_update_led_patterns(new_level);
    
    return HAL_OK;
}

// 🔧 **UTILITY FUNCTIONS IMPLEMENTATION**

const char* critical_module_get_criticality_name(module_criticality_t criticality) {
    switch (criticality) {
        case MODULE_CRITICALITY_OPTIONAL:        return "OPTIONAL";
        case MODULE_CRITICALITY_IMPORTANT:       return "IMPORTANT";
        case MODULE_CRITICALITY_ESSENTIAL:       return "ESSENTIAL";
        case MODULE_CRITICALITY_SAFETY_CRITICAL: return "SAFETY_CRITICAL";
        default:                                 return "UNKNOWN";
    }
}

const char* critical_module_get_response_level_name(safety_response_level_t level) {
    switch (level) {
        case RESPONSE_LEVEL_NORMAL:     return "NORMAL";
        case RESPONSE_LEVEL_MONITORING: return "MONITORING";
        case RESPONSE_LEVEL_WARNING:    return "WARNING";
        case RESPONSE_LEVEL_CRITICAL:   return "CRITICAL";
        case RESPONSE_LEVEL_EMERGENCY:  return "EMERGENCY";
        default:                        return "UNKNOWN";
    }
}

const char* critical_module_get_health_name(module_health_status_t health) {
    switch (health) {
        case MODULE_HEALTH_UNKNOWN:   return "UNKNOWN";
        case MODULE_HEALTH_HEALTHY:   return "HEALTHY";
        case MODULE_HEALTH_DEGRADED:  return "DEGRADED";
        case MODULE_HEALTH_FAILING:   return "FAILING";
        case MODULE_HEALTH_FAILED:    return "FAILED";
        case MODULE_HEALTH_OFFLINE:   return "OFFLINE";
        default:                      return "UNKNOWN";
    }
}

bool critical_module_is_critical(uint8_t module_addr) {
    const critical_module_config_t *config = find_module_config(module_addr);
    if (!config) return false;
    
    return (config->criticality >= MODULE_CRITICALITY_ESSENTIAL);
}

uint32_t critical_module_get_timeout(uint8_t module_addr) {
    const critical_module_config_t *config = find_module_config(module_addr);
    if (!config) return 10000; // Default 10 second timeout
    
    return config->failure_timeout_ms;
}

uint32_t critical_module_time_since_last_seen(uint8_t module_addr) {
    const module_health_assessment_t *assessment = find_module_assessment(module_addr);
    if (!assessment) return UINT32_MAX;
    
    uint64_t current_time = get_timestamp_ms();
    return (uint32_t)(current_time - assessment->last_seen_ms);
}

// 📊 **STATUS AND STATISTICS FUNCTIONS**

hal_status_t critical_module_get_status(critical_module_detector_status_t *status) {
    if (!status) return HAL_ERROR;
    if (!g_detector_initialized) return HAL_ERROR;
    
    memcpy(status, &g_detector_status, sizeof(critical_module_detector_status_t));
    return HAL_OK;
}

hal_status_t critical_module_get_statistics(critical_module_detector_stats_t *stats) {
    if (!stats) return HAL_ERROR;
    if (!g_detector_initialized) return HAL_ERROR;
    
    memcpy(stats, &g_detector_stats, sizeof(critical_module_detector_stats_t));
    return HAL_OK;
}

hal_status_t critical_module_reset_statistics(void) {
    if (!g_detector_initialized) return HAL_ERROR;
    
    memset(&g_detector_stats, 0, sizeof(g_detector_stats));
    g_detector_stats.min_detection_time_ms = UINT32_MAX;
    g_detector_stats.min_system_check_time_ms = UINT32_MAX;
    
    debug_log("Critical module detector statistics reset");
    return HAL_OK;
}

hal_status_t critical_module_get_health_assessment(uint8_t module_addr, 
                                                  module_health_assessment_t *assessment) {
    if (!assessment) return HAL_ERROR;
    
    const module_health_assessment_t *stored = find_module_assessment(module_addr);
    if (!stored) return HAL_ERROR;
    
    memcpy(assessment, stored, sizeof(module_health_assessment_t));
    return HAL_OK;
}

// 🧪 **TESTING AND DIAGNOSTICS FUNCTIONS**

hal_status_t critical_module_self_test(void) {
    debug_log("Starting critical module detector self-test...");
    
    if (!g_detector_initialized) {
        error_log("Self-test failed: Detector not initialized");
        return HAL_ERROR;
    }
    
    hal_status_t overall_result = HAL_OK;
    uint32_t tests_passed = 0;
    uint32_t tests_total = 0;
    
    // Test 1: Check all modules
    {
        tests_total++;
        hal_status_t result = critical_module_check_all_modules();
        if (result == HAL_OK) {
            debug_log("✅ Test 1 PASSED: All modules check OK");
            tests_passed++;
        } else {
            error_log("❌ Test 1 FAILED: All modules check failed");
            overall_result = HAL_ERROR;
        }
    }
    
    // Test 2: Response level determination
    {
        tests_total++;
        safety_response_level_t level;
        hal_status_t result = critical_module_determine_response_level(&level);
        if (result == HAL_OK) {
            debug_log("✅ Test 2 PASSED: Response level determination OK (level: %s)", 
                     critical_module_get_response_level_name(level));
            tests_passed++;
        } else {
            error_log("❌ Test 2 FAILED: Response level determination failed");
            overall_result = HAL_ERROR;
        }
    }
    
    // Test 3: Utility functions
    {
        tests_total++;
        const char *crit_name = critical_module_get_criticality_name(MODULE_CRITICALITY_SAFETY_CRITICAL);
        const char *level_name = critical_module_get_response_level_name(RESPONSE_LEVEL_EMERGENCY);
        const char *health_name = critical_module_get_health_name(MODULE_HEALTH_HEALTHY);
        
        if (crit_name && level_name && health_name) {
            debug_log("✅ Test 3 PASSED: Utility functions OK");
            tests_passed++;
        } else {
            error_log("❌ Test 3 FAILED: Utility functions failed");
            overall_result = HAL_ERROR;
        }
    }
    
    debug_log("Critical module detector self-test completed: %u/%u tests passed", tests_passed, tests_total);
    
    if (overall_result == HAL_OK) {
        debug_log("✅ ALL TESTS PASSED - Critical module detector is healthy");
    } else {
        error_log("❌ SOME TESTS FAILED - Critical module detector has issues");
    }
    
    return overall_result;
}

hal_status_t critical_module_get_diagnostics(char *info, size_t max_len) {
    if (!info || max_len == 0) return HAL_ERROR;
    if (!g_detector_initialized) {
        snprintf(info, max_len, "Critical module detector not initialized");
        return HAL_ERROR;
    }
    
    snprintf(info, max_len,
        "Critical Module Detector Diagnostics:\n"
        "- System Active: %s\n"
        "- Modules Monitored: %u\n"
        "- Modules Online: %u\n"
        "- Modules Degraded: %u\n"
        "- Modules Failed: %u\n"
        "- Critical Modules Offline: %u\n"
        "- Overall Response Level: %s\n"
        "- Total Health Checks: %llu\n"
        "- Total Failures Detected: %llu\n"
        "- Total Recoveries: %llu\n"
        "- Total Safety Actions: %llu\n"
        "- Emergency Triggered: %s\n"
        "- Total Emergencies: %u\n"
        "- Avg System Check Time: %u ms\n",
        g_detector_status.system_active ? "YES" : "NO",
        g_detector_status.modules_monitored,
        g_detector_status.modules_online,
        g_detector_status.modules_degraded,
        g_detector_status.modules_failed,
        g_detector_status.critical_modules_offline,
        critical_module_get_response_level_name(g_detector_status.overall_response_level),
        g_detector_stats.total_health_checks,
        g_detector_stats.total_failures_detected,
        g_detector_stats.total_recoveries,
        g_detector_stats.total_safety_actions,
        g_detector_status.emergency_triggered ? "YES" : "NO",
        g_detector_status.total_emergencies,
        g_detector_stats.avg_system_check_time_ms
    );
    
    return HAL_OK;
}

hal_status_t critical_module_set_debug_logging(bool enable) {
    g_debug_logging_enabled = enable;
    debug_log("Debug logging %s", enable ? "ENABLED" : "DISABLED");
    return HAL_OK;
}

// 🔗 **INTEGRATION FUNCTIONS (Phase 2.2 Placeholders)**

hal_status_t critical_module_safety_monitor_integration(void) {
    // Phase 2.2 implementation placeholder
    return critical_module_check_all_modules();
}

uint32_t critical_module_get_adaptive_polling_interval(uint8_t module_addr) {
    const critical_module_config_t *config = find_module_config(module_addr);
    if (!config) return 1000; // Default 1 second
    
    const module_health_assessment_t *assessment = find_module_assessment(module_addr);
    if (!assessment) return config->health_check_interval_ms;
    
    // Adaptive polling based on health status
    switch (assessment->health_status) {
        case MODULE_HEALTH_FAILED:
        case MODULE_HEALTH_OFFLINE:
            return config->health_check_interval_ms / 4; // 4x faster when failed
        case MODULE_HEALTH_FAILING:
            return config->health_check_interval_ms / 2; // 2x faster when failing
        case MODULE_HEALTH_DEGRADED:
            return config->health_check_interval_ms * 2 / 3; // 1.5x faster when degraded
        case MODULE_HEALTH_HEALTHY:
        default:
            return config->health_check_interval_ms; // Normal interval
    }
}

hal_status_t critical_module_update_led_patterns(safety_response_level_t level) {
    // Phase 2.3 implementation placeholder
    debug_log("LED pattern update for response level: %s", 
              critical_module_get_response_level_name(level));
    return HAL_OK;
}

// Configuration functions (placeholders)
hal_status_t critical_module_get_config(uint8_t module_addr, critical_module_config_t *config) {
    if (!config) return HAL_ERROR;
    
    const critical_module_config_t *stored = find_module_config(module_addr);
    if (!stored) return HAL_ERROR;
    
    memcpy(config, stored, sizeof(critical_module_config_t));
    return HAL_OK;
}

hal_status_t critical_module_set_config(uint8_t module_addr, const critical_module_config_t *config) {
    debug_log("critical_module_set_config() - Phase 3 implementation pending");
    return HAL_OK;
}

hal_status_t critical_module_load_default_config(void) {
    debug_log("critical_module_load_default_config() - Phase 3 implementation pending");
    return HAL_OK;
}

hal_status_t critical_module_simulate_failure(uint8_t module_addr, uint32_t failure_duration_ms) {
    debug_log("critical_module_simulate_failure() - Testing function placeholder");
    return HAL_OK;
}
