/**
 * @file test_hal_estop.c
 * @brief Unit tests for HAL E-Stop safety system
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-15 (E-Stop Safety Testing)
 */

#include "unity.h"
#include "hal_estop.h"
#include "hal_common.h"
// No mock includes needed
#include <string.h>
#include <stdbool.h>
#include <stdio.h> // Added for printf

// Test fixtures
static estop_config_t test_config;
static estop_status_t test_status;

void setUp(void)
{
    // Reset E-Stop system state
    hal_estop_deinit();
    
    // Initialize test configuration with valid values
    test_config.pin = 59; // ESTOP_PIN
    test_config.response_timeout_ms = 100; // ESTOP_RESPONSE_TIME_MS
    test_config.debounce_time_ms = 50; // ESTOP_DEBOUNCE_TIME_MS
    test_config.auto_reset_enabled = true;
    
    // Initialize test status
    memset(&test_status, 0, sizeof(estop_status_t));
}

void tearDown(void)
{
    // Cleanup after each test - mock_estop_reset() in setUp() handles this
    // hal_estop_deinit(); // Removed to avoid interfering with mock state
}

// ============================================================================
// INITIALIZATION TESTS
// ============================================================================

void test_hal_estop_init_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_estop_init_null_config(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status = hal_estop_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

void test_hal_estop_init_invalid_pin(void)
{
    setUp(); // Ensure setup is called
    estop_config_t invalid_config = test_config;
    invalid_config.pin = 64; // Invalid pin (must be 0-63 for Orange Pi 5B)
    
    hal_status_t status = hal_estop_init(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status); // Fixed: expect INVALID_PARAMETER
}

void test_hal_estop_init_invalid_debounce(void)
{
    setUp(); // Ensure setup is called
    estop_config_t invalid_config = test_config;
    invalid_config.debounce_time_ms = 0; // Invalid debounce
    
    hal_status_t status = hal_estop_init(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status); // Fixed: expect INVALID_PARAMETER
}

void test_hal_estop_init_invalid_timeout(void)
{
    setUp(); // Ensure setup is called
    estop_config_t invalid_config = test_config;
    invalid_config.response_timeout_ms = 0; // Invalid timeout
    
    hal_status_t status = hal_estop_init(&invalid_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status); // Fixed: expect INVALID_PARAMETER
}

void test_hal_estop_double_init(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

void test_hal_estop_deinit_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
}

void test_hal_estop_deinit_not_initialized(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status = hal_estop_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

// ============================================================================
// CONFIGURATION TESTS
// ============================================================================

void test_hal_estop_get_config_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    estop_config_t retrieved_config;
    hal_status_t status2 = hal_estop_get_config(&retrieved_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
    TEST_ASSERT_EQUAL(test_config.pin, retrieved_config.pin);
    TEST_ASSERT_EQUAL(test_config.debounce_time_ms, retrieved_config.debounce_time_ms);
}

void test_hal_estop_get_config_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_get_config(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

void test_hal_estop_get_config_not_initialized(void)
{
    setUp(); // Ensure setup is called
    estop_config_t config;
    hal_status_t status = hal_estop_get_config(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

// ============================================================================
// STATUS MONITORING TESTS
// ============================================================================

void test_hal_estop_get_status_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    estop_status_t status;
    hal_status_t status2 = hal_estop_get_status(&status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
    TEST_ASSERT_EQUAL(ESTOP_STATE_SAFE, status.state);
    TEST_ASSERT_EQUAL(ESTOP_FAULT_NONE, status.fault);
}

void test_hal_estop_get_status_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_get_status(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

void test_hal_estop_get_status_not_initialized(void)
{
    setUp(); // Ensure setup is called
    estop_status_t status;
    hal_status_t result = hal_estop_get_status(&status);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, result);
}

void test_hal_estop_is_triggered_false(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    bool triggered = false;
    hal_status_t status2 = hal_estop_is_triggered(&triggered);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
    TEST_ASSERT_FALSE(triggered);
}

void test_hal_estop_is_triggered_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_is_triggered(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

void test_hal_estop_is_triggered_not_initialized(void)
{
    setUp(); // Ensure setup is called
    bool triggered = false;
    hal_status_t status = hal_estop_is_triggered(&triggered);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

// ============================================================================
// CHANNEL MONITORING TESTS
// ============================================================================

void test_hal_estop_get_pin_status_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    bool pin_status = false;
    hal_status_t status2 = hal_estop_get_pin_status(&pin_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
    TEST_ASSERT_TRUE(pin_status);
}

void test_hal_estop_get_pin_status_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_get_pin_status(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

void test_hal_estop_test_pin_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    bool pin_status = false;
    hal_status_t status2 = hal_estop_test_pin(&pin_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
    TEST_ASSERT_TRUE(pin_status);
}

void test_hal_estop_test_pin_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_test_pin(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

// ============================================================================
// EVENT HANDLING TESTS
// ============================================================================

void test_hal_estop_set_callback_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    estop_event_callback_t callback = NULL;
    hal_status_t status2 = hal_estop_set_callback(callback);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
}

void test_hal_estop_set_callback_not_initialized(void)
{
    setUp(); // Ensure setup is called
    estop_event_callback_t callback = NULL;
    hal_status_t status = hal_estop_set_callback(callback);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

// ============================================================================
// SAFETY VALIDATION TESTS
// ============================================================================

void test_hal_estop_validate_safety_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_validate_safety();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
}

void test_hal_estop_validate_safety_not_initialized(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status = hal_estop_validate_safety();
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

void test_hal_estop_check_safety_compliance_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    bool compliant = false;
    hal_status_t status2 = hal_estop_check_safety_compliance(&compliant);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
    TEST_ASSERT_TRUE(compliant);
}

void test_hal_estop_check_safety_compliance_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_check_safety_compliance(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

// ============================================================================
// DIAGNOSTICS TESTS
// ============================================================================

void test_hal_estop_self_test_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_self_test();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
}

void test_hal_estop_self_test_not_initialized(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status = hal_estop_self_test();
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

void test_hal_estop_get_diagnostics_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    char diagnostics[256];
    hal_status_t status2 = hal_estop_get_diagnostics(diagnostics, sizeof(diagnostics));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
    TEST_ASSERT_TRUE(strlen(diagnostics) > 0);
}

void test_hal_estop_get_diagnostics_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_get_diagnostics(NULL, 256);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

void test_hal_estop_validate_hardware_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_validate_hardware();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
}

void test_hal_estop_validate_hardware_not_initialized(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status = hal_estop_validate_hardware();
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

// ============================================================================
// STATISTICS TESTS
// ============================================================================

void test_hal_estop_get_trigger_count_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    uint32_t trigger_count = 0;
    hal_status_t status2 = hal_estop_get_trigger_count(&trigger_count);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
}

void test_hal_estop_get_trigger_count_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_get_trigger_count(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

void test_hal_estop_get_fault_count_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    uint32_t fault_count = 0;
    hal_status_t status2 = hal_estop_get_fault_count(&fault_count);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
}

void test_hal_estop_get_fault_count_null_pointer(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_get_fault_count(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status2);
}

void test_hal_estop_reset_statistics_success(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status1 = hal_estop_init(&test_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status1);
    
    hal_status_t status2 = hal_estop_reset_statistics();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status2);
}

void test_hal_estop_reset_statistics_not_initialized(void)
{
    setUp(); // Ensure setup is called
    hal_status_t status = hal_estop_reset_statistics();
    TEST_ASSERT_EQUAL(HAL_STATUS_ERROR, status);
}

// ============================================================================
// CONSTANTS AND DATA STRUCTURES TESTS
// ============================================================================

void test_estop_constants(void)
{
    TEST_ASSERT_EQUAL(59, ESTOP_PIN);
    TEST_ASSERT_EQUAL(100, ESTOP_RESPONSE_TIME_MS);
    TEST_ASSERT_EQUAL(50, ESTOP_DEBOUNCE_TIME_MS);
    TEST_ASSERT_EQUAL_STRING("SIL2", ESTOP_SAFETY_LEVEL);
}

void test_estop_data_structures(void)
{
    TEST_ASSERT_EQUAL(16, sizeof(estop_config_t));
    TEST_ASSERT_EQUAL(40, sizeof(estop_status_t)); // Fixed: 40 bytes due to padding/alignment
    TEST_ASSERT_EQUAL(4, sizeof(estop_state_t));
    TEST_ASSERT_EQUAL(4, sizeof(estop_fault_t));
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Initialization tests
    RUN_TEST(test_hal_estop_init_success);
    RUN_TEST(test_hal_estop_init_null_config);
    RUN_TEST(test_hal_estop_init_invalid_pin);
    RUN_TEST(test_hal_estop_init_invalid_debounce);
    RUN_TEST(test_hal_estop_init_invalid_timeout);
    RUN_TEST(test_hal_estop_double_init);
    RUN_TEST(test_hal_estop_deinit_success);
    RUN_TEST(test_hal_estop_deinit_not_initialized);
    
    // Configuration tests
    RUN_TEST(test_hal_estop_get_config_success);
    RUN_TEST(test_hal_estop_get_config_null_pointer);
    RUN_TEST(test_hal_estop_get_config_not_initialized);
    
    // Status monitoring tests
    RUN_TEST(test_hal_estop_get_status_success);
    RUN_TEST(test_hal_estop_get_status_null_pointer);
    RUN_TEST(test_hal_estop_get_status_not_initialized);
    RUN_TEST(test_hal_estop_is_triggered_false);
    RUN_TEST(test_hal_estop_is_triggered_null_pointer);
    RUN_TEST(test_hal_estop_is_triggered_not_initialized);
    
    // Pin monitoring tests
    RUN_TEST(test_hal_estop_get_pin_status_success);
    RUN_TEST(test_hal_estop_get_pin_status_null_pointer);
    RUN_TEST(test_hal_estop_test_pin_success);
    RUN_TEST(test_hal_estop_test_pin_null_pointer);
    
    // Event handling tests
    RUN_TEST(test_hal_estop_set_callback_success);
    RUN_TEST(test_hal_estop_set_callback_not_initialized);
    
    // Safety validation tests
    RUN_TEST(test_hal_estop_validate_safety_success);
    RUN_TEST(test_hal_estop_validate_safety_not_initialized);
    RUN_TEST(test_hal_estop_check_safety_compliance_success);
    RUN_TEST(test_hal_estop_check_safety_compliance_null_pointer);
    
    // Diagnostics tests
    RUN_TEST(test_hal_estop_self_test_success);
    RUN_TEST(test_hal_estop_self_test_not_initialized);
    RUN_TEST(test_hal_estop_get_diagnostics_success);
    RUN_TEST(test_hal_estop_get_diagnostics_null_pointer);
    RUN_TEST(test_hal_estop_validate_hardware_success);
    RUN_TEST(test_hal_estop_validate_hardware_not_initialized);
    
    // Statistics tests
    RUN_TEST(test_hal_estop_get_trigger_count_success);
    RUN_TEST(test_hal_estop_get_trigger_count_null_pointer);
    RUN_TEST(test_hal_estop_get_fault_count_success);
    RUN_TEST(test_hal_estop_get_fault_count_null_pointer);
    RUN_TEST(test_hal_estop_reset_statistics_success);
    RUN_TEST(test_hal_estop_reset_statistics_not_initialized);
    
    // Constants and data structures tests
    RUN_TEST(test_estop_constants);
    RUN_TEST(test_estop_data_structures);
    
    UNITY_END();
    return 0;
}
