/**
 * @file test_di_do_module.c
 * @brief Test suite for DI/DO Module Handler
 * @version 1.0.0
 * @date 2025-01-27
 */

#include "di_do_module_handler.h"
#include "safety_manager.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int tests_passed = 0;
static int tests_failed = 0;

// Test helper function
static void test_assert(bool condition, const char *message) {
    if (!condition) {
        printf("❌ %s\n", message);
        tests_failed++;
    } else {
        printf("✅ %s\n", message);
        tests_passed++;
    }
}

// ============================================================================
// TEST FUNCTIONS
// ============================================================================

static void test_di_do_module_init(void) {
    printf("\n=== Testing DI/DO Module Initialization ===\n");
    
    di_do_module_handler_t handler;
    
    // Test initialization with null pointer check
    hal_status_t result = di_do_module_init(NULL, DI_DO_MODULE_ADDRESS);
    test_assert(result == HAL_STATUS_ERROR, "Null pointer initialization should fail");

    // Test proper initialization
    result = di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    test_assert(result == HAL_STATUS_OK, "DI/DO module initialization");
    test_assert(handler.initialized == true, "Initialization flag set");
    test_assert(handler.address == DI_DO_MODULE_ADDRESS, "Handler address set correctly");
    test_assert(handler.enabled == false, "Initial state is disabled");
    test_assert(handler.fault_code == DI_DO_FAULT_NONE, "Initial fault code is NONE");
    
    // Test double initialization
    result = di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    test_assert(result == HAL_STATUS_OK, "Double initialization check");
    
    // Test deinitialization
    result = di_do_module_deinit(&handler);
    test_assert(result == HAL_STATUS_OK, "DI/DO module deinitialization");
    test_assert(handler.initialized == false, "Initialization flag cleared");
    
    printf("Initialization tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_enable_disable(void) {
    printf("\n=== Testing DI/DO Module Enable/Disable ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    
    // Test enable
    hal_status_t result = di_do_module_enable(&handler, true);
    test_assert(result == HAL_STATUS_OK, "DI/DO module enable");
    test_assert(handler.enabled == true, "Module enabled flag set");
    
    // Test disable
    result = di_do_module_enable(&handler, false);
    test_assert(result == HAL_STATUS_OK, "DI/DO module disable");
    test_assert(handler.enabled == false, "Module disabled flag set");
    
    di_do_module_deinit(&handler);
    printf("Enable/Disable tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_digital_io(void) {
    printf("\n=== Testing DI/DO Module Digital I/O ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    di_do_module_enable(&handler, true);
    
    // Test digital output write
    hal_status_t result = di_do_module_write_digital_output(&handler, 0, 1);
    test_assert(result == HAL_STATUS_OK, "Digital output write (true)");
    test_assert(handler.data.digital_outputs[0] == 1, "Digital output value set");
    
    result = di_do_module_write_digital_output(&handler, 1, 0);
    test_assert(result == HAL_STATUS_OK, "Digital output write (false)");
    test_assert(handler.data.digital_outputs[1] == 0, "Digital output value cleared");
    
    // Test digital input read (mock)
    uint16_t value;
    result = di_do_module_read_digital_input(&handler, 0, &value);
    test_assert(result == HAL_STATUS_OK, "Digital input read");
    
    // Test toggle
    result = di_do_module_toggle_digital_output(&handler, 0);
    test_assert(result == HAL_STATUS_OK, "Digital output toggle");
    test_assert(handler.data.digital_outputs[0] == 0, "Digital output toggled");
    
    // Test invalid channel
    result = di_do_module_write_digital_output(&handler, DI_DO_MAX_DIGITAL_CHANNELS, 1);
    test_assert(result == HAL_STATUS_ERROR, "Invalid digital channel check");
    
    di_do_module_deinit(&handler);
    printf("Digital I/O tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_analog_io(void) {
    printf("\n=== Testing DI/DO Module Analog I/O ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    di_do_module_enable(&handler, true);
    
    // Test analog output write
    hal_status_t result = di_do_module_write_analog_output(&handler, 0, 2048);
    test_assert(result == HAL_STATUS_OK, "Analog output write (2048)");
    test_assert(handler.data.analog_outputs[0] == 2048, "Analog output value set");
    
    result = di_do_module_write_analog_output(&handler, 1, 0);
    test_assert(result == HAL_STATUS_OK, "Analog output write (0)");
    test_assert(handler.data.analog_outputs[1] == 0, "Analog output value cleared");
    
    // Test analog input read (mock)
    uint16_t value;
    result = di_do_module_read_analog_input(&handler, 0, &value);
    test_assert(result == HAL_STATUS_OK, "Analog input read");
    
    // Test invalid value
    result = di_do_module_write_analog_output(&handler, 0, DI_DO_MAX_ANALOG_VALUE + 1);
    test_assert(result == HAL_STATUS_ERROR, "Invalid analog value check");
    
    // Test invalid channel
    result = di_do_module_write_analog_output(&handler, DI_DO_MAX_ANALOG_CHANNELS, 1000);
    test_assert(result == HAL_STATUS_ERROR, "Invalid analog channel check");
    
    di_do_module_deinit(&handler);
    printf("Analog I/O tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_batch_operations(void) {
    printf("\n=== Testing DI/DO Module Batch Operations ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    di_do_module_enable(&handler, true);
    
    // Test batch digital write
    uint16_t digital_values[4] = {1, 0, 1, 0};
    hal_status_t result = di_do_module_batch_write_digital(&handler, 0, 4, digital_values);
    test_assert(result == HAL_STATUS_OK, "Batch digital write");
    
    for (int i = 0; i < 4; i++) {
        test_assert(handler.data.digital_outputs[i] == digital_values[i], "Batch digital values set");
    }
    
    // Test batch digital read
    uint16_t read_values[4];
    result = di_do_module_batch_read_digital(&handler, 0, 4, read_values);
    test_assert(result == HAL_STATUS_OK, "Batch digital read");
    
    // Test batch analog write
    uint16_t analog_values[4] = {1000, 2000, 3000, 4000};
    result = di_do_module_batch_write_analog(&handler, 0, 4, analog_values);
    test_assert(result == HAL_STATUS_OK, "Batch analog write");
    
    for (int i = 0; i < 4; i++) {
        test_assert(handler.data.analog_outputs[i] == analog_values[i], "Batch analog values set");
    }
    
    // Test batch analog read
    uint16_t analog_read_values[4];
    result = di_do_module_batch_read_analog(&handler, 0, 4, analog_read_values);
    test_assert(result == HAL_STATUS_OK, "Batch analog read");
    
    // Test invalid batch size
    result = di_do_module_batch_write_digital(&handler, 0, DI_DO_MAX_DIGITAL_CHANNELS + 1, digital_values);
    test_assert(result == HAL_STATUS_ERROR, "Invalid batch size check");
    
    di_do_module_deinit(&handler);
    printf("Batch operations tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_configuration(void) {
    printf("\n=== Testing DI/DO Module Configuration ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    
    // Test debounce configuration
    hal_status_t result = di_do_module_set_debounce_time(&handler, 0, 50);
    test_assert(result == HAL_STATUS_OK, "Set debounce time (50ms)");
    test_assert(handler.data.debounce_times[0] == 50, "Debounce time set correctly");
    
    // Test invalid debounce time
    result = di_do_module_set_debounce_time(&handler, 0, DI_DO_MAX_DEBOUNCE_TIME_MS + 1);
    test_assert(result == HAL_STATUS_ERROR, "Invalid debounce time check");
    
    // Test edge detection configuration
    result = di_do_module_set_edge_detection(&handler, 0, DI_DO_EDGE_RISING);
    test_assert(result == HAL_STATUS_OK, "Set rising edge detection");
    test_assert(handler.data.edge_config[0] == DI_DO_EDGE_RISING, "Edge detection set correctly");
    
    result = di_do_module_set_edge_detection(&handler, 1, DI_DO_EDGE_FALLING);
    test_assert(result == HAL_STATUS_OK, "Set falling edge detection");
    test_assert(handler.data.edge_config[1] == DI_DO_EDGE_FALLING, "Edge detection set correctly");
    
    // Test analog filter configuration
    result = di_do_module_set_analog_filter(&handler, 0, 100);
    test_assert(result == HAL_STATUS_OK, "Set analog filter (100ms)");
    test_assert(handler.data.filter_time_constants[0] == 100, "Analog filter set correctly");
    
    // Test channel enable/disable
    result = di_do_module_enable_digital_channel(&handler, 0, false);
    test_assert(result == HAL_STATUS_OK, "Disable digital channel");
    test_assert(handler.data.channel_enabled[0] == 0, "Digital channel disabled");
    
    result = di_do_module_enable_analog_channel(&handler, 0, false);
    test_assert(result == HAL_STATUS_OK, "Disable analog channel");
    test_assert(handler.data.channel_enabled[DI_DO_MAX_DIGITAL_CHANNELS] == 0, "Analog channel disabled");
    
    di_do_module_deinit(&handler);
    printf("Configuration tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_status_monitoring(void) {
    printf("\n=== Testing DI/DO Module Status Monitoring ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    
    // Test status
    uint8_t status;
    hal_status_t result = di_do_module_get_status(&handler, &status);
    test_assert(result == HAL_STATUS_OK, "Get module status");
    
    // Test fault status
    uint8_t fault_code;
    result = di_do_module_get_fault(&handler, &fault_code);
    test_assert(result == HAL_STATUS_OK, "Get fault status");
    test_assert(fault_code == DI_DO_FAULT_NONE, "Initial fault code is NONE");
    
    // Test temperature
    uint16_t temperature;
    result = di_do_module_get_temperature(&handler, &temperature);
    test_assert(result == HAL_STATUS_OK, "Get temperature");
    
    // Test voltage
    uint16_t voltage;
    result = di_do_module_get_voltage(&handler, &voltage);
    test_assert(result == HAL_STATUS_OK, "Get voltage");
    
    // Test statistics
    uint32_t statistics[10];
    result = di_do_module_get_statistics(&handler, statistics);
    test_assert(result == HAL_STATUS_OK, "Get statistics");
    
    // Test reset faults
    result = di_do_module_reset_faults(&handler);
    test_assert(result == HAL_STATUS_OK, "Reset faults");
    test_assert(handler.fault_code == DI_DO_FAULT_NONE, "Fault code reset");
    
    di_do_module_deinit(&handler);
    printf("Status monitoring tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_diagnostics(void) {
    printf("\n=== Testing DI/DO Module Diagnostics ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    
    // Test self-test
    hal_status_t result = di_do_module_self_test(&handler);
    test_assert(result == HAL_STATUS_OK, "Self-test execution");
    
    // Test calibration
    result = di_do_module_calibrate_analog(&handler);
    test_assert(result == HAL_STATUS_OK, "Analog calibration");
    
    // Test diagnostics
    char diagnostics[512];
    result = di_do_module_get_diagnostics(&handler, diagnostics, sizeof(diagnostics));
    test_assert(result == HAL_STATUS_OK, "Get diagnostics");
    test_assert(strlen(diagnostics) > 0, "Diagnostics not empty");
    
    di_do_module_deinit(&handler);
    printf("Diagnostics tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_modbus_communication(void) {
    printf("\n=== Testing DI/DO Module Modbus Communication ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    
    // Test register read
    uint16_t value;
    hal_status_t result = di_do_module_read_register(&handler, DI_DO_STATUS_REG, &value);
    test_assert(result == HAL_STATUS_OK, "Read register");
    
    // Test register write
    result = di_do_module_write_register(&handler, DI_DO_STATUS_REG, 0x01);
    test_assert(result == HAL_STATUS_OK, "Write register");
    
    // Test multiple registers read
    uint16_t values[4];
    result = di_do_module_read_registers(&handler, DI_DO_DIGITAL_INPUTS_REG, 4, values);
    test_assert(result == HAL_STATUS_OK, "Read multiple registers");
    
    // Test multiple registers write
    uint16_t write_values[4] = {1, 0, 1, 0};
    result = di_do_module_write_registers(&handler, DI_DO_DIGITAL_OUTPUTS_REG, 4, write_values);
    test_assert(result == HAL_STATUS_OK, "Write multiple registers");
    
    di_do_module_deinit(&handler);
    printf("Modbus communication tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_event_handling(void) {
    printf("\n=== Testing DI/DO Module Event Handling ===\n");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    
    // Test event callback setting
    hal_status_t result = di_do_module_set_event_callback(&handler, NULL);
    test_assert(result == HAL_STATUS_OK, "Set event callback");
    
    // Test event triggering
    result = di_do_module_trigger_event(&handler, DI_DO_EVENT_DIGITAL_INPUT_CHANGED, 0, 1);
    test_assert(result == HAL_STATUS_OK, "Trigger event");
    
    di_do_module_deinit(&handler);
    printf("Event handling tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_utility_functions(void) {
    printf("\n=== Testing DI/DO Module Utility Functions ===\n");
    
    // Test fault descriptions
    const char* fault_desc = di_do_module_get_fault_description(DI_DO_FAULT_NONE);
    test_assert(strcmp(fault_desc, "No fault") == 0, "Fault description for NONE");
    
    fault_desc = di_do_module_get_fault_description(DI_DO_FAULT_COMMUNICATION_ERROR);
    test_assert(strcmp(fault_desc, "Communication error") == 0, "Fault description for communication error");
    
    // Test event descriptions
    const char* event_desc = di_do_module_get_event_description(DI_DO_EVENT_DIGITAL_INPUT_CHANGED);
    test_assert(strcmp(event_desc, "Digital input changed") == 0, "Event description for digital input changed");
    
    // Test channel validation
    bool valid = di_do_module_validate_channel(0, false);
    test_assert(valid == true, "Valid digital channel");
    
    valid = di_do_module_validate_channel(DI_DO_MAX_DIGITAL_CHANNELS, false);
    test_assert(valid == false, "Invalid digital channel");
    
    valid = di_do_module_validate_channel(0, true);
    test_assert(valid == true, "Valid analog channel");
    
    valid = di_do_module_validate_channel(DI_DO_MAX_ANALOG_CHANNELS, true);
    test_assert(valid == false, "Invalid analog channel");
    
    // Test configuration validation
    di_do_config_t valid_config = {
        .debounce_time_ms = 50,
        .filter_time_ms = 100,
        .edge_detection = DI_DO_EDGE_RISING,
        .enable_analog_filtering = true,
        .enable_digital_debouncing = true,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .fault_threshold_temp = 85,
        .fault_threshold_voltage = 1800,
        .retry_count = 3,
        .timeout_ms = 1000
    };
    
    valid = di_do_module_validate_config(&valid_config);
    test_assert(valid == true, "Valid configuration");
    
    di_do_config_t invalid_config = valid_config;
    invalid_config.debounce_time_ms = DI_DO_MAX_DEBOUNCE_TIME_MS + 1;
    valid = di_do_module_validate_config(&invalid_config);
    test_assert(valid == false, "Invalid configuration");
    
    printf("Utility functions tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_di_do_module_error_handling(void) {
    printf("\n=== Testing DI/DO Module Error Handling ===\n");
    
    // Test NULL handler
    hal_status_t result = di_do_module_init(NULL, DI_DO_MODULE_ADDRESS);
    test_assert(result == HAL_STATUS_ERROR, "NULL handler initialization");
    
    di_do_module_handler_t handler;
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    
    // Test operations before initialization
    handler.initialized = false;
    result = di_do_module_enable(&handler, true);
    test_assert(result == HAL_STATUS_ERROR, "Operation before initialization");
    
    // Test operations after deinitialization
    di_do_module_init(&handler, DI_DO_MODULE_ADDRESS);
    di_do_module_deinit(&handler);
    result = di_do_module_enable(&handler, true);
    test_assert(result == HAL_STATUS_ERROR, "Operation after deinitialization");
    
    printf("Error handling tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

// ============================================================================
// MAIN TEST FUNCTION
// ============================================================================

int main(void) {
    printf("=== DI/DO Module Test Suite ===\n");
    printf("Testing DI/DO Module Handler (FW-32)\n\n");
    
    // Initialize safety manager for tests
    safety_config_t safety_config = {
        .safety_check_interval_ms = 100,
        .estop_response_timeout_ms = 1000,
        .safety_circuit_timeout_ms = 500,
        .sensor_timeout_ms = 200,
        .enable_auto_recovery = true,
        .enable_safety_monitoring = true,
        .enable_estop_monitoring = true,
        .enable_sensor_monitoring = true
    };
    
    if (safety_manager_init(&safety_config) != HAL_STATUS_OK) {
        printf("Failed to initialize safety manager\n");
        return 1;
    }
    
    // Run all tests
    test_di_do_module_init();
    test_di_do_module_enable_disable();
    test_di_do_module_digital_io();
    test_di_do_module_analog_io();
    test_di_do_module_batch_operations();
    test_di_do_module_configuration();
    test_di_do_module_status_monitoring();
    test_di_do_module_diagnostics();
    test_di_do_module_modbus_communication();
    test_di_do_module_event_handling();
    test_di_do_module_utility_functions();
    test_di_do_module_error_handling();
    
    // Cleanup
    safety_manager_deinit();
    
    // Print final results
    printf("\n=== Test Results ===\n");
    printf("Total tests: %d\n", tests_passed + tests_failed);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / (tests_passed + tests_failed) * 100.0f);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! DI/DO Module Handler is working correctly.\n");
        return 0;
    } else {
        printf("\n⚠️  Some tests failed. Please review the implementation.\n");
        return 1;
    }
}
