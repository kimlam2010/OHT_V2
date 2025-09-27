/**
 * @file test_fallback_manager.c
 * @brief Fallback Manager Unit Tests - OHT-50 Robot Network Fallback Management Tests
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Unit tests cho Fallback Manager functionality
 * bao gồm auto-fallback, recovery, và monitoring testing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include "fallback_manager.h"
#include "wifi_manager.h"
#include "wifi_ap_manager.h"

// Test Configuration
#define TEST_WIFI_SSID "OHT-50-Test-WiFi"
#define TEST_WIFI_PASSWORD "test_wifi_password_2025"
#define TEST_AP_SSID "OHT-50-Test-AP"
#define TEST_AP_PASSWORD "test_ap_password_2025"

// Test Statistics
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Test Helper Functions
static void test_assert(bool condition, const char *test_name, const char *message);
static void test_fallback_manager_init(void);
static void test_fallback_manager_deinit(void);
static void test_fallback_manager_start_monitoring(void);
static void test_fallback_manager_stop_monitoring(void);
static void test_fallback_manager_enable_auto_fallback(void);
static void test_fallback_manager_check_connection(void);
static void test_fallback_manager_trigger_fallback(void);
static void test_fallback_manager_attempt_recovery(void);
static void test_fallback_manager_get_status(void);
static void test_fallback_manager_get_config(void);
static void test_fallback_manager_set_config(void);
static void test_fallback_manager_get_statistics(void);
static void test_fallback_manager_reset_statistics(void);
static void test_fallback_manager_validate_config(void);
static void test_fallback_manager_set_wifi_params(void);
static void test_fallback_manager_set_ap_params(void);
static void test_fallback_manager_get_connection_type(void);
static void test_fallback_manager_force_wifi(void);
static void test_fallback_manager_force_ap(void);
static void test_fallback_manager_get_connection_quality(void);
static void test_fallback_manager_handle_monitoring(void);
static void test_fallback_manager_error_messages(void);
static void test_fallback_manager_edge_cases(void);
static void test_fallback_manager_performance(void);

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
 * @brief Test Fallback Manager initialization
 */
static void test_fallback_manager_init(void) {
    printf("\n🔧 Testing Fallback Manager Initialization...\n");
    
    // Test successful initialization
    int result = fallback_manager_init();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_init", 
                "Should initialize successfully");
    
    // Test double initialization
    result = fallback_manager_init();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_init_double", 
                "Should handle double initialization gracefully");
    
    // Test manager is enabled
    bool is_enabled = fallback_manager_is_enabled();
    test_assert(is_enabled, "fallback_manager_is_enabled_init", 
                "Should be enabled after initialization");
    
    // Test monitoring is not active
    bool is_monitoring = fallback_manager_is_monitoring();
    test_assert(!is_monitoring, "fallback_manager_is_monitoring_init", 
                "Should not be monitoring after initialization");
}

/**
 * @brief Test Fallback Manager deinitialization
 */
static void test_fallback_manager_deinit(void) {
    printf("\n🔧 Testing Fallback Manager Deinitialization...\n");
    
    // Test successful deinitialization
    int result = fallback_manager_deinit();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_deinit", 
                "Should deinitialize successfully");
    
    // Test double deinitialization
    result = fallback_manager_deinit();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_deinit_double", 
                "Should handle double deinitialization gracefully");
    
    // Reinitialize for other tests
    fallback_manager_init();
}

/**
 * @brief Test Fallback Manager start monitoring
 */
static void test_fallback_manager_start_monitoring(void) {
    printf("\n🔧 Testing Fallback Manager Start Monitoring...\n");
    
    // Test start monitoring
    int result = fallback_manager_start_monitoring();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_start_monitoring", 
                "Should start monitoring successfully");
    
    // Test monitoring is active
    bool is_monitoring = fallback_manager_is_monitoring();
    test_assert(is_monitoring, "fallback_manager_is_monitoring_start", 
                "Should be monitoring after start");
    
    // Test start monitoring when already running
    result = fallback_manager_start_monitoring();
    test_assert(result == FALLBACK_ERROR_ALREADY_RUNNING, "fallback_manager_start_monitoring_running", 
                "Should fail when already running");
}

/**
 * @brief Test Fallback Manager stop monitoring
 */
static void test_fallback_manager_stop_monitoring(void) {
    printf("\n🔧 Testing Fallback Manager Stop Monitoring...\n");
    
    // Test stop monitoring when running
    int result = fallback_manager_stop_monitoring();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_stop_monitoring", 
                "Should stop monitoring successfully");
    
    // Test monitoring is not active
    bool is_monitoring = fallback_manager_is_monitoring();
    test_assert(!is_monitoring, "fallback_manager_is_monitoring_stop", 
                "Should not be monitoring after stop");
    
    // Test stop monitoring when not running
    result = fallback_manager_stop_monitoring();
    test_assert(result == FALLBACK_ERROR_NOT_RUNNING, "fallback_manager_stop_monitoring_not_running", 
                "Should fail when not running");
}

/**
 * @brief Test Fallback Manager enable auto-fallback
 */
static void test_fallback_manager_enable_auto_fallback(void) {
    printf("\n🔧 Testing Fallback Manager Enable Auto-Fallback...\n");
    
    // Test enable auto-fallback
    int result = fallback_manager_enable_auto_fallback(true);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_enable_auto_fallback", 
                "Should enable auto-fallback successfully");
    
    // Test disable auto-fallback
    result = fallback_manager_enable_auto_fallback(false);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_disable_auto_fallback", 
                "Should disable auto-fallback successfully");
    
    // Test manager is disabled
    bool is_enabled = fallback_manager_is_enabled();
    test_assert(!is_enabled, "fallback_manager_is_enabled_disabled", 
                "Should be disabled after disable");
    
    // Re-enable for other tests
    fallback_manager_enable_auto_fallback(true);
}

/**
 * @brief Test Fallback Manager check connection
 */
static void test_fallback_manager_check_connection(void) {
    printf("\n🔧 Testing Fallback Manager Check Connection...\n");
    
    // Test check connection
    int result = fallback_manager_check_connection();
    // Note: This may fail if WiFi is not connected, which is expected in test environment
    test_assert(result == FALLBACK_SUCCESS || result == FALLBACK_ERROR_WIFI_MANAGER_FAILED, 
                "fallback_manager_check_connection", 
                "Should check connection (may fail if WiFi not connected)");
}

/**
 * @brief Test Fallback Manager trigger fallback
 */
static void test_fallback_manager_trigger_fallback(void) {
    printf("\n🔧 Testing Fallback Manager Trigger Fallback...\n");
    
    // Test trigger fallback
    int result = fallback_manager_trigger_fallback();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_trigger_fallback", 
                "Should trigger fallback successfully");
}

/**
 * @brief Test Fallback Manager attempt recovery
 */
static void test_fallback_manager_attempt_recovery(void) {
    printf("\n🔧 Testing Fallback Manager Attempt Recovery...\n");
    
    // Test attempt recovery
    int result = fallback_manager_attempt_recovery();
    // Note: This may fail if WiFi is not available, which is expected in test environment
    test_assert(result == FALLBACK_SUCCESS || result == FALLBACK_ERROR_WIFI_MANAGER_FAILED, 
                "fallback_manager_attempt_recovery", 
                "Should attempt recovery (may fail if WiFi not available)");
}

/**
 * @brief Test Fallback Manager get status
 */
static void test_fallback_manager_get_status(void) {
    printf("\n🔧 Testing Fallback Manager Get Status...\n");
    
    // Test get status
    fallback_status_info_t status;
    int result = fallback_manager_get_status(&status);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_get_status", 
                "Should get status successfully");
    
    test_assert(status.auto_fallback_enabled == true, "fallback_manager_status_auto_fallback", 
                "Auto-fallback should be enabled");
    
    test_assert(status.status == FALLBACK_STATUS_DISABLED || 
                status.status == FALLBACK_STATUS_MONITORING || 
                status.status == FALLBACK_STATUS_TRIGGERED, 
                "fallback_manager_status_valid", 
                "Status should be valid");
    
    // Test get status with NULL parameter
    result = fallback_manager_get_status(NULL);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_get_status_null", 
                "Should fail with NULL parameter");
}

/**
 * @brief Test Fallback Manager get configuration
 */
static void test_fallback_manager_get_config(void) {
    printf("\n🔧 Testing Fallback Manager Get Configuration...\n");
    
    // Test get configuration
    fallback_config_t config;
    int result = fallback_manager_get_config(&config);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_get_config", 
                "Should get configuration successfully");
    
    test_assert(config.auto_fallback_enabled == true, "fallback_manager_config_auto_fallback", 
                "Auto-fallback should be enabled in config");
    
    test_assert(config.connection_timeout_ms > 0, "fallback_manager_config_timeout", 
                "Connection timeout should be positive");
    
    test_assert(config.max_retry_attempts > 0, "fallback_manager_config_max_retry", 
                "Max retry attempts should be positive");
    
    // Test get configuration with NULL parameter
    result = fallback_manager_get_config(NULL);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_get_config_null", 
                "Should fail with NULL parameter");
}

/**
 * @brief Test Fallback Manager set configuration
 */
static void test_fallback_manager_set_config(void) {
    printf("\n🔧 Testing Fallback Manager Set Configuration...\n");
    
    // Test set valid configuration
    fallback_config_t config = {
        .auto_fallback_enabled = true,
        .connection_timeout_ms = 30000,
        .retry_interval_ms = 5000,
        .max_retry_attempts = 3,
        .ap_mode_enabled = true,
        .ap_startup_timeout_ms = 10000,
        .recovery_check_interval_ms = 10000,
        .signal_strength_threshold_dbm = -70,
        .aggressive_fallback = false
    };
    
    int result = fallback_manager_set_config(&config);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_set_config", 
                "Should set configuration successfully");
    
    // Verify configuration was set
    fallback_config_t retrieved_config;
    result = fallback_manager_get_config(&retrieved_config);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_get_config_after_set", 
                "Should get configuration after set");
    
    test_assert(retrieved_config.connection_timeout_ms == 30000, "fallback_manager_config_timeout_set", 
                "Connection timeout should be set correctly");
    
    test_assert(retrieved_config.max_retry_attempts == 3, "fallback_manager_config_max_retry_set", 
                "Max retry attempts should be set correctly");
    
    test_assert(retrieved_config.signal_strength_threshold_dbm == -70, "fallback_manager_config_threshold_set", 
                "Signal strength threshold should be set correctly");
    
    // Test set configuration with NULL parameter
    result = fallback_manager_set_config(NULL);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_set_config_null", 
                "Should fail with NULL parameter");
}

/**
 * @brief Test Fallback Manager get statistics
 */
static void test_fallback_manager_get_statistics(void) {
    printf("\n🔧 Testing Fallback Manager Get Statistics...\n");
    
    // Test get statistics
    fallback_statistics_t stats;
    int result = fallback_manager_get_statistics(&stats);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_get_statistics", 
                "Should get statistics successfully");
    
    // Test get statistics with NULL parameter
    result = fallback_manager_get_statistics(NULL);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_get_statistics_null", 
                "Should fail with NULL parameter");
}

/**
 * @brief Test Fallback Manager reset statistics
 */
static void test_fallback_manager_reset_statistics(void) {
    printf("\n🔧 Testing Fallback Manager Reset Statistics...\n");
    
    // Test reset statistics
    int result = fallback_manager_reset_statistics();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_reset_statistics", 
                "Should reset statistics successfully");
}

/**
 * @brief Test Fallback Manager configuration validation
 */
static void test_fallback_manager_validate_config(void) {
    printf("\n🔧 Testing Fallback Manager Configuration Validation...\n");
    
    // Test valid configuration
    fallback_config_t valid_config = {
        .auto_fallback_enabled = true,
        .connection_timeout_ms = 30000,
        .retry_interval_ms = 5000,
        .max_retry_attempts = 3,
        .ap_mode_enabled = true,
        .ap_startup_timeout_ms = 10000,
        .recovery_check_interval_ms = 10000,
        .signal_strength_threshold_dbm = -70,
        .aggressive_fallback = false
    };
    
    int result = fallback_manager_validate_config(&valid_config);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_validate_config_valid", 
                "Should validate valid configuration");
    
    // Test invalid timeout
    fallback_config_t invalid_config = valid_config;
    invalid_config.connection_timeout_ms = 500; // Too short
    result = fallback_manager_validate_config(&invalid_config);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_validate_config_invalid_timeout", 
                "Should fail with invalid timeout");
    
    // Test invalid max retry attempts
    invalid_config = valid_config;
    invalid_config.max_retry_attempts = 0; // Too low
    result = fallback_manager_validate_config(&invalid_config);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_validate_config_invalid_retry", 
                "Should fail with invalid max retry attempts");
    
    // Test invalid signal strength threshold
    invalid_config = valid_config;
    invalid_config.signal_strength_threshold_dbm = -20; // Too high
    result = fallback_manager_validate_config(&invalid_config);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_validate_config_invalid_threshold", 
                "Should fail with invalid signal strength threshold");
    
    // Test NULL configuration
    result = fallback_manager_validate_config(NULL);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_validate_config_null", 
                "Should fail with NULL configuration");
}

/**
 * @brief Test Fallback Manager set WiFi parameters
 */
static void test_fallback_manager_set_wifi_params(void) {
    printf("\n🔧 Testing Fallback Manager Set WiFi Parameters...\n");
    
    // Test set WiFi parameters
    int result = fallback_manager_set_wifi_params(TEST_WIFI_SSID, TEST_WIFI_PASSWORD);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_set_wifi_params", 
                "Should set WiFi parameters successfully");
    
    // Test set WiFi parameters with NULL SSID
    result = fallback_manager_set_wifi_params(NULL, TEST_WIFI_PASSWORD);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_set_wifi_params_null_ssid", 
                "Should fail with NULL SSID");
    
    // Test set WiFi parameters with NULL password
    result = fallback_manager_set_wifi_params(TEST_WIFI_SSID, NULL);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_set_wifi_params_null_password", 
                "Should fail with NULL password");
}

/**
 * @brief Test Fallback Manager set AP parameters
 */
static void test_fallback_manager_set_ap_params(void) {
    printf("\n🔧 Testing Fallback Manager Set AP Parameters...\n");
    
    // Test set AP parameters
    int result = fallback_manager_set_ap_params(TEST_AP_SSID, TEST_AP_PASSWORD);
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_set_ap_params", 
                "Should set AP parameters successfully");
    
    // Test set AP parameters with NULL SSID
    result = fallback_manager_set_ap_params(NULL, TEST_AP_PASSWORD);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_set_ap_params_null_ssid", 
                "Should fail with NULL SSID");
    
    // Test set AP parameters with NULL password
    result = fallback_manager_set_ap_params(TEST_AP_SSID, NULL);
    test_assert(result == FALLBACK_ERROR_INVALID_PARAM, "fallback_manager_set_ap_params_null_password", 
                "Should fail with NULL password");
}

/**
 * @brief Test Fallback Manager get connection type
 */
static void test_fallback_manager_get_connection_type(void) {
    printf("\n🔧 Testing Fallback Manager Get Connection Type...\n");
    
    // Test get connection type
    const char* connection_type = fallback_manager_get_connection_type();
    test_assert(connection_type != NULL, "fallback_manager_get_connection_type", 
                "Should get connection type successfully");
    
    test_assert(strlen(connection_type) > 0, "fallback_manager_connection_type_not_empty", 
                "Connection type should not be empty");
}

/**
 * @brief Test Fallback Manager force WiFi
 */
static void test_fallback_manager_force_wifi(void) {
    printf("\n🔧 Testing Fallback Manager Force WiFi...\n");
    
    // Test force WiFi
    int result = fallback_manager_force_wifi();
    // Note: This may fail if WiFi is not available, which is expected in test environment
    test_assert(result == FALLBACK_SUCCESS || result == FALLBACK_ERROR_WIFI_MANAGER_FAILED, 
                "fallback_manager_force_wifi", 
                "Should force WiFi (may fail if WiFi not available)");
}

/**
 * @brief Test Fallback Manager force AP
 */
static void test_fallback_manager_force_ap(void) {
    printf("\n🔧 Testing Fallback Manager Force AP...\n");
    
    // Test force AP
    int result = fallback_manager_force_ap();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_force_ap", 
                "Should force AP successfully");
}

/**
 * @brief Test Fallback Manager get connection quality
 */
static void test_fallback_manager_get_connection_quality(void) {
    printf("\n🔧 Testing Fallback Manager Get Connection Quality...\n");
    
    // Test get connection quality
    int quality = fallback_manager_get_connection_quality();
    test_assert(quality >= 0 && quality <= 100, "fallback_manager_get_connection_quality", 
                "Should get connection quality (0-100)");
}

/**
 * @brief Test Fallback Manager handle monitoring
 */
static void test_fallback_manager_handle_monitoring(void) {
    printf("\n🔧 Testing Fallback Manager Handle Monitoring...\n");
    
    // Test handle monitoring when not monitoring
    int result = fallback_manager_handle_monitoring();
    test_assert(result == FALLBACK_ERROR_NOT_RUNNING, "fallback_manager_handle_monitoring_not_running", 
                "Should fail when not monitoring");
    
    // Start monitoring
    fallback_manager_start_monitoring();
    
    // Test handle monitoring when monitoring
    result = fallback_manager_handle_monitoring();
    test_assert(result == FALLBACK_SUCCESS, "fallback_manager_handle_monitoring_running", 
                "Should handle monitoring when running");
}

/**
 * @brief Test Fallback Manager error messages
 */
static void test_fallback_manager_error_messages(void) {
    printf("\n🔧 Testing Fallback Manager Error Messages...\n");
    
    // Test all error codes have messages
    const char* success_msg = fallback_manager_get_error_message(FALLBACK_SUCCESS);
    test_assert(strlen(success_msg) > 0, "fallback_manager_error_message_success", 
                "Success message should not be empty");
    
    const char* invalid_param_msg = fallback_manager_get_error_message(FALLBACK_ERROR_INVALID_PARAM);
    test_assert(strlen(invalid_param_msg) > 0, "fallback_manager_error_message_invalid_param", 
                "Invalid param message should not be empty");
    
    const char* init_failed_msg = fallback_manager_get_error_message(FALLBACK_ERROR_INIT_FAILED);
    test_assert(strlen(init_failed_msg) > 0, "fallback_manager_error_message_init_failed", 
                "Init failed message should not be empty");
    
    const char* unknown_msg = fallback_manager_get_error_message(999);
    test_assert(strlen(unknown_msg) > 0, "fallback_manager_error_message_unknown", 
                "Unknown error message should not be empty");
}

/**
 * @brief Test Fallback Manager edge cases
 */
static void test_fallback_manager_edge_cases(void) {
    printf("\n🔧 Testing Fallback Manager Edge Cases...\n");
    
    // Test operations when not initialized
    fallback_manager_deinit();
    
    int result = fallback_manager_start_monitoring();
    test_assert(result == FALLBACK_ERROR_NOT_INITIALIZED, "fallback_manager_start_monitoring_not_initialized", 
                "Should fail when not initialized");
    
    result = fallback_manager_stop_monitoring();
    test_assert(result == FALLBACK_ERROR_NOT_INITIALIZED, "fallback_manager_stop_monitoring_not_initialized", 
                "Should fail when not initialized");
    
    fallback_status_info_t status;
    result = fallback_manager_get_status(&status);
    test_assert(result == FALLBACK_ERROR_NOT_INITIALIZED, "fallback_manager_get_status_not_initialized", 
                "Should fail when not initialized");
    
    // Reinitialize for other tests
    fallback_manager_init();
}

/**
 * @brief Test Fallback Manager performance
 */
static void test_fallback_manager_performance(void) {
    printf("\n🔧 Testing Fallback Manager Performance...\n");
    
    // Test status query performance
    clock_t start = clock();
    fallback_status_info_t status;
    for (int i = 0; i < 100; i++) {
        fallback_manager_get_status(&status);
    }
    clock_t end = clock();
    double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(cpu_time < 1.0, "fallback_manager_performance_status", 
                "Status queries should be fast (< 1 second for 100 calls)");
    
    // Test configuration query performance
    start = clock();
    fallback_config_t config;
    for (int i = 0; i < 100; i++) {
        fallback_manager_get_config(&config);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(cpu_time < 1.0, "fallback_manager_performance_config", 
                "Config queries should be fast (< 1 second for 100 calls)");
    
    // Test statistics query performance
    start = clock();
    fallback_statistics_t stats;
    for (int i = 0; i < 100; i++) {
        fallback_manager_get_statistics(&stats);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    test_assert(cpu_time < 1.0, "fallback_manager_performance_statistics", 
                "Statistics queries should be fast (< 1 second for 100 calls)");
}

/**
 * @brief Main test function
 */
int main(void) {
    printf("🚀 Starting Fallback Manager Unit Tests...\n");
    printf("==========================================\n");
    
    // Run all tests
    test_fallback_manager_init();
    test_fallback_manager_deinit();
    test_fallback_manager_start_monitoring();
    test_fallback_manager_stop_monitoring();
    test_fallback_manager_enable_auto_fallback();
    test_fallback_manager_check_connection();
    test_fallback_manager_trigger_fallback();
    test_fallback_manager_attempt_recovery();
    test_fallback_manager_get_status();
    test_fallback_manager_get_config();
    test_fallback_manager_set_config();
    test_fallback_manager_get_statistics();
    test_fallback_manager_reset_statistics();
    test_fallback_manager_validate_config();
    test_fallback_manager_set_wifi_params();
    test_fallback_manager_set_ap_params();
    test_fallback_manager_get_connection_type();
    test_fallback_manager_force_wifi();
    test_fallback_manager_force_ap();
    test_fallback_manager_get_connection_quality();
    test_fallback_manager_handle_monitoring();
    test_fallback_manager_error_messages();
    test_fallback_manager_edge_cases();
    test_fallback_manager_performance();
    
    // Print test results
    printf("\n==========================================\n");
    printf("📊 Test Results Summary:\n");
    printf("   Total Tests: %d\n", tests_run);
    printf("   ✅ Passed: %d\n", tests_passed);
    printf("   ❌ Failed: %d\n", tests_failed);
    printf("   📈 Success Rate: %.1f%%\n", (float)tests_passed / tests_run * 100.0f);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! Fallback Manager is working correctly.\n");
        return 0;
    } else {
        printf("\n⚠️  Some tests failed. Please check the implementation.\n");
        return 1;
    }
}
