#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>

#include "system_state_machine.h"

// Test fixtures
static int setup_state_machine(void **state) {
    system_config_t config = {
        .state_timeout_ms = 5000,
        .update_period_ms = 100,
        .auto_recovery_enabled = true,
        .safety_monitoring_enabled = true,
        .communication_monitoring_enabled = true,
        .sensor_monitoring_enabled = true
    };
    
    assert_int_equal(system_state_machine_init(&config), HAL_STATUS_OK);
    return 0;
}

static int teardown_state_machine(void **state) {
    system_state_machine_deinit();
    return 0;
}

// Test basic state transitions with new states
static void test_basic_state_transitions(void **state) {
    system_state_t current_state;
    
    // Test INIT -> IDLE (should already be done by init)
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_IDLE);
    
    // Test IDLE -> MOVE
    assert_int_equal(system_state_machine_enter_move(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_MOVE);
    
    // Test MOVE -> PAUSED (new functionality)
    assert_int_equal(system_state_machine_enter_paused(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_PAUSED);
    
    // Test PAUSED -> MOVE (resume functionality)
    assert_int_equal(system_state_machine_resume_from_pause(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_MOVE);
}

// Test new state transitions
static void test_new_state_transitions(void **state) {
    system_state_t current_state;
    
    // Test BOOT -> INIT transition
    assert_int_equal(system_state_machine_process_event(SYSTEM_EVENT_BOOT_COMPLETE), HAL_STATUS_OK);
    
    // Test IDLE -> CONFIG
    assert_int_equal(system_state_machine_enter_config(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_CONFIG);
    
    // Test CONFIG -> IDLE (config complete)
    assert_int_equal(system_state_machine_process_event(SYSTEM_EVENT_CONFIG_COMPLETE), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_IDLE);
}

// Test emergency transitions with new states
static void test_emergency_transitions(void **state) {
    system_state_t current_state;
    
    // From CONFIG state, E-Stop should work
    assert_int_equal(system_state_machine_enter_config(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_enter_estop(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_ESTOP);
    
    // Test ESTOP -> SAFE transition
    assert_int_equal(system_state_machine_enter_safe(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_SAFE);
    
    // Test SAFE -> IDLE transition
    assert_int_equal(system_state_machine_process_event(SYSTEM_EVENT_SAFE_RESET), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_IDLE);
}

// Test state timeout functionality
static void test_state_timeout(void **state) {
    hal_status_t status;
    
    // Test setting timeout for a state
    status = system_state_machine_set_state_timeout(SYSTEM_STATE_CONFIG, 5000);
    assert_int_equal(status, HAL_STATUS_OK);
    
    // Test invalid state
    status = system_state_machine_set_state_timeout(SYSTEM_STATE_MAX, 5000);
    assert_int_equal(status, HAL_STATUS_INVALID_PARAMETER);
}

// Test state statistics
static void test_state_statistics(void **state) {
    system_state_statistics_t stats;
    hal_status_t status;
    
    // Get initial statistics
    status = system_state_machine_get_state_statistics(&stats);
    assert_int_equal(status, HAL_STATUS_OK);
    
    // Statistics should be initialized
    assert_int_equal(stats.total_transitions, 0);
    assert_int_equal(stats.emergency_count, 0);
    
    // Test with NULL pointer
    status = system_state_machine_get_state_statistics(NULL);
    assert_int_equal(status, HAL_STATUS_INVALID_PARAMETER);
}

// Test pause/resume functionality
static void test_pause_resume(void **state) {
    system_state_t current_state;
    
    // Start in MOVE state
    assert_int_equal(system_state_machine_enter_move(), HAL_STATUS_OK);
    
    // Pause
    assert_int_equal(system_state_machine_enter_paused(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_PAUSED);
    
    // Resume
    assert_int_equal(system_state_machine_resume_from_pause(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_MOVE);
    
    // Test resume from non-paused state (should fail)
    assert_int_equal(system_state_machine_process_event(SYSTEM_EVENT_STOP_COMMAND), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_IDLE);
    
    hal_status_t status = system_state_machine_resume_from_pause();
    assert_int_equal(status, HAL_STATUS_INVALID_STATE);
}

// Test state name functions with new states
static void test_state_names(void **state) {
    const char* name;
    
    // Test new state names
    name = system_state_machine_get_state_name(SYSTEM_STATE_BOOT);
    assert_string_equal(name, "BOOT");
    
    name = system_state_machine_get_state_name(SYSTEM_STATE_PAUSED);
    assert_string_equal(name, "PAUSED");
    
    name = system_state_machine_get_state_name(SYSTEM_STATE_CONFIG);
    assert_string_equal(name, "CONFIG");
    
    name = system_state_machine_get_state_name(SYSTEM_STATE_SAFE);
    assert_string_equal(name, "SAFE");
    
    // Test unknown state
    name = system_state_machine_get_state_name(SYSTEM_STATE_MAX);
    assert_string_equal(name, "UNKNOWN");
}

// Test event name functions with new events
static void test_event_names(void **state) {
    const char* name;
    
    // Test new event names
    name = system_state_machine_get_event_name(SYSTEM_EVENT_BOOT_COMPLETE);
    assert_string_equal(name, "BOOT_COMPLETE");
    
    name = system_state_machine_get_event_name(SYSTEM_EVENT_PAUSE_COMMAND);
    assert_string_equal(name, "PAUSE_COMMAND");
    
    name = system_state_machine_get_event_name(SYSTEM_EVENT_RESUME_COMMAND);
    assert_string_equal(name, "RESUME_COMMAND");
    
    name = system_state_machine_get_event_name(SYSTEM_EVENT_CONFIG_COMMAND);
    assert_string_equal(name, "CONFIG_COMMAND");
    
    name = system_state_machine_get_event_name(SYSTEM_EVENT_CONFIG_COMPLETE);
    assert_string_equal(name, "CONFIG_COMPLETE");
    
    name = system_state_machine_get_event_name(SYSTEM_EVENT_CONFIG_FAILED);
    assert_string_equal(name, "CONFIG_FAILED");
    
    name = system_state_machine_get_event_name(SYSTEM_EVENT_SAFE_RESET);
    assert_string_equal(name, "SAFE_RESET");
}

// Test configuration state workflow
static void test_config_workflow(void **state) {
    system_state_t current_state;
    
    // Enter config from IDLE
    assert_int_equal(system_state_machine_enter_config(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_CONFIG);
    
    // Test config success
    assert_int_equal(system_state_machine_process_event(SYSTEM_EVENT_CONFIG_COMPLETE), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_IDLE);
    
    // Test config failure
    assert_int_equal(system_state_machine_enter_config(), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_process_event(SYSTEM_EVENT_CONFIG_FAILED), HAL_STATUS_OK);
    assert_int_equal(system_state_machine_get_state(&current_state), HAL_STATUS_OK);
    assert_int_equal(current_state, SYSTEM_STATE_FAULT);
}

// Test main function
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(test_basic_state_transitions, setup_state_machine, teardown_state_machine),
        cmocka_unit_test_setup_teardown(test_new_state_transitions, setup_state_machine, teardown_state_machine),
        cmocka_unit_test_setup_teardown(test_emergency_transitions, setup_state_machine, teardown_state_machine),
        cmocka_unit_test_setup_teardown(test_state_timeout, setup_state_machine, teardown_state_machine),
        cmocka_unit_test_setup_teardown(test_state_statistics, setup_state_machine, teardown_state_machine),
        cmocka_unit_test_setup_teardown(test_pause_resume, setup_state_machine, teardown_state_machine),
        cmocka_unit_test_setup_teardown(test_state_names, setup_state_machine, teardown_state_machine),
        cmocka_unit_test_setup_teardown(test_event_names, setup_state_machine, teardown_state_machine),
        cmocka_unit_test_setup_teardown(test_config_workflow, setup_state_machine, teardown_state_machine),
    };
    
    return cmocka_run_group_tests(tests, NULL, NULL);
}
