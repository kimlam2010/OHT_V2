/**
 * @file test_module_manager.c
 * @brief Test suite for Module Management System
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-07 (Module Management Testing)
 */

#include "module_manager.h"
#include "communication_manager.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test counters
static int test_total = 0;
static int test_passed = 0;
static int test_failed = 0;

// Test assertion helper
static void test_assert(bool condition, const char *test_name) {
    test_total++;
    if (condition) {
        printf("✅ PASS: %s\n", test_name);
        test_passed++;
    } else {
        printf("❌ FAIL: %s\n", test_name);
        test_failed++;
    }
}

// Test callback function
static bool callback_called = false;
static module_event_t last_event = MODULE_EVENT_NONE;
static uint8_t last_module_id = 0;

static void test_module_callback(module_event_t event, uint8_t module_id, const void *data) {
    callback_called = true;
    last_event = event;
    last_module_id = module_id;
    printf("Module Event: %s, Module ID: %d\n", module_manager_get_event_name(event), module_id);
}

// Test functions

static void test_module_manager_initialization(void) {
    printf("\n=== Testing Module Manager Initialization ===\n");
    
    // Test initialization with valid config
    module_config_t config = {
        .module_id = 0,
        .discovery_timeout_ms = 3000,
        .health_check_interval_ms = 5000,
        .max_response_time_ms = 800,
        .retry_count = 2,
        .auto_discovery_enabled = true,
        .health_monitoring_enabled = true,
        .config_flags = 0
    };
    
    hal_status_t status = module_manager_init(&config);
    test_assert(status == HAL_STATUS_OK, "Module Manager initialization with valid config");
    
    // Test double initialization
    status = module_manager_init(&config);
    test_assert(status == HAL_STATUS_ALREADY_INITIALIZED, "Module Manager double initialization");
    
    // Test initialization with NULL config
    module_manager_deinit();
    status = module_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Module Manager initialization with NULL config");
}

static void test_module_manager_configuration(void) {
    printf("\n=== Testing Module Manager Configuration ===\n");
    
    // Test get configuration
    module_config_t config;
    hal_status_t status = module_manager_get_config(&config);
    test_assert(status == HAL_STATUS_OK, "Get Module Manager configuration");
    test_assert(config.discovery_timeout_ms == 5000, "Default discovery timeout");
    test_assert(config.health_check_interval_ms == 10000, "Default health check interval");
    test_assert(config.max_response_time_ms == 1000, "Default max response time");
    test_assert(config.retry_count == 3, "Default retry count");
    test_assert(config.auto_discovery_enabled == true, "Default auto discovery enabled");
    test_assert(config.health_monitoring_enabled == true, "Default health monitoring enabled");
    
    // Test set configuration
    config.discovery_timeout_ms = 8000;
    config.health_check_interval_ms = 15000;
    config.max_response_time_ms = 1200;
    status = module_manager_set_config(&config);
    test_assert(status == HAL_STATUS_OK, "Set Module Manager configuration");
    
    // Verify configuration was set
    module_config_t new_config;
    status = module_manager_get_config(&new_config);
    test_assert(status == HAL_STATUS_OK, "Get updated configuration");
    test_assert(new_config.discovery_timeout_ms == 8000, "Updated discovery timeout");
    test_assert(new_config.health_check_interval_ms == 15000, "Updated health check interval");
    test_assert(new_config.max_response_time_ms == 1200, "Updated max response time");
    
    // Test set configuration with NULL
    status = module_manager_set_config(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Set configuration with NULL");
}

static void test_module_manager_status(void) {
    printf("\n=== Testing Module Manager Status ===\n");
    
    // Test get status
    module_mgr_status_t status_info;
    hal_status_t status = module_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get Module Manager status");
    test_assert(status_info.initialized == true, "Module Manager initialized");
    test_assert(status_info.discovery_active == false, "Discovery not active initially");
    test_assert(status_info.health_monitoring_active == false, "Health monitoring not active initially");
    test_assert(status_info.total_registered_modules == 0, "No modules registered initially");
    
    // Test get status with NULL
    status = module_manager_get_status(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Get status with NULL");
}

static void test_module_manager_statistics(void) {
    printf("\n=== Testing Module Manager Statistics ===\n");
    
    // Test get statistics
    module_stats_t stats;
    hal_status_t status = module_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get Module Manager statistics");
    test_assert(stats.total_modules == 0, "Initial total modules");
    test_assert(stats.online_modules == 0, "Initial online modules");
    test_assert(stats.offline_modules == 0, "Initial offline modules");
    test_assert(stats.error_modules == 0, "Initial error modules");
    test_assert(stats.discovery_attempts == 0, "Initial discovery attempts");
    test_assert(stats.successful_discoveries == 0, "Initial successful discoveries");
    
    // Test reset statistics
    status = module_manager_reset_statistics();
    test_assert(status == HAL_STATUS_OK, "Reset Module Manager statistics");
    
    // Verify statistics were reset
    status = module_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get reset statistics");
    test_assert(stats.total_modules == 0, "Reset total modules");
    test_assert(stats.online_modules == 0, "Reset online modules");
}

static void test_module_manager_event_callback(void) {
    printf("\n=== Testing Module Manager Event Callback ===\n");
    
    // Test set callback
    hal_status_t status = module_manager_set_callback(test_module_callback);
    test_assert(status == HAL_STATUS_OK, "Set Module Manager event callback");
    
    // Test set callback to NULL
    status = module_manager_set_callback(NULL);
    test_assert(status == HAL_STATUS_OK, "Set Module Manager callback to NULL");
}

static void test_module_manager_module_registration(void) {
    printf("\n=== Testing Module Manager Module Registration ===\n");
    
    // Test register module
    module_info_t module_info = {
        .module_id = 1,
        .type = MODULE_TYPE_SENSOR,
        .capabilities = 0x12345678,
        .config_flags = 0
    };
    strcpy(module_info.name, "TEST_SENSOR");
    strcpy(module_info.version, "2.1.0");
    strcpy(module_info.serial_number, "SN123456789");
    
    hal_status_t status = module_manager_register_module(1, &module_info);
    test_assert(status == HAL_STATUS_OK, "Register module");
    
    // Test register duplicate module
    status = module_manager_register_module(1, &module_info);
    test_assert(status == HAL_STATUS_ALREADY_ACTIVE, "Register duplicate module");
    
    // Test register module with invalid ID
    module_info.module_id = 0;
    status = module_manager_register_module(0, &module_info);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Register module with invalid ID");
    
    // Test register module with NULL info
    status = module_manager_register_module(2, NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Register module with NULL info");
    
    // Test get module info
    module_info_t retrieved_info;
    status = module_manager_get_module_info(1, &retrieved_info);
    test_assert(status == HAL_STATUS_OK, "Get module info");
    test_assert(retrieved_info.module_id == 1, "Module ID matches");
    test_assert(retrieved_info.type == MODULE_TYPE_SENSOR, "Module type matches");
    test_assert(strcmp(retrieved_info.name, "TEST_SENSOR") == 0, "Module name matches");
    test_assert(strcmp(retrieved_info.version, "2.1.0") == 0, "Module version matches");
    test_assert(strcmp(retrieved_info.serial_number, "SN123456789") == 0, "Module serial number matches");
    test_assert(retrieved_info.capabilities == 0x12345678, "Module capabilities match");
    
    // Test get module info for non-existent module
    status = module_manager_get_module_info(99, &retrieved_info);
    test_assert(status == HAL_STATUS_ERROR, "Get module info for non-existent module");
    
    // Test get module status
    module_status_info_t module_status;
    status = module_manager_get_module_status(1, &module_status);
    test_assert(status == HAL_STATUS_OK, "Get module status");
    test_assert(module_status.status == MODULE_STATUS_ONLINE, "Module status is online");
    test_assert(module_status.health == MODULE_HEALTH_GOOD, "Module health is good");
    test_assert(module_status.health_percentage == 90, "Module health percentage is 90");
    test_assert(module_status.error_count == 0, "Module error count is 0");
    test_assert(module_status.warning_count == 0, "Module warning count is 0");
    
    // Test get module status for non-existent module
    status = module_manager_get_module_status(99, &module_status);
    test_assert(status == HAL_STATUS_ERROR, "Get module status for non-existent module");
    
    // Test get registered modules
    uint8_t module_ids[10];
    uint32_t actual_count;
    status = module_manager_get_registered_modules(module_ids, 10, &actual_count);
    test_assert(status == HAL_STATUS_OK, "Get registered modules");
    test_assert(actual_count == 1, "One module registered");
    test_assert(module_ids[0] == 1, "Registered module ID is 1");
    
    // Test unregister module
    status = module_manager_unregister_module(1);
    test_assert(status == HAL_STATUS_OK, "Unregister module");
    
    // Test unregister non-existent module
    status = module_manager_unregister_module(99);
    test_assert(status == HAL_STATUS_ERROR, "Unregister non-existent module");
    
    // Verify module is unregistered
    status = module_manager_get_module_info(1, &retrieved_info);
    test_assert(status == HAL_STATUS_ERROR, "Get info for unregistered module");
}

static void test_module_manager_discovery(void) {
    printf("\n=== Testing Module Manager Discovery ===\n");
    
    // Test start discovery
    hal_status_t status = module_manager_start_discovery();
    test_assert(status == HAL_STATUS_OK, "Start module discovery");
    
    // Verify discovery is active
    module_mgr_status_t status_info;
    status = module_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get status after start discovery");
    test_assert(status_info.discovery_active == true, "Discovery is active");
    
    // Test stop discovery
    status = module_manager_stop_discovery();
    test_assert(status == HAL_STATUS_OK, "Stop module discovery");
    
    // Verify discovery is stopped
    status = module_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get status after stop discovery");
    test_assert(status_info.discovery_active == false, "Discovery is stopped");
}

static void test_module_manager_health_check(void) {
    printf("\n=== Testing Module Manager Health Check ===\n");
    
    // Register a test module first
    module_info_t module_info = {
        .module_id = 2,
        .type = MODULE_TYPE_ACTUATOR,
        .capabilities = 0x87654321,
        .config_flags = 0
    };
    strcpy(module_info.name, "TEST_ACTUATOR");
    strcpy(module_info.version, "1.5.2");
    strcpy(module_info.serial_number, "SN987654321");
    
    hal_status_t status = module_manager_register_module(2, &module_info);
    test_assert(status == HAL_STATUS_OK, "Register module for health check");
    
    // Test health check on specific module
    status = module_manager_health_check_module(2);
    test_assert(status == HAL_STATUS_ERROR || status == HAL_STATUS_OK, "Health check on specific module");
    
    // Test health check on non-existent module
    status = module_manager_health_check_module(99);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Health check on non-existent module");
    
    // Test health check on all modules
    status = module_manager_health_check_all();
    test_assert(status == HAL_STATUS_ERROR || status == HAL_STATUS_OK, "Health check on all modules");
    
    // Unregister test module
    module_manager_unregister_module(2);
}

static void test_module_manager_utility_functions(void) {
    printf("\n=== Testing Module Manager Utility Functions ===\n");
    
    // Test type name functions
    const char* type_name = module_manager_get_type_name(MODULE_TYPE_SENSOR);
    test_assert(strcmp(type_name, "SENSOR") == 0, "Sensor type name");
    
    type_name = module_manager_get_type_name(MODULE_TYPE_ACTUATOR);
    test_assert(strcmp(type_name, "ACTUATOR") == 0, "Actuator type name");
    
    type_name = module_manager_get_type_name(MODULE_TYPE_SAFETY);
    test_assert(strcmp(type_name, "SAFETY") == 0, "Safety type name");
    
    // Test status name functions
    const char* status_name = module_manager_get_status_name(MODULE_STATUS_ONLINE);
    test_assert(strcmp(status_name, "ONLINE") == 0, "Online status name");
    
    status_name = module_manager_get_status_name(MODULE_STATUS_OFFLINE);
    test_assert(strcmp(status_name, "OFFLINE") == 0, "Offline status name");
    
    status_name = module_manager_get_status_name(MODULE_STATUS_ERROR);
    test_assert(strcmp(status_name, "ERROR") == 0, "Error status name");
    
    // Test health name functions
    const char* health_name = module_manager_get_health_name(MODULE_HEALTH_EXCELLENT);
    test_assert(strcmp(health_name, "EXCELLENT") == 0, "Excellent health name");
    
    health_name = module_manager_get_health_name(MODULE_HEALTH_GOOD);
    test_assert(strcmp(health_name, "GOOD") == 0, "Good health name");
    
    health_name = module_manager_get_health_name(MODULE_HEALTH_FAILED);
    test_assert(strcmp(health_name, "FAILED") == 0, "Failed health name");
    
    // Test event name functions
    const char* event_name = module_manager_get_event_name(MODULE_EVENT_DISCOVERED);
    test_assert(strcmp(event_name, "DISCOVERED") == 0, "Discovered event name");
    
    event_name = module_manager_get_event_name(MODULE_EVENT_REGISTERED);
    test_assert(strcmp(event_name, "REGISTERED") == 0, "Registered event name");
    
    event_name = module_manager_get_event_name(MODULE_EVENT_ONLINE);
    test_assert(strcmp(event_name, "ONLINE") == 0, "Online event name");
}

static void test_module_manager_self_test(void) {
    printf("\n=== Testing Module Manager Self Test ===\n");
    
    // Test self test
    hal_status_t status = module_manager_self_test();
    test_assert(status == HAL_STATUS_OK, "Module Manager self test");
}

static void test_module_manager_reset(void) {
    printf("\n=== Testing Module Manager Reset ===\n");
    
    // Register some test modules first
    module_info_t module_info = {
        .module_id = 3,
        .type = MODULE_TYPE_CONTROLLER,
        .capabilities = 0x11111111,
        .config_flags = 0
    };
    strcpy(module_info.name, "TEST_CONTROLLER");
    strcpy(module_info.version, "3.0.1");
    strcpy(module_info.serial_number, "SN111111111");
    
    hal_status_t status = module_manager_register_module(3, &module_info);
    test_assert(status == HAL_STATUS_OK, "Register module for reset test");
    
    // Test reset
    status = module_manager_reset();
    test_assert(status == HAL_STATUS_OK, "Module Manager reset");
    
    // Verify reset
    module_mgr_status_t status_info;
    status = module_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get status after reset");
    test_assert(status_info.total_registered_modules == 0, "No modules after reset");
    test_assert(status_info.discovery_active == false, "Discovery stopped after reset");
    test_assert(status_info.health_monitoring_active == false, "Health monitoring stopped after reset");
    
    // Verify module is unregistered
    module_info_t retrieved_info;
    status = module_manager_get_module_info(3, &retrieved_info);
    test_assert(status == HAL_STATUS_ERROR, "Module unregistered after reset");
}

static void test_module_manager_update(void) {
    printf("\n=== Testing Module Manager Update ===\n");
    
    // Test update function
    hal_status_t status = module_manager_update();
    test_assert(status == HAL_STATUS_OK, "Module Manager update");
    
    // Test multiple updates
    for (int i = 0; i < 5; i++) {
        status = module_manager_update();
        test_assert(status == HAL_STATUS_OK, "Module Manager update iteration");
        hal_sleep_ms(10);
    }
}

static void test_module_manager_deinitialization(void) {
    printf("\n=== Testing Module Manager Deinitialization ===\n");
    
    // Test deinitialization
    hal_status_t status = module_manager_deinit();
    test_assert(status == HAL_STATUS_OK, "Module Manager deinitialization");
    
    // Test operations after deinitialization
    module_mgr_status_t status_info;
    status = module_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Get status after deinit");
    
    module_config_t config;
    status = module_manager_get_config(&config);
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Get config after deinit");
    
    status = module_manager_update();
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Update after deinit");
}

static void print_test_summary(void) {
    printf("\n=== Module Manager Test Summary ===\n");
    printf("Total Tests: %d\n", test_total);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Success Rate: %.1f%%\n", (float)test_passed / test_total * 100.0f);
    
    if (test_failed == 0) {
        printf("🎉 All tests passed!\n");
    } else {
        printf("⚠️  Some tests failed. Please review the implementation.\n");
    }
}

int main(void) {
    printf("🚀 Starting Module Manager Tests...\n");
    printf("==========================================\n");
    
    // Initialize test counters
    test_passed = 0;
    test_failed = 0;
    test_total = 0;
    
    // Initialize Communication Manager (required for Module Manager)
    comm_mgr_config_t comm_config = {
        .baud_rate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0,
        .timeout_ms = 1000,
        .retry_count = 3,
        .retry_delay_ms = 100,
        .modbus_slave_id = 2,
        .enable_crc_check = true,
        .enable_echo_suppression = true,
        .buffer_size = 256
    };
    // Device path is set internally in communication_manager.c
    
    hal_status_t status = comm_manager_init(&comm_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Failed to initialize Communication Manager: %d\n", status);
        return 1;
    }
    
    // Run all tests
    test_module_manager_initialization();
    test_module_manager_configuration();
    test_module_manager_status();
    test_module_manager_statistics();
    test_module_manager_event_callback();
    test_module_manager_module_registration();
    test_module_manager_discovery();
    test_module_manager_health_check();
    test_module_manager_utility_functions();
    test_module_manager_self_test();
    test_module_manager_reset();
    test_module_manager_update();
    test_module_manager_deinitialization();
    
    // Deinitialize Communication Manager
    comm_manager_deinit();
    
    // Print summary
    print_test_summary();
    
    return (test_failed == 0) ? 0 : 1;
}
