#include "diagnostics_manager.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Diagnostics Manager internal structure
typedef struct {
    bool initialized;
    bool health_monitoring_active;
    bool predictive_maintenance_active;
    
    diagnostics_mgr_config_t config;
    diagnostics_mgr_status_t status;
    diagnostics_mgr_stats_t statistics;
    
    diagnostics_component_health_t components[DIAGNOSTICS_MGR_MAX_COMPONENTS];
    diagnostics_test_t tests[DIAGNOSTICS_MGR_MAX_TESTS];
    diagnostics_alert_t alerts[DIAGNOSTICS_MGR_MAX_ALERTS];
    diagnostics_log_entry_t log_entries[DIAGNOSTICS_MGR_MAX_LOG_ENTRIES];
    
    bool component_registered[DIAGNOSTICS_MGR_MAX_COMPONENTS];
    bool test_registered[DIAGNOSTICS_MGR_MAX_TESTS];
    bool alert_registered[DIAGNOSTICS_MGR_MAX_ALERTS];
    
    diagnostics_event_callback_t event_callback;
    
    uint64_t last_health_check_time;
    uint64_t last_test_time;
    uint64_t start_time;
    uint32_t next_alert_id;
    uint32_t log_entry_index;
    
} diagnostics_manager_t;

// Global Diagnostics Manager instance
static diagnostics_manager_t g_diagnostics_manager = {0};

// Default configuration
static const diagnostics_mgr_config_t default_config = {
    .health_check_interval_ms = DIAGNOSTICS_MGR_HEALTH_CHECK_INTERVAL_MS,
    .test_interval_ms = 30000,  // 30 seconds
    .alert_retention_days = 7,
    .auto_health_check_enabled = true,
    .predictive_maintenance_enabled = true,
    .detailed_logging_enabled = true,
    .max_log_entries = DIAGNOSTICS_MGR_MAX_LOG_ENTRIES,
    .critical_health_threshold = 30.0f,
    .warning_health_threshold = 60.0f
};

// Default component names
static const char* default_component_names[] = {
    "System",
    "Network",
    "Security",
    "API",
    "Performance",
    "LED",
    "E-Stop",
    "Communication",
    "State Machine",
    "Hardware",
    "Firmware",
    "Memory",
    "CPU",
    "Storage",
    "Power"
};

// Default test configurations
static const diagnostics_test_t default_tests[] = {
    {
        .type = DIAGNOSTICS_TEST_FUNCTIONALITY,
        .name = "Functionality Test",
        .description = "Test basic functionality of system components",
        .enabled = true,
        .running = false,
        .last_run_time = 0,
        .duration_ms = 0,
        .passed = false,
        .pass_count = 0,
        .fail_count = 0,
        .success_rate = 0.0f,
        .result_message = ""
    },
    {
        .type = DIAGNOSTICS_TEST_PERFORMANCE,
        .name = "Performance Test",
        .description = "Test system performance and response times",
        .enabled = true,
        .running = false,
        .last_run_time = 0,
        .duration_ms = 0,
        .passed = false,
        .pass_count = 0,
        .fail_count = 0,
        .success_rate = 0.0f,
        .result_message = ""
    },
    {
        .type = DIAGNOSTICS_TEST_INTEGRITY,
        .name = "Integrity Test",
        .description = "Test data integrity and system consistency",
        .enabled = true,
        .running = false,
        .last_run_time = 0,
        .duration_ms = 0,
        .passed = false,
        .pass_count = 0,
        .fail_count = 0,
        .success_rate = 0.0f,
        .result_message = ""
    },
    {
        .type = DIAGNOSTICS_TEST_CONNECTIVITY,
        .name = "Connectivity Test",
        .description = "Test network and communication connectivity",
        .enabled = true,
        .running = false,
        .last_run_time = 0,
        .duration_ms = 0,
        .passed = false,
        .pass_count = 0,
        .fail_count = 0,
        .success_rate = 0.0f,
        .result_message = ""
    },
    {
        .type = DIAGNOSTICS_TEST_SECURITY,
        .name = "Security Test",
        .description = "Test security features and access controls",
        .enabled = true,
        .running = false,
        .last_run_time = 0,
        .duration_ms = 0,
        .passed = false,
        .pass_count = 0,
        .fail_count = 0,
        .success_rate = 0.0f,
        .result_message = ""
    }
};

// Forward declarations
static hal_status_t perform_health_check(void);
static hal_status_t run_component_tests(void);
static hal_status_t analyze_system_health(void);
static hal_status_t handle_diagnostics_event(diagnostics_event_t event, 
                                           diagnostics_component_t component, 
                                           const void *data);
static int find_component_index(diagnostics_component_t component);
static int find_test_index(diagnostics_test_type_t type);
static int find_free_component_slot(void);
static int find_free_test_slot(void);
static int find_free_alert_slot(void);
static void update_statistics(void);
static void add_log_entry(diagnostics_component_t component, 
                         diagnostics_alert_severity_t severity, 
                         const char *message, 
                         const char *details);

// Diagnostics Manager implementation

hal_status_t diagnostics_manager_init(const diagnostics_mgr_config_t *config) {
    if (g_diagnostics_manager.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize Diagnostics Manager structure
    memset(&g_diagnostics_manager, 0, sizeof(diagnostics_manager_t));
    
    // Set configuration
    if (config != NULL) {
        memcpy(&g_diagnostics_manager.config, config, sizeof(diagnostics_mgr_config_t));
    } else {
        memcpy(&g_diagnostics_manager.config, &default_config, sizeof(diagnostics_mgr_config_t));
    }
    
    // Initialize status
    g_diagnostics_manager.status.initialized = true;
    g_diagnostics_manager.status.health_monitoring_active = false;
    g_diagnostics_manager.status.predictive_maintenance_active = false;
    g_diagnostics_manager.status.active_components = 0;
    g_diagnostics_manager.status.active_tests = 0;
    g_diagnostics_manager.status.active_alerts = 0;
    g_diagnostics_manager.status.overall_system_health = 100.0f;
    g_diagnostics_manager.status.system_health_level = DIAGNOSTICS_HEALTH_EXCELLENT;
    
    // Initialize timing
    g_diagnostics_manager.start_time = hal_get_timestamp_us();
    g_diagnostics_manager.last_health_check_time = hal_get_timestamp_us();
    g_diagnostics_manager.last_test_time = hal_get_timestamp_us();
    g_diagnostics_manager.next_alert_id = 1;
    g_diagnostics_manager.log_entry_index = 0;
    
    // Initialize arrays
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_COMPONENTS; i++) {
        g_diagnostics_manager.component_registered[i] = false;
        memset(&g_diagnostics_manager.components[i], 0, sizeof(diagnostics_component_health_t));
    }
    
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_TESTS; i++) {
        g_diagnostics_manager.test_registered[i] = false;
        memset(&g_diagnostics_manager.tests[i], 0, sizeof(diagnostics_test_t));
    }
    
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_ALERTS; i++) {
        g_diagnostics_manager.alert_registered[i] = false;
        memset(&g_diagnostics_manager.alerts[i], 0, sizeof(diagnostics_alert_t));
    }
    
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_LOG_ENTRIES; i++) {
        memset(&g_diagnostics_manager.log_entries[i], 0, sizeof(diagnostics_log_entry_t));
    }
    
    // Register default tests
    for (int i = 0; i < 5; i++) {
        memcpy(&g_diagnostics_manager.tests[i], &default_tests[i], sizeof(diagnostics_test_t));
        g_diagnostics_manager.test_registered[i] = true;
        g_diagnostics_manager.status.active_tests++;
    }
    
    g_diagnostics_manager.initialized = true;
    
    handle_diagnostics_event(DIAGNOSTICS_EVENT_HEALTH_CHECK, 
                           DIAGNOSTICS_COMPONENT_SYSTEM, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_deinit(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Stop health monitoring
    diagnostics_manager_stop_health_monitoring();
    
    // Clear Diagnostics Manager
    memset(&g_diagnostics_manager, 0, sizeof(diagnostics_manager_t));
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_config(diagnostics_mgr_config_t *config) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_diagnostics_manager.config, sizeof(diagnostics_mgr_config_t));
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_set_config(const diagnostics_mgr_config_t *config) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_diagnostics_manager.config, config, sizeof(diagnostics_mgr_config_t));
    
    handle_diagnostics_event(DIAGNOSTICS_EVENT_HEALTH_CHECK, 
                           DIAGNOSTICS_COMPONENT_SYSTEM, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_status(diagnostics_mgr_status_t *status) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(status, &g_diagnostics_manager.status, sizeof(diagnostics_mgr_status_t));
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_statistics(diagnostics_mgr_stats_t *stats) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(stats, &g_diagnostics_manager.statistics, sizeof(diagnostics_mgr_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_reset_statistics(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&g_diagnostics_manager.statistics, 0, sizeof(diagnostics_mgr_stats_t));
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_register_component(diagnostics_component_t component, 
                                                  const char *name) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (name == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_component_index(component);
    if (index >= 0) {
        return HAL_STATUS_ALREADY_EXISTS;
    }
    
    index = find_free_component_slot();
    if (index < 0) {
        return HAL_STATUS_NO_MEMORY;
    }
    
    // Initialize component
    g_diagnostics_manager.components[index].component = component;
    strncpy(g_diagnostics_manager.components[index].name, name, 31);
    g_diagnostics_manager.components[index].name[31] = '\0';
    g_diagnostics_manager.components[index].health_level = DIAGNOSTICS_HEALTH_EXCELLENT;
    g_diagnostics_manager.components[index].health_score = 100.0f;
    g_diagnostics_manager.components[index].last_check_time = hal_get_timestamp_us();
    g_diagnostics_manager.components[index].uptime_ms = 0;
    g_diagnostics_manager.components[index].error_count = 0;
    g_diagnostics_manager.components[index].warning_count = 0;
    g_diagnostics_manager.components[index].success_count = 0;
    g_diagnostics_manager.components[index].enabled = true;
    g_diagnostics_manager.components[index].critical = false;
    strcpy(g_diagnostics_manager.components[index].status_message, "Component registered");
    strcpy(g_diagnostics_manager.components[index].recommendations, "Monitor component health");
    
    g_diagnostics_manager.component_registered[index] = true;
    g_diagnostics_manager.status.active_components++;
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_unregister_component(diagnostics_component_t component) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    int index = find_component_index(component);
    if (index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    g_diagnostics_manager.component_registered[index] = false;
    g_diagnostics_manager.status.active_components--;
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_update_component_health(diagnostics_component_t component, 
                                                        diagnostics_health_level_t health_level, 
                                                        float health_score, 
                                                        const char *status_message) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    int index = find_component_index(component);
    if (index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    diagnostics_component_health_t *comp_health = &g_diagnostics_manager.components[index];
    
    // Update component health
    comp_health->health_level = health_level;
    comp_health->health_score = health_score;
    comp_health->last_check_time = hal_get_timestamp_us();
    
    if (status_message != NULL) {
        strncpy(comp_health->status_message, status_message, 127);
        comp_health->status_message[127] = '\0';
    }
    
    // Update statistics based on health level
    switch (health_level) {
        case DIAGNOSTICS_HEALTH_EXCELLENT:
            comp_health->success_count++;
            break;
        case DIAGNOSTICS_HEALTH_GOOD:
            comp_health->success_count++;
            break;
        case DIAGNOSTICS_HEALTH_FAIR:
            comp_health->warning_count++;
            break;
        case DIAGNOSTICS_HEALTH_POOR:
            comp_health->warning_count++;
            break;
        case DIAGNOSTICS_HEALTH_CRITICAL:
            comp_health->error_count++;
            comp_health->critical = true;
            handle_diagnostics_event(DIAGNOSTICS_EVENT_COMPONENT_FAILED, component, NULL);
            break;
        case DIAGNOSTICS_HEALTH_FAILED:
            comp_health->error_count++;
            comp_health->critical = true;
            handle_diagnostics_event(DIAGNOSTICS_EVENT_COMPONENT_FAILED, component, NULL);
            break;
    }
    
    // Update overall system health
    analyze_system_health();
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_component_health(diagnostics_component_t component, 
                                                     diagnostics_component_health_t *health) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (health == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_component_index(component);
    if (index < 0) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    memcpy(health, &g_diagnostics_manager.components[index], sizeof(diagnostics_component_health_t));
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_all_component_health(diagnostics_component_health_t *health, 
                                                         uint32_t max_count) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (health == NULL || max_count == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    uint32_t count = 0;
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_COMPONENTS && count < max_count; i++) {
        if (g_diagnostics_manager.component_registered[i]) {
            memcpy(&health[count], &g_diagnostics_manager.components[i], sizeof(diagnostics_component_health_t));
            count++;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_start_health_monitoring(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_manager.status.health_monitoring_active = true;
    g_diagnostics_manager.health_monitoring_active = true;
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_stop_health_monitoring(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_manager.status.health_monitoring_active = false;
    g_diagnostics_manager.health_monitoring_active = false;
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_perform_health_check(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_manager.statistics.total_health_checks++;
    g_diagnostics_manager.last_health_check_time = hal_get_timestamp_us();
    
    // Perform health check on all registered components
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_COMPONENTS; i++) {
        if (g_diagnostics_manager.component_registered[i]) {
            // Simulate health check (in real implementation, this would check actual component status)
            diagnostics_health_level_t health_level = DIAGNOSTICS_HEALTH_EXCELLENT;
            float health_score = 100.0f;
            
            // Simulate some components having issues
            if (rand() % 100 < 10) {  // 10% chance of issues
                health_level = DIAGNOSTICS_HEALTH_FAIR;
                health_score = 70.0f + (rand() % 20);
            }
            
            diagnostics_manager_update_component_health(
                g_diagnostics_manager.components[i].component,
                health_level,
                health_score,
                "Health check completed"
            );
        }
    }
    
    handle_diagnostics_event(DIAGNOSTICS_EVENT_HEALTH_CHECK, 
                           DIAGNOSTICS_COMPONENT_SYSTEM, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_perform_full_diagnostics(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Perform health check
    diagnostics_manager_perform_health_check();
    
    // Run all tests
    diagnostics_manager_run_all_tests();
    
    // Analyze trends
    diagnostics_manager_analyze_trends();
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_run_all_tests(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_manager.last_test_time = hal_get_timestamp_us();
    
    // Run all registered tests
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_TESTS; i++) {
        if (g_diagnostics_manager.test_registered[i] && 
            g_diagnostics_manager.tests[i].enabled) {
            
            diagnostics_test_t *test = &g_diagnostics_manager.tests[i];
            test->running = true;
            test->last_run_time = hal_get_timestamp_us();
            
            // Simulate test execution
            uint64_t start_time = hal_get_timestamp_us();
            
            // Simulate test duration
            for (int j = 0; j < 1000000; j++) {
                // Simulate work
            }
            
            test->duration_ms = (uint32_t)((hal_get_timestamp_us() - start_time) / 1000ULL);
            test->running = false;
            
            // Simulate test result (90% pass rate)
            test->passed = (rand() % 100 < 90);
            
            if (test->passed) {
                test->pass_count++;
                g_diagnostics_manager.statistics.tests_passed++;
                strcpy(test->result_message, "Test passed successfully");
            } else {
                test->fail_count++;
                g_diagnostics_manager.statistics.tests_failed++;
                strcpy(test->result_message, "Test failed - check component status");
            }
            
            // Calculate success rate
            uint32_t total_runs = test->pass_count + test->fail_count;
            test->success_rate = total_runs > 0 ? (float)test->pass_count / total_runs * 100.0f : 0.0f;
            
            g_diagnostics_manager.statistics.total_tests_run++;
            
            handle_diagnostics_event(DIAGNOSTICS_EVENT_TEST_COMPLETED, 
                                   DIAGNOSTICS_COMPONENT_SYSTEM, NULL);
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_generate_alert(diagnostics_alert_severity_t severity, 
                                              diagnostics_component_t component, 
                                              const char *title, 
                                              const char *message) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (title == NULL || message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    int index = find_free_alert_slot();
    if (index < 0) {
        return HAL_STATUS_NO_MEMORY;
    }
    
    // Create alert
    diagnostics_alert_t *alert = &g_diagnostics_manager.alerts[index];
    alert->severity = severity;
    alert->component = component;
    strncpy(alert->title, title, 63);
    alert->title[63] = '\0';
    strncpy(alert->message, message, 255);
    alert->message[255] = '\0';
    alert->timestamp = hal_get_timestamp_us();
    alert->acknowledged = false;
    alert->resolved = false;
    strcpy(alert->resolution_notes, "");
    
    g_diagnostics_manager.alert_registered[index] = true;
    g_diagnostics_manager.status.active_alerts++;
    g_diagnostics_manager.statistics.total_alerts_generated++;
    
    // Update alert statistics
    switch (severity) {
        case DIAGNOSTICS_ALERT_INFO:
            g_diagnostics_manager.statistics.alerts_info++;
            break;
        case DIAGNOSTICS_ALERT_WARNING:
            g_diagnostics_manager.statistics.alerts_warning++;
            break;
        case DIAGNOSTICS_ALERT_ERROR:
            g_diagnostics_manager.statistics.alerts_error++;
            break;
        case DIAGNOSTICS_ALERT_CRITICAL:
            g_diagnostics_manager.statistics.alerts_critical++;
            break;
        case DIAGNOSTICS_ALERT_EMERGENCY:
            g_diagnostics_manager.statistics.alerts_emergency++;
            break;
    }
    
    handle_diagnostics_event(DIAGNOSTICS_EVENT_ALERT_GENERATED, component, NULL);
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_log_event(diagnostics_component_t component, 
                                         diagnostics_alert_severity_t severity, 
                                         const char *message, 
                                         const char *details) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    add_log_entry(component, severity, message, details);
    g_diagnostics_manager.statistics.total_log_entries++;
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_diagnostics(char *diagnostics, uint32_t max_length) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    if (diagnostics == NULL || max_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(diagnostics, max_length,
             "Diagnostics Manager Diagnostics:\n"
             "Health Monitoring Active: %s\n"
             "Predictive Maintenance Active: %s\n"
             "Active Components: %d\n"
             "Active Tests: %d\n"
             "Active Alerts: %d\n"
             "Overall System Health: %.2f%%\n"
             "System Health Level: %s\n"
             "Total Health Checks: %lu\n"
             "Total Tests Run: %lu\n"
             "Total Alerts Generated: %lu\n"
             "Total Log Entries: %lu\n"
             "Components Excellent: %d\n"
             "Components Good: %d\n"
             "Components Fair: %d\n"
             "Components Poor: %d\n"
             "Components Critical: %d\n"
             "Components Failed: %d\n"
             "Tests Passed: %d\n"
             "Tests Failed: %d\n"
             "Last Health Check: %lu ms ago\n"
             "Last Test Run: %lu ms ago",
             g_diagnostics_manager.status.health_monitoring_active ? "YES" : "NO",
             g_diagnostics_manager.status.predictive_maintenance_active ? "YES" : "NO",
             g_diagnostics_manager.status.active_components,
             g_diagnostics_manager.status.active_tests,
             g_diagnostics_manager.status.active_alerts,
             g_diagnostics_manager.status.overall_system_health,
             diagnostics_manager_get_health_level_name(g_diagnostics_manager.status.system_health_level),
             g_diagnostics_manager.statistics.total_health_checks,
             g_diagnostics_manager.statistics.total_tests_run,
             g_diagnostics_manager.statistics.total_alerts_generated,
             g_diagnostics_manager.statistics.total_log_entries,
             g_diagnostics_manager.statistics.components_excellent,
             g_diagnostics_manager.statistics.components_good,
             g_diagnostics_manager.statistics.components_fair,
             g_diagnostics_manager.statistics.components_poor,
             g_diagnostics_manager.statistics.components_critical,
             g_diagnostics_manager.statistics.components_failed,
             g_diagnostics_manager.statistics.tests_passed,
             g_diagnostics_manager.statistics.tests_failed,
             (uint32_t)((hal_get_timestamp_us() - g_diagnostics_manager.last_health_check_time) / 1000ULL),
             (uint32_t)((hal_get_timestamp_us() - g_diagnostics_manager.last_test_time) / 1000ULL));
    
    return HAL_STATUS_OK;
}

// Internal helper functions

static hal_status_t perform_health_check(void) {
    return diagnostics_manager_perform_health_check();
}

static hal_status_t run_component_tests(void) {
    return diagnostics_manager_run_all_tests();
}

static hal_status_t analyze_system_health(void) {
    float total_health_score = 0.0f;
    uint32_t component_count = 0;
    
    // Calculate average health score
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_COMPONENTS; i++) {
        if (g_diagnostics_manager.component_registered[i]) {
            total_health_score += g_diagnostics_manager.components[i].health_score;
            component_count++;
        }
    }
    
    if (component_count > 0) {
        g_diagnostics_manager.status.overall_system_health = total_health_score / component_count;
    }
    
    // Determine system health level
    if (g_diagnostics_manager.status.overall_system_health >= 90.0f) {
        g_diagnostics_manager.status.system_health_level = DIAGNOSTICS_HEALTH_EXCELLENT;
    } else if (g_diagnostics_manager.status.overall_system_health >= 80.0f) {
        g_diagnostics_manager.status.system_health_level = DIAGNOSTICS_HEALTH_GOOD;
    } else if (g_diagnostics_manager.status.overall_system_health >= 60.0f) {
        g_diagnostics_manager.status.system_health_level = DIAGNOSTICS_HEALTH_FAIR;
    } else if (g_diagnostics_manager.status.overall_system_health >= 40.0f) {
        g_diagnostics_manager.status.system_health_level = DIAGNOSTICS_HEALTH_POOR;
    } else if (g_diagnostics_manager.status.overall_system_health >= 20.0f) {
        g_diagnostics_manager.status.system_health_level = DIAGNOSTICS_HEALTH_CRITICAL;
    } else {
        g_diagnostics_manager.status.system_health_level = DIAGNOSTICS_HEALTH_FAILED;
    }
    
    // Update component statistics
    g_diagnostics_manager.statistics.components_excellent = 0;
    g_diagnostics_manager.statistics.components_good = 0;
    g_diagnostics_manager.statistics.components_fair = 0;
    g_diagnostics_manager.statistics.components_poor = 0;
    g_diagnostics_manager.statistics.components_critical = 0;
    g_diagnostics_manager.statistics.components_failed = 0;
    
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_COMPONENTS; i++) {
        if (g_diagnostics_manager.component_registered[i]) {
            switch (g_diagnostics_manager.components[i].health_level) {
                case DIAGNOSTICS_HEALTH_EXCELLENT:
                    g_diagnostics_manager.statistics.components_excellent++;
                    break;
                case DIAGNOSTICS_HEALTH_GOOD:
                    g_diagnostics_manager.statistics.components_good++;
                    break;
                case DIAGNOSTICS_HEALTH_FAIR:
                    g_diagnostics_manager.statistics.components_fair++;
                    break;
                case DIAGNOSTICS_HEALTH_POOR:
                    g_diagnostics_manager.statistics.components_poor++;
                    break;
                case DIAGNOSTICS_HEALTH_CRITICAL:
                    g_diagnostics_manager.statistics.components_critical++;
                    break;
                case DIAGNOSTICS_HEALTH_FAILED:
                    g_diagnostics_manager.statistics.components_failed++;
                    break;
            }
        }
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t handle_diagnostics_event(diagnostics_event_t event, 
                                           diagnostics_component_t component, 
                                           const void *data) {
    if (g_diagnostics_manager.event_callback != NULL) {
        g_diagnostics_manager.event_callback(event, component, data);
    }
    
    return HAL_STATUS_OK;
}

static int find_component_index(diagnostics_component_t component) {
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_COMPONENTS; i++) {
        if (g_diagnostics_manager.component_registered[i] && 
            g_diagnostics_manager.components[i].component == component) {
            return i;
        }
    }
    return -1;
}

static int find_test_index(diagnostics_test_type_t type) {
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_TESTS; i++) {
        if (g_diagnostics_manager.test_registered[i] && 
            g_diagnostics_manager.tests[i].type == type) {
            return i;
        }
    }
    return -1;
}

static int find_free_component_slot(void) {
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_COMPONENTS; i++) {
        if (!g_diagnostics_manager.component_registered[i]) {
            return i;
        }
    }
    return -1;
}

static int find_free_test_slot(void) {
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_TESTS; i++) {
        if (!g_diagnostics_manager.test_registered[i]) {
            return i;
        }
    }
    return -1;
}

static int find_free_alert_slot(void) {
    for (int i = 0; i < DIAGNOSTICS_MGR_MAX_ALERTS; i++) {
        if (!g_diagnostics_manager.alert_registered[i]) {
            return i;
        }
    }
    return -1;
}

static void add_log_entry(diagnostics_component_t component, 
                         diagnostics_alert_severity_t severity, 
                         const char *message, 
                         const char *details) {
    diagnostics_log_entry_t *entry = &g_diagnostics_manager.log_entries[g_diagnostics_manager.log_entry_index];
    
    entry->timestamp = hal_get_timestamp_us();
    entry->component = component;
    entry->severity = severity;
    
    if (message != NULL) {
        strncpy(entry->message, message, 255);
        entry->message[255] = '\0';
    }
    
    if (details != NULL) {
        strncpy(entry->details, details, 511);
        entry->details[511] = '\0';
    }
    
    g_diagnostics_manager.log_entry_index = 
        (g_diagnostics_manager.log_entry_index + 1) % DIAGNOSTICS_MGR_MAX_LOG_ENTRIES;
}

// Stub implementations for remaining functions
hal_status_t diagnostics_manager_run_test(diagnostics_test_type_t type) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_test_results(diagnostics_test_type_t type, 
                                                 diagnostics_test_t *test) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_all_test_results(diagnostics_test_t *tests, 
                                                     uint32_t max_count) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_acknowledge_alert(uint32_t alert_id) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_resolve_alert(uint32_t alert_id, 
                                              const char *resolution_notes) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_alerts(diagnostics_alert_t *alerts, 
                                           uint32_t max_count) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_clear_alerts(void) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_log_entries(diagnostics_log_entry_t *entries, 
                                                uint32_t max_count) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_clear_logs(void) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_enable_predictive_maintenance(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_manager.status.predictive_maintenance_active = true;
    g_diagnostics_manager.predictive_maintenance_active = true;
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_disable_predictive_maintenance(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_manager.status.predictive_maintenance_active = false;
    g_diagnostics_manager.predictive_maintenance_active = false;
    
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_analyze_trends(void) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_generate_maintenance_schedule(char *schedule, 
                                                             uint32_t max_length) {
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_set_event_callback(diagnostics_event_callback_t callback) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_manager.event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_clear_event_callback(void) {
    if (!g_diagnostics_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_diagnostics_manager.event_callback = NULL;
    return HAL_STATUS_OK;
}

hal_status_t diagnostics_manager_get_health_report(char *report, uint32_t max_length) {
    return diagnostics_manager_get_diagnostics(report, max_length);
}

hal_status_t diagnostics_manager_get_maintenance_report(char *report, uint32_t max_length) {
    return HAL_STATUS_OK;
}

const char* diagnostics_manager_get_component_name(diagnostics_component_t component) {
    if (component < 15) {
        return default_component_names[component];
    }
    return "Unknown";
}

const char* diagnostics_manager_get_health_level_name(diagnostics_health_level_t level) {
    switch (level) {
        case DIAGNOSTICS_HEALTH_EXCELLENT: return "Excellent";
        case DIAGNOSTICS_HEALTH_GOOD: return "Good";
        case DIAGNOSTICS_HEALTH_FAIR: return "Fair";
        case DIAGNOSTICS_HEALTH_POOR: return "Poor";
        case DIAGNOSTICS_HEALTH_CRITICAL: return "Critical";
        case DIAGNOSTICS_HEALTH_FAILED: return "Failed";
        default: return "Unknown";
    }
}

const char* diagnostics_manager_get_test_type_name(diagnostics_test_type_t type) {
    switch (type) {
        case DIAGNOSTICS_TEST_FUNCTIONALITY: return "Functionality";
        case DIAGNOSTICS_TEST_PERFORMANCE: return "Performance";
        case DIAGNOSTICS_TEST_INTEGRITY: return "Integrity";
        case DIAGNOSTICS_TEST_CONNECTIVITY: return "Connectivity";
        case DIAGNOSTICS_TEST_SECURITY: return "Security";
        case DIAGNOSTICS_TEST_STRESS: return "Stress";
        case DIAGNOSTICS_TEST_ENDURANCE: return "Endurance";
        case DIAGNOSTICS_TEST_COMPATIBILITY: return "Compatibility";
        default: return "Unknown";
    }
}

const char* diagnostics_manager_get_alert_severity_name(diagnostics_alert_severity_t severity) {
    switch (severity) {
        case DIAGNOSTICS_ALERT_INFO: return "Info";
        case DIAGNOSTICS_ALERT_WARNING: return "Warning";
        case DIAGNOSTICS_ALERT_ERROR: return "Error";
        case DIAGNOSTICS_ALERT_CRITICAL: return "Critical";
        case DIAGNOSTICS_ALERT_EMERGENCY: return "Emergency";
        default: return "Unknown";
    }
}
