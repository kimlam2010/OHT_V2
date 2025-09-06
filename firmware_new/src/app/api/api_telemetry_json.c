/**
 * @file api_telemetry_json.c
 * @brief API Telemetry JSON Serialization for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.2
 */

#include "api_telemetry.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Serialize telemetry data to JSON
 * @param data Telemetry data
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_telemetry_data_json(const api_telemetry_data_t *data, char *buffer, size_t buffer_size) {
    if (data == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = snprintf(buffer, buffer_size,
        "{\n"
        "  \"timestamp\": %lu,\n"
        "  \"type\": \"%s\",\n"
        "  \"source\": \"%s\",\n"
        "  \"data_size\": %u,\n"
        "  \"data\": %s\n"
        "}",
        data->timestamp,
        api_telemetry_get_type_name(data->type),
        data->source,
        data->data_size,
        data->data
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize telemetry statistics to JSON
 * @param stats Telemetry statistics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_telemetry_stats_json(const api_telemetry_stats_t *stats, char *buffer, size_t buffer_size) {
    if (stats == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = snprintf(buffer, buffer_size,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Telemetry statistics retrieved successfully\",\n"
        "  \"stats\": {\n"
        "    \"total_clients\": %u,\n"
        "    \"active_clients\": %u,\n"
        "    \"total_messages_sent\": %u,\n"
        "    \"total_messages_failed\": %u,\n"
        "    \"total_bytes_sent\": %lu,\n"
        "    \"uptime_ms\": %lu,\n"
        "    \"error_count\": %u\n"
        "  }\n"
        "}",
        stats->total_clients,
        stats->active_clients,
        stats->total_messages_sent,
        stats->total_messages_failed,
        stats->total_bytes_sent,
        stats->uptime_ms,
        stats->error_count
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize telemetry clients to JSON
 * @param clients Telemetry clients
 * @param count Number of clients
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_telemetry_clients_json(const api_telemetry_client_t *clients, uint32_t count, char *buffer, size_t buffer_size) {
    if (clients == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = 0;
    
    // Start JSON object
    offset += snprintf(buffer + offset, buffer_size - offset,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Telemetry clients retrieved successfully\",\n"
        "  \"client_count\": %u,\n"
        "  \"clients\": [\n",
        count
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    // Serialize clients
    for (uint32_t i = 0; i < count; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, buffer_size - offset, ",\n");
        }
        
        offset += snprintf(buffer + offset, buffer_size - offset,
            "    {\n"
            "      \"client_id\": %u,\n"
            "      \"socket_fd\": %d,\n"
            "      \"connected\": %s,\n"
            "      \"last_activity\": %lu,\n"
            "      \"messages_sent\": %u,\n"
            "      \"messages_failed\": %u,\n"
            "      \"subscribed_count\": %u,\n"
            "      \"subscribed_types\": [",
            clients[i].client_id,
            clients[i].socket_fd,
            clients[i].connected ? "true" : "false",
            clients[i].last_activity,
            clients[i].messages_sent,
            clients[i].messages_failed,
            clients[i].subscribed_count
        );
        
        if (offset < 0 || (size_t)offset >= buffer_size) {
            return HAL_STATUS_ERROR;
        }
        
        // Serialize subscribed types
        for (uint32_t j = 0; j < clients[i].subscribed_count; j++) {
            if (j > 0) {
                offset += snprintf(buffer + offset, buffer_size - offset, ", ");
            }
            offset += snprintf(buffer + offset, buffer_size - offset, "\"%s\"", 
                             api_telemetry_get_type_name(clients[i].subscribed_types[j]));
            
            if (offset < 0 || (size_t)offset >= buffer_size) {
                return HAL_STATUS_ERROR;
            }
        }
        
        offset += snprintf(buffer + offset, buffer_size - offset,
            "]\n"
            "    }"
        );
        
        if (offset < 0 || (size_t)offset >= buffer_size) {
            return HAL_STATUS_ERROR;
        }
    }
    
    // End JSON object
    offset += snprintf(buffer + offset, buffer_size - offset,
        "\n  ]\n"
        "}\n"
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}
