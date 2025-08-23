/**
 * @file test_io_module.c
 * @brief Test suite for IO Module Handler (FW-30)
 * @version 1.0
 * @date 2025-01-27
 */

#include "io_module_handler.h"
#include "safety_manager.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Test counters
static int tests_passed = 0;
static int tests_failed = 0;

// Test helper functions
static void test_assert(bool condition, const char* test_name) {
    if (condition) {
        printf("✅ %s\n", test_name);
        tests_passed++;
    } else {
        printf("❌ %s\n", test_name);
        tests_failed++;
    }
}

static void test_io_event_callback(io_module_handler_t *handler, io_event_t event) {
    printf("[TEST] IO Event: %d\n", event);
}

// Test functions
static void test_io_module_init() {
    printf("\n=== Testing IO Module Initialization ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 8,
        .digital_output_count = 8,
        .analog_input_count = 4,
        .analog_output_count = 4,
        .debounce_time_ms = 20,
        .sample_rate_hz = 100,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    // Test initialization with config
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization with config");
    test_assert(handler.initialized == true, "Handler initialized flag");
    test_assert(handler.address == 0x04, "Handler address set correctly");
    test_assert(handler.config.digital_input_count == 8, "Digital input count");
    test_assert(handler.config.analog_input_count == 4, "Analog input count");
    
    // Test initialization with NULL config (should use defaults)
    io_module_handler_t handler2;
    status = io_module_init(&handler2, NULL);
    test_assert(status == HAL_STATUS_OK, "IO module initialization with NULL config");
    test_assert(handler2.config.address == IO_DEFAULT_ADDRESS, "Default address");
    test_assert(handler2.config.digital_input_count == IO_DEFAULT_DIGITAL_INPUTS, "Default digital inputs");
    
    // Test double initialization
    status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_ALREADY_INITIALIZED, "Double initialization check");
    
    // Test deinitialization
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
    test_assert(handler.initialized == false, "Handler deinitialized flag");
    
    status = io_module_deinit(&handler2);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization 2");
}

static void test_io_module_enable_disable() {
    printf("\n=== Testing IO Module Enable/Disable ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 4,
        .digital_output_count = 4,
        .analog_input_count = 2,
        .analog_output_count = 2,
        .debounce_time_ms = 10,
        .sample_rate_hz = 50,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for enable test");
    
    // Test enable
    status = io_module_enable(&handler, true);
    test_assert(status == HAL_STATUS_OK, "IO module enable");
    test_assert(handler.enabled == true, "Handler enabled flag");
    test_assert(io_module_is_enabled(&handler) == true, "IO module is enabled check");
    
    // Test disable
    status = io_module_enable(&handler, false);
    test_assert(status == HAL_STATUS_OK, "IO module disable");
    test_assert(handler.enabled == false, "Handler disabled flag");
    test_assert(io_module_is_enabled(&handler) == false, "IO module is disabled check");
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_digital_io() {
    printf("\n=== Testing IO Module Digital I/O ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 4,
        .digital_output_count = 4,
        .analog_input_count = 2,
        .analog_output_count = 2,
        .debounce_time_ms = 10,
        .sample_rate_hz = 50,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for digital I/O test");
    
    status = io_module_enable(&handler, true);
    test_assert(status == HAL_STATUS_OK, "IO module enable for digital I/O test");
    
    // Test digital output write
    status = io_module_write_digital_output(&handler, 0, true);
    test_assert(status == HAL_STATUS_OK, "Digital output write (true)");
    
    status = io_module_write_digital_output(&handler, 1, false);
    test_assert(status == HAL_STATUS_OK, "Digital output write (false)");
    
    // Test digital input read (will fail without real hardware, but should not crash)
    bool value;
    status = io_module_read_digital_input(&handler, 0, &value);
    // Note: This will fail without real hardware, but the function should handle it gracefully
    printf("[TEST] Digital input read status: %d (expected to fail without hardware)\n", status);
    
    // Test invalid channel
    status = io_module_write_digital_output(&handler, 10, true);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid digital output channel");
    
    status = io_module_read_digital_input(&handler, 10, &value);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid digital input channel");
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_analog_io() {
    printf("\n=== Testing IO Module Analog I/O ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 2,
        .digital_output_count = 2,
        .analog_input_count = 4,
        .analog_output_count = 4,
        .debounce_time_ms = 10,
        .sample_rate_hz = 100,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for analog I/O test");
    
    status = io_module_enable(&handler, true);
    test_assert(status == HAL_STATUS_OK, "IO module enable for analog I/O test");
    
    // Test analog output write
    status = io_module_write_analog_output(&handler, 0, 2048);
    test_assert(status == HAL_STATUS_OK, "Analog output write (2048)");
    
    status = io_module_write_analog_output(&handler, 1, 4095);
    test_assert(status == HAL_STATUS_OK, "Analog output write (4095)");
    
    // Test analog input read (will fail without real hardware, but should not crash)
    uint16_t value;
    status = io_module_read_analog_input(&handler, 0, &value);
    // Note: This will fail without real hardware, but the function should handle it gracefully
    printf("[TEST] Analog input read status: %d (expected to fail without hardware)\n", status);
    
    // Test invalid value
    status = io_module_write_analog_output(&handler, 0, 5000);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid analog output value");
    
    // Test invalid channel
    status = io_module_write_analog_output(&handler, 10, 2048);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid analog output channel");
    
    status = io_module_read_analog_input(&handler, 10, &value);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid analog input channel");
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_debounce() {
    printf("\n=== Testing IO Module Debounce Logic ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 4,
        .digital_output_count = 4,
        .analog_input_count = 2,
        .analog_output_count = 2,
        .debounce_time_ms = 20,
        .sample_rate_hz = 50,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for debounce test");
    
    // Test debounce configuration
    status = io_module_set_debounce(&handler, 0, 50);
    test_assert(status == HAL_STATUS_OK, "Set debounce time (50ms)");
    
    status = io_module_set_debounce(&handler, 1, 100);
    test_assert(status == HAL_STATUS_OK, "Set debounce time (100ms)");
    
    // Test invalid debounce time
    status = io_module_set_debounce(&handler, 0, 0);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid debounce time (0ms)");
    
    status = io_module_set_debounce(&handler, 0, 200);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid debounce time (200ms)");
    
    // Test invalid channel
    status = io_module_set_debounce(&handler, 10, 50);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid channel for debounce");
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_edge_detection() {
    printf("\n=== Testing IO Module Edge Detection ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 4,
        .digital_output_count = 4,
        .analog_input_count = 2,
        .analog_output_count = 2,
        .debounce_time_ms = 10,
        .sample_rate_hz = 50,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for edge detection test");
    
    // Test edge detection configuration
    status = io_module_set_edge_detection(&handler, 0, IO_EDGE_RISING);
    test_assert(status == HAL_STATUS_OK, "Set rising edge detection");
    
    status = io_module_set_edge_detection(&handler, 1, IO_EDGE_FALLING);
    test_assert(status == HAL_STATUS_OK, "Set falling edge detection");
    
    status = io_module_set_edge_detection(&handler, 2, IO_EDGE_BOTH);
    test_assert(status == HAL_STATUS_OK, "Set both edges detection");
    
    status = io_module_set_edge_detection(&handler, 3, IO_EDGE_NONE);
    test_assert(status == HAL_STATUS_OK, "Set no edge detection");
    
    // Test invalid channel
    status = io_module_set_edge_detection(&handler, 10, IO_EDGE_RISING);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid channel for edge detection");
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_batch_operations() {
    printf("\n=== Testing IO Module Batch Operations ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 4,
        .digital_output_count = 4,
        .analog_input_count = 2,
        .analog_output_count = 2,
        .debounce_time_ms = 10,
        .sample_rate_hz = 50,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for batch operations test");
    
    status = io_module_enable(&handler, true);
    test_assert(status == HAL_STATUS_OK, "IO module enable for batch operations test");
    
    // Test batch read digital
    uint16_t inputs[4], outputs[4];
    status = io_module_batch_read_digital(&handler, inputs, outputs);
    // Note: This will fail without real hardware, but the function should handle it gracefully
    printf("[TEST] Batch read digital status: %d (expected to fail without hardware)\n", status);
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_validation() {
    printf("\n=== Testing IO Module Validation ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 4,
        .digital_output_count = 4,
        .analog_input_count = 2,
        .analog_output_count = 2,
        .debounce_time_ms = 10,
        .sample_rate_hz = 50,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for validation test");
    
    // Test validation functions
    test_assert(io_module_validate_digital_value(true) == true, "Digital value validation (true)");
    test_assert(io_module_validate_digital_value(false) == true, "Digital value validation (false)");
    
    test_assert(io_module_validate_analog_value(2048) == true, "Analog value validation (2048)");
    test_assert(io_module_validate_analog_value(4095) == true, "Analog value validation (4095)");
    test_assert(io_module_validate_analog_value(5000) == false, "Analog value validation (5000) - invalid");
    
    test_assert(io_module_validate_debounce_time(50) == true, "Debounce time validation (50ms)");
    test_assert(io_module_validate_debounce_time(0) == false, "Debounce time validation (0ms) - invalid");
    test_assert(io_module_validate_debounce_time(200) == false, "Debounce time validation (200ms) - invalid");
    
    test_assert(io_module_validate_sample_rate(100) == true, "Sample rate validation (100Hz)");
    test_assert(io_module_validate_sample_rate(0) == false, "Sample rate validation (0Hz) - invalid");
    test_assert(io_module_validate_sample_rate(2000) == false, "Sample rate validation (2000Hz) - invalid");
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_status_monitoring() {
    printf("\n=== Testing IO Module Status Monitoring ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 4,
        .digital_output_count = 4,
        .analog_input_count = 2,
        .analog_output_count = 2,
        .debounce_time_ms = 10,
        .sample_rate_hz = 50,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for status monitoring test");
    
    // Test status functions
    io_state_t state;
    io_fault_code_t fault_code;
    status = io_module_get_status(&handler, &state, &fault_code);
    test_assert(status == HAL_STATUS_OK, "Get IO module status");
    test_assert(state == IO_STATE_IDLE, "Initial state is IDLE");
    test_assert(fault_code == IO_FAULT_NONE, "Initial fault code is NONE");
    
    test_assert(io_module_has_faults(&handler) == false, "No faults initially");
    
    const char* fault_desc = io_module_get_fault_description(&handler);
    test_assert(strcmp(fault_desc, "No Fault") == 0, "Fault description for no fault");
    
    // Test fault reset
    status = io_module_reset_faults(&handler);
    test_assert(status == HAL_STATUS_OK, "Reset faults");
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_diagnostics() {
    printf("\n=== Testing IO Module Diagnostics ===\n");
    
    io_module_handler_t handler;
    io_module_config_t config = {
        .address = 0x04,
        .digital_input_count = 4,
        .digital_output_count = 4,
        .analog_input_count = 2,
        .analog_output_count = 2,
        .debounce_time_ms = 10,
        .sample_rate_hz = 50,
        .enable_edge_detection = true,
        .enable_health_monitoring = true,
        .enable_batch_operations = true
    };
    
    hal_status_t status = io_module_init(&handler, &config);
    test_assert(status == HAL_STATUS_OK, "IO module initialization for diagnostics test");
    
    // Test diagnostics
    char diagnostics[1024];
    status = io_module_get_diagnostics(&handler, diagnostics, sizeof(diagnostics));
    test_assert(status == HAL_STATUS_OK, "Get IO module diagnostics");
    test_assert(strlen(diagnostics) > 0, "Diagnostics string not empty");
    
    printf("[TEST] Diagnostics:\n%s\n", diagnostics);
    
    // Test statistics
    uint32_t total_reads, total_writes, total_errors;
    status = io_module_get_statistics(&handler, &total_reads, &total_writes, &total_errors);
    test_assert(status == HAL_STATUS_OK, "Get IO module statistics");
    test_assert(total_reads == 0, "Initial total reads is 0");
    test_assert(total_writes == 0, "Initial total writes is 0");
    test_assert(total_errors == 0, "Initial total errors is 0");
    
    // Test self-test
    status = io_module_self_test(&handler);
    // Note: This will fail without real hardware, but the function should handle it gracefully
    printf("[TEST] Self-test status: %d (expected to fail without hardware)\n", status);
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

static void test_io_module_utility_functions() {
    printf("\n=== Testing IO Module Utility Functions ===\n");
    
    // Test state strings
    test_assert(strcmp(io_module_get_state_string(IO_STATE_IDLE), "IDLE") == 0, "State string IDLE");
    test_assert(strcmp(io_module_get_state_string(IO_STATE_READING), "READING") == 0, "State string READING");
    test_assert(strcmp(io_module_get_state_string(IO_STATE_WRITING), "WRITING") == 0, "State string WRITING");
    test_assert(strcmp(io_module_get_state_string(IO_STATE_FAULT), "FAULT") == 0, "State string FAULT");
    
    // Test fault strings
    test_assert(strcmp(io_module_get_fault_string(IO_FAULT_NONE), "No Fault") == 0, "Fault string NONE");
    test_assert(strcmp(io_module_get_fault_string(IO_FAULT_COMMUNICATION_ERROR), "Communication Error") == 0, "Fault string COMMUNICATION_ERROR");
    test_assert(strcmp(io_module_get_fault_string(IO_FAULT_INVALID_CHANNEL), "Invalid Channel") == 0, "Fault string INVALID_CHANNEL");
    
    // Test edge strings
    test_assert(strcmp(io_module_get_edge_string(IO_EDGE_NONE), "None") == 0, "Edge string NONE");
    test_assert(strcmp(io_module_get_edge_string(IO_EDGE_RISING), "Rising") == 0, "Edge string RISING");
    test_assert(strcmp(io_module_get_edge_string(IO_EDGE_FALLING), "Falling") == 0, "Edge string FALLING");
    test_assert(strcmp(io_module_get_edge_string(IO_EDGE_BOTH), "Both") == 0, "Edge string BOTH");
    
    // Test timestamp
    uint32_t timestamp = io_module_get_timestamp();
    test_assert(timestamp > 0, "Timestamp is valid");
}

static void test_io_module_error_handling() {
    printf("\n=== Testing IO Module Error Handling ===\n");
    
    // Test NULL handler
    hal_status_t status = io_module_init(NULL, NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "NULL handler initialization");
    
    io_module_handler_t handler;
    status = io_module_init(&handler, NULL);
    test_assert(status == HAL_STATUS_OK, "Valid initialization");
    
    // Test operations on uninitialized handler
    io_module_handler_t uninit_handler;
    memset(&uninit_handler, 0, sizeof(io_module_handler_t));
    
    status = io_module_enable(&uninit_handler, true);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Enable uninitialized handler");
    
    status = io_module_deinit(&uninit_handler);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Deinit uninitialized handler");
    
    // Test operations on disabled handler
    status = io_module_enable(&handler, false);
    test_assert(status == HAL_STATUS_OK, "Disable handler");
    
    bool value;
    status = io_module_read_digital_input(&handler, 0, &value);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Read on disabled handler");
    
    status = io_module_write_digital_output(&handler, 0, true);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Write on disabled handler");
    
    status = io_module_deinit(&handler);
    test_assert(status == HAL_STATUS_OK, "IO module deinitialization");
}

// Main test function
int main(int argc, char *argv) {
    printf("=== IO Module Handler Test Suite (FW-30) ===\n");
    printf("Date: 2025-01-27\n");
    printf("Version: 1.0\n\n");
    
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
    
    hal_status_t safety_status = safety_manager_init(&safety_config);
    if (safety_status != HAL_STATUS_OK) {
        printf("❌ Safety manager initialization failed: %d\n", safety_status);
        return 1;
    }
    printf("✅ Safety manager initialized\n");
    
    // Run all tests
    test_io_module_init();
    test_io_module_enable_disable();
    test_io_module_digital_io();
    test_io_module_analog_io();
    test_io_module_debounce();
    test_io_module_edge_detection();
    test_io_module_batch_operations();
    test_io_module_validation();
    test_io_module_status_monitoring();
    test_io_module_diagnostics();
    test_io_module_utility_functions();
    test_io_module_error_handling();
    
    // Print test results
    printf("\n=== Test Results ===\n");
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("Total Tests: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("🎉 All tests passed!\n");
    } else {
        printf("⚠️  Some tests failed!\n");
    }
    
    // Deinitialize safety manager
    safety_manager_deinit();
    printf("✅ Safety manager deinitialized\n");
    
    return (tests_failed == 0) ? 0 : 1;
}
