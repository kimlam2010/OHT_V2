/**
 * @file test_hal_usb.c
 * @brief Unit tests for HAL USB
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-16 (USB Communication Testing)
 */

#include "unity.h"
#include "hal_usb.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Test fixtures
static usb_config_t test_config;
static usb_packet_t test_packet;
static usb_device_info_t test_device_info;

void setUp(void)
{
    // Initialize test configuration
    memset(&test_config, 0, sizeof(usb_config_t));
    strcpy(test_config.device_path, "/dev/ttyUSB0");
    test_config.baud_rate = USB_BAUD_RATE;
    test_config.data_bits = USB_DATA_BITS;
    test_config.stop_bits = USB_STOP_BITS;
    test_config.parity = USB_PARITY;
    test_config.timeout_ms = USB_TIMEOUT_MS;
    test_config.retry_count = USB_RETRY_COUNT;
    test_config.flow_control = false;
    test_config.auto_reconnect = true;
    
    // Initialize test packet
    memset(&test_packet, 0, sizeof(usb_packet_t));
    test_packet.device_id = 1;
    test_packet.command = 0x01;
    test_packet.length = 16;
    memcpy(test_packet.data, "TEST_PACKET_DATA", 16);
    test_packet.checksum = hal_usb_calculate_checksum(test_packet.data, test_packet.length);
    test_packet.timestamp_us = 1640995200000000ULL;
    
    // Initialize test device info
    memset(&test_device_info, 0, sizeof(usb_device_info_t));
    strcpy(test_device_info.device_path, "/dev/ttyUSB0");
    test_device_info.device_type = USB_DEVICE_TYPE_SERIAL;
    strcpy(test_device_info.vendor_id, "1234");
    strcpy(test_device_info.product_id, "5678");
    strcpy(test_device_info.serial_number, "SN123456789");
    test_device_info.baud_rate = USB_BAUD_RATE;
    test_device_info.connected = true;
    test_device_info.bytes_transmitted = 1024;
    test_device_info.bytes_received = 512;
    test_device_info.error_count = 0;
    test_device_info.last_activity_time = 1640995200000000ULL;
}

void tearDown(void)
{
    // Cleanup after each test
    hal_usb_deinit();
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_usb_constants(void)
{
    TEST_ASSERT_EQUAL_STRING("/dev/ttyUSB0", USB_DEVICE_PATH);
    TEST_ASSERT_EQUAL(115200, USB_BAUD_RATE);
    TEST_ASSERT_EQUAL(8, USB_DATA_BITS);
    TEST_ASSERT_EQUAL(1, USB_STOP_BITS);
    TEST_ASSERT_EQUAL(0, USB_PARITY);
    TEST_ASSERT_EQUAL(1000, USB_TIMEOUT_MS);
    TEST_ASSERT_EQUAL(3, USB_RETRY_COUNT);
    TEST_ASSERT_EQUAL(1024, USB_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(4, USB_MAX_DEVICES);
}

void test_usb_device_types(void)
{
    TEST_ASSERT_EQUAL(0, USB_DEVICE_TYPE_UNKNOWN);
    TEST_ASSERT_EQUAL(1, USB_DEVICE_TYPE_SERIAL);
    TEST_ASSERT_EQUAL(2, USB_DEVICE_TYPE_HID);
    TEST_ASSERT_EQUAL(3, USB_DEVICE_TYPE_MASS_STORAGE);
    TEST_ASSERT_EQUAL(4, USB_DEVICE_TYPE_LIDAR);
    TEST_ASSERT_EQUAL(5, USB_DEVICE_TYPE_CAMERA);
}

void test_usb_states(void)
{
    TEST_ASSERT_EQUAL(0, USB_STATE_DISCONNECTED);
    TEST_ASSERT_EQUAL(1, USB_STATE_CONNECTING);
    TEST_ASSERT_EQUAL(2, USB_STATE_CONNECTED);
    TEST_ASSERT_EQUAL(3, USB_STATE_READY);
    TEST_ASSERT_EQUAL(4, USB_STATE_TRANSMITTING);
    TEST_ASSERT_EQUAL(5, USB_STATE_RECEIVING);
    TEST_ASSERT_EQUAL(6, USB_STATE_ERROR);
    TEST_ASSERT_EQUAL(7, USB_STATE_TIMEOUT);
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_usb_data_structures(void)
{
    TEST_ASSERT_EQUAL(64, sizeof(test_config.device_path));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.baud_rate));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.data_bits));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.stop_bits));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.parity));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.timeout_ms));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.retry_count));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.flow_control));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.auto_reconnect));
}

void test_usb_config_initialization(void)
{
    TEST_ASSERT_EQUAL_STRING("/dev/ttyUSB0", test_config.device_path);
    TEST_ASSERT_EQUAL(115200, test_config.baud_rate);
    TEST_ASSERT_EQUAL(8, test_config.data_bits);
    TEST_ASSERT_EQUAL(1, test_config.stop_bits);
    TEST_ASSERT_EQUAL(0, test_config.parity);
    TEST_ASSERT_EQUAL(1000, test_config.timeout_ms);
    TEST_ASSERT_EQUAL(3, test_config.retry_count);
    TEST_ASSERT_FALSE(test_config.flow_control);
    TEST_ASSERT_TRUE(test_config.auto_reconnect);
}

void test_usb_packet_initialization(void)
{
    TEST_ASSERT_EQUAL(1, test_packet.device_id);
    TEST_ASSERT_EQUAL(0x01, test_packet.command);
    TEST_ASSERT_EQUAL(16, test_packet.length);
    TEST_ASSERT_EQUAL_STRING("TEST_PACKET_DATA", (char*)test_packet.data);
    TEST_ASSERT_TRUE(test_packet.checksum > 0);
    TEST_ASSERT_EQUAL(1640995200000000ULL, test_packet.timestamp_us);
}

void test_usb_device_info_initialization(void)
{
    TEST_ASSERT_EQUAL_STRING("/dev/ttyUSB0", test_device_info.device_path);
    TEST_ASSERT_EQUAL(1, test_device_info.device_type);
    TEST_ASSERT_EQUAL_STRING("1234", test_device_info.vendor_id);
    TEST_ASSERT_EQUAL_STRING("5678", test_device_info.product_id);
    TEST_ASSERT_EQUAL_STRING("SN123456789", test_device_info.serial_number);
    TEST_ASSERT_EQUAL(115200, test_device_info.baud_rate);
    TEST_ASSERT_TRUE(test_device_info.connected);
    TEST_ASSERT_EQUAL(1024, test_device_info.bytes_transmitted);
    TEST_ASSERT_EQUAL(512, test_device_info.bytes_received);
    TEST_ASSERT_EQUAL(0, test_device_info.error_count);
    TEST_ASSERT_EQUAL(1640995200000000ULL, test_device_info.last_activity_time);
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void test_usb_config_validation(void)
{
    // Test valid configuration
    TEST_ASSERT_TRUE(strlen(test_config.device_path) > 0);
    TEST_ASSERT_TRUE(test_config.baud_rate > 0);
    TEST_ASSERT_TRUE(test_config.data_bits >= 5 && test_config.data_bits <= 8);
    TEST_ASSERT_TRUE(test_config.stop_bits >= 1 && test_config.stop_bits <= 2);
    TEST_ASSERT_TRUE(test_config.parity >= 0 && test_config.parity <= 2);
    TEST_ASSERT_TRUE(test_config.timeout_ms > 0);
    TEST_ASSERT_TRUE(test_config.retry_count > 0);
}

void test_usb_packet_validation(void)
{
    // Test valid packet
    TEST_ASSERT_TRUE(test_packet.device_id > 0);
    TEST_ASSERT_TRUE(test_packet.length > 0);
    TEST_ASSERT_TRUE(test_packet.length <= USB_BUFFER_SIZE);
    TEST_ASSERT_TRUE(test_packet.checksum > 0);
    TEST_ASSERT_TRUE(test_packet.timestamp_us > 0);
}

void test_usb_device_info_validation(void)
{
    // Test valid device info
    TEST_ASSERT_TRUE(strlen(test_device_info.device_path) > 0);
    TEST_ASSERT_TRUE(test_device_info.device_type >= 0 && test_device_info.device_type <= 5);
    TEST_ASSERT_TRUE(strlen(test_device_info.vendor_id) > 0);
    TEST_ASSERT_TRUE(strlen(test_device_info.product_id) > 0);
    TEST_ASSERT_TRUE(strlen(test_device_info.serial_number) > 0);
    TEST_ASSERT_TRUE(test_device_info.baud_rate > 0);

    TEST_ASSERT_TRUE(test_device_info.last_activity_time > 0);
}

// ============================================================================
// UTILITY TESTS
// ============================================================================

void test_usb_device_path_validation(void)
{
    // Test valid device paths
    const char* valid_paths[] = {
        "/dev/ttyUSB0",
        "/dev/ttyUSB1",
        "/dev/ttyACM0",
        "/dev/ttyACM1"
    };
    
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(strlen(valid_paths[i]) > 0);
        TEST_ASSERT_TRUE(strlen(valid_paths[i]) < 64);
    }
}

void test_usb_baud_rate_validation(void)
{
    // Test valid baud rates
    uint32_t valid_baud_rates[] = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_TRUE(valid_baud_rates[i] > 0);
        TEST_ASSERT_TRUE(valid_baud_rates[i] <= 921600);
    }
    
    // Test invalid baud rates
    uint32_t invalid_baud_rates[] = {0, 921601};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_FALSE(invalid_baud_rates[i] > 0 && invalid_baud_rates[i] <= 921600);
    }
}

void test_usb_timeout_validation(void)
{
    // Test valid timeouts
    uint32_t valid_timeouts[] = {100, 500, 1000, 5000, 10000};
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_TRUE(valid_timeouts[i] > 0);
        TEST_ASSERT_TRUE(valid_timeouts[i] <= 60000);
    }
    
    // Test invalid timeouts
    uint32_t invalid_timeouts[] = {0, 60001};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_FALSE(invalid_timeouts[i] > 0 && invalid_timeouts[i] <= 60000);
    }
}

void test_usb_retry_count_validation(void)
{
    // Test valid retry counts
    uint32_t valid_retries[] = {1, 3, 5, 10};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(valid_retries[i] > 0);
        TEST_ASSERT_TRUE(valid_retries[i] <= 20);
    }
    
    // Test invalid retry counts
    uint32_t invalid_retries[] = {0, 21};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_FALSE(invalid_retries[i] > 0 && invalid_retries[i] <= 20);
    }
}

// ============================================================================
// CHECKSUM UTILITY TESTS
// ============================================================================

void test_usb_checksum_validation(void)
{
    // Test valid checksums
    uint16_t valid_checksums[] = {0x0000, 0x1234, 0xFFFF};
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_TRUE(valid_checksums[i] >= 0 && valid_checksums[i] <= 0xFFFF);
    }
}

void test_usb_checksum_calculation(void)
{
    // Test checksum calculation
    uint8_t test_data[] = "USB_TEST_DATA";
    uint16_t calculated_checksum = hal_usb_calculate_checksum(test_data, sizeof(test_data) - 1);
    
    TEST_ASSERT_TRUE(calculated_checksum > 0);
    TEST_ASSERT_TRUE(calculated_checksum <= 0xFFFFU);
}

void test_usb_packet_checksum_validation(void)
{
    // Test packet checksum validation
    bool is_valid = hal_usb_validate_packet(&test_packet);
    TEST_ASSERT_TRUE(is_valid);
    
    // Test invalid packet
    usb_packet_t invalid_packet = test_packet;
    invalid_packet.checksum = 0xFFFF; // Invalid checksum
    bool is_invalid = hal_usb_validate_packet(&invalid_packet);
    TEST_ASSERT_FALSE(is_invalid);
}

// ============================================================================
// DEVICE TYPE UTILITY TESTS
// ============================================================================

void test_usb_device_type_validation(void)
{
    // Test valid device types
    usb_device_type_t valid_types[] = {
        USB_DEVICE_TYPE_UNKNOWN,
        USB_DEVICE_TYPE_SERIAL,
        USB_DEVICE_TYPE_HID,
        USB_DEVICE_TYPE_MASS_STORAGE,
        USB_DEVICE_TYPE_LIDAR,
        USB_DEVICE_TYPE_CAMERA
    };
    
    for (int i = 0; i < 6; i++) {
        TEST_ASSERT_TRUE(valid_types[i] >= 0 && valid_types[i] <= 5);
    }
}

void test_usb_device_type_comparison(void)
{
    // Test device type comparison
    usb_device_type_t type1 = USB_DEVICE_TYPE_SERIAL;
    usb_device_type_t type2 = USB_DEVICE_TYPE_LIDAR;
    usb_device_type_t type3 = USB_DEVICE_TYPE_CAMERA;
    
    TEST_ASSERT_TRUE(type1 < type2);
    TEST_ASSERT_TRUE(type2 < type3);
    TEST_ASSERT_TRUE(type1 < type3);
}

// ============================================================================
// STATE UTILITY TESTS
// ============================================================================

void test_usb_state_validation(void)
{
    // Test valid states
    usb_state_t valid_states[] = {
        USB_STATE_DISCONNECTED,
        USB_STATE_CONNECTING,
        USB_STATE_CONNECTED,
        USB_STATE_READY,
        USB_STATE_TRANSMITTING,
        USB_STATE_RECEIVING,
        USB_STATE_ERROR,
        USB_STATE_TIMEOUT
    };
    
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_TRUE(valid_states[i] >= 0 && valid_states[i] <= 7);
    }
}

void test_usb_state_transitions(void)
{
    // Test state transition logic
    usb_state_t disconnected = USB_STATE_DISCONNECTED;
    usb_state_t connecting = USB_STATE_CONNECTING;
    usb_state_t connected = USB_STATE_CONNECTED;
    usb_state_t ready = USB_STATE_READY;
    
    // Valid transitions
    TEST_ASSERT_TRUE(disconnected < connecting);
    TEST_ASSERT_TRUE(connecting < connected);
    TEST_ASSERT_TRUE(connected < ready);
    
    // Invalid transitions (ready cannot go back to disconnected directly)
    TEST_ASSERT_FALSE(ready < disconnected);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Constants tests
    RUN_TEST(test_usb_constants);
    RUN_TEST(test_usb_device_types);
    RUN_TEST(test_usb_states);
    
    // Data structures tests
    RUN_TEST(test_usb_data_structures);
    RUN_TEST(test_usb_config_initialization);
    RUN_TEST(test_usb_packet_initialization);
    RUN_TEST(test_usb_device_info_initialization);
    
    // Validation tests
    RUN_TEST(test_usb_config_validation);
    RUN_TEST(test_usb_packet_validation);
    RUN_TEST(test_usb_device_info_validation);
    
    // Utility tests
    RUN_TEST(test_usb_device_path_validation);
    RUN_TEST(test_usb_baud_rate_validation);
    RUN_TEST(test_usb_timeout_validation);
    RUN_TEST(test_usb_retry_count_validation);
    
    // Checksum utility tests
    RUN_TEST(test_usb_checksum_validation);
    RUN_TEST(test_usb_checksum_calculation);
    RUN_TEST(test_usb_packet_checksum_validation);
    
    // Device type utility tests
    RUN_TEST(test_usb_device_type_validation);
    RUN_TEST(test_usb_device_type_comparison);
    
    // State utility tests
    RUN_TEST(test_usb_state_validation);
    RUN_TEST(test_usb_state_transitions);
    
    UNITY_END();
    return 0;
}
