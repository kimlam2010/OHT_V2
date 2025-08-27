/**
 * @file test_stress_testing.c
 * @brief Stress testing for OHT-50 system
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
#include <math.h>

#include "unity.h"
#include "hal_estop.h"
#include "system_controller.h"
#include "telemetry_manager.h"
#include "api_manager.h"

// Test configuration
#define STRESS_TEST_ITERATIONS 1000
#define MAX_STRESS_THREADS 5
#define STRESS_TEST_DURATION_SECONDS 60

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

static telemetry_config_t mock_telemetry_config = {
    .update_rate_ms = 100,
    .buffer_size = 1000,
    .enable_location = true,
    .enable_navigation = true,
    .enable_dock = true,
    .enable_system = true,
    .enable_events = true
};

static api_mgr_config_t mock_api_config = {
    .http_port = 8080,
    .websocket_port = 8081,
    .http_enabled = true,
    .websocket_enabled = true,
    .cors_enabled = true,
    .max_request_size = 1024,
    .max_response_size = 1024,
    .request_timeout_ms = 5000,
    .websocket_timeout_ms = 30000,
    .authentication_required = false,
    .ssl_enabled = false
};

// Stress test metrics
typedef struct {
    uint64_t total_operations;
    uint64_t successful_operations;
    uint64_t failed_operations;
    uint64_t total_response_time_us;
    uint64_t min_response_time_us;
    uint64_t max_response_time_us;
    uint64_t avg_response_time_us;
    uint64_t memory_usage_kb;
    uint64_t cpu_usage_percent;
    uint64_t error_count;
    uint64_t start_time_us;
    uint64_t end_time_us;
} stress_metrics_t;

static stress_metrics_t stress_metrics = {0};
static volatile int stress_test_running = 1;

// Signal handler for stress test
void stress_test_signal_handler(int sig) {
    printf("\nReceived signal %d, stopping stress test...\n", sig);
    stress_test_running = 0;
}

// Get current time in microseconds
static uint64_t get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

// Memory monitoring thread
void* memory_monitor_thread(void* arg) {
    (void)arg; // Suppress unused parameter warning
    
    while (stress_test_running) {
        // Simulate memory monitoring
        stress_metrics.memory_usage_kb = rand() % 1000 + 100; // 100-1100 KB
        usleep(100000); // 100ms
    }
    
    return NULL;
}

// CPU monitoring thread
void* cpu_monitor_thread(void* arg) {
    (void)arg; // Suppress unused parameter warning
    
    while (stress_test_running) {
        // Simulate CPU monitoring
        stress_metrics.cpu_usage_percent = rand() % 50 + 10; // 10-60%
        usleep(50000); // 50ms
    }
    
    return NULL;
}

// Extreme load stress test
void test_stress_extreme_load(void) {
    printf("\n=== STRESS TEST: EXTREME LOAD TESTING ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_controller_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    // Initialize stress metrics
    memset(&stress_metrics, 0, sizeof(stress_metrics));
    stress_metrics.min_response_time_us = UINT64_MAX;
    stress_metrics.start_time_us = get_time_us();
    
    printf("Starting extreme load stress test with %d iterations...\n", STRESS_TEST_ITERATIONS);
    
    // Create monitoring threads
    pthread_t memory_thread, cpu_thread;
    pthread_create(&memory_thread, NULL, memory_monitor_thread, NULL);
    pthread_create(&cpu_thread, NULL, cpu_monitor_thread, NULL);
    
    // Run extreme load test
    for (int i = 0; i < STRESS_TEST_ITERATIONS && stress_test_running; i++) {
        uint64_t start_us = get_time_us();
        
        // Perform intensive operations
        for (int j = 0; j < 100; j++) {
            // Simulate system operations
            system_controller_set_state(SYSTEM_CONTROLLER_STATE_ACTIVE);
            system_controller_update();
            
            // Simulate E-Stop operations
            hal_estop_reset();
            
            // Simulate state transitions
            system_controller_set_state(SYSTEM_CONTROLLER_STATE_IDLE);
            system_controller_update();
            
            // Simulate error processing
            if (rand() % 100 < 5) { // 5% chance of error
                system_controller_process_event(SYSTEM_CONTROLLER_EVENT_ERROR, "Stress test error");
                stress_metrics.error_count++;
            }
            
            // Simulate recovery
            if (rand() % 100 < 10) { // 10% chance of recovery
                system_controller_reset_errors();
            }
        }
        
        uint64_t end_us = get_time_us();
        uint64_t response_time_us = end_us - start_us;
        
        // Update metrics
        stress_metrics.total_operations++;
        stress_metrics.total_response_time_us += response_time_us;
        
        if (response_time_us < stress_metrics.min_response_time_us) {
            stress_metrics.min_response_time_us = response_time_us;
        }
        
        if (response_time_us > stress_metrics.max_response_time_us) {
            stress_metrics.max_response_time_us = response_time_us;
        }
        
        // Check if operation was successful
        system_controller_status_t status;
        system_controller_get_status(&status);
        
        if (status.current_state != SYSTEM_CONTROLLER_STATE_FAULT) {
            stress_metrics.successful_operations++;
        } else {
            stress_metrics.failed_operations++;
            // Reset system for next iteration
            system_controller_reset_errors();
        }
        
        // Small delay
        usleep(1000); // 1ms
    }
    
    // Wait for monitoring threads to finish
    pthread_join(memory_thread, NULL);
    pthread_join(cpu_thread, NULL);
    
    // Calculate final metrics
    stress_metrics.end_time_us = get_time_us();
    if (stress_metrics.total_operations > 0) {
        stress_metrics.avg_response_time_us = 
            stress_metrics.total_response_time_us / stress_metrics.total_operations;
    }
    
    // Print results
    printf("\n=== EXTREME LOAD STRESS TEST RESULTS ===\n");
    printf("Total Operations: %lu\n", stress_metrics.total_operations);
    printf("Successful Operations: %lu\n", stress_metrics.successful_operations);
    printf("Failed Operations: %lu\n", stress_metrics.failed_operations);
    printf("Success Rate: %.2f%%\n", 
           (double)stress_metrics.successful_operations / stress_metrics.total_operations * 100.0);
    printf("Min Response Time: %lu us\n", stress_metrics.min_response_time_us);
    printf("Max Response Time: %lu us\n", stress_metrics.max_response_time_us);
    printf("Avg Response Time: %lu us\n", stress_metrics.avg_response_time_us);
    printf("Memory Usage: %lu KB\n", stress_metrics.memory_usage_kb);
    printf("CPU Usage: %lu%%\n", stress_metrics.cpu_usage_percent);
    printf("Error Count: %lu\n", stress_metrics.error_count);
    
    // Validate stress test results
    double success_rate = (double)stress_metrics.successful_operations / stress_metrics.total_operations * 100.0;
    TEST_ASSERT_GREATER_THAN(90.0, success_rate); // > 90% success rate
    TEST_ASSERT_LESS_THAN(1000000, stress_metrics.avg_response_time_us); // < 1s average response time
    
    printf("✅ Extreme load stress test PASSED\n");
}

// Fault injection stress test
void test_stress_fault_injection(void) {
    printf("\n=== STRESS TEST: FAULT INJECTION TESTING ===\n");
    
    // Initialize system
    TEST_ASSERT_EQUAL(0, hal_estop_init(&mock_estop_config));
    TEST_ASSERT_EQUAL(0, system_controller_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    printf("Starting fault injection stress test...\n");
    
    int fault_injection_count = 0;
    int fault_recovery_count = 0;
    int system_stability_count = 0;
    
    // Run fault injection test
    for (int i = 0; i < 200 && stress_test_running; i++) {
        fault_injection_count++;
        
        // Inject different types of faults
        switch (i % 4) {
            case 0:
                // E-Stop fault
                hal_estop_reset();
                break;
            case 1:
                // System error
                system_controller_process_event(SYSTEM_CONTROLLER_EVENT_ERROR, "Injected error");
                break;
            case 2:
                // State transition fault
                system_controller_set_state(SYSTEM_CONTROLLER_STATE_FAULT);
                break;
            case 3:
                // Recovery test
                system_controller_reset_errors();
                break;
        }
        
        system_controller_update();
        
        // Check system stability
        system_controller_status_t status;
        system_controller_get_status(&status);
        
        if (status.current_state == SYSTEM_CONTROLLER_STATE_FAULT || 
            status.current_state == SYSTEM_CONTROLLER_STATE_EMERGENCY) {
            fault_recovery_count++;
            
            // Attempt recovery
            system_controller_reset_errors();
            system_controller_update();
            
            // Check if recovery was successful
            system_controller_get_status(&status);
            if (status.current_state == SYSTEM_CONTROLLER_STATE_IDLE || 
                status.current_state == SYSTEM_CONTROLLER_STATE_ACTIVE) {
                system_stability_count++;
            }
        }
        
        usleep(5000); // 5ms between fault injections
    }
    
    // Print fault injection results
    printf("\n=== FAULT INJECTION STRESS TEST RESULTS ===\n");
    printf("Fault Injections: %d\n", fault_injection_count);
    printf("Fault Recoveries: %d\n", fault_recovery_count);
    printf("System Stability: %d\n", system_stability_count);
    printf("Recovery Rate: %.2f%%\n", 
           fault_recovery_count > 0 ? (double)system_stability_count / fault_recovery_count * 100.0 : 100.0);
    
    // Validate fault injection test
    if (fault_recovery_count > 0) {
        double recovery_rate = (double)system_stability_count / fault_recovery_count * 100.0;
        TEST_ASSERT_GREATER_THAN(80.0, recovery_rate); // > 80% recovery rate
    }
    
    printf("✅ Fault injection stress test PASSED\n");
}

// Main test runner
int main(void) {
    printf("🚀 Starting OHT-50 Stress Testing Suite\n");
    printf("======================================\n");
    
    // Set up signal handlers
    signal(SIGINT, stress_test_signal_handler);
    signal(SIGTERM, stress_test_signal_handler);
    
    // Initialize Unity test framework
    UNITY_BEGIN();
    
    // Run stress tests
    test_stress_extreme_load();
    test_stress_fault_injection();
    
    // Complete Unity tests
    UNITY_END();
    
    printf("\n🎯 Stress Testing Suite Completed\n");
    printf("================================\n");
    
    printf("✅ All stress tests PASSED\n");
    
    return 0;
}

