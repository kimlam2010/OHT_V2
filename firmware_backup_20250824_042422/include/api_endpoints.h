/**
 * @file api_endpoints.h
 * @brief API Endpoints for OHT-50 Master Module Firmware
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-03 (API Endpoints Implementation)
 */

#ifndef API_ENDPOINTS_H
#define API_ENDPOINTS_H

#include "http_server.h"
#include "hal_common.h"

// API Version
#define API_VERSION "v1"
#define API_BASE_PATH "/api/v1"

// API Endpoints
#define API_SYSTEM_STATUS     "/system/status"
#define API_SYSTEM_HEALTH     "/system/health"
#define API_MODULES           "/modules"
#define API_MODULE_COMMAND    "/modules/{id}/command"
#define API_SAFETY_STATUS     "/safety/status"
#define API_SAFETY_ESTOP      "/safety/estop"
#define API_CONFIG            "/config"
#define API_DIAGNOSTICS       "/diagnostics"

// API Response Structures
typedef struct {
    char system_name[64];
    char version[32];
    char status[32];
    uint64_t uptime_ms;
    uint32_t active_modules;
    bool estop_active;
    bool safety_ok;
} api_system_status_t;

typedef struct {
    char status[32];
    uint64_t timestamp;
    uint32_t response_time_ms;
    char details[256];
} api_health_status_t;

typedef struct {
    uint8_t module_id;
    char module_type[32];
    char status[32];
    bool online;
    uint64_t last_seen;
    char version[32];
} api_module_info_t;

typedef struct {
    api_module_info_t modules[16];
    uint32_t module_count;
} api_modules_list_t;

typedef struct {
    char command[64];
    char parameters[256];
    uint32_t timeout_ms;
} api_module_command_t;

typedef struct {
    bool estop_active;
    bool safety_ok;
    uint32_t safety_level;
    char safety_message[256];
    uint64_t last_safety_check;
} api_safety_status_t;

typedef struct {
    char estop_reason[128];
    uint64_t timestamp;
    bool acknowledged;
} api_estop_request_t;

typedef struct {
    char config_data[1024];
    uint32_t config_version;
    uint64_t last_updated;
} api_config_data_t;

typedef struct {
    uint32_t total_requests;
    uint32_t successful_requests;
    uint32_t failed_requests;
    uint64_t uptime_ms;
    char system_info[512];
    char error_log[1024];
} api_diagnostics_t;

// API Endpoint Handler Functions

/**
 * @brief Handle GET /api/v1/system/status
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_system_status(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle GET /api/v1/system/health
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_system_health(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle GET /api/v1/modules
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_modules_list(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle GET /api/v1/modules/{id}
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_module_info(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle POST /api/v1/modules/{id}/command
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_module_command(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle GET /api/v1/safety/status
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_safety_status(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle POST /api/v1/safety/estop
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_safety_estop(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle GET /api/v1/config
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_config_get(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle PUT /api/v1/config
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_config_set(const http_request_t *request, http_response_t *response);

/**
 * @brief Handle GET /api/v1/diagnostics
 * @param request HTTP request
 * @param response HTTP response
 * @return HAL status
 */
hal_status_t api_handle_diagnostics(const http_request_t *request, http_response_t *response);

// API Utility Functions

/**
 * @brief Initialize API endpoints
 * @param server HTTP server instance
 * @return HAL status
 */
hal_status_t api_endpoints_init(http_server_t *server);

/**
 * @brief Deinitialize API endpoints
 * @param server HTTP server instance
 * @return HAL status
 */
hal_status_t api_endpoints_deinit(http_server_t *server);

/**
 * @brief Extract module ID from path
 * @param path Request path
 * @return Module ID or -1 if not found
 */
int api_extract_module_id(const char *path);

/**
 * @brief Create JSON response from system status
 * @param status System status
 * @param json_buffer Output buffer
 * @param buffer_size Buffer size
 * @return HAL status
 */
hal_status_t api_create_system_status_json(const api_system_status_t *status, char *json_buffer, int buffer_size);

/**
 * @brief Create JSON response from modules list
 * @param modules Modules list
 * @param json_buffer Output buffer
 * @param buffer_size Buffer size
 * @return HAL status
 */
hal_status_t api_create_modules_list_json(const api_modules_list_t *modules, char *json_buffer, int buffer_size);

/**
 * @brief Create JSON response from safety status
 * @param safety Safety status
 * @param json_buffer Output buffer
 * @param buffer_size Buffer size
 * @return HAL status
 */
hal_status_t api_create_safety_status_json(const api_safety_status_t *safety, char *json_buffer, int buffer_size);

/**
 * @brief Create JSON response from diagnostics
 * @param diagnostics Diagnostics data
 * @param json_buffer Output buffer
 * @param buffer_size Buffer size
 * @return HAL status
 */
hal_status_t api_create_diagnostics_json(const api_diagnostics_t *diagnostics, char *json_buffer, int buffer_size);

/**
 * @brief Parse JSON request body
 * @param body Request body
 * @param json_buffer Output buffer
 * @param buffer_size Buffer size
 * @return HAL status
 */
hal_status_t api_parse_json_body(const char *body, char *json_buffer, int buffer_size);

/**
 * @brief Validate API request
 * @param request HTTP request
 * @param required_method Required HTTP method
 * @param required_path Required path pattern
 * @return HAL status
 */
hal_status_t api_validate_request(const http_request_t *request, http_method_t required_method, const char *required_path);

/**
 * @brief Create API error response
 * @param response HTTP response
 * @param error_code Error code
 * @param error_message Error message
 * @return HAL status
 */
hal_status_t api_create_error_response(http_response_t *response, http_status_t error_code, const char *error_message);

/**
 * @brief Create API success response
 * @param response HTTP response
 * @param data JSON data
 * @return HAL status
 */
hal_status_t api_create_success_response(http_response_t *response, const char *data);

#endif // API_ENDPOINTS_H
