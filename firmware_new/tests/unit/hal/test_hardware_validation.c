/**
 * @file test_hardware_validation.c
 * @brief Hardware validation tests for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team EMBED
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "unity.h"
#include "hal_common.h"
#include "hal_gpio.h"
#include "hal_led.h"
#include "hal_estop.h"
#include "hal_relay.h"
#include "hal_rs485.h"
#include "hal_network.h"

// Test configuration
#define TEST_TIMEOUT_MS 5000
#define TEST_GPIO_PIN 54  // LED_POWER_PIN
#define TEST_RS485_DEVICE "/dev/ttyOHT485"
#define TEST_NETWORK_INTERFACE "eth0"

// Test results
static struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} test_results = {0};

void setUp(void) {
    // Initialize test environment
    test_results.total_tests++;
}

void tearDown(void) {
    // Cleanup after each test
}

// Test 1: GPIO Hardware Validation
void test_gpio_hardware_validation(void) {
    printf("\n=== GPIO Hardware Validation Test ===\n");
    
    // Test GPIO pin access
    char gpio_path[64];
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d", TEST_GPIO_PIN);
    
    // Check if GPIO directory exists
    if (access(gpio_path, F_OK) == 0) {
        printf("✅ GPIO pin %d directory exists\n", TEST_GPIO_PIN);
        test_results.passed_tests++;
    } else {
        printf("❌ GPIO pin %d directory not found\n", TEST_GPIO_PIN);
        test_results.failed_tests++;
        TEST_ASSERT_TRUE(false);
    }
    
    // Test GPIO export
    FILE *fp = fopen("/sys/class/gpio/export", "w");
    if (fp) {
        fprintf(fp, "%d", TEST_GPIO_PIN);
        fclose(fp);
        usleep(100000); // 100ms delay
        
        if (access(gpio_path, F_OK) == 0) {
            printf("✅ GPIO pin %d export successful\n", TEST_GPIO_PIN);
            test_results.passed_tests++;
        } else {
            printf("❌ GPIO pin %d export failed\n", TEST_GPIO_PIN);
            test_results.failed_tests++;
            TEST_ASSERT_TRUE(false);
        }
    } else {
        printf("❌ Cannot access GPIO export\n");
        test_results.failed_tests++;
        TEST_ASSERT_TRUE(false);
    }
}

// Test 2: RS485 Hardware Validation
void test_rs485_hardware_validation(void) {
    printf("\n=== RS485 Hardware Validation Test ===\n");
    
    // Check if RS485 device exists
    if (access(TEST_RS485_DEVICE, F_OK) == 0) {
        printf("✅ RS485 device %s exists\n", TEST_RS485_DEVICE);
        test_results.passed_tests++;
    } else {
        printf("⚠️  RS485 device %s not found (may need udev rules)\n", TEST_RS485_DEVICE);
        // Don't fail test, just warn
    }
    
    // Check if UART1 device exists
    if (access("/dev/ttyS1", F_OK) == 0) {
        printf("✅ UART1 device /dev/ttyS1 exists\n");
        test_results.passed_tests++;
    } else {
        printf("❌ UART1 device /dev/ttyS1 not found\n");
        test_results.failed_tests++;
        TEST_ASSERT_TRUE(false);
    }
}

// Test 3: Network Hardware Validation
void test_network_hardware_validation(void) {
    printf("\n=== Network Hardware Validation Test ===\n");
    
    // Check if network interface exists
    char interface_path[64];
    snprintf(interface_path, sizeof(interface_path), "/sys/class/net/%s", TEST_NETWORK_INTERFACE);
    
    if (access(interface_path, F_OK) == 0) {
        printf("✅ Network interface %s exists\n", TEST_NETWORK_INTERFACE);
        test_results.passed_tests++;
    } else {
        printf("⚠️  Network interface %s not found (may be wlan0)\n", TEST_NETWORK_INTERFACE);
        // Check for WiFi interface
        if (access("/sys/class/net/wlan0", F_OK) == 0) {
            printf("✅ WiFi interface wlan0 exists\n");
            test_results.passed_tests++;
        } else {
            printf("❌ No network interfaces found\n");
            test_results.failed_tests++;
            TEST_ASSERT_TRUE(false);
        }
    }
}

// Test 4: LED Hardware Validation
void test_led_hardware_validation(void) {
    printf("\n=== LED Hardware Validation Test ===\n");
    
    // Test LED GPIO pins
    int led_pins[] = {LED_POWER_PIN, LED_SYSTEM_PIN, LED_COMM_PIN, LED_NETWORK_PIN, LED_ERROR_PIN};
    char *led_names[] = {"Power", "System", "Communication", "Network", "Error"};
    
    for (int i = 0; i < 5; i++) {
        char gpio_path[64];
        snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d", led_pins[i]);
        
        // Export LED pin
        FILE *fp = fopen("/sys/class/gpio/export", "w");
        if (fp) {
            fprintf(fp, "%d", led_pins[i]);
            fclose(fp);
            usleep(50000); // 50ms delay
            
            if (access(gpio_path, F_OK) == 0) {
                printf("✅ %s LED pin %d accessible\n", led_names[i], led_pins[i]);
                test_results.passed_tests++;
            } else {
                printf("❌ %s LED pin %d not accessible\n", led_names[i], led_pins[i]);
                test_results.failed_tests++;
            }
        }
    }
}

// Test 5: E-Stop Hardware Validation
void test_estop_hardware_validation(void) {
    printf("\n=== E-Stop Hardware Validation Test ===\n");
    
    // Test E-Stop GPIO pin
    char gpio_path[64];
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d", ESTOP_PIN);
    
    // Export E-Stop pin
    FILE *fp = fopen("/sys/class/gpio/export", "w");
    if (fp) {
        fprintf(fp, "%d", ESTOP_PIN);
        fclose(fp);
        usleep(50000); // 50ms delay
        
        if (access(gpio_path, F_OK) == 0) {
            printf("✅ E-Stop pin %d accessible\n", ESTOP_PIN);
            test_results.passed_tests++;
        } else {
            printf("❌ E-Stop pin %d not accessible\n", ESTOP_PIN);
            test_results.failed_tests++;
            TEST_ASSERT_TRUE(false);
        }
    }
}

// Test 6: Relay Hardware Validation
void test_relay_hardware_validation(void) {
    printf("\n=== Relay Hardware Validation Test ===\n");
    
    // Test Relay GPIO pins
    int relay_pins[] = {RELAY1_OUTPUT_PIN, RELAY2_OUTPUT_PIN};
    char *relay_names[] = {"Relay 1", "Relay 2"};
    
    for (int i = 0; i < 2; i++) {
        char gpio_path[64];
        snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d", relay_pins[i]);
        
        // Export relay pin
        FILE *fp = fopen("/sys/class/gpio/export", "w");
        if (fp) {
            fprintf(fp, "%d", relay_pins[i]);
            fclose(fp);
            usleep(50000); // 50ms delay
            
            if (access(gpio_path, F_OK) == 0) {
                printf("✅ %s pin %d accessible\n", relay_names[i], relay_pins[i]);
                test_results.passed_tests++;
            } else {
                printf("❌ %s pin %d not accessible\n", relay_names[i], relay_pins[i]);
                test_results.failed_tests++;
            }
        }
    }
}

// Test 7: System Resources Validation
void test_system_resources_validation(void) {
    printf("\n=== System Resources Validation Test ===\n");
    
    // Check system memory
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp) {
        char line[256];
        unsigned long total_mem = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                sscanf(line, "MemTotal: %lu", &total_mem);
                break;
            }
        }
        fclose(fp);
        
        if (total_mem > 0) {
            printf("✅ System memory: %lu KB\n", total_mem);
            test_results.passed_tests++;
        } else {
            printf("❌ Cannot read system memory\n");
            test_results.failed_tests++;
        }
    }
    
    // Check CPU info
    fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char line[256];
        int cpu_count = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "processor", 9) == 0) {
                cpu_count++;
            }
        }
        fclose(fp);
        
        if (cpu_count > 0) {
            printf("✅ CPU cores: %d\n", cpu_count);
            test_results.passed_tests++;
        } else {
            printf("❌ Cannot read CPU info\n");
            test_results.failed_tests++;
        }
    }
}

// Main test runner
int main(void) {
    printf("=== OHT-50 Master Module Hardware Validation Tests ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    
    UNITY_BEGIN();
    
    // Run hardware validation tests
    RUN_TEST(test_gpio_hardware_validation);
    RUN_TEST(test_rs485_hardware_validation);
    RUN_TEST(test_network_hardware_validation);
    RUN_TEST(test_led_hardware_validation);
    RUN_TEST(test_estop_hardware_validation);
    RUN_TEST(test_relay_hardware_validation);
    RUN_TEST(test_system_resources_validation);
    
    // Print test summary
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", test_results.total_tests);
    printf("Passed: %d\n", test_results.passed_tests);
    printf("Failed: %d\n", test_results.failed_tests);
    printf("Success rate: %.1f%%\n", 
           (float)test_results.passed_tests / test_results.total_tests * 100.0);
    
    if (test_results.failed_tests == 0) {
        printf("✅ All hardware validation tests PASSED!\n");
    } else {
        printf("❌ Some hardware validation tests FAILED!\n");
    }
    
    UNITY_END();
    return 0;
}
