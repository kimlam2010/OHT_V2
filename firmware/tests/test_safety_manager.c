/**
 * @file test_safety_manager.c
 * @brief Test program for Safety Manager
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-02 (Safety Integration Testing)
 */

#include "safety_manager.h"
#include "hal_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Test callback function
static void test_safety_callback(safety_event_t event, safety_fault_t fault) {
    printf("Safety Callback: Event=%s, Fault=%s\n", 
           safety_manager_get_event_name(event),
           safety_manager_get_fault_name(fault));
}

// Test function prototypes
static hal_status_t test_initialization(void);
static hal_status_t test_safety_monitoring(void);
static hal_status_t test_estop_integration(void);
static hal_status_t test_fault_handling(void);
static hal_status_t test_emergency_procedures(void);
static hal_status_t test_safety_recovery(void);
static hal_status_t test_diagnostics(void);

int main(int argc, char *argv[]) {
    printf("=== OHT-50 Safety Manager Test ===\n");
    printf("Version: %s\n", HAL_VERSION_STRING);
    printf("Date: %s\n", __DATE__);
    printf("Team: FW\n");
    printf("Task: FW-02 (Safety Integration Testing)\n\n");
    
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
    
    // Test 2: Safety Monitoring
    printf("Test 2: Safety Monitoring\n");
    printf("-------------------------\n");
    test_count++;
    status = test_safety_monitoring();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: Safety monitoring test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: Safety monitoring test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 3: E-Stop Integration
    printf("Test 3: E-Stop Integration\n");
    printf("-------------------------\n");
    test_count++;
    status = test_estop_integration();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: E-Stop integration test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: E-Stop integration test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 4: Fault Handling
    printf("Test 4: Fault Handling\n");
    printf("---------------------\n");
    test_count++;
    status = test_fault_handling();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: Fault handling test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: Fault handling test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 5: Emergency Procedures
    printf("Test 5: Emergency Procedures\n");
    printf("----------------------------\n");
    test_count++;
    status = test_emergency_procedures();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: Emergency procedures test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: Emergency procedures test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 6: Safety Recovery
    printf("Test 6: Safety Recovery\n");
    printf("----------------------\n");
    test_count++;
    status = test_safety_recovery();
    if (status == HAL_STATUS_OK) {
        printf("✓ PASS: Safety recovery test\n");
        pass_count++;
    } else {
        printf("✗ FAIL: Safety recovery test (status=%d)\n", status);
    }
    printf("\n");
    
    // Test 7: Diagnostics
    printf("Test 7: Diagnostics\n");
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
    safety_config_t config = {
        .safety_check_interval_ms = 1000,
        .estop_response_timeout_ms = 100,
        .safety_circuit_timeout_ms = 500,
        .sensor_timeout_ms = 2000,
        .enable_auto_recovery = true,
        .enable_safety_monitoring = true,
        .enable_estop_monitoring = true,
        .enable_sensor_monitoring = true
    };
    
    // Initialize safety manager
    hal_status_t status = safety_manager_init(&config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check initial status
    safety_status_t status_info;
    status = safety_manager_get_status(&status_info);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (status_info.current_level != SAFETY_LEVEL_NORMAL) {
        printf("Expected initial safety level NORMAL, got %s\n", 
               safety_manager_get_level_name(status_info.current_level));
        return HAL_STATUS_ERROR;
    }
    
    if (status_info.estop_triggered) {
        printf("E-Stop should not be triggered initially\n");
        return HAL_STATUS_ERROR;
    }
    
    if (status_info.current_fault != SAFETY_FAULT_NONE) {
        printf("Expected initial fault NONE, got %s\n", 
               safety_manager_get_fault_name(status_info.current_fault));
        return HAL_STATUS_ERROR;
    }
    
    printf("Initialization successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_safety_monitoring(void) {
    // Test safety monitoring
    hal_status_t status = safety_manager_update();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check if system is safe
    bool safe;
    status = safety_manager_is_safe(&safe);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (!safe) {
        printf("System should be safe initially\n");
        return HAL_STATUS_ERROR;
    }
    
    // Test safety circuit validation
    status = safety_manager_validate_safety_circuit();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Test sensor validation
    status = safety_manager_validate_sensors();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    printf("Safety monitoring successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_estop_integration(void) {
    // Test E-Stop trigger
    hal_status_t status = safety_manager_handle_estop_trigger();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check E-Stop status
    bool triggered;
    status = safety_manager_check_estop(&triggered);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (!triggered) {
        printf("E-Stop should be triggered\n");
        return HAL_STATUS_ERROR;
    }
    
    // Check safety level
    safety_status_t status_info;
    status = safety_manager_get_status(&status_info);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (status_info.current_level != SAFETY_LEVEL_EMERGENCY) {
        printf("Expected safety level EMERGENCY after E-Stop, got %s\n", 
               safety_manager_get_level_name(status_info.current_level));
        return HAL_STATUS_ERROR;
    }
    
    // Test E-Stop reset
    status = safety_manager_handle_estop_reset();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = safety_manager_check_estop(&triggered);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (triggered) {
        printf("E-Stop should be reset\n");
        return HAL_STATUS_ERROR;
    }
    
    printf("E-Stop integration successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_fault_handling(void) {
    // Test fault triggering
    hal_status_t status = safety_manager_trigger_fault(SAFETY_FAULT_SENSOR_FAILURE);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check fault status
    safety_fault_t fault;
    status = safety_manager_get_fault(&fault);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (fault != SAFETY_FAULT_SENSOR_FAILURE) {
        printf("Expected fault SENSOR_FAILURE, got %s\n", 
               safety_manager_get_fault_name(fault));
        return HAL_STATUS_ERROR;
    }
    
    // Check safety level
    safety_status_t status_info;
    status = safety_manager_get_status(&status_info);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (status_info.current_level != SAFETY_LEVEL_CRITICAL) {
        printf("Expected safety level CRITICAL after fault, got %s\n", 
               safety_manager_get_level_name(status_info.current_level));
        return HAL_STATUS_ERROR;
    }
    
    // Test fault clearing
    status = safety_manager_clear_fault();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = safety_manager_get_fault(&fault);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (fault != SAFETY_FAULT_NONE) {
        printf("Expected fault NONE after clear, got %s\n", 
               safety_manager_get_fault_name(fault));
        return HAL_STATUS_ERROR;
    }
    
    printf("Fault handling successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_emergency_procedures(void) {
    // Test emergency stop
    hal_status_t status = safety_manager_emergency_stop();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Small delay to see LED pattern
    usleep(100000); // 100ms
    
    // Test safety shutdown
    status = safety_manager_safety_shutdown();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Small delay to see LED pattern
    usleep(100000); // 100ms
    
    printf("Emergency procedures successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_safety_recovery(void) {
    // Test safety recovery
    hal_status_t status = safety_manager_safety_recovery();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check if system is back to normal
    safety_status_t status_info;
    status = safety_manager_get_status(&status_info);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    if (status_info.current_level != SAFETY_LEVEL_NORMAL) {
        printf("Expected safety level NORMAL after recovery, got %s\n", 
               safety_manager_get_level_name(status_info.current_level));
        return HAL_STATUS_ERROR;
    }
    
    if (status_info.current_fault != SAFETY_FAULT_NONE) {
        printf("Expected fault NONE after recovery, got %s\n", 
               safety_manager_get_fault_name(status_info.current_fault));
        return HAL_STATUS_ERROR;
    }
    
    printf("Safety recovery successful\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_diagnostics(void) {
    // Get safety diagnostics
    char diagnostics[1024];
    hal_status_t status = safety_manager_get_diagnostics(diagnostics, sizeof(diagnostics));
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check if diagnostics contain expected information
    if (strstr(diagnostics, "Safety Manager Diagnostics:") == NULL) {
        printf("Diagnostics missing header\n");
        return HAL_STATUS_ERROR;
    }
    
    if (strstr(diagnostics, "Current Level:") == NULL) {
        printf("Diagnostics missing current level\n");
        return HAL_STATUS_ERROR;
    }
    
    if (strstr(diagnostics, "E-Stop Triggered:") == NULL) {
        printf("Diagnostics missing E-Stop status\n");
        return HAL_STATUS_ERROR;
    }
    
    printf("Diagnostics:\n%s\n", diagnostics);
    printf("Diagnostics test successful\n");
    return HAL_STATUS_OK;
}
