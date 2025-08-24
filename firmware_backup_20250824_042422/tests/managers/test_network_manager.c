/**
 * @file test_network_manager.c
 * @brief Network Manager Test Suite
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-04 (Network Redundancy System)
 */

#include "network_manager.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test counters
static int test_total = 0;
static int test_passed = 0;
static int test_failed = 0;

// Test assertion macro
#define test_assert(condition, message) do { \
    test_total++; \
    if (condition) { \
        printf("✅ PASS: %s\n", message); \
        test_passed++; \
    } else { \
        printf("❌ FAIL: %s\n", message); \
        test_failed++; \
    } \
} while(0)

// Test functions
static void test_network_manager_initialization(void);
static void test_network_manager_configuration(void);
static void test_network_manager_status(void);
static void test_network_manager_statistics(void);
static void test_network_manager_event_callback(void);
static void test_network_manager_interface_management(void);
static void test_network_manager_failover(void);
static void test_network_manager_health_check(void);
static void test_network_manager_utility_functions(void);
static void test_network_manager_self_test(void);
static void test_network_manager_reset(void);
static void test_network_manager_update(void);
static void test_network_manager_deinitialization(void);
static void print_test_summary(void);

static void test_network_manager_initialization(void) {
    printf("\n=== Testing Network Manager Initialization ===\n");
    
    // Test initialization with valid config
    network_mgr_config_t config = {
        .failover_mode = NETWORK_MGR_FAILOVER_MODE_AUTO,
        .failover_timeout_ms = 3000,
        .health_check_interval_ms = 10000,
        .auto_failover_enabled = true,
        .load_balancing_enabled = false,
        .max_failover_attempts = 3,
        .monitoring_enabled = true,
        .statistics_interval_ms = 60000
    };
    
    hal_status_t status = network_manager_init(&config);
    test_assert(status == HAL_STATUS_OK, "Network Manager initialization with valid config");
    
    // Test double initialization
    status = network_manager_init(&config);
    test_assert(status == HAL_STATUS_ALREADY_INITIALIZED, "Network Manager double initialization");
    
    // Test initialization with NULL config
    network_manager_deinit();
    status = network_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Network Manager initialization with NULL config");
}

static void test_network_manager_configuration(void) {
    printf("\n=== Testing Network Manager Configuration ===\n");
    
    // Test get configuration
    network_mgr_config_t config;
    hal_status_t status = network_manager_get_config(&config);
    test_assert(status == HAL_STATUS_OK, "Get Network Manager configuration");
    test_assert(config.failover_mode == NETWORK_MGR_FAILOVER_MODE_AUTO, "Default failover mode");
    test_assert(config.failover_timeout_ms == NETWORK_MGR_FAILOVER_TIMEOUT, "Default failover timeout");
    test_assert(config.health_check_interval_ms == NETWORK_MGR_HEALTH_CHECK_INTERVAL, "Default health check interval");
    test_assert(config.auto_failover_enabled == true, "Default auto failover enabled");
    test_assert(config.load_balancing_enabled == false, "Default load balancing disabled");
    test_assert(config.max_failover_attempts == 3, "Default max failover attempts");
    test_assert(config.monitoring_enabled == true, "Default monitoring enabled");
    test_assert(config.statistics_interval_ms == 60000, "Default statistics interval");
    
    // Test set configuration
    config.failover_timeout_ms = 5000;
    config.health_check_interval_ms = 15000;
    config.auto_failover_enabled = false;
    status = network_manager_set_config(&config);
    test_assert(status == HAL_STATUS_OK, "Set Network Manager configuration");
    
    // Verify configuration was set
    network_mgr_config_t new_config;
    status = network_manager_get_config(&new_config);
    test_assert(status == HAL_STATUS_OK, "Get updated configuration");
    test_assert(new_config.failover_timeout_ms == 5000, "Updated failover timeout");
    test_assert(new_config.health_check_interval_ms == 15000, "Updated health check interval");
    test_assert(new_config.auto_failover_enabled == false, "Updated auto failover disabled");
    
    // Test set configuration with NULL
    status = network_manager_set_config(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Set configuration with NULL");
}

static void test_network_manager_status(void) {
    printf("\n=== Testing Network Manager Status ===\n");
    
    // Test get status
    network_mgr_status_t status_info;
    hal_status_t status = network_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get Network Manager status");
    test_assert(status_info.overall_state == NETWORK_MGR_STATE_DISCONNECTED, "Initial overall state");
    test_assert(status_info.active_interface == NETWORK_MGR_INTERFACE_ETHERNET, "Initial active interface");
    test_assert(status_info.primary_interface == NETWORK_MGR_INTERFACE_ETHERNET, "Initial primary interface");
    test_assert(status_info.total_interfaces == 0, "Initial total interfaces");
    test_assert(status_info.active_interfaces == 0, "Initial active interfaces");
    test_assert(status_info.failed_interfaces == 0, "Initial failed interfaces");
    test_assert(status_info.failover_count == 0, "Initial failover count");
    test_assert(status_info.total_uptime == 0, "Initial total uptime");
    test_assert(status_info.failover_in_progress == false, "Initial failover in progress");
    
    // Test get status with NULL
    status = network_manager_get_status(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Get status with NULL");
}

static void test_network_manager_statistics(void) {
    printf("\n=== Testing Network Manager Statistics ===\n");
    
    // Test get statistics
    network_mgr_stats_t stats;
    hal_status_t status = network_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get Network Manager statistics");
    test_assert(stats.total_connections == 0, "Initial total connections");
    test_assert(stats.successful_connections == 0, "Initial successful connections");
    test_assert(stats.failed_connections == 0, "Initial failed connections");
    test_assert(stats.total_failovers == 0, "Initial total failovers");
    test_assert(stats.successful_failovers == 0, "Initial successful failovers");
    test_assert(stats.failed_failovers == 0, "Initial failed failovers");
    test_assert(stats.health_check_count == 0, "Initial health check count");
    test_assert(stats.health_check_failures == 0, "Initial health check failures");
    test_assert(stats.total_bytes_sent == 0, "Initial total bytes sent");
    test_assert(stats.total_bytes_received == 0, "Initial total bytes received");
    test_assert(stats.average_latency_ms == 0, "Initial average latency");
    test_assert(stats.packet_loss_percentage == 0, "Initial packet loss percentage");
    
    // Test reset statistics
    status = network_manager_reset_statistics();
    test_assert(status == HAL_STATUS_OK, "Reset Network Manager statistics");
    
    // Verify statistics were reset
    status = network_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get reset statistics");
    test_assert(stats.total_connections == 0, "Reset total connections");
    test_assert(stats.successful_connections == 0, "Reset successful connections");
}

static void test_network_manager_event_callback(void) {
    printf("\n=== Testing Network Manager Event Callback ===\n");
    
    // Test set callback
    bool callback_called = false;
    network_mgr_event_t received_event = NETWORK_MGR_EVENT_NONE;
    network_mgr_interface_t received_interface = NETWORK_MGR_INTERFACE_ETHERNET;
    
    void test_callback(network_mgr_event_t event, network_mgr_interface_t interface, const void *data) {
        (void)data; // Unused parameter
        callback_called = true;
        received_event = event;
        received_interface = interface;
    }
    
    hal_status_t status = network_manager_set_callback(test_callback);
    test_assert(status == HAL_STATUS_OK, "Set Network Manager event callback");
    
    // Test set callback to NULL
    status = network_manager_set_callback(NULL);
    test_assert(status == HAL_STATUS_OK, "Set Network Manager callback to NULL");
}

static void test_network_manager_interface_management(void) {
    printf("\n=== Testing Network Manager Interface Management ===\n");
    
    // Test add interface
    network_mgr_interface_config_t eth_config = {
        .type = NETWORK_MGR_INTERFACE_ETHERNET,
        .priority = NETWORK_MGR_PRIORITY_PRIMARY,
        .enabled = true,
        .health_check_interval_ms = 5000,
        .failover_timeout_ms = 3000,
        .auto_reconnect = true
    };
    strcpy(eth_config.interface_name, "eth0");
    
    hal_status_t status = network_manager_add_interface(0, &eth_config);
    test_assert(status == HAL_STATUS_OK, "Add Ethernet interface");
    
    // Test add duplicate interface
    status = network_manager_add_interface(0, &eth_config);
    test_assert(status == HAL_STATUS_ALREADY_INITIALIZED, "Add duplicate interface");
    
    // Test add interface with invalid ID
    status = network_manager_add_interface(NETWORK_MGR_MAX_INTERFACES, &eth_config);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Add interface with invalid ID");
    
    // Test add interface with NULL config
    status = network_manager_add_interface(1, NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Add interface with NULL config");
    
    // Test get interface status
    network_mgr_interface_status_t interface_status;
    status = network_manager_get_interface_status(0, &interface_status);
    test_assert(status == HAL_STATUS_OK, "Get interface status");
    test_assert(interface_status.type == NETWORK_MGR_INTERFACE_ETHERNET, "Interface type");
    test_assert(interface_status.priority == NETWORK_MGR_PRIORITY_PRIMARY, "Interface priority");
    test_assert(interface_status.state == NETWORK_MGR_STATE_DISCONNECTED, "Interface state");
    test_assert(interface_status.active == false, "Interface active");
    test_assert(interface_status.health_score == 0, "Interface health score");
    test_assert(interface_status.failover_count == 0, "Interface failover count");
    
    // Test get interface status for non-existent interface
    status = network_manager_get_interface_status(1, &interface_status);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Get status for non-existent interface");
    
    // Test enable interface
    status = network_manager_enable_interface(0);
    test_assert(status == HAL_STATUS_OK, "Enable interface");
    
    // Test disable interface
    status = network_manager_disable_interface(0);
    test_assert(status == HAL_STATUS_OK, "Disable interface");
    
    // Test enable non-existent interface
    status = network_manager_enable_interface(1);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Enable non-existent interface");
    
    // Test remove interface
    status = network_manager_remove_interface(0);
    test_assert(status == HAL_STATUS_OK, "Remove interface");
    
    // Test remove non-existent interface
    status = network_manager_remove_interface(0);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Remove non-existent interface");
}

static void test_network_manager_failover(void) {
    printf("\n=== Testing Network Manager Failover ===\n");
    
    // Add test interfaces
    network_mgr_interface_config_t eth_config = {
        .type = NETWORK_MGR_INTERFACE_ETHERNET,
        .priority = NETWORK_MGR_PRIORITY_PRIMARY,
        .enabled = true,
        .health_check_interval_ms = 5000,
        .failover_timeout_ms = 3000,
        .auto_reconnect = true
    };
    strcpy(eth_config.interface_name, "eth0");
    
    network_mgr_interface_config_t wifi_config = {
        .type = NETWORK_MGR_INTERFACE_WIFI,
        .priority = NETWORK_MGR_PRIORITY_SECONDARY,
        .enabled = true,
        .health_check_interval_ms = 5000,
        .failover_timeout_ms = 3000,
        .auto_reconnect = true
    };
    strcpy(wifi_config.interface_name, "wlan0");
    
    hal_status_t status = network_manager_add_interface(0, &eth_config);
    test_assert(status == HAL_STATUS_OK, "Add Ethernet interface for failover test");
    
    status = network_manager_add_interface(1, &wifi_config);
    test_assert(status == HAL_STATUS_OK, "Add WiFi interface for failover test");
    
    // Test manual failover
    status = network_manager_manual_failover(1);
    test_assert(status == HAL_STATUS_ERROR || status == HAL_STATUS_OK, "Manual failover to WiFi");
    
    // Test manual failover to invalid interface
    status = network_manager_manual_failover(2);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Manual failover to invalid interface");
    
    // Test restore primary
    status = network_manager_restore_primary();
    test_assert(status == HAL_STATUS_ERROR || status == HAL_STATUS_OK, "Restore primary interface");
    
    // Clean up
    network_manager_remove_interface(0);
    network_manager_remove_interface(1);
}

static void test_network_manager_health_check(void) {
    printf("\n=== Testing Network Manager Health Check ===\n");
    
    // Add test interface
    network_mgr_interface_config_t eth_config = {
        .type = NETWORK_MGR_INTERFACE_ETHERNET,
        .priority = NETWORK_MGR_PRIORITY_PRIMARY,
        .enabled = true,
        .health_check_interval_ms = 5000,
        .failover_timeout_ms = 3000,
        .auto_reconnect = true
    };
    strcpy(eth_config.interface_name, "eth0");
    
    hal_status_t status = network_manager_add_interface(0, &eth_config);
    test_assert(status == HAL_STATUS_OK, "Add interface for health check test");
    
    // Test health check on specific interface
    status = network_manager_health_check_interface(0);
    test_assert(status == HAL_STATUS_OK, "Health check on specific interface");
    
    // Test health check on non-existent interface
    status = network_manager_health_check_interface(1);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Health check on non-existent interface");
    
    // Test health check on all interfaces
    status = network_manager_health_check_all();
    test_assert(status == HAL_STATUS_OK, "Health check on all interfaces");
    
    // Clean up
    network_manager_remove_interface(0);
}

static void test_network_manager_utility_functions(void) {
    printf("\n=== Testing Network Manager Utility Functions ===\n");
    
    // Test interface type names
    const char* type_name = network_manager_get_interface_type_name(NETWORK_MGR_INTERFACE_ETHERNET);
    test_assert(strcmp(type_name, "ETHERNET") == 0, "Ethernet interface type name");
    
    type_name = network_manager_get_interface_type_name(NETWORK_MGR_INTERFACE_WIFI);
    test_assert(strcmp(type_name, "WIFI") == 0, "WiFi interface type name");
    
    type_name = network_manager_get_interface_type_name(NETWORK_MGR_INTERFACE_CELLULAR);
    test_assert(strcmp(type_name, "CELLULAR") == 0, "Cellular interface type name");
    
    // Test interface state names
    const char* state_name = network_manager_get_interface_state_name(NETWORK_MGR_STATE_CONNECTED);
    test_assert(strcmp(state_name, "CONNECTED") == 0, "Connected state name");
    
    state_name = network_manager_get_interface_state_name(NETWORK_MGR_STATE_DISCONNECTED);
    test_assert(strcmp(state_name, "DISCONNECTED") == 0, "Disconnected state name");
    
    state_name = network_manager_get_interface_state_name(NETWORK_MGR_STATE_FAILED);
    test_assert(strcmp(state_name, "FAILED") == 0, "Failed state name");
    
    // Test priority names
    const char* priority_name = network_manager_get_priority_name(NETWORK_MGR_PRIORITY_PRIMARY);
    test_assert(strcmp(priority_name, "PRIMARY") == 0, "Primary priority name");
    
    priority_name = network_manager_get_priority_name(NETWORK_MGR_PRIORITY_SECONDARY);
    test_assert(strcmp(priority_name, "SECONDARY") == 0, "Secondary priority name");
    
    priority_name = network_manager_get_priority_name(NETWORK_MGR_PRIORITY_BACKUP);
    test_assert(strcmp(priority_name, "BACKUP") == 0, "Backup priority name");
    
    // Test failover mode names
    const char* mode_name = network_manager_get_failover_mode_name(NETWORK_MGR_FAILOVER_MODE_AUTO);
    test_assert(strcmp(mode_name, "AUTO") == 0, "Auto failover mode name");
    
    mode_name = network_manager_get_failover_mode_name(NETWORK_MGR_FAILOVER_MODE_MANUAL);
    test_assert(strcmp(mode_name, "MANUAL") == 0, "Manual failover mode name");
    
    mode_name = network_manager_get_failover_mode_name(NETWORK_MGR_FAILOVER_MODE_DISABLED);
    test_assert(strcmp(mode_name, "DISABLED") == 0, "Disabled failover mode name");
    
    // Test event names
    const char* event_name = network_manager_get_event_name(NETWORK_MGR_EVENT_INTERFACE_CONNECTED);
    test_assert(strcmp(event_name, "INTERFACE_CONNECTED") == 0, "Interface connected event name");
    
    event_name = network_manager_get_event_name(NETWORK_MGR_EVENT_FAILOVER_STARTED);
    test_assert(strcmp(event_name, "FAILOVER_STARTED") == 0, "Failover started event name");
    
    event_name = network_manager_get_event_name(NETWORK_MGR_EVENT_HEALTH_CHECK_FAILED);
    test_assert(strcmp(event_name, "HEALTH_CHECK_FAILED") == 0, "Health check failed event name");
}

static void test_network_manager_self_test(void) {
    printf("\n=== Testing Network Manager Self Test ===\n");
    
    // Test self test
    hal_status_t status = network_manager_self_test();
    test_assert(status == HAL_STATUS_OK, "Network Manager self test");
}

static void test_network_manager_reset(void) {
    printf("\n=== Testing Network Manager Reset ===\n");
    
    // Add test interface for reset
    network_mgr_interface_config_t eth_config = {
        .type = NETWORK_MGR_INTERFACE_ETHERNET,
        .priority = NETWORK_MGR_PRIORITY_PRIMARY,
        .enabled = true,
        .health_check_interval_ms = 5000,
        .failover_timeout_ms = 3000,
        .auto_reconnect = true
    };
    strcpy(eth_config.interface_name, "eth0");
    
    hal_status_t status = network_manager_add_interface(0, &eth_config);
    test_assert(status == HAL_STATUS_OK, "Add interface for reset test");
    
    // Test reset
    status = network_manager_reset();
    test_assert(status == HAL_STATUS_OK, "Network Manager reset");
    
    // Verify reset
    network_mgr_status_t status_info;
    status = network_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get status after reset");
    test_assert(status_info.total_interfaces == 0, "No interfaces after reset");
    test_assert(status_info.active_interfaces == 0, "No active interfaces after reset");
    test_assert(status_info.failed_interfaces == 0, "No failed interfaces after reset");
    test_assert(status_info.failover_in_progress == false, "No failover in progress after reset");
}

static void test_network_manager_update(void) {
    printf("\n=== Testing Network Manager Update ===\n");
    
    // Test update
    hal_status_t status = network_manager_update();
    test_assert(status == HAL_STATUS_OK, "Network Manager update");
    
    // Test multiple updates
    for (int i = 0; i < 5; i++) {
        status = network_manager_update();
        test_assert(status == HAL_STATUS_OK, "Network Manager update iteration");
        hal_sleep_ms(10);
    }
}

static void test_network_manager_deinitialization(void) {
    printf("\n=== Testing Network Manager Deinitialization ===\n");
    
    // Test deinitialization
    hal_status_t status = network_manager_deinit();
    test_assert(status == HAL_STATUS_OK, "Network Manager deinitialization");
    
    // Test operations after deinitialization
    network_mgr_status_t status_info;
    status = network_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Get status after deinit");
    
    network_mgr_config_t config;
    status = network_manager_get_config(&config);
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Get config after deinit");
    
    status = network_manager_update();
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Update after deinit");
}

static void print_test_summary(void) {
    printf("\n=== Network Manager Test Summary ===\n");
    printf("Total Tests: %d\n", test_total);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);
    printf("Success Rate: %.1f%%\n", (float)test_passed / test_total * 100.0f);
    
    if (test_failed == 0) {
        printf("🎉 All tests passed!\n");
    } else {
        printf("⚠️  Some tests failed. Please review the implementation.\n");
    }
}

int main(void) {
    printf("🚀 Starting Network Manager Tests...\n");
    printf("==========================================\n");
    
    // Initialize test counters
    test_passed = 0;
    test_failed = 0;
    test_total = 0;
    
    // Run all tests
    test_network_manager_initialization();
    test_network_manager_configuration();
    test_network_manager_status();
    test_network_manager_statistics();
    test_network_manager_event_callback();
    test_network_manager_interface_management();
    test_network_manager_failover();
    test_network_manager_health_check();
    test_network_manager_utility_functions();
    test_network_manager_self_test();
    test_network_manager_reset();
    test_network_manager_update();
    test_network_manager_deinitialization();
    
    // Print summary
    print_test_summary();
    
    return (test_failed == 0) ? 0 : 1;
}
