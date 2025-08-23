/**
 * @file test_motor_module.c
 * @brief Test suite for Motor Module Handler
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-29 (Motor Module Implementation)
 */

#include "motor_module_handler.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test configuration
static const motor_module_config_t test_config = {
    .address = 0x03,
    .command_timeout_ms = 1000,
    .response_timeout_ms = 500,
    .default_velocity = 1000,
    .default_acceleration = 500,
    .default_jerk = 100,
    .enable_safety_checks = true,
    .enable_position_limits = true,
    .enable_velocity_limits = true,
    .enable_acceleration_limits = true
};

// Test event callback
static void test_motor_event_callback(motor_module_handler_t *handler, motor_event_t event) {
    printf("[TEST] Motor Event: %s\n", motor_module_get_event_name(event));
}

// Test functions
static bool test_motor_initialization(void) {
    printf("\n=== Testing Motor Module Initialization ===\n");
    
    // Test 1: Initialize with NULL config (should use defaults)
    motor_module_handler_t handler1;
    hal_status_t status = motor_module_init(&handler1, NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: motor_module_init(NULL) returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: motor_module_init(NULL)\n");
    
    // Test 2: Try to initialize again (should return ALREADY_INITIALIZED)
    status = motor_module_init(&handler1, NULL);
    if (status != HAL_STATUS_ALREADY_INITIALIZED) {
        printf("❌ Test 2 failed: motor_module_init(NULL) returned %d, expected ALREADY_INITIALIZED\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_init(NULL) - already initialized\n");
    
    // Test 3: Deinitialize
    status = motor_module_deinit(&handler1);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: motor_module_deinit() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: motor_module_deinit()\n");
    
    // Test 4: Initialize with custom config
    motor_module_handler_t handler2;
    status = motor_module_init(&handler2, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: motor_module_init(&test_config) returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: motor_module_init(&test_config)\n");
    
    // Test 5: Verify configuration
    if (handler2.address != test_config.address) {
        printf("❌ Test 5 failed: Address mismatch\n");
        return false;
    }
    if (handler2.command_timeout_ms != test_config.command_timeout_ms) {
        printf("❌ Test 5 failed: Command timeout mismatch\n");
        return false;
    }
    printf("✅ Test 5 passed: Configuration verified\n");
    
    motor_module_deinit(&handler2);
    return true;
}

static bool test_motor_enable_disable(void) {
    printf("\n=== Testing Motor Enable/Disable ===\n");
    
    // Test 1: Initialize motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 2: Enable motor
    status = motor_module_enable(&handler, true);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: motor_module_enable(true) returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_enable(true)\n");
    
    // Test 3: Check if enabled
    if (!motor_module_is_enabled(&handler)) {
        printf("❌ Test 3 failed: Motor should be enabled\n");
        return false;
    }
    printf("✅ Test 3 passed: motor_module_is_enabled() - enabled\n");
    
    // Test 4: Disable motor
    status = motor_module_enable(&handler, false);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: motor_module_enable(false) returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: motor_module_enable(false)\n");
    
    // Test 5: Check if disabled
    if (motor_module_is_enabled(&handler)) {
        printf("❌ Test 5 failed: Motor should be disabled\n");
        return false;
    }
    printf("✅ Test 5 passed: motor_module_is_enabled() - disabled\n");
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_movement_commands(void) {
    printf("\n=== Testing Motor Movement Commands ===\n");
    
    // Test 1: Initialize and enable motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    status = motor_module_enable(&handler, true);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not enable motor\n");
        return false;
    }
    
    // Test 2: Move to position
    status = motor_module_move_to_position(&handler, 1000, 500, 200);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: motor_module_move_to_position() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_move_to_position()\n");
    
    // Test 3: Check if moving
    if (!motor_module_is_moving(&handler)) {
        printf("❌ Test 3 failed: Motor should be moving\n");
        return false;
    }
    printf("✅ Test 3 passed: motor_module_is_moving() - moving\n");
    
    // Test 4: Stop motor
    status = motor_module_stop(&handler);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: motor_module_stop() returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: motor_module_stop()\n");
    
    // Test 5: Emergency stop
    status = motor_module_emergency_stop(&handler);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 5 failed: motor_module_emergency_stop() returned %d\n", status);
        return false;
    }
    printf("✅ Test 5 passed: motor_module_emergency_stop()\n");
    
    // Test 6: Hard stop
    status = motor_module_hard_stop(&handler);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 6 failed: motor_module_hard_stop() returned %d\n", status);
        return false;
    }
    printf("✅ Test 6 passed: motor_module_hard_stop()\n");
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_validation(void) {
    printf("\n=== Testing Motor Validation ===\n");
    
    // Test 1: Initialize motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 2: Validate position
    bool valid = motor_module_validate_position(&handler, 1000);
    if (!valid) {
        printf("❌ Test 2 failed: Position 1000 should be valid\n");
        return false;
    }
    printf("✅ Test 2 passed: motor_module_validate_position() - valid position\n");
    
    // Test 3: Validate invalid position
    valid = motor_module_validate_position(&handler, 70000);
    if (valid) {
        printf("❌ Test 3 failed: Position 70000 should be invalid\n");
        return false;
    }
    printf("✅ Test 3 passed: motor_module_validate_position() - invalid position\n");
    
    // Test 4: Validate velocity
    valid = motor_module_validate_velocity(&handler, 1000);
    if (!valid) {
        printf("❌ Test 4 failed: Velocity 1000 should be valid\n");
        return false;
    }
    printf("✅ Test 4 passed: motor_module_validate_velocity() - valid velocity\n");
    
    // Test 5: Validate invalid velocity
    valid = motor_module_validate_velocity(&handler, 0);
    if (valid) {
        printf("❌ Test 5 failed: Velocity 0 should be invalid\n");
        return false;
    }
    printf("✅ Test 5 passed: motor_module_validate_velocity() - invalid velocity\n");
    
    // Test 6: Validate acceleration
    valid = motor_module_validate_acceleration(&handler, 500);
    if (!valid) {
        printf("❌ Test 6 failed: Acceleration 500 should be valid\n");
        return false;
    }
    printf("✅ Test 6 passed: motor_module_validate_acceleration() - valid acceleration\n");
    
    // Test 7: Validate invalid acceleration
    valid = motor_module_validate_acceleration(&handler, 0);
    if (valid) {
        printf("❌ Test 7 failed: Acceleration 0 should be invalid\n");
        return false;
    }
    printf("✅ Test 7 passed: motor_module_validate_acceleration() - invalid acceleration\n");
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_data_access(void) {
    printf("\n=== Testing Motor Data Access ===\n");
    
    // Test 1: Initialize motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 2: Set position target
    status = motor_module_set_position_target(&handler, 2000);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: motor_module_set_position_target() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_set_position_target()\n");
    
    // Test 3: Set velocity target
    status = motor_module_set_velocity_target(&handler, 800);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: motor_module_set_velocity_target() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: motor_module_set_velocity_target()\n");
    
    // Test 4: Set acceleration limit
    status = motor_module_set_acceleration_limit(&handler, 300);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: motor_module_set_acceleration_limit() returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: motor_module_set_acceleration_limit()\n");
    
    // Test 5: Get motor data
    motor_module_data_t data;
    status = motor_module_get_data(&handler, &data);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 5 failed: motor_module_get_data() returned %d\n", status);
        return false;
    }
    
    if (data.position_target != 2000) {
        printf("❌ Test 5 failed: Position target mismatch\n");
        return false;
    }
    if (data.velocity_target != 800) {
        printf("❌ Test 5 failed: Velocity target mismatch\n");
        return false;
    }
    if (data.acceleration_limit != 300) {
        printf("❌ Test 5 failed: Acceleration limit mismatch\n");
        return false;
    }
    printf("✅ Test 5 passed: motor_module_get_data()\n");
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_status_monitoring(void) {
    printf("\n=== Testing Motor Status Monitoring ===\n");
    
    // Test 1: Initialize motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 2: Get status
    motor_state_t state;
    motor_fault_code_t fault_code;
    status = motor_module_get_status(&handler, &state, &fault_code);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: motor_module_get_status() returned %d\n", status);
        return false;
    }
    
    if (state != MOTOR_STATE_DISABLED) {
        printf("❌ Test 2 failed: Expected DISABLED state\n");
        return false;
    }
    if (fault_code != MOTOR_FAULT_NONE) {
        printf("❌ Test 2 failed: Expected NO_FAULT\n");
        return false;
    }
    printf("✅ Test 2 passed: motor_module_get_status()\n");
    
    // Test 3: Check if has faults
    bool has_faults = motor_module_has_faults(&handler);
    if (has_faults) {
        printf("❌ Test 3 failed: Motor should not have faults\n");
        return false;
    }
    printf("✅ Test 3 passed: motor_module_has_faults() - no faults\n");
    
    // Test 4: Get fault description
    const char* fault_desc = motor_module_get_fault_description(&handler);
    if (fault_desc == NULL) {
        printf("❌ Test 4 failed: Fault description is NULL\n");
        return false;
    }
    printf("✅ Test 4 passed: motor_module_get_fault_description()\n");
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_safety_integration(void) {
    printf("\n=== Testing Motor Safety Integration ===\n");
    
    // Test 1: Initialize motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 2: Check safety
    status = motor_module_check_safety(&handler);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: motor_module_check_safety() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_check_safety()\n");
    
    // Test 3: Enable motor (should check safety)
    status = motor_module_enable(&handler, true);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: motor_module_enable() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: motor_module_enable() with safety check\n");
    
    // Test 4: Move to position (should check safety)
    status = motor_module_move_to_position(&handler, 1000, 500, 200);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: motor_module_move_to_position() returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: motor_module_move_to_position() with safety check\n");
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_fault_detection(void) {
    printf("\n=== Testing Motor Fault Detection ===\n");
    
    // Test 1: Initialize motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 2: Reset faults
    status = motor_module_reset_faults(&handler);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: motor_module_reset_faults() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_reset_faults()\n");
    
    // Test 3: Check fault status
    bool has_faults = motor_module_has_faults(&handler);
    if (has_faults) {
        printf("❌ Test 3 failed: Motor should not have faults after reset\n");
        return false;
    }
    printf("✅ Test 3 passed: motor_module_has_faults() - no faults after reset\n");
    
    // Test 4: Get fault name
    const char* fault_name = motor_module_get_fault_name(MOTOR_FAULT_OVERCURRENT);
    if (fault_name == NULL) {
        printf("❌ Test 4 failed: Fault name is NULL\n");
        return false;
    }
    printf("✅ Test 4 passed: motor_module_get_fault_name() - %s\n", fault_name);
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_modbus_communication(void) {
    printf("\n=== Testing Motor Modbus Communication ===\n");
    
    // Test 1: Initialize motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 2: Write register
    status = motor_module_write_register(&handler, MOTOR_POSITION_TARGET_REG, 1500);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: motor_module_write_register() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_write_register()\n");
    
    // Test 3: Read register
    uint16_t value;
    status = motor_module_read_register(&handler, MOTOR_POSITION_TARGET_REG, &value);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: motor_module_read_register() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: motor_module_read_register() - value: %u\n", value);
    
    // Test 4: Write multiple registers
    uint16_t data[2] = {2000, 1000};
    status = motor_module_write_registers(&handler, MOTOR_POSITION_TARGET_REG, 2, data);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: motor_module_write_registers() returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: motor_module_write_registers()\n");
    
    // Test 5: Read multiple registers
    uint16_t read_data[2];
    status = motor_module_read_registers(&handler, MOTOR_POSITION_TARGET_REG, 2, read_data);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 5 failed: motor_module_read_registers() returned %d\n", status);
        return false;
    }
    printf("✅ Test 5 passed: motor_module_read_registers() - values: %u, %u\n", read_data[0], read_data[1]);
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_operations(void) {
    printf("\n=== Testing Motor Operations ===\n");
    
    // Test 1: Initialize motor
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 2: Set event callback
    status = motor_module_set_callback(&handler, test_motor_event_callback);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: motor_module_set_callback() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_set_callback()\n");
    
    // Test 3: Self-test
    status = motor_module_self_test(&handler);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: motor_module_self_test() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: motor_module_self_test()\n");
    
    // Test 4: Get diagnostics
    char diagnostics[1024];
    status = motor_module_get_diagnostics(&handler, diagnostics, sizeof(diagnostics));
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: motor_module_get_diagnostics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: motor_module_get_diagnostics()\n");
    printf("Diagnostics:\n%s\n", diagnostics);
    
    // Test 5: Reset statistics
    status = motor_module_reset_statistics(&handler);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 5 failed: motor_module_reset_statistics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 5 passed: motor_module_reset_statistics()\n");
    
    // Test 6: Get statistics
    uint32_t total_moves, successful_moves, failed_moves;
    uint64_t total_runtime;
    status = motor_module_get_statistics(&handler, &total_moves, &successful_moves, &failed_moves, &total_runtime);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 6 failed: motor_module_get_statistics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 6 passed: motor_module_get_statistics()\n");
    
    motor_module_deinit(&handler);
    return true;
}

static bool test_motor_error_handling(void) {
    printf("\n=== Testing Motor Error Handling ===\n");
    
    // Test 1: Test with NULL handler
    hal_status_t status = motor_module_init(NULL, NULL);
    if (status != HAL_STATUS_INVALID_PARAMETER) {
        printf("❌ Test 1 failed: motor_module_init(NULL) returned %d, expected INVALID_PARAMETER\n", status);
        return false;
    }
    printf("✅ Test 1 passed: motor_module_init(NULL) - invalid parameter\n");
    
    // Test 2: Test without initialization
    motor_module_handler_t handler;
    status = motor_module_enable(&handler, true);
    if (status != HAL_STATUS_NOT_INITIALIZED) {
        printf("❌ Test 2 failed: motor_module_enable() returned %d, expected NOT_INITIALIZED\n", status);
        return false;
    }
    printf("✅ Test 2 passed: motor_module_enable() - not initialized\n");
    
    // Test 3: Initialize and test invalid parameters
    status = motor_module_init(&handler, &test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: Could not initialize motor\n");
        return false;
    }
    
    // Test 4: Test invalid position
    status = motor_module_move_to_position(&handler, 70000, 500, 200);
    if (status != HAL_STATUS_INVALID_PARAMETER) {
        printf("❌ Test 4 failed: motor_module_move_to_position() returned %d, expected INVALID_PARAMETER\n", status);
        return false;
    }
    printf("✅ Test 4 passed: motor_module_move_to_position() - invalid position\n");
    
    motor_module_deinit(&handler);
    return true;
}

int main(int argc, char *argv) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;  // Suppress unused parameter warning
    
    printf("=== Motor Module Handler Test Suite ===\n");
    printf("Version: 1.0.0\n");
    printf("Date: 2025-01-27\n");
    printf("Team: FW\n");
    printf("Task: FW-29 (Motor Module Implementation)\n\n");
    
    bool all_tests_passed = true;
    
    // Run test suites
    if (!test_motor_initialization()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_enable_disable()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_movement_commands()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_validation()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_data_access()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_status_monitoring()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_safety_integration()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_fault_detection()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_modbus_communication()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_operations()) {
        all_tests_passed = false;
    }
    
    if (!test_motor_error_handling()) {
        all_tests_passed = false;
    }
    
    // Summary
    printf("\n=== Test Summary ===\n");
    if (all_tests_passed) {
        printf("✅ All tests passed!\n");
        printf("Motor Module Handler is working correctly.\n");
        return 0;
    } else {
        printf("❌ Some tests failed!\n");
        printf("Please review the failed tests above.\n");
        return 1;
    }
}
