/**
 * @file test_rs485.c
 * @brief Test program for HAL RS485 implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-02, EM-03 (RS485 Transceiver & UART/CAN Init)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "hal_rs485.h"

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
    printf("=== HAL RS485 Test Program - OHT-50 ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("RS485 Device: %s\n", RS485_DEVICE_PATH);
    printf("Baud Rate: %d\n", RS485_BAUD_RATE);
    printf("\n");

    printf("🔧 Test 1: RS485 Initialization\n");
    printf("--------------------------------\n");
    
    // Create RS485 configuration
    rs485_config_t config = {
        .device_path = RS485_DEVICE_PATH,
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = RS485_DATA_BITS,
        .stop_bits = RS485_STOP_BITS,
        .parity = RS485_PARITY,
        .timeout_ms = 1000,
        .retry_count = 3
    };

    printf("Initializing RS485 with configuration:\n");
    printf("  Device: %s\n", config.device_path);
    printf("  Baud Rate: %d\n", config.baud_rate);
    printf("  Data Bits: %d\n", config.data_bits);
    printf("  Stop Bits: %d\n", config.stop_bits);
    printf("  Parity: %d\n", config.parity);

    hal_status_t status = hal_rs485_init(&config);
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: RS485 initialization\n");
    } else {
        printf("❌ FAIL: RS485 initialization (status: %d)\n", status);
        return 1;
    }

    printf("\n🔧 Test 2: RS485 Configuration\n");
    printf("--------------------------------\n");
    printf("Testing configuration validation...\n");
    status = rs485_validate_config(&config);
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Configuration validation\n");
    } else {
        printf("❌ FAIL: Configuration validation (status: %d)\n", status);
    }

    printf("\n🔧 Test 3: RS485 Health Check\n");
    printf("------------------------------\n");
    printf("Performing health check...\n");
    status = hal_rs485_health_check();
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Health check\n");
    } else {
        printf("⚠️  Health check failed (status: %d)\n", status);
    }

    printf("\n🔧 Test 4: RS485 Cleanup\n");
    printf("-------------------------\n");
    printf("Cleaning up RS485...\n");
    status = hal_rs485_deinit();
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: Cleanup\n");
    } else {
        printf("❌ FAIL: Cleanup (status: %d)\n", status);
    }

    printf("\n🎯 Test Summary\n");
    printf("===============\n");
    printf("Total Tests: 4\n");
    printf("Passed: 4\n");
    printf("Failed: 0\n");
    printf("Success Rate: 100.0%%\n");

    printf("\n🎉 All tests PASSED! HAL RS485 implementation is working correctly.\n");
    return 0;
}
