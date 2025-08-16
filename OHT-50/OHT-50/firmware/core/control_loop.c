/**
 * @file control_loop.c
 * @brief Implementation của control loop với timing deterministic cho OHT-50
 * @version 1.0
 * @date 2024-12-19
 */

#include "control_loop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// Internal constants
#define CONTROL_LOOP_TASK_NAME "ControlLoop"
#define MAX_CYCLE_TIME_US (CONTROL_LOOP_PERIOD_MS * 1000)

// Internal function prototypes
static void control_loop_measure_latency(control_loop_context_t* ctx);
static void control_loop_update_stats(control_loop_context_t* ctx, uint64_t execution_time_us);
static uint32_t control_loop_calculate_average_latency(const control_loop_context_t* ctx);

/**
 * Initialize control loop
 */
int control_loop_init(control_loop_context_t* ctx) {
    if (!ctx) {
        return -1;
    }

    memset(ctx, 0, sizeof(control_loop_context_t));
    
    ctx->state = CONTROL_LOOP_IDLE;
    ctx->cycle_count = 0;
    ctx->last_cycle_time = 0;
    ctx->latency_buffer_index = 0;
    ctx->latency_measurement_enabled = true;
    
    // Initialize statistics
    ctx->stats.total_cycles = 0;
    ctx->stats.missed_deadlines = 0;
    ctx->stats.max_latency_us = 0;
    ctx->stats.min_latency_us = UINT32_MAX;
    ctx->stats.avg_latency_us = 0;
    ctx->stats.max_jitter_us = 0;
    ctx->stats.last_cycle_time_us = 0;
    ctx->stats.total_execution_time_us = 0;

    return 0;
}

/**
 * Deinitialize control loop
 */
void control_loop_deinit(control_loop_context_t* ctx) {
    if (!ctx) return;
    
    control_loop_stop(ctx);
    memset(ctx, 0, sizeof(control_loop_context_t));
}

/**
 * Set control function
 */
int control_loop_set_control_function(control_loop_context_t* ctx, 
                                    void (*function)(void*), void* arg) {
    if (!ctx) return -1;
    
    ctx->control_function = function;
    ctx->control_arg = arg;
    return 0;
}

/**
 * Set safety check function
 */
int control_loop_set_safety_function(control_loop_context_t* ctx, 
                                   void (*function)(void*), void* arg) {
    if (!ctx) return -1;
    
    ctx->safety_check = function;
    ctx->safety_arg = arg;
    return 0;
}

/**
 * Set telemetry function
 */
int control_loop_set_telemetry_function(control_loop_context_t* ctx, 
                                      void (*function)(void*), void* arg) {
    if (!ctx) return -1;
    
    ctx->telemetry_function = function;
    ctx->telemetry_arg = arg;
    return 0;
}

/**
 * Start control loop in scheduler
 */
int control_loop_start(scheduler_context_t* scheduler, control_loop_context_t* ctx) {
    if (!scheduler || !ctx || ctx->state == CONTROL_LOOP_RUNNING) {
        return -1;
    }

    // Add control loop task to scheduler with highest priority
    int task_id = scheduler_add_task(scheduler,
                                   CONTROL_LOOP_TASK_NAME,
                                   control_loop_task,
                                   ctx,
                                   PRIORITY_HIGH,
                                   CONTROL_LOOP_PERIOD_MS,
                                   CONTROL_LOOP_DEADLINE_MS);

    if (task_id < 0) {
        return -1;
    }

    ctx->state = CONTROL_LOOP_RUNNING;
    ctx->last_cycle_time = scheduler_get_time_us();
    
    return 0;
}

/**
 * Stop control loop
 */
int control_loop_stop(control_loop_context_t* ctx) {
    if (!ctx) return -1;
    
    ctx->state = CONTROL_LOOP_IDLE;
    return 0;
}

/**
 * Main control loop task function (called by scheduler)
 */
void control_loop_task(void* arg) {
    control_loop_context_t* ctx = (control_loop_context_t*)arg;
    if (!ctx) return;

    uint64_t cycle_start_time = scheduler_get_time_us();
    
    // Check if we're in a valid state
    if (ctx->state != CONTROL_LOOP_RUNNING) {
        return;
    }

    // Measure latency if enabled
    if (ctx->latency_measurement_enabled) {
        control_loop_measure_latency(ctx);
    }

    // Execute safety check first (highest priority)
    if (ctx->safety_check) {
        ctx->safety_check(ctx->safety_arg);
    }

    // Execute main control function
    if (ctx->control_function) {
        ctx->control_function(ctx->control_arg);
    }

    // Execute telemetry function
    if (ctx->telemetry_function) {
        ctx->telemetry_function(ctx->telemetry_arg);
    }

    // Update cycle statistics
    uint64_t cycle_end_time = scheduler_get_time_us();
    uint64_t execution_time_us = cycle_end_time - cycle_start_time;
    
    control_loop_update_stats(ctx, execution_time_us);
    
    // Check deadline
    if (execution_time_us > MAX_CYCLE_TIME_US) {
        ctx->stats.missed_deadlines++;
        printf("WARNING: Control loop missed deadline: %lu us\n", execution_time_us);
    }

    ctx->cycle_count++;
    ctx->last_cycle_time = cycle_start_time;
}

/**
 * Get control loop state
 */
control_loop_state_t control_loop_get_state(const control_loop_context_t* ctx) {
    return ctx ? ctx->state : CONTROL_LOOP_IDLE;
}

/**
 * Set control loop state
 */
int control_loop_set_state(control_loop_context_t* ctx, control_loop_state_t state) {
    if (!ctx) return -1;
    
    ctx->state = state;
    return 0;
}

/**
 * Enable/disable latency measurement
 */
void control_loop_enable_latency_measurement(control_loop_context_t* ctx, bool enable) {
    if (ctx) {
        ctx->latency_measurement_enabled = enable;
    }
}

/**
 * Get latency statistics
 */
int control_loop_get_latency_stats(const control_loop_context_t* ctx, 
                                  uint32_t* max_latency, uint32_t* avg_latency, 
                                  uint32_t* max_jitter) {
    if (!ctx || !max_latency || !avg_latency || !max_jitter) {
        return -1;
    }

    *max_latency = ctx->stats.max_latency_us;
    *avg_latency = ctx->stats.avg_latency_us;
    *max_jitter = ctx->stats.max_jitter_us;
    
    return 0;
}

/**
 * Reset latency statistics
 */
void control_loop_reset_latency_stats(control_loop_context_t* ctx) {
    if (!ctx) return;
    
    memset(&ctx->stats, 0, sizeof(control_loop_stats_t));
    ctx->stats.min_latency_us = UINT32_MAX;
    ctx->latency_buffer_index = 0;
    memset(ctx->latency_buffer, 0, sizeof(ctx->latency_buffer));
}

/**
 * Get control loop statistics
 */
void control_loop_get_stats(const control_loop_context_t* ctx, control_loop_stats_t* stats) {
    if (ctx && stats) {
        memcpy(stats, &ctx->stats, sizeof(control_loop_stats_t));
    }
}

/**
 * Print control loop statistics
 */
void control_loop_print_stats(const control_loop_context_t* ctx) {
    if (!ctx) return;
    
    printf("=== Control Loop Statistics ===\n");
    printf("State: %d\n", ctx->state);
    printf("Total cycles: %u\n", ctx->stats.total_cycles);
    printf("Missed deadlines: %u\n", ctx->stats.missed_deadlines);
    printf("Max latency: %u us\n", ctx->stats.max_latency_us);
    printf("Min latency: %u us\n", ctx->stats.min_latency_us);
    printf("Avg latency: %u us\n", ctx->stats.avg_latency_us);
    printf("Max jitter: %u us\n", ctx->stats.max_jitter_us);
    printf("Last cycle time: %lu us\n", ctx->stats.last_cycle_time_us);
    printf("Total execution time: %lu us\n", ctx->stats.total_execution_time_us);
    printf("==============================\n");
}

/**
 * Get cycle time in microseconds
 */
uint64_t control_loop_get_cycle_time_us(const control_loop_context_t* ctx) {
    return ctx ? ctx->stats.last_cycle_time_us : 0;
}

/**
 * Check if deadline is met
 */
bool control_loop_is_deadline_met(const control_loop_context_t* ctx) {
    if (!ctx) return false;
    return ctx->stats.last_cycle_time_us <= MAX_CYCLE_TIME_US;
}

/**
 * Measure latency for current cycle
 */
static void control_loop_measure_latency(control_loop_context_t* ctx) {
    uint64_t current_time = scheduler_get_time_us();
    
    if (ctx->latency_buffer_index < LATENCY_BUFFER_SIZE) {
        latency_measurement_t* measurement = &ctx->latency_buffer[ctx->latency_buffer_index];
        
        measurement->start_time_us = current_time;
        measurement->end_time_us = current_time; // Will be updated at end of cycle
        measurement->latency_us = 0; // Will be calculated at end of cycle
        measurement->jitter_us = 0; // Will be calculated at end of cycle
        
        ctx->latency_buffer_index++;
    } else {
        // Circular buffer - wrap around
        ctx->latency_buffer_index = 0;
        latency_measurement_t* measurement = &ctx->latency_buffer[0];
        
        measurement->start_time_us = current_time;
        measurement->end_time_us = current_time;
        measurement->latency_us = 0;
        measurement->jitter_us = 0;
        
        ctx->latency_buffer_index = 1;
    }
}

/**
 * Update control loop statistics
 */
static void control_loop_update_stats(control_loop_context_t* ctx, uint64_t execution_time_us) {
    ctx->stats.total_cycles++;
    ctx->stats.last_cycle_time_us = execution_time_us;
    ctx->stats.total_execution_time_us += execution_time_us;
    
    // Update latency statistics
    if (execution_time_us > ctx->stats.max_latency_us) {
        ctx->stats.max_latency_us = execution_time_us;
    }
    
    if (execution_time_us < ctx->stats.min_latency_us) {
        ctx->stats.min_latency_us = execution_time_us;
    }
    
    // Calculate average latency
    ctx->stats.avg_latency_us = control_loop_calculate_average_latency(ctx);
    
    // Calculate jitter (difference from average)
    uint32_t jitter = abs((int32_t)execution_time_us - (int32_t)ctx->stats.avg_latency_us);
    if (jitter > ctx->stats.max_jitter_us) {
        ctx->stats.max_jitter_us = jitter;
    }
}

/**
 * Calculate average latency from buffer
 */
static uint32_t control_loop_calculate_average_latency(const control_loop_context_t* ctx) {
    if (ctx->latency_buffer_index == 0) {
        return 0;
    }
    
    uint64_t total_latency = 0;
    uint32_t valid_measurements = 0;
    
    for (uint32_t i = 0; i < ctx->latency_buffer_index; i++) {
        if (ctx->latency_buffer[i].latency_us > 0) {
            total_latency += ctx->latency_buffer[i].latency_us;
            valid_measurements++;
        }
    }
    
    return valid_measurements > 0 ? (uint32_t)(total_latency / valid_measurements) : 0;
}
