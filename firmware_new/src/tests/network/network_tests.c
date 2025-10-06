/**
 * @file network_tests.c
 * @brief Network Manager Tests - OHT-50 Robot Network Management Testing
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Test suite cho Network Manager implementation
 * bao gồm unit tests, integration tests, và performance tests.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include "network_manager.h"
#include "wifi_manager.h"
#include "network_api.h"
#include "network_config.h"

// Test Configuration
#define MAX_TEST_NETWORKS 10
#define TEST_TIMEOUT_MS 5000
#define PERFORMANCE_THRESHOLD_MS 100

// Test Statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Test Helper Functions
static void test_start(const char *test_name);
static void test_pass(const char *test_name);
static void test_fail(const char *test_name, const char *reason);
static uint64_t get_timestamp_ms(void);
static void print_test_results(void);

// Network Manager Tests
static void test_network_manager_init(void);
static void test_network_manager_connect_wifi(void);
static void test_network_manager_disconnect_wifi(void);
static void test_network_manager_scan_networks(void);
static void test_network_manager_get_status(void);
static void test_network_manager_roaming(void);
static void test_network_manager_mobile_app(void);
static void test_network_manager_config(void);
static void test_network_manager_performance(void);

// WiFi Manager Tests
static void test_wifi_manager_init(void);
static void test_wifi_manager_scan(void);
static void test_wifi_manager_connect(void);
static void test_wifi_manager_disconnect(void);
static void test_wifi_manager_signal_strength(void);
static void test_wifi_manager_roaming(void);
static void test_wifi_manager_statistics(void);

// Network API Tests
static void test_network_api_status(void);
static void test_network_api_wifi_connect(void);
static void test_network_api_wifi_scan(void);
static void test_network_api_performance(void);
static void test_network_api_health(void);
static void test_network_api_auth(void);

// Network Configuration Tests
static void test_network_config_load_save(void);
static void test_network_config_validation(void);
static void test_network_config_defaults(void);

// Performance Tests
static void test_performance_network_status(void);
static void test_performance_wifi_scan(void);
static void test_performance_wifi_connect(void);

/**
 * @brief Main test runner
 */
int main(void) {
    printf("=== OHT-50 Network Manager Test Suite ===\n\n");
    
    // Network Manager Tests
    printf("--- Network Manager Tests ---\n");
    test_network_manager_init();
    test_network_manager_connect_wifi();
    test_network_manager_disconnect_wifi();
    test_network_manager_scan_networks();
    test_network_manager_get_status();
    test_network_manager_roaming();
    test_network_manager_mobile_app();
    test_network_manager_config();
    test_network_manager_performance();
    
    // WiFi Manager Tests
    printf("\n--- WiFi Manager Tests ---\n");
    test_wifi_manager_init();
    test_wifi_manager_scan();
    test_wifi_manager_connect();
    test_wifi_manager_disconnect();
    test_wifi_manager_signal_strength();
    test_wifi_manager_roaming();
    test_wifi_manager_statistics();
    
    // Network API Tests
    printf("\n--- Network API Tests ---\n");
    test_network_api_status();
    test_network_api_wifi_connect();
    test_network_api_wifi_scan();
    test_network_api_performance();
    test_network_api_health();
    test_network_api_auth();
    
    // Network Configuration Tests
    printf("\n--- Network Configuration Tests ---\n");
    test_network_config_load_save();
    test_network_config_validation();
    test_network_config_defaults();
    
    // Performance Tests
    printf("\n--- Performance Tests ---\n");
    test_performance_network_status();
    test_performance_wifi_scan();
    test_performance_wifi_connect();
    
    // Print Results
    printf("\n=== Test Results ===\n");
    print_test_results();
    
    return (tests_failed > 0) ? 1 : 0;
}

// Network Manager Tests Implementation

/**
 * @brief Test Network Manager initialization
 */
static void test_network_manager_init(void) {
    test_start("Network Manager Init");
    
    int result = network_manager_init();
    if (result == NETWORK_SUCCESS) {
        test_pass("Network Manager Init");
    } else {
        test_fail("Network Manager Init", "Initialization failed");
    }
}

/**
 * @brief Test WiFi connection
 */
static void test_network_manager_connect_wifi(void) {
    test_start("Network Manager Connect WiFi");
    
    const char *test_ssid = "OHT-50-Test";
    const char *test_password = "test_password_123";
    
    int result = network_manager_connect_wifi(test_ssid, test_password);
    if (result == NETWORK_SUCCESS) {
        test_pass("Network Manager Connect WiFi");
    } else {
        test_fail("Network Manager Connect WiFi", "Connection failed");
    }
}

/**
 * @brief Test WiFi disconnection
 */
static void test_network_manager_disconnect_wifi(void) {
    test_start("Network Manager Disconnect WiFi");
    
    int result = network_manager_disconnect_wifi();
    if (result == NETWORK_SUCCESS) {
        test_pass("Network Manager Disconnect WiFi");
    } else {
        test_fail("Network Manager Disconnect WiFi", "Disconnection failed");
    }
}

/**
 * @brief Test network scanning
 */
static void test_network_manager_scan_networks(void) {
    test_start("Network Manager Scan Networks");
    
    wifi_network_t networks[MAX_TEST_NETWORKS];
    int count = network_manager_scan_networks(networks, MAX_TEST_NETWORKS);
    
    if (count > 0) {
        printf("  Found %d networks:\n", count);
        for (int i = 0; i < count; i++) {
            printf("    - %s (Signal: %d dBm, Security: %s)\n", 
                   networks[i].ssid, networks[i].signal_strength, networks[i].security);
        }
        test_pass("Network Manager Scan Networks");
    } else {
        test_fail("Network Manager Scan Networks", "No networks found");
    }
}

/**
 * @brief Test network status
 */
static void test_network_manager_get_status(void) {
    test_start("Network Manager Get Status");
    
    oht_network_status_t status;
    int result = network_manager_get_status(&status);
    
    if (result == NETWORK_SUCCESS) {
        printf("  Connected: %s\n", status.connected ? "Yes" : "No");
        printf("  SSID: %s\n", status.current_ssid);
        printf("  Signal: %d dBm\n", status.signal_strength);
        printf("  IP: %s\n", status.ip_address);
        printf("  Latency: %.1f ms\n", status.latency_ms);
        test_pass("Network Manager Get Status");
    } else {
        test_fail("Network Manager Get Status", "Failed to get status");
    }
}

/**
 * @brief Test roaming functionality
 */
static void test_network_manager_roaming(void) {
    test_start("Network Manager Roaming");
    
    int result = network_manager_enable_roaming(true);
    if (result == NETWORK_SUCCESS) {
        result = network_manager_enable_roaming(false);
        if (result == NETWORK_SUCCESS) {
            test_pass("Network Manager Roaming");
        } else {
            test_fail("Network Manager Roaming", "Failed to disable roaming");
        }
    } else {
        test_fail("Network Manager Roaming", "Failed to enable roaming");
    }
}

/**
 * @brief Test mobile app support
 */
static void test_network_manager_mobile_app(void) {
    test_start("Network Manager Mobile App");
    
    int result = network_manager_enable_mobile_app(true);
    if (result == NETWORK_SUCCESS) {
        result = network_manager_enable_mobile_app(false);
        if (result == NETWORK_SUCCESS) {
            test_pass("Network Manager Mobile App");
        } else {
            test_fail("Network Manager Mobile App", "Failed to disable mobile app");
        }
    } else {
        test_fail("Network Manager Mobile App", "Failed to enable mobile app");
    }
}

/**
 * @brief Test configuration management
 */
static void test_network_manager_config(void) {
    test_start("Network Manager Config");
    
    oht_network_config_t config;
    int result = network_manager_get_config(&config);
    
    if (result == NETWORK_SUCCESS) {
        printf("  WiFi Enabled: %s\n", config.wifi_enabled ? "Yes" : "No");
        printf("  SSID: %s\n", config.wifi_ssid);
        printf("  Security: %d\n", config.wifi_security_type);
        printf("  Roaming: %s\n", config.roaming_enabled ? "Enabled" : "Disabled");
        
        result = network_manager_set_config(&config);
        if (result == NETWORK_SUCCESS) {
            test_pass("Network Manager Config");
        } else {
            test_fail("Network Manager Config", "Failed to set config");
        }
    } else {
        test_fail("Network Manager Config", "Failed to get config");
    }
}

/**
 * @brief Test performance metrics
 */
static void test_network_manager_performance(void) {
    test_start("Network Manager Performance");
    
    performance_metrics_t metrics;
    int result = network_manager_get_performance(&metrics);
    
    if (result == NETWORK_SUCCESS) {
        printf("  Response Time: %u ms\n", metrics.response_time_ms);
        printf("  Request Count: %u\n", metrics.request_count);
        printf("  Error Count: %u\n", metrics.error_count);
        printf("  Success Rate: %.1f%%\n", metrics.success_rate);
        test_pass("Network Manager Performance");
    } else {
        test_fail("Network Manager Performance", "Failed to get performance metrics");
    }
}

// WiFi Manager Tests Implementation

/**
 * @brief Test WiFi Manager initialization
 */
static void test_wifi_manager_init(void) {
    test_start("WiFi Manager Init");
    
    int result = wifi_manager_init();
    if (result == WIFI_SUCCESS) {
        test_pass("WiFi Manager Init");
    } else {
        test_fail("WiFi Manager Init", "Initialization failed");
    }
}

/**
 * @brief Test WiFi scanning
 */
static void test_wifi_manager_scan(void) {
    test_start("WiFi Manager Scan");
    
    wifi_scan_result_t networks[MAX_TEST_NETWORKS];
    int count = wifi_manager_scan_networks(networks, MAX_TEST_NETWORKS, TEST_TIMEOUT_MS);
    
    if (count > 0) {
        printf("  Found %d networks:\n", count);
        for (int i = 0; i < count; i++) {
            printf("    - %s (Signal: %d dBm, Quality: %d)\n", 
                   networks[i].ssid, networks[i].signal_strength_dbm, networks[i].signal_quality);
        }
        test_pass("WiFi Manager Scan");
    } else {
        test_fail("WiFi Manager Scan", "No networks found");
    }
}

/**
 * @brief Test WiFi connection
 */
static void test_wifi_manager_connect(void) {
    test_start("WiFi Manager Connect");
    
    wifi_connection_params_t params = {
        .ssid = "OHT-50-Test",
        .password = "test_password_123",
        .security_type = OHT_WIFI_SECURITY_WPA2,
        .preferred_band = WIFI_BAND_5G,
        .auto_connect = true,
        .connection_timeout_ms = TEST_TIMEOUT_MS
    };
    
    int result = wifi_manager_connect(&params);
    if (result == WIFI_SUCCESS) {
        test_pass("WiFi Manager Connect");
    } else {
        test_fail("WiFi Manager Connect", "Connection failed");
    }
}

/**
 * @brief Test WiFi disconnection
 */
static void test_wifi_manager_disconnect(void) {
    test_start("WiFi Manager Disconnect");
    
    int result = wifi_manager_disconnect();
    if (result == WIFI_SUCCESS) {
        test_pass("WiFi Manager Disconnect");
    } else {
        test_fail("WiFi Manager Disconnect", "Disconnection failed");
    }
}

/**
 * @brief Test signal strength
 */
static void test_wifi_manager_signal_strength(void) {
    test_start("WiFi Manager Signal Strength");
    
    int signal = wifi_manager_get_signal_strength();
    wifi_signal_quality_t quality = wifi_manager_get_signal_quality();
    
    printf("  Signal Strength: %d dBm\n", signal);
    printf("  Signal Quality: %d\n", quality);
    
    test_pass("WiFi Manager Signal Strength");
}

/**
 * @brief Test roaming functionality
 */
static void test_wifi_manager_roaming(void) {
    test_start("WiFi Manager Roaming");
    
    wifi_roaming_config_t config = {
        .enabled = true,
        .signal_threshold_dbm = -70,
        .scan_interval_ms = 5000,
        .handover_timeout_ms = 2000,
        .aggressive_roaming = false
    };
    
    int result = wifi_manager_enable_roaming(&config);
    if (result == WIFI_SUCCESS) {
        result = wifi_manager_handle_roaming();
        if (result == WIFI_SUCCESS) {
            test_pass("WiFi Manager Roaming");
        } else {
            test_fail("WiFi Manager Roaming", "Failed to handle roaming");
        }
    } else {
        test_fail("WiFi Manager Roaming", "Failed to enable roaming");
    }
}

/**
 * @brief Test statistics
 */
static void test_wifi_manager_statistics(void) {
    test_start("WiFi Manager Statistics");
    
    wifi_statistics_t stats;
    int result = wifi_manager_get_statistics(&stats);
    
    if (result == WIFI_SUCCESS) {
        printf("  Connection Attempts: %u\n", stats.connection_attempts);
        printf("  Successful Connections: %u\n", stats.successful_connections);
        printf("  Failed Connections: %u\n", stats.failed_connections);
        printf("  Success Rate: %.1f%%\n", stats.connection_success_rate);
        printf("  Roaming Events: %u\n", stats.roaming_events);
        test_pass("WiFi Manager Statistics");
    } else {
        test_fail("WiFi Manager Statistics", "Failed to get statistics");
    }
}

// Network API Tests Implementation

/**
 * @brief Test Network API status endpoint
 */
static void test_network_api_status(void) {
    test_start("Network API Status");
    
    http_request_t req = {
        .method = "GET",
        .path = "/api/v1/network/status",
        .headers = "",
        .body = "",
        .auth_token = "",
        .required_auth = AUTH_NONE
    };
    
    http_response_t resp;
    int result = network_api_handle_request(&req, &resp);
    
    if (result == 0 && resp.status_code == 200) {
        printf("  Status Code: %d\n", resp.status_code);
        printf("  Response: %s\n", resp.body);
        test_pass("Network API Status");
    } else {
        test_fail("Network API Status", "API request failed");
    }
}

/**
 * @brief Test Network API WiFi connect endpoint
 */
static void test_network_api_wifi_connect(void) {
    test_start("Network API WiFi Connect");
    
    http_request_t req = {
        .method = "POST",
        .path = "/api/v1/network/wifi/connect",
        .headers = "Content-Type: application/json",
        .body = "{\"ssid\": \"OHT-50-Test\", \"password\": \"test_password_123\"}",
        .auth_token = "oht50_operator_token_2025",
        .required_auth = AUTH_OPERATOR
    };
    
    http_response_t resp;
    int result = network_api_handle_request(&req, &resp);
    
    if (result == 0 && resp.status_code == 200) {
        printf("  Status Code: %d\n", resp.status_code);
        printf("  Response: %s\n", resp.body);
        test_pass("Network API WiFi Connect");
    } else {
        test_fail("Network API WiFi Connect", "API request failed");
    }
}

/**
 * @brief Test Network API WiFi scan endpoint
 */
static void test_network_api_wifi_scan(void) {
    test_start("Network API WiFi Scan");
    
    http_request_t req = {
        .method = "GET",
        .path = "/api/v1/network/wifi/scan",
        .headers = "",
        .body = "",
        .auth_token = "",
        .required_auth = AUTH_NONE
    };
    
    http_response_t resp;
    int result = network_api_handle_request(&req, &resp);
    
    if (result == 0 && resp.status_code == 200) {
        printf("  Status Code: %d\n", resp.status_code);
        printf("  Response: %s\n", resp.body);
        test_pass("Network API WiFi Scan");
    } else {
        test_fail("Network API WiFi Scan", "API request failed");
    }
}

/**
 * @brief Test Network API performance endpoint
 */
static void test_network_api_performance(void) {
    test_start("Network API Performance");
    
    http_request_t req = {
        .method = "GET",
        .path = "/api/v1/network/performance",
        .headers = "",
        .body = "",
        .auth_token = "",
        .required_auth = AUTH_NONE
    };
    
    http_response_t resp;
    int result = network_api_handle_request(&req, &resp);
    
    if (result == 0 && resp.status_code == 200) {
        printf("  Status Code: %d\n", resp.status_code);
        printf("  Response: %s\n", resp.body);
        test_pass("Network API Performance");
    } else {
        test_fail("Network API Performance", "API request failed");
    }
}

/**
 * @brief Test Network API health endpoint
 */
static void test_network_api_health(void) {
    test_start("Network API Health");
    
    http_request_t req = {
        .method = "GET",
        .path = "/api/v1/network/health",
        .headers = "",
        .body = "",
        .auth_token = "",
        .required_auth = AUTH_NONE
    };
    
    http_response_t resp;
    int result = network_api_handle_request(&req, &resp);
    
    if (result == 0 && resp.status_code == 200) {
        printf("  Status Code: %d\n", resp.status_code);
        printf("  Response: %s\n", resp.body);
        test_pass("Network API Health");
    } else {
        test_fail("Network API Health", "API request failed");
    }
}

/**
 * @brief Test Network API authentication
 */
static void test_network_api_auth(void) {
    test_start("Network API Auth");
    
    // Test valid authentication
    bool valid = network_api_validate_auth("oht50_operator_token_2025", AUTH_OPERATOR);
    if (valid) {
        // Test invalid authentication
        valid = network_api_validate_auth("invalid_token", AUTH_OPERATOR);
        if (!valid) {
            test_pass("Network API Auth");
        } else {
            test_fail("Network API Auth", "Invalid token accepted");
        }
    } else {
        test_fail("Network API Auth", "Valid token rejected");
    }
}

// Network Configuration Tests Implementation

/**
 * @brief Test configuration load/save
 */
static void test_network_config_load_save(void) {
    test_start("Network Config Load/Save");
    
    oht_network_config_t config;
    int result = network_config_load(&config);
    
    if (result == CONFIG_VALID) {
        printf("  Loaded SSID: %s\n", config.wifi_ssid);
        printf("  WiFi Enabled: %s\n", config.wifi_enabled ? "Yes" : "No");
        
        result = network_config_save(&config);
        if (result == CONFIG_VALID) {
            test_pass("Network Config Load/Save");
        } else {
            test_fail("Network Config Load/Save", "Failed to save config");
        }
    } else {
        test_fail("Network Config Load/Save", "Failed to load config");
    }
}

/**
 * @brief Test configuration validation
 */
static void test_network_config_validation(void) {
    test_start("Network Config Validation");
    
    oht_network_config_t config;
    network_config_create_default(&config);
    
    config_validation_result_t result = network_config_validate(&config);
    if (result == CONFIG_VALID) {
        // Test invalid SSID
        config.wifi_ssid[0] = '\0';
        result = network_config_validate(&config);
        if (result == CONFIG_INVALID_SSID) {
            test_pass("Network Config Validation");
        } else {
            test_fail("Network Config Validation", "Invalid SSID not detected");
        }
    } else {
        test_fail("Network Config Validation", "Valid config rejected");
    }
}

/**
 * @brief Test default configuration
 */
static void test_network_config_defaults(void) {
    test_start("Network Config Defaults");
    
    oht_network_config_t config;
    int result = network_config_create_default(&config);
    
    if (result == CONFIG_VALID) {
        printf("  Default SSID: %s\n", config.wifi_ssid);
        printf("  Default Security: %d\n", config.wifi_security_type);
        printf("  Default Signal Threshold: %d\n", config.signal_strength);
        
        result = network_config_reset();
        if (result == CONFIG_VALID) {
            test_pass("Network Config Defaults");
        } else {
            test_fail("Network Config Defaults", "Failed to reset config");
        }
    } else {
        test_fail("Network Config Defaults", "Failed to create default config");
    }
}

// Performance Tests Implementation

/**
 * @brief Test network status performance
 */
static void test_performance_network_status(void) {
    test_start("Performance Network Status");
    
    uint64_t start_time = get_timestamp_ms();
    
    oht_network_status_t status;
    int result = network_manager_get_status(&status);
    
    uint64_t end_time = get_timestamp_ms();
    uint64_t response_time = end_time - start_time;
    
    printf("  Response Time: %lu ms\n", response_time);
    
    if (result == NETWORK_SUCCESS && response_time <= PERFORMANCE_THRESHOLD_MS) {
        test_pass("Performance Network Status");
    } else {
        test_fail("Performance Network Status", "Response time too slow");
    }
}

/**
 * @brief Test WiFi scan performance
 */
static void test_performance_wifi_scan(void) {
    test_start("Performance WiFi Scan");
    
    uint64_t start_time = get_timestamp_ms();
    
    wifi_network_t networks[MAX_TEST_NETWORKS];
    int count = network_manager_scan_networks(networks, MAX_TEST_NETWORKS);
    
    uint64_t end_time = get_timestamp_ms();
    uint64_t response_time = end_time - start_time;
    
    printf("  Response Time: %lu ms\n", response_time);
    printf("  Networks Found: %d\n", count);
    
    if (count > 0 && response_time <= 10000) { // 10 second threshold for scan
        test_pass("Performance WiFi Scan");
    } else {
        test_fail("Performance WiFi Scan", "Scan too slow or no networks");
    }
}

/**
 * @brief Test WiFi connect performance
 */
static void test_performance_wifi_connect(void) {
    test_start("Performance WiFi Connect");
    
    uint64_t start_time = get_timestamp_ms();
    
    int result = network_manager_connect_wifi("OHT-50-Test", "test_password_123");
    
    uint64_t end_time = get_timestamp_ms();
    uint64_t response_time = end_time - start_time;
    
    printf("  Response Time: %lu ms\n", response_time);
    
    if (result == NETWORK_SUCCESS && response_time <= 5000) { // 5 second threshold for connect
        test_pass("Performance WiFi Connect");
    } else {
        test_fail("Performance WiFi Connect", "Connection too slow or failed");
    }
}

// Test Helper Functions Implementation

/**
 * @brief Start test
 */
static void test_start(const char *test_name) {
    tests_run++;
    printf("  [%d] %s... ", tests_run, test_name);
    fflush(stdout);
}

/**
 * @brief Test passed
 */
static void test_pass(const char *test_name) {
    tests_passed++;
    printf("PASS\n");
}

/**
 * @brief Test failed
 */
static void test_fail(const char *test_name, const char *reason) {
    tests_failed++;
    printf("FAIL (%s)\n", reason);
}

/**
 * @brief Get current timestamp in milliseconds
 */
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * @brief Print test results
 */
static void print_test_results(void) {
    printf("Tests Run: %d\n", tests_run);
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("Success Rate: %.1f%%\n", (float)tests_passed * 100.0f / tests_run);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! Network Manager is ready for deployment.\n");
    } else {
        printf("\n❌ %d test(s) failed. Please fix issues before deployment.\n", tests_failed);
    }
}
