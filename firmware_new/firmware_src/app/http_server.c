/**
 * @file http_server.c
 * @brief HTTP Server Foundation Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>

// Global HTTP Server Instance
http_server_instance_t g_http_server = {0};

// Memory management and timeout configuration
#define HTTP_SERVER_SOCKET_TIMEOUT_SEC 30
#define HTTP_SERVER_MAX_CONNECTIONS 10
#define HTTP_SERVER_BUFFER_SIZE 8192
#define HTTP_SERVER_MEMORY_CHECK_INTERVAL 1000

// Private function declarations
static hal_status_t http_server_setup_signal_handlers(void);
static hal_status_t http_server_cleanup_resources(void);
static hal_status_t http_server_validate_config(const http_server_config_t *config);
static hal_status_t http_server_find_route(const char *path, http_method_t method, http_route_t **route);
static hal_status_t http_server_parse_request_line(const char *line, http_request_t *request);
static hal_status_t http_server_parse_headers(const char *raw_headers, http_request_t *request);
static hal_status_t http_server_parse_body(const char *raw_body, size_t body_length, http_request_t *request);
static hal_status_t http_server_send_response(int client_socket, const http_response_t *response);
static hal_status_t http_server_read_request(int client_socket, char *buffer, size_t buffer_size, size_t *received_length);
static hal_status_t http_server_set_socket_timeout(int socket_fd, int timeout_seconds);
static hal_status_t http_server_check_memory_usage(void);
static hal_status_t http_server_cleanup_connection(int client_socket);

// Signal handler for graceful shutdown
static volatile bool g_http_shutdown_requested = false;
static void http_server_signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Shutdown signal received");
        g_http_shutdown_requested = true;
    }
}

/**
 * @brief Initialize HTTP Server with configuration
 * @param config Pointer to configuration structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_init(const http_server_config_t *config) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Initializing...");
    
    // Validate input parameters
    if (config == NULL) {
        hal_log_error("HTTP_SERVER", "http_server_init", __LINE__, 
                     HAL_STATUS_INVALID_PARAMETER, "Config parameter is NULL");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    hal_status_t validation_result = http_server_validate_config(config);
    if (validation_result != HAL_STATUS_OK) {
        hal_log_error("HTTP_SERVER", "http_server_init", __LINE__, 
                     validation_result, "Configuration validation failed");
        return validation_result;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&g_http_server.mutex, NULL) != 0) {
        hal_log_error("HTTP_SERVER", "http_server_init", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to initialize mutex");
        return HAL_STATUS_ERROR;
    }
    
    // Copy configuration
    memcpy(&g_http_server.config, config, sizeof(http_server_config_t));
    
    // Initialize status
    memset(&g_http_server.status, 0, sizeof(http_server_status_t));
    g_http_server.status.overall_status = HAL_DEVICE_STATUS_INITIALIZING;
    
    // Initialize routes array
    memset(g_http_server.routes, 0, sizeof(g_http_server.routes));
    g_http_server.route_count = 0;
    
    // Initialize server socket
    g_http_server.server_socket = -1;
    
    // Set initialization flags
    g_http_server.initialized = true;
    g_http_server.running = false;
    
    // Setup signal handlers
    hal_status_t signal_result = http_server_setup_signal_handlers();
    if (signal_result != HAL_STATUS_OK) {
        hal_log_error("HTTP_SERVER", "http_server_init", __LINE__, 
                     signal_result, "Failed to setup signal handlers");
        http_server_deinit();
        return signal_result;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Initialized successfully");
    g_http_server.status.overall_status = HAL_DEVICE_STATUS_OK;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start HTTP Server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_start(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Starting on port %d", g_http_server.config.port);
    
    if (!g_http_server.initialized) {
        hal_log_error("HTTP_SERVER", "http_server_start", __LINE__, 
                     HAL_STATUS_NOT_INITIALIZED, "HTTP Server not initialized");
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (g_http_server.running) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "HTTP Server: Already running");
        return HAL_STATUS_ALREADY_ACTIVE;
    }
    
    // Create socket
    g_http_server.server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_http_server.server_socket < 0) {
        hal_log_error("HTTP_SERVER", "http_server_start", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to create server socket: %s", strerror(errno));
        return HAL_STATUS_IO_ERROR;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(g_http_server.server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        hal_log_error("HTTP_SERVER", "http_server_start", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to set socket options: %s", strerror(errno));
        close(g_http_server.server_socket);
        g_http_server.server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(g_http_server.config.port);
    
    if (bind(g_http_server.server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        hal_log_error("HTTP_SERVER", "http_server_start", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to bind server socket: %s", strerror(errno));
        close(g_http_server.server_socket);
        g_http_server.server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Listen for connections
    if (listen(g_http_server.server_socket, g_http_server.config.max_connections) < 0) {
        hal_log_error("HTTP_SERVER", "http_server_start", __LINE__, 
                     HAL_STATUS_IO_ERROR, "Failed to listen on server socket: %s", strerror(errno));
        close(g_http_server.server_socket);
        g_http_server.server_socket = -1;
        return HAL_STATUS_IO_ERROR;
    }
    
    // Start server thread
    if (pthread_create(&g_http_server.server_thread, NULL, http_server_thread, NULL) != 0) {
        hal_log_error("HTTP_SERVER", "http_server_start", __LINE__, 
                     HAL_STATUS_ERROR, "Failed to create server thread: %s", strerror(errno));
        close(g_http_server.server_socket);
        g_http_server.server_socket = -1;
        return HAL_STATUS_ERROR;
    }
    
    g_http_server.running = true;
    g_http_server.status.running = true;
    g_http_server.status.listening = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Started successfully on port %d", g_http_server.config.port);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Stop HTTP Server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_stop(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Stopping...");
    
    if (!g_http_server.running) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "HTTP Server: Not running");
        return HAL_STATUS_OK;
    }
    
    g_http_server.running = false;
    g_http_server.status.running = false;
    g_http_server.status.listening = false;
    
    // Close server socket
    if (g_http_server.server_socket >= 0) {
        close(g_http_server.server_socket);
        g_http_server.server_socket = -1;
    }
    
    // Wait for server thread to finish
    if (g_http_server.server_thread) {
        pthread_join(g_http_server.server_thread, NULL);
        g_http_server.server_thread = 0;
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Stopped successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize HTTP Server
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Deinitializing...");
    
    // Stop server if running
    if (g_http_server.running) {
        http_server_stop();
    }
    
    // Cleanup resources
    hal_status_t cleanup_result = http_server_cleanup_resources();
    if (cleanup_result != HAL_STATUS_OK) {
        hal_log_error("HTTP_SERVER", "http_server_deinit", __LINE__, 
                     cleanup_result, "Failed to cleanup resources");
    }
    
    // Destroy mutex
    if (g_http_server.initialized) {
        pthread_mutex_destroy(&g_http_server.mutex);
    }
    
    // Reset instance
    memset(&g_http_server, 0, sizeof(http_server_instance_t));
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Deinitialized successfully");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get HTTP Server status
 * @param status Pointer to status structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_get_status(http_server_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_http_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_http_server.mutex);
    
    // Update statistics
    g_http_server.status.statistics.uptime_ms = hal_get_timestamp_ms();
    
    // Copy status
    memcpy(status, &g_http_server.status, sizeof(http_server_status_t));
    
    pthread_mutex_unlock(&g_http_server.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Register HTTP route
 * @param route Pointer to route structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_register_route(const http_route_t *route) {
    if (route == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_http_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_http_server.mutex);
    
    // Check if route already exists
    http_route_t *existing_route;
    hal_status_t find_result = http_server_find_route(route->path, route->method, &existing_route);
    if (find_result == HAL_STATUS_OK) {
        pthread_mutex_unlock(&g_http_server.mutex);
        hal_log_message(HAL_LOG_LEVEL_WARNING, "HTTP Server: Route %s %s already exists", 
                       route->path, http_method_to_string(route->method));
        return HAL_STATUS_ALREADY_EXISTS;
    }
    
    // Check if we have space for more routes
    if (g_http_server.route_count >= 64) {
        pthread_mutex_unlock(&g_http_server.mutex);
        hal_log_error("HTTP_SERVER", "http_server_register_route", __LINE__, 
                     HAL_STATUS_NO_MEMORY, "Maximum number of routes reached");
        return HAL_STATUS_NO_MEMORY;
    }
    
    // Add route
    memcpy(&g_http_server.routes[g_http_server.route_count], route, sizeof(http_route_t));
    g_http_server.route_count++;
    
    pthread_mutex_unlock(&g_http_server.mutex);
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Registered route %s %s", 
                   route->path, http_method_to_string(route->method));
    
    return HAL_STATUS_OK;
}

/**
 * @brief Handle HTTP request
 * @param request Pointer to request structure
 * @param response Pointer to response structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_handle_request(const http_request_t *request, http_response_t *response) {
    if (request == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_http_server.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Handle OPTIONS preflight requests
    if (request->method == HTTP_METHOD_OPTIONS) {
        return http_server_handle_cors_preflight(request, response);
    }
    
    // Find matching route
    http_route_t *route;
    hal_status_t find_result = http_server_find_route(request->path, request->method, &route);
    
    if (find_result != HAL_STATUS_OK) {
        // Route not found
        return http_server_create_error_response(response, HTTP_STATUS_NOT_FOUND, "Route not found");
    }
    
    // Check authentication if required
    if (route->requires_authentication) {
        hal_status_t auth_result = http_server_validate_authentication(request);
        if (auth_result != HAL_STATUS_OK) {
            return http_server_require_authentication(response);
        }
    }
    
    // Call route handler
    hal_status_t handler_result = route->handler(request, response);
    if (handler_result != HAL_STATUS_OK) {
        hal_log_error("HTTP_SERVER", "http_server_handle_request", __LINE__, 
                     handler_result, "Route handler failed for %s %s", 
                     http_method_to_string(request->method), request->path);
        return http_server_create_error_response(response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Internal server error");
    }
    
    // Add CORS headers if enabled
    if (g_http_server.config.enable_cors) {
        http_server_add_cors_headers(response);
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Parse HTTP request
 * @param raw_request Raw HTTP request string
 * @param request Pointer to request structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_parse_request(const char *raw_request, http_request_t *request) {
    if (raw_request == NULL || request == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize request structure
    memset(request, 0, sizeof(http_request_t));
    gettimeofday(&request->timestamp, NULL);
    
    // Find request line end
    const char *line_end = strstr(raw_request, "\r\n");
    if (line_end == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Parse request line
    char request_line[512];
    size_t line_length = line_end - raw_request;
    if (line_length >= sizeof(request_line)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    strncpy(request_line, raw_request, line_length);
    request_line[line_length] = '\0';
    
    hal_status_t line_result = http_server_parse_request_line(request_line, request);
    if (line_result != HAL_STATUS_OK) {
        return line_result;
    }
    
    // Find headers section
    const char *headers_start = line_end + 2;
    const char *headers_end = strstr(headers_start, "\r\n\r\n");
    if (headers_end == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Parse headers
    char headers_section[2048];
    size_t headers_length = headers_end - headers_start;
    if (headers_length >= sizeof(headers_section)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    strncpy(headers_section, headers_start, headers_length);
    headers_section[headers_length] = '\0';
    
    hal_status_t headers_result = http_server_parse_headers(headers_section, request);
    if (headers_result != HAL_STATUS_OK) {
        return headers_result;
    }
    
    // Parse body if present - use Content-Length header instead of strlen
    const char *body_start = headers_end + 4;
    size_t body_length = 0;
    
    // Find Content-Length header
    for (int i = 0; i < (int)request->header_count && i < HTTP_SERVER_MAX_HEADERS; i++) {
        if (strcasecmp(request->headers[i].name, "Content-Length") == 0) {
            body_length = (size_t)strtoul(request->headers[i].value, NULL, 10);
            break;
        }
    }
    
    // Validate body length
    if (body_length > 0) {
        if (body_length > HTTP_SERVER_MAX_REQUEST_SIZE) {
            hal_log_message(HAL_LOG_LEVEL_ERROR, "HTTP Server: Body too large: %zu bytes", body_length);
            return HAL_STATUS_ERROR;
        }
        
        hal_status_t body_result = http_server_parse_body(body_start, body_length, request);
        if (body_result != HAL_STATUS_OK) {
            return body_result;
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create success response
 * @param response Pointer to response structure
 * @param content_type Content type string
 * @param body Response body
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_create_success_response(http_response_t *response, const char *content_type, const char *body) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize response
    memset(response, 0, sizeof(http_response_t));
    response->status_code = HTTP_STATUS_OK;
    strcpy(response->version, "HTTP/1.1");
    gettimeofday(&response->timestamp, NULL);
    
    // Add content type header
    if (content_type != NULL) {
        http_server_add_header(response, "Content-Type", content_type);
    }
    
    // Set body
    if (body != NULL) {
        response->body_length = strlen(body);
        response->body = malloc(response->body_length + 1);
        if (response->body != NULL) {
            strcpy(response->body, body);
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create error response
 * @param response Pointer to response structure
 * @param status_code HTTP status code
 * @param error_message Error message
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_create_error_response(http_response_t *response, http_status_code_t status_code, const char *error_message) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize response
    memset(response, 0, sizeof(http_response_t));
    response->status_code = status_code;
    strcpy(response->version, "HTTP/1.1");
    gettimeofday(&response->timestamp, NULL);
    
    // Add content type header
    http_server_add_header(response, "Content-Type", "application/json");
    
    // Create error JSON
    char error_json[512];
    snprintf(error_json, sizeof(error_json), 
             "{\"error\":true,\"status_code\":%d,\"message\":\"%s\"}", 
             status_code, error_message ? error_message : "Unknown error");
    
    response->body_length = strlen(error_json);
    response->body = malloc(response->body_length + 1);
    if (response->body != NULL) {
        strcpy(response->body, error_json);
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Create JSON response
 * @param response Pointer to response structure
 * @param json_data JSON data string
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_create_json_response(http_response_t *response, const char *json_data) {
    return http_server_create_success_response(response, "application/json", json_data);
}

/**
 * @brief Add header to response
 * @param response Pointer to response structure
 * @param name Header name
 * @param value Header value
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_add_header(http_response_t *response, const char *name, const char *value) {
    if (response == NULL || name == NULL || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (response->header_count >= HTTP_SERVER_MAX_HEADERS) {
        return HAL_STATUS_NO_MEMORY;
    }
    
    strncpy(response->headers[response->header_count].name, name, 63);
    response->headers[response->header_count].name[63] = '\0';
    
    strncpy(response->headers[response->header_count].value, value, 255);
    response->headers[response->header_count].value[255] = '\0';
    
    response->header_count++;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Add CORS headers to response
 * @param response Pointer to response structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_add_cors_headers(http_response_t *response) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    http_server_add_header(response, "Access-Control-Allow-Origin", g_http_server.config.cors_origin);
    http_server_add_header(response, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    http_server_add_header(response, "Access-Control-Allow-Headers", "Content-Type, Authorization");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get HTTP method string
 * @param method HTTP method
 * @return const char* Method string
 */
const char* http_method_to_string(http_method_t method) {
    switch (method) {
        case HTTP_METHOD_GET: return "GET";
        case HTTP_METHOD_POST: return "POST";
        case HTTP_METHOD_PUT: return "PUT";
        case HTTP_METHOD_DELETE: return "DELETE";
        case HTTP_METHOD_PATCH: return "PATCH";
        case HTTP_METHOD_HEAD: return "HEAD";
        case HTTP_METHOD_OPTIONS: return "OPTIONS";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Convert string to HTTP method
 * @param method_str Method string
 * @return http_method_t HTTP method
 */
http_method_t http_string_to_method(const char *method_str) {
    if (method_str == NULL) {
        return HTTP_METHOD_UNKNOWN;
    }
    
    if (strcmp(method_str, "GET") == 0) return HTTP_METHOD_GET;
    if (strcmp(method_str, "POST") == 0) return HTTP_METHOD_POST;
    if (strcmp(method_str, "PUT") == 0) return HTTP_METHOD_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_METHOD_DELETE;
    if (strcmp(method_str, "PATCH") == 0) return HTTP_METHOD_PATCH;
    if (strcmp(method_str, "HEAD") == 0) return HTTP_METHOD_HEAD;
    if (strcmp(method_str, "OPTIONS") == 0) return HTTP_METHOD_OPTIONS;
    
    return HTTP_METHOD_UNKNOWN;
}

/**
 * @brief Get HTTP status string
 * @param status_code HTTP status code
 * @return const char* Status string
 */
const char* http_status_to_string(http_status_code_t status_code) {
    switch (status_code) {
        case HTTP_STATUS_OK: return "OK";
        case HTTP_STATUS_CREATED: return "Created";
        case HTTP_STATUS_NO_CONTENT: return "No Content";
        case HTTP_STATUS_BAD_REQUEST: return "Bad Request";
        case HTTP_STATUS_UNAUTHORIZED: return "Unauthorized";
        case HTTP_STATUS_FORBIDDEN: return "Forbidden";
        case HTTP_STATUS_NOT_FOUND: return "Not Found";
        case HTTP_STATUS_METHOD_NOT_ALLOWED: return "Method Not Allowed";
        case HTTP_STATUS_INTERNAL_SERVER_ERROR: return "Internal Server Error";
        case HTTP_STATUS_NOT_IMPLEMENTED: return "Not Implemented";
        case HTTP_STATUS_SERVICE_UNAVAILABLE: return "Service Unavailable";
        default: return "Unknown";
    }
}

/**
 * @brief Get HTTP Server version
 * @param major Major version number
 * @param minor Minor version number
 * @param patch Patch version number
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t http_server_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch) {
    if (major == NULL || minor == NULL || patch == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *major = HTTP_SERVER_VERSION_MAJOR;
    *minor = HTTP_SERVER_VERSION_MINOR;
    *patch = HTTP_SERVER_VERSION_PATCH;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get HTTP Server version string
 * @return const char* Version string
 */
const char* http_server_get_version_string(void) {
    return HTTP_SERVER_VERSION_STRING;
}

// Private function implementations
static hal_status_t http_server_validate_config(const http_server_config_t *config) {
    if (config->port == 0 || config->port > 65535) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_connections == 0 || config->max_connections > HTTP_SERVER_MAX_CONNECTIONS) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_request_size == 0 || config->max_request_size > HTTP_SERVER_MAX_REQUEST_SIZE) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_response_size == 0 || config->max_response_size > HTTP_SERVER_MAX_RESPONSE_SIZE) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t http_server_setup_signal_handlers(void) {
    signal(SIGINT, http_server_signal_handler);
    signal(SIGTERM, http_server_signal_handler);
    return HAL_STATUS_OK;
}

static hal_status_t http_server_cleanup_resources(void) {
    // Close server socket
    if (g_http_server.server_socket >= 0) {
        close(g_http_server.server_socket);
        g_http_server.server_socket = -1;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t http_server_find_route(const char *path, http_method_t method, http_route_t **route) {
    for (uint32_t i = 0; i < g_http_server.route_count; i++) {
        if (strcmp(g_http_server.routes[i].path, path) == 0 && 
            g_http_server.routes[i].method == method) {
            *route = &g_http_server.routes[i];
            return HAL_STATUS_OK;
        }
    }
    return HAL_STATUS_NOT_FOUND;
}

static hal_status_t http_server_parse_request_line(const char *line, http_request_t *request) {
    char method_str[16], path_str[256], version_str[16];
    
    if (sscanf(line, "%15s %255s %15s", method_str, path_str, version_str) != 3) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Parse method
    request->method = http_string_to_method(method_str);
    if (request->method == HTTP_METHOD_UNKNOWN) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Parse path and query string
    const char *query_start = strchr(path_str, '?');
    if (query_start != NULL) {
        size_t path_length = query_start - path_str;
        strncpy(request->path, path_str, path_length);
        request->path[path_length] = '\0';
        strncpy(request->query_string, query_start + 1, sizeof(request->query_string) - 1);
        request->query_string[sizeof(request->query_string) - 1] = '\0';
    } else {
        strncpy(request->path, path_str, sizeof(request->path) - 1);
        request->path[sizeof(request->path) - 1] = '\0';
    }
    
    // Parse version
    strncpy(request->version, version_str, sizeof(request->version) - 1);
    request->version[sizeof(request->version) - 1] = '\0';
    
    return HAL_STATUS_OK;
}

static hal_status_t http_server_parse_headers(const char *raw_headers, http_request_t *request) {
    const char *line_start = raw_headers;
    const char *line_end;
    
    while ((line_end = strstr(line_start, "\r\n")) != NULL) {
        if (request->header_count >= HTTP_SERVER_MAX_HEADERS) {
            break;
        }
        
        size_t line_length = line_end - line_start;
        if (line_length == 0) {
            break; // Empty line indicates end of headers
        }
        
        char header_line[512];
        if (line_length >= sizeof(header_line)) {
            line_length = sizeof(header_line) - 1;
        }
        
        strncpy(header_line, line_start, line_length);
        header_line[line_length] = '\0';
        
        // Parse header name and value
        const char *colon = strchr(header_line, ':');
        if (colon != NULL) {
            size_t name_length = colon - header_line;
            strncpy(request->headers[request->header_count].name, header_line, name_length);
            request->headers[request->header_count].name[name_length] = '\0';
            
            // Skip colon and whitespace
            const char *value_start = colon + 1;
            while (*value_start == ' ' || *value_start == '\t') {
                value_start++;
            }
            
            strncpy(request->headers[request->header_count].value, value_start, 
                   sizeof(request->headers[request->header_count].value) - 1);
            request->headers[request->header_count].value[sizeof(request->headers[request->header_count].value) - 1] = '\0';
            
            request->header_count++;
        }
        
        line_start = line_end + 2;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t http_server_parse_body(const char *raw_body, size_t body_length, http_request_t *request) {
    if (body_length > 0) {
        request->body = malloc(body_length + 1);
        if (request->body == NULL) {
            return HAL_STATUS_NO_MEMORY;
        }
        
        strncpy(request->body, raw_body, body_length);
        request->body[body_length] = '\0';
        request->body_length = body_length;
    }
    
    return HAL_STATUS_OK;
}

// HTTP Server Thread Function
void* http_server_thread(void *arg) {
    (void)arg; // Unused parameter
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Server thread started");
    
    while (!g_http_shutdown_requested && g_http_server.running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(g_http_server.server_socket, 
                                  (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by signal
            }
            hal_log_error("HTTP_SERVER", "http_server_thread", __LINE__, 
                         HAL_STATUS_IO_ERROR, "Failed to accept client connection: %s", strerror(errno));
            continue;
        }
        
        // Handle client request in a separate thread
        pthread_t client_thread;
        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;
        
        if (pthread_create(&client_thread, NULL, http_server_client_thread, client_socket_ptr) != 0) {
            hal_log_error("HTTP_SERVER", "http_server_thread", __LINE__, 
                         HAL_STATUS_ERROR, "Failed to create client thread: %s", strerror(errno));
            close(client_socket);
            free(client_socket_ptr);
        } else {
            pthread_detach(client_thread);
        }
    }
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "HTTP Server: Server thread stopped");
    return NULL;
}

// HTTP Client Thread Function
void* http_server_client_thread(void *arg) {
    int client_socket = *(int*)arg;
    free(arg);
    
    char request_buffer[HTTP_SERVER_MAX_REQUEST_SIZE];
    size_t received_length;
    
    // Read request
    hal_status_t read_result = http_server_read_request(client_socket, request_buffer, 
                                                       sizeof(request_buffer), &received_length);
    if (read_result != HAL_STATUS_OK) {
        hal_log_error("HTTP_SERVER", "http_server_client_thread", __LINE__, 
                     read_result, "Failed to read request from client");
        http_server_cleanup_connection(client_socket);
        return NULL;
    }
    
    // Parse request
    http_request_t request;
    hal_status_t parse_result = http_server_parse_request(request_buffer, &request);
    if (parse_result != HAL_STATUS_OK) {
        hal_log_error("HTTP_SERVER", "http_server_client_thread", __LINE__, 
                     parse_result, "Failed to parse request");
        http_server_cleanup_connection(client_socket);
        return NULL;
    }
    
    // Handle request
    http_response_t response;
    hal_status_t handle_result = http_server_handle_request(&request, &response);
    if (handle_result != HAL_STATUS_OK) {
        hal_log_error("HTTP_SERVER", "http_server_client_thread", __LINE__, 
                     handle_result, "Failed to handle request");
        http_server_cleanup_connection(client_socket);
        return NULL;
    }
    
    // Send response
    hal_status_t send_result = http_server_send_response(client_socket, &response);
    if (send_result != HAL_STATUS_OK) {
        hal_log_error("HTTP_SERVER", "http_server_client_thread", __LINE__, 
                     send_result, "Failed to send response");
    }
    
    // Cleanup memory and connection
    if (request.body != NULL) {
        free(request.body);
    }
    if (response.body != NULL) {
        free(response.body);
    }
    
    http_server_cleanup_connection(client_socket);
    return NULL;
}

// Helper functions for timeout and memory management
/**
 * @brief Set socket timeout to prevent hanging connections
 * @param socket_fd Socket file descriptor
 * @param timeout_seconds Timeout in seconds
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
static hal_status_t http_server_set_socket_timeout(int socket_fd, int timeout_seconds) {
    struct timeval timeout;
    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        hal_log_error("HTTP_SERVER", "http_server_set_socket_timeout", __LINE__,
                     HAL_STATUS_ERROR, "Failed to set receive timeout");
        return HAL_STATUS_ERROR;
    }
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        hal_log_error("HTTP_SERVER", "http_server_set_socket_timeout", __LINE__,
                     HAL_STATUS_ERROR, "Failed to set send timeout");
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Check memory usage and log warnings if high
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
static hal_status_t http_server_check_memory_usage(void) {
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    char line[256];
    long mem_total = 0, mem_available = 0;
    
    while (fgets(line, sizeof(line), meminfo)) {
        if (sscanf(line, "MemTotal: %ld kB", &mem_total) == 1) {
            continue;
        }
        if (sscanf(line, "MemAvailable: %ld kB", &mem_available) == 1) {
            break;
        }
    }
    fclose(meminfo);
    
    if (mem_total > 0 && mem_available > 0) {
        double usage_percent = ((double)(mem_total - mem_available) / mem_total) * 100.0;
        if (usage_percent > 85.0) {
            hal_log_message(HAL_LOG_LEVEL_WARNING, 
                           "HTTP Server: High memory usage detected: %.1f%%", usage_percent);
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Cleanup connection resources properly
 * @param client_socket Client socket to cleanup
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
static hal_status_t http_server_cleanup_connection(int client_socket) {
    if (client_socket >= 0) {
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        hal_log_message(HAL_LOG_LEVEL_DEBUG, "HTTP Server: Connection cleaned up");
    }
    return HAL_STATUS_OK;
}

static const char* http_server_get_status_text(int status_code) {
    switch (status_code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 408: return "Request Timeout";
        case 413: return "Payload Too Large";
        case 429: return "Too Many Requests";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}

// Enhanced request reading with Content-Length support
static hal_status_t http_server_read_request(int client_socket, char *buffer, size_t buffer_size, size_t *received_length) {
    // Set socket timeout to prevent hanging
    hal_status_t timeout_result = http_server_set_socket_timeout(client_socket, HTTP_SERVER_SOCKET_TIMEOUT_SEC);
    if (timeout_result != HAL_STATUS_OK) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "HTTP Server: Failed to set socket timeout");
    }
    
    // Check memory usage periodically
    static uint64_t last_memory_check = 0;
    uint64_t current_time = hal_get_timestamp_ms();
    if (current_time - last_memory_check > HTTP_SERVER_MEMORY_CHECK_INTERVAL) {
        http_server_check_memory_usage();
        last_memory_check = current_time;
    }
    
    size_t total_received = 0;
    size_t headers_end_pos = 0;
    size_t expected_body_length = 0;
    bool headers_complete = false;
    
    // Read in chunks until we have complete request
    while (total_received < buffer_size - 1) {
        ssize_t bytes_received = recv(client_socket, 
                                     buffer + total_received, 
                                     buffer_size - total_received - 1, 
                                     0);
        
    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            hal_log_error("HTTP_SERVER", "http_server_read_request", __LINE__,
                         HAL_STATUS_TIMEOUT, "Socket read timeout");
            return HAL_STATUS_TIMEOUT;
        } else {
            hal_log_error("HTTP_SERVER", "http_server_read_request", __LINE__,
                         HAL_STATUS_ERROR, "Socket read error: %s", strerror(errno));
            return HAL_STATUS_ERROR;
        }
    } else if (bytes_received == 0) {
        hal_log_message(HAL_LOG_LEVEL_DEBUG, "HTTP Server: Client disconnected");
        return HAL_STATUS_ERROR;
    }
    
        total_received += bytes_received;
        buffer[total_received] = '\0';
        
        // Check if headers are complete
        if (!headers_complete) {
            const char *headers_end = strstr(buffer, "\r\n\r\n");
            if (headers_end != NULL) {
                headers_complete = true;
                headers_end_pos = (headers_end - buffer) + 4;
                
                // Parse Content-Length from headers
                const char *content_length_start = strstr(buffer, "Content-Length:");
                if (content_length_start != NULL) {
                    content_length_start += 15; // Skip "Content-Length:"
                    while (*content_length_start == ' ' || *content_length_start == '\t') {
                        content_length_start++;
                    }
                    expected_body_length = (size_t)strtoul(content_length_start, NULL, 10);
                }
            }
        }
        
        // Check if we have complete request
        if (headers_complete) {
            size_t current_body_length = total_received - headers_end_pos;
            if (current_body_length >= expected_body_length) {
                break; // Complete request received
            }
        }
        
        // Prevent infinite loop for requests without body
        if (headers_complete && expected_body_length == 0) {
            break;
        }
    }
    
    *received_length = total_received;
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "HTTP Server: Read %zu bytes from client (headers: %zu, body: %zu)", 
                    total_received, headers_end_pos, total_received - headers_end_pos);
    
    return HAL_STATUS_OK;
}

static hal_status_t http_server_send_response(int client_socket, const http_response_t *response) {
    if (!response || client_socket < 0) {
        return HAL_STATUS_ERROR;
    }
    
    // Serialize response to buffer
    char response_buffer[HTTP_SERVER_MAX_RESPONSE_SIZE];
    size_t buffer_used = 0;
    
    // Build status line
    int status_line_len = snprintf(response_buffer + buffer_used, 
                                   sizeof(response_buffer) - buffer_used,
                                   "HTTP/1.1 %d %s\r\n",
                                   response->status_code,
                                   http_server_get_status_text(response->status_code));
    if (status_line_len < 0 || status_line_len >= (int)(sizeof(response_buffer) - buffer_used)) {
        hal_log_message(HAL_LOG_LEVEL_ERROR, "HTTP Server: Response buffer too small for status line");
        return HAL_STATUS_ERROR;
    }
    buffer_used += status_line_len;
    
    // Add headers
    for (int i = 0; i < (int)response->header_count && i < HTTP_SERVER_MAX_HEADERS; i++) {
        int header_len = snprintf(response_buffer + buffer_used,
                                  sizeof(response_buffer) - buffer_used,
                                  "%s: %s\r\n",
                                  response->headers[i].name,
                                  response->headers[i].value);
        if (header_len < 0 || header_len >= (int)(sizeof(response_buffer) - buffer_used)) {
            hal_log_message(HAL_LOG_LEVEL_ERROR, "HTTP Server: Response buffer too small for headers");
            return HAL_STATUS_ERROR;
        }
        buffer_used += header_len;
    }
    
    // Add Content-Length header if body exists
    if (response->body && response->body_length > 0) {
        int content_len_header = snprintf(response_buffer + buffer_used,
                                          sizeof(response_buffer) - buffer_used,
                                          "Content-Length: %zu\r\n",
                                          response->body_length);
        if (content_len_header < 0 || content_len_header >= (int)(sizeof(response_buffer) - buffer_used)) {
            hal_log_message(HAL_LOG_LEVEL_ERROR, "HTTP Server: Response buffer too small for Content-Length");
            return HAL_STATUS_ERROR;
        }
        buffer_used += content_len_header;
    }
    
    // End headers
    if (buffer_used + 2 >= sizeof(response_buffer)) {
        hal_log_message(HAL_LOG_LEVEL_ERROR, "HTTP Server: Response buffer too small for header end");
        return HAL_STATUS_ERROR;
    }
    strcpy(response_buffer + buffer_used, "\r\n");
    buffer_used += 2;
    
    // Send headers
    ssize_t bytes_sent = send(client_socket, response_buffer, buffer_used, 0);
    if (bytes_sent < 0 || (size_t)bytes_sent != buffer_used) {
        hal_log_message(HAL_LOG_LEVEL_ERROR, "HTTP Server: Failed to send response headers");
        return HAL_STATUS_ERROR;
    }
    
    // Send body if exists
    if (response->body && response->body_length > 0) {
        bytes_sent = send(client_socket, response->body, response->body_length, 0);
        if (bytes_sent < 0 || (size_t)bytes_sent != response->body_length) {
            hal_log_message(HAL_LOG_LEVEL_ERROR, "HTTP Server: Failed to send response body");
            return HAL_STATUS_ERROR;
        }
    }
    
    hal_log_message(HAL_LOG_LEVEL_DEBUG, "HTTP Server: Sent response %d, %zu bytes", 
                    response->status_code, buffer_used + (response->body ? response->body_length : 0));
    
    return HAL_STATUS_OK;
}

hal_status_t http_server_serialize_response(const http_response_t *response, char *buffer, size_t buffer_size) {
    if (!response || !buffer || buffer_size == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    size_t buffer_used = 0;
    
    // Build status line
    int status_line_len = snprintf(buffer + buffer_used, 
                                   buffer_size - buffer_used,
                                   "HTTP/1.1 %d %s\r\n",
                                   response->status_code,
                                   http_server_get_status_text(response->status_code));
    if (status_line_len < 0 || status_line_len >= (int)(buffer_size - buffer_used)) {
        return HAL_STATUS_ERROR;
    }
    buffer_used += status_line_len;
    
    // Add headers
    for (int i = 0; i < (int)response->header_count && i < HTTP_SERVER_MAX_HEADERS; i++) {
        int header_len = snprintf(buffer + buffer_used,
                                  buffer_size - buffer_used,
                                  "%s: %s\r\n",
                                  response->headers[i].name,
                                  response->headers[i].value);
        if (header_len < 0 || header_len >= (int)(buffer_size - buffer_used)) {
            return HAL_STATUS_ERROR;
        }
        buffer_used += header_len;
    }
    
    // Add Content-Length header if body exists
    if (response->body && response->body_length > 0) {
        int content_len_header = snprintf(buffer + buffer_used,
                                          buffer_size - buffer_used,
                                          "Content-Length: %zu\r\n",
                                          response->body_length);
        if (content_len_header < 0 || content_len_header >= (int)(buffer_size - buffer_used)) {
            return HAL_STATUS_ERROR;
        }
        buffer_used += content_len_header;
    }
    
    // End headers
    if (buffer_used + 2 >= buffer_size) {
        return HAL_STATUS_ERROR;
    }
    strcpy(buffer + buffer_used, "\r\n");
    buffer_used += 2;
    
    // Add body if exists
    if (response->body && response->body_length > 0) {
        if (buffer_used + response->body_length >= buffer_size) {
            return HAL_STATUS_ERROR;
        }
        memcpy(buffer + buffer_used, response->body, response->body_length);
        buffer_used += response->body_length;
    }
    
    // Null terminate
    if (buffer_used < buffer_size) {
        buffer[buffer_used] = '\0';
    }
    
    return HAL_STATUS_OK;
}

hal_status_t http_server_create_html_response(http_response_t *response, const char *html_content) {
    if (!response || !html_content) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize response
    memset(response, 0, sizeof(http_response_t));
    response->status_code = 200;
    
    // Add Content-Type header
    http_server_add_header(response, "Content-Type", "text/html; charset=utf-8");
    
    // Add CORS headers if configured
    if (g_http_server.config.cors_origin && strlen(g_http_server.config.cors_origin) > 0) {
        http_server_add_cors_headers(response);
    }
    
    // Set body
    response->body = (char*)html_content;
    response->body_length = strlen(html_content);
    
    return HAL_STATUS_OK;
}

hal_status_t http_server_handle_cors_preflight(const http_request_t *request, http_response_t *response) {
    if (!request || !response) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize response
    memset(response, 0, sizeof(http_response_t));
    response->status_code = 204; // No Content
    
    // Add CORS headers
    http_server_add_cors_headers(response);
    
    // Add Allow header with supported methods
    http_server_add_header(response, "Allow", "GET, POST, PUT, DELETE, OPTIONS");
    
    // Add Access-Control-Allow-Methods
    http_server_add_header(response, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    
    // Add Access-Control-Allow-Headers if requested
    const char *request_headers = NULL;
    for (int i = 0; i < (int)request->header_count && i < HTTP_SERVER_MAX_HEADERS; i++) {
        if (strcasecmp(request->headers[i].name, "Access-Control-Request-Headers") == 0) {
            request_headers = request->headers[i].value;
            break;
        }
    }
    
    if (request_headers) {
        char allow_headers[512];
        snprintf(allow_headers, sizeof(allow_headers), "Content-Type, Authorization, X-Requested-With");
        http_server_add_header(response, "Access-Control-Allow-Headers", allow_headers);
    }
    
    // Add Access-Control-Max-Age
    http_server_add_header(response, "Access-Control-Max-Age", "86400"); // 24 hours
    
    return HAL_STATUS_OK;
}

hal_status_t http_server_validate_authentication(const http_request_t *request) {
    // Simple API key authentication for OHT-50
    const char* auth_header = NULL;
    
    // Look for Authorization header
    for (int i = 0; i < (int)request->header_count && i < HTTP_SERVER_MAX_HEADERS; i++) {
        if (strcasecmp(request->headers[i].name, "Authorization") == 0) {
            auth_header = request->headers[i].value;
            break;
        }
    }
    
    if (!auth_header) {
        hal_log_error("HTTP_AUTH", "http_server_validate_authentication", __LINE__,
                     HAL_STATUS_AUTHENTICATION_FAILED, "Missing Authorization header");
        return HAL_STATUS_AUTHENTICATION_FAILED;
    }
    
    // Check for Bearer token format
    if (strncmp(auth_header, "Bearer ", 7) != 0) {
        hal_log_error("HTTP_AUTH", "http_server_validate_authentication", __LINE__,
                     HAL_STATUS_AUTHENTICATION_FAILED, "Invalid Authorization format");
        return HAL_STATUS_AUTHENTICATION_FAILED;
    }
    
    const char* token = auth_header + 7; // Skip "Bearer "
    
    // Token validation using environment variables (more secure than hardcoded tokens)
    const char* admin_token = getenv("OHT50_ADMIN_TOKEN");
    const char* operator_token = getenv("OHT50_OPERATOR_TOKEN");
    const char* readonly_token = getenv("OHT50_READONLY_TOKEN");
    
    // Fallback to default tokens if environment variables not set (for development only)
    if (!admin_token) admin_token = "oht50_admin_token_2025";
    if (!operator_token) operator_token = "oht50_operator_token_2025";
    if (!readonly_token) readonly_token = "oht50_readonly_token_2025";
    
    // Validate token
    if (strcmp(token, admin_token) == 0) {
        hal_log_message(HAL_LOG_LEVEL_DEBUG, "[HTTP_AUTH] ✅ Admin token validated");
        return HAL_STATUS_OK;
    } else if (strcmp(token, operator_token) == 0) {
        hal_log_message(HAL_LOG_LEVEL_DEBUG, "[HTTP_AUTH] ✅ Operator token validated");
        return HAL_STATUS_OK;
    } else if (strcmp(token, readonly_token) == 0) {
        hal_log_message(HAL_LOG_LEVEL_DEBUG, "[HTTP_AUTH] ✅ Readonly token validated");
            return HAL_STATUS_OK;
    }
    
    hal_log_error("HTTP_AUTH", "http_server_validate_authentication", __LINE__,
                 HAL_STATUS_AUTHENTICATION_FAILED, "Invalid or expired token");
    return HAL_STATUS_AUTHENTICATION_FAILED;
}

hal_status_t http_server_require_authentication(http_response_t *response) {
    // Return 401 Unauthorized with authentication requirements
    const char* auth_response = 
        "{"
        "\"success\":false,"
        "\"error\":\"Authentication required\","
        "\"message\":\"Please provide valid Bearer token in Authorization header\","
        "\"auth_methods\":[\"Bearer token\"],"
        "\"example\":\"Authorization: Bearer oht50_admin_token_2025\""
        "}";
    
    return http_server_create_error_response(response, HTTP_STATUS_UNAUTHORIZED, auth_response);
}
