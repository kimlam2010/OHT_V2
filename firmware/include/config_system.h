/**
 * @file config_system.h
 * @brief Configuration System for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#ifndef CONFIG_SYSTEM_H
#define CONFIG_SYSTEM_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Configuration sections
typedef enum {
    CONFIG_SECTION_SYSTEM = 0,
    CONFIG_SECTION_SAFETY,
    CONFIG_SECTION_COMMUNICATION,
    CONFIG_SECTION_MODULES,
    CONFIG_SECTION_PERFORMANCE,
    CONFIG_SECTION_MAX
} config_section_t;

// System configuration
typedef struct {
    char device_name[64];
    char device_version[32];
    uint32_t startup_timeout_ms;
    uint32_t watchdog_timeout_ms;
    bool debug_mode;
    uint32_t log_level;
} system_config_t;

// Safety configuration
typedef struct {
    uint32_t estop_timeout_ms;
    uint32_t safety_check_interval_ms;
    uint32_t fault_clear_timeout_ms;
    bool dual_channel_estop;
    uint32_t emergency_stop_delay_ms;
} safety_config_t;

// Communication configuration
typedef struct {
    uint32_t rs485_baud_rate;
    uint32_t rs485_timeout_ms;
    uint32_t rs485_retry_count;
    uint32_t rs485_retry_delay_ms;
    bool rs485_auto_discovery;
    uint32_t discovery_interval_ms;
} communication_config_t;

// Module configuration
typedef struct {
    uint8_t power_module_address;
    uint8_t motor_module_address;
    uint8_t io_module_address;
    uint32_t module_timeout_ms;
    uint32_t module_heartbeat_interval_ms;
} module_config_t;

// Performance configuration
typedef struct {
    uint32_t control_loop_frequency_hz;
    uint32_t telemetry_interval_ms;
    uint32_t diagnostics_interval_ms;
    bool performance_monitoring;
    uint32_t metrics_update_interval_ms;
} performance_config_t;

// Main configuration structure
typedef struct {
    system_config_t system;
    safety_config_t safety;
    communication_config_t communication;
    module_config_t modules;
    performance_config_t performance;
    uint64_t last_modified_time;
    uint32_t config_version;
} master_config_t;

// Configuration validation result
typedef struct {
    bool valid;
    char error_message[256];
    config_section_t failed_section;
} config_validation_t;

// Function prototypes
hal_status_t config_system_init(const char *config_file_path);
hal_status_t config_system_deinit(void);
hal_status_t config_system_load(void);
hal_status_t config_system_save(void);
hal_status_t config_system_reset_to_defaults(void);

// Configuration access functions
hal_status_t config_system_get_master_config(master_config_t *config);
hal_status_t config_system_set_master_config(const master_config_t *config);

hal_status_t config_system_get_system_config(system_config_t *config);
hal_status_t config_system_set_system_config(const system_config_t *config);

hal_status_t config_system_get_safety_config(safety_config_t *config);
hal_status_t config_system_set_safety_config(const safety_config_t *config);

hal_status_t config_system_get_communication_config(communication_config_t *config);
hal_status_t config_system_set_communication_config(const communication_config_t *config);

hal_status_t config_system_get_module_config(module_config_t *config);
hal_status_t config_system_set_module_config(const module_config_t *config);

hal_status_t config_system_get_performance_config(performance_config_t *config);
hal_status_t config_system_set_performance_config(const performance_config_t *config);

// Configuration validation
config_validation_t config_system_validate_config(const master_config_t *config);
hal_status_t config_system_validate_section(config_section_t section, const void *config_data);

// Configuration backup and restore
hal_status_t config_system_backup(const char *backup_file_path);
hal_status_t config_system_restore(const char *backup_file_path);

// Configuration monitoring
hal_status_t config_system_register_change_callback(void (*callback)(config_section_t section));
hal_status_t config_system_get_last_modified_time(uint64_t *timestamp);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_SYSTEM_H
