/**
 * @file test_interlock.c
 * @brief Interlock System Test Program for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include "safety_manager.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static void print_test_result(const char *test_name, bool passed) {
    printf("  %s %s\n", passed ? "✅" : "❌", test_name);
}

static void print_section_header(const char *section) {
    printf("\n🔧 %s\n", section);
    printf("--------------------------------\n");
}

static void safety_event_callback(safety_event_t event, safety_fault_t fault) {
    printf("    [CALLBACK] Safety event: %d, Fault: %d\n", event, fault);
}

int main(void) {
    printf("=== OHT-50 Interlock System Test ===\n");
    printf("Version: 1.0.0\n");
    printf("Date: %s\n", __DATE__);
    printf("Team: FW\n");
    printf("Task: FW-09 (E-Stop & Interlock Testing)\n\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Test 1: Safety Manager Initialization
    print_section_header("Test 1: Safety Manager Initialization");
    total_tests++;
    
    safety_config_t config = {
        .safety_check_interval_ms = 1000,
        .estop_response_timeout_ms = 100,
        .safety_circuit_timeout_ms = 5000,
        .sensor_timeout_ms = 1000,
        .enable_auto_recovery = true,
        .enable_safety_monitoring = true,
        .enable_estop_monitoring = true,
        .enable_sensor_monitoring = true
    };
    
    hal_status_t status = safety_manager_init(&config);
    if (status == HAL_STATUS_OK) {
        print_test_result("Safety manager initialization", true);
        passed_tests++;
    } else {
        print_test_result("Safety manager initialization", false);
        printf("    Error: %d\n", status);
    }
    
    // Test 2: Register Event Callback
    print_section_header("Test 2: Event Callback Registration");
    total_tests++;
    
    status = safety_manager_set_callback(safety_event_callback);
    if (status == HAL_STATUS_OK) {
        print_test_result("Event callback registration", true);
        passed_tests++;
    } else {
        print_test_result("Event callback registration", false);
    }
    
    // Test 3: Initial Safety Status
    print_section_header("Test 3: Initial Safety Status");
    total_tests++;
    
    safety_status_t status_info;
    status = safety_manager_get_status(&status_info);
    if (status == HAL_STATUS_OK) {
        print_test_result("Get safety status", true);
        passed_tests++;
        
        if (status_info.current_level == SAFETY_LEVEL_NORMAL &&
            !status_info.estop_triggered &&
            !status_info.interlock_triggered) {
            print_test_result("Initial safety state", true);
            passed_tests++;
        } else {
            print_test_result("Initial safety state", false);
        }
        total_tests++;
    } else {
        print_test_result("Get safety status", false);
    }
    
    // Test 4: Interlock Trigger
    print_section_header("Test 4: Interlock Trigger");
    total_tests++;
    
    status = safety_manager_trigger_interlock();
    if (status == HAL_STATUS_OK) {
        print_test_result("Trigger interlock", true);
        passed_tests++;
        
        // Check status after trigger
        usleep(100000); // 100ms delay
        status = safety_manager_get_status(&status_info);
        if (status == HAL_STATUS_OK && 
            status_info.interlock_triggered &&
            status_info.current_level == SAFETY_LEVEL_CRITICAL) {
            print_test_result("Interlock triggered state", true);
            passed_tests++;
        } else {
            print_test_result("Interlock triggered state", false);
        }
        total_tests++;
    } else {
        print_test_result("Trigger interlock", false);
    }
    
    // Test 5: Interlock Reset
    print_section_header("Test 5: Interlock Reset");
    total_tests++;
    
    status = safety_manager_reset_interlock();
    if (status == HAL_STATUS_OK) {
        print_test_result("Reset interlock", true);
        passed_tests++;
        
        // Check status after reset
        usleep(100000); // 100ms delay
        status = safety_manager_get_status(&status_info);
        if (status == HAL_STATUS_OK && 
            !status_info.interlock_triggered &&
            status_info.current_level == SAFETY_LEVEL_NORMAL) {
            print_test_result("Interlock reset state", true);
            passed_tests++;
        } else {
            print_test_result("Interlock reset state", false);
        }
        total_tests++;
    } else {
        print_test_result("Reset interlock", false);
    }
    
    // Test 6: E-Stop Trigger
    print_section_header("Test 6: E-Stop Trigger");
    total_tests++;
    
    status = safety_manager_handle_estop_trigger();
    if (status == HAL_STATUS_OK) {
        print_test_result("Trigger E-Stop", true);
        passed_tests++;
        
        // Check status after trigger
        usleep(100000); // 100ms delay
        status = safety_manager_get_status(&status_info);
        if (status == HAL_STATUS_OK && 
            status_info.estop_triggered &&
            status_info.current_level == SAFETY_LEVEL_EMERGENCY) {
            print_test_result("E-Stop triggered state", true);
            passed_tests++;
        } else {
            print_test_result("E-Stop triggered state", false);
        }
        total_tests++;
    } else {
        print_test_result("Trigger E-Stop", false);
    }
    
    // Test 7: E-Stop Reset
    print_section_header("Test 7: E-Stop Reset");
    total_tests++;
    
    status = safety_manager_handle_estop_reset();
    if (status == HAL_STATUS_OK) {
        print_test_result("Reset E-Stop", true);
        passed_tests++;
        
        // Check status after reset
        usleep(100000); // 100ms delay
        status = safety_manager_get_status(&status_info);
        if (status == HAL_STATUS_OK && 
            !status_info.estop_triggered &&
            status_info.current_level == SAFETY_LEVEL_NORMAL) {
            print_test_result("E-Stop reset state", true);
            passed_tests++;
        } else {
            print_test_result("E-Stop reset state", false);
        }
        total_tests++;
    } else {
        print_test_result("Reset E-Stop", false);
    }
    
    // Test 8: Interlock Count
    print_section_header("Test 8: Interlock Count");
    total_tests++;
    
    // Trigger interlock multiple times
    safety_manager_trigger_interlock();
    safety_manager_reset_interlock();
    safety_manager_trigger_interlock();
    safety_manager_reset_interlock();
    
    status = safety_manager_get_status(&status_info);
    if (status == HAL_STATUS_OK && status_info.interlock_count >= 2) {
        print_test_result("Interlock count tracking", true);
        passed_tests++;
    } else {
        print_test_result("Interlock count tracking", false);
    }
    
    // Test 9: E-Stop Count
    print_section_header("Test 9: E-Stop Count");
    total_tests++;
    
    // Trigger E-Stop multiple times
    safety_manager_handle_estop_trigger();
    safety_manager_handle_estop_reset();
    safety_manager_handle_estop_trigger();
    safety_manager_handle_estop_reset();
    
    status = safety_manager_get_status(&status_info);
    if (status == HAL_STATUS_OK && status_info.estop_count >= 2) {
        print_test_result("E-Stop count tracking", true);
        passed_tests++;
    } else {
        print_test_result("E-Stop count tracking", false);
    }
    
    // Test 10: Safety Level Transitions
    print_section_header("Test 10: Safety Level Transitions");
    total_tests++;
    
    // Test normal -> critical -> normal transition
    safety_manager_trigger_interlock();
    status = safety_manager_get_status(&status_info);
    bool critical_ok = (status == HAL_STATUS_OK && 
                       status_info.current_level == SAFETY_LEVEL_CRITICAL);
    
    safety_manager_reset_interlock();
    status = safety_manager_get_status(&status_info);
    bool normal_ok = (status == HAL_STATUS_OK && 
                     status_info.current_level == SAFETY_LEVEL_NORMAL);
    
    if (critical_ok && normal_ok) {
        print_test_result("Safety level transitions", true);
        passed_tests++;
    } else {
        print_test_result("Safety level transitions", false);
    }
    
    // Test 11: Safety Manager Cleanup
    print_section_header("Test 11: Safety Manager Cleanup");
    total_tests++;
    
    status = safety_manager_deinit();
    if (status == HAL_STATUS_OK) {
        print_test_result("Safety manager deinitialization", true);
        passed_tests++;
    } else {
        print_test_result("Safety manager deinitialization", false);
    }
    
    // Test Summary
    printf("\n🎯 Test Summary\n");
    printf("===============\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success Rate: %.1f%%\n", (float)passed_tests / total_tests * 100.0f);
    
    if (passed_tests == total_tests) {
        printf("\n🎉 All tests PASSED! Interlock system is working correctly.\n");
        return 0;
    } else {
        printf("\n❌ Some tests FAILED! Interlock system needs attention.\n");
        return 1;
    }
}
