/**
 * @file control_loop.h
 * @brief Control loop với timing deterministic cho OHT-50
 * @version 1.0
 * @date 2024-12-19
 */

#ifndef OHT_CONTROL_LOOP_H
#define OHT_CONTROL_LOOP_H

#include <stdint.h>
#include <stdbool.h>
#include "scheduler.h"

#ifdef __cplusplus
extern "C" {
#endif

// Control loop configuration
#define CONTROL_LOOP_PERIOD_MS 1
#define CONTROL_LOOP_DEADLINE_MS 1
#define MAX_LATENCY_US 100
#define LATENCY_BUFFER_SIZE 1000

// Control loop states
typedef enum {
    CONTROL_LOOP_IDLE = 0,
    CONTROL_LOOP_RUNNING = 1,
    CONTROL_LOOP_FAULT = 2,
    CONTROL_LOOP_ESTOP = 3
} control_loop_state_t;

// Latency measurement structure
typedef struct {
    uint64_t start_time_us;
    uint64_t end_time_us;
    uint64_t latency_us;
    uint64_t jitter_us;
} latency_measurement_t;

// Control loop statistics
typedef struct {
    uint32_t total_cycles;
    uint32_t missed_deadlines;
    uint32_t max_latency_us;
    uint32_t min_latency_us;
    uint32_t avg_latency_us;
    uint32_t max_jitter_us;
    uint64_t last_cycle_time_us;
    uint64_t total_execution_time_us;
} control_loop_stats_t;

// Control loop context
typedef struct {
    control_loop_state_t state;
    uint64_t cycle_count;
    uint64_t last_cycle_time;
    control_loop_stats_t stats;
    latency_measurement_t latency_buffer[LATENCY_BUFFER_SIZE];
    uint32_t latency_buffer_index;
    bool latency_measurement_enabled;
    
    // Callback functions
    void (*control_function)(void*);
    void* control_arg;
    void (*safety_check)(void*);
    void* safety_arg;
    void (*telemetry_function)(void*);
    void* telemetry_arg;
} control_loop_context_t;

// Function prototypes
int control_loop_init(control_loop_context_t* ctx);
void control_loop_deinit(control_loop_context_t* ctx);

int control_loop_set_control_function(control_loop_context_t* ctx, 
                                    void (*function)(void*), void* arg);
int control_loop_set_safety_function(control_loop_context_t* ctx, 
                                   void (*function)(void*), void* arg);
int control_loop_set_telemetry_function(control_loop_context_t* ctx, 
                                      void (*function)(void*), void* arg);

int control_loop_start(scheduler_context_t* scheduler, control_loop_context_t* ctx);
int control_loop_stop(control_loop_context_t* ctx);

// Control loop main function (called by scheduler)
void control_loop_task(void* arg);

// State management
control_loop_state_t control_loop_get_state(const control_loop_context_t* ctx);
int control_loop_set_state(control_loop_context_t* ctx, control_loop_state_t state);

// Latency measurement
void control_loop_enable_latency_measurement(control_loop_context_t* ctx, bool enable);
int control_loop_get_latency_stats(const control_loop_context_t* ctx, 
                                  uint32_t* max_latency, uint32_t* avg_latency, 
                                  uint32_t* max_jitter);
void control_loop_reset_latency_stats(control_loop_context_t* ctx);

// Statistics
void control_loop_get_stats(const control_loop_context_t* ctx, control_loop_stats_t* stats);
void control_loop_print_stats(const control_loop_context_t* ctx);

// Timing utilities
uint64_t control_loop_get_cycle_time_us(const control_loop_context_t* ctx);
bool control_loop_is_deadline_met(const control_loop_context_t* ctx);

#ifdef __cplusplus
}
#endif

#endif // OHT_CONTROL_LOOP_H
