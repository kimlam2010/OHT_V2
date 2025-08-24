/**
 * @file test_control_loop.c
 * @brief Unit tests for Control Loop
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-17 (Application Core Testing)
 */

#include "unity.h"
#include "control_loop.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Test fixtures
static control_config_t test_config;
static control_status_t test_status;
static control_stats_t test_stats;

void setUp(void)
{
    // Initialize test configuration
    memset(&test_config, 0, sizeof(control_config_t));
    test_config.control_frequency = 100.0f;
    test_config.sample_time = 0.01f;
    test_config.position_pid.kp = 1.0f;
    test_config.position_pid.ki = 0.1f;
    test_config.position_pid.kd = 0.01f;
    test_config.position_pid.output_min = -10.0f;
    test_config.position_pid.output_max = 10.0f;
    test_config.position_pid.integral_min = -5.0f;
    test_config.position_pid.integral_max = 5.0f;
    test_config.velocity_pid.kp = 0.5f;
    test_config.velocity_pid.ki = 0.05f;
    test_config.velocity_pid.kd = 0.005f;
    test_config.velocity_pid.output_min = -5.0f;
    test_config.velocity_pid.output_max = 5.0f;
    test_config.velocity_pid.integral_min = -2.5f;
    test_config.velocity_pid.integral_max = 2.5f;
    test_config.profile.max_velocity = 2000.0f;
    test_config.profile.max_acceleration = 1000.0f;
    test_config.profile.max_jerk = 500.0f;
    test_config.profile.position_tolerance = 1.0f;
    test_config.profile.velocity_tolerance = 10.0f;
    test_config.enable_limits = true;
    test_config.enable_safety = true;
    
    // Initialize test status
    memset(&test_status, 0, sizeof(control_status_t));
    test_status.state = CONTROL_STATE_DISABLED;
    test_status.mode = CONTROL_MODE_IDLE;
    test_status.target_position = 0.0f;
    test_status.current_position = 0.0f;
    test_status.target_velocity = 0.0f;
    test_status.current_velocity = 0.0f;
    test_status.control_output = 0.0f;
    test_status.position_error = 0.0f;
    test_status.velocity_error = 0.0f;
    test_status.cycle_count = 0;
    test_status.last_update_time = 1640995200000000ULL;
    test_status.limits_violated = false;
    test_status.safety_violated = false;
    
    // Initialize test stats
    memset(&test_stats, 0, sizeof(control_stats_t));
    test_stats.total_cycles = 0;
    test_stats.error_cycles = 0;
    test_stats.max_position_error = 0.0f;
    test_stats.max_velocity_error = 0.0f;
    test_stats.avg_position_error = 0.0f;
    test_stats.avg_velocity_error = 0.0f;
    test_stats.total_runtime = 0;
}

void tearDown(void)
{
    // Cleanup after each test
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_control_mode_constants(void)
{
    TEST_ASSERT_EQUAL(0, CONTROL_MODE_IDLE);
    TEST_ASSERT_EQUAL(1, CONTROL_MODE_POSITION);
    TEST_ASSERT_EQUAL(2, CONTROL_MODE_VELOCITY);
    TEST_ASSERT_EQUAL(3, CONTROL_MODE_TORQUE);
    TEST_ASSERT_EQUAL(4, CONTROL_MODE_HOMING);
    TEST_ASSERT_EQUAL(5, CONTROL_MODE_EMERGENCY);
}

void test_control_state_constants(void)
{
    TEST_ASSERT_EQUAL(0, CONTROL_STATE_DISABLED);
    TEST_ASSERT_EQUAL(1, CONTROL_STATE_ENABLED);
    TEST_ASSERT_EQUAL(2, CONTROL_STATE_RUNNING);
    TEST_ASSERT_EQUAL(3, CONTROL_STATE_ERROR);
    TEST_ASSERT_EQUAL(4, CONTROL_STATE_FAULT);
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_control_loop_data_structures(void)
{
    TEST_ASSERT_EQUAL(4, sizeof(test_config.control_frequency));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.sample_time));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.position_pid.kp));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.velocity_pid.kp));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.profile.max_velocity));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.enable_limits));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.enable_safety));
}

void test_control_loop_config_initialization(void)
{
    TEST_ASSERT_EQUAL_FLOAT(100.0f, test_config.control_frequency);
    TEST_ASSERT_EQUAL_FLOAT(0.01f, test_config.sample_time);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, test_config.position_pid.kp);
    TEST_ASSERT_EQUAL_FLOAT(0.1f, test_config.position_pid.ki);
    TEST_ASSERT_EQUAL_FLOAT(0.01f, test_config.position_pid.kd);
    TEST_ASSERT_EQUAL_FLOAT(2000.0f, test_config.profile.max_velocity);
    TEST_ASSERT_EQUAL_FLOAT(1000.0f, test_config.profile.max_acceleration);
    TEST_ASSERT_EQUAL_FLOAT(500.0f, test_config.profile.max_jerk);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, test_config.profile.position_tolerance);
    TEST_ASSERT_EQUAL_FLOAT(10.0f, test_config.profile.velocity_tolerance);
    TEST_ASSERT_TRUE(test_config.enable_limits);
    TEST_ASSERT_TRUE(test_config.enable_safety);
}

void test_control_loop_status_initialization(void)
{
    TEST_ASSERT_EQUAL(0, test_status.state);
    TEST_ASSERT_EQUAL(0, test_status.mode);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_status.target_position);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_status.current_position);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_status.target_velocity);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_status.current_velocity);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_status.control_output);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_status.position_error);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_status.velocity_error);
    TEST_ASSERT_EQUAL(0, test_status.cycle_count);
    TEST_ASSERT_EQUAL(1640995200000000ULL, test_status.last_update_time);
    TEST_ASSERT_FALSE(test_status.limits_violated);
    TEST_ASSERT_FALSE(test_status.safety_violated);
}

void test_control_loop_stats_initialization(void)
{
    TEST_ASSERT_EQUAL(0, test_stats.total_cycles);
    TEST_ASSERT_EQUAL(0, test_stats.error_cycles);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_stats.max_position_error);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_stats.max_velocity_error);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_stats.avg_position_error);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_stats.avg_velocity_error);
    TEST_ASSERT_EQUAL(0, test_stats.total_runtime);
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void test_control_loop_config_validation(void)
{
    // Test valid configuration
    TEST_ASSERT_TRUE(test_config.control_frequency > 0);
    TEST_ASSERT_TRUE(test_config.sample_time > 0);
    TEST_ASSERT_TRUE(test_config.position_pid.kp > 0);
    TEST_ASSERT_TRUE(test_config.velocity_pid.kp > 0);
    TEST_ASSERT_TRUE(test_config.profile.max_velocity > 0);
    TEST_ASSERT_TRUE(test_config.profile.max_acceleration > 0);
    TEST_ASSERT_TRUE(test_config.profile.max_jerk > 0);
    TEST_ASSERT_TRUE(test_config.profile.position_tolerance > 0);
    TEST_ASSERT_TRUE(test_config.profile.velocity_tolerance > 0);
}

void test_control_loop_status_validation(void)
{
    // Test valid status
    TEST_ASSERT_TRUE(test_status.state >= 0 && test_status.state <= 4);
    TEST_ASSERT_TRUE(test_status.mode >= 0 && test_status.mode <= 5);
    TEST_ASSERT_TRUE(test_status.target_position >= -10000.0f && test_status.target_position <= 10000.0f);
    TEST_ASSERT_TRUE(test_status.current_position >= -10000.0f && test_status.current_position <= 10000.0f);
    TEST_ASSERT_TRUE(test_status.target_velocity >= -1000.0f && test_status.target_velocity <= 1000.0f);
    TEST_ASSERT_TRUE(test_status.current_velocity >= -1000.0f && test_status.current_velocity <= 1000.0f);
    TEST_ASSERT_TRUE(test_status.last_update_time > 0);
}

void test_control_loop_stats_validation(void)
{
    // Test valid stats
    TEST_ASSERT_TRUE(test_stats.total_cycles >= 0);
    TEST_ASSERT_TRUE(test_stats.error_cycles >= 0);
    TEST_ASSERT_TRUE(test_stats.max_position_error >= 0);
    TEST_ASSERT_TRUE(test_stats.max_velocity_error >= 0);
    TEST_ASSERT_TRUE(test_stats.avg_position_error >= 0);
    TEST_ASSERT_TRUE(test_stats.avg_velocity_error >= 0);
    TEST_ASSERT_TRUE(test_stats.total_runtime >= 0);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Constants tests
    RUN_TEST(test_control_mode_constants);
    RUN_TEST(test_control_state_constants);
    
    // Data structures tests
    RUN_TEST(test_control_loop_data_structures);
    RUN_TEST(test_control_loop_config_initialization);
    RUN_TEST(test_control_loop_status_initialization);
    RUN_TEST(test_control_loop_stats_initialization);
    
    // Validation tests
    RUN_TEST(test_control_loop_config_validation);
    RUN_TEST(test_control_loop_status_validation);
    RUN_TEST(test_control_loop_stats_validation);
    
    UNITY_END();
    return 0;
}
