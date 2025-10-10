/**
 * @file register_data_api.c
 * @brief Register Data API Implementation - Complete API with runtime values
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include "register_data_api.h"
#include "../../hal/register/register_info.h"
#include "../storage/register_value_cache.h"
#include "communication_manager.h"
#include "../utils/register_json_serializer.h"
#include "../domain/module_management/module_polling_manager.h"
#include "../domain/module_management/module_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Helper functions
static int parse_module_addr_from_path(const char *path, uint8_t *addr_out);
static int parse_register_addr_from_path(const char *path, uint16_t *reg_addr_out);
static char* format_timestamp(uint64_t timestamp_ms);

// GET /api/v1/modules - List online modules
int api_get_modules_list(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    (void)request;
    
    printf("[DATA-API] GET /api/v1/modules\n");
    
    // Build JSON response with online modules
    char *json = malloc(4096);
    if (!json) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
                                                 "Memory allocation failed");
    }
    
    int pos = snprintf(json, 4096, "{\"success\":true,\"modules\":[");
    
    int module_count = 0;
    
    // Iterate through all possible module addresses
    for (int addr = 0; addr <= 0xFF; addr++) {
        module_polling_info_t info;
        hal_status_t status = module_polling_manager_get_module_info((uint8_t)addr, &info);
        
        if (status == HAL_STATUS_OK && info.is_online) {
            const char *type_name = "Unknown";
            switch (info.type) {
                case MODULE_TYPE_POWER: type_name = "Power"; break;
                case MODULE_TYPE_SAFETY: type_name = "Safety"; break;
                case MODULE_TYPE_TRAVEL_MOTOR: type_name = "Travel Motor"; break;
                case MODULE_TYPE_DOCK: type_name = "Dock"; break;
                default: break;
            }
            
            uint64_t last_update = 0;
            register_cache_get_last_update((uint8_t)addr, &last_update);
            
            if (module_count > 0) {
                pos += snprintf(json + pos, 4096 - pos, ",");
            }
            
            pos += snprintf(json + pos, 4096 - pos,
                           "{\"address\":%d,\"type\":\"%s\",\"online\":true,\"last_seen\":\"%s\"}",
                           addr, type_name, format_timestamp(last_update));
            
            module_count++;
        }
    }
    
    pos += snprintf(json + pos, 4096 - pos, "]}");
    
    response->status_code = 200;
    response->body = json;
    response->body_length = strlen(json);
    
    printf("[DATA-API] Found %d online modules\n", module_count);
    return 0;
}

// GET /api/v1/modules/{addr}/data - Get module data with values
int api_get_module_data(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    printf("[DATA-API] GET /api/v1/modules/{addr}/data: %s\n", request->path);
    
    // Parse module address from path
    uint8_t module_addr;
    if (parse_module_addr_from_path(request->path, &module_addr) != 0) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST,
                                                 "Invalid module address in path");
    }
    
    printf("[DATA-API] Module address: 0x%02X\n", module_addr);
    
    // Check if module is online
    module_polling_info_t info;
    hal_status_t poll_status = module_polling_manager_get_module_info(module_addr, &info);
    
    if (poll_status != HAL_STATUS_OK || !info.is_online) {
        char error_json[256];
        snprintf(error_json, sizeof(error_json),
                "{\"success\":false,\"error\":\"Module 0x%02X is offline\",\"online\":false}",
                module_addr);
        response->status_code = 404;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }
    
    // Get register metadata
    const register_list_response_t *reg_list = get_module_register_list_api(module_addr);
    if (!reg_list || !reg_list->valid) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_NOT_FOUND,
                                                 "Module not found or no registers defined");
    }
    
    // Get cached register values
    register_value_entry_t cached_values[MAX_CACHED_REGISTERS_PER_MODULE];
    uint16_t cached_count = 0;
    register_cache_get_all(module_addr, cached_values, MAX_CACHED_REGISTERS_PER_MODULE, &cached_count);
    
    // Build JSON response combining metadata + values
    char *json = malloc(65536);  // Large buffer for many registers
    if (!json) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
                                                 "Memory allocation failed");
    }
    
    int pos = snprintf(json, 65536,
                      "{\"success\":true,\"data\":{\"module_addr\":%d,\"module_name\":\"%s\","
                      "\"online\":true,\"register_count\":%d,\"registers\":[",
                      module_addr, reg_list->module_name, reg_list->count);
    
    // Iterate through metadata and merge with cached values
    for (uint16_t i = 0; i < reg_list->count; i++) {
        const register_info_t *reg = &reg_list->registers[i];
        
        if (i > 0) {
            pos += snprintf(json + pos, 65536 - pos, ",");
        }
        
        pos += snprintf(json + pos, 65536 - pos,
                       "{\"address\":\"0x%04X\",\"name\":\"%s\",\"mode\":\"%s\","
                       "\"data_type\":\"%s\",\"description\":\"%s\"",
                       reg->address, reg->description,
                       register_mode_to_string(reg->mode),
                       register_data_type_to_string(reg->data_type),
                       reg->description);
        
        // Try to find cached value
        uint16_t value = 0;
        uint64_t timestamp = 0;
        hal_status_t cache_status = register_cache_get(module_addr, reg->address, &value, &timestamp);
        
        if (cache_status == HAL_STATUS_OK) {
            pos += snprintf(json + pos, 65536 - pos,
                           ",\"value\":%d,\"timestamp\":\"%s\"",
                           value, format_timestamp(timestamp));
        } else {
            pos += snprintf(json + pos, 65536 - pos, ",\"value\":null,\"timestamp\":null");
        }
        
        pos += snprintf(json + pos, 65536 - pos, "}");
    }
    
    pos += snprintf(json + pos, 65536 - pos, "]}}");
    
    response->status_code = 200;
    response->body = json;
    response->body_length = strlen(json);
    
    printf("[DATA-API] Success: %d registers with %d cached values\n", reg_list->count, cached_count);
    return 0;
}

// POST /api/v1/modules/{addr}/registers/{reg_addr} - Write register
int api_write_register(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    printf("[DATA-API] POST /api/v1/modules/{addr}/registers/{reg_addr}: %s\n", request->path);
    // 1) Enforce authentication for POST endpoints
    const char *auth_header = NULL;
    for (int h = 0; h < request->header_count; h++) {
        if (strcasecmp(request->headers[h].name, "Authorization") == 0) {
            auth_header = request->headers[h].value;
            break;
        }
    }
    int auth_result = api_manager_validate_auth_header(auth_header);
    if (auth_result < 0) {
        return api_manager_create_auth_error_response(response);
    }
    
    // Parse module address
    uint8_t module_addr;
    if (parse_module_addr_from_path(request->path, &module_addr) != 0) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST,
                                                 "Invalid module address");
    }
    
    // Parse register address
    uint16_t reg_addr;
    if (parse_register_addr_from_path(request->path, &reg_addr) != 0) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST,
                                                 "Invalid register address");
    }
    
    printf("[DATA-API] Write: Module=0x%02X, Register=0x%04X\n", module_addr, reg_addr);
    
    // Parse JSON body for value
    if (!request->body || request->body_length == 0) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST,
                                                 "Missing request body");
    }
    
    // Simple JSON parsing for {"value": <number>}
    const char *value_str = strstr(request->body, "\"value\"");
    if (!value_str) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST,
                                                 "Missing 'value' field in request");
    }
    
    value_str = strchr(value_str, ':');
    if (!value_str) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST,
                                                 "Invalid JSON format");
    }
    
    uint16_t value = (uint16_t)atoi(value_str + 1);
    
    // 2) Validate register existence strictly (ignore 0xFFFF sentinel)
    const register_info_t *reg_info = get_register_info(module_addr, reg_addr);
    if (!reg_info) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_NOT_FOUND,
                                                 "Register not found");
    }
    
    // 3) Check if register is writable per metadata
    if (reg_info->mode == REG_MODE_READ_ONLY) {
        char error_json[256];
        snprintf(error_json, sizeof(error_json),
                "{\"success\":false,\"message\":\"Register 0x%04X is read-only\",\"timeout\":false}",
                reg_addr);
        response->status_code = 400;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }

    // 4) Validate value against min/max constraints
    if (!validate_register_value(module_addr, reg_addr, value)) {
        return api_manager_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST,
                                                 "Value out of range");
    }
    
    // 5) Write to module via RS485
    hal_status_t write_status = comm_manager_modbus_write_single_register(module_addr, reg_addr, value);
    
    char result_json[512];
    if (write_status == HAL_STATUS_OK) {
        // Update cache on successful write
        register_cache_store(module_addr, reg_addr, value);
        
        snprintf(result_json, sizeof(result_json),
                "{\"success\":true,\"message\":\"Write successful\",\"timeout\":false,"
                "\"register\":\"0x%04X\",\"value\":%d}",
                reg_addr, value);
        response->status_code = 200;
    } else if (write_status == HAL_STATUS_TIMEOUT) {
        snprintf(result_json, sizeof(result_json),
                "{\"success\":false,\"message\":\"Timeout waiting for response\",\"timeout\":true}");
        response->status_code = 504;
    } else {
        snprintf(result_json, sizeof(result_json),
                "{\"success\":false,\"message\":\"Write failed\",\"timeout\":false}");
        response->status_code = 500;
    }
    
    response->body = strdup(result_json);
    response->body_length = strlen(result_json);
    
    printf("[DATA-API] Write result: success=%d, status=%d\n", write_status == HAL_STATUS_OK, write_status);
    return 0;
}

// Initialize API
int register_data_api_init(void) {
    printf("[DATA-API] Initializing register data API endpoints...\n");
    
    // Register route: GET /api/v1/modules
    int result = api_manager_register_endpoint("/api/v1/modules", API_MGR_HTTP_GET, api_get_modules_list);
    if (result != 0) {
        printf("[DATA-API] Error: Failed to register modules list endpoint\n");
        return -1;
    }
    
    printf("[DATA-API] ✅ Registered: GET /api/v1/modules\n");
    printf("[DATA-API] Register data API endpoints initialized successfully\n");
    return 0;
}

// Helper functions
static int parse_module_addr_from_path(const char *path, uint8_t *addr_out) {
    if (!path || !addr_out) return -1;
    
    const char *prefix = "/api/v1/modules/";
    const char *start = strstr(path, prefix);
    if (!start) return -1;
    
    start += strlen(prefix);
    
    char *endptr;
    long addr = strtol(start, &endptr, 0);
    
    if (endptr == start || addr < 0 || addr > 255) {
        return -1;
    }
    
    *addr_out = (uint8_t)addr;
    return 0;
}

static int parse_register_addr_from_path(const char *path, uint16_t *reg_addr_out) {
    if (!path || !reg_addr_out) return -1;
    
    const char *last_slash = strrchr(path, '/');
    if (!last_slash) return -1;
    
    const char *reg_str = last_slash + 1;
    
    char *endptr;
    long reg_addr = strtol(reg_str, &endptr, 0);
    
    if (endptr == reg_str || reg_addr < 0 || reg_addr > 0xFFFF) {
        return -1;
    }
    
    *reg_addr_out = (uint16_t)reg_addr;
    return 0;
}

static char* format_timestamp(uint64_t timestamp_ms) {
    static char buffer[32];
    time_t seconds = (time_t)(timestamp_ms / 1000);
    struct tm *tm_info = gmtime(&seconds);
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", tm_info);
    return buffer;
}

