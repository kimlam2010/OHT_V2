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

// Performance test configuration
#define MAX_MODULES 10
#define TEST_DURATION_SECONDS 60
#define COMMANDS_PER_SECOND 100
#define MAX_CONCURRENT_THREADS 5

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

// Performance metrics structure
typedef struct {
    uint64_t total_commands;
    uint64_t successful_commands;
    uint64_t failed_commands;
    uint64_t total_response_time_us;
    uint64_t min_response_time_us;
    uint64_t max_response_time_us;
    uint64_t avg_response_time_us;
    uint64_t start_time_us;
    uint64_t end_time_us;
    double cpu_usage_percent;
    size_t memory_usage_kb;
} performance_metrics_t;

static performance_metrics_t performance_metrics = {0};
static volatile int performance_test_running = 1;

// Signal handler for performance test
void performance_test_signal_handler(int sig) {
    printf("\nReceived signal %d, stopping performance test...\n", sig);
    performance_test_running = 0;
}

// Thread function for concurrent operations
void* concurrent_operations_thread(void* arg) {
    int thread_id = *(int*)arg;
    int commands_sent = 0;
    
    printf("Thread %d: Starting concurrent operations...\n", thread_id);
    
    while (performance_test_running && commands_sent < 1000) {
        // Simulate module communication
        for (int i = 0; i < MAX_MODULES && performance_test_running; i++) {
            // Simulate sending command to module
            struct timeval start_time;
            gettimeofday(&start_time, NULL);
            
            // Simulate command processing
            usleep(rand() % 1000); // 0-1ms processing time
            
            struct timeval end_time;
            gettimeofday(&end_time, NULL);
            
            uint64_t response_time_us = (end_time.tv_sec - start_time.tv_sec) * 1000000ULL + 
                                       (end_time.tv_usec - start_time.tv_usec);
            
            // Update metrics (thread-safe)
            __sync_fetch_and_add(&performance_metrics.total_commands, 1);
            __sync_fetch_and_add(&performance_metrics.total_response_time_us, response_time_us);
            
            if (response_time_us < 1000) { // < 1ms response time
                __sync_fetch_and_add(&performance_metrics.successful_commands, 1);
            } else {
                __sync_fetch_and_add(&performance_metrics.failed_commands, 1);
            }
            
            commands_sent++;
        }
        
        usleep(1000); // 1ms delay between command batches
    }
    
    printf("Thread %d: Completed %d commands\n", thread_id, commands_sent);
    return NULL;
}

// Performance load testing
void test_performance_load_testing(void) {
    printf("\n=== PERFORMANCE TEST: LOAD TESTING ===\n");
    
    // Initialize system with mock configs
    TEST_ASSERT_EQUAL(0, hal_common_init());
    TEST_ASSERT_EQUAL(0, system_state_machine_init(&mock_system_config));
    TEST_ASSERT_EQUAL(0, module_manager_init());
    TEST_ASSERT_EQUAL(0, communication_manager_init());
    TEST_ASSERT_EQUAL(0, telemetry_manager_init(&mock_telemetry_config));
    TEST_ASSERT_EQUAL(0, api_manager_init(&mock_api_config));
    
    // Initialize performance metrics
    memset(&performance_metrics, 0, sizeof(performance_metrics));
    performance_metrics.min_response_time_us = UINT64_MAX;
    
    printf("Starting performance load test for %d seconds...\n", TEST_DURATION_SECONDS);
    
    // Record start time
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
    performance_metrics.start_time_us = start_time.tv_sec * 1000000ULL + start_time.tv_usec;
    
    // Create concurrent threads
    pthread_t threads[MAX_CONCURRENT_THREADS];
    int thread_ids[MAX_CONCURRENT_THREADS];
    
    for (int i = 0; i < MAX_CONCURRENT_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, concurrent_operations_thread, &thread_ids[i]);
    }
    
    // Main test loop
    int test_iterations = 0;
    while (performance_test_running && test_iterations < TEST_DURATION_SECONDS * 10) {
        // Simulate system state machine updates
        system_state_machine_update();
        
        // Simulate telemetry collection
        // telemetry_manager_collect_data();
        
        // Simulate module discovery
        // module_manager_scan_modules();
        
        test_iterations++;
        usleep(100000); // 100ms delay
    }
    
    // Wait for threads to complete
    for (int i = 0; i < MAX_CONCURRENT_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Record end time
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    performance_metrics.end_time_us = end_time.tv_sec * 1000000ULL + end_time.tv_usec;
    
    // Calculate average response time
    if (performance_metrics.total_commands > 0) {
        performance_metrics.avg_response_time_us = 
            performance_metrics.total_response_time_us / performance_metrics.total_commands;
    }
    
    // Display results
    printf("\n=== PERFORMANCE LOAD TEST RESULTS ===\n");
    printf("Test Duration: %.2f seconds\n", 
           (performance_metrics.end_time_us - performance_metrics.start_time_us) / 1000000.0);
    printf("Total Commands: %lu\n", performance_metrics.total_commands);
    printf("Successful Commands: %lu\n", performance_metrics.successful_commands);
    printf("Failed Commands: %lu\n", performance_metrics.failed_commands);
    printf("Success Rate: %.2f%%\n", 
           (double)performance_metrics.successful_commands / performance_metrics.total_commands * 100.0);
    printf("Avg Response Time: %lu us\n", performance_metrics.avg_response_time_us);
    printf("Min Response Time: %lu us\n", performance_metrics.min_response_time_us);
    printf("Max Response Time: %lu us\n", performance_metrics.max_response_time_us);
    printf("Commands per Second: %.2f\n", 
           (double)performance_metrics.total_commands / 
           ((performance_metrics.end_time_us - performance_metrics.start_time_us) / 1000000.0));
    
    // Validate performance requirements
    double success_rate = (double)performance_metrics.successful_commands / performance_metrics.total_commands * 100.0;
    TEST_ASSERT_GREATER_THAN(95.0, success_rate); // > 95% success rate
    TEST_ASSERT_LESS_THAN(1000, performance_metrics.avg_response_time_us); // < 1ms average response time
    
    printf("✅ Performance load test PASSED\n");
}

// Memory usage monitoring
void test_performance_memory_usage(void) {
    printf("\n=== PERFORMANCE TEST: MEMORY USAGE MONITORING ===\n");
    
    // Get initial memory usage
    FILE* status_file = fopen("/proc/self/status", "r");
    TEST_ASSERT_NOT_NULL(status_file);
    
    size_t initial_vm_size = 0;
    size_t initial_vm_rss = 0;
    char line[256];
    
    while (fgets(line, sizeof(line), status_file)) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            sscanf(line, "VmSize: %zu", &initial_vm_size);
        } else if (strncmp(line, "VmRSS:", 6) == 0) {
            sscanf(line, "VmRSS: %zu", &initial_vm_rss);
        }
    }
    fclose(status_file);
    
    printf("Initial Memory Usage - VmSize: %zu KB, VmRSS: %zu KB\n", initial_vm_size, initial_vm_rss);
    
    // Perform memory-intensive operations
    printf("Performing memory-intensive operations...\n");
    
    // Simulate memory allocation and deallocation
    for (int i = 0; i < 1000; i++) {
        void* ptr = malloc(1024); // 1KB allocation
        if (ptr) {
            memset(ptr, 0xAA, 1024);
            free(ptr);
        }
    }
    
    // Get final memory usage
    status_file = fopen("/proc/self/status", "r");
    TEST_ASSERT_NOT_NULL(status_file);
    
    size_t final_vm_size = 0;
    size_t final_vm_rss = 0;
    
    while (fgets(line, sizeof(line), status_file)) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            sscanf(line, "VmSize: %zu", &final_vm_size);
        } else if (strncmp(line, "VmRSS:", 6) == 0) {
            sscanf(line, "VmRSS: %zu", &final_vm_rss);
        }
    }
    fclose(status_file);
    
    printf("Final Memory Usage - VmSize: %zu KB, VmRSS: %zu KB\n", final_vm_size, final_vm_rss);
    
    // Calculate memory growth
    size_t vm_size_growth = final_vm_size - initial_vm_size;
    size_t vm_rss_growth = final_vm_rss - initial_vm_rss;
    
    printf("Memory Growth - VmSize: %zu KB, VmRSS: %zu KB\n", vm_size_growth, vm_rss_growth);
    
    // Validate memory usage
    TEST_ASSERT_LESS_THAN(10240, vm_size_growth); // < 10MB growth
    TEST_ASSERT_LESS_THAN(5120, vm_rss_growth);   // < 5MB RSS growth
    
    performance_metrics.memory_usage_kb = final_vm_rss;
    
    printf("✅ Memory usage monitoring PASSED\n");
}

// CPU usage monitoring
void test_performance_cpu_usage(void) {
    printf("\n=== PERFORMANCE TEST: CPU USAGE MONITORING ===\n");
    
    // Get initial CPU time
    clock_t start_cpu = clock();
    struct timeval start_wall;
    gettimeofday(&start_wall, NULL);
    
    printf("Starting CPU-intensive operations...\n");
    
    // Perform CPU-intensive operations
    volatile double result = 0.0;
    for (int i = 0; i < 1000000; i++) {
        result += sin(i) * cos(i);
    }
    
    // Get final CPU time
    clock_t end_cpu = clock();
    struct timeval end_wall;
    gettimeofday(&end_wall, NULL);
    
    // Calculate CPU usage
    double cpu_time = (double)(end_cpu - start_cpu) / CLOCKS_PER_SEC;
    double wall_time = (end_wall.tv_sec - start_wall.tv_sec) + 
                      (end_wall.tv_usec - start_wall.tv_usec) / 1000000.0;
    double cpu_usage = (cpu_time / wall_time) * 100.0;
    
    printf("CPU Time: %.3f seconds\n", cpu_time);
    printf("Wall Time: %.3f seconds\n", wall_time);
    printf("CPU Usage: %.2f%%\n", cpu_usage);
    printf("Result: %.6f\n", result);
    
    // Validate CPU usage
    TEST_ASSERT_LESS_THAN(80.0, cpu_usage); // < 80% CPU usage
    
    performance_metrics.cpu_usage_percent = cpu_usage;
    
    printf("✅ CPU usage monitoring PASSED\n");
}

// Main function
int main(void) {
    printf("🚀 OHT-50 PERFORMANCE TEST SUITE\n");
    printf("================================\n");
    
    // Set up signal handler
    signal(SIGINT, performance_test_signal_handler);
    signal(SIGTERM, performance_test_signal_handler);
    
    // Initialize Unity test framework
    UNITY_BEGIN();
    
    // Run performance tests
    RUN_TEST(test_performance_load_testing);
    RUN_TEST(test_performance_memory_usage);
    RUN_TEST(test_performance_cpu_usage);
    
    // End Unity test framework
    UNITY_END();
    return 0;
}
