/**
 * @file test_led_manager.c
 * @brief Test program for LED Manager
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-03 (LED Manager Implementation)
 */

#include "led_manager.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test event callback
static void test_led_event_callback(led_mgr_event_t event, led_mgr_pattern_t pattern) {
    printf("LED Event: %d, Pattern Type: %s\n", event, 
           led_manager_get_pattern_type_name(pattern.type));
}

// Test counters
static int test_passed = 0;
static int test_failed = 0;
static int test_total = 0;

// Test helper functions
static void test_assert(bool condition, const char *test_name, ...) {
    test_total++;
    if (condition) {
        printf("✅ PASS: %s\n", test_name);
        test_passed++;
    } else {
        printf("❌ FAIL: %s\n", test_name);
        test_failed++;
    }
}

static void test_led_manager_initialization(void) {
    printf("\n=== Testing LED Manager Initialization ===\n");
    
    // Test initialization with valid config
    led_mgr_config_t config = {
        .update_interval_ms = 100,
        .animation_speed_ms = 50,
        .enable_animations = true,
        .enable_transitions = true,
        .default_brightness = 100,
        .auto_dimming = false
    };
    
    hal_status_t status = led_manager_init(&config);
    test_assert(status == HAL_STATUS_OK, "LED Manager initialization with valid config");
    
    // Test initialization with NULL config
    status = led_manager_init(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "LED Manager initialization with NULL config");
    
    // Test double initialization
    status = led_manager_init(&config);
    test_assert(status == HAL_STATUS_OK, "LED Manager double initialization");
}

static void test_led_manager_configuration(void) {
    printf("\n=== Testing LED Manager Configuration ===\n");
    
    // Test get configuration
    led_mgr_config_t config;
    hal_status_t status = led_manager_get_config(&config);
    test_assert(status == HAL_STATUS_OK, "Get LED configuration");
    test_assert(config.update_interval_ms == 100, "Update interval configuration");
    test_assert(config.default_brightness == 100, "Default brightness configuration");
    
    // Test set configuration
    led_mgr_config_t new_config = {
        .update_interval_ms = 200,
        .animation_speed_ms = 100,
        .enable_animations = false,
        .enable_transitions = false,
        .default_brightness = 80,
        .auto_dimming = true
    };
    
    status = led_manager_set_config(&new_config);
    test_assert(status == HAL_STATUS_OK, "Set LED configuration");
    
    // Verify configuration was set
    led_mgr_config_t verify_config;
    status = led_manager_get_config(&verify_config);
    test_assert(status == HAL_STATUS_OK, "Verify LED configuration");
    test_assert(verify_config.update_interval_ms == 200, "Updated interval configuration");
    test_assert(verify_config.default_brightness == 80, "Updated brightness configuration");
    
    // Test set configuration with NULL
    status = led_manager_set_config(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Set LED configuration with NULL");
}

static void test_led_manager_brightness_control(void) {
    printf("\n=== Testing LED Manager Brightness Control ===\n");
    
    // Test set brightness
    hal_status_t status = led_manager_set_brightness(50);
    test_assert(status == HAL_STATUS_OK, "Set LED brightness to 50%");
    
    // Test get brightness
    uint8_t brightness;
    status = led_manager_get_brightness(&brightness);
    test_assert(status == HAL_STATUS_OK, "Get LED brightness");
    test_assert(brightness == 50, "Brightness value verification");
    
    // Test set brightness with overflow
    status = led_manager_set_brightness(150);
    test_assert(status == HAL_STATUS_OK, "Set LED brightness with overflow");
    
    status = led_manager_get_brightness(&brightness);
    test_assert(status == HAL_STATUS_OK, "Get LED brightness after overflow");
    test_assert(brightness == 100, "Brightness clamped to 100%");
    
    // Test get brightness with NULL
    status = led_manager_get_brightness(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Get LED brightness with NULL");
}

static void test_led_manager_pattern_management(void) {
    printf("\n=== Testing LED Manager Pattern Management ===\n");
    
    // Test set pattern
    led_mgr_pattern_t pattern = {
        .type = LED_MGR_PATTERN_TYPE_SYSTEM_STATE,
        .animation = LED_MGR_ANIMATION_BLINK_SLOW,
        .priority = LED_MGR_PRIORITY_NORMAL,
        .duration_ms = 1000,
        .blink_period_ms = 500,
        .fade_period_ms = 0,
        .brightness = 75,
        .power_led_enabled = true,
        .system_led_enabled = true,
        .comm_led_enabled = false,
        .network_led_enabled = false,
        .error_led_enabled = false
    };
    
    hal_status_t status = led_manager_set_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Set LED pattern");
    
    // Test get pattern
    led_mgr_pattern_t get_pattern;
    status = led_manager_get_pattern(&get_pattern);
    test_assert(status == HAL_STATUS_OK, "Get LED pattern");
    test_assert(get_pattern.type == LED_MGR_PATTERN_TYPE_SYSTEM_STATE, "Pattern type verification");
    test_assert(get_pattern.animation == LED_MGR_ANIMATION_BLINK_SLOW, "Pattern animation verification");
    test_assert(get_pattern.brightness == 75, "Pattern brightness verification");
    
    // Test set pattern with NULL
    status = led_manager_set_pattern(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Set LED pattern with NULL");
    
    // Test get pattern with NULL
    status = led_manager_get_pattern(NULL);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Get LED pattern with NULL");
    
    // Test clear pattern
    status = led_manager_clear_pattern();
    test_assert(status == HAL_STATUS_OK, "Clear LED pattern");
    
    // Verify pattern is cleared
    status = led_manager_get_pattern(&get_pattern);
    test_assert(status == HAL_STATUS_OK, "Get LED pattern after clear");
    test_assert(get_pattern.type == LED_MGR_PATTERN_TYPE_NONE, "Pattern cleared verification");
}

static void test_led_manager_system_state_patterns(void) {
    printf("\n=== Testing LED Manager System State Patterns ===\n");
    
    // Test all system state patterns
    system_state_t states[] = {
        SYSTEM_STATE_INIT,
        SYSTEM_STATE_IDLE,
        SYSTEM_STATE_MOVE,
        SYSTEM_STATE_DOCK,
        SYSTEM_STATE_FAULT,
        SYSTEM_STATE_ESTOP,
        SYSTEM_STATE_SHUTDOWN
    };
    
    for (size_t i = 0; i < sizeof(states) / sizeof(states[0]); i++) {
        hal_status_t status = led_manager_set_system_state_pattern(states[i]);
        test_assert(status == HAL_STATUS_OK, 
                   "Set system state pattern for state");
        
        // Verify pattern was set
        led_mgr_pattern_t pattern;
        status = led_manager_get_pattern(&pattern);
        test_assert(status == HAL_STATUS_OK, "Get pattern after setting system state");
        test_assert(pattern.type == LED_MGR_PATTERN_TYPE_SYSTEM_STATE, 
                   "Pattern type is SYSTEM_STATE for state");
        
        // Small delay to see the pattern
        hal_sleep_ms(100);
    }
    
    // Test update system state
    hal_status_t status = led_manager_update_system_state(SYSTEM_STATE_IDLE);
    test_assert(status == HAL_STATUS_OK, "Update system state to IDLE");
    
    // Test update with same state (should not change)
    status = led_manager_update_system_state(SYSTEM_STATE_IDLE);
    test_assert(status == HAL_STATUS_OK, "Update system state with same state");
}

static void test_led_manager_safety_patterns(void) {
    printf("\n=== Testing LED Manager Safety Patterns ===\n");
    
    // Test all safety level patterns
    safety_level_t levels[] = {
        SAFETY_LEVEL_NORMAL,
        SAFETY_LEVEL_WARNING,
        SAFETY_LEVEL_CRITICAL,
        SAFETY_LEVEL_EMERGENCY
    };
    
    for (size_t i = 0; i < sizeof(levels) / sizeof(levels[0]); i++) {
        hal_status_t status = led_manager_set_safety_pattern(levels[i]);
        test_assert(status == HAL_STATUS_OK, 
                   "Set safety pattern for level");
        
        // Verify pattern was set
        led_mgr_pattern_t pattern;
        status = led_manager_get_pattern(&pattern);
        test_assert(status == HAL_STATUS_OK, "Get pattern after setting safety level");
        test_assert(pattern.type == LED_MGR_PATTERN_TYPE_SAFETY_CONDITION, 
                   "Pattern type is SAFETY_CONDITION for level");
        
        // Small delay to see the pattern
        hal_sleep_ms(100);
    }
    
    // Test update safety level
    hal_status_t status = led_manager_update_safety_level(SAFETY_LEVEL_WARNING);
    test_assert(status == HAL_STATUS_OK, "Update safety level to WARNING");
    
    // Test update with same level (should not change)
    status = led_manager_update_safety_level(SAFETY_LEVEL_WARNING);
    test_assert(status == HAL_STATUS_OK, "Update safety level with same level");
}

static void test_led_manager_communication_patterns(void) {
    printf("\n=== Testing LED Manager Communication Patterns ===\n");
    
    // Test communication connected
    hal_status_t status = led_manager_set_communication_pattern(true, false);
    test_assert(status == HAL_STATUS_OK, "Set communication pattern - connected, no error");
    
    // Verify pattern
    led_mgr_pattern_t pattern;
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get communication pattern");
    test_assert(pattern.type == LED_MGR_PATTERN_TYPE_COMMUNICATION, "Communication pattern type");
    test_assert(pattern.animation == LED_MGR_ANIMATION_SOLID, "Connected animation");
    
    // Test communication disconnected
    status = led_manager_set_communication_pattern(false, false);
    test_assert(status == HAL_STATUS_OK, "Set communication pattern - disconnected, no error");
    
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get disconnected pattern");
    test_assert(pattern.animation == LED_MGR_ANIMATION_BLINK_SLOW, "Disconnected animation");
    
    // Test communication error
    status = led_manager_set_communication_pattern(true, true);
    test_assert(status == HAL_STATUS_OK, "Set communication pattern - connected, error");
    
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get error pattern");
    test_assert(pattern.animation == LED_MGR_ANIMATION_BLINK_FAST, "Error animation");
    test_assert(pattern.error_led_enabled == true, "Error LED enabled");
    
    // Test update communication status
    status = led_manager_update_communication_status(false, false);
    test_assert(status == HAL_STATUS_OK, "Update communication status");
}

static void test_led_manager_error_patterns(void) {
    printf("\n=== Testing LED Manager Error Patterns ===\n");
    
    // Test error pattern with low severity
    hal_status_t status = led_manager_set_error_pattern(1, 3);
    test_assert(status == HAL_STATUS_OK, "Set error pattern with low severity");
    
    // Verify pattern
    led_mgr_pattern_t pattern;
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get error pattern");
    test_assert(pattern.type == LED_MGR_PATTERN_TYPE_ERROR, "Error pattern type");
    test_assert(pattern.priority == LED_MGR_PRIORITY_HIGH, "Low severity priority");
    test_assert(pattern.error_led_enabled == true, "Error LED enabled");
    
    // Test error pattern with high severity
    status = led_manager_set_error_pattern(2, 8);
    test_assert(status == HAL_STATUS_OK, "Set error pattern with high severity");
    
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get high severity error pattern");
    test_assert(pattern.priority == LED_MGR_PRIORITY_EMERGENCY, "High severity priority");
    
    // Test clear error pattern
    status = led_manager_clear_error_pattern();
    test_assert(status == HAL_STATUS_OK, "Clear error pattern");
}

static void test_led_manager_animation_control(void) {
    printf("\n=== Testing LED Manager Animation Control ===\n");
    
    // Test start animation
    hal_status_t status = led_manager_start_animation(LED_MGR_ANIMATION_BLINK_FAST, 2000);
    test_assert(status == HAL_STATUS_OK, "Start LED animation");
    
    // Verify animation was set
    led_mgr_pattern_t pattern;
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get pattern after starting animation");
    test_assert(pattern.animation == LED_MGR_ANIMATION_BLINK_FAST, "Animation type verification");
    test_assert(pattern.duration_ms == 2000, "Animation duration verification");
    
    // Test stop animation
    status = led_manager_stop_animation();
    test_assert(status == HAL_STATUS_OK, "Stop LED animation");
    
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get pattern after stopping animation");
    test_assert(pattern.animation == LED_MGR_ANIMATION_SOLID, "Animation stopped to solid");
    test_assert(pattern.duration_ms == 0, "Animation duration cleared");
}

static void test_led_manager_predefined_patterns(void) {
    printf("\n=== Testing LED Manager Predefined Patterns ===\n");
    
    // Test system ready pattern
    hal_status_t status = led_manager_system_ready();
    test_assert(status == HAL_STATUS_OK, "System ready pattern");
    
    led_mgr_pattern_t pattern;
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get system ready pattern");
    test_assert(pattern.type == LED_MGR_PATTERN_TYPE_SYSTEM_STATE, "System ready pattern type");
    test_assert(pattern.animation == LED_MGR_ANIMATION_SOLID, "System ready animation");
    
    hal_sleep_ms(100);
    
    // Test system error pattern
    status = led_manager_system_error();
    test_assert(status == HAL_STATUS_OK, "System error pattern");
    
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get system error pattern");
    test_assert(pattern.type == LED_MGR_PATTERN_TYPE_ERROR, "System error pattern type");
    test_assert(pattern.animation == LED_MGR_ANIMATION_BLINK_FAST, "System error animation");
    
    hal_sleep_ms(100);
    
    // Test system warning pattern
    status = led_manager_system_warning();
    test_assert(status == HAL_STATUS_OK, "System warning pattern");
    
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get system warning pattern");
    test_assert(pattern.type == LED_MGR_PATTERN_TYPE_SAFETY_CONDITION, "System warning pattern type");
    test_assert(pattern.animation == LED_MGR_ANIMATION_BLINK_SLOW, "System warning animation");
    
    hal_sleep_ms(100);
    
    // Test system shutdown pattern
    status = led_manager_system_shutdown();
    test_assert(status == HAL_STATUS_OK, "System shutdown pattern");
    
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get system shutdown pattern");
    test_assert(pattern.type == LED_MGR_PATTERN_TYPE_SYSTEM_STATE, "System shutdown pattern type");
    test_assert(pattern.brightness == 50, "System shutdown brightness");
    
    hal_sleep_ms(100);
    
    // Test emergency stop pattern
    status = led_manager_emergency_stop();
    test_assert(status == HAL_STATUS_OK, "Emergency stop pattern");
    
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_OK, "Get emergency stop pattern");
    test_assert(pattern.type == LED_MGR_PATTERN_TYPE_ERROR, "Emergency stop pattern type");
    test_assert(pattern.priority == LED_MGR_PRIORITY_EMERGENCY, "Emergency stop priority");
}

static void test_led_manager_utility_functions(void) {
    printf("\n=== Testing LED Manager Utility Functions ===\n");
    
    // Test pattern type name
    const char *name = led_manager_get_pattern_type_name(LED_MGR_PATTERN_TYPE_SYSTEM_STATE);
    test_assert(strcmp(name, "SYSTEM_STATE") == 0, "Pattern type name for SYSTEM_STATE");
    
    name = led_manager_get_pattern_type_name(LED_MGR_PATTERN_TYPE_ERROR);
    test_assert(strcmp(name, "ERROR") == 0, "Pattern type name for ERROR");
    
    // Test animation name
    name = led_manager_get_animation_name(LED_MGR_ANIMATION_BLINK_SLOW);
    test_assert(strcmp(name, "BLINK_SLOW") == 0, "Animation name for BLINK_SLOW");
    
    name = led_manager_get_animation_name(LED_MGR_ANIMATION_SOLID);
    test_assert(strcmp(name, "SOLID") == 0, "Animation name for SOLID");
    
    // Test priority name
    name = led_manager_get_priority_name(LED_MGR_PRIORITY_CRITICAL);
    test_assert(strcmp(name, "CRITICAL") == 0, "Priority name for CRITICAL");
    
    name = led_manager_get_priority_name(LED_MGR_PRIORITY_EMERGENCY);
    test_assert(strcmp(name, "EMERGENCY") == 0, "Priority name for EMERGENCY");
}

static void test_led_manager_diagnostics(void) {
    printf("\n=== Testing LED Manager Diagnostics ===\n");
    
    // Set a pattern for diagnostics
    led_manager_system_ready();
    
    // Test diagnostics
    char diagnostics[1024];
    hal_status_t status = led_manager_get_diagnostics(diagnostics, sizeof(diagnostics));
    test_assert(status == HAL_STATUS_OK, "Get LED diagnostics");
    test_assert(strlen(diagnostics) > 0, "Diagnostics string not empty");
    
    printf("Diagnostics:\n%s\n", diagnostics);
    
    // Test diagnostics with NULL
    status = led_manager_get_diagnostics(NULL, 100);
    test_assert(status == HAL_STATUS_INVALID_PARAMETER, "Get diagnostics with NULL");
}

static void test_led_manager_self_test(void) {
    printf("\n=== Testing LED Manager Self Test ===\n");
    
    // Test self test
    hal_status_t status = led_manager_self_test();
    test_assert(status == HAL_STATUS_OK, "LED manager self test");
}

static void test_led_manager_reset(void) {
    printf("\n=== Testing LED Manager Reset ===\n");
    
    // Test reset
    hal_status_t status = led_manager_reset();
    test_assert(status == HAL_STATUS_OK, "LED manager reset");
    
    // Verify reset to default configuration
    led_mgr_config_t config;
    status = led_manager_get_config(&config);
    test_assert(status == HAL_STATUS_OK, "Get config after reset");
    test_assert(config.update_interval_ms == 100, "Reset update interval");
    test_assert(config.default_brightness == 100, "Reset default brightness");
    
    uint8_t brightness;
    status = led_manager_get_brightness(&brightness);
    test_assert(status == HAL_STATUS_OK, "Get brightness after reset");
    test_assert(brightness == 100, "Reset brightness");
}

static void test_led_manager_event_callback(void) {
    printf("\n=== Testing LED Manager Event Callback ===\n");
    
    // Test set callback
    hal_status_t status = led_manager_set_callback(test_led_event_callback);
    test_assert(status == HAL_STATUS_OK, "Set LED event callback");
    
    // Trigger an event by setting a pattern
    status = led_manager_system_ready();
    test_assert(status == HAL_STATUS_OK, "Trigger event with system ready");
    
    // Test set callback with NULL
    status = led_manager_set_callback(NULL);
    test_assert(status == HAL_STATUS_OK, "Set LED event callback to NULL");
}

static void test_led_manager_status(void) {
    printf("\n=== Testing LED Manager Status ===\n");
    
    // Test get status
    led_mgr_status_t status;
    hal_status_t hal_status = led_manager_get_status(&status);
    test_assert(hal_status == HAL_STATUS_OK, "Get LED status");
    test_assert(status.pattern_active == true, "Pattern active status");
    test_assert(status.pattern_count > 0, "Pattern count > 0");
    
    // Test get status with NULL
    hal_status = led_manager_get_status(NULL);
    test_assert(hal_status == HAL_STATUS_INVALID_PARAMETER, "Get status with NULL");
}

static void test_led_manager_update(void) {
    printf("\n=== Testing LED Manager Update ===\n");
    
    // Test update function
    hal_status_t status = led_manager_update();
    test_assert(status == HAL_STATUS_OK, "LED manager update");
    
    // Test multiple updates
    for (int i = 0; i < 5; i++) {
        status = led_manager_update();
        test_assert(status == HAL_STATUS_OK, "LED manager update iteration");
        hal_sleep_ms(10);
    }
}

static void test_led_manager_deinitialization(void) {
    printf("\n=== Testing LED Manager Deinitialization ===\n");
    
    // Test deinitialization
    hal_status_t status = led_manager_deinit();
    test_assert(status == HAL_STATUS_OK, "LED manager deinitialization");
    
    // Test operations after deinitialization
    led_mgr_pattern_t pattern;
    status = led_manager_get_pattern(&pattern);
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Get pattern after deinit");
    
    status = led_manager_set_brightness(50);
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Set brightness after deinit");
    
    status = led_manager_update();
    test_assert(status == HAL_STATUS_NOT_INITIALIZED, "Update after deinit");
}

static void print_test_summary(void) {
    printf("\n=== LED Manager Test Summary ===\n");
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
    printf("🚀 Starting LED Manager Tests...\n");
    printf("================================\n");
    
    // Initialize test counters
    test_passed = 0;
    test_failed = 0;
    test_total = 0;
    
    // Run all tests
    test_led_manager_initialization();
    test_led_manager_configuration();
    test_led_manager_brightness_control();
    test_led_manager_pattern_management();
    test_led_manager_system_state_patterns();
    test_led_manager_safety_patterns();
    test_led_manager_communication_patterns();
    test_led_manager_error_patterns();
    test_led_manager_animation_control();
    test_led_manager_predefined_patterns();
    test_led_manager_utility_functions();
    test_led_manager_diagnostics();
    test_led_manager_self_test();
    test_led_manager_reset();
    test_led_manager_event_callback();
    test_led_manager_status();
    test_led_manager_update();
    test_led_manager_deinitialization();
    
    // Print summary
    print_test_summary();
    
    return (test_failed == 0) ? 0 : 1;
}
