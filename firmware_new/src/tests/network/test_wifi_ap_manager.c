/**
 * @file test_wifi_ap_manager.c
 * @brief WiFi AP Manager Unit Tests - OHT-50 Robot WiFi Access Point Management Tests
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Unit tests cho WiFi AP Manager functionality
 * bao gồm AP start/stop, client management, và fallback testing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include "wifi_ap_manager.h"
#include "hal_wifi_ap.h"

// Test Configuration
#define TEST_SSID "OHT-50-Test-Hotspot"
#define TEST_PASSWORD "test_password_2025"
#define TEST_IP "192.168.5.1"
#define TEST_NETMASK "255.255.255.0"
#define TEST_CHANNEL 11
#define TEST_MAX_CLIENTS 5

// Test Statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Test Helper Functions
static void test_assert(bool condition, const char *test_name, const char *message);
static void test_wifi_ap_manager_init(void);
static void test_wifi_ap_manager_deinit(void);
static void test_wifi_ap_manager_start(void);
static void test_wifi_ap_manager_stop(void);
static void test_wifi_ap_manager_get_status(void);
static void test_wifi_ap_manager_get_clients(void);
static void test_wifi_ap_manager_kick_client(void);
static void test_wifi_ap_manager_get_config(void);
static void test_wifi_ap_manager_set_config(void);
static void test_wifi_ap_manager_get_statistics(void);
static void test_wifi_ap_manager_reset_statistics(void);
static void test_wifi_ap_manager_validate_config(void);
static void test_wifi_ap_manager_set_auto_fallback(void);
static void test_wifi_ap_manager_trigger_fallback(void);
static void test_wifi_ap_manager_get_fallback_status(void);
static void test_wifi_ap_manager_error_messages(void);
static void test_wifi_ap_manager_edge_cases(void);
static void test_wifi_ap_manager_performance(void);

/**
 * @brief Test assertion helper
 */
static void test_assert(bool condition, const char *test_name, const char *message) {
    tests_run++;
    
    if (condition) {
        tests_passed++;
        printf("✅ PASS: %s - %s\n", test_name, message);
    } else {
        tests_failed++;
        printf("❌ FAIL: %s - %s\n", test_name, message);
    }
}

/**
 * @brief Test WiFi AP Manager initialization
 */
static void test_wifi_ap_manager_init(void) {
    printf("\n🔧 Testing WiFi AP Manager Initialization...\n");
    
    // Test successful initialization
    int result = wifi_ap_manager_init();
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_init", 
                "Should initialize successfully");
    
    // Test double initialization
    result = wifi_ap_manager_init();
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_init_double", 
                "Should handle double initialization gracefully");
    
    // Test manager is running
    bool is_running = wifi_ap_manager_is_running();
    test_assert(!is_running, "wifi_ap_manager_is_running_init", 
                "Should not be running after initialization");
}

/**
 * @brief Test WiFi AP Manager deinitialization
 */
static void test_wifi_ap_manager_deinit(void) {
    printf("\n🔧 Testing WiFi AP Manager Deinitialization...\n");
    
    // Test successful deinitialization
    int result = wifi_ap_manager_deinit();
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_deinit", 
                "Should deinitialize successfully");
    
    // Test double deinitialization
    result = wifi_ap_manager_deinit();
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_deinit_double", 
                "Should handle double deinitialization gracefully");
    
    // Reinitialize for other tests
    wifi_ap_manager_init();
}

/**
 * @brief Test WiFi AP Manager start functionality
 */
static void test_wifi_ap_manager_start(void) {
    printf("\n🔧 Testing WiFi AP Manager Start...\n");
    
    // Test start with valid configuration
    wifi_ap_config_t config = {
        .ap_enabled = true,
        .ap_channel = TEST_CHANNEL,
        .max_clients = TEST_MAX_CLIENTS,
        .auto_fallback = true,
        .fallback_timeout_ms = 30000
    };
    strncpy(config.ap_ssid, TEST_SSID, sizeof(config.ap_ssid) - 1);
    strncpy(config.ap_password, TEST_PASSWORD, sizeof(config.ap_password) - 1);
    strncpy(config.ap_ip, TEST_IP, sizeof(config.ap_ip) - 1);
    strncpy(config.ap_netmask, TEST_NETMASK, sizeof(config.ap_netmask) - 1);
    config.ap_security_type = AP_SECURITY_WPA2;
    
    int result = wifi_ap_manager_start(&config);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_start_valid", 
                "Should start with valid configuration");
    
    // Test manager is running
    bool is_running = wifi_ap_manager_is_running();
    test_assert(is_running, "wifi_ap_manager_is_running_start", 
                "Should be running after start");
    
    // Test start when already running
    result = wifi_ap_manager_start(&config);
    test_assert(result == WIFI_AP_ERROR_ALREADY_RUNNING, "wifi_ap_manager_start_running", 
                "Should fail when already running");
    
    // Test start with NULL config
    result = wifi_ap_manager_start(NULL);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_start_null", 
                "Should fail with NULL configuration");
    
    // Test start with invalid SSID
    wifi_ap_config_t invalid_config = config;
    strncpy(invalid_config.ap_ssid, "", sizeof(invalid_config.ap_ssid) - 1);
    result = wifi_ap_manager_start(&invalid_config);
    test_assert(result == WIFI_AP_ERROR_INVALID_SSID, "wifi_ap_manager_start_invalid_ssid", 
                "Should fail with invalid SSID");
    
    // Test start with weak password
    invalid_config = config;
    strncpy(invalid_config.ap_password, "123", sizeof(invalid_config.ap_password) - 1);
    result = wifi_ap_manager_start(&invalid_config);
    test_assert(result == WIFI_AP_ERROR_WEAK_PASSWORD, "wifi_ap_manager_start_weak_password", 
                "Should fail with weak password");
}

/**
 * @brief Test WiFi AP Manager stop functionality
 */
static void test_wifi_ap_manager_stop(void) {
    printf("\n🔧 Testing WiFi AP Manager Stop...\n");
    
    // Test stop when running
    int result = wifi_ap_manager_stop();
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_stop_running", 
                "Should stop when running");
    
    // Test manager is not running
    bool is_running = wifi_ap_manager_is_running();
    test_assert(!is_running, "wifi_ap_manager_is_running_stop", 
                "Should not be running after stop");
    
    // Test stop when not running
    result = wifi_ap_manager_stop();
    test_assert(result == WIFI_AP_ERROR_NOT_RUNNING, "wifi_ap_manager_stop_not_running", 
                "Should fail when not running");
}

/**
 * @brief Test WiFi AP Manager status functionality
 */
static void test_wifi_ap_manager_get_status(void) {
    printf("\n🔧 Testing WiFi AP Manager Status...\n");
    
    // Test get status when not running
    wifi_ap_status_t status;
    int result = wifi_ap_manager_get_status(&status);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_get_status", 
                "Should get status successfully");
    
    test_assert(status.status == AP_STATUS_DISABLED, "wifi_ap_manager_status_disabled", 
                "Status should be disabled when not running");
    
    test_assert(!status.ap_enabled, "wifi_ap_manager_status_not_enabled", 
                "AP should not be enabled when not running");
    
    // Test get status with NULL parameter
    result = wifi_ap_manager_get_status(NULL);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_get_status_null", 
                "Should fail with NULL parameter");
    
    // Start AP and test status
    wifi_ap_config_t config = {
        .ap_enabled = true,
        .ap_channel = TEST_CHANNEL,
        .max_clients = TEST_MAX_CLIENTS,
        .auto_fallback = true,
        .fallback_timeout_ms = 30000
    };
    strncpy(config.ap_ssid, TEST_SSID, sizeof(config.ap_ssid) - 1);
    strncpy(config.ap_password, TEST_PASSWORD, sizeof(config.ap_password) - 1);
    strncpy(config.ap_ip, TEST_IP, sizeof(config.ap_ip) - 1);
    strncpy(config.ap_netmask, TEST_NETMASK, sizeof(config.ap_netmask) - 1);
    config.ap_security_type = AP_SECURITY_WPA2;
    
    wifi_ap_manager_start(&config);
    
    result = wifi_ap_manager_get_status(&status);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_get_status_running", 
                "Should get status when running");
    
    test_assert(status.status == AP_STATUS_RUNNING, "wifi_ap_manager_status_running", 
                "Status should be running when AP is active");
    
    test_assert(status.ap_enabled, "wifi_ap_manager_status_enabled", 
                "AP should be enabled when running");
    
    test_assert(strcmp(status.ap_ssid, TEST_SSID) == 0, "wifi_ap_manager_status_ssid", 
                "SSID should match configuration");
    
    test_assert(strcmp(status.ap_ip, TEST_IP) == 0, "wifi_ap_manager_status_ip", 
                "IP should match configuration");
    
    test_assert(status.ap_channel == TEST_CHANNEL, "wifi_ap_manager_status_channel", 
                "Channel should match configuration");
    
    test_assert(status.max_clients == TEST_MAX_CLIENTS, "wifi_ap_manager_status_max_clients", 
                "Max clients should match configuration");
}

/**
 * @brief Test WiFi AP Manager client management
 */
static void test_wifi_ap_manager_get_clients(void) {
    printf("\n🔧 Testing WiFi AP Manager Client Management...\n");
    
    // Test get clients when not running
    wifi_ap_client_t clients[10];
    int result = wifi_ap_manager_get_clients(clients, 10);
    test_assert(result == WIFI_AP_ERROR_NOT_RUNNING, "wifi_ap_manager_get_clients_not_running", 
                "Should fail when not running");
    
    // Start AP
    wifi_ap_config_t config = {
        .ap_enabled = true,
        .ap_channel = TEST_CHANNEL,
        .max_clients = TEST_MAX_CLIENTS,
        .auto_fallback = true,
        .fallback_timeout_ms = 30000
    };
    strncpy(config.ap_ssid, TEST_SSID, sizeof(config.ap_ssid) - 1);
    strncpy(config.ap_password, TEST_PASSWORD, sizeof(config.ap_password) - 1);
    strncpy(config.ap_ip, TEST_IP, sizeof(config.ap_ip) - 1);
    strncpy(config.ap_netmask, TEST_NETMASK, sizeof(config.ap_netmask) - 1);
    config.ap_security_type = AP_SECURITY_WPA2;
    
    wifi_ap_manager_start(&config);
    
    // Test get clients when running
    result = wifi_ap_manager_get_clients(clients, 10);
    test_assert(result >= 0, "wifi_ap_manager_get_clients_running", 
                "Should get clients when running");
    
    // Test get clients with NULL parameter
    result = wifi_ap_manager_get_clients(NULL, 10);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_get_clients_null", 
                "Should fail with NULL parameter");
    
    // Test get clients with invalid max_count
    result = wifi_ap_manager_get_clients(clients, 0);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_get_clients_invalid_count", 
                "Should fail with invalid max_count");
}

/**
 * @brief Test WiFi AP Manager kick client functionality
 */
static void test_wifi_ap_manager_kick_client(void) {
    printf("\n🔧 Testing WiFi AP Manager Kick Client...\n");
    
    // Test kick client when not running
    int result = wifi_ap_manager_kick_client("aa:bb:cc:dd:ee:ff");
    test_assert(result == WIFI_AP_ERROR_NOT_RUNNING, "wifi_ap_manager_kick_client_not_running", 
                "Should fail when not running");
    
    // Start AP
    wifi_ap_config_t config = {
        .ap_enabled = true,
        .ap_channel = TEST_CHANNEL,
        .max_clients = TEST_MAX_CLIENTS,
        .auto_fallback = true,
        .fallback_timeout_ms = 30000
    };
    strncpy(config.ap_ssid, TEST_SSID, sizeof(config.ap_ssid) - 1);
    strncpy(config.ap_password, TEST_PASSWORD, sizeof(config.ap_password) - 1);
    strncpy(config.ap_ip, TEST_IP, sizeof(config.ap_ip) - 1);
    strncpy(config.ap_netmask, TEST_NETMASK, sizeof(config.ap_netmask) - 1);
    config.ap_security_type = AP_SECURITY_WPA2;
    
    wifi_ap_manager_start(&config);
    
    // Test kick client when running
    result = wifi_ap_manager_kick_client("aa:bb:cc:dd:ee:ff");
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_kick_client_running", 
                "Should kick client when running");
    
    // Test kick client with NULL parameter
    result = wifi_ap_manager_kick_client(NULL);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_kick_client_null", 
                "Should fail with NULL parameter");
}

/**
 * @brief Test WiFi AP Manager configuration management
 */
static void test_wifi_ap_manager_get_config(void) {
    printf("\n🔧 Testing WiFi AP Manager Configuration...\n");
    
    // Test get configuration
    wifi_ap_config_t config;
    int result = wifi_ap_manager_get_config(&config);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_get_config", 
                "Should get configuration successfully");
    
    test_assert(strcmp(config.ap_ssid, "OHT-50-Hotspot") == 0, "wifi_ap_manager_config_ssid", 
                "Default SSID should be correct");
    
    test_assert(config.ap_channel == 6, "wifi_ap_manager_config_channel", 
                "Default channel should be correct");
    
    test_assert(config.max_clients == 10, "wifi_ap_manager_config_max_clients", 
                "Default max clients should be correct");
    
    // Test get configuration with NULL parameter
    result = wifi_ap_manager_get_config(NULL);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_get_config_null", 
                "Should fail with NULL parameter");
}

/**
 * @brief Test WiFi AP Manager set configuration
 */
static void test_wifi_ap_manager_set_config(void) {
    printf("\n🔧 Testing WiFi AP Manager Set Configuration...\n");
    
    // Test set valid configuration
    wifi_ap_config_t config = {
        .ap_enabled = true,
        .ap_channel = TEST_CHANNEL,
        .max_clients = TEST_MAX_CLIENTS,
        .auto_fallback = true,
        .fallback_timeout_ms = 30000
    };
    strncpy(config.ap_ssid, TEST_SSID, sizeof(config.ap_ssid) - 1);
    strncpy(config.ap_password, TEST_PASSWORD, sizeof(config.ap_password) - 1);
    strncpy(config.ap_ip, TEST_IP, sizeof(config.ap_ip) - 1);
    strncpy(config.ap_netmask, TEST_NETMASK, sizeof(config.ap_netmask) - 1);
    config.ap_security_type = AP_SECURITY_WPA2;
    
    int result = wifi_ap_manager_set_config(&config);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_set_config", 
                "Should set configuration successfully");
    
    // Verify configuration was set
    wifi_ap_config_t retrieved_config;
    result = wifi_ap_manager_get_config(&retrieved_config);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_get_config_after_set", 
                "Should get configuration after set");
    
    test_assert(strcmp(retrieved_config.ap_ssid, TEST_SSID) == 0, "wifi_ap_manager_config_ssid_set", 
                "SSID should be set correctly");
    
    test_assert(retrieved_config.ap_channel == TEST_CHANNEL, "wifi_ap_manager_config_channel_set", 
                "Channel should be set correctly");
    
    test_assert(retrieved_config.max_clients == TEST_MAX_CLIENTS, "wifi_ap_manager_config_max_clients_set", 
                "Max clients should be set correctly");
    
    // Test set configuration with NULL parameter
    result = wifi_ap_manager_set_config(NULL);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_set_config_null", 
                "Should fail with NULL parameter");
}

/**
 * @brief Test WiFi AP Manager statistics
 */
static void test_wifi_ap_manager_get_statistics(void) {
    printf("\n🔧 Testing WiFi AP Manager Statistics...\n");
    
    // Test get statistics
    wifi_ap_statistics_t stats;
    int result = wifi_ap_manager_get_statistics(&stats);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_get_statistics", 
                "Should get statistics successfully");
    
    // Test get statistics with NULL parameter
    result = wifi_ap_manager_get_statistics(NULL);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_get_statistics_null", 
                "Should fail with NULL parameter");
}

/**
 * @brief Test WiFi AP Manager reset statistics
 */
static void test_wifi_ap_manager_reset_statistics(void) {
    printf("\n🔧 Testing WiFi AP Manager Reset Statistics...\n");
    
    // Test reset statistics
    int result = wifi_ap_manager_reset_statistics();
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_reset_statistics", 
                "Should reset statistics successfully");
}

/**
 * @brief Test WiFi AP Manager configuration validation
 */
static void test_wifi_ap_manager_validate_config(void) {
    printf("\n🔧 Testing WiFi AP Manager Configuration Validation...\n");
    
    // Test valid configuration
    wifi_ap_config_t valid_config = {
        .ap_enabled = true,
        .ap_channel = 6,
        .max_clients = 10,
        .auto_fallback = true,
        .fallback_timeout_ms = 30000
    };
    strncpy(valid_config.ap_ssid, "ValidSSID", sizeof(valid_config.ap_ssid) - 1);
    strncpy(valid_config.ap_password, "valid_password_123", sizeof(valid_config.ap_password) - 1);
    strncpy(valid_config.ap_ip, "192.168.1.1", sizeof(valid_config.ap_ip) - 1);
    strncpy(valid_config.ap_netmask, "255.255.255.0", sizeof(valid_config.ap_netmask) - 1);
    valid_config.ap_security_type = AP_SECURITY_WPA2;
    
    int result = wifi_ap_manager_validate_config(&valid_config);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_validate_config_valid", 
                "Should validate valid configuration");
    
    // Test invalid SSID
    wifi_ap_config_t invalid_config = valid_config;
    strncpy(invalid_config.ap_ssid, "", sizeof(invalid_config.ap_ssid) - 1);
    result = wifi_ap_manager_validate_config(&invalid_config);
    test_assert(result == WIFI_AP_ERROR_INVALID_SSID, "wifi_ap_manager_validate_config_invalid_ssid", 
                "Should fail with invalid SSID");
    
    // Test weak password
    invalid_config = valid_config;
    strncpy(invalid_config.ap_password, "123", sizeof(invalid_config.ap_password) - 1);
    result = wifi_ap_manager_validate_config(&invalid_config);
    test_assert(result == WIFI_AP_ERROR_WEAK_PASSWORD, "wifi_ap_manager_validate_config_weak_password", 
                "Should fail with weak password");
    
    // Test invalid channel
    invalid_config = valid_config;
    invalid_config.ap_channel = 0;
    result = wifi_ap_manager_validate_config(&invalid_config);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_validate_config_invalid_channel", 
                "Should fail with invalid channel");
    
    // Test NULL configuration
    result = wifi_ap_manager_validate_config(NULL);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_validate_config_null", 
                "Should fail with NULL configuration");
}

/**
 * @brief Test WiFi AP Manager auto-fallback functionality
 */
static void test_wifi_ap_manager_set_auto_fallback(void) {
    printf("\n🔧 Testing WiFi AP Manager Auto-Fallback...\n");
    
    // Test set auto-fallback enabled
    int result = wifi_ap_manager_set_auto_fallback(true, 30000);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_set_auto_fallback_enabled", 
                "Should set auto-fallback enabled");
    
    // Test set auto-fallback disabled
    result = wifi_ap_manager_set_auto_fallback(false, 0);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_set_auto_fallback_disabled", 
                "Should set auto-fallback disabled");
}

/**
 * @brief Test WiFi AP Manager trigger fallback
 */
static void test_wifi_ap_manager_trigger_fallback(void) {
    printf("\n🔧 Testing WiFi AP Manager Trigger Fallback...\n");
    
    // Test trigger fallback
    int result = wifi_ap_manager_trigger_fallback();
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_trigger_fallback", 
                "Should trigger fallback successfully");
}

/**
 * @brief Test WiFi AP Manager get fallback status
 */
static void test_wifi_ap_manager_get_fallback_status(void) {
    printf("\n🔧 Testing WiFi AP Manager Get Fallback Status...\n");
    
    // Test get fallback status
    bool enabled;
    uint32_t timeout_ms;
    int result = wifi_ap_manager_get_fallback_status(&enabled, &timeout_ms);
    test_assert(result == WIFI_AP_SUCCESS, "wifi_ap_manager_get_fallback_status", 
                "Should get fallback status successfully");
    
    // Test get fallback status with NULL parameters
    result = wifi_ap_manager_get_fallback_status(NULL, &timeout_ms);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_get_fallback_status_null_enabled", 
                "Should fail with NULL enabled parameter");
    
    result = wifi_ap_manager_get_fallback_status(&enabled, NULL);
    test_assert(result == WIFI_AP_ERROR_INVALID_PARAM, "wifi_ap_manager_get_fallback_status_null_timeout", 
                "Should fail with NULL timeout parameter");
}

/**
 * @brief Test WiFi AP Manager error messages
 */
static void test_wifi_ap_manager_error_messages(void) {
    printf("\n🔧 Testing WiFi AP Manager Error Messages...\n");
    
    // Test all error codes have messages
    const char* success_msg = wifi_ap_manager_get_error_message(WIFI_AP_SUCCESS);
    test_assert(strlen(success_msg) > 0, "wifi_ap_manager_error_message_success", 
                "Success message should not be empty");
    
    const char* invalid_param_msg = wifi_ap_manager_get_error_message(WIFI_AP_ERROR_INVALID_PARAM);
    test_assert(strlen(invalid_param_msg) > 0, "wifi_ap_manager_error_message_invalid_param", 
                "Invalid param message should not be empty");
    
    const char* init_failed_msg = wifi_ap_manager_get_error_message(WIFI_AP_ERROR_INIT_FAILED);
    test_assert(strlen(init_failed_msg) > 0, "wifi_ap_manager_error_message_init_failed", 
                "Init failed message should not be empty");
    
    const char* unknown_msg = wifi_ap_manager_get_error_message(999);
    test_assert(strlen(unknown_msg) > 0, "wifi_ap_manager_error_message_unknown", 
                "Unknown error message should not be empty");
}

/**
 * @brief Test WiFi AP Manager edge cases
 */
static void test_wifi_ap_manager_edge_cases(void) {
    printf("\n🔧 Testing WiFi AP Manager Edge Cases...\n");
    
    // Test operations when not initialized
    wifi_ap_manager_deinit();
    
    int result = wifi_ap_manager_start(NULL);
    test_assert(result == WIFI_AP_ERROR_NOT_INITIALIZED, "wifi_ap_manager_start_not_initialized", 
                "Should fail when not initialized");
    
    result = wifi_ap_manager_stop();
    test_assert(result == WIFI_AP_ERROR_NOT_INITIALIZED, "wifi_ap_manager_stop_not_initialized", 
                "Should fail when not initialized");
    
    wifi_ap_status_t status;
    result = wifi_ap_manager_get_status(&status);
    test_assert(result == WIFI_AP_ERROR_NOT_INITIALIZED, "wifi_ap_manager_get_status_not_initialized", 
                "Should fail when not initialized");
    
    // Reinitialize for other tests
    wifi_ap_manager_init();
}

/**
 * @brief Test WiFi AP Manager performance
 */
static void test_wifi_ap_manager_performance(void) {
    printf("\n🔧 Testing WiFi AP Manager Performance...\n");
    
    // Test status query performance
    clock_t start = clock();
    wifi_ap_status_t status;
    for (int i = 0; i < 100; i++) {
        wifi_ap_manager_get_status(&status);
    }
    clock_t end = clock();
    double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(cpu_time < 1.0, "wifi_ap_manager_performance_status", 
                "Status queries should be fast (< 1 second for 100 calls)");
    
    // Test configuration query performance
    start = clock();
    wifi_ap_config_t config;
    for (int i = 0; i < 100; i++) {
        wifi_ap_manager_get_config(&config);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(cpu_time < 1.0, "wifi_ap_manager_performance_config", 
                "Config queries should be fast (< 1 second for 100 calls)");
    
    // Test statistics query performance
    start = clock();
    wifi_ap_statistics_t stats;
    for (int i = 0; i < 100; i++) {
        wifi_ap_manager_get_statistics(&stats);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(cpu_time < 1.0, "wifi_ap_manager_performance_statistics", 
                "Statistics queries should be fast (< 1 second for 100 calls)");
}

/**
 * @brief Main test function
 */
int main(void) {
    printf("🚀 Starting WiFi AP Manager Unit Tests...\n");
    printf("==========================================\n");
    
    // Run all tests
    test_wifi_ap_manager_init();
    test_wifi_ap_manager_deinit();
    test_wifi_ap_manager_start();
    test_wifi_ap_manager_stop();
    test_wifi_ap_manager_get_status();
    test_wifi_ap_manager_get_clients();
    test_wifi_ap_manager_kick_client();
    test_wifi_ap_manager_get_config();
    test_wifi_ap_manager_set_config();
    test_wifi_ap_manager_get_statistics();
    test_wifi_ap_manager_reset_statistics();
    test_wifi_ap_manager_validate_config();
    test_wifi_ap_manager_set_auto_fallback();
    test_wifi_ap_manager_trigger_fallback();
    test_wifi_ap_manager_get_fallback_status();
    test_wifi_ap_manager_error_messages();
    test_wifi_ap_manager_edge_cases();
    test_wifi_ap_manager_performance();
    
    // Print test results
    printf("\n==========================================\n");
    printf("📊 Test Results Summary:\n");
    printf("   Total Tests: %d\n", tests_run);
    printf("   ✅ Passed: %d\n", tests_passed);
    printf("   ❌ Failed: %d\n", tests_failed);
    printf("   📈 Success Rate: %.1f%%\n", (float)tests_passed / tests_run * 100.0f);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! WiFi AP Manager is working correctly.\n");
        return 0;
    } else {
        printf("\n⚠️  Some tests failed. Please check the implementation.\n");
        return 1;
    }
}
