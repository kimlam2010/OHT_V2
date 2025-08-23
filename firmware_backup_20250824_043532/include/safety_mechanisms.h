/**
 * @file safety_mechanisms.h
 * @brief Safety Mechanisms Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-05 (Safety Mechanisms Implementation)
 */

#ifndef SAFETY_MECHANISMS_H
#define SAFETY_MECHANISMS_H

#include "hal_common.h"
#include "system_state_machine.h"
#include <stdint.h>
#include <stdbool.h>

// Safety levels
typedef enum {
    SAFETY_LEVEL_NONE = 0,      // No safety
    SAFETY_LEVEL_SIL1,          // SIL1 safety level
    SAFETY_LEVEL_SIL2,          // SIL2 safety level
    SAFETY_LEVEL_SIL3,          // SIL3 safety level
    SAFETY_LEVEL_SIL4           // SIL4 safety level
} safety_level_t;

// Safety states
typedef enum {
    SAFETY_STATE_SAFE = 0,      // System safe
    SAFETY_STATE_WARNING,       // Safety warning
    SAFETY_STATE_VIOLATION,     // Safety violation
    SAFETY_STATE_EMERGENCY,     // Emergency stop
    SAFETY_STATE_FAULT          // Safety fault
} safety_state_t;

// Safety fault types
typedef enum {
    SAFETY_FAULT_NONE = 0,      // No fault
    SAFETY_FAULT_ESTOP,         // E-Stop fault
    SAFETY_FAULT_COLLISION,     // Collision detected
    SAFETY_FAULT_SPEED_LIMIT,   // Speed limit exceeded
    SAFETY_FAULT_POSITION_LIMIT, // Position limit exceeded
    SAFETY_FAULT_SENSOR,        // Sensor fault
    SAFETY_FAULT_COMMUNICATION, // Communication fault
    SAFETY_FAULT_SOFTWARE,      // Software fault
    SAFETY_FAULT_HARDWARE       // Hardware fault
} safety_fault_t;

// Safety zone definition
typedef struct {
    float min_position;         // Minimum safe position (mm)
    float max_position;         // Maximum safe position (mm)
    float min_velocity;         // Minimum safe velocity (mm/s)
    float max_velocity;         // Maximum safe velocity (mm/s)
    float min_acceleration;     // Minimum safe acceleration (mm/s²)
    float max_acceleration;     // Maximum safe acceleration (mm/s²)
    bool enabled;               // Zone enabled
} safety_zone_t;

// Collision detection parameters
typedef struct {
    float detection_distance;   // Collision detection distance (mm)
    float reaction_time;        // Reaction time (ms)
    bool enabled;               // Collision detection enabled
    bool lidar_based;           // Use LiDAR for detection
    bool sensor_based;          // Use sensors for detection
} collision_detection_t;

// Speed monitoring parameters
typedef struct {
    float max_speed;            // Maximum allowed speed (mm/s)
    float warning_speed;        // Warning speed threshold (mm/s)
    float emergency_speed;      // Emergency speed threshold (mm/s)
    bool enabled;               // Speed monitoring enabled
    bool real_time;             // Real-time monitoring
} speed_monitoring_t;

// Position monitoring parameters
typedef struct {
    float min_position;         // Minimum allowed position (mm)
    float max_position;         // Maximum allowed position (mm)
    float soft_limit_margin;    // Soft limit margin (mm)
    float hard_limit_margin;    // Hard limit margin (mm)
    bool enabled;               // Position monitoring enabled
} position_monitoring_t;

// Safety configuration
typedef struct {
    safety_level_t level;       // Safety level
    safety_zone_t zone;         // Safety zone
    collision_detection_t collision; // Collision detection
    speed_monitoring_t speed;   // Speed monitoring
    position_monitoring_t position; // Position monitoring
    bool enable_estop;          // Enable E-Stop monitoring
    bool enable_dual_channel;   // Enable dual-channel safety
    uint32_t update_frequency;  // Safety update frequency (Hz)
} safety_config_t;

// Safety status
typedef struct {
    safety_state_t state;       // Current safety state
    safety_fault_t fault;       // Current safety fault
    bool estop_active;          // E-Stop active
    bool collision_detected;    // Collision detected
    bool speed_limit_exceeded;  // Speed limit exceeded
    bool position_limit_exceeded; // Position limit exceeded
    bool safety_zone_violated;  // Safety zone violated
    float current_speed;        // Current speed (mm/s)
    float current_position;     // Current position (mm)
    uint64_t last_update_time;  // Last update time (us)
    uint32_t violation_count;   // Safety violation count
} safety_status_t;

// Safety statistics
typedef struct {
    uint32_t total_violations;  // Total safety violations
    uint32_t estop_triggers;    // E-Stop trigger count
    uint32_t collision_events;  // Collision event count
    uint32_t speed_violations;  // Speed violation count
    uint32_t position_violations; // Position violation count
    uint64_t total_runtime;     // Total runtime (us)
    float max_speed_recorded;   // Maximum speed recorded
    float min_position_recorded; // Minimum position recorded
    float max_position_recorded; // Maximum position recorded
} safety_stats_t;

// Function Prototypes

/**
 * @brief Initialize safety mechanisms
 * @param config Safety configuration
 * @return HAL status
 */
hal_status_t safety_mechanisms_init(const safety_config_t *config);

/**
 * @brief Deinitialize safety mechanisms
 * @return HAL status
 */
hal_status_t safety_mechanisms_deinit(void);

/**
 * @brief Update safety mechanisms (call periodically)
 * @return HAL status
 */
hal_status_t safety_mechanisms_update(void);

/**
 * @brief Get safety status
 * @param status Pointer to store safety status
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_status(safety_status_t *status);

/**
 * @brief Get safety statistics
 * @param stats Pointer to store safety statistics
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_stats(safety_stats_t *stats);

/**
 * @brief Reset safety statistics
 * @return HAL status
 */
hal_status_t safety_mechanisms_reset_stats(void);

/**
 * @brief Check if system is safe
 * @param safe Pointer to store safe status
 * @return HAL status
 */
hal_status_t safety_mechanisms_is_safe(bool *safe);

/**
 * @brief Check if E-Stop is active
 * @param active Pointer to store E-Stop status
 * @return HAL status
 */
hal_status_t safety_mechanisms_is_estop_active(bool *active);

/**
 * @brief Check if collision is detected
 * @param detected Pointer to store collision status
 * @return HAL status
 */
hal_status_t safety_mechanisms_is_collision_detected(bool *detected);

/**
 * @brief Check if speed limit is exceeded
 * @param exceeded Pointer to store speed limit status
 * @return HAL status
 */
hal_status_t safety_mechanisms_is_speed_limit_exceeded(bool *exceeded);

/**
 * @brief Check if position limit is exceeded
 * @param exceeded Pointer to store position limit status
 * @return HAL status
 */
hal_status_t safety_mechanisms_is_position_limit_exceeded(bool *exceeded);

/**
 * @brief Check if safety zone is violated
 * @param violated Pointer to store safety zone status
 * @return HAL status
 */
hal_status_t safety_mechanisms_is_safety_zone_violated(bool *violated);

/**
 * @brief Set safety zone
 * @param zone Safety zone parameters
 * @return HAL status
 */
hal_status_t safety_mechanisms_set_safety_zone(const safety_zone_t *zone);

/**
 * @brief Get safety zone
 * @param zone Pointer to store safety zone
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_safety_zone(safety_zone_t *zone);

/**
 * @brief Set collision detection parameters
 * @param collision Collision detection parameters
 * @return HAL status
 */
hal_status_t safety_mechanisms_set_collision_detection(const collision_detection_t *collision);

/**
 * @brief Get collision detection parameters
 * @param collision Pointer to store collision detection parameters
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_collision_detection(collision_detection_t *collision);

/**
 * @brief Set speed monitoring parameters
 * @param speed Speed monitoring parameters
 * @return HAL status
 */
hal_status_t safety_mechanisms_set_speed_monitoring(const speed_monitoring_t *speed);

/**
 * @brief Get speed monitoring parameters
 * @param speed Pointer to store speed monitoring parameters
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_speed_monitoring(speed_monitoring_t *speed);

/**
 * @brief Set position monitoring parameters
 * @param position Position monitoring parameters
 * @return HAL status
 */
hal_status_t safety_mechanisms_set_position_monitoring(const position_monitoring_t *position);

/**
 * @brief Get position monitoring parameters
 * @param position Pointer to store position monitoring parameters
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_position_monitoring(position_monitoring_t *position);

/**
 * @brief Trigger emergency stop
 * @return HAL status
 */
hal_status_t safety_mechanisms_trigger_emergency_stop(void);

/**
 * @brief Reset emergency stop
 * @return HAL status
 */
hal_status_t safety_mechanisms_reset_emergency_stop(void);

/**
 * @brief Clear safety faults
 * @return HAL status
 */
hal_status_t safety_mechanisms_clear_faults(void);

/**
 * @brief Get safety fault
 * @param fault Pointer to store safety fault
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_fault(safety_fault_t *fault);

/**
 * @brief Set safety level
 * @param level Safety level
 * @return HAL status
 */
hal_status_t safety_mechanisms_set_safety_level(safety_level_t level);

/**
 * @brief Get safety level
 * @param level Pointer to store safety level
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_safety_level(safety_level_t *level);

/**
 * @brief Validate safety configuration
 * @param config Safety configuration
 * @return true if valid
 */
bool safety_mechanisms_validate_config(const safety_config_t *config);

/**
 * @brief Get safety diagnostic information
 * @param info Pointer to store diagnostic info
 * @param max_len Maximum length of info string
 * @return HAL status
 */
hal_status_t safety_mechanisms_get_diagnostics(char *info, size_t max_len);

// Utility Functions

/**
 * @brief Get safety level name as string
 * @param level Safety level
 * @return Level name string
 */
const char* safety_mechanisms_get_level_name(safety_level_t level);

/**
 * @brief Get safety state name as string
 * @param state Safety state
 * @return State name string
 */
const char* safety_mechanisms_get_state_name(safety_state_t state);

/**
 * @brief Get safety fault name as string
 * @param fault Safety fault
 * @return Fault name string
 */
const char* safety_mechanisms_get_fault_name(safety_fault_t fault);

#endif // SAFETY_MECHANISMS_H
