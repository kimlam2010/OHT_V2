/**
 * @file register_json_serializer.c
 * @brief JSON Serializer Implementation for Register Metadata
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 * @task Issue #203 - Register Metadata API Implementation
 */

#include "register_json_serializer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// ENUM TO STRING CONVERTERS IMPLEMENTATION
// ============================================================================

const char* register_mode_to_string(uint8_t mode) {
    switch (mode) {
        case REG_MODE_READ_ONLY:   return "READ";
        case REG_MODE_WRITE_ONLY:  return "WRITE";
        case REG_MODE_READ_WRITE:  return "READ_WRITE";
        case REG_MODE_WRITE_ONCE:  return "WRITE_ONCE";
        default:                    return "UNKNOWN";
    }
}

const char* register_data_type_to_string(uint8_t data_type) {
    switch (data_type) {
        case REG_DATA_TYPE_STRING:  return "STRING";
        case REG_DATA_TYPE_UINT8:   return "UINT8";
        case REG_DATA_TYPE_UINT16:  return "UINT16";
        case REG_DATA_TYPE_UINT32:  return "UINT32";
        case REG_DATA_TYPE_INT8:    return "INT8";
        case REG_DATA_TYPE_INT16:   return "INT16";
        case REG_DATA_TYPE_INT32:   return "INT32";
        case REG_DATA_TYPE_FLOAT:   return "FLOAT";
        default:                     return "UNKNOWN";
    }
}

const char* register_access_level_to_string(uint8_t access_level) {
    switch (access_level) {
        case REG_ACCESS_USER:    return "USER";
        case REG_ACCESS_ADMIN:   return "ADMIN";
        case REG_ACCESS_SYSTEM:  return "SYSTEM";
        default:                  return "UNKNOWN";
    }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Escape JSON string (basic implementation)
 */
static void json_escape_string(char* dest, const char* src, size_t dest_size) {
    if (src == NULL || dest == NULL || dest_size == 0) {
        if (dest && dest_size > 0) dest[0] = '\0';
        return;
    }
    
    size_t j = 0;
    for (size_t i = 0; src[i] != '\0' && j < dest_size - 1; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            if (j < dest_size - 2) {
                dest[j++] = '\\';
            }
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

// ============================================================================
// JSON SERIALIZERS IMPLEMENTATION
// ============================================================================

char* serialize_register_to_json(const register_info_t* reg) {
    if (reg == NULL) {
        printf("[JSON_SER] Error: NULL register pointer\n");
        return NULL;
    }
    
    // Allocate buffer (1KB should be enough for single register)
    size_t buffer_size = 1024;
    char* json = (char*)malloc(buffer_size);
    if (json == NULL) {
        printf("[JSON_SER] Error: Failed to allocate buffer\n");
        return NULL;
    }
    
    // Escape strings
    char desc_escaped[256] = {0};
    char unit_escaped[64] = {0};
    
    if (reg->description) {
        json_escape_string(desc_escaped, reg->description, sizeof(desc_escaped));
    }
    if (reg->unit) {
        json_escape_string(unit_escaped, reg->unit, sizeof(unit_escaped));
    }
    
    // Serialize to JSON
    int written = snprintf(json, buffer_size,
        "{\n"
        "  \"address\": \"0x%04X\",\n"
        "  \"mode\": \"%s\",\n"
        "  \"data_type\": \"%s\",\n"
        "  \"access_level\": \"%s\",\n"
        "  \"is_safe_register\": %s,\n"
        "  \"description\": \"%s\",\n"
        "  \"unit\": \"%s\",\n"
        "  \"min_value\": %u,\n"
        "  \"max_value\": %u,\n"
        "  \"default_value\": %u\n"
        "}",
        reg->address,
        register_mode_to_string(reg->mode),
        register_data_type_to_string(reg->data_type),
        register_access_level_to_string(reg->access_level),
        reg->is_safe_register ? "true" : "false",
        desc_escaped,
        unit_escaped,
        reg->min_value,
        reg->max_value,
        reg->default_value
    );
    
    if (written < 0 || (size_t)written >= buffer_size) {
        printf("[JSON_SER] Error: Buffer overflow during serialization\n");
        free(json);
        return NULL;
    }
    
    return json;
}

char* serialize_register_list_to_json(const register_list_response_t* response) {
    if (response == NULL || !response->valid) {
        printf("[JSON_SER] Error: Invalid response structure\n");
        return NULL;
    }
    
    // Estimate buffer size (500 bytes per register + 1KB overhead)
    size_t buffer_size = estimate_json_buffer_size(response->count);
    char* json = (char*)malloc(buffer_size);
    if (json == NULL) {
        printf("[JSON_SER] Error: Failed to allocate buffer (%zu bytes)\n", buffer_size);
        return NULL;
    }
    
    // Start JSON object
    int pos = snprintf(json, buffer_size,
        "{\n"
        "  \"success\": true,\n"
        "  \"data\": {\n"
        "    \"module_addr\": %u,\n"
        "    \"module_name\": \"%s\",\n"
        "    \"register_count\": %u,\n"
        "    \"registers\": [\n",
        response->module_addr,
        response->module_name ? response->module_name : "Unknown",
        response->count
    );
    
    if (pos < 0 || (size_t)pos >= buffer_size) {
        printf("[JSON_SER] Error: Buffer overflow in header\n");
        free(json);
        return NULL;
    }
    
    // Serialize each register
    for (uint16_t i = 0; i < response->count; i++) {
        const register_info_t* reg = &response->registers[i];
        
        // Escape strings
        char desc_escaped[256] = {0};
        char unit_escaped[64] = {0};
        
        if (reg->description) {
            json_escape_string(desc_escaped, reg->description, sizeof(desc_escaped));
        }
        if (reg->unit) {
            json_escape_string(unit_escaped, reg->unit, sizeof(unit_escaped));
        }
        
        // Add register object
        int written = snprintf(json + pos, buffer_size - pos,
            "      {\n"
            "        \"address\": \"0x%04X\",\n"
            "        \"mode\": \"%s\",\n"
            "        \"data_type\": \"%s\",\n"
            "        \"access_level\": \"%s\",\n"
            "        \"is_safe_register\": %s,\n"
            "        \"description\": \"%s\",\n"
            "        \"unit\": \"%s\",\n"
            "        \"min_value\": %u,\n"
            "        \"max_value\": %u,\n"
            "        \"default_value\": %u\n"
            "      }%s\n",
            reg->address,
            register_mode_to_string(reg->mode),
            register_data_type_to_string(reg->data_type),
            register_access_level_to_string(reg->access_level),
            reg->is_safe_register ? "true" : "false",
            desc_escaped,
            unit_escaped,
            reg->min_value,
            reg->max_value,
            reg->default_value,
            (i < response->count - 1) ? "," : ""  // No comma for last item
        );
        
        if (written < 0 || (size_t)written >= buffer_size - pos) {
            printf("[JSON_SER] Error: Buffer overflow at register %u\n", i);
            free(json);
            return NULL;
        }
        
        pos += written;
    }
    
    // Close JSON object
    int written = snprintf(json + pos, buffer_size - pos,
        "    ]\n"
        "  }\n"
        "}\n"
    );
    
    if (written < 0 || (size_t)written >= buffer_size - pos) {
        printf("[JSON_SER] Error: Buffer overflow in footer\n");
        free(json);
        return NULL;
    }
    
    printf("[JSON_SER] Successfully serialized %u registers (%d bytes)\n", 
           response->count, pos + written);
    
    return json;
}

char* create_error_json(const char* error_message) {
    size_t buffer_size = 512;
    char* json = (char*)malloc(buffer_size);
    if (json == NULL) {
        return NULL;
    }
    
    char msg_escaped[256] = {0};
    if (error_message) {
        json_escape_string(msg_escaped, error_message, sizeof(msg_escaped));
    }
    
    snprintf(json, buffer_size,
        "{\n"
        "  \"success\": false,\n"
        "  \"error\": \"%s\"\n"
        "}\n",
        msg_escaped
    );
    
    return json;
}

size_t estimate_json_buffer_size(uint16_t register_count) {
    // ~500 bytes per register + 1KB overhead
    return (register_count * 500) + 1024;
}

