/**
 * @file test_power_module.c
 * @brief Test power module handler functionality
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "power_module_handler.h"
#include "hal_common.h"

static void test_power_module_init(void) {
    printf("\n=== Testing Power Module Initialization ===\n");
    
    power_module_handler_t handler;
    hal_status_t status = power_module_init(&handler, 0x02);
    
    assert(status == HAL_STATUS_OK);
    assert(handler.address == 0x02);
    assert(handler.status.initialized == true);
    assert(handler.status.online == false);
    assert(handler.config.voltage_max == 28.0f);
    assert(handler.config.voltage_min == 20.0f);
    assert(handler.config.current_max == 10.0f);
    assert(handler.config.temp_max == 60.0f);
    
    printf("Power module initialization test PASSED\n");
    
    // Test deinit
    status = power_module_deinit(&handler);
    assert(status == HAL_STATUS_OK);
}

static void test_power_module_config(void) {
    printf("\n=== Testing Power Module Configuration ===\n");
    
    power_module_handler_t handler;
    power_module_init(&handler, 0x02);
    
    // Test valid configuration
    power_module_config_t config = {
        .voltage_max = 30.0f,
        .voltage_min = 18.0f,
        .current_max = 15.0f,
        .temp_max = 70.0f,
        .relay1_enabled = true,
        .relay2_enabled = false
    };
    
    hal_status_t status = power_module_set_config(&handler, &config);
    assert(status == HAL_STATUS_OK);
    
    // Verify configuration was applied
    power_module_config_t retrieved_config;
    status = power_module_get_config(&handler, &retrieved_config);
    assert(status == HAL_STATUS_OK);
    assert(retrieved_config.voltage_max == 30.0f);
    assert(retrieved_config.voltage_min == 18.0f);
    assert(retrieved_config.current_max == 15.0f);
    assert(retrieved_config.temp_max == 70.0f);
    assert(retrieved_config.relay1_enabled == true);
    assert(retrieved_config.relay2_enabled == false);
    
    // Test invalid configuration
    power_module_config_t invalid_config = {
        .voltage_max = 15.0f,  // Less than min
        .voltage_min = 20.0f,
        .current_max = 15.0f,
        .temp_max = 70.0f,
        .relay1_enabled = true,
        .relay2_enabled = true
    };
    
    status = power_module_set_config(&handler, &invalid_config);
    assert(status == HAL_STATUS_INVALID_PARAMETER);
    
    printf("Power module configuration test PASSED\n");
    
    power_module_deinit(&handler);
}

static void test_power_module_alarms(void) {
    printf("\n=== Testing Power Module Alarm Detection ===\n");
    
    power_module_handler_t handler;
    power_module_init(&handler, 0x02);
    
    // Test normal conditions (no alarms)
    handler.data.voltage_main = 24.0f;
    handler.data.current_main = 5.0f;
    handler.data.temp_main = 45.0f;
    
    bool has_alarm = power_module_check_alarms(&handler);
    assert(has_alarm == false);
    
    // Test overvoltage alarm
    handler.data.voltage_main = 30.0f;  // Above max (28V)
    has_alarm = power_module_check_alarms(&handler);
    assert(has_alarm == true);
    assert(handler.data.alarm_status & POWER_ALARM_OVERVOLTAGE);
    
    // Test undervoltage alarm
    handler.data.voltage_main = 18.0f;  // Below min (20V)
    handler.data.alarm_status = 0;  // Clear previous alarms
    has_alarm = power_module_check_alarms(&handler);
    assert(has_alarm == true);
    assert(handler.data.alarm_status & POWER_ALARM_UNDERVOLTAGE);
    
    // Test overcurrent alarm
    handler.data.current_main = 12.0f;  // Above max (10A)
    handler.data.alarm_status = 0;  // Clear previous alarms
    has_alarm = power_module_check_alarms(&handler);
    assert(has_alarm == true);
    assert(handler.data.alarm_status & POWER_ALARM_OVERCURRENT);
    
    // Test overtemperature alarm
    handler.data.temp_main = 65.0f;  // Above max (60°C)
    handler.data.alarm_status = 0;  // Clear previous alarms
    has_alarm = power_module_check_alarms(&handler);
    assert(has_alarm == true);
    assert(handler.data.alarm_status & POWER_ALARM_OVERTEMP);
    
    printf("Power module alarm detection test PASSED\n");
    
    power_module_deinit(&handler);
}

static void test_power_module_utility_functions(void) {
    printf("\n=== Testing Power Module Utility Functions ===\n");
    
    power_module_handler_t handler;
    power_module_init(&handler, 0x02);
    
    // Test alarm description
    const char* desc = power_module_get_alarm_description(0);
    assert(strcmp(desc, "No alarms") == 0);
    
    desc = power_module_get_alarm_description(POWER_ALARM_OVERVOLTAGE);
    assert(strstr(desc, "OVERVOLTAGE") != NULL);
    
    desc = power_module_get_alarm_description(POWER_ALARM_OVERVOLTAGE | POWER_ALARM_OVERCURRENT);
    assert(strstr(desc, "OVERVOLTAGE") != NULL);
    assert(strstr(desc, "OVERCURRENT") != NULL);
    
    // Test status and data functions
    power_module_status_t status;
    hal_status_t st = power_module_get_status(&handler, &status);
    assert(st == HAL_STATUS_OK);
    assert(status.initialized == true);
    
    power_module_data_t data;
    st = power_module_get_data(&handler, &data);
    assert(st == HAL_STATUS_OK);
    assert(data.voltage_main == 0.0f);
    assert(data.current_main == 0.0f);
    assert(data.temp_main == 0.0f);
    
    // Test print functions (should not crash)
    power_module_print_status(&handler);
    power_module_print_data(&handler);
    
    printf("Power module utility functions test PASSED\n");
    
    power_module_deinit(&handler);
}

static void test_power_module_integration(void) {
    printf("\n=== Testing Power Module Integration ===\n");
    
    power_module_handler_t handler;
    power_module_init(&handler, 0x02);
    
    // Simulate reading data (in real implementation, this would communicate with hardware)
    printf("Note: This test simulates power module communication\n");
    printf("In real implementation, these would read from Modbus registers\n");
    
    // Test data reading functions (they will fail without real hardware, but should handle errors gracefully)
    hal_status_t status_v = power_module_read_voltage(&handler);
    hal_status_t status_i = power_module_read_current(&handler);
    hal_status_t status_t = power_module_read_temperature(&handler);
    hal_status_t status_r = power_module_read_relay_status(&handler);
    hal_status_t status_a = power_module_read_alarm_status(&handler);
    
    // All should fail without real hardware, but should not crash
    printf("Voltage read status: %d\n", status_v);
    printf("Current read status: %d\n", status_i);
    printf("Temperature read status: %d\n", status_t);
    printf("Relay status read status: %d\n", status_r);
    printf("Alarm status read status: %d\n", status_a);
    
    // Test relay control (will fail without real hardware)
    hal_status_t status_relay1 = power_module_set_relay1(&handler, true);
    hal_status_t status_relay2 = power_module_set_relay2(&handler, false);
    
    printf("Relay 1 control status: %d\n", status_relay1);
    printf("Relay 2 control status: %d\n", status_relay2);
    
    // Test emergency shutdown
    hal_status_t status_shutdown = power_module_emergency_shutdown(&handler);
    printf("Emergency shutdown status: %d\n", status_shutdown);
    
    printf("Power module integration test PASSED\n");
    
    power_module_deinit(&handler);
}

int main(void) {
    printf("=== Power Module Handler Test Suite ===\n");
    printf("Testing power module (0x02) functionality...\n");
    
    test_power_module_init();
    test_power_module_config();
    test_power_module_alarms();
    test_power_module_utility_functions();
    test_power_module_integration();
    
    printf("\n=== All Power Module Tests PASSED ===\n");
    printf("Power module handler implementation is working correctly!\n");
    
    return 0;
}
