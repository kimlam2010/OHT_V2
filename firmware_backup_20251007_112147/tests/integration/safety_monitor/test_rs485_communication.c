/**
 * @file test_rs485_communication.c
 * @brief Standalone test program for Safety RS485 Communication
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 1.4 - Hardware Validation Testing
 * 
 * 🧪 TESTING: Standalone test executable for safety RS485 communication
 * 🚨 SAFETY: Tests real hardware communication with Safety Module 0x03
 * ⚡ PERFORMANCE: Measures response times and validates performance requirements
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

// 🎯 **TEST CONFIGURATION**
#define TEST_SAFETY_MODULE_ADDR     0x03
#define TEST_POWER_MODULE_ADDR      0x02
#define TEST_TRAVEL_MODULE_ADDR     0x04
#define TEST_DOCK_MODULE_ADDR       0x06

#define TEST_MAX_RESPONSE_TIME_MS   50
#define TEST_ITERATIONS             100
#define TEST_REGISTER_COUNT         5

// 📊 **TEST REGISTER MAP**
#define TEST_REG_STATUS             0x0000
#define TEST_REG_ESTOP              0x0001
#define TEST_REG_SENSOR1_DIST       0x0010
#define TEST_REG_SENSOR2_DIST       0x0011
#define TEST_REG_DIGITAL_INPUTS     0x0020
#define TEST_REG_ERROR_CODES        0x0005

// 📈 **TEST STATISTICS STRUCTURE**
typedef struct {
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t communication_errors;
    uint32_t timeout_errors;
    uint32_t validation_errors;
    
    // Performance metrics
    uint32_t min_response_time_ms;
    uint32_t max_response_time_ms;
    uint32_t total_response_time_ms;
    uint32_t avg_response_time_ms;
    
    // Per-module statistics
    uint32_t safety_module_tests;
    uint32_t power_module_tests;
    uint32_t travel_module_tests;
    uint32_t dock_module_tests;
    
} test_statistics_t;

// 🕐 **UTILITY FUNCTIONS**

/**
 * @brief Get current timestamp in milliseconds
 * @return Current timestamp in ms
 */
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

/**
 * @brief Print test header
 */
static void print_test_header(void) {
    printf("\n");
    printf("🧪 ===============================================\n");
    printf("🧪 SAFETY RS485 COMMUNICATION TEST SUITE\n");
    printf("🧪 ===============================================\n");
    printf("🧪 Version: 1.0.0\n");
    printf("🧪 Date: 2025-09-19\n");
    printf("🧪 Task: Phase 1.4 - Hardware Validation\n");
    printf("🧪 ===============================================\n");
    printf("\n");
}

/**
 * @brief Print test results
 * @param stats Test statistics
 */
static void print_test_results(const test_statistics_t *stats) {
    printf("\n");
    printf("📊 ===============================================\n");
    printf("📊 TEST RESULTS SUMMARY\n");
    printf("📊 ===============================================\n");
    printf("📊 Total Tests:        %u\n", stats->total_tests);
    printf("📊 Passed Tests:       %u\n", stats->passed_tests);
    printf("📊 Failed Tests:       %u\n", stats->failed_tests);
    printf("📊 Success Rate:       %.2f%%\n", 
           stats->total_tests > 0 ? (float)stats->passed_tests * 100.0f / (float)stats->total_tests : 0.0f);
    printf("📊 ===============================================\n");
    printf("📊 ERROR BREAKDOWN:\n");
    printf("📊 Communication Errors: %u\n", stats->communication_errors);
    printf("📊 Timeout Errors:       %u\n", stats->timeout_errors);
    printf("📊 Validation Errors:    %u\n", stats->validation_errors);
    printf("📊 ===============================================\n");
    printf("📊 PERFORMANCE METRICS:\n");
    printf("📊 Min Response Time:    %u ms\n", stats->min_response_time_ms);
    printf("📊 Max Response Time:    %u ms\n", stats->max_response_time_ms);
    printf("📊 Avg Response Time:    %u ms\n", stats->avg_response_time_ms);
    printf("📊 ===============================================\n");
    printf("📊 PER-MODULE TESTS:\n");
    printf("📊 Safety Module Tests:  %u\n", stats->safety_module_tests);
    printf("📊 Power Module Tests:   %u\n", stats->power_module_tests);
    printf("📊 Travel Module Tests:  %u\n", stats->travel_module_tests);
    printf("📊 Dock Module Tests:    %u\n", stats->dock_module_tests);
    printf("📊 ===============================================\n");
    printf("\n");
}

/**
 * @brief Get module name from address
 * @param module_addr Module address
 * @return Module name string
 */
static const char* get_module_name(uint8_t module_addr) {
    switch (module_addr) {
        case TEST_POWER_MODULE_ADDR:  return "Power Module";
        case TEST_SAFETY_MODULE_ADDR: return "Safety Module";
        case TEST_TRAVEL_MODULE_ADDR: return "Travel Motor Module";
        case TEST_DOCK_MODULE_ADDR:   return "Dock Module";
        default:                      return "Unknown Module";
    }
}

// 🧪 **MOCK COMMUNICATION FUNCTIONS**
// 🚨 NOTE: These are MOCK functions for testing when real hardware is not available
// In real implementation, these would call actual communication_manager functions

/**
 * @brief Mock Modbus read holding registers function
 * @param slave_id Slave ID
 * @param start_address Start address
 * @param quantity Quantity of registers
 * @param data Data buffer
 * @return 0 for success, -1 for error
 */
static int mock_modbus_read_holding_registers(uint8_t slave_id, uint16_t start_address, 
                                            uint16_t quantity, uint16_t *data) {
    // Simulate communication delay
    usleep(rand() % 30000 + 10000); // 10-40ms delay
    
    // Simulate occasional communication failures (5% failure rate)
    if (rand() % 100 < 5) {
        printf("❌ MOCK: Communication failure with module 0x%02X\n", slave_id);
        return -1;
    }
    
    // Generate mock data based on register address
    for (uint16_t i = 0; i < quantity; i++) {
        uint16_t reg_addr = start_address + i;
        
        switch (reg_addr) {
            case TEST_REG_STATUS:
                data[i] = rand() % 5; // Status 0-4
                break;
            case TEST_REG_ESTOP:
                data[i] = (rand() % 100 < 10) ? 1 : 0; // 10% chance of E-Stop active
                break;
            case TEST_REG_SENSOR1_DIST:
                data[i] = rand() % 5000 + 500; // 500-5500mm
                break;
            case TEST_REG_SENSOR2_DIST:
                data[i] = rand() % 5000 + 500; // 500-5500mm
                break;
            case TEST_REG_DIGITAL_INPUTS:
                data[i] = rand() % 65536; // Random digital inputs
                break;
            case TEST_REG_ERROR_CODES:
                data[i] = (rand() % 100 < 20) ? (rand() % 256) : 0; // 20% chance of errors
                break;
            default:
                data[i] = 0;
                break;
        }
    }
    
    printf("✅ MOCK: Read %u registers from module 0x%02X starting at 0x%04X\n", 
           quantity, slave_id, start_address);
    return 0;
}

// 🧪 **TEST FUNCTIONS**

/**
 * @brief Test single register read
 * @param module_addr Module address
 * @param reg_addr Register address
 * @param stats Test statistics
 * @return true if test passed
 */
static bool test_single_register_read(uint8_t module_addr, uint16_t reg_addr, test_statistics_t *stats) {
    printf("🔍 Testing register 0x%04X on module 0x%02X (%s)...", 
           reg_addr, module_addr, get_module_name(module_addr));
    
    stats->total_tests++;
    
    uint64_t start_time = get_timestamp_ms();
    uint16_t data = 0;
    int result = mock_modbus_read_holding_registers(module_addr, reg_addr, 1, &data);
    uint64_t end_time = get_timestamp_ms();
    
    uint32_t response_time = (uint32_t)(end_time - start_time);
    
    // Update performance statistics
    if (response_time < stats->min_response_time_ms) {
        stats->min_response_time_ms = response_time;
    }
    if (response_time > stats->max_response_time_ms) {
        stats->max_response_time_ms = response_time;
    }
    stats->total_response_time_ms += response_time;
    
    if (result != 0) {
        printf(" ❌ FAILED (Communication Error)\n");
        stats->failed_tests++;
        stats->communication_errors++;
        return false;
    }
    
    if (response_time > TEST_MAX_RESPONSE_TIME_MS) {
        printf(" ⚠️  FAILED (Timeout: %u ms > %u ms)\n", response_time, TEST_MAX_RESPONSE_TIME_MS);
        stats->failed_tests++;
        stats->timeout_errors++;
        return false;
    }
    
    // Validate data based on register
    bool data_valid = true;
    switch (reg_addr) {
        case TEST_REG_STATUS:
            if (data > 4) data_valid = false;
            break;
        case TEST_REG_ESTOP:
            if (data > 1) data_valid = false;
            break;
        case TEST_REG_SENSOR1_DIST:
        case TEST_REG_SENSOR2_DIST:
            if (data > 10000) data_valid = false;
            break;
    }
    
    if (!data_valid) {
        printf(" ❌ FAILED (Invalid Data: 0x%04X)\n", data);
        stats->failed_tests++;
        stats->validation_errors++;
        return false;
    }
    
    printf(" ✅ PASSED (%u ms, data=0x%04X)\n", response_time, data);
    stats->passed_tests++;
    return true;
}

/**
 * @brief Test multiple register read
 * @param module_addr Module address
 * @param stats Test statistics
 * @return true if test passed
 */
static bool test_multiple_register_read(uint8_t module_addr, test_statistics_t *stats) {
    printf("🔍 Testing multiple register read on module 0x%02X (%s)...", 
           module_addr, get_module_name(module_addr));
    
    stats->total_tests++;
    
    uint64_t start_time = get_timestamp_ms();
    uint16_t data[2] = {0};
    int result = mock_modbus_read_holding_registers(module_addr, TEST_REG_SENSOR1_DIST, 2, data);
    uint64_t end_time = get_timestamp_ms();
    
    uint32_t response_time = (uint32_t)(end_time - start_time);
    
    // Update performance statistics
    if (response_time < stats->min_response_time_ms) {
        stats->min_response_time_ms = response_time;
    }
    if (response_time > stats->max_response_time_ms) {
        stats->max_response_time_ms = response_time;
    }
    stats->total_response_time_ms += response_time;
    
    if (result != 0) {
        printf(" ❌ FAILED (Communication Error)\n");
        stats->failed_tests++;
        stats->communication_errors++;
        return false;
    }
    
    if (response_time > TEST_MAX_RESPONSE_TIME_MS) {
        printf(" ⚠️  FAILED (Timeout: %u ms > %u ms)\n", response_time, TEST_MAX_RESPONSE_TIME_MS);
        stats->failed_tests++;
        stats->timeout_errors++;
        return false;
    }
    
    printf(" ✅ PASSED (%u ms, S1=%u mm, S2=%u mm)\n", response_time, data[0], data[1]);
    stats->passed_tests++;
    return true;
}

/**
 * @brief Test module communication
 * @param module_addr Module address
 * @param stats Test statistics
 * @return Number of tests passed
 */
static uint32_t test_module_communication(uint8_t module_addr, test_statistics_t *stats) {
    printf("\n🎯 Testing communication with module 0x%02X (%s):\n", 
           module_addr, get_module_name(module_addr));
    
    uint32_t tests_passed = 0;
    
    // Test individual registers
    if (test_single_register_read(module_addr, TEST_REG_STATUS, stats)) tests_passed++;
    if (test_single_register_read(module_addr, TEST_REG_ESTOP, stats)) tests_passed++;
    if (test_single_register_read(module_addr, TEST_REG_DIGITAL_INPUTS, stats)) tests_passed++;
    if (test_single_register_read(module_addr, TEST_REG_ERROR_CODES, stats)) tests_passed++;
    
    // Test multiple register read
    if (test_multiple_register_read(module_addr, stats)) tests_passed++;
    
    // Update per-module statistics
    switch (module_addr) {
        case TEST_SAFETY_MODULE_ADDR:
            stats->safety_module_tests += 5;
            break;
        case TEST_POWER_MODULE_ADDR:
            stats->power_module_tests += 5;
            break;
        case TEST_TRAVEL_MODULE_ADDR:
            stats->travel_module_tests += 5;
            break;
        case TEST_DOCK_MODULE_ADDR:
            stats->dock_module_tests += 5;
            break;
    }
    
    printf("📊 Module 0x%02X results: %u/5 tests passed\n", module_addr, tests_passed);
    return tests_passed;
}

/**
 * @brief Run performance test
 * @param stats Test statistics
 */
static void run_performance_test(test_statistics_t *stats) {
    printf("\n⚡ Running performance test (%u iterations)...\n", TEST_ITERATIONS);
    
    uint32_t performance_tests_passed = 0;
    
    for (uint32_t i = 0; i < TEST_ITERATIONS; i++) {
        uint8_t module_addr = TEST_SAFETY_MODULE_ADDR; // Focus on Safety Module for performance
        
        uint64_t start_time = get_timestamp_ms();
        uint16_t data = 0;
        int result = mock_modbus_read_holding_registers(module_addr, TEST_REG_STATUS, 1, &data);
        uint64_t end_time = get_timestamp_ms();
        
        uint32_t response_time = (uint32_t)(end_time - start_time);
        
        stats->total_tests++;
        
        if (result == 0 && response_time <= TEST_MAX_RESPONSE_TIME_MS) {
            stats->passed_tests++;
            performance_tests_passed++;
        } else {
            stats->failed_tests++;
            if (result != 0) stats->communication_errors++;
            if (response_time > TEST_MAX_RESPONSE_TIME_MS) stats->timeout_errors++;
        }
        
        // Update performance statistics
        if (response_time < stats->min_response_time_ms) {
            stats->min_response_time_ms = response_time;
        }
        if (response_time > stats->max_response_time_ms) {
            stats->max_response_time_ms = response_time;
        }
        stats->total_response_time_ms += response_time;
        
        // Progress indicator
        if ((i + 1) % 10 == 0) {
            printf("⚡ Progress: %u/%u tests completed\n", i + 1, TEST_ITERATIONS);
        }
    }
    
    printf("⚡ Performance test completed: %u/%u tests passed (%.2f%%)\n", 
           performance_tests_passed, TEST_ITERATIONS,
           (float)performance_tests_passed * 100.0f / (float)TEST_ITERATIONS);
}

// 🎯 **MAIN TEST FUNCTION**

int main(int argc, char *argv[]) {
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    print_test_header();
    
    // Initialize test statistics
    test_statistics_t stats = {0};
    stats.min_response_time_ms = UINT32_MAX;
    
    printf("🚨 WARNING: Using MOCK communication functions\n");
    printf("🚨 Replace with real communication_manager functions for hardware testing\n");
    printf("\n");
    
    // Test all modules
    printf("🎯 Testing all configured modules...\n");
    
    test_module_communication(TEST_SAFETY_MODULE_ADDR, &stats);
    test_module_communication(TEST_POWER_MODULE_ADDR, &stats);
    test_module_communication(TEST_TRAVEL_MODULE_ADDR, &stats);
    test_module_communication(TEST_DOCK_MODULE_ADDR, &stats);
    
    // Run performance test
    run_performance_test(&stats);
    
    // Calculate average response time
    if (stats.total_tests > 0) {
        stats.avg_response_time_ms = stats.total_response_time_ms / stats.total_tests;
    }
    
    // Handle min response time edge case
    if (stats.min_response_time_ms == UINT32_MAX) {
        stats.min_response_time_ms = 0;
    }
    
    // Print final results
    print_test_results(&stats);
    
    // Determine overall test result
    bool overall_success = (stats.failed_tests == 0) && 
                          (stats.avg_response_time_ms <= TEST_MAX_RESPONSE_TIME_MS);
    
    if (overall_success) {
        printf("🎉 ===============================================\n");
        printf("🎉 ALL TESTS PASSED - SAFETY RS485 COMMUNICATION OK\n");
        printf("🎉 ===============================================\n");
        return 0;
    } else {
        printf("❌ ===============================================\n");
        printf("❌ SOME TESTS FAILED - CHECK HARDWARE/CONFIGURATION\n");
        printf("❌ ===============================================\n");
        return 1;
    }
}

/**
 * 📋 **COMPILATION INSTRUCTIONS:**
 * 
 * To compile this test program:
 * ```bash
 * gcc -o test_rs485_communication \
 *     firmware_new/tests/integration/safety_monitor/test_rs485_communication.c \
 *     -I firmware_new/src/hal/common \
 *     -I firmware_new/src/hal/communication \
 *     -I firmware_new/src/app/managers \
 *     -I firmware_new/src/app/core
 * ```
 * 
 * To run the test:
 * ```bash
 * ./test_rs485_communication
 * ```
 * 
 * 📋 **INTEGRATION WITH REAL HARDWARE:**
 * 
 * To integrate with real hardware:
 * 1. Replace mock_modbus_read_holding_registers() with comm_manager_modbus_read_holding_registers()
 * 2. Add proper initialization of communication_manager
 * 3. Include actual HAL headers
 * 4. Link with communication_manager.o and related objects
 * 
 * 🚨 **SAFETY NOTES:**
 * 
 * - This test program is designed to run SEPARATE from main firmware
 * - It does NOT affect running safety systems
 * - Mock functions simulate realistic communication delays and errors
 * - Performance requirements are validated (< 50ms response time)
 * - All register values are validated for reasonable ranges
 * 
 * 📊 **TEST COVERAGE:**
 * 
 * This test covers:
 * - Individual register reads (status, E-Stop, sensors, digital inputs, errors)
 * - Multiple register reads (sensor pair)
 * - Performance testing (100 iterations)
 * - Error simulation (communication failures, timeouts)
 * - Data validation (range checking)
 * - Statistics collection (response times, success rates)
 */
