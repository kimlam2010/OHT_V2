#include "unity.h"
#include "module_manager.h"
#include "hal_common.h"
#include "safety_types.h"
#include <string.h>

void setUp(void) {
    // Setup test environment
}

void tearDown(void) {
    module_manager_deinit();
}

// Test module discovery initialization
void test_module_discovery_initialization_works_correctly(void) {
    // Initialize module manager
    hal_status_t mm_result = module_manager_init();
    TEST_ASSERT_EQUAL(0, mm_result);
    
    // Start module manager
    hal_status_t start_result = module_manager_start();
    TEST_ASSERT_EQUAL(0, start_result);
}

// Test module discovery
void test_module_discovery_works_correctly(void) {
    module_manager_init();
    module_manager_start();
    
    // Discover all modules
    hal_status_t result = module_manager_discover_modules();
    TEST_ASSERT_EQUAL(0, result);
}

// Test module health check
void test_module_health_check_works_correctly(void) {
    module_manager_init();
    module_manager_start();
    
    // Health check all modules
    hal_status_t result = module_manager_health_check_all();
    TEST_ASSERT_EQUAL(0, result);
}

// Test module statistics
void test_module_statistics_works_correctly(void) {
    module_manager_init();
    module_manager_start();
    
    // Get statistics
    module_stats_t stats;
    hal_status_t result = module_manager_get_statistics(&stats);
    TEST_ASSERT_EQUAL(0, result);
}

// Test module manager self test
void test_module_manager_self_test_works_correctly(void) {
    module_manager_init();
    
    // Run self test
    hal_status_t result = module_manager_self_test();
    TEST_ASSERT_EQUAL(0, result);
}

// Test utility functions
void test_module_utility_functions_work_correctly(void) {
    // Test type name function
    const char* power_name = module_manager_get_type_name(MODULE_TYPE_POWER);
    TEST_ASSERT_NOT_NULL(power_name);
    
    // Test status name function
    const char* online_name = module_manager_get_status_name(MODULE_STATUS_ONLINE);
    TEST_ASSERT_NOT_NULL(online_name);
    
    // Test health name function
    const char* good_name = module_manager_get_health_name(MODULE_HEALTH_GOOD);
    TEST_ASSERT_NOT_NULL(good_name);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_module_discovery_initialization_works_correctly);
    RUN_TEST(test_module_discovery_works_correctly);
    RUN_TEST(test_module_health_check_works_correctly);
    RUN_TEST(test_module_statistics_works_correctly);
    RUN_TEST(test_module_manager_self_test_works_correctly);
    RUN_TEST(test_module_utility_functions_work_correctly);
    
    return UNITY_END();
}
