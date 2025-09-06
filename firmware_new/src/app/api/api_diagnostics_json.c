/**
 * @file api_diagnostics_json.c
 * @brief API Diagnostics JSON Serialization for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.3
 */

#include "api_diagnostics.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Serialize system diagnostics to JSON
 * @param diagnostics System diagnostics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_system_diagnostics_json(const api_system_diagnostics_t *diagnostics, char *buffer, size_t buffer_size) {
    if (diagnostics == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = snprintf(buffer, buffer_size,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"System diagnostics retrieved successfully\",\n"
        "  \"diagnostics\": {\n"
        "    \"uptime_ms\": %lu,\n"
        "    \"memory_usage_kb\": %u,\n"
        "    \"memory_total_kb\": %u,\n"
        "    \"memory_usage_percent\": %.2f,\n"
        "    \"cpu_usage_percent\": %.2f,\n"
        "    \"task_count\": %u,\n"
        "    \"error_count\": %u,\n"
        "    \"warning_count\": %u,\n"
        "    \"network_connections\": %u,\n"
        "    \"api_requests_total\": %u,\n"
        "    \"api_requests_failed\": %u,\n"
        "    \"api_response_time_avg\": %lu,\n"
        "    \"timestamp\": %lu\n"
        "  }\n"
        "}",
        diagnostics->uptime_ms,
        diagnostics->memory_usage_kb,
        diagnostics->memory_total_kb,
        diagnostics->memory_total_kb > 0 ? (float)diagnostics->memory_usage_kb / diagnostics->memory_total_kb * 100.0f : 0.0f,
        diagnostics->cpu_usage_percent,
        diagnostics->task_count,
        diagnostics->error_count,
        diagnostics->warning_count,
        diagnostics->network_connections,
        diagnostics->api_requests_total,
        diagnostics->api_requests_failed,
        diagnostics->api_response_time_avg,
        hal_get_timestamp_ms()
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize module diagnostics to JSON
 * @param diagnostics Module diagnostics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_module_diagnostics_json(const api_module_diagnostics_t *diagnostics, char *buffer, size_t buffer_size) {
    if (diagnostics == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = snprintf(buffer, buffer_size,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Module diagnostics retrieved successfully\",\n"
        "  \"diagnostics\": {\n"
        "    \"module_name\": \"%s\",\n"
        "    \"online\": %s,\n"
        "    \"last_heartbeat\": %lu,\n"
        "    \"error_count\": %u,\n"
        "    \"warning_count\": %u,\n"
        "    \"message_count\": %u,\n"
        "    \"message_errors\": %u,\n"
        "    \"response_time_avg\": %.2f,\n"
        "    \"status_message\": \"%s\",\n"
        "    \"timestamp\": %lu\n"
        "  }\n"
        "}",
        diagnostics->module_name,
        diagnostics->online ? "true" : "false",
        diagnostics->last_heartbeat,
        diagnostics->error_count,
        diagnostics->warning_count,
        diagnostics->message_count,
        diagnostics->message_errors,
        diagnostics->response_time_avg,
        diagnostics->status_message,
        hal_get_timestamp_ms()
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize log entry to JSON
 * @param entry Log entry
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_log_entry_json(const api_log_entry_t *entry, char *buffer, size_t buffer_size) {
    if (entry == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = snprintf(buffer, buffer_size,
        "{\n"
        "  \"timestamp\": %lu,\n"
        "  \"level\": \"%s\",\n"
        "  \"category\": \"%s\",\n"
        "  \"source\": \"%s\",\n"
        "  \"line_number\": %u,\n"
        "  \"message\": \"%s\",\n"
        "  \"details\": \"%s\"\n"
        "}",
        entry->timestamp,
        api_diagnostics_get_log_level_name(entry->level),
        api_diagnostics_get_log_category_name(entry->category),
        entry->source,
        entry->line_number,
        entry->message,
        entry->details
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize diagnostics statistics to JSON
 * @param stats Diagnostics statistics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_diagnostics_stats_json(const api_diagnostics_stats_t *stats, char *buffer, size_t buffer_size) {
    if (stats == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = 0;
    
    // Start JSON object
    offset += snprintf(buffer + offset, buffer_size - offset,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Diagnostics statistics retrieved successfully\",\n"
        "  \"stats\": {\n"
        "    \"total_log_entries\": %u,\n"
        "    \"diagnostics_requests\": %u,\n"
        "    \"last_diagnostics_time\": %lu,\n"
        "    \"error_rate_per_hour\": %u,\n"
        "    \"warning_rate_per_hour\": %u,\n"
        "    \"log_entries_by_level\": {\n",
        stats->total_log_entries,
        stats->diagnostics_requests,
        stats->last_diagnostics_time,
        stats->error_rate_per_hour,
        stats->warning_rate_per_hour
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    // Serialize log entries by level
    for (int i = 0; i < API_LOG_LEVEL_MAX; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, buffer_size - offset, ",\n");
        }
        offset += snprintf(buffer + offset, buffer_size - offset,
            "      \"%s\": %u",
            api_diagnostics_get_log_level_name((api_log_level_t)i),
            stats->log_entries_by_level[i]
        );
        
        if (offset < 0 || (size_t)offset >= buffer_size) {
            return HAL_STATUS_ERROR;
        }
    }
    
    offset += snprintf(buffer + offset, buffer_size - offset,
        "\n    },\n"
        "    \"log_entries_by_category\": {\n"
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    // Serialize log entries by category
    for (int i = 0; i < API_LOG_CATEGORY_MAX; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, buffer_size - offset, ",\n");
        }
        offset += snprintf(buffer + offset, buffer_size - offset,
            "      \"%s\": %u",
            api_diagnostics_get_log_category_name((api_log_category_t)i),
            stats->log_entries_by_category[i]
        );
        
        if (offset < 0 || (size_t)offset >= buffer_size) {
            return HAL_STATUS_ERROR;
        }
    }
    
    // End JSON object
    offset += snprintf(buffer + offset, buffer_size - offset,
        "\n    }\n"
        "  }\n"
        "}\n"
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}
