/**
 * @file test_ota_update.c
 * @brief Test suite for OTA Update HAL
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-06 (OTA Update System)
 */

#include "hal_ota_update.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test configuration
static const ota_update_config_t test_config = {
    .update_server_url = "https://test.ota.oht50.local/firmware",
    .current_version = "1.0.0",
    .check_interval_ms = 60000,  // 1 minute for testing
    .download_timeout_ms = 30000,  // 30 seconds
    .verification_timeout_ms = 10000,  // 10 seconds
    .installation_timeout_ms = 20000,  // 20 seconds
    .auto_check_enabled = true,
    .auto_download_enabled = false,
    .auto_install_enabled = false,
    .rollback_enabled = true,
    .max_rollback_attempts = 3,
    .verify_checksum = true,
    .verify_signature = false,
    .certificate_path = "/etc/ssl/certs/test_ca.pem"
};

// Test event callback
static void test_ota_event_callback(ota_update_event_t event, const void *data) {
    printf("[TEST] OTA Event: %s\n", hal_ota_update_get_event_name(event));
    if (data != NULL) {
        if (event == OTA_UPDATE_EVENT_DOWNLOAD_PROGRESS || 
            event == OTA_UPDATE_EVENT_INSTALLATION_PROGRESS) {
            printf("[TEST] Progress: %u%%\n", *(uint32_t*)data);
        }
    }
}

// Test functions
static bool test_ota_initialization(void) {
    printf("\n=== Testing OTA Initialization ===\n");
    
    // Test 1: Initialize with NULL config (should use defaults)
    hal_status_t status = hal_ota_update_init(NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: hal_ota_update_init(NULL) returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_init(NULL)\n");
    
    // Test 2: Try to initialize again (should return ALREADY_INITIALIZED)
    status = hal_ota_update_init(NULL);
    if (status != HAL_STATUS_ALREADY_INITIALIZED) {
        printf("❌ Test 2 failed: hal_ota_update_init(NULL) returned %d, expected ALREADY_INITIALIZED\n", status);
        return false;
    }
    printf("✅ Test 2 passed: hal_ota_update_init(NULL) - already initialized\n");
    
    // Test 3: Deinitialize
    status = hal_ota_update_deinit();
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: hal_ota_update_deinit() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: hal_ota_update_deinit()\n");
    
    // Test 4: Initialize with custom config
    status = hal_ota_update_init(&test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: hal_ota_update_init(&test_config) returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: hal_ota_update_init(&test_config)\n");
    
    return true;
}

static bool test_ota_configuration(void) {
    printf("\n=== Testing OTA Configuration ===\n");
    
    // Test 1: Get configuration
    ota_update_config_t config;
    hal_status_t status = hal_ota_update_get_config(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: hal_ota_update_get_config() returned %d\n", status);
        return false;
    }
    
    if (strcmp(config.update_server_url, test_config.update_server_url) != 0) {
        printf("❌ Test 1 failed: URL mismatch\n");
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_get_config()\n");
    
    // Test 2: Set configuration
    ota_update_config_t new_config = test_config;
    strcpy(new_config.update_server_url, "https://new.ota.oht50.local/firmware");
    new_config.auto_check_enabled = false;
    
    status = hal_ota_update_set_config(&new_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_set_config() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: hal_ota_update_set_config()\n");
    
    // Test 3: Verify configuration was updated
    status = hal_ota_update_get_config(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: hal_ota_update_get_config() returned %d\n", status);
        return false;
    }
    
    if (strcmp(config.update_server_url, new_config.update_server_url) != 0) {
        printf("❌ Test 3 failed: URL not updated\n");
        return false;
    }
    if (config.auto_check_enabled != new_config.auto_check_enabled) {
        printf("❌ Test 3 failed: auto_check_enabled not updated\n");
        return false;
    }
    printf("✅ Test 3 passed: Configuration updated successfully\n");
    
    return true;
}

static bool test_ota_status_and_statistics(void) {
    printf("\n=== Testing OTA Status and Statistics ===\n");
    
    // Test 1: Get status
    ota_update_status_t status_info;
    hal_status_t status = hal_ota_update_get_status(&status_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: hal_ota_update_get_status() returned %d\n", status);
        return false;
    }
    
    if (status_info.state != OTA_UPDATE_STATE_IDLE) {
        printf("❌ Test 1 failed: Expected IDLE state, got %s\n", 
               hal_ota_update_get_state_name(status_info.state));
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_get_status()\n");
    
    // Test 2: Get statistics
    ota_update_stats_t stats;
    status = hal_ota_update_get_statistics(&stats);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_get_statistics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: hal_ota_update_get_statistics()\n");
    
    // Test 3: Reset statistics
    status = hal_ota_update_reset_statistics();
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: hal_ota_update_reset_statistics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: hal_ota_update_reset_statistics()\n");
    
    return true;
}

static bool test_ota_update_checking(void) {
    printf("\n=== Testing OTA Update Checking ===\n");
    
    // Test 1: Check for updates
    ota_update_firmware_info_t firmware_info;
    hal_status_t status = hal_ota_update_check_for_updates(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: hal_ota_update_check_for_updates() returned %d\n", status);
        return false;
    }
    
    if (strlen(firmware_info.version) == 0) {
        printf("❌ Test 1 failed: No firmware version returned\n");
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_check_for_updates() - found version %s\n", firmware_info.version);
    
    // Test 2: Get current firmware info
    ota_update_firmware_info_t current_firmware;
    status = hal_ota_update_get_current_firmware_info(&current_firmware);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_get_current_firmware_info() returned %d\n", status);
        return false;
    }
    
    if (strlen(current_firmware.version) == 0) {
        printf("❌ Test 2 failed: No current firmware version\n");
        return false;
    }
    printf("✅ Test 2 passed: hal_ota_update_get_current_firmware_info() - version %s\n", current_firmware.version);
    
    return true;
}

static bool test_ota_download_and_verification(void) {
    printf("\n=== Testing OTA Download and Verification ===\n");
    
    // Test 1: Download firmware
    ota_update_firmware_info_t firmware_info;
    hal_status_t status = hal_ota_update_check_for_updates(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not get firmware info\n");
        return false;
    }
    
    status = hal_ota_update_download_firmware(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: hal_ota_update_download_firmware() returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_download_firmware()\n");
    
    // Test 2: Verify firmware
    status = hal_ota_update_verify_firmware(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_verify_firmware() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: hal_ota_update_verify_firmware()\n");
    
    return true;
}

static bool test_ota_installation_and_rollback(void) {
    printf("\n=== Testing OTA Installation and Rollback ===\n");
    
    // Test 1: Install firmware
    ota_update_firmware_info_t firmware_info;
    hal_status_t status = hal_ota_update_check_for_updates(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not get firmware info\n");
        return false;
    }
    
    status = hal_ota_update_install_firmware(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: hal_ota_update_install_firmware() returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_install_firmware()\n");
    
    // Test 2: Get backup firmware info
    ota_update_firmware_info_t backup_firmware[OTA_UPDATE_BACKUP_SLOTS];
    uint32_t actual_count;
    status = hal_ota_update_get_backup_firmware_info(backup_firmware, OTA_UPDATE_BACKUP_SLOTS, &actual_count);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_get_backup_firmware_info() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: hal_ota_update_get_backup_firmware_info() - %u backups\n", actual_count);
    
    // Test 3: Rollback firmware (if backup available)
    if (actual_count > 0) {
        status = hal_ota_update_rollback_firmware();
        if (status != HAL_STATUS_OK) {
            printf("❌ Test 3 failed: hal_ota_update_rollback_firmware() returned %d\n", status);
            return false;
        }
        printf("✅ Test 3 passed: hal_ota_update_rollback_firmware()\n");
    } else {
        printf("⚠️  Test 3 skipped: No backup firmware available\n");
    }
    
    return true;
}

static bool test_ota_operations(void) {
    printf("\n=== Testing OTA Operations ===\n");
    
    // Test 1: Cancel operation
    hal_status_t status = hal_ota_update_cancel_operation();
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: hal_ota_update_cancel_operation() returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_cancel_operation()\n");
    
    // Test 2: Self-test
    status = hal_ota_update_self_test();
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_self_test() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: hal_ota_update_self_test()\n");
    
    // Test 3: Get diagnostics
    char diagnostics[1024];
    status = hal_ota_update_get_diagnostics(diagnostics, sizeof(diagnostics));
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: hal_ota_update_get_diagnostics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: hal_ota_update_get_diagnostics()\n");
    printf("Diagnostics:\n%s\n", diagnostics);
    
    return true;
}

static bool test_ota_update_cycle(void) {
    printf("\n=== Testing OTA Update Cycle ===\n");
    
    // Test 1: Set event callback
    hal_status_t status = hal_ota_update_set_callback(test_ota_event_callback);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: hal_ota_update_set_callback() returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_set_callback()\n");
    
    // Test 2: Simulate update cycle
    ota_update_firmware_info_t firmware_info;
    status = hal_ota_update_check_for_updates(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_check_for_updates() returned %d\n", status);
        return false;
    }
    
    status = hal_ota_update_download_firmware(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_download_firmware() returned %d\n", status);
        return false;
    }
    
    status = hal_ota_update_verify_firmware(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_verify_firmware() returned %d\n", status);
        return false;
    }
    
    status = hal_ota_update_install_firmware(&firmware_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: hal_ota_update_install_firmware() returned %d\n", status);
        return false;
    }
    
    printf("✅ Test 2 passed: Complete update cycle initiated\n");
    
    // Test 3: Update cycle (simulate progress)
    for (int i = 0; i < 10; i++) {
        status = hal_ota_update_update();
        if (status != HAL_STATUS_OK) {
            printf("❌ Test 3 failed: hal_ota_update_update() returned %d\n", status);
            return false;
        }
        usleep(100000);  // 100ms delay
    }
    printf("✅ Test 3 passed: hal_ota_update_update() cycle\n");
    
    return true;
}

static bool test_ota_error_handling(void) {
    printf("\n=== Testing OTA Error Handling ===\n");
    
    // Test 1: Test with invalid parameters
    hal_status_t status = hal_ota_update_get_status(NULL);
    if (status != HAL_STATUS_INVALID_PARAMETER) {
        printf("❌ Test 1 failed: hal_ota_update_get_status(NULL) returned %d, expected INVALID_PARAMETER\n", status);
        return false;
    }
    printf("✅ Test 1 passed: hal_ota_update_get_status(NULL) - invalid parameter\n");
    
    // Test 2: Test without initialization
    hal_ota_update_deinit();
    status = hal_ota_update_get_status(NULL);
    if (status != HAL_STATUS_NOT_INITIALIZED) {
        printf("❌ Test 2 failed: hal_ota_update_get_status() returned %d, expected NOT_INITIALIZED\n", status);
        return false;
    }
    printf("✅ Test 2 passed: hal_ota_update_get_status() - not initialized\n");
    
    // Test 3: Re-initialize for remaining tests
    status = hal_ota_update_init(&test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: hal_ota_update_init() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: hal_ota_update_init() - re-initialized\n");
    
    return true;
}

int main(int argc, char *argv) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;  // Suppress unused parameter warning
    
    printf("=== OTA Update HAL Test Suite ===\n");
    printf("Version: 1.0.0\n");
    printf("Date: 2025-01-27\n");
    printf("Team: FW\n");
    printf("Task: FW-06 (OTA Update System)\n\n");
    
    bool all_tests_passed = true;
    
    // Run test suites
    if (!test_ota_initialization()) {
        all_tests_passed = false;
    }
    
    if (!test_ota_configuration()) {
        all_tests_passed = false;
    }
    
    if (!test_ota_status_and_statistics()) {
        all_tests_passed = false;
    }
    
    if (!test_ota_update_checking()) {
        all_tests_passed = false;
    }
    
    if (!test_ota_download_and_verification()) {
        all_tests_passed = false;
    }
    
    if (!test_ota_installation_and_rollback()) {
        all_tests_passed = false;
    }
    
    if (!test_ota_operations()) {
        all_tests_passed = false;
    }
    
    if (!test_ota_update_cycle()) {
        all_tests_passed = false;
    }
    
    if (!test_ota_error_handling()) {
        all_tests_passed = false;
    }
    
    // Cleanup
    hal_ota_update_deinit();
    
    // Summary
    printf("\n=== Test Summary ===\n");
    if (all_tests_passed) {
        printf("✅ All tests passed!\n");
        printf("OTA Update HAL is working correctly.\n");
        return 0;
    } else {
        printf("❌ Some tests failed!\n");
        printf("Please review the failed tests above.\n");
        return 1;
    }
}
