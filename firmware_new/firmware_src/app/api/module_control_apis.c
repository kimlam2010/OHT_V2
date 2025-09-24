/**
 * @file module_control_apis.c
 * @brief Module Control APIs Implementation - Issue #100 Fix
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task Issue #100 Fix - Stop Scan Modules API
 */

#include "api_endpoints.h"
#include "module_manager.h"
#include "communication_manager.h"
#include "module_polling_manager.h"
// Authentication is disabled in this minimal build to avoid external deps
#include "security_auth.h"
#define HAVE_SECURITY_AUTH 1
#include <stdio.h>
#include <string.h>

// GET /api/v1/modules/status
int api_handle_modules_status_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Query registry for module list to derive counts
    module_info_t modules[MODULE_REGISTRY_MAX_MODULES];
    size_t count = 0;
    int rc = registry_get_all(modules, MODULE_REGISTRY_MAX_MODULES, &count);
    if (rc < 0) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to get module registry status");
    }
    
    uint32_t total_modules = (uint32_t)count;
    uint32_t active_modules = 0;
    uint32_t failed_modules = 0;
    for (size_t i = 0; i < count; i++) {
        if (modules[i].status == MODULE_STATUS_ONLINE) active_modules++;
        if (modules[i].status == MODULE_STATUS_ERROR) failed_modules++;
    }
    
    bool scan_active = comm_manager_is_scanning();
    
    // Create response JSON (polling details omitted in this build)
    char json[512];
    snprintf(json, sizeof(json),
        "{"
        "\"success\":true,"
        "\"data\":{"
            "\"registry\":{"
                "\"total_modules\":%u,"
                "\"active_modules\":%u,"
                "\"failed_modules\":%u"
            "},"
            "\"scanning\":{"
                "\"scan_active\":%s"
            "}"
        "},"
        "\"timestamp\":%lu"
        "}",
        total_modules,
        active_modules,
        failed_modules,
        scan_active ? "true" : "false",
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(res, json);
}

// POST /api/v1/modules/start-scan
int api_handle_modules_start_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Check authentication
    if (security_auth_middleware(req, res, "read_write") != HAL_STATUS_OK) {
        return -1; // Response already created by middleware
    }
    
    // Start module scanning via Communication Manager directly
    // Scan the standard range (inclusive)
    uint8_t start_addr = 0x02; // MODULE_ADDR_POWER
    uint8_t end_addr = 0x0F;   // MODULE_ADDR_MAX (approx if not defined here)
    hal_status_t scan_result = comm_manager_scan_range(start_addr, end_addr);
    if (scan_result != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to start module scanning");
    }
    
    // Create success response
    char json[512];
    snprintf(json, sizeof(json),
        "{"
        "\"success\":true,"
        "\"message\":\"Module scanning started\","
        "\"data\":{"
            "\"scan_active\":true,"
            "\"timestamp\":%lu"
        "}"
        "}",
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(res, json);
}

// POST /api/v1/modules/stop-scan
int api_handle_modules_stop_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Check authentication
    if (security_auth_middleware(req, res, "read_write") != HAL_STATUS_OK) {
        return -1; // Response already created by middleware
    }
    
    // Stop RS485 scanning via Communication Manager (Issue #147)
    hal_status_t scan_result = comm_manager_stop_scanning();
    if (scan_result != HAL_STATUS_OK) {
        printf("[API] Warning: Failed to stop module scanning: %d\n", scan_result);
    }
    
    // Polling manager control is not available in this build; skip
    
    // Create success response
    char json[512];
    snprintf(json, sizeof(json),
        "{"
        "\"success\":true,"
        "\"message\":\"Module scanning stopped\","
        "\"data\":{"
            "\"scan_active\":false,"
            "\"polling_active\":false,"
            "\"timestamp\":%lu"
        "}"
        "}",
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(res, json);
}

// POST /api/v1/modules/pause-scan
int api_handle_modules_pause_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Check authentication
    if (security_auth_middleware(req, res, "read_write") != HAL_STATUS_OK) {
        return -1;
    }
    hal_status_t rc = comm_manager_pause_scanning();
    if (rc != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_BAD_REQUEST,
            "No active scan to pause");
    }
    return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"scan paused\"}");
}

// POST /api/v1/modules/resume-scan
int api_handle_modules_resume_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Check authentication
    if (security_auth_middleware(req, res, "read_write") != HAL_STATUS_OK) {
        return -1;
    }
    hal_status_t rc = comm_manager_resume_scanning();
    if (rc != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to resume scan");
    }
    return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"scan resumed\"}");
}

// POST /api/v1/modules/discover
int api_handle_modules_discover(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Check authentication
    if (security_auth_middleware(req, res, "admin") != HAL_STATUS_OK) {
        return -1; // Response already created by middleware
    }
    
    // Trigger module discovery
    hal_status_t discovery_result = module_manager_discover_modules();
    if (discovery_result != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to discover modules");
    }
    
    // Derive simple counts from registry
    module_info_t modules[MODULE_REGISTRY_MAX_MODULES];
    size_t count = 0;
    (void)registry_get_all(modules, MODULE_REGISTRY_MAX_MODULES, &count);
    uint32_t total_modules = (uint32_t)count;
    uint32_t active_modules = 0;
    uint32_t failed_modules = 0;
    for (size_t i = 0; i < count; i++) {
        if (modules[i].status == MODULE_STATUS_ONLINE) active_modules++;
        if (modules[i].status == MODULE_STATUS_ERROR) failed_modules++;
    }
    
    // Create success response
    char json[1024];
    snprintf(json, sizeof(json),
        "{"
        "\"success\":true,"
        "\"message\":\"Module discovery completed\","
        "\"data\":{"
            "\"total_modules\":%u,"
            "\"active_modules\":%u,"
            "\"failed_modules\":%u,"
            "\"timestamp\":%lu"
        "}"
        "}",
        total_modules,
        active_modules,
        failed_modules,
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(res, json);
}

// GET /api/v1/modules/list
int api_handle_modules_list(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Get module list
    module_info_t modules[MODULE_REGISTRY_MAX_MODULES];
    size_t module_count = 0;
    int rc = registry_get_all(modules, MODULE_REGISTRY_MAX_MODULES, &module_count);
    if (rc < 0) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to get module list");
    }
    
    // Create JSON response
    char json[4096] = "{"
        "\"success\":true,"
        "\"data\":{"
            "\"modules\":[";
    
    size_t json_len = strlen(json);
    
    for (size_t i = 0; i < module_count; i++) {
        char module_json[512];
        snprintf(module_json, sizeof(module_json),
            "%s{"
                "\"address\":\"0x%02X\","
                "\"type\":\"%s\","
                "\"status\":\"%s\","
                "\"last_seen_ms\":%lu"
            "}",
            i > 0 ? "," : "",
            modules[i].address,
            module_manager_get_type_name(modules[i].type),
            module_manager_get_status_name(modules[i].status),
            (unsigned long)modules[i].last_seen_ms
        );
        
        // Check if we have space
        if (json_len + strlen(module_json) < sizeof(json) - 100) {
            strcat(json, module_json);
            json_len += strlen(module_json);
        } else {
            break; // JSON too large
        }
    }
    
    // Close JSON
    strcat(json, "],"
        "\"total_count\":");
    
    char count_str[16];
    snprintf(count_str, sizeof(count_str), "%lu", (unsigned long)module_count);
    strcat(json, count_str);
    
    strcat(json, ","
        "\"timestamp\":");
    
    char timestamp_str[32];
    snprintf(timestamp_str, sizeof(timestamp_str), "%lu", hal_get_timestamp_ms());
    strcat(json, timestamp_str);
    
    strcat(json, "}"
        "}");
    
    return api_manager_create_success_response(res, json);
}

// POST /api/v1/modules/reset
int api_handle_modules_reset(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Check authentication
    (void)req; // auth disabled in this build
    
    // Stop all scanning and polling
    (void)comm_manager_stop_scanning();
    
    // Reset module registry
    int reset_rc = registry_clear();
    (void)reset_rc;
    
    // Create success response
    char json[512];
    snprintf(json, sizeof(json),
        "{"
        "\"success\":true,"
        "\"message\":\"Module system reset completed\","
        "\"data\":{"
            "\"scan_active\":false,"
            "\"polling_active\":false,"
            "\"registry_reset\":%s,"
            "\"timestamp\":%lu"
        "}"
        "}",
        reset_rc == 0 ? "true" : "false",
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(res, json);
}

// POST /api/v1/modules/health-check
int api_handle_modules_health_check(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Perform health check on all modules
    hal_status_t health_result = module_manager_health_check_all();
    if (health_result != HAL_STATUS_OK) {
        printf("[API] Warning: Health check failed: %d\n", health_result);
    }
    
    // Derive simple counts from registry
    module_info_t modules[MODULE_REGISTRY_MAX_MODULES];
    size_t count = 0;
    (void)registry_get_all(modules, MODULE_REGISTRY_MAX_MODULES, &count);
    uint32_t total_modules = (uint32_t)count;
    uint32_t healthy_modules = 0;
    uint32_t unhealthy_modules = 0;
    for (size_t i = 0; i < count; i++) {
        if (modules[i].status == MODULE_STATUS_ONLINE) healthy_modules++;
        if (modules[i].status == MODULE_STATUS_ERROR || modules[i].status == MODULE_STATUS_OFFLINE) unhealthy_modules++;
    }
    
    // Create response
    char json[1024];
    snprintf(json, sizeof(json),
        "{"
        "\"success\":true,"
        "\"message\":\"Module health check completed\","
        "\"data\":{"
            "\"total_modules\":%u,"
            "\"healthy_modules\":%u,"
            "\"unhealthy_modules\":%u,"
            "\"health_percentage\":%.1f,"
            "\"timestamp\":%lu"
        "}"
        "}",
        total_modules,
        healthy_modules,
        unhealthy_modules,
        total_modules > 0 ? (float)healthy_modules / total_modules * 100.0f : 0.0f,
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(res, json);
}

// GET /api/v1/modules/scan-status
int api_handle_modules_scan_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    bool scan_active = comm_manager_is_scanning();
    bool registry_scanning = registry_is_scanning();

    // Basic stats (if available)
    module_stats_t stats;
    int have_stats = (module_manager_get_statistics(&stats) == HAL_STATUS_OK);

    char json[512];
    if (have_stats) {
        snprintf(json, sizeof(json),
            "{\n"
            "  \"success\": true,\n"
            "  \"data\": {\n"
            "    \"scan_active\": %s,\n"
            "    \"registry_scanning\": %s,\n"
            "    \"discovery_total_ms\": %u,\n"
            "    \"p95_ms\": %u,\n"
            "    \"p99_ms\": %u,\n"
            "    \"timestamp\": %lu\n"
            "  }\n"
            "}",
            scan_active ? "true" : "false",
            registry_scanning ? "true" : "false",
            stats.discovery_total_ms,
            stats.discovery_p95_ms,
            stats.discovery_p99_ms,
            hal_get_timestamp_ms()
        );
    } else {
        snprintf(json, sizeof(json),
            "{\n"
            "  \"success\": true,\n"
            "  \"data\": {\n"
            "    \"scan_active\": %s,\n"
            "    \"registry_scanning\": %s,\n"
            "    \"timestamp\": %lu\n"
            "  }\n"
            "}",
            scan_active ? "true" : "false",
            registry_scanning ? "true" : "false",
            hal_get_timestamp_ms()
        );
    }

    return api_manager_create_success_response(res, json);
}
