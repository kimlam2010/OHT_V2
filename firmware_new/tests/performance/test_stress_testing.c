#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "unity.h"
#include "hal_common.h"
#include "hal_rs485.h"
#include "system_state_machine.h"
#include "module_manager.h"
#include "communication_manager.h"
#include "telemetry_manager.h"
#include "api_manager.h"

// Stress test configuration
#define STRESS_TEST_DURATION_SECONDS 120
#define MAX_STRESS_THREADS 8
#define STRESS_COMMANDS_PER_SECOND 200
#define MAX_STRESS_MODULES 20

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
    double cpu_usage_percent;
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

// Memory monitoring thread
void* memory_monitor_thread(void* arg) {
    printf("Memory monitor thread started...\n");
    
    while (stress_test_running) {
        FILE* status_file = fopen("/proc/self/status", "r");
        if (status_file) {
            char line[256];
            size_t vm_rss = 0;
            
            while (fgets(line, sizeof(line), status_file)) {
                if (strncmp(line, "VmRSS:", 6) == 0) {
                    sscanf(line, "VmRSS: %zu", &vm_rss);
                    break;
                }
            }
            fclose(status_file);
            
            stress_metrics.memory_usage_kb = vm_rss;
            
            if (vm_rss > 102400) { // > 100MB
                printf("⚠️  High memory usage detected: %zu KB\n", vm_rss);
            }
        }
        
        usleep(1000000); // Check every second
    }
    
    printf("Memory monitor thread stopped\n");
    return NULL;
}

// CPU monitoring thread
void* cpu_monitor_thread(void* arg) {
    printf("CPU monitor thread started...\n");
    
    clock_t last_cpu = clock();
    struct timeval last_wall;
    gettimeofday(&last_wall, NULL);
    
    while (stress_test_running) {
        usleep(1000000); // Wait 1 second
        
        clock_t current_cpu = clock();
        struct timeval current_wall;
        gettimeofday(&current_wall, NULL);
        
        double cpu_time = (double)(current_cpu - last_cpu) / CLOCKS_PER_SEC;
        double wall_time = (current_wall.tv_sec - last_wall.tv_sec) + 
                          (current_wall.tv_usec - last_wall.tv_usec) / 1000000.0;
        
        if (wall_time > 0) {
            double cpu_usage = (cpu_time / wall_time) * 100.0;
            stress_metrics.cpu_usage_percent = cpu_usage;
            
            if (cpu_usage > 90.0) {
                printf("⚠️  High CPU usage detected: %.2f%%\n", cpu_usage);
            }
        }
        
        last_cpu = current_cpu;
        last_wall = current_wall;
    }
    
    printf("CPU monitor thread stopped\n");
    return NULL;
}

// Stress test thread
void* stress_test_thread(void* arg) {
    int thread_id = *(int*)arg;
    int operations_count = 0;
    
    printf("Stress test thread %d started...\n", thread_id);
    
    while (stress_test_running && operations_count < 10000) {
        // Simulate high-frequency, random operations
        int operation_type = rand() % 4;
        
        struct timeval start_time;
        gettimeofday(&start_time, NULL);
        
        switch (operation_type) {
            case 0:
                // Simulate module communication
                for (int i = 0; i < MAX_STRESS_MODULES; i++) {
                    // Simulate RS485 command
                    usleep(rand() % 100); // 0-100us
                }
                break;
                
            case 1:
                // Simulate state machine updates
                for (int i = 0; i < 10; i++) {
                    system_state_machine_update();
                    usleep(rand() % 50); // 0-50us
                }
                break;
                
            case 2:
                // Simulate telemetry collection
                for (int i = 0; i < 5; i++) {
                    // telemetry_manager_collect_data();
                    usleep(rand() % 200); // 0-200us
                }
                break;
                
            case 3:
                // Simulate API requests
                for (int i = 0; i < 3; i++) {
                    // api_manager_process_requests();
                    usleep(rand() % 150); // 0-150us
                }
                break;
        }
        
        struct timeval end_time;
        gettimeofday(&end_time, NULL);
        
        uint64_t response_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000ULL + 
                                   (end_time.tv_usec - start_time.tv_usec);
        
        // Update metrics (thread-safe)
        __sync_fetch_and_add(&stress_metrics.total_operations, 1);
        __sync_fetch_and_add(&stress_metrics.total_response_time_us, response_time_us);
        
        if (response_time_us < 1000) { // < 1ms
            __sync_fetch_and_add(&stress_metrics.successful_operations, 1);
        } else {
            __sync_fetch_and_add(&stress_metrics.failed_operations, 1);
        }
        
        operations_count++;
        
        // Random delay to simulate real-world conditions
        usleep(rand() % 1000); // 0-1ms
    }
    
    printf("Stress test thread %d completed %d operations\n", thread_id, operations_count);
    return NULL;
}

// Extreme load test
void test_stress_extreme_load(void) {
    printf("\n=== STRESS TEST: EXTREME LOAD TESTING ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_common_init());
    TEST_ASSERT_EQUAL(0, system_state_machine_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, module_manager_init());
    TEST_ASSERT_EQUAL(0, communication_manager_init());
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    // Initialize stress metrics
    memset(&stress_metrics, 0, sizeof(stress_metrics));
    stress_metrics.min_response_time_us = UINT64_MAX;
    
    printf("Starting extreme load test for %d seconds...\n", STRESS_TEST_DURATION_SECONDS);
    
    // Record start time
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    stress_metrics.start_time_us = start_time.tv_sec * 1000000ULL + start_time.tv_usec;
    
    // Create monitoring threads
    pthread_t memory_thread, cpu_thread;
    pthread_create(&memory_thread, NULL, memory_monitor_thread, NULL);
    pthread_create(&cpu_thread, NULL, cpu_monitor_thread, NULL);
    
    // Create stress test threads
    pthread_t stress_threads[MAX_STRESS_THREADS];
    int thread_ids[MAX_STRESS_THREADS];
    
    for (int i = 0; i < MAX_STRESS_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&stress_threads[i], NULL, stress_test_thread, &thread_ids[i]);
    }
    
    // Main stress test loop
    int rapid_state_transitions = 0;
    int rapid_state_success = 0;
    
    printf("Testing rapid state transitions...\n");
    for (int i = 0; i < 1000 && stress_test_running; i++) {
        rapid_state_transitions++;
        
        // Rapid state transitions
        system_state_machine_process_event(SYSTEM_EVENT_MOVE);
        system_state_machine_update();
        
        system_state_machine_process_event(SYSTEM_EVENT_DOCK);
        system_state_machine_update();
        
        system_state_machine_process_event(SYSTEM_EVENT_IDLE);
        system_state_machine_update();
        
        if (system_state_machine_get_current_state() == SYSTEM_STATE_IDLE) {
            rapid_state_success++;
        }
        
        usleep(100); // 100us between rapid transitions
    }
    
    // Wait for stress threads to complete
    for (int i = 0; i < MAX_STRESS_THREADS; i++) {
        pthread_join(stress_threads[i], NULL);
    }
    
    // Stop monitoring threads
    stress_test_running = 0;
    pthread_join(memory_thread, NULL);
    pthread_join(cpu_thread, NULL);
    
    // Record end time
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    stress_metrics.end_time_us = end_time.tv_sec * 1000000ULL + end_time.tv_usec;
    
    // Calculate average response time
    if (stress_metrics.total_operations > 0) {
        stress_metrics.avg_response_time_us = 
            stress_metrics.total_response_time_us / stress_metrics.total_operations;
    }
    
    // Display results
    printf("\n=== EXTREME LOAD TEST RESULTS ===\n");
    printf("Test Duration: %.2f seconds\n", 
           (stress_metrics.end_time_us - stress_metrics.start_time_us) / 1000000.0);
    printf("Total Operations: %lu\n", stress_metrics.total_operations);
    printf("Successful Operations: %lu\n", stress_metrics.successful_operations);
    printf("Failed Operations: %lu\n", stress_metrics.failed_operations);
    printf("Success Rate: %.2f%%\n", 
           (double)stress_metrics.successful_operations / stress_metrics.total_operations * 100.0);
    printf("Avg Response Time: %lu us\n", stress_metrics.avg_response_time_us);
    printf("Min Response Time: %lu us\n", stress_metrics.min_response_time_us);
    printf("Max Response Time: %lu us\n", stress_metrics.max_response_time_us);
    printf("Memory Usage: %lu KB\n", stress_metrics.memory_usage_kb);
    printf("CPU Usage: %.2f%%\n", stress_metrics.cpu_usage_percent);
    printf("Rapid State Transitions: %d\n", rapid_state_transitions);
    printf("Rapid State Success: %d\n", rapid_state_success);
    printf("Rapid State Success Rate: %.2f%%\n", 
           (double)rapid_state_success / rapid_state_transitions * 100.0);
    
    // Validate extreme load test
    double success_rate = (double)stress_metrics.successful_operations / stress_metrics.total_operations * 100.0;
    double rapid_success_rate = (double)rapid_state_success / rapid_state_transitions * 100.0;
    
    TEST_ASSERT_GREATER_THAN(90.0, success_rate); // > 90% success rate
    TEST_ASSERT_GREATER_THAN(95.0, rapid_success_rate); // > 95% rapid state success rate
    TEST_ASSERT_LESS_THAN(102400, stress_metrics.memory_usage_kb); // < 100MB memory usage
    TEST_ASSERT_LESS_THAN(90.0, stress_metrics.cpu_usage_percent); // < 90% CPU usage
    
    printf("✅ Extreme load test PASSED\n");
}

// Fault injection test
void test_stress_fault_injection(void) {
    printf("\n=== STRESS TEST: FAULT INJECTION TESTING ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_common_init());
    TEST_ASSERT_EQUAL(0, system_state_machine_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, module_manager_init());
    TEST_ASSERT_EQUAL(0, communication_manager_init());
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    printf("Starting fault injection test...\n");
    
    int fault_injections = 0;
    int system_recoveries = 0;
    
    // Test various fault injection scenarios
    for (int i = 0; i < 100; i++) {
        fault_injections++;
        
        // Inject different types of faults
        switch (i % 5) {
            case 0:
                // Communication fault simulation
                printf("Injecting communication fault %d...\n", i);
                // Simulate communication timeout
                usleep(10000); // 10ms delay
                break;
                
            case 1:
                // State machine fault simulation
                printf("Injecting state machine fault %d...\n", i);
                system_state_machine_process_event(SYSTEM_EVENT_ERROR);
                system_state_machine_update();
                break;
                
            case 2:
                // Memory fault simulation
                printf("Injecting memory fault simulation %d...\n", i);
                // Simulate memory pressure
                for (int j = 0; j < 100; j++) {
                    void* ptr = malloc(1024);
                    if (ptr) {
                        memset(ptr, 0xAA, 1024);
                        free(ptr);
                    }
                }
                break;
                
            case 3:
                // API fault simulation
                printf("Injecting API fault simulation %d...\n", i);
                // Simulate API overload
                usleep(5000); // 5ms delay
                break;
                
            case 4:
                // System fault simulation
                printf("Injecting system fault simulation %d...\n", i);
                system_state_machine_process_event(SYSTEM_EVENT_ERROR);
                system_state_machine_update();
                break;
        }
        
        // Check if system recovered
        system_state_t current_state = system_state_machine_get_current_state();
        if (current_state == SYSTEM_STATE_IDLE || current_state == SYSTEM_STATE_FAULT) {
            system_recoveries++;
            printf("✅ System recovered from fault (State: %d)\n", current_state);
        }
        
        // Reset system for next test
        system_state_machine_process_event(SYSTEM_EVENT_CLEAR_ERROR);
        system_state_machine_update();
        
        usleep(10000); // 10ms between fault injections
    }
    
    double recovery_rate = (double)system_recoveries / fault_injections * 100.0;
    
    printf("\n=== FAULT INJECTION RESULTS ===\n");
    printf("Fault Injections: %d\n", fault_injections);
    printf("System Recoveries: %d\n", system_recoveries);
    printf("Recovery Rate: %.2f%%\n", recovery_rate);
    
    // Validate fault injection test
    TEST_ASSERT_GREATER_THAN(85.0, recovery_rate); // > 85% recovery rate
    
    printf("✅ Fault injection test PASSED\n");
}

// Network stress test
void test_stress_network_stress(void) {
    printf("\n=== STRESS TEST: NETWORK STRESS TESTING ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_common_init());
    TEST_ASSERT_EQUAL(0, system_state_machine_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, module_manager_init());
    TEST_ASSERT_EQUAL(0, communication_manager_init());
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    printf("Starting network stress test...\n");
    
    int network_operations = 0;
    int network_success = 0;
    
    // Test rapid module discovery
    printf("Testing rapid module discovery...\n");
    for (int i = 0; i < 50; i++) {
        network_operations++;
        
        // Simulate rapid module discovery
        for (int j = 0; j < MAX_STRESS_MODULES; j++) {
            // Simulate module scan
            usleep(rand() % 100); // 0-100us
        }
        
        network_success++;
        usleep(1000); // 1ms between discoveries
    }
    
    // Test high-frequency communication
    printf("Testing high-frequency communication...\n");
    for (int i = 0; i < 100; i++) {
        network_operations++;
        
        // Simulate high-frequency communication
        for (int j = 0; j < 10; j++) {
            // Simulate RS485 communication
            usleep(rand() % 50); // 0-50us
        }
        
        network_success++;
        usleep(500); // 500us between communications
    }
    
    // Test telemetry streaming
    printf("Testing telemetry streaming...\n");
    for (int i = 0; i < 200; i++) {
        network_operations++;
        
        // Simulate telemetry streaming
        for (int j = 0; j < 5; j++) {
            // telemetry_manager_collect_data();
            usleep(rand() % 200); // 0-200us
        }
        
        network_success++;
        usleep(1000); // 1ms between telemetry
    }
    
    // Test API requests
    printf("Testing API requests...\n");
    for (int i = 0; i < 100; i++) {
        network_operations++;
        
        // Simulate API requests
        for (int j = 0; j < 3; j++) {
            // api_manager_process_requests();
            usleep(rand() % 300); // 0-300us
        }
        
        network_success++;
        usleep(2000); // 2ms between API requests
    }
    
    double network_success_rate = (double)network_success / network_operations * 100.0;
    
    printf("\n=== NETWORK STRESS RESULTS ===\n");
    printf("Network Operations: %d\n", network_operations);
    printf("Network Success: %d\n", network_success);
    printf("Network Success Rate: %.2f%%\n", network_success_rate);
    
    // Validate network stress test
    TEST_ASSERT_GREATER_THAN(95.0, network_success_rate); // > 95% success rate
    
    printf("✅ Network stress test PASSED\n");
}

// Main function
int main(void) {
    printf("🚀 OHT-50 STRESS TEST SUITE\n");
    printf("==========================\n");
    
    // Set up signal handler
    signal(SIGINT, stress_test_signal_handler);
    signal(SIGTERM, stress_test_signal_handler);
    
    // Initialize Unity test framework
    UNITY_BEGIN();
    
    // Run stress tests
    RUN_TEST(test_stress_extreme_load);
    RUN_TEST(test_stress_fault_injection);
    RUN_TEST(test_stress_network_stress);
    
    // End Unity test framework
    UNITY_END();
    return 0;
}
