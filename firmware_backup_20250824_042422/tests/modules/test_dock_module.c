#include "dock_module_handler.h"
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

static void test_dock_module_init(void) {
    printf("\n=== Testing Dock Module Initialization ===\n");

    dock_module_handler_t handler;

    // Test initialization with null pointer check
    hal_status_t result = dock_module_init(NULL, DOCK_DEFAULT_ADDRESS);
    test_assert(result == HAL_STATUS_ERROR, "Null pointer initialization should fail");

    // Test proper initialization
    result = dock_module_init(&handler, DOCK_DEFAULT_ADDRESS);
    test_assert(result == HAL_STATUS_OK, "Dock module initialization");
    test_assert(handler.initialized == true, "Initialization flag set");
    test_assert(handler.address == DOCK_DEFAULT_ADDRESS, "Handler address set correctly");
    test_assert(handler.enabled == false, "Initial state is disabled");
    test_assert(handler.data.fault_code == DOCK_FAULT_NONE, "Initial fault code is NONE");

    // Test double initialization
    result = dock_module_init(&handler, DOCK_DEFAULT_ADDRESS);
    test_assert(result == HAL_STATUS_OK, "Double initialization check");

    // Test deinitialization
    result = dock_module_deinit(&handler);
    test_assert(result == HAL_STATUS_OK, "Dock module deinitialization");
    test_assert(handler.initialized == false, "Initialization flag cleared");

    printf("Initialization tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_enable(void) {
    printf("\n=== Testing Dock Module Enable/Disable ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);

    // Test enable
    hal_status_t status = dock_module_enable(&handler, true);
    test_assert(status == HAL_STATUS_OK, "Dock module enable");
    test_assert(handler.enabled == true, "Enable flag set");

    // Test disable
    status = dock_module_enable(&handler, false);
    test_assert(status == HAL_STATUS_OK, "Dock module disable");
    test_assert(handler.enabled == false, "Enable flag cleared");

    dock_module_deinit(&handler);
    printf("Enable/Disable tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_docking_control(void) {
    printf("\n=== Testing Docking Control ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);
    dock_module_enable(&handler, true);

    // Test start docking
    hal_status_t status = dock_module_start_docking(&handler, 1000);
    test_assert(status == HAL_STATUS_OK, "Start docking sequence");
    test_assert(handler.data.status == DOCK_STATUS_APPROACHING, "Status changed to APPROACHING");
    test_assert(handler.data.position_target == 1000, "Target position set correctly");

    // Test stop docking
    status = dock_module_stop_docking(&handler);
    test_assert(status == HAL_STATUS_OK, "Stop docking sequence");
    test_assert(handler.data.status == DOCK_STATUS_IDLE, "Status returned to IDLE");

    // Test emergency stop
    status = dock_module_emergency_stop(&handler);
    test_assert(status == HAL_STATUS_OK, "Emergency stop");
    test_assert(handler.data.status == DOCK_STATUS_EMERGENCY_STOP, "Status changed to EMERGENCY_STOP");
    test_assert(handler.emergency_stop_active == true, "Emergency stop flag set");
    test_assert(handler.data.fault_code == DOCK_FAULT_EMERGENCY_STOP, "Fault code set to EMERGENCY_STOP");

    dock_module_deinit(&handler);
    printf("Docking control tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_position_status(void) {
    printf("\n=== Testing Position and Status Functions ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);

    uint16_t position, distance, angle;
    uint8_t status;

    // Test get position
    hal_status_t result = dock_module_get_position(&handler, &position);
    test_assert(result == HAL_STATUS_OK, "Get position");
    test_assert(position == handler.data.current_position, "Position value correct");

    // Test set position
    result = dock_module_set_position(&handler, 1500);
    test_assert(result == HAL_STATUS_OK, "Set position");
    test_assert(handler.data.position_target == 1500, "Target position updated");

    // Test get distance to dock
    result = dock_module_get_distance_to_dock(&handler, &distance);
    test_assert(result == HAL_STATUS_OK, "Get distance to dock");
    test_assert(distance == handler.data.distance_to_dock, "Distance value correct");

    // Test get alignment angle
    result = dock_module_get_alignment_angle(&handler, &angle);
    test_assert(result == HAL_STATUS_OK, "Get alignment angle");
    test_assert(angle == handler.data.alignment_angle, "Alignment angle correct");

    // Test get status
    result = dock_module_get_status(&handler, &status);
    test_assert(result == HAL_STATUS_OK, "Get status");
    test_assert(status == handler.data.status, "Status value correct");

    dock_module_deinit(&handler);
    printf("Position and status tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_configuration(void) {
    printf("\n=== Testing Configuration Functions ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);

    // Test set approach distance
    hal_status_t result = dock_module_set_approach_distance(&handler, 600);
    test_assert(result == HAL_STATUS_OK, "Set approach distance (600mm)");
    test_assert(handler.config.approach_distance == 600, "Approach distance updated");

    // Test set final speed
    result = dock_module_set_final_speed(&handler, 75);
    test_assert(result == HAL_STATUS_OK, "Set final speed (75mm/s)");
    test_assert(handler.config.final_speed == 75, "Final speed updated");

    // Test set accuracy threshold
    result = dock_module_set_accuracy_threshold(&handler, 3);
    test_assert(result == HAL_STATUS_OK, "Set accuracy threshold (3mm)");
    test_assert(handler.config.accuracy_threshold == 3, "Accuracy threshold updated");

    // Test set timeout
    result = dock_module_set_timeout(&handler, 45);
    test_assert(result == HAL_STATUS_OK, "Set timeout (45s)");
    test_assert(handler.config.timeout == 45, "Timeout updated");

    // Test set retry count
    result = dock_module_set_retry_count(&handler, 5);
    test_assert(result == HAL_STATUS_OK, "Set retry count (5)");
    test_assert(handler.config.retry_count == 5, "Retry count updated");

    // Test set debounce time
    result = dock_module_set_debounce_time(&handler, 150);
    test_assert(result == HAL_STATUS_OK, "Set debounce time (150ms)");
    test_assert(handler.config.debounce_time == 150, "Debounce time updated");

    // Test set alignment tolerance
    result = dock_module_set_alignment_tolerance(&handler, 75);
    test_assert(result == HAL_STATUS_OK, "Set alignment tolerance (7.5°)");
    test_assert(handler.config.alignment_tolerance == 75, "Alignment tolerance updated");

    dock_module_deinit(&handler);
    printf("Configuration tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_calibration(void) {
    printf("\n=== Testing Calibration Functions ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);
    dock_module_enable(&handler, true);

    // Test start calibration
    hal_status_t result = dock_module_start_calibration(&handler);
    test_assert(result == HAL_STATUS_OK, "Start calibration");
    test_assert(handler.data.status == DOCK_STATUS_CALIBRATING, "Status changed to CALIBRATING");

    // Test stop calibration
    result = dock_module_stop_calibration(&handler);
    test_assert(result == HAL_STATUS_OK, "Stop calibration");
    test_assert(handler.data.status == DOCK_STATUS_IDLE, "Status returned to IDLE");

    // Test set reference position
    result = dock_module_set_reference_position(&handler, 2000);
    test_assert(result == HAL_STATUS_OK, "Set reference position (2000mm)");

    dock_module_deinit(&handler);
    printf("Calibration tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_fault_diagnostics(void) {
    printf("\n=== Testing Fault and Diagnostic Functions ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);

    uint8_t fault_status, fault_code;
    uint16_t temperature, voltage, current;
    const char *fault_desc;

    // Test get fault status
    hal_status_t result = dock_module_get_fault_status(&handler, &fault_status);
    test_assert(result == HAL_STATUS_OK, "Get fault status");
    test_assert(fault_status == handler.data.fault_status, "Fault status correct");

    // Test get fault code
    result = dock_module_get_fault_code(&handler, &fault_code);
    test_assert(result == HAL_STATUS_OK, "Get fault code");
    test_assert(fault_code == handler.data.fault_code, "Fault code correct");

    // Test get fault description
    fault_desc = dock_module_get_fault_description(&handler);
    test_assert(fault_desc != NULL, "Fault description not null");
    test_assert(strcmp(fault_desc, "No fault") == 0, "Initial fault description correct");

    // Test get temperature
    result = dock_module_get_temperature(&handler, &temperature);
    test_assert(result == HAL_STATUS_OK, "Get temperature");
    test_assert(temperature == handler.data.temperature, "Temperature value correct");

    // Test get voltage
    result = dock_module_get_voltage(&handler, &voltage);
    test_assert(result == HAL_STATUS_OK, "Get voltage");
    test_assert(voltage == handler.data.voltage, "Voltage value correct");

    // Test get current
    result = dock_module_get_current(&handler, &current);
    test_assert(result == HAL_STATUS_OK, "Get current");
    test_assert(current == handler.data.current, "Current value correct");

    // Test reset faults
    result = dock_module_reset_faults(&handler);
    test_assert(result == HAL_STATUS_OK, "Reset faults");
    test_assert(handler.data.fault_status == 0, "Fault status cleared");
    test_assert(handler.data.fault_code == DOCK_FAULT_NONE, "Fault code cleared");

    dock_module_deinit(&handler);
    printf("Fault and diagnostic tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_statistics(void) {
    printf("\n=== Testing Statistics Functions ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);

    uint16_t docking_count, failed_count;
    uint32_t last_time, uptime;

    // Test get docking count
    hal_status_t result = dock_module_get_docking_count(&handler, &docking_count);
    test_assert(result == HAL_STATUS_OK, "Get docking count");
    test_assert(docking_count == handler.data.docking_count, "Docking count correct");

    // Test get failed docking count
    result = dock_module_get_failed_docking_count(&handler, &failed_count);
    test_assert(result == HAL_STATUS_OK, "Get failed docking count");
    test_assert(failed_count == handler.data.failed_docking_count, "Failed docking count correct");

    // Test get last docking time
    result = dock_module_get_last_docking_time(&handler, &last_time);
    test_assert(result == HAL_STATUS_OK, "Get last docking time");
    test_assert(last_time == handler.data.last_docking_time, "Last docking time correct");

    // Test get uptime
    result = dock_module_get_uptime(&handler, &uptime);
    test_assert(result == HAL_STATUS_OK, "Get uptime");
    test_assert(uptime == handler.data.uptime, "Uptime correct");

    dock_module_deinit(&handler);
    printf("Statistics tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_modbus_communication(void) {
    printf("\n=== Testing Modbus Communication ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);

    uint16_t value;
    uint16_t data[4];

    // Test read register
    hal_status_t result = dock_module_read_register(&handler, DOCK_POSITION_TARGET_REG, &value);
    test_assert(result == HAL_STATUS_OK, "Read position target register");

    // Test write register
    result = dock_module_write_register(&handler, DOCK_POSITION_TARGET_REG, 2500);
    test_assert(result == HAL_STATUS_OK, "Write position target register");

    // Test read multiple registers
    result = dock_module_read_registers(&handler, DOCK_POSITION_TARGET_REG, 4, data);
    test_assert(result == HAL_STATUS_OK, "Read multiple registers");

    // Test write multiple registers
    uint16_t write_data[4] = {1000, 2000, 3000, 4000};
    result = dock_module_write_registers(&handler, DOCK_POSITION_TARGET_REG, 4, write_data);
    test_assert(result == HAL_STATUS_OK, "Write multiple registers");

    dock_module_deinit(&handler);
    printf("Modbus communication tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_event_handling(void) {
    printf("\n=== Testing Event Handling ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);

    // Test set event callback
    hal_status_t result = dock_module_set_event_callback(&handler, NULL);
    test_assert(result == HAL_STATUS_OK, "Set event callback");
    test_assert(handler.event_callback == NULL, "Event callback set to NULL");

    // Test trigger event
    result = dock_module_trigger_event(&handler, DOCK_EVENT_DOCKING_STARTED);
    test_assert(result == HAL_STATUS_OK, "Trigger event");

    dock_module_deinit(&handler);
    printf("Event handling tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_diagnostics(void) {
    printf("\n=== Testing Diagnostic Functions ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);

    // Test self test
    hal_status_t result = dock_module_self_test(&handler);
    test_assert(result == HAL_STATUS_OK, "Self test");

    // Test get statistics
    result = dock_module_get_statistics(&handler, NULL);
    test_assert(result == HAL_STATUS_OK, "Get statistics");

    // Test validate config
    result = dock_module_validate_config(&handler);
    test_assert(result == HAL_STATUS_OK, "Validate configuration");

    // Test get info
    result = dock_module_get_info(&handler, NULL);
    test_assert(result == HAL_STATUS_OK, "Get module info");

    dock_module_deinit(&handler);
    printf("Diagnostic tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_error_handling(void) {
    printf("\n=== Testing Error Handling ===\n");

    dock_module_handler_t handler;

    // Test with NULL handler
    hal_status_t result = dock_module_init(NULL, 0x05);
    test_assert(result == HAL_STATUS_ERROR, "Init with NULL handler");

    // Test with uninitialized handler
    result = dock_module_enable(&handler, true);
    test_assert(result == HAL_STATUS_ERROR, "Enable uninitialized handler");

    // Initialize handler
    dock_module_init(&handler, 0x05);

    // Test with NULL parameters
    uint16_t position;
    result = dock_module_get_position(&handler, NULL);
    test_assert(result == HAL_STATUS_ERROR, "Get position with NULL parameter");

    result = dock_module_get_position(NULL, &position);
    test_assert(result == HAL_STATUS_ERROR, "Get position with NULL handler");

    // Test invalid configuration values
    result = dock_module_set_approach_distance(&handler, DOCK_MAX_APPROACH_DISTANCE + 1);
    test_assert(result == HAL_STATUS_ERROR, "Set invalid approach distance");

    result = dock_module_set_final_speed(&handler, DOCK_MAX_FINAL_SPEED + 1);
    test_assert(result == HAL_STATUS_ERROR, "Set invalid final speed");

    result = dock_module_set_accuracy_threshold(&handler, DOCK_MAX_ACCURACY_THRESHOLD + 1);
    test_assert(result == HAL_STATUS_ERROR, "Set invalid accuracy threshold");

    dock_module_deinit(&handler);
    printf("Error handling tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

static void test_dock_module_state_machine(void) {
    printf("\n=== Testing State Machine ===\n");

    dock_module_handler_t handler;
    dock_module_init(&handler, 0x05);
    dock_module_enable(&handler, true);

    // Test state transitions
    hal_status_t result = dock_module_start_docking(&handler, 1000);
    test_assert(result == HAL_STATUS_OK, "Start docking");
    test_assert(handler.data.status == DOCK_STATUS_APPROACHING, "State: APPROACHING");

    // Simulate state machine update
    dock_module_update(&handler);

    // Test undocking
    dock_module_stop_docking(&handler);
    handler.data.status = DOCK_STATUS_DOCKED; // Simulate docked state

    result = dock_module_start_undocking(&handler);
    test_assert(result == HAL_STATUS_OK, "Start undocking");
    test_assert(handler.data.status == DOCK_STATUS_UNDOCKING, "State: UNDOCKING");

    dock_module_deinit(&handler);
    printf("State machine tests completed: %d passed, %d failed\n", tests_passed, tests_failed);
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main(void) {
    printf("=== Dock Module Test Suite ===\n");
    printf("Testing Dock Module Handler (FW-31)\n\n");

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
    test_dock_module_init();
    test_dock_module_enable();
    test_dock_module_docking_control();
    test_dock_module_position_status();
    test_dock_module_configuration();
    test_dock_module_calibration();
    test_dock_module_fault_diagnostics();
    test_dock_module_statistics();
    test_dock_module_modbus_communication();
    test_dock_module_event_handling();
    test_dock_module_diagnostics();
    test_dock_module_error_handling();
    test_dock_module_state_machine();

    // Cleanup
    safety_manager_deinit();

    // Print final results
    printf("\n=== Test Results ===\n");
    printf("Total tests: %d\n", tests_passed + tests_failed);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / (tests_passed + tests_failed) * 100.0f);

    if (tests_failed == 0) {
        printf("\n🎉 All tests passed!\n");
    } else {
        printf("\n⚠️  Some tests failed. Please review the implementation.\n");
    }

    return (tests_failed == 0) ? 0 : 1;
}
