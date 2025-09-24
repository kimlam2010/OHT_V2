/**
 * @file safety_config_api.h
 * @brief Safety Configuration HTTP API for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 3.2 - HTTP API Endpoints
 * 
 * 🌐 HTTP API: REST API endpoints for safety configuration management
 * 🔧 CONFIGURATION: Remote configuration management capabilities
 * 🛡️ SECURITY: Authentication and validation for configuration changes
 * 📊 MONITORING: Real-time safety system status and statistics
 */

#ifndef SAFETY_CONFIG_API_H
#define SAFETY_CONFIG_API_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Include existing headers
#include "../../config/safety_config_manager.h"
#include "../../../hal/common/hal_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// 🌐 **API ENDPOINT DEFINITIONS**
#define SAFETY_API_BASE_PATH              "/api/v1/safety"
#define SAFETY_API_CONFIG_PATH            "/api/v1/safety/config"
#define SAFETY_API_CONFIG_RESET_PATH      "/api/v1/safety/config/reset"
#define SAFETY_API_STATUS_PATH            "/api/v1/safety/status"
#define SAFETY_API_STATISTICS_PATH        "/api/v1/safety/statistics"
#define SAFETY_API_MODULES_PATH           "/api/v1/safety/modules"
#define SAFETY_API_DIAGNOSTICS_PATH       "/api/v1/safety/diagnostics"

// 📊 **HTTP STATUS CODES**
#define HTTP_STATUS_OK                    200
#define HTTP_STATUS_CREATED               201
#define HTTP_STATUS_BAD_REQUEST           400
#define HTTP_STATUS_UNAUTHORIZED          401
#define HTTP_STATUS_FORBIDDEN             403
#define HTTP_STATUS_NOT_FOUND             404
#define HTTP_STATUS_METHOD_NOT_ALLOWED    405
#define HTTP_STATUS_CONFLICT              409
#define HTTP_STATUS_INTERNAL_ERROR        500
#define HTTP_STATUS_SERVICE_UNAVAILABLE   503

// 🔧 **API REQUEST STRUCTURE**
typedef struct {
    char method[16];                         // HTTP method (GET, POST, PUT, DELETE)
    char path[256];                          // Request path
    char query_string[512];                  // Query string parameters
    char *body;                              // Request body (JSON/YAML)
    size_t body_length;                      // Body length
    
    // Headers
    char content_type[64];                   // Content-Type header
    char authorization[256];                 // Authorization header
    char user_agent[128];                    // User-Agent header
    
    // Request metadata
    uint64_t request_timestamp;              // Request timestamp
    char client_ip[64];                      // Client IP address
    uint32_t request_id;                     // Unique request ID
    
} safety_api_request_t;

/**
 * @brief API Response Structure
 */
typedef struct {
    uint32_t status_code;                    // HTTP status code
    char *body;                              // Response body (JSON)
    size_t body_length;                      // Body length
    size_t body_capacity;                    // Body buffer capacity
    
    // Headers
    char content_type[64];                   // Content-Type header
    char cache_control[64];                  // Cache-Control header
    
    // Response metadata
    uint64_t response_timestamp;             // Response timestamp
    uint32_t processing_time_ms;             // Processing time
    bool success;                            // Request was successful
    
} safety_api_response_t;

/**
 * @brief API Statistics
 */
typedef struct {
    // Request statistics
    uint32_t total_requests;                 // Total API requests
    uint32_t successful_requests;            // Successful requests
    uint32_t failed_requests;                // Failed requests
    uint32_t unauthorized_requests;          // Unauthorized requests
    
    // Per-endpoint statistics
    uint32_t config_get_requests;            // GET /config requests
    uint32_t config_put_requests;            // PUT /config requests
    uint32_t config_reset_requests;          // POST /config/reset requests
    uint32_t status_requests;                // GET /status requests
    uint32_t statistics_requests;            // GET /statistics requests
    uint32_t modules_requests;               // GET /modules requests
    uint32_t diagnostics_requests;           // GET /diagnostics requests
    
    // Performance statistics
    uint32_t min_processing_time_ms;         // Minimum processing time
    uint32_t max_processing_time_ms;         // Maximum processing time
    uint32_t avg_processing_time_ms;         // Average processing time
    
    // Error statistics
    uint32_t validation_errors;              // Configuration validation errors
    uint32_t file_errors;                    // File operation errors
    uint32_t system_errors;                  // System errors
    
} safety_api_statistics_t;

// 🔧 **FUNCTION PROTOTYPES**

// API System Management

/**
 * @brief Initialize Safety Configuration API
 * @return HAL status
 * 
 * 🌐 INITIALIZATION: Sets up HTTP API endpoints
 */
hal_status_t safety_config_api_init(void);

/**
 * @brief Deinitialize Safety Configuration API
 * @return HAL status
 */
hal_status_t safety_config_api_deinit(void);

/**
 * @brief Start Safety Configuration API server
 * @param port HTTP server port (default: 8080)
 * @return HAL status
 * 
 * 🚀 SERVER START: Starts HTTP API server
 */
hal_status_t safety_config_api_start_server(uint16_t port);

/**
 * @brief Stop Safety Configuration API server
 * @return HAL status
 */
hal_status_t safety_config_api_stop_server(void);

// Request Handling Functions

/**
 * @brief Handle HTTP API request
 * @param request HTTP request structure
 * @param response HTTP response structure
 * @return HAL status
 * 
 * 🌐 REQUEST HANDLER: Main HTTP request dispatcher
 */
hal_status_t safety_config_api_handle_request(const safety_api_request_t *request,
                                             safety_api_response_t *response);

/**
 * @brief Handle GET /api/v1/safety/config
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 * 
 * 📄 GET CONFIG: Returns current safety configuration as JSON
 */
hal_status_t safety_config_api_get_config(const safety_api_request_t *request,
                                         safety_api_response_t *response);

/**
 * @brief Handle PUT /api/v1/safety/config
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 * 
 * 🔧 UPDATE CONFIG: Updates safety configuration from JSON
 * 🛡️ VALIDATION: Validates configuration before applying
 * 💾 BACKUP: Creates backup before applying changes
 */
hal_status_t safety_config_api_put_config(const safety_api_request_t *request,
                                         safety_api_response_t *response);

/**
 * @brief Handle POST /api/v1/safety/config/reset
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 * 
 * 🏭 FACTORY RESET: Resets configuration to factory defaults
 */
hal_status_t safety_config_api_reset_config(const safety_api_request_t *request,
                                           safety_api_response_t *response);

/**
 * @brief Handle GET /api/v1/safety/status
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 * 
 * 📊 STATUS: Returns current safety system status as JSON
 */
hal_status_t safety_config_api_get_status(const safety_api_request_t *request,
                                         safety_api_response_t *response);

/**
 * @brief Handle GET /api/v1/safety/statistics
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 * 
 * 📈 STATISTICS: Returns safety system statistics as JSON
 */
hal_status_t safety_config_api_get_statistics(const safety_api_request_t *request,
                                             safety_api_response_t *response);

/**
 * @brief Handle GET /api/v1/safety/modules
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 * 
 * 📊 MODULES: Returns module status and health information as JSON
 */
hal_status_t safety_config_api_get_modules(const safety_api_request_t *request,
                                          safety_api_response_t *response);

/**
 * @brief Handle GET /api/v1/safety/diagnostics
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 * 
 * 🔍 DIAGNOSTICS: Returns comprehensive diagnostic information as JSON
 */
hal_status_t safety_config_api_get_diagnostics(const safety_api_request_t *request,
                                              safety_api_response_t *response);

// Utility Functions

/**
 * @brief Create API response structure
 * @param response Response structure to initialize
 * @param initial_capacity Initial body buffer capacity
 * @return HAL status
 */
hal_status_t safety_config_api_create_response(safety_api_response_t *response, size_t initial_capacity);

/**
 * @brief Free API response structure
 * @param response Response structure to free
 * @return HAL status
 */
hal_status_t safety_config_api_free_response(safety_api_response_t *response);

/**
 * @brief Set response body (JSON)
 * @param response Response structure
 * @param json_body JSON body string
 * @return HAL status
 */
hal_status_t safety_config_api_set_response_body(safety_api_response_t *response, const char *json_body);

/**
 * @brief Set response status
 * @param response Response structure
 * @param status_code HTTP status code
 * @param message Status message
 * @return HAL status
 */
hal_status_t safety_config_api_set_response_status(safety_api_response_t *response, 
                                                  uint32_t status_code, 
                                                  const char *message);

/**
 * @brief Parse JSON request body
 * @param request Request structure
 * @param config Output configuration structure
 * @return HAL status
 * 
 * 📄 JSON PARSING: Parses JSON request body to configuration structure
 */
hal_status_t safety_config_api_parse_json_config(const safety_api_request_t *request,
                                                safety_system_config_t *config);

/**
 * @brief Convert configuration to JSON
 * @param config Configuration structure
 * @param json_output Output JSON string buffer
 * @param max_len Maximum JSON string length
 * @return HAL status
 * 
 * 📄 JSON GENERATION: Converts configuration structure to JSON
 */
hal_status_t safety_config_api_config_to_json(const safety_system_config_t *config,
                                             char *json_output, size_t max_len);

// Authentication and Security

/**
 * @brief Authenticate API request
 * @param request Request to authenticate
 * @return HAL status
 * 
 * 🔒 AUTHENTICATION: Validates request authorization
 */
hal_status_t safety_config_api_authenticate_request(const safety_api_request_t *request);

/**
 * @brief Check if operation is authorized
 * @param request Request to check
 * @param operation Operation to authorize
 * @return HAL status
 * 
 * 🛡️ AUTHORIZATION: Checks if operation is authorized
 */
hal_status_t safety_config_api_authorize_operation(const safety_api_request_t *request, 
                                                  const char *operation);

/**
 * @brief Log API access
 * @param request Request to log
 * @param response Response to log
 * @return HAL status
 * 
 * 📝 AUDIT LOG: Logs API access for security audit
 */
hal_status_t safety_config_api_log_access(const safety_api_request_t *request,
                                         const safety_api_response_t *response);

// Statistics and Monitoring

/**
 * @brief Get API statistics
 * @param stats Output statistics structure
 * @return HAL status
 */
hal_status_t safety_config_api_get_api_statistics(safety_api_statistics_t *stats);

/**
 * @brief Reset API statistics
 * @return HAL status
 */
hal_status_t safety_config_api_reset_statistics(void);

/**
 * @brief Get API health status
 * @return HAL status
 * 
 * 🏥 HEALTH CHECK: Returns API server health status
 */
hal_status_t safety_config_api_health_check(void);

// Testing Functions

/**
 * @brief Perform API self-test
 * @return HAL status
 * 
 * 🧪 TESTING: Tests all API endpoints
 */
hal_status_t safety_config_api_self_test(void);

/**
 * @brief Get API diagnostic information
 * @param info Buffer for diagnostic info
 * @param max_len Maximum buffer length
 * @return HAL status
 */
hal_status_t safety_config_api_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Enable/disable API debug logging
 * @param enable Enable debug logging
 * @return HAL status
 */
hal_status_t safety_config_api_set_debug_logging(bool enable);

#ifdef __cplusplus
}
#endif

#endif // SAFETY_CONFIG_API_H

/**
 * 📋 **API ENDPOINT DOCUMENTATION:**
 * 
 * 🌐 **GET /api/v1/safety/config**
 * - **Description:** Get current safety configuration
 * - **Method:** GET
 * - **Authentication:** Required
 * - **Response:** JSON configuration object
 * - **Example:**
 * ```json
 * {
 *   "success": true,
 *   "data": {
 *     "config_version": "1.0.0",
 *     "system_name": "OHT-50 Master Module",
 *     "global_settings": { ... },
 *     "modules": { ... }
 *   },
 *   "timestamp": "2025-09-19T16:30:00Z"
 * }
 * ```
 * 
 * 🔧 **PUT /api/v1/safety/config**
 * - **Description:** Update safety configuration
 * - **Method:** PUT
 * - **Authentication:** Required (Admin)
 * - **Request Body:** JSON configuration object
 * - **Response:** Success/failure with validation results
 * - **Example Request:**
 * ```json
 * {
 *   "global_settings": {
 *     "enable_module_monitoring": true,
 *     "system_update_interval_ms": 100
 *   },
 *   "modules": {
 *     "safety_module": {
 *       "timeout_ms": 0,
 *       "safety_action": "ESTOP_IMMEDIATE"
 *     }
 *   }
 * }
 * ```
 * 
 * 🏭 **POST /api/v1/safety/config/reset**
 * - **Description:** Reset configuration to factory defaults
 * - **Method:** POST
 * - **Authentication:** Required (Admin)
 * - **Response:** Success/failure message
 * 
 * 📊 **GET /api/v1/safety/status**
 * - **Description:** Get current safety system status
 * - **Method:** GET
 * - **Authentication:** Optional
 * - **Response:** JSON status object
 * - **Example:**
 * ```json
 * {
 *   "success": true,
 *   "data": {
 *     "system_active": true,
 *     "response_level": "NORMAL",
 *     "modules_online": 4,
 *     "modules_failed": 0,
 *     "emergency_triggered": false,
 *     "last_check_time": "2025-09-19T16:30:00Z"
 *   }
 * }
 * ```
 * 
 * 📈 **GET /api/v1/safety/statistics**
 * - **Description:** Get safety system statistics
 * - **Method:** GET
 * - **Authentication:** Optional
 * - **Response:** JSON statistics object
 * 
 * 📊 **GET /api/v1/safety/modules**
 * - **Description:** Get module status and health information
 * - **Method:** GET
 * - **Authentication:** Optional
 * - **Response:** JSON modules array
 * 
 * 🔍 **GET /api/v1/safety/diagnostics**
 * - **Description:** Get comprehensive diagnostic information
 * - **Method:** GET
 * - **Authentication:** Required
 * - **Response:** JSON diagnostics object
 * 
 * 📋 **SECURITY FEATURES:**
 * 
 * - **Authentication:** Bearer token authentication
 * - **Authorization:** Role-based access control
 * - **Audit Logging:** All configuration changes logged
 * - **Rate Limiting:** Prevents API abuse
 * - **Input Validation:** Comprehensive request validation
 * 
 * 📋 **ERROR HANDLING:**
 * 
 * - **400 Bad Request:** Invalid request format or parameters
 * - **401 Unauthorized:** Missing or invalid authentication
 * - **403 Forbidden:** Insufficient permissions
 * - **404 Not Found:** Endpoint not found
 * - **409 Conflict:** Configuration conflict or validation error
 * - **500 Internal Error:** System error
 * - **503 Service Unavailable:** System not ready
 */
