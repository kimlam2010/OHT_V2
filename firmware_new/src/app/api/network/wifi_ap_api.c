/**
 * @file wifi_ap_api.c
 * @brief WiFi AP API Implementation - OHT-50 Robot WiFi Access Point REST API
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Implementation của WiFi AP API cho OHT-50 robots
 * cung cấp REST endpoints cho WiFi Access Point management.
 */

#include "wifi_ap_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

// WiFi AP API State
static bool wifi_ap_api_initialized = false;
static api_route_t wifi_ap_routes[] = {
    {"/api/v1/network/ap/start", "POST", handle_post_ap_start, AUTH_ADMIN},
    {"/api/v1/network/ap/stop", "POST", handle_post_ap_stop, AUTH_ADMIN},
    {"/api/v1/network/ap/status", "GET", handle_get_ap_status, AUTH_NONE},
    {"/api/v1/network/ap/config", "GET", handle_get_ap_config, AUTH_OPERATOR},
    {"/api/v1/network/ap/config", "POST", handle_post_ap_config, AUTH_ADMIN},
    {"/api/v1/network/ap/clients", "GET", handle_get_ap_clients, AUTH_NONE},
    {"/api/v1/network/ap/clients/kick", "POST", handle_post_ap_kick_client, AUTH_ADMIN},
    {"/api/v1/network/ap/statistics", "GET", handle_get_ap_statistics, AUTH_OPERATOR},
    {"/api/v1/network/ap/statistics/reset", "POST", handle_post_ap_statistics_reset, AUTH_ADMIN},
    {"/api/v1/network/fallback/enable", "POST", handle_post_fallback_enable, AUTH_ADMIN},
    {"/api/v1/network/fallback/status", "GET", handle_get_fallback_status, AUTH_NONE},
    {"/api/v1/network/fallback/trigger", "POST", handle_post_fallback_trigger, AUTH_ADMIN},
    {NULL, NULL, NULL, AUTH_NONE}
};

// Authentication tokens (for development)
static const char* operator_token = "oht50_operator_token_2025";
static const char* admin_token = "oht50_admin_token_2025";

// Internal Functions
static const char* get_current_timestamp(void);
static void log_api_request(const char *method, const char *path, int status_code);

/**
 * @brief Initialize WiFi AP API
 */
int wifi_ap_api_init(void) {
    if (wifi_ap_api_initialized) {
        return WIFI_AP_API_SUCCESS;
    }
    
    // Initialize WiFi AP Manager
    if (wifi_ap_manager_init() != WIFI_AP_SUCCESS) {
        printf("[WIFI_AP_API] Failed to initialize WiFi AP Manager\n");
        return WIFI_AP_API_ERROR_INIT_FAILED;
    }
    
    // Initialize Fallback Manager
    // Note: Fallback Manager will be initialized separately
    // if (fallback_manager_init() != FALLBACK_SUCCESS) {
    //     printf("[WIFI_AP_API] Failed to initialize Fallback Manager\n");
    //     return WIFI_AP_API_ERROR_INIT_FAILED;
    // }
    
    // Register WiFi AP API routes with API Manager
    if (wifi_ap_api_register_routes() != 0) {
        printf("[WIFI_AP_API] Failed to register routes\n");
        return WIFI_AP_API_ERROR_INIT_FAILED;
    }
    
    wifi_ap_api_initialized = true;
    
    printf("[WIFI_AP_API] Initialized successfully\n");
    return WIFI_AP_API_SUCCESS;
}

/**
 * @brief Deinitialize WiFi AP API
 */
int wifi_ap_api_deinit(void) {
    if (!wifi_ap_api_initialized) {
        return WIFI_AP_API_SUCCESS;
    }
    
    wifi_ap_manager_deinit();
    
    wifi_ap_api_initialized = false;
    
    printf("[WIFI_AP_API] Deinitialized successfully\n");
    return WIFI_AP_API_SUCCESS;
}

/**
 * @brief Handle HTTP request for WiFi AP API
 */
int wifi_ap_api_handle_request(http_request_t *req, http_response_t *resp) {
    if (!req || !resp) {
        return WIFI_AP_API_ERROR_INVALID_PARAM;
    }
    
    // Find matching route
    for (int i = 0; wifi_ap_routes[i].path != NULL; i++) {
        if (strcmp(req->path, wifi_ap_routes[i].path) == 0 && 
            strcmp(req->method, wifi_ap_routes[i].method) == 0) {
            
            // Check authentication
            if (wifi_ap_routes[i].required_auth != AUTH_NONE) {
                if (!wifi_ap_api_validate_auth(req->auth_token, wifi_ap_routes[i].required_auth)) {
                    wifi_ap_api_send_error_response(resp, 401, "Unauthorized");
                    return WIFI_AP_API_ERROR_AUTH_FAILED;
                }
            }
            
            // Call handler
            int result = wifi_ap_routes[i].handler(req, resp);
            log_api_request(req->method, req->path, resp->status_code);
            return result;
        }
    }
    
    // Route not found - return error code instead of sending response
    return WIFI_AP_API_ERROR_INVALID_PARAM;
}

/**
 * @brief Send JSON response
 */
int wifi_ap_api_send_json_response(http_response_t *resp, int status_code, const char *json_data) {
    if (!resp || !json_data) {
        return WIFI_AP_API_ERROR_INVALID_PARAM;
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
    
    return WIFI_AP_API_SUCCESS;
}

/**
 * @brief Send error response
 */
int wifi_ap_api_send_error_response(http_response_t *resp, int status_code, const char *error_message) {
    if (!resp || !error_message) {
        return WIFI_AP_API_ERROR_INVALID_PARAM;
    }
    
    char json_error[512];
    snprintf(json_error, sizeof(json_error),
             "{\"success\": false, \"error\": \"%s\", \"timestamp\": \"%s\"}",
             error_message, get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, status_code, json_error);
}

/**
 * @brief Register WiFi AP API routes with API Manager
 */
int wifi_ap_api_register_routes(void) {
    // Register all WiFi AP routes with API Manager
    for (int i = 0; wifi_ap_routes[i].path != NULL; i++) {
        // Convert method string to API Manager method
        api_mgr_http_method_t method;
        if (strcmp(wifi_ap_routes[i].method, "GET") == 0) {
            method = API_MGR_HTTP_GET;
        } else if (strcmp(wifi_ap_routes[i].method, "POST") == 0) {
            method = API_MGR_HTTP_POST;
        } else {
            printf("[WIFI_AP_API] Unsupported method: %s\n", wifi_ap_routes[i].method);
            continue;
        }
        
        // Create wrapper handler that calls wifi_ap_api_handle_request
        int result = api_manager_register_endpoint(wifi_ap_routes[i].path, method, 
                                                  wifi_ap_api_wrapper_handler);
        if (result != 0) {
            printf("[WIFI_AP_API] Failed to register route: %s %s\n", 
                   wifi_ap_routes[i].method, wifi_ap_routes[i].path);
            return -1;
        }
        
        printf("[WIFI_AP_API] Registered route: %s %s\n", 
               wifi_ap_routes[i].method, wifi_ap_routes[i].path);
    }
    
    return 0;
}

/**
 * @brief Wrapper handler for API Manager integration
 */
int wifi_ap_api_wrapper_handler(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // Convert API Manager request to WiFi AP API request
    http_request_t wifi_ap_req;
    http_response_t wifi_ap_resp;
    
    // Copy request data
    strncpy(wifi_ap_req.method, req->method == API_MGR_HTTP_GET ? "GET" : "POST", sizeof(wifi_ap_req.method));
    strncpy(wifi_ap_req.path, req->path, sizeof(wifi_ap_req.path));
    strncpy(wifi_ap_req.body, req->body ? req->body : "", sizeof(wifi_ap_req.body));
    
    // Extract auth token from headers
    wifi_ap_req.auth_token[0] = '\0';
    for (int i = 0; i < req->header_count; i++) {
        if (strcasecmp(req->headers[i].name, "Authorization") == 0) {
            strncpy(wifi_ap_req.auth_token, req->headers[i].value, sizeof(wifi_ap_req.auth_token));
            break;
        }
    }
    
    // Initialize response
    memset(&wifi_ap_resp, 0, sizeof(wifi_ap_resp));
    
    // Call WiFi AP API handler
    int result = wifi_ap_api_handle_request(&wifi_ap_req, &wifi_ap_resp);
    
    // Convert response back to API Manager format
    res->status_code = wifi_ap_resp.status_code;
    if (wifi_ap_resp.body && strlen(wifi_ap_resp.body) > 0) {
        res->body = strdup(wifi_ap_resp.body);
        res->body_length = strlen(wifi_ap_resp.body);
    } else {
        res->body = NULL;
        res->body_length = 0;
    }
    
    return result;
}

/**
 * @brief Validate authentication token
 */
bool wifi_ap_api_validate_auth(const char *token, auth_level_t required_level) {
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
 * @brief Parse JSON request body for string
 */
int wifi_ap_api_parse_json_string(const char *body, const char *key, char *value, int max_len) {
    if (!body || !key || !value || max_len <= 0) {
        return WIFI_AP_API_ERROR_INVALID_PARAM;
    }
    
    char search_key[128];
    snprintf(search_key, sizeof(search_key), "\"%s\":", key);
    
    const char *key_pos = strstr(body, search_key);
    if (!key_pos) {
        return WIFI_AP_API_ERROR_JSON_PARSE_FAILED;
    }
    
    const char *value_start = strchr(key_pos, ':');
    if (!value_start) {
        return WIFI_AP_API_ERROR_JSON_PARSE_FAILED;
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
            return WIFI_AP_API_ERROR_JSON_PARSE_FAILED;
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
    
    return WIFI_AP_API_SUCCESS;
}

/**
 * @brief Parse JSON request body for boolean
 */
int wifi_ap_api_parse_json_bool(const char *body, const char *key, bool *value) {
    if (!body || !key || !value) {
        return WIFI_AP_API_ERROR_INVALID_PARAM;
    }
    
    char str_value[16];
    if (wifi_ap_api_parse_json_string(body, key, str_value, sizeof(str_value)) != WIFI_AP_API_SUCCESS) {
        return WIFI_AP_API_ERROR_JSON_PARSE_FAILED;
    }
    
    if (strcmp(str_value, "true") == 0) {
        *value = true;
    } else if (strcmp(str_value, "false") == 0) {
        *value = false;
    } else {
        return WIFI_AP_API_ERROR_JSON_PARSE_FAILED;
    }
    
    return WIFI_AP_API_SUCCESS;
}

/**
 * @brief Parse JSON request body for integer
 */
int wifi_ap_api_parse_json_int(const char *body, const char *key, int *value) {
    if (!body || !key || !value) {
        return WIFI_AP_API_ERROR_INVALID_PARAM;
    }
    
    char str_value[16];
    if (wifi_ap_api_parse_json_string(body, key, str_value, sizeof(str_value)) != WIFI_AP_API_SUCCESS) {
        return WIFI_AP_API_ERROR_JSON_PARSE_FAILED;
    }
    
    *value = atoi(str_value);
    return WIFI_AP_API_SUCCESS;
}

/**
 * @brief Get current timestamp in ISO format
 */
const char* wifi_ap_api_get_current_timestamp(void) {
    return get_current_timestamp();
}

// API Endpoint Handlers

/**
 * @brief Handle POST /api/v1/network/ap/start
 */
int handle_post_ap_start(http_request_t *req, http_response_t *resp) {
    wifi_ap_config_t config = {0};
    
    // Parse configuration from JSON
    char ssid[32] = "OHT-50-Hotspot";
    char password[64] = "oht50_secure_2025";
    int security_type = AP_SECURITY_WPA2;
    char ip_address[16] = "192.168.4.1";
    char netmask[16] = "255.255.255.0";
    int channel = 6;
    uint32_t max_clients = 10;
    
    wifi_ap_api_parse_json_string(req->body, "ssid", ssid, sizeof(ssid));
    wifi_ap_api_parse_json_string(req->body, "password", password, sizeof(password));
    wifi_ap_api_parse_json_int(req->body, "security_type", &security_type);
    wifi_ap_api_parse_json_string(req->body, "ip_address", ip_address, sizeof(ip_address));
    wifi_ap_api_parse_json_string(req->body, "netmask", netmask, sizeof(netmask));
    wifi_ap_api_parse_json_int(req->body, "channel", &channel);
    wifi_ap_api_parse_json_int(req->body, "max_clients", (int*)&max_clients);
    
    // Set configuration
    config.ap_enabled = true;
    strncpy(config.ap_ssid, ssid, sizeof(config.ap_ssid) - 1);
    strncpy(config.ap_password, password, sizeof(config.ap_password) - 1);
    config.ap_security_type = (ap_security_type_t)security_type;
    strncpy(config.ap_ip, ip_address, sizeof(config.ap_ip) - 1);
    strncpy(config.ap_netmask, netmask, sizeof(config.ap_netmask) - 1);
    config.ap_channel = channel;
    config.max_clients = max_clients;
    config.auto_fallback = true;
    config.fallback_timeout_ms = 30000;
    
    int result = wifi_ap_manager_start(&config);
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"WiFi AP started successfully\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/ap/stop
 */
int handle_post_ap_stop(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    int result = wifi_ap_manager_stop();
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"WiFi AP stopped successfully\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/ap/status
 */
int handle_get_ap_status(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    wifi_ap_status_t status;
    int result = wifi_ap_manager_get_status(&status);
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"status\": %d, "
             "\"ap_enabled\": %s, "
             "\"ap_ssid\": \"%s\", "
             "\"ap_ip\": \"%s\", "
             "\"ap_channel\": %d, "
             "\"connected_clients\": %d, "
             "\"max_clients\": %d, "
             "\"uptime_seconds\": %u, "
             "\"total_bytes_sent\": %u, "
             "\"total_bytes_received\": %u, "
             "\"cpu_usage_percent\": %.1f, "
             "\"memory_usage_percent\": %.1f"
             "}, \"timestamp\": \"%s\"}",
             status.status,
             status.ap_enabled ? "true" : "false",
             status.ap_ssid,
             status.ap_ip,
             status.ap_channel,
             status.connected_clients,
             status.max_clients,
             status.uptime_seconds,
             status.total_bytes_sent,
             status.total_bytes_received,
             status.cpu_usage_percent,
             status.memory_usage_percent,
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/ap/config
 */
int handle_get_ap_config(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    wifi_ap_config_t config;
    int result = wifi_ap_manager_get_config(&config);
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"ap_enabled\": %s, "
             "\"ap_ssid\": \"%s\", "
             "\"ap_security_type\": %d, "
             "\"ap_ip\": \"%s\", "
             "\"ap_netmask\": \"%s\", "
             "\"ap_channel\": %d, "
             "\"max_clients\": %u, "
             "\"auto_fallback\": %s, "
             "\"fallback_timeout_ms\": %u"
             "}, \"timestamp\": \"%s\"}",
             config.ap_enabled ? "true" : "false",
             config.ap_ssid,
             config.ap_security_type,
             config.ap_ip,
             config.ap_netmask,
             config.ap_channel,
             config.max_clients,
             config.auto_fallback ? "true" : "false",
             config.fallback_timeout_ms,
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/ap/config
 */
int handle_post_ap_config(http_request_t *req, http_response_t *resp) {
    wifi_ap_config_t config = {0};
    
    // Parse configuration from JSON
    bool ap_enabled = true;
    char ap_ssid[32] = "OHT-50-Hotspot";
    char ap_password[64] = "oht50_secure_2025";
    int ap_security_type = AP_SECURITY_WPA2;
    char ap_ip[16] = "192.168.4.1";
    char ap_netmask[16] = "255.255.255.0";
    int ap_channel = 6;
    uint32_t max_clients = 10;
    bool auto_fallback = true;
    uint32_t fallback_timeout_ms = 30000;
    
    wifi_ap_api_parse_json_bool(req->body, "ap_enabled", &ap_enabled);
    wifi_ap_api_parse_json_string(req->body, "ap_ssid", ap_ssid, sizeof(ap_ssid));
    wifi_ap_api_parse_json_string(req->body, "ap_password", ap_password, sizeof(ap_password));
    wifi_ap_api_parse_json_int(req->body, "ap_security_type", &ap_security_type);
    wifi_ap_api_parse_json_string(req->body, "ap_ip", ap_ip, sizeof(ap_ip));
    wifi_ap_api_parse_json_string(req->body, "ap_netmask", ap_netmask, sizeof(ap_netmask));
    wifi_ap_api_parse_json_int(req->body, "ap_channel", &ap_channel);
    wifi_ap_api_parse_json_int(req->body, "max_clients", (int*)&max_clients);
    wifi_ap_api_parse_json_bool(req->body, "auto_fallback", &auto_fallback);
    wifi_ap_api_parse_json_int(req->body, "fallback_timeout_ms", (int*)&fallback_timeout_ms);
    
    // Set configuration
    config.ap_enabled = ap_enabled;
    strncpy(config.ap_ssid, ap_ssid, sizeof(config.ap_ssid) - 1);
    strncpy(config.ap_password, ap_password, sizeof(config.ap_password) - 1);
    config.ap_security_type = (ap_security_type_t)ap_security_type;
    strncpy(config.ap_ip, ap_ip, sizeof(config.ap_ip) - 1);
    strncpy(config.ap_netmask, ap_netmask, sizeof(config.ap_netmask) - 1);
    config.ap_channel = ap_channel;
    config.max_clients = max_clients;
    config.auto_fallback = auto_fallback;
    config.fallback_timeout_ms = fallback_timeout_ms;
    
    int result = wifi_ap_manager_set_config(&config);
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"AP configuration updated\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/ap/clients
 */
int handle_get_ap_clients(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    wifi_ap_client_t clients[10];
    int count = wifi_ap_manager_get_clients(clients, 10);
    
    if (count < 0) {
        const char *error_msg = wifi_ap_manager_get_error_message(count);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[4096] = "{\"success\": true, \"data\": {\"clients\": [";
    
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            strcat(json_response, ", ");
        }
        
        char client_json[512];
        snprintf(client_json, sizeof(client_json),
                 "{\"mac_address\": \"%s\", \"ip_address\": \"%s\", \"hostname\": \"%s\", "
                 "\"signal_strength_dbm\": %d, \"connected_time_seconds\": %u, "
                 "\"bytes_sent\": %u, \"bytes_received\": %u, \"authenticated\": %s}",
                 clients[i].mac_address,
                 clients[i].ip_address,
                 clients[i].hostname,
                 clients[i].signal_strength_dbm,
                 clients[i].connected_time_seconds,
                 clients[i].bytes_sent,
                 clients[i].bytes_received,
                 clients[i].authenticated ? "true" : "false");
        
        strcat(json_response, client_json);
    }
    
    strcat(json_response, "], \"count\": ");
    char count_str[16];
    snprintf(count_str, sizeof(count_str), "%d", count);
    strcat(json_response, count_str);
    
    snprintf(json_response + strlen(json_response), 
             sizeof(json_response) - strlen(json_response),
             "}, \"timestamp\": \"%s\"}", get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/ap/clients/kick
 */
int handle_post_ap_kick_client(http_request_t *req, http_response_t *resp) {
    char mac_address[18];
    
    if (wifi_ap_api_parse_json_string(req->body, "mac_address", mac_address, sizeof(mac_address)) != WIFI_AP_API_SUCCESS) {
        return wifi_ap_api_send_error_response(resp, 400, "Missing mac_address parameter");
    }
    
    int result = wifi_ap_manager_kick_client(mac_address);
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"Client kicked successfully\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/ap/statistics
 */
int handle_get_ap_statistics(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    wifi_ap_statistics_t stats;
    int result = wifi_ap_manager_get_statistics(&stats);
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"ap_start_count\": %u, "
             "\"ap_stop_count\": %u, "
             "\"client_connections\": %u, "
             "\"client_disconnections\": %u, "
             "\"fallback_triggers\": %u, "
             "\"total_uptime_seconds\": %u, "
             "\"total_bytes_sent\": %u, "
             "\"total_bytes_received\": %u, "
             "\"average_cpu_usage\": %.1f, "
             "\"average_memory_usage\": %.1f"
             "}, \"timestamp\": \"%s\"}",
             stats.ap_start_count,
             stats.ap_stop_count,
             stats.client_connections,
             stats.client_disconnections,
             stats.fallback_triggers,
             stats.total_uptime_seconds,
             stats.total_bytes_sent,
             stats.total_bytes_received,
             stats.average_cpu_usage,
             stats.average_memory_usage,
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/ap/statistics/reset
 */
int handle_post_ap_statistics_reset(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    int result = wifi_ap_manager_reset_statistics();
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"AP statistics reset successfully\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/fallback/enable
 */
int handle_post_fallback_enable(http_request_t *req, http_response_t *resp) {
    bool enabled;
    uint32_t timeout_ms = 30000;
    
    if (wifi_ap_api_parse_json_bool(req->body, "enabled", &enabled) != WIFI_AP_API_SUCCESS) {
        return wifi_ap_api_send_error_response(resp, 400, "Missing enabled parameter");
    }
    
    wifi_ap_api_parse_json_int(req->body, "timeout_ms", (int*)&timeout_ms);
    
    int result = wifi_ap_manager_set_auto_fallback(enabled, timeout_ms);
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"Auto-fallback configuration updated\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle GET /api/v1/network/fallback/status
 */
int handle_get_fallback_status(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    bool enabled;
    uint32_t timeout_ms;
    
    int result = wifi_ap_manager_get_fallback_status(&enabled, &timeout_ms);
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"data\": {"
             "\"enabled\": %s, "
             "\"timeout_ms\": %u"
             "}, \"timestamp\": \"%s\"}",
             enabled ? "true" : "false",
             timeout_ms,
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Handle POST /api/v1/network/fallback/trigger
 */
int handle_post_fallback_trigger(http_request_t *req __attribute__((unused)), http_response_t *resp) {
    int result = wifi_ap_manager_trigger_fallback();
    
    if (result != WIFI_AP_SUCCESS) {
        const char *error_msg = wifi_ap_manager_get_error_message(result);
        return wifi_ap_api_send_error_response(resp, 500, error_msg);
    }
    
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"success\": true, \"message\": \"Fallback triggered successfully\", \"timestamp\": \"%s\"}",
             get_current_timestamp());
    
    return wifi_ap_api_send_json_response(resp, 200, json_response);
}

/**
 * @brief Get error message for error code
 */
const char* wifi_ap_api_get_error_message(int error_code) {
    switch (error_code) {
        case WIFI_AP_API_SUCCESS:
            return "Success";
        case WIFI_AP_API_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case WIFI_AP_API_ERROR_INIT_FAILED:
            return "Initialization failed";
        case WIFI_AP_API_ERROR_NOT_INITIALIZED:
            return "API not initialized";
        case WIFI_AP_API_ERROR_AUTH_FAILED:
            return "Authentication failed";
        case WIFI_AP_API_ERROR_JSON_PARSE_FAILED:
            return "JSON parse failed";
        case WIFI_AP_API_ERROR_MANAGER_FAILED:
            return "Manager operation failed";
        default:
            return "Unknown error";
    }
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
    printf("[WIFI_AP_API] %s %s -> %d\n", method, path, status_code);
}
