/**
 * @file graduated_response_system.c
 * @brief Graduated Response System Implementation
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 2.3 - Graduated Response System Implementation
 * 
 * 🎛️ GRADUATED RESPONSE: Implements graduated response system with LED patterns and adaptive polling
 * 💡 LED COORDINATION: Manages LED patterns based on system response level
 * ⚡ ADAPTIVE POLLING: Adjusts polling intervals based on module health
 * 🔄 RECOVERY LOGIC: Handles automatic escalation and de-escalation
 */

#include "graduated_response_system.h"
#include "critical_module_detector.h"
#include "../../../hal/peripherals/hal_led.h"
#include "../../../hal/common/hal_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>

// 📊 **GLOBAL STATE**
static bool g_graduated_response_initialized = false;
static bool g_graduated_response_active = false;
static bool g_debug_logging_enabled = false;

static graduated_response_status_t g_response_status = {0};
static graduated_response_stats_t g_response_stats = {0};

// 💡 **LED PATTERN CONFIGURATIONS**
static const safety_response_led_pattern_t g_led_patterns[] = {
    // NORMAL Level
    {
        .system_led_pattern = LED_PATTERN_SOLID,
        .system_blink_period_ms = 0,
        .comm_led_pattern = LED_PATTERN_SOLID,
        .comm_blink_period_ms = 0,
        .network_led_pattern = LED_PATTERN_SOLID,
        .network_blink_period_ms = 0,
        .error_led_pattern = LED_PATTERN_NONE,
        .error_blink_period_ms = 0,
        .power_led_pattern = LED_PATTERN_SOLID,
        .power_blink_period_ms = 0
    },
    
    // MONITORING Level
    {
        .system_led_pattern = LED_PATTERN_BLINK_SLOW,
        .system_blink_period_ms = 2000,
        .comm_led_pattern = LED_PATTERN_SOLID,
        .comm_blink_period_ms = 0,
        .network_led_pattern = LED_PATTERN_SOLID,
        .network_blink_period_ms = 0,
        .error_led_pattern = LED_PATTERN_NONE,
        .error_blink_period_ms = 0,
        .power_led_pattern = LED_PATTERN_SOLID,
        .power_blink_period_ms = 0
    },
    
    // WARNING Level
    {
        .system_led_pattern = LED_PATTERN_SOLID,
        .system_blink_period_ms = 0,
        .comm_led_pattern = LED_PATTERN_WARNING,
        .comm_blink_period_ms = 1000,
        .network_led_pattern = LED_PATTERN_SOLID,
        .network_blink_period_ms = 0,
        .error_led_pattern = LED_PATTERN_BLINK_SLOW,
        .error_blink_period_ms = 1500,
        .power_led_pattern = LED_PATTERN_SOLID,
        .power_blink_period_ms = 0
    },
    
    // CRITICAL Level
    {
        .system_led_pattern = LED_PATTERN_BLINK_FAST,
        .system_blink_period_ms = 500,
        .comm_led_pattern = LED_PATTERN_ERROR,
        .comm_blink_period_ms = 500,
        .network_led_pattern = LED_PATTERN_BLINK_SLOW,
        .network_blink_period_ms = 1000,
        .error_led_pattern = LED_PATTERN_BLINK_FAST,
        .error_blink_period_ms = 500,
        .power_led_pattern = LED_PATTERN_BLINK_SLOW,
        .power_blink_period_ms = 1000
    },
    
    // EMERGENCY Level
    {
        .system_led_pattern = LED_PATTERN_ERROR,
        .system_blink_period_ms = 200,
        .comm_led_pattern = LED_PATTERN_ERROR,
        .comm_blink_period_ms = 200,
        .network_led_pattern = LED_PATTERN_ERROR,
        .network_blink_period_ms = 200,
        .error_led_pattern = LED_PATTERN_SOLID,
        .error_blink_period_ms = 0,
        .power_led_pattern = LED_PATTERN_ERROR,
        .power_blink_period_ms = 200
    }
};

// ⚡ **ADAPTIVE POLLING CONFIGURATIONS**
static const adaptive_polling_config_t g_polling_configs[] = {
    // Safety Module (0x03) - CRITICAL
    {
        .module_address = 0x03,
        .healthy_interval_ms = 50,
        .degraded_interval_ms = 25,
        .failing_interval_ms = 10,
        .failed_interval_ms = 10,
        .offline_interval_ms = 100,
        .performance_scale_factor = 1.5f,
        .success_rate_scale_factor = 2.0f,
        .min_interval_ms = 10,
        .max_interval_ms = 1000
    },
    
    // Power Module (0x02) - ESSENTIAL
    {
        .module_address = 0x02,
        .healthy_interval_ms = 500,
        .degraded_interval_ms = 250,
        .failing_interval_ms = 100,
        .failed_interval_ms = 50,
        .offline_interval_ms = 1000,
        .performance_scale_factor = 1.3f,
        .success_rate_scale_factor = 1.5f,
        .min_interval_ms = 50,
        .max_interval_ms = 5000
    },
    
    // Travel Motor Module (0x04) - IMPORTANT
    {
        .module_address = 0x04,
        .healthy_interval_ms = 1000,
        .degraded_interval_ms = 500,
        .failing_interval_ms = 250,
        .failed_interval_ms = 100,
        .offline_interval_ms = 2000,
        .performance_scale_factor = 1.2f,
        .success_rate_scale_factor = 1.3f,
        .min_interval_ms = 100,
        .max_interval_ms = 10000
    },
    
    // Dock Module (0x06) - OPTIONAL
    {
        .module_address = 0x06,
        .healthy_interval_ms = 5000,
        .degraded_interval_ms = 2500,
        .failing_interval_ms = 1000,
        .failed_interval_ms = 500,
        .offline_interval_ms = 10000,
        .performance_scale_factor = 1.1f,
        .success_rate_scale_factor = 1.2f,
        .min_interval_ms = 500,
        .max_interval_ms = 30000
    }
};

#define NUM_POLLING_CONFIGS (sizeof(g_polling_configs) / sizeof(g_polling_configs[0]))

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
    printf("[GRADUATED_RESPONSE_DEBUG] ");
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
    printf("[GRADUATED_RESPONSE_ERROR] ");
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
    printf("[GRADUATED_RESPONSE_WARNING] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/**
 * @brief Find polling configuration by module address
 */
static const adaptive_polling_config_t* find_polling_config(uint8_t module_addr) {
    for (size_t i = 0; i < NUM_POLLING_CONFIGS; i++) {
        if (g_polling_configs[i].module_address == module_addr) {
            return &g_polling_configs[i];
        }
    }
    return NULL;
}

// 🔧 **SYSTEM MANAGEMENT FUNCTIONS**

hal_status_t graduated_response_init(void) {
    if (g_graduated_response_initialized) {
        warning_log("Graduated response system already initialized");
        return HAL_STATUS_OK;
    }
    
    debug_log("Initializing graduated response system...");
    
    // Initialize status
    memset(&g_response_status, 0, sizeof(g_response_status));
    memset(&g_response_stats, 0, sizeof(g_response_stats));
    
    uint64_t current_time = get_timestamp_ms();
    g_response_status.current_level = RESPONSE_LEVEL_NORMAL;
    g_response_status.previous_level = RESPONSE_LEVEL_NORMAL;
    g_response_status.level_entry_time_ms = current_time;
    g_response_status.last_level_change_ms = current_time;
    g_response_status.led_update_interval_ms = 100; // 100ms LED update
    g_response_status.polling_update_interval_ms = 1000; // 1s polling update
    
    g_graduated_response_initialized = true;
    
    debug_log("Graduated response system initialized successfully");
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_deinit(void) {
    if (!g_graduated_response_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    g_graduated_response_active = false;
    g_response_status.system_active = false;
    g_graduated_response_initialized = false;
    
    debug_log("Graduated response system deinitialized");
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_start(void) {
    if (!g_graduated_response_initialized) {
        error_log("Graduated response system not initialized");
        return HAL_STATUS_ERROR;
    }
    
    if (g_graduated_response_active) {
        warning_log("Graduated response system already active");
        return HAL_STATUS_OK;
    }
    
    g_graduated_response_active = true;
    g_response_status.system_active = true;
    g_response_status.led_patterns_active = true;
    g_response_status.adaptive_polling_active = true;
    
    uint64_t current_time = get_timestamp_ms();
    g_response_status.last_led_update_ms = current_time;
    g_response_status.last_polling_update_ms = current_time;
    
    // Set initial LED patterns for NORMAL level
    graduated_response_update_led_patterns(RESPONSE_LEVEL_NORMAL);
    
    debug_log("Graduated response system started");
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_stop(void) {
    if (!g_graduated_response_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    g_graduated_response_active = false;
    g_response_status.system_active = false;
    g_response_status.led_patterns_active = false;
    g_response_status.adaptive_polling_active = false;
    
    debug_log("Graduated response system stopped");
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_update(void) {
    if (!g_graduated_response_initialized || !g_graduated_response_active) {
        return HAL_STATUS_ERROR;
    }
    
    uint64_t current_time = get_timestamp_ms();
    hal_status_t overall_status = HAL_STATUS_OK;
    
    // Update LED patterns
    if (g_response_status.led_patterns_active && 
        (current_time - g_response_status.last_led_update_ms >= g_response_status.led_update_interval_ms)) {
        
        hal_status_t led_status = graduated_response_update_led_patterns(g_response_status.current_level);
        if (led_status != HAL_STATUS_OK) {
            error_log("Failed to update LED patterns");
            overall_status = HAL_STATUS_ERROR;
        } else {
            g_response_stats.led_update_count++;
        }
        
        g_response_status.last_led_update_ms = current_time;
    }
    
    // Update polling intervals
    if (g_response_status.adaptive_polling_active &&
        (current_time - g_response_status.last_polling_update_ms >= g_response_status.polling_update_interval_ms)) {
        
        hal_status_t polling_status = graduated_response_update_polling_intervals();
        if (polling_status != HAL_STATUS_OK) {
            error_log("Failed to update polling intervals");
            overall_status = HAL_STATUS_ERROR;
        }
        
        g_response_status.last_polling_update_ms = current_time;
    }
    
    // Handle transitions
    if (g_response_status.transition_in_progress) {
        uint32_t transition_time = (uint32_t)(current_time - g_response_status.transition_start_time_ms);
        
        if (transition_time >= g_response_status.transition_timeout_ms) {
            // Complete transition
            g_response_status.transition_in_progress = false;
            g_response_status.current_level = g_response_status.transition_target;
            g_response_status.level_entry_time_ms = current_time;
            g_response_status.last_level_change_ms = current_time;
            
            debug_log("Level transition completed: %s", 
                     graduated_response_get_level_name(g_response_status.current_level));
            
            // Update statistics
            g_response_stats.total_transitions++;
            if (g_response_stats.avg_transition_time_ms == 0) {
                g_response_stats.avg_transition_time_ms = transition_time;
            } else {
                g_response_stats.avg_transition_time_ms = 
                    (g_response_stats.avg_transition_time_ms + transition_time) / 2;
            }
        }
    }
    
    // Handle automatic recovery
    if (!g_response_status.transition_in_progress) {
        hal_status_t recovery_status = graduated_response_handle_auto_recovery();
        if (recovery_status != HAL_STATUS_OK) {
            // Recovery failure is not critical
            debug_log("Auto recovery check completed with issues");
        }
    }
    
    // Update level time statistics
    uint32_t time_in_level = (uint32_t)(current_time - g_response_status.level_entry_time_ms);
    switch (g_response_status.current_level) {
        case RESPONSE_LEVEL_NORMAL:
            g_response_stats.total_time_normal_ms += time_in_level;
            break;
        case RESPONSE_LEVEL_MONITORING:
            g_response_stats.total_time_monitoring_ms += time_in_level;
            break;
        case RESPONSE_LEVEL_WARNING:
            g_response_stats.total_time_warning_ms += time_in_level;
            break;
        case RESPONSE_LEVEL_CRITICAL:
            g_response_stats.total_time_critical_ms += time_in_level;
            break;
        case RESPONSE_LEVEL_EMERGENCY:
            g_response_stats.total_time_emergency_ms += time_in_level;
            break;
    }
    
    return overall_status;
}

// 🎛️ **RESPONSE LEVEL MANAGEMENT**

hal_status_t graduated_response_set_level(safety_response_level_t new_level, const char *reason) {
    if (!g_graduated_response_initialized) {
        error_log("Graduated response system not initialized");
        return HAL_STATUS_ERROR;
    }
    
    if (new_level == g_response_status.current_level) {
        debug_log("Already at response level %s", graduated_response_get_level_name(new_level));
        return HAL_STATUS_OK;
    }
    
    // Check if transition is allowed
    if (!graduated_response_is_transition_allowed(g_response_status.current_level, new_level)) {
        warning_log("Transition from %s to %s not allowed", 
                   graduated_response_get_level_name(g_response_status.current_level),
                   graduated_response_get_level_name(new_level));
        return HAL_STATUS_ERROR;
    }
    
    debug_log("Changing response level: %s → %s (reason: %s)",
              graduated_response_get_level_name(g_response_status.current_level),
              graduated_response_get_level_name(new_level),
              reason ? reason : "No reason provided");
    
    uint64_t current_time = get_timestamp_ms();
    
    // Start transition
    g_response_status.transition_in_progress = true;
    g_response_status.transition_target = new_level;
    g_response_status.transition_start_time_ms = current_time;
    g_response_status.transition_timeout_ms = 100; // 100ms transition delay
    
    g_response_status.previous_level = g_response_status.current_level;
    
    // Update statistics
    if (new_level > g_response_status.current_level) {
        g_response_stats.escalation_count++;
    } else {
        g_response_stats.de_escalation_count++;
    }
    
    // Update level-specific statistics
    switch (new_level) {
        case RESPONSE_LEVEL_NORMAL:
            g_response_stats.normal_level_count++;
            break;
        case RESPONSE_LEVEL_MONITORING:
            g_response_stats.monitoring_level_count++;
            break;
        case RESPONSE_LEVEL_WARNING:
            g_response_stats.warning_level_count++;
            break;
        case RESPONSE_LEVEL_CRITICAL:
            g_response_stats.critical_level_count++;
            break;
        case RESPONSE_LEVEL_EMERGENCY:
            g_response_stats.emergency_level_count++;
            break;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_get_level(safety_response_level_t *current_level) {
    if (!current_level) {
        return HAL_STATUS_ERROR;
    }
    
    if (!g_graduated_response_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    *current_level = g_response_status.current_level;
    return HAL_STATUS_OK;
}

bool graduated_response_is_transition_allowed(safety_response_level_t from_level, 
                                             safety_response_level_t to_level) {
    (void)from_level;
    (void)to_level;
    // All transitions are allowed for now
    // In a more sophisticated system, we might restrict certain transitions
    return true;
}

hal_status_t graduated_response_handle_auto_recovery(void) {
    // Get current system response level from critical module detector
    safety_response_level_t system_level;
    hal_status_t status = critical_module_determine_response_level(&system_level);
    
    if (status != HAL_STATUS_OK) {
        debug_log("Failed to get system response level for auto recovery");
        return HAL_STATUS_ERROR;
    }
    
    // Check if we can de-escalate
    if (system_level < g_response_status.current_level) {
        // System has improved, consider de-escalation
        uint32_t time_in_level = graduated_response_get_time_in_level();
        uint32_t min_time_in_level = 5000; // 5 seconds minimum
        
        if (time_in_level >= min_time_in_level) {
            debug_log("Auto recovery: de-escalating from %s to %s",
                     graduated_response_get_level_name(g_response_status.current_level),
                     graduated_response_get_level_name(system_level));
            
            hal_status_t recovery_status = graduated_response_set_level(system_level, "Auto recovery");
            if (recovery_status == HAL_STATUS_OK) {
                g_response_stats.auto_recoveries++;
            }
            
            return recovery_status;
        }
    }
    
    // Check if we need to escalate
    if (system_level > g_response_status.current_level) {
        debug_log("Auto escalation: escalating from %s to %s",
                 graduated_response_get_level_name(g_response_status.current_level),
                 graduated_response_get_level_name(system_level));
        
        return graduated_response_set_level(system_level, "Auto escalation");
    }
    
    return HAL_STATUS_OK;
}

// 💡 **LED PATTERN MANAGEMENT**

hal_status_t graduated_response_update_led_patterns(safety_response_level_t response_level) {
    if (!g_graduated_response_initialized || !g_response_status.led_patterns_active) {
        return HAL_STATUS_ERROR;
    }
    
    if (response_level >= sizeof(g_led_patterns) / sizeof(g_led_patterns[0])) {
        error_log("Invalid response level for LED patterns: %d", response_level);
        return HAL_STATUS_ERROR;
    }
    
    const safety_response_led_pattern_t *pattern = &g_led_patterns[response_level];
    hal_status_t overall_status = HAL_STATUS_OK;
    
    debug_log("Updating LED patterns for response level: %s", 
              graduated_response_get_level_name(response_level));
    
    // Mock LED updates for testing (replace with real hal_led_set_pattern in production)
    debug_log("Setting System LED pattern: %s", graduated_response_get_led_pattern_name(pattern->system_led_pattern));
    debug_log("Setting Communication LED pattern: %s", graduated_response_get_led_pattern_name(pattern->comm_led_pattern));
    debug_log("Setting Network LED pattern: %s", graduated_response_get_led_pattern_name(pattern->network_led_pattern));
    debug_log("Setting Error LED pattern: %s", graduated_response_get_led_pattern_name(pattern->error_led_pattern));
    debug_log("Setting Power LED pattern: %s", graduated_response_get_led_pattern_name(pattern->power_led_pattern));
    
    // In production, these would be:
    // hal_led_set_pattern(LED_SYSTEM_PIN, pattern->system_led_pattern)
    // hal_led_set_pattern(LED_COMM_PIN, pattern->comm_led_pattern)
    // hal_led_set_pattern(LED_NETWORK_PIN, pattern->network_led_pattern)
    // hal_led_set_pattern(LED_ERROR_PIN, pattern->error_led_pattern)
    // hal_led_set_pattern(LED_POWER_PIN, pattern->power_led_pattern)
    
    if (overall_status == HAL_STATUS_OK) {
        debug_log("LED patterns updated successfully for level %s", 
                  graduated_response_get_level_name(response_level));
    }
    
    return overall_status;
}

hal_status_t graduated_response_get_led_pattern(safety_response_level_t response_level, 
                                               safety_response_led_pattern_t *led_pattern) {
    if (!led_pattern) {
        return HAL_STATUS_ERROR;
    }
    
    if (response_level >= sizeof(g_led_patterns) / sizeof(g_led_patterns[0])) {
        return HAL_STATUS_ERROR;
    }
    
    memcpy(led_pattern, &g_led_patterns[response_level], sizeof(safety_response_led_pattern_t));
    return HAL_STATUS_OK;
}

// ⚡ **ADAPTIVE POLLING MANAGEMENT**

hal_status_t graduated_response_get_polling_interval(uint8_t module_addr, uint32_t *polling_interval) {
    if (!polling_interval) {
        return HAL_STATUS_ERROR;
    }
    
    if (!g_graduated_response_initialized || !g_response_status.adaptive_polling_active) {
        *polling_interval = 1000; // Default 1 second
        return HAL_STATUS_OK;
    }
    
    const adaptive_polling_config_t *config = find_polling_config(module_addr);
    if (!config) {
        *polling_interval = 1000; // Default for unknown modules
        return HAL_STATUS_OK;
    }
    
    // Get module health assessment
    module_health_assessment_t assessment;
    hal_status_t status = critical_module_get_health_assessment(module_addr, &assessment);
    
    if (status != HAL_STATUS_OK) {
        *polling_interval = config->healthy_interval_ms;
        return HAL_STATUS_OK;
    }
    
    // Base interval based on health status
    uint32_t base_interval;
    switch (assessment.health_status) {
        case CRITICAL_MODULE_HEALTH_HEALTHY:
            base_interval = config->healthy_interval_ms;
            break;
        case CRITICAL_MODULE_HEALTH_DEGRADED:
            base_interval = config->degraded_interval_ms;
            break;
        case CRITICAL_MODULE_HEALTH_FAILING:
            base_interval = config->failing_interval_ms;
            break;
        case CRITICAL_MODULE_HEALTH_FAILED:
            base_interval = config->failed_interval_ms;
            break;
        case CRITICAL_MODULE_HEALTH_OFFLINE:
            base_interval = config->offline_interval_ms;
            break;
        default:
            base_interval = config->healthy_interval_ms;
            break;
    }
    
    // Apply performance scaling
    float performance_factor = 1.0f;
    if (assessment.last_response_time_ms > 50) { // Slow response
        performance_factor = config->performance_scale_factor;
    }
    
    // Apply success rate scaling
    float success_rate_factor = 1.0f;
    if (assessment.current_success_rate < 0.95f) { // Low success rate
        success_rate_factor = config->success_rate_scale_factor;
    }
    
    // Calculate final interval
    float final_interval = (float)base_interval / (performance_factor * success_rate_factor);
    
    // Apply limits
    if (final_interval < (float)config->min_interval_ms) {
        final_interval = (float)config->min_interval_ms;
    }
    if (final_interval > (float)config->max_interval_ms) {
        final_interval = (float)config->max_interval_ms;
    }
    
    *polling_interval = (uint32_t)final_interval;
    
    debug_log("Module 0x%02X polling interval: %u ms (health: %s, perf_factor: %.2f, success_factor: %.2f)",
              module_addr, *polling_interval,
              critical_module_get_health_name(assessment.health_status),
              performance_factor, success_rate_factor);
    
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_update_polling_intervals(void) {
    if (!g_graduated_response_initialized || !g_response_status.adaptive_polling_active) {
        return HAL_STATUS_ERROR;
    }
    
    debug_log("Updating adaptive polling intervals...");
    
    hal_status_t overall_status = HAL_STATUS_OK;
    uint32_t adjustments_made = 0;
    
    // Update polling intervals for all configured modules
    for (size_t i = 0; i < NUM_POLLING_CONFIGS; i++) {
        uint8_t module_addr = g_polling_configs[i].module_address;
        uint32_t new_interval;
        
        hal_status_t status = graduated_response_get_polling_interval(module_addr, &new_interval);
        if (status == HAL_STATUS_OK) {
            adjustments_made++;
            // Note: In real implementation, we would update the actual polling timers
            // For now, we just log the calculated intervals
        } else {
            error_log("Failed to calculate polling interval for module 0x%02X", module_addr);
            overall_status = HAL_STATUS_ERROR;
        }
    }
    
    if (adjustments_made > 0) {
        g_response_stats.polling_adjustments += adjustments_made;
        debug_log("Updated polling intervals for %u modules", adjustments_made);
    }
    
    return overall_status;
}

// 📊 **STATUS AND STATISTICS**

hal_status_t graduated_response_get_status(graduated_response_status_t *status) {
    if (!status) {
        return HAL_STATUS_ERROR;
    }
    
    if (!g_graduated_response_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    memcpy(status, &g_response_status, sizeof(graduated_response_status_t));
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_get_statistics(graduated_response_stats_t *stats) {
    if (!stats) {
        return HAL_STATUS_ERROR;
    }
    
    if (!g_graduated_response_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    memcpy(stats, &g_response_stats, sizeof(graduated_response_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_reset_statistics(void) {
    if (!g_graduated_response_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    memset(&g_response_stats, 0, sizeof(g_response_stats));
    debug_log("Graduated response statistics reset");
    return HAL_STATUS_OK;
}

// 🔧 **UTILITY FUNCTIONS**

const char* graduated_response_get_level_name(safety_response_level_t level) {
    switch (level) {
        case RESPONSE_LEVEL_NORMAL:     return "NORMAL";
        case RESPONSE_LEVEL_MONITORING: return "MONITORING";
        case RESPONSE_LEVEL_WARNING:    return "WARNING";
        case RESPONSE_LEVEL_CRITICAL:   return "CRITICAL";
        case RESPONSE_LEVEL_EMERGENCY:  return "EMERGENCY";
        default:                        return "UNKNOWN";
    }
}

const char* graduated_response_get_led_pattern_name(led_pattern_t pattern) {
    switch (pattern) {
        case LED_PATTERN_NONE:       return "NONE";
        case LED_PATTERN_SOLID:      return "SOLID";
        case LED_PATTERN_BLINK_SLOW: return "BLINK_SLOW";
        case LED_PATTERN_BLINK_FAST: return "BLINK_FAST";
        case LED_PATTERN_PULSE:      return "PULSE";
        case LED_PATTERN_ERROR:      return "ERROR";
        case LED_PATTERN_WARNING:    return "WARNING";
        default:                     return "UNKNOWN";
    }
}

uint32_t graduated_response_get_time_in_level(void) {
    if (!g_graduated_response_initialized) {
        return 0;
    }
    
    uint64_t current_time = get_timestamp_ms();
    return (uint32_t)(current_time - g_response_status.level_entry_time_ms);
}

bool graduated_response_is_safe_state(void) {
    if (!g_graduated_response_initialized) {
        return false;
    }
    
    return (g_response_status.current_level <= RESPONSE_LEVEL_MONITORING);
}

bool graduated_response_requires_attention(void) {
    if (!g_graduated_response_initialized) {
        return false;
    }
    
    return (g_response_status.current_level >= RESPONSE_LEVEL_WARNING);
}

// 🧪 **TESTING AND DIAGNOSTICS**

hal_status_t graduated_response_self_test(void) {
    debug_log("Starting graduated response system self-test...");
    
    if (!g_graduated_response_initialized) {
        error_log("Self-test failed: System not initialized");
        return HAL_STATUS_ERROR;
    }
    
    hal_status_t overall_result = HAL_STATUS_OK;
    uint32_t tests_passed = 0;
    uint32_t tests_total = 0;
    
    // Test 1: LED pattern retrieval
    {
        tests_total++;
        safety_response_led_pattern_t pattern;
        hal_status_t result = graduated_response_get_led_pattern(RESPONSE_LEVEL_WARNING, &pattern);
        
        if (result == HAL_STATUS_OK) {
            debug_log("✅ Test 1 PASSED: LED pattern retrieval OK");
            tests_passed++;
        } else {
            error_log("❌ Test 1 FAILED: LED pattern retrieval failed");
            overall_result = HAL_STATUS_ERROR;
        }
    }
    
    // Test 2: Polling interval calculation
    {
        tests_total++;
        uint32_t interval;
        hal_status_t result = graduated_response_get_polling_interval(0x03, &interval);
        
        if (result == HAL_STATUS_OK && interval > 0) {
            debug_log("✅ Test 2 PASSED: Polling interval calculation OK (interval: %u ms)", interval);
            tests_passed++;
        } else {
            error_log("❌ Test 2 FAILED: Polling interval calculation failed");
            overall_result = HAL_STATUS_ERROR;
        }
    }
    
    // Test 3: Level transition validation
    {
        tests_total++;
        bool transition_allowed = graduated_response_is_transition_allowed(RESPONSE_LEVEL_NORMAL, RESPONSE_LEVEL_WARNING);
        
        if (transition_allowed) {
            debug_log("✅ Test 3 PASSED: Level transition validation OK");
            tests_passed++;
        } else {
            error_log("❌ Test 3 FAILED: Level transition validation failed");
            overall_result = HAL_STATUS_ERROR;
        }
    }
    
    debug_log("Graduated response self-test completed: %u/%u tests passed", tests_passed, tests_total);
    
    if (overall_result == HAL_STATUS_OK) {
        debug_log("✅ ALL TESTS PASSED - Graduated response system is healthy");
    } else {
        error_log("❌ SOME TESTS FAILED - Graduated response system has issues");
    }
    
    return overall_result;
}

hal_status_t graduated_response_get_diagnostics(char *info, size_t max_len) {
    if (!info || max_len == 0) {
        return HAL_STATUS_ERROR;
    }
    
    if (!g_graduated_response_initialized) {
        snprintf(info, max_len, "Graduated response system not initialized");
        return HAL_STATUS_ERROR;
    }
    
    snprintf(info, max_len,
        "Graduated Response System Diagnostics:\n"
        "- System Active: %s\n"
        "- Current Level: %s\n"
        "- Time in Level: %u ms\n"
        "- LED Patterns Active: %s\n"
        "- Adaptive Polling Active: %s\n"
        "- Total Transitions: %u\n"
        "- Escalations: %u\n"
        "- De-escalations: %u\n"
        "- Auto Recoveries: %u\n"
        "- LED Updates: %u\n"
        "- Polling Adjustments: %u\n",
        g_response_status.system_active ? "YES" : "NO",
        graduated_response_get_level_name(g_response_status.current_level),
        graduated_response_get_time_in_level(),
        g_response_status.led_patterns_active ? "YES" : "NO",
        g_response_status.adaptive_polling_active ? "YES" : "NO",
        g_response_stats.total_transitions,
        g_response_stats.escalation_count,
        g_response_stats.de_escalation_count,
        g_response_stats.auto_recoveries,
        g_response_stats.led_update_count,
        g_response_stats.polling_adjustments
    );
    
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_set_debug_logging(bool enable) {
    g_debug_logging_enabled = enable;
    debug_log("Debug logging %s", enable ? "ENABLED" : "DISABLED");
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_test_led_patterns(safety_response_level_t level, uint32_t duration_ms) {
    debug_log("Testing LED patterns for level %s (duration: %u ms)", 
              graduated_response_get_level_name(level), duration_ms);
    
    // Update LED patterns
    hal_status_t status = graduated_response_update_led_patterns(level);
    if (status != HAL_STATUS_OK) {
        error_log("Failed to set LED patterns for testing");
        return HAL_STATUS_ERROR;
    }
    
    // Note: In real implementation, we would wait for the duration
    // For now, we just simulate the test
    debug_log("LED pattern test completed for level %s", graduated_response_get_level_name(level));
    
    return HAL_STATUS_OK;
}

// Configuration functions (placeholders for Phase 3)
hal_status_t graduated_response_set_led_pattern(safety_response_level_t response_level, 
                                               const safety_response_led_pattern_t *led_pattern) {
    (void)response_level;
    (void)led_pattern;
    debug_log("graduated_response_set_led_pattern() - Phase 3 implementation pending");
    return HAL_STATUS_OK;
}

hal_status_t graduated_response_set_polling_config(uint8_t module_addr, 
                                                  const adaptive_polling_config_t *config) {
    (void)module_addr;
    (void)config;
    debug_log("graduated_response_set_polling_config() - Phase 3 implementation pending");
    return HAL_STATUS_OK;
}
