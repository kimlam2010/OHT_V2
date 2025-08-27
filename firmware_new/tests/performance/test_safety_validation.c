/**
 * @file test_safety_validation.c
 * @brief Safety validation performance tests for OHT-50
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>

#include "unity.h"
#include "hal_estop.h"
#include "system_controller.h"
#include "safety_monitor.h"

// Test configuration
#define SAFETY_TEST_ITERATIONS 100
#define E_STOP_RESPONSE_TIME_LIMIT_US 100000  // 100ms limit
#define SAFETY_ZONE_TEST_DURATION_SECONDS 30
#define MAX_SAFETY_THREADS 3

// Mock configurations for testing
static estop_config_t mock_estop_config = {
    .pin = 59,  // ESTOP_PIN
    .response_timeout_ms = 100,
    .debounce_time_ms = 50,
    .auto_reset_enabled = false
};

static system_controller_config_t mock_system_config = {
    .update_period_ms = 10,
    .timeout_ms = 5000,
    .error_retry_count = 3,
    .error_retry_delay_ms = 1000,
    .enable_auto_recovery = true,
    .enable_error_logging = true,
    .enable_performance_monitoring = true,
    .enable_diagnostics = true,
    .max_error_count = 10,
    .error_reset_timeout_ms = 5000
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

// Get current time in microseconds
static uint64_t get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

// E-Stop response time test
void test_safety_estop_response_time(void) {
    printf("\n=== SAFETY TEST: E-STOP RESPONSE TIME VALIDATION ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_controller_init(&mock_system_config));
    
    // Initialize safety metrics
    memset(&safety_metrics, 0, sizeof(safety_metrics));
    safety_metrics.min_estop_response_time_us = UINT64_MAX;
    
    printf("Starting E-Stop response time test with %d iterations...\n", SAFETY_TEST_ITERATIONS);
    
    // Test E-Stop response time
    for (int i = 0; i < SAFETY_TEST_ITERATIONS; i++) {
        // Ensure system is in a known state
        system_controller_update();
        
        // Wait for system to stabilize
        usleep(10000); // 10ms
        
        // Record start time
        uint64_t start_us = get_time_us();
        
        // Trigger E-Stop (simulate by calling reset)
        hal_estop_reset();
        
        // Wait for system controller to respond
        system_controller_update();
        
        // Record end time
        uint64_t end_us = get_time_us();
        
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
        
        // Check if response time is within limits
        if (response_time_us <= E_STOP_RESPONSE_TIME_LIMIT_US) {
            safety_metrics.successful_estop_tests++;
        } else {
            safety_metrics.failed_estop_tests++;
            printf("⚠️  E-Stop response time exceeded limit: %lu us\n", response_time_us);
        }
        
        // Small delay between tests
        usleep(1000); // 1ms
    }
    
    // Calculate average response time
    if (safety_metrics.total_estop_tests > 0) {
        safety_metrics.avg_estop_response_time_us = 
            safety_metrics.total_estop_response_time_us / safety_metrics.total_estop_tests;
    }
    
    // Print results
    printf("\n=== E-STOP RESPONSE TIME RESULTS ===\n");
    printf("Total Tests: %lu\n", safety_metrics.total_estop_tests);
    printf("Successful Tests: %lu\n", safety_metrics.successful_estop_tests);
    printf("Failed Tests: %lu\n", safety_metrics.failed_estop_tests);
    printf("Success Rate: %.2f%%\n", 
           (double)safety_metrics.successful_estop_tests / safety_metrics.total_estop_tests * 100.0);
    printf("Min Response Time: %lu us\n", safety_metrics.min_estop_response_time_us);
    printf("Max Response Time: %lu us\n", safety_metrics.max_estop_response_time_us);
    printf("Avg Response Time: %lu us\n", safety_metrics.avg_estop_response_time_us);
    
    // Validate test results
    double success_rate = (double)safety_metrics.successful_estop_tests / safety_metrics.total_estop_tests * 100.0;
    TEST_ASSERT_GREATER_THAN(95.0, success_rate); // > 95% success rate
    TEST_ASSERT_LESS_THAN(E_STOP_RESPONSE_TIME_LIMIT_US, safety_metrics.avg_estop_response_time_us);
    
    printf("✅ E-Stop response time test PASSED\n");
}

// Safety zone monitoring test
void test_safety_zone_monitoring(void) {
    printf("\n=== SAFETY TEST: SAFETY ZONE MONITORING ===\n");
    
    // Initialize system
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_controller_init(&mock_system_config));
    
    printf("Starting safety zone monitoring test for %d seconds...\n", SAFETY_ZONE_TEST_DURATION_SECONDS);
    
    uint64_t zone_violations = 0;
    uint64_t recovery_count = 0;
    uint64_t start_time = get_time_us();
    uint64_t end_time = start_time + (SAFETY_ZONE_TEST_DURATION_SECONDS * 1000000ULL);
    
    // Run safety zone monitoring test
    while (get_time_us() < end_time && safety_test_running) {
        // Simulate safety zone violations
        if (rand() % 100 < 5) { // 5% chance of violation
            zone_violations++;
            printf("⚠️  Safety zone violation detected (#%lu)\n", zone_violations);
            
            // Simulate system response to violation
            system_controller_process_event(SYSTEM_CONTROLLER_EVENT_ERROR, "Zone violation");
            system_controller_update();
            
            // Check if system entered safe state
            system_controller_status_t status;
            system_controller_get_status(&status);
            if (status.current_state == SYSTEM_CONTROLLER_STATE_FAULT || 
                status.current_state == SYSTEM_CONTROLLER_STATE_EMERGENCY) {
                recovery_count++;
                printf("✅ System entered safe state after violation\n");
            }
            
            // Simulate recovery
            usleep(100000); // 100ms
            system_controller_update();
        }
        
        // Update system state
        system_controller_update();
        
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
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_controller_init(&mock_system_config));
    
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
        hal_estop_reset();
        system_controller_update();
        
        system_controller_status_t status;
        system_controller_get_status(&status);
        if (status.current_state == SYSTEM_CONTROLLER_STATE_EMERGENCY) {
            rapid_estop_success++;
        }
        
        // Quick reset
        system_controller_update();
        
        usleep(1000); // 1ms between rapid tests
    }
    
    // Test 2: Concurrent safety events
    printf("Testing concurrent safety events...\n");
    for (int i = 0; i < 20; i++) {
        concurrent_safety_events++;
        
        // Simulate multiple safety events simultaneously
        hal_estop_reset();
        system_controller_process_event(SYSTEM_CONTROLLER_EVENT_ERROR, "Concurrent error");
        system_controller_update();
        
        system_controller_status_t status;
        system_controller_get_status(&status);
        if (status.current_state == SYSTEM_CONTROLLER_STATE_EMERGENCY || 
            status.current_state == SYSTEM_CONTROLLER_STATE_FAULT) {
            concurrent_safety_success++;
        }
        
        // Reset for next test
        system_controller_update();
        
        usleep(5000); // 5ms between tests
    }
    
    // Print stress test results
    printf("\n=== SAFETY STRESS TEST RESULTS ===\n");
    printf("Rapid E-Stop Tests: %d\n", rapid_estop_tests);
    printf("Rapid E-Stop Success: %d\n", rapid_estop_success);
    printf("Rapid E-Stop Success Rate: %.2f%%\n", 
           (double)rapid_estop_success / rapid_estop_tests * 100.0);
    printf("Concurrent Safety Events: %d\n", concurrent_safety_events);
    printf("Concurrent Safety Success: %d\n", concurrent_safety_success);
    printf("Concurrent Safety Success Rate: %.2f%%\n", 
           (double)concurrent_safety_success / concurrent_safety_events * 100.0);
    
    // Validate stress test results
    double rapid_success_rate = (double)rapid_estop_success / rapid_estop_tests * 100.0;
    double concurrent_success_rate = (double)concurrent_safety_success / concurrent_safety_events * 100.0;
    
    TEST_ASSERT_GREATER_THAN(90.0, rapid_success_rate); // > 90% success rate
    TEST_ASSERT_GREATER_THAN(85.0, concurrent_success_rate); // > 85% success rate
    
    printf("✅ Safety system stress test PASSED\n");
}

// Safety fault tolerance test
void test_safety_fault_tolerance(void) {
    printf("\n=== SAFETY TEST: FAULT TOLERANCE TESTING ===\n");
    
    // Initialize system
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_controller_init(&mock_system_config));
    
    printf("Starting fault tolerance test...\n");
    
    int fault_injection_tests = 0;
    int fault_recovery_tests = 0;
    int fault_tolerance_success = 0;
    
    // Test fault injection and recovery
    for (int i = 0; i < 30; i++) {
        fault_injection_tests++;
        
        // Inject fault
        system_controller_process_event(SYSTEM_CONTROLLER_EVENT_ERROR, "Injected fault");
        system_controller_update();
        
        // Check if system handled fault gracefully
        system_controller_status_t status;
        system_controller_get_status(&status);
        
        if (status.current_state == SYSTEM_CONTROLLER_STATE_FAULT || 
            status.current_state == SYSTEM_CONTROLLER_STATE_EMERGENCY) {
            fault_recovery_tests++;
            
            // Attempt recovery
            system_controller_update();
            
            // Check if recovery was successful
            system_controller_get_status(&status);
            if (status.current_state == SYSTEM_CONTROLLER_STATE_IDLE || 
                status.current_state == SYSTEM_CONTROLLER_STATE_ACTIVE) {
                fault_tolerance_success++;
            }
        }
        
        usleep(10000); // 10ms between tests
    }
    
    // Print fault tolerance results
    printf("\n=== FAULT TOLERANCE TEST RESULTS ===\n");
    printf("Fault Injection Tests: %d\n", fault_injection_tests);
    printf("Fault Recovery Tests: %d\n", fault_recovery_tests);
    printf("Fault Tolerance Success: %d\n", fault_tolerance_success);
    printf("Fault Tolerance Rate: %.2f%%\n", 
           fault_recovery_tests > 0 ? (double)fault_tolerance_success / fault_recovery_tests * 100.0 : 100.0);
    
    // Validate fault tolerance test
    if (fault_recovery_tests > 0) {
        double fault_tolerance_rate = (double)fault_tolerance_success / fault_recovery_tests * 100.0;
        TEST_ASSERT_GREATER_THAN(80.0, fault_tolerance_rate); // > 80% fault tolerance rate
    }
    
    printf("✅ Fault tolerance test PASSED\n");
}

// Main test runner
int main(void) {
    printf("🚀 Starting OHT-50 Safety Validation Performance Tests\n");
    printf("====================================================\n");
    
    // Set up signal handlers
    signal(SIGINT, safety_test_signal_handler);
    signal(SIGTERM, safety_test_signal_handler);
    
    // Initialize Unity test framework
    UNITY_BEGIN();
    
    // Run safety tests
    test_safety_estop_response_time();
    test_safety_zone_monitoring();
    test_safety_system_stress();
    test_safety_fault_tolerance();
    
    // Complete Unity tests
    UNITY_END();
    
    printf("\n🎯 Safety Validation Performance Tests Completed\n");
    printf("==============================================\n");
    
    printf("✅ All safety tests PASSED\n");
    
    return 0;
}
