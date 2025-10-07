#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

// Include network manager headers
#include "../../app/managers/network/network_manager.h"
#include "../../app/managers/network/wifi_manager.h"
#include "../../app/config/network/network_config.h"

// Test utilities
static uint32_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

// Test counters
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

static void test_start(const char *test_name) {
    tests_run++;
    printf("  [%d] %s... ", tests_run, test_name);
    fflush(stdout);
}

static void test_pass(const char *test_name) {
    tests_passed++;
    printf("PASS\n");
}

static void test_fail(const char *test_name, const char *reason) {
    tests_failed++;
    printf("FAIL (%s)\n", reason);
}

// Test Network Manager Initialization
void test_network_manager_init(void) {
    test_start("Network Manager Init");
    
    int result = network_manager_init();
    if (result == NETWORK_SUCCESS) {
        test_pass("Network Manager Init");
    } else {
        test_fail("Network Manager Init", "Initialization failed");
    }
}

// Test Network Status
void test_network_status(void) {
    test_start("Network Status");
    
    oht_network_status_t status;
    int result = network_manager_get_status(&status);
    
    if (result == NETWORK_SUCCESS) {
        printf("    Connected: %s, SSID: %s, Signal: %d dBm\n", 
               status.connected ? "Yes" : "No", 
               status.current_ssid, 
               status.signal_strength);
        test_pass("Network Status");
    } else {
        test_fail("Network Status", "Failed to get status");
    }
}

// Test Network Configuration
void test_network_config(void) {
    test_start("Network Configuration");
    
    oht_network_config_t config;
    int result = network_manager_get_config(&config);
    
    if (result == NETWORK_SUCCESS) {
        printf("    WiFi: %s, SSID: %s, Security: %d\n", 
               config.wifi_enabled ? "Enabled" : "Disabled",
               config.wifi_ssid,
               config.wifi_security_type);
        test_pass("Network Configuration");
    } else {
        test_fail("Network Configuration", "Failed to get config");
    }
}

// Test WiFi Connection
void test_wifi_connection(void) {
    test_start("WiFi Connection");
    
    // Test connection
    int result = network_manager_connect_wifi("TestSSID", "TestPassword");
    if (result == NETWORK_SUCCESS) {
        // Test disconnection
        result = network_manager_disconnect_wifi();
        if (result == NETWORK_SUCCESS) {
            test_pass("WiFi Connection");
        } else {
            test_fail("WiFi Connection", "Disconnection failed");
        }
    } else {
        test_fail("WiFi Connection", "Connection failed");
    }
}

// Test WiFi Scanning
void test_wifi_scanning(void) {
    test_start("WiFi Scanning");
    
    wifi_network_t networks[5];
    int count = network_manager_scan_networks(networks, 5);
    
    if (count >= 0) {
        printf("    Found %d networks\n", count);
        for (int i = 0; i < count && i < 3; i++) {
            printf("      - %s (Signal: %d dBm)\n", 
                   networks[i].ssid, networks[i].signal_strength);
        }
        test_pass("WiFi Scanning");
    } else {
        test_fail("WiFi Scanning", "Scan failed");
    }
}

// Test Roaming Features
void test_roaming_features(void) {
    test_start("Roaming Features");
    
    int result = network_manager_enable_roaming(true);
    if (result == NETWORK_SUCCESS) {
        result = network_manager_enable_roaming(false);
        if (result == NETWORK_SUCCESS) {
            test_pass("Roaming Features");
        } else {
            test_fail("Roaming Features", "Failed to disable roaming");
        }
    } else {
        test_fail("Roaming Features", "Failed to enable roaming");
    }
}

// Test Mobile App Features
void test_mobile_app_features(void) {
    test_start("Mobile App Features");
    
    int result = network_manager_enable_mobile_app(true);
    if (result == NETWORK_SUCCESS) {
        result = network_manager_enable_mobile_app(false);
        if (result == NETWORK_SUCCESS) {
            test_pass("Mobile App Features");
        } else {
            test_fail("Mobile App Features", "Failed to disable mobile app");
        }
    } else {
        test_fail("Mobile App Features", "Failed to enable mobile app");
    }
}

// Test Error Handling
void test_error_handling(void) {
    test_start("Error Handling");
    
    // Test NULL parameters
    int result = network_manager_get_status(NULL);
    if (result == NETWORK_ERROR_INVALID_PARAM) {
        result = network_manager_get_config(NULL);
        if (result == NETWORK_ERROR_INVALID_PARAM) {
            test_pass("Error Handling");
        } else {
            test_fail("Error Handling", "Config NULL handling failed");
        }
    } else {
        test_fail("Error Handling", "Status NULL handling failed");
    }
}

// Test Performance
void test_performance(void) {
    test_start("Performance Test");
    
    const int iterations = 100;
    uint32_t start_time = get_timestamp_ms();
    
    for (int i = 0; i < iterations; i++) {
        oht_network_status_t status;
        network_manager_get_status(&status);
    }
    
    uint32_t end_time = get_timestamp_ms();
    float avg_time = (float)(end_time - start_time) / iterations;
    
    printf("    Avg time: %.3f ms per call\n", avg_time);
    
    if (avg_time < 1.0f) {
        test_pass("Performance Test");
    } else {
        test_fail("Performance Test", "Too slow");
    }
}

// Main test runner
int main(void) {
    printf("🧪 Network Management Phase 1 Testing\n");
    printf("=====================================\n\n");
    
    // Run all tests
    test_network_manager_init();
    test_network_status();
    test_network_config();
    test_wifi_connection();
    test_wifi_scanning();
    test_roaming_features();
    test_mobile_app_features();
    test_error_handling();
    test_performance();
    
    // Print results
    printf("\n=== Test Results ===\n");
    printf("Tests Run: %d\n", tests_run);
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("Success Rate: %.1f%%\n", (float)tests_passed * 100.0f / tests_run);
    
    if (tests_failed == 0) {
        printf("\n🎉 All Network Management Phase 1 tests passed!\n");
        printf("✅ Ready for Phase 2 implementation\n");
        return 0;
    } else {
        printf("\n❌ %d test(s) failed. Please fix issues before Phase 2.\n", tests_failed);
        return 1;
    }
}
