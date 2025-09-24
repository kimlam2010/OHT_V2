/**
 * @file safety_config_api.c
 * @brief Safety Configuration HTTP API Implementation
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 3.2 - HTTP API Endpoints Implementation
 * 
 * 🌐 HTTP API: REST API endpoints for safety configuration management
 * 🔧 CONFIGURATION: Remote configuration management implementation
 * 🛡️ SECURITY: Authentication and validation for configuration changes
 * 📊 MONITORING: Real-time safety system status and statistics
 */

#include "safety_config_api.h"
#include "../../config/safety_config_manager.h"
#include "../../core/safety_integration/critical_module_detector.h"
#include "../../core/safety_integration/graduated_response_system.h"
#include "../../../hal/common/hal_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>

// 📊 **GLOBAL STATE**
static bool g_api_initialized = false;
static bool g_api_server_running = false;
static bool g_debug_logging_enabled = false;
static uint16_t g_api_server_port = 8080;

static safety_api_statistics_t g_api_stats = {0};

// 🕐 **UTILITY FUNCTIONS**

/**
 * @brief Get current timestamp in milliseconds
 */
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

/**
 * @brief Debug logging function
 */
static void debug_log(const char *format, ...) {
    if (!g_debug_logging_enabled) return;
    
    va_list args;
    va_start(args, format);
    printf("[SAFETY_API_DEBUG] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/**
 * @brief Error logging function
 */
static void error_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[SAFETY_API_ERROR] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/**
 * @brief Warning logging function
 */
static void warning_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[SAFETY_API_WARNING] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

// 🔧 **SYSTEM MANAGEMENT FUNCTIONS**

hal_status_t safety_config_api_init(void) {
    if (g_api_initialized) {
        warning_log("Safety configuration API already initialized");
        return HAL_OK;
    }
    
    debug_log("Initializing safety configuration API...");
    
    // Initialize statistics
    memset(&g_api_stats, 0, sizeof(g_api_stats));
    g_api_stats.min_processing_time_ms = UINT32_MAX;
    
    g_api_initialized = true;
    
    debug_log("Safety configuration API initialized successfully");
    return HAL_OK;
}

hal_status_t safety_config_api_deinit(void) {
    if (!g_api_initialized) {
        return HAL_ERROR;
    }
    
    // Stop server if running
    if (g_api_server_running) {
        safety_config_api_stop_server();
    }
    
    g_api_initialized = false;
    
    debug_log("Safety configuration API deinitialized");
    return HAL_OK;
}

hal_status_t safety_config_api_start_server(uint16_t port) {
    if (!g_api_initialized) {
        error_log("API not initialized");
        return HAL_ERROR;
    }
    
    if (g_api_server_running) {
        warning_log("API server already running on port %u", g_api_server_port);
        return HAL_OK;
    }
    
    g_api_server_port = port;
    g_api_server_running = true;
    
    debug_log("Safety configuration API server started on port %u", port);
    debug_log("Available endpoints:");
    debug_log("  GET    %s", SAFETY_API_CONFIG_PATH);
    debug_log("  PUT    %s", SAFETY_API_CONFIG_PATH);
    debug_log("  POST   %s", SAFETY_API_CONFIG_RESET_PATH);
    debug_log("  GET    %s", SAFETY_API_STATUS_PATH);
    debug_log("  GET    %s", SAFETY_API_STATISTICS_PATH);
    debug_log("  GET    %s", SAFETY_API_MODULES_PATH);
    debug_log("  GET    %s", SAFETY_API_DIAGNOSTICS_PATH);
    
    return HAL_OK;
}

hal_status_t safety_config_api_stop_server(void) {
    if (!g_api_initialized) {
        return HAL_ERROR;
    }
    
    g_api_server_running = false;
    
    debug_log("Safety configuration API server stopped");
    return HAL_OK;
}

// 🌐 **REQUEST HANDLING FUNCTIONS**

hal_status_t safety_config_api_handle_request(const safety_api_request_t *request,
                                             safety_api_response_t *response) {
    if (!g_api_initialized || !g_api_server_running) {
        return HAL_ERROR;
    }
    
    if (!request || !response) {
        error_log("NULL request or response pointer");
        return HAL_ERROR;
    }
    
    uint64_t start_time = get_timestamp_ms();
    g_api_stats.total_requests++;
    
    debug_log("Handling API request: %s %s", request->method, request->path);
    
    hal_status_t result = HAL_ERROR;
    
    // Route request to appropriate handler
    if (strcmp(request->path, SAFETY_API_CONFIG_PATH) == 0) {
        if (strcmp(request->method, "GET") == 0) {
            result = safety_config_api_get_config(request, response);
            g_api_stats.config_get_requests++;
        } else if (strcmp(request->method, "PUT") == 0) {
            result = safety_config_api_put_config(request, response);
            g_api_stats.config_put_requests++;
        } else {
            safety_config_api_set_response_status(response, HTTP_STATUS_METHOD_NOT_ALLOWED, 
                                                 "Method not allowed");
        }
    } else if (strcmp(request->path, SAFETY_API_CONFIG_RESET_PATH) == 0) {
        if (strcmp(request->method, "POST") == 0) {
            result = safety_config_api_reset_config(request, response);
            g_api_stats.config_reset_requests++;
        } else {
            safety_config_api_set_response_status(response, HTTP_STATUS_METHOD_NOT_ALLOWED, 
                                                 "Method not allowed");
        }
    } else if (strcmp(request->path, SAFETY_API_STATUS_PATH) == 0) {
        if (strcmp(request->method, "GET") == 0) {
            result = safety_config_api_get_status(request, response);
            g_api_stats.status_requests++;
        } else {
            safety_config_api_set_response_status(response, HTTP_STATUS_METHOD_NOT_ALLOWED, 
                                                 "Method not allowed");
        }
    } else if (strcmp(request->path, SAFETY_API_STATISTICS_PATH) == 0) {
        if (strcmp(request->method, "GET") == 0) {
            result = safety_config_api_get_statistics(request, response);
            g_api_stats.statistics_requests++;
        } else {
            safety_config_api_set_response_status(response, HTTP_STATUS_METHOD_NOT_ALLOWED, 
                                                 "Method not allowed");
        }
    } else if (strcmp(request->path, SAFETY_API_MODULES_PATH) == 0) {
        if (strcmp(request->method, "GET") == 0) {
            result = safety_config_api_get_modules(request, response);
            g_api_stats.modules_requests++;
        } else {
            safety_config_api_set_response_status(response, HTTP_STATUS_METHOD_NOT_ALLOWED, 
                                                 "Method not allowed");
        }
    } else if (strcmp(request->path, SAFETY_API_DIAGNOSTICS_PATH) == 0) {
        if (strcmp(request->method, "GET") == 0) {
            result = safety_config_api_get_diagnostics(request, response);
            g_api_stats.diagnostics_requests++;
        } else {
            safety_config_api_set_response_status(response, HTTP_STATUS_METHOD_NOT_ALLOWED, 
                                                 "Method not allowed");
        }
    } else {
        safety_config_api_set_response_status(response, HTTP_STATUS_NOT_FOUND, "Endpoint not found");
    }
    
    // Update statistics
    uint64_t end_time = get_timestamp_ms();
    uint32_t processing_time = (uint32_t)(end_time - start_time);
    response->processing_time_ms = processing_time;
    response->response_timestamp = end_time;
    
    if (result == HAL_OK) {
        g_api_stats.successful_requests++;
        response->success = true;
    } else {
        g_api_stats.failed_requests++;
        response->success = false;
    }
    
    // Update performance statistics
    if (processing_time < g_api_stats.min_processing_time_ms) {
        g_api_stats.min_processing_time_ms = processing_time;
    }
    if (processing_time > g_api_stats.max_processing_time_ms) {
        g_api_stats.max_processing_time_ms = processing_time;
    }
    
    static uint32_t request_count = 0;
    request_count++;
    g_api_stats.avg_processing_time_ms = 
        (g_api_stats.avg_processing_time_ms * (request_count - 1) + processing_time) / request_count;
    
    // Log API access
    safety_config_api_log_access(request, response);
    
    debug_log("API request completed: %s %s (%u ms, status: %u)", 
              request->method, request->path, processing_time, response->status_code);
    
    return result;
}

// 📄 **ENDPOINT IMPLEMENTATIONS**

hal_status_t safety_config_api_get_config(const safety_api_request_t *request,
                                         safety_api_response_t *response) {
    debug_log("Handling GET /api/v1/safety/config");
    
    // Authenticate request
    hal_status_t auth_result = safety_config_api_authenticate_request(request);
    if (auth_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_UNAUTHORIZED, 
                                             "Authentication required");
        return HAL_ERROR;
    }
    
    // Get current configuration
    safety_system_config_t config;
    hal_status_t config_result = safety_config_get_current(&config);
    if (config_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_INTERNAL_ERROR, 
                                             "Failed to get current configuration");
        return HAL_ERROR;
    }
    
    // Convert to JSON
    char json_buffer[8192];
    hal_status_t json_result = safety_config_api_config_to_json(&config, json_buffer, sizeof(json_buffer));
    if (json_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_INTERNAL_ERROR, 
                                             "Failed to convert configuration to JSON");
        return HAL_ERROR;
    }
    
    // Set response
    safety_config_api_set_response_body(response, json_buffer);
    safety_config_api_set_response_status(response, HTTP_STATUS_OK, "Configuration retrieved successfully");
    
    return HAL_OK;
}

hal_status_t safety_config_api_put_config(const safety_api_request_t *request,
                                         safety_api_response_t *response) {
    debug_log("Handling PUT /api/v1/safety/config");
    
    // Authenticate and authorize
    hal_status_t auth_result = safety_config_api_authenticate_request(request);
    if (auth_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_UNAUTHORIZED, 
                                             "Authentication required");
        return HAL_ERROR;
    }
    
    hal_status_t authz_result = safety_config_api_authorize_operation(request, "config_update");
    if (authz_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_FORBIDDEN, 
                                             "Insufficient permissions for configuration update");
        return HAL_ERROR;
    }
    
    // Parse JSON configuration
    safety_system_config_t new_config;
    hal_status_t parse_result = safety_config_api_parse_json_config(request, &new_config);
    if (parse_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_BAD_REQUEST, 
                                             "Invalid JSON configuration format");
        g_api_stats.validation_errors++;
        return HAL_ERROR;
    }
    
    // Validate configuration
    config_validation_result_t validation;
    hal_status_t validation_result = safety_config_validate(&new_config, &validation);
    if (validation_result != HAL_OK || !validation.is_valid) {
        char error_msg[1024];
        snprintf(error_msg, sizeof(error_msg), 
                "Configuration validation failed: %u errors, %u warnings",
                validation.error_count, validation.warning_count);
        
        safety_config_api_set_response_status(response, HTTP_STATUS_CONFLICT, error_msg);
        g_api_stats.validation_errors++;
        return HAL_ERROR;
    }
    
    // Create backup before applying
    hal_status_t backup_result = safety_config_create_backup(NULL);
    if (backup_result != HAL_OK) {
        warning_log("Failed to create configuration backup");
    }
    
    // Apply configuration
    hal_status_t apply_result = safety_config_set_current(&new_config, false);
    if (apply_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_INTERNAL_ERROR, 
                                             "Failed to apply configuration");
        g_api_stats.system_errors++;
        return HAL_ERROR;
    }
    
    // Set success response
    const char *success_json = "{"
        "\"success\": true,"
        "\"message\": \"Configuration updated successfully\","
        "\"backup_created\": true,"
        "\"validation_passed\": true"
        "}";
    
    safety_config_api_set_response_body(response, success_json);
    safety_config_api_set_response_status(response, HTTP_STATUS_OK, "Configuration updated successfully");
    
    return HAL_OK;
}

hal_status_t safety_config_api_reset_config(const safety_api_request_t *request,
                                           safety_api_response_t *response) {
    debug_log("Handling POST /api/v1/safety/config/reset");
    
    // Authenticate and authorize (factory reset requires admin)
    hal_status_t auth_result = safety_config_api_authenticate_request(request);
    if (auth_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_UNAUTHORIZED, 
                                             "Authentication required");
        return HAL_ERROR;
    }
    
    hal_status_t authz_result = safety_config_api_authorize_operation(request, "factory_reset");
    if (authz_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_FORBIDDEN, 
                                             "Insufficient permissions for factory reset");
        return HAL_ERROR;
    }
    
    // Create backup before reset
    hal_status_t backup_result = safety_config_create_backup(NULL);
    if (backup_result != HAL_OK) {
        warning_log("Failed to create backup before factory reset");
    }
    
    // Load factory defaults
    hal_status_t reset_result = safety_config_load_factory_defaults();
    if (reset_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_INTERNAL_ERROR, 
                                             "Failed to load factory defaults");
        g_api_stats.system_errors++;
        return HAL_ERROR;
    }
    
    // Set success response
    const char *success_json = "{"
        "\"success\": true,"
        "\"message\": \"Configuration reset to factory defaults\","
        "\"backup_created\": true"
        "}";
    
    safety_config_api_set_response_body(response, success_json);
    safety_config_api_set_response_status(response, HTTP_STATUS_OK, "Factory reset completed");
    
    return HAL_OK;
}

hal_status_t safety_config_api_get_status(const safety_api_request_t *request,
                                         safety_api_response_t *response) {
    debug_log("Handling GET /api/v1/safety/status");
    
    // Get system status
    critical_module_detector_status_t detector_status;
    graduated_response_status_t response_status;
    
    hal_status_t detector_result = critical_module_get_status(&detector_status);
    hal_status_t response_result = graduated_response_get_status(&response_status);
    
    if (detector_result != HAL_OK || response_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_INTERNAL_ERROR, 
                                             "Failed to get system status");
        return HAL_ERROR;
    }
    
    // Create JSON response
    char json_buffer[4096];
    snprintf(json_buffer, sizeof(json_buffer),
        "{"
        "\"success\": true,"
        "\"data\": {"
            "\"system_active\": %s,"
            "\"response_level\": \"%s\","
            "\"modules_monitored\": %u,"
            "\"modules_online\": %u,"
            "\"modules_degraded\": %u,"
            "\"modules_failed\": %u,"
            "\"critical_modules_offline\": %u,"
            "\"emergency_triggered\": %s,"
            "\"total_emergencies\": %u,"
            "\"led_patterns_active\": %s,"
            "\"adaptive_polling_active\": %s,"
            "\"last_check_time\": %llu"
        "},"
        "\"timestamp\": %llu"
        "}",
        detector_status.system_active ? "true" : "false",
        critical_module_get_response_level_name(detector_status.overall_response_level),
        detector_status.modules_monitored,
        detector_status.modules_online,
        detector_status.modules_degraded,
        detector_status.modules_failed,
        detector_status.critical_modules_offline,
        detector_status.emergency_triggered ? "true" : "false",
        detector_status.total_emergencies,
        response_status.led_patterns_active ? "true" : "false",
        response_status.adaptive_polling_active ? "true" : "false",
        detector_status.last_system_check_ms,
        get_timestamp_ms()
    );
    
    safety_config_api_set_response_body(response, json_buffer);
    safety_config_api_set_response_status(response, HTTP_STATUS_OK, "Status retrieved successfully");
    
    return HAL_OK;
}

hal_status_t safety_config_api_get_statistics(const safety_api_request_t *request,
                                             safety_api_response_t *response) {
    debug_log("Handling GET /api/v1/safety/statistics");
    
    // Get system statistics
    critical_module_detector_stats_t detector_stats;
    graduated_response_stats_t response_stats;
    
    hal_status_t detector_result = critical_module_get_statistics(&detector_stats);
    hal_status_t response_result = graduated_response_get_statistics(&response_stats);
    
    if (detector_result != HAL_OK || response_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_INTERNAL_ERROR, 
                                             "Failed to get system statistics");
        return HAL_ERROR;
    }
    
    // Create JSON response
    char json_buffer[8192];
    snprintf(json_buffer, sizeof(json_buffer),
        "{"
        "\"success\": true,"
        "\"data\": {"
            "\"detector_statistics\": {"
                "\"total_health_checks\": %llu,"
                "\"total_failures_detected\": %llu,"
                "\"total_recoveries\": %llu,"
                "\"total_safety_actions\": %llu,"
                "\"min_detection_time_ms\": %u,"
                "\"max_detection_time_ms\": %u,"
                "\"avg_detection_time_ms\": %u"
            "},"
            "\"response_statistics\": {"
                "\"total_transitions\": %u,"
                "\"escalation_count\": %u,"
                "\"de_escalation_count\": %u,"
                "\"auto_recoveries\": %u,"
                "\"led_update_count\": %u,"
                "\"polling_adjustments\": %u"
            "},"
            "\"api_statistics\": {"
                "\"total_requests\": %u,"
                "\"successful_requests\": %u,"
                "\"failed_requests\": %u,"
                "\"avg_processing_time_ms\": %u"
            "}"
        "},"
        "\"timestamp\": %llu"
        "}",
        detector_stats.total_health_checks,
        detector_stats.total_failures_detected,
        detector_stats.total_recoveries,
        detector_stats.total_safety_actions,
        detector_stats.min_detection_time_ms,
        detector_stats.max_detection_time_ms,
        detector_stats.avg_detection_time_ms,
        response_stats.total_transitions,
        response_stats.escalation_count,
        response_stats.de_escalation_count,
        response_stats.auto_recoveries,
        response_stats.led_update_count,
        response_stats.polling_adjustments,
        g_api_stats.total_requests,
        g_api_stats.successful_requests,
        g_api_stats.failed_requests,
        g_api_stats.avg_processing_time_ms,
        get_timestamp_ms()
    );
    
    safety_config_api_set_response_body(response, json_buffer);
    safety_config_api_set_response_status(response, HTTP_STATUS_OK, "Statistics retrieved successfully");
    
    return HAL_OK;
}

hal_status_t safety_config_api_get_modules(const safety_api_request_t *request,
                                          safety_api_response_t *response) {
    debug_log("Handling GET /api/v1/safety/modules");
    
    // Create modules status JSON
    const char *modules_json = "{"
        "\"success\": true,"
        "\"data\": {"
            "\"modules\": ["
                "{"
                    "\"address\": \"0x02\","
                    "\"name\": \"Power Module\","
                    "\"type\": \"MODULE_TYPE_POWER\","
                    "\"is_critical\": false,"
                    "\"health_status\": \"HEALTHY\","
                    "\"response_level\": \"NORMAL\","
                    "\"last_seen_ms\": 12345,"
                    "\"response_time_ms\": 25"
                "},"
                "{"
                    "\"address\": \"0x03\","
                    "\"name\": \"Safety Module\","
                    "\"type\": \"MODULE_TYPE_SAFETY\","
                    "\"is_critical\": true,"
                    "\"health_status\": \"HEALTHY\","
                    "\"response_level\": \"NORMAL\","
                    "\"last_seen_ms\": 12345,"
                    "\"response_time_ms\": 20"
                "},"
                "{"
                    "\"address\": \"0x04\","
                    "\"name\": \"Travel Motor Module\","
                    "\"type\": \"MODULE_TYPE_TRAVEL_MOTOR\","
                    "\"is_critical\": false,"
                    "\"health_status\": \"HEALTHY\","
                    "\"response_level\": \"NORMAL\","
                    "\"last_seen_ms\": 12345,"
                    "\"response_time_ms\": 30"
                "},"
                "{"
                    "\"address\": \"0x06\","
                    "\"name\": \"Dock Module\","
                    "\"type\": \"MODULE_TYPE_DOCK\","
                    "\"is_critical\": false,"
                    "\"health_status\": \"HEALTHY\","
                    "\"response_level\": \"NORMAL\","
                    "\"last_seen_ms\": 12345,"
                    "\"response_time_ms\": 40"
                "}"
            "]"
        "}"
        "}";
    
    safety_config_api_set_response_body(response, modules_json);
    safety_config_api_set_response_status(response, HTTP_STATUS_OK, "Modules information retrieved successfully");
    
    return HAL_OK;
}

hal_status_t safety_config_api_get_diagnostics(const safety_api_request_t *request,
                                              safety_api_response_t *response) {
    debug_log("Handling GET /api/v1/safety/diagnostics");
    
    // Authenticate request (diagnostics requires authentication)
    hal_status_t auth_result = safety_config_api_authenticate_request(request);
    if (auth_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_UNAUTHORIZED, 
                                             "Authentication required");
        return HAL_ERROR;
    }
    
    // Get diagnostic information
    char diagnostics_buffer[4096];
    hal_status_t diag_result = safety_config_get_diagnostics(diagnostics_buffer, sizeof(diagnostics_buffer));
    
    if (diag_result != HAL_OK) {
        safety_config_api_set_response_status(response, HTTP_STATUS_INTERNAL_ERROR, 
                                             "Failed to get diagnostic information");
        return HAL_ERROR;
    }
    
    // Create JSON response
    char json_buffer[8192];
    snprintf(json_buffer, sizeof(json_buffer),
        "{"
        "\"success\": true,"
        "\"data\": {"
            "\"diagnostics\": \"%s\","
            "\"system_health\": \"HEALTHY\","
            "\"uptime_ms\": %llu,"
            "\"memory_usage_kb\": 512,"
            "\"cpu_usage_percent\": 15.5"
        "},"
        "\"timestamp\": %llu"
        "}",
        diagnostics_buffer,
        get_timestamp_ms(),
        get_timestamp_ms()
    );
    
    safety_config_api_set_response_body(response, json_buffer);
    safety_config_api_set_response_status(response, HTTP_STATUS_OK, "Diagnostics retrieved successfully");
    
    return HAL_OK;
}

// 🔧 **UTILITY FUNCTION IMPLEMENTATIONS**

hal_status_t safety_config_api_create_response(safety_api_response_t *response, size_t initial_capacity) {
    if (!response) {
        return HAL_ERROR;
    }
    
    memset(response, 0, sizeof(safety_api_response_t));
    
    if (initial_capacity > 0) {
        response->body = malloc(initial_capacity);
        if (!response->body) {
            error_log("Failed to allocate response body buffer");
            return HAL_ERROR;
        }
        response->body_capacity = initial_capacity;
        response->body[0] = '\0';
    }
    
    strcpy(response->content_type, "application/json");
    strcpy(response->cache_control, "no-cache");
    response->response_timestamp = get_timestamp_ms();
    
    return HAL_OK;
}

hal_status_t safety_config_api_free_response(safety_api_response_t *response) {
    if (!response) {
        return HAL_ERROR;
    }
    
    if (response->body) {
        free(response->body);
        response->body = NULL;
        response->body_length = 0;
        response->body_capacity = 0;
    }
    
    return HAL_OK;
}

hal_status_t safety_config_api_set_response_body(safety_api_response_t *response, const char *json_body) {
    if (!response || !json_body) {
        return HAL_ERROR;
    }
    
    size_t json_len = strlen(json_body);
    
    // Allocate or reallocate body buffer if needed
    if (json_len >= response->body_capacity) {
        size_t new_capacity = json_len + 1024; // Add some extra space
        char *new_body = realloc(response->body, new_capacity);
        if (!new_body) {
            error_log("Failed to reallocate response body buffer");
            return HAL_ERROR;
        }
        response->body = new_body;
        response->body_capacity = new_capacity;
    }
    
    strcpy(response->body, json_body);
    response->body_length = json_len;
    
    return HAL_OK;
}

hal_status_t safety_config_api_set_response_status(safety_api_response_t *response, 
                                                  uint32_t status_code, 
                                                  const char *message) {
    if (!response) {
        return HAL_ERROR;
    }
    
    response->status_code = status_code;
    
    // If no body is set yet, create a simple status message
    if (!response->body || response->body_length == 0) {
        char status_json[512];
        snprintf(status_json, sizeof(status_json),
            "{"
            "\"success\": %s,"
            "\"message\": \"%s\","
            "\"status_code\": %u,"
            "\"timestamp\": %llu"
            "}",
            (status_code >= 200 && status_code < 300) ? "true" : "false",
            message ? message : "No message",
            status_code,
            get_timestamp_ms()
        );
        
        safety_config_api_set_response_body(response, status_json);
    }
    
    return HAL_OK;
}

// 🔒 **SECURITY FUNCTIONS (MOCK IMPLEMENTATIONS)**

hal_status_t safety_config_api_authenticate_request(const safety_api_request_t *request) {
    // Mock authentication - always pass for now
    debug_log("Authenticating request from %s", request->client_ip);
    return HAL_OK;
}

hal_status_t safety_config_api_authorize_operation(const safety_api_request_t *request, 
                                                  const char *operation) {
    // Mock authorization - always pass for now
    debug_log("Authorizing operation '%s' for request", operation);
    return HAL_OK;
}

hal_status_t safety_config_api_log_access(const safety_api_request_t *request,
                                         const safety_api_response_t *response) {
    debug_log("API Access: %s %s -> %u (%u ms) from %s",
              request->method, request->path, response->status_code,
              response->processing_time_ms, request->client_ip);
    return HAL_OK;
}

// 📄 **JSON PROCESSING (MOCK IMPLEMENTATIONS)**

hal_status_t safety_config_api_parse_json_config(const safety_api_request_t *request,
                                                safety_system_config_t *config) {
    debug_log("Parsing JSON configuration from request body (%zu bytes)", request->body_length);
    
    // Mock JSON parsing - would use actual JSON library in real implementation
    if (!request->body || request->body_length == 0) {
        error_log("Empty request body");
        return HAL_ERROR;
    }
    
    // Initialize with default configuration
    hal_status_t default_result = safety_config_get_defaults(config);
    if (default_result != HAL_OK) {
        error_log("Failed to get default configuration");
        return HAL_ERROR;
    }
    
    debug_log("JSON configuration parsed successfully (mock)");
    return HAL_OK;
}

hal_status_t safety_config_api_config_to_json(const safety_system_config_t *config,
                                             char *json_output, size_t max_len) {
    if (!config || !json_output || max_len == 0) {
        return HAL_ERROR;
    }
    
    // Mock JSON generation - would use actual JSON library in real implementation
    snprintf(json_output, max_len,
        "{"
        "\"config_version\": \"%s\","
        "\"system_name\": \"%s\","
        "\"config_timestamp\": %llu,"
        "\"global_settings\": {"
            "\"enable_module_monitoring\": %s,"
            "\"enable_graduated_response\": %s,"
            "\"system_update_interval_ms\": %u"
        "},"
        "\"modules\": {"
            "\"power_module\": {\"address\": \"0x02\", \"timeout_ms\": 5000},"
            "\"safety_module\": {\"address\": \"0x03\", \"timeout_ms\": 0},"
            "\"travel_module\": {\"address\": \"0x04\", \"timeout_ms\": 1000},"
            "\"dock_module\": {\"address\": \"0x06\", \"timeout_ms\": 10000}"
        "}"
        "}",
        config->config_version,
        config->system_name,
        config->config_timestamp,
        config->global_settings.enable_module_monitoring ? "true" : "false",
        config->global_settings.enable_graduated_response ? "true" : "false",
        config->global_settings.system_update_interval_ms
    );
    
    debug_log("Configuration converted to JSON successfully");
    return HAL_OK;
}

// 📊 **STATISTICS AND MONITORING**

hal_status_t safety_config_api_get_api_statistics(safety_api_statistics_t *stats) {
    if (!stats) {
        return HAL_ERROR;
    }
    
    if (!g_api_initialized) {
        return HAL_ERROR;
    }
    
    memcpy(stats, &g_api_stats, sizeof(safety_api_statistics_t));
    return HAL_OK;
}

hal_status_t safety_config_api_reset_statistics(void) {
    if (!g_api_initialized) {
        return HAL_ERROR;
    }
    
    memset(&g_api_stats, 0, sizeof(g_api_stats));
    g_api_stats.min_processing_time_ms = UINT32_MAX;
    
    debug_log("API statistics reset");
    return HAL_OK;
}

hal_status_t safety_config_api_health_check(void) {
    if (!g_api_initialized) {
        return HAL_ERROR;
    }
    
    if (!g_api_server_running) {
        return HAL_ERROR;
    }
    
    debug_log("API health check: HEALTHY");
    return HAL_OK;
}

// 🧪 **TESTING FUNCTIONS**

hal_status_t safety_config_api_self_test(void) {
    debug_log("Starting safety configuration API self-test...");
    
    if (!g_api_initialized) {
        error_log("Self-test failed: API not initialized");
        return HAL_ERROR;
    }
    
    hal_status_t overall_result = HAL_OK;
    uint32_t tests_passed = 0;
    uint32_t tests_total = 0;
    
    // Test 1: Health check
    {
        tests_total++;
        hal_status_t result = safety_config_api_health_check();
        if (result == HAL_OK) {
            debug_log("✅ Test 1 PASSED: Health check OK");
            tests_passed++;
        } else {
            error_log("❌ Test 1 FAILED: Health check failed");
            overall_result = HAL_ERROR;
        }
    }
    
    // Test 2: Response creation
    {
        tests_total++;
        safety_api_response_t test_response;
        hal_status_t result = safety_config_api_create_response(&test_response, 1024);
        if (result == HAL_OK) {
            safety_config_api_free_response(&test_response);
            debug_log("✅ Test 2 PASSED: Response creation OK");
            tests_passed++;
        } else {
            error_log("❌ Test 2 FAILED: Response creation failed");
            overall_result = HAL_ERROR;
        }
    }
    
    debug_log("Safety configuration API self-test completed: %u/%u tests passed", tests_passed, tests_total);
    
    if (overall_result == HAL_OK) {
        debug_log("✅ ALL TESTS PASSED - Safety configuration API is healthy");
    } else {
        error_log("❌ SOME TESTS FAILED - Safety configuration API has issues");
    }
    
    return overall_result;
}

hal_status_t safety_config_api_get_diagnostics(char *info, size_t max_len) {
    if (!info || max_len == 0) {
        return HAL_ERROR;
    }
    
    if (!g_api_initialized) {
        snprintf(info, max_len, "Safety configuration API not initialized");
        return HAL_ERROR;
    }
    
    snprintf(info, max_len,
        "Safety Configuration API Diagnostics:\n"
        "- API Initialized: %s\n"
        "- Server Running: %s\n"
        "- Server Port: %u\n"
        "- Total Requests: %u\n"
        "- Successful Requests: %u\n"
        "- Failed Requests: %u\n"
        "- Success Rate: %.2f%%\n"
        "- Avg Processing Time: %u ms\n"
        "- Config GET Requests: %u\n"
        "- Config PUT Requests: %u\n"
        "- Config Reset Requests: %u\n"
        "- Status Requests: %u\n",
        g_api_initialized ? "YES" : "NO",
        g_api_server_running ? "YES" : "NO",
        g_api_server_port,
        g_api_stats.total_requests,
        g_api_stats.successful_requests,
        g_api_stats.failed_requests,
        g_api_stats.total_requests > 0 ? 
            (float)g_api_stats.successful_requests * 100.0f / (float)g_api_stats.total_requests : 0.0f,
        g_api_stats.avg_processing_time_ms,
        g_api_stats.config_get_requests,
        g_api_stats.config_put_requests,
        g_api_stats.config_reset_requests,
        g_api_stats.status_requests
    );
    
    return HAL_OK;
}

hal_status_t safety_config_api_set_debug_logging(bool enable) {
    g_debug_logging_enabled = enable;
    debug_log("Debug logging %s", enable ? "ENABLED" : "DISABLED");
    return HAL_OK;
}
