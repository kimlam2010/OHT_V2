/**
 * @file api_error_handling.c
 * @brief API Error Handling & Validation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.2
 */

#include "api_error_handling.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <uuid/uuid.h>

// Global error handling state
static api_error_config_t g_error_config = {0};
static api_error_stats_t g_error_stats = {0};
static api_error_log_entry_t g_error_log[API_ERROR_MAX_LOG_ENTRIES] = {0};
static uint32_t g_log_count = 0;
static uint32_t g_log_index = 0;
static bool g_initialized = false;

// Error code mappings
static const char* g_error_codes[API_ERROR_TYPE_MAX] = {
    [API_ERROR_TYPE_NONE] = "NONE",
    [API_ERROR_TYPE_VALIDATION_FAILED] = "VALIDATION_FAILED",
    [API_ERROR_TYPE_INVALID_PARAMETER] = "INVALID_PARAMETER",
    [API_ERROR_TYPE_MISSING_PARAMETER] = "MISSING_PARAMETER",
    [API_ERROR_TYPE_INVALID_FORMAT] = "INVALID_FORMAT",
    [API_ERROR_TYPE_AUTHENTICATION_FAILED] = "AUTHENTICATION_FAILED",
    [API_ERROR_TYPE_AUTHORIZATION_FAILED] = "AUTHORIZATION_FAILED",
    [API_ERROR_TYPE_SESSION_EXPIRED] = "SESSION_EXPIRED",
    [API_ERROR_TYPE_RATE_LIMIT_EXCEEDED] = "RATE_LIMIT_EXCEEDED",
    [API_ERROR_TYPE_RESOURCE_NOT_FOUND] = "RESOURCE_NOT_FOUND",
    [API_ERROR_TYPE_RESOURCE_CONFLICT] = "RESOURCE_CONFLICT",
    [API_ERROR_TYPE_INTERNAL_SERVER_ERROR] = "INTERNAL_SERVER_ERROR",
    [API_ERROR_TYPE_SERVICE_UNAVAILABLE] = "SERVICE_UNAVAILABLE",
    [API_ERROR_TYPE_TIMEOUT] = "TIMEOUT",
    [API_ERROR_TYPE_NETWORK_ERROR] = "NETWORK_ERROR",
    [API_ERROR_TYPE_DATABASE_ERROR] = "DATABASE_ERROR",
    [API_ERROR_TYPE_EXTERNAL_API_ERROR] = "EXTERNAL_API_ERROR"
};

// Error messages
static const char* g_error_messages[API_ERROR_TYPE_MAX] = {
    [API_ERROR_TYPE_NONE] = "No error",
    [API_ERROR_TYPE_VALIDATION_FAILED] = "Validation failed",
    [API_ERROR_TYPE_INVALID_PARAMETER] = "Invalid parameter",
    [API_ERROR_TYPE_MISSING_PARAMETER] = "Missing required parameter",
    [API_ERROR_TYPE_INVALID_FORMAT] = "Invalid format",
    [API_ERROR_TYPE_AUTHENTICATION_FAILED] = "Authentication failed",
    [API_ERROR_TYPE_AUTHORIZATION_FAILED] = "Authorization failed",
    [API_ERROR_TYPE_SESSION_EXPIRED] = "Session expired",
    [API_ERROR_TYPE_RATE_LIMIT_EXCEEDED] = "Rate limit exceeded",
    [API_ERROR_TYPE_RESOURCE_NOT_FOUND] = "Resource not found",
    [API_ERROR_TYPE_RESOURCE_CONFLICT] = "Resource conflict",
    [API_ERROR_TYPE_INTERNAL_SERVER_ERROR] = "Internal server error",
    [API_ERROR_TYPE_SERVICE_UNAVAILABLE] = "Service unavailable",
    [API_ERROR_TYPE_TIMEOUT] = "Request timeout",
    [API_ERROR_TYPE_NETWORK_ERROR] = "Network error",
    [API_ERROR_TYPE_DATABASE_ERROR] = "Database error",
    [API_ERROR_TYPE_EXTERNAL_API_ERROR] = "External API error"
};

// Category names
static const char* g_category_names[API_ERROR_CATEGORY_MAX] = {
    [API_ERROR_CATEGORY_NONE] = "None",
    [API_ERROR_CATEGORY_VALIDATION] = "Validation",
    [API_ERROR_CATEGORY_AUTHENTICATION] = "Authentication",
    [API_ERROR_CATEGORY_AUTHORIZATION] = "Authorization",
    [API_ERROR_CATEGORY_BUSINESS_LOGIC] = "Business Logic",
    [API_ERROR_CATEGORY_SYSTEM] = "System",
    [API_ERROR_CATEGORY_NETWORK] = "Network",
    [API_ERROR_CATEGORY_DATABASE] = "Database",
    [API_ERROR_CATEGORY_EXTERNAL_API] = "External API"
};

// Severity names
static const char* g_severity_names[API_ERROR_SEVERITY_MAX] = {
    [API_ERROR_SEVERITY_INFO] = "Info",
    [API_ERROR_SEVERITY_WARNING] = "Warning",
    [API_ERROR_SEVERITY_ERROR] = "Error",
    [API_ERROR_SEVERITY_CRITICAL] = "Critical"
};

/**
 * @brief Initialize error handling system
 */
hal_status_t api_error_init(const api_error_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (g_initialized) {
        return HAL_STATUS_OK;
    }
    
    // Copy configuration
    memcpy(&g_error_config, config, sizeof(api_error_config_t));
    
    // Initialize statistics
    memset(&g_error_stats, 0, sizeof(api_error_stats_t));
    
    // Initialize log
    memset(g_error_log, 0, sizeof(g_error_log));
    g_log_count = 0;
    g_log_index = 0;
    
    g_initialized = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "Error handling system initialized");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize error handling system
 */
hal_status_t api_error_deinit(void) {
    if (!g_initialized) {
        return HAL_STATUS_OK;
    }
    
    g_initialized = false;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "Error handling system deinitialized");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create error information
 */
hal_status_t api_error_create(api_error_type_t type, api_error_category_t category, 
                             api_error_severity_t severity, const char *message, 
                             const char *details, const api_error_context_t *context,
                             api_error_info_t *error_info) {
    if (error_info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Initialize error info
    memset(error_info, 0, sizeof(api_error_info_t));
    
    error_info->type = type;
    error_info->category = category;
    error_info->severity = severity;
    
    // Set error code and message
    strncpy(error_info->error_code, api_error_get_code(type), sizeof(error_info->error_code) - 1);
    
    if (message != NULL) {
        strncpy(error_info->message, message, sizeof(error_info->message) - 1);
    } else {
        strncpy(error_info->message, api_error_get_message(type), sizeof(error_info->message) - 1);
    }
    
    if (details != NULL) {
        strncpy(error_info->details, details, sizeof(error_info->details) - 1);
    }
    
    // Copy context
    if (context != NULL) {
        memcpy(&error_info->context, context, sizeof(api_error_context_t));
    } else {
        error_info->context.timestamp = hal_get_timestamp_ms();
    }
    
    // Set system information
    error_info->hal_status = HAL_STATUS_ERROR;
    error_info->system_errno = errno;
    
    // Generate stack trace if enabled
    if (g_error_config.stack_trace_enabled) {
        // In a real implementation, this would capture the actual stack trace
        snprintf(error_info->stack_trace, sizeof(error_info->stack_trace),
                "Stack trace not implemented in this version");
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Log error
 */
hal_status_t api_error_log(const api_error_info_t *error_info) {
    if (error_info == NULL || !g_initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_error_config.error_logging_enabled) {
        return HAL_STATUS_OK;
    }
    
    // Update statistics
    g_error_stats.total_errors++;
    g_error_stats.errors_by_category[error_info->category]++;
    g_error_stats.errors_by_severity[error_info->severity]++;
    g_error_stats.errors_by_type[error_info->type]++;
    
    if (error_info->category == API_ERROR_CATEGORY_VALIDATION) {
        g_error_stats.validation_errors++;
    } else if (error_info->category == API_ERROR_CATEGORY_AUTHENTICATION) {
        g_error_stats.authentication_errors++;
    } else if (error_info->category == API_ERROR_CATEGORY_AUTHORIZATION) {
        g_error_stats.authorization_errors++;
    } else if (error_info->category == API_ERROR_CATEGORY_SYSTEM) {
        g_error_stats.system_errors++;
    }
    
    g_error_stats.last_error_time = error_info->context.timestamp;
    
    // Add to log
    if (g_log_count < API_ERROR_MAX_LOG_ENTRIES) {
        api_error_log_entry_t *entry = &g_error_log[g_log_count];
        entry->timestamp = error_info->context.timestamp;
        entry->type = error_info->type;
        entry->severity = error_info->severity;
        strncpy(entry->error_code, error_info->error_code, sizeof(entry->error_code) - 1);
        strncpy(entry->message, error_info->message, sizeof(entry->message) - 1);
        strncpy(entry->endpoint, error_info->context.endpoint, sizeof(entry->endpoint) - 1);
        strncpy(entry->client_ip, error_info->context.client_ip, sizeof(entry->client_ip) - 1);
        strncpy(entry->session_id, error_info->context.session_id, sizeof(entry->session_id) - 1);
        entry->occurrence_count = 1;
        
        g_log_count++;
    } else {
        // Rotate log entries
        api_error_log_entry_t *entry = &g_error_log[g_log_index];
        entry->timestamp = error_info->context.timestamp;
        entry->type = error_info->type;
        entry->severity = error_info->severity;
        strncpy(entry->error_code, error_info->error_code, sizeof(entry->error_code) - 1);
        strncpy(entry->message, error_info->message, sizeof(entry->message) - 1);
        strncpy(entry->endpoint, error_info->context.endpoint, sizeof(entry->endpoint) - 1);
        strncpy(entry->client_ip, error_info->context.client_ip, sizeof(entry->client_ip) - 1);
        strncpy(entry->session_id, error_info->context.session_id, sizeof(entry->session_id) - 1);
        entry->occurrence_count = 1;
        
        g_log_index = (g_log_index + 1) % API_ERROR_MAX_LOG_ENTRIES;
    }
    
    // Log to system log
    hal_log_message(HAL_LOG_LEVEL_ERROR, "API Error: %s - %s", 
                   error_info->error_code, error_info->message);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create error response
 */
hal_status_t api_error_create_response(const api_error_info_t *error_info, 
                                      api_error_response_t *response) {
    if (error_info == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize response
    memset(response, 0, sizeof(api_error_response_t));
    
    response->success = false;
    strncpy(response->error_code, error_info->error_code, sizeof(response->error_code) - 1);
    strncpy(response->message, error_info->message, sizeof(response->message) - 1);
    
    // Include details if configured
    if (g_error_config.client_error_details) {
        strncpy(response->details, error_info->details, sizeof(response->details) - 1);
    }
    
    // Copy context
    memcpy(&response->context, &error_info->context, sizeof(api_error_context_t));
    
    // Copy validation errors
    response->validation_error_count = error_info->validation_error_count;
    for (uint32_t i = 0; i < error_info->validation_error_count && i < API_ERROR_MAX_VALIDATION_ERRORS; i++) {
        memcpy(&response->validation_errors[i], &error_info->validation_errors[i], 
               sizeof(api_validation_error_t));
    }
    
    response->timestamp = error_info->context.timestamp;
    strncpy(response->request_id, error_info->context.request_id, sizeof(response->request_id) - 1);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get error statistics
 */
hal_status_t api_error_get_stats(api_error_stats_t *stats) {
    if (stats == NULL || !g_initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(stats, &g_error_stats, sizeof(api_error_stats_t));
    
    return HAL_STATUS_OK;
}

/**
 * @brief Clear error statistics
 */
hal_status_t api_error_clear_stats(void) {
    if (!g_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    memset(&g_error_stats, 0, sizeof(api_error_stats_t));
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get error log entries
 */
hal_status_t api_error_get_log_entries(api_error_log_entry_t *entries, 
                                      uint32_t max_entries, uint32_t *actual_count) {
    if (entries == NULL || actual_count == NULL || !g_initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    uint32_t count = (g_log_count < max_entries) ? g_log_count : max_entries;
    
    for (uint32_t i = 0; i < count; i++) {
        memcpy(&entries[i], &g_error_log[i], sizeof(api_error_log_entry_t));
    }
    
    *actual_count = count;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Clear error log
 */
hal_status_t api_error_clear_log(void) {
    if (!g_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    memset(g_error_log, 0, sizeof(g_error_log));
    g_log_count = 0;
    g_log_index = 0;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create error context from HTTP request
 */
hal_status_t api_error_create_context_from_request(const api_mgr_http_request_t *request,
                                                  api_error_context_t *context) {
    if (request == NULL || context == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize context
    memset(context, 0, sizeof(api_error_context_t));
    
    // Generate request ID
    api_error_generate_request_id(context->request_id);
    
    // Copy request information
    strncpy(context->endpoint, request->path, sizeof(context->endpoint) - 1);
    strncpy(context->method, (request->method == API_MGR_HTTP_GET) ? "GET" :
                            (request->method == API_MGR_HTTP_POST) ? "POST" :
                            (request->method == API_MGR_HTTP_PUT) ? "PUT" :
                            (request->method == API_MGR_HTTP_DELETE) ? "DELETE" : "UNKNOWN",
            sizeof(context->method) - 1);
    
    // Extract client IP from headers
    for (uint32_t i = 0; i < request->header_count; i++) {
        if (strcasecmp(request->headers[i].name, "X-Forwarded-For") == 0 ||
            strcasecmp(request->headers[i].name, "X-Real-IP") == 0) {
            strncpy(context->client_ip, request->headers[i].value, sizeof(context->client_ip) - 1);
            break;
        }
    }
    
    // Extract user agent
    for (uint32_t i = 0; i < request->header_count; i++) {
        if (strcasecmp(request->headers[i].name, "User-Agent") == 0) {
            strncpy(context->user_agent, request->headers[i].value, sizeof(context->user_agent) - 1);
            break;
        }
    }
    
    // Extract session ID
    for (uint32_t i = 0; i < request->header_count; i++) {
        if (strcasecmp(request->headers[i].name, "Authorization") == 0) {
            const char *auth_header = request->headers[i].value;
            if (strncmp(auth_header, "Bearer ", 7) == 0) {
                strncpy(context->session_id, auth_header + 7, sizeof(context->session_id) - 1);
            }
            break;
        }
    }
    
    context->timestamp = hal_get_timestamp_ms();
    context->error_count = 1;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Generate request ID
 */
hal_status_t api_error_generate_request_id(char *request_id) {
    if (request_id == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Generate UUID
    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid, request_id);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get error code from error type
 */
const char* api_error_get_code(api_error_type_t type) {
    if (type >= API_ERROR_TYPE_MAX) {
        return "UNKNOWN";
    }
    return g_error_codes[type];
}

/**
 * @brief Get error message from error type
 */
const char* api_error_get_message(api_error_type_t type) {
    if (type >= API_ERROR_TYPE_MAX) {
        return "Unknown error";
    }
    return g_error_messages[type];
}

/**
 * @brief Get error category name
 */
const char* api_error_get_category_name(api_error_category_t category) {
    if (category >= API_ERROR_CATEGORY_MAX) {
        return "Unknown";
    }
    return g_category_names[category];
}

/**
 * @brief Get error severity name
 */
const char* api_error_get_severity_name(api_error_severity_t severity) {
    if (severity >= API_ERROR_SEVERITY_MAX) {
        return "Unknown";
    }
    return g_severity_names[severity];
}

/**
 * @brief Error handling middleware
 */
hal_status_t api_error_middleware(const api_mgr_http_request_t *request, 
                                 api_mgr_http_response_t *response,
                                 hal_status_t (*next_handler)(const api_mgr_http_request_t*, api_mgr_http_response_t*)) {
    if (request == NULL || response == NULL || next_handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create error context
    api_error_context_t context;
    api_error_create_context_from_request(request, &context);
    
    // Call next handler
    hal_status_t result = next_handler(request, response);
    
    // If there was an error, handle it
    if (result != HAL_STATUS_OK) {
        api_error_info_t error_info;
        api_error_create(API_ERROR_TYPE_INTERNAL_SERVER_ERROR, API_ERROR_CATEGORY_SYSTEM,
                        API_ERROR_SEVERITY_ERROR, "Handler returned error", 
                        "Handler function returned non-OK status", &context, &error_info);
        
        api_error_log(&error_info);
        api_error_global_handler(&error_info, request, response);
    }
    
    return result;
}

/**
 * @brief Global error handler
 */
hal_status_t api_error_global_handler(const api_error_info_t *error_info,
                                     const api_mgr_http_request_t *request,
                                     api_mgr_http_response_t *response) {
    if (error_info == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Suppress unused parameter warning
    (void)request;
    
    // Create error response
    api_error_response_t error_response;
    api_error_create_response(error_info, &error_response);
    
    // Set HTTP status code based on error type
    api_mgr_http_response_code_t status_code = API_MGR_RESPONSE_INTERNAL_SERVER_ERROR;
    
    switch (error_info->type) {
        case API_ERROR_TYPE_VALIDATION_FAILED:
        case API_ERROR_TYPE_INVALID_PARAMETER:
        case API_ERROR_TYPE_MISSING_PARAMETER:
        case API_ERROR_TYPE_INVALID_FORMAT:
            status_code = API_MGR_RESPONSE_BAD_REQUEST;
            break;
        case API_ERROR_TYPE_AUTHENTICATION_FAILED:
        case API_ERROR_TYPE_SESSION_EXPIRED:
            status_code = API_MGR_RESPONSE_UNAUTHORIZED;
            break;
        case API_ERROR_TYPE_AUTHORIZATION_FAILED:
            status_code = API_MGR_RESPONSE_FORBIDDEN;
            break;
        case API_ERROR_TYPE_RESOURCE_NOT_FOUND:
            status_code = API_MGR_RESPONSE_NOT_FOUND;
            break;
        case API_ERROR_TYPE_RESOURCE_CONFLICT:
            status_code = API_MGR_RESPONSE_BAD_REQUEST; // Use BAD_REQUEST as fallback
            break;
        case API_ERROR_TYPE_RATE_LIMIT_EXCEEDED:
            status_code = API_MGR_RESPONSE_BAD_REQUEST; // Use BAD_REQUEST as fallback
            break;
        case API_ERROR_TYPE_SERVICE_UNAVAILABLE:
            status_code = API_MGR_RESPONSE_INTERNAL_SERVER_ERROR; // Use INTERNAL_SERVER_ERROR as fallback
            break;
        case API_ERROR_TYPE_TIMEOUT:
            status_code = API_MGR_RESPONSE_INTERNAL_SERVER_ERROR; // Use INTERNAL_SERVER_ERROR as fallback
            break;
        default:
            status_code = API_MGR_RESPONSE_INTERNAL_SERVER_ERROR;
            break;
    }
    
    // Serialize error response to JSON
    char json_buffer[2048];
    int json_len = api_error_serialize_response_json(&error_response, json_buffer, sizeof(json_buffer));
    
    if (json_len > 0) {
        // Set response
        response->status_code = status_code;
        response->header_count = 0;
        
        // Add content type header
        strncpy(response->headers[0].name, "Content-Type", sizeof(response->headers[0].name) - 1);
        strncpy(response->headers[0].value, "application/json", sizeof(response->headers[0].value) - 1);
        response->header_count = 1;
        
        // Set response body
        response->body = malloc(json_len + 1);
        if (response->body != NULL) {
            strncpy(response->body, json_buffer, json_len);
            response->body[json_len] = '\0';
            response->body_length = json_len;
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize error response to JSON
 */
int api_error_serialize_response_json(const api_error_response_t *response, 
                                     char *buffer, size_t buffer_size) {
    if (response == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"success\":%s,"
        "\"error_code\":\"%s\","
        "\"message\":\"%s\","
        "\"details\":\"%s\","
        "\"timestamp\":%lu,"
        "\"request_id\":\"%s\","
        "\"context\":{"
            "\"endpoint\":\"%s\","
            "\"method\":\"%s\","
            "\"client_ip\":\"%s\""
        "},"
        "\"validation_errors\":[",
        response->success ? "false" : "false",
        response->error_code,
        response->message,
        response->details,
        response->timestamp,
        response->request_id,
        response->context.endpoint,
        response->context.method,
        response->context.client_ip
    );
    
    // Add validation errors
    for (uint32_t i = 0; i < response->validation_error_count && written < (int)buffer_size; i++) {
        if (i > 0) {
            written += snprintf(buffer + written, buffer_size - written, ",");
        }
        
        written += snprintf(buffer + written, buffer_size - written,
            "{"
            "\"field_name\":\"%s\","
            "\"error_message\":\"%s\","
            "\"provided_value\":\"%s\","
            "\"expected_format\":\"%s\""
            "}",
            response->validation_errors[i].field_name,
            response->validation_errors[i].error_message,
            response->validation_errors[i].provided_value,
            response->validation_errors[i].expected_format
        );
    }
    
    written += snprintf(buffer + written, buffer_size - written, "]}");
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Serialize error statistics to JSON
 */
int api_error_serialize_stats_json(const api_error_stats_t *stats, 
                                  char *buffer, size_t buffer_size) {
    if (stats == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"total_errors\":%u,"
        "\"validation_errors\":%u,"
        "\"authentication_errors\":%u,"
        "\"authorization_errors\":%u,"
        "\"system_errors\":%u,"
        "\"last_error_time\":%lu,"
        "\"error_rate_per_minute\":%u,"
        "\"errors_by_category\":{",
        stats->total_errors,
        stats->validation_errors,
        stats->authentication_errors,
        stats->authorization_errors,
        stats->system_errors,
        stats->last_error_time,
        stats->error_rate_per_minute
    );
    
    // Add errors by category
    for (int i = 0; i < API_ERROR_CATEGORY_MAX && written < (int)buffer_size; i++) {
        if (i > 0) {
            written += snprintf(buffer + written, buffer_size - written, ",");
        }
        written += snprintf(buffer + written, buffer_size - written,
            "\"%s\":%u", api_error_get_category_name(i), stats->errors_by_category[i]);
    }
    
    written += snprintf(buffer + written, buffer_size - written, "}}");
    
    return (written < (int)buffer_size) ? written : -1;
}

/**
 * @brief Serialize error log entries to JSON
 */
int api_error_serialize_log_json(const api_error_log_entry_t *entries, 
                                uint32_t entry_count, char *buffer, size_t buffer_size) {
    if (entries == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size, "{\"log_entries\":[");
    
    for (uint32_t i = 0; i < entry_count && written < (int)buffer_size; i++) {
        if (i > 0) {
            written += snprintf(buffer + written, buffer_size - written, ",");
        }
        
        written += snprintf(buffer + written, buffer_size - written,
            "{"
            "\"timestamp\":%lu,"
            "\"type\":\"%s\","
            "\"severity\":\"%s\","
            "\"error_code\":\"%s\","
            "\"message\":\"%s\","
            "\"endpoint\":\"%s\","
            "\"client_ip\":\"%s\","
            "\"occurrence_count\":%u"
            "}",
            entries[i].timestamp,
            api_error_get_code(entries[i].type),
            api_error_get_severity_name(entries[i].severity),
            entries[i].error_code,
            entries[i].message,
            entries[i].endpoint,
            entries[i].client_ip,
            entries[i].occurrence_count
        );
    }
    
    written += snprintf(buffer + written, buffer_size - written, "],\"total\":%u}", entry_count);
    
    return (written < (int)buffer_size) ? written : -1;
}
