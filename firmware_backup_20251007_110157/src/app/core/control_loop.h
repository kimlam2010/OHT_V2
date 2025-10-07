/**
 * @file control_loop.h
 * @brief Control Loop Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-04 (Control Loop Implementation)
 */

#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include "hal_common.h"
#include "system_state_machine.h"
#include <stdint.h>
#include <stdbool.h>

// Control modes
typedef enum {
    CONTROL_MODE_IDLE = 0,      // No control
    CONTROL_MODE_VELOCITY,      // Velocity control
    CONTROL_MODE_EMERGENCY      // Emergency stop
} control_mode_t;

// Control states
typedef enum {
    CONTROL_STATE_DISABLED = 0, // Control disabled
    CONTROL_STATE_ENABLED,      // Control enabled
    CONTROL_STATE_RUNNING,      // Control running
    CONTROL_STATE_ERROR,        // Control error
    CONTROL_STATE_FAULT         // Control fault
} control_state_t;

// PID parameters
typedef struct {
    float kp;                   // Proportional gain
    float ki;                   // Integral gain
    float kd;                   // Derivative gain
    float output_min;           // Minimum output
    float output_max;           // Maximum output
    float integral_min;         // Minimum integral
    float integral_max;         // Maximum integral
} pid_params_t;

// Motion profile parameters
typedef struct {
    float max_velocity;         // Maximum velocity (mm/s)
    float max_acceleration;     // Maximum acceleration (mm/s²)
    float max_jerk;            // Maximum jerk (mm/s³)
    float position_tolerance;   // Position tolerance (mm)
    float velocity_tolerance;   // Velocity tolerance (mm/s)
} motion_profile_t;

// Control loop configuration
typedef struct {
    float control_frequency;    // Control loop frequency (Hz)
    float sample_time;          // Sample time (s)
    pid_params_t velocity_pid;  // Velocity PID parameters
    motion_profile_t profile;   // Motion profile
    bool enable_limits;         // Enable position/velocity limits
    bool enable_safety;         // Enable safety monitoring
    float position_min_mm;      // Minimum allowed position (mm)
    float position_max_mm;      // Maximum allowed position (mm)
} control_config_t;

// Control loop status
typedef struct {
    control_state_t state;      // Current control state
    control_mode_t mode;        // Current control mode
    float target_velocity;      // Target velocity (mm/s)
    float current_velocity;     // Current velocity (mm/s)
    float control_output;       // Control output
    float velocity_error;       // Velocity error (mm/s)
    uint32_t cycle_count;       // Control cycle count
    uint64_t last_update_time;  // Last update time (us)
    bool limits_violated;       // Position/velocity limits violated
    bool safety_violated;       // Safety limits violated
} control_status_t;

// Control loop statistics
typedef struct {
    uint32_t total_cycles;      // Total control cycles
    uint32_t error_cycles;      // Error cycles
    float max_position_error;   // Maximum position error
    float max_velocity_error;   // Maximum velocity error
    float avg_position_error;   // Average position error
    float avg_velocity_error;   // Average velocity error
    uint64_t total_runtime;     // Total runtime (us)
} control_stats_t;

// Function Prototypes

/**
 * @brief Initialize control loop
 * @param config Control configuration
 * @return HAL status
 */
hal_status_t control_loop_init(const control_config_t *config);

/**
 * @brief Deinitialize control loop
 * @return HAL status
 */
hal_status_t control_loop_deinit(void);

/**
 * @brief Update control loop (call periodically)
 * @return HAL status
 */
hal_status_t control_loop_update(void);

/**
 * @brief Set control mode
 * @param mode Control mode
 * @return HAL status
 */
hal_status_t control_loop_set_mode(control_mode_t mode);

/**
 * @brief Get control mode
 * @param mode Pointer to store control mode
 * @return HAL status
 */
hal_status_t control_loop_get_mode(control_mode_t *mode);

/**
 * @brief Enable control loop
 * @return HAL status
 */
hal_status_t control_loop_enable(void);

/**
 * @brief Disable control loop
 * @return HAL status
 */
hal_status_t control_loop_disable(void);

/**
 * @brief Check if control loop is enabled
 * @param enabled Pointer to store enabled status
 * @return HAL status
 */
hal_status_t control_loop_is_enabled(bool *enabled);

/**
 * @brief Set target position
 * @param position Target position (mm)
 * @return HAL status
 */
hal_status_t control_loop_set_target_position(float position);

/**
 * @brief Get target position
 * @param position Pointer to store target position
 * @return HAL status
 */
hal_status_t control_loop_get_target_position(float *position);

/**
 * @brief Set target velocity
 * @param velocity Target velocity (mm/s)
 * @return HAL status
 */
hal_status_t control_loop_set_target_velocity(float velocity);

/**
 * @brief Get target velocity
 * @param velocity Pointer to store target velocity
 * @return HAL status
 */
hal_status_t control_loop_get_target_velocity(float *velocity);

/**
 * @brief Get current position
 * @param position Pointer to store current position
 * @return HAL status
 */
hal_status_t control_loop_get_current_position(float *position);

/**
 * @brief Get current velocity
 * @param velocity Pointer to store current velocity
 * @return HAL status
 */
hal_status_t control_loop_get_current_velocity(float *velocity);

/**
 * @brief Set PID parameters
 * @param pid_type PID type (position/velocity)
 * @param params PID parameters
 * @return HAL status
 */
hal_status_t control_loop_set_pid_params(bool is_position_pid, const pid_params_t *params);

/**
 * @brief Get PID parameters
 * @param pid_type PID type (position/velocity)
 * @param params Pointer to store PID parameters
 * @return HAL status
 */
hal_status_t control_loop_get_pid_params(bool is_position_pid, pid_params_t *params);

/**
 * @brief Set motion profile
 * @param profile Motion profile parameters
 * @return HAL status
 */
hal_status_t control_loop_set_motion_profile(const motion_profile_t *profile);

/**
 * @brief Get motion profile
 * @param profile Pointer to store motion profile
 * @return HAL status
 */
hal_status_t control_loop_get_motion_profile(motion_profile_t *profile);

/**
 * @brief Get control status
 * @param status Pointer to store control status
 * @return HAL status
 */
hal_status_t control_loop_get_status(control_status_t *status);

/**
 * @brief Get control statistics
 * @param stats Pointer to store control statistics
 * @return HAL status
 */
hal_status_t control_loop_get_stats(control_stats_t *stats);

/**
 * @brief Reset control statistics
 * @return HAL status
 */
hal_status_t control_loop_reset_stats(void);

/**
 * @brief Check if target reached
 * @param reached Pointer to store reached status
 * @return HAL status
 */
hal_status_t control_loop_is_target_reached(bool *reached);

/**
 * @brief Emergency stop
 * @return HAL status
 */
hal_status_t control_loop_emergency_stop(void);

/**
 * @brief Clear control errors
 * @return HAL status
 */
hal_status_t control_loop_clear_errors(void);

/**
 * @brief Get control diagnostic information
 * @param info Pointer to store diagnostic info
 * @param max_len Maximum length of info string
 * @return HAL status
 */
hal_status_t control_loop_get_diagnostics(char *info, size_t max_len);

// Utility Functions

/**
 * @brief Get control mode name as string
 * @param mode Control mode
 * @return Mode name string
 */
const char* control_loop_get_mode_name(control_mode_t mode);

/**
 * @brief Get control state name as string
 * @param state Control state
 * @return State name string
 */
const char* control_loop_get_state_name(control_state_t state);

/**
 * @brief Validate control configuration
 * @param config Control configuration
 * @return true if valid
 */
bool control_loop_validate_config(const control_config_t *config);

#endif // CONTROL_LOOP_H
