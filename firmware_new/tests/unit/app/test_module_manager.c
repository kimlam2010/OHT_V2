/**
 * @file test_module_manager.c
 * @brief Unit tests for Module Manager
 * @version 1.0.0
 * @date 2025-01-27
 * @author FW Team
 */

#include "unity.h"
#include "module_manager.h"
#include "hal_common.h"
#include "communication_manager.h"
#include "system_state_machine.h"
#include <string.h>

// Test data structures
static module_info_t test_module_info;
static module_status_info_t test_module_status;
static module_config_t test_module_config;
static module_stats_t test_module_stats;

// ============================================================================
// SETUP AND TEARDOWN
// ============================================================================

void setUp(void)
{
    // Initialize test module info
    test_module_info.module_id = 1;
    test_module_info.address = 0x02;
    test_module_info.type = MODULE_TYPE_POWER;
    strcpy(test_module_info.name, "Test Power Module");
    strcpy(test_module_info.version, "1.0.0");
    strcpy(test_module_info.serial_number, "PWR001");
    test_module_info.capabilities = POWER_CAP_VOLTAGE_MONITOR | POWER_CAP_CURRENT_MONITOR;
    test_module_info.config_flags = 0;
    test_module_info.status = MODULE_STATUS_ONLINE;
    test_module_info.last_seen_ms = 1640995200000ULL;

    // Initialize test module status
    test_module_status.status = MODULE_STATUS_ONLINE;
    test_module_status.health = MODULE_HEALTH_GOOD;
    test_module_status.health_percentage = 85;
    test_module_status.uptime_seconds = 3600;
    test_module_status.error_count = 0;
    test_module_status.warning_count = 1;
    test_module_status.last_communication = 1640995200000ULL;
    test_module_status.response_time_ms = 50;
    test_module_status.timeout_count = 0;
    test_module_status.crc_error_count = 0;

    // Initialize test module config
    test_module_config.module_id = 1;
    test_module_config.discovery_timeout_ms = 5000;
    test_module_config.health_check_interval_ms = 1000;
    test_module_config.response_timeout_ms = 1000;
    test_module_config.retry_count = 3;
    test_module_config.config_flags = 0;

    // Initialize test module stats
    test_module_stats.total_modules = 5;
    test_module_stats.online_modules = 4;
    test_module_stats.offline_modules = 1;
    test_module_stats.error_modules = 0;
    test_module_stats.discovery_count = 10;
    test_module_stats.timeout_count = 2;
    test_module_stats.error_count = 0;
    test_module_stats.total_uptime = 86400000ULL;
}

void tearDown(void)
{
    // Cleanup if needed
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_module_type_constants(void)
{
    // Test module type constants
    TEST_ASSERT_EQUAL(0, MODULE_TYPE_UNKNOWN);
    TEST_ASSERT_EQUAL(1, MODULE_TYPE_MOTOR);
    TEST_ASSERT_EQUAL(2, MODULE_TYPE_IO);
    TEST_ASSERT_EQUAL(3, MODULE_TYPE_DOCK);
    TEST_ASSERT_EQUAL(4, MODULE_TYPE_SENSOR);
    TEST_ASSERT_EQUAL(5, MODULE_TYPE_POWER);
    TEST_ASSERT_EQUAL(6, MODULE_TYPE_ACTUATOR);
    TEST_ASSERT_EQUAL(7, MODULE_TYPE_CONTROLLER);
    TEST_ASSERT_EQUAL(8, MODULE_TYPE_SAFETY);
    TEST_ASSERT_EQUAL(9, MODULE_TYPE_COMMUNICATION);
    TEST_ASSERT_EQUAL(10, MODULE_TYPE_STORAGE);
    TEST_ASSERT_EQUAL(11, MODULE_TYPE_DISPLAY);
}

void test_module_status_constants(void)
{
    // Test module status constants
    TEST_ASSERT_EQUAL(0, MODULE_STATUS_UNKNOWN);
    TEST_ASSERT_EQUAL(1, MODULE_STATUS_OFFLINE);
    TEST_ASSERT_EQUAL(2, MODULE_STATUS_ONLINE);
    TEST_ASSERT_EQUAL(3, MODULE_STATUS_ERROR);
    TEST_ASSERT_EQUAL(4, MODULE_STATUS_WARNING);
    TEST_ASSERT_EQUAL(5, MODULE_STATUS_MAINTENANCE);
}

void test_module_health_constants(void)
{
    // Test module health constants
    TEST_ASSERT_EQUAL(0, MODULE_HEALTH_UNKNOWN);
    TEST_ASSERT_EQUAL(1, MODULE_HEALTH_EXCELLENT);
    TEST_ASSERT_EQUAL(2, MODULE_HEALTH_GOOD);
    TEST_ASSERT_EQUAL(3, MODULE_HEALTH_FAIR);
    TEST_ASSERT_EQUAL(4, MODULE_HEALTH_POOR);
    TEST_ASSERT_EQUAL(5, MODULE_HEALTH_CRITICAL);
    TEST_ASSERT_EQUAL(6, MODULE_HEALTH_FAILED);
}

void test_module_event_constants(void)
{
    // Test module event constants
    TEST_ASSERT_EQUAL(0, MODULE_EVENT_NONE);
    TEST_ASSERT_EQUAL(1, MODULE_EVENT_DISCOVERED);
    TEST_ASSERT_EQUAL(2, MODULE_EVENT_REGISTERED);
    TEST_ASSERT_EQUAL(3, MODULE_EVENT_ONLINE);
    TEST_ASSERT_EQUAL(4, MODULE_EVENT_OFFLINE);
    TEST_ASSERT_EQUAL(5, MODULE_EVENT_ERROR);
    TEST_ASSERT_EQUAL(6, MODULE_EVENT_WARNING);
    TEST_ASSERT_EQUAL(7, MODULE_EVENT_HEALTH_CHANGE);
    TEST_ASSERT_EQUAL(8, MODULE_EVENT_CONFIG_CHANGE);
    TEST_ASSERT_EQUAL(9, MODULE_EVENT_UPDATED);
    TEST_ASSERT_EQUAL(10, MODULE_EVENT_TIMEOUT);
}

void test_power_capability_constants(void)
{
    // Test power capability constants
    TEST_ASSERT_EQUAL(1, POWER_CAP_VOLTAGE_MONITOR);
    TEST_ASSERT_EQUAL(2, POWER_CAP_CURRENT_MONITOR);
    TEST_ASSERT_EQUAL(4, POWER_CAP_TEMP_MONITOR);
    TEST_ASSERT_EQUAL(8, POWER_CAP_RELAY_CONTROL);
    TEST_ASSERT_EQUAL(16, POWER_CAP_OVERVOLTAGE_PROTECT);
    TEST_ASSERT_EQUAL(32, POWER_CAP_OVERCURRENT_PROTECT);
    TEST_ASSERT_EQUAL(64, POWER_CAP_OVERTEMP_PROTECT);
}

void test_power_register_constants(void)
{
    // Test power register constants
    TEST_ASSERT_EQUAL(0x0000, POWER_REG_VOLTAGE_MAIN);
    TEST_ASSERT_EQUAL(0x0001, POWER_REG_CURRENT_MAIN);
    TEST_ASSERT_EQUAL(0x0002, POWER_REG_TEMP_MAIN);
    TEST_ASSERT_EQUAL(0x0004, POWER_REG_RELAY1_STATUS);
    TEST_ASSERT_EQUAL(0x0005, POWER_REG_RELAY2_STATUS);
    TEST_ASSERT_EQUAL(0x0006, POWER_REG_RELAY1_CONTROL);
    TEST_ASSERT_EQUAL(0x0007, POWER_REG_RELAY2_CONTROL);
    TEST_ASSERT_EQUAL(0x0008, POWER_REG_ALARM_STATUS);
    TEST_ASSERT_EQUAL(0x00F0, POWER_REG_DEVICE_ID);
    TEST_ASSERT_EQUAL(0x00F7, POWER_REG_MODULE_TYPE);
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_module_manager_data_structures(void)
{
    // Test module info structure
    TEST_ASSERT_EQUAL(1, test_module_info.module_id);
    TEST_ASSERT_EQUAL(0x02, test_module_info.address);
    TEST_ASSERT_EQUAL(MODULE_TYPE_POWER, test_module_info.type);
    TEST_ASSERT_EQUAL_STRING("Test Power Module", test_module_info.name);
    TEST_ASSERT_EQUAL_STRING("1.0.0", test_module_info.version);
    TEST_ASSERT_EQUAL_STRING("PWR001", test_module_info.serial_number);
    TEST_ASSERT_EQUAL(POWER_CAP_VOLTAGE_MONITOR | POWER_CAP_CURRENT_MONITOR, test_module_info.capabilities);
    TEST_ASSERT_EQUAL(0, test_module_info.config_flags);
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, test_module_info.status);
    TEST_ASSERT_EQUAL(1640995200000ULL, test_module_info.last_seen_ms);

    // Test module status structure
    TEST_ASSERT_EQUAL(MODULE_STATUS_ONLINE, test_module_status.status);
    TEST_ASSERT_EQUAL(MODULE_HEALTH_GOOD, test_module_status.health);
    TEST_ASSERT_EQUAL(85, test_module_status.health_percentage);
    TEST_ASSERT_EQUAL(3600, test_module_status.uptime_seconds);
    TEST_ASSERT_EQUAL(0, test_module_status.error_count);
    TEST_ASSERT_EQUAL(1, test_module_status.warning_count);
    TEST_ASSERT_EQUAL(1640995200000ULL, test_module_status.last_communication);
    TEST_ASSERT_EQUAL(50, test_module_status.response_time_ms);
    TEST_ASSERT_EQUAL(0, test_module_status.timeout_count);
    TEST_ASSERT_EQUAL(0, test_module_status.crc_error_count);

    // Test module config structure
    TEST_ASSERT_EQUAL(1, test_module_config.module_id);
    TEST_ASSERT_EQUAL(5000, test_module_config.discovery_timeout_ms);
    TEST_ASSERT_EQUAL(1000, test_module_config.health_check_interval_ms);
    TEST_ASSERT_EQUAL(1000, test_module_config.response_timeout_ms);
    TEST_ASSERT_EQUAL(3, test_module_config.retry_count);
    TEST_ASSERT_EQUAL(0, test_module_config.config_flags);

    // Test module stats structure
    TEST_ASSERT_EQUAL(5, test_module_stats.total_modules);
    TEST_ASSERT_EQUAL(4, test_module_stats.online_modules);
    TEST_ASSERT_EQUAL(1, test_module_stats.offline_modules);
    TEST_ASSERT_EQUAL(0, test_module_stats.error_modules);
    TEST_ASSERT_EQUAL(10, test_module_stats.discovery_count);
    TEST_ASSERT_EQUAL(2, test_module_stats.timeout_count);
    TEST_ASSERT_EQUAL(0, test_module_stats.error_count);
    TEST_ASSERT_EQUAL(86400000ULL, test_module_stats.total_uptime);
}

// ============================================================================
// INITIALIZATION TESTS
// ============================================================================

void test_module_manager_config_initialization(void)
{
    // Test configuration initialization
    module_config_t config = {0};
    
    // Initialize with test values
    config.module_id = 1;
    config.discovery_timeout_ms = 5000;
    config.health_check_interval_ms = 1000;
    config.response_timeout_ms = 1000;
    config.retry_count = 3;
    config.config_flags = 0;

    // Verify initialization
    TEST_ASSERT_EQUAL(1, config.module_id);
    TEST_ASSERT_EQUAL(5000, config.discovery_timeout_ms);
    TEST_ASSERT_EQUAL(1000, config.health_check_interval_ms);
    TEST_ASSERT_EQUAL(1000, config.response_timeout_ms);
    TEST_ASSERT_EQUAL(3, config.retry_count);
    TEST_ASSERT_EQUAL(0, config.config_flags);
}

void test_module_manager_status_initialization(void)
{
    // Test status initialization
    module_status_info_t status = {0};
    
    // Verify default values
    TEST_ASSERT_EQUAL(MODULE_STATUS_UNKNOWN, status.status);
    TEST_ASSERT_EQUAL(MODULE_HEALTH_UNKNOWN, status.health);
    TEST_ASSERT_EQUAL(0, status.health_percentage);
    TEST_ASSERT_EQUAL(0, status.uptime_seconds);
    TEST_ASSERT_EQUAL(0, status.error_count);
    TEST_ASSERT_EQUAL(0, status.warning_count);
    TEST_ASSERT_EQUAL(0, status.last_communication);
    TEST_ASSERT_EQUAL(0, status.response_time_ms);
    TEST_ASSERT_EQUAL(0, status.timeout_count);
    TEST_ASSERT_EQUAL(0, status.crc_error_count);
}

void test_module_manager_stats_initialization(void)
{
    // Test statistics initialization
    module_stats_t stats = {0};
    
    // Verify default values
    TEST_ASSERT_EQUAL(0, stats.total_modules);
    TEST_ASSERT_EQUAL(0, stats.online_modules);
    TEST_ASSERT_EQUAL(0, stats.offline_modules);
    TEST_ASSERT_EQUAL(0, stats.error_modules);
    TEST_ASSERT_EQUAL(0, stats.discovery_count);
    TEST_ASSERT_EQUAL(0, stats.timeout_count);
    TEST_ASSERT_EQUAL(0, stats.error_count);
    TEST_ASSERT_EQUAL(0, stats.total_uptime);
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void test_module_manager_config_validation(void)
{
    // Test valid configuration
    TEST_ASSERT_TRUE(test_module_config.module_id > 0);
    TEST_ASSERT_TRUE(test_module_config.discovery_timeout_ms > 0);
    TEST_ASSERT_TRUE(test_module_config.health_check_interval_ms > 0);
    TEST_ASSERT_TRUE(test_module_config.response_timeout_ms > 0);
    TEST_ASSERT_TRUE(test_module_config.retry_count > 0);
}

void test_module_manager_status_validation(void)
{
    // Test valid status
    TEST_ASSERT_TRUE(test_module_status.status >= MODULE_STATUS_UNKNOWN && test_module_status.status <= MODULE_STATUS_MAINTENANCE);
    TEST_ASSERT_TRUE(test_module_status.health >= MODULE_HEALTH_UNKNOWN && test_module_status.health <= MODULE_HEALTH_FAILED);
    TEST_ASSERT_TRUE(test_module_status.health_percentage >= 0 && test_module_status.health_percentage <= 100);
    TEST_ASSERT_TRUE(test_module_status.uptime_seconds >= 0);
    TEST_ASSERT_TRUE(test_module_status.error_count >= 0);
    TEST_ASSERT_TRUE(test_module_status.warning_count >= 0);
    TEST_ASSERT_TRUE(test_module_status.last_communication > 0);
    TEST_ASSERT_TRUE(test_module_status.response_time_ms >= 0);
    TEST_ASSERT_TRUE(test_module_status.timeout_count >= 0);
    TEST_ASSERT_TRUE(test_module_status.crc_error_count >= 0);
}

void test_module_manager_stats_validation(void)
{
    // Test valid stats
    TEST_ASSERT_TRUE(test_module_stats.total_modules >= 0);
    TEST_ASSERT_TRUE(test_module_stats.online_modules >= 0);
    TEST_ASSERT_TRUE(test_module_stats.offline_modules >= 0);
    TEST_ASSERT_TRUE(test_module_stats.error_modules >= 0);
    TEST_ASSERT_TRUE(test_module_stats.discovery_count >= 0);
    TEST_ASSERT_TRUE(test_module_stats.timeout_count >= 0);
    TEST_ASSERT_TRUE(test_module_stats.error_count >= 0);
    TEST_ASSERT_TRUE(test_module_stats.total_uptime >= 0);
    
    // Test logical relationships
    TEST_ASSERT_TRUE(test_module_stats.online_modules + test_module_stats.offline_modules + test_module_stats.error_modules <= test_module_stats.total_modules);
}

// ============================================================================
// UTILITY FUNCTION TESTS
// ============================================================================

void test_module_manager_type_names(void)
{
    // Test type name functions
    const char* type_name;
    
    type_name = module_manager_get_type_name(MODULE_TYPE_UNKNOWN);
    TEST_ASSERT_NOT_NULL(type_name);
    
    type_name = module_manager_get_type_name(MODULE_TYPE_POWER);
    TEST_ASSERT_NOT_NULL(type_name);
    
    type_name = module_manager_get_type_name(MODULE_TYPE_MOTOR);
    TEST_ASSERT_NOT_NULL(type_name);
}

void test_module_manager_status_names(void)
{
    // Test status name functions
    const char* status_name;
    
    status_name = module_manager_get_status_name(MODULE_STATUS_UNKNOWN);
    TEST_ASSERT_NOT_NULL(status_name);
    
    status_name = module_manager_get_status_name(MODULE_STATUS_ONLINE);
    TEST_ASSERT_NOT_NULL(status_name);
    
    status_name = module_manager_get_status_name(MODULE_STATUS_ERROR);
    TEST_ASSERT_NOT_NULL(status_name);
}

void test_module_manager_health_names(void)
{
    // Test health name functions
    const char* health_name;
    
    health_name = module_manager_get_health_name(MODULE_HEALTH_UNKNOWN);
    TEST_ASSERT_NOT_NULL(health_name);
    
    health_name = module_manager_get_health_name(MODULE_HEALTH_GOOD);
    TEST_ASSERT_NOT_NULL(health_name);
    
    health_name = module_manager_get_health_name(MODULE_HEALTH_CRITICAL);
    TEST_ASSERT_NOT_NULL(health_name);
}

void test_module_manager_event_names(void)
{
    // Test event name functions
    const char* event_name;
    
    event_name = module_manager_get_event_name(MODULE_EVENT_NONE);
    TEST_ASSERT_NOT_NULL(event_name);
    
    event_name = module_manager_get_event_name(MODULE_EVENT_DISCOVERED);
    TEST_ASSERT_NOT_NULL(event_name);
    
    event_name = module_manager_get_event_name(MODULE_EVENT_ERROR);
    TEST_ASSERT_NOT_NULL(event_name);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Constants tests
    RUN_TEST(test_module_type_constants);
    RUN_TEST(test_module_status_constants);
    RUN_TEST(test_module_health_constants);
    RUN_TEST(test_module_event_constants);
    RUN_TEST(test_power_capability_constants);
    RUN_TEST(test_power_register_constants);
    
    // Data structures tests
    RUN_TEST(test_module_manager_data_structures);
    
    // Initialization tests
    RUN_TEST(test_module_manager_config_initialization);
    RUN_TEST(test_module_manager_status_initialization);
    RUN_TEST(test_module_manager_stats_initialization);
    
    // Validation tests
    RUN_TEST(test_module_manager_config_validation);
    RUN_TEST(test_module_manager_status_validation);
    RUN_TEST(test_module_manager_stats_validation);
    
    // Utility function tests
    RUN_TEST(test_module_manager_type_names);
    RUN_TEST(test_module_manager_status_names);
    RUN_TEST(test_module_manager_health_names);
    RUN_TEST(test_module_manager_event_names);
    
    UNITY_END();
    return 0;
}
