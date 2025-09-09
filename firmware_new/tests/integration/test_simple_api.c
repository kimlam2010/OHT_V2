/**
 * @file test_simple_api.c
 * @brief Simple API test for debugging
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

void setUp(void) {
    // Initialize HAL
    hal_gpio_init();
}

void tearDown(void) {
    // Clean up
    api_manager_deinit();
    hal_gpio_deinit();
}

void test_api_manager_basic_init(void) {
    // Test basic API Manager initialization
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
    
    if (result == HAL_STATUS_OK) {
        // Test status
        api_mgr_status_t status;
        result = api_manager_get_status(&status);
        printf("API Manager status result: %d\n", result);
        printf("HTTP server running: %s\n", status.http_server_running ? "true" : "false");
        printf("WebSocket server running: %s\n", status.websocket_server_running ? "true" : "false");
        
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    } else {
        printf("API Manager init failed with error: %d\n", result);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_api_manager_basic_init);
    UNITY_END();
    return 0;
}
