/**
 * @file test_system_integration.c
 * @brief Integration tests for OHT-50 Master Module System
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "unity.h"
#include "hal_common.h"
#include "hal_gpio.h"
#include "api_manager.h"
#include "communication_manager.h"
#include "module_manager.h"
#include "safety_manager.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Test fixtures
static api_mgr_config_t api_config;
static comm_mgr_config_t comm_config;
static module_mgr_config_t module_config;
static safety_mgr_config_t safety_config;

void setUp(void) {
    // Initialize API configuration
    api_config.http_port = 8080;
    api_config.websocket_port = 8081;
    api_config.http_enabled = true;
    api_config.websocket_enabled = true;
    api_config.cors_enabled = true;
    strcpy(api_config.cors_origin, "http://localhost:3000");
    api_config.max_request_size = 4096;
    api_config.max_response_size = 8192;
    api_config.request_timeout_ms = 5000;
    api_config.websocket_timeout_ms = 30000;
    api_config.authentication_required = false;
    api_config.ssl_enabled = false;
    
    // Initialize communication configuration
    comm_config.baud_rate = 115200;
    comm_config.data_bits = 8;
    comm_config.stop_bits = 1;
    comm_config.parity = 0;
    comm_config.timeout_ms = 1000;
    comm_config.retry_count = 3;
    comm_config.retry_delay_ms = 100;
    comm_config.modbus_slave_id = 1;
    comm_config.enable_crc_check = true;
    comm_config.enable_echo_suppression = true;
    comm_config.buffer_size = 256;
    
    // Initialize module configuration
    module_config.max_modules = 10;
    module_config.discovery_interval_ms = 5000;
    module_config.health_check_interval_ms = 1000;
    module_config.response_timeout_ms = 500;
    module_config.max_retries = 3;
    module_config.auto_discovery_enabled = true;
    
    // Initialize safety configuration
    safety_config.estop_pin = 1;
    safety_config.estop_active_low = true;
    safety_config.estop_debounce_ms = 50;
    safety_config.safety_check_interval_ms = 100;
    safety_config.max_speed_limit = 1000;
    safety_config.max_acceleration_limit = 500;
    safety_config.safety_zone_enabled = true;
    safety_config.interlock_enabled = true;
}

void tearDown(void) {
    // Clean up all managers
    safety_manager_deinit();
    module_manager_deinit();
    comm_manager_deinit();
    api_manager_deinit();
    hal_gpio_deinit();
}

// Test complete system initialization
void test_system_complete_initialization_returns_success(void) {
    // Initialize HAL layer
    hal_status_t hal_status = hal_gpio_init();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, hal_status);
    
    // Initialize communication manager
    hal_status_t comm_status = comm_manager_init(&comm_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_status);
    
    // Initialize module manager
    hal_status_t module_status = module_manager_init(&module_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, module_status);
    
    // Initialize safety manager
    hal_status_t safety_status = safety_manager_init(&safety_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_status);
    
    // Initialize API manager
    hal_status_t api_status = api_manager_init(&api_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, api_status);
    
    // Verify all managers are initialized
    comm_mgr_status_t comm_mgr_status;
    comm_status = comm_manager_get_status(&comm_mgr_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, comm_status);
    TEST_ASSERT_TRUE(comm_mgr_status.initialized);
    
    module_mgr_status_t module_mgr_status;
    module_status = module_manager_get_status(&module_mgr_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, module_status);
    TEST_ASSERT_TRUE(module_mgr_status.initialized);
    
    safety_mgr_status_t safety_mgr_status;
    safety_status = safety_manager_get_status(&safety_mgr_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_status);
    TEST_ASSERT_TRUE(safety_mgr_status.initialized);
    
    api_mgr_status_t api_mgr_status;
    api_status = api_manager_get_status(&api_mgr_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, api_status);
    TEST_ASSERT_TRUE(api_mgr_status.initialized);
}

// Test system communication flow
void test_system_communication_flow_works_correctly(void) {
    // Initialize system
    hal_gpio_init();
    comm_manager_init(&comm_config);
    module_manager_init(&module_config);
    safety_manager_init(&safety_config);
    api_manager_init(&api_config);
    
    // Start communication
    hal_status_t status = comm_manager_connect();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Start module discovery
    status = module_manager_start_discovery();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify communication is active
    comm_mgr_status_t comm_status;
    comm_manager_get_status(&comm_status);
    TEST_ASSERT_EQUAL(COMM_MGR_STATUS_CONNECTED, comm_status.status);
    
    // Verify module discovery is active
    module_mgr_status_t module_status;
    module_manager_get_status(&module_status);
    TEST_ASSERT_TRUE(module_status.discovery_active);
}

// Test API communication with backend
void test_api_backend_communication_works_correctly(void) {
    // Initialize system
    hal_gpio_init();
    api_manager_init(&api_config);
    
    // Start HTTP server
    hal_status_t status = api_manager_start_http_server();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Start WebSocket server
    status = api_manager_start_websocket_server();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify servers are active
    api_mgr_status_t api_status;
    api_manager_get_status(&api_status);
    TEST_ASSERT_TRUE(api_status.http_server_active);
    TEST_ASSERT_TRUE(api_status.websocket_server_active);
    
    // Test HTTP request processing
    api_mgr_http_request_t request = {0};
    api_mgr_http_response_t response = {0};
    
    request.method = API_MGR_HTTP_GET;
    strcpy(request.url, "/api/v1/system/status");
    strcpy(request.path, "/api/v1/system/status");
    request.content_type = API_MGR_CONTENT_TYPE_JSON;
    strcpy(request.client_ip, "127.0.0.1");
    
    status = api_manager_process_http_request(&request, &response);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(API_MGR_RESPONSE_OK, response.status_code);
    
    // Test WebSocket message broadcast
    const char *message = "{\"type\":\"system_status\",\"data\":\"online\"}";
    status = api_manager_broadcast_websocket_message(message, strlen(message));
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

// Test safety system integration
void test_safety_system_integration_works_correctly(void) {
    // Initialize system
    hal_gpio_init();
    safety_manager_init(&safety_config);
    
    // Start safety monitoring
    hal_status_t status = safety_manager_start_monitoring();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify safety system is active
    safety_mgr_status_t safety_status;
    safety_manager_get_status(&safety_status);
    TEST_ASSERT_TRUE(safety_status.monitoring_active);
    TEST_ASSERT_FALSE(safety_status.estop_active);
    
    // Test E-Stop functionality (simulated)
    status = safety_manager_trigger_estop(SAFETY_MGR_ESTOP_REASON_MANUAL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify E-Stop is active
    safety_manager_get_status(&safety_status);
    TEST_ASSERT_TRUE(safety_status.estop_active);
    TEST_ASSERT_EQUAL(SAFETY_MGR_ESTOP_REASON_MANUAL, safety_status.estop_reason);
    
    // Test E-Stop acknowledgment
    status = safety_manager_acknowledge_estop();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify E-Stop is acknowledged
    safety_manager_get_status(&safety_status);
    TEST_ASSERT_TRUE(safety_status.estop_acknowledged);
}

// Test module discovery and registration
void test_module_discovery_and_registration_works_correctly(void) {
    // Initialize system
    hal_gpio_init();
    comm_manager_init(&comm_config);
    module_manager_init(&module_config);
    
    // Start communication and discovery
    comm_manager_connect();
    module_manager_start_discovery();
    
    // Simulate module discovery
    module_info_t module_info = {
        .module_id = 1,
        .module_type = MODULE_TYPE_POWER,
        .status = MODULE_STATUS_ONLINE,
        .version = "1.0.0"
    };
    
    // Register module
    hal_status_t status = module_manager_register_module(&module_info);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify module is registered
    module_info_t retrieved_info;
    status = module_manager_get_module_info(1, &retrieved_info);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(1, retrieved_info.module_id);
    TEST_ASSERT_EQUAL(MODULE_TYPE_POWER, retrieved_info.module_type);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, retrieved_info.status);
    
    // Test module health check
    status = module_manager_perform_health_check(1);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify health check updated module status
    module_manager_get_module_info(1, &retrieved_info);
    TEST_ASSERT_GREATER_THAN(0, retrieved_info.last_health_check);
}

// Test error handling and recovery
void test_error_handling_and_recovery_works_correctly(void) {
    // Initialize system
    hal_gpio_init();
    comm_manager_init(&comm_config);
    module_manager_init(&module_config);
    safety_manager_init(&safety_config);
    api_manager_init(&api_config);
    
    // Test communication error recovery
    comm_mgr_status_t comm_status;
    comm_manager_get_status(&comm_status);
    TEST_ASSERT_EQUAL(COMM_MGR_STATUS_DISCONNECTED, comm_status.status);
    
    // Attempt to connect
    hal_status_t status = comm_manager_connect();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify connection
    comm_manager_get_status(&comm_status);
    TEST_ASSERT_EQUAL(COMM_MGR_STATUS_CONNECTED, comm_status.status);
    
    // Test module error handling
    module_info_t invalid_module = {
        .module_id = 999,  // Invalid module ID
        .module_type = MODULE_TYPE_UNKNOWN,
        .status = MODULE_STATUS_ERROR
    };
    
    status = module_manager_register_module(&invalid_module);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    // Test API error handling
    api_mgr_http_request_t invalid_request = {0};
    api_mgr_http_response_t response = {0};
    
    status = api_manager_process_http_request(&invalid_request, &response);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

// Test system performance under load
void test_system_performance_under_load(void) {
    // Initialize system
    hal_gpio_init();
    api_manager_init(&api_config);
    api_manager_start_http_server();
    
    // Test API performance under load
    uint64_t start_time = hal_get_timestamp_us();
    
    for (int i = 0; i < 100; i++) {
        api_mgr_http_request_t request = {0};
        api_mgr_http_response_t response = {0};
        
        request.method = API_MGR_HTTP_GET;
        snprintf(request.url, sizeof(request.url), "/api/v1/system/status?test=%d", i);
        snprintf(request.path, sizeof(request.path), "/api/v1/system/status");
        request.content_type = API_MGR_CONTENT_TYPE_JSON;
        strcpy(request.client_ip, "127.0.0.1");
        
        hal_status_t status = api_manager_process_http_request(&request, &response);
        TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    }
    
    uint64_t end_time = hal_get_timestamp_us();
    uint64_t duration = end_time - start_time;
    
    // Should process 100 requests in reasonable time (< 100ms)
    TEST_ASSERT_LESS_THAN(100000, duration);
    
    // Verify system is still responsive
    api_mgr_status_t api_status;
    hal_status_t status = api_manager_get_status(&api_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    TEST_ASSERT_TRUE(api_status.http_server_active);
}

// Test system shutdown and cleanup
void test_system_shutdown_and_cleanup_works_correctly(void) {
    // Initialize system
    hal_gpio_init();
    comm_manager_init(&comm_config);
    module_manager_init(&module_config);
    safety_manager_init(&safety_config);
    api_manager_init(&api_config);
    
    // Start all services
    comm_manager_connect();
    module_manager_start_discovery();
    safety_manager_start_monitoring();
    api_manager_start_http_server();
    api_manager_start_websocket_server();
    
    // Verify all services are active
    comm_mgr_status_t comm_status;
    module_mgr_status_t module_status;
    safety_mgr_status_t safety_status;
    api_mgr_status_t api_status;
    
    comm_manager_get_status(&comm_status);
    module_manager_get_status(&module_status);
    safety_manager_get_status(&safety_status);
    api_manager_get_status(&api_status);
    
    TEST_ASSERT_TRUE(comm_status.initialized);
    TEST_ASSERT_TRUE(module_status.initialized);
    TEST_ASSERT_TRUE(safety_status.initialized);
    TEST_ASSERT_TRUE(api_status.initialized);
    
    // Shutdown system
    hal_status_t status = api_manager_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = safety_manager_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = module_manager_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = comm_manager_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_gpio_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify all services are deinitialized
    status = api_manager_get_status(&api_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
    
    status = safety_manager_get_status(&safety_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
    
    status = module_manager_get_status(&module_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
    
    status = comm_manager_get_status(&comm_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

// Main test runner
int main(void) {
    UNITY_BEGIN();
    
    // System integration tests
    RUN_TEST(test_system_complete_initialization_returns_success);
    RUN_TEST(test_system_communication_flow_works_correctly);
    RUN_TEST(test_api_backend_communication_works_correctly);
    RUN_TEST(test_safety_system_integration_works_correctly);
    RUN_TEST(test_module_discovery_and_registration_works_correctly);
    RUN_TEST(test_error_handling_and_recovery_works_correctly);
    RUN_TEST(test_system_performance_under_load);
    RUN_TEST(test_system_shutdown_and_cleanup_works_correctly);
    
    UNITY_END();
    return 0;
}
