#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "performance_manager.h"
#include "diagnostics_manager.h"
#include "hal_common.h"

// Test results structure
typedef struct {
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t skipped_tests;
} test_results_t;

static test_results_t g_test_results = {0};

// Test utilities
static void test_init(const char *test_name) {
    printf("\n=== Testing: %s ===\n", test_name);
    g_test_results.total_tests = 0;
    g_test_results.passed_tests = 0;
    g_test_results.failed_tests = 0;
    g_test_results.skipped_tests = 0;
}

static void test_assert(bool condition, const char *test_name) {
    g_test_results.total_tests++;
    if (condition) {
        printf("✓ %s\n", test_name);
        g_test_results.passed_tests++;
    } else {
        printf("✗ %s\n", test_name);
        g_test_results.failed_tests++;
    }
}

static void test_skip(const char *test_name) {
    printf("- %s (SKIPPED)\n", test_name);
    g_test_results.skipped_tests++;
}

static void test_summary(void) {
    printf("\n--- Test Summary ---\n");
    printf("Total: %d, Passed: %d, Failed: %d, Skipped: %d\n", 
           g_test_results.total_tests, g_test_results.passed_tests, 
           g_test_results.failed_tests, g_test_results.skipped_tests);
    
    if (g_test_results.failed_tests == 0) {
        printf("✓ All tests PASSED!\n");
    } else {
        printf("✗ Some tests FAILED!\n");
    }
}

// Performance Manager Tests
static void test_performance_manager_basic(void) {
    test_init("Performance Manager Basic Functions");
    
    // Test initialization
    hal_status_t status = performance_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Performance Manager initialization");
    
    // Test configuration
    performance_mgr_config_t config;
    status = performance_manager_get_config(&config);
    test_assert(status == HAL_STATUS_OK, "Get performance configuration");
    test_assert(config.sample_interval_ms == PERFORMANCE_MGR_SAMPLE_INTERVAL_MS, "Default sample interval");
    test_assert(config.auto_optimization_enabled == true, "Auto optimization enabled by default");
    
    // Test status
    performance_mgr_status_t status_info;
    status = performance_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get performance status");
    test_assert(status_info.initialized == true, "Performance manager initialized");
    test_assert(status_info.monitoring_active == false, "Monitoring not active initially");
    
    // Test statistics
    performance_mgr_stats_t stats;
    status = performance_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get performance statistics");
    
    // Test deinitialization
    status = performance_manager_deinit();
    test_assert(status == HAL_STATUS_OK, "Performance Manager deinitialization");
    
    test_summary();
}

static void test_performance_manager_metrics(void) {
    test_init("Performance Manager Metrics");
    
    // Initialize
    hal_status_t status = performance_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Performance Manager initialization");
    
    // Register metrics
    status = performance_manager_register_metric(PERFORMANCE_METRIC_CPU_USAGE, "CPU Usage", 70.0f, 85.0f, 95.0f);
    test_assert(status == HAL_STATUS_OK, "Register CPU usage metric");
    
    status = performance_manager_register_metric(PERFORMANCE_METRIC_MEMORY_USAGE, "Memory Usage", 75.0f, 85.0f, 95.0f);
    test_assert(status == HAL_STATUS_OK, "Register memory usage metric");
    
    status = performance_manager_register_metric(PERFORMANCE_METRIC_RESPONSE_TIME, "Response Time", 50.0f, 100.0f, 200.0f);
    test_assert(status == HAL_STATUS_OK, "Register response time metric");
    
    // Update metrics
    status = performance_manager_update_metric(PERFORMANCE_METRIC_CPU_USAGE, 45.5f);
    test_assert(status == HAL_STATUS_OK, "Update CPU usage metric");
    
    status = performance_manager_update_metric(PERFORMANCE_METRIC_MEMORY_USAGE, 62.3f);
    test_assert(status == HAL_STATUS_OK, "Update memory usage metric");
    
    status = performance_manager_update_metric(PERFORMANCE_METRIC_RESPONSE_TIME, 35.7f);
    test_assert(status == HAL_STATUS_OK, "Update response time metric");
    
    // Get metric data
    performance_metric_t cpu_metric;
    status = performance_manager_get_metric(PERFORMANCE_METRIC_CPU_USAGE, &cpu_metric);
    test_assert(status == HAL_STATUS_OK, "Get CPU usage metric");
    test_assert(cpu_metric.current_value == 45.5f, "CPU usage value correct");
    test_assert(cpu_metric.sample_count > 0, "Sample count incremented");
    
    // Test threshold violation
    status = performance_manager_update_metric(PERFORMANCE_METRIC_CPU_USAGE, 90.0f);
    test_assert(status == HAL_STATUS_OK, "Update CPU usage to threshold level");
    
    // Get all metrics
    performance_metric_t metrics[10];
    status = performance_manager_get_all_metrics(metrics, 10);
    test_assert(status == HAL_STATUS_OK, "Get all metrics");
    
    // Cleanup
    performance_manager_deinit();
    
    test_summary();
}

static void test_performance_manager_monitoring(void) {
    test_init("Performance Manager Monitoring");
    
    // Initialize
    hal_status_t status = performance_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Performance Manager initialization");
    
    // Register metrics
    performance_manager_register_metric(PERFORMANCE_METRIC_CPU_USAGE, "CPU Usage", 70.0f, 85.0f, 95.0f);
    performance_manager_register_metric(PERFORMANCE_METRIC_MEMORY_USAGE, "Memory Usage", 75.0f, 85.0f, 95.0f);
    
    // Start monitoring
    status = performance_manager_start_monitoring();
    test_assert(status == HAL_STATUS_OK, "Start performance monitoring");
    
    // Update metrics multiple times
    for (int i = 0; i < 5; i++) {
        performance_manager_update_metric(PERFORMANCE_METRIC_CPU_USAGE, 50.0f + i * 5.0f);
        performance_manager_update_metric(PERFORMANCE_METRIC_MEMORY_USAGE, 60.0f + i * 3.0f);
        usleep(100000);  // 100ms delay
    }
    
    // Check statistics
    performance_mgr_stats_t stats;
    status = performance_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get performance statistics");
    test_assert(stats.total_samples > 0, "Samples recorded");
    test_assert(stats.current_cpu_usage_percent > 0, "CPU usage recorded");
    test_assert(stats.current_memory_usage_percent > 0, "Memory usage recorded");
    
    // Stop monitoring
    status = performance_manager_stop_monitoring();
    test_assert(status == HAL_STATUS_OK, "Stop performance monitoring");
    
    // Get diagnostics
    char diagnostics[1024];
    status = performance_manager_get_diagnostics(diagnostics, sizeof(diagnostics));
    test_assert(status == HAL_STATUS_OK, "Get performance diagnostics");
    test_assert(strlen(diagnostics) > 0, "Diagnostics content");
    
    // Cleanup
    performance_manager_deinit();
    
    test_summary();
}

// Diagnostics Manager Tests
static void test_diagnostics_manager_basic(void) {
    test_init("Diagnostics Manager Basic Functions");
    
    // Test initialization
    hal_status_t status = diagnostics_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Diagnostics Manager initialization");
    
    // Test configuration
    diagnostics_mgr_config_t config;
    status = diagnostics_manager_get_config(&config);
    test_assert(status == HAL_STATUS_OK, "Get diagnostics configuration");
    test_assert(config.health_check_interval_ms == DIAGNOSTICS_MGR_HEALTH_CHECK_INTERVAL_MS, "Default health check interval");
    test_assert(config.auto_health_check_enabled == true, "Auto health check enabled by default");
    
    // Test status
    diagnostics_mgr_status_t status_info;
    status = diagnostics_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get diagnostics status");
    test_assert(status_info.initialized == true, "Diagnostics manager initialized");
    test_assert(status_info.health_monitoring_active == false, "Health monitoring not active initially");
    
    // Test statistics
    diagnostics_mgr_stats_t stats;
    status = diagnostics_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get diagnostics statistics");
    
    // Test deinitialization
    status = diagnostics_manager_deinit();
    test_assert(status == HAL_STATUS_OK, "Diagnostics Manager deinitialization");
    
    test_summary();
}

static void test_diagnostics_manager_components(void) {
    test_init("Diagnostics Manager Component Health");
    
    // Initialize
    hal_status_t status = diagnostics_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Diagnostics Manager initialization");
    
    // Register components
    status = diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_NETWORK, "Network Component");
    test_assert(status == HAL_STATUS_OK, "Register network component");
    
    status = diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_SECURITY, "Security Component");
    test_assert(status == HAL_STATUS_OK, "Register security component");
    
    status = diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_API, "API Component");
    test_assert(status == HAL_STATUS_OK, "Register API component");
    
    // Update component health
    status = diagnostics_manager_update_component_health(DIAGNOSTICS_COMPONENT_NETWORK, 
                                                       DIAGNOSTICS_HEALTH_EXCELLENT, 95.5f, 
                                                       "Network operating normally");
    test_assert(status == HAL_STATUS_OK, "Update network component health");
    
    status = diagnostics_manager_update_component_health(DIAGNOSTICS_COMPONENT_SECURITY, 
                                                       DIAGNOSTICS_HEALTH_GOOD, 87.2f, 
                                                       "Security systems active");
    test_assert(status == HAL_STATUS_OK, "Update security component health");
    
    status = diagnostics_manager_update_component_health(DIAGNOSTICS_COMPONENT_API, 
                                                       DIAGNOSTICS_HEALTH_FAIR, 72.8f, 
                                                       "API performance degraded");
    test_assert(status == HAL_STATUS_OK, "Update API component health");
    
    // Get component health
    diagnostics_component_health_t network_health;
    status = diagnostics_manager_get_component_health(DIAGNOSTICS_COMPONENT_NETWORK, &network_health);
    test_assert(status == HAL_STATUS_OK, "Get network component health");
    test_assert(network_health.health_level == DIAGNOSTICS_HEALTH_EXCELLENT, "Network health level correct");
    test_assert(network_health.health_score == 95.5f, "Network health score correct");
    
    // Get all component health
    diagnostics_component_health_t all_health[10];
    status = diagnostics_manager_get_all_component_health(all_health, 10);
    test_assert(status == HAL_STATUS_OK, "Get all component health");
    
    // Check overall system health
    diagnostics_mgr_status_t status_info;
    status = diagnostics_manager_get_status(&status_info);
    test_assert(status == HAL_STATUS_OK, "Get diagnostics status");
    test_assert(status_info.overall_system_health > 0.0f, "Overall system health calculated");
    
    // Cleanup
    diagnostics_manager_deinit();
    
    test_summary();
}

static void test_diagnostics_manager_health_monitoring(void) {
    test_init("Diagnostics Manager Health Monitoring");
    
    // Initialize
    hal_status_t status = diagnostics_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Diagnostics Manager initialization");
    
    // Register components
    diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_NETWORK, "Network Component");
    diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_SECURITY, "Security Component");
    diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_API, "API Component");
    
    // Start health monitoring
    status = diagnostics_manager_start_health_monitoring();
    test_assert(status == HAL_STATUS_OK, "Start health monitoring");
    
    // Perform health check
    status = diagnostics_manager_perform_health_check();
    test_assert(status == HAL_STATUS_OK, "Perform health check");
    
    // Check statistics
    diagnostics_mgr_stats_t stats;
    status = diagnostics_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get diagnostics statistics");
    test_assert(stats.total_health_checks > 0, "Health checks recorded");
    
    // Generate alert
    status = diagnostics_manager_generate_alert(DIAGNOSTICS_ALERT_WARNING, 
                                              DIAGNOSTICS_COMPONENT_NETWORK, 
                                              "Network Performance Warning", 
                                              "Network latency increased above threshold");
    test_assert(status == HAL_STATUS_OK, "Generate network alert");
    
    // Log event
    status = diagnostics_manager_log_event(DIAGNOSTICS_COMPONENT_SECURITY, 
                                         DIAGNOSTICS_ALERT_INFO, 
                                         "Security scan completed", 
                                         "All security checks passed successfully");
    test_assert(status == HAL_STATUS_OK, "Log security event");
    
    // Perform full diagnostics
    status = diagnostics_manager_perform_full_diagnostics();
    test_assert(status == HAL_STATUS_OK, "Perform full diagnostics");
    
    // Stop health monitoring
    status = diagnostics_manager_stop_health_monitoring();
    test_assert(status == HAL_STATUS_OK, "Stop health monitoring");
    
    // Get diagnostics
    char diagnostics[1024];
    status = diagnostics_manager_get_diagnostics(diagnostics, sizeof(diagnostics));
    test_assert(status == HAL_STATUS_OK, "Get diagnostics information");
    test_assert(strlen(diagnostics) > 0, "Diagnostics content");
    
    // Cleanup
    diagnostics_manager_deinit();
    
    test_summary();
}

static void test_diagnostics_manager_tests(void) {
    test_init("Diagnostics Manager Tests");
    
    // Initialize
    hal_status_t status = diagnostics_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Diagnostics Manager initialization");
    
    // Run all tests
    status = diagnostics_manager_run_all_tests();
    test_assert(status == HAL_STATUS_OK, "Run all diagnostics tests");
    
    // Check test statistics
    diagnostics_mgr_stats_t stats;
    status = diagnostics_manager_get_statistics(&stats);
    test_assert(status == HAL_STATUS_OK, "Get diagnostics statistics");
    test_assert(stats.total_tests_run > 0, "Tests executed");
    test_assert(stats.tests_passed > 0, "Tests passed");
    
    // Enable predictive maintenance
    status = diagnostics_manager_enable_predictive_maintenance();
    test_assert(status == HAL_STATUS_OK, "Enable predictive maintenance");
    
    // Analyze trends
    status = diagnostics_manager_analyze_trends();
    test_assert(status == HAL_STATUS_OK, "Analyze trends");
    
    // Disable predictive maintenance
    status = diagnostics_manager_disable_predictive_maintenance();
    test_assert(status == HAL_STATUS_OK, "Disable predictive maintenance");
    
    // Cleanup
    diagnostics_manager_deinit();
    
    test_summary();
}

// Integration Tests
static void test_week4_integration(void) {
    test_init("Week 4 Integration");
    
    // Initialize both managers
    hal_status_t status = performance_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Performance Manager initialization");
    
    status = diagnostics_manager_init(NULL);
    test_assert(status == HAL_STATUS_OK, "Diagnostics Manager initialization");
    
    // Register performance metrics
    performance_manager_register_metric(PERFORMANCE_METRIC_CPU_USAGE, "CPU Usage", 70.0f, 85.0f, 95.0f);
    performance_manager_register_metric(PERFORMANCE_METRIC_MEMORY_USAGE, "Memory Usage", 75.0f, 85.0f, 95.0f);
    
    // Register diagnostic components
    diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_PERFORMANCE, "Performance Component");
    diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_SYSTEM, "System Component");
    
    // Start monitoring
    performance_manager_start_monitoring();
    diagnostics_manager_start_health_monitoring();
    
    // Simulate system operation
    for (int i = 0; i < 3; i++) {
        // Update performance metrics
        performance_manager_update_metric(PERFORMANCE_METRIC_CPU_USAGE, 50.0f + i * 10.0f);
        performance_manager_update_metric(PERFORMANCE_METRIC_MEMORY_USAGE, 60.0f + i * 5.0f);
        
        // Update component health
        diagnostics_manager_update_component_health(DIAGNOSTICS_COMPONENT_PERFORMANCE, 
                                                  DIAGNOSTICS_HEALTH_EXCELLENT, 90.0f + i * 2.0f, 
                                                  "Performance monitoring active");
        
        usleep(200000);  // 200ms delay
    }
    
    // Check integration
    performance_mgr_status_t perf_status;
    status = performance_manager_get_status(&perf_status);
    test_assert(status == HAL_STATUS_OK, "Get performance status");
    
    diagnostics_mgr_status_t diag_status;
    status = diagnostics_manager_get_status(&diag_status);
    test_assert(status == HAL_STATUS_OK, "Get diagnostics status");
    
    // Generate integrated alert
    diagnostics_manager_generate_alert(DIAGNOSTICS_ALERT_INFO, 
                                     DIAGNOSTICS_COMPONENT_PERFORMANCE, 
                                     "Performance Optimization", 
                                     "System performance optimized successfully");
    
    // Get diagnostics from both managers
    char perf_diagnostics[1024];
    status = performance_manager_get_diagnostics(perf_diagnostics, sizeof(perf_diagnostics));
    test_assert(status == HAL_STATUS_OK, "Get performance diagnostics");
    
    char diag_diagnostics[1024];
    status = diagnostics_manager_get_diagnostics(diag_diagnostics, sizeof(diag_diagnostics));
    test_assert(status == HAL_STATUS_OK, "Get diagnostics information");
    
    // Stop monitoring
    performance_manager_stop_monitoring();
    diagnostics_manager_stop_health_monitoring();
    
    // Cleanup
    performance_manager_deinit();
    diagnostics_manager_deinit();
    
    test_summary();
}

// Main test runner
int main(void) {
    printf("=== FW TEAM - WEEK 4 MODULES TEST SUITE ===\n");
    printf("Testing Performance Manager and Diagnostics Manager\n");
    printf("Date: 2025-01-27\n");
    printf("Team: FW\n");
    printf("Task: FW-08 (Week 4 Testing)\n\n");
    
    uint32_t total_tests = 0;
    uint32_t total_passed = 0;
    uint32_t total_failed = 0;
    uint32_t total_skipped = 0;
    
    // Run Performance Manager tests
    test_performance_manager_basic();
    total_tests += g_test_results.total_tests;
    total_passed += g_test_results.passed_tests;
    total_failed += g_test_results.failed_tests;
    total_skipped += g_test_results.skipped_tests;
    
    test_performance_manager_metrics();
    total_tests += g_test_results.total_tests;
    total_passed += g_test_results.passed_tests;
    total_failed += g_test_results.failed_tests;
    total_skipped += g_test_results.skipped_tests;
    
    test_performance_manager_monitoring();
    total_tests += g_test_results.total_tests;
    total_passed += g_test_results.passed_tests;
    total_failed += g_test_results.failed_tests;
    total_skipped += g_test_results.skipped_tests;
    
    // Run Diagnostics Manager tests
    test_diagnostics_manager_basic();
    total_tests += g_test_results.total_tests;
    total_passed += g_test_results.passed_tests;
    total_failed += g_test_results.failed_tests;
    total_skipped += g_test_results.skipped_tests;
    
    test_diagnostics_manager_components();
    total_tests += g_test_results.total_tests;
    total_passed += g_test_results.passed_tests;
    total_failed += g_test_results.failed_tests;
    total_skipped += g_test_results.skipped_tests;
    
    test_diagnostics_manager_health_monitoring();
    total_tests += g_test_results.total_tests;
    total_passed += g_test_results.passed_tests;
    total_failed += g_test_results.failed_tests;
    total_skipped += g_test_results.skipped_tests;
    
    test_diagnostics_manager_tests();
    total_tests += g_test_results.total_tests;
    total_passed += g_test_results.passed_tests;
    total_failed += g_test_results.failed_tests;
    total_skipped += g_test_results.skipped_tests;
    
    // Run integration tests
    test_week4_integration();
    total_tests += g_test_results.total_tests;
    total_passed += g_test_results.passed_tests;
    total_failed += g_test_results.failed_tests;
    total_skipped += g_test_results.skipped_tests;
    
    // Final summary
    printf("\n=== FINAL TEST SUMMARY ===\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", total_passed);
    printf("Failed: %d\n", total_failed);
    printf("Skipped: %d\n", total_skipped);
    
    float success_rate = total_tests > 0 ? (float)total_passed / total_tests * 100.0f : 0.0f;
    printf("Success Rate: %.1f%%\n", success_rate);
    
    if (total_failed == 0) {
        printf("\n🎉 All tests PASSED! Week 4 modules are ready for production!\n");
        return 0;
    } else {
        printf("\n⚠️  Some tests failed. Please review and fix issues.\n");
        return 1;
    }
}
