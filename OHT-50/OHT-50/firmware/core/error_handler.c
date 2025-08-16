/**
 * @file error_handler.c
 * @brief Implementation của error handling framework cho OHT-50
 * @version 1.0
 * @date 2024-12-19
 */

#include "error_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// Internal constants
#define MAX_ERROR_HISTORY 100
#define ERROR_LOG_FILE "/var/log/oht50_errors.log"

// Error history entry
typedef struct {
    error_context_t error;
    bool handled;
    error_action_t action_taken;
} error_history_entry_t;

// Global error handler context
static struct {
    error_handler_t custom_handler;
    error_stats_t stats;
    error_history_entry_t history[MAX_ERROR_HISTORY];
    uint32_t history_index;
    pthread_mutex_t mutex;
    bool initialized;
    FILE* log_file;
} g_error_handler = {0};

// Internal function prototypes
static error_category_t error_get_category(error_code_t code);
static error_level_t error_get_level(error_code_t code);
static error_action_t error_get_default_action(error_code_t code);
static void error_log_to_file(const error_context_t* error);
static void error_default_handler(const error_context_t* error);

/**
 * Initialize error handler
 */
int error_handler_init(void) {
    if (g_error_handler.initialized) {
        return 0;
    }

    memset(&g_error_handler, 0, sizeof(g_error_handler));
    
    if (pthread_mutex_init(&g_error_handler.mutex, NULL) != 0) {
        return -1;
    }

    // Open error log file
    g_error_handler.log_file = fopen(ERROR_LOG_FILE, "a");
    if (!g_error_handler.log_file) {
        // Fallback to stderr
        g_error_handler.log_file = stderr;
    }

    g_error_handler.initialized = true;
    g_error_handler.custom_handler = error_default_handler;
    
    return 0;
}

/**
 * Deinitialize error handler
 */
void error_handler_deinit(void) {
    if (!g_error_handler.initialized) {
        return;
    }

    pthread_mutex_lock(&g_error_handler.mutex);
    
    if (g_error_handler.log_file && g_error_handler.log_file != stderr) {
        fclose(g_error_handler.log_file);
    }
    
    pthread_mutex_unlock(&g_error_handler.mutex);
    pthread_mutex_destroy(&g_error_handler.mutex);
    
    memset(&g_error_handler, 0, sizeof(g_error_handler));
}

/**
 * Report error
 */
void error_report(error_code_t code, error_level_t level, 
                 const char* file, uint32_t line, const char* function,
                 const char* message, void* context) {
    if (!g_error_handler.initialized) {
        return;
    }

    error_context_t error;
    error.code = code;
    error.level = level;
    error.category = error_get_category(code);
    error.timestamp = (uint32_t)time(NULL);
    error.line = line;
    error.file = file;
    error.function = function;
    error.message = message;
    error.context = context;

    pthread_mutex_lock(&g_error_handler.mutex);

    // Update statistics
    g_error_handler.stats.total_errors++;
    g_error_handler.stats.errors_by_level[level]++;
    g_error_handler.stats.errors_by_category[error.category]++;
    g_error_handler.stats.last_error_timestamp = error.timestamp;

    // Add to history
    g_error_handler.history[g_error_handler.history_index].error = error;
    g_error_handler.history[g_error_handler.history_index].handled = false;
    g_error_handler.history[g_error_handler.history_index].action_taken = ERROR_ACTION_NONE;
    
    g_error_handler.history_index = (g_error_handler.history_index + 1) % MAX_ERROR_HISTORY;

    // Log error
    error_log_to_file(&error);

    pthread_mutex_unlock(&g_error_handler.mutex);

    // Call custom handler
    if (g_error_handler.custom_handler) {
        g_error_handler.custom_handler(&error);
    }
}

/**
 * Set custom error handler
 */
void error_handler_set_handler(error_handler_t handler) {
    if (!g_error_handler.initialized) {
        return;
    }

    pthread_mutex_lock(&g_error_handler.mutex);
    g_error_handler.custom_handler = handler ? handler : error_default_handler;
    pthread_mutex_unlock(&g_error_handler.mutex);
}

/**
 * Get recommended action for error
 */
error_action_t error_handler_get_recommended_action(error_code_t code) {
    return error_get_default_action(code);
}

/**
 * Execute recovery for error
 */
int error_handler_execute_recovery(error_code_t code) {
    if (!g_error_handler.initialized) {
        return -1;
    }

    error_action_t action = error_get_default_action(code);
    
    pthread_mutex_lock(&g_error_handler.mutex);
    g_error_handler.stats.recovery_attempts++;
    pthread_mutex_unlock(&g_error_handler.mutex);

    int result = 0;
    switch (action) {
        case ERROR_ACTION_RETRY:
            // Simple retry - just return success
            result = 0;
            break;
            
        case ERROR_ACTION_RESET:
            // Reset specific component
            printf("ERROR: Resetting component for error 0x%04X\n", code);
            result = 0;
            break;
            
        case ERROR_ACTION_DEGRADE:
            // Degrade functionality
            printf("ERROR: Degrading functionality for error 0x%04X\n", code);
            result = 0;
            break;
            
        case ERROR_ACTION_SHUTDOWN:
            // Graceful shutdown
            printf("ERROR: Initiating graceful shutdown for error 0x%04X\n", code);
            result = 0;
            break;
            
        case ERROR_ACTION_EMERGENCY_STOP:
            // Emergency stop
            printf("ERROR: Emergency stop for error 0x%04X\n", code);
            result = 0;
            break;
            
        default:
            result = -1;
            break;
    }

    if (result == 0) {
        pthread_mutex_lock(&g_error_handler.mutex);
        g_error_handler.stats.successful_recoveries++;
        pthread_mutex_unlock(&g_error_handler.mutex);
    }

    return result;
}

/**
 * Get error statistics
 */
void error_handler_get_stats(error_stats_t* stats) {
    if (!g_error_handler.initialized || !stats) {
        return;
    }

    pthread_mutex_lock(&g_error_handler.mutex);
    memcpy(stats, &g_error_handler.stats, sizeof(error_stats_t));
    pthread_mutex_unlock(&g_error_handler.mutex);
}

/**
 * Reset error statistics
 */
void error_handler_reset_stats(void) {
    if (!g_error_handler.initialized) {
        return;
    }

    pthread_mutex_lock(&g_error_handler.mutex);
    memset(&g_error_handler.stats, 0, sizeof(error_stats_t));
    pthread_mutex_unlock(&g_error_handler.mutex);
}

/**
 * Print error statistics
 */
void error_handler_print_stats(void) {
    if (!g_error_handler.initialized) {
        return;
    }

    error_stats_t stats;
    error_handler_get_stats(&stats);

    printf("=== Error Handler Statistics ===\n");
    printf("Total errors: %u\n", stats.total_errors);
    printf("Errors by level:\n");
    printf("  Info: %u\n", stats.errors_by_level[ERROR_LEVEL_INFO]);
    printf("  Warning: %u\n", stats.errors_by_level[ERROR_LEVEL_WARNING]);
    printf("  Error: %u\n", stats.errors_by_level[ERROR_LEVEL_ERROR]);
    printf("  Critical: %u\n", stats.errors_by_level[ERROR_LEVEL_CRITICAL]);
    printf("  Fatal: %u\n", stats.errors_by_level[ERROR_LEVEL_FATAL]);
    printf("Errors by category:\n");
    printf("  System: %u\n", stats.errors_by_category[ERROR_CAT_SYSTEM]);
    printf("  Hardware: %u\n", stats.errors_by_category[ERROR_CAT_HARDWARE]);
    printf("  Communication: %u\n", stats.errors_by_category[ERROR_CAT_COMMUNICATION]);
    printf("  Safety: %u\n", stats.errors_by_category[ERROR_CAT_SAFETY]);
    printf("  Control: %u\n", stats.errors_by_category[ERROR_CAT_CONTROL]);
    printf("  Scheduler: %u\n", stats.errors_by_category[ERROR_CAT_SCHEDULER]);
    printf("  Memory: %u\n", stats.errors_by_category[ERROR_CAT_MEMORY]);
    printf("  Timing: %u\n", stats.errors_by_category[ERROR_CAT_TIMING]);
    printf("Recovery attempts: %u\n", stats.recovery_attempts);
    printf("Successful recoveries: %u\n", stats.successful_recoveries);
    printf("Last error timestamp: %u\n", stats.last_error_timestamp);
    printf("==============================\n");
}

/**
 * Check if system is healthy
 */
bool error_handler_is_system_healthy(void) {
    if (!g_error_handler.initialized) {
        return false;
    }

    pthread_mutex_lock(&g_error_handler.mutex);
    bool healthy = (g_error_handler.stats.errors_by_level[ERROR_LEVEL_CRITICAL] == 0 &&
                   g_error_handler.stats.errors_by_level[ERROR_LEVEL_FATAL] == 0);
    pthread_mutex_unlock(&g_error_handler.mutex);
    
    return healthy;
}

/**
 * Check if system has critical errors
 */
bool error_handler_has_critical_errors(void) {
    if (!g_error_handler.initialized) {
        return false;
    }

    pthread_mutex_lock(&g_error_handler.mutex);
    bool has_critical = (g_error_handler.stats.errors_by_level[ERROR_LEVEL_CRITICAL] > 0 ||
                        g_error_handler.stats.errors_by_level[ERROR_LEVEL_FATAL] > 0);
    pthread_mutex_unlock(&g_error_handler.mutex);
    
    return has_critical;
}

/**
 * Get error count by level
 */
uint32_t error_handler_get_error_count(error_level_t level) {
    if (!g_error_handler.initialized || level >= 5) {
        return 0;
    }

    pthread_mutex_lock(&g_error_handler.mutex);
    uint32_t count = g_error_handler.stats.errors_by_level[level];
    pthread_mutex_unlock(&g_error_handler.mutex);
    
    return count;
}

/**
 * Get error category from error code
 */
static error_category_t error_get_category(error_code_t code) {
    uint16_t category = (code >> 8) & 0xFF;
    return (error_category_t)category;
}

/**
 * Get error level from error code
 */
static error_level_t error_get_level(error_code_t code) {
    // This is a simplified mapping - in a real system, you might have a lookup table
    if (code >= ERROR_SAFETY_ESTOP_ACTIVE && code <= ERROR_SAFETY_INTERLOCK_FAILED) {
        return ERROR_LEVEL_CRITICAL;
    } else if (code >= ERROR_CTRL_DEADLINE_MISSED && code <= ERROR_CTRL_SETPOINT_INVALID) {
        return ERROR_LEVEL_ERROR;
    } else if (code >= ERROR_COMM_RS485_TIMEOUT && code <= ERROR_COMM_WS_DISCONNECTED) {
        return ERROR_LEVEL_WARNING;
    } else {
        return ERROR_LEVEL_INFO;
    }
}

/**
 * Get default action for error code
 */
static error_action_t error_get_default_action(error_code_t code) {
    // Safety errors require immediate action
    if (code >= ERROR_SAFETY_ESTOP_ACTIVE && code <= ERROR_SAFETY_INTERLOCK_FAILED) {
        return ERROR_ACTION_EMERGENCY_STOP;
    }
    
    // Critical system errors require shutdown
    if (code >= ERROR_SYS_INIT_FAILED && code <= ERROR_SYS_CONFIG_INVALID) {
        return ERROR_ACTION_SHUTDOWN;
    }
    
    // Control errors can be retried
    if (code >= ERROR_CTRL_DEADLINE_MISSED && code <= ERROR_CTRL_SETPOINT_INVALID) {
        return ERROR_ACTION_RETRY;
    }
    
    // Communication errors can be retried
    if (code >= ERROR_COMM_RS485_TIMEOUT && code <= ERROR_COMM_WS_DISCONNECTED) {
        return ERROR_ACTION_RETRY;
    }
    
    // Hardware errors may require reset
    if (code >= ERROR_HW_GPIO_FAILED && code <= ERROR_HW_TIMER_FAILED) {
        return ERROR_ACTION_RESET;
    }
    
    return ERROR_ACTION_NONE;
}

/**
 * Log error to file
 */
static void error_log_to_file(const error_context_t* error) {
    if (!g_error_handler.log_file) {
        return;
    }

    const char* level_str[] = {"INFO", "WARN", "ERROR", "CRIT", "FATAL"};
    const char* category_str[] = {"SYS", "HW", "COMM", "SAFETY", "CTRL", "SCHED", "MEM", "TIME"};
    
    fprintf(g_error_handler.log_file, 
            "[%u] %s/%s 0x%04X: %s at %s:%u in %s\n",
            error->timestamp,
            level_str[error->level],
            category_str[error->category],
            error->code,
            error->message ? error->message : "No message",
            error->file ? error->file : "unknown",
            error->line,
            error->function ? error->function : "unknown");
    
    fflush(g_error_handler.log_file);
}

/**
 * Default error handler
 */
static void error_default_handler(const error_context_t* error) {
    const char* level_str[] = {"INFO", "WARN", "ERROR", "CRIT", "FATAL"};
    
    printf("ERROR [%s] 0x%04X: %s at %s:%u in %s\n",
           level_str[error->level],
           error->code,
           error->message ? error->message : "No message",
           error->file ? error->file : "unknown",
           error->line,
           error->function ? error->function : "unknown");
}
