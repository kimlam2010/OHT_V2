/**
 * @file api_config_json.c
 * @brief API Configuration JSON Serialization for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.1
 */

#include "api_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief Serialize configuration response to JSON
 * @param response Configuration response
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_config_response_json(const api_config_response_t *response, char *buffer, size_t buffer_size) {
    if (response == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = 0;
    
    // Start JSON object
    offset += snprintf(buffer + offset, buffer_size - offset,
        "{\n"
        "  \"success\": %s,\n"
        "  \"message\": \"%s\",\n"
        "  \"parameter_count\": %u,\n"
        "  \"parameters\": [\n",
        response->success ? "true" : "false",
        response->message,
        response->parameter_count
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    // Serialize parameters
    for (uint32_t i = 0; i < response->parameter_count; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, buffer_size - offset, ",\n");
        }
        
        offset += snprintf(buffer + offset, buffer_size - offset, "    {\n");
        
        // Serialize parameter fields
        offset += snprintf(buffer + offset, buffer_size - offset,
            "      \"key\": \"%s\",\n"
            "      \"description\": \"%s\",\n"
            "      \"type\": \"%s\",\n"
            "      \"category\": \"%s\",\n"
            "      \"read_only\": %s,\n"
            "      \"requires_restart\": %s,\n"
            "      \"default_value\": \"%s\",\n"
            "      \"current_value\": \"%s\",\n"
            "      \"min_value\": \"%s\",\n"
            "      \"max_value\": \"%s\",\n"
            "      \"valid_values\": \"%s\"\n",
            response->parameters[i].key,
            response->parameters[i].description,
            api_config_get_type_name(response->parameters[i].type),
            api_config_get_category_name(response->parameters[i].category),
            response->parameters[i].read_only ? "true" : "false",
            response->parameters[i].requires_restart ? "true" : "false",
            response->parameters[i].default_value,
            response->parameters[i].current_value,
            response->parameters[i].min_value,
            response->parameters[i].max_value,
            response->parameters[i].valid_values
        );
        
        offset += snprintf(buffer + offset, buffer_size - offset, "    }");
        
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

/**
 * @brief Serialize configuration parameter to JSON
 * @param parameter Configuration parameter
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_config_parameter_json(const api_config_parameter_t *parameter, char *buffer, size_t buffer_size) {
    if (parameter == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = snprintf(buffer, buffer_size,
        "{\n"
        "  \"key\": \"%s\",\n"
        "  \"description\": \"%s\",\n"
        "  \"type\": \"%s\",\n"
        "  \"category\": \"%s\",\n"
        "  \"read_only\": %s,\n"
        "  \"requires_restart\": %s,\n"
        "  \"default_value\": \"%s\",\n"
        "  \"current_value\": \"%s\",\n"
        "  \"min_value\": \"%s\",\n"
        "  \"max_value\": \"%s\",\n"
        "  \"valid_values\": \"%s\"\n"
        "}\n",
        parameter->key,
        parameter->description,
        api_config_get_type_name(parameter->type),
        api_config_get_category_name(parameter->category),
        parameter->read_only ? "true" : "false",
        parameter->requires_restart ? "true" : "false",
        parameter->default_value,
        parameter->current_value,
        parameter->min_value,
        parameter->max_value,
        parameter->valid_values
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize configuration categories to JSON
 * @param categories Configuration categories
 * @param count Number of categories
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_config_categories_json(const api_config_category_info_t *categories, uint32_t count, char *buffer, size_t buffer_size) {
    if (categories == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = 0;
    
    // Start JSON object
    offset += snprintf(buffer + offset, buffer_size - offset,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Configuration categories retrieved successfully\",\n"
        "  \"category_count\": %u,\n"
        "  \"categories\": [\n",
        count
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    // Serialize categories
    for (uint32_t i = 0; i < count; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, buffer_size - offset, ",\n");
        }
        
        offset += snprintf(buffer + offset, buffer_size - offset,
            "    {\n"
            "      \"category\": \"%s\",\n"
            "      \"name\": \"%s\",\n"
            "      \"description\": \"%s\",\n"
            "      \"parameter_count\": %u,\n"
            "      \"parameters\": [\n",
            api_config_get_category_name(categories[i].category),
            categories[i].name,
            categories[i].description,
            categories[i].parameter_count
        );
        
        if (offset < 0 || (size_t)offset >= buffer_size) {
            return HAL_STATUS_ERROR;
        }
        
        // Serialize parameters for this category
        for (uint32_t j = 0; j < categories[i].parameter_count; j++) {
            if (j > 0) {
                offset += snprintf(buffer + offset, buffer_size - offset, ",\n");
            }
            
            offset += snprintf(buffer + offset, buffer_size - offset,
                "        {\n"
                "          \"key\": \"%s\",\n"
                "          \"description\": \"%s\",\n"
                "          \"type\": \"%s\",\n"
                "          \"read_only\": %s,\n"
                "          \"requires_restart\": %s,\n"
                "          \"default_value\": \"%s\",\n"
                "          \"current_value\": \"%s\"\n"
                "        }",
                categories[i].parameters[j].key,
                categories[i].parameters[j].description,
                api_config_get_type_name(categories[i].parameters[j].type),
                categories[i].parameters[j].read_only ? "true" : "false",
                categories[i].parameters[j].requires_restart ? "true" : "false",
                categories[i].parameters[j].default_value,
                categories[i].parameters[j].current_value
            );
            
            if (offset < 0 || (size_t)offset >= buffer_size) {
                return HAL_STATUS_ERROR;
            }
        }
        
        offset += snprintf(buffer + offset, buffer_size - offset,
            "\n      ]\n"
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

/**
 * @brief Serialize configuration backup to JSON
 * @param backup Configuration backup
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_config_backup_json(const api_config_backup_t *backup, char *buffer, size_t buffer_size) {
    if (backup == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int offset = 0;
    
    // Start JSON object
    offset += snprintf(buffer + offset, buffer_size - offset,
        "{\n"
        "  \"success\": true,\n"
        "  \"message\": \"Configuration backup created successfully\",\n"
        "  \"backup\": {\n"
        "    \"timestamp\": \"%s\",\n"
        "    \"version\": \"%s\",\n"
        "    \"parameter_count\": %u,\n"
        "    \"parameters\": [\n",
        backup->timestamp,
        backup->version,
        backup->parameter_count
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    // Serialize backup parameters
    for (uint32_t i = 0; i < backup->parameter_count; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, buffer_size - offset, ",\n");
        }
        
        offset += snprintf(buffer + offset, buffer_size - offset,
            "      {\n"
            "        \"key\": \"%s\",\n"
            "        \"value\": \"%s\"\n"
            "      }",
            backup->parameters[i].key,
            backup->parameters[i].current_value
        );
        
        if (offset < 0 || (size_t)offset >= buffer_size) {
            return HAL_STATUS_ERROR;
        }
    }
    
    // End JSON object
    offset += snprintf(buffer + offset, buffer_size - offset,
        "\n    ]\n"
        "  }\n"
        "}\n"
    );
    
    if (offset < 0 || (size_t)offset >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Parse configuration value
 * @param value String value
 * @param type Configuration type
 * @param result Parsed result buffer
 * @param result_size Result buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_parse_value(const char *value, api_config_type_t type, void *result, size_t result_size) {
    if (value == NULL || result == NULL || result_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    switch (type) {
        case API_CONFIG_TYPE_STRING:
            strncpy((char*)result, value, result_size - 1);
            ((char*)result)[result_size - 1] = '\0';
            break;
            
        case API_CONFIG_TYPE_INTEGER:
            if (result_size >= sizeof(int)) {
                *(int*)result = atoi(value);
            } else {
                return HAL_STATUS_ERROR;
            }
            break;
            
        case API_CONFIG_TYPE_FLOAT:
            if (result_size >= sizeof(float)) {
                *(float*)result = (float)atof(value);
            } else {
                return HAL_STATUS_ERROR;
            }
            break;
            
        case API_CONFIG_TYPE_BOOLEAN:
            if (result_size >= sizeof(bool)) {
                *(bool*)result = (strcmp(value, "true") == 0);
            } else {
                return HAL_STATUS_ERROR;
            }
            break;
            
        case API_CONFIG_TYPE_JSON:
            strncpy((char*)result, value, result_size - 1);
            ((char*)result)[result_size - 1] = '\0';
            break;
            
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Format configuration value
 * @param value Value to format
 * @param type Configuration type
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_format_value(const void *value, api_config_type_t type, char *buffer, size_t buffer_size) {
    if (value == NULL || buffer == NULL || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    switch (type) {
        case API_CONFIG_TYPE_STRING:
        case API_CONFIG_TYPE_JSON:
            strncpy(buffer, (const char*)value, buffer_size - 1);
            buffer[buffer_size - 1] = '\0';
            break;
            
        case API_CONFIG_TYPE_INTEGER:
            snprintf(buffer, buffer_size, "%d", *(const int*)value);
            break;
            
        case API_CONFIG_TYPE_FLOAT:
            snprintf(buffer, buffer_size, "%.2f", *(const float*)value);
            break;
            
        case API_CONFIG_TYPE_BOOLEAN:
            strncpy(buffer, *(const bool*)value ? "true" : "false", buffer_size - 1);
            buffer[buffer_size - 1] = '\0';
            break;
            
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}
