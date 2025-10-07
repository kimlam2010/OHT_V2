/**
 * @file test_lidar_unit_tests.c
 * @brief Comprehensive Unit Tests for LiDAR HAL v2.3.0
 * @version 2.3.0
 * @date 2025-01-28
 * @team EMBED
 * 
 * This file contains comprehensive unit tests for all LiDAR HAL features:
 * - Enhanced Resolution System (v2.0.0)
 * - Advanced Multi-Sample & Calibration (v2.1.0)
 * - Multi-Threading & Memory Pool (v2.2.0)
 * - Adaptive Processing & Hardware Acceleration (v2.3.0)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "hal_lidar.h"
#include "hal_common.h"

// Test configuration
#define MAX_TEST_ITERATIONS    1000
#define PERFORMANCE_TEST_ITERATIONS 10000
#define MEMORY_TEST_SIZE       1024
#define THREAD_TEST_COUNT      4

// Test statistics
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    double total_time_ms;
    double average_time_ms;
} test_stats_t;

// Performance metrics
typedef struct {
    double min_time_ms;
    double max_time_ms;
    double avg_time_ms;
    double std_dev_ms;
    uint64_t iterations;
} performance_metrics_t;

// Global test statistics
static test_stats_t g_test_stats = {0};
static performance_metrics_t g_perf_metrics = {0};

// Test utilities
static void test_start(const char* test_name);
static void test_end(const char* test_name, bool passed);
static void performance_start(void);
static void performance_end(void);
static double get_time_ms(void);
static void print_test_summary(void);
static void print_performance_summary(void);

// Test categories
static void test_enhanced_resolution(void);
static void test_advanced_multi_sample(void);
static void test_multi_threading(void);
static void test_adaptive_processing(void);
static void test_error_handling(void);
static void test_performance_benchmarks(void);

// Individual test functions
static bool test_adaptive_resolution_config(void);
static bool test_focus_area_control(void);
static bool test_accuracy_configuration(void);
static bool test_calibration_system(void);
static bool test_threading_system(void);
static bool test_memory_pool_management(void);
static bool test_adaptive_processing_system(void);
static bool test_hardware_acceleration(void);
static bool test_load_balancing(void);
static bool test_performance_scaling(void);

// Performance test functions
static void benchmark_adaptive_resolution(void);
static void benchmark_multi_sample_processing(void);
static void benchmark_threading_performance(void);
static void benchmark_memory_allocation(void);
static void benchmark_adaptive_processing(void);

int main(void) {
    printf("🧪 LIDAR HAL COMPREHENSIVE UNIT TESTS v2.3.0\n");
    printf("==============================================\n");
    printf("Testing all LiDAR HAL features with comprehensive coverage\n");
    printf("Date: 2025-01-28\n");
    printf("Team: EMBED\n\n");

    // Initialize LiDAR HAL
    hal_status_t status = hal_lidar_init();
    if (status != HAL_OK) {
        printf("❌ LiDAR HAL initialization failed: %d\n", status);
        return -1;
    }
    printf("✅ LiDAR HAL initialized successfully\n\n");

    // Run all test categories
    printf("🚀 STARTING COMPREHENSIVE UNIT TESTS\n");
    printf("=====================================\n\n");

    test_enhanced_resolution();
    test_advanced_multi_sample();
    test_multi_threading();
    test_adaptive_processing();
    test_error_handling();
    test_performance_benchmarks();

    // Print final results
    printf("\n🏁 UNIT TEST RESULTS SUMMARY\n");
    printf("============================\n");
    print_test_summary();
    print_performance_summary();

    printf("\n🎯 LiDAR HAL v2.3.0 Unit Tests Complete!\n");
    return (g_test_stats.failed_tests > 0) ? -1 : 0;
}

// Test utilities implementation
static void test_start(const char* test_name) {
    printf("🧪 Testing: %s\n", test_name);
    g_test_stats.total_tests++;
}

static void test_end(const char* test_name, bool passed) {
    if (passed) {
        printf("   ✅ PASSED: %s\n", test_name);
        g_test_stats.passed_tests++;
    } else {
        printf("   ❌ FAILED: %s\n", test_name);
        g_test_stats.failed_tests++;
    }
}

static void performance_start(void) {
    g_perf_metrics.min_time_ms = 999999.0;
    g_perf_metrics.max_time_ms = 0.0;
    g_perf_metrics.avg_time_ms = 0.0;
    g_perf_metrics.iterations = 0;
}

static void performance_end(void) {
    if (g_perf_metrics.iterations > 0) {
        g_perf_metrics.avg_time_ms /= g_perf_metrics.iterations;
    }
}

static double get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

static void print_test_summary(void) {
    printf("📊 Test Statistics:\n");
    printf("   • Total Tests: %d\n", g_test_stats.total_tests);
    printf("   • Passed: %d (%.1f%%)\n", g_test_stats.passed_tests, 
           (double)g_test_stats.passed_tests / g_test_stats.total_tests * 100.0);
    printf("   • Failed: %d (%.1f%%)\n", g_test_stats.failed_tests,
           (double)g_test_stats.failed_tests / g_test_stats.total_tests * 100.0);
    printf("   • Success Rate: %.1f%%\n", 
           (double)g_test_stats.passed_tests / g_test_stats.total_tests * 100.0);
}

static void print_performance_summary(void) {
    printf("⚡ Performance Metrics:\n");
    printf("   • Min Time: %.3f ms\n", g_perf_metrics.min_time_ms);
    printf("   • Max Time: %.3f ms\n", g_perf_metrics.max_time_ms);
    printf("   • Avg Time: %.3f ms\n", g_perf_metrics.avg_time_ms);
    printf("   • Iterations: %lu\n", g_perf_metrics.iterations);
}

// Enhanced Resolution System Tests
static void test_enhanced_resolution(void) {
    printf("🎯 ENHANCED RESOLUTION SYSTEM TESTS\n");
    printf("===================================\n");

    test_start("Adaptive Resolution Configuration");
    bool result = test_adaptive_resolution_config();
    test_end("Adaptive Resolution Configuration", result);

    test_start("Focus Area Control");
    result = test_focus_area_control();
    test_end("Focus Area Control", result);

    printf("\n");
}

static bool test_adaptive_resolution_config(void) {
    lidar_adaptive_config_t config = {
        .base_resolution_deg = 0.72f,
        .focus_resolution_deg = 0.36f,
        .focus_start_angle_deg = 45.0f,
        .focus_end_angle_deg = 135.0f,
        .enable_adaptive = true,
        .enable_focus_area = true
    };

    // Test configuration
    hal_status_t status = hal_lidar_set_adaptive_resolution(&config);
    if (status != HAL_OK) {
        printf("      ❌ Configuration failed: %d\n", status);
        return false;
    }

    // Test retrieval
    lidar_adaptive_config_t retrieved_config;
    status = hal_lidar_get_adaptive_status(&retrieved_config);
    if (status != HAL_OK) {
        printf("      ❌ Status retrieval failed: %d\n", status);
        return false;
    }

    // Validate values
    if (retrieved_config.base_resolution_deg != config.base_resolution_deg ||
        retrieved_config.focus_resolution_deg != config.focus_resolution_deg ||
        retrieved_config.focus_start_angle_deg != config.focus_start_angle_deg ||
        retrieved_config.focus_end_angle_deg != config.focus_end_angle_deg ||
        retrieved_config.enable_adaptive != config.enable_adaptive ||
        retrieved_config.enable_focus_area != config.enable_focus_area) {
        printf("      ❌ Configuration mismatch\n");
        return false;
    }

    printf("      ✅ Configuration: base=%.2f°, focus=%.2f°, area=%.1f°-%.1f°\n",
           retrieved_config.base_resolution_deg,
           retrieved_config.focus_resolution_deg,
           retrieved_config.focus_start_angle_deg,
           retrieved_config.focus_end_angle_deg);
    return true;
}

static bool test_focus_area_control(void) {
    // Test focus area setting
    hal_status_t status = hal_lidar_set_focus_area(60.0f, 120.0f, 0.18f);
    if (status != HAL_OK) {
        printf("      ❌ Focus area setting failed: %d\n", status);
        return false;
    }

    // Test retrieval
    lidar_adaptive_config_t config;
    status = hal_lidar_get_adaptive_status(&config);
    if (status != HAL_OK) {
        printf("      ❌ Status retrieval failed: %d\n", status);
        return false;
    }

    // Validate focus area
    if (config.focus_start_angle_deg != 60.0f ||
        config.focus_end_angle_deg != 120.0f ||
        config.focus_resolution_deg != 0.18f) {
        printf("      ❌ Focus area mismatch\n");
        return false;
    }

    printf("      ✅ Focus area: %.1f°-%.1f° at %.2f° resolution\n",
           config.focus_start_angle_deg,
           config.focus_end_angle_deg,
           config.focus_resolution_deg);
    return true;
}

// Advanced Multi-Sample & Calibration Tests
static void test_advanced_multi_sample(void) {
    printf("📊 ADVANCED MULTI-SAMPLE & CALIBRATION TESTS\n");
    printf("============================================\n");

    test_start("Accuracy Configuration");
    bool result = test_accuracy_configuration();
    test_end("Accuracy Configuration", result);

    test_start("Calibration System");
    result = test_calibration_system();
    test_end("Calibration System", result);

    printf("\n");
}

static bool test_accuracy_configuration(void) {
    lidar_accuracy_config_t config = {
        .sample_count = 5,
        .sample_interval_ms = 20,
        .enable_outlier_filter = true,
        .outlier_threshold = 20.0f,
        .enable_smoothing = true,
        .smoothing_factor = 0.3f,
        .enable_statistical_averaging = true,
        .confidence_level = 0.95f,
        .enable_weighted_averaging = true,
        .enable_temporal_filtering = true,
        .temporal_window_size = 10,
        .quality_threshold = 0.8f
    };

    // Test configuration
    hal_status_t status = hal_lidar_configure_accuracy(&config);
    if (status != HAL_OK) {
        printf("      ❌ Accuracy configuration failed: %d\n", status);
        return false;
    }

    // Test advanced configuration
    status = hal_lidar_configure_advanced_accuracy(&config);
    if (status != HAL_OK) {
        printf("      ❌ Advanced accuracy configuration failed: %d\n", status);
        return false;
    }

    // Test statistical averaging
    status = hal_lidar_enable_statistical_averaging(true, 0.95f);
    if (status != HAL_OK) {
        printf("      ❌ Statistical averaging failed: %d\n", status);
        return false;
    }

    printf("      ✅ Accuracy: samples=%d, interval=%dms, confidence=%.2f\n",
           config.sample_count, config.sample_interval_ms, config.confidence_level);
    return true;
}

static bool test_calibration_system(void) {
    // Test distance calibration
    hal_status_t status = hal_lidar_calibrate_distance(1000.0f, 1000.0f);
    if (status != HAL_OK) {
        printf("      ❌ Distance calibration failed: %d\n", status);
        return false;
    }

    // Test calibration application
    lidar_calibration_t calibration = {
        .calibration_factor = 1.05f,
        .calibration_offset_mm = 5.0f,
        .enable_auto_calibration = true,
        .calibration_count = 1,
        .calibration_confidence = 0.95f,
        .enable_dynamic_calibration = true,
        .enable_adaptive_calibration = true,
        .calibration_drift_threshold = 0.05f
    };

    status = hal_lidar_apply_calibration(&calibration);
    if (status != HAL_OK) {
        printf("      ❌ Calibration application failed: %d\n", status);
        return false;
    }

    // Test auto calibration
    status = hal_lidar_auto_calibrate();
    if (status != HAL_OK) {
        printf("      ❌ Auto calibration failed: %d\n", status);
        return false;
    }

    // Test multiple point calibration
    lidar_calibration_point_t points[3] = {
        {0.0f, 1000.0f, 1000.0f, 0.0f, 0.95f},
        {90.0f, 2000.0f, 2000.0f, 0.0f, 0.95f},
        {180.0f, 1500.0f, 1500.0f, 0.0f, 0.95f}
    };

    status = hal_lidar_calibrate_multiple_points(points, 3);
    if (status != HAL_OK) {
        printf("      ❌ Multiple point calibration failed: %d\n", status);
        return false;
    }

    // Test drift detection
    float drift_factor;
    bool drift_detected;
    status = hal_lidar_detect_calibration_drift(&drift_factor, &drift_detected);
    if (status != HAL_OK) {
        printf("      ❌ Drift detection failed: %d\n", status);
        return false;
    }

    printf("      ✅ Calibration: factor=%.3f, offset=%.1fmm, drift=%.3f\n",
           calibration.calibration_factor,
           calibration.calibration_offset_mm,
           drift_factor);
    return true;
}

// Multi-Threading & Memory Pool Tests
static void test_multi_threading(void) {
    printf("🧵 MULTI-THREADING & MEMORY POOL TESTS\n");
    printf("======================================\n");

    test_start("Threading System");
    bool result = test_threading_system();
    test_end("Threading System", result);

    test_start("Memory Pool Management");
    result = test_memory_pool_management();
    test_end("Memory Pool Management", result);

    printf("\n");
}

static bool test_threading_system(void) {
    // Configure threading
    lidar_threading_config_t config = {
        .thread_count = 4,
        .thread_stack_size = 65536,
        .thread_priority = 0,
        .enable_parallel_processing = true,
        .enable_thread_priority = true,
        .enable_cpu_affinity = false,
        .cpu_core = 0
    };

    hal_status_t status = hal_lidar_configure_threading(&config);
    if (status != HAL_OK) {
        printf("      ❌ Threading configuration failed: %d\n", status);
        return false;
    }

    // Enable parallel processing
    status = hal_lidar_enable_parallel_processing(true);
    if (status != HAL_OK) {
        printf("      ❌ Parallel processing enable failed: %d\n", status);
        return false;
    }

    // Test thread priority
    status = hal_lidar_set_thread_priority(0, 10);
    if (status != HAL_OK) {
        printf("      ❌ Thread priority setting failed: %d\n", status);
        return false;
    }

    // Test thread affinity (may fail on some systems)
    status = hal_lidar_set_thread_affinity(0, 0);
    // Don't fail test if affinity is not supported
    if (status != HAL_OK && status != HAL_ERROR_NOT_SUPPORTED) {
        printf("      ❌ Thread affinity setting failed: %d\n", status);
        return false;
    }

    printf("      ✅ Threading: %d threads, stack=%zu, priority=%d\n",
           config.thread_count, config.thread_stack_size, config.thread_priority);
    return true;
}

static bool test_memory_pool_management(void) {
    // Configure memory pool
    lidar_memory_pool_t config = {
        .pool_size = 1048576,  // 1MB
        .block_size = 4096,    // 4KB
        .max_blocks = 256,
        .alignment = 64,
        .enable_compaction = true,
        .enable_statistics = true
    };

    hal_status_t status = hal_lidar_configure_memory_pool(&config);
    if (status != HAL_OK) {
        printf("      ❌ Memory pool configuration failed: %d\n", status);
        return false;
    }

    // Test memory allocation
    void *memory_block = NULL;
    status = hal_lidar_allocate_memory_block(MEMORY_TEST_SIZE, &memory_block);
    if (status != HAL_OK) {
        printf("      ❌ Memory allocation failed: %d\n", status);
        return false;
    }

    if (memory_block == NULL) {
        printf("      ❌ Memory block is NULL\n");
        return false;
    }

    // Test memory deallocation
    status = hal_lidar_deallocate_memory_block(memory_block);
    if (status != HAL_OK) {
        printf("      ❌ Memory deallocation failed: %d\n", status);
        return false;
    }

    // Test memory compaction
    status = hal_lidar_compact_memory_pool();
    if (status != HAL_OK) {
        printf("      ❌ Memory compaction failed: %d\n", status);
        return false;
    }

    printf("      ✅ Memory pool: %zu bytes, %zu blocks, alignment=%u\n",
           config.pool_size, config.max_blocks, config.alignment);
    return true;
}

// Adaptive Processing & Hardware Acceleration Tests
static void test_adaptive_processing(void) {
    printf("🚀 ADAPTIVE PROCESSING & HARDWARE ACCELERATION TESTS\n");
    printf("====================================================\n");

    test_start("Adaptive Processing System");
    bool result = test_adaptive_processing_system();
    test_end("Adaptive Processing System", result);

    test_start("Hardware Acceleration");
    result = test_hardware_acceleration();
    test_end("Hardware Acceleration", result);

    test_start("Load Balancing");
    result = test_load_balancing();
    test_end("Load Balancing", result);

    test_start("Performance Scaling");
    result = test_performance_scaling();
    test_end("Performance Scaling", result);

    printf("\n");
}

static bool test_adaptive_processing_system(void) {
    // Configure adaptive processing
    lidar_adaptive_processing_config_t config = {
        .max_algorithms = 6,
        .update_interval_ms = 100,
        .learning_rate = 0.15f,
        .convergence_threshold = 0.01f,
        .performance_window_ms = 1000,
        .enable_optimization = true,
        .enable_scaling = true,
        .enable_balancing = true
    };

    hal_status_t status = hal_lidar_configure_adaptive_processing(&config);
    if (status != HAL_OK) {
        printf("      ❌ Adaptive processing configuration failed: %d\n", status);
        return false;
    }

    // Enable adaptive processing
    status = hal_lidar_enable_adaptive_processing(true);
    if (status != HAL_OK) {
        printf("      ❌ Adaptive processing enable failed: %d\n", status);
        return false;
    }

    // Test performance optimization
    status = hal_lidar_optimize_performance();
    if (status != HAL_OK) {
        printf("      ❌ Performance optimization failed: %d\n", status);
        return false;
    }

    // Test performance metrics
    float efficiency, throughput, latency;
    status = hal_lidar_get_performance_metrics(&efficiency, &throughput, &latency);
    if (status != HAL_OK) {
        printf("      ❌ Performance metrics retrieval failed: %d\n", status);
        return false;
    }

    printf("      ✅ Adaptive processing: algorithms=%d, efficiency=%.1f%%, throughput=%.1f ops/s\n",
           config.max_algorithms, efficiency, throughput);
    return true;
}

static bool test_hardware_acceleration(void) {
    // Configure hardware acceleration
    lidar_hardware_acceleration_config_t config = {
        .max_devices = 3,
        .batch_size = 128,
        .queue_size = 256,
        .enable_gpu = true,
        .enable_dsp = true,
        .enable_neon = true,
        .gpu_utilization = 0.8f,
        .dsp_utilization = 0.7f,
        .neon_utilization = 0.9f
    };

    hal_status_t status = hal_lidar_configure_hardware_acceleration(&config);
    if (status != HAL_OK) {
        printf("      ❌ Hardware acceleration configuration failed: %d\n", status);
        return false;
    }

    // Enable hardware acceleration
    status = hal_lidar_enable_hardware_acceleration(true);
    if (status != HAL_OK) {
        printf("      ❌ Hardware acceleration enable failed: %d\n", status);
        return false;
    }

    printf("      ✅ Hardware acceleration: devices=%d, batch=%d, gpu=%.1f%%, dsp=%.1f%%, neon=%.1f%%\n",
           config.max_devices, config.batch_size,
           config.gpu_utilization * 100.0f,
           config.dsp_utilization * 100.0f,
           config.neon_utilization * 100.0f);
    return true;
}

static bool test_load_balancing(void) {
    // Configure load balancing
    lidar_load_balancing_config_t config = {
        .max_workloads = 8,
        .update_rate_ms = 50,
        .threshold = 0.75f,
        .migration_cost = 0.1f,
        .enable_migration = true,
        .enable_scheduling = true,
        .enable_monitoring = true
    };

    hal_status_t status = hal_lidar_configure_load_balancing(&config);
    if (status != HAL_OK) {
        printf("      ❌ Load balancing configuration failed: %d\n", status);
        return false;
    }

    // Enable load balancing
    status = hal_lidar_enable_load_balancing(true);
    if (status != HAL_OK) {
        printf("      ❌ Load balancing enable failed: %d\n", status);
        return false;
    }

    // Test workload balancing
    status = hal_lidar_balance_workload();
    if (status != HAL_OK) {
        printf("      ❌ Workload balancing failed: %d\n", status);
        return false;
    }

    printf("      ✅ Load balancing: workloads=%d, threshold=%.2f, migration=%.2f\n",
           config.max_workloads, config.threshold, config.migration_cost);
    return true;
}

static bool test_performance_scaling(void) {
    // Configure performance scaling
    lidar_performance_scaling_config_t config = {
        .min_frequency_mhz = 200,
        .max_frequency_mhz = 2400,
        .step_size_mhz = 100,
        .target_latency_ms = 5,
        .power_budget_mw = 8000,
        .enable_dynamic_scaling = true,
        .enable_power_management = true,
        .enable_thermal_control = true
    };

    hal_status_t status = hal_lidar_configure_performance_scaling(&config);
    if (status != HAL_OK) {
        printf("      ❌ Performance scaling configuration failed: %d\n", status);
        return false;
    }

    // Enable performance scaling
    status = hal_lidar_enable_performance_scaling(true);
    if (status != HAL_OK) {
        printf("      ❌ Performance scaling enable failed: %d\n", status);
        return false;
    }

    // Test performance scaling
    status = hal_lidar_scale_performance(1800); // 1.8 GHz
    if (status != HAL_OK) {
        printf("      ❌ Performance scaling failed: %d\n", status);
        return false;
    }

    // Test power consumption
    uint32_t power_mw;
    status = hal_lidar_get_power_consumption(&power_mw);
    if (status != HAL_OK) {
        printf("      ❌ Power consumption retrieval failed: %d\n", status);
        return false;
    }

    // Test thermal status
    float temperature_c;
    status = hal_lidar_get_thermal_status(&temperature_c);
    if (status != HAL_OK) {
        printf("      ❌ Thermal status retrieval failed: %d\n", status);
        return false;
    }

    printf("      ✅ Performance scaling: freq=%d-%d MHz, power=%u mW, temp=%.1f°C\n",
           config.min_frequency_mhz, config.max_frequency_mhz, power_mw, temperature_c);
    return true;
}

// Error Handling Tests
static void test_error_handling(void) {
    printf("⚠️ ERROR HANDLING TESTS\n");
    printf("=======================\n");

    test_start("Invalid Parameter Handling");
    bool result = true;

    // Test NULL parameter handling
    hal_status_t status = hal_lidar_set_adaptive_resolution(NULL);
    if (status != HAL_ERROR_INVALID_PARAM) {
        printf("      ❌ NULL parameter not handled correctly: %d\n", status);
        result = false;
    }

    status = hal_lidar_configure_accuracy(NULL);
    if (status != HAL_ERROR_INVALID_PARAM) {
        printf("      ❌ NULL accuracy config not handled correctly: %d\n", status);
        result = false;
    }

    status = hal_lidar_configure_threading(NULL);
    if (status != HAL_ERROR_INVALID_PARAM) {
        printf("      ❌ NULL threading config not handled correctly: %d\n", status);
        result = false;
    }

    status = hal_lidar_configure_adaptive_processing(NULL);
    if (status != HAL_ERROR_INVALID_PARAM) {
        printf("      ❌ NULL adaptive processing config not handled correctly: %d\n", status);
        result = false;
    }

    // Test invalid values
    lidar_adaptive_config_t invalid_config = {
        .base_resolution_deg = -1.0f,  // Invalid negative value
        .focus_resolution_deg = 0.0f,
        .focus_start_angle_deg = 0.0f,
        .focus_end_angle_deg = 0.0f,
        .enable_adaptive = true,
        .enable_focus_area = true
    };

    status = hal_lidar_set_adaptive_resolution(&invalid_config);
    if (status != HAL_ERROR_INVALID_PARAM) {
        printf("      ❌ Invalid resolution value not handled correctly: %d\n", status);
        result = false;
    }

    test_end("Invalid Parameter Handling", result);

    printf("\n");
}

// Performance Benchmarks
static void test_performance_benchmarks(void) {
    printf("⚡ PERFORMANCE BENCHMARKS\n");
    printf("=========================\n");

    benchmark_adaptive_resolution();
    benchmark_multi_sample_processing();
    benchmark_threading_performance();
    benchmark_memory_allocation();
    benchmark_adaptive_processing();

    printf("\n");
}

static void benchmark_adaptive_resolution(void) {
    printf("🎯 Benchmarking Adaptive Resolution...\n");
    performance_start();

    lidar_adaptive_config_t config = {
        .base_resolution_deg = 0.72f,
        .focus_resolution_deg = 0.36f,
        .focus_start_angle_deg = 45.0f,
        .focus_end_angle_deg = 135.0f,
        .enable_adaptive = true,
        .enable_focus_area = true
    };

    for (uint64_t i = 0; i < PERFORMANCE_TEST_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_set_adaptive_resolution(&config);
        hal_lidar_get_adaptive_status(&config);
        
        double end_time = get_time_ms();
        double duration = end_time - start_time;
        
        if (duration < g_perf_metrics.min_time_ms) g_perf_metrics.min_time_ms = duration;
        if (duration > g_perf_metrics.max_time_ms) g_perf_metrics.max_time_ms = duration;
        g_perf_metrics.avg_time_ms += duration;
        g_perf_metrics.iterations++;
    }

    performance_end();
    printf("   📊 Adaptive Resolution: min=%.3fms, max=%.3fms, avg=%.3fms\n",
           g_perf_metrics.min_time_ms, g_perf_metrics.max_time_ms, g_perf_metrics.avg_time_ms);
}

static void benchmark_multi_sample_processing(void) {
    printf("📊 Benchmarking Multi-Sample Processing...\n");
    performance_start();

    lidar_accuracy_config_t config = {
        .sample_count = 5,
        .sample_interval_ms = 20,
        .enable_outlier_filter = true,
        .outlier_threshold = 20.0f,
        .enable_smoothing = true,
        .smoothing_factor = 0.3f,
        .enable_statistical_averaging = true,
        .confidence_level = 0.95f
    };

    for (uint64_t i = 0; i < PERFORMANCE_TEST_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_configure_accuracy(&config);
        hal_lidar_enable_statistical_averaging(true, 0.95f);
        
        double end_time = get_time_ms();
        double duration = end_time - start_time;
        
        if (duration < g_perf_metrics.min_time_ms) g_perf_metrics.min_time_ms = duration;
        if (duration > g_perf_metrics.max_time_ms) g_perf_metrics.max_time_ms = duration;
        g_perf_metrics.avg_time_ms += duration;
        g_perf_metrics.iterations++;
    }

    performance_end();
    printf("   📊 Multi-Sample Processing: min=%.3fms, max=%.3fms, avg=%.3fms\n",
           g_perf_metrics.min_time_ms, g_perf_metrics.max_time_ms, g_perf_metrics.avg_time_ms);
}

static void benchmark_threading_performance(void) {
    printf("🧵 Benchmarking Threading Performance...\n");
    performance_start();

    lidar_threading_config_t config = {
        .thread_count = 4,
        .thread_stack_size = 65536,
        .thread_priority = 0,
        .enable_parallel_processing = true,
        .enable_thread_priority = true,
        .enable_cpu_affinity = false
    };

    for (uint64_t i = 0; i < PERFORMANCE_TEST_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_configure_threading(&config);
        hal_lidar_enable_parallel_processing(true);
        
        double end_time = get_time_ms();
        double duration = end_time - start_time;
        
        if (duration < g_perf_metrics.min_time_ms) g_perf_metrics.min_time_ms = duration;
        if (duration > g_perf_metrics.max_time_ms) g_perf_metrics.max_time_ms = duration;
        g_perf_metrics.avg_time_ms += duration;
        g_perf_metrics.iterations++;
    }

    performance_end();
    printf("   📊 Threading Performance: min=%.3fms, max=%.3fms, avg=%.3fms\n",
           g_perf_metrics.min_time_ms, g_perf_metrics.max_time_ms, g_perf_metrics.avg_time_ms);
}

static void benchmark_memory_allocation(void) {
    printf("💾 Benchmarking Memory Allocation...\n");
    performance_start();

    lidar_memory_pool_t config = {
        .pool_size = 1048576,
        .block_size = 4096,
        .max_blocks = 256,
        .alignment = 64,
        .enable_compaction = true,
        .enable_statistics = true
    };

    hal_lidar_configure_memory_pool(&config);

    for (uint64_t i = 0; i < PERFORMANCE_TEST_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        void *memory_block = NULL;
        hal_lidar_allocate_memory_block(MEMORY_TEST_SIZE, &memory_block);
        if (memory_block) {
            hal_lidar_deallocate_memory_block(memory_block);
        }
        
        double end_time = get_time_ms();
        double duration = end_time - start_time;
        
        if (duration < g_perf_metrics.min_time_ms) g_perf_metrics.min_time_ms = duration;
        if (duration > g_perf_metrics.max_time_ms) g_perf_metrics.max_time_ms = duration;
        g_perf_metrics.avg_time_ms += duration;
        g_perf_metrics.iterations++;
    }

    performance_end();
    printf("   📊 Memory Allocation: min=%.3fms, max=%.3fms, avg=%.3fms\n",
           g_perf_metrics.min_time_ms, g_perf_metrics.max_time_ms, g_perf_metrics.avg_time_ms);
}

static void benchmark_adaptive_processing(void) {
    printf("🚀 Benchmarking Adaptive Processing...\n");
    performance_start();

    lidar_adaptive_processing_config_t config = {
        .max_algorithms = 6,
        .update_interval_ms = 100,
        .learning_rate = 0.15f,
        .convergence_threshold = 0.01f,
        .performance_window_ms = 1000,
        .enable_optimization = true,
        .enable_scaling = true,
        .enable_balancing = true
    };

    hal_lidar_configure_adaptive_processing(&config);
    hal_lidar_enable_adaptive_processing(true);

    for (uint64_t i = 0; i < PERFORMANCE_TEST_ITERATIONS; i++) {
        double start_time = get_time_ms();
        
        hal_lidar_optimize_performance();
        hal_lidar_scale_performance(1800);
        hal_lidar_balance_workload();
        
        double end_time = get_time_ms();
        double duration = end_time - start_time;
        
        if (duration < g_perf_metrics.min_time_ms) g_perf_metrics.min_time_ms = duration;
        if (duration > g_perf_metrics.max_time_ms) g_perf_metrics.max_time_ms = duration;
        g_perf_metrics.avg_time_ms += duration;
        g_perf_metrics.iterations++;
    }

    performance_end();
    printf("   📊 Adaptive Processing: min=%.3fms, max=%.3fms, avg=%.3fms\n",
           g_perf_metrics.min_time_ms, g_perf_metrics.max_time_ms, g_perf_metrics.avg_time_ms);
}
