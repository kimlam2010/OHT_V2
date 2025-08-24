/**
 * @file test_api_manager.c
 * @brief Unit tests for API Manager functionality
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "unity.h"
#include "api_manager.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// Test fixtures
static api_mgr_config_t test_config;
static api_mgr_http_request_t test_request;
static api_mgr_http_response_t test_response;

void setUp(void) {
    // Initialize test configuration
    test_config.http_port = 8080;
    test_config.websocket_port = 8081;
    test_config.http_enabled = true;
    test_config.websocket_enabled = true;
    test_config.cors_enabled = true;
    strcpy(test_config.cors_origin, "http://localhost:3000");
    test_config.max_request_size = 4096;
    test_config.max_response_size = 8192;
    test_config.request_timeout_ms = 5000;
    test_config.websocket_timeout_ms = 30000;
    test_config.authentication_required = false;
    test_config.ssl_enabled = false;
    
    // Initialize test request
    memset(&test_request, 0, sizeof(test_request));
    test_request.method = API_MGR_HTTP_GET;
    strcpy(test_request.url, "/api/v1/system/status");
    strcpy(test_request.path, "/api/v1/system/status");
    test_request.content_type = API_MGR_CONTENT_TYPE_JSON;
    strcpy(test_request.client_ip, "127.0.0.1");
    
    // Initialize test response
    memset(&test_response, 0, sizeof(test_response));
}

void tearDown(void) {
    // Clean up after each test
    api_manager_deinit();
}

// Test API Manager initialization
void test_api_manager_init_returns_success(void) {
    hal_status_t status = api_manager_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_api_manager_init_null_config_returns_error(void) {
    hal_status_t status = api_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_api_manager_init_already_initialized_returns_already_initialized(void) {
    api_manager_init(&test_config);
    hal_status_t status = api_manager_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ALREADY_INITIALIZED, status);
}

// Test API Manager configuration
void test_api_manager_get_config_returns_valid_config(void) {
    api_manager_init(&test_config);
    
    api_mgr_config_t config;
    hal_status_t status = api_manager_get_config(&config);
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(test_config.http_port, config.http_port);
    TEST_ASSERT_EQUAL(test_config.websocket_port, config.websocket_port);
    TEST_ASSERT_EQUAL(test_config.http_enabled, config.http_enabled);
    TEST_ASSERT_EQUAL(test_config.websocket_enabled, config.websocket_enabled);
}

void test_api_manager_get_config_null_config_returns_error(void) {
    api_manager_init(&test_config);
    
    hal_status_t status = api_manager_get_config(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_api_manager_get_config_not_initialized_returns_error(void) {
    api_mgr_config_t config;
    hal_status_t status = api_manager_get_config(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Test API Manager status
void test_api_manager_get_status_returns_valid_status(void) {
    api_manager_init(&test_config);
    
    api_mgr_status_t status;
    hal_status_t result = api_manager_get_status(&status);
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_TRUE(status.initialized);
    TEST_ASSERT_EQUAL(test_config.http_enabled, status.http_server_active);
    TEST_ASSERT_EQUAL(test_config.websocket_enabled, status.websocket_server_active);
}

void test_api_manager_get_status_null_status_returns_error(void) {
    api_manager_init(&test_config);
    
    hal_status_t result = api_manager_get_status(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, result);
}

void test_api_manager_get_status_not_initialized_returns_error(void) {
    api_mgr_status_t status;
    hal_status_t result = api_manager_get_status(&status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, result);
}

// Test API Manager statistics
void test_api_manager_get_statistics_returns_valid_stats(void) {
    api_manager_init(&test_config);
    
    api_mgr_stats_t stats;
    hal_status_t result = api_manager_get_statistics(&stats);
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_GREATER_THAN_OR_EQUAL(0, stats.http_requests_received);
    TEST_ASSERT_GREATER_THAN_OR_EQUAL(0, stats.http_responses_sent);
    TEST_ASSERT_GREATER_THAN_OR_EQUAL(0, stats.websocket_connections);
}

void test_api_manager_get_statistics_null_stats_returns_error(void) {
    api_manager_init(&test_config);
    
    hal_status_t result = api_manager_get_statistics(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, result);
}

void test_api_manager_get_statistics_not_initialized_returns_error(void) {
    api_mgr_stats_t stats;
    hal_status_t result = api_manager_get_statistics(&stats);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, result);
}

// Test API Manager request handling
void test_api_manager_process_http_request_returns_success(void) {
    api_manager_init(&test_config);
    
    hal_status_t status = api_manager_process_http_request(&test_request, &test_response);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_api_manager_process_http_request_null_request_returns_error(void) {
    api_manager_init(&test_config);
    
    hal_status_t status = api_manager_process_http_request(NULL, &test_response);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_api_manager_process_http_request_null_response_returns_error(void) {
    api_manager_init(&test_config);
    
    hal_status_t status = api_manager_process_http_request(&test_request, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_api_manager_process_http_request_not_initialized_returns_error(void) {
    hal_status_t status = api_manager_process_http_request(&test_request, &test_response);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Test API Manager WebSocket functionality
void test_api_mgr_websocket_broadcast_returns_success(void) {
    api_manager_init(&test_config);
    
    const char *message = "{\"type\":\"test\",\"data\":\"hello\"}";
    hal_status_t status = api_manager_broadcast_websocket_message(message, strlen(message));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_api_mgr_websocket_send_message_returns_success(void) {
    api_manager_init(&test_config);
    
    const char *message = "{\"type\":\"test\",\"data\":\"hello\"}";
    hal_status_t status = api_manager_send_websocket_message(1, message, strlen(message));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_api_mgr_websocket_disconnect_returns_success(void) {
    api_manager_init(&test_config);
    
    hal_status_t status = api_manager_disconnect_websocket_client(1);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

// Test API Manager endpoint registration
void test_api_manager_register_endpoint_returns_success(void) {
    api_manager_init(&test_config);
    
    api_mgr_endpoint_t endpoint = {
        .method = API_MGR_HTTP_GET,
        .authentication_required = false,
        .enabled = true
    };
    strcpy(endpoint.path, "/api/v1/test");
    
    hal_status_t status = api_manager_register_endpoint(&endpoint);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_api_manager_register_endpoint_invalid_method_returns_error(void) {
    api_manager_init(&test_config);
    
    api_mgr_endpoint_t endpoint = {
        .method = 999,  // Invalid method
        .authentication_required = false,
        .enabled = true
    };
    strcpy(endpoint.path, "/api/v1/test");
    
    hal_status_t status = api_manager_register_endpoint(&endpoint);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_api_manager_register_endpoint_null_path_returns_error(void) {
    api_manager_init(&test_config);
    
    api_mgr_endpoint_t endpoint = {
        .method = API_MGR_HTTP_GET,
        .authentication_required = false,
        .enabled = true
    };
    // Empty path
    
    hal_status_t status = api_manager_register_endpoint(&endpoint);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

// Test API Manager error handling
void test_api_mgr_invalid_port_returns_error(void) {
    api_mgr_config_t invalid_config = test_config;
    invalid_config.http_port = 0;  // Invalid port
    
    hal_status_t status = api_manager_init(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_api_mgr_operations_after_deinit_return_error(void) {
    api_manager_init(&test_config);
    api_manager_deinit();
    
    api_mgr_status_t status;
    hal_status_t result = api_manager_get_status(&status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, result);
}

// Test API Manager performance
void test_api_mgr_request_processing_performance(void) {
    api_manager_init(&test_config);
    
    uint64_t start_time = hal_get_timestamp_us();
    
    for (int i = 0; i < 100; i++) {
        api_manager_process_http_request(&test_request, &test_response);
    }
    
    uint64_t end_time = hal_get_timestamp_us();
    uint64_t duration = end_time - start_time;
    
    // Should process 100 requests in reasonable time (< 10ms)
    TEST_ASSERT_LESS_THAN(10000, duration);
}

// Test API Manager edge cases
void test_api_mgr_multiple_initializations_handled_correctly(void) {
    hal_status_t status1 = api_manager_init(&test_config);
    hal_status_t status2 = api_manager_init(&test_config);
    hal_status_t status3 = api_manager_deinit();
    hal_status_t status4 = api_manager_deinit();
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    TEST_ASSERT_EQUAL(HAL_STATUS_ALREADY_INITIALIZED, status2);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status3);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status4);
}

void test_api_mgr_large_request_handling(void) {
    api_manager_init(&test_config);
    
    // Create a large request
    api_mgr_http_request_t large_request = test_request;
    large_request.body_length = test_config.max_request_size - 1;
    large_request.body = malloc(large_request.body_length);
    memset(large_request.body, 'A', large_request.body_length);
    
    hal_status_t status = api_manager_process_http_request(&large_request, &test_response);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    free(large_request.body);
}

// Main test runner
int main(void) {
    UNITY_BEGIN();
    
    // API Manager initialization tests
    RUN_TEST(test_api_manager_init_returns_success);
    RUN_TEST(test_api_manager_init_null_config_returns_error);
    RUN_TEST(test_api_manager_init_already_initialized_returns_already_initialized);
    
    // API Manager configuration tests
    RUN_TEST(test_api_manager_get_config_returns_valid_config);
    RUN_TEST(test_api_manager_get_config_null_config_returns_error);
    RUN_TEST(test_api_manager_get_config_not_initialized_returns_error);
    
    // API Manager status tests
    RUN_TEST(test_api_manager_get_status_returns_valid_status);
    RUN_TEST(test_api_manager_get_status_null_status_returns_error);
    RUN_TEST(test_api_manager_get_status_not_initialized_returns_error);
    
    // API Manager statistics tests
    RUN_TEST(test_api_manager_get_statistics_returns_valid_stats);
    RUN_TEST(test_api_manager_get_statistics_null_stats_returns_error);
    RUN_TEST(test_api_manager_get_statistics_not_initialized_returns_error);
    
    // API Manager request handling tests
    RUN_TEST(test_api_manager_process_http_request_returns_success);
    RUN_TEST(test_api_manager_process_http_request_null_request_returns_error);
    RUN_TEST(test_api_manager_process_http_request_null_response_returns_error);
    RUN_TEST(test_api_manager_process_http_request_not_initialized_returns_error);
    
    // API Manager WebSocket tests
    RUN_TEST(test_api_mgr_websocket_broadcast_returns_success);
    RUN_TEST(test_api_mgr_websocket_send_message_returns_success);
    RUN_TEST(test_api_mgr_websocket_disconnect_returns_success);
    
    // API Manager endpoint registration tests
    RUN_TEST(test_api_manager_register_endpoint_returns_success);
    RUN_TEST(test_api_manager_register_endpoint_invalid_method_returns_error);
    RUN_TEST(test_api_manager_register_endpoint_null_path_returns_error);
    
    // API Manager error handling tests
    RUN_TEST(test_api_mgr_invalid_port_returns_error);
    RUN_TEST(test_api_mgr_operations_after_deinit_return_error);
    
    // API Manager performance tests
    RUN_TEST(test_api_mgr_request_processing_performance);
    
    // API Manager edge case tests
    RUN_TEST(test_api_mgr_multiple_initializations_handled_correctly);
    RUN_TEST(test_api_mgr_large_request_handling);
    
    UNITY_END();
    return 0;
}
