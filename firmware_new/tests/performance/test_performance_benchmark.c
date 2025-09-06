/**
 * @file test_performance_benchmark.c
 * @brief Performance benchmark tests for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team EMBED
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "unity.h"
#include "hal_common.h"
#include "hal_gpio.h"
#include "hal_led.h"

// Benchmark configuration
#define BENCHMARK_ITERATIONS 1000
#define BENCHMARK_TIMEOUT_MS 10000
#define TEST_GPIO_PIN 54

// Performance metrics
typedef struct {
    uint64_t min_us;
    uint64_t max_us;
    uint64_t avg_us;
    uint64_t total_us;
    int iterations;
} performance_metrics_t;

// Global performance data
static performance_metrics_t gpio_metrics = {0};
static performance_metrics_t led_metrics = {0};
static performance_metrics_t timestamp_metrics = {0};

void setUp(void) {
    // Initialize performance metrics
    memset(&gpio_metrics, 0, sizeof(performance_metrics_t));
    memset(&led_metrics, 0, sizeof(performance_metrics_t));
    memset(&timestamp_metrics, 0, sizeof(performance_metrics_t));
}

void tearDown(void) {
    // Cleanup
}

// Calculate performance metrics
void calculate_metrics(performance_metrics_t *metrics, uint64_t duration_us) {
    if (metrics->iterations == 0) {
        metrics->min_us = duration_us;
        metrics->max_us = duration_us;
    } else {
        if (duration_us < metrics->min_us) metrics->min_us = duration_us;
        if (duration_us > metrics->max_us) metrics->max_us = duration_us;
    }
    
    metrics->total_us += duration_us;
    metrics->iterations++;
    metrics->avg_us = metrics->total_us / metrics->iterations;
}

// Print performance metrics
void print_metrics(const char *test_name, const performance_metrics_t *metrics) {
    printf("📊 %s Performance:\n", test_name);
    printf("   Iterations: %d\n", metrics->iterations);
    printf("   Min: %lu μs\n", metrics->min_us);
    printf("   Max: %lu μs\n", metrics->max_us);
    printf("   Avg: %lu μs\n", metrics->avg_us);
    printf("   Total: %lu μs\n", metrics->total_us);
    printf("   Throughput: %.2f ops/sec\n", 
           (float)metrics->iterations / (metrics->total_us / 1000000.0));
}

// Test 1: GPIO Performance Benchmark
void test_gpio_performance_benchmark(void) {
    printf("\n=== GPIO Performance Benchmark ===\n");
    
    // Initialize GPIO
    hal_status_t status = hal_gpio_init();
    if (status != HAL_STATUS_OK) {
        printf("❌ GPIO initialization failed\n");
        TEST_FAIL();
        return;
    }
    
    // Configure test pin
    gpio_config_t config = {
        .pin_number = TEST_GPIO_PIN,
        .direction = GPIO_DIRECTION_OUT,
        .edge = GPIO_EDGE_NONE,
        .bias = GPIO_BIAS_DISABLE,
        .drive = GPIO_DRIVE_2MA,
        .active_low = false,
        .debounce_ms = 0
    };
    
    status = hal_gpio_configure_pin(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ GPIO pin configuration failed\n");
        TEST_FAIL();
        return;
    }
    
    printf("Running GPIO write/read benchmark (%d iterations)...\n", BENCHMARK_ITERATIONS);
    
    // Benchmark GPIO operations
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        uint64_t start_time = hal_get_timestamp_us();
        
        // GPIO write
        hal_gpio_set_value(TEST_GPIO_PIN, (i % 2) == 0);
        
        // GPIO read
        bool value;
        hal_gpio_get_value(TEST_GPIO_PIN, &value);
        
        uint64_t end_time = hal_get_timestamp_us();
        uint64_t duration = end_time - start_time;
        
        calculate_metrics(&gpio_metrics, duration);
        
        if (i % 100 == 0) {
            printf("Progress: %d/%d\n", i, BENCHMARK_ITERATIONS);
        }
    }
    
    print_metrics("GPIO Write/Read", &gpio_metrics);
    
    // Cleanup
    hal_gpio_set_value(TEST_GPIO_PIN, false);
    hal_gpio_deinit();
    
    // Performance validation
    if (gpio_metrics.avg_us < 1000) { // Less than 1ms average
        printf("✅ GPIO performance: EXCELLENT (< 1ms avg)\n");
    } else if (gpio_metrics.avg_us < 5000) { // Less than 5ms average
        printf("✅ GPIO performance: GOOD (< 5ms avg)\n");
    } else {
        printf("⚠️  GPIO performance: SLOW (> 5ms avg)\n");
    }
}

// Test 2: LED Performance Benchmark
void test_led_performance_benchmark(void) {
    printf("\n=== LED Performance Benchmark ===\n");
    
    // Initialize LED
    hal_status_t status = hal_led_init();
    if (status != HAL_STATUS_OK) {
        printf("❌ LED initialization failed\n");
        TEST_FAIL();
        return;
    }
    
    printf("Running LED control benchmark (%d iterations)...\n", BENCHMARK_ITERATIONS);
    
    // Benchmark LED operations
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        uint64_t start_time = hal_get_timestamp_us();
        
        // LED state changes
        hal_led_power_set((i % 2) == 0 ? LED_STATE_ON : LED_STATE_OFF);
        hal_led_system_set((i % 3) == 0 ? LED_STATE_BLINK_SLOW : LED_STATE_OFF);
        
        uint64_t end_time = hal_get_timestamp_us();
        uint64_t duration = end_time - start_time;
        
        calculate_metrics(&led_metrics, duration);
        
        if (i % 100 == 0) {
            printf("Progress: %d/%d\n", i, BENCHMARK_ITERATIONS);
        }
    }
    
    print_metrics("LED Control", &led_metrics);
    
    // Cleanup
    hal_led_power_set(LED_STATE_OFF);
    hal_led_system_set(LED_STATE_OFF);
    hal_led_deinit();
    
    // Performance validation
    if (led_metrics.avg_us < 500) { // Less than 0.5ms average
        printf("✅ LED performance: EXCELLENT (< 0.5ms avg)\n");
    } else if (led_metrics.avg_us < 2000) { // Less than 2ms average
        printf("✅ LED performance: GOOD (< 2ms avg)\n");
    } else {
        printf("⚠️  LED performance: SLOW (> 2ms avg)\n");
    }
}

// Test 3: Timestamp Performance Benchmark
void test_timestamp_performance_benchmark(void) {
    printf("\n=== Timestamp Performance Benchmark ===\n");
    
    printf("Running timestamp function benchmark (%d iterations)...\n", BENCHMARK_ITERATIONS);
    
    // Benchmark timestamp operations
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        uint64_t start_time = hal_get_timestamp_us();
        
        // Timestamp operations
        uint64_t timestamp_us = hal_get_timestamp_us();
        uint64_t timestamp_ms = hal_get_timestamp_ms();
        
        uint64_t end_time = hal_get_timestamp_us();
        uint64_t duration = end_time - start_time;
        
        calculate_metrics(&timestamp_metrics, duration);
        
        if (i % 100 == 0) {
            printf("Progress: %d/%d\n", i, BENCHMARK_ITERATIONS);
        }
    }
    
    print_metrics("Timestamp Functions", &timestamp_metrics);
    
    // Performance validation
    if (timestamp_metrics.avg_us < 10) { // Less than 10μs average
        printf("✅ Timestamp performance: EXCELLENT (< 10μs avg)\n");
    } else if (timestamp_metrics.avg_us < 100) { // Less than 100μs average
        printf("✅ Timestamp performance: GOOD (< 100μs avg)\n");
    } else {
        printf("⚠️  Timestamp performance: SLOW (> 100μs avg)\n");
    }
}

// Test 4: Memory Usage Benchmark
void test_memory_usage_benchmark(void) {
    printf("\n=== Memory Usage Benchmark ===\n");
    
    // Get initial memory info
    FILE *fp = fopen("/proc/self/status", "r");
    if (fp) {
        char line[256];
        unsigned long vm_rss = 0, vm_size = 0;
        
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "VmRSS:", 6) == 0) {
                sscanf(line, "VmRSS: %lu", &vm_rss);
            } else if (strncmp(line, "VmSize:", 7) == 0) {
                sscanf(line, "VmSize: %lu", &vm_size);
            }
        }
        fclose(fp);
        
        printf("📊 Memory Usage:\n");
        printf("   Virtual Memory: %lu KB\n", vm_size);
        printf("   Resident Memory: %lu KB\n", vm_rss);
        printf("   Memory Efficiency: %.2f%%\n", (float)vm_rss / vm_size * 100.0);
        
        // Memory validation
        if (vm_rss < 10240) { // Less than 10MB
            printf("✅ Memory usage: EXCELLENT (< 10MB)\n");
        } else if (vm_rss < 51200) { // Less than 50MB
            printf("✅ Memory usage: GOOD (< 50MB)\n");
        } else {
            printf("⚠️  Memory usage: HIGH (> 50MB)\n");
        }
    }
}

// Test 5: System Load Benchmark
void test_system_load_benchmark(void) {
    printf("\n=== System Load Benchmark ===\n");
    
    // Get system load
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp) {
        float load_1min, load_5min, load_15min;
        int running_processes, total_processes;
        
        fscanf(fp, "%f %f %f %d/%d", 
               &load_1min, &load_5min, &load_15min, 
               &running_processes, &total_processes);
        fclose(fp);
        
        printf("📊 System Load:\n");
        printf("   1-minute average: %.2f\n", load_1min);
        printf("   5-minute average: %.2f\n", load_5min);
        printf("   15-minute average: %.2f\n", load_15min);
        printf("   Running processes: %d/%d\n", running_processes, total_processes);
        
        // Load validation
        if (load_1min < 0.5) {
            printf("✅ System load: EXCELLENT (< 0.5)\n");
        } else if (load_1min < 1.0) {
            printf("✅ System load: GOOD (< 1.0)\n");
        } else {
            printf("⚠️  System load: HIGH (> 1.0)\n");
        }
    }
}

// Test 6: Overall Performance Summary
void test_performance_summary(void) {
    printf("\n=== Overall Performance Summary ===\n");
    
    printf("📊 Performance Summary:\n");
    printf("   GPIO Write/Read: %lu μs avg\n", gpio_metrics.avg_us);
    printf("   LED Control: %lu μs avg\n", led_metrics.avg_us);
    printf("   Timestamp Functions: %lu μs avg\n", timestamp_metrics.avg_us);
    
    // Overall performance score
    int score = 0;
    int total_tests = 0;
    
    if (gpio_metrics.avg_us < 1000) score++;
    if (led_metrics.avg_us < 500) score++;
    if (timestamp_metrics.avg_us < 10) score++;
    total_tests = 3;
    
    float performance_score = (float)score / total_tests * 100.0;
    printf("   Performance Score: %.1f%%\n", performance_score);
    
    if (performance_score >= 90.0) {
        printf("✅ Overall Performance: EXCELLENT\n");
    } else if (performance_score >= 70.0) {
        printf("✅ Overall Performance: GOOD\n");
    } else {
        printf("⚠️  Overall Performance: NEEDS IMPROVEMENT\n");
    }
}

// Main benchmark runner
int main(void) {
    printf("=== OHT-50 Master Module Performance Benchmarks ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("Iterations per test: %d\n", BENCHMARK_ITERATIONS);
    
    UNITY_BEGIN();
    
    // Run performance benchmarks
    RUN_TEST(test_gpio_performance_benchmark);
    RUN_TEST(test_led_performance_benchmark);
    RUN_TEST(test_timestamp_performance_benchmark);
    RUN_TEST(test_memory_usage_benchmark);
    RUN_TEST(test_system_load_benchmark);
    RUN_TEST(test_performance_summary);
    
    printf("\n=== Benchmark Complete ===\n");
    
    return UNITY_END();
}
