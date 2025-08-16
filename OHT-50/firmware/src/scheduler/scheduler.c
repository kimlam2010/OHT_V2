/**
 * OHT-50 Firmware - Real-time Scheduler Implementation
 * 
 * @file scheduler.c
 * @version 1.0.0
 * @date 2025-01-27
 * @author FW Team
 */

#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

// Configuration
#define MAX_TASKS 32
#define MAX_PRIORITY_LEVELS 5

// Global variables
static scheduler_task_t g_tasks[MAX_TASKS];
static uint32_t g_task_count = 0;
static uint32_t g_next_task_id = 1;
static bool g_scheduler_running = false;
static scheduler_stats_t g_stats = {0};
static pthread_mutex_t g_scheduler_mutex = PTHREAD_MUTEX_INITIALIZER;

// Priority queues for each priority level
static uint32_t g_priority_queues[MAX_PRIORITY_LEVELS][MAX_TASKS];
static uint32_t g_queue_sizes[MAX_PRIORITY_LEVELS] = {0};

/**
 * Get current time in microseconds
 */
static uint64_t get_time_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
}

/**
 * Initialize scheduler
 */
int scheduler_init(void) {
    pthread_mutex_lock(&g_scheduler_mutex);
    
    // Clear task array
    memset(g_tasks, 0, sizeof(g_tasks));
    g_task_count = 0;
    g_next_task_id = 1;
    
    // Clear priority queues
    memset(g_priority_queues, 0, sizeof(g_priority_queues));
    memset(g_queue_sizes, 0, sizeof(g_queue_sizes));
    
    // Clear statistics
    memset(&g_stats, 0, sizeof(g_stats));
    
    g_scheduler_running = true;
    
    pthread_mutex_unlock(&g_scheduler_mutex);
    
    printf("[SCHEDULER] Initialized successfully\n");
    return 0;
}

/**
 * Cleanup scheduler
 */
void scheduler_cleanup(void) {
    pthread_mutex_lock(&g_scheduler_mutex);
    g_scheduler_running = false;
    pthread_mutex_unlock(&g_scheduler_mutex);
    
    printf("[SCHEDULER] Cleanup completed\n");
}

/**
 * Add task to priority queue
 */
static void add_to_priority_queue(uint32_t task_id, scheduler_priority_t priority) {
    if (priority >= MAX_PRIORITY_LEVELS) {
        return;
    }
    
    if (g_queue_sizes[priority] < MAX_TASKS) {
        g_priority_queues[priority][g_queue_sizes[priority]] = task_id;
        g_queue_sizes[priority]++;
    }
}

/**
 * Remove task from priority queue
 */
static void remove_from_priority_queue(uint32_t task_id, scheduler_priority_t priority) {
    if (priority >= MAX_PRIORITY_LEVELS) {
        return;
    }
    
    for (uint32_t i = 0; i < g_queue_sizes[priority]; i++) {
        if (g_priority_queues[priority][i] == task_id) {
            // Shift remaining tasks
            for (uint32_t j = i; j < g_queue_sizes[priority] - 1; j++) {
                g_priority_queues[priority][j] = g_priority_queues[priority][j + 1];
            }
            g_queue_sizes[priority]--;
            break;
        }
    }
}

/**
 * Add task to scheduler
 */
int scheduler_add_task(const scheduler_task_t* task) {
    if (!task || !task->function) {
        return -1;
    }
    
    pthread_mutex_lock(&g_scheduler_mutex);
    
    if (g_task_count >= MAX_TASKS) {
        pthread_mutex_unlock(&g_scheduler_mutex);
        return -1;
    }
    
    // Find free slot
    uint32_t slot = 0;
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].id == 0) {
            slot = i;
            break;
        }
    }
    
    // Copy task data
    g_tasks[slot] = *task;
    g_tasks[slot].id = g_next_task_id++;
    g_tasks[slot].state = TASK_STATE_READY;
    g_tasks[slot].last_run_time = 0;
    g_tasks[slot].execution_time_us = 0;
    g_tasks[slot].deadline_missed = 0;
    
    // Add to priority queue
    add_to_priority_queue(g_tasks[slot].id, g_tasks[slot].priority);
    
    g_task_count++;
    
    pthread_mutex_unlock(&g_scheduler_mutex);
    
    printf("[SCHEDULER] Added task '%s' (ID: %u, Priority: %d)\n", 
           task->name, g_tasks[slot].id, task->priority);
    
    return g_tasks[slot].id;
}

/**
 * Remove task from scheduler
 */
int scheduler_remove_task(uint32_t task_id) {
    pthread_mutex_lock(&g_scheduler_mutex);
    
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].id == task_id) {
            // Remove from priority queue
            remove_from_priority_queue(task_id, g_tasks[i].priority);
            
            // Clear task
            memset(&g_tasks[i], 0, sizeof(scheduler_task_t));
            g_task_count--;
            
            pthread_mutex_unlock(&g_scheduler_mutex);
            
            printf("[SCHEDULER] Removed task ID %u\n", task_id);
            return 0;
        }
    }
    
    pthread_mutex_unlock(&g_scheduler_mutex);
    return -1;
}

/**
 * Enable task
 */
int scheduler_enable_task(uint32_t task_id) {
    pthread_mutex_lock(&g_scheduler_mutex);
    
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].id == task_id) {
            g_tasks[i].enabled = true;
            g_tasks[i].state = TASK_STATE_READY;
            
            pthread_mutex_unlock(&g_scheduler_mutex);
            return 0;
        }
    }
    
    pthread_mutex_unlock(&g_scheduler_mutex);
    return -1;
}

/**
 * Disable task
 */
int scheduler_disable_task(uint32_t task_id) {
    pthread_mutex_lock(&g_scheduler_mutex);
    
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].id == task_id) {
            g_tasks[i].enabled = false;
            g_tasks[i].state = TASK_STATE_SUSPENDED;
            
            pthread_mutex_unlock(&g_scheduler_mutex);
            return 0;
        }
    }
    
    pthread_mutex_unlock(&g_scheduler_mutex);
    return -1;
}

/**
 * Set task priority
 */
int scheduler_set_priority(uint32_t task_id, scheduler_priority_t priority) {
    if (priority >= MAX_PRIORITY_LEVELS) {
        return -1;
    }
    
    pthread_mutex_lock(&g_scheduler_mutex);
    
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].id == task_id) {
            // Remove from old priority queue
            remove_from_priority_queue(task_id, g_tasks[i].priority);
            
            // Update priority
            g_tasks[i].priority = priority;
            
            // Add to new priority queue
            add_to_priority_queue(task_id, priority);
            
            pthread_mutex_unlock(&g_scheduler_mutex);
            return 0;
        }
    }
    
    pthread_mutex_unlock(&g_scheduler_mutex);
    return -1;
}

/**
 * Set task period
 */
int scheduler_set_period(uint32_t task_id, uint32_t period_ms) {
    pthread_mutex_lock(&g_scheduler_mutex);
    
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].id == task_id) {
            g_tasks[i].period_ms = period_ms;
            pthread_mutex_unlock(&g_scheduler_mutex);
            return 0;
        }
    }
    
    pthread_mutex_unlock(&g_scheduler_mutex);
    return -1;
}

/**
 * Find highest priority ready task
 */
static uint32_t find_highest_priority_task(uint32_t current_time) {
    for (int priority = MAX_PRIORITY_LEVELS - 1; priority >= 0; priority--) {
        for (uint32_t i = 0; i < g_queue_sizes[priority]; i++) {
            uint32_t task_id = g_priority_queues[priority][i];
            
            // Find task in array
            for (uint32_t j = 0; j < MAX_TASKS; j++) {
                if (g_tasks[j].id == task_id && g_tasks[j].enabled) {
                    // Check if task is ready to run
                    if (g_tasks[j].state == TASK_STATE_READY) {
                        if (g_tasks[j].period_ms == 0 || 
                            (current_time - g_tasks[j].last_run_time) >= g_tasks[j].period_ms) {
                            return task_id;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    return 0; // No task ready
}

/**
 * Scheduler tick function
 */
void scheduler_tick(void) {
    static uint32_t tick_count = 0;
    uint32_t current_time = tick_count++;
    
    pthread_mutex_lock(&g_scheduler_mutex);
    
    if (!g_scheduler_running) {
        pthread_mutex_unlock(&g_scheduler_mutex);
        return;
    }
    
    g_stats.total_ticks++;
    
    // Find highest priority ready task
    uint32_t task_id = find_highest_priority_task(current_time);
    
    if (task_id != 0) {
        // Find task in array
        for (uint32_t i = 0; i < MAX_TASKS; i++) {
            if (g_tasks[i].id == task_id) {
                // Execute task
                g_tasks[i].state = TASK_STATE_RUNNING;
                g_tasks[i].last_run_time = current_time;
                
                uint64_t start_time = get_time_us();
                
                // Execute task function
                g_tasks[i].function();
                
                uint64_t end_time = get_time_us();
                uint32_t execution_time = (uint32_t)(end_time - start_time);
                
                g_tasks[i].execution_time_us = execution_time;
                g_tasks[i].state = TASK_STATE_READY;
                
                // Update statistics
                g_stats.context_switches++;
                
                // Check for deadline miss
                if (g_tasks[i].period_ms > 0 && execution_time > g_tasks[i].period_ms * 1000) {
                    g_tasks[i].deadline_missed++;
                    g_stats.deadline_misses++;
                }
                
                // Update latency statistics
                if (execution_time > g_stats.max_latency_us) {
                    g_stats.max_latency_us = execution_time;
                }
                
                // Simple average latency calculation
                g_stats.avg_latency_us = (g_stats.avg_latency_us + execution_time) / 2;
                
                break;
            }
        }
    } else {
        g_stats.idle_ticks++;
    }
    
    pthread_mutex_unlock(&g_scheduler_mutex);
}

/**
 * Get scheduler statistics
 */
int scheduler_get_stats(scheduler_stats_t* stats) {
    if (!stats) {
        return -1;
    }
    
    pthread_mutex_lock(&g_scheduler_mutex);
    *stats = g_stats;
    pthread_mutex_unlock(&g_scheduler_mutex);
    
    return 0;
}

/**
 * Reset scheduler statistics
 */
void scheduler_reset_stats(void) {
    pthread_mutex_lock(&g_scheduler_mutex);
    memset(&g_stats, 0, sizeof(g_stats));
    pthread_mutex_unlock(&g_scheduler_mutex);
}

/**
 * Get current system time in microseconds
 */
uint64_t scheduler_get_time_us(void) {
    return get_time_us();
}

/**
 * Get task information
 */
int scheduler_get_task_info(uint32_t task_id, scheduler_task_t* task) {
    if (!task) {
        return -1;
    }
    
    pthread_mutex_lock(&g_scheduler_mutex);
    
    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].id == task_id) {
            *task = g_tasks[i];
            pthread_mutex_unlock(&g_scheduler_mutex);
            return 0;
        }
    }
    
    pthread_mutex_unlock(&g_scheduler_mutex);
    return -1;
}

/**
 * Get number of tasks
 */
uint32_t scheduler_get_task_count(void) {
    return g_task_count;
}

/**
 * Check if scheduler is running
 */
bool scheduler_is_running(void) {
    return g_scheduler_running;
}
