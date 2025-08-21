/**
 * @file test_config_system.c
 * @brief Configuration System Test Program for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include "config_system.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TEST_CONFIG_FILE "/tmp/test_config.bin"

static void print_test_result(const char *test_name, bool passed) {
    printf("  %s %s\n", passed ? "✅" : "❌", test_name);
}

static void print_section_header(const char *section) {
    printf("\n🔧 %s\n", section);
    printf("--------------------------------\n");
}

int main(void) {
    printf("=== OHT-50 Configuration System Test ===\n");
    printf("Version: 1.0.0\n");
    printf("Date: %s\n", __DATE__);
    printf("Team: FW\n");
    printf("Task: FW-08 (Configuration System Testing)\n\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Test 1: Initialization
    print_section_header("Test 1: Configuration System Initialization");
    total_tests++;
    
    hal_status_t status = config_system_init(TEST_CONFIG_FILE);
    if (status == HAL_STATUS_OK) {
        print_test_result("Configuration system initialization", true);
        passed_tests++;
    } else {
        print_test_result("Configuration system initialization", false);
        printf("    Error: %d\n", status);
    }
    
    // Test 2: Default Configuration
    print_section_header("Test 2: Default Configuration");
    total_tests++;
    
    master_config_t config;
    status = config_system_get_master_config(&config);
    if (status == HAL_STATUS_OK) {
        print_test_result("Get master configuration", true);
        passed_tests++;
        
        // Verify default values
        if (strcmp(config.system.device_name, "OHT-50-Master") == 0 &&
            config.safety.estop_timeout_ms == 100 &&
            config.communication.rs485_baud_rate == 115200) {
            print_test_result("Default configuration values", true);
            passed_tests++;
        } else {
            print_test_result("Default configuration values", false);
        }
        total_tests++;
    } else {
        print_test_result("Get master configuration", false);
    }
    
    // Test 3: Configuration Validation
    print_section_header("Test 3: Configuration Validation");
    total_tests++;
    
    config_validation_t validation = config_system_validate_config(&config);
    if (validation.valid) {
        print_test_result("Configuration validation", true);
        passed_tests++;
    } else {
        print_test_result("Configuration validation", false);
        printf("    Error: %s\n", validation.error_message);
    }
    
    // Test 4: System Configuration
    print_section_header("Test 4: System Configuration");
    total_tests++;
    
    system_config_t system_config;
    status = config_system_get_system_config(&system_config);
    if (status == HAL_STATUS_OK) {
        print_test_result("Get system configuration", true);
        passed_tests++;
        
        // Modify and set
        strcpy(system_config.device_name, "OHT-50-Test");
        system_config.debug_mode = true;
        status = config_system_set_system_config(&system_config);
        if (status == HAL_STATUS_OK) {
            print_test_result("Set system configuration", true);
            passed_tests++;
        } else {
            print_test_result("Set system configuration", false);
        }
        total_tests++;
    } else {
        print_test_result("Get system configuration", false);
    }
    
    // Test 5: Safety Configuration
    print_section_header("Test 5: Safety Configuration");
    total_tests++;
    
    safety_config_t safety_config;
    status = config_system_get_safety_config(&safety_config);
    if (status == HAL_STATUS_OK) {
        print_test_result("Get safety configuration", true);
        passed_tests++;
        
        // Modify and set
        safety_config.estop_timeout_ms = 150;
        safety_config.dual_channel_estop = false;
        status = config_system_set_safety_config(&safety_config);
        if (status == HAL_STATUS_OK) {
            print_test_result("Set safety configuration", true);
            passed_tests++;
        } else {
            print_test_result("Set safety configuration", false);
        }
        total_tests++;
    } else {
        print_test_result("Get safety configuration", false);
    }
    
    // Test 6: Communication Configuration
    print_section_header("Test 6: Communication Configuration");
    total_tests++;
    
    communication_config_t comm_config;
    status = config_system_get_communication_config(&comm_config);
    if (status == HAL_STATUS_OK) {
        print_test_result("Get communication configuration", true);
        passed_tests++;
        
        // Modify and set
        comm_config.rs485_baud_rate = 57600;
        comm_config.rs485_retry_count = 5;
        status = config_system_set_communication_config(&comm_config);
        if (status == HAL_STATUS_OK) {
            print_test_result("Set communication configuration", true);
            passed_tests++;
        } else {
            print_test_result("Set communication configuration", false);
        }
        total_tests++;
    } else {
        print_test_result("Get communication configuration", false);
    }
    
    // Test 7: Module Configuration
    print_section_header("Test 7: Module Configuration");
    total_tests++;
    
    module_config_t module_config;
    status = config_system_get_module_config(&module_config);
    if (status == HAL_STATUS_OK) {
        print_test_result("Get module configuration", true);
        passed_tests++;
        
        // Modify and set
        module_config.power_module_address = 0x05;
        module_config.motor_module_address = 0x06;
        status = config_system_set_module_config(&module_config);
        if (status == HAL_STATUS_OK) {
            print_test_result("Set module configuration", true);
            passed_tests++;
        } else {
            print_test_result("Set module configuration", false);
        }
        total_tests++;
    } else {
        print_test_result("Get module configuration", false);
    }
    
    // Test 8: Performance Configuration
    print_section_header("Test 8: Performance Configuration");
    total_tests++;
    
    performance_config_t perf_config;
    status = config_system_get_performance_config(&perf_config);
    if (status == HAL_STATUS_OK) {
        print_test_result("Get performance configuration", true);
        passed_tests++;
        
        // Modify and set
        perf_config.control_loop_frequency_hz = 200;
        perf_config.telemetry_interval_ms = 50;
        status = config_system_set_performance_config(&perf_config);
        if (status == HAL_STATUS_OK) {
            print_test_result("Set performance configuration", true);
            passed_tests++;
        } else {
            print_test_result("Set performance configuration", false);
        }
        total_tests++;
    } else {
        print_test_result("Get performance configuration", false);
    }
    
    // Test 9: Configuration Persistence
    print_section_header("Test 9: Configuration Persistence");
    total_tests++;
    
    // Save configuration
    status = config_system_save();
    if (status == HAL_STATUS_OK) {
        print_test_result("Save configuration to file", true);
        passed_tests++;
        
        // Load configuration
        status = config_system_load();
        if (status == HAL_STATUS_OK) {
            print_test_result("Load configuration from file", true);
            passed_tests++;
        } else {
            print_test_result("Load configuration from file", false);
        }
        total_tests++;
    } else {
        print_test_result("Save configuration to file", false);
    }
    
    // Test 10: Reset to Defaults
    print_section_header("Test 10: Reset to Defaults");
    total_tests++;
    
    status = config_system_reset_to_defaults();
    if (status == HAL_STATUS_OK) {
        print_test_result("Reset configuration to defaults", true);
        passed_tests++;
        
        // Verify reset
        master_config_t reset_config;
        status = config_system_get_master_config(&reset_config);
        if (status == HAL_STATUS_OK && 
            strcmp(reset_config.system.device_name, "OHT-50-Master") == 0) {
            print_test_result("Verify default configuration", true);
            passed_tests++;
        } else {
            print_test_result("Verify default configuration", false);
        }
        total_tests++;
    } else {
        print_test_result("Reset configuration to defaults", false);
    }
    
    // Test 11: Configuration Monitoring
    print_section_header("Test 11: Configuration Monitoring");
    total_tests++;
    
    uint64_t timestamp;
    status = config_system_get_last_modified_time(&timestamp);
    if (status == HAL_STATUS_OK && timestamp > 0) {
        print_test_result("Get last modified time", true);
        passed_tests++;
    } else {
        print_test_result("Get last modified time", false);
    }
    
    // Test 12: Cleanup
    print_section_header("Test 12: Configuration System Cleanup");
    total_tests++;
    
    status = config_system_deinit();
    if (status == HAL_STATUS_OK) {
        print_test_result("Configuration system deinitialization", true);
        passed_tests++;
    } else {
        print_test_result("Configuration system deinitialization", false);
    }
    
    // Clean up test file
    unlink(TEST_CONFIG_FILE);
    
    // Test Summary
    printf("\n🎯 Test Summary\n");
    printf("===============\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success Rate: %.1f%%\n", (float)passed_tests / total_tests * 100.0f);
    
    if (passed_tests == total_tests) {
        printf("\n🎉 All tests PASSED! Configuration system is working correctly.\n");
        return 0;
    } else {
        printf("\n❌ Some tests FAILED! Configuration system needs attention.\n");
        return 1;
    }
}
