/**
 * OHT-50 Firmware - Real-time Scheduler
 * 
 * @file scheduler.h
 * @version 1.0.0
 * @date 2025-01-27
 * @author FW Team
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Priority levels
typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_CRITICAL = 3,
    PRIORITY_SAFETY = 4,
    PRIORITY_MAX = 5
} scheduler_priority_t;

// Task states
typedef enum {
    TASK_STATE_IDLE = 0,
    TASK_STATE_READY = 1,
    TASK_STATE_RUNNING = 2,
    TASK_STATE_BLOCKED = 3,
    TASK_STATE_SUSPENDED = 4
} task_state_t;

// Task function pointer
typedef void (*task_function_t)(void);

// Task structure
typedef struct {
    uint32_t id;                    // Task ID
    char name[32];                  // Task name
    task_function_t function;       // Task function
    scheduler_priority_t priority;  // Task priority
    task_state_t state;            // Current state
    uint32_t period_ms;            // Period in milliseconds
    uint32_t last_run_time;        // Last run time
    uint32_t execution_time_us;    // Execution time in microseconds
    uint32_t deadline_missed;      // Number of deadline misses
    bool enabled;                  // Task enabled flag
} scheduler_task_t;

// Scheduler statistics
typedef struct {
    uint32_t total_ticks;          // Total scheduler ticks
    uint32_t idle_ticks;           // Idle ticks
    uint32_t max_latency_us;       // Maximum latency in microseconds
    uint32_t avg_latency_us;       // Average latency in microseconds
    uint32_t deadline_misses;      // Total deadline misses
    uint32_t context_switches;     // Number of context switches
} scheduler_stats_t;

// Function prototypes

/**
 * Initialize scheduler
 * @return 0 on success, -1 on failure
 */
int scheduler_init(void);

/**
 * Cleanup scheduler
 */
void scheduler_cleanup(void);

/**
 * Add task to scheduler
 * @param task Task structure
 * @return Task ID on success, -1 on failure
 */
int scheduler_add_task(const scheduler_task_t* task);

/**
 * Remove task from scheduler
 * @param task_id Task ID to remove
 * @return 0 on success, -1 on failure
 */
int scheduler_remove_task(uint32_t task_id);

/**
 * Enable task
 * @param task_id Task ID to enable
 * @return 0 on success, -1 on failure
 */
int scheduler_enable_task(uint32_t task_id);

/**
 * Disable task
 * @param task_id Task ID to disable
 * @return 0 on success, -1 on failure
 */
int scheduler_disable_task(uint32_t task_id);

/**
 * Set task priority
 * @param task_id Task ID
 * @param priority New priority
 * @return 0 on success, -1 on failure
 */
int scheduler_set_priority(uint32_t task_id, scheduler_priority_t priority);

/**
 * Set task period
 * @param task_id Task ID
 * @param period_ms Period in milliseconds
 * @return 0 on success, -1 on failure
 */
int scheduler_set_period(uint32_t task_id, uint32_t period_ms);

/**
 * Scheduler tick function (called every 1ms)
 */
void scheduler_tick(void);

/**
 * Get scheduler statistics
 * @param stats Pointer to statistics structure
 * @return 0 on success, -1 on failure
 */
int scheduler_get_stats(scheduler_stats_t* stats);

/**
 * Reset scheduler statistics
 */
void scheduler_reset_stats(void);

/**
 * Get current system time in microseconds
 * @return System time in microseconds
 */
uint64_t scheduler_get_time_us(void);

/**
 * Get task information
 * @param task_id Task ID
 * @param task Pointer to task structure
 * @return 0 on success, -1 on failure
 */
int scheduler_get_task_info(uint32_t task_id, scheduler_task_t* task);

/**
 * Get number of tasks
 * @return Number of tasks
 */
uint32_t scheduler_get_task_count(void);

/**
 * Check if scheduler is running
 * @return true if running, false otherwise
 */
bool scheduler_is_running(void);

// Predefined task functions
void control_loop_tick(void);
void rs485_tick(void);

#ifdef __cplusplus
}
#endif

#endif // SCHEDULER_H
