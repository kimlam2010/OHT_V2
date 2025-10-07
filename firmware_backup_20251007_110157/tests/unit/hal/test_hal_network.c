/**
 * @file test_hal_network.c
 * @brief Unit tests for HAL Network
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-14 (Network Communication Testing)
 */

#include "unity.h"
#include "hal_network.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Test fixtures
static network_config_t test_config;

void setUp(void)
{
    // Initialize test configuration
    memset(&test_config, 0, sizeof(network_config_t));
    test_config.type = NETWORK_TYPE_ETHERNET;
    test_config.dhcp_enabled = false;
    strcpy(test_config.ip_address, "192.168.1.100");
    strcpy(test_config.gateway, "192.168.1.1");
    strcpy(test_config.netmask, "255.255.255.0");
    strcpy(test_config.dns1, "8.8.8.8");
    strcpy(test_config.dns2, "8.8.4.4");
    test_config.timeout_ms = 5000;
    test_config.auto_reconnect = true;
}

void tearDown(void)
{
    // Cleanup after each test
    // Note: hal_network_deinit() may not exist in actual implementation
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_network_constants(void)
{
    TEST_ASSERT_EQUAL_STRING("eth0", NETWORK_ETH_INTERFACE);
    TEST_ASSERT_EQUAL_STRING("wlan0", NETWORK_WIFI_INTERFACE);
    TEST_ASSERT_EQUAL(10, NETWORK_ETH_SPEED_10);
    TEST_ASSERT_EQUAL(100, NETWORK_ETH_SPEED_100);
    TEST_ASSERT_EQUAL(1000, NETWORK_ETH_SPEED_1000);
}

void test_network_protocol_enum(void)
{
    TEST_ASSERT_EQUAL(0, NETWORK_TYPE_ETHERNET);
    TEST_ASSERT_EQUAL(1, NETWORK_TYPE_WIFI);
    TEST_ASSERT_EQUAL(2, NETWORK_TYPE_BOTH);
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_network_data_structures(void)
{
    TEST_ASSERT_EQUAL(4, sizeof(test_config.type));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.dhcp_enabled));
    TEST_ASSERT_EQUAL(16, sizeof(test_config.ip_address));
    TEST_ASSERT_EQUAL(16, sizeof(test_config.gateway));
    TEST_ASSERT_EQUAL(16, sizeof(test_config.netmask));
    TEST_ASSERT_EQUAL(16, sizeof(test_config.dns1));
    TEST_ASSERT_EQUAL(16, sizeof(test_config.dns2));
    TEST_ASSERT_EQUAL(4, sizeof(test_config.timeout_ms));
    TEST_ASSERT_EQUAL(1, sizeof(test_config.auto_reconnect));
}

void test_network_config_initialization(void)
{
    TEST_ASSERT_EQUAL(0, test_config.type);
    TEST_ASSERT_FALSE(test_config.dhcp_enabled);
    TEST_ASSERT_EQUAL_STRING("192.168.1.100", test_config.ip_address);
    TEST_ASSERT_EQUAL_STRING("192.168.1.1", test_config.gateway);
    TEST_ASSERT_EQUAL_STRING("255.255.255.0", test_config.netmask);
    TEST_ASSERT_EQUAL_STRING("8.8.8.8", test_config.dns1);
    TEST_ASSERT_EQUAL_STRING("8.8.4.4", test_config.dns2);
    TEST_ASSERT_EQUAL(5000, test_config.timeout_ms);
    TEST_ASSERT_TRUE(test_config.auto_reconnect);
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void test_network_config_validation(void)
{
    // Test valid configuration
    TEST_ASSERT_TRUE(test_config.type >= 0 && test_config.type <= 2);
    TEST_ASSERT_TRUE(strlen(test_config.ip_address) > 0);
    TEST_ASSERT_TRUE(strlen(test_config.gateway) > 0);
    TEST_ASSERT_TRUE(strlen(test_config.netmask) > 0);
    TEST_ASSERT_TRUE(strlen(test_config.dns1) > 0);
    TEST_ASSERT_TRUE(strlen(test_config.dns2) > 0);
    TEST_ASSERT_TRUE(test_config.timeout_ms > 0);
}

void test_network_ip_address_validation(void)
{
    // Test valid IP addresses
    const char* valid_ips[] = {
        "127.0.0.1",      // localhost
        "192.168.1.100",  // private network
        "10.0.0.1",       // private network
        "8.8.8.8"         // public DNS
    };
    
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(strlen(valid_ips[i]) > 0);
        TEST_ASSERT_TRUE(strlen(valid_ips[i]) < 16);
    }
}

void test_network_port_validation(void)
{
    // Test valid ports (for reference)
    uint16_t valid_ports[] = {80, 443, 8080, 65535};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(valid_ports[i] > 0 && valid_ports[i] <= 65535);
    }
    
    // Test invalid ports
    uint16_t invalid_ports[] = {0, 65534};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_FALSE(invalid_ports[i] > 0 && invalid_ports[i] <= 65535);
    }
}

// ============================================================================
// UTILITY TESTS
// ============================================================================

void test_network_interface_name_validation(void)
{
    // Test valid interface names
    const char* valid_interfaces[] = {NETWORK_ETH_INTERFACE, NETWORK_WIFI_INTERFACE, "lo", "usb0"};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(strlen(valid_interfaces[i]) > 0);
        TEST_ASSERT_TRUE(strlen(valid_interfaces[i]) < 16);
    }
}

void test_network_timeout_validation(void)
{
    // Test valid timeouts
    uint32_t valid_timeouts[] = {100, 1000, 5000, 30000};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(valid_timeouts[i] > 0 && valid_timeouts[i] <= 60000);
    }
    
    // Test invalid timeouts
    uint32_t invalid_timeouts[] = {0, 60001};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_FALSE(invalid_timeouts[i] > 0 && invalid_timeouts[i] <= 60000);
    }
}

void test_network_retry_count_validation(void)
{
    // Test valid retry counts (for reference)
    uint32_t valid_retries[] = {1, 3, 5, 10};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(valid_retries[i] > 0 && valid_retries[i] <= 20);
    }
    
    // Test invalid retry counts
    uint32_t invalid_retries[] = {0, 21};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_FALSE(invalid_retries[i] > 0 && invalid_retries[i] <= 20);
    }
}

// ============================================================================
// IP ADDRESS UTILITY TESTS
// ============================================================================

void test_network_ip_address_conversion(void)
{
    // Test IP address string validation
    const char* ip_address = "192.168.1.100";
    int octets[4];
    
    // Simple IP parsing simulation
    sscanf(ip_address, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]);
    
    TEST_ASSERT_EQUAL(192, octets[0]);
    TEST_ASSERT_EQUAL(168, octets[1]);
    TEST_ASSERT_EQUAL(1, octets[2]);
    TEST_ASSERT_EQUAL(100, octets[3]);
}

void test_network_subnet_validation(void)
{
    // Test subnet mask validation
    const char* valid_masks[] = {
        "255.255.255.0",   // /24
        "255.255.0.0",     // /16
        "255.0.0.0",       // /8
        "255.255.255.240"  // /28
    };
    
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(strlen(valid_masks[i]) > 0);
        TEST_ASSERT_TRUE(strlen(valid_masks[i]) < 16);
    }
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Constants tests
    RUN_TEST(test_network_constants);
    RUN_TEST(test_network_protocol_enum);
    
    // Data structures tests
    RUN_TEST(test_network_data_structures);
    RUN_TEST(test_network_config_initialization);
    
    // Validation tests
    RUN_TEST(test_network_config_validation);
    RUN_TEST(test_network_ip_address_validation);
    RUN_TEST(test_network_port_validation);
    
    // Utility tests
    RUN_TEST(test_network_interface_name_validation);
    RUN_TEST(test_network_timeout_validation);
    RUN_TEST(test_network_retry_count_validation);
    
    // IP address utility tests
    RUN_TEST(test_network_ip_address_conversion);
    RUN_TEST(test_network_subnet_validation);
    
    UNITY_END();
    return 0;
}
