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
#include "module_polling_manager.h"
#include "security_auth.h"
#include <stdio.h>
#include <string.h>

// GET /api/v1/modules/status
int api_handle_modules_status_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Get module registry status
    module_registry_status_t registry_status;
    if (module_manager_get_registry_status(&registry_status) != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to get module registry status");
    }
    
    // Get polling manager status
    polling_manager_status_t polling_status;
    if (module_polling_manager_get_status(&polling_status) != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to get polling manager status");
    }
    
    // Create response JSON
    char json[2048];
    snprintf(json, sizeof(json),
        "{"
        "\"success\":true,"
        "\"data\":{"
            "\"registry\":{"
                "\"total_modules\":%u,"
                "\"active_modules\":%u,"
                "\"failed_modules\":%u,"
                "\"last_scan_time\":%lu"
            "},"
            "\"polling\":{"
                "\"enabled\":%s,"
                "\"active_polls\":%u,"
                "\"total_polls\":%u,"
                "\"failed_polls\":%u,"
                "\"last_poll_time\":%lu"
            "},"
            "\"scanning\":{"
                "\"scan_active\":%s,"
                "\"scan_interval_ms\":%u,"
                "\"last_scan_duration_ms\":%u"
            "}"
        "},"
        "\"timestamp\":%lu"
        "}",
        registry_status.total_modules,
        registry_status.active_modules,
        registry_status.failed_modules,
        registry_status.last_scan_time,
        polling_status.enabled ? "true" : "false",
        polling_status.active_polls,
        polling_status.total_polls,
        polling_status.failed_polls,
        polling_status.last_poll_time,
        polling_status.scan_active ? "true" : "false",
        polling_status.scan_interval_ms,
        polling_status.last_scan_duration_ms,
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
    
    // Start module scanning
    hal_status_t scan_result = module_manager_start_scanning();
    if (scan_result != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to start module scanning");
    }
    
    // Start polling manager
    hal_status_t polling_result = module_polling_manager_start();
    if (polling_result != HAL_STATUS_OK) {
        printf("[API] Warning: Failed to start polling manager: %d\n", polling_result);
    }
    
    // Create success response
    char json[512];
    snprintf(json, sizeof(json),
        "{"
        "\"success\":true,"
        "\"message\":\"Module scanning started\","
        "\"data\":{"
            "\"scan_active\":true,"
            "\"polling_active\":%s,"
            "\"timestamp\":%lu"
        "}"
        "}",
        polling_result == HAL_STATUS_OK ? "true" : "false",
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
    
    // Stop module scanning
    hal_status_t scan_result = module_manager_stop_scanning();
    if (scan_result != HAL_STATUS_OK) {
        printf("[API] Warning: Failed to stop module scanning: %d\n", scan_result);
    }
    
    // Stop polling manager
    hal_status_t polling_result = module_polling_manager_stop();
    if (polling_result != HAL_STATUS_OK) {
        printf("[API] Warning: Failed to stop polling manager: %d\n", polling_result);
    }
    
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

// POST /api/v1/modules/discover
int api_handle_modules_discover(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Check authentication
    if (security_auth_middleware(req, res, "read_write") != HAL_STATUS_OK) {
        return -1; // Response already created by middleware
    }
    
    // Trigger module discovery
    hal_status_t discovery_result = module_manager_discover_modules();
    if (discovery_result != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to discover modules");
    }
    
    // Get updated registry status
    module_registry_status_t registry_status;
    if (module_manager_get_registry_status(&registry_status) != HAL_STATUS_OK) {
        registry_status.total_modules = 0;
        registry_status.active_modules = 0;
        registry_status.failed_modules = 0;
        registry_status.last_scan_time = 0;
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
            "\"discovery_time\":%lu,"
            "\"timestamp\":%lu"
        "}"
        "}",
        registry_status.total_modules,
        registry_status.active_modules,
        registry_status.failed_modules,
        registry_status.last_scan_time,
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(res, json);
}

// GET /api/v1/modules/list
int api_handle_modules_list(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Get module list
    module_info_t modules[32];
    uint32_t module_count = 0;
    
    hal_status_t list_result = module_manager_get_module_list(modules, sizeof(modules) / sizeof(modules[0]), &module_count);
    if (list_result != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to get module list");
    }
    
    // Create JSON response
    char json[4096] = "{"
        "\"success\":true,"
        "\"data\":{"
            "\"modules\":[";
    
    size_t json_len = strlen(json);
    
    for (uint32_t i = 0; i < module_count; i++) {
        char module_json[512];
        snprintf(module_json, sizeof(module_json),
            "%s{"
                "\"id\":%u,"
                "\"address\":\"0x%02X\","
                "\"type\":\"%s\","
                "\"status\":\"%s\","
                "\"last_seen\":%lu,"
                "\"response_time_ms\":%u"
            "}",
            i > 0 ? "," : "",
            modules[i].id,
            modules[i].address,
            modules[i].type_name,
            modules[i].status_name,
            modules[i].last_seen_timestamp,
            modules[i].response_time_ms
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
    snprintf(count_str, sizeof(count_str), "%u", module_count);
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
    if (security_auth_middleware(req, res, "admin") != HAL_STATUS_OK) {
        return -1; // Response already created by middleware
    }
    
    // Stop all scanning and polling
    module_manager_stop_scanning();
    module_polling_manager_stop();
    
    // Reset module registry
    hal_status_t reset_result = module_manager_reset_registry();
    if (reset_result != HAL_STATUS_OK) {
        printf("[API] Warning: Failed to reset module registry: %d\n", reset_result);
    }
    
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
        reset_result == HAL_STATUS_OK ? "true" : "false",
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(res, json);
}

// POST /api/v1/modules/health-check
int api_handle_modules_health_check(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Perform health check on all modules
    uint32_t total_modules = 0;
    uint32_t healthy_modules = 0;
    uint32_t unhealthy_modules = 0;
    
    hal_status_t health_result = module_manager_health_check_all(&total_modules, &healthy_modules, &unhealthy_modules);
    if (health_result != HAL_STATUS_OK) {
        printf("[API] Warning: Health check failed: %d\n", health_result);
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
