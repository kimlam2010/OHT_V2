/**
 * @file register_metadata_api.c
 * @brief HTTP API Endpoints Implementation for Register Metadata (Issue #203)
 * @version 1.0.1 - Fixed to use API Manager
 * @date 2025-01-28
 * @author FW Team
 * @task Issue #203 - Register Metadata API Implementation
 */

#include "register_metadata_api.h"
#include "../../hal/register/register_info.h"
#include "../utils/register_json_serializer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Parse module address from URL path
 * @param path URL path (e.g. "/api/v1/modules/2/registers")
 * @param addr_out Pointer to store parsed address
 * @return 0 on success, -1 on error
 */
static int parse_module_addr_from_path(const char *path, uint8_t *addr_out) {
    if (!path || !addr_out) return -1;
    
    // Find "/modules/" in path
    const char *prefix = "/api/v1/modules/";
    const char *start = strstr(path, prefix);
    if (!start) return -1;
    
    // Move to address part
    start += strlen(prefix);
    
    // Parse address (hex or decimal)
    char *endptr;
    long addr = strtol(start, &endptr, 0);  // 0 = auto-detect base (0x for hex)
    
    if (endptr == start || addr < 0 || addr > 255) {
        return -1;
    }
    
    *addr_out = (uint8_t)addr;
    return 0;
}

/**
 * @brief Parse register address from URL path
 * @param path URL path (e.g. "/api/v1/modules/2/registers/0x0000")
 * @param reg_addr_out Pointer to store parsed register address
 * @return 0 on success, -1 on error
 */
static int parse_register_addr_from_path(const char *path, uint16_t *reg_addr_out) {
    if (!path || !reg_addr_out) return -1;
    
    // Find last "/" which precedes register address
    const char *last_slash = strrchr(path, '/');
    if (!last_slash) return -1;
    
    // Move past "/"
    const char *reg_str = last_slash + 1;
    
    // Parse register address (hex or decimal)
    char *endptr;
    long reg_addr = strtol(reg_str, &endptr, 0);  // 0 = auto-detect base
    
    if (endptr == reg_str || reg_addr < 0 || reg_addr > 0xFFFF) {
        return -1;
    }
    
    *reg_addr_out = (uint16_t)reg_addr;
    return 0;
}

// ============================================================================
// API ENDPOINT HANDLERS
// ============================================================================

/**
 * @brief Handle GET /api/v1/modules/{addr}/registers
 */
int api_get_module_registers_wrapper(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    printf("[REG_API] GET /api/v1/modules/{addr}/registers\n");
    
    // Parse module address from path
    uint8_t module_addr;
    if (parse_module_addr_from_path(request->path, &module_addr) != 0) {
        const char *error_json = "{\"success\":false,\"error\":\"Invalid module address in path\"}";
        response->status_code = 400;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }
    
    printf("[REG_API] Module address: 0x%02X\n", module_addr);
    
    // Get register list for module
    const register_list_response_t *reg_list = get_module_register_list_api(module_addr);
    if (!reg_list || !reg_list->valid) {
        char error_json[256];
        snprintf(error_json, sizeof(error_json),
                "{\"success\":false,\"error\":\"Module 0x%02X not found or no registers defined\"}",
                module_addr);
        response->status_code = 404;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }
    
    // Serialize to JSON
    char *json_body = serialize_register_list_to_json(reg_list);
    if (!json_body) {
        const char *error_json = "{\"success\":false,\"error\":\"Failed to serialize register list\"}";
        response->status_code = 500;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }
    
    // Success response
    response->status_code = 200;
    response->body = json_body;
    response->body_length = strlen(json_body);
    
    printf("[REG_API] Success: %u registers for module 0x%02X\n", reg_list->count, module_addr);
    return 0;
}

/**
 * @brief Handle GET /api/v1/modules/{addr}/registers/{reg_addr}
 */
int api_get_single_register_wrapper(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    printf("[REG_API] GET /api/v1/modules/{addr}/registers/{reg_addr}\n");
    
    // Parse module address
    uint8_t module_addr;
    if (parse_module_addr_from_path(request->path, &module_addr) != 0) {
        const char *error_json = "{\"success\":false,\"error\":\"Invalid module address in path\"}";
        response->status_code = 400;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }
    
    // Parse register address
    uint16_t reg_addr;
    if (parse_register_addr_from_path(request->path, &reg_addr) != 0) {
        const char *error_json = "{\"success\":false,\"error\":\"Invalid register address in path\"}";
        response->status_code = 400;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }
    
    printf("[REG_API] Module: 0x%02X, Register: 0x%04X\n", module_addr, reg_addr);
    
    // Get register info
    const register_info_t *reg_info = get_register_info(module_addr, reg_addr);
    if (!reg_info) {
        char error_json[256];
        snprintf(error_json, sizeof(error_json),
                "{\"success\":false,\"error\":\"Register 0x%04X not found in module 0x%02X\"}",
                reg_addr, module_addr);
        response->status_code = 404;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }
    
    // Serialize to JSON
    char *json_body = serialize_register_to_json(reg_info);
    if (!json_body) {
        const char *error_json = "{\"success\":false,\"error\":\"Failed to serialize register\"}";
        response->status_code = 500;
        response->body = strdup(error_json);
        response->body_length = strlen(error_json);
        return 0;
    }
    
    // Success response
    response->status_code = 200;
    response->body = json_body;
    response->body_length = strlen(json_body);
    
    printf("[REG_API] Success: Register 0x%04X from module 0x%02X\n", reg_addr, module_addr);
    return 0;
}

// ============================================================================
// API INITIALIZATION
// ============================================================================

int register_metadata_api_init(void) {
    printf("[REG_API] Initializing register metadata API endpoints...\n");
    
    // Register route: GET /api/v1/modules/{addr}/registers
    int result = api_manager_register_endpoint(
        "/api/v1/modules/registers",  // Simplified path - will be parsed dynamically
        API_MGR_HTTP_GET,
        api_get_module_registers_wrapper
    );
    
    if (result != 0) {
        printf("[REG_API] Error: Failed to register module registers endpoint (error: %d)\n", result);
        return -1;
    }
    
    printf("[REG_API] ✅ Registered: GET /api/v1/modules/{addr}/registers\n");
    
    // Register route: GET /api/v1/modules/{addr}/registers/{reg_addr}
    result = api_manager_register_endpoint(
        "/api/v1/modules/registers/single",  // Simplified path - will be parsed dynamically
        API_MGR_HTTP_GET,
        api_get_single_register_wrapper
    );
    
    if (result != 0) {
        printf("[REG_API] Error: Failed to register single register endpoint (error: %d)\n", result);
        return -1;
    }
    
    printf("[REG_API] ✅ Registered: GET /api/v1/modules/{addr}/registers/{reg_addr}\n");
    printf("[REG_API] Register metadata API endpoints initialized successfully\n");
    return 0;
}
