/**
 * @file test_power_module_validation.c
 * @brief Unit tests for Power Module Data Validation
 * @version 1.0.0
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
// Mock function definitions - REMOVED (using real HAL functions)
// Mock function declarations - REMOVED (using real HAL functions)
// Test data - REMOVED (using real HAL functions)

// Validation ranges
#define MIN_VOLTAGE_MV 3000   // 3.0V minimum
#define MAX_VOLTAGE_MV 5000   // 5.0V maximum
#define MIN_CURRENT_MA -10000 // -10A minimum
#define MAX_CURRENT_MA 10000  // 10A maximum
#define MIN_SOC_PERCENT 0     // 0% minimum
#define MAX_SOC_PERCENT 100   // 100% maximum
#define MIN_TEMPERATURE_C -40 // -40°C minimum
#define MAX_TEMPERATURE_C 85  // 85°C maximum

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
    
    // Reset mock data - REMOVED (using real HAL functions)
}

void tearDown(void)
{
    // Cleanup
    power_module_handler_deinit();
}

// Mock implementations - REMOVED (using real HAL functions)

// Validation helper functions
static bool validate_voltage_range(uint16_t voltage_mv)
{
    return (voltage_mv >= MIN_VOLTAGE_MV && voltage_mv <= MAX_VOLTAGE_MV);
}

static bool validate_current_range(int16_t current_ma)
{
    return (current_ma >= MIN_CURRENT_MA && current_ma <= MAX_CURRENT_MA);
}

static bool validate_soc_range(uint8_t soc_percent)
{
    return (soc_percent >= MIN_SOC_PERCENT && soc_percent <= MAX_SOC_PERCENT);
}

static bool validate_temperature_range(int16_t temperature_c)
{
    return (temperature_c >= MIN_TEMPERATURE_C && temperature_c <= MAX_TEMPERATURE_C);
}

// Test cases

void test_power_module_voltage_validation(void)
{
    printf("=== TEST: Power Module Voltage Validation ===\n");
    
    // Initialize power module
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test valid voltage ranges
    uint16_t valid_voltages[] = {3000, 3500, 4000, 4500, 5000};
    for (int i = 0; i < 5; i++) {
        bool is_valid = validate_voltage_range(valid_voltages[i]);
        printf("Voltage %d mV: %s\n", valid_voltages[i], is_valid ? "VALID" : "INVALID");
        TEST_ASSERT_TRUE(is_valid);
    }
    
    // Test invalid voltage ranges
    uint16_t invalid_voltages[] = {0, 1000, 2000, 6000, 10000};
    for (int i = 0; i < 5; i++) {
        bool is_valid = validate_voltage_range(invalid_voltages[i]);
        printf("Voltage %d mV: %s\n", invalid_voltages[i], is_valid ? "VALID" : "INVALID");
        TEST_ASSERT_FALSE(is_valid);
    }
    
    printf("✅ Voltage validation test PASSED\n");
}

void test_power_module_current_validation(void)
{
    printf("=== TEST: Power Module Current Validation ===\n");
    
    // Initialize power module
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test valid current ranges
    int16_t valid_currents[] = {-10000, -5000, 0, 5000, 10000};
    for (int i = 0; i < 5; i++) {
        bool is_valid = validate_current_range(valid_currents[i]);
        printf("Current %d mA: %s\n", valid_currents[i], is_valid ? "VALID" : "INVALID");
        TEST_ASSERT_TRUE(is_valid);
    }
    
    // Test invalid current ranges
    int16_t invalid_currents[] = {-15000, -12000, 12000, 15000, 20000};
    for (int i = 0; i < 5; i++) {
        bool is_valid = validate_current_range(invalid_currents[i]);
        printf("Current %d mA: %s\n", invalid_currents[i], is_valid ? "VALID" : "INVALID");
        TEST_ASSERT_FALSE(is_valid);
    }
    
    printf("✅ Current validation test PASSED\n");
}

void test_power_module_soc_validation(void)
{
    printf("=== TEST: Power Module SOC Validation ===\n");
    
    // Initialize power module
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test valid SOC ranges
    uint8_t valid_socs[] = {0, 25, 50, 75, 100};
    for (int i = 0; i < 5; i++) {
        bool is_valid = validate_soc_range(valid_socs[i]);
        printf("SOC %d%%: %s\n", valid_socs[i], is_valid ? "VALID" : "INVALID");
        TEST_ASSERT_TRUE(is_valid);
    }
    
    // Test invalid SOC ranges (using uint8_t overflow)
    uint8_t invalid_socs[] = {101, 150, 200, 255};
    for (int i = 0; i < 4; i++) {
        bool is_valid = validate_soc_range(invalid_socs[i]);
        printf("SOC %d%%: %s\n", invalid_socs[i], is_valid ? "VALID" : "INVALID");
        TEST_ASSERT_FALSE(is_valid);
    }
    
    printf("✅ SOC validation test PASSED\n");
}

void test_power_module_temperature_validation(void)
{
    printf("=== TEST: Power Module Temperature Validation ===\n");
    
    // Initialize power module
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test valid temperature ranges
    int16_t valid_temps[] = {-40, -20, 0, 25, 50, 85};
    for (int i = 0; i < 6; i++) {
        bool is_valid = validate_temperature_range(valid_temps[i]);
        printf("Temperature %d°C: %s\n", valid_temps[i], is_valid ? "VALID" : "INVALID");
        TEST_ASSERT_TRUE(is_valid);
    }
    
    // Test invalid temperature ranges
    int16_t invalid_temps[] = {-50, -45, 90, 100, 150};
    for (int i = 0; i < 5; i++) {
        bool is_valid = validate_temperature_range(invalid_temps[i]);
        printf("Temperature %d°C: %s\n", invalid_temps[i], is_valid ? "VALID" : "INVALID");
        TEST_ASSERT_FALSE(is_valid);
    }
    
    printf("✅ Temperature validation test PASSED\n");
}

void test_power_module_data_integrity(void)
{
    printf("=== TEST: Power Module Data Integrity ===\n");
    
    // Initialize power module
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test data structure integrity
    power_module_data_t data;
    memset(&data, 0, sizeof(power_module_data_t));
    
    // Set valid data
    data.battery_voltage = 42.0f;  // 42.0V
    data.battery_current = 1.0f;   // 1.0A
    data.battery_soc = 75.0f;      // 75%
    data.temperature = 25.0f;      // 25°C
    data.v_out = 12.6f;           // 12.6V
    data.i_out = 2.0f;            // 2.0A
    data.v_out_12v = 12.0f;       // 12.0V
    data.v_out_5v = 5.0f;         // 5.0V
    data.v_out_3v3 = 3.3f;        // 3.3V
    
    // Validate all fields (convert to appropriate units for validation)
    TEST_ASSERT_TRUE(validate_voltage_range((uint16_t)(data.battery_voltage * 1000))); // Convert V to mV
    TEST_ASSERT_TRUE(validate_current_range((int16_t)(data.battery_current * 1000))); // Convert A to mA
    TEST_ASSERT_TRUE(validate_soc_range((uint8_t)data.battery_soc)); // Convert float to uint8_t
    TEST_ASSERT_TRUE(validate_temperature_range((int16_t)data.temperature)); // Convert float to int16_t
    TEST_ASSERT_TRUE(validate_voltage_range((uint16_t)(data.v_out * 1000))); // Convert V to mV
    TEST_ASSERT_TRUE(validate_current_range((int16_t)(data.i_out * 1000))); // Convert A to mA
    TEST_ASSERT_TRUE(validate_voltage_range((uint16_t)(data.v_out_12v * 1000))); // Convert V to mV
    TEST_ASSERT_TRUE(validate_voltage_range((uint16_t)(data.v_out_5v * 1000))); // Convert V to mV
    TEST_ASSERT_TRUE(validate_voltage_range((uint16_t)(data.v_out_3v3 * 1000))); // Convert V to mV
    
    printf("✅ Data integrity test PASSED\n");
}

void test_power_module_threshold_validation(void)
{
    printf("=== TEST: Power Module Threshold Validation ===\n");
    
    // Initialize power module
    hal_status_t status = power_module_handler_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test configuration thresholds
    printf("Config thresholds:\n");
    printf("  Max cell threshold 1: %d mV\n", test_config.max_cell_threshold_1);
    printf("  Min cell threshold 1: %d mV\n", test_config.min_cell_threshold_1);
    printf("  Max pack threshold 1: %d mV\n", test_config.max_pack_threshold_1);
    printf("  Min pack threshold 1: %d mV\n", test_config.min_pack_threshold_1);
    
    // Validate threshold relationships
    TEST_ASSERT_GREATER_THAN(test_config.min_cell_threshold_1, test_config.max_cell_threshold_1);
    TEST_ASSERT_GREATER_THAN(test_config.min_pack_threshold_1, test_config.max_pack_threshold_1);
    
    // Test that thresholds are within valid ranges
    TEST_ASSERT_TRUE(validate_voltage_range(test_config.max_cell_threshold_1));
    TEST_ASSERT_TRUE(validate_voltage_range(test_config.min_cell_threshold_1));
    TEST_ASSERT_TRUE(validate_voltage_range(test_config.max_pack_threshold_1));
    TEST_ASSERT_TRUE(validate_voltage_range(test_config.min_pack_threshold_1));
    
    printf("✅ Threshold validation test PASSED\n");
}

// Test runner
int main(void)
{
    UNITY_BEGIN();
    
    printf("🚀 Starting OHT-50 Power Module Validation Tests\n");
    printf("===============================================\n");
    
    // Validation tests
    RUN_TEST(test_power_module_voltage_validation);
    RUN_TEST(test_power_module_current_validation);
    RUN_TEST(test_power_module_soc_validation);
    RUN_TEST(test_power_module_temperature_validation);
    RUN_TEST(test_power_module_data_integrity);
    RUN_TEST(test_power_module_threshold_validation);
    
    printf("===============================================\n");
    printf("📊 POWER MODULE VALIDATION TEST SUMMARY\n");
    printf("===============================================\n");
    
    UNITY_END();
    return 0;
}
