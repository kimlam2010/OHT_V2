#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/hal_network.h"
#include "../include/hal_common.h"

// Test configuration
#define TEST_DELAY_MS 1000

// Test results
static int tests_passed = 0;
static int tests_failed = 0;

// Test function prototypes
static void test_network_init(void);
static void test_ethernet_basic(void);
static void test_network_status(void);

// Helper functions
static void print_test_result(const char *test_name, bool passed);
static void delay_ms(uint32_t ms);
static void network_event_callback(network_state_t state, network_type_t type);

int main(void) {
    printf("=== Network System Test Program ===\n");
    printf("Testing Master Module network system (Ethernet + WiFi)...\n\n");

    // Run simplified tests
    test_network_init();
    test_ethernet_basic();
    test_network_status();

    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    printf("Success rate: %.1f%%\n", 
           (float)tests_passed / (tests_passed + tests_failed) * 100.0);

    // Cleanup
    hal_network_deinit();

    return (tests_failed == 0) ? 0 : 1;
}

static void test_network_init(void) {
    printf("Testing network initialization...\n");
    
    // Set up callback
    hal_status_t status = hal_network_set_callback(network_event_callback);
    if (status != HAL_STATUS_OK) {
        print_test_result("Network Set Callback", false);
        tests_failed++;
        return;
    }
    
    status = hal_network_init(NULL);
    bool passed = (status == HAL_STATUS_OK);
    
    print_test_result("Network Init", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_ethernet_basic(void) {
    printf("Testing Ethernet basic functions...\n");
    
    bool passed = true;
    
    // Test Ethernet init
    hal_status_t status = hal_ethernet_init();
    if (status != HAL_STATUS_OK) {
        passed = false;
        printf("Ethernet init failed\n");
    } else {
        printf("Ethernet interface initialized\n");
    }
    
    // Test Ethernet connect (simulated)
    status = hal_ethernet_connect();
    if (status != HAL_STATUS_OK) {
        passed = false;
        printf("Ethernet connect failed\n");
    } else {
        printf("Ethernet connect successful\n");
    }
    
    print_test_result("Ethernet Basic Functions", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void test_network_status(void) {
    printf("Testing network status...\n");
    
    bool passed = true;
    
    // Test get status
    network_status_t status_info;
    hal_status_t status = hal_network_get_status(&status_info);
    if (status != HAL_STATUS_OK) {
        passed = false;
        printf("Get status failed\n");
    } else {
        printf("Network status: State=%d, Type=%d, IP=%s\n", 
               status_info.state, status_info.active_type, status_info.ip_address);
    }
    
    // Test get config
    network_config_t config;
    status = hal_network_get_config(&config);
    if (status != HAL_STATUS_OK) {
        passed = false;
        printf("Get config failed\n");
    } else {
        printf("Network config: DHCP=%s, IP=%s\n", 
               config.dhcp_enabled ? "enabled" : "disabled", config.ip_address);
    }
    
    print_test_result("Network Status", passed);
    
    if (passed) {
        tests_passed++;
    } else {
        tests_failed++;
    }
}

static void print_test_result(const char *test_name, bool passed) {
    if (passed) {
        printf("  ✅ %s: PASSED\n", test_name);
    } else {
        printf("  ❌ %s: FAILED\n", test_name);
    }
}

static void delay_ms(uint32_t ms) {
    usleep(ms * 1000);
}

static void network_event_callback(network_state_t state, network_type_t type) {
    printf("Network Event: State=%d, Type=%d\n", state, type);
}
