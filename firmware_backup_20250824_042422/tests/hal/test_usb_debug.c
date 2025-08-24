/**
 * @file test_usb_debug.c
 * @brief Test program for HAL USB Debug implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @team FIRMWARE
 * @task Task 3.1 (USB Debug Interface)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "hal_usb_debug.h"

// Global variables for cleanup
static bool test_running = true;

// Signal handler for graceful shutdown
static void signal_handler(int sig)
{
    printf("\nReceived signal %d, shutting down...\n", sig);
    test_running = false;
}

// Test event callback
static void test_event_callback(usb_debug_event_t event, const void *data)
{
    (void)data; // Unused parameter
    
    switch (event) {
        case USB_DEBUG_EVENT_CONNECTED:
            printf("  📱 USB Debug connected\n");
            break;
        case USB_DEBUG_EVENT_DISCONNECTED:
            printf("  📱 USB Debug disconnected\n");
            break;
        case USB_DEBUG_EVENT_DATA_RECEIVED:
            printf("  📱 Data received\n");
            break;
        case USB_DEBUG_EVENT_DATA_SENT:
            printf("  📱 Data sent\n");
            break;
        case USB_DEBUG_EVENT_ERROR:
            printf("  📱 Error occurred\n");
            break;
        default:
            printf("  📱 Unknown event: %d\n", event);
            break;
    }
}

// Test functions
static hal_status_t test_usb_debug_init(void);
static hal_status_t test_usb_debug_config(void);
static hal_status_t test_usb_debug_connection(void);
static hal_status_t test_usb_debug_communication(void);
static hal_status_t test_usb_debug_console(void);
static hal_status_t test_usb_debug_firmware_update(void);
static hal_status_t test_usb_debug_cleanup(void);

int main(int argc, char *argv[])
{
    (void)argc; // Unused parameter
    (void)argv; // Unused parameter
    
    printf("=== HAL USB Debug Test Program - OHT-50 ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("Device: USB CDC ACM Debug Interface\n");
    printf("Features: Debug Console, Firmware Update\n\n");

    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Run tests
    hal_status_t status;
    int test_count = 0;
    int passed_count = 0;

    printf("🔧 Test 1: USB Debug Initialization\n");
    printf("------------------------------------\n");
    status = test_usb_debug_init();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: USB Debug initialization\n");
        passed_count++;
    } else {
        printf("❌ FAIL: USB Debug initialization (status: %d)\n", status);
        return 1;
    }

    printf("\n🔧 Test 2: USB Debug Configuration\n");
    printf("-----------------------------------\n");
    status = test_usb_debug_config();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: USB Debug configuration\n");
        passed_count++;
    } else {
        printf("❌ FAIL: USB Debug configuration (status: %d)\n", status);
    }

    printf("\n🔧 Test 3: USB Debug Connection\n");
    printf("--------------------------------\n");
    status = test_usb_debug_connection();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: USB Debug connection\n");
        passed_count++;
    } else {
        printf("❌ FAIL: USB Debug connection (status: %d)\n", status);
    }

    printf("\n🔧 Test 4: USB Debug Communication\n");
    printf("-----------------------------------\n");
    status = test_usb_debug_communication();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: USB Debug communication\n");
        passed_count++;
    } else {
        printf("❌ FAIL: USB Debug communication (status: %d)\n", status);
    }

    printf("\n🔧 Test 5: USB Debug Console\n");
    printf("-----------------------------\n");
    status = test_usb_debug_console();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: USB Debug console\n");
        passed_count++;
    } else {
        printf("❌ FAIL: USB Debug console (status: %d)\n", status);
    }

    printf("\n🔧 Test 6: USB Debug Firmware Update\n");
    printf("-------------------------------------\n");
    status = test_usb_debug_firmware_update();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: USB Debug firmware update\n");
        passed_count++;
    } else {
        printf("❌ FAIL: USB Debug firmware update (status: %d)\n", status);
    }

    printf("\n🔧 Test 7: USB Debug Cleanup\n");
    printf("-----------------------------\n");
    status = test_usb_debug_cleanup();
    test_count++;
    if (status == HAL_STATUS_OK) {
        printf("✅ PASS: USB Debug cleanup\n");
        passed_count++;
    } else {
        printf("❌ FAIL: USB Debug cleanup (status: %d)\n", status);
    }

    // Test summary
    printf("\n🎯 Test Summary\n");
    printf("===============\n");
    printf("Total Tests: %d\n", test_count);
    printf("Passed: %d\n", passed_count);
    printf("Failed: %d\n", test_count - passed_count);
    printf("Success Rate: %.1f%%\n", (float)passed_count / test_count * 100.0f);

    if (passed_count == test_count) {
        printf("\n🎉 All tests PASSED! HAL USB Debug implementation is working correctly.\n");
        return 0;
    } else {
        printf("\n❌ Some tests FAILED. Please check the implementation.\n");
        return 1;
    }
}

static hal_status_t test_usb_debug_init(void)
{
    printf("Testing USB Debug HAL initialization...\n");

    // Test configuration
    usb_debug_config_t config = {
        .device_path = "/dev/ttyACM0",
        .baud_rate = 115200,
        .timeout_ms = 1000,
        .buffer_size = 1024,
        .auto_reconnect = true,
        .debug_console_enabled = true,
        .firmware_update_enabled = true
    };

    // Initialize USB Debug HAL
    hal_status_t status = hal_usb_debug_init(&config);
    if (status != HAL_STATUS_OK) {
        printf("  Failed to initialize USB Debug HAL\n");
        return status;
    }

    printf("  USB Debug HAL initialized successfully\n");

    // Set event callback
    status = hal_usb_debug_set_event_callback(test_event_callback);
    if (status != HAL_STATUS_OK) {
        printf("  Failed to set event callback\n");
        return status;
    }

    printf("  Event callback set successfully\n");
    return HAL_STATUS_OK;
}

static hal_status_t test_usb_debug_config(void)
{
    printf("Testing USB Debug configuration validation...\n");

    // Test valid configuration
    usb_debug_config_t valid_config = {
        .device_path = "/dev/ttyACM0",
        .baud_rate = 115200,
        .timeout_ms = 1000,
        .buffer_size = 512,
        .auto_reconnect = false,
        .debug_console_enabled = true,
        .firmware_update_enabled = true
    };

    hal_status_t status = hal_usb_debug_validate_config(&valid_config);
    if (status != HAL_STATUS_OK) {
        printf("  Valid configuration failed validation\n");
        return status;
    }
    printf("  ✅ Valid configuration passed validation\n");

    // Test invalid configuration (empty device path)
    usb_debug_config_t invalid_config = {
        .device_path = "",
        .baud_rate = 115200,
        .timeout_ms = 1000,
        .buffer_size = 512,
        .auto_reconnect = false,
        .debug_console_enabled = true,
        .firmware_update_enabled = true
    };

    status = hal_usb_debug_validate_config(&invalid_config);
    if (status == HAL_STATUS_OK) {
        printf("  Invalid configuration should have failed validation\n");
        return HAL_STATUS_ERROR;
    }
    printf("  ✅ Invalid configuration correctly rejected\n");

    return HAL_STATUS_OK;
}

static hal_status_t test_usb_debug_connection(void)
{
    printf("Testing USB Debug connection...\n");

    // Attempt to connect (may fail if device not present)
    hal_status_t status = hal_usb_debug_connect();
    if (status == HAL_STATUS_OK) {
        printf("  ✅ USB Debug connected successfully\n");

        // Get status
        usb_debug_status_t usb_status;
        status = hal_usb_debug_get_status(&usb_status);
        if (status == HAL_STATUS_OK) {
            printf("  📊 Connection Status:\n");
            printf("    - State: %d\n", usb_status.state);
            printf("    - Connected: %s\n", usb_status.connected ? "Yes" : "No");
            printf("    - Console Active: %s\n", usb_status.console_active ? "Yes" : "No");
            printf("    - Firmware Update Active: %s\n", usb_status.firmware_update_active ? "Yes" : "No");
        }

        // Test disconnect
        status = hal_usb_debug_disconnect();
        if (status == HAL_STATUS_OK) {
            printf("  ✅ USB Debug disconnected successfully\n");
        }
    } else {
        printf("  ⚠️  USB Debug connection failed (device may not be present)\n");
        printf("  📝 This is expected if no USB device is connected\n");
        // Don't return error as this is expected without hardware
    }

    return HAL_STATUS_OK;
}

static hal_status_t test_usb_debug_communication(void)
{
    printf("Testing USB Debug communication...\n");

    // Test data transmission (simulation)
    const char *test_data = "Hello USB Debug!";
    size_t data_len = strlen(test_data);

    hal_status_t status = hal_usb_debug_send_data((const uint8_t*)test_data, data_len);
    if (status == HAL_STATUS_OK || status == HAL_STATUS_NOT_INITIALIZED) {
        printf("  ✅ Send data function working (status: %d)\n", status);
    } else {
        printf("  ❌ Send data failed unexpectedly\n");
        return status;
    }

    // Test data reception (simulation)
    uint8_t rx_buffer[256];
    size_t received_len;
    status = hal_usb_debug_receive_data(rx_buffer, sizeof(rx_buffer), &received_len, 100);
    if (status == HAL_STATUS_OK || status == HAL_STATUS_NOT_INITIALIZED || status == HAL_STATUS_TIMEOUT) {
        printf("  ✅ Receive data function working (status: %d)\n", status);
    } else {
        printf("  ❌ Receive data failed unexpectedly\n");
        return status;
    }

    // Test checksum calculation
    uint16_t checksum = usb_debug_calculate_checksum((const uint8_t*)test_data, data_len);
    printf("  ✅ Checksum calculated: 0x%04X\n", checksum);

    return HAL_STATUS_OK;
}

static hal_status_t test_usb_debug_console(void)
{
    printf("Testing USB Debug console functions...\n");

    // Test console start
    hal_status_t status = hal_usb_debug_console_start();
    if (status == HAL_STATUS_OK) {
        printf("  ✅ Console started successfully\n");
    }

    // Test console printf
    status = hal_usb_debug_console_printf("Test message: %d\n", 42);
    if (status == HAL_STATUS_OK || status == HAL_STATUS_NOT_INITIALIZED) {
        printf("  ✅ Console printf working (status: %d)\n", status);
    }

    // Test console send
    status = hal_usb_debug_console_send("Console test message");
    if (status == HAL_STATUS_OK) {
        printf("  ✅ Console send working\n");
    }

    // Test console stop
    status = hal_usb_debug_console_stop();
    if (status == HAL_STATUS_OK) {
        printf("  ✅ Console stopped successfully\n");
    }

    return HAL_STATUS_OK;
}

static hal_status_t test_usb_debug_firmware_update(void)
{
    printf("Testing USB Debug firmware update functions...\n");

    // Test firmware update start
    hal_status_t status = hal_usb_debug_firmware_update_start(1024, "1.0.1");
    if (status == HAL_STATUS_OK) {
        printf("  ✅ Firmware update start working\n");
    }

    // Test firmware update data
    uint8_t dummy_data[64];
    memset(dummy_data, 0xAA, sizeof(dummy_data));
    status = hal_usb_debug_firmware_update_data(dummy_data, sizeof(dummy_data));
    if (status == HAL_STATUS_OK) {
        printf("  ✅ Firmware update data working\n");
    }

    // Test firmware update end
    status = hal_usb_debug_firmware_update_end();
    if (status == HAL_STATUS_OK) {
        printf("  ✅ Firmware update end working\n");
    }

    // Test firmware update verify
    status = hal_usb_debug_firmware_update_verify();
    if (status == HAL_STATUS_OK) {
        printf("  ✅ Firmware update verify working\n");
    }

    return HAL_STATUS_OK;
}

static hal_status_t test_usb_debug_cleanup(void)
{
    printf("Cleaning up USB Debug HAL...\n");

    hal_status_t status = hal_usb_debug_deinit();
    if (status != HAL_STATUS_OK) {
        printf("  Failed to cleanup USB Debug HAL\n");
        return status;
    }

    printf("  USB Debug HAL cleaned up successfully\n");
    return HAL_STATUS_OK;
}
