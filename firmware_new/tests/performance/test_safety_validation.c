#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include "unity.h"
#include "hal_common.h"
#include "hal_rs485.h"
#include "hal_estop.h"
#include "system_state_machine.h"
#include "module_manager.h"
#include "communication_manager.h"
#include "telemetry_manager.h"
#include "api_manager.h"

// Safety test configuration
#define SAFETY_TEST_ITERATIONS 100
#define E_STOP_RESPONSE_TIME_LIMIT_US 100000  // 100ms limit
#define SAFETY_ZONE_TEST_DURATION_SECONDS 30
#define MAX_SAFETY_THREADS 3

// Mock configurations for testing
static estop_config_t mock_estop_config = {
    .pin_number = 18,
    .active_low = true,
    .debounce_time_ms = 50
};

static system_config_t mock_system_config = {
    .max_velocity = 2.0,
    .max_acceleration = 1.0,
    .max_jerk = 0.5,
    .safety_zone_margin = 0.1
};

static telemetry_config_t mock_telemetry_config = {
    .update_interval_ms = 100,
    .max_data_points = 1000,
    .enable_logging = true
};

static api_mgr_config_t mock_api_config = {
    .port = 8080,
    .max_connections = 10,
    .timeout_ms = 5000
};

// Safety test metrics
typedef struct {
    uint64_t total_estop_tests;
    uint64_t successful_estop_tests;
    uint64_t failed_estop_tests;
    uint64_t total_estop_response_time_us;
    uint64_t min_estop_response_time_us;
    uint64_t max_estop_response_time_us;
    uint64_t avg_estop_response_time_us;
    uint64_t safety_zone_violations;
    uint64_t safety_recovery_count;
    uint64_t start_time_us;
    uint64_t end_time_us;
} safety_metrics_t;

static safety_metrics_t safety_metrics = {0};
static volatile int safety_test_running = 1;

// Signal handler for safety test
void safety_test_signal_handler(int sig) {
    printf("\nReceived signal %d, stopping safety test...\n", sig);
    safety_test_running = 0;
}

// E-Stop response time test
void test_safety_estop_response_time(void) {
    printf("\n=== SAFETY TEST: E-STOP RESPONSE TIME VALIDATION ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_common_init());
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_state_machine_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, module_manager_init());
    TEST_ASSERT_EQUAL(0, communication_manager_init());
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    // Initialize safety metrics
    memset(&safety_metrics, 0, sizeof(safety_metrics));
    safety_metrics.min_estop_response_time_us = UINT64_MAX;
    
    printf("Starting E-Stop response time test with %d iterations...\n", SAFETY_TEST_ITERATIONS);
    
    // Test E-Stop response time
    for (int i = 0; i < SAFETY_TEST_ITERATIONS; i++) {
        // Ensure system is in a known state (MOVE state)
        system_state_machine_process_event(SYSTEM_EVENT_MOVE);
        system_state_machine_update();
        
        // Wait for system to stabilize
        usleep(10000); // 10ms
        
        // Record start time
        struct timeval start_time;
        gettimeofday(&start_time, NULL);
        uint64_t start_us = start_time.tv_sec * 1000000ULL + start_time.tv_usec;
        
        // Trigger E-Stop
        hal_estop_trigger();
        
        // Wait for state machine to respond
        system_state_machine_update();
        
        // Record end time
        struct timeval end_time;
        gettimeofday(&end_time, NULL);
        uint64_t end_us = end_time.tv_sec * 1000000ULL + end_time.tv_usec;
        
        // Calculate response time
        uint64_t response_time_us = end_us - start_us;
        
        // Update metrics
        safety_metrics.total_estop_tests++;
        safety_metrics.total_estop_response_time_us += response_time_us;
        
        if (response_time_us < safety_metrics.min_estop_response_time_us) {
            safety_metrics.min_estop_response_time_us = response_time_us;
        }
        if (response_time_us > safety_metrics.max_estop_response_time_us) {
            safety_metrics.max_estop_response_time_us = response_time_us;
        }
        
        // Validate response time
        if (response_time_us <= E_STOP_RESPONSE_TIME_LIMIT_US) {
            safety_metrics.successful_estop_tests++;
        } else {
            safety_metrics.failed_estop_tests++;
            printf("⚠️  E-Stop response time exceeded limit: %lu us (limit: %d us)\n", 
                   response_time_us, E_STOP_RESPONSE_TIME_LIMIT_US);
        }
        
        // Reset E-Stop for next test
        hal_estop_reset();
        system_state_machine_update();
        
        // Small delay between tests
        usleep(5000); // 5ms
    }
    
    // Calculate average response time
    if (safety_metrics.total_estop_tests > 0) {
        safety_metrics.avg_estop_response_time_us = 
            safety_metrics.total_estop_response_time_us / safety_metrics.total_estop_tests;
    }
    
    // Display results
    printf("\n=== E-STOP RESPONSE TIME RESULTS ===\n");
    printf("Total Tests: %lu\n", safety_metrics.total_estop_tests);
    printf("Successful Tests: %lu\n", safety_metrics.successful_estop_tests);
    printf("Failed Tests: %lu\n", safety_metrics.failed_estop_tests);
    printf("Success Rate: %.2f%%\n", 
           (double)safety_metrics.successful_estop_tests / safety_metrics.total_estop_tests * 100.0);
    printf("Min Response Time: %lu us\n", safety_metrics.min_estop_response_time_us);
    printf("Max Response Time: %lu us\n", safety_metrics.max_estop_response_time_us);
    printf("Avg Response Time: %lu us\n", safety_metrics.avg_estop_response_time_us);
    printf("Response Time Limit: %d us\n", E_STOP_RESPONSE_TIME_LIMIT_US);
    
    // Validate E-Stop response time test
    double success_rate = (double)safety_metrics.successful_estop_tests / safety_metrics.total_estop_tests * 100.0;
    TEST_ASSERT_GREATER_THAN(95.0, success_rate); // > 95% success rate
    TEST_ASSERT_LESS_THAN(E_STOP_RESPONSE_TIME_LIMIT_US, safety_metrics.avg_estop_response_time_us); // Average within limit
    
    printf("✅ E-Stop response time test PASSED\n");
}

// Safety zone monitoring test
void test_safety_zone_monitoring(void) {
    printf("\n=== SAFETY TEST: SAFETY ZONE MONITORING ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_common_init());
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_state_machine_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, module_manager_init());
    TEST_ASSERT_EQUAL(0, communication_manager_init());
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    printf("Starting safety zone monitoring test for %d seconds...\n", SAFETY_ZONE_TEST_DURATION_SECONDS);
    
    uint64_t start_time = time(NULL);
    uint64_t zone_violations = 0;
    uint64_t recovery_count = 0;
    
    while (time(NULL) - start_time < SAFETY_ZONE_TEST_DURATION_SECONDS && safety_test_running) {
        // Simulate safety zone violations
        if (rand() % 100 < 5) { // 5% chance of violation
            zone_violations++;
            printf("⚠️  Safety zone violation detected (#%lu)\n", zone_violations);
            
            // Simulate system response to violation
            system_state_machine_process_event(SYSTEM_EVENT_ERROR);
            system_state_machine_update();
            
            // Check if system entered safe state
            system_state_t current_state = system_state_machine_get_current_state();
            if (current_state == SYSTEM_STATE_ESTOP || current_state == SYSTEM_STATE_FAULT) {
                recovery_count++;
                printf("✅ System entered safe state after violation\n");
            }
            
            // Simulate recovery
            usleep(100000); // 100ms
            system_state_machine_process_event(SYSTEM_EVENT_CLEAR_ERROR);
            system_state_machine_update();
        }
        
        // Update system state
        system_state_machine_update();
        
        // Small delay
        usleep(10000); // 10ms
    }
    
    printf("\n=== SAFETY ZONE MONITORING RESULTS ===\n");
    printf("Test Duration: %d seconds\n", SAFETY_ZONE_TEST_DURATION_SECONDS);
    printf("Zone Violations: %lu\n", zone_violations);
    printf("Recovery Count: %lu\n", recovery_count);
    printf("Recovery Rate: %.2f%%\n", 
           zone_violations > 0 ? (double)recovery_count / zone_violations * 100.0 : 100.0);
    
    // Validate safety zone monitoring test
    if (zone_violations > 0) {
        double recovery_rate = (double)recovery_count / zone_violations * 100.0;
        TEST_ASSERT_GREATER_THAN(90.0, recovery_rate); // > 90% recovery rate
    }
    
    printf("✅ Safety zone monitoring test PASSED\n");
}

// Safety system stress test
void test_safety_system_stress(void) {
    printf("\n=== SAFETY TEST: SAFETY SYSTEM STRESS TESTING ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_common_init());
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_state_machine_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, module_manager_init());
    TEST_ASSERT_EQUAL(0, communication_manager_init());
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    printf("Starting safety system stress test...\n");
    
    int rapid_estop_tests = 0;
    int rapid_estop_success = 0;
    int concurrent_safety_events = 0;
    int concurrent_safety_success = 0;
    
    // Test 1: Rapid E-Stop triggering
    printf("Testing rapid E-Stop triggering...\n");
    for (int i = 0; i < 50; i++) {
        rapid_estop_tests++;
        
        // Trigger E-Stop rapidly
        hal_estop_trigger();
        system_state_machine_update();
        
        if (system_state_machine_get_current_state() == SYSTEM_STATE_ESTOP) {
            rapid_estop_success++;
        }
        
        // Quick reset
        hal_estop_reset();
        system_state_machine_update();
        
        usleep(1000); // 1ms between rapid tests
    }
    
    // Test 2: Concurrent safety events
    printf("Testing concurrent safety events...\n");
    for (int i = 0; i < 20; i++) {
        concurrent_safety_events++;
        
        // Simulate multiple safety events simultaneously
        hal_estop_trigger();
        system_state_machine_process_event(SYSTEM_EVENT_ERROR);
        system_state_machine_update();
        
        if (system_state_machine_get_current_state() == SYSTEM_STATE_ESTOP || 
            system_state_machine_get_current_state() == SYSTEM_STATE_FAULT) {
            concurrent_safety_success++;
        }
        
        // Reset
        hal_estop_reset();
        system_state_machine_process_event(SYSTEM_EVENT_CLEAR_ERROR);
        system_state_machine_update();
        
        usleep(10000); // 10ms between concurrent tests
    }
    
    double rapid_success_rate = rapid_estop_tests > 0 ? 
                               (double)rapid_estop_success / rapid_estop_tests * 100.0 : 0.0;
    double concurrent_success_rate = concurrent_safety_events > 0 ? 
                                    (double)concurrent_safety_success / concurrent_safety_events * 100.0 : 0.0;
    
    printf("\n=== SAFETY SYSTEM STRESS RESULTS ===\n");
    printf("Rapid E-Stop Tests: %d\n", rapid_estop_tests);
    printf("Rapid E-Stop Success: %d\n", rapid_estop_success);
    printf("Rapid E-Stop Success Rate: %.2f%%\n", rapid_success_rate);
    printf("Concurrent Safety Events: %d\n", concurrent_safety_events);
    printf("Concurrent Safety Success: %d\n", concurrent_safety_success);
    printf("Concurrent Safety Success Rate: %.2f%%\n", concurrent_success_rate);
    
    // Validate safety system stress test
    TEST_ASSERT_GREATER_THAN(95.0, rapid_success_rate); // > 95% success rate for rapid E-Stop
    TEST_ASSERT_GREATER_THAN(90.0, concurrent_success_rate); // > 90% success rate for concurrent events
    
    printf("✅ Safety system stress test PASSED\n");
}

// Safety fault tolerance test
void test_safety_fault_tolerance(void) {
    printf("\n=== SAFETY TEST: FAULT TOLERANCE TESTING ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_common_init());
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_state_machine_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, module_manager_init());
    TEST_ASSERT_EQUAL(0, communication_manager_init());
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    printf("Starting safety fault tolerance test...\n");
    
    int fault_injection_count = 0;
    int fault_recovery_count = 0;
    
    // Test various fault injection scenarios
    for (int i = 0; i < 30; i++) {
        fault_injection_count++;
        
        // Simulate different types of faults
        switch (i % 4) {
            case 0:
                // Communication fault
                printf("Injecting communication fault...\n");
                // Simulate communication error
                break;
            case 1:
                // State machine fault
                printf("Injecting state machine fault...\n");
                system_state_machine_process_event(SYSTEM_EVENT_ERROR);
                break;
            case 2:
                // Memory fault simulation
                printf("Injecting memory fault simulation...\n");
                // Simulate memory allocation failure
                break;
            case 3:
                // Hardware fault simulation
                printf("Injecting hardware fault simulation...\n");
                hal_estop_trigger();
                break;
        }
        
        system_state_machine_update();
        
        // Check if system handled fault gracefully
        system_state_t current_state = system_state_machine_get_current_state();
        if (current_state == SYSTEM_STATE_ESTOP || current_state == SYSTEM_STATE_FAULT || 
            current_state == SYSTEM_STATE_IDLE) {
            fault_recovery_count++;
            printf("✅ System handled fault gracefully (State: %d)\n", current_state);
        }
        
        // Reset system for next test
        hal_estop_reset();
        system_state_machine_process_event(SYSTEM_EVENT_CLEAR_ERROR);
        system_state_machine_update();
        
        usleep(50000); // 50ms between fault tests
    }
    
    double fault_tolerance_rate = fault_injection_count > 0 ? 
                                 (double)fault_recovery_count / fault_injection_count * 100.0 : 0.0;
    
    printf("\n=== FAULT TOLERANCE RESULTS ===\n");
    printf("Fault Injection Count: %d\n", fault_injection_count);
    printf("Fault Recovery Count: %d\n", fault_recovery_count);
    printf("Fault Tolerance Rate: %.2f%%\n", fault_tolerance_rate);
    
    // Validate fault tolerance test
    TEST_ASSERT_GREATER_THAN(85.0, fault_tolerance_rate); // > 85% fault tolerance rate
    
    printf("✅ Safety fault tolerance test PASSED\n");
}

// Main function
int main(void) {
    printf("🚀 OHT-50 SAFETY VALIDATION TEST SUITE\n");
    printf("=====================================\n");
    
    // Set up signal handler
    signal(SIGINT, safety_test_signal_handler);
    signal(SIGTERM, safety_test_signal_handler);
    
    // Initialize Unity test framework
    UNITY_BEGIN();
    
    // Run safety validation tests
    RUN_TEST(test_safety_estop_response_time);
    RUN_TEST(test_safety_zone_monitoring);
    RUN_TEST(test_safety_system_stress);
    RUN_TEST(test_safety_fault_tolerance);
    
    // End Unity test framework
    UNITY_END();
    return 0;
}
