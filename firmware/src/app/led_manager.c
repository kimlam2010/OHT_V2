/**
 * @file led_manager.c
 * @brief LED Manager Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-03 (LED Manager Implementation)
 */

#include "led_manager.h"
#include <string.h>
#include <stdio.h>

// Internal LED manager structure
typedef struct {
    led_mgr_config_t config;
    led_mgr_status_t status;
    led_mgr_event_callback_t event_callback;
    bool initialized;
    uint64_t last_update_time;
    uint64_t animation_start_time;
    uint8_t current_brightness;
    system_state_t last_system_state;
    safety_level_t last_safety_level;
    bool last_communication_status;
    bool last_communication_error;
} led_manager_t;

// Global LED manager instance
static led_manager_t g_led_manager = {0};

// Predefined LED patterns
static const led_mgr_pattern_t system_state_patterns[] = {
    // SYSTEM_STATE_INIT
    {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_BLINK_SLOW,
        .priority = LED_MGR_PRIORITY_NORMAL,
        .duration_ms = 0,
        .blink_period_ms = 1000,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = false
    },
    // SYSTEM_STATE_IDLE
    {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_SOLID,
        .priority = LED_MGR_PRIORITY_NORMAL,
        .duration_ms = 0,
        .blink_period_ms = 0,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = true,
        .network_led_enabled = true,
        .error_led_enabled = false
    },
    // SYSTEM_STATE_MOVE
    {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_BLINK_FAST,
        .priority = LED_MGR_PRIORITY_HIGH,
        .duration_ms = 0,
        .blink_period_ms = 200,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = true,
        .network_led_enabled = true,
        .error_led_enabled = false
    },
    // SYSTEM_STATE_DOCK
    {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_PULSE,
        .priority = LED_MGR_PRIORITY_HIGH,
        .duration_ms = 0,
        .blink_period_ms = 500,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = true,
        .network_led_enabled = true,
        .error_led_enabled = false
    },
    // SYSTEM_STATE_FAULT
    {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_BLINK_FAST,
        .priority = LED_MGR_PRIORITY_CRITICAL,
        .duration_ms = 0,
        .blink_period_ms = 200,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = true
    },
    // SYSTEM_STATE_ESTOP
    {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_SOLID,
        .priority = LED_MGR_PRIORITY_EMERGENCY,
        .duration_ms = 0,
        .blink_period_ms = 0,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = true
    },
    // SYSTEM_STATE_SHUTDOWN
    {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_BLINK_SLOW,
        .priority = LED_MGR_PRIORITY_HIGH,
        .duration_ms = 0,
        .blink_period_ms = 1000,
        .fade_period_ms = 0,
        .brightness = 50,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = false
    }
};

// Safety level patterns
static const led_mgr_pattern_t safety_level_patterns[] = {
    // SAFETY_LEVEL_NORMAL
    {
        .type = LED_MGR_PATTERN_TYPE_SAFETY_CONDITION,
        .animation = LED_MGR_ANIMATION_SOLID,
        .priority = LED_MGR_PRIORITY_NORMAL,
        .duration_ms = 0,
        .blink_period_ms = 0,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = true,
        .network_led_enabled = true,
        .error_led_enabled = false
    },
    // SAFETY_LEVEL_WARNING
    {
        .type = LED_MGR_PATTERN_TYPE_SAFETY_CONDITION,
        .animation = LED_MGR_ANIMATION_BLINK_SLOW,
        .priority = LED_MGR_PRIORITY_HIGH,
        .duration_ms = 0,
        .blink_period_ms = 1000,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = true,
        .network_led_enabled = true,
        .error_led_enabled = false
    },
    // SAFETY_LEVEL_CRITICAL
    {
        .type = LED_MGR_PATTERN_TYPE_SAFETY_CONDITION,
        .animation = LED_MGR_ANIMATION_BLINK_FAST,
        .priority = LED_MGR_PRIORITY_CRITICAL,
        .duration_ms = 0,
        .blink_period_ms = 200,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = true
    },
    // SAFETY_LEVEL_EMERGENCY
    {
        .type = LED_MGR_PATTERN_TYPE_SAFETY_CONDITION,
        .animation = LED_MGR_ANIMATION_SOLID,
        .priority = LED_MGR_PRIORITY_EMERGENCY,
        .duration_ms = 0,
        .blink_period_ms = 0,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = true
    }
};

// Forward declarations
static hal_status_t apply_led_pattern(const led_mgr_pattern_t *pattern);
static hal_status_t update_led_animation(void);
static hal_status_t check_pattern_timeout(void);
static hal_status_t handle_led_event(led_mgr_event_t event);
static led_state_t animation_to_led_state(led_mgr_animation_t animation, uint32_t frame);

// LED manager implementation
hal_status_t led_manager_init(const led_mgr_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize LED manager structure
    memset(&g_led_manager, 0, sizeof(led_manager_t));
    
    // Set configuration
    memcpy(&g_led_manager.config, config, sizeof(led_mgr_config_t));
    
    // Initialize status
    g_led_manager.status.current_pattern.type = LED_MGR_PATTERN_TYPE_NONE;
    g_led_manager.status.previous_pattern.type = LED_MGR_PATTERN_TYPE_NONE;
    g_led_manager.status.current_priority = LED_MGR_PRIORITY_NORMAL;
    g_led_manager.status.pattern_start_time = hal_get_timestamp_us();
    g_led_manager.status.last_update_time = hal_get_timestamp_us();
    g_led_manager.status.pattern_active = false;
    g_led_manager.status.pattern_count = 0;
    g_led_manager.status.animation_frame = 0;
    
    // Initialize other variables
    g_led_manager.last_update_time = hal_get_timestamp_us();
    g_led_manager.animation_start_time = hal_get_timestamp_us();
    g_led_manager.current_brightness = config->default_brightness;
    g_led_manager.last_system_state = SYSTEM_STATE_INIT;
    g_led_manager.last_safety_level = SAFETY_LEVEL_NORMAL;
    g_led_manager.last_communication_status = false;
    g_led_manager.last_communication_error = false;
    g_led_manager.initialized = true;
    
    // Set initial LED pattern
    led_manager_system_ready();
    
    return HAL_STATUS_OK;
}

hal_status_t led_manager_deinit(void) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Clear all LEDs
    hal_led_power_set(LED_STATE_OFF);
    hal_led_system_set(LED_STATE_OFF);
    hal_led_comm_set(LED_STATE_OFF);
    hal_led_network_set(LED_STATE_OFF);
    hal_led_error_set(LED_STATE_OFF);
    
    // Clear LED manager
    memset(&g_led_manager, 0, sizeof(led_manager_t));
    
    return HAL_STATUS_OK;
}

hal_status_t led_manager_update(void) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = hal_get_timestamp_us();
    g_led_manager.last_update_time = current_time;
    g_led_manager.status.last_update_time = current_time;
    
    // Update LED animation
    update_led_animation();
    
    // Check pattern timeout
    check_pattern_timeout();
    
    // Update HAL LED module
    hal_led_update();
    
    return HAL_STATUS_OK;
}

hal_status_t led_manager_set_pattern(const led_mgr_pattern_t *pattern) {
    if (!g_led_manager.initialized || pattern == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Store previous pattern
    memcpy(&g_led_manager.status.previous_pattern, &g_led_manager.status.current_pattern, sizeof(led_mgr_pattern_t));
    
    // Set new pattern
    memcpy(&g_led_manager.status.current_pattern, pattern, sizeof(led_mgr_pattern_t));
    g_led_manager.status.current_priority = pattern->priority;
    g_led_manager.status.pattern_start_time = hal_get_timestamp_us();
    g_led_manager.status.pattern_active = true;
    g_led_manager.status.pattern_count++;
    g_led_manager.status.animation_frame = 0;
    g_led_manager.animation_start_time = hal_get_timestamp_us();
    
    // Apply pattern to LEDs
    hal_status_t status = apply_led_pattern(pattern);
    
    // Handle event
    handle_led_event(LED_MGR_EVENT_SYSTEM_STATE_CHANGED);
    
    return status;
}

hal_status_t led_manager_get_pattern(led_mgr_pattern_t *pattern) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (pattern == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(pattern, &g_led_manager.status.current_pattern, sizeof(led_mgr_pattern_t));
    return HAL_STATUS_OK;
}

hal_status_t led_manager_clear_pattern(void) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Clear current pattern
    memset(&g_led_manager.status.current_pattern, 0, sizeof(led_mgr_pattern_t));
    g_led_manager.status.current_pattern.type = LED_MGR_PATTERN_TYPE_NONE;
    g_led_manager.status.pattern_active = false;
    
    // Turn off all LEDs
    hal_led_power_set(LED_STATE_OFF);
    hal_led_system_set(LED_STATE_OFF);
    hal_led_comm_set(LED_STATE_OFF);
    hal_led_network_set(LED_STATE_OFF);
    hal_led_error_set(LED_STATE_OFF);
    
    return HAL_STATUS_OK;
}

hal_status_t led_manager_set_callback(led_mgr_event_callback_t callback) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_led_manager.event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t led_manager_get_status(led_mgr_status_t *status) {
    if (!g_led_manager.initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(status, &g_led_manager.status, sizeof(led_mgr_status_t));
    return HAL_STATUS_OK;
}

// System State LED Functions
hal_status_t led_manager_set_system_state_pattern(system_state_t state) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (state >= sizeof(system_state_patterns) / sizeof(system_state_patterns[0])) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return led_manager_set_pattern(&system_state_patterns[state]);
}

hal_status_t led_manager_update_system_state(system_state_t state) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check if state has changed
    if (state != g_led_manager.last_system_state) {
        g_led_manager.last_system_state = state;
        return led_manager_set_system_state_pattern(state);
    }
    
    return HAL_STATUS_OK;
}

// Safety Condition LED Functions
hal_status_t led_manager_set_safety_pattern(safety_level_t level) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (level >= sizeof(safety_level_patterns) / sizeof(safety_level_patterns[0])) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return led_manager_set_pattern(&safety_level_patterns[level]);
}

hal_status_t led_manager_update_safety_level(safety_level_t level) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check if safety level has changed
    if (level != g_led_manager.last_safety_level) {
        g_led_manager.last_safety_level = level;
        return led_manager_set_safety_pattern(level);
    }
    
    return HAL_STATUS_OK;
}

// Communication LED Functions
hal_status_t led_manager_set_communication_pattern(bool connected, bool error) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    led_mgr_pattern_t pattern = {
        .type = LED_MGR_PATTERN_TYPE_COMMUNICATION,
        .animation = error ? LED_MGR_ANIMATION_BLINK_FAST : (connected ? LED_MGR_ANIMATION_SOLID : LED_MGR_ANIMATION_BLINK_SLOW),
        .priority = error ? LED_MGR_PRIORITY_HIGH : LED_MGR_PRIORITY_NORMAL,
        .duration_ms = 0,
        .blink_period_ms = error ? 200 : (connected ? 0 : 1000),
        .fade_period_ms = 0,
        .brightness = g_led_manager.current_brightness,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = true,
        .network_led_enabled = connected,
        .error_led_enabled = error
    };
    
    return led_manager_set_pattern(&pattern);
}

hal_status_t led_manager_update_communication_status(bool connected, bool error) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check if communication status has changed
    if (connected != g_led_manager.last_communication_status || 
        error != g_led_manager.last_communication_error) {
        g_led_manager.last_communication_status = connected;
        g_led_manager.last_communication_error = error;
        return led_manager_set_communication_pattern(connected, error);
    }
    
    return HAL_STATUS_OK;
}

// Error LED Functions
hal_status_t led_manager_set_error_pattern(uint32_t error_type, uint8_t error_severity) {
    (void)error_type; // Suppress unused parameter warning
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    led_mgr_pattern_t pattern = {
        .type = LED_MGR_PATTERN_TYPE_ERROR,
        .animation = LED_MGR_ANIMATION_BLINK_FAST,
        .priority = (error_severity > 7) ? LED_MGR_PRIORITY_EMERGENCY : 
                   (error_severity > 5) ? LED_MGR_PRIORITY_CRITICAL : LED_MGR_PRIORITY_HIGH,
        .duration_ms = 0,
        .blink_period_ms = 200,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = true
    };
    
    return led_manager_set_pattern(&pattern);
}

hal_status_t led_manager_clear_error_pattern(void) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Return to previous pattern or system ready
    if (g_led_manager.status.previous_pattern.type != LED_MGR_PATTERN_TYPE_NONE) {
        return led_manager_set_pattern(&g_led_manager.status.previous_pattern);
    } else {
        return led_manager_system_ready();
    }
}

// Animation Functions
hal_status_t led_manager_start_animation(led_mgr_animation_t animation, uint32_t duration_ms) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_led_manager.status.current_pattern.animation = animation;
    g_led_manager.status.current_pattern.duration_ms = duration_ms;
    g_led_manager.animation_start_time = hal_get_timestamp_us();
    g_led_manager.status.animation_frame = 0;
    
    return HAL_STATUS_OK;
}

hal_status_t led_manager_stop_animation(void) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_led_manager.status.current_pattern.animation = LED_MGR_ANIMATION_SOLID;
    g_led_manager.status.current_pattern.duration_ms = 0;
    
    return HAL_STATUS_OK;
}

hal_status_t led_manager_set_brightness(uint8_t brightness) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (brightness > 100) {
        brightness = 100;
    }
    
    g_led_manager.current_brightness = brightness;
    g_led_manager.status.current_pattern.brightness = brightness;
    
    // Apply brightness to all LEDs
    hal_led_set_brightness(LED_POWER_PIN, brightness);
    hal_led_set_brightness(LED_SYSTEM_PIN, brightness);
    hal_led_set_brightness(LED_COMM_PIN, brightness);
    hal_led_set_brightness(LED_NETWORK_PIN, brightness);
    hal_led_set_brightness(LED_ERROR_PIN, brightness);
    
    return HAL_STATUS_OK;
}

hal_status_t led_manager_get_brightness(uint8_t *brightness) {
    if (!g_led_manager.initialized || brightness == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *brightness = g_led_manager.current_brightness;
    return HAL_STATUS_OK;
}

// Configuration Functions
hal_status_t led_manager_set_config(const led_mgr_config_t *config) {
    if (!g_led_manager.initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_led_manager.config, config, sizeof(led_mgr_config_t));
    return HAL_STATUS_OK;
}

hal_status_t led_manager_get_config(led_mgr_config_t *config) {
    if (!g_led_manager.initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_led_manager.config, sizeof(led_mgr_config_t));
    return HAL_STATUS_OK;
}

// Utility Functions
const char* led_manager_get_pattern_type_name(led_mgr_pattern_type_t type) {
    switch (type) {
        case LED_MGR_PATTERN_TYPE_NONE: return "NONE";
        case LED_MGR_PATTERN_TYPE_SYSTEM_STATE: return "SYSTEM_STATE";
        case LED_MGR_PATTERN_TYPE_SAFETY_CONDITION: return "SAFETY_CONDITION";
        case LED_MGR_PATTERN_TYPE_COMMUNICATION: return "COMMUNICATION";
        case LED_MGR_PATTERN_TYPE_ERROR: return "ERROR";
        case LED_MGR_PATTERN_TYPE_CUSTOM: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

const char* led_manager_get_animation_name(led_mgr_animation_t animation) {
    switch (animation) {
        case LED_MGR_ANIMATION_NONE: return "NONE";
        case LED_MGR_ANIMATION_SOLID: return "SOLID";
        case LED_MGR_ANIMATION_BLINK_SLOW: return "BLINK_SLOW";
        case LED_MGR_ANIMATION_BLINK_FAST: return "BLINK_FAST";
        case LED_MGR_ANIMATION_PULSE: return "PULSE";
        case LED_MGR_ANIMATION_FADE: return "FADE";
        case LED_MGR_ANIMATION_CHASE: return "CHASE";
        case LED_MGR_ANIMATION_BREATH: return "BREATH";
        default: return "UNKNOWN";
    }
}

const char* led_manager_get_priority_name(led_mgr_priority_t priority) {
    switch (priority) {
        case LED_MGR_PRIORITY_LOW: return "LOW";
        case LED_MGR_PRIORITY_NORMAL: return "NORMAL";
        case LED_MGR_PRIORITY_HIGH: return "HIGH";
        case LED_MGR_PRIORITY_CRITICAL: return "CRITICAL";
        case LED_MGR_PRIORITY_EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

hal_status_t led_manager_get_diagnostics(char *info, size_t max_len) {
    if (!g_led_manager.initialized || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(info, max_len,
        "LED Manager Diagnostics:\n"
        "Current Pattern Type: %s\n"
        "Current Animation: %s\n"
        "Current Priority: %s\n"
        "Pattern Active: %s\n"
        "Pattern Count: %u\n"
        "Animation Frame: %u\n"
        "Current Brightness: %u%%\n"
        "Last System State: %s\n"
        "Last Safety Level: %s\n"
        "Communication Status: %s\n"
        "Communication Error: %s\n"
        "Pattern Duration: %lu ms\n"
        "Last Update: %lu ms ago\n",
        led_manager_get_pattern_type_name(g_led_manager.status.current_pattern.type),
        led_manager_get_animation_name(g_led_manager.status.current_pattern.animation),
        led_manager_get_priority_name(g_led_manager.status.current_priority),
        g_led_manager.status.pattern_active ? "YES" : "NO",
        g_led_manager.status.pattern_count,
        g_led_manager.status.animation_frame,
        g_led_manager.current_brightness,
        "UNKNOWN", // system_state_machine_get_state_name(g_led_manager.last_system_state),
        "UNKNOWN", // safety_manager_get_level_name(g_led_manager.last_safety_level),
        g_led_manager.last_communication_status ? "CONNECTED" : "DISCONNECTED",
        g_led_manager.last_communication_error ? "YES" : "NO",
        (unsigned long)g_led_manager.status.current_pattern.duration_ms,
        (unsigned long)((hal_get_timestamp_us() - g_led_manager.status.last_update_time) / 1000)
    );
    
    return HAL_STATUS_OK;
}

hal_status_t led_manager_self_test(void) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Test all LED patterns
    hal_status_t status = HAL_STATUS_OK;
    
    // Test system ready pattern
    status = led_manager_system_ready();
    if (status != HAL_STATUS_OK) return status;
    
    hal_sleep_ms(500);
    
    // Test system error pattern
    status = led_manager_system_error();
    if (status != HAL_STATUS_OK) return status;
    
    hal_sleep_ms(500);
    
    // Test emergency stop pattern
    status = led_manager_emergency_stop();
    if (status != HAL_STATUS_OK) return status;
    
    hal_sleep_ms(500);
    
    // Return to system ready
    status = led_manager_system_ready();
    
    return status;
}

hal_status_t led_manager_reset(void) {
    if (!g_led_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Reset to default configuration
    led_mgr_config_t default_config = {
        .update_interval_ms = 100,
        .animation_speed_ms = 50,
        .enable_animations = true,
        .enable_transitions = true,
        .default_brightness = 100,
        .auto_dimming = false
    };
    
    led_manager_set_config(&default_config);
    led_manager_set_brightness(100);
    led_manager_system_ready();
    
    return HAL_STATUS_OK;
}

// Predefined Pattern Functions
hal_status_t led_manager_system_ready(void) {
    led_mgr_pattern_t pattern = {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_SOLID,
        .priority = LED_MGR_PRIORITY_NORMAL,
        .duration_ms = 0,
        .blink_period_ms = 0,
        .fade_period_ms = 0,
        .brightness = g_led_manager.current_brightness,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = true,
        .network_led_enabled = true,
        .error_led_enabled = false
    };
    
    return led_manager_set_pattern(&pattern);
}

hal_status_t led_manager_system_error(void) {
    led_mgr_pattern_t pattern = {
        .type = LED_MGR_PATTERN_TYPE_ERROR,
        .animation = LED_MGR_ANIMATION_BLINK_FAST,
        .priority = LED_MGR_PRIORITY_CRITICAL,
        .duration_ms = 0,
        .blink_period_ms = 200,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = true
    };
    
    return led_manager_set_pattern(&pattern);
}

hal_status_t led_manager_system_warning(void) {
    led_mgr_pattern_t pattern = {
        .type = LED_MGR_PATTERN_TYPE_SAFETY_CONDITION,
        .animation = LED_MGR_ANIMATION_BLINK_SLOW,
        .priority = LED_MGR_PRIORITY_HIGH,
        .duration_ms = 0,
        .blink_period_ms = 1000,
        .fade_period_ms = 0,
        .brightness = g_led_manager.current_brightness,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = true,
        .network_led_enabled = true,
        .error_led_enabled = false
    };
    
    return led_manager_set_pattern(&pattern);
}

hal_status_t led_manager_system_shutdown(void) {
    led_mgr_pattern_t pattern = {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_BLINK_SLOW,
        .priority = LED_MGR_PRIORITY_HIGH,
        .duration_ms = 0,
        .blink_period_ms = 1000,
        .fade_period_ms = 0,
        .brightness = 50,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = false
    };
    
    return led_manager_set_pattern(&pattern);
}

hal_status_t led_manager_emergency_stop(void) {
    led_mgr_pattern_t pattern = {
        .type = LED_MGR_PATTERN_TYPE_ERROR,
        .animation = LED_MGR_ANIMATION_SOLID,
        .priority = LED_MGR_PRIORITY_EMERGENCY,
        .duration_ms = 0,
        .blink_period_ms = 0,
        .fade_period_ms = 0,
        .brightness = 100,
        .power_led_enabled = true,
        .system_led_enabled = false,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = true
    };
    
    return led_manager_set_pattern(&pattern);
}

// Internal helper functions
static hal_status_t apply_led_pattern(const led_mgr_pattern_t *pattern) {
    if (pattern == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Apply pattern to individual LEDs
    if (pattern->power_led_enabled) {
        hal_led_set_state(LED_POWER_PIN, LED_STATE_ON);
    } else {
        hal_led_set_state(LED_POWER_PIN, LED_STATE_OFF);
    }
    
    if (pattern->system_led_enabled) {
        hal_led_set_state(LED_SYSTEM_PIN, LED_STATE_ON);
    } else {
        hal_led_set_state(LED_SYSTEM_PIN, LED_STATE_OFF);
    }
    
    if (pattern->comm_led_enabled) {
        hal_led_set_state(LED_COMM_PIN, LED_STATE_ON);
    } else {
        hal_led_set_state(LED_COMM_PIN, LED_STATE_OFF);
    }
    
    if (pattern->network_led_enabled) {
        hal_led_set_state(LED_NETWORK_PIN, LED_STATE_ON);
    } else {
        hal_led_set_state(LED_NETWORK_PIN, LED_STATE_OFF);
    }
    
    if (pattern->error_led_enabled) {
        hal_led_set_state(LED_ERROR_PIN, LED_STATE_ON);
    } else {
        hal_led_set_state(LED_ERROR_PIN, LED_STATE_OFF);
    }
    
    // Apply brightness
    hal_led_set_brightness(LED_POWER_PIN, pattern->brightness);
    hal_led_set_brightness(LED_SYSTEM_PIN, pattern->brightness);
    hal_led_set_brightness(LED_COMM_PIN, pattern->brightness);
    hal_led_set_brightness(LED_NETWORK_PIN, pattern->brightness);
    hal_led_set_brightness(LED_ERROR_PIN, pattern->brightness);
    
    return HAL_STATUS_OK;
}

static hal_status_t update_led_animation(void) {
    if (!g_led_manager.status.pattern_active) {
        return HAL_STATUS_OK;
    }
    
    const led_mgr_pattern_t *pattern = &g_led_manager.status.current_pattern;
    uint64_t current_time = hal_get_timestamp_us();
    uint64_t animation_time = (current_time - g_led_manager.animation_start_time) / 1000ULL; // Convert to ms
    
    // Update animation frame
    if (pattern->blink_period_ms > 0) {
        g_led_manager.status.animation_frame = (animation_time / pattern->blink_period_ms) % 2;
    } else {
        g_led_manager.status.animation_frame = 0;
    }
    
    // Apply animation state to LEDs
    led_state_t led_state = animation_to_led_state(pattern->animation, g_led_manager.status.animation_frame);
    
    if (pattern->power_led_enabled) {
        hal_led_set_state(LED_POWER_PIN, led_state);
    }
    if (pattern->system_led_enabled) {
        hal_led_set_state(LED_SYSTEM_PIN, led_state);
    }
    if (pattern->comm_led_enabled) {
        hal_led_set_state(LED_COMM_PIN, led_state);
    }
    if (pattern->network_led_enabled) {
        hal_led_set_state(LED_NETWORK_PIN, led_state);
    }
    if (pattern->error_led_enabled) {
        hal_led_set_state(LED_ERROR_PIN, led_state);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t check_pattern_timeout(void) {
    if (!g_led_manager.status.pattern_active) {
        return HAL_STATUS_OK;
    }
    
    const led_mgr_pattern_t *pattern = &g_led_manager.status.current_pattern;
    
    // Check if pattern has timed out
    if (pattern->duration_ms > 0) {
        uint64_t current_time = hal_get_timestamp_us();
        uint64_t pattern_duration = (current_time - g_led_manager.status.pattern_start_time) / 1000ULL;
        
        if (pattern_duration >= pattern->duration_ms) {
            // Pattern timeout - clear pattern
            led_manager_clear_pattern();
            handle_led_event(LED_MGR_EVENT_PATTERN_TIMEOUT);
        }
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t handle_led_event(led_mgr_event_t event) {
    // Call event callback if set
    if (g_led_manager.event_callback != NULL) {
        g_led_manager.event_callback(event, g_led_manager.status.current_pattern);
    }
    
    return HAL_STATUS_OK;
}

static led_state_t animation_to_led_state(led_mgr_animation_t animation, uint32_t frame) {
    switch (animation) {
        case LED_MGR_ANIMATION_SOLID:
            return LED_STATE_ON;
            
        case LED_MGR_ANIMATION_BLINK_SLOW:
        case LED_MGR_ANIMATION_BLINK_FAST:
            return (frame == 0) ? LED_STATE_ON : LED_STATE_OFF;
            
        case LED_MGR_ANIMATION_PULSE:
            return (frame == 0) ? LED_STATE_ON : LED_STATE_OFF;
            
        case LED_MGR_ANIMATION_FADE:
        case LED_MGR_ANIMATION_CHASE:
        case LED_MGR_ANIMATION_BREATH:
            // For now, treat as solid
            return LED_STATE_ON;
            
        case LED_MGR_ANIMATION_NONE:
        default:
            return LED_STATE_OFF;
    }
}
