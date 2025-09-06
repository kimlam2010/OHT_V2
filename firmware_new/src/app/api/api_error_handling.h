/**
 * @file api_error_handling.h
 * @brief API Error Handling & Validation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.2
 */

#ifndef API_ERROR_HANDLING_H
#define API_ERROR_HANDLING_H

#include "hal_common.h"
#include "api_manager.h"

// Error Handling Version
#define API_ERROR_VERSION_MAJOR 1
#define API_ERROR_VERSION_MINOR 0
#define API_ERROR_VERSION_PATCH 0
#define API_ERROR_VERSION_STRING "1.0.0"

// Error Handling Limits
#define API_ERROR_MAX_MESSAGE_LEN 512
#define API_ERROR_MAX_DETAILS_LEN 1024
#define API_ERROR_MAX_STACK_TRACE_LEN 2048
#define API_ERROR_MAX_VALIDATION_ERRORS 32
#define API_ERROR_MAX_LOG_ENTRIES 1000

// Error Categories
typedef enum {
    API_ERROR_CATEGORY_NONE = 0,
    API_ERROR_CATEGORY_VALIDATION,      // Input validation errors
    API_ERROR_CATEGORY_AUTHENTICATION,  // Authentication errors
    API_ERROR_CATEGORY_AUTHORIZATION,   // Authorization errors
    API_ERROR_CATEGORY_BUSINESS_LOGIC,  // Business logic errors
    API_ERROR_CATEGORY_SYSTEM,          // System errors
    API_ERROR_CATEGORY_NETWORK,         // Network errors
    API_ERROR_CATEGORY_DATABASE,        // Database errors
    API_ERROR_CATEGORY_EXTERNAL_API,    // External API errors
    API_ERROR_CATEGORY_MAX
} api_error_category_t;

// Error Severity Levels
typedef enum {
    API_ERROR_SEVERITY_INFO = 0,
    API_ERROR_SEVERITY_WARNING,
    API_ERROR_SEVERITY_ERROR,
    API_ERROR_SEVERITY_CRITICAL,
    API_ERROR_SEVERITY_MAX
} api_error_severity_t;

// Error Types
typedef enum {
    API_ERROR_TYPE_NONE = 0,
    API_ERROR_TYPE_VALIDATION_FAILED,
    API_ERROR_TYPE_INVALID_PARAMETER,
    API_ERROR_TYPE_MISSING_PARAMETER,
    API_ERROR_TYPE_INVALID_FORMAT,
    API_ERROR_TYPE_AUTHENTICATION_FAILED,
    API_ERROR_TYPE_AUTHORIZATION_FAILED,
    API_ERROR_TYPE_SESSION_EXPIRED,
    API_ERROR_TYPE_RATE_LIMIT_EXCEEDED,
    API_ERROR_TYPE_RESOURCE_NOT_FOUND,
    API_ERROR_TYPE_RESOURCE_CONFLICT,
    API_ERROR_TYPE_INTERNAL_SERVER_ERROR,
    API_ERROR_TYPE_SERVICE_UNAVAILABLE,
    API_ERROR_TYPE_TIMEOUT,
    API_ERROR_TYPE_NETWORK_ERROR,
    API_ERROR_TYPE_DATABASE_ERROR,
    API_ERROR_TYPE_EXTERNAL_API_ERROR,
    API_ERROR_TYPE_MAX
} api_error_type_t;

// Validation Error Structure
typedef struct {
    char field_name[64];                // Field name that failed validation
    char error_message[256];            // Validation error message
    char provided_value[128];           // Value that was provided
    char expected_format[128];          // Expected format or constraint
} api_validation_error_t;

// Error Context Structure
typedef struct {
    char request_id[64];                // Request ID for tracing
    char endpoint[128];                 // API endpoint
    char method[16];                    // HTTP method
    char client_ip[16];                 // Client IP address
    char user_agent[256];               // User agent string
    char session_id[64];                // Session ID (if authenticated)
    uint64_t timestamp;                 // Error timestamp
    uint32_t error_count;               // Number of errors in this request
} api_error_context_t;

// Error Information Structure
typedef struct {
    api_error_type_t type;              // Error type
    api_error_category_t category;      // Error category
    api_error_severity_t severity;      // Error severity
    char error_code[32];                // Error code
    char message[API_ERROR_MAX_MESSAGE_LEN]; // Error message
    char details[API_ERROR_MAX_DETAILS_LEN]; // Error details
    char stack_trace[API_ERROR_MAX_STACK_TRACE_LEN]; // Stack trace
    api_error_context_t context;        // Error context
    api_validation_error_t validation_errors[API_ERROR_MAX_VALIDATION_ERRORS]; // Validation errors
    uint32_t validation_error_count;    // Number of validation errors
    hal_status_t hal_status;            // HAL status code
    int system_errno;                   // System errno
} api_error_info_t;

// Error Response Structure
typedef struct {
    bool success;                       // Always false for errors
    char error_code[32];                // Error code
    char message[API_ERROR_MAX_MESSAGE_LEN]; // Error message
    char details[API_ERROR_MAX_DETAILS_LEN]; // Error details
    api_error_context_t context;        // Error context
    api_validation_error_t validation_errors[API_ERROR_MAX_VALIDATION_ERRORS]; // Validation errors
    uint32_t validation_error_count;    // Number of validation errors
    uint64_t timestamp;                 // Error timestamp
    char request_id[64];                // Request ID
} api_error_response_t;

// Error Log Entry Structure
typedef struct {
    uint64_t timestamp;                 // Log timestamp
    api_error_type_t type;              // Error type
    api_error_severity_t severity;      // Error severity
    char error_code[32];                // Error code
    char message[API_ERROR_MAX_MESSAGE_LEN]; // Error message
    char endpoint[128];                 // API endpoint
    char client_ip[16];                 // Client IP
    char session_id[64];                // Session ID
    uint32_t occurrence_count;          // Number of times this error occurred
} api_error_log_entry_t;

// Error Statistics Structure
typedef struct {
    uint32_t total_errors;              // Total number of errors
    uint32_t errors_by_category[API_ERROR_CATEGORY_MAX]; // Errors by category
    uint32_t errors_by_severity[API_ERROR_SEVERITY_MAX]; // Errors by severity
    uint32_t errors_by_type[API_ERROR_TYPE_MAX]; // Errors by type
    uint32_t validation_errors;         // Number of validation errors
    uint32_t authentication_errors;     // Number of authentication errors
    uint32_t authorization_errors;      // Number of authorization errors
    uint32_t system_errors;             // Number of system errors
    uint64_t last_error_time;           // Timestamp of last error
    uint32_t error_rate_per_minute;     // Error rate per minute
} api_error_stats_t;

// Error Handling Configuration
typedef struct {
    bool error_logging_enabled;         // Enable error logging
    bool stack_trace_enabled;           // Enable stack trace collection
    bool validation_error_details;      // Include validation error details
    bool client_error_details;          // Include error details in client response
    uint32_t max_log_entries;           // Maximum number of log entries
    uint32_t log_retention_days;        // Log retention period in days
    uint32_t error_rate_threshold;      // Error rate threshold per minute
    bool auto_alert_enabled;            // Enable automatic alerts
    char alert_email[128];              // Alert email address
} api_error_config_t;

// Error Handling Functions

/**
 * @brief Initialize error handling system
 * @param config Error handling configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_init(const api_error_config_t *config);

/**
 * @brief Deinitialize error handling system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_deinit(void);

/**
 * @brief Create error information
 * @param type Error type
 * @param category Error category
 * @param severity Error severity
 * @param message Error message
 * @param details Error details
 * @param context Error context
 * @param error_info Error information (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_create(api_error_type_t type, api_error_category_t category, 
                             api_error_severity_t severity, const char *message, 
                             const char *details, const api_error_context_t *context,
                             api_error_info_t *error_info);

/**
 * @brief Log error
 * @param error_info Error information
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_log(const api_error_info_t *error_info);

/**
 * @brief Create error response
 * @param error_info Error information
 * @param response Error response (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_create_response(const api_error_info_t *error_info, 
                                      api_error_response_t *response);

/**
 * @brief Get error statistics
 * @param stats Error statistics (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_get_stats(api_error_stats_t *stats);

/**
 * @brief Clear error statistics
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_clear_stats(void);

/**
 * @brief Get error log entries
 * @param entries Error log entries array
 * @param max_entries Maximum number of entries
 * @param actual_count Actual number of entries returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_get_log_entries(api_error_log_entry_t *entries, 
                                      uint32_t max_entries, uint32_t *actual_count);

/**
 * @brief Clear error log
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_clear_log(void);

// Error Context Functions

/**
 * @brief Create error context from HTTP request
 * @param request HTTP request
 * @param context Error context (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_create_context_from_request(const api_mgr_http_request_t *request,
                                                  api_error_context_t *context);

/**
 * @brief Generate request ID
 * @param request_id Request ID (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_generate_request_id(char *request_id);

// Error Code Functions

/**
 * @brief Get error code from error type
 * @param type Error type
 * @return const char* Error code string
 */
const char* api_error_get_code(api_error_type_t type);

/**
 * @brief Get error message from error type
 * @param type Error type
 * @return const char* Error message string
 */
const char* api_error_get_message(api_error_type_t type);

/**
 * @brief Get error category name
 * @param category Error category
 * @return const char* Category name
 */
const char* api_error_get_category_name(api_error_category_t category);

/**
 * @brief Get error severity name
 * @param severity Error severity
 * @return const char* Severity name
 */
const char* api_error_get_severity_name(api_error_severity_t severity);

// Error Handling Middleware

/**
 * @brief Error handling middleware
 * @param request HTTP request
 * @param response HTTP response
 * @param next_handler Next handler function
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_middleware(const api_mgr_http_request_t *request, 
                                 api_mgr_http_response_t *response,
                                 hal_status_t (*next_handler)(const api_mgr_http_request_t*, api_mgr_http_response_t*));

/**
 * @brief Global error handler
 * @param error_info Error information
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_error_global_handler(const api_error_info_t *error_info,
                                     const api_mgr_http_request_t *request,
                                     api_mgr_http_response_t *response);

// JSON Serialization Functions

/**
 * @brief Serialize error response to JSON
 * @param response Error response
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_error_serialize_response_json(const api_error_response_t *response, 
                                     char *buffer, size_t buffer_size);

/**
 * @brief Serialize error statistics to JSON
 * @param stats Error statistics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_error_serialize_stats_json(const api_error_stats_t *stats, 
                                  char *buffer, size_t buffer_size);

/**
 * @brief Serialize error log entries to JSON
 * @param entries Error log entries
 * @param entry_count Number of entries
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_error_serialize_log_json(const api_error_log_entry_t *entries, 
                                uint32_t entry_count, char *buffer, size_t buffer_size);

#endif // API_ERROR_HANDLING_H
