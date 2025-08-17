#ifndef DIAGNOSTICS_MANAGER_H
#define DIAGNOSTICS_MANAGER_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Diagnostics Manager Configuration
#define DIAGNOSTICS_MGR_MAX_COMPONENTS       15
#define DIAGNOSTICS_MGR_MAX_TESTS            20
#define DIAGNOSTICS_MGR_MAX_ALERTS           10
#define DIAGNOSTICS_MGR_MAX_LOG_ENTRIES      100
#define DIAGNOSTICS_MGR_HEALTH_CHECK_INTERVAL_MS 5000

// System Components
typedef enum {
    DIAGNOSTICS_COMPONENT_SYSTEM = 0,
    DIAGNOSTICS_COMPONENT_NETWORK,
    DIAGNOSTICS_COMPONENT_SECURITY,
    DIAGNOSTICS_COMPONENT_API,
    DIAGNOSTICS_COMPONENT_PERFORMANCE,
    DIAGNOSTICS_COMPONENT_LED,
    DIAGNOSTICS_COMPONENT_ESTOP,
    DIAGNOSTICS_COMPONENT_COMMUNICATION,
    DIAGNOSTICS_COMPONENT_STATE_MACHINE,
    DIAGNOSTICS_COMPONENT_HARDWARE,
    DIAGNOSTICS_COMPONENT_FIRMWARE,
    DIAGNOSTICS_COMPONENT_MEMORY,
    DIAGNOSTICS_COMPONENT_CPU,
    DIAGNOSTICS_COMPONENT_STORAGE,
    DIAGNOSTICS_COMPONENT_POWER
} diagnostics_component_t;

// Health Status Levels
typedef enum {
    DIAGNOSTICS_HEALTH_EXCELLENT = 0,
    DIAGNOSTICS_HEALTH_GOOD,
    DIAGNOSTICS_HEALTH_FAIR,
    DIAGNOSTICS_HEALTH_POOR,
    DIAGNOSTICS_HEALTH_CRITICAL,
    DIAGNOSTICS_HEALTH_FAILED
} diagnostics_health_level_t;

// Test Types
typedef enum {
    DIAGNOSTICS_TEST_FUNCTIONALITY = 0,
    DIAGNOSTICS_TEST_PERFORMANCE,
    DIAGNOSTICS_TEST_INTEGRITY,
    DIAGNOSTICS_TEST_CONNECTIVITY,
    DIAGNOSTICS_TEST_SECURITY,
    DIAGNOSTICS_TEST_STRESS,
    DIAGNOSTICS_TEST_ENDURANCE,
    DIAGNOSTICS_TEST_COMPATIBILITY
} diagnostics_test_type_t;

// Alert Severity Levels
typedef enum {
    DIAGNOSTICS_ALERT_INFO = 0,
    DIAGNOSTICS_ALERT_WARNING,
    DIAGNOSTICS_ALERT_ERROR,
    DIAGNOSTICS_ALERT_CRITICAL,
    DIAGNOSTICS_ALERT_EMERGENCY
} diagnostics_alert_severity_t;

// Component Health Structure
typedef struct {
    diagnostics_component_t component;
    char name[32];
    diagnostics_health_level_t health_level;
    float health_score;
    uint64_t last_check_time;
    uint64_t uptime_ms;
    uint32_t error_count;
    uint32_t warning_count;
    uint32_t success_count;
    bool enabled;
    bool critical;
    char status_message[128];
    char recommendations[256];
} diagnostics_component_health_t;

// Diagnostic Test Structure
typedef struct {
    diagnostics_test_type_t type;
    char name[32];
    char description[128];
    bool enabled;
    bool running;
    uint64_t last_run_time;
    uint64_t duration_ms;
    bool passed;
    uint32_t pass_count;
    uint32_t fail_count;
    float success_rate;
    char result_message[256];
} diagnostics_test_t;

// Diagnostic Alert Structure
typedef struct {
    diagnostics_alert_severity_t severity;
    diagnostics_component_t component;
    char title[64];
    char message[256];
    uint64_t timestamp;
    bool acknowledged;
    bool resolved;
    char resolution_notes[256];
} diagnostics_alert_t;

// Diagnostic Log Entry Structure
typedef struct {
    uint64_t timestamp;
    diagnostics_component_t component;
    diagnostics_alert_severity_t severity;
    char message[256];
    char details[512];
} diagnostics_log_entry_t;

// Diagnostics Manager Configuration
typedef struct {
    uint32_t health_check_interval_ms;
    uint32_t test_interval_ms;
    uint32_t alert_retention_days;
    bool auto_health_check_enabled;
    bool predictive_maintenance_enabled;
    bool detailed_logging_enabled;
    uint32_t max_log_entries;
    float critical_health_threshold;
    float warning_health_threshold;
} diagnostics_mgr_config_t;

// Diagnostics Manager Status
typedef struct {
    bool initialized;
    bool health_monitoring_active;
    bool predictive_maintenance_active;
    uint32_t active_components;
    uint32_t active_tests;
    uint32_t active_alerts;
    uint64_t last_health_check_time;
    uint64_t last_test_time;
    float overall_system_health;
    diagnostics_health_level_t system_health_level;
} diagnostics_mgr_status_t;

// Diagnostics Manager Statistics
typedef struct {
    uint64_t total_health_checks;
    uint64_t total_tests_run;
    uint64_t total_alerts_generated;
    uint64_t total_log_entries;
    uint32_t components_excellent;
    uint32_t components_good;
    uint32_t components_fair;
    uint32_t components_poor;
    uint32_t components_critical;
    uint32_t components_failed;
    uint32_t tests_passed;
    uint32_t tests_failed;
    uint32_t alerts_info;
    uint32_t alerts_warning;
    uint32_t alerts_error;
    uint32_t alerts_critical;
    uint32_t alerts_emergency;
} diagnostics_mgr_stats_t;

// Diagnostic Event Types
typedef enum {
    DIAGNOSTICS_EVENT_HEALTH_CHECK = 0,
    DIAGNOSTICS_EVENT_TEST_COMPLETED,
    DIAGNOSTICS_EVENT_ALERT_GENERATED,
    DIAGNOSTICS_EVENT_COMPONENT_FAILED,
    DIAGNOSTICS_EVENT_SYSTEM_DEGRADED,
    DIAGNOSTICS_EVENT_MAINTENANCE_REQUIRED,
    DIAGNOSTICS_EVENT_PREDICTIVE_ALERT,
    DIAGNOSTICS_EVENT_RECOVERY_COMPLETED
} diagnostics_event_t;

// Diagnostic Event Callback
typedef void (*diagnostics_event_callback_t)(diagnostics_event_t event, 
                                           diagnostics_component_t component, 
                                           const void *data);

// Diagnostics Manager API Functions

// Initialization and Configuration
hal_status_t diagnostics_manager_init(const diagnostics_mgr_config_t *config);
hal_status_t diagnostics_manager_deinit(void);
hal_status_t diagnostics_manager_get_config(diagnostics_mgr_config_t *config);
hal_status_t diagnostics_manager_set_config(const diagnostics_mgr_config_t *config);

// Status and Statistics
hal_status_t diagnostics_manager_get_status(diagnostics_mgr_status_t *status);
hal_status_t diagnostics_manager_get_statistics(diagnostics_mgr_stats_t *stats);
hal_status_t diagnostics_manager_reset_statistics(void);

// Component Health Management
hal_status_t diagnostics_manager_register_component(diagnostics_component_t component, 
                                                  const char *name);
hal_status_t diagnostics_manager_unregister_component(diagnostics_component_t component);
hal_status_t diagnostics_manager_update_component_health(diagnostics_component_t component, 
                                                        diagnostics_health_level_t health_level, 
                                                        float health_score, 
                                                        const char *status_message);
hal_status_t diagnostics_manager_get_component_health(diagnostics_component_t component, 
                                                     diagnostics_component_health_t *health);
hal_status_t diagnostics_manager_get_all_component_health(diagnostics_component_health_t *health, 
                                                         uint32_t max_count);

// Test Management
hal_status_t diagnostics_manager_register_test(diagnostics_test_type_t type, 
                                             const char *name, 
                                             const char *description);
hal_status_t diagnostics_manager_unregister_test(diagnostics_test_type_t type);
hal_status_t diagnostics_manager_run_test(diagnostics_test_type_t type);
hal_status_t diagnostics_manager_run_all_tests(void);
hal_status_t diagnostics_manager_get_test_results(diagnostics_test_type_t type, 
                                                 diagnostics_test_t *test);
hal_status_t diagnostics_manager_get_all_test_results(diagnostics_test_t *tests, 
                                                     uint32_t max_count);

// Alert Management
hal_status_t diagnostics_manager_generate_alert(diagnostics_alert_severity_t severity, 
                                              diagnostics_component_t component, 
                                              const char *title, 
                                              const char *message);
hal_status_t diagnostics_manager_acknowledge_alert(uint32_t alert_id);
hal_status_t diagnostics_manager_resolve_alert(uint32_t alert_id, 
                                              const char *resolution_notes);
hal_status_t diagnostics_manager_get_alerts(diagnostics_alert_t *alerts, 
                                           uint32_t max_count);
hal_status_t diagnostics_manager_clear_alerts(void);

// Logging
hal_status_t diagnostics_manager_log_event(diagnostics_component_t component, 
                                         diagnostics_alert_severity_t severity, 
                                         const char *message, 
                                         const char *details);
hal_status_t diagnostics_manager_get_log_entries(diagnostics_log_entry_t *entries, 
                                                uint32_t max_count);
hal_status_t diagnostics_manager_clear_logs(void);

// Health Monitoring and Control
hal_status_t diagnostics_manager_start_health_monitoring(void);
hal_status_t diagnostics_manager_stop_health_monitoring(void);
hal_status_t diagnostics_manager_perform_health_check(void);
hal_status_t diagnostics_manager_perform_full_diagnostics(void);

// Predictive Maintenance
hal_status_t diagnostics_manager_enable_predictive_maintenance(void);
hal_status_t diagnostics_manager_disable_predictive_maintenance(void);
hal_status_t diagnostics_manager_analyze_trends(void);
hal_status_t diagnostics_manager_generate_maintenance_schedule(char *schedule, 
                                                             uint32_t max_length);

// Event Handling
hal_status_t diagnostics_manager_set_event_callback(diagnostics_event_callback_t callback);
hal_status_t diagnostics_manager_clear_event_callback(void);

// Utility Functions
hal_status_t diagnostics_manager_get_diagnostics(char *diagnostics, uint32_t max_length);
hal_status_t diagnostics_manager_get_health_report(char *report, uint32_t max_length);
hal_status_t diagnostics_manager_get_maintenance_report(char *report, uint32_t max_length);
const char* diagnostics_manager_get_component_name(diagnostics_component_t component);
const char* diagnostics_manager_get_health_level_name(diagnostics_health_level_t level);
const char* diagnostics_manager_get_test_type_name(diagnostics_test_type_t type);
const char* diagnostics_manager_get_alert_severity_name(diagnostics_alert_severity_t severity);

#ifdef __cplusplus
}
#endif

#endif // DIAGNOSTICS_MANAGER_H
