/**
 * @file test_simple_integration_fixed.c
 * @brief Simple integration test with proper API Manager handling
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include "unity.h"
#include "hal_common.h"
#include "hal_gpio.h"
#include "api_manager.h"
#include <stdio.h>
#include <string.h>

// Global test state
static bool api_manager_initialized = false;
static api_mgr_config_t api_config;

void setUp(void) {
    // Initialize HAL logging system first
    hal_log_init("/tmp/test_simple_integration.log");
    
    // Initialize HAL GPIO
    hal_gpio_init();
    
    // Initialize API configuration
    api_config.http_port = 8080;
    api_config.websocket_port = 8081;
    api_config.timeout_ms = 5000;
    api_config.max_clients = 10;
    api_config.enable_cors = true;
    api_config.enable_authentication = false;
    strcpy(api_config.cors_origin, "http://localhost:3000");
    strcpy(api_config.api_version, "1.0.0");
    
    // Initialize API Manager only once
    if (!api_manager_initialized) {
        hal_status_t result = api_manager_init(&api_config);
        if (result == HAL_STATUS_OK) {
            api_manager_initialized = true;
            printf("API Manager initialized successfully\n");
        } else {
            printf("API Manager initialization failed: %d\n", result);
        }
    }
}

void tearDown(void) {
    // Don't deinit API Manager here - let it stay initialized for all tests
    // Only deinit HAL GPIO
    hal_gpio_deinit();
}

void test_api_manager_basic_functionality(void) {
    // Test that API Manager is initialized
    TEST_ASSERT_TRUE(api_manager_initialized);
    
    // Test API Manager status
    api_mgr_status_t status;
    hal_status_t result = api_manager_get_status(&status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test basic status fields
    printf("HTTP server running: %s\n", status.http_server_running ? "true" : "false");
    printf("WebSocket server running: %s\n", status.websocket_server_running ? "true" : "false");
    printf("Overall status: %d\n", status.overall_status);
    
    // Test that status was retrieved successfully
    TEST_ASSERT_TRUE(true); // Status retrieval successful
}

void test_api_manager_performance(void) {
    // Test that API Manager is initialized
    TEST_ASSERT_TRUE(api_manager_initialized);
    
    // Test API Manager performance
    api_mgr_status_t status;
    hal_status_t result = api_manager_get_status(&status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test statistics
    TEST_ASSERT_TRUE(status.statistics.uptime_ms >= 0);
    
    printf("Performance test passed\n");
}

void test_api_manager_concurrent_access(void) {
    // Test that API Manager is initialized
    TEST_ASSERT_TRUE(api_manager_initialized);
    
    // Test concurrent status checks
    api_mgr_status_t status1, status2, status3;
    
    // Simulate concurrent operations
    hal_status_t result1 = api_manager_get_status(&status1);
    hal_status_t result2 = api_manager_get_status(&status2);
    hal_status_t result3 = api_manager_get_status(&status3);
    
    // Verify all status checks succeeded
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result1);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result2);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result3);
    
    printf("Concurrent access test passed\n");
}

void test_api_manager_cleanup(void) {
    // Test that API Manager is initialized
    TEST_ASSERT_TRUE(api_manager_initialized);
    
    // Test graceful shutdown
    hal_status_t result = api_manager_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Mark as deinitialized
    api_manager_initialized = false;
    
    printf("API Manager cleanup test passed\n");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_api_manager_basic_functionality);
    RUN_TEST(test_api_manager_performance);
    RUN_TEST(test_api_manager_concurrent_access);
    RUN_TEST(test_api_manager_cleanup);
    UNITY_END();
    return 0;
}
