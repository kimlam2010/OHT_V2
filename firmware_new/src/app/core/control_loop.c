/**
 * @file control_loop.c
 * @brief Control Loop Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-04 (Control Loop Implementation)
 */

#include "control_loop.h"
#include "hal_common.h"
#include "safety_monitor.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// TODO: Add motor module integration when linking issues are resolved

// Internal control loop structure
typedef struct {
    control_config_t config;
    control_status_t status;
    control_stats_t stats;
    bool initialized;
    bool enabled;
    
    // PID controllers
    struct {
        float setpoint;
        float input;
        float output;
        float error;
        float prev_error;
        float integral;
        float derivative;
        pid_params_t params;
    } velocity_pid;
    
    // Motion control
    float target_velocity;
    float current_velocity;
    float control_output;
    float commanded_velocity;
    
    // Timing
    uint64_t last_update_time;
    uint64_t start_time;
    
    // Safety
    bool limits_violated;
    bool safety_violated;
} control_loop_t;

// Global control loop instance
static control_loop_t g_control_loop = {0};

// Forward declarations
static hal_status_t update_pid_controller(bool is_position_pid, float setpoint, float input, float *output);
static hal_status_t check_limits(void);
static hal_status_t update_statistics(void);
static hal_status_t apply_control_output(float output);
static float clamp_value(float value, float min, float max);
static float limit_acceleration(float desired_velocity, float current_velocity, float max_accel, float dt);

// Control loop implementation
hal_status_t control_loop_init(const control_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!control_loop_validate_config(config)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (g_control_loop.initialized) {
        return HAL_STATUS_OK;
    }
    
    // Initialize control loop structure
    memset(&g_control_loop, 0, sizeof(control_loop_t));
    
    // Copy configuration
    memcpy(&g_control_loop.config, config, sizeof(control_config_t));
    
    // Initialize PID controllers
    memcpy(&g_control_loop.velocity_pid.params, &config->velocity_pid, sizeof(pid_params_t));
    
    // Initialize status
    g_control_loop.status.state = CONTROL_STATE_DISABLED;
    g_control_loop.status.mode = CONTROL_MODE_IDLE;
    g_control_loop.status.cycle_count = 0;
    g_control_loop.status.last_update_time = hal_get_timestamp_us();
    g_control_loop.start_time = hal_get_timestamp_us();
    
    // Initialize statistics
    g_control_loop.stats.total_cycles = 0;
    g_control_loop.stats.error_cycles = 0;
    g_control_loop.stats.max_position_error = 0.0f;
    g_control_loop.stats.max_velocity_error = 0.0f;
    g_control_loop.stats.avg_position_error = 0.0f;
    g_control_loop.stats.avg_velocity_error = 0.0f;
    g_control_loop.stats.total_runtime = 0;
    
    g_control_loop.initialized = true;
    g_control_loop.enabled = false;
    
    printf("Control loop initialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t control_loop_deinit(void) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_OK;
    }
    
    // Disable control loop
    control_loop_disable();
    
    // Clear control loop
    memset(&g_control_loop, 0, sizeof(control_loop_t));
    
    printf("Control loop deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t control_loop_update(void) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!g_control_loop.enabled) {
        return HAL_STATUS_OK;
    }
    
    uint64_t current_time = hal_get_timestamp_us();
    float dt = (current_time - g_control_loop.last_update_time) / 1000000.0f;
    
    if (dt < g_control_loop.config.sample_time) {
        return HAL_STATUS_OK; // Not time to update yet
    }
    
    // Update timing
    g_control_loop.last_update_time = current_time;
    g_control_loop.status.last_update_time = current_time;
    g_control_loop.status.cycle_count++;
    g_control_loop.stats.total_cycles++;
    
    // Get current velocity from real sensors (stub: set equal to last value until motor integration wired)
    // NOTE: Simulation removed per EXEC PLAN Gate A. Integration with motor module will update this.
    g_control_loop.current_velocity = g_control_loop.current_velocity;
    
    // Update status
    g_control_loop.status.current_velocity = g_control_loop.current_velocity;
    g_control_loop.status.target_velocity = g_control_loop.target_velocity;
    
    // Calculate errors
    g_control_loop.status.velocity_error = g_control_loop.target_velocity - g_control_loop.current_velocity;
    
    // Update PID controllers based on mode
    float velocity_output = 0.0f;
    
    switch (g_control_loop.status.mode) {
        case CONTROL_MODE_VELOCITY:
            // Velocity control: use velocity PID
            // Apply acceleration limiting prior to PID to respect profile
            g_control_loop.commanded_velocity = limit_acceleration(
                g_control_loop.target_velocity,
                g_control_loop.current_velocity,
                g_control_loop.config.profile.max_acceleration,
                dt);
            update_pid_controller(false, g_control_loop.commanded_velocity, g_control_loop.current_velocity, &velocity_output);
            g_control_loop.control_output = velocity_output;
            break;
            
        case CONTROL_MODE_EMERGENCY:
            // Emergency mode: stop all motion
            g_control_loop.control_output = 0.0f;
            break;
            
        case CONTROL_MODE_IDLE:
        default:
            // Idle mode: no control output
            g_control_loop.control_output = 0.0f;
            break;
    }
    
    // Apply limits (use velocity PID bounds)
    g_control_loop.control_output = clamp_value(
        g_control_loop.control_output,
        g_control_loop.velocity_pid.params.output_min,
        g_control_loop.velocity_pid.params.output_max);
    
    // Update status
    g_control_loop.status.control_output = g_control_loop.control_output;
    
    // Check limits and safety
    check_limits();

    // Safety integration: if E-Stop active, force emergency mode and zero output
    if (g_control_loop.config.enable_safety) {
        bool estop_active = false;
        if (safety_monitor_is_estop_active(&estop_active) == HAL_STATUS_OK && estop_active) {
            g_control_loop.status.mode = CONTROL_MODE_EMERGENCY;
            g_control_loop.status.state = CONTROL_STATE_ERROR;
            g_control_loop.control_output = 0.0f;
            apply_control_output(0.0f);
            return HAL_STATUS_OK;
        }
    }
    
    // Apply control output to actuators
    apply_control_output(g_control_loop.control_output);
    
    // Update statistics
    update_statistics();
    
    return HAL_STATUS_OK;
}

hal_status_t control_loop_set_mode(control_mode_t mode) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (mode >= CONTROL_MODE_EMERGENCY + 1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    g_control_loop.status.mode = mode;
    g_control_loop.status.state = CONTROL_STATE_ENABLED;
    
    // Reset PID controllers when changing modes
    g_control_loop.velocity_pid.integral = 0.0f;
    g_control_loop.velocity_pid.prev_error = 0.0f;
    
    printf("Control mode set to: %s\n", control_loop_get_mode_name(mode));
    return HAL_STATUS_OK;
}

hal_status_t control_loop_get_mode(control_mode_t *mode) {
    if (!g_control_loop.initialized || mode == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *mode = g_control_loop.status.mode;
    return HAL_STATUS_OK;
}

hal_status_t control_loop_enable(void) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_control_loop.enabled = true;
    g_control_loop.status.state = CONTROL_STATE_ENABLED;
    
    printf("Control loop enabled\n");
    return HAL_STATUS_OK;
}

hal_status_t control_loop_disable(void) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_OK;
    }
    
    g_control_loop.enabled = false;
    g_control_loop.status.state = CONTROL_STATE_DISABLED;
    g_control_loop.control_output = 0.0f;
    
    // Apply zero output
    apply_control_output(0.0f);
    
    printf("Control loop disabled\n");
    return HAL_STATUS_OK;
}

hal_status_t control_loop_is_enabled(bool *enabled) {
    if (!g_control_loop.initialized || enabled == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *enabled = g_control_loop.enabled;
    return HAL_STATUS_OK;
}

hal_status_t control_loop_set_target_position(float position) { (void)position; return HAL_STATUS_INVALID_STATE; }

hal_status_t control_loop_get_target_position(float *position) { (void)position; return HAL_STATUS_INVALID_STATE; }

hal_status_t control_loop_set_target_velocity(float velocity) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check velocity limits if enabled
    if (g_control_loop.config.enable_limits) {
        velocity = clamp_value(velocity, -g_control_loop.config.profile.max_velocity, g_control_loop.config.profile.max_velocity);
    }
    
    g_control_loop.target_velocity = velocity;
    g_control_loop.velocity_pid.setpoint = velocity;
    
    return HAL_STATUS_OK;
}

hal_status_t control_loop_get_target_velocity(float *velocity) {
    if (!g_control_loop.initialized || velocity == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *velocity = g_control_loop.target_velocity;
    return HAL_STATUS_OK;
}

hal_status_t control_loop_get_current_position(float *position) { (void)position; return HAL_STATUS_INVALID_STATE; }

hal_status_t control_loop_get_current_velocity(float *velocity) {
    if (!g_control_loop.initialized || velocity == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *velocity = g_control_loop.current_velocity;
    return HAL_STATUS_OK;
}

hal_status_t control_loop_set_pid_params(bool is_position_pid, const pid_params_t *params) {
    if (!g_control_loop.initialized || params == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!is_position_pid) {
        memcpy(&g_control_loop.velocity_pid.params, params, sizeof(pid_params_t));
        memcpy(&g_control_loop.config.velocity_pid, params, sizeof(pid_params_t));
    }
    
    return HAL_STATUS_OK;
}

hal_status_t control_loop_get_pid_params(bool is_position_pid, pid_params_t *params) {
    if (!g_control_loop.initialized || params == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!is_position_pid) {
        memcpy(params, &g_control_loop.velocity_pid.params, sizeof(pid_params_t));
    }
    
    return HAL_STATUS_OK;
}

hal_status_t control_loop_set_motion_profile(const motion_profile_t *profile) {
    if (!g_control_loop.initialized || profile == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_control_loop.config.profile, profile, sizeof(motion_profile_t));
    return HAL_STATUS_OK;
}

hal_status_t control_loop_get_motion_profile(motion_profile_t *profile) {
    if (!g_control_loop.initialized || profile == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(profile, &g_control_loop.config.profile, sizeof(motion_profile_t));
    return HAL_STATUS_OK;
}

hal_status_t control_loop_get_status(control_status_t *status) {
    if (!g_control_loop.initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(status, &g_control_loop.status, sizeof(control_status_t));
    return HAL_STATUS_OK;
}

hal_status_t control_loop_get_stats(control_stats_t *stats) {
    if (!g_control_loop.initialized || stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(stats, &g_control_loop.stats, sizeof(control_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t control_loop_reset_stats(void) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&g_control_loop.stats, 0, sizeof(control_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t control_loop_is_target_reached(bool *reached) {
    if (!g_control_loop.initialized || reached == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    float velocity_error = fabsf(g_control_loop.status.velocity_error);
    
    // Velocity-only criteria: target near zero and velocity error within tolerance
    *reached = (fabsf(g_control_loop.target_velocity) <= g_control_loop.config.profile.velocity_tolerance) &&
               (velocity_error <= g_control_loop.config.profile.velocity_tolerance);
    
    return HAL_STATUS_OK;
}

hal_status_t control_loop_emergency_stop(void) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_control_loop.status.state = CONTROL_STATE_ERROR;
    g_control_loop.status.mode = CONTROL_MODE_EMERGENCY;
    g_control_loop.control_output = 0.0f;
    
    // Apply zero output immediately
    apply_control_output(0.0f);
    
    printf("Control loop emergency stop\n");
    return HAL_STATUS_OK;
}

hal_status_t control_loop_clear_errors(void) {
    if (!g_control_loop.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_control_loop.status.state = CONTROL_STATE_ENABLED;
    g_control_loop.status.limits_violated = false;
    g_control_loop.status.safety_violated = false;
    
    return HAL_STATUS_OK;
}

hal_status_t control_loop_get_diagnostics(char *info, size_t max_len) {
    if (!g_control_loop.initialized || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(info, max_len,
        "Control Loop Diagnostics:\n"
        "State: %s\n"
        "Mode: %s\n"
        "Enabled: %s\n"
        "Target Velocity: %.2f mm/s\n"
        "Current Velocity: %.2f mm/s\n"
        "Velocity Error: %.2f mm/s\n"
        "Control Output: %.2f\n"
        "Cycle Count: %u\n"
        "Limits Violated: %s\n"
        "Safety Violated: %s\n",
        control_loop_get_state_name(g_control_loop.status.state),
        control_loop_get_mode_name(g_control_loop.status.mode),
        g_control_loop.enabled ? "YES" : "NO",
        g_control_loop.target_velocity,
        g_control_loop.current_velocity,
        g_control_loop.status.velocity_error,
        g_control_loop.control_output,
        g_control_loop.status.cycle_count,
        g_control_loop.status.limits_violated ? "YES" : "NO",
        g_control_loop.status.safety_violated ? "YES" : "NO"
    );
    
    return HAL_STATUS_OK;
}

// Utility functions
const char* control_loop_get_mode_name(control_mode_t mode) {
    switch (mode) {
        case CONTROL_MODE_IDLE: return "IDLE";
        case CONTROL_MODE_VELOCITY: return "VELOCITY";
        case CONTROL_MODE_EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

const char* control_loop_get_state_name(control_state_t state) {
    switch (state) {
        case CONTROL_STATE_DISABLED: return "DISABLED";
        case CONTROL_STATE_ENABLED: return "ENABLED";
        case CONTROL_STATE_RUNNING: return "RUNNING";
        case CONTROL_STATE_ERROR: return "ERROR";
        case CONTROL_STATE_FAULT: return "FAULT";
        default: return "UNKNOWN";
    }
}

bool control_loop_validate_config(const control_config_t *config) {
    if (config == NULL) {
        printf("Config validation failed: NULL config\n");
        return false;
    }
    
    // Validate control frequency
    if (config->control_frequency <= 0.0f || config->control_frequency > 10000.0f) {
        printf("Config validation failed: control_frequency=%f (valid: 0-10000)\n", config->control_frequency);
        return false;
    }
    
    // Validate sample time
    if (config->sample_time <= 0.0f || config->sample_time > 1.0f) {
        printf("Config validation failed: sample_time=%f (valid: 0-1)\n", config->sample_time);
        return false;
    }
    
    // Validate motion profile
    if (config->profile.max_velocity <= 0.0f) {
        printf("Config validation failed: max_velocity=%f (must be > 0)\n", config->profile.max_velocity);
        return false;
    }
    if (config->profile.max_acceleration <= 0.0f) {
        printf("Config validation failed: max_acceleration=%f (must be > 0)\n", config->profile.max_acceleration);
        return false;
    }
    if (config->profile.max_jerk <= 0.0f) {
        printf("Config validation failed: max_jerk=%f (must be > 0)\n", config->profile.max_jerk);
        return false;
    }
    
    printf("Config validation passed\n");
    return true;
}

// Internal helper functions
static hal_status_t update_pid_controller(bool is_position_pid, float setpoint, float input, float *output) {
    if (output == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    struct {
        float setpoint;
        float input;
        float output;
        float error;
        float prev_error;
        float integral;
        float derivative;
        pid_params_t params;
    } *pid = /* position PID removed */ &g_control_loop.velocity_pid;
    
    // Calculate error
    pid->setpoint = setpoint;
    pid->input = input;
    pid->error = setpoint - input;
    
    // Calculate integral term
    pid->integral += pid->error * g_control_loop.config.sample_time;
    pid->integral = clamp_value(pid->integral, pid->params.integral_min, pid->params.integral_max);
    
    // Calculate derivative term
    pid->derivative = (pid->error - pid->prev_error) / g_control_loop.config.sample_time;
    
    // Calculate PID output
    pid->output = pid->params.kp * pid->error + 
                  pid->params.ki * pid->integral + 
                  pid->params.kd * pid->derivative;
    
    // Apply output limits
    pid->output = clamp_value(pid->output, pid->params.output_min, pid->params.output_max);
    
    // Store error for next iteration
    pid->prev_error = pid->error;
    
    *output = pid->output;
    return HAL_STATUS_OK;
}

static hal_status_t check_limits(void) {
    bool limits_violated = false;
    bool safety_violated = false;
    
    // Position limits removed in velocity-only mode
    
    // Check velocity limits
    if (g_control_loop.config.enable_limits) {
        if (fabsf(g_control_loop.current_velocity) > g_control_loop.config.profile.max_velocity) {
            limits_violated = true;
        }
    }
    
    // Check safety limits (if safety monitoring enabled)
    if (g_control_loop.config.enable_safety) {
        // TODO: Integrate with safety mechanisms
        // For now, no safety violations
    }
    
    g_control_loop.status.limits_violated = limits_violated;
    g_control_loop.status.safety_violated = safety_violated;
    
    if (limits_violated || safety_violated) {
        g_control_loop.status.state = CONTROL_STATE_ERROR;
        g_control_loop.stats.error_cycles++;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t update_statistics(void) {
    // Update maximum errors (position error removed)
    float abs_velocity_error = fabsf(g_control_loop.status.velocity_error);
    
    if (abs_velocity_error > g_control_loop.stats.max_velocity_error) {
        g_control_loop.stats.max_velocity_error = abs_velocity_error;
    }
    
    // Update average errors (simple moving average)
    g_control_loop.stats.avg_velocity_error = (g_control_loop.stats.avg_velocity_error * 0.9f) + (abs_velocity_error * 0.1f);
    
    // Update runtime
    g_control_loop.stats.total_runtime = hal_get_timestamp_us() - g_control_loop.start_time;
    
    return HAL_STATUS_OK;
}

static hal_status_t apply_control_output(float output) {
    // Apply control output to actual actuators (motors, etc.)
    g_control_loop.control_output = output;
    
    // TODO: Apply control output to actual motor module handlers
    // For now, simulate actuator response with realistic behavior
    static float last_output = 0.0f;
    static uint32_t output_count = 0;
    
    // Simulate actuator response delay and saturation
    float actuator_output = output;
    if (actuator_output > 1.0f) actuator_output = 1.0f;
    if (actuator_output < -1.0f) actuator_output = -1.0f;
    
    // Simulate actuator dynamics (rate limiting)
    float max_rate = 0.1f; // Maximum change per cycle
    if (actuator_output - last_output > max_rate) {
        actuator_output = last_output + max_rate;
    } else if (actuator_output - last_output < -max_rate) {
        actuator_output = last_output - max_rate;
    }
    last_output = actuator_output;
    
    // Log significant control outputs
    if (output_count % 100 == 0) { // Log every 100 cycles
        printf("[CONTROL] Output: %.3f -> Actuator: %.3f\n", output, actuator_output);
    }
    output_count++;
    
    return HAL_STATUS_OK;
}

static float clamp_value(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static float limit_acceleration(float desired_velocity, float current_velocity, float max_accel, float dt) {
    float dv = desired_velocity - current_velocity;
    float max_dv = max_accel * dt;
    if (dv > max_dv) dv = max_dv;
    if (dv < -max_dv) dv = -max_dv;
    return current_velocity + dv;
}
