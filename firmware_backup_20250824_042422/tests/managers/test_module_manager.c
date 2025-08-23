/**
 * @file test_module_manager.c
 * @brief Test program for Module Manager implementation
 * @version 2.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-07 (Module Management Testing)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "module_manager.h"
#include "hal_common.h"

// Test assertion macro
#define test_assert(condition, message) \
    do { \
        if (!(condition)) { \
            printf("❌ FAIL: %s\n", message); \
            return false; \
        } else { \
            printf("✅ PASS: %s\n", message); \
        } \
    } while(0)

// Test callback function
void test_module_callback(module_event_t event, uint8_t module_id, const void *data) {
    printf("Module event: %s, Module ID: %d\n", 
           module_manager_get_event_name(event), module_id);
    (void)data; // Suppress unused parameter warning
}

// Test functions
static bool test_module_manager_initialization(void);
static bool test_module_manager_basic_operations(void);
static bool test_module_manager_module_registration(void);
static bool test_module_manager_module_management(void);
static bool test_module_manager_health_check(void);
static bool test_module_manager_statistics(void);
static bool test_module_manager_utility_functions(void);
static bool test_module_manager_reset(void);
static bool test_module_manager_deinitialization(void);

int main(int argc, char *argv[]) {
    (void)argc; // Suppress unused parameter warning
    (void)argv; // Suppress unused parameter warning
    
    printf("=== Module Manager Test Suite ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("\n");
    
    bool all_tests_passed = true;
    
    // Run all tests
    all_tests_passed &= test_module_manager_initialization();
    all_tests_passed &= test_module_manager_basic_operations();
    all_tests_passed &= test_module_manager_module_registration();
    all_tests_passed &= test_module_manager_module_management();
    all_tests_passed &= test_module_manager_health_check();
    all_tests_passed &= test_module_manager_statistics();
    all_tests_passed &= test_module_manager_utility_functions();
    all_tests_passed &= test_module_manager_reset();
    all_tests_passed &= test_module_manager_deinitialization();
    
    // Final result
    printf("\n=== Test Results ===\n");
    if (all_tests_passed) {
        printf("�� All tests PASSED!\n");
        return 0;
    } else {
        printf("❌ Some tests FAILED!\n");
        return 1;
    }
}

static bool test_module_manager_initialization(void) {
    printf("\n=== Testing Module Manager Initialization ===\n");
    
    // Test initialization
    hal_status_t status = module_manager_init();
    test_assert(status == HAL_STATUS_OK, "Module Manager initialization");
    
    // Test double initialization
    status = module_manager_init();
    test_assert(status == HAL_STATUS_ALREADY_INITIALIZED, "Module Manager double initialization");
    
    return true;
}

static bool test_module_manager_basic_operations(void) {
    printf("\n=== Testing Module Manager Basic Operations ===\n");
    
    // Test start
    hal_status_t status = module_manager_start();
    test_assert(status == HAL_STATUS_OK, "Module Manager start");
    
    // Test stop
    status = module_manager_stop();
    test_assert(status == HAL_STATUS_OK, "Module Manager stop");
    
    return true;
}

static bool test_module_manager_module_registration(void) {
    printf("\n=== Testing Module Manager Module Registration ===\n");
    
    // Test valid module registration
    module_info_t module_info = {
        .module_id = 1,
        .address = 0x01,
        .type = MODULE_TYPE_SENSOR,
        .status = MODULE_STATUS_ONLINE
    };
    strncpy(module_info.name, "Test Sensor", sizeof(module_info.name) - 1);
    strncpy(module_info.version, "1.0.0", sizeof(module_info.version) - 1);
    
    hal_status_t status = module_manager_register_module(&module_info);
    test_assert(status == HAL_STATUS_OK, "Valid module registration");
    
    // Test duplicate registration
    status = module_manager_register_module(&module_info);
    test_assert(status == HAL_STATUS_OK, "Duplicate module registration (update)");
    
    // Test invalid module ID
    module_info.module_id = 0;
    status = module_manager_register_module(&module_info);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid module ID registration");
    
    // Test NULL module info
    status = module_manager_register_module(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "NULL module info registration");
    
    return true;
}

static bool test_module_manager_module_management(void) {
    printf("\n=== Testing Module Manager Module Management ===\n");
    
    // Register a test module
    module_info_t module_info = {
        .module_id = 2,
        .address = 0x02,
        .type = MODULE_TYPE_ACTUATOR,
        .status = MODULE_STATUS_ONLINE
    };
    strncpy(module_info.name, "Test Actuator", sizeof(module_info.name) - 1);
    
    hal_status_t status = module_manager_register_module(&module_info);
    test_assert(status == HAL_STATUS_OK, "Register test module");
    
    // Test get module info
    module_info_t retrieved_info;
    status = module_manager_get_module_info(2, &retrieved_info);
    test_assert(status == HAL_STATUS_OK, "Get module info");
    test_assert(retrieved_info.module_id == 2, "Correct module ID");
    test_assert(strcmp(retrieved_info.name, "Test Actuator") == 0, "Correct module name");
    
    // Test get module status
    module_status_info_t status_info;
    status = module_manager_get_module_status(2, &status_info);
    test_assert(status == HAL_STATUS_OK, "Get module status");
    
    // Test get registered modules
    uint8_t module_ids[10];
    uint32_t actual_count;
    status = module_manager_get_registered_modules(module_ids, 10, &actual_count);
    test_assert(status == HAL_STATUS_OK, "Get registered modules");
    test_assert(actual_count >= 1, "At least one module registered");
    
    // Test unregister module
    status = module_manager_unregister_module(2);
    test_assert(status == HAL_STATUS_OK, "Unregister module");
    
    // Test get non-existent module
    status = module_manager_get_module_info(2, &retrieved_info);
    test_assert(status == HAL_STATUS_NOT_FOUND, "Get non-existent module");
    
    return true;
}

static bool test_module_manager_health_check(void) {
    printf("\n=== Testing Module Manager Health Check ===\n");
    
    // Register a test module
    module_info_t module_info = {
        .module_id = 3,
        .address = 0x03,
        .type = MODULE_TYPE_POWER,
        .status = MODULE_STATUS_ONLINE
    };
    strncpy(module_info.name, "Test Power", sizeof(module_info.name) - 1);
    
    hal_status_t status = module_manager_register_module(&module_info);
    test_assert(status == HAL_STATUS_OK, "Register test module for health check");
    
    // Test health check on specific module
    status = module_manager_health_check_module(3);
    test_assert(status == HAL_STATUS_OK, "Health check on specific module");
    
    // Test health check on non-existent module
    status = module_manager_health_check_module(99);
    test_assert(status == HAL_STATUS_NOT_FOUND, "Health check on non-existent module");
    
    // Test health check on all modules
    status = module_manager_health_check_all();
    test_assert(status == HAL_STATUS_OK, "Health check on all modules");
    
    return true;
}

static bool test_module_manager_statistics(void) {
    printf("\n=== Testing Module Manager Statistics ===\n");
    
    // Test get statistics
    module_stats_t stats;
    hal_status_t status = module_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get Module Manager statistics");
    test_assert(stats.total_modules >= 0, "Valid total modules count");
    test_assert(stats.online_modules >= 0, "Valid online modules count");
    test_assert(stats.offline_modules >= 0, "Valid offline modules count");
    test_assert(stats.error_modules >= 0, "Valid error modules count");
    
    // Test reset statistics
    status = module_manager_reset_statistics();
    test_assert(status == HAL_STATUS_OK, "Reset Module Manager statistics");
    
    // Verify statistics were reset
    status = module_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get reset statistics");
    test_assert(stats.total_modules == 0, "Reset total modules");
    test_assert(stats.online_modules == 0, "Reset online modules");
    test_assert(stats.offline_modules == 0, "Reset offline modules");
    test_assert(stats.error_modules == 0, "Reset error modules");
    
    return true;
}

static bool test_module_manager_utility_functions(void) {
    printf("\n=== Testing Module Manager Utility Functions ===\n");
    
    // Test type name functions
    const char* type_name = module_manager_get_type_name(MODULE_TYPE_SENSOR);
    test_assert(strcmp(type_name, "Sensor") == 0, "Sensor type name");
    
    type_name = module_manager_get_type_name(MODULE_TYPE_UNKNOWN);
    test_assert(strcmp(type_name, "Unknown") == 0, "Unknown type name");
    
    // Test status name functions
    const char* status_name = module_manager_get_status_name(MODULE_STATUS_ONLINE);
    test_assert(strcmp(status_name, "Online") == 0, "Online status name");
    
    status_name = module_manager_get_status_name(MODULE_STATUS_OFFLINE);
    test_assert(strcmp(status_name, "Offline") == 0, "Offline status name");
    
    // Test health name functions
    const char* health_name = module_manager_get_health_name(MODULE_HEALTH_EXCELLENT);
    test_assert(strcmp(health_name, "Excellent") == 0, "Excellent health name");
    
    health_name = module_manager_get_health_name(MODULE_HEALTH_FAILED);
    test_assert(strcmp(health_name, "Failed") == 0, "Failed health name");
    
    // Test event name functions
    const char* event_name = module_manager_get_event_name(MODULE_EVENT_DISCOVERED);
    test_assert(strcmp(event_name, "Discovered") == 0, "Discovered event name");
    
    event_name = module_manager_get_event_name(MODULE_EVENT_ONLINE);
    test_assert(strcmp(event_name, "Online") == 0, "Online event name");
    
    return true;
}

static bool test_module_manager_reset(void) {
    printf("\n=== Testing Module Manager Reset ===\n");
    
    // Register some test modules first
    module_info_t module_info = {
        .module_id = 4,
        .address = 0x04,
        .type = MODULE_TYPE_CONTROLLER,
        .status = MODULE_STATUS_ONLINE
    };
    strncpy(module_info.name, "Test Controller", sizeof(module_info.name) - 1);
    
    hal_status_t status = module_manager_register_module(&module_info);
    test_assert(status == HAL_STATUS_OK, "Register test module before reset");
    
    // Test reset
    status = module_manager_reset();
    test_assert(status == HAL_STATUS_OK, "Module Manager reset");
    
    // Verify modules were cleared
    uint8_t module_ids[10];
    uint32_t actual_count;
    status = module_manager_get_registered_modules(module_ids, 10, &actual_count);
    test_assert(status == HAL_STATUS_OK, "Get modules after reset");
    test_assert(actual_count == 0, "No modules after reset");
    
    return true;
}

static bool test_module_manager_deinitialization(void) {
    printf("\n=== Testing Module Manager Deinitialization ===\n");
    
    // Test deinitialization
    hal_status_t status = module_manager_deinit();
    test_assert(status == HAL_STATUS_OK, "Module Manager deinitialization");
    
    // Test operations after deinitialization
    status = module_manager_start();
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Start after deinitialization");
    
    status = module_manager_stop();
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Stop after deinitialization");
    
    return true;
}
