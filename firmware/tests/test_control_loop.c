/**
 * @file test_control_loop.c
 * @brief Test suite for Control Loop module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-04 (Control Loop Implementation)
 */

#include "control_loop.h"
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

static void test_skip(const char *test_name, const char *reason) {
    printf("- SKIP: %s - %s\n", test_name, reason);
}

// Test functions
static void test_initialization(void) {
    printf("\n=== Test 1: Initialization ===\n");
    
    // Test configuration
    control_config_t config = {
        .control_frequency = 1000.0f,
        .sample_time = 0.001f,
        .position_pid = {
            .kp = 1.0f,
            .ki = 0.1f,
            .kd = 0.01f,
            .output_min = -100.0f,
            .output_max = 100.0f,
            .integral_min = -10.0f,
            .integral_max = 10.0f
        },
        .velocity_pid = {
            .kp = 0.5f,
            .ki = 0.05f,
            .kd = 0.005f,
            .output_min = -50.0f,
            .output_max = 50.0f,
            .integral_min = -5.0f,
            .integral_max = 5.0f
        },
        .profile = {
            .max_velocity = 100.0f,
            .max_acceleration = 50.0f,
            .max_jerk = 25.0f,
            .position_tolerance = 1.0f,
            .velocity_tolerance = 0.1f
        },
        .enable_limits = true,
        .enable_safety = true
    };
    
    // Test initialization
    hal_status_t status = control_loop_init(&config);
    test_assert(status == HAL_STATUS_OK, "Initialization", "Control loop should initialize successfully");
    
    // Test double initialization
    status = control_loop_init(&config);
    test_assert(status == HAL_STATUS_OK, "Double Initialization", "Double initialization should be safe");
    
    // Test invalid configuration
    control_config_t invalid_config = config;
    invalid_config.control_frequency = -1.0f; // Invalid frequency
    status = control_loop_init(&invalid_config);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Invalid Config", "Should reject invalid configuration");
}

static void test_mode_control(void) {
    printf("\n=== Test 2: Mode Control ===\n");
    
    // Test mode setting
    hal_status_t status = control_loop_set_mode(CONTROL_MODE_POSITION);
    test_assert(status == HAL_STATUS_OK, "Set Position Mode", "Should set position mode successfully");
    
    control_mode_t mode;
    status = control_loop_get_mode(&mode);
    test_assert(status == HAL_STATUS_OK && mode == CONTROL_MODE_POSITION, "Get Position Mode", "Should get correct mode");
    
    // Test velocity mode
    status = control_loop_set_mode(CONTROL_MODE_VELOCITY);
    test_assert(status == HAL_STATUS_OK, "Set Velocity Mode", "Should set velocity mode successfully");
    
    status = control_loop_get_mode(&mode);
    test_assert(status == HAL_STATUS_OK && mode == CONTROL_MODE_VELOCITY, "Get Velocity Mode", "Should get correct mode");
    
    // Test emergency mode
    status = control_loop_set_mode(CONTROL_MODE_EMERGENCY);
    test_assert(status == HAL_STATUS_OK, "Set Emergency Mode", "Should set emergency mode successfully");
    
    status = control_loop_get_mode(&mode);
    test_assert(status == HAL_STATUS_OK && mode == CONTROL_MODE_EMERGENCY, "Get Emergency Mode", "Should get correct mode");
}

static void test_enable_disable(void) {
    printf("\n=== Test 3: Enable/Disable ===\n");
    
    // Test enable
    hal_status_t status = control_loop_enable();
    test_assert(status == HAL_STATUS_OK, "Enable", "Should enable control loop successfully");
    
    bool enabled;
    status = control_loop_is_enabled(&enabled);
    test_assert(status == HAL_STATUS_OK && enabled == true, "Check Enabled", "Should be enabled");
    
    // Test disable
    status = control_loop_disable();
    test_assert(status == HAL_STATUS_OK, "Disable", "Should disable control loop successfully");
    
    status = control_loop_is_enabled(&enabled);
    test_assert(status == HAL_STATUS_OK && enabled == false, "Check Disabled", "Should be disabled");
}

static void test_position_control(void) {
    printf("\n=== Test 4: Position Control ===\n");
    
    // Enable control loop
    control_loop_enable();
    control_loop_set_mode(CONTROL_MODE_POSITION);
    
    // Test position setting
    hal_status_t status = control_loop_set_target_position(100.0f);
    test_assert(status == HAL_STATUS_OK, "Set Target Position", "Should set target position successfully");
    
    float position;
    status = control_loop_get_target_position(&position);
    test_assert(status == HAL_STATUS_OK && position == 100.0f, "Get Target Position", "Should get correct target position");
    
    // Test position limits
    status = control_loop_set_target_position(-50.0f); // Should be clamped
    test_assert(status == HAL_STATUS_OK, "Set Negative Position", "Should handle negative position");
    
    status = control_loop_set_target_position(15000.0f); // Should be clamped to max
    test_assert(status == HAL_STATUS_OK, "Set Large Position", "Should clamp large position");
    
    // Test current position
    status = control_loop_get_current_position(&position);
    test_assert(status == HAL_STATUS_OK, "Get Current Position", "Should get current position");
}

static void test_velocity_control(void) {
    printf("\n=== Test 5: Velocity Control ===\n");
    
    // Enable control loop
    control_loop_enable();
    control_loop_set_mode(CONTROL_MODE_VELOCITY);
    
    // Test velocity setting
    hal_status_t status = control_loop_set_target_velocity(50.0f);
    test_assert(status == HAL_STATUS_OK, "Set Target Velocity", "Should set target velocity successfully");
    
    float velocity;
    status = control_loop_get_target_velocity(&velocity);
    test_assert(status == HAL_STATUS_OK && velocity == 50.0f, "Get Target Velocity", "Should get correct target velocity");
    
    // Test velocity limits
    status = control_loop_set_target_velocity(-25.0f);
    test_assert(status == HAL_STATUS_OK, "Set Negative Velocity", "Should handle negative velocity");
    
    status = control_loop_set_target_velocity(150.0f); // Should be clamped
    test_assert(status == HAL_STATUS_OK, "Set Large Velocity", "Should clamp large velocity");
    
    // Test current velocity
    status = control_loop_get_current_velocity(&velocity);
    test_assert(status == HAL_STATUS_OK, "Get Current Velocity", "Should get current velocity");
}

static void test_pid_parameters(void) {
    printf("\n=== Test 6: PID Parameters ===\n");
    
    // Test position PID parameters
    pid_params_t pos_pid = {
        .kp = 2.0f,
        .ki = 0.2f,
        .kd = 0.02f,
        .output_min = -200.0f,
        .output_max = 200.0f,
        .integral_min = -20.0f,
        .integral_max = 20.0f
    };
    
    hal_status_t status = control_loop_set_pid_params(true, &pos_pid);
    test_assert(status == HAL_STATUS_OK, "Set Position PID", "Should set position PID parameters");
    
    pid_params_t retrieved_pid;
    status = control_loop_get_pid_params(true, &retrieved_pid);
    test_assert(status == HAL_STATUS_OK, "Get Position PID", "Should get position PID parameters");
    test_assert(memcmp(&pos_pid, &retrieved_pid, sizeof(pid_params_t)) == 0, "PID Parameters Match", "Retrieved parameters should match");
    
    // Test velocity PID parameters
    pid_params_t vel_pid = {
        .kp = 1.5f,
        .ki = 0.15f,
        .kd = 0.015f,
        .output_min = -100.0f,
        .output_max = 100.0f,
        .integral_min = -10.0f,
        .integral_max = 10.0f
    };
    
    status = control_loop_set_pid_params(false, &vel_pid);
    test_assert(status == HAL_STATUS_OK, "Set Velocity PID", "Should set velocity PID parameters");
    
    status = control_loop_get_pid_params(false, &retrieved_pid);
    test_assert(status == HAL_STATUS_OK, "Get Velocity PID", "Should get velocity PID parameters");
    test_assert(memcmp(&vel_pid, &retrieved_pid, sizeof(pid_params_t)) == 0, "Velocity PID Parameters Match", "Retrieved parameters should match");
}

static void test_motion_profile(void) {
    printf("\n=== Test 7: Motion Profile ===\n");
    
    // Test motion profile setting
    motion_profile_t profile = {
        .max_velocity = 200.0f,
        .max_acceleration = 100.0f,
        .max_jerk = 50.0f,
        .position_tolerance = 2.0f,
        .velocity_tolerance = 0.2f
    };
    
    hal_status_t status = control_loop_set_motion_profile(&profile);
    test_assert(status == HAL_STATUS_OK, "Set Motion Profile", "Should set motion profile successfully");
    
    motion_profile_t retrieved_profile;
    status = control_loop_get_motion_profile(&retrieved_profile);
    test_assert(status == HAL_STATUS_OK, "Get Motion Profile", "Should get motion profile successfully");
    test_assert(memcmp(&profile, &retrieved_profile, sizeof(motion_profile_t)) == 0, "Motion Profile Match", "Retrieved profile should match");
}

static void test_status_and_statistics(void) {
    printf("\n=== Test 8: Status and Statistics ===\n");
    
    // Enable control loop
    control_loop_enable();
    control_loop_set_mode(CONTROL_MODE_POSITION);
    control_loop_set_target_position(500.0f);
    
    // Test status
    control_status_t status;
    hal_status_t hal_status = control_loop_get_status(&status);
    test_assert(hal_status == HAL_STATUS_OK, "Get Status", "Should get control status successfully");
    test_assert(status.state == CONTROL_STATE_ENABLED, "Status State", "Should be in enabled state");
    test_assert(status.mode == CONTROL_MODE_POSITION, "Status Mode", "Should be in position mode");
    test_assert(status.target_position == 500.0f, "Status Target Position", "Should have correct target position");
    
    // Test statistics
    control_stats_t stats;
    hal_status = control_loop_get_stats(&stats);
    test_assert(hal_status == HAL_STATUS_OK, "Get Statistics", "Should get control statistics successfully");
    
    // Test statistics reset
    hal_status = control_loop_reset_stats();
    test_assert(hal_status == HAL_STATUS_OK, "Reset Statistics", "Should reset statistics successfully");
}

static void test_target_reached(void) {
    printf("\n=== Test 9: Target Reached ===\n");
    
    // Enable control loop
    control_loop_enable();
    control_loop_set_mode(CONTROL_MODE_POSITION);
    control_loop_set_target_position(1000.0f);
    
    // Test target reached (should be false initially)
    bool reached;
    hal_status_t status = control_loop_is_target_reached(&reached);
    test_assert(status == HAL_STATUS_OK, "Check Target Reached", "Should check target reached status");
    test_assert(reached == false, "Target Not Reached", "Target should not be reached initially");
}

static void test_emergency_stop(void) {
    printf("\n=== Test 10: Emergency Stop ===\n");
    
    // Enable control loop
    control_loop_enable();
    control_loop_set_mode(CONTROL_MODE_POSITION);
    
    // Test emergency stop
    hal_status_t status = control_loop_emergency_stop();
    test_assert(status == HAL_STATUS_OK, "Emergency Stop", "Should trigger emergency stop successfully");
    
    // Check mode changed to emergency
    control_mode_t mode;
    status = control_loop_get_mode(&mode);
    test_assert(status == HAL_STATUS_OK && mode == CONTROL_MODE_EMERGENCY, "Emergency Mode", "Should be in emergency mode");
    
    // Test clear errors
    status = control_loop_clear_errors();
    test_assert(status == HAL_STATUS_OK, "Clear Errors", "Should clear errors successfully");
}

static void test_diagnostics(void) {
    printf("\n=== Test 11: Diagnostics ===\n");
    
    // Enable control loop
    control_loop_enable();
    control_loop_set_mode(CONTROL_MODE_VELOCITY);
    control_loop_set_target_velocity(75.0f);
    
    // Test diagnostics
    char diagnostics[1024];
    hal_status_t status = control_loop_get_diagnostics(diagnostics, sizeof(diagnostics));
    test_assert(status == HAL_STATUS_OK, "Get Diagnostics", "Should get diagnostics successfully");
    test_assert(strlen(diagnostics) > 0, "Diagnostics Content", "Diagnostics should have content");
    
    printf("Diagnostics:\n%s\n", diagnostics);
}

static void test_utility_functions(void) {
    printf("\n=== Test 12: Utility Functions ===\n");
    
    // Test mode names
    const char* mode_name = control_loop_get_mode_name(CONTROL_MODE_POSITION);
    test_assert(strcmp(mode_name, "POSITION") == 0, "Position Mode Name", "Should return correct mode name");
    
    mode_name = control_loop_get_mode_name(CONTROL_MODE_VELOCITY);
    test_assert(strcmp(mode_name, "VELOCITY") == 0, "Velocity Mode Name", "Should return correct mode name");
    
    mode_name = control_loop_get_mode_name(CONTROL_MODE_EMERGENCY);
    test_assert(strcmp(mode_name, "EMERGENCY") == 0, "Emergency Mode Name", "Should return correct mode name");
    
    // Test state names
    const char* state_name = control_loop_get_state_name(CONTROL_STATE_ENABLED);
    test_assert(strcmp(state_name, "ENABLED") == 0, "Enabled State Name", "Should return correct state name");
    
    state_name = control_loop_get_state_name(CONTROL_STATE_DISABLED);
    test_assert(strcmp(state_name, "DISABLED") == 0, "Disabled State Name", "Should return correct state name");
    
    state_name = control_loop_get_state_name(CONTROL_STATE_ERROR);
    test_assert(strcmp(state_name, "ERROR") == 0, "Error State Name", "Should return correct state name");
}

static void test_control_loop_update(void) {
    printf("\n=== Test 13: Control Loop Update ===\n");
    
    // Enable control loop
    control_loop_enable();
    control_loop_set_mode(CONTROL_MODE_POSITION);
    control_loop_set_target_position(250.0f);
    
    // Test control loop update
    hal_status_t status = control_loop_update();
    test_assert(status == HAL_STATUS_OK, "Control Loop Update", "Should update control loop successfully");
    
    // Test multiple updates
    for (int i = 0; i < 5; i++) {
        status = control_loop_update();
        test_assert(status == HAL_STATUS_OK, "Multiple Updates", "Should handle multiple updates");
        usleep(1000); // 1ms delay
    }
}

int main(int argc, char *argv[]) {
    printf("=== Control Loop Test Suite ===\n");
    printf("Testing FW-04 Control Loop Implementation\n\n");
    
    // Run all tests
    test_initialization();
    test_mode_control();
    test_enable_disable();
    test_position_control();
    test_velocity_control();
    test_pid_parameters();
    test_motion_profile();
    test_status_and_statistics();
    test_target_reached();
    test_emergency_stop();
    test_diagnostics();
    test_utility_functions();
    test_control_loop_update();
    
    // Cleanup
    control_loop_deinit();
    
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
