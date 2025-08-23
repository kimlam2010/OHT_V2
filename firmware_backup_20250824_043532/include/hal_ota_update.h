/**
 * @file hal_ota_update.h
 * @brief OTA (Over-The-Air) Update HAL for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-06 (OTA Update System)
 */

#ifndef HAL_OTA_UPDATE_H
#define HAL_OTA_UPDATE_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// OTA Update Configuration
#define OTA_UPDATE_MAX_URL_LENGTH       256
#define OTA_UPDATE_MAX_VERSION_LENGTH   32
#define OTA_UPDATE_MAX_CHECKSUM_LENGTH  64
#define OTA_UPDATE_MAX_DESCRIPTION_LENGTH 128
#define OTA_UPDATE_BUFFER_SIZE          4096
#define OTA_UPDATE_MAX_RETRIES          3
#define OTA_UPDATE_TIMEOUT_MS           30000  // 30 seconds
#define OTA_UPDATE_BACKUP_SLOTS         2

// OTA Update States
typedef enum {
    OTA_UPDATE_STATE_IDLE = 0,
    OTA_UPDATE_STATE_CHECKING,
    OTA_UPDATE_STATE_DOWNLOADING,
    OTA_UPDATE_STATE_VERIFYING,
    OTA_UPDATE_STATE_INSTALLING,
    OTA_UPDATE_STATE_COMPLETED,
    OTA_UPDATE_STATE_FAILED,
    OTA_UPDATE_STATE_ROLLBACK
} ota_update_state_t;

// OTA Update Error Types
typedef enum {
    OTA_UPDATE_ERROR_NONE = 0,
    OTA_UPDATE_ERROR_NETWORK,
    OTA_UPDATE_ERROR_DOWNLOAD,
    OTA_UPDATE_ERROR_VERIFICATION,
    OTA_UPDATE_ERROR_INSTALLATION,
    OTA_UPDATE_ERROR_ROLLBACK,
    OTA_UPDATE_ERROR_INSUFFICIENT_SPACE,
    OTA_UPDATE_ERROR_INVALID_FIRMWARE,
    OTA_UPDATE_ERROR_VERSION_MISMATCH,
    OTA_UPDATE_ERROR_CHECKSUM_MISMATCH,
    OTA_UPDATE_ERROR_TIMEOUT
} ota_update_error_t;

// OTA Update Event Types
typedef enum {
    OTA_UPDATE_EVENT_NONE = 0,
    OTA_UPDATE_EVENT_CHECK_STARTED,
    OTA_UPDATE_EVENT_CHECK_COMPLETED,
    OTA_UPDATE_EVENT_DOWNLOAD_STARTED,
    OTA_UPDATE_EVENT_DOWNLOAD_PROGRESS,
    OTA_UPDATE_EVENT_DOWNLOAD_COMPLETED,
    OTA_UPDATE_EVENT_VERIFICATION_STARTED,
    OTA_UPDATE_EVENT_VERIFICATION_COMPLETED,
    OTA_UPDATE_EVENT_INSTALLATION_STARTED,
    OTA_UPDATE_EVENT_INSTALLATION_PROGRESS,
    OTA_UPDATE_EVENT_INSTALLATION_COMPLETED,
    OTA_UPDATE_EVENT_ROLLBACK_STARTED,
    OTA_UPDATE_EVENT_ROLLBACK_COMPLETED,
    OTA_UPDATE_EVENT_ERROR
} ota_update_event_t;

// OTA Update Firmware Info
typedef struct {
    char version[OTA_UPDATE_MAX_VERSION_LENGTH];
    char description[OTA_UPDATE_MAX_DESCRIPTION_LENGTH];
    uint32_t size_bytes;
    char checksum[OTA_UPDATE_MAX_CHECKSUM_LENGTH];
    uint64_t build_timestamp;
    uint32_t compatibility_version;
    bool is_stable;
    bool requires_reboot;
} ota_update_firmware_info_t;

// OTA Update Configuration
typedef struct {
    char update_server_url[OTA_UPDATE_MAX_URL_LENGTH];
    char current_version[OTA_UPDATE_MAX_VERSION_LENGTH];
    uint32_t check_interval_ms;
    uint32_t download_timeout_ms;
    uint32_t verification_timeout_ms;
    uint32_t installation_timeout_ms;
    bool auto_check_enabled;
    bool auto_download_enabled;
    bool auto_install_enabled;
    bool rollback_enabled;
    uint32_t max_rollback_attempts;
    bool verify_checksum;
    bool verify_signature;
    char certificate_path[128];
} ota_update_config_t;

// OTA Update Status
typedef struct {
    ota_update_state_t state;
    ota_update_error_t last_error;
    uint32_t progress_percent;
    uint64_t downloaded_bytes;
    uint64_t total_bytes;
    uint64_t start_time;
    uint64_t last_update_time;
    uint32_t retry_count;
    bool rollback_available;
    char current_firmware_version[OTA_UPDATE_MAX_VERSION_LENGTH];
    char available_firmware_version[OTA_UPDATE_MAX_VERSION_LENGTH];
} ota_update_status_t;

// OTA Update Statistics
typedef struct {
    uint32_t total_updates;
    uint32_t successful_updates;
    uint32_t failed_updates;
    uint32_t rollback_count;
    uint64_t total_download_time;
    uint64_t total_installation_time;
    uint64_t last_update_time;
    uint32_t average_download_speed;
    uint32_t average_installation_time;
} ota_update_stats_t;

// OTA Update Event Callback
typedef void (*ota_update_event_callback_t)(ota_update_event_t event, const void *data);

// Public API Functions

/**
 * @brief Initialize OTA Update HAL
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t hal_ota_update_init(const ota_update_config_t *config);

/**
 * @brief Deinitialize OTA Update HAL
 * @return HAL status
 */
hal_status_t hal_ota_update_deinit(void);

/**
 * @brief Update OTA Update HAL (call periodically)
 * @return HAL status
 */
hal_status_t hal_ota_update_update(void);

/**
 * @brief Get OTA Update status
 * @param status Status structure
 * @return HAL status
 */
hal_status_t hal_ota_update_get_status(ota_update_status_t *status);

/**
 * @brief Get OTA Update configuration
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t hal_ota_update_get_config(ota_update_config_t *config);

/**
 * @brief Set OTA Update configuration
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t hal_ota_update_set_config(const ota_update_config_t *config);

/**
 * @brief Get OTA Update statistics
 * @param stats Statistics structure
 * @return HAL status
 */
hal_status_t hal_ota_update_get_statistics(ota_update_stats_t *stats);

/**
 * @brief Reset OTA Update statistics
 * @return HAL status
 */
hal_status_t hal_ota_update_reset_statistics(void);

/**
 * @brief Set OTA Update event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t hal_ota_update_set_callback(ota_update_event_callback_t callback);

// OTA Update Operations

/**
 * @brief Check for available firmware updates
 * @param firmware_info Firmware info structure (output)
 * @return HAL status
 */
hal_status_t hal_ota_update_check_for_updates(ota_update_firmware_info_t *firmware_info);

/**
 * @brief Download firmware update
 * @param firmware_info Firmware info structure
 * @return HAL status
 */
hal_status_t hal_ota_update_download_firmware(const ota_update_firmware_info_t *firmware_info);

/**
 * @brief Verify downloaded firmware
 * @param firmware_info Firmware info structure
 * @return HAL status
 */
hal_status_t hal_ota_update_verify_firmware(const ota_update_firmware_info_t *firmware_info);

/**
 * @brief Install firmware update
 * @param firmware_info Firmware info structure
 * @return HAL status
 */
hal_status_t hal_ota_update_install_firmware(const ota_update_firmware_info_t *firmware_info);

/**
 * @brief Rollback to previous firmware
 * @return HAL status
 */
hal_status_t hal_ota_update_rollback_firmware(void);

/**
 * @brief Cancel current OTA operation
 * @return HAL status
 */
hal_status_t hal_ota_update_cancel_operation(void);

/**
 * @brief Get firmware info for current version
 * @param firmware_info Firmware info structure
 * @return HAL status
 */
hal_status_t hal_ota_update_get_current_firmware_info(ota_update_firmware_info_t *firmware_info);

/**
 * @brief Get firmware info for backup versions
 * @param firmware_info Array of firmware info structures
 * @param max_count Maximum number of entries
 * @param actual_count Actual number of entries (output)
 * @return HAL status
 */
hal_status_t hal_ota_update_get_backup_firmware_info(ota_update_firmware_info_t *firmware_info, 
                                                   uint32_t max_count, uint32_t *actual_count);

// Utility Functions

/**
 * @brief Get OTA Update state name
 * @param state OTA Update state
 * @return State name string
 */
const char* hal_ota_update_get_state_name(ota_update_state_t state);

/**
 * @brief Get OTA Update error name
 * @param error OTA Update error
 * @return Error name string
 */
const char* hal_ota_update_get_error_name(ota_update_error_t error);

/**
 * @brief Get OTA Update event name
 * @param event OTA Update event
 * @return Event name string
 */
const char* hal_ota_update_get_event_name(ota_update_event_t event);

/**
 * @brief Get OTA Update diagnostics
 * @param info Diagnostic information buffer
 * @param max_len Maximum buffer length
 * @return HAL status
 */
hal_status_t hal_ota_update_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Perform OTA Update self-test
 * @return HAL status
 */
hal_status_t hal_ota_update_self_test(void);

/**
 * @brief Reset OTA Update system
 * @return HAL status
 */
hal_status_t hal_ota_update_reset(void);

#endif // HAL_OTA_UPDATE_H
