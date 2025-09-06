/**
 * @file api_diagnostics.c
 * @brief API Diagnostics & Logging Management Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.3
 */

#include "api_diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/sysinfo.h>

// Global variables
static bool g_diagnostics_initialized = false;
static api_diagnostics_config_t g_diagnostics_config;
static api_log_entry_t g_log_buffer[1000];  // Max 1000 log entries
static uint32_t g_log_count = 0;
static uint32_t g_log_index = 0;
static api_diagnostics_stats_t g_diagnostics_stats = {0};
static pthread_mutex_t g_diagnostics_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint64_t g_diagnostics_start_time = 0;

/**
 * @brief Initialize diagnostics management
 * @param config Diagnostics configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_init(const api_diagnostics_config_t *config) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Diagnostics: Initializing...");
    
    if (g_diagnostics_initialized) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Diagnostics: Already initialized");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize configuration
    g_diagnostics_config = *config;
    
    // Initialize log buffer
    memset(g_log_buffer, 0, sizeof(g_log_buffer));
    g_log_count = 0;
    g_log_index = 0;
    
    // Initialize statistics
    memset(&g_diagnostics_stats, 0, sizeof(g_diagnostics_stats));
    g_diagnostics_start_time = hal_get_timestamp_ms();
    
    g_diagnostics_initialized = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Diagnostics: Initialized with buffer size %u", g_diagnostics_config.log_buffer_size);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize diagnostics management
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Diagnostics: Deinitializing...");
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_initialized = false;
    g_log_count = 0;
    g_log_index = 0;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Diagnostics: Deinitialized");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Log message
 * @param level Log level
 * @param category Log category
 * @param source Source identifier
 * @param line_number Line number
 * @param message Log message
 * @param details Additional details (optional)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_log(api_log_level_t level, api_log_category_t category, 
                                const char *source, uint32_t line_number, 
                                const char *message, const char *details) {
    if (source == NULL || message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check if logging is enabled and level is sufficient
    if (!g_diagnostics_config.diagnostics_enabled || level > g_diagnostics_config.min_log_level) {
        return HAL_STATUS_OK;
    }
    
    pthread_mutex_lock(&g_diagnostics_mutex);
    
    // Create log entry
    api_log_entry_t *entry = &g_log_buffer[g_log_index];
    entry->timestamp = hal_get_timestamp_ms();
    entry->level = level;
    entry->category = category;
    strncpy(entry->source, source, sizeof(entry->source) - 1);
    entry->source[sizeof(entry->source) - 1] = '\0';
    entry->line_number = line_number;
    strncpy(entry->message, message, sizeof(entry->message) - 1);
    entry->message[sizeof(entry->message) - 1] = '\0';
    
    if (details != NULL) {
        strncpy(entry->details, details, sizeof(entry->details) - 1);
        entry->details[sizeof(entry->details) - 1] = '\0';
    } else {
        entry->details[0] = '\0';
    }
    
    // Update statistics
    g_diagnostics_stats.total_log_entries++;
    g_diagnostics_stats.log_entries_by_level[level]++;
    g_diagnostics_stats.log_entries_by_category[category]++;
    
    // Update log buffer
    g_log_index = (g_log_index + 1) % g_diagnostics_config.max_log_entries;
    if (g_log_count < g_diagnostics_config.max_log_entries) {
        g_log_count++;
    }
    
    pthread_mutex_unlock(&g_diagnostics_mutex);
    
    // Log to console if enabled
    if (g_diagnostics_config.log_to_console) {
        const char *level_name = api_diagnostics_get_log_level_name(level);
        const char *category_name = api_diagnostics_get_log_category_name(category);
        
        printf("[%s] [%s] [%s:%u] %s", level_name, category_name, source, line_number, message);
        if (details != NULL && strlen(details) > 0) {
            printf(" - %s", details);
        }
        printf("\n");
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get system diagnostics
 * @param diagnostics System diagnostics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_system(api_system_diagnostics_t *diagnostics) {
    if (diagnostics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Get system information
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0) {
        return HAL_STATUS_ERROR;
    }
    
    // Fill diagnostics structure
    diagnostics->uptime_ms = hal_get_timestamp_ms() - g_diagnostics_start_time;
    diagnostics->memory_usage_kb = (sys_info.totalram - sys_info.freeram) / 1024;
    diagnostics->memory_total_kb = sys_info.totalram / 1024;
    diagnostics->cpu_usage_percent = 0.0f; // Would need more complex calculation
    diagnostics->task_count = sys_info.procs;
    diagnostics->error_count = g_diagnostics_stats.log_entries_by_level[API_LOG_LEVEL_ERROR];
    diagnostics->warning_count = g_diagnostics_stats.log_entries_by_level[API_LOG_LEVEL_WARNING];
    diagnostics->network_connections = 0; // Would need network manager
    diagnostics->api_requests_total = 0; // Would need API manager stats
    diagnostics->api_requests_failed = 0; // Would need API manager stats
    diagnostics->api_response_time_avg = 0; // Would need API manager stats
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get module diagnostics
 * @param module_name Module name
 * @param diagnostics Module diagnostics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_module(const char *module_name, api_module_diagnostics_t *diagnostics) {
    if (module_name == NULL || diagnostics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Initialize diagnostics structure
    strncpy(diagnostics->module_name, module_name, sizeof(diagnostics->module_name) - 1);
    diagnostics->module_name[sizeof(diagnostics->module_name) - 1] = '\0';
    
    // Get module-specific diagnostics (placeholder implementation)
    diagnostics->online = true; // Would need module manager
    diagnostics->last_heartbeat = hal_get_timestamp_ms();
    diagnostics->error_count = 0;
    diagnostics->warning_count = 0;
    diagnostics->message_count = 0;
    diagnostics->message_errors = 0;
    diagnostics->response_time_avg = 0.0f;
    strcpy(diagnostics->status_message, "Module operational");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get all module diagnostics
 * @param diagnostics Array of module diagnostics
 * @param max_modules Maximum number of modules
 * @param actual_count Actual number of modules returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_all_modules(api_module_diagnostics_t *diagnostics, 
                                            uint32_t max_modules, uint32_t *actual_count) {
    if (diagnostics == NULL || actual_count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Get diagnostics for known modules
    const char *module_names[] = {
        "system_controller",
        "safety_manager",
        "motor_module",
        "power_module",
        "dock_module",
        "network_manager",
        "api_manager"
    };
    
    uint32_t module_count = sizeof(module_names) / sizeof(module_names[0]);
    *actual_count = 0;
    
    for (uint32_t i = 0; i < module_count && *actual_count < max_modules; i++) {
        hal_status_t result = api_diagnostics_get_module(module_names[i], &diagnostics[*actual_count]);
        if (result == HAL_STATUS_OK) {
            (*actual_count)++;
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get log entries
 * @param level Filter by log level (API_LOG_LEVEL_MAX for all)
 * @param category Filter by category (API_LOG_CATEGORY_MAX for all)
 * @param max_entries Maximum number of entries
 * @param entries Array of log entries
 * @param actual_count Actual number of entries returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_logs(api_log_level_t level, api_log_category_t category,
                                     uint32_t max_entries, api_log_entry_t *entries, uint32_t *actual_count) {
    if (entries == NULL || actual_count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_diagnostics_mutex);
    
    *actual_count = 0;
    
    // Start from the oldest entry
    uint32_t start_index = (g_log_index - g_log_count + g_diagnostics_config.max_log_entries) % g_diagnostics_config.max_log_entries;
    
    for (uint32_t i = 0; i < g_log_count && *actual_count < max_entries; i++) {
        uint32_t entry_index = (start_index + i) % g_diagnostics_config.max_log_entries;
        api_log_entry_t *entry = &g_log_buffer[entry_index];
        
        // Apply filters
        bool level_match = (level == API_LOG_LEVEL_MAX || entry->level == level);
        bool category_match = (category == API_LOG_CATEGORY_MAX || entry->category == category);
        
        if (level_match && category_match) {
            entries[*actual_count] = *entry;
            (*actual_count)++;
        }
    }
    
    pthread_mutex_unlock(&g_diagnostics_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Clear log entries
 * @param level Clear by log level (API_LOG_LEVEL_MAX for all)
 * @param category Clear by category (API_LOG_CATEGORY_MAX for all)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_clear_logs(api_log_level_t level, api_log_category_t category) {
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_diagnostics_mutex);
    
    if (level == API_LOG_LEVEL_MAX && category == API_LOG_CATEGORY_MAX) {
        // Clear all logs
        memset(g_log_buffer, 0, sizeof(g_log_buffer));
        g_log_count = 0;
        g_log_index = 0;
    } else {
        // Clear specific logs (more complex implementation needed)
        // For now, just clear all
        memset(g_log_buffer, 0, sizeof(g_log_buffer));
        g_log_count = 0;
        g_log_index = 0;
    }
    
    pthread_mutex_unlock(&g_diagnostics_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Diagnostics: Logs cleared");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get diagnostics statistics
 * @param stats Diagnostics statistics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_stats(api_diagnostics_stats_t *stats) {
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_diagnostics_mutex);
    
    *stats = g_diagnostics_stats;
    
    pthread_mutex_unlock(&g_diagnostics_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get diagnostics configuration
 * @param config Diagnostics configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_config(api_diagnostics_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *config = g_diagnostics_config;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set diagnostics configuration
 * @param config Diagnostics configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_set_config(const api_diagnostics_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_diagnostics_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_config = *config;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Diagnostics: Configuration updated");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get log level name
 * @param level Log level
 * @return const char* Level name
 */
const char* api_diagnostics_get_log_level_name(api_log_level_t level) {
    switch (level) {
        case API_LOG_LEVEL_ERROR: return "ERROR";
        case API_LOG_LEVEL_WARNING: return "WARNING";
        case API_LOG_LEVEL_INFO: return "INFO";
        case API_LOG_LEVEL_DEBUG: return "DEBUG";
        case API_LOG_LEVEL_TRACE: return "TRACE";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Get log category name
 * @param category Log category
 * @return const char* Category name
 */
const char* api_diagnostics_get_log_category_name(api_log_category_t category) {
    switch (category) {
        case API_LOG_CATEGORY_SYSTEM: return "SYSTEM";
        case API_LOG_CATEGORY_SAFETY: return "SAFETY";
        case API_LOG_CATEGORY_MOTOR: return "MOTOR";
        case API_LOG_CATEGORY_POWER: return "POWER";
        case API_LOG_CATEGORY_DOCK: return "DOCK";
        case API_LOG_CATEGORY_NETWORK: return "NETWORK";
        case API_LOG_CATEGORY_API: return "API";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Get log level from string
 * @param level_name Level name string
 * @return api_log_level_t Log level
 */
api_log_level_t api_diagnostics_get_log_level_from_string(const char *level_name) {
    if (level_name == NULL) {
        return API_LOG_LEVEL_MAX;
    }
    
    for (int i = 0; i < API_LOG_LEVEL_MAX; i++) {
        if (strcmp(api_diagnostics_get_log_level_name((api_log_level_t)i), level_name) == 0) {
            return (api_log_level_t)i;
        }
    }
    
    return API_LOG_LEVEL_MAX;
}

/**
 * @brief Get log category from string
 * @param category_name Category name string
 * @return api_log_category_t Log category
 */
api_log_category_t api_diagnostics_get_log_category_from_string(const char *category_name) {
    if (category_name == NULL) {
        return API_LOG_CATEGORY_MAX;
    }
    
    for (int i = 0; i < API_LOG_CATEGORY_MAX; i++) {
        if (strcmp(api_diagnostics_get_log_category_name((api_log_category_t)i), category_name) == 0) {
            return (api_log_category_t)i;
        }
    }
    
    return API_LOG_CATEGORY_MAX;
}
