/**
 * @file performance_metrics.c
 * @brief Performance Metrics System Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include "performance_metrics.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

// Internal performance metrics structure
static struct {
    performance_metrics_t metrics;
    performance_config_t config;
    bool initialized;
    uint64_t start_time_us;
    uint64_t last_update_time_us;
} g_performance_metrics = {0};

// Internal function prototypes
static uint64_t get_timestamp_us(void);
static hal_status_t update_system_metrics(void);
static hal_status_t update_communication_metrics(void);
static hal_status_t update_safety_metrics(void);

/**
 * @brief Initialize performance metrics system
 * @param config Performance configuration
 * @return HAL status
 */
hal_status_t performance_metrics_init(const performance_config_t *config)
{
    if (g_performance_metrics.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize structure
    memset(&g_performance_metrics, 0, sizeof(g_performance_metrics));
    
    // Copy configuration
    memcpy(&g_performance_metrics.config, config, sizeof(performance_config_t));
    
    // Initialize metrics
    memset(&g_performance_metrics.metrics, 0, sizeof(performance_metrics_t));
    
    // Set start time
    g_performance_metrics.start_time_us = get_timestamp_us();
    g_performance_metrics.metrics.startup_time_ms = 0; // Will be updated on first update
    g_performance_metrics.last_update_time_us = g_performance_metrics.start_time_us;
    
    g_performance_metrics.initialized = true;
    
    printf("[PERF] Performance metrics system initialized\n");
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize performance metrics system
 * @return HAL status
 */
hal_status_t performance_metrics_deinit(void)
{
    if (!g_performance_metrics.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_performance_metrics.initialized = false;
    printf("[PERF] Performance metrics system deinitialized\n");
    return HAL_STATUS_OK;
}

/**
 * @brief Update all performance metrics
 * @return HAL status
 */
hal_status_t performance_metrics_update(void)
{
    if (!g_performance_metrics.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = get_timestamp_us();
    
    // Update uptime
    g_performance_metrics.metrics.uptime_seconds = (current_time - g_performance_metrics.start_time_us) / 1000000ULL;
    
    // Update startup time on first update
    if (g_performance_metrics.metrics.startup_time_ms == 0) {
        g_performance_metrics.metrics.startup_time_ms = (current_time - g_performance_metrics.start_time_us) / 1000ULL;
    }
    
    // Update system metrics if enabled
    if (g_performance_metrics.config.enable_cpu_monitoring || 
        g_performance_metrics.config.enable_memory_monitoring) {
        update_system_metrics();
    }
    
    // Update communication metrics if enabled
    if (g_performance_metrics.config.enable_communication_monitoring) {
        update_communication_metrics();
    }
    
    // Update safety metrics if enabled
    if (g_performance_metrics.config.enable_safety_monitoring) {
        update_safety_metrics();
    }
    
    g_performance_metrics.metrics.last_update_time_us = current_time;
    g_performance_metrics.last_update_time_us = current_time;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get current performance metrics
 * @param metrics Pointer to metrics structure
 * @return HAL status
 */
hal_status_t performance_metrics_get(performance_metrics_t *metrics)
{
    if (!g_performance_metrics.initialized || metrics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(metrics, &g_performance_metrics.metrics, sizeof(performance_metrics_t));
    return HAL_STATUS_OK;
}

/**
 * @brief Reset performance metrics
 * @return HAL status
 */
hal_status_t performance_metrics_reset(void)
{
    if (!g_performance_metrics.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = get_timestamp_us();
    
    // Reset all metrics except timestamps
    memset(&g_performance_metrics.metrics, 0, sizeof(performance_metrics_t));
    
    // Preserve timestamps
    g_performance_metrics.metrics.last_update_time_us = current_time;
    g_performance_metrics.metrics.last_metrics_reset_time_us = current_time;
    
    printf("[PERF] Performance metrics reset\n");
    return HAL_STATUS_OK;
}

/**
 * @brief Update RS485 performance metrics
 * @param latency_ms Latency in milliseconds
 * @param throughput_bps Throughput in bits per second
 * @param error_rate Error rate percentage
 * @return HAL status
 */
hal_status_t performance_metrics_update_rs485(uint32_t latency_ms, uint32_t throughput_bps, uint32_t error_rate)
{
    if (!g_performance_metrics.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_performance_metrics.metrics.rs485_latency_ms = latency_ms;
    g_performance_metrics.metrics.rs485_throughput_bps = throughput_bps;
    g_performance_metrics.metrics.rs485_error_rate_percent = error_rate;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Update E-Stop performance metrics
 * @param response_time_ms Response time in milliseconds
 * @return HAL status
 */
hal_status_t performance_metrics_update_estop(uint32_t response_time_ms)
{
    if (!g_performance_metrics.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_performance_metrics.metrics.estop_response_time_ms = response_time_ms;
    return HAL_STATUS_OK;
}

/**
 * @brief Update state transition performance metrics
 * @param transition_time_ms Transition time in milliseconds
 * @return HAL status
 */
hal_status_t performance_metrics_update_state_transition(uint32_t transition_time_ms)
{
    if (!g_performance_metrics.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_performance_metrics.metrics.state_transition_time_ms = transition_time_ms;
    g_performance_metrics.metrics.state_transition_count++;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Update module discovery performance metrics
 * @param discovery_time_ms Discovery time in milliseconds
 * @return HAL status
 */
hal_status_t performance_metrics_update_module_discovery(uint32_t discovery_time_ms)
{
    if (!g_performance_metrics.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_performance_metrics.metrics.module_discovery_time_ms = discovery_time_ms;
    return HAL_STATUS_OK;
}

/**
 * @brief Start performance timer
 * @param start_time Pointer to store start time
 * @return HAL status
 */
hal_status_t performance_metrics_start_timer(uint64_t *start_time)
{
    if (!g_performance_metrics.initialized || start_time == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *start_time = get_timestamp_us();
    return HAL_STATUS_OK;
}

/**
 * @brief End performance timer and calculate elapsed time
 * @param start_time Start time from start_timer
 * @param elapsed_ms Pointer to store elapsed time in milliseconds
 * @return HAL status
 */
hal_status_t performance_metrics_end_timer(uint64_t start_time, uint32_t *elapsed_ms)
{
    if (!g_performance_metrics.initialized || elapsed_ms == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    uint64_t end_time = get_timestamp_us();
    *elapsed_ms = (uint32_t)((end_time - start_time) / 1000ULL);
    
    return HAL_STATUS_OK;
}

// Internal helper functions

/**
 * @brief Get current timestamp in microseconds
 * @return Timestamp in microseconds
 */
static uint64_t get_timestamp_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

/**
 * @brief Update system performance metrics
 * @return HAL status
 */
static hal_status_t update_system_metrics(void)
{
    // Simple CPU usage estimation (placeholder)
    if (g_performance_metrics.config.enable_cpu_monitoring) {
        // In a real implementation, this would read /proc/stat
        g_performance_metrics.metrics.cpu_usage_percent = 15; // Placeholder
    }
    
    // Simple memory usage estimation (placeholder)
    if (g_performance_metrics.config.enable_memory_monitoring) {
        // In a real implementation, this would read /proc/meminfo
        g_performance_metrics.metrics.memory_usage_percent = 25; // Placeholder
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Update communication performance metrics
 * @return HAL status
 */
static hal_status_t update_communication_metrics(void)
{
    // This would be updated by actual communication functions
    // For now, just maintain current values
    return HAL_STATUS_OK;
}

/**
 * @brief Update safety performance metrics
 * @return HAL status
 */
static hal_status_t update_safety_metrics(void)
{
    // This would be updated by actual safety functions
    // For now, just maintain current values
    return HAL_STATUS_OK;
}
