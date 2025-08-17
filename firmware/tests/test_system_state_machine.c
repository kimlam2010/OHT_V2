/**
 * @file test_system_state_machine.c
 * @brief Test program for System State Machine
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-01 (System State Machine Testing)
 */

#include "system_state_machine.h"
#include "hal_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Test callback function
static void test_event_callback(system_state_t state, system_event_t event) {
    printf("Event Callback: State=%s, Event=%s\n", 
           system_state_machine_get_state_name(state),
           system_state_machine_get_event_name(event));
}

// Test function prototypes
static hal_status_t test_initialization(void);
static hal_status_t test_state_transitions(void);
static hal_status_t test_safety_events(void);
static hal_status_t test_fault_handling(void);
static hal_status_t test_led_patterns(void);
static hal_status_t test_diagnostics(void);

int main(int argc, char *argv[]) {
    printf("=== OHT-50 System State Machine Test ===\n");
    printf("Version: %s\n", HAL_VERSION_STRING);
    printf("Date: %s\n", __DATE__);
    printf("Team: FW\n");
    printf("Task: FW-01 (System State Machine Testing)\n\n");
    
    hal_status_t status = HAL_STATUS_OK;
    int test_count = 0;
    int pass_count = 0;
    
    // Test 1: Initialization
    printf("Test 1: Initialization\n");
    printf("----------------------\n");
    test_count++;
    status = test_initialization();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: Initialization test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: Initialization test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 2: State Transitions
    printf("Test 2: State Transitions\n");
    printf("-------------------------\n");
    test_count++;
    status = test_state_transitions();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: State transitions test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: State transitions test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 3: Safety Events
    printf("Test 3: Safety Events\n");
    printf("---------------------\n");
    test_count++;
    status = test_safety_events();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: Safety events test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: Safety events test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 4: Fault Handling
    printf("Test 4: Fault Handling\n");
    printf("----------------------\n");
    test_count++;
    status = test_fault_handling();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: Fault handling test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: Fault handling test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 5: LED Patterns
    printf("Test 5: LED Patterns\n");
    printf("-------------------\n");
    test_count++;
    status = test_led_patterns();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: LED patterns test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: LED patterns test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 6: Diagnostics
    printf("Test 6: Diagnostics\n");
    printf("-------------------\n");
    test_count++;
    status = test_diagnostics();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: Diagnostics test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: Diagnostics test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test Summary
    printf("=== Test Summary ===\n");
    printf("Total Tests: %d\n", test_count);
    printf("Passed: %d\n", pass_count);
    printf("Failed: %d\n", test_count - pass_count);
    printf("Success Rate: %.1f%%\n", (float)pass_count / test_count * 100.0f);
    
    if (pass_count == test_count) {
        printf("✓ ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("✗ SOME TESTS FAILED!\n");
        return 1;
    }
}

static hal_status_t test_initialization(void) {
    // Test configuration
    system_config_t config = {
        .state_timeout_ms = 5000,
        .update_period_ms = 100,
        .auto_recovery_enabled = true,
        .safety_monitoring_enabled = true,
        .communication_monitoring_enabled = true,
        .sensor_monitoring_enabled = true
    };
    
    // Initialize state machine
    hal_status_t status = system_state_machine_init(&config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check initial state
    system_state_t state;
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_INIT) {
        printf("Expected initial state INIT, got %s\n", 
               system_state_machine_get_state_name(state));
        return HAL_STATUS_ERROR;
    }
    
    // Check system ready status
    bool ready;
    status = system_state_machine_is_ready(&ready);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (ready) {
        printf("System should not be ready in INIT state\n");
        return HAL_STATUS_ERROR;
    }
    
    printf("Initialization successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_state_transitions(void) {
    // Test transition from INIT to IDLE
    hal_status_t status = system_state_machine_process_event(SYSTEM_EVENT_INIT_COMPLETE);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check state transition
    system_state_t state;
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_IDLE) {
        printf("Expected state IDLE after INIT_COMPLETE, got %s\n", 
               system_state_machine_get_state_name(state));
        return HAL_STATUS_ERROR;
    }
    
    // Check system ready status
    bool ready;
    status = system_state_machine_is_ready(&ready);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (!ready) {
        printf("System should be ready in IDLE state\n");
        return HAL_STATUS_ERROR;
    }
    
    // Test transition from IDLE to MOVE
    status = system_state_machine_process_event(SYSTEM_EVENT_MOVE_COMMAND);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_MOVE) {
        printf("Expected state MOVE after MOVE_COMMAND, got %s\n", 
               system_state_machine_get_state_name(state));
        return HAL_STATUS_ERROR;
    }
    
    // Test transition from MOVE to IDLE
    status = system_state_machine_process_event(SYSTEM_EVENT_STOP_COMMAND);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_IDLE) {
        printf("Expected state IDLE after STOP_COMMAND, got %s\n", 
               system_state_machine_get_state_name(state));
        return HAL_STATUS_ERROR;
    }
    
    printf("State transitions successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_safety_events(void) {
    // Test E-Stop event
    hal_status_t status = system_state_machine_process_event(SYSTEM_EVENT_ESTOP_TRIGGERED);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check state transition to ESTOP
    system_state_t state;
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_ESTOP) {
        printf("Expected state ESTOP after ESTOP_TRIGGERED, got %s\n", 
               system_state_machine_get_state_name(state));
        return HAL_STATUS_ERROR;
    }
    
    // Check safety status
    bool safe;
    status = system_state_machine_is_safe(&safe);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (safe) {
        printf("System should not be safe in ESTOP state\n");
        return HAL_STATUS_ERROR;
    }
    
    // Test E-Stop reset
    status = system_state_machine_process_event(SYSTEM_EVENT_ESTOP_RESET);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_IDLE) {
        printf("Expected state IDLE after ESTOP_RESET, got %s\n", 
               system_state_machine_get_state_name(state));
        return HAL_STATUS_ERROR;
    }
    
    printf("Safety events successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_fault_handling(void) {
    // Test fault detection
    hal_status_t status = system_state_machine_trigger_fault(SYSTEM_FAULT_COMMUNICATION);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check state transition to FAULT
    system_state_t state;
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_FAULT) {
        printf("Expected state FAULT after fault trigger, got %s\n", 
               system_state_machine_get_state_name(state));
        return HAL_STATUS_ERROR;
    }
    
    // Check fault type
    system_fault_t fault;
    status = system_state_machine_get_fault(&fault);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (fault != SYSTEM_FAULT_COMMUNICATION) {
        printf("Expected fault COMMUNICATION, got %s\n", 
               system_state_machine_get_fault_name(fault));
        return HAL_STATUS_ERROR;
    }
    
    // Test fault clearing
    status = system_state_machine_clear_fault();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_IDLE) {
        printf("Expected state IDLE after fault clear, got %s\n", 
               system_state_machine_get_state_name(state));
        return HAL_STATUS_ERROR;
    }
    
    printf("Fault handling successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_led_patterns(void) {
    // Test LED patterns for different states
    hal_status_t status;
    system_state_t state;
    
    // Test MOVE state LED pattern
    status = system_state_machine_process_event(SYSTEM_EVENT_MOVE_COMMAND);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_MOVE) {
        printf("Failed to enter MOVE state for LED test\n");
        return HAL_STATUS_ERROR;
    }
    
    // Small delay to see LED pattern
    usleep(100000); // 100ms
    
    // Test DOCK state LED pattern
    status = system_state_machine_process_event(SYSTEM_EVENT_DOCK_COMMAND);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = system_state_machine_get_state(&state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (state != SYSTEM_STATE_DOCK) {
        printf("Failed to enter DOCK state for LED test\n");
        return HAL_STATUS_ERROR;
    }
    
    // Small delay to see LED pattern
    usleep(100000); // 100ms
    
    // Return to IDLE
    status = system_state_machine_process_event(SYSTEM_EVENT_STOP_COMMAND);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    printf("LED patterns test successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_diagnostics(void) {
    // Get system diagnostics
    char diagnostics[1024];
    hal_status_t status = system_state_machine_get_diagnostics(diagnostics, sizeof(diagnostics));
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check if diagnostics contain expected information
    if (strstr(diagnostics, "State Machine Diagnostics:") == NULL) {
        printf("Diagnostics missing header\n");
        return HAL_STATUS_ERROR;
    }
    
    if (strstr(diagnostics, "Current State:") == NULL) {
        printf("Diagnostics missing current state\n");
        return HAL_STATUS_ERROR;
    }
    
    if (strstr(diagnostics, "System Ready:") == NULL) {
        printf("Diagnostics missing system ready status\n");
        return HAL_STATUS_ERROR;
    }
    
    printf("Diagnostics:\n%s\n", diagnostics);
    printf("Diagnostics test successful\n");
    return HAL_STATUS_OK;
}
