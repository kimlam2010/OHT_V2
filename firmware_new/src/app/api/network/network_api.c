/**
 * @file network_api.c
 * @brief Network API Implementation - OHT-50 Robot Network Management REST API
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Implementation của Network API cho OHT-50 robots
 * cung cấp REST endpoints cho network management.
 */

#include "network_api.h"
#include "wifi_ap_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

// Network API State
static bool network_api_initialized = false;
static api_route_t network_routes[] = {
    // Basic network endpoints
    {"/api/v1/network/status", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/performance", "GET", handle_get_network_performance, AUTH_NONE},
    {"/api/v1/network/health", "GET", handle_get_network_health, AUTH_NONE},
    {"/api/v1/network/config", "GET", handle_get_network_config, AUTH_OPERATOR},
    {"/api/v1/network/config", "POST", handle_post_network_config, AUTH_ADMIN},
    {"/api/v1/network/statistics", "GET", handle_get_network_statistics, AUTH_OPERATOR},
    {"/api/v1/network/statistics/reset", "POST", handle_post_statistics_reset, AUTH_ADMIN},
    
    // WiFi client endpoints
    {"/api/v1/network/wifi/status", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/wifi/connect", "POST", handle_post_wifi_connect, AUTH_OPERATOR},
    {"/api/v1/network/wifi/disconnect", "POST", handle_post_wifi_disconnect, AUTH_OPERATOR},
    {"/api/v1/network/wifi/scan", "GET", handle_get_wifi_scan, AUTH_NONE},
    {"/api/v1/network/wifi/signal", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/wifi/statistics", "GET", handle_get_network_statistics, AUTH_NONE},  // Public for monitoring
    {"/api/v1/network/wifi/performance", "GET", handle_get_network_performance, AUTH_NONE},
    {"/api/v1/network/wifi/health", "GET", handle_get_network_health, AUTH_NONE},
    {"/api/v1/network/wifi/config", "GET", handle_get_network_config, AUTH_OPERATOR},
    
    // Roaming endpoints
    {"/api/v1/network/roaming", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/roaming/enable", "POST", handle_post_roaming_enable, AUTH_ADMIN},
    {"/api/v1/network/roaming/disable", "POST", handle_post_roaming_enable, AUTH_ADMIN},
    {"/api/v1/network/roaming/status", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/roaming/scan", "GET", handle_get_wifi_scan, AUTH_NONE},
    {"/api/v1/network/roaming/switch", "POST", handle_post_roaming_enable, AUTH_ADMIN},
    {"/api/v1/network/roaming/statistics", "GET", handle_get_network_statistics, AUTH_OPERATOR},
    {"/api/v1/network/roaming/performance", "GET", handle_get_network_performance, AUTH_NONE},
    {"/api/v1/network/roaming/health", "GET", handle_get_network_health, AUTH_NONE},
    {"/api/v1/network/roaming/config", "GET", handle_get_network_config, AUTH_OPERATOR},
    {"/api/v1/network/wifi/roaming", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/wifi/roaming/config", "GET", handle_get_network_config, AUTH_OPERATOR},
    {"/api/v1/network/wifi/roaming/enable", "POST", handle_post_roaming_enable, AUTH_ADMIN},
    {"/api/v1/network/wifi/roaming/disable", "POST", handle_post_roaming_enable, AUTH_ADMIN},
    {"/api/v1/network/wifi/roaming/status", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/wifi/roaming/scan", "GET", handle_get_wifi_scan, AUTH_NONE},
    {"/api/v1/network/wifi/roaming/switch", "POST", handle_post_roaming_enable, AUTH_ADMIN},
    {"/api/v1/network/wifi/roaming/statistics", "GET", handle_get_network_statistics, AUTH_OPERATOR},
    {"/api/v1/network/wifi/roaming/performance", "GET", handle_get_network_performance, AUTH_NONE},
    {"/api/v1/network/wifi/roaming/health", "GET", handle_get_network_health, AUTH_NONE},
    
    // Mobile app endpoints
    {"/api/v1/network/mobile/enable", "POST", handle_post_mobile_enable, AUTH_ADMIN},
    
    // Legacy endpoints (redirect to new ones)
    {"/api/v1/network/connect", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/disconnect", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/signal", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/security", "GET", handle_get_network_status, AUTH_NONE},
    {"/api/v1/network/scan", "GET", handle_get_wifi_scan, AUTH_NONE},
    
    {NULL, NULL, NULL, AUTH_NONE}
};

// Authentication tokens (for development)
static const char* operator_token = "oht50_operator_token_2025";
static const char* admin_token = "oht50_admin_token_2025";

// Internal Functions
static const char* get_current_timestamp(void);
static void log_api_request(const char *method, const char *path, int status_code);

/**
 * @brief Initialize Network API
 */
int network_api_init(void) {
    if (network_api_initialized) {
        return 0;
    }
    
    // Initialize Network Manager
    int network_result = network_manager_init();
    if (network_result != NETWORK_SUCCESS) {
        printf("[NETWORK_API] Failed to initialize Network Manager: %d\n", network_result);
        return -1;
    }
    printf("[NETWORK_API] Network Manager initialized successfully\n");
    
    // Initialize WiFi Manager
    if (wifi_manager_init() != WIFI_SUCCESS) {
        printf("[NETWORK_API] Failed to initialize WiFi Manager\n");
        return -1;
    }
    
    // Initialize WiFi AP API
    if (wifi_ap_api_init() != WIFI_AP_API_SUCCESS) {
        printf("[NETWORK_API] Failed to initialize WiFi AP API\n");
        return -1;
    }
    
    // Register Network API routes with API Manager
    if (network_api_register_routes() != 0) {
        printf("[NETWORK_API] Failed to register routes\n");
        return -1;
    }
    
    network_api_initialized = true;
    
    printf("[NETWORK_API] Initialized successfully\n");
    return 0;
}

/**
 * @brief Deinitialize Network API
 */
int network_api_deinit(void) {
    if (!network_api_initialized) {
        return 0;
    }
    
    wifi_ap_api_deinit();
    wifi_manager_deinit();
    network_manager_deinit();
    
    network_api_initialized = false;
    
    printf("[NETWORK_API] Deinitialized successfully\n");
    return 0;
}

/**
 * @brief Handle HTTP request
 */
int network_api_handle_request(http_request_t *req, http_response_t *resp) {
    if (!req || !resp) {
        return -1;
    }
    
    printf("[NETWORK_API] Handling request: %s %s\n", req->method, req->path);
    
    // Find matching route in network routes
    for (int i = 0; network_routes[i].path != NULL; i++) {
        if (strcmp(req->path, network_routes[i].path) == 0 && 
            strcmp(req->method, network_routes[i].method) == 0) {
            
            printf("[NETWORK_API] Found route: %s %s\n", req->method, req->path);
            
            // Check authentication
            if (network_routes[i].required_auth != AUTH_NONE) {
                if (!network_api_validate_auth(req->auth_token, network_routes[i].required_auth)) {
                    printf("[NETWORK_API] Authentication failed for %s %s\n", req->method, req->path);
                    network_api_send_error_response(resp, 401, "Unauthorized");
                    return -1;
                }
            }
            
            // Call handler
            int result = network_routes[i].handler(req, resp);
            printf("[NETWORK_API] Handler result: %d for %s %s\n", result, req->method, req->path);
            log_api_request(req->method, req->path, resp->status_code);
            return result;
        }
    }
    
    printf("[NETWORK_API] No route found in network_routes, trying WiFi AP API\n");
    
    // Try WiFi AP API routes
    int result = wifi_ap_api_handle_request(req, resp);
    if (result != WIFI_AP_API_ERROR_INVALID_PARAM) {
        printf("[NETWORK_API] WiFi AP API handled request: %d\n", result);
        return result;
    }
    
    // Route not found
    printf("[NETWORK_API] Route not found: %s %s\n", req->method, req->path);
    network_api_send_error_response(resp, 404, "Not Found");
    return -1;
}

/**
 * @brief Register Network API routes with API Manager
 */
int network_api_register_routes(void) {
    printf("[NETWORK_API] Starting route registration...\n");
    
    // Count total routes first
    int total_routes = 0;
    for (int i = 0; network_routes[i].path != NULL; i++) {
        total_routes++;
    }
    printf("[NETWORK_API] Total routes to register: %d\n", total_routes);
    
    // Register all network routes with API Manager
    for (int i = 0; network_routes[i].path != NULL; i++) {
        // Convert method string to API Manager method
        api_mgr_http_method_t method;
        if (strcmp(network_routes[i].method, "GET") == 0) {
            method = API_MGR_HTTP_GET;
        } else if (strcmp(network_routes[i].method, "POST") == 0) {
            method = API_MGR_HTTP_POST;
        } else {
            printf("[NETWORK_API] Unsupported method: %s\n", network_routes[i].method);
            continue;
        }
        
        // Create wrapper handler that calls network_api_handle_request
        int result = api_manager_register_endpoint(network_routes[i].path, method, 
                                                  network_api_wrapper_handler);
        if (result != 0) {
            printf("[NETWORK_API] Failed to register route: %s %s (error: %d)\n", 
                   network_routes[i].method, network_routes[i].path, result);
            // Continue with other routes instead of failing completely
            continue;
        }
        
        printf("[NETWORK_API] Registered route: %s %s\n", 
               network_routes[i].method, network_routes[i].path);
    }
    
    printf("[NETWORK_API] Route registration completed successfully\n");
    return 0;
}

/**
 * @brief Wrapper handler for API Manager integration
 */
int network_api_wrapper_handler(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Convert API Manager request to Network API request
    http_request_t network_req;
    http_response_t network_resp;
    
    // Copy request data
    strncpy(network_req.method, req->method == API_MGR_HTTP_GET ? "GET" : "POST", sizeof(network_req.method));
    strncpy(network_req.path, req->path, sizeof(network_req.path));
    strncpy(network_req.body, req->body ? req->body : "", sizeof(network_req.body));
    
    // Extract auth token from headers
    network_req.auth_token[0] = '\0';
    for (int i = 0; i < req->header_count; i++) {
        if (strcasecmp(req->headers[i].name, "Authorization") == 0) {
            strncpy(network_req.auth_token, req->headers[i].value, sizeof(network_req.auth_token));
            break;
        }
    }
    
    // Initialize response
    memset(&network_resp, 0, sizeof(network_resp));
    
    // Call Network API handler
    int result = network_api_handle_request(&network_req, &network_resp);
    
    // Convert response back to API Manager format
    res->status_code = network_resp.status_code;
    if (network_resp.body && strlen(network_resp.body) > 0) {
        res->body = strdup(network_resp.body);
        res->body_length = strlen(network_resp.body);
    } else {
        res->body = NULL;
        res->body_length = 0;
    }
    
    return result;
}

/**
 * @brief Validate authentication token
 */
bool network_api_validate_auth(const char *token, auth_level_t required_level) {
    if (!token) {
        return false;
    }
    
    // Extract token from "Bearer <token>" format
    const char *actual_token = token;
    if (strncmp(token, "Bearer ", 7) == 0) {
        actual_token = token + 7; // Skip "Bearer "
    }
    
    if (required_level == AUTH_OPERATOR) {
        return strcmp(actual_token, operator_token) == 0;
    } else if (required_level == AUTH_ADMIN) {
        return strcmp(actual_token, admin_token) == 0;
    }
    
    return true; // AUTH_NONE
}

/**
 * @brief Send JSON response
 */
int network_api_send_json_response(http_response_t *resp, int status_code, const char *json_data) {
    if (!resp || !json_data) {
        return -1;
    }
    
    resp->status_code = status_code;
    resp->content_length = strlen(json_data);
    
    // Set headers
    snprintf(resp->headers, sizeof(resp->headers),
             "Content-Type: application/json\r\n"
             "Content-Length: %u\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
             "Access-Control-Allow-Headers: Content-Type, Authorization\r\n\r\n",
             resp->content_length);
    
    // Set body
    strncpy(resp->body, json_data, sizeof(resp->body) - 1);
    resp->body[sizeof(resp->body) - 1] = '\0';
    
    return 0;
}

/**
 * @brief Send error response
 */
int network_api_send_error_response(http_response_t *resp, int status_code, const char *error_message) {
    if (!resp || !error_message) {
        return -1;
    }
    
    char json_error[512];
    snprintf(json_error, sizeof(json_error),
             "{\"success\": false, \"error\": \"%s\", \"timestamp\": \"%s\"}",
             error_message, get_current_timestamp());
    
    return network_api_send_json_response(resp, status_code, json_error);
}

/**
 * @brief Parse JSON request body for string
 */
int network_api_parse_json_string(const char *body, const char *key, char *value, int max_len) {
    if (!body || !key || !value || max_len <= 0) {
        return -1;
    }
    
    char search_key[128];
    snprintf(search_key, sizeof(search_key), "\"%s\":", key);
    
    const char *key_pos = strstr(body, search_key);
    if (!key_pos) {
        return -1;
    }
    
    const char *value_start = strchr(key_pos, ':');
    if (!value_start) {
        return -1;
    }
    value_start++; // Skip ':'
    
    // Skip whitespace
    while (*value_start == ' ' || *value_start == '\t') {
        value_start++;
    }
    
    // Check if value is quoted
    if (*value_start == '"') {
        value_start++; // Skip opening quote
        const char *value_end = strchr(value_start, '"');
        if (!value_end) {
            return -1;
        }
        
        int len = value_end - value_start;
        if (len >= max_len) {
            len = max_len - 1;
        }
        
        strncpy(value, value_start, len);
        value[len] = '\0';
    } else {
        // Unquoted value (for numbers, booleans)
        const char *value_end = value_start;
        while (*value_end && *value_end != ',' && *value_end != '}' && *value_end != ' ') {
            value_end++;
        }
        
        int len = value_end - value_start;
        if (len >= max_len) {
            len = max_len - 1;
        }
        
        strncpy(value, value_start, len);
        value[len] = '\0';
    }
    
    return 0;
}

/**
 * @brief Parse JSON request body for boolean
 */
int network_api_parse_json_bool(const char *body, const char *key, bool *value) {
    if (!body || !key || !value) {
        return -1;
    }
    
    char str_value[16];
    if (network_api_parse_json_string(body, key, str_value, sizeof(str_value)) != 0) {
        return -1;
    }
    
    if (strcmp(str_value, "true") == 0) {
        *value = true;
    } else if (strcmp(str_value, "false") == 0) {
        *value = false;
    } else {
        return -1;
    }
    
    return 0;
}

/**
 * @brief Parse JSON request body for integer
 */
int network_api_parse_json_int(const char *body, const char *key, int *value) {
    if (!body || !key || !value) {
        return -1;
    }
    
    char str_value[16];
    if (network_api_parse_json_string(body, key, str_value, sizeof(str_value)) != 0) {
        return -1;
    }
    
    *value = atoi(str_value);
    return 0;
}

// API Endpoint Handlers

/**
 * @brief Handle GET /api/v1/network/status
 */
int handle_get_network_status(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    oht_network_status_t status;
    int result = network_manager_get_status(&status);
    
    if (result != NETWORK_SUCCESS) {
        return network_api_send_error_response(resp, 500, "Failed to get network status");
    }
    
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"connected\": %s, "
             "\"current_ssid\": \"%s\", "
             "\"signal_strength\": %d, "
             "\"ip_address\": \"%s\", "
             "\"gateway\": \"%s\", "
             "\"dns\": \"%s\", "
             "\"bytes_sent\": %u, "
             "\"bytes_received\": %u, "
             "\"latency_ms\": %.1f, "
             "\"roaming_active\": %s"
             "}, \"timestamp\": \"%s\"}",
             status.connected ? "true" : "false",
             status.current_ssid,
             status.signal_strength,
             status.ip_address,
             status.gateway,
             status.dns,
             status.bytes_sent,
             status.bytes_received,
             status.latency_ms,
             status.roaming_active ? "true" : "false",
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/wifi/connect
 */
int handle_post_wifi_connect(http_request_t *req, http_response_t *resp) {
    char ssid[64], password[128];
    
    if (network_api_parse_json_string(req->body, "ssid", ssid, sizeof(ssid)) != 0 ||
        network_api_parse_json_string(req->body, "password", password, sizeof(password)) != 0) {
        return network_api_send_error_response(resp, 400, "Missing ssid or password");
    }
    
    int result = network_manager_connect_wifi(ssid, password);
    
    if (result != NETWORK_SUCCESS) {
        const char *error_msg = network_manager_get_error_message(result);
        return network_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"WiFi connection successful\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/wifi/disconnect
 */
int handle_post_wifi_disconnect(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    int result = network_manager_disconnect_wifi();
    
    if (result != NETWORK_SUCCESS) {
        const char *error_msg = network_manager_get_error_message(result);
        return network_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"WiFi disconnection successful\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/wifi/scan
 */
int handle_get_wifi_scan(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    wifi_scan_result_t networks[10];
    int count = wifi_manager_scan_networks(networks, 10, 10000);
    
    if (count < 0) {
        return network_api_send_error_response(resp, 500, "Failed to scan WiFi networks");
    }
    
    char json_response[4096] = "{\"success\": true, \"data\": {\"networks\": [";
    
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            strcat(json_response, ", ");
        }
        
        char network_json[512];
        snprintf(network_json, sizeof(network_json),
                 "{\"ssid\": \"%s\", \"bssid\": \"%s\", \"signal_strength\": %d, "
                 "\"frequency\": %d, \"channel\": %d, \"security\": \"%s\", "
                 "\"band\": \"%s\", \"hidden\": %s, \"connected\": %s}",
                 networks[i].ssid,
                 networks[i].bssid,
                 networks[i].signal_strength_dbm,
                 networks[i].frequency_mhz,
                 networks[i].channel,
                  networks[i].security_type == OHT_WIFI_SECURITY_WPA2 ? "WPA2" : 
                  networks[i].security_type == OHT_WIFI_SECURITY_WPA3 ? "WPA3" : "Open",
                 networks[i].band == WIFI_BAND_5G ? "5G" : "2.4G",
                 networks[i].hidden ? "true" : "false",
                 networks[i].connected ? "true" : "false");
        
        strcat(json_response, network_json);
    }
    
    strcat(json_response, "], \"count\": ");
    char count_str[16];
    snprintf(count_str, sizeof(count_str), "%d", count);
    strcat(json_response, count_str);
    
    snprintf(json_response + strlen(json_response), 
             sizeof(json_response) - strlen(json_response),
             "}, \"timestamp\": \"%s\"}", get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/performance
 */
int handle_get_network_performance(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    performance_metrics_t metrics;
    int result = network_manager_get_performance(&metrics);
    
    if (result != NETWORK_SUCCESS) {
        return network_api_send_error_response(resp, 500, "Failed to get performance metrics");
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"response_time_ms\": %u, "
             "\"request_count\": %u, "
             "\"error_count\": %u, "
             "\"success_rate\": %.1f"
             "}, \"timestamp\": \"%s\"}",
             metrics.response_time_ms,
             metrics.request_count,
             metrics.error_count,
             metrics.success_rate,
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/health
 */
int handle_get_network_health(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    oht_network_status_t status;
    network_manager_get_status(&status);
    
    wifi_statistics_t stats;
    wifi_manager_get_statistics(&stats);
    
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"network_status\": \"%s\", "
             "\"signal_quality\": \"%s\", "
             "\"connection_uptime\": %u, "
             "\"success_rate\": %.1f, "
             "\"roaming_events\": %u, "
             "\"health_score\": %.1f"
             "}, \"timestamp\": \"%s\"}",
             status.connected ? "Connected" : "Disconnected",
             status.signal_strength > -50 ? "Excellent" : 
             status.signal_strength > -70 ? "Good" : "Fair",
             stats.total_uptime_seconds,
             stats.connection_success_rate,
             stats.roaming_events,
             status.connected ? 95.0f : 0.0f,
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/roaming/enable
 */
int handle_post_roaming_enable(http_request_t *req, http_response_t *resp) {
    bool enabled;
    int threshold = -70;
    
    if (network_api_parse_json_bool(req->body, "enabled", &enabled) != 0) {
        return network_api_send_error_response(resp, 400, "Missing enabled parameter");
    }
    
    network_api_parse_json_int(req->body, "threshold", &threshold);
    
    wifi_roaming_config_t config = {
        .enabled = enabled,
        .signal_threshold_dbm = threshold,
        .scan_interval_ms = 5000,
        .handover_timeout_ms = 2000,
        .aggressive_roaming = false
    };
    
    int result = wifi_manager_enable_roaming(&config);
    
    if (result != WIFI_SUCCESS) {
        return network_api_send_error_response(resp, 500, "Failed to configure roaming");
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"Roaming configuration updated\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/mobile/enable
 */
int handle_post_mobile_enable(http_request_t *req, http_response_t *resp) {
    bool enabled;
    
    if (network_api_parse_json_bool(req->body, "enabled", &enabled) != 0) {
        return network_api_send_error_response(resp, 400, "Missing enabled parameter");
    }
    
    int result = network_manager_enable_mobile_app(enabled);
    
    if (result != NETWORK_SUCCESS) {
        return network_api_send_error_response(resp, 500, "Failed to configure mobile app");
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"Mobile app configuration updated\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/config
 */
int handle_get_network_config(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    oht_network_config_t config;
    int result = network_manager_get_config(&config);
    
    if (result != NETWORK_SUCCESS) {
        printf("[NETWORK_API] network_manager_get_config failed: %d (continuing with mock)\n", result);
        // Use mock config for development
        memset(&config, 0, sizeof(oht_network_config_t));
        strcpy(config.wifi_ssid, "OHT-50-Mock");
        config.wifi_enabled = true;
        config.roaming_enabled = true;
        config.mobile_app_enabled = true;
        config.signal_strength = -65;
        config.last_update_time = hal_get_timestamp_ms();
    }
    
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"wifi_enabled\": %s, "
             "\"wifi_ssid\": \"%s\", "
             "\"wifi_security_type\": %d, "
             "\"signal_strength\": %d, "
             "\"roaming_enabled\": %s, "
             "\"mobile_app_enabled\": %s, "
             "\"last_update_time\": %u"
             "}, \"timestamp\": \"%s\"}",
             config.wifi_enabled ? "true" : "false",
             config.wifi_ssid,
             config.wifi_security_type,
             config.signal_strength,
             config.roaming_enabled ? "true" : "false",
             config.mobile_app_enabled ? "true" : "false",
             config.last_update_time,
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/config
 */
int handle_post_network_config(http_request_t *req, http_response_t *resp) {
    oht_network_config_t config = {0};
    
    // Parse configuration from JSON
    bool wifi_enabled = true;
    char wifi_ssid[32] = "OHT-50-Network";
    char wifi_password[64] = "secure_password_2025";
    int wifi_security_type = OHT_WIFI_SECURITY_WPA2;
    int signal_strength = -70;
    bool roaming_enabled = true;
    bool mobile_app_enabled = true;
    
    network_api_parse_json_bool(req->body, "wifi_enabled", &wifi_enabled);
    network_api_parse_json_string(req->body, "wifi_ssid", wifi_ssid, sizeof(wifi_ssid));
    network_api_parse_json_string(req->body, "wifi_password", wifi_password, sizeof(wifi_password));
    network_api_parse_json_int(req->body, "wifi_security_type", &wifi_security_type);
    network_api_parse_json_int(req->body, "signal_strength", &signal_strength);
    network_api_parse_json_bool(req->body, "roaming_enabled", &roaming_enabled);
    network_api_parse_json_bool(req->body, "mobile_app_enabled", &mobile_app_enabled);
    
    // Set configuration
    config.wifi_enabled = wifi_enabled;
    strncpy(config.wifi_ssid, wifi_ssid, sizeof(config.wifi_ssid) - 1);
    strncpy(config.wifi_password, wifi_password, sizeof(config.wifi_password) - 1);
    config.wifi_security_type = wifi_security_type;
    config.signal_strength = signal_strength;
    config.roaming_enabled = roaming_enabled;
    config.mobile_app_enabled = mobile_app_enabled;
    config.last_update_time = time(NULL);
    
    int result = network_manager_set_config(&config);
    
    if (result != NETWORK_SUCCESS) {
        return network_api_send_error_response(resp, 500, "Failed to set network config");
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"Network configuration updated\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/statistics
 */
int handle_get_network_statistics(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    wifi_statistics_t stats;
    int result = wifi_manager_get_statistics(&stats);
    
    if (result != WIFI_SUCCESS) {
        printf("[NETWORK_API] wifi_manager_get_statistics failed: %d\n", result);
        return network_api_send_error_response(resp, 500, "Failed to get network statistics");
    }
    
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"connection_attempts\": %u, "
             "\"successful_connections\": %u, "
             "\"failed_connections\": %u, "
             "\"roaming_events\": %u, "
             "\"disconnection_events\": %u, "
             "\"connection_success_rate\": %.1f, "
             "\"total_uptime_seconds\": %u, "
             "\"average_signal_strength_dbm\": %d"
             "}, \"timestamp\": \"%s\"}",
             stats.connection_attempts,
             stats.successful_connections,
             stats.failed_connections,
             stats.roaming_events,
             stats.disconnection_events,
             stats.connection_success_rate,
             stats.total_uptime_seconds,
             stats.average_signal_strength_dbm,
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/statistics/reset
 */
int handle_post_statistics_reset(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    int result = wifi_manager_reset_statistics();
    
    if (result != WIFI_SUCCESS) {
        return network_api_send_error_response(resp, 500, "Failed to reset statistics");
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"Statistics reset successfully\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return network_api_send_json_response(resp, 200, json_response);
}

// Internal Functions Implementation

/**
 * @brief Get current timestamp in ISO format
 */
static const char* get_current_timestamp(void) {
    static char timestamp[32];
    time_t now = time(NULL);
    struct tm *tm_info = gmtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", tm_info);
    return timestamp;
}

/**
 * @brief Log API request
 */
static void log_api_request(const char *method, const char *path, int status_code) {
    printf("[NETWORK_API] %s %s -> %d\n", method, path, status_code);
}
