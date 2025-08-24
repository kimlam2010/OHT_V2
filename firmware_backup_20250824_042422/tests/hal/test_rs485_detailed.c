/**
 * @file test_rs485_detailed.c
 * @brief Detailed test program for HAL RS485 implementation
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
#include <time.h>
#include "hal_rs485.h"

#define TEST_BUFFER_SIZE 256
#define LOOPBACK_TEST_COUNT 10
#define TRANSMIT_TEST_COUNT 5

static bool test_running = true;

void signal_handler(int sig) {
    printf("\n🛑 Received signal %d, stopping tests...\n", sig);
    test_running = false;
}

void print_statistics(const rs485_statistics_t *stats) {
    printf("📊 RS485 Statistics:\n");
    printf("  Bytes Transmitted: %lu\n", stats->bytes_transmitted);
    printf("  Bytes Received: %lu\n", stats->bytes_received);
    printf("  Frames Transmitted: %lu\n", stats->frames_transmitted);
    printf("  Frames Received: %lu\n", stats->frames_received);
    printf("  CRC Errors: %lu\n", stats->errors_crc);
    printf("  Timeout Errors: %lu\n", stats->errors_timeout);
    printf("  Parity Errors: %lu\n", stats->errors_parity);
    printf("  Timestamp: %lu us\n", stats->timestamp_us);
}

void print_status(const rs485_status_t status) {
    switch (status) {
        case RS485_STATUS_IDLE:
            printf("IDLE");
            break;
        case RS485_STATUS_TRANSMITTING:
            printf("TRANSMITTING");
            break;
        case RS485_STATUS_RECEIVING:
            printf("RECEIVING");
            break;
        case RS485_STATUS_ERROR:
            printf("ERROR");
            break;
        default:
            printf("UNKNOWN");
            break;
    }
}

int test_rs485_loopback(void) {
    printf("\n🔄 Test: RS485 Loopback Test\n");
    printf("============================\n");
    
    rs485_config_t config = {
        .device_path = RS485_DEVICE_PATH,
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = RS485_DATA_BITS,
        .stop_bits = RS485_STOP_BITS,
        .parity = RS485_PARITY,
        .timeout_ms = 1000,
        .retry_count = 3
    };
    
    // Initialize RS485
    hal_status_t status = hal_rs485_init(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ FAIL: RS485 initialization (status: %d)\n", status);
        return -1;
    }
    
    // Open device
    status = hal_rs485_open();
    if (status != HAL_STATUS_OK) {
        printf("❌ FAIL: RS485 open (status: %d)\n", status);
        hal_rs485_deinit();
        return -1;
    }
    
    printf("✅ RS485 device opened successfully\n");
    
    // Test data
    uint8_t tx_buffer[TEST_BUFFER_SIZE];
    uint8_t rx_buffer[TEST_BUFFER_SIZE];
    size_t actual_length;
    int success_count = 0;
    
    for (int i = 0; i < LOOPBACK_TEST_COUNT && test_running; i++) {
        // Prepare test data
        int data_length = snprintf((char*)tx_buffer, TEST_BUFFER_SIZE, 
                                 "RS485 Loopback Test #%d - Timestamp: %lu", 
                                 i + 1, (unsigned long)time(NULL));
        
        printf("  Test %d/%d: Transmitting %d bytes... ", i + 1, LOOPBACK_TEST_COUNT, data_length);
        
        // Transmit data
        status = hal_rs485_transmit(tx_buffer, data_length);
        if (status != HAL_STATUS_OK) {
            printf("❌ FAIL (transmit status: %d)\n", status);
            continue;
        }
        
        // Small delay for loopback
        usleep(10000); // 10ms
        
        // Receive data
        status = hal_rs485_receive(rx_buffer, TEST_BUFFER_SIZE, &actual_length);
        if (status == HAL_STATUS_OK && actual_length > 0) {
            // Compare data
            if (actual_length == data_length && memcmp(tx_buffer, rx_buffer, data_length) == 0) {
                printf("✅ PASS (received %zu bytes)\n", actual_length);
                success_count++;
            } else {
                printf("❌ FAIL (data mismatch)\n");
                printf("    Expected: %d bytes, Received: %zu bytes\n", data_length, actual_length);
            }
        } else {
            printf("❌ FAIL (receive status: %d, length: %zu)\n", status, actual_length);
        }
        
        usleep(100000); // 100ms delay between tests
    }
    
    // Get and print statistics
    rs485_statistics_t stats;
    status = hal_rs485_get_statistics(&stats);
    if (status == HAL_STATUS_OK) {
        print_statistics(&stats);
    }
    
    printf("\n📈 Loopback Test Results:\n");
    printf("  Total Tests: %d\n", LOOPBACK_TEST_COUNT);
    printf("  Successful: %d\n", success_count);
    printf("  Failed: %d\n", LOOPBACK_TEST_COUNT - success_count);
    printf("  Success Rate: %.1f%%\n", (float)success_count / LOOPBACK_TEST_COUNT * 100.0f);
    
    // Cleanup
    hal_rs485_close();
    hal_rs485_deinit();
    
    return (success_count == LOOPBACK_TEST_COUNT) ? 0 : -1;
}

int test_rs485_transmit_only(void) {
    printf("\n📤 Test: RS485 Transmit Only Test\n");
    printf("==================================\n");
    
    rs485_config_t config = {
        .device_path = RS485_DEVICE_PATH,
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = RS485_DATA_BITS,
        .stop_bits = RS485_STOP_BITS,
        .parity = RS485_PARITY,
        .timeout_ms = 1000,
        .retry_count = 3
    };
    
    // Initialize RS485
    hal_status_t status = hal_rs485_init(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ FAIL: RS485 initialization (status: %d)\n", status);
        return -1;
    }
    
    // Open device
    status = hal_rs485_open();
    if (status != HAL_STATUS_OK) {
        printf("❌ FAIL: RS485 open (status: %d)\n", status);
        hal_rs485_deinit();
        return -1;
    }
    
    printf("✅ RS485 device opened successfully\n");
    
    // Test data
    uint8_t tx_buffer[TEST_BUFFER_SIZE];
    int success_count = 0;
    
    for (int i = 0; i < TRANSMIT_TEST_COUNT && test_running; i++) {
        // Prepare test data
        int data_length = snprintf((char*)tx_buffer, TEST_BUFFER_SIZE, 
                                 "RS485 Transmit Test #%d - Data: 0x%02X%02X%02X%02X", 
                                 i + 1, i, i+1, i+2, i+3);
        
        printf("  Test %d/%d: Transmitting %d bytes... ", i + 1, TRANSMIT_TEST_COUNT, data_length);
        
        // Get status before transmit
        rs485_status_t status_before;
        hal_rs485_get_status(&status_before);
        printf("(Status: ");
        print_status(status_before);
        printf(") ");
        
        // Transmit data
        status = hal_rs485_transmit(tx_buffer, data_length);
        if (status == HAL_STATUS_OK) {
            printf("✅ PASS\n");
            success_count++;
        } else {
            printf("❌ FAIL (status: %d)\n", status);
        }
        
        usleep(200000); // 200ms delay between tests
    }
    
    // Get and print statistics
    rs485_statistics_t stats;
    status = hal_rs485_get_statistics(&stats);
    if (status == HAL_STATUS_OK) {
        print_statistics(&stats);
    }
    
    printf("\n📈 Transmit Test Results:\n");
    printf("  Total Tests: %d\n", TRANSMIT_TEST_COUNT);
    printf("  Successful: %d\n", success_count);
    printf("  Failed: %d\n", TRANSMIT_TEST_COUNT - success_count);
    printf("  Success Rate: %.1f%%\n", (float)success_count / TRANSMIT_TEST_COUNT * 100.0f);
    
    // Cleanup
    hal_rs485_close();
    hal_rs485_deinit();
    
    return (success_count == TRANSMIT_TEST_COUNT) ? 0 : -1;
}

int test_rs485_error_handling(void) {
    printf("\n⚠️  Test: RS485 Error Handling Test\n");
    printf("===================================\n");
    
    rs485_config_t config = {
        .device_path = RS485_DEVICE_PATH,
        .baud_rate = RS485_BAUD_RATE,
        .data_bits = RS485_DATA_BITS,
        .stop_bits = RS485_STOP_BITS,
        .parity = RS485_PARITY,
        .timeout_ms = 100, // Short timeout for testing
        .retry_count = 2
    };
    
    // Initialize RS485
    hal_status_t status = hal_rs485_init(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ FAIL: RS485 initialization (status: %d)\n", status);
        return -1;
    }
    
    // Open device
    status = hal_rs485_open();
    if (status != HAL_STATUS_OK) {
        printf("❌ FAIL: RS485 open (status: %d)\n", status);
        hal_rs485_deinit();
        return -1;
    }
    
    printf("✅ RS485 device opened successfully\n");
    
    // Test 1: Try to receive with no data (should timeout)
    printf("  Test 1: Receive timeout test... ");
    uint8_t rx_buffer[TEST_BUFFER_SIZE];
    size_t actual_length;
    status = hal_rs485_receive(rx_buffer, TEST_BUFFER_SIZE, &actual_length);
    if (status == HAL_STATUS_TIMEOUT) {
        printf("✅ PASS (expected timeout)\n");
    } else {
        printf("❌ FAIL (unexpected status: %d)\n", status);
    }
    
    // Test 2: Try to transmit empty data (should fail)
    printf("  Test 2: Transmit empty data test... ");
    status = hal_rs485_transmit(NULL, 0);
    if (status == HAL_STATUS_INVALID_PARAMETER) {
        printf("✅ PASS (expected invalid parameter)\n");
    } else {
        printf("❌ FAIL (unexpected status: %d)\n", status);
    }
    
    // Test 3: Health check
    printf("  Test 3: Health check test... ");
    status = hal_rs485_health_check();
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS\n");
    } else {
        printf("⚠️  Health check failed (status: %d)\n", status);
    }
    
    // Get and print statistics
    rs485_statistics_t stats;
    status = hal_rs485_get_statistics(&stats);
    if (status == HAL_STATUS_OK) {
        print_statistics(&stats);
    }
    
    // Cleanup
    hal_rs485_close();
    hal_rs485_deinit();
    
    return 0;
}

int main(int argc, char *argv[]) {
    printf("=== HAL RS485 Detailed Test Program - OHT-50 ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("RS485 Device: %s\n", RS485_DEVICE_PATH);
    printf("Baud Rate: %d\n", RS485_BAUD_RATE);
    printf("Press Ctrl+C to stop tests\n");
    printf("\n");
    
    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Run tests
    if (test_rs485_loopback() == 0) {
        passed_tests++;
    }
    total_tests++;
    
    if (test_rs485_transmit_only() == 0) {
        passed_tests++;
    }
    total_tests++;
    
    if (test_rs485_error_handling() == 0) {
        passed_tests++;
    }
    total_tests++;
    
    printf("\n🎯 Final Test Summary\n");
    printf("====================\n");
    printf("Total Test Suites: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success Rate: %.1f%%\n", (float)passed_tests / total_tests * 100.0f);
    
    if (passed_tests == total_tests) {
        printf("\n🎉 All test suites PASSED! HAL RS485 implementation is working correctly.\n");
        return 0;
    } else {
        printf("\n⚠️  Some test suites FAILED. Please check the implementation.\n");
        return 1;
    }
}
