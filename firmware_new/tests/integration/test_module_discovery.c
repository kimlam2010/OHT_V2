#include "unity.h"
#include "module_manager.h"
#include "communication_manager.h"
#include "power_module_handler.h"
#include "safety_module_handler.h"
#include "travel_motor_module_handler.h"
#include "dock_module_handler.h"
#include "hal_common.h"
#include <string.h>

// Test fixtures
static module_manager_t test_module_manager;
static communication_manager_t test_comm_manager;
static module_manager_config_t test_mm_config;
static communication_manager_config_t test_cm_config;

void setUp(void) {
    memset(&test_module_manager, 0, sizeof(test_module_manager));
    memset(&test_comm_manager, 0, sizeof(test_comm_manager));
    memset(&test_mm_config, 0, sizeof(test_mm_config));
    memset(&test_cm_config, 0, sizeof(test_cm_config));
    
    // Configure module manager
    test_mm_config.max_modules = 10;
    test_mm_config.discovery_timeout_ms = 5000;
    test_mm_config.health_check_interval_ms = 1000;
    
    // Configure communication manager
    test_cm_config.rs485_config.baud_rate = 115200;
    test_cm_config.rs485_config.data_bits = 8;
    test_cm_config.rs485_config.stop_bits = 1;
    test_cm_config.rs485_config.parity = RS485_PARITY_NONE;
    test_cm_config.rs485_config.timeout_ms = 100;
    test_cm_config.rs485_config.retry_count = 3;
}

void tearDown(void) {
    module_manager_deinit(&test_module_manager);
    communication_manager_deinit(&test_comm_manager);
}

// Test module discovery initialization
void test_module_discovery_initialization_works_correctly(void) {
    // Initialize communication manager first
    hal_status_t cm_result = communication_manager_init(&test_comm_manager, &test_cm_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, cm_result);
    
    // Initialize module manager
    hal_status_t mm_result = module_manager_init(&test_module_manager, &test_mm_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, mm_result);
    
    TEST_ASSERT_TRUE(test_module_manager.initialized);
    TEST_ASSERT_TRUE(test_comm_manager.initialized);
}

// Test power module discovery
void test_power_module_discovery_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    
    // Discover power module
    hal_status_t result = module_manager_discover_module(&test_module_manager, MODULE_TYPE_POWER);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Verify power module is registered
    module_info_t* power_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_POWER);
    TEST_ASSERT_NOT_NULL(power_module);
    TEST_ASSERT_EQUAL(MODULE_TYPE_POWER, power_module->type);
    TEST_ASSERT_EQUAL(0x02, power_module->address);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, power_module->status);
}

// Test safety module discovery
void test_safety_module_discovery_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    
    // Discover safety module
    hal_status_t result = module_manager_discover_module(&test_module_manager, MODULE_TYPE_SAFETY);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Verify safety module is registered
    module_info_t* safety_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_SAFETY);
    TEST_ASSERT_NOT_NULL(safety_module);
    TEST_ASSERT_EQUAL(MODULE_TYPE_SAFETY, safety_module->type);
    TEST_ASSERT_EQUAL(0x03, safety_module->address);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, safety_module->status);
}

// Test travel motor module discovery
void test_travel_motor_module_discovery_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    
    // Discover travel motor module
    hal_status_t result = module_manager_discover_module(&test_module_manager, MODULE_TYPE_TRAVEL_MOTOR);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Verify travel motor module is registered
    module_info_t* motor_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_TRAVEL_MOTOR);
    TEST_ASSERT_NOT_NULL(motor_module);
    TEST_ASSERT_EQUAL(MODULE_TYPE_TRAVEL_MOTOR, motor_module->type);
    TEST_ASSERT_EQUAL(0x04, motor_module->address);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, motor_module->status);
}

// Test dock module discovery
void test_dock_module_discovery_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    
    // Discover dock module
    hal_status_t result = module_manager_discover_module(&test_module_manager, MODULE_TYPE_DOCK);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Verify dock module is registered
    module_info_t* dock_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_DOCK);
    TEST_ASSERT_NOT_NULL(dock_module);
    TEST_ASSERT_EQUAL(MODULE_TYPE_DOCK, dock_module->type);
    TEST_ASSERT_EQUAL(0x05, dock_module->address);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, dock_module->status);
}

// Test auto-discovery of all modules
void test_auto_discovery_all_modules_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    
    // Auto-discover all modules
    hal_status_t result = module_manager_auto_discover(&test_module_manager);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Verify all modules are discovered
    module_info_t* power_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_POWER);
    module_info_t* safety_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_SAFETY);
    module_info_t* motor_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_TRAVEL_MOTOR);
    module_info_t* dock_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_DOCK);
    
    TEST_ASSERT_NOT_NULL(power_module);
    TEST_ASSERT_NOT_NULL(safety_module);
    TEST_ASSERT_NOT_NULL(motor_module);
    TEST_ASSERT_NOT_NULL(dock_module);
    
    // Verify module counts
    uint8_t module_count = module_manager_get_module_count(&test_module_manager);
    TEST_ASSERT_EQUAL(4, module_count);
}

// Test module communication
void test_module_communication_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    module_manager_auto_discover(&test_module_manager);
    
    // Test power module communication
    uint16_t battery_voltage;
    hal_status_t result = module_manager_read_register(&test_module_manager, MODULE_TYPE_POWER, 
                                                      POWER_REG_BATTERY_VOLTAGE, &battery_voltage);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Test safety module communication
    uint8_t safety_status;
    result = module_manager_read_register(&test_module_manager, MODULE_TYPE_SAFETY, 
                                        SAFETY_REG_SYSTEM_STATUS, &safety_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Test motor module communication
    uint16_t motor_speed;
    result = module_manager_read_register(&test_module_manager, MODULE_TYPE_TRAVEL_MOTOR, 
                                        MOTOR_REG_MOTOR1_SPEED, &motor_speed);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Test dock module communication
    uint16_t imu_status;
    result = module_manager_read_register(&test_module_manager, MODULE_TYPE_DOCK, 
                                        DOCK_REG_IMU_STATUS, &imu_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
}

// Test module health monitoring
void test_module_health_monitoring_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    module_manager_auto_discover(&test_module_manager);
    
    // Check health of all modules
    module_health_t power_health = module_manager_get_module_health(&test_module_manager, MODULE_TYPE_POWER);
    module_health_t safety_health = module_manager_get_module_health(&test_module_manager, MODULE_TYPE_SAFETY);
    module_health_t motor_health = module_manager_get_module_health(&test_module_manager, MODULE_TYPE_TRAVEL_MOTOR);
    module_health_t dock_health = module_manager_get_module_health(&test_module_manager, MODULE_TYPE_DOCK);
    
    TEST_ASSERT_EQUAL(MODULE_HEALTH_GOOD, power_health);
    TEST_ASSERT_EQUAL(MODULE_HEALTH_GOOD, safety_health);
    TEST_ASSERT_EQUAL(MODULE_HEALTH_GOOD, motor_health);
    TEST_ASSERT_EQUAL(MODULE_HEALTH_GOOD, dock_health);
}

// Test module status monitoring
void test_module_status_monitoring_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    module_manager_auto_discover(&test_module_manager);
    
    // Update module status
    hal_status_t result = module_manager_update_all_modules(&test_module_manager);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Check status of all modules
    module_info_t* power_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_POWER);
    module_info_t* safety_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_SAFETY);
    module_info_t* motor_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_TRAVEL_MOTOR);
    module_info_t* dock_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_DOCK);
    
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, power_module->status);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, safety_module->status);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, motor_module->status);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, dock_module->status);
}

// Test error handling
void test_module_discovery_error_handling_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    
    // Try to discover non-existent module
    hal_status_t result = module_manager_discover_module(&test_module_manager, MODULE_TYPE_UNKNOWN);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
    
    // Try to get non-existent module
    module_info_t* unknown_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_UNKNOWN);
    TEST_ASSERT_NULL(unknown_module);
    
    // Try to read from non-existent module
    uint16_t value;
    result = module_manager_read_register(&test_module_manager, MODULE_TYPE_UNKNOWN, 0x0000, &value);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

// Test module removal
void test_module_removal_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    module_manager_auto_discover(&test_module_manager);
    
    // Remove power module
    hal_status_t result = module_manager_remove_module(&test_module_manager, MODULE_TYPE_POWER);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Verify module is removed
    module_info_t* power_module = module_manager_get_module(&test_module_manager, MODULE_TYPE_POWER);
    TEST_ASSERT_NULL(power_module);
    
    // Verify module count is reduced
    uint8_t module_count = module_manager_get_module_count(&test_module_manager);
    TEST_ASSERT_EQUAL(3, module_count);
}

// Test module re-discovery
void test_module_rediscovery_works_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    module_manager_auto_discover(&test_module_manager);
    
    // Remove all modules
    module_manager_remove_module(&test_module_manager, MODULE_TYPE_POWER);
    module_manager_remove_module(&test_module_manager, MODULE_TYPE_SAFETY);
    module_manager_remove_module(&test_module_manager, MODULE_TYPE_TRAVEL_MOTOR);
    module_manager_remove_module(&test_module_manager, MODULE_TYPE_DOCK);
    
    // Re-discover all modules
    hal_status_t result = module_manager_auto_discover(&test_module_manager);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    
    // Verify all modules are re-discovered
    uint8_t module_count = module_manager_get_module_count(&test_module_manager);
    TEST_ASSERT_EQUAL(4, module_count);
}

// Test performance
void test_module_discovery_performance_is_acceptable(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    
    // Measure discovery time
    uint64_t start_time = hal_get_timestamp_ms();
    hal_status_t result = module_manager_auto_discover(&test_module_manager);
    uint64_t end_time = hal_get_timestamp_ms();
    uint64_t discovery_time = end_time - start_time;
    
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result);
    TEST_ASSERT_LESS_THAN(5000, discovery_time); // Should complete within 5 seconds
}

// Test concurrent operations
void test_concurrent_module_operations_work_correctly(void) {
    communication_manager_init(&test_comm_manager, &test_cm_config);
    module_manager_init(&test_module_manager, &test_mm_config);
    module_manager_auto_discover(&test_module_manager);
    
    // Simulate concurrent read operations
    uint16_t power_value, safety_value, motor_value, dock_value;
    
    hal_status_t result1 = module_manager_read_register(&test_module_manager, MODULE_TYPE_POWER, 
                                                       POWER_REG_BATTERY_VOLTAGE, &power_value);
    hal_status_t result2 = module_manager_read_register(&test_module_manager, MODULE_TYPE_SAFETY, 
                                                       SAFETY_REG_SYSTEM_STATUS, &safety_value);
    hal_status_t result3 = module_manager_read_register(&test_module_manager, MODULE_TYPE_TRAVEL_MOTOR, 
                                                       MOTOR_REG_MOTOR1_SPEED, &motor_value);
    hal_status_t result4 = module_manager_read_register(&test_module_manager, MODULE_TYPE_DOCK, 
                                                       DOCK_REG_IMU_STATUS, &dock_value);
    
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result1);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result2);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result3);
    TEST_ASSERT_EQUAL(HAL_STATUS_SUCCESS, result4);
}

int main(void) {
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_module_discovery_initialization_works_correctly);
    
    // Individual module discovery tests
    RUN_TEST(test_power_module_discovery_works_correctly);
    RUN_TEST(test_safety_module_discovery_works_correctly);
    RUN_TEST(test_travel_motor_module_discovery_works_correctly);
    RUN_TEST(test_dock_module_discovery_works_correctly);
    
    // Auto-discovery tests
    RUN_TEST(test_auto_discovery_all_modules_works_correctly);
    
    // Communication tests
    RUN_TEST(test_module_communication_works_correctly);
    
    // Health and status monitoring tests
    RUN_TEST(test_module_health_monitoring_works_correctly);
    RUN_TEST(test_module_status_monitoring_works_correctly);
    
    // Error handling tests
    RUN_TEST(test_module_discovery_error_handling_works_correctly);
    
    // Module management tests
    RUN_TEST(test_module_removal_works_correctly);
    RUN_TEST(test_module_rediscovery_works_correctly);
    
    // Performance tests
    RUN_TEST(test_module_discovery_performance_is_acceptable);
    
    // Concurrent operation tests
    RUN_TEST(test_concurrent_module_operations_work_correctly);
    
    return UNITY_END();
}
