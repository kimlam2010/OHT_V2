/**
 * @file hal_config_persistence.h
 * @brief HAL header for Configuration Persistence System
 * @version 1.0.0
 * @date 2025-01-27
 * @team FIRMWARE
 * @task Task 3.2 (Configuration Persistence)
 */

#ifndef HAL_CONFIG_PERSISTENCE_H
#define HAL_CONFIG_PERSISTENCE_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// Configuration storage paths
#define CONFIG_STORAGE_PATH         "/etc/oht50"
#define CONFIG_PRIMARY_FILE         "/etc/oht50/config.json"
#define CONFIG_BACKUP_FILE          "/etc/oht50/config_backup.json"
#define CONFIG_FACTORY_FILE         "/etc/oht50/config_factory.json"
#define CONFIG_TEMP_FILE            "/tmp/oht50_config_temp.json"
#define CONFIG_LOCK_FILE            "/var/lock/oht50_config.lock"

// Configuration limits
#define CONFIG_MAX_FILE_SIZE        (64 * 1024)  // 64KB
#define CONFIG_MAX_KEY_LENGTH       64
#define CONFIG_MAX_VALUE_LENGTH     256
#define CONFIG_MAX_SECTION_LENGTH   32
#define CONFIG_VERSION_LENGTH       16
#define CONFIG_CHECKSUM_LENGTH      8

// Configuration types
typedef enum {
    CONFIG_TYPE_SYSTEM = 0,
    CONFIG_TYPE_HARDWARE,
    CONFIG_TYPE_NETWORK,
    CONFIG_TYPE_SAFETY,
    CONFIG_TYPE_DEBUG,
    CONFIG_TYPE_USER
} config_type_t;

// Configuration states
typedef enum {
    CONFIG_STATE_UNINITIALIZED = 0,
    CONFIG_STATE_LOADING,
    CONFIG_STATE_READY,
    CONFIG_STATE_SAVING,
    CONFIG_STATE_ERROR,
    CONFIG_STATE_CORRUPTED,
    CONFIG_STATE_LOCKED
} config_state_t;

// Configuration events
typedef enum {
    CONFIG_EVENT_LOADED = 0,
    CONFIG_EVENT_SAVED,
    CONFIG_EVENT_BACKUP_CREATED,
    CONFIG_EVENT_RESTORED,
    CONFIG_EVENT_RESET_TO_FACTORY,
    CONFIG_EVENT_ERROR,
    CONFIG_EVENT_CORRUPTED,
    CONFIG_EVENT_LOCKED,
    CONFIG_EVENT_UNLOCKED
} config_event_t;

// Configuration entry structure
typedef struct {
    char section[CONFIG_MAX_SECTION_LENGTH];
    char key[CONFIG_MAX_KEY_LENGTH];
    char value[CONFIG_MAX_VALUE_LENGTH];
    config_type_t type;
    bool is_readonly;
    bool is_sensitive;
    uint64_t timestamp_us;
} config_entry_t;

// Configuration header structure
typedef struct {
    char magic[8];              // "OHT50CFG"
    char version[CONFIG_VERSION_LENGTH];
    uint32_t entry_count;
    uint32_t file_size;
    char checksum[CONFIG_CHECKSUM_LENGTH];
    uint64_t created_timestamp_us;
    uint64_t modified_timestamp_us;
    uint32_t backup_count;
    bool factory_reset_flag;
    bool emergency_mode;
    uint8_t reserved[16];
} __attribute__((packed)) config_header_t;

// Configuration statistics
typedef struct {
    uint32_t total_entries;
    uint32_t load_count;
    uint32_t save_count;
    uint32_t backup_count;
    uint32_t restore_count;
    uint32_t error_count;
    uint32_t corruption_count;
    uint64_t total_size_bytes;
    uint64_t last_save_timestamp_us;
    uint64_t last_backup_timestamp_us;
} config_statistics_t;

// Configuration status
typedef struct {
    config_state_t state;
    bool initialized;
    bool loaded;
    bool modified;
    bool locked;
    bool backup_available;
    bool factory_available;
    uint32_t entry_count;
    uint32_t file_size;
    uint64_t last_modified_us;
    char current_version[CONFIG_VERSION_LENGTH];
    char checksum[CONFIG_CHECKSUM_LENGTH];
} config_status_t;

// Configuration validation result
typedef struct {
    bool valid;
    bool header_valid;
    bool checksum_valid;
    bool entries_valid;
    uint32_t error_count;
    char error_message[256];
} config_validation_t;

// Configuration event callback
typedef void (*config_event_callback_t)(config_event_t event, const void *data);

// HAL Configuration Persistence functions
hal_status_t hal_config_init(void);
hal_status_t hal_config_deinit(void);
hal_status_t hal_config_load(void);
hal_status_t hal_config_save(void);
hal_status_t hal_config_reload(void);

// Configuration entry management
hal_status_t hal_config_set_string(const char *section, const char *key, const char *value);
hal_status_t hal_config_get_string(const char *section, const char *key, char *value, size_t max_length);
hal_status_t hal_config_set_int(const char *section, const char *key, int32_t value);
hal_status_t hal_config_get_int(const char *section, const char *key, int32_t *value);
hal_status_t hal_config_set_bool(const char *section, const char *key, bool value);
hal_status_t hal_config_get_bool(const char *section, const char *key, bool *value);
hal_status_t hal_config_set_float(const char *section, const char *key, float value);
hal_status_t hal_config_get_float(const char *section, const char *key, float *value);

// Configuration entry utilities
hal_status_t hal_config_delete_entry(const char *section, const char *key);
hal_status_t hal_config_entry_exists(const char *section, const char *key, bool *exists);
hal_status_t hal_config_get_entry_count(uint32_t *count);
hal_status_t hal_config_list_sections(char sections[][CONFIG_MAX_SECTION_LENGTH], uint32_t max_sections, uint32_t *section_count);
hal_status_t hal_config_list_keys(const char *section, char keys[][CONFIG_MAX_KEY_LENGTH], uint32_t max_keys, uint32_t *key_count);

// Backup and restore functions
hal_status_t hal_config_create_backup(void);
hal_status_t hal_config_restore_from_backup(void);
hal_status_t hal_config_delete_backup(void);
hal_status_t hal_config_backup_exists(bool *exists);

// Factory reset functions
hal_status_t hal_config_reset_to_factory(void);
hal_status_t hal_config_create_factory_defaults(void);
hal_status_t hal_config_factory_exists(bool *exists);

// Configuration validation
hal_status_t hal_config_validate(config_validation_t *result);
hal_status_t hal_config_repair(void);
hal_status_t hal_config_verify_checksum(bool *valid);

// Configuration locking
hal_status_t hal_config_lock(void);
hal_status_t hal_config_unlock(void);
hal_status_t hal_config_is_locked(bool *locked);

// Configuration status and info
hal_status_t hal_config_get_status(config_status_t *status);
hal_status_t hal_config_get_statistics(config_statistics_t *statistics);
hal_status_t hal_config_reset_statistics(void);

// Event handling
hal_status_t hal_config_set_event_callback(config_event_callback_t callback);

// Configuration import/export
hal_status_t hal_config_export_to_file(const char *filepath);
hal_status_t hal_config_import_from_file(const char *filepath);
hal_status_t hal_config_export_to_buffer(char *buffer, size_t buffer_size, size_t *actual_size);
hal_status_t hal_config_import_from_buffer(const char *buffer, size_t buffer_size);

// Configuration versioning
hal_status_t hal_config_get_version(char *version, size_t max_length);
hal_status_t hal_config_set_version(const char *version);
hal_status_t hal_config_is_version_compatible(const char *version, bool *compatible);

// Emergency mode functions
hal_status_t hal_config_enter_emergency_mode(void);
hal_status_t hal_config_exit_emergency_mode(void);
hal_status_t hal_config_is_emergency_mode(bool *emergency);

// Configuration templates
hal_status_t hal_config_load_template(const char *template_name);
hal_status_t hal_config_save_as_template(const char *template_name);
hal_status_t hal_config_list_templates(char templates[][64], uint32_t max_templates, uint32_t *template_count);

// Utility functions
hal_status_t hal_config_calculate_checksum(const char *filepath, char *checksum);
hal_status_t hal_config_get_file_size(const char *filepath, uint32_t *size);
hal_status_t hal_config_ensure_directory(const char *dirpath);

// Default configuration values
#define CONFIG_DEFAULT_SYSTEM_NAME          "OHT-50 Master Module"
#define CONFIG_DEFAULT_SYSTEM_VERSION       "1.0.0"
#define CONFIG_DEFAULT_LOG_LEVEL            "INFO"
#define CONFIG_DEFAULT_DEBUG_ENABLED        false
#define CONFIG_DEFAULT_SAFETY_TIMEOUT_MS    1000
#define CONFIG_DEFAULT_RS485_BAUD_RATE      115200
#define CONFIG_DEFAULT_NETWORK_DHCP         true
#define CONFIG_DEFAULT_LED_BRIGHTNESS       100
#define CONFIG_DEFAULT_WATCHDOG_TIMEOUT     30

#endif // HAL_CONFIG_PERSISTENCE_H
