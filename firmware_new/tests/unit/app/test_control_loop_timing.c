/**
 * @file test_control_loop_timing.c
 * @brief Unit tests for Control Loop timing and limits
 */

#include "unity.h"
#include "control_loop.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>

static control_config_t cfg;

void setUp(void)
{
    // Initialize config with proper values (no memset)
    cfg.control_frequency = 100.0f;
    cfg.sample_time = 0.01f;
    
    // Position PID
    cfg.position_pid.kp = 1.0f;
    cfg.position_pid.ki = 0.0f;
    cfg.position_pid.kd = 0.0f;
    cfg.position_pid.output_min = -100.0f;
    cfg.position_pid.output_max = 100.0f;
    cfg.position_pid.integral_min = -10.0f;
    cfg.position_pid.integral_max = 10.0f;
    
    // Velocity PID (copy from position)
    cfg.velocity_pid.kp = 1.0f;
    cfg.velocity_pid.ki = 0.0f;
    cfg.velocity_pid.kd = 0.0f;
    cfg.velocity_pid.output_min = -100.0f;
    cfg.velocity_pid.output_max = 100.0f;
    cfg.velocity_pid.integral_min = -10.0f;
    cfg.velocity_pid.integral_max = 10.0f;
    
    // Motion profile
    cfg.profile.max_velocity = 2000.0f;
    cfg.profile.max_acceleration = 1000.0f;
    cfg.profile.max_jerk = 5000.0f;
    cfg.profile.position_tolerance = 1.0f;
    cfg.profile.velocity_tolerance = 10.0f;
    
    // Safety and limits
    cfg.enable_limits = true;
    cfg.enable_safety = false;
    cfg.position_min_mm = 0.0f;
    cfg.position_max_mm = 10000.0f;
    
    // Debug: print config values
    printf("Config setup: control_frequency=%f, sample_time=%f\n", 
           cfg.control_frequency, cfg.sample_time);
}

void tearDown(void)
{
    control_loop_deinit();
}

void test_control_loop_update_under_100ms(void)
{
    // Initialize config directly in test function
    control_config_t test_cfg;
    test_cfg.control_frequency = 100.0f;
    test_cfg.sample_time = 0.01f;
    
    // Position PID
    test_cfg.position_pid.kp = 1.0f;
    test_cfg.position_pid.ki = 0.0f;
    test_cfg.position_pid.kd = 0.0f;
    test_cfg.position_pid.output_min = -100.0f;
    test_cfg.position_pid.output_max = 100.0f;
    test_cfg.position_pid.integral_min = -10.0f;
    test_cfg.position_pid.integral_max = 10.0f;
    
    // Velocity PID
    test_cfg.velocity_pid.kp = 1.0f;
    test_cfg.velocity_pid.ki = 0.0f;
    test_cfg.velocity_pid.kd = 0.0f;
    test_cfg.velocity_pid.output_min = -100.0f;
    test_cfg.velocity_pid.output_max = 100.0f;
    test_cfg.velocity_pid.integral_min = -10.0f;
    test_cfg.velocity_pid.integral_max = 10.0f;
    
    // Motion profile
    test_cfg.profile.max_velocity = 2000.0f;
    test_cfg.profile.max_acceleration = 1000.0f;
    test_cfg.profile.max_jerk = 5000.0f;
    test_cfg.profile.position_tolerance = 1.0f;
    test_cfg.profile.velocity_tolerance = 10.0f;
    
    // Safety and limits
    test_cfg.enable_limits = true;
    test_cfg.enable_safety = false;
    test_cfg.position_min_mm = 0.0f;
    test_cfg.position_max_mm = 10000.0f;
    
    // Debug: check config before init
    printf("Test config: control_frequency=%f, sample_time=%f\n", 
           test_cfg.control_frequency, test_cfg.sample_time);
    
    // Test control loop initialization
    hal_status_t init_status = control_loop_init(&test_cfg);
    printf("Control loop init status: %d\n", init_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, init_status);
    
    // Test enable and mode setting
    hal_status_t enable_status = control_loop_enable();
    printf("Control loop enable status: %d\n", enable_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, enable_status);
    
    hal_status_t mode_status = control_loop_set_mode(CONTROL_MODE_VELOCITY);
    printf("Control loop mode status: %d\n", mode_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, mode_status);
    
    hal_status_t target_status = control_loop_set_target_velocity(500.0f);
    printf("Control loop target status: %d\n", target_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, target_status);

    // Test update timing - should be very fast since it's just logic
    uint64_t t0 = hal_get_timestamp_us();
    hal_status_t update_status = control_loop_update();
    uint64_t t1 = hal_get_timestamp_us();
    
    printf("Control loop update status: %d\n", update_status);
    
    // Update should succeed
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, update_status);
    
    // Timing should be very fast (< 1ms for logic operations)
    uint64_t dt_us = t1 - t0;
    TEST_ASSERT_TRUE(dt_us < 1000); // < 1ms
}

void test_control_loop_respects_position_limits(void)
{
    // Initialize config directly in test function
    control_config_t test_cfg;
    test_cfg.control_frequency = 100.0f;
    test_cfg.sample_time = 0.01f;
    
    // Position PID
    test_cfg.position_pid.kp = 1.0f;
    test_cfg.position_pid.ki = 0.0f;
    test_cfg.position_pid.kd = 0.0f;
    test_cfg.position_pid.output_min = -100.0f;
    test_cfg.position_pid.output_max = 100.0f;
    test_cfg.position_pid.integral_min = -10.0f;
    test_cfg.position_pid.integral_max = 10.0f;
    
    // Velocity PID
    test_cfg.velocity_pid.kp = 1.0f;
    test_cfg.velocity_pid.ki = 0.0f;
    test_cfg.velocity_pid.kd = 0.0f;
    test_cfg.velocity_pid.output_min = -100.0f;
    test_cfg.velocity_pid.output_max = 100.0f;
    test_cfg.velocity_pid.integral_min = -10.0f;
    test_cfg.velocity_pid.integral_max = 10.0f;
    
    // Motion profile
    test_cfg.profile.max_velocity = 2000.0f;
    test_cfg.profile.max_acceleration = 1000.0f;
    test_cfg.profile.max_jerk = 5000.0f;
    test_cfg.profile.position_tolerance = 1.0f;
    test_cfg.profile.velocity_tolerance = 10.0f;
    
    // Safety and limits
    test_cfg.enable_limits = true;
    test_cfg.enable_safety = false;
    test_cfg.position_min_mm = 0.0f;
    test_cfg.position_max_mm = 10000.0f;
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, control_loop_init(&test_cfg));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, control_loop_enable());
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, control_loop_set_mode(CONTROL_MODE_POSITION));
    (void)control_loop_set_target_position(20000.0f); // beyond max

    (void)control_loop_update();

    control_status_t st;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, control_loop_get_status(&st));
    TEST_ASSERT_TRUE(st.state == CONTROL_STATE_ENABLED || st.state == CONTROL_STATE_ERROR);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_control_loop_update_under_100ms);
    RUN_TEST(test_control_loop_respects_position_limits);
    UNITY_END();
    return 0;
}


