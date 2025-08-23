/**
 * @file safety_mechanisms.c
 * @brief Safety Mechanisms Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-05 (Safety Mechanisms Implementation)
 */

#include "safety_mechanisms.h"
#include "hal_common.h"
#include "hal_estop.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

// Internal safety mechanisms structure
typedef struct {
    safety_config_t config;
    safety_status_t status;
    safety_stats_t stats;
    bool initialized;
    
    // E-Stop monitoring
    bool estop_triggered;
    uint64_t estop_trigger_time;
    uint32_t estop_trigger_count;
    
    // Collision detection
    bool collision_detected;
    float collision_distance;
    uint64_t collision_detect_time;
    uint32_t collision_event_count;
    
    // Speed monitoring
    float current_speed;
    float max_speed_recorded;
    bool speed_limit_exceeded;
    uint64_t speed_violation_time;
    uint32_t speed_violation_count;
    
    // Position monitoring
    float current_position;
    float min_position_recorded;
    float max_position_recorded;
    bool position_limit_exceeded;
    uint64_t position_violation_time;
    uint32_t position_violation_count;
    
    // Safety zone monitoring
    bool safety_zone_violated;
    uint64_t zone_violation_time;
    uint32_t zone_violation_count;
    
    // Timing
    uint64_t last_update_time;
    uint64_t start_time;
    
    // Fault tracking
    safety_fault_t current_fault;
    uint64_t fault_time;
    uint32_t fault_count;
    
} safety_mechanisms_t;

// Global safety mechanisms instance
static safety_mechanisms_t g_safety_mechanisms = {0};

// Forward declarations
static hal_status_t check_estop_status(void);
static hal_status_t check_collision_detection(void);
static hal_status_t check_speed_monitoring(void);
static hal_status_t check_position_monitoring(void);
static hal_status_t check_safety_zone(void);
static hal_status_t update_safety_state(void);
static hal_status_t update_statistics(void);
static hal_status_t trigger_safety_response(safety_fault_t fault);
static float clamp_value(float value, float min, float max);

// Safety mechanisms implementation
hal_status_t safety_mechanisms_init(const safety_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!safety_mechanisms_validate_config(config)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (g_safety_mechanisms.initialized) {
        return HAL_STATUS_OK;
    }
    
    // Initialize safety mechanisms structure
    memset(&g_safety_mechanisms, 0, sizeof(safety_mechanisms_t));
    
    // Copy configuration
    memcpy(&g_safety_mechanisms.config, config, sizeof(safety_config_t));
    
    // Initialize status
    g_safety_mechanisms.status.state = SAFETY_STATE_SAFE;
    g_safety_mechanisms.status.fault = SAFETY_FAULT_NONE;
    g_safety_mechanisms.status.estop_active = false;
    g_safety_mechanisms.status.collision_detected = false;
    g_safety_mechanisms.status.speed_limit_exceeded = false;
    g_safety_mechanisms.status.position_limit_exceeded = false;
    g_safety_mechanisms.status.safety_zone_violated = false;
    g_safety_mechanisms.status.violation_count = 0;
    g_safety_mechanisms.status.last_update_time = hal_get_timestamp_us();
    g_safety_mechanisms.start_time = hal_get_timestamp_us();
    
    // Initialize statistics
    g_safety_mechanisms.stats.total_violations = 0;
    g_safety_mechanisms.stats.estop_triggers = 0;
    g_safety_mechanisms.stats.collision_events = 0;
    g_safety_mechanisms.stats.speed_violations = 0;
    g_safety_mechanisms.stats.position_violations = 0;
    g_safety_mechanisms.stats.total_runtime = 0;
    g_safety_mechanisms.stats.max_speed_recorded = 0.0f;
    g_safety_mechanisms.stats.min_position_recorded = 0.0f;
    g_safety_mechanisms.stats.max_position_recorded = 0.0f;
    
    // Initialize E-Stop monitoring
    g_safety_mechanisms.estop_triggered = false;
    g_safety_mechanisms.estop_trigger_time = 0;
    g_safety_mechanisms.estop_trigger_count = 0;
    
    // Initialize collision detection
    g_safety_mechanisms.collision_detected = false;
    g_safety_mechanisms.collision_distance = 0.0f;
    g_safety_mechanisms.collision_detect_time = 0;
    g_safety_mechanisms.collision_event_count = 0;
    
    // Initialize speed monitoring
    g_safety_mechanisms.current_speed = 0.0f;
    g_safety_mechanisms.max_speed_recorded = 0.0f;
    g_safety_mechanisms.speed_limit_exceeded = false;
    g_safety_mechanisms.speed_violation_time = 0;
    g_safety_mechanisms.speed_violation_count = 0;
    
    // Initialize position monitoring
    g_safety_mechanisms.current_position = 0.0f;
    g_safety_mechanisms.min_position_recorded = 0.0f;
    g_safety_mechanisms.max_position_recorded = 0.0f;
    g_safety_mechanisms.position_limit_exceeded = false;
    g_safety_mechanisms.position_violation_time = 0;
    g_safety_mechanisms.position_violation_count = 0;
    
    // Initialize safety zone monitoring
    g_safety_mechanisms.safety_zone_violated = false;
    g_safety_mechanisms.zone_violation_time = 0;
    g_safety_mechanisms.zone_violation_count = 0;
    
    // Initialize fault tracking
    g_safety_mechanisms.current_fault = SAFETY_FAULT_NONE;
    g_safety_mechanisms.fault_time = 0;
    g_safety_mechanisms.fault_count = 0;
    
    g_safety_mechanisms.initialized = true;
    
    printf("Safety mechanisms initialized successfully (Level: %s)\n", 
           safety_mechanisms_get_level_name(config->level));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_deinit(void) {
    if (!g_safety_mechanisms.initialized) {
        return HAL_STATUS_OK;
    }
    
    // Clear safety mechanisms
    memset(&g_safety_mechanisms, 0, sizeof(safety_mechanisms_t));
    
    printf("Safety mechanisms deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_update(void) {
    if (!g_safety_mechanisms.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = hal_get_timestamp_us();
    float dt = (current_time - g_safety_mechanisms.last_update_time) / 1000000.0f;
    
    // Update timing
    g_safety_mechanisms.last_update_time = current_time;
    g_safety_mechanisms.status.last_update_time = current_time;
    
    // Check E-Stop status
    check_estop_status();
    
    // Check collision detection
    check_collision_detection();
    
    // Check speed monitoring
    check_speed_monitoring();
    
    // Check position monitoring
    check_position_monitoring();
    
    // Check safety zone
    check_safety_zone();
    
    // Update safety state
    update_safety_state();
    
    // Update statistics
    update_statistics();
    
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_get_status(safety_status_t *status) {
    if (!g_safety_mechanisms.initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(status, &g_safety_mechanisms.status, sizeof(safety_status_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_get_stats(safety_stats_t *stats) {
    if (!g_safety_mechanisms.initialized || stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(stats, &g_safety_mechanisms.stats, sizeof(safety_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_reset_stats(void) {
    if (!g_safety_mechanisms.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&g_safety_mechanisms.stats, 0, sizeof(safety_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_is_safe(bool *safe) {
    if (!g_safety_mechanisms.initialized || safe == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *safe = (g_safety_mechanisms.status.state == SAFETY_STATE_SAFE);
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_is_estop_active(bool *active) {
    if (!g_safety_mechanisms.initialized || active == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *active = g_safety_mechanisms.status.estop_active;
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_is_collision_detected(bool *detected) {
    if (!g_safety_mechanisms.initialized || detected == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *detected = g_safety_mechanisms.status.collision_detected;
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_is_speed_limit_exceeded(bool *exceeded) {
    if (!g_safety_mechanisms.initialized || exceeded == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *exceeded = g_safety_mechanisms.status.speed_limit_exceeded;
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_is_position_limit_exceeded(bool *exceeded) {
    if (!g_safety_mechanisms.initialized || exceeded == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *exceeded = g_safety_mechanisms.status.position_limit_exceeded;
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_is_safety_zone_violated(bool *violated) {
    if (!g_safety_mechanisms.initialized || violated == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *violated = g_safety_mechanisms.status.safety_zone_violated;
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_set_safety_zone(const safety_zone_t *zone) {
    if (!g_safety_mechanisms.initialized || zone == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_safety_mechanisms.config.zone, zone, sizeof(safety_zone_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_get_safety_zone(safety_zone_t *zone) {
    if (!g_safety_mechanisms.initialized || zone == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(zone, &g_safety_mechanisms.config.zone, sizeof(safety_zone_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_set_collision_detection(const collision_detection_t *collision) {
    if (!g_safety_mechanisms.initialized || collision == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_safety_mechanisms.config.collision, collision, sizeof(collision_detection_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_get_collision_detection(collision_detection_t *collision) {
    if (!g_safety_mechanisms.initialized || collision == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(collision, &g_safety_mechanisms.config.collision, sizeof(collision_detection_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_set_speed_monitoring(const speed_monitoring_t *speed) {
    if (!g_safety_mechanisms.initialized || speed == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_safety_mechanisms.config.speed, speed, sizeof(speed_monitoring_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_get_speed_monitoring(speed_monitoring_t *speed) {
    if (!g_safety_mechanisms.initialized || speed == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(speed, &g_safety_mechanisms.config.speed, sizeof(speed_monitoring_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_set_position_monitoring(const position_monitoring_t *position) {
    if (!g_safety_mechanisms.initialized || position == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_safety_mechanisms.config.position, position, sizeof(position_monitoring_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_get_position_monitoring(position_monitoring_t *position) {
    if (!g_safety_mechanisms.initialized || position == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(position, &g_safety_mechanisms.config.position, sizeof(position_monitoring_t));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_trigger_emergency_stop(void) {
    if (!g_safety_mechanisms.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_mechanisms.estop_triggered = true;
    g_safety_mechanisms.estop_trigger_time = hal_get_timestamp_us();
    g_safety_mechanisms.estop_trigger_count++;
    g_safety_mechanisms.stats.estop_triggers++;
    
    g_safety_mechanisms.status.estop_active = true;
    g_safety_mechanisms.status.state = SAFETY_STATE_EMERGENCY;
    g_safety_mechanisms.current_fault = SAFETY_FAULT_ESTOP;
    
    printf("Emergency stop triggered!\n");
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_reset_emergency_stop(void) {
    if (!g_safety_mechanisms.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_mechanisms.estop_triggered = false;
    g_safety_mechanisms.status.estop_active = false;
    
    // Only reset state if no other faults
    if (g_safety_mechanisms.current_fault == SAFETY_FAULT_ESTOP) {
        g_safety_mechanisms.current_fault = SAFETY_FAULT_NONE;
        g_safety_mechanisms.status.state = SAFETY_STATE_SAFE;
    }
    
    printf("Emergency stop reset\n");
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_clear_faults(void) {
    if (!g_safety_mechanisms.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_safety_mechanisms.current_fault = SAFETY_FAULT_NONE;
    g_safety_mechanisms.status.state = SAFETY_STATE_SAFE;
    g_safety_mechanisms.status.fault = SAFETY_FAULT_NONE;
    
    // Clear violation flags
    g_safety_mechanisms.status.collision_detected = false;
    g_safety_mechanisms.status.speed_limit_exceeded = false;
    g_safety_mechanisms.status.position_limit_exceeded = false;
    g_safety_mechanisms.status.safety_zone_violated = false;
    
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_get_fault(safety_fault_t *fault) {
    if (!g_safety_mechanisms.initialized || fault == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *fault = g_safety_mechanisms.current_fault;
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_set_safety_level(safety_level_t level) {
    if (!g_safety_mechanisms.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (level >= SAFETY_LEVEL_SIL4 + 1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    g_safety_mechanisms.config.level = level;
    printf("Safety level set to: %s\n", safety_mechanisms_get_level_name(level));
    return HAL_STATUS_OK;
}

hal_status_t safety_mechanisms_get_safety_level(safety_level_t *level) {
    if (!g_safety_mechanisms.initialized || level == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *level = g_safety_mechanisms.config.level;
    return HAL_STATUS_OK;
}

bool safety_mechanisms_validate_config(const safety_config_t *config) {
    if (config == NULL) {
        return false;
    }
    
    // Validate safety level
    if (config->level >= SAFETY_LEVEL_SIL4 + 1) {
        return false;
    }
    
    // Validate update frequency
    if (config->update_frequency <= 0 || config->update_frequency > 10000) {
        return false;
    }
    
    // Validate safety zone
    if (config->zone.enabled) {
        if (config->zone.min_position >= config->zone.max_position) {
            return false;
        }
        if (config->zone.min_velocity >= config->zone.max_velocity) {
            return false;
        }
        if (config->zone.min_acceleration >= config->zone.max_acceleration) {
            return false;
        }
    }
    
    // Validate collision detection
    if (config->collision.enabled) {
        if (config->collision.detection_distance <= 0.0f) {
            return false;
        }
        if (config->collision.reaction_time <= 0.0f) {
            return false;
        }
    }
    
    // Validate speed monitoring
    if (config->speed.enabled) {
        if (config->speed.max_speed <= 0.0f) {
            return false;
        }
        if (config->speed.warning_speed >= config->speed.max_speed) {
            return false;
        }
        if (config->speed.emergency_speed >= config->speed.max_speed) {
            return false;
        }
    }
    
    // Validate position monitoring
    if (config->position.enabled) {
        if (config->position.min_position >= config->position.max_position) {
            return false;
        }
        if (config->position.soft_limit_margin < 0.0f) {
            return false;
        }
        if (config->position.hard_limit_margin < 0.0f) {
            return false;
        }
    }
    
    return true;
}

hal_status_t safety_mechanisms_get_diagnostics(char *info, size_t max_len) {
    if (!g_safety_mechanisms.initialized || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(info, max_len,
        "Safety Mechanisms Diagnostics:\n"
        "State: %s\n"
        "Level: %s\n"
        "Fault: %s\n"
        "E-Stop Active: %s\n"
        "Collision Detected: %s\n"
        "Speed Limit Exceeded: %s\n"
        "Position Limit Exceeded: %s\n"
        "Safety Zone Violated: %s\n"
        "Current Speed: %.2f mm/s\n"
        "Current Position: %.2f mm\n"
        "Violation Count: %u\n"
        "E-Stop Triggers: %u\n"
        "Collision Events: %u\n"
        "Speed Violations: %u\n"
        "Position Violations: %u\n"
        "Max Speed Recorded: %.2f mm/s\n"
        "Min Position Recorded: %.2f mm\n"
        "Max Position Recorded: %.2f mm\n",
        safety_mechanisms_get_state_name(g_safety_mechanisms.status.state),
        safety_mechanisms_get_level_name(g_safety_mechanisms.config.level),
        safety_mechanisms_get_fault_name(g_safety_mechanisms.status.fault),
        g_safety_mechanisms.status.estop_active ? "YES" : "NO",
        g_safety_mechanisms.status.collision_detected ? "YES" : "NO",
        g_safety_mechanisms.status.speed_limit_exceeded ? "YES" : "NO",
        g_safety_mechanisms.status.position_limit_exceeded ? "YES" : "NO",
        g_safety_mechanisms.status.safety_zone_violated ? "YES" : "NO",
        g_safety_mechanisms.status.current_speed,
        g_safety_mechanisms.status.current_position,
        g_safety_mechanisms.status.violation_count,
        g_safety_mechanisms.stats.estop_triggers,
        g_safety_mechanisms.stats.collision_events,
        g_safety_mechanisms.stats.speed_violations,
        g_safety_mechanisms.stats.position_violations,
        g_safety_mechanisms.stats.max_speed_recorded,
        g_safety_mechanisms.stats.min_position_recorded,
        g_safety_mechanisms.stats.max_position_recorded
    );
    
    return HAL_STATUS_OK;
}

// Utility functions
const char* safety_mechanisms_get_level_name(safety_level_t level) {
    switch (level) {
        case SAFETY_LEVEL_NONE: return "NONE";
        case SAFETY_LEVEL_SIL1: return "SIL1";
        case SAFETY_LEVEL_SIL2: return "SIL2";
        case SAFETY_LEVEL_SIL3: return "SIL3";
        case SAFETY_LEVEL_SIL4: return "SIL4";
        default: return "UNKNOWN";
    }
}

const char* safety_mechanisms_get_state_name(safety_state_t state) {
    switch (state) {
        case SAFETY_STATE_SAFE: return "SAFE";
        case SAFETY_STATE_WARNING: return "WARNING";
        case SAFETY_STATE_VIOLATION: return "VIOLATION";
        case SAFETY_STATE_EMERGENCY: return "EMERGENCY";
        case SAFETY_STATE_FAULT: return "FAULT";
        default: return "UNKNOWN";
    }
}

const char* safety_mechanisms_get_fault_name(safety_fault_t fault) {
    switch (fault) {
        case SAFETY_FAULT_NONE: return "NONE";
        case SAFETY_FAULT_ESTOP: return "ESTOP";
        case SAFETY_FAULT_COLLISION: return "COLLISION";
        case SAFETY_FAULT_SPEED_LIMIT: return "SPEED_LIMIT";
        case SAFETY_FAULT_POSITION_LIMIT: return "POSITION_LIMIT";
        case SAFETY_FAULT_SENSOR: return "SENSOR";
        case SAFETY_FAULT_COMMUNICATION: return "COMMUNICATION";
        case SAFETY_FAULT_SOFTWARE: return "SOFTWARE";
        case SAFETY_FAULT_HARDWARE: return "HARDWARE";
        default: return "UNKNOWN";
    }
}

// Internal helper functions
static hal_status_t check_estop_status(void) {
    if (!g_safety_mechanisms.config.enable_estop) {
        return HAL_STATUS_OK;
    }
    
    bool estop_triggered = false;
    hal_status_t status = hal_estop_is_triggered(&estop_triggered);
    
    if (status == HAL_STATUS_OK) {
        if (estop_triggered && !g_safety_mechanisms.estop_triggered) {
            // E-Stop just triggered
            g_safety_mechanisms.estop_triggered = true;
            g_safety_mechanisms.estop_trigger_time = hal_get_timestamp_us();
            g_safety_mechanisms.estop_trigger_count++;
            g_safety_mechanisms.stats.estop_triggers++;
            
            g_safety_mechanisms.status.estop_active = true;
            g_safety_mechanisms.current_fault = SAFETY_FAULT_ESTOP;
            
            printf("E-Stop triggered!\n");
        } else if (!estop_triggered && g_safety_mechanisms.estop_triggered) {
            // E-Stop released
            g_safety_mechanisms.estop_triggered = false;
            g_safety_mechanisms.status.estop_active = false;
            
            // Only clear fault if it was E-Stop related
            if (g_safety_mechanisms.current_fault == SAFETY_FAULT_ESTOP) {
                g_safety_mechanisms.current_fault = SAFETY_FAULT_NONE;
            }
            
            printf("E-Stop released\n");
        }
    }
    
    return status;
}

static hal_status_t check_collision_detection(void) {
    if (!g_safety_mechanisms.config.collision.enabled) {
        return HAL_STATUS_OK;
    }
    
    // TODO: Integrate with actual collision detection sensors (LiDAR, etc.)
    // For now, use mock collision detection
    bool collision_detected = false;
    float collision_distance = 1000.0f; // Mock distance
    
    // Mock collision detection logic
    if (collision_distance < g_safety_mechanisms.config.collision.detection_distance) {
        collision_detected = true;
    }
    
    if (collision_detected && !g_safety_mechanisms.collision_detected) {
        // Collision just detected
        g_safety_mechanisms.collision_detected = true;
        g_safety_mechanisms.collision_distance = collision_distance;
        g_safety_mechanisms.collision_detect_time = hal_get_timestamp_us();
        g_safety_mechanisms.collision_event_count++;
        g_safety_mechanisms.stats.collision_events++;
        
        g_safety_mechanisms.status.collision_detected = true;
        g_safety_mechanisms.current_fault = SAFETY_FAULT_COLLISION;
        
        printf("Collision detected at distance: %.2f mm\n", collision_distance);
    } else if (!collision_detected && g_safety_mechanisms.collision_detected) {
        // Collision cleared
        g_safety_mechanisms.collision_detected = false;
        g_safety_mechanisms.status.collision_detected = false;
        
        // Only clear fault if it was collision related
        if (g_safety_mechanisms.current_fault == SAFETY_FAULT_COLLISION) {
            g_safety_mechanisms.current_fault = SAFETY_FAULT_NONE;
        }
        
        printf("Collision cleared\n");
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t check_speed_monitoring(void) {
    if (!g_safety_mechanisms.config.speed.enabled) {
        return HAL_STATUS_OK;
    }
    
    // TODO: Get actual speed from sensors/encoders
    // For now, use mock speed
    float current_speed = g_safety_mechanisms.current_speed;
    
    // Update current speed in status
    g_safety_mechanisms.status.current_speed = current_speed;
    
    // Check speed limits
    bool speed_limit_exceeded = false;
    if (fabsf(current_speed) > g_safety_mechanisms.config.speed.max_speed) {
        speed_limit_exceeded = true;
    }
    
    if (speed_limit_exceeded && !g_safety_mechanisms.speed_limit_exceeded) {
        // Speed limit just exceeded
        g_safety_mechanisms.speed_limit_exceeded = true;
        g_safety_mechanisms.speed_violation_time = hal_get_timestamp_us();
        g_safety_mechanisms.speed_violation_count++;
        g_safety_mechanisms.stats.speed_violations++;
        
        g_safety_mechanisms.status.speed_limit_exceeded = true;
        g_safety_mechanisms.current_fault = SAFETY_FAULT_SPEED_LIMIT;
        
        printf("Speed limit exceeded: %.2f mm/s (max: %.2f mm/s)\n", 
               current_speed, g_safety_mechanisms.config.speed.max_speed);
    } else if (!speed_limit_exceeded && g_safety_mechanisms.speed_limit_exceeded) {
        // Speed limit violation cleared
        g_safety_mechanisms.speed_limit_exceeded = false;
        g_safety_mechanisms.status.speed_limit_exceeded = false;
        
        // Only clear fault if it was speed related
        if (g_safety_mechanisms.current_fault == SAFETY_FAULT_SPEED_LIMIT) {
            g_safety_mechanisms.current_fault = SAFETY_FAULT_NONE;
        }
        
        printf("Speed limit violation cleared\n");
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t check_position_monitoring(void) {
    if (!g_safety_mechanisms.config.position.enabled) {
        return HAL_STATUS_OK;
    }
    
    // TODO: Get actual position from sensors/encoders
    // For now, use mock position
    float current_position = g_safety_mechanisms.current_position;
    
    // Update current position in status
    g_safety_mechanisms.status.current_position = current_position;
    
    // Check position limits
    bool position_limit_exceeded = false;
    if (current_position < g_safety_mechanisms.config.position.min_position ||
        current_position > g_safety_mechanisms.config.position.max_position) {
        position_limit_exceeded = true;
    }
    
    if (position_limit_exceeded && !g_safety_mechanisms.position_limit_exceeded) {
        // Position limit just exceeded
        g_safety_mechanisms.position_limit_exceeded = true;
        g_safety_mechanisms.position_violation_time = hal_get_timestamp_us();
        g_safety_mechanisms.position_violation_count++;
        g_safety_mechanisms.stats.position_violations++;
        
        g_safety_mechanisms.status.position_limit_exceeded = true;
        g_safety_mechanisms.current_fault = SAFETY_FAULT_POSITION_LIMIT;
        
        printf("Position limit exceeded: %.2f mm (range: %.2f - %.2f mm)\n", 
               current_position, 
               g_safety_mechanisms.config.position.min_position,
               g_safety_mechanisms.config.position.max_position);
    } else if (!position_limit_exceeded && g_safety_mechanisms.position_limit_exceeded) {
        // Position limit violation cleared
        g_safety_mechanisms.position_limit_exceeded = false;
        g_safety_mechanisms.status.position_limit_exceeded = false;
        
        // Only clear fault if it was position related
        if (g_safety_mechanisms.current_fault == SAFETY_FAULT_POSITION_LIMIT) {
            g_safety_mechanisms.current_fault = SAFETY_FAULT_NONE;
        }
        
        printf("Position limit violation cleared\n");
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t check_safety_zone(void) {
    if (!g_safety_mechanisms.config.zone.enabled) {
        return HAL_STATUS_OK;
    }
    
    bool zone_violated = false;
    
    // Check position zone
    if (g_safety_mechanisms.status.current_position < g_safety_mechanisms.config.zone.min_position ||
        g_safety_mechanisms.status.current_position > g_safety_mechanisms.config.zone.max_position) {
        zone_violated = true;
    }
    
    // Check velocity zone
    if (fabsf(g_safety_mechanisms.status.current_speed) < g_safety_mechanisms.config.zone.min_velocity ||
        fabsf(g_safety_mechanisms.status.current_speed) > g_safety_mechanisms.config.zone.max_velocity) {
        zone_violated = true;
    }
    
    if (zone_violated && !g_safety_mechanisms.safety_zone_violated) {
        // Safety zone just violated
        g_safety_mechanisms.safety_zone_violated = true;
        g_safety_mechanisms.zone_violation_time = hal_get_timestamp_us();
        g_safety_mechanisms.zone_violation_count++;
        
        g_safety_mechanisms.status.safety_zone_violated = true;
        
        printf("Safety zone violated\n");
    } else if (!zone_violated && g_safety_mechanisms.safety_zone_violated) {
        // Safety zone violation cleared
        g_safety_mechanisms.safety_zone_violated = false;
        g_safety_mechanisms.status.safety_zone_violated = false;
        
        printf("Safety zone violation cleared\n");
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t update_safety_state(void) {
    safety_state_t new_state = SAFETY_STATE_SAFE;
    safety_fault_t new_fault = SAFETY_FAULT_NONE;
    
    // Determine state based on violations
    if (g_safety_mechanisms.status.estop_active) {
        new_state = SAFETY_STATE_EMERGENCY;
        new_fault = SAFETY_FAULT_ESTOP;
    } else if (g_safety_mechanisms.status.collision_detected) {
        new_state = SAFETY_STATE_EMERGENCY;
        new_fault = SAFETY_FAULT_COLLISION;
    } else if (g_safety_mechanisms.status.speed_limit_exceeded) {
        new_state = SAFETY_STATE_VIOLATION;
        new_fault = SAFETY_FAULT_SPEED_LIMIT;
    } else if (g_safety_mechanisms.status.position_limit_exceeded) {
        new_state = SAFETY_STATE_VIOLATION;
        new_fault = SAFETY_FAULT_POSITION_LIMIT;
    } else if (g_safety_mechanisms.status.safety_zone_violated) {
        new_state = SAFETY_STATE_WARNING;
    }
    
    // Update state and fault
    g_safety_mechanisms.status.state = new_state;
    g_safety_mechanisms.status.fault = new_fault;
    g_safety_mechanisms.current_fault = new_fault;
    
    // Update violation count
    uint32_t violation_count = 0;
    if (g_safety_mechanisms.status.estop_active) violation_count++;
    if (g_safety_mechanisms.status.collision_detected) violation_count++;
    if (g_safety_mechanisms.status.speed_limit_exceeded) violation_count++;
    if (g_safety_mechanisms.status.position_limit_exceeded) violation_count++;
    if (g_safety_mechanisms.status.safety_zone_violated) violation_count++;
    
    g_safety_mechanisms.status.violation_count = violation_count;
    
    return HAL_STATUS_OK;
}

static hal_status_t update_statistics(void) {
    // Update maximum speed recorded
    if (fabsf(g_safety_mechanisms.status.current_speed) > g_safety_mechanisms.stats.max_speed_recorded) {
        g_safety_mechanisms.stats.max_speed_recorded = fabsf(g_safety_mechanisms.status.current_speed);
    }
    
    // Update position range recorded
    if (g_safety_mechanisms.status.current_position < g_safety_mechanisms.stats.min_position_recorded) {
        g_safety_mechanisms.stats.min_position_recorded = g_safety_mechanisms.status.current_position;
    }
    if (g_safety_mechanisms.status.current_position > g_safety_mechanisms.stats.max_position_recorded) {
        g_safety_mechanisms.stats.max_position_recorded = g_safety_mechanisms.status.current_position;
    }
    
    // Update total violations
    g_safety_mechanisms.stats.total_violations = g_safety_mechanisms.status.violation_count;
    
    // Update runtime
    g_safety_mechanisms.stats.total_runtime = hal_get_timestamp_us() - g_safety_mechanisms.start_time;
    
    return HAL_STATUS_OK;
}

static hal_status_t trigger_safety_response(safety_fault_t fault) {
    // TODO: Implement safety response actions
    // For now, just log the fault
    printf("Safety response triggered for fault: %s\n", safety_mechanisms_get_fault_name(fault));
    
    return HAL_STATUS_OK;
}

static float clamp_value(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
