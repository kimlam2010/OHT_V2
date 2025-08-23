/**
 * @file test_module_registry.c
 * @brief Test module registry functionality for Phase 1
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "module_manager.h"
#include "hal_common.h"

// Test event callback
static int test_event_count = 0;
static module_event_t last_event = MODULE_EVENT_DISCOVERED;
static module_info_t last_event_info = {0};

static void test_event_callback(module_event_t event, const module_info_t *info) {
    test_event_count++;
    last_event = event;
    if (info) {
        memcpy(&last_event_info, info, sizeof(module_info_t));
    }
    printf("[TEST] Event %d: %s addr=0x%02X\n", 
           test_event_count, 
           event == MODULE_EVENT_DISCOVERED ? "DISCOVERED" :
           event == MODULE_EVENT_ONLINE ? "ONLINE" :
           event == MODULE_EVENT_OFFLINE ? "OFFLINE" :
           event == MODULE_EVENT_UPDATED ? "UPDATED" :
           event == MODULE_EVENT_TIMEOUT ? "TIMEOUT" : "UNKNOWN",
           info ? info->address : 0xFF);
}

static void test_basic_operations(void) {
    printf("\n=== Testing Basic Operations ===\n");
    
    // Initialize registry
    assert(registry_init() == 0);
    assert(registry_count_online() == 0);
    
    // Set event callback
    registry_set_event_callback(test_event_callback);
    test_event_count = 0;
    
    // Add a module
    module_info_t mi = {0};
    mi.address = 0x02;
    mi.type = MODULE_TYPE_MOTOR;
    strcpy(mi.name, "test_motor");
    mi.status = MODULE_STATUS_ONLINE;
    mi.last_seen_ms = hal_get_timestamp_ms();
    
    assert(registry_add_or_update(&mi) == 0);
    assert(test_event_count == 1);
    assert(last_event == MODULE_EVENT_DISCOVERED);
    assert(registry_count_online() == 1);
    
    // Get the module
    module_info_t retrieved = {0};
    assert(registry_get(0x02, &retrieved) == 0);
    assert(retrieved.address == 0x02);
    assert(retrieved.type == MODULE_TYPE_MOTOR);
    assert(strcmp(retrieved.name, "test_motor") == 0);
    
    // Update the module
    strcpy(mi.name, "updated_motor");
    assert(registry_add_or_update(&mi) == 0);
    assert(test_event_count == 2);
    assert(last_event == MODULE_EVENT_UPDATED);
    
    // Mark offline
    assert(registry_mark_offline(0x02) == 0);
    assert(test_event_count == 3);
    assert(last_event == MODULE_EVENT_OFFLINE);
    assert(registry_count_online() == 0);
    
    // Mark online
    assert(registry_mark_online(0x02, MODULE_TYPE_MOTOR, "1.0") == 0);
    assert(test_event_count == 4);
    assert(last_event == MODULE_EVENT_ONLINE);
    assert(registry_count_online() == 1);
    
    printf("Basic operations test PASSED\n");
}

static void test_multiple_modules(void) {
    printf("\n=== Testing Multiple Modules ===\n");
    
    registry_clear();
    test_event_count = 0;
    
    // Add multiple modules using registry_mark_online
    // Each call triggers: DISCOVERED (from add_or_update) + ONLINE (from mark_online)
    assert(registry_mark_online(0x02, MODULE_TYPE_MOTOR, "1.0") == 0);
    assert(registry_mark_online(0x03, MODULE_TYPE_IO, "1.1") == 0);
    assert(registry_mark_online(0x04, MODULE_TYPE_DOCK, "1.2") == 0);
    
    assert(registry_count_online() == 3);
    assert(test_event_count == 6); // 3 DISCOVERED + 3 ONLINE events
    
    // Test registry_get_all
    module_info_t modules[10];
    size_t actual_count = 0;
    assert(registry_get_all(modules, 10, &actual_count) == 0);
    assert(actual_count == 3);
    
    printf("Found %zu modules:\n", actual_count);
    for (size_t i = 0; i < actual_count; i++) {
        printf("  addr=0x%02X, type=%d, name=%s, status=%d\n",
               modules[i].address, modules[i].type, modules[i].name, modules[i].status);
    }
    
    // Test has_offline_saved
    assert(registry_has_offline_saved() == false);
    
    // Mark one offline
    assert(registry_mark_offline(0x03) == 0);
    assert(registry_has_offline_saved() == true);
    assert(registry_count_online() == 2);
    
    printf("Multiple modules test PASSED\n");
}

static void test_yaml_persistence(void) {
    printf("\n=== Testing YAML Persistence ===\n");
    
    registry_clear();
    
    // Add some test modules
    assert(registry_mark_online(0x02, MODULE_TYPE_MOTOR, "1.0") == 0);
    assert(registry_set_meta(0x02, "motor_left", MODULE_TYPE_MOTOR) == 0);
    
    assert(registry_mark_online(0x03, MODULE_TYPE_IO, "1.1") == 0);
    assert(registry_set_meta(0x03, "io_safety", MODULE_TYPE_IO) == 0);
    
    // Save to test file
    const char *test_file = "test_modules.yaml";
    assert(registry_save_yaml(test_file) == 0);
    printf("Saved modules to %s\n", test_file);
    
    // Clear and reload
    registry_clear();
    assert(registry_count_online() == 0);
    
    assert(registry_load_yaml(test_file) == 0);
    assert(registry_count_online() == 2);
    
    // Verify loaded data
    module_info_t mi;
    assert(registry_get(0x02, &mi) == 0);
    assert(strcmp(mi.name, "motor_left") == 0);
    assert(mi.type == MODULE_TYPE_MOTOR);
    
    assert(registry_get(0x03, &mi) == 0);
    assert(strcmp(mi.name, "io_safety") == 0);
    assert(mi.type == MODULE_TYPE_IO);
    
    // Clean up
    remove(test_file);
    
    printf("YAML persistence test PASSED\n");
}

static void test_scan_simulation(void) {
    printf("\n=== Testing Scan Simulation ===\n");
    
    registry_clear();
    test_event_count = 0;
    
    // Simulate scanning process
    registry_set_scanning(true);
    assert(registry_is_scanning() == true);
    
    // Simulate finding modules (each triggers DISCOVERED + ONLINE)
    assert(registry_mark_online(0x02, MODULE_TYPE_MOTOR, "1.0") == 0);
    assert(registry_mark_online(0x03, MODULE_TYPE_IO, "1.1") == 0);
    assert(registry_mark_online(0x04, MODULE_TYPE_DOCK, "1.2") == 0);
    
    registry_set_scanning(false);
    assert(registry_is_scanning() == false);
    
    assert(registry_count_online() == 3);
    assert(test_event_count == 6); // 3 DISCOVERED + 3 ONLINE events
    
    printf("Scan simulation test PASSED\n");
}

int main(void) {
    printf("=== Module Registry Test Suite ===\n");
    printf("Testing Phase 1 functionality...\n");
    
    test_basic_operations();
    test_multiple_modules();
    test_yaml_persistence();
    test_scan_simulation();
    
    printf("\n=== All Tests PASSED ===\n");
    printf("Module Registry Phase 1 implementation is working correctly!\n");
    
    return 0;
}
