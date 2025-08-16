/**
 * @file scheduler.c
 * @brief Implementation của real-time scheduler cho OHT-50
 * @version 1.0
 * @date 2024-12-19
 */

#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>

// Internal constants
#define MAX_TASK_NAME_LEN 32
#define SCHEDULER_TICK_MS 1
#define MAX_MISSED_DEADLINES 10

// Internal function prototypes
static void* scheduler_thread_function(void* arg);
static void scheduler_dispatcher(scheduler_context_t* ctx);
static int scheduler_find_highest_priority_task(scheduler_context_t* ctx);
static void scheduler_update_task_timing(scheduler_context_t* ctx, uint32_t task_id);
static int scheduler_set_thread_priority(pthread_t thread, int priority);

// Global scheduler context
static scheduler_context_t* g_scheduler_ctx = NULL;

/**
 * Initialize scheduler
 */
int scheduler_init(scheduler_context_t* ctx, uint32_t max_tasks) {
    if (!ctx || max_tasks == 0) {
        return -1;
    }

    memset(ctx, 0, sizeof(scheduler_context_t));
    
    ctx->tasks = calloc(max_tasks, sizeof(scheduler_task_t));
    if (!ctx->tasks) {
        return -1;
    }

    ctx->max_tasks = max_tasks;
    ctx->task_count = 0;
    ctx->running = false;

    // Initialize mutex and condition variable
    if (pthread_mutex_init(&ctx->scheduler_mutex, NULL) != 0) {
        free(ctx->tasks);
        return -1;
    }

    if (pthread_cond_init(&ctx->scheduler_cond, NULL) != 0) {
        pthread_mutex_destroy(&ctx->scheduler_mutex);
        free(ctx->tasks);
        return -1;
    }

    // Set tick period
    ctx->tick_period.tv_sec = 0;
    ctx->tick_period.tv_nsec = SCHEDULER_TICK_MS * 1000000; // Convert to nanoseconds

    g_scheduler_ctx = ctx;
    return 0;
}

/**
 * Deinitialize scheduler
 */
void scheduler_deinit(scheduler_context_t* ctx) {
    if (!ctx) return;

    scheduler_stop(ctx);

    if (ctx->tasks) {
        free(ctx->tasks);
        ctx->tasks = NULL;
    }

    pthread_mutex_destroy(&ctx->scheduler_mutex);
    pthread_cond_destroy(&ctx->scheduler_cond);

    g_scheduler_ctx = NULL;
}

/**
 * Add task to scheduler
 */
int scheduler_add_task(scheduler_context_t* ctx, 
                      const char* name,
                      void (*function)(void*),
                      void* arg,
                      scheduler_priority_t priority,
                      uint32_t period_ms,
                      uint32_t deadline_ms) {
    if (!ctx || !name || !function || ctx->task_count >= ctx->max_tasks) {
        return -1;
    }

    pthread_mutex_lock(&ctx->scheduler_mutex);

    // Find free slot
    uint32_t task_id = ctx->task_count;
    scheduler_task_t* task = &ctx->tasks[task_id];

    // Initialize task
    task->id = task_id;
    strncpy(task->name, name, MAX_TASK_NAME_LEN - 1);
    task->name[MAX_TASK_NAME_LEN - 1] = '\0';
    task->function = function;
    task->arg = arg;
    task->priority = priority;
    task->status = TASK_READY;
    task->period_ms = period_ms;
    task->deadline_ms = deadline_ms;
    task->exec_count = 0;
    task->missed_deadlines = 0;
    task->is_periodic = (period_ms > 0);

    // Initialize task mutex
    if (pthread_mutex_init(&task->mutex, NULL) != 0) {
        pthread_mutex_unlock(&ctx->scheduler_mutex);
        return -1;
    }

    // Set initial timing
    uint64_t now = scheduler_get_time_ms();
    task->last_run_time = now;
    task->next_run_time = now + (task->is_periodic ? period_ms : deadline_ms);

    ctx->task_count++;

    pthread_mutex_unlock(&ctx->scheduler_mutex);
    return task_id;
}

/**
 * Remove task from scheduler
 */
int scheduler_remove_task(scheduler_context_t* ctx, uint32_t task_id) {
    if (!ctx || task_id >= ctx->task_count) {
        return -1;
    }

    pthread_mutex_lock(&ctx->scheduler_mutex);

    scheduler_task_t* task = &ctx->tasks[task_id];
    
    // Suspend task first
    task->status = TASK_SUSPENDED;
    
    // Wait for task to finish if running
    if (task->status == TASK_RUNNING) {
        pthread_mutex_unlock(&ctx->scheduler_mutex);
        pthread_join(task->thread, NULL);
        pthread_mutex_lock(&ctx->scheduler_mutex);
    }

    // Clean up task
    pthread_mutex_destroy(&task->mutex);
    memset(task, 0, sizeof(scheduler_task_t));

    // Compact task array
    for (uint32_t i = task_id; i < ctx->task_count - 1; i++) {
        ctx->tasks[i] = ctx->tasks[i + 1];
        ctx->tasks[i].id = i;
    }

    ctx->task_count--;

    pthread_mutex_unlock(&ctx->scheduler_mutex);
    return 0;
}

/**
 * Start scheduler
 */
int scheduler_start(scheduler_context_t* ctx) {
    if (!ctx || ctx->running) {
        return -1;
    }

    ctx->running = true;
    ctx->start_time = scheduler_get_time_ms();
    ctx->tick_count = 0;

    // Start dispatcher thread with high priority
    pthread_t dispatcher_thread;
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);

    if (pthread_create(&dispatcher_thread, &attr, scheduler_thread_function, ctx) != 0) {
        ctx->running = false;
        return -1;
    }

    pthread_attr_destroy(&attr);
    return 0;
}

/**
 * Stop scheduler
 */
int scheduler_stop(scheduler_context_t* ctx) {
    if (!ctx || !ctx->running) {
        return -1;
    }

    ctx->running = false;
    pthread_cond_broadcast(&ctx->scheduler_cond);
    return 0;
}

/**
 * Scheduler dispatcher thread function
 */
static void* scheduler_thread_function(void* arg) {
    scheduler_context_t* ctx = (scheduler_context_t*)arg;
    
    // Set thread priority
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

    while (ctx->running) {
        scheduler_dispatcher(ctx);
        
        // Sleep for tick period
        nanosleep(&ctx->tick_period, NULL);
        ctx->tick_count++;
    }

    return NULL;
}

/**
 * Main dispatcher function
 */
static void scheduler_dispatcher(scheduler_context_t* ctx) {
    pthread_mutex_lock(&ctx->scheduler_mutex);

    uint64_t now = scheduler_get_time_ms();
    int highest_priority_task = scheduler_find_highest_priority_task(ctx);

    if (highest_priority_task >= 0) {
        scheduler_task_t* task = &ctx->tasks[highest_priority_task];
        
        // Check if task is ready to run
        if (task->status == TASK_READY && now >= task->next_run_time) {
            // Check deadline
            if (now > task->last_run_time + task->deadline_ms) {
                task->missed_deadlines++;
                printf("WARNING: Task %s missed deadline\n", task->name);
            }

            // Update timing
            scheduler_update_task_timing(ctx, highest_priority_task);

            // Execute task
            task->status = TASK_RUNNING;
            task->exec_count++;
            
            pthread_mutex_unlock(&ctx->scheduler_mutex);
            
            // Execute task function
            task->function(task->arg);
            
            pthread_mutex_lock(&ctx->scheduler_mutex);
            task->status = TASK_READY;
        }
    }

    pthread_mutex_unlock(&ctx->scheduler_mutex);
}

/**
 * Find highest priority ready task
 */
static int scheduler_find_highest_priority_task(scheduler_context_t* ctx) {
    int highest_priority = -1;
    int selected_task = -1;
    uint64_t earliest_deadline = UINT64_MAX;

    for (uint32_t i = 0; i < ctx->task_count; i++) {
        scheduler_task_t* task = &ctx->tasks[i];
        
        if (task->status == TASK_READY) {
            // EDF (Earliest Deadline First) within same priority
            if (task->priority < highest_priority || 
                (task->priority == highest_priority && task->next_run_time < earliest_deadline)) {
                highest_priority = task->priority;
                earliest_deadline = task->next_run_time;
                selected_task = i;
            }
        }
    }

    return selected_task;
}

/**
 * Update task timing for next execution
 */
static void scheduler_update_task_timing(scheduler_context_t* ctx, uint32_t task_id) {
    scheduler_task_t* task = &ctx->tasks[task_id];
    uint64_t now = scheduler_get_time_ms();
    
    task->last_run_time = now;
    
    if (task->is_periodic) {
        task->next_run_time = now + task->period_ms;
    } else {
        task->next_run_time = now + task->deadline_ms;
    }
}

/**
 * Get current time in milliseconds
 */
uint64_t scheduler_get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * Get current time in microseconds
 */
uint64_t scheduler_get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

/**
 * Delay for specified milliseconds
 */
void scheduler_delay_ms(uint32_t ms) {
    usleep(ms * 1000);
}

/**
 * Delay for specified microseconds
 */
void scheduler_delay_us(uint32_t us) {
    usleep(us);
}

/**
 * Get task statistics
 */
void scheduler_get_stats(scheduler_context_t* ctx, uint32_t task_id, 
                        uint32_t* exec_count, uint32_t* missed_deadlines) {
    if (!ctx || task_id >= ctx->task_count || !exec_count || !missed_deadlines) {
        return;
    }

    pthread_mutex_lock(&ctx->scheduler_mutex);
    scheduler_task_t* task = &ctx->tasks[task_id];
    *exec_count = task->exec_count;
    *missed_deadlines = task->missed_deadlines;
    pthread_mutex_unlock(&ctx->scheduler_mutex);
}

/**
 * Set interrupt priority (Linux specific)
 */
int scheduler_set_interrupt_priority(int irq, int priority) {
    // This is a placeholder for interrupt priority management
    // In a real implementation, this would interact with the kernel
    // For now, we'll use thread priorities as a proxy
    return 0;
}

/**
 * Enable interrupt
 */
int scheduler_enable_interrupt(int irq) {
    // Placeholder for interrupt enable/disable
    return 0;
}

/**
 * Disable interrupt
 */
int scheduler_disable_interrupt(int irq) {
    // Placeholder for interrupt enable/disable
    return 0;
}
