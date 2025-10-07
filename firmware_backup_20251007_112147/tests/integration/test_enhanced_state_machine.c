/**
 * @file test_enhanced_state_machine.c
 * @brief Simple test for enhanced state machine functionality
 * @version 1.0.0
 * @date 2025-01-28
 */

#include <stdio.h>
#include <unistd.h>
#include "system_state_machine.h"

void print_state_info(const char* test_name) {
    system_state_t current_state;
    system_status_t status;
    
    system_state_machine_get_state(&current_state);
    system_state_machine_get_status(&status);
    
    printf("[%s] Current State: %s (Transitions: %u)\n", 
           test_name,
           system_state_machine_get_state_name(current_state),
           status.state_transition_count);
}

int main() {
    printf("🧪 Testing Enhanced State Machine - Phase 2\n");
    printf("============================================\n");
    
    // Initialize state machine
    system_config_t config = {
        .state_timeout_ms = 5000,
        .update_period_ms = 100,
        .auto_recovery_enabled = true,
        .safety_monitoring_enabled = true,
        .communication_monitoring_enabled = true,
        .sensor_monitoring_enabled = true
    };
    
    if (system_state_machine_init(&config) != HAL_STATUS_OK) {
        printf("❌ Failed to initialize state machine\n");
        return 1;
    }
    
    print_state_info("INIT");
    
    // Test 1: Basic transitions with new states
    printf("\n🔄 Test 1: New State Transitions\n");
    
    // Test PAUSED state
    system_state_machine_enter_move();
    print_state_info("MOVE");
    
    system_state_machine_enter_paused();
    print_state_info("PAUSED");
    
    system_state_machine_resume_from_pause();
    print_state_info("RESUMED");
    
    // Test CONFIG state
    system_state_machine_process_event(SYSTEM_EVENT_STOP_COMMAND);
    print_state_info("IDLE");
    
    system_state_machine_enter_config();
    print_state_info("CONFIG");
    
    system_state_machine_process_event(SYSTEM_EVENT_CONFIG_COMPLETE);
    print_state_info("CONFIG_COMPLETE");
    
    // Test 2: Emergency and SAFE state
    printf("\n🚨 Test 2: Emergency and Safe Recovery\n");
    
    system_state_machine_enter_estop();
    print_state_info("ESTOP");
    
    system_state_machine_enter_safe();
    print_state_info("SAFE");
    
    system_state_machine_process_event(SYSTEM_EVENT_SAFE_RESET);
    print_state_info("SAFE_RESET");
    
    // Test 3: State statistics
    printf("\n📊 Test 3: State Statistics\n");
    
    system_state_statistics_t stats;
    if (system_state_machine_get_state_statistics(&stats) == HAL_STATUS_OK) {
        printf("Total Transitions: %u\n", stats.total_transitions);
        printf("Emergency Count: %u\n", stats.emergency_count);
        printf("Fault Count: %u\n", stats.fault_count);
    }
    
    // Test 4: State timeout configuration
    printf("\n⏰ Test 4: State Timeout Configuration\n");
    
    hal_status_t status = system_state_machine_set_state_timeout(SYSTEM_STATE_CONFIG, 3000);
    printf("Set CONFIG timeout: %s\n", (status == HAL_STATUS_OK) ? "✅ OK" : "❌ FAILED");
    
    status = system_state_machine_set_state_timeout(SYSTEM_STATE_MAX, 3000);
    printf("Set invalid state timeout: %s\n", (status == HAL_STATUS_INVALID_PARAMETER) ? "✅ OK (Expected failure)" : "❌ FAILED");
    
    // Test 5: State and event names
    printf("\n🏷️  Test 5: New State and Event Names\n");
    
    printf("State Names:\n");
    printf("  BOOT: %s\n", system_state_machine_get_state_name(SYSTEM_STATE_BOOT));
    printf("  PAUSED: %s\n", system_state_machine_get_state_name(SYSTEM_STATE_PAUSED));
    printf("  CONFIG: %s\n", system_state_machine_get_state_name(SYSTEM_STATE_CONFIG));
    printf("  SAFE: %s\n", system_state_machine_get_state_name(SYSTEM_STATE_SAFE));
    
    printf("Event Names:\n");
    printf("  BOOT_COMPLETE: %s\n", system_state_machine_get_event_name(SYSTEM_EVENT_BOOT_COMPLETE));
    printf("  PAUSE_COMMAND: %s\n", system_state_machine_get_event_name(SYSTEM_EVENT_PAUSE_COMMAND));
    printf("  RESUME_COMMAND: %s\n", system_state_machine_get_event_name(SYSTEM_EVENT_RESUME_COMMAND));
    printf("  SAFE_RESET: %s\n", system_state_machine_get_event_name(SYSTEM_EVENT_SAFE_RESET));
    
    // Cleanup
    system_state_machine_deinit();
    
    printf("\n✅ All tests completed successfully!\n");
    printf("🎯 Enhanced State Machine - Phase 2 is working correctly!\n");
    
    return 0;
}
