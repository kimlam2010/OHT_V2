/**
 * @file test_power_module_handler.c
 * @brief Unit tests for Power Module Handler
 * @version 2.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include "unity.h"
#include "power_module_handler.h"
#include "hal_common.h"
#include "hal_rs485.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Test fixtures
static power_module_config_t test_config;
static power_module_data_t test_data;
static power_module_status_t test_status;

// Mock functions - replace real HAL functions
#define hal_rs485_transmit mock_hal_rs485_transmit
#define hal_rs485_receive mock_hal_rs485_receive
#define modbus_calculate_crc mock_modbus_calculate_crc

static hal_status_t mock_hal_rs485_transmit(const uint8_t *data, size_t length);
static hal_status_t mock_hal_rs485_receive(uint8_t *data, size_t max_length, size_t *received_length);
static uint16_t mock_modbus_calculate_crc(const uint8_t *data, size_t length);

// Test data
static uint8_t mock_tx_buffer[256];
static uint8_t mock_rx_buffer[256];
static size_t mock_tx_length = 0;
static size_t mock_rx_length = 0;
static bool mock_communication_success = true;

void setUp(void)
{
    // Initialize test configuration
    memset(&test_config, 0, sizeof(power_module_config_t));
    test_config.slave_id = 0x02;
    test_config.baudrate_code = 3; // 38400 baud
    test_config.parity = 1; // Even parity
    test_config.stop_bits = 1;
    test_config.fc_mask = 0x07; // FC3, FC6, FC16
    
    // Battery thresholds
    test_config.max_cell_threshold_1 = 4200; // 4.2V
    test_config.min_cell_threshold_1 = 3000; // 3.0V
    test_config.max_cell_threshold_2 = 4300; // 4.3V
    test_config.min_cell_threshold_2 = 2900; // 2.9V
    test_config.max_pack_threshold_1 = 50000; // 50V
    test_config.min_pack_threshold_1 = 40000; // 40V
    test_config.max_pack_threshold_2 = 52000; // 52V
    test_config.min_pack_threshold_2 = 38000; // 38V
    
    // Charging configuration
    test_config.charge_voltage_set = 12.6f;
    test_config.charge_current_set = 2.0f;
    test_config.charge_enable = true;
    
    // Output configuration
    test_config.output_12v_enabled = true;
    test_config.output_5v_enabled = true;
    test_config.output_3v3_enabled = true;
    
    // Protection thresholds
    test_config.overvoltage_threshold = 13.0f;
    test_config.overcurrent_threshold = 5.0f;
    test_config.overtemp_threshold = 60.0f;
    
    // Voltage usage threshold
    test_config.use_v_ths = 0.8f;
    
    // Clear test data
    memset(&test_data, 0, sizeof(power_module_data_t));
    memset(&test_status, 0, sizeof(power_module_status_t));
    
    // Reset mock data
    mock_tx_length = 0;
    mock_rx_length = 0;
    mock_communication_success = true;
    memset(mock_tx_buffer, 0, sizeof(mock_tx_buffer));
    memset(mock_rx_buffer, 0, sizeof(mock_rx_buffer));
}

void tearDown(void)
{
    // Cleanup
    power_module_handler_deinit();
}

// Test cases

void test_power_module_handler_init_success(void)
{
    setUp(); // Initialize test configuration
    
    printf("Testing power module handler init with config:\n");
    printf("  slave_id: %d\n", test_config.slave_id);
    printf("  baudrate_code: %d\n", test_config.baudrate_code);
    printf("  parity: %d\n", test_config.parity);
    printf("  stop_bits: %d\n", test_config.stop_bits);
    printf("  max_cell_threshold_1: %d\n", test_config.max_cell_threshold_1);
    printf("  min_cell_threshold_1: %d\n", test_config.min_cell_threshold_1);
    
    hal_status_t status = power_module_handler_init(&test_config);
    printf("Init status: %d (expected: %d)\n", status, HAL_STATUS_OK);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    tearDown(); // Cleanup
}

void test_power_module_handler_init_null_config(void)
{
    hal_status_t status = power_module_handler_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_init_invalid_slave_id(void)
{
    test_config.slave_id = 0; // Invalid slave ID
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_init_invalid_baudrate(void)
{
    test_config.baudrate_code = 10; // Invalid baudrate code
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_init_invalid_thresholds(void)
{
    test_config.max_cell_threshold_1 = 3000; // Max < Min
    test_config.min_cell_threshold_1 = 4200;
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_deinit_success(void)
{
    setUp(); // Initialize test configuration
    
    // First initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Then deinitialize
    status = power_module_handler_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    tearDown(); // Cleanup
}

void test_power_module_handler_deinit_not_initialized(void)
{
    hal_status_t status = power_module_handler_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_read_data_success(void)
{
    setUp(); // Initialize test configuration
    
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Read data
    status = power_module_handler_read_data(&test_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify data structure is initialized
    TEST_ASSERT_EQUAL(0x02, test_data.device_id);
    TEST_ASSERT_EQUAL(0x0200, test_data.firmware_version);
    TEST_ASSERT_EQUAL(0x0100, test_data.hardware_version);
    
    tearDown(); // Cleanup
}

void test_power_module_handler_read_data_null_pointer(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Read data with null pointer
    status = power_module_handler_read_data(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_read_data_not_initialized(void)
{
    hal_status_t status = power_module_handler_read_data(&test_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_write_register_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Write register
    status = power_module_handler_write_register(POWER_REG_V_SET, 126); // 12.6V
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_write_register_not_initialized(void)
{
    hal_status_t status = power_module_handler_write_register(POWER_REG_V_SET, 126);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_read_register_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Read register
    uint16_t value;
    status = power_module_handler_read_register(POWER_REG_BATTERY_VOLTAGE, &value);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_read_register_null_pointer(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Read register with null pointer
    status = power_module_handler_read_register(POWER_REG_BATTERY_VOLTAGE, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_read_register_not_initialized(void)
{
    uint16_t value;
    hal_status_t status = power_module_handler_read_register(POWER_REG_BATTERY_VOLTAGE, &value);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_get_status_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get status
    status = power_module_handler_get_status(&test_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify status structure
    TEST_ASSERT_TRUE(test_status.initialized);
    TEST_ASSERT_FALSE(test_status.online);
    TEST_ASSERT_EQUAL(0, test_status.error_count);
    TEST_ASSERT_EQUAL(0, test_status.warning_count);
}

void test_power_module_handler_get_status_null_pointer(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get status with null pointer
    status = power_module_handler_get_status(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_get_status_not_initialized(void)
{
    hal_status_t status = power_module_handler_get_status(&test_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_set_charging_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Set charging parameters
    status = power_module_handler_set_charging(2.0f, 12.6f, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_set_charging_invalid_current(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Set charging with invalid current
    status = power_module_handler_set_charging(-1.0f, 12.6f, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_set_charging_invalid_voltage(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Set charging with invalid voltage
    status = power_module_handler_set_charging(2.0f, -1.0f, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_set_charging_not_initialized(void)
{
    hal_status_t status = power_module_handler_set_charging(2.0f, 12.6f, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_control_outputs_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Control outputs
    status = power_module_handler_control_outputs(true, true, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_control_outputs_disabled_output(void)
{
    // Initialize with disabled outputs
    test_config.output_12v_enabled = false;
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Try to enable disabled output
    status = power_module_handler_control_outputs(true, false, false);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_control_outputs_not_initialized(void)
{
    hal_status_t status = power_module_handler_control_outputs(true, true, true);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_reset_faults_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Reset faults
    status = power_module_handler_reset_faults();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_reset_faults_not_initialized(void)
{
    hal_status_t status = power_module_handler_reset_faults();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_get_capabilities_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get capabilities
    uint32_t capabilities = power_module_handler_get_capabilities();
    TEST_ASSERT_TRUE(capabilities != 0);
    
    // Verify basic capabilities are set
    TEST_ASSERT_TRUE(capabilities & POWER_CAP_VOLTAGE_MONITOR);
    TEST_ASSERT_TRUE(capabilities & POWER_CAP_CURRENT_MONITOR);
    TEST_ASSERT_TRUE(capabilities & POWER_CAP_TEMP_MONITOR);
    TEST_ASSERT_TRUE(capabilities & POWER_CAP_RELAY_CONTROL);
}

void test_power_module_handler_get_capabilities_not_initialized(void)
{
    uint32_t capabilities = power_module_handler_get_capabilities();
    TEST_ASSERT_EQUAL(0, capabilities);
}

void test_power_module_handler_auto_detect_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Auto detect
    status = power_module_handler_auto_detect(0x02, 1000);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_auto_detect_invalid_slave_id(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Auto detect with invalid slave ID
    status = power_module_handler_auto_detect(0, 1000);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_auto_detect_not_initialized(void)
{
    hal_status_t status = power_module_handler_auto_detect(0x02, 1000);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_get_module_info_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get module info
    uint16_t device_id, module_type, firmware_version, hardware_version;
    status = power_module_handler_get_module_info(&device_id, &module_type, &firmware_version, &hardware_version);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_get_module_info_null_pointer(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get module info with null pointer
    status = power_module_handler_get_module_info(NULL, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_get_module_info_not_initialized(void)
{
    uint16_t device_id, module_type, firmware_version, hardware_version;
    hal_status_t status = power_module_handler_get_module_info(&device_id, &module_type, &firmware_version, &hardware_version);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_get_module_name_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get module name
    char name_buffer[16];
    status = power_module_handler_get_module_name(name_buffer, sizeof(name_buffer));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_get_module_name_null_pointer(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get module name with null pointer
    status = power_module_handler_get_module_name(NULL, 16);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_get_module_name_small_buffer(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get module name with small buffer
    char name_buffer[5]; // Too small
    status = power_module_handler_get_module_name(name_buffer, sizeof(name_buffer));
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_get_module_name_not_initialized(void)
{
    char name_buffer[16];
    hal_status_t status = power_module_handler_get_module_name(name_buffer, sizeof(name_buffer));
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_reset_system_errors_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Reset system errors
    status = power_module_handler_reset_system_errors();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_reset_system_errors_not_initialized(void)
{
    hal_status_t status = power_module_handler_reset_system_errors();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_power_module_handler_get_system_status_success(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get system status
    uint16_t system_status, system_error;
    status = power_module_handler_get_system_status(&system_status, &system_error);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_power_module_handler_get_system_status_null_pointer(void)
{
    // Initialize
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get system status with null pointer
    status = power_module_handler_get_system_status(NULL, NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_power_module_handler_get_system_status_not_initialized(void)
{
    uint16_t system_status, system_error;
    hal_status_t status = power_module_handler_get_system_status(&system_status, &system_error);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Mock function implementations
static hal_status_t mock_hal_rs485_transmit(const uint8_t *data, size_t length)
{
    if (!data || length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Always return success in test environment
    if (length > sizeof(mock_tx_buffer)) {
        return HAL_STATUS_ERROR;
    }
    
    memcpy(mock_tx_buffer, data, length);
    mock_tx_length = length;
    
    return HAL_STATUS_OK;
}

static hal_status_t mock_hal_rs485_receive(uint8_t *data, size_t max_length, size_t *received_length)
{
    if (!data || !received_length || max_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Always return success in test environment
    // Simulate successful Modbus response
    uint8_t mock_response[] = {
        0x02,  // Slave address
        0x03,  // Function code (Read Holding Registers)
        0x02,  // Byte count
        0x00, 0x7B,  // Register value (123)
        0x00, 0x00   // CRC (placeholder)
    };
    
    size_t response_length = sizeof(mock_response);
    if (response_length > max_length) {
        response_length = max_length;
    }
    
    memcpy(data, mock_response, response_length);
    *received_length = response_length;
    
    return HAL_STATUS_OK;
}

static uint16_t mock_modbus_calculate_crc(const uint8_t *data, size_t length)
{
    // Simple mock CRC calculation
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}

// Test runner
int main(void)
{
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_power_module_handler_init_success);
    RUN_TEST(test_power_module_handler_init_null_config);
    RUN_TEST(test_power_module_handler_init_invalid_slave_id);
    RUN_TEST(test_power_module_handler_init_invalid_baudrate);
    RUN_TEST(test_power_module_handler_init_invalid_thresholds);
    
    // Deinitialization tests
    RUN_TEST(test_power_module_handler_deinit_success);
    RUN_TEST(test_power_module_handler_deinit_not_initialized);
    
    // Data reading tests
    RUN_TEST(test_power_module_handler_read_data_success);
    RUN_TEST(test_power_module_handler_read_data_null_pointer);
    RUN_TEST(test_power_module_handler_read_data_not_initialized);
    
    // Register access tests
    RUN_TEST(test_power_module_handler_write_register_success);
    RUN_TEST(test_power_module_handler_write_register_not_initialized);
    RUN_TEST(test_power_module_handler_read_register_success);
    RUN_TEST(test_power_module_handler_read_register_null_pointer);
    RUN_TEST(test_power_module_handler_read_register_not_initialized);
    
    // Status tests
    RUN_TEST(test_power_module_handler_get_status_success);
    RUN_TEST(test_power_module_handler_get_status_null_pointer);
    RUN_TEST(test_power_module_handler_get_status_not_initialized);
    
    // Charging control tests
    RUN_TEST(test_power_module_handler_set_charging_success);
    RUN_TEST(test_power_module_handler_set_charging_invalid_current);
    RUN_TEST(test_power_module_handler_set_charging_invalid_voltage);
    RUN_TEST(test_power_module_handler_set_charging_not_initialized);
    
    // Output control tests
    RUN_TEST(test_power_module_handler_control_outputs_success);
    RUN_TEST(test_power_module_handler_control_outputs_disabled_output);
    RUN_TEST(test_power_module_handler_control_outputs_not_initialized);
    
    // Fault management tests
    RUN_TEST(test_power_module_handler_reset_faults_success);
    RUN_TEST(test_power_module_handler_reset_faults_not_initialized);
    
    // Capabilities tests
    RUN_TEST(test_power_module_handler_get_capabilities_success);
    RUN_TEST(test_power_module_handler_get_capabilities_not_initialized);
    
    // Auto detection tests
    RUN_TEST(test_power_module_handler_auto_detect_success);
    RUN_TEST(test_power_module_handler_auto_detect_invalid_slave_id);
    RUN_TEST(test_power_module_handler_auto_detect_not_initialized);
    
    // Module information tests
    RUN_TEST(test_power_module_handler_get_module_info_success);
    RUN_TEST(test_power_module_handler_get_module_info_null_pointer);
    RUN_TEST(test_power_module_handler_get_module_info_not_initialized);
    
    // Module name tests
    RUN_TEST(test_power_module_handler_get_module_name_success);
    RUN_TEST(test_power_module_handler_get_module_name_null_pointer);
    RUN_TEST(test_power_module_handler_get_module_name_small_buffer);
    RUN_TEST(test_power_module_handler_get_module_name_not_initialized);
    
    // System management tests
    RUN_TEST(test_power_module_handler_reset_system_errors_success);
    RUN_TEST(test_power_module_handler_reset_system_errors_not_initialized);
    RUN_TEST(test_power_module_handler_get_system_status_success);
    RUN_TEST(test_power_module_handler_get_system_status_null_pointer);
    RUN_TEST(test_power_module_handler_get_system_status_not_initialized);
    
    UNITY_END();
    return 0;
}
