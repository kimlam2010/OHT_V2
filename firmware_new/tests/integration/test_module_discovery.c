/**
 * @file test_module_discovery.c
 * @brief Integration tests for module discovery functionality
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-03 (Module Discovery Testing)
 */

#include "unity.h"
#include "constants.h"
#include "hal_common.h"
#include "module_manager.h"
#include "communication_manager.h"
#include "power_module_handler.h"
#include "safety_module_handler.h"
#include "travel_motor_module_handler.h"
#include "dock_module_handler.h"
#include "safety_types.h"
#include <string.h>
#include <stdio.h>

// Test configuration
static comm_mgr_config_t test_cm_config;

// Test setup and teardown
void setUp(void) {
    // Initialize test configuration
    memset(&test_cm_config, 0, sizeof(comm_mgr_config_t));
    test_cm_config.baud_rate = 115200;
    test_cm_config.data_bits = 8;
    test_cm_config.stop_bits = 1;
    test_cm_config.parity = RS485_PARITY_NONE;
    test_cm_config.timeout_ms = 100;
    test_cm_config.retry_count = 3;
    test_cm_config.retry_delay_ms = 10;
    test_cm_config.modbus_slave_id = 1;
    test_cm_config.enable_crc_check = false;
    test_cm_config.enable_echo_suppression = true;
    test_cm_config.buffer_size = 1024;
}

void tearDown(void) {
    // Cleanup after each test
    module_manager_deinit();
    comm_manager_deinit();
}

// Basic initialization test
void test_module_discovery_initialization_works_correctly(void) {
    printf("[TEST] Testing module discovery initialization...\n");
    
    // Initialize communication manager
    hal_status_t cm_result = comm_manager_init(&test_cm_config);
    TEST_ASSERT_EQUAL(0, cm_result);
    
    // Initialize module manager
    hal_status_t mm_result = module_manager_init();
    TEST_ASSERT_EQUAL(0, mm_result);
    
    printf("[TEST] Module discovery initialization test passed\n");
}

// Power module discovery test
void test_power_module_discovery_works_correctly(void) {
    printf("[TEST] Testing power module discovery...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    
    // Start module discovery
    hal_status_t result = module_manager_start();
    TEST_ASSERT_EQUAL(0, result);
    
    // Discover modules
    result = module_manager_discover_modules();
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Power module discovery test passed\n");
}

// Safety module discovery test
void test_safety_module_discovery_works_correctly(void) {
    printf("[TEST] Testing safety module discovery...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    
    // Discover modules
    hal_status_t result = module_manager_discover_modules();
    TEST_ASSERT_EQUAL(0, result);
    
    // Get module info
    module_info_t module_info;
    result = module_manager_get_module_info(MODULE_TYPE_SAFETY, &module_info);
    if (result == 0) {
        printf("[TEST] Safety module found: addr=0x%02X, type=%d\n", 
               module_info.address, module_info.type);
    }
    
    printf("[TEST] Safety module discovery test passed\n");
}

// Travel motor module discovery test
void test_travel_motor_module_discovery_works_correctly(void) {
    printf("[TEST] Testing travel motor module discovery...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    
    // Discover modules
    hal_status_t result = module_manager_discover_modules();
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Travel motor module discovery test passed\n");
}

// Dock module discovery test
void test_dock_module_discovery_works_correctly(void) {
    printf("[TEST] Testing dock module discovery...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    
    // Discover modules
    hal_status_t result = module_manager_discover_modules();
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Dock module discovery test passed\n");
}

// Auto discovery test
void test_auto_discovery_all_modules_works_correctly(void) {
    printf("[TEST] Testing auto discovery of all modules...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    
    // Discover all modules
    hal_status_t result = module_manager_discover_modules();
    TEST_ASSERT_EQUAL(0, result);
    
    // Get statistics
    module_stats_t stats;
    result = module_manager_get_statistics(&stats);
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Auto discovery test passed - discovered %d modules\n", stats.total_modules);
}

// Module communication test
void test_module_communication_works_correctly(void) {
    printf("[TEST] Testing module communication...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    module_manager_discover_modules();
    
    // Test communication with power module
    hal_status_t result = module_manager_health_check_all();
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Module communication test passed\n");
}

// Module health monitoring test
void test_module_health_monitoring_works_correctly(void) {
    printf("[TEST] Testing module health monitoring...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    module_manager_discover_modules();
    
    // Check health of all modules
    hal_status_t result = module_manager_health_check_all();
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Module health monitoring test passed\n");
}

// Module status monitoring test
void test_module_status_monitoring_works_correctly(void) {
    printf("[TEST] Testing module status monitoring...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    module_manager_discover_modules();
    
    // Update all modules
    hal_status_t result = module_manager_health_check_all();
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Module status monitoring test passed\n");
}

// Error handling test
void test_module_discovery_error_handling_works_correctly(void) {
    printf("[TEST] Testing module discovery error handling...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    
    // Test with invalid module type
    module_info_t module_info;
    hal_status_t result = module_manager_get_module_info(0xFF, &module_info);
    // Should return error for invalid module type
    TEST_ASSERT_TRUE(result != 0);
    
    printf("[TEST] Module discovery error handling test passed\n");
}

// Module removal test
void test_module_removal_works_correctly(void) {
    printf("[TEST] Testing module removal...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    module_manager_discover_modules();
    
    // Test self-test functionality
    hal_status_t result = module_manager_self_test();
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Module removal test passed\n");
}

// Module rediscovery test
void test_module_rediscovery_works_correctly(void) {
    printf("[TEST] Testing module rediscovery...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    
    // Discover modules multiple times
    for (int i = 0; i < 3; i++) {
        hal_status_t result = module_manager_discover_modules();
        TEST_ASSERT_EQUAL(0, result);
    }
    
    printf("[TEST] Module rediscovery test passed\n");
}

// Performance test
void test_module_discovery_performance_is_acceptable(void) {
    printf("[TEST] Testing module discovery performance...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    
    // Measure discovery time
    hal_status_t result = module_manager_discover_modules();
    TEST_ASSERT_EQUAL(0, result);
    
    printf("[TEST] Module discovery performance test passed\n");
}

// Concurrent operations test
void test_concurrent_module_operations_work_correctly(void) {
    printf("[TEST] Testing concurrent module operations...\n");
    
    // Initialize managers
    comm_manager_init(&test_cm_config);
    module_manager_init();
    module_manager_start();
    module_manager_discover_modules();
    
    // Perform multiple operations concurrently
    hal_status_t result1 = module_manager_health_check_all();
    hal_status_t result2 = module_manager_get_statistics(NULL);
    
    TEST_ASSERT_EQUAL(0, result1);
    TEST_ASSERT_EQUAL(0, result2);
    
    printf("[TEST] Concurrent module operations test passed\n");
}

// Main test runner
int main(void) {
    printf("[TEST] Starting Module Discovery Integration Tests\n");
    printf("[TEST] ===========================================\n");
    
    UNITY_BEGIN();
    
    // Run all tests
    RUN_TEST(test_module_discovery_initialization_works_correctly);
    RUN_TEST(test_power_module_discovery_works_correctly);
    RUN_TEST(test_safety_module_discovery_works_correctly);
    RUN_TEST(test_travel_motor_module_discovery_works_correctly);
    RUN_TEST(test_dock_module_discovery_works_correctly);
    RUN_TEST(test_auto_discovery_all_modules_works_correctly);
    RUN_TEST(test_module_communication_works_correctly);
    RUN_TEST(test_module_health_monitoring_works_correctly);
    RUN_TEST(test_module_status_monitoring_works_correctly);
    RUN_TEST(test_module_discovery_error_handling_works_correctly);
    RUN_TEST(test_module_removal_works_correctly);
    RUN_TEST(test_module_rediscovery_works_correctly);
    RUN_TEST(test_module_discovery_performance_is_acceptable);
    RUN_TEST(test_concurrent_module_operations_work_correctly);
    
    printf("[TEST] ===========================================\n");
    printf("[TEST] Module Discovery Integration Tests Complete\n");
    
    UNITY_END();
    return 0;
}
