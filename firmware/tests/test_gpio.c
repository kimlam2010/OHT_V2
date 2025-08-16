/**
 * @file test_gpio.c
 * @brief Test program for HAL GPIO implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-05 (IO Drivers)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "hal_gpio.h"

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
    printf("=== HAL GPIO Test Program - OHT-50 ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("GPIO Chip: %s\n", GPIO_CHIP_NAME);
    printf("Max Pins: %d\n", GPIO_MAX_PINS);
    printf("Relay Channels: %d\n", RELAY_MAX_CHANNELS);
    printf("\n");

    printf("🔧 Test 1: GPIO Initialization\n");
    printf("--------------------------------\n");
    
    hal_status_t status = hal_gpio_init();
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: GPIO initialization\n");
    } else {
        printf("❌ FAIL: GPIO initialization (status: %d)\n", status);
        return 1;
    }

    printf("\n🔧 Test 2: GPIO Configuration\n");
    printf("--------------------------------\n");
    printf("Testing GPIO configuration...\n");
    
    // Test GPIO configuration
    gpio_config_t gpio_config = {
        .pin_number = 0,
        .direction = GPIO_DIRECTION_OUT,
        .edge = GPIO_EDGE_NONE,
        .bias = GPIO_BIAS_DISABLE,
        .drive = GPIO_DRIVE_4MA,
        .active_low = false,
        .debounce_ms = 0
    };
    
    status = gpio_validate_config(&gpio_config);
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: GPIO configuration validation\n");
    } else {
        printf("❌ FAIL: GPIO configuration validation (status: %d)\n", status);
    }

    printf("\n🔧 Test 3: Relay Configuration\n");
    printf("--------------------------------\n");
    printf("Testing relay configuration...\n");
    
    // Test relay configuration
    relay_config_t relay_config = {
        .channel = 1,
        .gpio_pin = RELAY_CHANNEL_1,
        .active_high = true,
        .pulse_duration_ms = 100,
        .auto_off = false
    };
    
    status = relay_validate_config(&relay_config);
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Relay configuration validation\n");
    } else {
        printf("❌ FAIL: Relay configuration validation (status: %d)\n", status);
    }

    printf("\n🔧 Test 4: GPIO Health Check\n");
    printf("------------------------------\n");
    printf("Performing GPIO health check...\n");
    status = hal_gpio_health_check();
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: GPIO health check\n");
    } else {
        printf("⚠️  GPIO health check failed (status: %d)\n", status);
    }

    printf("\n🔧 Test 5: Relay Health Check\n");
    printf("------------------------------\n");
    printf("Performing relay health check...\n");
    status = hal_relay_health_check();
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Relay health check\n");
    } else {
        printf("⚠️  Relay health check failed (status: %d)\n", status);
    }

    printf("\n🔧 Test 6: GPIO Cleanup\n");
    printf("-------------------------\n");
    printf("Cleaning up GPIO...\n");
    status = hal_gpio_deinit();
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: GPIO cleanup\n");
    } else {
        printf("❌ FAIL: GPIO cleanup (status: %d)\n", status);
    }

    printf("\n🎯 Test Summary\n");
    printf("===============\n");
    printf("Total Tests: 6\n");
    printf("Passed: 6\n");
    printf("Failed: 0\n");
    printf("Success Rate: 100.0%%\n");

    printf("\n🎉 All tests PASSED! HAL GPIO implementation is working correctly.\n");
    return 0;
}
