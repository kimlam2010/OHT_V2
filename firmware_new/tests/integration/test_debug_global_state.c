/**
 * @file test_debug_global_state.c
 * @brief Debug global API Manager state
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
    hal_log_init("/tmp/test_debug_global_state.log");
    
    // Initialize HAL GPIO
    hal_gpio_init();
}

void tearDown(void) {
    // Clean up
    hal_gpio_deinit();
    hal_log_close();
}

void test_global_state_before_init(void) {
    // Test global state before initialization
    printf("Testing global state before initialization...\n");
    
    // Try to get status before init
    api_mgr_status_t status;
    hal_status_t result = api_manager_get_status(&status);
    printf("Status before init result: %d\n", result);
    
    // Should return NOT_INITIALIZED
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, result);
}

void test_global_state_after_init(void) {
    // Test global state after initialization
    printf("Testing global state after initialization...\n");
    
    // Initialize API Manager
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
    
    hal_status_t init_result = api_manager_init(&config);
    printf("API Manager init result: %d\n", init_result);
    
    if (init_result == HAL_STATUS_OK) {
        // Try to get status after init
        api_mgr_status_t status;
        hal_status_t result = api_manager_get_status(&status);
        printf("Status after init result: %d\n", result);
        
        if (result == HAL_STATUS_OK) {
            printf("HTTP server running: %s\n", status.http_server_running ? "true" : "false");
            printf("WebSocket server running: %s\n", status.websocket_server_running ? "true" : "false");
            printf("Overall status: %d\n", status.overall_status);
            printf("Uptime: %lu ms\n", status.statistics.uptime_ms);
        } else {
            printf("ERROR: Status retrieval failed after successful init\n");
        }
        
        // Clean up
        api_manager_deinit();
    } else {
        printf("ERROR: API Manager init failed\n");
    }
    
    // Always pass to see debug output
    TEST_ASSERT_TRUE(true);
}

void test_global_state_after_deinit(void) {
    // Test global state after deinitialization
    printf("Testing global state after deinitialization...\n");
    
    // Initialize API Manager
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
    
    hal_status_t init_result = api_manager_init(&config);
    printf("API Manager init result: %d\n", init_result);
    
    if (init_result == HAL_STATUS_OK) {
        // Deinitialize
        hal_status_t deinit_result = api_manager_deinit();
        printf("API Manager deinit result: %d\n", deinit_result);
        
        // Try to get status after deinit
        api_mgr_status_t status;
        hal_status_t result = api_manager_get_status(&status);
        printf("Status after deinit result: %d\n", result);
        
        // Should return NOT_INITIALIZED
        TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, result);
    } else {
        printf("ERROR: API Manager init failed\n");
        TEST_ASSERT_TRUE(false);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_global_state_before_init);
    RUN_TEST(test_global_state_after_init);
    RUN_TEST(test_global_state_after_deinit);
    UNITY_END();
    return 0;
}
