#ifndef PERFORMANCE_MANAGER_H
#define PERFORMANCE_MANAGER_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Performance Manager Configuration
#define PERFORMANCE_MGR_MAX_METRICS          20
#define PERFORMANCE_MGR_MAX_THRESHOLDS       10
#define PERFORMANCE_MGR_MAX_OPTIMIZATIONS    5
#define PERFORMANCE_MGR_SAMPLE_INTERVAL_MS   1000
#define PERFORMANCE_MGR_HISTORY_SIZE         60  // 1 minute at 1s intervals

// Performance Metrics Types
typedef enum {
    PERFORMANCE_METRIC_CPU_USAGE = 0,
    PERFORMANCE_METRIC_MEMORY_USAGE,
    PERFORMANCE_METRIC_RESPONSE_TIME,
    PERFORMANCE_METRIC_THROUGHPUT,
    PERFORMANCE_METRIC_ERROR_RATE,
    PERFORMANCE_METRIC_LATENCY,
    PERFORMANCE_METRIC_BANDWIDTH,
    PERFORMANCE_METRIC_POWER_CONSUMPTION,
    PERFORMANCE_METRIC_TEMPERATURE,
    PERFORMANCE_METRIC_UPTIME,
    PERFORMANCE_METRIC_ACTIVE_CONNECTIONS,
    PERFORMANCE_METRIC_QUEUE_DEPTH,
    PERFORMANCE_METRIC_CACHE_HIT_RATE,
    PERFORMANCE_METRIC_DISK_USAGE,
    PERFORMANCE_METRIC_NETWORK_UTILIZATION,
    PERFORMANCE_METRIC_CUSTOM_1,
    PERFORMANCE_METRIC_CUSTOM_2,
    PERFORMANCE_METRIC_CUSTOM_3,
    PERFORMANCE_METRIC_CUSTOM_4,
    PERFORMANCE_METRIC_CUSTOM_5
} performance_metric_type_t;

// Performance Threshold Types
typedef enum {
    PERFORMANCE_THRESHOLD_WARNING = 0,
    PERFORMANCE_THRESHOLD_CRITICAL,
    PERFORMANCE_THRESHOLD_EMERGENCY
} performance_threshold_type_t;

// Performance Optimization Types
typedef enum {
    PERFORMANCE_OPTIMIZATION_MEMORY_CLEANUP = 0,
    PERFORMANCE_OPTIMIZATION_CPU_THROTTLING,
    PERFORMANCE_OPTIMIZATION_CACHE_OPTIMIZATION,
    PERFORMANCE_OPTIMIZATION_NETWORK_TUNING,
    PERFORMANCE_OPTIMIZATION_POWER_SAVING
} performance_optimization_type_t;

// Performance Metric Structure
typedef struct {
    performance_metric_type_t type;
    char name[32];
    float current_value;
    float min_value;
    float max_value;
    float average_value;
    float threshold_warning;
    float threshold_critical;
    float threshold_emergency;
    bool enabled;
    bool threshold_exceeded;
    uint64_t last_update_time;
    uint32_t sample_count;
    float history[PERFORMANCE_MGR_HISTORY_SIZE];
    uint8_t history_index;
} performance_metric_t;

// Performance Threshold Structure
typedef struct {
    performance_threshold_type_t type;
    performance_metric_type_t metric_type;
    float threshold_value;
    bool enabled;
    bool triggered;
    uint64_t last_trigger_time;
    uint32_t trigger_count;
    char action[64];
} performance_threshold_t;

// Performance Optimization Structure
typedef struct {
    performance_optimization_type_t type;
    char name[32];
    bool enabled;
    bool active;
    float effectiveness;
    uint64_t last_optimization_time;
    uint32_t optimization_count;
    uint32_t success_count;
    char description[128];
} performance_optimization_t;

// Performance Manager Configuration
typedef struct {
    uint32_t sample_interval_ms;
    uint32_t optimization_interval_ms;
    uint32_t threshold_check_interval_ms;
    bool auto_optimization_enabled;
    bool threshold_monitoring_enabled;
    bool performance_logging_enabled;
    uint32_t max_cpu_usage_percent;
    uint32_t max_memory_usage_percent;
    uint32_t max_response_time_ms;
    uint32_t max_error_rate_percent;
} performance_mgr_config_t;

// Performance Manager Status
typedef struct {
    bool initialized;
    bool monitoring_active;
    bool optimization_active;
    uint32_t active_metrics;
    uint32_t active_thresholds;
    uint32_t active_optimizations;
    uint64_t last_sample_time;
    uint64_t last_optimization_time;
    uint64_t last_threshold_check_time;
    float overall_performance_score;
} performance_mgr_status_t;

// Performance Manager Statistics
typedef struct {
    uint64_t total_samples;
    uint64_t total_optimizations;
    uint64_t total_threshold_violations;
    uint64_t total_performance_events;
    uint32_t current_cpu_usage_percent;
    uint32_t current_memory_usage_percent;
    uint32_t current_response_time_ms;
    uint32_t current_error_rate_percent;
    uint64_t system_uptime_ms;
    uint32_t optimization_success_rate;
} performance_mgr_stats_t;

// Performance Event Types
typedef enum {
    PERFORMANCE_EVENT_THRESHOLD_EXCEEDED = 0,
    PERFORMANCE_EVENT_OPTIMIZATION_TRIGGERED,
    PERFORMANCE_EVENT_CRITICAL_PERFORMANCE,
    PERFORMANCE_EVENT_SYSTEM_OVERLOAD,
    PERFORMANCE_EVENT_RECOVERY_COMPLETED,
    PERFORMANCE_EVENT_METRIC_UPDATE,
    PERFORMANCE_EVENT_OPTIMIZATION_SUCCESS,
    PERFORMANCE_EVENT_OPTIMIZATION_FAILED
} performance_event_t;

// Performance Event Callback
typedef void (*performance_event_callback_t)(performance_event_t event, 
                                           performance_metric_type_t metric_type, 
                                           float value, 
                                           const void *data);

// Performance Manager API Functions

// Initialization and Configuration
hal_status_t performance_manager_init(const performance_mgr_config_t *config);
hal_status_t performance_manager_deinit(void);
hal_status_t performance_manager_get_config(performance_mgr_config_t *config);
hal_status_t performance_manager_set_config(const performance_mgr_config_t *config);

// Status and Statistics
hal_status_t performance_manager_get_status(performance_mgr_status_t *status);
hal_status_t performance_manager_get_statistics(performance_mgr_stats_t *stats);
hal_status_t performance_manager_reset_statistics(void);

// Metric Management
hal_status_t performance_manager_register_metric(performance_metric_type_t type, 
                                               const char *name, 
                                               float warning_threshold, 
                                               float critical_threshold, 
                                               float emergency_threshold);
hal_status_t performance_manager_unregister_metric(performance_metric_type_t type);
hal_status_t performance_manager_update_metric(performance_metric_type_t type, float value);
hal_status_t performance_manager_get_metric(performance_metric_type_t type, performance_metric_t *metric);
hal_status_t performance_manager_get_all_metrics(performance_metric_t *metrics, uint32_t max_count);

// Threshold Management
hal_status_t performance_manager_set_threshold(performance_metric_type_t metric_type,
                                              performance_threshold_type_t threshold_type,
                                              float threshold_value,
                                              const char *action);
hal_status_t performance_manager_clear_threshold(performance_metric_type_t metric_type,
                                                performance_threshold_type_t threshold_type);
hal_status_t performance_manager_get_thresholds(performance_threshold_t *thresholds, uint32_t max_count);

// Optimization Management
hal_status_t performance_manager_enable_optimization(performance_optimization_type_t type);
hal_status_t performance_manager_disable_optimization(performance_optimization_type_t type);
hal_status_t performance_manager_trigger_optimization(performance_optimization_type_t type);
hal_status_t performance_manager_get_optimizations(performance_optimization_t *optimizations, uint32_t max_count);

// Monitoring and Control
hal_status_t performance_manager_start_monitoring(void);
hal_status_t performance_manager_stop_monitoring(void);
hal_status_t performance_manager_update(void);
hal_status_t performance_manager_force_optimization(void);

// Event Handling
hal_status_t performance_manager_set_event_callback(performance_event_callback_t callback);
hal_status_t performance_manager_clear_event_callback(void);

// Utility Functions
hal_status_t performance_manager_get_diagnostics(char *diagnostics, uint32_t max_length);
hal_status_t performance_manager_get_performance_report(char *report, uint32_t max_length);
const char* performance_manager_get_metric_name(performance_metric_type_t type);
const char* performance_manager_get_optimization_name(performance_optimization_type_t type);

#ifdef __cplusplus
}
#endif

#endif // PERFORMANCE_MANAGER_H
