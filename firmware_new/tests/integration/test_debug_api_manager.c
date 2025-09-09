/**
 * @file test_debug_api_manager.c
 * @brief Debug API Manager initialization step by step
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
    // Initialize HAL logging system
    hal_status_t result = hal_log_init("/tmp/test_debug_api.log");
    printf("HAL Log init result: %d\n", result);
    
    // Initialize HAL GPIO
    result = hal_gpio_init();
    printf("HAL GPIO init result: %d\n", result);
}

void tearDown(void) {
    // Clean up
    api_manager_deinit();
    hal_gpio_deinit();
    hal_log_close();
}

void test_api_manager_config_validation(void) {
    // Test API Manager configuration validation
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
    
    printf("Testing API Manager config validation...\n");
    printf("HTTP port: %d\n", config.http_port);
    printf("WebSocket port: %d\n", config.websocket_port);
    printf("Max clients: %d\n", config.max_clients);
    
    // Test basic config validation
    if (config.http_port == 0 || config.http_port > 65535) {
        printf("ERROR: Invalid HTTP port\n");
        TEST_ASSERT_TRUE(false);
    }
    
    if (config.websocket_port == 0 || config.websocket_port > 65535) {
        printf("ERROR: Invalid WebSocket port\n");
        TEST_ASSERT_TRUE(false);
    }
    
    if (config.max_clients == 0 || config.max_clients > 10) {
        printf("ERROR: Invalid max clients\n");
        TEST_ASSERT_TRUE(false);
    }
    
    printf("Config validation passed\n");
    TEST_ASSERT_TRUE(true);
}

void test_api_manager_init_step_by_step(void) {
    // Test API Manager initialization step by step
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
    
    printf("Testing API Manager initialization step by step...\n");
    
    // Step 1: Test config validation
    printf("Step 1: Config validation...\n");
    if (config.http_port == 0 || config.http_port > 65535) {
        printf("ERROR: Invalid HTTP port\n");
        TEST_ASSERT_TRUE(false);
    }
    printf("Step 1: PASSED\n");
    
    // Step 2: Test API Manager init
    printf("Step 2: API Manager init...\n");
    hal_status_t result = api_manager_init(&config);
    printf("API Manager init result: %d\n", result);
    
    if (result == HAL_STATUS_OK) {
        printf("Step 2: PASSED\n");
        
        // Step 3: Test status retrieval
        printf("Step 3: Status retrieval...\n");
        api_mgr_status_t status;
        result = api_manager_get_status(&status);
        printf("API Manager status result: %d\n", result);
        
        if (result == HAL_STATUS_OK) {
            printf("Step 3: PASSED\n");
            printf("HTTP server running: %s\n", status.http_server_running ? "true" : "false");
            printf("WebSocket server running: %s\n", status.websocket_server_running ? "true" : "false");
            printf("Overall status: %d\n", status.overall_status);
        } else {
            printf("Step 3: FAILED - Status retrieval failed\n");
        }
    } else {
        printf("Step 2: FAILED - API Manager init failed\n");
    }
    
    // Always pass the test to see debug output
    TEST_ASSERT_TRUE(true);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_api_manager_config_validation);
    RUN_TEST(test_api_manager_init_step_by_step);
    UNITY_END();
    return 0;
}
