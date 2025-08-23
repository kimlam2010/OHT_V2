/**
 * @file test_power_module_handler.c
 * @brief Test cases for Power Module Handler
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "power_module_handler.h"
#include "hal_common.h"

// Test configuration
static power_module_config_t test_config = {
    .slave_id = 0x02,
    .baudrate_code = 1,  // 9600 baud
    .parity = 0,         // None
    .stop_bits = 1,
    .fc_mask = 0x07,     // FC3, FC6, FC16
    .max_cell_threshold_1 = 4200,  // 4.2V per cell
    .min_cell_threshold_1 = 3000,  // 3.0V per cell
    .max_cell_threshold_2 = 4250,  // 4.25V per cell
    .min_cell_threshold_2 = 2800,  // 2.8V per cell
    .max_pack_threshold_1 = 25200, // 25.2V pack
    .min_pack_threshold_1 = 18000, // 18.0V pack
    .max_pack_threshold_2 = 25500, // 25.5V pack
    .min_pack_threshold_2 = 16800, // 16.8V pack
    .charge_voltage_set = 25.2f,   // 25.2V
    .charge_current_set = 5.0f,    // 5A
    .charge_enable = true,
    .output_12v_enabled = true,
    .output_5v_enabled = true,
    .output_3v3_enabled = true,
    .overvoltage_threshold = 30.0f, // 30V
    .overcurrent_threshold = 10.0f, // 10A
    .overtemp_threshold = 60.0f,    // 60°C
    .use_v_ths = 12.0f             // 12V threshold
};

// Test function prototypes
static void test_power_module_init(void);
static void test_power_module_read_data(void);
static void test_power_module_read_register(void);
static void test_power_module_write_register(void);
static void test_power_module_set_charging(void);
static void test_power_module_control_outputs(void);
static void test_power_module_get_status(void);
static void test_power_module_get_capabilities(void);
static void test_power_module_reset_faults(void);
static void test_power_module_auto_detect(void);
static void test_power_module_get_module_info(void);
static void test_power_module_get_module_name(void);
static void test_power_module_reset_system_errors(void);
static void test_power_module_get_system_status(void);
static void test_power_module_deinit(void);

// Test helper functions
static void print_test_result(const char *test_name, bool passed);
static void print_power_module_data(const power_module_data_t *data);
static void print_power_module_status(const power_module_status_t *status);

/**
 * @brief Main test function
 */
int main(void)
{
    printf("=== Power Module Handler Test Suite ===\n\n");
    
    // Run all tests
    test_power_module_init();
    test_power_module_read_data();
    test_power_module_read_register();
    test_power_module_write_register();
    test_power_module_set_charging();
    test_power_module_control_outputs();
    test_power_module_get_status();
    test_power_module_get_capabilities();
    test_power_module_reset_faults();
    test_power_module_auto_detect();
    test_power_module_get_module_info();
    test_power_module_get_module_name();
    test_power_module_reset_system_errors();
    test_power_module_get_system_status();
    test_power_module_deinit();
    
    printf("\n=== All tests completed ===\n");
    return 0;
}

/**
 * @brief Test power module initialization
 */
static void test_power_module_init(void)
{
    printf("Testing power module initialization...\n");
    
    // Test with valid configuration
    hal_status_t status = power_module_handler_init(&test_config);
    print_test_result("Init with valid config", status == HAL_STATUS_OK);
    
    // Test with NULL configuration
    status = power_module_handler_init(NULL);
    print_test_result("Init with NULL config", status == HAL_STATUS_INVALID_PARAMETER);
    
    // Test double initialization
    status = power_module_handler_init(&test_config);
    print_test_result("Double init", status == HAL_STATUS_ALREADY_INITIALIZED);
    
    printf("\n");
}

/**
 * @brief Test power module data reading
 */
static void test_power_module_read_data(void)
{
    printf("Testing power module data reading...\n");
    
    power_module_data_t data;
    hal_status_t status;
    
    // Test with valid pointer
    status = power_module_handler_read_data(&data);
    print_test_result("Read data with valid pointer", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        print_power_module_data(&data);
    }
    
    // Test with NULL pointer
    status = power_module_handler_read_data(NULL);
    print_test_result("Read data with NULL pointer", status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("\n");
}

/**
 * @brief Test power module register reading
 */
static void test_power_module_read_register(void)
{
    printf("Testing power module register reading...\n");
    
    uint16_t value;
    hal_status_t status;
    
    // Test reading battery voltage
    status = power_module_handler_read_register(POWER_REG_BATTERY_VOLTAGE, &value);
    print_test_result("Read battery voltage", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  Battery voltage: %d (raw), %.1fV (scaled)\n", value, value / 10.0f);
    }
    
    // Test reading battery current
    status = power_module_handler_read_register(POWER_REG_BATTERY_CURRENT, &value);
    print_test_result("Read battery current", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  Battery current: %d (raw), %.1fA (scaled)\n", value, value / 10.0f);
    }
    
    // Test reading battery SOC
    status = power_module_handler_read_register(POWER_REG_BATTERY_SOC, &value);
    print_test_result("Read battery SOC", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  Battery SOC: %d (raw), %.1f%% (scaled)\n", value, value / 10.0f);
    }
    
    // Test with NULL pointer
    status = power_module_handler_read_register(POWER_REG_BATTERY_VOLTAGE, NULL);
    print_test_result("Read register with NULL pointer", status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("\n");
}

/**
 * @brief Test power module register writing
 */
static void test_power_module_write_register(void)
{
    printf("Testing power module register writing...\n");
    
    hal_status_t status;
    
    // Test writing charge voltage setpoint
    status = power_module_handler_write_register(POWER_REG_V_SET, 252); // 25.2V
    print_test_result("Write charge voltage setpoint", status == HAL_STATUS_OK);
    
    // Test writing charge current setpoint
    status = power_module_handler_write_register(POWER_REG_I_SET, 50); // 5.0A
    print_test_result("Write charge current setpoint", status == HAL_STATUS_OK);
    
    // Test writing charge request
    status = power_module_handler_write_register(POWER_REG_CHARGE_REQUEST, 1);
    print_test_result("Write charge request", status == HAL_STATUS_OK);
    
    // Test writing output ON/OFF
    status = power_module_handler_write_register(POWER_REG_ON_OFF, 1);
    print_test_result("Write output ON", status == HAL_STATUS_OK);
    
    printf("\n");
}

/**
 * @brief Test power module charging control
 */
static void test_power_module_set_charging(void)
{
    printf("Testing power module charging control...\n");
    
    hal_status_t status;
    
    // Test enabling charging
    status = power_module_handler_set_charging(5.0f, 25.2f, true);
    print_test_result("Enable charging (5A, 25.2V)", status == HAL_STATUS_OK);
    
    // Test disabling charging
    status = power_module_handler_set_charging(0.0f, 0.0f, false);
    print_test_result("Disable charging", status == HAL_STATUS_OK);
    
    // Test invalid parameters
    status = power_module_handler_set_charging(-1.0f, 25.2f, true);
    print_test_result("Invalid negative current", status == HAL_STATUS_INVALID_PARAMETER);
    
    status = power_module_handler_set_charging(5.0f, -1.0f, true);
    print_test_result("Invalid negative voltage", status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("\n");
}

/**
 * @brief Test power module output control
 */
static void test_power_module_control_outputs(void)
{
    printf("Testing power module output control...\n");
    
    hal_status_t status;
    
    // Test enabling all outputs
    status = power_module_handler_control_outputs(true, true, true);
    print_test_result("Enable all outputs", status == HAL_STATUS_OK);
    
    // Test disabling all outputs
    status = power_module_handler_control_outputs(false, false, false);
    print_test_result("Disable all outputs", status == HAL_STATUS_OK);
    
    // Test mixed output states
    status = power_module_handler_control_outputs(true, false, true);
    print_test_result("Mixed output states", status == HAL_STATUS_OK);
    
    printf("\n");
}

/**
 * @brief Test power module status
 */
static void test_power_module_get_status(void)
{
    printf("Testing power module status...\n");
    
    power_module_status_t status;
    hal_status_t hal_status;
    
    // Test getting status
    hal_status = power_module_handler_get_status(&status);
    print_test_result("Get status with valid pointer", hal_status == HAL_STATUS_OK);
    
    if (hal_status == HAL_STATUS_OK) {
        print_power_module_status(&status);
    }
    
    // Test with NULL pointer
    hal_status = power_module_handler_get_status(NULL);
    print_test_result("Get status with NULL pointer", hal_status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("\n");
}

/**
 * @brief Test power module capabilities
 */
static void test_power_module_get_capabilities(void)
{
    printf("Testing power module capabilities...\n");
    
    uint32_t capabilities = power_module_handler_get_capabilities();
    
    printf("  Capabilities: 0x%08X\n", capabilities);
    printf("  Voltage Monitor: %s\n", (capabilities & POWER_CAP_VOLTAGE_MONITOR) ? "Yes" : "No");
    printf("  Current Monitor: %s\n", (capabilities & POWER_CAP_CURRENT_MONITOR) ? "Yes" : "No");
    printf("  Temperature Monitor: %s\n", (capabilities & POWER_CAP_TEMP_MONITOR) ? "Yes" : "No");
    printf("  Relay Control: %s\n", (capabilities & POWER_CAP_RELAY_CONTROL) ? "Yes" : "No");
    printf("  Overvoltage Protection: %s\n", (capabilities & POWER_CAP_OVERVOLTAGE_PROTECT) ? "Yes" : "No");
    printf("  Overcurrent Protection: %s\n", (capabilities & POWER_CAP_OVERCURRENT_PROTECT) ? "Yes" : "No");
    printf("  Overtemperature Protection: %s\n", (capabilities & POWER_CAP_OVERTEMP_PROTECT) ? "Yes" : "No");
    printf("  Battery Monitor: %s\n", (capabilities & POWER_CAP_BATTERY_MONITOR) ? "Yes" : "No");
    printf("  Charging Control: %s\n", (capabilities & POWER_CAP_CHARGING_CONTROL) ? "Yes" : "No");
    printf("  Power Distribution: %s\n", (capabilities & POWER_CAP_POWER_DISTRIBUTION) ? "Yes" : "No");
    
    print_test_result("Get capabilities", capabilities != 0);
    
    printf("\n");
}

/**
 * @brief Test power module fault reset
 */
static void test_power_module_reset_faults(void)
{
    printf("Testing power module fault reset...\n");
    
    hal_status_t status = power_module_handler_reset_faults();
    print_test_result("Reset faults", status == HAL_STATUS_OK);
    
    printf("\n");
}

/**
 * @brief Test power module auto detect
 */
static void test_power_module_auto_detect(void)
{
    printf("Testing power module auto detect...\n");
    
    // Test auto detect with valid slave ID
    hal_status_t status = power_module_handler_auto_detect(0x02, 1000);
    print_test_result("Auto detect slave ID 0x02", status == HAL_STATUS_OK);
    
    // Test auto detect with invalid slave ID
    status = power_module_handler_auto_detect(0, 1000);
    print_test_result("Auto detect invalid slave ID", status == HAL_STATUS_INVALID_PARAMETER);
    
    // Test auto detect with out of range slave ID
    status = power_module_handler_auto_detect(248, 1000);
    print_test_result("Auto detect out of range slave ID", status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("\n");
}

/**
 * @brief Test power module get module info
 */
static void test_power_module_get_module_info(void)
{
    printf("Testing power module get module info...\n");
    
    uint16_t device_id, module_type, firmware_version, hardware_version;
    hal_status_t status = power_module_handler_get_module_info(&device_id, &module_type, 
                                                              &firmware_version, &hardware_version);
    print_test_result("Get module info", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  Module Info:\n");
        printf("    Device ID: 0x%04X\n", device_id);
        printf("    Module Type: 0x%04X\n", module_type);
        printf("    Firmware Version: %d.%d\n", firmware_version >> 8, firmware_version & 0xFF);
        printf("    Hardware Version: %d.%d\n", hardware_version >> 8, hardware_version & 0xFF);
    }
    
    // Test with NULL pointers
    status = power_module_handler_get_module_info(NULL, &module_type, &firmware_version, &hardware_version);
    print_test_result("Get module info with NULL device_id", status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("\n");
}

/**
 * @brief Test power module get module name
 */
static void test_power_module_get_module_name(void)
{
    printf("Testing power module get module name...\n");
    
    char module_name[32];
    hal_status_t status = power_module_handler_get_module_name(module_name, sizeof(module_name));
    print_test_result("Get module name", status == HAL_STATUS_OK);
    
    if (status == HAL_STATUS_OK) {
        printf("  Module Name: %s\n", module_name);
    }
    
    // Test with NULL buffer
    status = power_module_handler_get_module_name(NULL, 32);
    print_test_result("Get module name with NULL buffer", status == HAL_STATUS_INVALID_PARAMETER);
    
    // Test with small buffer
    char small_buffer[5];
    status = power_module_handler_get_module_name(small_buffer, sizeof(small_buffer));
    print_test_result("Get module name with small buffer", status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("\n");
}

/**
 * @brief Test power module reset system errors
 */
static void test_power_module_reset_system_errors(void)
{
    printf("Testing power module reset system errors...\n");
    
    hal_status_t status = power_module_handler_reset_system_errors();
    print_test_result("Reset system errors", status == HAL_STATUS_OK);
    
    printf("\n");
}

/**
 * @brief Test power module get system status
 */
static void test_power_module_get_system_status(void)
{
    printf("Testing power module get system status...\n");
    
    uint16_t system_status, system_error;
    hal_status_t hal_status = power_module_handler_get_system_status(&system_status, &system_error);
    print_test_result("Get system status", hal_status == HAL_STATUS_OK);
    
    if (hal_status == HAL_STATUS_OK) {
        printf("  System Status: 0x%04X\n", system_status);
        printf("  System Error: 0x%04X\n", system_error);
    }
    
    // Test with NULL pointers
    hal_status = power_module_handler_get_system_status(NULL, &system_error);
    print_test_result("Get system status with NULL system_status", hal_status == HAL_STATUS_INVALID_PARAMETER);
    
    hal_status = power_module_handler_get_system_status(&system_status, NULL);
    print_test_result("Get system status with NULL system_error", hal_status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("\n");
}

/**
 * @brief Test power module deinitialization
 */
static void test_power_module_deinit(void)
{
    printf("Testing power module deinitialization...\n");
    
    hal_status_t status = power_module_handler_deinit();
    print_test_result("Deinit", status == HAL_STATUS_OK);
    
    // Test deinit when not initialized
    status = power_module_handler_deinit();
    print_test_result("Double deinit", status == HAL_STATUS_ERROR);
    
    printf("\n");
}

/**
 * @brief Print test result
 */
static void print_test_result(const char *test_name, bool passed)
{
    printf("  %s: %s\n", test_name, passed ? "PASS" : "FAIL");
}

/**
 * @brief Print power module data
 */
static void print_power_module_data(const power_module_data_t *data)
{
    printf("  Power Module Data:\n");
    printf("    Battery: %.1fV, %.1fA, %.1f%%, %d°C\n", 
           data->battery_voltage, data->battery_current, data->battery_soc, data->temperature);
    printf("    Cells: Max=%dmV, Min=%dmV, Diff=%dmV\n", 
           data->max_cell_v, data->min_cell_v, data->cell_diff);
    printf("    Charge: Status=%d, Current=%.1fA, Voltage=%.1fV\n", 
           data->charge_status, data->i_set, data->v_set);
    printf("    Outputs: 12V=%.1fV/%.1fA, 5V=%.1fV/%.1fA, 3.3V=%.1fV/%.1fA\n",
           data->v_out_12v, data->i_out_12v, data->v_out_5v, data->i_out_5v, 
           data->v_out_3v3, data->i_out_3v3);
    printf("    Relays: 12V=%d, 5V=%d, 3.3V=%d, Fault=%d\n",
           data->rl_12v, data->rl_5v, data->rl_3v3, data->rl_fault);
}

/**
 * @brief Print power module status
 */
static void print_power_module_status(const power_module_status_t *status)
{
    printf("  Power Module Status:\n");
    printf("    Initialized: %s\n", status->initialized ? "Yes" : "No");
    printf("    Online: %s\n", status->online ? "Yes" : "No");
    printf("    Error Count: %u\n", status->error_count);
    printf("    Warning Count: %u\n", status->warning_count);
    printf("    Uptime: %llu seconds\n", status->uptime_seconds);
    printf("    Last Communication: %llu ms ago\n", status->last_communication_ms);
}
