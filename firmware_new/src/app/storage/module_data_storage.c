/**
 * @file module_data_storage.c
 * @brief Module Data Storage Functions Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (Module Data Storage Implementation)
 */

#include "module_data_storage.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Global Module Data Storage
module_data_t g_module_data[MAX_MODULES] = {0};
module_telemetry_history_t g_module_telemetry_history[MAX_MODULES] = {0};
module_command_history_t g_module_command_history[MAX_MODULES] = {0};

// Thread safety mutex
static pthread_mutex_t g_storage_mutex = PTHREAD_MUTEX_INITIALIZER;

// Private function declarations
static void module_data_storage_initialize_module(int module_id);
static bool module_data_storage_is_valid_module_id(int module_id);
static void module_data_storage_cleanup_old_telemetry_records(int module_id);
static void module_data_storage_cleanup_old_command_records(int module_id);

/**
 * @brief Initialize Module Data Storage System
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_init(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "Module Data Storage: Initializing storage system");
    
    // Initialize all module data structures
    for (uint32_t i = 0; i < (uint32_t)MAX_MODULES; i++) {
        module_data_storage_initialize_module(i);
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "Module Data Storage: Initialized %d modules", MAX_MODULES);
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize Module Data Storage System
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "Module Data Storage: Deinitializing storage system");
    
    // Clean up mutex
    pthread_mutex_destroy(&g_storage_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "Module Data Storage: Deinitialized");
    return HAL_STATUS_OK;
}

/**
 * @brief Get Module Data Storage Status
 * @param active_modules Pointer to store number of active modules
 * @param total_modules Pointer to store total number of modules
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_status(int *active_modules, int *total_modules) {
    if (!active_modules || !total_modules) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    *total_modules = MAX_MODULES;
    *active_modules = 0;
    
    for (uint32_t i = 0; i < (uint32_t)MAX_MODULES; i++) {
        if (g_module_data[i].module_active) {
            (*active_modules)++;
        }
    }
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Update Module Telemetry Data
 * @param module_id Module ID (0-24)
 * @param telemetry Pointer to telemetry data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_update_telemetry(int module_id, const module_telemetry_storage_t *telemetry) {
    if (!module_data_storage_is_valid_module_id(module_id) || !telemetry) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    // Update telemetry data
    g_module_data[module_id].telemetry = *telemetry;
    g_module_data[module_id].telemetry.timestamp = (unsigned long)time(NULL);
    g_module_data[module_id].telemetry.data_valid = true;
    g_module_data[module_id].last_activity = (unsigned long)time(NULL);
    g_module_data[module_id].module_active = true;
    
    // Add to history
    module_telemetry_history_t *history = &g_module_telemetry_history[module_id];
    history->records[history->current_index] = *telemetry;
    history->records[history->current_index].timestamp = g_module_data[module_id].telemetry.timestamp;
    history->records[history->current_index].data_valid = true;
    
    history->current_index = (history->current_index + 1) % MAX_HISTORY_RECORDS;
    if (history->record_count < MAX_HISTORY_RECORDS) {
        history->record_count++;
    }
    
    history->newest_timestamp = g_module_data[module_id].telemetry.timestamp;
    if (history->record_count == 1) {
        history->oldest_timestamp = history->newest_timestamp;
    }
    
    // Cleanup old records if needed
    module_data_storage_cleanup_old_telemetry_records(module_id);
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "Module Data Storage: Updated telemetry for module %d", module_id);
    return HAL_STATUS_OK;
}

/**
 * @brief Get Module Telemetry Data
 * @param module_id Module ID (0-24)
 * @param telemetry Pointer to store telemetry data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_telemetry(int module_id, module_telemetry_storage_t *telemetry) {
    if (!module_data_storage_is_valid_module_id(module_id) || !telemetry) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    if (!g_module_data[module_id].module_active || !g_module_data[module_id].telemetry.data_valid) {
        pthread_mutex_unlock(&g_storage_mutex);
        return HAL_STATUS_NOT_FOUND;
    }
    
    *telemetry = g_module_data[module_id].telemetry;
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get Module Telemetry History
 * @param module_id Module ID (0-24)
 * @param max_records Maximum number of records to return
 * @param history Pointer to store history data
 * @param record_count Pointer to store actual record count
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_telemetry_history(int module_id, int max_records,
                                                      module_telemetry_storage_t *history, int *record_count) {
    if (!module_data_storage_is_valid_module_id(module_id) || !history || !record_count) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    module_telemetry_history_t *telemetry_history = &g_module_telemetry_history[module_id];
    
    if (telemetry_history->record_count == 0) {
        *record_count = 0;
        pthread_mutex_unlock(&g_storage_mutex);
        return HAL_STATUS_NOT_FOUND;
    }
    
    int records_to_copy = (max_records < telemetry_history->record_count) ? 
                         max_records : telemetry_history->record_count;
    
    // Copy records in chronological order (oldest first)
    int start_index = (telemetry_history->current_index - telemetry_history->record_count + MAX_HISTORY_RECORDS) % MAX_HISTORY_RECORDS;
    
    for (int i = 0; i < records_to_copy; i++) {
        int record_index = (start_index + i) % MAX_HISTORY_RECORDS;
        history[i] = telemetry_history->records[record_index];
    }
    
    *record_count = records_to_copy;
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Update Module Configuration Data
 * @param module_id Module ID (0-24)
 * @param config Pointer to configuration data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_update_config(int module_id, const module_config_storage_t *config) {
    if (!module_data_storage_is_valid_module_id(module_id) || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    g_module_data[module_id].config = *config;
    g_module_data[module_id].config.last_updated = (unsigned long)time(NULL);
    g_module_data[module_id].config.config_valid = true;
    g_module_data[module_id].last_activity = (unsigned long)time(NULL);
    g_module_data[module_id].module_active = true;
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "Module Data Storage: Updated config for module %d", module_id);
    return HAL_STATUS_OK;
}

/**
 * @brief Get Module Configuration Data
 * @param module_id Module ID (0-24)
 * @param config Pointer to store configuration data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_config(int module_id, module_config_storage_t *config) {
    if (!module_data_storage_is_valid_module_id(module_id) || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    if (!g_module_data[module_id].module_active || !g_module_data[module_id].config.config_valid) {
        pthread_mutex_unlock(&g_storage_mutex);
        return HAL_STATUS_NOT_FOUND;
    }
    
    *config = g_module_data[module_id].config;
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set Module Configuration Data
 * @param module_id Module ID (0-24)
 * @param config Pointer to configuration data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_set_config(int module_id, const module_config_storage_t *config) {
    if (!module_data_storage_is_valid_module_id(module_id) || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Update configuration
    hal_status_t result = module_data_storage_update_config(module_id, config);
    
    if (result == HAL_STATUS_OK) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "Module Data Storage: Configuration set for module %d", module_id);
    }
    
    return result;
}

/**
 * @brief Update Module Health Data
 * @param module_id Module ID (0-24)
 * @param health Pointer to health data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_update_health(int module_id, const module_health_storage_t *health) {
    if (!module_data_storage_is_valid_module_id(module_id) || !health) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    g_module_data[module_id].health = *health;
    g_module_data[module_id].health.last_updated = (unsigned long)time(NULL);
    g_module_data[module_id].health.health_valid = true;
    g_module_data[module_id].last_activity = (unsigned long)time(NULL);
    g_module_data[module_id].module_active = true;
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "Module Data Storage: Updated health for module %d", module_id);
    return HAL_STATUS_OK;
}

/**
 * @brief Get Module Health Data
 * @param module_id Module ID (0-24)
 * @param health Pointer to store health data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_health(int module_id, module_health_storage_t *health) {
    if (!module_data_storage_is_valid_module_id(module_id) || !health) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    if (!g_module_data[module_id].module_active || !g_module_data[module_id].health.health_valid) {
        pthread_mutex_unlock(&g_storage_mutex);
        return HAL_STATUS_NOT_FOUND;
    }
    
    *health = g_module_data[module_id].health;
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Add Module Command to History
 * @param module_id Module ID (0-24)
 * @param command Pointer to command data
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_add_command(int module_id, const module_command_storage_t *command) {
    if (!module_data_storage_is_valid_module_id(module_id) || !command) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    // Update last command
    g_module_data[module_id].last_command = *command;
    g_module_data[module_id].last_command.timestamp = (unsigned long)time(NULL);
    g_module_data[module_id].last_activity = (unsigned long)time(NULL);
    g_module_data[module_id].module_active = true;
    
    // Add to command history
    module_command_history_t *history = &g_module_command_history[module_id];
    history->records[history->current_index] = *command;
    history->records[history->current_index].timestamp = g_module_data[module_id].last_command.timestamp;
    
    history->current_index = (history->current_index + 1) % MAX_COMMAND_HISTORY;
    if (history->record_count < MAX_COMMAND_HISTORY) {
        history->record_count++;
    }
    
    history->newest_timestamp = g_module_data[module_id].last_command.timestamp;
    if (history->record_count == 1) {
        history->oldest_timestamp = history->newest_timestamp;
    }
    
    // Cleanup old records if needed
    module_data_storage_cleanup_old_command_records(module_id);
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "Module Data Storage: Added command for module %d", module_id);
    return HAL_STATUS_OK;
}

/**
 * @brief Get Module Command History
 * @param module_id Module ID (0-24)
 * @param max_records Maximum number of records to return
 * @param history Pointer to store history data
 * @param record_count Pointer to store actual record count
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_command_history(int module_id, int max_records,
                                                    module_command_storage_t *history, int *record_count) {
    if (!module_data_storage_is_valid_module_id(module_id) || !history || !record_count) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    module_command_history_t *command_history = &g_module_command_history[module_id];
    
    if (command_history->record_count == 0) {
        *record_count = 0;
        pthread_mutex_unlock(&g_storage_mutex);
        return HAL_STATUS_NOT_FOUND;
    }
    
    int records_to_copy = (max_records < command_history->record_count) ? 
                         max_records : command_history->record_count;
    
    // Copy records in chronological order (oldest first)
    int start_index = (command_history->current_index - command_history->record_count + MAX_COMMAND_HISTORY) % MAX_COMMAND_HISTORY;
    
    for (int i = 0; i < records_to_copy; i++) {
        int record_index = (start_index + i) % MAX_COMMAND_HISTORY;
        history[i] = command_history->records[record_index];
    }
    
    *record_count = records_to_copy;
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Check if Module is Active
 * @param module_id Module ID (0-24)
 * @return true if module is active, false otherwise
 */
bool module_data_storage_is_module_active(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return false;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    bool active = g_module_data[module_id].module_active;
    pthread_mutex_unlock(&g_storage_mutex);
    
    return active;
}

/**
 * @brief Check if Module Telemetry is Fresh
 * @param module_id Module ID (0-24)
 * @return true if telemetry is fresh, false otherwise
 */
bool module_data_storage_is_telemetry_fresh(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return false;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    if (!g_module_data[module_id].module_active || !g_module_data[module_id].telemetry.data_valid) {
        pthread_mutex_unlock(&g_storage_mutex);
        return false;
    }
    
    unsigned long current_time = (unsigned long)time(NULL);
    unsigned long data_age = current_time - g_module_data[module_id].telemetry.timestamp;
    bool fresh = (data_age * 1000) < DATA_FRESHNESS_TIMEOUT_MS;
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return fresh;
}

/**
 * @brief Check if Module Configuration is Valid
 * @param module_id Module ID (0-24)
 * @return true if configuration is valid, false otherwise
 */
bool module_data_storage_is_config_valid(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return false;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    bool valid = g_module_data[module_id].config.config_valid;
    pthread_mutex_unlock(&g_storage_mutex);
    
    return valid;
}

/**
 * @brief Check if Module Health is Valid
 * @param module_id Module ID (0-24)
 * @return true if health is valid, false otherwise
 */
bool module_data_storage_is_health_valid(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return false;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    bool valid = g_module_data[module_id].health.health_valid;
    pthread_mutex_unlock(&g_storage_mutex);
    
    return valid;
}

/**
 * @brief Cleanup Old Module Data
 * @param module_id Module ID (0-24)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_cleanup_old_data(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    // Cleanup old telemetry records
    module_data_storage_cleanup_old_telemetry_records(module_id);
    
    // Cleanup old command records
    module_data_storage_cleanup_old_command_records(module_id);
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "Module Data Storage: Cleaned up old data for module %d", module_id);
    return HAL_STATUS_OK;
}

/**
 * @brief Reset Module Data
 * @param module_id Module ID (0-24)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_reset_module_data(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    // Reset module data
    module_data_storage_initialize_module(module_id);
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "Module Data Storage: Reset data for module %d", module_id);
    return HAL_STATUS_OK;
}

/**
 * @brief Get Module Summary
 * @param module_id Module ID (0-24)
 * @param summary Pointer to store summary string
 * @param summary_size Size of summary buffer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_module_summary(int module_id, char *summary, size_t summary_size) {
    if (!module_data_storage_is_valid_module_id(module_id) || !summary || summary_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    module_data_t *data = &g_module_data[module_id];
    
    snprintf(summary, summary_size,
             "Module %d: %s, Active: %s, Telemetry: %s, Config: %s, Health: %s",
             module_id,
             data->module_name,
             data->module_active ? "Yes" : "No",
             data->telemetry.data_valid ? "Valid" : "Invalid",
             data->config.config_valid ? "Valid" : "Invalid",
             data->health.health_valid ? "Valid" : "Invalid");
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get Module Statistics
 * @param module_id Module ID (0-24)
 * @param stats_json Pointer to store statistics JSON
 * @param stats_size Size of statistics buffer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_statistics(int module_id, char *stats_json, size_t stats_size) {
    if (!module_data_storage_is_valid_module_id(module_id) || !stats_json || stats_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    module_data_t *data = &g_module_data[module_id];
    module_telemetry_history_t *telemetry_history = &g_module_telemetry_history[module_id];
    module_command_history_t *command_history = &g_module_command_history[module_id];
    
    snprintf(stats_json, stats_size,
             "{\"module_id\":%d,\"module_name\":\"%s\",\"active\":%s,"
             "\"telemetry_records\":%d,\"command_records\":%d,"
             "\"last_activity\":%lu,\"uptime_seconds\":%lu}",
             module_id,
             data->module_name,
             data->module_active ? "true" : "false",
             telemetry_history->record_count,
             command_history->record_count,
             data->last_activity,
             data->health.uptime_seconds);
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get System Statistics
 * @param stats_json Pointer to store statistics JSON
 * @param stats_size Size of statistics buffer
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t module_data_storage_get_system_statistics(char *stats_json, size_t stats_size) {
    if (!stats_json || stats_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_storage_mutex);
    
    int active_modules = 0;
    int total_telemetry_records = 0;
    int total_command_records = 0;
    
    for (uint32_t i = 0; i < (uint32_t)MAX_MODULES; i++) {
        if (g_module_data[i].module_active) {
            active_modules++;
        }
        total_telemetry_records += g_module_telemetry_history[i].record_count;
        total_command_records += g_module_command_history[i].record_count;
    }
    
    snprintf(stats_json, stats_size,
             "{\"total_modules\":%d,\"active_modules\":%d,"
             "\"total_telemetry_records\":%d,\"total_command_records\":%d,"
             "\"storage_initialized\":true}",
             MAX_MODULES,
             active_modules,
             total_telemetry_records,
             total_command_records);
    
    pthread_mutex_unlock(&g_storage_mutex);
    
    return HAL_STATUS_OK;
}

// Private Functions

/**
 * @brief Initialize Module Data Structure
 * @param module_id Module ID (0-24)
 */
static void module_data_storage_initialize_module(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return;
    }
    
    // Initialize module data
    memset(&g_module_data[module_id], 0, sizeof(module_data_t));
    g_module_data[module_id].module_id = module_id;
    snprintf(g_module_data[module_id].module_name, sizeof(g_module_data[module_id].module_name), "Module_%d", module_id);
    
    // Initialize telemetry history
    memset(&g_module_telemetry_history[module_id], 0, sizeof(module_telemetry_history_t));
    
    // Initialize command history
    memset(&g_module_command_history[module_id], 0, sizeof(module_command_history_t));
}

/**
 * @brief Check if Module ID is Valid
 * @param module_id Module ID to check
 * @return true if valid, false otherwise
 */
static bool module_data_storage_is_valid_module_id(int module_id) {
    return (module_id >= 0 && (uint32_t)module_id < (uint32_t)MAX_MODULES);
}

/**
 * @brief Cleanup Old Telemetry Records
 * @param module_id Module ID (0-24)
 */
static void module_data_storage_cleanup_old_telemetry_records(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return;
    }
    
    module_telemetry_history_t *history = &g_module_telemetry_history[module_id];
    unsigned long current_time = (unsigned long)time(NULL);
    unsigned long cleanup_threshold = current_time - (24 * 60 * 60); // 24 hours ago
    
    // Remove old records (older than 24 hours)
    int removed_count = 0;
    for (int i = 0; i < history->record_count; i++) {
        int record_index = (history->current_index - history->record_count + i + MAX_HISTORY_RECORDS) % MAX_HISTORY_RECORDS;
        if (history->records[record_index].timestamp < cleanup_threshold) {
            memset(&history->records[record_index], 0, sizeof(module_telemetry_storage_t));
            removed_count++;
        }
    }
    
    if (removed_count > 0) {
        history->record_count -= removed_count;
        hal_log_message(HAL_LOG_LEVEL_DEBUG, "Module Data Storage: Cleaned up %d old telemetry records for module %d", removed_count, module_id);
    }
}

/**
 * @brief Cleanup Old Command Records
 * @param module_id Module ID (0-24)
 */
static void module_data_storage_cleanup_old_command_records(int module_id) {
    if (!module_data_storage_is_valid_module_id(module_id)) {
        return;
    }
    
    module_command_history_t *history = &g_module_command_history[module_id];
    unsigned long current_time = (unsigned long)time(NULL);
    unsigned long cleanup_threshold = current_time - (7 * 24 * 60 * 60); // 7 days ago
    
    // Remove old records (older than 7 days)
    int removed_count = 0;
    for (int i = 0; i < history->record_count; i++) {
        int record_index = (history->current_index - history->record_count + i + MAX_COMMAND_HISTORY) % MAX_COMMAND_HISTORY;
        if (history->records[record_index].timestamp < cleanup_threshold) {
            memset(&history->records[record_index], 0, sizeof(module_command_storage_t));
            removed_count++;
        }
    }
    
    if (removed_count > 0) {
        history->record_count -= removed_count;
        hal_log_message(HAL_LOG_LEVEL_DEBUG, "Module Data Storage: Cleaned up %d old command records for module %d", removed_count, module_id);
    }
}
