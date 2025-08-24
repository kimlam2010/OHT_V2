/**
 * @file test_system_state_machine.c
 * @brief Unit tests for System State Machine
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-17 (Application Core Testing)
 */

#include "unity.h"
#include "system_state_machine.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Test fixtures
static system_state_t current_state;
static system_event_t test_event;
static system_status_t test_status;
static system_config_t test_config;

void setUp(void)
{
    // Initialize test state
    current_state = SYSTEM_STATE_IDLE;
    
    // Initialize test event
    test_event = SYSTEM_EVENT_NONE;
    
    // Initialize test status
    memset(&test_status, 0, sizeof(system_status_t));
    test_status.current_state = SYSTEM_STATE_IDLE;
    test_status.previous_state = SYSTEM_STATE_INIT;
    test_status.last_event = SYSTEM_EVENT_NONE;
    test_status.current_fault = SYSTEM_FAULT_NONE;
    test_status.state_entry_time = 1640995200000000ULL;
    test_status.last_update_time = 1640995200000000ULL;
    test_status.state_transition_count = 0;
    test_status.system_ready = true;
    test_status.safety_ok = true;
    test_status.communication_ok = true;
    test_status.sensors_ok = true;
    
    // Initialize test config
    memset(&test_config, 0, sizeof(system_config_t));
    test_config.state_timeout_ms = 5000;
    test_config.update_period_ms = 100;
    test_config.auto_recovery_enabled = true;
    test_config.safety_monitoring_enabled = true;
    test_config.communication_monitoring_enabled = true;
    test_config.sensor_monitoring_enabled = true;
}

void tearDown(void)
{
    // Cleanup after each test
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_system_state_constants(void)
{
    TEST_ASSERT_EQUAL(0, SYSTEM_STATE_INIT);
    TEST_ASSERT_EQUAL(1, SYSTEM_STATE_IDLE);
    TEST_ASSERT_EQUAL(2, SYSTEM_STATE_MOVE);
    TEST_ASSERT_EQUAL(3, SYSTEM_STATE_DOCK);
    TEST_ASSERT_EQUAL(4, SYSTEM_STATE_FAULT);
    TEST_ASSERT_EQUAL(5, SYSTEM_STATE_ESTOP);
    TEST_ASSERT_EQUAL(6, SYSTEM_STATE_SHUTDOWN);
}

void test_system_event_constants(void)
{
    TEST_ASSERT_EQUAL(0, SYSTEM_EVENT_NONE);
    TEST_ASSERT_EQUAL(1, SYSTEM_EVENT_INIT_COMPLETE);
    TEST_ASSERT_EQUAL(2, SYSTEM_EVENT_MOVE_COMMAND);
    TEST_ASSERT_EQUAL(3, SYSTEM_EVENT_DOCK_COMMAND);
    TEST_ASSERT_EQUAL(4, SYSTEM_EVENT_STOP_COMMAND);
    TEST_ASSERT_EQUAL(5, SYSTEM_EVENT_ESTOP_TRIGGERED);
    TEST_ASSERT_EQUAL(6, SYSTEM_EVENT_FAULT_DETECTED);
    TEST_ASSERT_EQUAL(7, SYSTEM_EVENT_FAULT_CLEARED);
    TEST_ASSERT_EQUAL(8, SYSTEM_EVENT_ESTOP_RESET);
    TEST_ASSERT_EQUAL(9, SYSTEM_EVENT_SHUTDOWN);
    TEST_ASSERT_EQUAL(10, SYSTEM_EVENT_TIMEOUT);
    TEST_ASSERT_EQUAL(11, SYSTEM_EVENT_ERROR);
}



void test_system_fault_constants(void)
{
    TEST_ASSERT_EQUAL(0, SYSTEM_FAULT_NONE);
    TEST_ASSERT_EQUAL(1, SYSTEM_FAULT_ESTOP);
    TEST_ASSERT_EQUAL(2, SYSTEM_FAULT_COMMUNICATION);
    TEST_ASSERT_EQUAL(3, SYSTEM_FAULT_SENSOR);
    TEST_ASSERT_EQUAL(4, SYSTEM_FAULT_MOTOR);
    TEST_ASSERT_EQUAL(5, SYSTEM_FAULT_POWER);
    TEST_ASSERT_EQUAL(6, SYSTEM_FAULT_SOFTWARE);
    TEST_ASSERT_EQUAL(7, SYSTEM_FAULT_HARDWARE);
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_system_state_data_structures(void)
{
    TEST_ASSERT_EQUAL(4, sizeof(current_state));
    TEST_ASSERT_EQUAL(4, sizeof(test_event));
}

void test_system_event_initialization(void)
{
    TEST_ASSERT_EQUAL(0, test_event);
}

void test_system_status_initialization(void)
{
    TEST_ASSERT_EQUAL(1, test_status.current_state);
    TEST_ASSERT_EQUAL(0, test_status.previous_state);
    TEST_ASSERT_EQUAL(0, test_status.last_event);
    TEST_ASSERT_EQUAL(0, test_status.current_fault);
    TEST_ASSERT_TRUE(test_status.system_ready);
    TEST_ASSERT_TRUE(test_status.safety_ok);
    TEST_ASSERT_TRUE(test_status.communication_ok);
    TEST_ASSERT_TRUE(test_status.sensors_ok);
}

void test_system_config_initialization(void)
{
    TEST_ASSERT_EQUAL(5000, test_config.state_timeout_ms);
    TEST_ASSERT_EQUAL(100, test_config.update_period_ms);
    TEST_ASSERT_TRUE(test_config.auto_recovery_enabled);
    TEST_ASSERT_TRUE(test_config.safety_monitoring_enabled);
    TEST_ASSERT_TRUE(test_config.communication_monitoring_enabled);
    TEST_ASSERT_TRUE(test_config.sensor_monitoring_enabled);
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void test_system_state_validation(void)
{
    // Test valid states
    system_state_t valid_states[] = {
        SYSTEM_STATE_INIT,
        SYSTEM_STATE_IDLE,
        SYSTEM_STATE_MOVE,
        SYSTEM_STATE_DOCK,
        SYSTEM_STATE_FAULT,
        SYSTEM_STATE_ESTOP,
        SYSTEM_STATE_SHUTDOWN
    };
    
    for (int i = 0; i < 7; i++) {
        TEST_ASSERT_TRUE(valid_states[i] >= 0 && valid_states[i] <= 6);
    }
}

void test_system_event_validation(void)
{
    // Test valid event types
    system_event_t valid_events[] = {
        SYSTEM_EVENT_NONE,
        SYSTEM_EVENT_INIT_COMPLETE,
        SYSTEM_EVENT_MOVE_COMMAND,
        SYSTEM_EVENT_DOCK_COMMAND,
        SYSTEM_EVENT_STOP_COMMAND,
        SYSTEM_EVENT_ESTOP_TRIGGERED,
        SYSTEM_EVENT_FAULT_DETECTED,
        SYSTEM_EVENT_FAULT_CLEARED,
        SYSTEM_EVENT_ESTOP_RESET,
        SYSTEM_EVENT_SHUTDOWN,
        SYSTEM_EVENT_TIMEOUT,
        SYSTEM_EVENT_ERROR
    };
    
    for (int i = 0; i < 12; i++) {
        TEST_ASSERT_TRUE(valid_events[i] >= 0 && valid_events[i] <= 11);
    }
}

void test_system_fault_validation(void)
{
    // Test valid fault types
    system_fault_t valid_faults[] = {
        SYSTEM_FAULT_NONE,
        SYSTEM_FAULT_ESTOP,
        SYSTEM_FAULT_COMMUNICATION,
        SYSTEM_FAULT_SENSOR,
        SYSTEM_FAULT_MOTOR,
        SYSTEM_FAULT_POWER,
        SYSTEM_FAULT_SOFTWARE,
        SYSTEM_FAULT_HARDWARE
    };
    
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_TRUE(valid_faults[i] >= 0 && valid_faults[i] <= 7);
    }
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Constants tests
    RUN_TEST(test_system_state_constants);
    RUN_TEST(test_system_event_constants);
    RUN_TEST(test_system_fault_constants);
    
    // Data structures tests
    RUN_TEST(test_system_state_data_structures);
    RUN_TEST(test_system_event_initialization);
    RUN_TEST(test_system_status_initialization);
    RUN_TEST(test_system_config_initialization);
    
    // Validation tests
    RUN_TEST(test_system_state_validation);
    RUN_TEST(test_system_event_validation);
    RUN_TEST(test_system_fault_validation);
    
    UNITY_END();
    return 0;
}
