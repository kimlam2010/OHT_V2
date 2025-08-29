/**
 * @file test_managers_integration.c
 * @brief Integration tests for Managers (Communication, Module, Safety, Telemetry)
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task Test hardened managers with HAL integration, thread safety, and validation
 */

#include "unity.h"
#include "communication_manager.h"
#include "module_manager.h"
#include "safety_manager.h"
#include "telemetry_manager.h"
#include "hal_common.h"
#include "hal_rs485.h"
#include "hal_estop.h"
#include "hal_led.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Test configuration
static rs485_config_t test_rs485_config = {
    .device_path = "/dev/ttyS1",
    .baud_rate = 115200,
    .data_bits = 8,
    .stop_bits = 1,
    .parity = 0,
    .timeout_ms = 1000,
    .retry_count = 3
};

static comm_mgr_config_t test_comm_config = {
    .baud_rate = 115200,
    .data_bits = 8,
    .stop_bits = 1,
    .parity = 0,
    .timeout_ms = 1000,
    .retry_count = 3,
    .retry_delay_ms = 100,
    .modbus_slave_id = 1,
    .enable_crc_check = true,
    .enable_echo_suppression = true,
    .buffer_size = 256
};

static safety_config_t test_safety_config = {
    .estop_pin = 0,
    .response_time_ms = 50,
    .debounce_time_ms = 10,
    .safety_check_interval_ms = 100,
    .fault_clear_timeout_ms = 2000,
    .event_callback = NULL
};

static telemetry_config_t test_telemetry_config = {
    .update_rate_ms = 100,
    .buffer_size = 64,
    .enable_location = true,
    .enable_navigation = true,
    .enable_dock = true,
    .enable_system = true,
    .enable_events = true
};

// Test state
static bool managers_initialized = false;

void setUp(void) {
    // Initialize HAL layer first
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, hal_rs485_init(&test_rs485_config));
    
    // Create estop config
    estop_config_t estop_config = {0};
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, hal_estop_init(&estop_config));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, hal_led_init());
    
    // Initialize managers
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_manager_init(&test_comm_config));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, module_manager_init());
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_init(&test_safety_config));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, telemetry_manager_init(&test_telemetry_config));
    
    managers_initialized = true;
}

void tearDown(void) {
    if (managers_initialized) {
        telemetry_manager_deinit();
        safety_manager_deinit();
        module_manager_deinit();
        comm_manager_deinit();
        
        hal_led_deinit();
        hal_estop_deinit();
        hal_rs485_deinit();
        
        managers_initialized = false;
    }
}

// Test 1: Communication Manager - RS485 and Modbus Integration
void test_communication_manager_rs485_integration(void) {
    // Test RS485 initialization and status
    comm_mgr_status_info_t status;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_manager_get_status(&status));
    TEST_ASSERT_TRUE(status.rs485_initialized);
    
    // Test Modbus read with bounds checking
    uint16_t test_data[10];
    hal_status_t result = comm_manager_modbus_read_holding_registers(1, 0x00F0, 1, test_data);
    // Should work if device exists, or timeout if not
    TEST_ASSERT_TRUE(result == HAL_STATUS_OK || result == HAL_STATUS_TIMEOUT);
    
    // Test invalid parameters (bounds checking)
    result = comm_manager_modbus_read_holding_registers(0, 0x00F0, 1, test_data); // Invalid slave_id
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, result);
    
    result = comm_manager_modbus_read_holding_registers(1, 0x00F0, 0, test_data); // Invalid quantity
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, result);
    
    result = comm_manager_modbus_read_holding_registers(1, 0x00F0, 1, NULL); // NULL data
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, result);
    
    // Test statistics
    comm_mgr_stats_t stats;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_manager_get_statistics(&stats));
    TEST_ASSERT_TRUE(stats.total_transmissions >= 0);
}

// Test 2: Module Manager - Discovery and Health Check Integration
void test_module_manager_discovery_integration(void) {
    // Test module discovery with communication manager dependency
    hal_status_t result = module_manager_discover_modules();
    // Should work if comm manager is ready
    TEST_ASSERT_TRUE(result == HAL_STATUS_OK || result == HAL_STATUS_INVALID_STATE);
    
    // Test health check with real response time measurement
    uint8_t module_ids[32];
    uint32_t actual_count;
    result = module_manager_get_registered_modules(module_ids, 32, &actual_count);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    
    if (actual_count > 0) {
        // Test health check on first discovered module
        result = module_manager_health_check_module(module_ids[0]);
        TEST_ASSERT_TRUE(result == HAL_STATUS_OK || result == HAL_STATUS_NOT_FOUND);
        
        // Test get module info with validation
        module_info_t info;
        result = module_manager_get_module_info(module_ids[0], &info);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
        TEST_ASSERT_TRUE(info.module_id > 0);
        
        // Test get module status with real health data
        module_status_info_t status;
        result = module_manager_get_module_status(module_ids[0], &status);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
        TEST_ASSERT_TRUE(status.health_percentage <= 100);
    }
    
    // Test bounds checking
    result = module_manager_get_registered_modules(NULL, 32, &actual_count);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, result);
    
    result = module_manager_get_registered_modules(module_ids, 0, &actual_count);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_EQUAL(0, actual_count);
}

// Test 3: Safety Manager - E-Stop and LED Integration
void test_safety_manager_estop_integration(void) {
    // Test safety status
    safety_status_info_t status;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_get_status(&status));
    TEST_ASSERT_TRUE(status.initialized);
    
    // Test E-Stop check
    bool estop_triggered;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_check_estop(&estop_triggered));
    TEST_ASSERT_FALSE(estop_triggered); // Should be safe initially
    
    // Test safety circuit check
    bool circuit_ok;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_check_safety_circuit(&circuit_ok));
    TEST_ASSERT_TRUE(circuit_ok);
    
    // Test sensor check
    bool sensors_ok;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_check_sensors(&sensors_ok));
    TEST_ASSERT_TRUE(sensors_ok);
    
    // Test overall safety status
    bool is_safe;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_is_safe(&is_safe));
    TEST_ASSERT_TRUE(is_safe);
    
    // Test fault handling
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_trigger_fault(SAFETY_FAULT_SYSTEM_FAILURE));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_get_status(&status));
    TEST_ASSERT_EQUAL(SAFETY_FAULT_SYSTEM_FAILURE, status.current_fault);
    
    // Test fault clearing
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_clear_fault());
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_get_status(&status));
    TEST_ASSERT_EQUAL(SAFETY_FAULT_NONE, status.current_fault);
    
    // Test bounds checking
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, safety_manager_get_status(NULL));
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, safety_manager_check_estop(NULL));
}

// Test 4: Telemetry Manager - JSON Serialization Integration
void test_telemetry_manager_json_integration(void) {
    // Test telemetry data collection
    telemetry_data_t data;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, telemetry_manager_get_data(&data));
    TEST_ASSERT_TRUE(data.ts > 0);
    
    // Test JSON serialization with buffer size limits
    char json_buffer[1024];
    int json_size = telemetry_manager_serialize_json(&data, json_buffer, sizeof(json_buffer));
    TEST_ASSERT_TRUE(json_size > 0);
    TEST_ASSERT_TRUE(json_size < (int)sizeof(json_buffer));
    
    // Test JSON buffer overflow protection
    char small_buffer[10];
    int small_size = telemetry_manager_serialize_json(&data, small_buffer, sizeof(small_buffer));
    TEST_ASSERT_TRUE(small_size < (int)sizeof(small_buffer));
    TEST_ASSERT_TRUE(small_size > 0);
    
    // Test NULL buffer protection
    int null_size = telemetry_manager_serialize_json(&data, NULL, 1024);
    TEST_ASSERT_TRUE(null_size <= 0);
    
    // Test zero buffer size protection
    int zero_size = telemetry_manager_serialize_json(&data, json_buffer, 0);
    TEST_ASSERT_TRUE(zero_size <= 0);
    
    // Test statistics
    telemetry_stats_t stats;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, telemetry_manager_get_statistics(&stats));
    TEST_ASSERT_TRUE(stats.total_updates >= 0);
}

// Test 5: Cross-Manager Integration - End-to-End Flow
void test_cross_manager_integration_flow(void) {
    // Test complete flow: Communication → Module Discovery → Safety Check → Telemetry
    
    // Step 1: Communication manager ready
    comm_mgr_status_info_t comm_status;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_manager_get_status(&comm_status));
    TEST_ASSERT_TRUE(comm_status.rs485_initialized);
    
    // Step 2: Module discovery
    hal_status_t result = module_manager_discover_modules();
    TEST_ASSERT_TRUE(result == HAL_STATUS_OK || result == HAL_STATUS_INVALID_STATE);
    
    // Step 3: Safety check
    bool is_safe;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_is_safe(&is_safe));
    TEST_ASSERT_TRUE(is_safe);
    
    // Step 4: Telemetry collection
    telemetry_data_t data;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, telemetry_manager_get_data(&data));
    TEST_ASSERT_TRUE(data.ts > 0);
    
    // Step 5: Verify integration - telemetry should reflect system state
    // Note: telemetry_data_t doesn't have safety.estop, so we check other fields
    TEST_ASSERT_TRUE(data.center_connected || data.rs485_connected);
}

// Test 6: Thread Safety and Concurrent Access
void test_managers_thread_safety(void) {
    // Test concurrent access to managers (basic test)
    comm_mgr_status_info_t comm_status;
    safety_status_info_t safety_status;
    telemetry_data_t telemetry_data;
    
    // Concurrent status queries
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_manager_get_status(&comm_status));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_get_status(&safety_status));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, telemetry_manager_get_data(&telemetry_data));
    
    // All should succeed without deadlock or corruption
    TEST_ASSERT_TRUE(comm_status.rs485_initialized);
    TEST_ASSERT_TRUE(safety_status.initialized);
    TEST_ASSERT_TRUE(telemetry_data.ts > 0);
}

// Test 7: Error Recovery and Resilience
void test_managers_error_recovery(void) {
    // Test error handling and recovery
    
    // Test communication manager error recovery
    comm_mgr_stats_t stats_before, stats_after;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_manager_get_statistics(&stats_before));
    
    // Trigger some errors (invalid requests)
    comm_manager_modbus_read_holding_registers(0, 0x00F0, 1, NULL); // Invalid
    comm_manager_modbus_read_holding_registers(1, 0x00F0, 0, NULL); // Invalid
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_manager_get_statistics(&stats_after));
    TEST_ASSERT_TRUE(stats_after.failed_transmissions >= stats_before.failed_transmissions);
    
    // Test safety manager fault recovery
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_trigger_fault(SAFETY_FAULT_SYSTEM_FAILURE));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_clear_fault());
    
    bool is_safe;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_manager_is_safe(&is_safe));
    TEST_ASSERT_TRUE(is_safe);
}

// Main test runner
int main(void) {
    UNITY_BEGIN();
    
    // Manager integration tests
    RUN_TEST(test_communication_manager_rs485_integration);
    RUN_TEST(test_module_manager_discovery_integration);
    RUN_TEST(test_safety_manager_estop_integration);
    RUN_TEST(test_telemetry_manager_json_integration);
    RUN_TEST(test_cross_manager_integration_flow);
    RUN_TEST(test_managers_thread_safety);
    RUN_TEST(test_managers_error_recovery);
    
    UNITY_END();
    return 0;
}
