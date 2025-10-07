/**
 * @file test_lidar_performance_benchmarks.c
 * @brief Performance Benchmarks for LiDAR HAL v2.3.0
 * @version 2.3.0
 * @date 2025-01-28
 * @team EMBED
 * 
 * This file contains comprehensive performance benchmarks for all LiDAR HAL features:
 * - Enhanced Resolution System Performance
 * - Advanced Multi-Sample Processing Performance
 * - Multi-Threading Performance
 * - Memory Pool Performance
 * - Adaptive Processing Performance
 * - Hardware Acceleration Performance
 * - Load Balancing Performance
 * - Performance Scaling Benchmarks
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include "hal_lidar.h"
#include "hal_common.h"

// Benchmark configuration
#define BENCHMARK_ITERATIONS       10000
#define BENCHMARK_WARMUP_ITERATIONS 1000
#define BENCHMARK_MEMORY_SIZE      4096
#define BENCHMARK_THREAD_COUNT     4
#define BENCHMARK_SAMPLE_COUNT     100

// Performance targets
#define TARGET_RESOLUTION_TIME_MS      1.0
#define TARGET_ACCURACY_TIME_MS        2.0
#define TARGET_THREADING_TIME_MS       5.0
#define TARGET_MEMORY_TIME_MS          0.5
#define TARGET_ADAPTIVE_TIME_MS        10.0
#define TARGET_HW_ACCEL_TIME_MS        3.0
#define TARGET_LOAD_BALANCE_TIME_MS    2.0
#define TARGET_SCALING_TIME_MS         1.0

// Performance metrics structure
typedef struct {
    char name[64];
    double min_time_ms;
    double max_time_ms;
    double avg_time_ms;
    double median_time_ms;
    double std_dev_ms;
    double p95_time_ms;
    double p99_time_ms;
    uint64_t iterations;
    bool target_met;
    double target_time_ms;
} benchmark_metrics_t;

// Global benchmark results
static benchmark_metrics_t g_benchmark_results[16];
static int g_benchmark_count = 0;

// Benchmark utilities
static void benchmark_start(const char* name, double target_time_ms);
static void benchmark_end(void);
static void benchmark_iteration(double duration_ms);
static void calculate_statistics(benchmark_metrics_t* metrics, double* times, uint64_t count);
static void print_benchmark_results(void);
static void print_performance_summary(void);
static double get_time_ms(void);
static void warmup_benchmark(void (*benchmark_func)(void));

// Benchmark functions
static void benchmark_enhanced_resolution(void);
static void benchmark_advanced_multi_sample(void);
static void benchmark_multi_threading(void);
static void benchmark_memory_pool(void);
static void benchmark_adaptive_processing(void);
static void benchmark_hardware_acceleration(void);
static void benchmark_load_balancing(void);
static void benchmark_performance_scaling(void);
static void benchmark_integration_performance(void);
static void benchmark_stress_test(void);

// Performance comparison functions
static void compare_baseline_vs_enhanced(void);
static void compare_feature_performance(void);
static void analyze_performance_trends(void);

int main(void) {
    printf("⚡ LIDAR HAL PERFORMANCE BENCHMARKS v2.3.0\n");
    printf("===========================================\n");
    printf("Comprehensive performance testing for all LiDAR HAL features\n");
    printf("Date: 2025-01-28\n");
    printf("Team: EMBED\n\n");

    // Initialize LiDAR HAL
    lidar_config_t config = {
        .device_path = "/dev/ttyUSB0",
        .baud_rate = 460800,
        .scan_rate_hz = 10,
        .emergency_stop_mm = 100,
        .warning_mm = 200,
        .safe_mm = 500,
        .sample_rate_hz = 1000,
        .angular_resolution = 0.72f
    };
    hal_status_t status = hal_lidar_init(&config);
    if (status != HAL_STATUS_OK) {
        printf("❌ LiDAR HAL initialization failed: %d\n", status);
        return -1;
    }
    printf("✅ LiDAR HAL initialized successfully\n\n");

    // Run all benchmarks
    printf("🚀 STARTING PERFORMANCE BENCHMARKS\n");
    printf("===================================\n\n");

    // Individual feature benchmarks
    benchmark_enhanced_resolution();
    benchmark_advanced_multi_sample();
    benchmark_multi_threading();
    benchmark_memory_pool();
    benchmark_adaptive_processing();
    benchmark_hardware_acceleration();
    benchmark_load_balancing();
    benchmark_performance_scaling();

    // Integration and stress tests
    benchmark_integration_performance();
    benchmark_stress_test();

    // Performance analysis
    compare_baseline_vs_enhanced();
    compare_feature_performance();
    analyze_performance_trends();

    // Print final results
    printf("\n🏁 PERFORMANCE BENCHMARK RESULTS\n");
    printf("=================================\n");
    print_benchmark_results();
    print_performance_summary();

    printf("\n🎯 LiDAR HAL v2.3.0 Performance Benchmarks Complete!\n");
    return 0;
}

// Benchmark utilities implementation
static void benchmark_start(const char* name, double target_time_ms) {
    strncpy(g_benchmark_results[g_benchmark_count].name, name, sizeof(g_benchmark_results[g_benchmark_count].name) - 1);
    g_benchmark_results[g_benchmark_count].min_time_ms = 999999.0;
    g_benchmark_results[g_benchmark_count].max_time_ms = 0.0;
    g_benchmark_results[g_benchmark_count].avg_time_ms = 0.0;
    g_benchmark_results[g_benchmark_count].iterations = 0;
    g_benchmark_results[g_benchmark_count].target_time_ms = target_time_ms;
    g_benchmark_results[g_benchmark_count].target_met = false;
    
    printf("⚡ Benchmarking: %s (target: %.1fms)\n", name, target_time_ms);
}

static void benchmark_end(void) {
    benchmark_metrics_t* metrics = &g_benchmark_results[g_benchmark_count];
    
    if (metrics->iterations > 0) {
        metrics->avg_time_ms /= metrics->iterations;
        metrics->target_met = (metrics->avg_time_ms <= metrics->target_time_ms);
        
        printf("   📊 Results: min=%.3fms, max=%.3fms, avg=%.3fms, target=%.1fms %s\n",
               metrics->min_time_ms, metrics->max_time_ms, metrics->avg_time_ms,
               metrics->target_time_ms, metrics->target_met ? "✅" : "❌");
    }
    
    g_benchmark_count++;
}

static void benchmark_iteration(double duration_ms) {
    benchmark_metrics_t* metrics = &g_benchmark_results[g_benchmark_count];
    
    if (duration_ms < metrics->min_time_ms) metrics->min_time_ms = duration_ms;
    if (duration_ms > metrics->max_time_ms) metrics->max_time_ms = duration_ms;
    metrics->avg_time_ms += duration_ms;
    metrics->iterations++;
}

static void calculate_statistics(benchmark_metrics_t* metrics, double* times, uint64_t count) {
    if (count == 0) return;
    
    // Sort times for median calculation
    for (uint64_t i = 0; i < count - 1; i++) {
        for (uint64_t j = i + 1; j < count; j++) {
            if (times[i] > times[j]) {
                double temp = times[i];
                times[i] = times[j];
                times[j] = temp;
            }
        }
    }
    
    // Calculate median
    if (count % 2 == 0) {
        metrics->median_time_ms = (times[count/2 - 1] + times[count/2]) / 2.0;
    } else {
        metrics->median_time_ms = times[count/2];
    }
    
    // Calculate percentiles
    metrics->p95_time_ms = times[(uint64_t)(count * 0.95)];
    metrics->p99_time_ms = times[(uint64_t)(count * 0.99)];
    
    // Calculate standard deviation
    double variance = 0.0;
    for (uint64_t i = 0; i < count; i++) {
        double diff = times[i] - metrics->avg_time_ms;
        variance += diff * diff;
    }
    metrics->std_dev_ms = sqrt(variance / count);
}

static void print_benchmark_results(void) {
    printf("📊 Individual Benchmark Results:\n");
    printf("===============================\n");
    
    for (int i = 0; i < g_benchmark_count; i++) {
        benchmark_metrics_t* metrics = &g_benchmark_results[i];
        printf("%-30s: avg=%.3fms, min=%.3fms, max=%.3fms, target=%.1fms %s\n",
               metrics->name, metrics->avg_time_ms, metrics->min_time_ms,
               metrics->max_time_ms, metrics->target_time_ms,
               metrics->target_met ? "✅" : "❌");
    }
}

static void print_performance_summary(void) {
    printf("\n📈 Performance Summary:\n");
    printf("======================\n");
    
    int targets_met = 0;
    double total_improvement = 0.0;
    
    for (int i = 0; i < g_benchmark_count; i++) {
        benchmark_metrics_t* metrics = &g_benchmark_results[i];
        if (metrics->target_met) targets_met++;
        
        double improvement = ((metrics->target_time_ms - metrics->avg_time_ms) / metrics->target_time_ms) * 100.0;
        total_improvement += improvement;
    }
    
    printf("🎯 Targets Met: %d/%d (%.1f%%)\n", targets_met, g_benchmark_count,
           (double)targets_met / g_benchmark_count * 100.0);
    printf("📈 Average Improvement: %.1f%%\n", total_improvement / g_benchmark_count);
    
    // Performance categories
    printf("\n🏆 Performance Categories:\n");
    printf("   • Enhanced Resolution: %.3fms (target: %.1fms)\n",
           g_benchmark_results[0].avg_time_ms, g_benchmark_results[0].target_time_ms);
    printf("   • Multi-Sample Processing: %.3fms (target: %.1fms)\n",
           g_benchmark_results[1].avg_time_ms, g_benchmark_results[1].target_time_ms);
    printf("   • Multi-Threading: %.3fms (target: %.1fms)\n",
           g_benchmark_results[2].avg_time_ms, g_benchmark_results[2].target_time_ms);
    printf("   • Memory Pool: %.3fms (target: %.1fms)\n",
           g_benchmark_results[3].avg_time_ms, g_benchmark_results[3].target_time_ms);
    printf("   • Adaptive Processing: %.3fms (target: %.1fms)\n",
           g_benchmark_results[4].avg_time_ms, g_benchmark_results[4].target_time_ms);
}

static double get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

static void warmup_benchmark(void (*benchmark_func)(void)) {
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        benchmark_func();
    }
}

// Enhanced Resolution System Benchmark
static void benchmark_enhanced_resolution(void) {
    benchmark_start("Enhanced Resolution System", TARGET_RESOLUTION_TIME_MS);
    
    lidar_adaptive_config_t config = {
        .base_resolution_deg = 0.72f,
        .focus_resolution_deg = 0.36f,
        .focus_angle_start = 45,
        .focus_angle_end = 135,
        .adaptive_enabled = true,
        .focus_priority = 3
    };
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        hal_lidar_set_adaptive_resolution(&config);
        hal_lidar_get_adaptive_status(&config);
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_set_adaptive_resolution(&config);
        hal_lidar_get_adaptive_status(&config);
        hal_lidar_set_focus_area(60.0f, 120.0f, 0.18f);
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Advanced Multi-Sample Processing Benchmark
static void benchmark_advanced_multi_sample(void) {
    benchmark_start("Advanced Multi-Sample Processing", TARGET_ACCURACY_TIME_MS);
    
    lidar_accuracy_config_t config = {
        .sample_count = 5,
        .sample_interval_ms = 20,
        .enable_outlier_filter = true,
        .outlier_threshold = 20.0f,
        .enable_smoothing = true,
        .smoothing_window = 3,
        .enable_statistical_averaging = true,
        .confidence_level = 0.95f,
        .enable_weighted_averaging = true,
        .enable_temporal_filtering = true,
        .temporal_window_size = 10,
        .quality_threshold = 0.8f
    };
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        hal_lidar_configure_accuracy(&config);
        hal_lidar_enable_statistical_averaging(true, 0.95f);
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_configure_accuracy(&config);
        hal_lidar_configure_advanced_accuracy(&config);
        hal_lidar_enable_statistical_averaging(true, 0.95f);
        hal_lidar_calibrate_distance(1000);
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Multi-Threading Performance Benchmark
static void benchmark_multi_threading(void) {
    benchmark_start("Multi-Threading Performance", TARGET_THREADING_TIME_MS);
    
    lidar_threading_config_t config = {
        .thread_count = BENCHMARK_THREAD_COUNT,
        .thread_stack_size = 65536,
        .scan_thread_priority = 0,
        .processing_thread_priority = 0,
        .calibration_thread_priority = 0,
        .enable_parallel_processing = true,
        .enable_thread_affinity = false,
        .cpu_cores = 1
    };
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        hal_lidar_configure_threading(&config);
        hal_lidar_enable_parallel_processing(true);
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_configure_threading(&config);
        hal_lidar_enable_parallel_processing(true);
        hal_lidar_set_thread_priority(0, 10);
        hal_lidar_set_thread_affinity(0, 0);
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Memory Pool Performance Benchmark
static void benchmark_memory_pool(void) {
    benchmark_start("Memory Pool Performance", TARGET_MEMORY_TIME_MS);
    
    lidar_memory_pool_t config = {
        .pool_size = 1048576,  // 1MB
        .block_size = 4096,    // 4KB
        .max_blocks = 256,
        .alignment = 64,
        .enable_compaction = true,
        .enable_statistics = true
    };
    
    hal_lidar_configure_memory_pool(&config);
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        void *memory_block = NULL;
        hal_lidar_allocate_memory_block(&memory_block, BENCHMARK_MEMORY_SIZE);
        if (memory_block) {
            hal_lidar_deallocate_memory_block(memory_block);
        }
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        void *memory_block = NULL;
        hal_lidar_allocate_memory_block(&memory_block, BENCHMARK_MEMORY_SIZE);
        if (memory_block) {
            hal_lidar_deallocate_memory_block(memory_block);
        }
        hal_lidar_compact_memory_pool();
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Adaptive Processing Performance Benchmark
static void benchmark_adaptive_processing(void) {
    benchmark_start("Adaptive Processing Performance", TARGET_ADAPTIVE_TIME_MS);
    
    lidar_adaptive_processing_config_t config = {
        .algorithm_count = 6,
        .update_interval_ms = 100,
        .learning_rate = 0.15f,
        .convergence_threshold = 0.01f,
        .performance_window_ms = 1000,
        .enable_real_time_optimization = true,
        .enable_dynamic_scaling = true,
        .enable_intelligent_balancing = true
    };
    
    hal_lidar_configure_adaptive_processing(&config);
    hal_lidar_enable_adaptive_processing(true);
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        hal_lidar_optimize_performance();
        hal_lidar_scale_performance(1800);
        hal_lidar_balance_workload();
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_optimize_performance();
        hal_lidar_scale_performance(1800);
        hal_lidar_balance_workload();
        
        float efficiency, throughput, latency;
        hal_lidar_get_performance_metrics(&efficiency, &throughput, &latency);
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Hardware Acceleration Performance Benchmark
static void benchmark_hardware_acceleration(void) {
    benchmark_start("Hardware Acceleration Performance", TARGET_HW_ACCEL_TIME_MS);
    
    lidar_hardware_acceleration_config_t config = {
        .enable_gpu_acceleration = true,
        .enable_dsp_acceleration = true,
        .enable_neon_acceleration = true,
        .device_count = 3,
        .batch_size = 128,
        .queue_size = 256,
        .enable_parallel_execution = true,
        .enable_memory_optimization = true,
        .enable_cache_optimization = true
    };
    
    hal_lidar_configure_hardware_acceleration(&config);
    hal_lidar_enable_hardware_acceleration(true);
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        hal_lidar_configure_hardware_acceleration(&config);
        hal_lidar_enable_hardware_acceleration(true);
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_configure_hardware_acceleration(&config);
        hal_lidar_enable_hardware_acceleration(true);
        
        lidar_hardware_acceleration_config_t status_config;
        hal_lidar_get_hardware_acceleration_status(&status_config);
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Load Balancing Performance Benchmark
static void benchmark_load_balancing(void) {
    benchmark_start("Load Balancing Performance", TARGET_LOAD_BALANCE_TIME_MS);
    
    lidar_load_balancing_config_t config = {
        .workload_count = 8,
        .update_rate_ms = 50,
        .balance_threshold = 0.75f,
        .migration_cost = 0.1f,
        .enable_workload_migration = true,
        .enable_dynamic_scheduling = true,
        .enable_performance_monitoring = true
    };
    
    hal_lidar_configure_load_balancing(&config);
    hal_lidar_enable_load_balancing(true);
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        hal_lidar_balance_workload();
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_configure_load_balancing(&config);
        hal_lidar_enable_load_balancing(true);
        hal_lidar_balance_workload();
        
        lidar_load_balancing_config_t status_config;
        hal_lidar_get_load_balancing_status(&status_config);
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Performance Scaling Benchmark
static void benchmark_performance_scaling(void) {
    benchmark_start("Performance Scaling", TARGET_SCALING_TIME_MS);
    
    lidar_performance_scaling_config_t config = {
        .min_frequency_mhz = 200,
        .max_frequency_mhz = 2400,
        .step_size_mhz = 100,
        .target_latency_ms = 5,
        .power_budget_mw = 8000,
        .enable_dynamic_frequency = true,
        .enable_power_management = true,
        .enable_thermal_management = true
    };
    
    hal_lidar_configure_performance_scaling(&config);
    hal_lidar_enable_performance_scaling(true);
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        hal_lidar_scale_performance(1800);
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_configure_performance_scaling(&config);
        hal_lidar_enable_performance_scaling(true);
        hal_lidar_scale_performance(1800);
        
        uint32_t power_mw;
        hal_lidar_get_power_consumption(&power_mw);
        
        float temperature_c;
        hal_lidar_get_thermal_status(&temperature_c);
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Integration Performance Benchmark
static void benchmark_integration_performance(void) {
    benchmark_start("Integration Performance", 50.0); // 50ms target
    
    // Configure all features
    lidar_adaptive_config_t adaptive_config = {
        .base_resolution_deg = 0.72f,
        .focus_resolution_deg = 0.36f,
        .focus_angle_start = 45,
        .focus_angle_end = 135,
        .adaptive_enabled = true,
        .focus_priority = 3
    };
    
    lidar_accuracy_config_t accuracy_config = {
        .sample_count = 5,
        .sample_interval_ms = 20,
        .enable_outlier_filter = true,
        .outlier_threshold = 20.0f,
        .enable_smoothing = true,
        .smoothing_window = 3,
        .enable_statistical_averaging = true,
        .confidence_level = 0.95f
    };
    
    lidar_threading_config_t threading_config = {
        .thread_count = 4,
        .thread_stack_size = 65536,
        .scan_thread_priority = 0,
        .processing_thread_priority = 0,
        .calibration_thread_priority = 0,
        .enable_parallel_processing = true,
        .enable_thread_affinity = false
    };
    
    lidar_adaptive_processing_config_t adaptive_processing_config = {
        .algorithm_count = 6,
        .update_interval_ms = 100,
        .learning_rate = 0.15f,
        .convergence_threshold = 0.01f,
        .performance_window_ms = 1000,
        .enable_real_time_optimization = true,
        .enable_dynamic_scaling = true,
        .enable_intelligent_balancing = true
    };
    
    // Warmup
    for (int i = 0; i < BENCHMARK_WARMUP_ITERATIONS; i++) {
        hal_lidar_set_adaptive_resolution(&adaptive_config);
        hal_lidar_configure_accuracy(&accuracy_config);
        hal_lidar_configure_threading(&threading_config);
        hal_lidar_configure_adaptive_processing(&adaptive_processing_config);
    }
    
    // Benchmark
    for (uint64_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        // Configure all features
        hal_lidar_set_adaptive_resolution(&adaptive_config);
        hal_lidar_configure_accuracy(&accuracy_config);
        hal_lidar_configure_threading(&threading_config);
        hal_lidar_configure_adaptive_processing(&adaptive_processing_config);
        
        // Enable all features
        hal_lidar_enable_parallel_processing(true);
        hal_lidar_enable_adaptive_processing(true);
        
        // Perform operations
        hal_lidar_optimize_performance();
        hal_lidar_scale_performance(1800);
        hal_lidar_balance_workload();
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
    }
    
    benchmark_end();
}

// Stress Test Benchmark
static void benchmark_stress_test(void) {
    benchmark_start("Stress Test Performance", 100.0); // 100ms target
    
    // Stress test with high iteration count
    const uint64_t stress_iterations = BENCHMARK_ITERATIONS * 2;
    
    for (uint64_t i = 0; i < stress_iterations; i++) {
        double start_time = get_time_ms();
        
        // Rapid configuration changes
        lidar_adaptive_config_t adaptive_config = {
            .base_resolution_deg = 0.72f + (i % 10) * 0.1f,
            .focus_resolution_deg = 0.36f + (i % 5) * 0.05f,
            .focus_angle_start = 45 + (i % 90),
            .focus_angle_end = 135 + (i % 90),
            .adaptive_enabled = (i % 2) == 0,
            .focus_priority = (i % 3) + 1
        };
        
        hal_lidar_set_adaptive_resolution(&adaptive_config);
        hal_lidar_get_adaptive_status(&adaptive_config);
        
        // Memory allocation/deallocation stress
        void *memory_block = NULL;
        hal_lidar_allocate_memory_block(&memory_block, BENCHMARK_MEMORY_SIZE);
        if (memory_block) {
            hal_lidar_deallocate_memory_block(memory_block);
        }
        
        // Performance optimization stress
        hal_lidar_optimize_performance();
        hal_lidar_scale_performance(1200 + (i % 1200));
        hal_lidar_balance_workload();
        
        double end_time = get_time_ms();
        benchmark_iteration(end_time - start_time);
        
        // Progress indicator
        if (i % (stress_iterations / 10) == 0) {
            printf("   📊 Stress test progress: %lu%%\n", (i * 100) / stress_iterations);
        }
    }
    
    benchmark_end();
}

// Performance comparison functions
static void compare_baseline_vs_enhanced(void) {
    printf("\n📊 BASELINE vs ENHANCED PERFORMANCE COMPARISON\n");
    printf("===============================================\n");
    
    // Simulated baseline performance (typical values)
    double baseline_times[] = {
        5.0,   // Enhanced Resolution
        8.0,   // Multi-Sample Processing
        15.0,  // Multi-Threading
        2.0,   // Memory Pool
        25.0,  // Adaptive Processing
        10.0,  // Hardware Acceleration
        6.0,   // Load Balancing
        3.0    // Performance Scaling
    };
    
    const char* feature_names[] = {
        "Enhanced Resolution",
        "Multi-Sample Processing",
        "Multi-Threading",
        "Memory Pool",
        "Adaptive Processing",
        "Hardware Acceleration",
        "Load Balancing",
        "Performance Scaling"
    };
    
    printf("Feature                    | Baseline | Enhanced | Improvement\n");
    printf("---------------------------|----------|----------|------------\n");
    
    double total_improvement = 0.0;
    for (int i = 0; i < 8 && i < g_benchmark_count; i++) {
        double improvement = ((baseline_times[i] - g_benchmark_results[i].avg_time_ms) / baseline_times[i]) * 100.0;
        total_improvement += improvement;
        
        printf("%-26s | %8.1f | %8.3f | %8.1f%%\n",
               feature_names[i], baseline_times[i], g_benchmark_results[i].avg_time_ms, improvement);
    }
    
    printf("---------------------------|----------|----------|------------\n");
    printf("Average Improvement: %.1f%%\n", total_improvement / 8.0);
}

static void compare_feature_performance(void) {
    printf("\n📈 FEATURE PERFORMANCE RANKING\n");
    printf("==============================\n");
    
    // Sort features by performance (ascending - faster is better)
    for (int i = 0; i < g_benchmark_count - 1; i++) {
        for (int j = i + 1; j < g_benchmark_count; j++) {
            if (g_benchmark_results[i].avg_time_ms > g_benchmark_results[j].avg_time_ms) {
                benchmark_metrics_t temp = g_benchmark_results[i];
                g_benchmark_results[i] = g_benchmark_results[j];
                g_benchmark_results[j] = temp;
            }
        }
    }
    
    printf("Rank | Feature                    | Avg Time | Target | Status\n");
    printf("-----|----------------------------|----------|--------|-------\n");
    
    for (int i = 0; i < g_benchmark_count; i++) {
        printf("%4d | %-26s | %8.3f | %6.1f | %s\n",
               i + 1, g_benchmark_results[i].name, g_benchmark_results[i].avg_time_ms,
               g_benchmark_results[i].target_time_ms,
               g_benchmark_results[i].target_met ? "✅" : "❌");
    }
}

static void analyze_performance_trends(void) {
    printf("\n📊 PERFORMANCE TREND ANALYSIS\n");
    printf("=============================\n");
    
    // Analyze performance consistency
    printf("Performance Consistency Analysis:\n");
    for (int i = 0; i < g_benchmark_count; i++) {
        benchmark_metrics_t* metrics = &g_benchmark_results[i];
        double consistency = (metrics->max_time_ms - metrics->min_time_ms) / metrics->avg_time_ms * 100.0;
        
        printf("   • %-26s: %.1f%% variation (%.3f-%.3fms)\n",
               metrics->name, consistency, metrics->min_time_ms, metrics->max_time_ms);
    }
    
    // Performance categories
    printf("\nPerformance Categories:\n");
    printf("   • Fastest (< 1ms): ");
    for (int i = 0; i < g_benchmark_count; i++) {
        if (g_benchmark_results[i].avg_time_ms < 1.0) {
            printf("%s ", g_benchmark_results[i].name);
        }
    }
    printf("\n");
    
    printf("   • Fast (1-5ms): ");
    for (int i = 0; i < g_benchmark_count; i++) {
        if (g_benchmark_results[i].avg_time_ms >= 1.0 && g_benchmark_results[i].avg_time_ms < 5.0) {
            printf("%s ", g_benchmark_results[i].name);
        }
    }
    printf("\n");
    
    printf("   • Moderate (5-20ms): ");
    for (int i = 0; i < g_benchmark_count; i++) {
        if (g_benchmark_results[i].avg_time_ms >= 5.0 && g_benchmark_results[i].avg_time_ms < 20.0) {
            printf("%s ", g_benchmark_results[i].name);
        }
    }
    printf("\n");
    
    printf("   • Slow (> 20ms): ");
    for (int i = 0; i < g_benchmark_count; i++) {
        if (g_benchmark_results[i].avg_time_ms >= 20.0) {
            printf("%s ", g_benchmark_results[i].name);
        }
    }
    printf("\n");
}
