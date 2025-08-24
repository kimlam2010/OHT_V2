/**
 * @file test_motor_simple.c
 * @brief Simple test for Motor Module Handler
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-29 (Motor Module Implementation)
 */

#include "motor_module_handler.h"
#include "safety_manager.h"
#include "hal_common.h"
#include <stdio.h>

int main(int argc, char *argv) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;  // Suppress unused parameter warning
    
    printf("=== Simple Motor Module Test ===\n");
    
    // Initialize safety manager first
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
    
    hal_status_t safety_status = safety_manager_init(&safety_config);
    if (safety_status != HAL_STATUS_OK) {
        printf("❌ Safety manager initialization failed: %d\n", safety_status);
        return 1;
    }
    printf("✅ Safety manager initialized\n");
    
    // Test basic structure sizes
    printf("Size of motor_module_handler_t: %zu bytes\n", sizeof(motor_module_handler_t));
    printf("Size of motor_module_data_t: %zu bytes\n", sizeof(motor_module_data_t));
    printf("Size of motor_module_config_t: %zu bytes\n", sizeof(motor_module_config_t));
    
    // Test initialization
    motor_module_handler_t handler;
    motor_module_config_t config = {
        .address = 0x03,
        .command_timeout_ms = 1000,
        .response_timeout_ms = 500,
        .default_velocity = 1000,
        .default_acceleration = 500,
        .default_jerk = 100,
        .enable_safety_checks = true,
        .enable_position_limits = true,
        .enable_velocity_limits = true,
        .enable_acceleration_limits = true
    };
    
    hal_status_t status = motor_module_init(&handler, &config);
    if (status == HAL_STATUS_OK) {
        printf("✅ Motor module initialization successful\n");
    } else {
        printf("❌ Motor module initialization failed: %d\n", status);
        return 1;
    }
    
    // Test basic functions
    if (motor_module_is_enabled(&handler) == false) {
        printf("✅ Motor is initially disabled (expected)\n");
    } else {
        printf("❌ Motor should be initially disabled\n");
        return 1;
    }
    
    // Test enable
    status = motor_module_enable(&handler, true);
    if (status == HAL_STATUS_OK) {
        printf("✅ Motor enable successful\n");
    } else {
        printf("❌ Motor enable failed: %d\n", status);
        return 1;
    }
    
    if (motor_module_is_enabled(&handler) == true) {
        printf("✅ Motor is now enabled\n");
    } else {
        printf("❌ Motor should be enabled\n");
        return 1;
    }
    
    // Test disable
    status = motor_module_enable(&handler, false);
    if (status == HAL_STATUS_OK) {
        printf("✅ Motor disable successful\n");
    } else {
        printf("❌ Motor disable failed: %d\n", status);
        return 1;
    }
    
    // Test deinitialization
    status = motor_module_deinit(&handler);
    if (status == HAL_STATUS_OK) {
        printf("✅ Motor module deinitialization successful\n");
    } else {
        printf("❌ Motor module deinitialization failed: %d\n", status);
        return 1;
    }
    
    // Deinitialize safety manager
    safety_manager_deinit();
    printf("✅ Safety manager deinitialized\n");
    
    printf("✅ All basic tests passed!\n");
    return 0;
}
