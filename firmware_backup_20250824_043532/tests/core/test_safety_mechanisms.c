/**
 * @file test_safety_mechanisms.c
 * @brief Test suite for Safety Mechanisms module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-05 (Safety Mechanisms Implementation)
 */

#include "safety_mechanisms.h"
#include "hal_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Test results
static int test_passed = 0;
static int test_failed = 0;

// Test utilities
static void test_assert(bool condition, const char *test_name, const char *message) {
    if (condition) {
        printf("✓ PASS: %s - %s\n", test_name, message);
        test_passed++;
    } else {
        printf("✗ FAIL: %s - %s\n", test_name, message);
        test_failed++;
    }
}

// Test functions
static void test_initialization(void) {
    printf("\n=== Test 1: Initialization ===\n");
    
    // Test configuration
    safety_config_t config = {
        .level = SAFETY_LEVEL_SIL2,
        .zone = {
            .min_position = 0.0f,
            .max_position = 1000.0f,
            .min_velocity = 0.0f,
            .max_velocity = 100.0f,
            .min_acceleration = -50.0f,
            .max_acceleration = 50.0f,
            .enabled = true
        },
        .collision = {
            .detection_distance = 100.0f,
            .reaction_time = 50.0f,
            .enabled = true,
            .lidar_based = true,
            .sensor_based = false
        },
        .speed = {
            .max_speed = 100.0f,
            .warning_speed = 80.0f,
            .emergency_speed = 95.0f,
            .enabled = true,
            .real_time = true
        },
        .position = {
            .min_position = 0.0f,
            .max_position = 1000.0f,
            .soft_limit_margin = 10.0f,
            .hard_limit_margin = 5.0f,
            .enabled = true
        },
        .enable_estop = true,
        .enable_dual_channel = true,
        .update_frequency = 1000
    };
    
    // Test initialization
    hal_status_t status = safety_mechanisms_init(&config);
    test_assert(status == HAL_STATUS_OK, "Initialization", "Safety mechanisms should initialize successfully");
    
    // Test double initialization
    status = safety_mechanisms_init(&config);
    test_assert(status == HAL_STATUS_OK, "Double Initialization", "Double initialization should be safe");
}

static void test_safety_status(void) {
    printf("\n=== Test 2: Safety Status ===\n");
    
    // Test safety status
    safety_status_t status;
    hal_status_t hal_status = safety_mechanisms_get_status(&status);
    test_assert(hal_status == HAL_STATUS_OK, "Get Status", "Should get safety status successfully");
    test_assert(status.state == SAFETY_STATE_SAFE, "Initial State", "Should start in safe state");
    test_assert(status.fault == SAFETY_FAULT_NONE, "Initial Fault", "Should start with no fault");
    test_assert(status.estop_active == false, "Initial E-Stop", "Should start with E-Stop inactive");
}

static void test_estop_functionality(void) {
    printf("\n=== Test 3: E-Stop Functionality ===\n");
    
    // Test E-Stop trigger
    hal_status_t status = safety_mechanisms_trigger_emergency_stop();
    test_assert(status == HAL_STATUS_OK, "Trigger E-Stop", "Should trigger emergency stop successfully");
    
    // Check E-Stop status
    bool estop_active;
    status = safety_mechanisms_is_estop_active(&estop_active);
    test_assert(status == HAL_STATUS_OK && estop_active == true, "Check E-Stop Active", "E-Stop should be active");
    
    // Test E-Stop reset
    status = safety_mechanisms_reset_emergency_stop();
    test_assert(status == HAL_STATUS_OK, "Reset E-Stop", "Should reset emergency stop successfully");
    
    status = safety_mechanisms_is_estop_active(&estop_active);
    test_assert(status == HAL_STATUS_OK && estop_active == false, "Check E-Stop Reset", "E-Stop should be inactive");
}

static void test_safety_checks(void) {
    printf("\n=== Test 4: Safety Checks ===\n");
    
    // Test if system is safe
    bool safe;
    hal_status_t status = safety_mechanisms_is_safe(&safe);
    test_assert(status == HAL_STATUS_OK, "Check System Safe", "Should check if system is safe");
    test_assert(safe == true, "System Safe", "System should be safe initially");
    
    // Test fault status
    safety_fault_t fault;
    status = safety_mechanisms_get_fault(&fault);
    test_assert(status == HAL_STATUS_OK, "Get Fault", "Should get current fault");
    test_assert(fault == SAFETY_FAULT_NONE, "No Fault", "Should have no fault initially");
}

static void test_fault_clearing(void) {
    printf("\n=== Test 5: Fault Clearing ===\n");
    
    // Trigger E-Stop to create a fault
    safety_mechanisms_trigger_emergency_stop();
    
    // Check fault exists
    safety_fault_t fault;
    hal_status_t status = safety_mechanisms_get_fault(&fault);
    test_assert(status == HAL_STATUS_OK && fault == SAFETY_FAULT_ESTOP, "Fault Exists", "Should have E-Stop fault");
    
    // Clear faults
    status = safety_mechanisms_clear_faults();
    test_assert(status == HAL_STATUS_OK, "Clear Faults", "Should clear faults successfully");
    
    // Check fault cleared
    status = safety_mechanisms_get_fault(&fault);
    test_assert(status == HAL_STATUS_OK && fault == SAFETY_FAULT_NONE, "Fault Cleared", "Fault should be cleared");
}

static void test_statistics(void) {
    printf("\n=== Test 6: Statistics ===\n");
    
    // Test statistics
    safety_stats_t stats;
    hal_status_t status = safety_mechanisms_get_stats(&stats);
    test_assert(status == HAL_STATUS_OK, "Get Statistics", "Should get safety statistics successfully");
    
    // Test statistics reset
    status = safety_mechanisms_reset_stats();
    test_assert(status == HAL_STATUS_OK, "Reset Statistics", "Should reset statistics successfully");
}

static void test_diagnostics(void) {
    printf("\n=== Test 7: Diagnostics ===\n");
    
    // Test diagnostics
    char diagnostics[1024];
    hal_status_t status = safety_mechanisms_get_diagnostics(diagnostics, sizeof(diagnostics));
    test_assert(status == HAL_STATUS_OK, "Get Diagnostics", "Should get diagnostics successfully");
    test_assert(strlen(diagnostics) > 0, "Diagnostics Content", "Diagnostics should have content");
    
    printf("Diagnostics:\n%s\n", diagnostics);
}

static void test_utility_functions(void) {
    printf("\n=== Test 8: Utility Functions ===\n");
    
    // Test level names
    const char* level_name = safety_mechanisms_get_level_name(SAFETY_LEVEL_SIL2);
    test_assert(strcmp(level_name, "SIL2") == 0, "SIL2 Level Name", "Should return correct level name");
    
    // Test state names
    const char* state_name = safety_mechanisms_get_state_name(SAFETY_STATE_SAFE);
    test_assert(strcmp(state_name, "SAFE") == 0, "Safe State Name", "Should return correct state name");
    
    // Test fault names
    const char* fault_name = safety_mechanisms_get_fault_name(SAFETY_FAULT_NONE);
    test_assert(strcmp(fault_name, "NONE") == 0, "None Fault Name", "Should return correct fault name");
}

static void test_safety_update(void) {
    printf("\n=== Test 9: Safety Update ===\n");
    
    // Test safety mechanisms update
    hal_status_t status = safety_mechanisms_update();
    test_assert(status == HAL_STATUS_OK, "Safety Update", "Should update safety mechanisms successfully");
    
    // Test multiple updates
    for (int i = 0; i < 5; i++) {
        status = safety_mechanisms_update();
        test_assert(status == HAL_STATUS_OK, "Multiple Updates", "Should handle multiple updates");
        usleep(1000); // 1ms delay
    }
}

int main(int argc, char *argv[]) {
    printf("=== Safety Mechanisms Test Suite ===\n");
    printf("Testing FW-05 Safety Mechanisms Implementation\n\n");
    
    // Run all tests
    test_initialization();
    test_safety_status();
    test_estop_functionality();
    test_safety_checks();
    test_fault_clearing();
    test_statistics();
    test_diagnostics();
    test_utility_functions();
    test_safety_update();
    
    // Cleanup
    safety_mechanisms_deinit();
    
    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Total Tests: %d\n", test_passed + test_failed);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Success Rate: %.1f%%\n", (test_passed * 100.0f) / (test_passed + test_failed));
    
    if (test_failed == 0) {
        printf("✓ ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("✗ SOME TESTS FAILED!\n");
        return 1;
    }
}
