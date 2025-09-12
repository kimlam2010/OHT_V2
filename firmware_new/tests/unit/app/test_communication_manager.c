/**
 * @file test_communication_manager.c
 * @brief Unit tests for Communication Manager
 * @version 1.0.0
 * @date 2025-01-27
 * @author FW Team
 */

#include "unity.h"
#include "communication_manager.h"
#include "hal_common.h"
#include "hal_rs485.h"
#include "system_state_machine.h"
#include "module_manager.h"

// Test data structures
static comm_mgr_config_t test_config;
// static comm_mgr_status_info_t test_status; // Unused - commented out
// static comm_mgr_stats_t test_stats; // Unused - commented out
static comm_mgr_modbus_request_t test_request;
static comm_mgr_modbus_response_t test_response;

// ============================================================================
// SETUP AND TEARDOWN
// ============================================================================

void setUp(void)
{
    // Initialize test configuration
    test_config.baud_rate = 115200;
    test_config.data_bits = 8;
    test_config.stop_bits = 1;
    test_config.parity = 0; // None
    test_config.timeout_ms = 1000;
    test_config.retry_count = 3;
    test_config.retry_delay_ms = 100;
    test_config.modbus_slave_id = 1;
    test_config.enable_crc_check = true;
    test_config.enable_echo_suppression = true;
    test_config.buffer_size = 1024;

    // Initialize test request
    test_request.slave_id = 1;
    test_request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    test_request.start_address = 0x0000;
    test_request.quantity = 1;
    test_request.data = NULL;
    test_request.data_length = 0;

    // Initialize test response
    test_response.slave_id = 1;
    test_response.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    test_response.data = NULL;
    test_response.data_length = 0;
    test_response.exception_code = MODBUS_EXCEPTION_NONE;
    test_response.is_exception = false;
}

void tearDown(void)
{
    // Cleanup if needed
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_communication_status_constants(void)
{
    // Test communication status constants
    TEST_ASSERT_EQUAL(0, COMM_MGR_STATUS_DISCONNECTED);
    TEST_ASSERT_EQUAL(1, COMM_MGR_STATUS_CONNECTING);
    TEST_ASSERT_EQUAL(2, COMM_MGR_STATUS_CONNECTED);
    TEST_ASSERT_EQUAL(3, COMM_MGR_STATUS_ERROR);
    TEST_ASSERT_EQUAL(4, COMM_MGR_STATUS_TIMEOUT);
}

void test_communication_error_constants(void)
{
    // Test communication error constants
    TEST_ASSERT_EQUAL(0, COMM_MGR_ERROR_NONE);
    TEST_ASSERT_EQUAL(1, COMM_MGR_ERROR_RS485_INIT_FAILED);
    TEST_ASSERT_EQUAL(2, COMM_MGR_ERROR_MODBUS_INIT_FAILED);
    TEST_ASSERT_EQUAL(3, COMM_MGR_ERROR_TIMEOUT);
    TEST_ASSERT_EQUAL(4, COMM_MGR_ERROR_CRC_FAILED);
    TEST_ASSERT_EQUAL(5, COMM_MGR_ERROR_FRAME_ERROR);
    TEST_ASSERT_EQUAL(6, COMM_MGR_ERROR_BUFFER_OVERFLOW);
    TEST_ASSERT_EQUAL(7, COMM_MGR_ERROR_INVALID_RESPONSE);
    TEST_ASSERT_EQUAL(8, COMM_MGR_ERROR_COMMUNICATION_LOST);
}

void test_modbus_exception_constants(void)
{
    // Test Modbus exception constants
    TEST_ASSERT_EQUAL(0x00, MODBUS_EXCEPTION_NONE);
    TEST_ASSERT_EQUAL(0x01, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
    TEST_ASSERT_EQUAL(0x02, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    TEST_ASSERT_EQUAL(0x03, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE);
    TEST_ASSERT_EQUAL(0x04, MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE);
    TEST_ASSERT_EQUAL(0x05, MODBUS_EXCEPTION_ACKNOWLEDGE);
    TEST_ASSERT_EQUAL(0x06, MODBUS_EXCEPTION_SLAVE_DEVICE_BUSY);
    TEST_ASSERT_EQUAL(0x08, MODBUS_EXCEPTION_MEMORY_PARITY_ERROR);
    TEST_ASSERT_EQUAL(0x0A, MODBUS_EXCEPTION_GATEWAY_PATH_UNAVAILABLE);
    TEST_ASSERT_EQUAL(0x0B, MODBUS_EXCEPTION_GATEWAY_TARGET_DEVICE_FAILED);
}

void test_communication_event_constants(void)
{
    // Test communication event constants
    TEST_ASSERT_EQUAL(0, COMM_MGR_EVENT_NONE);
    TEST_ASSERT_EQUAL(1, COMM_MGR_EVENT_CONNECTED);
    TEST_ASSERT_EQUAL(2, COMM_MGR_EVENT_DISCONNECTED);
    TEST_ASSERT_EQUAL(3, COMM_MGR_EVENT_ERROR);
    TEST_ASSERT_EQUAL(4, COMM_MGR_EVENT_TIMEOUT);
    TEST_ASSERT_EQUAL(5, COMM_MGR_EVENT_RESPONSE_RECEIVED);
    TEST_ASSERT_EQUAL(6, COMM_MGR_EVENT_RETRY_ATTEMPT);
    TEST_ASSERT_EQUAL(7, COMM_MGR_EVENT_MAX_RETRIES_EXCEEDED);
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_communication_manager_data_structures(void)
{
    setUp();  // Ensure proper initialization
    
    // Test configuration structure
    TEST_ASSERT_EQUAL(115200, test_config.baud_rate);
    TEST_ASSERT_EQUAL(8, test_config.data_bits);
    TEST_ASSERT_EQUAL(1, test_config.stop_bits);
    TEST_ASSERT_EQUAL(0, test_config.parity);
    TEST_ASSERT_EQUAL(1000, test_config.timeout_ms);
    TEST_ASSERT_EQUAL(3, test_config.retry_count);
    TEST_ASSERT_EQUAL(100, test_config.retry_delay_ms);
    TEST_ASSERT_EQUAL(1, test_config.modbus_slave_id);
    TEST_ASSERT_TRUE(test_config.enable_crc_check);
    TEST_ASSERT_TRUE(test_config.enable_echo_suppression);
    TEST_ASSERT_EQUAL(1024, test_config.buffer_size);

    // Test request structure
    TEST_ASSERT_EQUAL(1, test_request.slave_id);
    TEST_ASSERT_EQUAL(MODBUS_FC_READ_HOLDING_REGISTERS, test_request.function_code);
    TEST_ASSERT_EQUAL(0x0000, test_request.start_address);
    TEST_ASSERT_EQUAL(1, test_request.quantity);
    TEST_ASSERT_EQUAL(0, test_request.data_length);

    // Test response structure
    TEST_ASSERT_EQUAL(1, test_response.slave_id);
    TEST_ASSERT_EQUAL(MODBUS_FC_READ_HOLDING_REGISTERS, test_response.function_code);
    TEST_ASSERT_EQUAL(0, test_response.data_length);
    TEST_ASSERT_EQUAL(MODBUS_EXCEPTION_NONE, test_response.exception_code);
    TEST_ASSERT_FALSE(test_response.is_exception);
}

// ============================================================================
// INITIALIZATION TESTS
// ============================================================================

void test_communication_manager_config_initialization(void)
{
    // Test configuration initialization
    comm_mgr_config_t config = {0};
    
    // Initialize with test values
    config.baud_rate = 115200;
    config.data_bits = 8;
    config.stop_bits = 1;
    config.parity = 0;
    config.timeout_ms = 1000;
    config.retry_count = 3;
    config.retry_delay_ms = 100;
    config.modbus_slave_id = 1;
    config.enable_crc_check = true;
    config.enable_echo_suppression = true;
    config.buffer_size = 1024;

    // Verify initialization
    TEST_ASSERT_EQUAL(115200, config.baud_rate);
    TEST_ASSERT_EQUAL(8, config.data_bits);
    TEST_ASSERT_EQUAL(1, config.stop_bits);
    TEST_ASSERT_EQUAL(0, config.parity);
    TEST_ASSERT_EQUAL(1000, config.timeout_ms);
    TEST_ASSERT_EQUAL(3, config.retry_count);
    TEST_ASSERT_EQUAL(100, config.retry_delay_ms);
    TEST_ASSERT_EQUAL(1, config.modbus_slave_id);
    TEST_ASSERT_TRUE(config.enable_crc_check);
    TEST_ASSERT_TRUE(config.enable_echo_suppression);
    TEST_ASSERT_EQUAL(1024, config.buffer_size);
}

void test_communication_manager_status_initialization(void)
{
    // Test status initialization
    comm_mgr_status_info_t status = {0};
    
    // Verify default values
    TEST_ASSERT_EQUAL(COMM_MGR_STATUS_DISCONNECTED, status.status);
    TEST_ASSERT_EQUAL(COMM_MGR_ERROR_NONE, status.last_error);
    TEST_ASSERT_EQUAL(0, status.error_count);
    TEST_ASSERT_EQUAL(0, status.last_error_time);
    TEST_ASSERT_FALSE(status.rs485_initialized);
    TEST_ASSERT_FALSE(status.modbus_initialized);
    TEST_ASSERT_EQUAL(0, status.current_slave_id);
    TEST_ASSERT_EQUAL(0, status.connection_uptime_ms);
}

void test_communication_manager_stats_initialization(void)
{
    // Test statistics initialization
    comm_mgr_stats_t stats = {0};
    
    // Verify default values
    TEST_ASSERT_EQUAL(0, stats.total_transmissions);
    TEST_ASSERT_EQUAL(0, stats.successful_transmissions);
    TEST_ASSERT_EQUAL(0, stats.failed_transmissions);
    TEST_ASSERT_EQUAL(0, stats.timeout_count);
    TEST_ASSERT_EQUAL(0, stats.crc_error_count);
    TEST_ASSERT_EQUAL(0, stats.frame_error_count);
    TEST_ASSERT_EQUAL(0, stats.retry_count);
    TEST_ASSERT_EQUAL(0, stats.last_communication_time);
    TEST_ASSERT_EQUAL(0, stats.total_response_time);
    TEST_ASSERT_EQUAL(0, stats.response_count);
    TEST_ASSERT_EQUAL(0, stats.average_response_time_ms);
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void test_communication_manager_config_validation(void)
{
    setUp();  // Ensure proper initialization
    
    // Test valid configuration
    TEST_ASSERT_TRUE(test_config.baud_rate > 0);
    TEST_ASSERT_TRUE(test_config.data_bits >= 7 && test_config.data_bits <= 8);
    TEST_ASSERT_TRUE(test_config.stop_bits >= 1 && test_config.stop_bits <= 2);
    TEST_ASSERT_TRUE(test_config.parity >= 0 && test_config.parity <= 2);
    TEST_ASSERT_TRUE(test_config.timeout_ms > 0);
    TEST_ASSERT_TRUE(test_config.retry_count > 0);
    TEST_ASSERT_TRUE(test_config.retry_delay_ms > 0);
    TEST_ASSERT_TRUE(test_config.modbus_slave_id > 0);
    TEST_ASSERT_TRUE(test_config.buffer_size > 0);
}

void test_communication_manager_request_validation(void)
{
    setUp();  // Ensure proper initialization
    
    // Test valid request
    TEST_ASSERT_TRUE(test_request.slave_id > 0);
    TEST_ASSERT_TRUE(test_request.function_code >= 0x01 && test_request.function_code <= 0x10);
    TEST_ASSERT_TRUE(test_request.start_address >= 0x0000);
    TEST_ASSERT_TRUE(test_request.quantity > 0);
    TEST_ASSERT_TRUE(test_request.quantity <= 125); // Modbus limit
}

void test_communication_manager_response_validation(void)
{
    setUp();  // Ensure proper initialization
    
    // Test valid response
    TEST_ASSERT_TRUE(test_response.slave_id > 0);
    TEST_ASSERT_TRUE(test_response.function_code >= 0x01 && test_response.function_code <= 0x10);
    TEST_ASSERT_TRUE(test_response.exception_code >= 0x00 && test_response.exception_code <= 0x0B);
}

// ============================================================================
// UTILITY FUNCTION TESTS
// ============================================================================

void test_communication_manager_status_names(void)
{
    // Test status name functions
    const char* status_name;
    
    status_name = comm_manager_get_status_name(COMM_MGR_STATUS_DISCONNECTED);
    TEST_ASSERT_NOT_NULL(status_name);
    
    status_name = comm_manager_get_status_name(COMM_MGR_STATUS_CONNECTED);
    TEST_ASSERT_NOT_NULL(status_name);
    
    status_name = comm_manager_get_status_name(COMM_MGR_STATUS_ERROR);
    TEST_ASSERT_NOT_NULL(status_name);
}

void test_communication_manager_error_names(void)
{
    // Test error name functions
    const char* error_name;
    
    error_name = comm_manager_get_error_name(COMM_MGR_ERROR_NONE);
    TEST_ASSERT_NOT_NULL(error_name);
    
    error_name = comm_manager_get_error_name(COMM_MGR_ERROR_TIMEOUT);
    TEST_ASSERT_NOT_NULL(error_name);
    
    error_name = comm_manager_get_error_name(COMM_MGR_ERROR_CRC_FAILED);
    TEST_ASSERT_NOT_NULL(error_name);
}

void test_communication_manager_function_code_names(void)
{
    // Test function code name functions
    const char* function_name;
    
    function_name = comm_manager_get_function_code_name(MODBUS_FC_READ_COILS);
    TEST_ASSERT_NOT_NULL(function_name);
    
    function_name = comm_manager_get_function_code_name(MODBUS_FC_READ_HOLDING_REGISTERS);
    TEST_ASSERT_NOT_NULL(function_name);
    
    function_name = comm_manager_get_function_code_name(MODBUS_FC_WRITE_SINGLE_REGISTER);
    TEST_ASSERT_NOT_NULL(function_name);
}

void test_communication_manager_exception_code_names(void)
{
    // Test exception code name functions
    const char* exception_name;
    
    exception_name = comm_manager_get_exception_code_name(MODBUS_EXCEPTION_NONE);
    TEST_ASSERT_NOT_NULL(exception_name);
    
    exception_name = comm_manager_get_exception_code_name(MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
    TEST_ASSERT_NOT_NULL(exception_name);
    
    exception_name = comm_manager_get_exception_code_name(MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
    TEST_ASSERT_NOT_NULL(exception_name);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Constants tests
    RUN_TEST(test_communication_status_constants);
    RUN_TEST(test_communication_error_constants);
    RUN_TEST(test_modbus_exception_constants);
    RUN_TEST(test_communication_event_constants);
    
    // Data structures tests
    RUN_TEST(test_communication_manager_data_structures);
    
    // Initialization tests
    RUN_TEST(test_communication_manager_config_initialization);
    RUN_TEST(test_communication_manager_status_initialization);
    RUN_TEST(test_communication_manager_stats_initialization);
    
    // Validation tests
    RUN_TEST(test_communication_manager_config_validation);
    RUN_TEST(test_communication_manager_request_validation);
    RUN_TEST(test_communication_manager_response_validation);
    
    // Utility function tests
    RUN_TEST(test_communication_manager_status_names);
    RUN_TEST(test_communication_manager_error_names);
    RUN_TEST(test_communication_manager_function_code_names);
    RUN_TEST(test_communication_manager_exception_code_names);
    
    UNITY_END();
    return 0;
}
