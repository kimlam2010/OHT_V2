/**
 * @file scheduler.h
 * @brief Real-time scheduler với priority levels cho OHT-50
 * @version 1.0
 * @date 2024-12-19
 */

#ifndef OHT_SCHEDULER_H
#define OHT_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// Priority levels (cao nhất = 0)
typedef enum {
    PRIORITY_CRITICAL = 0,    // E-Stop, Safety
    PRIORITY_HIGH = 1,        // Control loop, State machine
    PRIORITY_MEDIUM = 2,      // Communication, Telemetry
    PRIORITY_LOW = 3,         // Logging, Diagnostics
    PRIORITY_BACKGROUND = 4   // Maintenance, Cleanup
} scheduler_priority_t;

// Task status
typedef enum {
    TASK_READY = 0,
    TASK_RUNNING = 1,
    TASK_BLOCKED = 2,
    TASK_SUSPENDED = 3,
    TASK_TERMINATED = 4
} task_status_t;

// Task structure
typedef struct {
    uint32_t id;
    char name[32];
    void (*function)(void*);
    void* arg;
    scheduler_priority_t priority;
    task_status_t status;
    uint32_t period_ms;       // 0 = one-shot
    uint32_t deadline_ms;     // Relative deadline
    uint64_t last_run_time;   // Last execution time
    uint64_t next_run_time;   // Next scheduled time
    uint32_t exec_count;      // Execution counter
    uint32_t missed_deadlines; // Missed deadlines counter
    pthread_t thread;
    pthread_mutex_t mutex;
    bool is_periodic;
} scheduler_task_t;

// Scheduler context
typedef struct {
    scheduler_task_t* tasks;
    uint32_t task_count;
    uint32_t max_tasks;
    pthread_mutex_t scheduler_mutex;
    pthread_cond_t scheduler_cond;
    bool running;
    uint64_t start_time;
    uint32_t tick_count;
    struct timespec tick_period;
} scheduler_context_t;

// Function prototypes
int scheduler_init(scheduler_context_t* ctx, uint32_t max_tasks);
void scheduler_deinit(scheduler_context_t* ctx);

int scheduler_add_task(scheduler_context_t* ctx, 
                      const char* name,
                      void (*function)(void*),
                      void* arg,
                      scheduler_priority_t priority,
                      uint32_t period_ms,
                      uint32_t deadline_ms);

int scheduler_remove_task(scheduler_context_t* ctx, uint32_t task_id);
int scheduler_start(scheduler_context_t* ctx);
int scheduler_stop(scheduler_context_t* ctx);

// Task control
int scheduler_suspend_task(scheduler_context_t* ctx, uint32_t task_id);
int scheduler_resume_task(scheduler_context_t* ctx, uint32_t task_id);
int scheduler_set_priority(scheduler_context_t* ctx, uint32_t task_id, scheduler_priority_t priority);

// Timing utilities
uint64_t scheduler_get_time_ms(void);
uint64_t scheduler_get_time_us(void);
void scheduler_delay_ms(uint32_t ms);
void scheduler_delay_us(uint32_t us);

// Statistics
void scheduler_get_stats(scheduler_context_t* ctx, uint32_t task_id, 
                        uint32_t* exec_count, uint32_t* missed_deadlines);

// Interrupt priority management
int scheduler_set_interrupt_priority(int irq, int priority);
int scheduler_enable_interrupt(int irq);
int scheduler_disable_interrupt(int irq);

#ifdef __cplusplus
}
#endif

#endif // OHT_SCHEDULER_H
