/**
 * @file module_data_storage.h
 * @brief Module Data Storage Functions for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (Module Data Storage Implementation)
 */

#ifndef MODULE_DATA_STORAGE_H
#define MODULE_DATA_STORAGE_H

#include "hal_common.h"
#include "api/api_endpoints.h"
#include "constants.h"
#include <time.h>
#include <stdbool.h>

// Maximum number of modules supported
#// Maximum number of history records per module
#define MAX_HISTORY_RECORDS 1000

// Maximum number of commands per module
#define MAX_COMMAND_HISTORY 500

// Data freshness timeout (in milliseconds)
#define DATA_FRESHNESS_TIMEOUT_MS 5000

// Module Data Storage Structures
typedef struct {
    int module_id;
    char module_name[32];
    float voltage;
    float current;
    float power;
    float temperature;
    float efficiency;
    float load_percentage;
    unsigned long timestamp;
    int data_freshness_ms;
    bool data_valid;
} module_telemetry_storage_t;

typedef struct {
    int module_id;
    char module_name[32];
    bool emergency_stop_enabled;
    int response_time_ms;
    bool auto_recovery;
    char config_version[16];
    unsigned long last_updated;
    bool config_valid;
} module_config_storage_t;

typedef struct {
    int module_id;
    char command[32];
    char parameters[256];
    char reason[128];
    bool success;
    char result_message[256];
    unsigned long timestamp;
    int execution_time_ms;
} module_command_storage_t;

typedef struct {
    int module_id;
    char health_status[32];
    float health_score;
    unsigned long uptime_seconds;
    int error_count;
    int warning_count;
    unsigned long last_updated;
    bool health_valid;
} module_health_storage_t;

typedef struct {
    int module_id;
    char module_name[32];
    module_telemetry_storage_t telemetry;
    module_config_storage_t config;
    module_health_storage_t health;
    module_command_storage_t last_command;
    unsigned long last_activity;
    bool module_active;
} module_data_t;

typedef struct {
    module_telemetry_storage_t records[MAX_HISTORY_RECORDS];
    int record_count;
    int current_index;
    unsigned long oldest_timestamp;
    unsigned long newest_timestamp;
} module_telemetry_history_t;

typedef struct {
    module_command_storage_t records[MAX_COMMAND_HISTORY];
    int record_count;
    int current_index;
    unsigned long oldest_timestamp;
    unsigned long newest_timestamp;
} module_command_history_t;

// Global Module Data Storage
extern module_data_t g_module_data[MAX_MODULES];
extern module_telemetry_history_t g_module_telemetry_history[MAX_MODULES];
extern module_command_history_t g_module_command_history[MAX_MODULES];

// Module Data Storage Core Functions
hal_status_t module_data_storage_init(void);
hal_status_t module_data_storage_deinit(void);
hal_status_t module_data_storage_get_status(int *active_modules, int *total_modules);

// Module Telemetry Storage Functions
hal_status_t module_data_storage_update_telemetry(int module_id, const module_telemetry_storage_t *telemetry);
hal_status_t module_data_storage_get_telemetry(int module_id, module_telemetry_storage_t *telemetry);
hal_status_t module_data_storage_get_telemetry_history(int module_id, int max_records, 
                                                      module_telemetry_storage_t *history, int *record_count);

// Module Configuration Storage Functions
hal_status_t module_data_storage_update_config(int module_id, const module_config_storage_t *config);
hal_status_t module_data_storage_get_config(int module_id, module_config_storage_t *config);
hal_status_t module_data_storage_set_config(int module_id, const module_config_storage_t *config);

// Module Health Storage Functions
hal_status_t module_data_storage_update_health(int module_id, const module_health_storage_t *health);
hal_status_t module_data_storage_get_health(int module_id, module_health_storage_t *health);

// Module Command Storage Functions
hal_status_t module_data_storage_add_command(int module_id, const module_command_storage_t *command);
hal_status_t module_data_storage_get_command_history(int module_id, int max_records,
                                                    module_command_storage_t *history, int *record_count);

// Module Data Validation Functions
bool module_data_storage_is_module_active(int module_id);
bool module_data_storage_is_telemetry_fresh(int module_id);
bool module_data_storage_is_config_valid(int module_id);
bool module_data_storage_is_health_valid(int module_id);

// Module Data Utility Functions
hal_status_t module_data_storage_cleanup_old_data(int module_id);
hal_status_t module_data_storage_reset_module_data(int module_id);
hal_status_t module_data_storage_get_module_summary(int module_id, char *summary, size_t summary_size);

// Module Data Statistics Functions
hal_status_t module_data_storage_get_statistics(int module_id, char *stats_json, size_t stats_size);
hal_status_t module_data_storage_get_system_statistics(char *stats_json, size_t stats_size);

#endif // MODULE_DATA_STORAGE_H
