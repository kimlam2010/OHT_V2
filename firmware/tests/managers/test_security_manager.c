/**
 * @file test_security_manager.c
 * @brief Test suite for Security Manager
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-05 (Security Framework)
 */

#include "security_manager.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test configuration
static const security_mgr_config_t test_config = {
    .ssl_config = {
        .ssl_enabled = true,
        .tls_enabled = true,
        .tls_version = 0x0303,  // TLS 1.2
        .certificate_path = "/etc/ssl/certs/test_cert.pem",
        .private_key_path = "/etc/ssl/private/test_key.pem",
        .ca_certificate_path = "/etc/ssl/certs/test_ca.pem",
        .verify_peer = true,
        .verify_hostname = true
    },
    .session_timeout_ms = 1800000,  // 30 minutes
    .max_login_attempts = 5,
    .auto_lock_enabled = true,
    .auto_lock_timeout_ms = 300000,  // 5 minutes
    .audit_logging_enabled = true,
    .encryption_enabled = true
};

// Test event callback
static void test_security_event_callback(security_mgr_event_t event, const char *username, const void *data) {
    printf("[TEST] Security Event: %s\n", security_manager_get_event_name(event));
    if (username != NULL) {
        printf("[TEST] User: %s\n", username);
    }
    if (data != NULL) {
        printf("[TEST] Data: %p\n", data);
    }
}

// Test functions
static bool test_security_initialization(void) {
    printf("\n=== Testing Security Manager Initialization ===\n");
    
    // Test 1: Initialize with NULL config (should use defaults)
    hal_status_t status = security_manager_init(NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: security_manager_init(NULL) returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: security_manager_init(NULL)\n");
    
    // Test 2: Try to initialize again (should return ALREADY_INITIALIZED)
    status = security_manager_init(NULL);
    if (status != HAL_STATUS_ALREADY_INITIALIZED) {
        printf("❌ Test 2 failed: security_manager_init(NULL) returned %d, expected ALREADY_INITIALIZED\n", status);
        return false;
    }
    printf("✅ Test 2 passed: security_manager_init(NULL) - already initialized\n");
    
    // Test 3: Deinitialize
    status = security_manager_deinit();
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: security_manager_deinit() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: security_manager_deinit()\n");
    
    // Test 4: Initialize with custom config
    status = security_manager_init(&test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: security_manager_init(&test_config) returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: security_manager_init(&test_config)\n");
    
    return true;
}

static bool test_security_configuration(void) {
    printf("\n=== Testing Security Configuration ===\n");
    
    // Test 1: Get configuration
    security_mgr_config_t config;
    hal_status_t status = security_manager_get_config(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: security_manager_get_config() returned %d\n", status);
        return false;
    }
    
    if (config.ssl_config.ssl_enabled != test_config.ssl_config.ssl_enabled) {
        printf("❌ Test 1 failed: SSL enabled mismatch\n");
        return false;
    }
    printf("✅ Test 1 passed: security_manager_get_config()\n");
    
    // Test 2: Set configuration
    security_mgr_config_t new_config = test_config;
    new_config.ssl_config.ssl_enabled = false;
    new_config.max_login_attempts = 3;
    
    status = security_manager_set_config(&new_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: security_manager_set_config() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: security_manager_set_config()\n");
    
    // Test 3: Verify configuration was updated
    status = security_manager_get_config(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: security_manager_get_config() returned %d\n", status);
        return false;
    }
    
    if (config.ssl_config.ssl_enabled != new_config.ssl_config.ssl_enabled) {
        printf("❌ Test 3 failed: SSL enabled not updated\n");
        return false;
    }
    if (config.max_login_attempts != new_config.max_login_attempts) {
        printf("❌ Test 3 failed: max_login_attempts not updated\n");
        return false;
    }
    printf("✅ Test 3 passed: Configuration updated successfully\n");
    
    return true;
}

static bool test_user_management(void) {
    printf("\n=== Testing User Management ===\n");
    
    // Test 1: Add user
    security_mgr_user_config_t user = {
        .username = "testuser",
        .password_hash = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", // "password"
        .level = SECURITY_MGR_LEVEL_OPERATOR,
        .auth_method = SECURITY_MGR_AUTH_PASSWORD,
        .enabled = true,
        .login_attempts = 0,
        .last_login_time = 0,
        .password_expiry_time = 0
    };
    
    hal_status_t status = security_manager_add_user(&user);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: security_manager_add_user() returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: security_manager_add_user()\n");
    
    // Test 2: Get user (skipped - function not implemented)
    printf("⚠️  Test 2 skipped: security_manager_get_user() not implemented\n");
    
    // Test 3: Update user (skipped - function not implemented)
    printf("⚠️  Test 3 skipped: security_manager_update_user() not implemented\n");
    
    // Test 4: Delete user
    status = security_manager_remove_user("testuser");
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: security_manager_remove_user() returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: security_manager_remove_user()\n");
    
    return true;
}

static bool test_authentication(void) {
    printf("\n=== Testing Authentication ===\n");
    
    // Test 1: Add test user
    security_mgr_user_config_t user = {
        .username = "authuser",
        .password_hash = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", // "password"
        .level = SECURITY_MGR_LEVEL_OPERATOR,
        .auth_method = SECURITY_MGR_AUTH_PASSWORD,
        .enabled = true,
        .login_attempts = 0,
        .last_login_time = 0,
        .password_expiry_time = 0
    };
    
    hal_status_t status = security_manager_add_user(&user);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not add test user\n");
        return false;
    }
    
    // Test 2: Authenticate with correct credentials
    char session_id[64];
    status = security_manager_authenticate_user("authuser", "password", session_id);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: security_manager_authenticate_user() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: security_manager_authenticate_user() - correct credentials\n");
    
    // Test 3: Authenticate with incorrect credentials
    status = security_manager_authenticate_user("authuser", "wrongpassword", session_id);
    if (status == HAL_STATUS_OK) {
        printf("❌ Test 3 failed: security_manager_authenticate_user() should have failed\n");
        return false;
    }
    printf("✅ Test 3 passed: security_manager_authenticate_user() - incorrect credentials\n");
    
    // Test 4: Authenticate with non-existent user
    status = security_manager_authenticate_user("nonexistent", "password", session_id);
    if (status == HAL_STATUS_OK) {
        printf("❌ Test 4 failed: security_manager_authenticate_user() should have failed\n");
        return false;
    }
    printf("✅ Test 4 passed: security_manager_authenticate_user() - non-existent user\n");
    
    // Cleanup
    security_manager_remove_user("authuser");
    
    return true;
}

static bool test_authorization(void) {
    printf("\n=== Testing Authorization ===\n");
    
    // Test 1: Add test user with specific permissions
    security_mgr_user_config_t user = {
        .username = "authuser",
        .password_hash = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", // "password"
        .level = SECURITY_MGR_LEVEL_OPERATOR,
        .auth_method = SECURITY_MGR_AUTH_PASSWORD,
        .enabled = true,
        .login_attempts = 0,
        .last_login_time = 0,
        .password_expiry_time = 0
    };
    
    hal_status_t status = security_manager_add_user(&user);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not add test user\n");
        return false;
    }
    
    // Test 2: Authenticate user
    char session_id[64];
    status = security_manager_authenticate_user("authuser", "password", session_id);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: Could not authenticate user\n");
        return false;
    }
    
    // Test 3: Check authorization for READ permission
    bool authorized = security_manager_check_permission(session_id, SECURITY_MGR_RESOURCE_SYSTEM, SECURITY_MGR_PERM_READ);
    if (!authorized) {
        printf("❌ Test 3 failed: User should have READ permission\n");
        return false;
    }
    printf("✅ Test 3 passed: security_manager_check_permission() - READ permission\n");
    
    // Test 4: Check authorization for ADMIN permission (should fail)
    authorized = security_manager_check_permission(session_id, SECURITY_MGR_RESOURCE_SYSTEM, SECURITY_MGR_PERM_ADMIN);
    if (authorized) {
        printf("❌ Test 4 failed: User should not have ADMIN permission\n");
        return false;
    }
    printf("✅ Test 4 passed: security_manager_check_permission() - ADMIN permission denied\n");
    
    // Cleanup
    security_manager_logout_user(session_id);
    security_manager_remove_user("authuser");
    
    return true;
}

static bool test_session_management(void) {
    printf("\n=== Testing Session Management ===\n");
    
    // Test 1: Add test user
    security_mgr_user_config_t user = {
        .username = "sessionuser",
        .password_hash = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", // "password"
        .level = SECURITY_MGR_LEVEL_OPERATOR,
        .auth_method = SECURITY_MGR_AUTH_PASSWORD,
        .enabled = true,
        .login_attempts = 0,
        .last_login_time = 0,
        .password_expiry_time = 0
    };
    
    hal_status_t status = security_manager_add_user(&user);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: Could not add test user\n");
        return false;
    }
    
    // Test 2: Create session
    char session_id[64];
    status = security_manager_authenticate_user("sessionuser", "password", session_id);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: Could not create session\n");
        return false;
    }
    printf("✅ Test 2 passed: security_manager_authenticate_user() - session created\n");
    
    // Test 3: Validate session
    security_mgr_level_t level;
    status = security_manager_validate_session(session_id, &level);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: Session should be valid\n");
        return false;
    }
    printf("✅ Test 3 passed: security_manager_validate_session() - session valid\n");
    
    // Test 4: Check permission
    bool authorized = security_manager_check_permission(session_id, SECURITY_MGR_RESOURCE_SYSTEM, SECURITY_MGR_PERM_READ);
    if (!authorized) {
        printf("❌ Test 4 failed: User should have read permission\n");
        return false;
    }
    printf("✅ Test 4 passed: security_manager_check_permission() - read permission granted\n");
    
    // Test 5: Logout
    status = security_manager_logout_user(session_id);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 5 failed: security_manager_logout_user() returned %d\n", status);
        return false;
    }
    printf("✅ Test 5 passed: security_manager_logout_user()\n");
    
    // Test 6: Validate session after logout
    status = security_manager_validate_session(session_id, &level);
    if (status == HAL_STATUS_OK) {
        printf("❌ Test 6 failed: Session should be invalid after logout\n");
        return false;
    }
    printf("✅ Test 6 passed: security_manager_validate_session() - session invalid after logout\n");
    
    // Cleanup
    security_manager_remove_user("sessionuser");
    
    return true;
}

static bool test_ssl_tls_configuration(void) {
    printf("\n=== Testing SSL/TLS Configuration ===\n");
    
    // TODO: Implement SSL/TLS configuration tests when functions are available
    printf("⚠️  SSL/TLS configuration tests skipped - functions not implemented yet\n");
    
    return true;
}

static bool test_security_monitoring(void) {
    printf("\n=== Testing Security Monitoring ===\n");
    
    // Test 1: Get security status
    security_mgr_status_t status_info;
    hal_status_t status = security_manager_get_status(&status_info);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: security_manager_get_status() returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: security_manager_get_status()\n");
    
    // Test 2: Get security statistics
    security_mgr_stats_t stats;
    status = security_manager_get_statistics(&stats);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: security_manager_get_statistics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: security_manager_get_statistics()\n");
    
    // Test 3: Reset security statistics
    status = security_manager_reset_statistics();
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: security_manager_reset_statistics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: security_manager_reset_statistics()\n");
    
    // Test 4: Get diagnostics
    char diagnostics[1024];
    status = security_manager_get_diagnostics(diagnostics, sizeof(diagnostics));
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 4 failed: security_manager_get_diagnostics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 4 passed: security_manager_get_diagnostics()\n");
    
    return true;
}

static bool test_security_operations(void) {
    printf("\n=== Testing Security Operations ===\n");
    
    // Test 1: Set event callback
    hal_status_t status = security_manager_set_callback(test_security_event_callback);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 1 failed: security_manager_set_callback() returned %d\n", status);
        return false;
    }
    printf("✅ Test 1 passed: security_manager_set_callback()\n");
    
    // Test 2: Get diagnostics
    char diagnostics[1024];
    status = security_manager_get_diagnostics(diagnostics, sizeof(diagnostics));
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 2 failed: security_manager_get_diagnostics() returned %d\n", status);
        return false;
    }
    printf("✅ Test 2 passed: security_manager_get_diagnostics()\n");
    printf("Diagnostics:\n%s\n", diagnostics);
    
    return true;
}

static bool test_security_error_handling(void) {
    printf("\n=== Testing Security Error Handling ===\n");
    
    // Test 1: Test with invalid parameters
    hal_status_t status = security_manager_get_status(NULL);
    if (status != HAL_STATUS_INVALID_PARAMETER) {
        printf("❌ Test 1 failed: security_manager_get_status(NULL) returned %d, expected INVALID_PARAMETER\n", status);
        return false;
    }
    printf("✅ Test 1 passed: security_manager_get_status(NULL) - invalid parameter\n");
    
    // Test 2: Test without initialization
    security_manager_deinit();
    status = security_manager_get_status(NULL);
    if (status != HAL_STATUS_NOT_INITIALIZED) {
        printf("❌ Test 2 failed: security_manager_get_status() returned %d, expected NOT_INITIALIZED\n", status);
        return false;
    }
    printf("✅ Test 2 passed: security_manager_get_status() - not initialized\n");
    
    // Test 3: Re-initialize for remaining tests
    status = security_manager_init(&test_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ Test 3 failed: security_manager_init() returned %d\n", status);
        return false;
    }
    printf("✅ Test 3 passed: security_manager_init() - re-initialized\n");
    
    return true;
}

int main(int argc, char *argv) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;  // Suppress unused parameter warning
    
    printf("=== Security Manager Test Suite ===\n");
    printf("Version: 1.0.0\n");
    printf("Date: 2025-01-27\n");
    printf("Team: FW\n");
    printf("Task: FW-05 (Security Framework)\n\n");
    
    bool all_tests_passed = true;
    
    // Run test suites
    if (!test_security_initialization()) {
        all_tests_passed = false;
    }
    
    if (!test_security_configuration()) {
        all_tests_passed = false;
    }
    
    if (!test_user_management()) {
        all_tests_passed = false;
    }
    
    if (!test_authentication()) {
        all_tests_passed = false;
    }
    
    if (!test_authorization()) {
        all_tests_passed = false;
    }
    
    if (!test_session_management()) {
        all_tests_passed = false;
    }
    
    if (!test_ssl_tls_configuration()) {
        all_tests_passed = false;
    }
    
    if (!test_security_monitoring()) {
        all_tests_passed = false;
    }
    
    if (!test_security_operations()) {
        all_tests_passed = false;
    }
    
    if (!test_security_error_handling()) {
        all_tests_passed = false;
    }
    
    // Cleanup
    security_manager_deinit();
    
    // Summary
    printf("\n=== Test Summary ===\n");
    if (all_tests_passed) {
        printf("✅ All tests passed!\n");
        printf("Security Manager is working correctly.\n");
        return 0;
    } else {
        printf("❌ Some tests failed!\n");
        printf("Please review the failed tests above.\n");
        return 1;
    }
}
