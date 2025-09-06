/**
 * @file api_diagnostics.h
 * @brief API Diagnostics & Logging Management for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.3
 */

#ifndef API_DIAGNOSTICS_H
#define API_DIAGNOSTICS_H

#include "hal_common.h"
#include "api_manager.h"

// Diagnostics Management Version
#define API_DIAGNOSTICS_VERSION_MAJOR 1
#define API_DIAGNOSTICS_VERSION_MINOR 0
#define API_DIAGNOSTICS_VERSION_PATCH 0
#define API_DIAGNOSTICS_VERSION_STRING "1.0.0"

// Log Levels
typedef enum {
    API_LOG_LEVEL_ERROR = 0,
    API_LOG_LEVEL_WARNING,
    API_LOG_LEVEL_INFO,
    API_LOG_LEVEL_DEBUG,
    API_LOG_LEVEL_TRACE,
    API_LOG_LEVEL_MAX
} api_log_level_t;

// Log Categories
typedef enum {
    API_LOG_CATEGORY_SYSTEM = 0,
    API_LOG_CATEGORY_SAFETY,
    API_LOG_CATEGORY_MOTOR,
    API_LOG_CATEGORY_POWER,
    API_LOG_CATEGORY_DOCK,
    API_LOG_CATEGORY_NETWORK,
    API_LOG_CATEGORY_API,
    API_LOG_CATEGORY_MAX
} api_log_category_t;

// Log Entry Structure
typedef struct {
    uint64_t timestamp;              // Timestamp in milliseconds
    api_log_level_t level;           // Log level
    api_log_category_t category;     // Log category
    char source[32];                 // Source identifier
    uint32_t line_number;            // Line number
    char message[256];               // Log message
    char details[512];               // Additional details
} api_log_entry_t;

// System Diagnostics Structure
typedef struct {
    uint64_t uptime_ms;              // System uptime
    uint32_t memory_usage_kb;        // Memory usage in KB
    uint32_t memory_total_kb;        // Total memory in KB
    float cpu_usage_percent;         // CPU usage percentage
    uint32_t task_count;             // Number of running tasks
    uint32_t error_count;            // Total error count
    uint32_t warning_count;          // Total warning count
    uint32_t network_connections;    // Active network connections
    uint32_t api_requests_total;     // Total API requests
    uint32_t api_requests_failed;    // Failed API requests
    uint64_t api_response_time_avg;  // Average API response time
} api_system_diagnostics_t;

// Module Diagnostics Structure
typedef struct {
    char module_name[32];            // Module name
    bool online;                     // Online status
    uint64_t last_heartbeat;         // Last heartbeat timestamp
    uint32_t error_count;            // Module error count
    uint32_t warning_count;          // Module warning count
    uint32_t message_count;          // Total messages processed
    uint32_t message_errors;         // Message processing errors
    float response_time_avg;         // Average response time
    char status_message[128];        // Status message
} api_module_diagnostics_t;

// Diagnostics Configuration
typedef struct {
    bool diagnostics_enabled;        // Enable diagnostics
    uint32_t log_buffer_size;        // Log buffer size
    uint32_t max_log_entries;        // Maximum log entries
    api_log_level_t min_log_level;   // Minimum log level
    bool log_to_file;                // Log to file
    bool log_to_console;             // Log to console
    char log_file_path[256];         // Log file path
    uint32_t log_rotation_size;      // Log rotation size in KB
} api_diagnostics_config_t;

// Diagnostics Statistics
typedef struct {
    uint32_t total_log_entries;      // Total log entries
    uint32_t log_entries_by_level[API_LOG_LEVEL_MAX]; // Log entries by level
    uint32_t log_entries_by_category[API_LOG_CATEGORY_MAX]; // Log entries by category
    uint32_t diagnostics_requests;   // Number of diagnostics requests
    uint64_t last_diagnostics_time;  // Last diagnostics request time
    uint32_t error_rate_per_hour;    // Error rate per hour
    uint32_t warning_rate_per_hour;  // Warning rate per hour
} api_diagnostics_stats_t;

// Diagnostics Management Functions

/**
 * @brief Initialize diagnostics management
 * @param config Diagnostics configuration
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_init(const api_diagnostics_config_t *config);

/**
 * @brief Deinitialize diagnostics management
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_deinit(void);

/**
 * @brief Log message
 * @param level Log level
 * @param category Log category
 * @param source Source identifier
 * @param line_number Line number
 * @param message Log message
 * @param details Additional details (optional)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_log(api_log_level_t level, api_log_category_t category, 
                                const char *source, uint32_t line_number, 
                                const char *message, const char *details);

/**
 * @brief Get system diagnostics
 * @param diagnostics System diagnostics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_system(api_system_diagnostics_t *diagnostics);

/**
 * @brief Get module diagnostics
 * @param module_name Module name
 * @param diagnostics Module diagnostics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_module(const char *module_name, api_module_diagnostics_t *diagnostics);

/**
 * @brief Get all module diagnostics
 * @param diagnostics Array of module diagnostics
 * @param max_modules Maximum number of modules
 * @param actual_count Actual number of modules returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_all_modules(api_module_diagnostics_t *diagnostics, 
                                            uint32_t max_modules, uint32_t *actual_count);

/**
 * @brief Get log entries
 * @param level Filter by log level (API_LOG_LEVEL_MAX for all)
 * @param category Filter by category (API_LOG_CATEGORY_MAX for all)
 * @param max_entries Maximum number of entries
 * @param entries Array of log entries
 * @param actual_count Actual number of entries returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_logs(api_log_level_t level, api_log_category_t category,
                                     uint32_t max_entries, api_log_entry_t *entries, uint32_t *actual_count);

/**
 * @brief Clear log entries
 * @param level Clear by log level (API_LOG_LEVEL_MAX for all)
 * @param category Clear by category (API_LOG_CATEGORY_MAX for all)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_clear_logs(api_log_level_t level, api_log_category_t category);

/**
 * @brief Get diagnostics statistics
 * @param stats Diagnostics statistics structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_stats(api_diagnostics_stats_t *stats);

/**
 * @brief Get diagnostics configuration
 * @param config Diagnostics configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_get_config(api_diagnostics_config_t *config);

/**
 * @brief Set diagnostics configuration
 * @param config Diagnostics configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_diagnostics_set_config(const api_diagnostics_config_t *config);

// Diagnostics Endpoint Handlers

/**
 * @brief Handle GET /api/v1/diagnostics/system - Get system diagnostics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_system(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/diagnostics/modules - Get all module diagnostics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_modules(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/diagnostics/module/{name} - Get specific module diagnostics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_module(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/diagnostics/logs - Get log entries
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_logs(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle DELETE /api/v1/diagnostics/logs - Clear log entries
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_clear_logs(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/diagnostics/stats - Get diagnostics statistics
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_stats(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/diagnostics/config - Get diagnostics configuration
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_config(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/diagnostics/config - Set diagnostics configuration
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_diagnostics_set_config(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

// JSON Serialization Functions

/**
 * @brief Serialize system diagnostics to JSON
 * @param diagnostics System diagnostics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_system_diagnostics_json(const api_system_diagnostics_t *diagnostics, char *buffer, size_t buffer_size);

/**
 * @brief Serialize module diagnostics to JSON
 * @param diagnostics Module diagnostics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_module_diagnostics_json(const api_module_diagnostics_t *diagnostics, char *buffer, size_t buffer_size);

/**
 * @brief Serialize log entry to JSON
 * @param entry Log entry
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_log_entry_json(const api_log_entry_t *entry, char *buffer, size_t buffer_size);

/**
 * @brief Serialize diagnostics statistics to JSON
 * @param stats Diagnostics statistics
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_diagnostics_stats_json(const api_diagnostics_stats_t *stats, char *buffer, size_t buffer_size);

// Utility Functions

/**
 * @brief Get log level name
 * @param level Log level
 * @return const char* Level name
 */
const char* api_diagnostics_get_log_level_name(api_log_level_t level);

/**
 * @brief Get log category name
 * @param category Log category
 * @return const char* Category name
 */
const char* api_diagnostics_get_log_category_name(api_log_category_t category);

/**
 * @brief Get log level from string
 * @param level_name Level name string
 * @return api_log_level_t Log level
 */
api_log_level_t api_diagnostics_get_log_level_from_string(const char *level_name);

/**
 * @brief Get log category from string
 * @param category_name Category name string
 * @return api_log_category_t Log category
 */
api_log_category_t api_diagnostics_get_log_category_from_string(const char *category_name);

// Convenience Macros

#define API_LOG_ERROR(category, source, message) \
    api_diagnostics_log(API_LOG_LEVEL_ERROR, category, source, __LINE__, message, NULL)

#define API_LOG_WARNING(category, source, message) \
    api_diagnostics_log(API_LOG_LEVEL_WARNING, category, source, __LINE__, message, NULL)

#define API_LOG_INFO(category, source, message) \
    api_diagnostics_log(API_LOG_LEVEL_INFO, category, source, __LINE__, message, NULL)

#define API_LOG_DEBUG(category, source, message) \
    api_diagnostics_log(API_LOG_LEVEL_DEBUG, category, source, __LINE__, message, NULL)

#define API_LOG_TRACE(category, source, message) \
    api_diagnostics_log(API_LOG_LEVEL_TRACE, category, source, __LINE__, message, NULL)

#define API_LOG_ERROR_DETAILS(category, source, message, details) \
    api_diagnostics_log(API_LOG_LEVEL_ERROR, category, source, __LINE__, message, details)

#define API_LOG_WARNING_DETAILS(category, source, message, details) \
    api_diagnostics_log(API_LOG_LEVEL_WARNING, category, source, __LINE__, message, details)

#define API_LOG_INFO_DETAILS(category, source, message, details) \
    api_diagnostics_log(API_LOG_LEVEL_INFO, category, source, __LINE__, message, details)

#define API_LOG_DEBUG_DETAILS(category, source, message, details) \
    api_diagnostics_log(API_LOG_LEVEL_DEBUG, category, source, __LINE__, message, details)

#define API_LOG_TRACE_DETAILS(category, source, message, details) \
    api_diagnostics_log(API_LOG_LEVEL_TRACE, category, source, __LINE__, message, details)

#endif // API_DIAGNOSTICS_H
