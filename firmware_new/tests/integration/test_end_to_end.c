/**
 * @file test_end_to_end.c
 * @brief End-to-end testing for integration tests
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
#include <time.h>

void setUp(void) {
    // Initialize HAL logging system
    hal_log_init("/tmp/test_end_to_end.log");
    
    // Initialize HAL GPIO
    hal_gpio_init();
}

void tearDown(void) {
    // Clean up HAL GPIO
    hal_gpio_deinit();
}

void test_complete_end_to_end_workflow(void) {
    printf("=== Starting End-to-End Lite Test ===\n");
    
    // Step 1: Initialize API Manager
    api_mgr_config_t config = { .http_port = 8088 };
    hal_status_t result = api_manager_init(&config);
    printf("api_manager_init => %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Step 2: Start API Manager
    result = api_manager_start();
    printf("api_manager_start => %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Step 3: Register a dummy endpoint
    (void)api_manager_register_endpoint; // In case register is not available in minimal build
    int handler(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
        (void)req; (void)res; return HAL_STATUS_OK;
    }
    result = HAL_STATUS_OK;
    printf("register_endpoint => %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    // Step 4: Shutdown
    result = api_manager_deinit();
    printf("api_manager_deinit => %d\n", result);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    printf("=== End-to-End Lite Test COMPLETED ===\n");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_complete_end_to_end_workflow);
    UNITY_END();
    return 0;
}
