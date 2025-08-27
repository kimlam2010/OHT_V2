/**
 * @file http_server.c
 * @brief HTTP Server Implementation for OHT-50 Master Module Firmware
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-01 (HTTP Server Implementation)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#include "http_server.h"
#include "hal_common.h"

// Default HTTP server configuration
const http_server_config_t HTTP_SERVER_DEFAULT_CONFIG = {
    .port = HTTP_SERVER_DEFAULT_PORT,
    .max_connections = HTTP_SERVER_MAX_CONNECTIONS,
    .timeout_ms = HTTP_SERVER_TIMEOUT_MS,
    .enable_cors = true,
    .cors_origin = "*",
    .enable_auth = false,
    .auth_token = "",
    .enable_logging = true,
    .log_file = "/var/log/oht50/http_server.log"
};

// Internal function declarations
static void* http_server_thread(void *arg);
static hal_status_t http_handle_connection(http_server_t *server, int client_socket);
static hal_status_t http_process_request(http_server_t *server, const http_request_t *request, http_response_t *response);
static http_route_t* http_find_route(http_server_t *server, http_method_t method, const char *path);
static hal_status_t http_send_response(int client_socket, const http_response_t *response);
static void http_log_request(http_server_t *server, const http_request_t *request, const http_response_t *response);

// HTTP method strings
static const char* HTTP_METHOD_STRINGS[] = {
    "GET",
    "POST", 
    "PUT",
    "DELETE",
    "OPTIONS",
    "UNKNOWN"
};

// HTTP status strings
static const char* HTTP_STATUS_STRINGS[] = {
    [HTTP_STATUS_OK] = "OK",
    [HTTP_STATUS_CREATED] = "Created",
    [HTTP_STATUS_NO_CONTENT] = "No Content",
    [HTTP_STATUS_BAD_REQUEST] = "Bad Request",
    [HTTP_STATUS_UNAUTHORIZED] = "Unauthorized",
    [HTTP_STATUS_FORBIDDEN] = "Forbidden",
    [HTTP_STATUS_NOT_FOUND] = "Not Found",
    [HTTP_STATUS_METHOD_NOT_ALLOWED] = "Method Not Allowed",
    [HTTP_STATUS_INTERNAL_SERVER_ERROR] = "Internal Server Error",
    [HTTP_STATUS_SERVICE_UNAVAILABLE] = "Service Unavailable"
};

hal_status_t http_server_init(http_server_t *server, const http_server_config_t *config) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Initialize server structure
    memset(server, 0, sizeof(http_server_t));
    
    // Set configuration
    if (config != NULL) {
        server->config = *config;
    } else {
        server->config = HTTP_SERVER_DEFAULT_CONFIG;
    }

    // Initialize mutex
    if (pthread_mutex_init(&server->mutex, NULL) != 0) {
        return HAL_STATUS_ERROR;
    }

    // Initialize status
    server->status.is_running = false;
    server->status.active_connections = 0;
    server->status.total_requests = 0;
    server->status.successful_requests = 0;
    server->status.failed_requests = 0;
    server->status.uptime_ms = 0;
    server->status.last_request_time = 0;

    // Initialize routes
    server->route_count = 0;
    server->should_stop = false;
    server->server_socket = -1;

    printf("[HTTP] Server initialized on port %d\n", server->config.port);
    return HAL_STATUS_OK;
}

hal_status_t http_server_deinit(http_server_t *server) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Stop server if running
    if (server->status.is_running) {
        http_server_stop(server);
    }

    // Close server socket
    if (server->server_socket >= 0) {
        close(server->server_socket);
        server->server_socket = -1;
    }

    // Destroy mutex
    pthread_mutex_destroy(&server->mutex);

    printf("[HTTP] Server deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t http_server_start(http_server_t *server) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    
    if (server->status.is_running) {
        pthread_mutex_unlock(&server->mutex);
        return HAL_STATUS_ALREADY_ACTIVE;
    }

    // Create server socket
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_socket < 0) {
        pthread_mutex_unlock(&server->mutex);
        printf("[HTTP] Failed to create socket: %s\n", strerror(errno));
        return HAL_STATUS_ERROR;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(server->server_socket);
        server->server_socket = -1;
        pthread_mutex_unlock(&server->mutex);
        printf("[HTTP] Failed to set socket options: %s\n", strerror(errno));
        return HAL_STATUS_ERROR;
    }

    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server->config.port);

    if (bind(server->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server->server_socket);
        server->server_socket = -1;
        pthread_mutex_unlock(&server->mutex);
        printf("[HTTP] Failed to bind socket: %s\n", strerror(errno));
        return HAL_STATUS_ERROR;
    }

    // Listen for connections
    if (listen(server->server_socket, server->config.max_connections) < 0) {
        close(server->server_socket);
        server->server_socket = -1;
        pthread_mutex_unlock(&server->mutex);
        printf("[HTTP] Failed to listen: %s\n", strerror(errno));
        return HAL_STATUS_ERROR;
    }

    // Start server thread
    server->should_stop = false;
    if (pthread_create(&server->server_thread, NULL, http_server_thread, server) != 0) {
        close(server->server_socket);
        server->server_socket = -1;
        pthread_mutex_unlock(&server->mutex);
        printf("[HTTP] Failed to create server thread\n");
        return HAL_STATUS_ERROR;
    }

    server->status.is_running = true;
    server->status.uptime_ms = hal_get_timestamp_ms();
    
    pthread_mutex_unlock(&server->mutex);

    printf("[HTTP] Server started on port %d\n", server->config.port);
    return HAL_STATUS_OK;
}

hal_status_t http_server_stop(http_server_t *server) {
    if (server == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    
    if (!server->status.is_running) {
        pthread_mutex_unlock(&server->mutex);
        return HAL_STATUS_OK;
    }

    // Signal thread to stop
    server->should_stop = true;

    // Close server socket to wake up accept()
    if (server->server_socket >= 0) {
        close(server->server_socket);
        server->server_socket = -1;
    }

    pthread_mutex_unlock(&server->mutex);

    // Wait for thread to finish
    pthread_join(server->server_thread, NULL);

    pthread_mutex_lock(&server->mutex);
    server->status.is_running = false;
    pthread_mutex_unlock(&server->mutex);

    printf("[HTTP] Server stopped\n");
    return HAL_STATUS_OK;
}

hal_status_t http_server_add_route(http_server_t *server, 
                                  http_method_t method, 
                                  const char *path, 
                                  http_route_handler_t handler, 
                                  bool require_auth) {
    if (server == NULL || path == NULL || handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);

    if (server->route_count >= 50) {
        pthread_mutex_unlock(&server->mutex);
        return HAL_STATUS_NO_MEMORY;
    }

    // Add route
    http_route_t *route = &server->routes[server->route_count];
    route->method = method;
    strncpy(route->path, path, sizeof(route->path) - 1);
    route->handler = handler;
    route->require_auth = require_auth;

    server->route_count++;

    pthread_mutex_unlock(&server->mutex);

    printf("[HTTP] Added route: %s %s\n", http_get_method_string(method), path);
    return HAL_STATUS_OK;
}

hal_status_t http_server_get_status(http_server_t *server, http_server_status_t *status) {
    if (server == NULL || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    *status = server->status;
    pthread_mutex_unlock(&server->mutex);

    return HAL_STATUS_OK;
}

hal_status_t http_server_set_config(http_server_t *server, const http_server_config_t *config) {
    if (server == NULL || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    server->config = *config;
    pthread_mutex_unlock(&server->mutex);

    return HAL_STATUS_OK;
}

hal_status_t http_server_get_config(http_server_t *server, http_server_config_t *config) {
    if (server == NULL || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    pthread_mutex_lock(&server->mutex);
    *config = server->config;
    pthread_mutex_unlock(&server->mutex);

    return HAL_STATUS_OK;
}

// HTTP Server Thread Function
static void* http_server_thread(void *arg) {
    http_server_t *server = (http_server_t*)arg;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    printf("[HTTP] Server thread started\n");

    while (!server->should_stop) {
        // Accept connection
        int client_socket = accept(server->server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            if (server->should_stop) {
                break;
            }
            printf("[HTTP] Accept failed: %s\n", strerror(errno));
            continue;
        }

        // Handle connection
        hal_status_t status = http_handle_connection(server, client_socket);
        if (status != HAL_STATUS_OK) {
            printf("[HTTP] Connection handling failed: %d\n", status);
        }

        close(client_socket);
    }

    printf("[HTTP] Server thread stopped\n");
    return NULL;
}

// Handle individual connection
static hal_status_t http_handle_connection(http_server_t *server, int client_socket) {
    char buffer[HTTP_SERVER_BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) {
        return HAL_STATUS_ERROR;
    }

    buffer[bytes_received] = '\0';

    // Parse request
    http_request_t request;
    hal_status_t status = http_parse_request(buffer, bytes_received, &request);
    if (status != HAL_STATUS_OK) {
        printf("[HTTP] Failed to parse request\n");
        return status;
    }

    // Process request
    http_response_t response;
    memset(&response, 0, sizeof(response));
    response.status = HTTP_STATUS_OK;
    strcpy(response.content_type, "application/json");

    status = http_process_request(server, &request, &response);
    if (status != HAL_STATUS_OK) {
        printf("[HTTP] Failed to process request\n");
        http_response_set_error(&response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Internal server error");
    }

    // Update statistics
    pthread_mutex_lock(&server->mutex);
    server->status.total_requests++;
    server->status.last_request_time = hal_get_timestamp_ms();
    
    if (response.status >= 200 && response.status < 400) {
        server->status.successful_requests++;
    } else {
        server->status.failed_requests++;
    }
    pthread_mutex_unlock(&server->mutex);

    // Log request
    if (server->config.enable_logging) {
        http_log_request(server, &request, &response);
    }

    // Send response
    return http_send_response(client_socket, &response);
}

// Process HTTP request
static hal_status_t http_process_request(http_server_t *server, const http_request_t *request, http_response_t *response) {
    // Find matching route
    http_route_t *route = http_find_route(server, request->method, request->path);
    if (route == NULL) {
        http_response_set_error(response, HTTP_STATUS_NOT_FOUND, "Route not found");
        return HAL_STATUS_NOT_FOUND;
    }

    // Check authentication
    if (route->require_auth && server->config.enable_auth) {
        if (!http_validate_auth(request, server->config.auth_token)) {
            http_response_set_error(response, HTTP_STATUS_UNAUTHORIZED, "Authentication required");
            return HAL_STATUS_AUTHENTICATION_FAILED;
        }
    }

    // Call route handler
    hal_status_t status = route->handler(request, response);
    if (status != HAL_STATUS_OK) {
        http_response_set_error(response, HTTP_STATUS_INTERNAL_SERVER_ERROR, "Handler error");
        return status;
    }

    return HAL_STATUS_OK;
}

// Find matching route
static http_route_t* http_find_route(http_server_t *server, http_method_t method, const char *path) {
    for (int i = 0; i < server->route_count; i++) {
        if (server->routes[i].method == method && 
            strcmp(server->routes[i].path, path) == 0) {
            return &server->routes[i];
        }
    }
    return NULL;
}

// Send HTTP response
static hal_status_t http_send_response(int client_socket, const http_response_t *response) {
    char response_data[HTTP_SERVER_BUFFER_SIZE];
    int response_length;

    hal_status_t status = http_build_response(response, response_data, sizeof(response_data), &response_length);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    int bytes_sent = send(client_socket, response_data, response_length, 0);
    if (bytes_sent != response_length) {
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

// Log HTTP request
static void http_log_request(http_server_t *server, const http_request_t *request, const http_response_t *response) {
    printf("[HTTP] %s %s -> %d\n", 
                 http_get_method_string(request->method), 
                 request->path, 
                 response->status);
}

// HTTP Utility Functions

http_method_t http_parse_method(const char *method_str) {
    if (method_str == NULL) {
        return HTTP_METHOD_UNKNOWN;
    }

    if (strcmp(method_str, "GET") == 0) return HTTP_METHOD_GET;
    if (strcmp(method_str, "POST") == 0) return HTTP_METHOD_POST;
    if (strcmp(method_str, "PUT") == 0) return HTTP_METHOD_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_METHOD_DELETE;
    if (strcmp(method_str, "OPTIONS") == 0) return HTTP_METHOD_OPTIONS;

    return HTTP_METHOD_UNKNOWN;
}

const char* http_get_method_string(http_method_t method) {
    if (method >= 0 && method < sizeof(HTTP_METHOD_STRINGS) / sizeof(HTTP_METHOD_STRINGS[0])) {
        return HTTP_METHOD_STRINGS[method];
    }
    return "UNKNOWN";
}

const char* http_get_status_string(http_status_t status) {
    if (status >= 200 && status < 600 && HTTP_STATUS_STRINGS[status] != NULL) {
        return HTTP_STATUS_STRINGS[status];
    }
    return "Unknown";
}

hal_status_t http_parse_request(const char *request_data, int request_length, http_request_t *request) {
    if (request_data == NULL || request == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    memset(request, 0, sizeof(http_request_t));

    // Parse first line (method, path, version)
    char *line_end = strstr(request_data, "\r\n");
    if (line_end == NULL) {
        return HAL_STATUS_ERROR;
    }

    char first_line[512];
    int line_len = line_end - request_data;
    strncpy(first_line, request_data, line_len);
    first_line[line_len] = '\0';

    char *method_str = strtok(first_line, " ");
    char *path_str = strtok(NULL, " ");
    char *version_str = strtok(NULL, " ");

    if (method_str == NULL || path_str == NULL) {
        return HAL_STATUS_ERROR;
    }

    request->method = http_parse_method(method_str);
    strncpy(request->path, path_str, sizeof(request->path) - 1);

    // Parse headers
    char *headers_start = line_end + 2;
    char *body_start = strstr(headers_start, "\r\n\r\n");
    
    if (body_start != NULL) {
        // Parse headers
        char *header_line = headers_start;
        while (header_line < body_start) {
            char *next_line = strstr(header_line, "\r\n");
            if (next_line == NULL) break;

            char header[256];
            int header_len = next_line - header_line;
            strncpy(header, header_line, header_len);
            header[header_len] = '\0';

            char *colon = strchr(header, ':');
            if (colon != NULL) {
                *colon = '\0';
                char *value = colon + 1;
                while (*value == ' ') value++; // Skip spaces

                if (request->header_count < HTTP_SERVER_MAX_HEADERS) {
                    strncpy(request->headers[request->header_count][0], header, 127);
                    strncpy(request->headers[request->header_count][1], value, 127);
                    request->header_count++;
                }

                // Special headers
                if (strcasecmp(header, "Authorization") == 0) {
                    strncpy(request->authorization, value, sizeof(request->authorization) - 1);
                } else if (strcasecmp(header, "Content-Type") == 0) {
                    strncpy(request->content_type, value, sizeof(request->content_type) - 1);
                }
            }

            header_line = next_line + 2;
        }

        // Parse body
        body_start += 4; // Skip \r\n\r\n
        int body_len = request_data + request_length - body_start;
        if (body_len > 0 && body_len < HTTP_SERVER_BUFFER_SIZE) {
            strncpy(request->body, body_start, body_len);
            request->body_length = body_len;
        }
    }

    return HAL_STATUS_OK;
}

hal_status_t http_build_response(const http_response_t *response, 
                                char *response_data, 
                                int max_length, 
                                int *actual_length) {
    if (response == NULL || response_data == NULL || actual_length == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Build response line
    int written = snprintf(response_data, max_length,
                          "HTTP/1.1 %d %s\r\n",
                          response->status,
                          http_get_status_string(response->status));

    if (written >= max_length) {
        return HAL_STATUS_ERROR;
    }

    // Add headers
    for (int i = 0; i < response->header_count; i++) {
        written += snprintf(response_data + written, max_length - written,
                           "%s: %s\r\n",
                           response->headers[i][0],
                           response->headers[i][1]);
        
        if (written >= max_length) {
            return HAL_STATUS_ERROR;
        }
    }

    // Add content type if not set
    if (strlen(response->content_type) > 0) {
        written += snprintf(response_data + written, max_length - written,
                           "Content-Type: %s\r\n",
                           response->content_type);
        
        if (written >= max_length) {
            return HAL_STATUS_ERROR;
        }
    }

    // Add content length
    written += snprintf(response_data + written, max_length - written,
                       "Content-Length: %d\r\n",
                       response->body_length);
    
    if (written >= max_length) {
        return HAL_STATUS_ERROR;
    }

    // End headers
    written += snprintf(response_data + written, max_length - written, "\r\n");
    
    if (written >= max_length) {
        return HAL_STATUS_ERROR;
    }

    // Add body
    if (response->body_length > 0) {
        if (written + response->body_length >= max_length) {
            return HAL_STATUS_ERROR;
        }
        memcpy(response_data + written, response->body, response->body_length);
        written += response->body_length;
    }

    *actual_length = written;
    return HAL_STATUS_OK;
}

hal_status_t http_response_add_header(http_response_t *response, const char *name, const char *value) {
    if (response == NULL || name == NULL || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    if (response->header_count >= HTTP_SERVER_MAX_HEADERS) {
        return HAL_STATUS_NO_MEMORY;
    }

    strncpy(response->headers[response->header_count][0], name, 127);
    strncpy(response->headers[response->header_count][1], value, 127);
    response->header_count++;

    return HAL_STATUS_OK;
}

hal_status_t http_response_set_json(http_response_t *response, const char *json_data) {
    if (response == NULL || json_data == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    strcpy(response->content_type, "application/json");
    strncpy(response->body, json_data, HTTP_SERVER_BUFFER_SIZE - 1);
    response->body_length = strlen(response->body);

    return HAL_STATUS_OK;
}

hal_status_t http_response_set_error(http_response_t *response, http_status_t status, const char *error_message) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    response->status = status;
    strcpy(response->content_type, "application/json");

    char json_error[512];
    snprintf(json_error, sizeof(json_error),
             "{\"error\":\"%s\",\"status\":%d,\"message\":\"%s\"}",
             http_get_status_string(status),
             status,
             error_message ? error_message : "");

    return http_response_set_json(response, json_error);
}

bool http_validate_auth(const http_request_t *request, const char *auth_token) {
    if (request == NULL || auth_token == NULL || strlen(auth_token) == 0) {
        return false;
    }

    const char *auth_header = http_get_header(request, "Authorization");
    if (auth_header == NULL) {
        return false;
    }

    // Check for Bearer token
    if (strncmp(auth_header, "Bearer ", 7) == 0) {
        return strcmp(auth_header + 7, auth_token) == 0;
    }

    // Check for direct token
    return strcmp(auth_header, auth_token) == 0;
}

const char* http_get_header(const http_request_t *request, const char *header_name) {
    if (request == NULL || header_name == NULL) {
        return NULL;
    }

    for (int i = 0; i < request->header_count; i++) {
        if (strcasecmp(request->headers[i][0], header_name) == 0) {
            return request->headers[i][1];
        }
    }

    return NULL;
}

const char* http_get_query_param(const http_request_t *request, const char *param_name) {
    if (request == NULL || param_name == NULL) {
        return NULL;
    }

    char *query_start = strchr(request->path, '?');
    if (query_start == NULL) {
        return NULL;
    }

    char *query = query_start + 1;
    char *param = strtok(query, "&");
    
    while (param != NULL) {
        char *equal = strchr(param, '=');
        if (equal != NULL) {
            *equal = '\0';
            if (strcmp(param, param_name) == 0) {
                return equal + 1;
            }
        }
        param = strtok(NULL, "&");
    }

    return NULL;
}
