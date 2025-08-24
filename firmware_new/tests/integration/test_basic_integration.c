/**
 * @file test_basic_integration.c
 * @brief Basic integration tests for OHT-50 Master Module System
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "unity.h"
#include "hal_common.h"
#include "hal_gpio.h"
#include "api_manager.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Test fixtures
static api_mgr_config_t api_config;

void setUp(void) {
    // Initialize API configuration
    api_config.http_port = 8080;
    api_config.websocket_port = 8081;
    api_config.http_enabled = true;
    api_config.websocket_enabled = true;
    api_config.cors_enabled = true;
    strcpy(api_config.cors_origin, "http://localhost:3000");
    api_config.max_request_size = 4096;
    api_config.max_response_size = 8192;
    api_config.request_timeout_ms = 5000;
    api_config.websocket_timeout_ms = 30000;
    api_config.authentication_required = false;
    api_config.ssl_enabled = false;
}

void tearDown(void) {
    // Clean up
    api_manager_deinit();
    hal_gpio_deinit();
}

// Test HAL and API integration
void test_hal_and_api_integration_works_correctly(void) {
    // Initialize HAL layer
    hal_status_t hal_status = hal_gpio_init();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, hal_status);
    
    // Initialize API manager
    hal_status_t api_status = api_manager_init(&api_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, api_status);
    
    // Verify both are initialized
    api_mgr_status_t api_mgr_status;
    api_status = api_manager_get_status(&api_mgr_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, api_status);
    TEST_ASSERT_TRUE(api_mgr_status.initialized);
    
    // Test GPIO operations
    gpio_config_t gpio_config = {
        .pin_number = 1,
        .direction = GPIO_DIRECTION_OUT,
        .bias = GPIO_BIAS_DISABLE,
        .drive = GPIO_DRIVE_2MA,
        .edge = GPIO_EDGE_NONE
    };
    
    hal_status = hal_gpio_configure_pin(&gpio_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, hal_status);
    
    // Test API operations
    api_mgr_http_request_t request = {0};
    api_mgr_http_response_t response = {0};
    
    request.method = API_MGR_HTTP_GET;
    strcpy(request.url, "/api/v1/system/status");
    strcpy(request.path, "/api/v1/system/status");
    request.content_type = API_MGR_CONTENT_TYPE_JSON;
    strcpy(request.client_ip, "127.0.0.1");
    
    api_status = api_manager_process_http_request(&request, &response);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, api_status);
}

// Test system performance integration
void test_system_performance_integration(void) {
    // Initialize system
    hal_gpio_init();
    api_manager_init(&api_config);
    
    // Test combined performance
    uint64_t start_time = hal_get_timestamp_us();
    
    // Perform GPIO operations
    gpio_config_t gpio_config = {
        .pin_number = 1,
        .direction = GPIO_DIRECTION_OUT,
        .bias = GPIO_BIAS_DISABLE,
        .drive = GPIO_DRIVE_2MA,
        .edge = GPIO_EDGE_NONE
    };
    
    hal_gpio_configure_pin(&gpio_config);
    
    // Perform API operations
    for (int i = 0; i < 10; i++) {
        api_mgr_http_request_t request = {0};
        api_mgr_http_response_t response = {0};
        
        request.method = API_MGR_HTTP_GET;
        snprintf(request.url, sizeof(request.url), "/api/v1/system/status?test=%d", i);
        snprintf(request.path, sizeof(request.path), "/api/v1/system/status");
        request.content_type = API_MGR_CONTENT_TYPE_JSON;
        strcpy(request.client_ip, "127.0.0.1");
        
        hal_status_t status = api_manager_process_http_request(&request, &response);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    }
    
    uint64_t end_time = hal_get_timestamp_us();
    uint64_t duration = end_time - start_time;
    
    // Should complete in reasonable time (< 10ms)
    TEST_ASSERT_LESS_THAN(10000, duration);
    
    // Verify system is still responsive
    api_mgr_status_t api_status;
    hal_status_t status = api_manager_get_status(&api_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_TRUE(api_status.initialized);
}

// Test error handling integration
void test_error_handling_integration(void) {
    // Initialize system
    hal_gpio_init();
    api_manager_init(&api_config);
    
    // Test GPIO error handling
    gpio_config_t invalid_config = {
        .pin_number = 999,  // Invalid pin
        .direction = GPIO_DIRECTION_OUT,
        .bias = GPIO_BIAS_DISABLE,
        .drive = GPIO_DRIVE_2MA,
        .edge = GPIO_EDGE_NONE
    };
    
    hal_status_t status = hal_gpio_configure_pin(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    // Test API error handling
    api_mgr_http_request_t invalid_request = {0};
    api_mgr_http_response_t response = {0};
    
    status = api_manager_process_http_request(&invalid_request, &response);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    // Verify system is still functional
    api_mgr_status_t api_status;
    status = api_manager_get_status(&api_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_TRUE(api_status.initialized);
}

// Test system shutdown integration
void test_system_shutdown_integration(void) {
    // Initialize system
    hal_gpio_init();
    api_manager_init(&api_config);
    
    // Verify both are initialized
    api_mgr_status_t api_status;
    hal_status_t status = api_manager_get_status(&api_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_TRUE(api_status.initialized);
    
    // Shutdown system
    status = api_manager_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_gpio_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify both are deinitialized
    status = api_manager_get_status(&api_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Test memory and resource management
void test_memory_and_resource_management(void) {
    // Initialize system multiple times
    for (int i = 0; i < 3; i++) {
        hal_gpio_init();
        api_manager_init(&api_config);
        
        // Verify initialization
        api_mgr_status_t api_status;
        hal_status_t status = api_manager_get_status(&api_status);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
        TEST_ASSERT_TRUE(api_status.initialized);
        
        // Cleanup
        api_manager_deinit();
        hal_gpio_deinit();
        
        // Verify cleanup
        status = api_manager_get_status(&api_status);
        TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
    }
}

// Test concurrent operations
void test_concurrent_operations(void) {
    // Initialize system
    hal_gpio_init();
    api_manager_init(&api_config);
    
    // Test concurrent GPIO and API operations
    uint64_t start_time = hal_get_timestamp_us();
    
    // Simulate concurrent operations
    for (int i = 0; i < 5; i++) {
        // GPIO operation
        gpio_config_t gpio_config = {
            .pin_number = i + 1,
            .direction = GPIO_DIRECTION_OUT,
            .bias = GPIO_BIAS_DISABLE,
            .drive = GPIO_DRIVE_2MA,
            .edge = GPIO_EDGE_NONE
        };
        
        hal_gpio_configure_pin(&gpio_config);
        
        // API operation
        api_mgr_http_request_t request = {0};
        api_mgr_http_response_t response = {0};
        
        request.method = API_MGR_HTTP_GET;
        snprintf(request.url, sizeof(request.url), "/api/v1/system/status?pin=%d", i);
        snprintf(request.path, sizeof(request.path), "/api/v1/system/status");
        request.content_type = API_MGR_CONTENT_TYPE_JSON;
        strcpy(request.client_ip, "127.0.0.1");
        
        hal_status_t status = api_manager_process_http_request(&request, &response);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    }
    
    uint64_t end_time = hal_get_timestamp_us();
    uint64_t duration = end_time - start_time;
    
    // Should complete in reasonable time
    TEST_ASSERT_LESS_THAN(5000, duration);
    
    // Verify system integrity
    api_mgr_status_t api_status;
    hal_status_t status = api_manager_get_status(&api_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_TRUE(api_status.initialized);
}

// Main test runner
int main(void) {
    UNITY_BEGIN();
    
    // Basic integration tests
    RUN_TEST(test_hal_and_api_integration_works_correctly);
    RUN_TEST(test_system_performance_integration);
    RUN_TEST(test_error_handling_integration);
    RUN_TEST(test_system_shutdown_integration);
    RUN_TEST(test_memory_and_resource_management);
    RUN_TEST(test_concurrent_operations);
    
    UNITY_END();
    return 0;
}
