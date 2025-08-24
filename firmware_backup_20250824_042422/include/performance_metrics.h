/**
 * @file performance_metrics.h
 * @brief Performance Metrics System for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#ifndef PERFORMANCE_METRICS_H
#define PERFORMANCE_METRICS_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Performance metrics structure
typedef struct {
    // System performance
    uint64_t startup_time_ms;
    uint64_t uptime_seconds;
    uint32_t cpu_usage_percent;
    uint32_t memory_usage_percent;
    
    // Communication performance
    uint32_t rs485_latency_ms;
    uint32_t rs485_throughput_bps;
    uint32_t rs485_error_rate_percent;
    uint32_t rs485_retry_count;
    
    // Safety performance
    uint32_t estop_response_time_ms;
    uint32_t fault_detection_time_ms;
    uint32_t safety_check_interval_ms;
    
    // State machine performance
    uint32_t state_transition_time_ms;
    uint32_t state_transition_count;
    uint32_t event_processing_time_ms;
    
    // Module performance
    uint32_t module_discovery_time_ms;
    uint32_t module_communication_latency_ms;
    uint32_t module_error_count;
    
    // Timestamps
    uint64_t last_update_time_us;
    uint64_t last_metrics_reset_time_us;
} performance_metrics_t;

// Performance metrics configuration
typedef struct {
    bool enable_cpu_monitoring;
    bool enable_memory_monitoring;
    bool enable_communication_monitoring;
    bool enable_safety_monitoring;
    uint32_t metrics_update_interval_ms;
    uint32_t metrics_history_size;
} performance_config_t;

// Function prototypes
hal_status_t performance_metrics_init(const performance_config_t *config);
hal_status_t performance_metrics_deinit(void);
hal_status_t performance_metrics_update(void);
hal_status_t performance_metrics_get(performance_metrics_t *metrics);
hal_status_t performance_metrics_reset(void);

// Specific metric updates
hal_status_t performance_metrics_update_rs485(uint32_t latency_ms, uint32_t throughput_bps, uint32_t error_rate);
hal_status_t performance_metrics_update_estop(uint32_t response_time_ms);
hal_status_t performance_metrics_update_state_transition(uint32_t transition_time_ms);
hal_status_t performance_metrics_update_module_discovery(uint32_t discovery_time_ms);

// Performance monitoring functions
hal_status_t performance_metrics_start_timer(uint64_t *start_time);
hal_status_t performance_metrics_end_timer(uint64_t start_time, uint32_t *elapsed_ms);

#ifdef __cplusplus
}
#endif

#endif // PERFORMANCE_METRICS_H
