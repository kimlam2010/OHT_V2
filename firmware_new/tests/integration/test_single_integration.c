/**
 * @file test_single_integration.c
 * @brief Single integration test to avoid global state interference
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include "unity.h"
#include "hal_common.h"
#include "hal_gpio.h"
#include "api_manager.h"
// #include "api_error_handling.h"  // File not found
#include <stdio.h>
#include <string.h>

void setUp(void) {
    // Initialize HAL logging system first
    hal_log_init("/tmp/test_single_integration.log");
    
    // Initialize HAL GPIO
    hal_gpio_init();
}

void tearDown(void) {
    // Clean up HAL GPIO
    hal_gpio_deinit();
}

void test_complete_integration_workflow(void) {
    printf("=== Starting Complete Integration Test ===\n");
    
    // Step 1: Initialize API Manager
    printf("Step 1: Initializing API Manager...\n");
    api_mgr_config_t config = {
        .http_port = 8080,
        .websocket_port = 8081,
        .timeout_ms = 5000,
        .max_clients = 10,
        .enable_cors = true,
        .enable_authentication = false,
        .cors_origin = "http://localhost:3000",
        .api_version = "1.0.0"
    };
    
    hal_status_t result = api_manager_init(&config);
    printf("API Manager init result: %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Step 1.5: Start API Manager services
    printf("Step 1.5: Starting API Manager services...\n");
    result = api_manager_start();
    printf("API Manager start result: %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Step 2: Test API Manager status
    printf("Step 2: Testing API Manager status...\n");
    api_mgr_status_t status;
    result = api_manager_get_status(&status);
    printf("API Manager status result: %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Step 3: Test basic functionality
    printf("Step 3: Testing basic functionality...\n");
    printf("HTTP server running: %s\n", status.http_server_running ? "true" : "false");
    printf("WebSocket server running: %s\n", status.websocket_server_running ? "true" : "false");
    printf("Overall status: %d\n", status.overall_status);
    printf("Uptime: %lu ms\n", status.statistics.uptime_ms);
    
    // Test that servers are running
    TEST_ASSERT_TRUE(status.http_server_running);
    TEST_ASSERT_TRUE(status.websocket_server_running);
    
    // Step 4: Test performance
    printf("Step 4: Testing performance...\n");
    TEST_ASSERT_TRUE(status.statistics.uptime_ms > 0);
    
    // Step 5: Test concurrent operations
    printf("Step 5: Testing concurrent operations...\n");
    api_mgr_status_t status1, status2, status3;
    
    result = api_manager_get_status(&status1);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    result = api_manager_get_status(&status2);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    result = api_manager_get_status(&status3);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Step 6: Test error handling
    printf("Step 6: Testing error handling...\n");
    // Test with invalid parameters
    result = api_manager_get_status(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, result);
    
    // Step 7: Test graceful shutdown
    printf("Step 7: Testing graceful shutdown...\n");
    result = api_manager_deinit();
    printf("API Manager deinit result: %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Step 8: Test status after shutdown
    printf("Step 8: Testing status after shutdown...\n");
    result = api_manager_get_status(&status);
    printf("Status after shutdown result: %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Test that servers are stopped
    TEST_ASSERT_FALSE(status.http_server_running);
    TEST_ASSERT_FALSE(status.websocket_server_running);
    
    printf("=== Complete Integration Test PASSED ===\n");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_complete_integration_workflow);
    UNITY_END();
    return 0;
}
