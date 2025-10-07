/**
 * @file test_wifi_ap_api.c
 * @brief WiFi AP API Unit Tests - OHT-50 Robot WiFi Access Point API Tests
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Unit tests cho WiFi AP API functionality
 * bao gồm REST endpoints, authentication, và JSON parsing testing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include "wifi_ap_api.h"
#include "wifi_ap_manager.h"

// Test Configuration
#define TEST_SSID "OHT-50-Test-Hotspot"
#define TEST_PASSWORD "test_password_2025"
#define TEST_IP "192.168.5.1"
#define TEST_NETMASK "255.255.255.0"
#define TEST_CHANNEL 11
#define TEST_MAX_CLIENTS 5

// Test Statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Test Helper Functions
static void test_assert(bool condition, const char *test_name, const char *message);
static void test_wifi_ap_api_init(void);
static void test_wifi_ap_api_deinit(void);
static void test_wifi_ap_api_validate_auth(void);
static void test_wifi_ap_api_parse_json_string(void);
static void test_wifi_ap_api_parse_json_bool(void);
static void test_wifi_ap_api_parse_json_int(void);
static void test_wifi_ap_api_send_json_response(void);
static void test_wifi_ap_api_send_error_response(void);
static void test_wifi_ap_api_get_current_timestamp(void);
static void test_handle_post_ap_start(void);
static void test_handle_post_ap_stop(void);
static void test_handle_get_ap_status(void);
static void test_handle_get_ap_config(void);
static void test_handle_post_ap_config(void);
static void test_handle_get_ap_clients(void);
static void test_handle_post_ap_kick_client(void);
static void test_handle_get_ap_statistics(void);
static void test_handle_post_ap_statistics_reset(void);
static void test_handle_post_fallback_enable(void);
static void test_handle_get_fallback_status(void);
static void test_handle_post_fallback_trigger(void);
static void test_wifi_ap_api_error_messages(void);
static void test_wifi_ap_api_edge_cases(void);
static void test_wifi_ap_api_performance(void);

/**
 * @brief Test assertion helper
 */
static void test_assert(bool condition, const char *test_name, const char *message) {
    tests_run++;
    
    if (condition) {
        tests_passed++;
        printf("✅ PASS: %s - %s\n", test_name, message);
    } else {
        tests_failed++;
        printf("❌ FAIL: %s - %s\n", test_name, message);
    }
}

/**
 * @brief Test WiFi AP API initialization
 */
static void test_wifi_ap_api_init(void) {
    printf("\n🔧 Testing WiFi AP API Initialization...\n");
    
    // Test successful initialization
    int result = wifi_ap_api_init();
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_init", 
                "Should initialize successfully");
    
    // Test double initialization
    result = wifi_ap_api_init();
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_init_double", 
                "Should handle double initialization gracefully");
}

/**
 * @brief Test WiFi AP API deinitialization
 */
static void test_wifi_ap_api_deinit(void) {
    printf("\n🔧 Testing WiFi AP API Deinitialization...\n");
    
    // Test successful deinitialization
    int result = wifi_ap_api_deinit();
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_deinit", 
                "Should deinitialize successfully");
    
    // Test double deinitialization
    result = wifi_ap_api_deinit();
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_deinit_double", 
                "Should handle double deinitialization gracefully");
    
    // Reinitialize for other tests
    wifi_ap_api_init();
}

/**
 * @brief Test WiFi AP API authentication validation
 */
static void test_wifi_ap_api_validate_auth(void) {
    printf("\n🔧 Testing WiFi AP API Authentication Validation...\n");
    
    // Test valid operator token
    bool result = wifi_ap_api_validate_auth("oht50_operator_token_2025", AUTH_OPERATOR);
    test_assert(result == true, "wifi_ap_api_validate_auth_operator", 
                "Should validate operator token");
    
    // Test valid admin token
    result = wifi_ap_api_validate_auth("oht50_admin_token_2025", AUTH_ADMIN);
    test_assert(result == true, "wifi_ap_api_validate_auth_admin", 
                "Should validate admin token");
    
    // Test invalid operator token
    result = wifi_ap_api_validate_auth("invalid_token", AUTH_OPERATOR);
    test_assert(result == false, "wifi_ap_api_validate_auth_invalid_operator", 
                "Should reject invalid operator token");
    
    // Test invalid admin token
    result = wifi_ap_api_validate_auth("invalid_token", AUTH_ADMIN);
    test_assert(result == false, "wifi_ap_api_validate_auth_invalid_admin", 
                "Should reject invalid admin token");
    
    // Test NULL token
    result = wifi_ap_api_validate_auth(NULL, AUTH_OPERATOR);
    test_assert(result == false, "wifi_ap_api_validate_auth_null", 
                "Should reject NULL token");
    
    // Test AUTH_NONE (should always pass)
    result = wifi_ap_api_validate_auth(NULL, AUTH_NONE);
    test_assert(result == true, "wifi_ap_api_validate_auth_none", 
                "Should pass for AUTH_NONE");
}

/**
 * @brief Test WiFi AP API JSON string parsing
 */
static void test_wifi_ap_api_parse_json_string(void) {
    printf("\n🔧 Testing WiFi AP API JSON String Parsing...\n");
    
    // Test valid JSON string parsing
    const char* json = "{\"ssid\": \"TestSSID\", \"password\": \"testpass123\"}";
    char value[32];
    
    int result = wifi_ap_api_parse_json_string(json, "ssid", value, sizeof(value));
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_parse_json_string_valid", 
                "Should parse valid JSON string");
    test_assert(strcmp(value, "TestSSID") == 0, "wifi_ap_api_parse_json_string_value", 
                "Should extract correct value");
    
    // Test missing key
    result = wifi_ap_api_parse_json_string(json, "missing_key", value, sizeof(value));
    test_assert(result == WIFI_AP_API_ERROR_JSON_PARSE_FAILED, "wifi_ap_api_parse_json_string_missing_key", 
                "Should fail for missing key");
    
    // Test NULL parameters
    result = wifi_ap_api_parse_json_string(NULL, "ssid", value, sizeof(value));
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_string_null_json", 
                "Should fail with NULL JSON");
    
    result = wifi_ap_api_parse_json_string(json, NULL, value, sizeof(value));
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_string_null_key", 
                "Should fail with NULL key");
    
    result = wifi_ap_api_parse_json_string(json, "ssid", NULL, sizeof(value));
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_string_null_value", 
                "Should fail with NULL value buffer");
    
    result = wifi_ap_api_parse_json_string(json, "ssid", value, 0);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_string_invalid_max_len", 
                "Should fail with invalid max_len");
}

/**
 * @brief Test WiFi AP API JSON boolean parsing
 */
static void test_wifi_ap_api_parse_json_bool(void) {
    printf("\n🔧 Testing WiFi AP API JSON Boolean Parsing...\n");
    
    // Test valid JSON boolean parsing
    const char* json = "{\"enabled\": true, \"disabled\": false}";
    bool value;
    
    int result = wifi_ap_api_parse_json_bool(json, "enabled", &value);
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_parse_json_bool_true", 
                "Should parse true boolean");
    test_assert(value == true, "wifi_ap_api_parse_json_bool_true_value", 
                "Should extract true value");
    
    result = wifi_ap_api_parse_json_bool(json, "disabled", &value);
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_parse_json_bool_false", 
                "Should parse false boolean");
    test_assert(value == false, "wifi_ap_api_parse_json_bool_false_value", 
                "Should extract false value");
    
    // Test invalid boolean value
    const char* invalid_json = "{\"invalid\": \"not_boolean\"}";
    result = wifi_ap_api_parse_json_bool(invalid_json, "invalid", &value);
    test_assert(result == WIFI_AP_API_ERROR_JSON_PARSE_FAILED, "wifi_ap_api_parse_json_bool_invalid", 
                "Should fail for invalid boolean");
    
    // Test NULL parameters
    result = wifi_ap_api_parse_json_bool(NULL, "enabled", &value);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_bool_null_json", 
                "Should fail with NULL JSON");
    
    result = wifi_ap_api_parse_json_bool(json, NULL, &value);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_bool_null_key", 
                "Should fail with NULL key");
    
    result = wifi_ap_api_parse_json_bool(json, "enabled", NULL);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_bool_null_value", 
                "Should fail with NULL value pointer");
}

/**
 * @brief Test WiFi AP API JSON integer parsing
 */
static void test_wifi_ap_api_parse_json_int(void) {
    printf("\n🔧 Testing WiFi AP API JSON Integer Parsing...\n");
    
    // Test valid JSON integer parsing
    const char* json = "{\"channel\": 6, \"max_clients\": 10}";
    int value;
    
    int result = wifi_ap_api_parse_json_int(json, "channel", &value);
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_parse_json_int_valid", 
                "Should parse valid integer");
    test_assert(value == 6, "wifi_ap_api_parse_json_int_value", 
                "Should extract correct integer value");
    
    result = wifi_ap_api_parse_json_int(json, "max_clients", &value);
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_parse_json_int_max_clients", 
                "Should parse max_clients integer");
    test_assert(value == 10, "wifi_ap_api_parse_json_int_max_clients_value", 
                "Should extract correct max_clients value");
    
    // Test NULL parameters
    result = wifi_ap_api_parse_json_int(NULL, "channel", &value);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_int_null_json", 
                "Should fail with NULL JSON");
    
    result = wifi_ap_api_parse_json_int(json, NULL, &value);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_int_null_key", 
                "Should fail with NULL key");
    
    result = wifi_ap_api_parse_json_int(json, "channel", NULL);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_parse_json_int_null_value", 
                "Should fail with NULL value pointer");
}

/**
 * @brief Test WiFi AP API send JSON response
 */
static void test_wifi_ap_api_send_json_response(void) {
    printf("\n🔧 Testing WiFi AP API Send JSON Response...\n");
    
    // Test send valid JSON response
    http_response_t resp = {0};
    const char* json_data = "{\"success\": true, \"message\": \"Test\"}";
    
    int result = wifi_ap_api_send_json_response(&resp, 200, json_data);
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_send_json_response", 
                "Should send JSON response successfully");
    
    test_assert(resp.status_code == 200, "wifi_ap_api_send_json_response_status", 
                "Should set correct status code");
    
    test_assert(resp.content_length == strlen(json_data), "wifi_ap_api_send_json_response_length", 
                "Should set correct content length");
    
    test_assert(strstr(resp.headers, "Content-Type: application/json") != NULL, "wifi_ap_api_send_json_response_headers", 
                "Should set correct headers");
    
    test_assert(strcmp(resp.body, json_data) == 0, "wifi_ap_api_send_json_response_body", 
                "Should set correct body");
    
    // Test send JSON response with NULL parameters
    result = wifi_ap_api_send_json_response(NULL, 200, json_data);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_send_json_response_null_resp", 
                "Should fail with NULL response");
    
    result = wifi_ap_api_send_json_response(&resp, 200, NULL);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_send_json_response_null_data", 
                "Should fail with NULL JSON data");
}

/**
 * @brief Test WiFi AP API send error response
 */
static void test_wifi_ap_api_send_error_response(void) {
    printf("\n🔧 Testing WiFi AP API Send Error Response...\n");
    
    // Test send valid error response
    http_response_t resp = {0};
    const char* error_message = "Test error message";
    
    int result = wifi_ap_api_send_error_response(&resp, 400, error_message);
    test_assert(result == WIFI_AP_API_SUCCESS, "wifi_ap_api_send_error_response", 
                "Should send error response successfully");
    
    test_assert(resp.status_code == 400, "wifi_ap_api_send_error_response_status", 
                "Should set correct status code");
    
    test_assert(strstr(resp.body, "success\": false") != NULL, "wifi_ap_api_send_error_response_success", 
                "Should set success to false");
    
    test_assert(strstr(resp.body, error_message) != NULL, "wifi_ap_api_send_error_response_message", 
                "Should include error message");
    
    test_assert(strstr(resp.body, "timestamp") != NULL, "wifi_ap_api_send_error_response_timestamp", 
                "Should include timestamp");
    
    // Test send error response with NULL parameters
    result = wifi_ap_api_send_error_response(NULL, 400, error_message);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_send_error_response_null_resp", 
                "Should fail with NULL response");
    
    result = wifi_ap_api_send_error_response(&resp, 400, NULL);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_send_error_response_null_message", 
                "Should fail with NULL error message");
}

/**
 * @brief Test WiFi AP API get current timestamp
 */
static void test_wifi_ap_api_get_current_timestamp(void) {
    printf("\n🔧 Testing WiFi AP API Get Current Timestamp...\n");
    
    // Test get current timestamp
    const char* timestamp = wifi_ap_api_get_current_timestamp();
    test_assert(timestamp != NULL, "wifi_ap_api_get_current_timestamp", 
                "Should get timestamp successfully");
    
    test_assert(strlen(timestamp) > 0, "wifi_ap_api_get_current_timestamp_not_empty", 
                "Timestamp should not be empty");
    
    test_assert(strstr(timestamp, "T") != NULL, "wifi_ap_api_get_current_timestamp_format", 
                "Timestamp should be in ISO format");
    
    test_assert(strstr(timestamp, "Z") != NULL, "wifi_ap_api_get_current_timestamp_utc", 
                "Timestamp should be UTC");
}

/**
 * @brief Test handle POST AP start
 */
static void test_handle_post_ap_start(void) {
    printf("\n🔧 Testing Handle POST AP Start...\n");
    
    // Test valid AP start request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    const char* json_body = "{\"ssid\": \"TestSSID\", \"password\": \"testpass123\", \"channel\": 6}";
    strncpy(req.body, json_body, sizeof(req.body) - 1);
    req.auth_token = "oht50_admin_token_2025";
    
    int result = handle_post_ap_start(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_post_ap_start", 
                "Should handle AP start request successfully");
    
    test_assert(resp.status_code == 200, "handle_post_ap_start_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_post_ap_start_success", 
                "Should return success response");
}

/**
 * @brief Test handle POST AP stop
 */
static void test_handle_post_ap_stop(void) {
    printf("\n🔧 Testing Handle POST AP Stop...\n");
    
    // Test valid AP stop request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    req.auth_token = "oht50_admin_token_2025";
    
    int result = handle_post_ap_stop(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_post_ap_stop", 
                "Should handle AP stop request successfully");
    
    test_assert(resp.status_code == 200, "handle_post_ap_stop_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_post_ap_stop_success", 
                "Should return success response");
}

/**
 * @brief Test handle GET AP status
 */
static void test_handle_get_ap_status(void) {
    printf("\n🔧 Testing Handle GET AP Status...\n");
    
    // Test valid AP status request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    int result = handle_get_ap_status(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_get_ap_status", 
                "Should handle AP status request successfully");
    
    test_assert(resp.status_code == 200, "handle_get_ap_status_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_get_ap_status_success", 
                "Should return success response");
    
    test_assert(strstr(resp.body, "data") != NULL, "handle_get_ap_status_data", 
                "Should include data in response");
}

/**
 * @brief Test handle GET AP config
 */
static void test_handle_get_ap_config(void) {
    printf("\n🔧 Testing Handle GET AP Config...\n");
    
    // Test valid AP config request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    req.auth_token = "oht50_operator_token_2025";
    
    int result = handle_get_ap_config(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_get_ap_config", 
                "Should handle AP config request successfully");
    
    test_assert(resp.status_code == 200, "handle_get_ap_config_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_get_ap_config_success", 
                "Should return success response");
    
    test_assert(strstr(resp.body, "data") != NULL, "handle_get_ap_config_data", 
                "Should include data in response");
}

/**
 * @brief Test handle POST AP config
 */
static void test_handle_post_ap_config(void) {
    printf("\n🔧 Testing Handle POST AP Config...\n");
    
    // Test valid AP config request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    const char* json_body = "{\"ap_ssid\": \"TestSSID\", \"ap_channel\": 6, \"max_clients\": 10}";
    strncpy(req.body, json_body, sizeof(req.body) - 1);
    req.auth_token = "oht50_admin_token_2025";
    
    int result = handle_post_ap_config(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_post_ap_config", 
                "Should handle AP config request successfully");
    
    test_assert(resp.status_code == 200, "handle_post_ap_config_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_post_ap_config_success", 
                "Should return success response");
}

/**
 * @brief Test handle GET AP clients
 */
static void test_handle_get_ap_clients(void) {
    printf("\n🔧 Testing Handle GET AP Clients...\n");
    
    // Test valid AP clients request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    int result = handle_get_ap_clients(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_get_ap_clients", 
                "Should handle AP clients request successfully");
    
    test_assert(resp.status_code == 200, "handle_get_ap_clients_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_get_ap_clients_success", 
                "Should return success response");
    
    test_assert(strstr(resp.body, "clients") != NULL, "handle_get_ap_clients_data", 
                "Should include clients data");
}

/**
 * @brief Test handle POST AP kick client
 */
static void test_handle_post_ap_kick_client(void) {
    printf("\n🔧 Testing Handle POST AP Kick Client...\n");
    
    // Test valid AP kick client request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    const char* json_body = "{\"mac_address\": \"aa:bb:cc:dd:ee:ff\"}";
    strncpy(req.body, json_body, sizeof(req.body) - 1);
    req.auth_token = "oht50_admin_token_2025";
    
    int result = handle_post_ap_kick_client(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_post_ap_kick_client", 
                "Should handle AP kick client request successfully");
    
    test_assert(resp.status_code == 200, "handle_post_ap_kick_client_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_post_ap_kick_client_success", 
                "Should return success response");
}

/**
 * @brief Test handle GET AP statistics
 */
static void test_handle_get_ap_statistics(void) {
    printf("\n🔧 Testing Handle GET AP Statistics...\n");
    
    // Test valid AP statistics request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    req.auth_token = "oht50_operator_token_2025";
    
    int result = handle_get_ap_statistics(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_get_ap_statistics", 
                "Should handle AP statistics request successfully");
    
    test_assert(resp.status_code == 200, "handle_get_ap_statistics_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_get_ap_statistics_success", 
                "Should return success response");
    
    test_assert(strstr(resp.body, "data") != NULL, "handle_get_ap_statistics_data", 
                "Should include data in response");
}

/**
 * @brief Test handle POST AP statistics reset
 */
static void test_handle_post_ap_statistics_reset(void) {
    printf("\n🔧 Testing Handle POST AP Statistics Reset...\n");
    
    // Test valid AP statistics reset request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    req.auth_token = "oht50_admin_token_2025";
    
    int result = handle_post_ap_statistics_reset(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_post_ap_statistics_reset", 
                "Should handle AP statistics reset request successfully");
    
    test_assert(resp.status_code == 200, "handle_post_ap_statistics_reset_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_post_ap_statistics_reset_success", 
                "Should return success response");
}

/**
 * @brief Test handle POST fallback enable
 */
static void test_handle_post_fallback_enable(void) {
    printf("\n🔧 Testing Handle POST Fallback Enable...\n");
    
    // Test valid fallback enable request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    const char* json_body = "{\"enabled\": true, \"timeout_ms\": 30000}";
    strncpy(req.body, json_body, sizeof(req.body) - 1);
    req.auth_token = "oht50_admin_token_2025";
    
    int result = handle_post_fallback_enable(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_post_fallback_enable", 
                "Should handle fallback enable request successfully");
    
    test_assert(resp.status_code == 200, "handle_post_fallback_enable_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_post_fallback_enable_success", 
                "Should return success response");
}

/**
 * @brief Test handle GET fallback status
 */
static void test_handle_get_fallback_status(void) {
    printf("\n🔧 Testing Handle GET Fallback Status...\n");
    
    // Test valid fallback status request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    int result = handle_get_fallback_status(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_get_fallback_status", 
                "Should handle fallback status request successfully");
    
    test_assert(resp.status_code == 200, "handle_get_fallback_status_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_get_fallback_status_success", 
                "Should return success response");
    
    test_assert(strstr(resp.body, "data") != NULL, "handle_get_fallback_status_data", 
                "Should include data in response");
}

/**
 * @brief Test handle POST fallback trigger
 */
static void test_handle_post_fallback_trigger(void) {
    printf("\n🔧 Testing Handle POST Fallback Trigger...\n");
    
    // Test valid fallback trigger request
    http_request_t req = {0};
    http_response_t resp = {0};
    
    req.auth_token = "oht50_admin_token_2025";
    
    int result = handle_post_fallback_trigger(&req, &resp);
    test_assert(result == WIFI_AP_API_SUCCESS, "handle_post_fallback_trigger", 
                "Should handle fallback trigger request successfully");
    
    test_assert(resp.status_code == 200, "handle_post_fallback_trigger_status", 
                "Should return 200 status code");
    
    test_assert(strstr(resp.body, "success\": true") != NULL, "handle_post_fallback_trigger_success", 
                "Should return success response");
}

/**
 * @brief Test WiFi AP API error messages
 */
static void test_wifi_ap_api_error_messages(void) {
    printf("\n🔧 Testing WiFi AP API Error Messages...\n");
    
    // Test all error codes have messages
    const char* success_msg = wifi_ap_api_get_error_message(WIFI_AP_API_SUCCESS);
    test_assert(strlen(success_msg) > 0, "wifi_ap_api_error_message_success", 
                "Success message should not be empty");
    
    const char* invalid_param_msg = wifi_ap_api_get_error_message(WIFI_AP_API_ERROR_INVALID_PARAM);
    test_assert(strlen(invalid_param_msg) > 0, "wifi_ap_api_error_message_invalid_param", 
                "Invalid param message should not be empty");
    
    const char* init_failed_msg = wifi_ap_api_get_error_message(WIFI_AP_API_ERROR_INIT_FAILED);
    test_assert(strlen(init_failed_msg) > 0, "wifi_ap_api_error_message_init_failed", 
                "Init failed message should not be empty");
    
    const char* unknown_msg = wifi_ap_api_get_error_message(999);
    test_assert(strlen(unknown_msg) > 0, "wifi_ap_api_error_message_unknown", 
                "Unknown error message should not be empty");
}

/**
 * @brief Test WiFi AP API edge cases
 */
static void test_wifi_ap_api_edge_cases(void) {
    printf("\n🔧 Testing WiFi AP API Edge Cases...\n");
    
    // Test operations when not initialized
    wifi_ap_api_deinit();
    
    http_request_t req = {0};
    http_response_t resp = {0};
    
    int result = wifi_ap_api_handle_request(&req, &resp);
    test_assert(result == WIFI_AP_API_ERROR_INVALID_PARAM, "wifi_ap_api_handle_request_not_initialized", 
                "Should fail when not initialized");
    
    // Reinitialize for other tests
    wifi_ap_api_init();
}

/**
 * @brief Test WiFi AP API performance
 */
static void test_wifi_ap_api_performance(void) {
    printf("\n🔧 Testing WiFi AP API Performance...\n");
    
    // Test JSON parsing performance
    const char* json = "{\"ssid\": \"TestSSID\", \"password\": \"testpass123\", \"channel\": 6}";
    char value[32];
    
    clock_t start = clock();
    for (int i = 0; i < 1000; i++) {
        wifi_ap_api_parse_json_string(json, "ssid", value, sizeof(value));
    }
    clock_t end = clock();
    double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(cpu_time < 1.0, "wifi_ap_api_performance_json_parsing", 
                "JSON parsing should be fast (< 1 second for 1000 calls)");
    
    // Test timestamp generation performance
    start = clock();
    for (int i = 0; i < 1000; i++) {
        wifi_ap_api_get_current_timestamp();
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(cpu_time < 1.0, "wifi_ap_api_performance_timestamp", 
                "Timestamp generation should be fast (< 1 second for 1000 calls)");
}

/**
 * @brief Main test function
 */
int main(void) {
    printf("🚀 Starting WiFi AP API Unit Tests...\n");
    printf("==========================================\n");
    
    // Run all tests
    test_wifi_ap_api_init();
    test_wifi_ap_api_deinit();
    test_wifi_ap_api_validate_auth();
    test_wifi_ap_api_parse_json_string();
    test_wifi_ap_api_parse_json_bool();
    test_wifi_ap_api_parse_json_int();
    test_wifi_ap_api_send_json_response();
    test_wifi_ap_api_send_error_response();
    test_wifi_ap_api_get_current_timestamp();
    test_handle_post_ap_start();
    test_handle_post_ap_stop();
    test_handle_get_ap_status();
    test_handle_get_ap_config();
    test_handle_post_ap_config();
    test_handle_get_ap_clients();
    test_handle_post_ap_kick_client();
    test_handle_get_ap_statistics();
    test_handle_post_ap_statistics_reset();
    test_handle_post_fallback_enable();
    test_handle_get_fallback_status();
    test_handle_post_fallback_trigger();
    test_wifi_ap_api_error_messages();
    test_wifi_ap_api_edge_cases();
    test_wifi_ap_api_performance();
    
    // Print test results
    printf("\n==========================================\n");
    printf("📊 Test Results Summary:\n");
    printf("   Total Tests: %d\n", tests_run);
    printf("   ✅ Passed: %d\n", tests_passed);
    printf("   ❌ Failed: %d\n", tests_failed);
    printf("   📈 Success Rate: %.1f%%\n", (float)tests_passed / tests_run * 100.0f);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! WiFi AP API is working correctly.\n");
        return 0;
    } else {
        printf("\n⚠️  Some tests failed. Please check the implementation.\n");
        return 1;
    }
}
