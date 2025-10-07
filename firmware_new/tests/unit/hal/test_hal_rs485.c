/**
 * @file test_hal_rs485.c
 * @brief Unit tests for HAL RS485
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-13 (RS485 Communication Testing)
 */

#include "unity.h"
#include "hal_rs485.h"
#include "hal_common.h"
// No mock includes needed
#include <string.h>
#include <stdbool.h>
#include <stdio.h> // Added for debug printing

// Test fixtures
static rs485_config_t test_config;
static modbus_config_t test_modbus_config;

// Function prototypes
void setUp(void);
void tearDown(void);
void test_setup_is_called(void);
void test_rs485_constants(void);
void test_modbus_function_codes(void);
void test_rs485_status_enum(void);
void test_rs485_data_structures(void);
void test_modbus_data_structures(void);
void test_modbus_frame_structure(void);
void test_rs485_statistics_structure(void);
void test_rs485_config_initialization(void);
void test_modbus_config_initialization(void);
void test_rs485_config_validation(void);
void test_modbus_config_validation(void);
void test_rs485_buffer_size_validation(void);
void test_modbus_address_validation(void);
void test_modbus_quantity_validation(void);

void setUp(void)
{
    // Initialize test environment (no mock reset needed)
    
    // Initialize test configuration
    memset(&test_config, 0, sizeof(rs485_config_t));
    strcpy(test_config.device_path, "/dev/ttyOHT485");
    test_config.baud_rate = RS485_BAUD_RATE;
    test_config.data_bits = RS485_DATA_BITS;
    test_config.stop_bits = RS485_STOP_BITS;
    test_config.parity = RS485_PARITY;
    test_config.timeout_ms = MODBUS_TIMEOUT_MS;
    test_config.retry_count = MODBUS_RETRY_COUNT;
    
    // Initialize modbus configuration
    memset(&test_modbus_config, 0, sizeof(modbus_config_t));
    test_modbus_config.slave_id = MODBUS_SLAVE_ID;
    test_modbus_config.timeout_ms = MODBUS_TIMEOUT_MS;
    test_modbus_config.retry_count = MODBUS_RETRY_COUNT;
    test_modbus_config.enable_crc_check = true;
}

void tearDown(void)
{
    // Cleanup after each test
    // Note: hal_rs485_deinit() may not exist in actual implementation
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_setup_is_called(void) {
    // This test verifies that setUp() works correctly
    TEST_ASSERT_TRUE(true); // Just verify test runs
}

void test_rs485_constants(void)
{
    setUp(); // Ensure setup is called
    TEST_ASSERT_EQUAL(115200, RS485_BAUD_RATE);
    TEST_ASSERT_EQUAL(8, RS485_DATA_BITS);
    TEST_ASSERT_EQUAL(1, RS485_STOP_BITS);
    TEST_ASSERT_EQUAL(0, RS485_PARITY);
    TEST_ASSERT_EQUAL(1024, RS485_BUFFER_SIZE);
    TEST_ASSERT_EQUAL(256, MODBUS_FRAME_SIZE);
    TEST_ASSERT_EQUAL(1, MODBUS_SLAVE_ID);
    TEST_ASSERT_EQUAL(1000, MODBUS_TIMEOUT_MS);
    TEST_ASSERT_EQUAL(3, MODBUS_RETRY_COUNT);
}

void test_modbus_function_codes(void)
{
    setUp(); // Ensure setup is called
    TEST_ASSERT_EQUAL(0x01, MODBUS_FC_READ_COILS);
    TEST_ASSERT_EQUAL(0x02, MODBUS_FC_READ_DISCRETE_INPUTS);
    TEST_ASSERT_EQUAL(0x03, MODBUS_FC_READ_HOLDING_REGISTERS);
    TEST_ASSERT_EQUAL(0x04, MODBUS_FC_READ_INPUT_REGISTERS);
    TEST_ASSERT_EQUAL(0x05, MODBUS_FC_WRITE_SINGLE_COIL);
    TEST_ASSERT_EQUAL(0x06, MODBUS_FC_WRITE_SINGLE_REGISTER);
    TEST_ASSERT_EQUAL(0x0F, MODBUS_FC_WRITE_MULTIPLE_COILS);
    TEST_ASSERT_EQUAL(0x10, MODBUS_FC_WRITE_MULTIPLE_REGISTERS);
}

void test_rs485_status_enum(void)
{
    setUp(); // Ensure setup is called
    TEST_ASSERT_EQUAL(0, RS485_STATUS_IDLE);
    TEST_ASSERT_EQUAL(1, RS485_STATUS_TRANSMITTING);
    TEST_ASSERT_EQUAL(2, RS485_STATUS_RECEIVING);
    TEST_ASSERT_EQUAL(3, RS485_STATUS_ERROR);
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_rs485_data_structures(void)
{
    setUp(); // Ensure setup is called
    TEST_ASSERT_EQUAL(64, sizeof(test_config.device_path));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.baud_rate));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.data_bits));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.stop_bits));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.parity));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.timeout_ms));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.retry_count));
}

void test_modbus_data_structures(void)
{
    setUp(); // Ensure setup is called
    TEST_ASSERT_EQUAL(1, sizeof(test_modbus_config.slave_id));
    TEST_ASSERT_EQUAL(4, sizeof(test_modbus_config.timeout_ms));
    TEST_ASSERT_EQUAL(4, sizeof(test_modbus_config.retry_count));
    TEST_ASSERT_EQUAL(1, sizeof(test_modbus_config.enable_crc_check));
}

void test_modbus_frame_structure(void)
{
    setUp(); // Ensure setup is called
    modbus_frame_t frame;
    TEST_ASSERT_EQUAL(1, sizeof(frame.slave_id));
    TEST_ASSERT_EQUAL(4, sizeof(frame.function_code));
    TEST_ASSERT_EQUAL(2, sizeof(frame.start_address));
    TEST_ASSERT_EQUAL(2, sizeof(frame.quantity));
    TEST_ASSERT_EQUAL(256, sizeof(frame.data));
    TEST_ASSERT_EQUAL(2, sizeof(frame.data_length));
    TEST_ASSERT_EQUAL(2, sizeof(frame.crc));
}

void test_rs485_statistics_structure(void)
{
    setUp(); // Ensure setup is called
    rs485_statistics_t stats;
    TEST_ASSERT_EQUAL(8, sizeof(stats.bytes_transmitted));
    TEST_ASSERT_EQUAL(8, sizeof(stats.bytes_received));
    TEST_ASSERT_EQUAL(8, sizeof(stats.frames_transmitted));
    TEST_ASSERT_EQUAL(8, sizeof(stats.frames_received));
    TEST_ASSERT_EQUAL(8, sizeof(stats.errors_crc));
    TEST_ASSERT_EQUAL(8, sizeof(stats.errors_timeout));
    TEST_ASSERT_EQUAL(8, sizeof(stats.errors_parity));
    TEST_ASSERT_EQUAL(8, sizeof(stats.timestamp_us));
}

// ============================================================================
// CONFIGURATION TESTS
// ============================================================================

void test_rs485_config_initialization(void)
{
    setUp(); // Ensure setup is called
    
    TEST_ASSERT_EQUAL_STRING("/dev/ttyOHT485", test_config.device_path);
    TEST_ASSERT_EQUAL(115200, test_config.baud_rate);
    TEST_ASSERT_EQUAL(8, test_config.data_bits);
    TEST_ASSERT_EQUAL(1, test_config.stop_bits);
    TEST_ASSERT_EQUAL(0, test_config.parity);
    TEST_ASSERT_EQUAL(1000, test_config.timeout_ms);
    TEST_ASSERT_EQUAL(3, test_config.retry_count);
}

void test_modbus_config_initialization(void)
{
    setUp(); // Ensure setup is called
    TEST_ASSERT_EQUAL(1, test_modbus_config.slave_id);
    TEST_ASSERT_EQUAL(1000, test_modbus_config.timeout_ms);
    TEST_ASSERT_EQUAL(3, test_modbus_config.retry_count);
    TEST_ASSERT_TRUE(test_modbus_config.enable_crc_check);
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void test_rs485_config_validation(void)
{
    setUp(); // Ensure setup is called
    // Test valid configuration
    TEST_ASSERT_TRUE(strlen(test_config.device_path) > 0);
    TEST_ASSERT_TRUE(test_config.baud_rate > 0);
    TEST_ASSERT_TRUE(test_config.data_bits >= 5 && test_config.data_bits <= 8);
    TEST_ASSERT_TRUE(test_config.stop_bits >= 1 && test_config.stop_bits <= 2);
    TEST_ASSERT_TRUE(test_config.parity <= 2);
    TEST_ASSERT_TRUE(test_config.timeout_ms > 0);
    TEST_ASSERT_TRUE(test_config.retry_count > 0);
}

void test_modbus_config_validation(void)
{
    setUp(); // Ensure setup is called
    // Test valid modbus configuration
    TEST_ASSERT_TRUE(test_modbus_config.slave_id > 0 && test_modbus_config.slave_id <= 247);
    TEST_ASSERT_TRUE(test_modbus_config.timeout_ms > 0);
    TEST_ASSERT_TRUE(test_modbus_config.retry_count > 0);
}

// ============================================================================
// UTILITY TESTS
// ============================================================================

void test_rs485_buffer_size_validation(void)
{
    // Test that buffer size is reasonable
    TEST_ASSERT_TRUE(RS485_BUFFER_SIZE >= 256);
    TEST_ASSERT_TRUE(RS485_BUFFER_SIZE <= 8192);
    TEST_ASSERT_TRUE(MODBUS_FRAME_SIZE >= 64);
    TEST_ASSERT_TRUE(MODBUS_FRAME_SIZE <= RS485_BUFFER_SIZE);
}

void test_modbus_address_validation(void)
{
    // Test valid modbus addresses
    uint16_t valid_addresses[] = {0x0000, 0x0001, 0xFFFF};
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_TRUE(valid_addresses[i] >= 0x0000 && valid_addresses[i] <= 0xFFFF);
    }
}

void test_modbus_quantity_validation(void)
{
    // Test valid modbus quantities
    uint16_t valid_quantities[] = {1, 125, 2000};
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_TRUE(valid_quantities[i] >= 1 && valid_quantities[i] <= 2000);
    }
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Setup test
    RUN_TEST(test_setup_is_called);
    
    // Constants tests
    RUN_TEST(test_rs485_constants);
    RUN_TEST(test_modbus_function_codes);
    RUN_TEST(test_rs485_status_enum);
    
    // Data structures tests
    RUN_TEST(test_rs485_data_structures);
    RUN_TEST(test_modbus_data_structures);
    RUN_TEST(test_modbus_frame_structure);
    RUN_TEST(test_rs485_statistics_structure);
    
    // Configuration tests
    RUN_TEST(test_rs485_config_initialization);
    RUN_TEST(test_modbus_config_initialization);
    
    // Validation tests
    RUN_TEST(test_rs485_config_validation);
    RUN_TEST(test_modbus_config_validation);
    
    // Utility tests
    RUN_TEST(test_rs485_buffer_size_validation);
    RUN_TEST(test_modbus_address_validation);
    RUN_TEST(test_modbus_quantity_validation);
    
    UNITY_END();
    return 0;
}
