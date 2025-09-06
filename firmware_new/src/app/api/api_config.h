/**
 * @file api_config.h
 * @brief API Configuration Management for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.1
 */

#ifndef API_CONFIG_H
#define API_CONFIG_H

#include "hal_common.h"
#include "api_manager.h"

// Configuration Management Version
#define API_CONFIG_VERSION_MAJOR 1
#define API_CONFIG_VERSION_MINOR 0
#define API_CONFIG_VERSION_PATCH 0
#define API_CONFIG_VERSION_STRING "1.0.0"

// Configuration Categories
typedef enum {
    API_CONFIG_CATEGORY_SYSTEM = 0,
    API_CONFIG_CATEGORY_SAFETY,
    API_CONFIG_CATEGORY_MOTOR,
    API_CONFIG_CATEGORY_POWER,
    API_CONFIG_CATEGORY_DOCK,
    API_CONFIG_CATEGORY_NETWORK,
    API_CONFIG_CATEGORY_MAX
} api_config_category_t;

// Configuration Data Types
typedef enum {
    API_CONFIG_TYPE_STRING = 0,
    API_CONFIG_TYPE_INTEGER,
    API_CONFIG_TYPE_FLOAT,
    API_CONFIG_TYPE_BOOLEAN,
    API_CONFIG_TYPE_JSON,
    API_CONFIG_TYPE_MAX
} api_config_type_t;

// Configuration Parameter Structure
typedef struct {
    char key[64];                    // Configuration key
    char description[128];           // Parameter description
    api_config_type_t type;          // Data type
    api_config_category_t category;  // Configuration category
    bool read_only;                  // Read-only flag
    bool requires_restart;           // Requires restart flag
    char default_value[256];         // Default value
    char current_value[256];         // Current value
    char min_value[64];              // Minimum value (for numeric types)
    char max_value[64];              // Maximum value (for numeric types)
    char valid_values[512];          // Valid values (for enum types)
} api_config_parameter_t;

// Configuration Category Structure
typedef struct {
    api_config_category_t category;
    char name[32];
    char description[128];
    uint32_t parameter_count;
    api_config_parameter_t parameters[32];  // Max 32 parameters per category
} api_config_category_info_t;

// Configuration Response Structure
typedef struct {
    bool success;
    char message[256];
    uint32_t parameter_count;
    api_config_parameter_t parameters[64];  // Max 64 parameters total
} api_config_response_t;

// Configuration Update Request Structure
typedef struct {
    char key[64];
    char value[256];
    bool validate_only;              // If true, only validate without applying
} api_config_update_request_t;

// Configuration Update Response Structure
typedef struct {
    bool success;
    char message[256];
    bool requires_restart;
    char validation_errors[512];
} api_config_update_response_t;

// Configuration Backup Structure
typedef struct {
    char timestamp[32];
    char version[16];
    uint32_t parameter_count;
    api_config_parameter_t parameters[128];  // Max 128 parameters in backup
} api_config_backup_t;

// Configuration Management Functions

/**
 * @brief Initialize configuration management
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_init(void);

/**
 * @brief Deinitialize configuration management
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_deinit(void);

/**
 * @brief Get all configuration parameters
 * @param response Configuration response structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_all(api_config_response_t *response);

/**
 * @brief Get configuration parameters by category
 * @param category Configuration category
 * @param response Configuration response structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_category(api_config_category_t category, api_config_response_t *response);

/**
 * @brief Get specific configuration parameter
 * @param key Configuration key
 * @param parameter Configuration parameter structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_parameter(const char *key, api_config_parameter_t *parameter);

/**
 * @brief Set configuration parameter
 * @param request Configuration update request
 * @param response Configuration update response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_set_parameter(const api_config_update_request_t *request, api_config_update_response_t *response);

/**
 * @brief Validate configuration parameter
 * @param key Configuration key
 * @param value Configuration value
 * @return hal_status_t HAL_STATUS_OK if valid, error code if invalid
 */
hal_status_t api_config_validate_parameter(const char *key, const char *value);

/**
 * @brief Reset configuration to defaults
 * @param category Configuration category (or API_CONFIG_CATEGORY_MAX for all)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_reset_defaults(api_config_category_t category);

/**
 * @brief Create configuration backup
 * @param backup Configuration backup structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_create_backup(api_config_backup_t *backup);

/**
 * @brief Restore configuration from backup
 * @param backup Configuration backup structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_restore_backup(const api_config_backup_t *backup);

/**
 * @brief Get configuration categories
 * @param categories Array of category info structures
 * @param max_categories Maximum number of categories
 * @param actual_count Actual number of categories returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_categories(api_config_category_info_t *categories, uint32_t max_categories, uint32_t *actual_count);

/**
 * @brief Apply configuration changes
 * @param requires_restart Pointer to restart flag
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_apply_changes(bool *requires_restart);

/**
 * @brief Get configuration status
 * @param initialized Pointer to initialized flag
 * @param parameter_count Pointer to parameter count
 * @param last_update Pointer to last update timestamp
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_status(bool *initialized, uint32_t *parameter_count, uint64_t *last_update);

/**
 * @brief Get configuration version
 * @param major Major version number
 * @param minor Minor version number
 * @param patch Patch version number
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch);

/**
 * @brief Get configuration version string
 * @return const char* Version string
 */
const char* api_config_get_version_string(void);

// Configuration Endpoint Handlers

/**
 * @brief Handle GET /api/v1/config - Get all configuration
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_get_all(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/config/{category} - Get configuration by category
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_get_category(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/config/parameter/{key} - Get specific parameter
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_get_parameter(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/config/parameter/{key} - Set configuration parameter
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_set_parameter(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/config/reset - Reset configuration to defaults
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_reset(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/config/backup - Create configuration backup
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_backup(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/config/restore - Restore configuration from backup
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_restore(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle GET /api/v1/config/categories - Get configuration categories
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_categories(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Handle POST /api/v1/config/apply - Apply configuration changes
 * @param request HTTP request
 * @param response HTTP response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_handle_config_apply(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

// JSON Serialization Functions

/**
 * @brief Serialize configuration response to JSON
 * @param response Configuration response
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_config_response_json(const api_config_response_t *response, char *buffer, size_t buffer_size);

/**
 * @brief Serialize configuration parameter to JSON
 * @param parameter Configuration parameter
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_config_parameter_json(const api_config_parameter_t *parameter, char *buffer, size_t buffer_size);

/**
 * @brief Serialize configuration categories to JSON
 * @param categories Configuration categories
 * @param count Number of categories
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_config_categories_json(const api_config_category_info_t *categories, uint32_t count, char *buffer, size_t buffer_size);

/**
 * @brief Serialize configuration backup to JSON
 * @param backup Configuration backup
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_serialize_config_backup_json(const api_config_backup_t *backup, char *buffer, size_t buffer_size);

// Utility Functions

/**
 * @brief Get configuration category name
 * @param category Configuration category
 * @return const char* Category name
 */
const char* api_config_get_category_name(api_config_category_t category);

/**
 * @brief Get configuration category description
 * @param category Configuration category
 * @return const char* Category description
 */
const char* api_config_get_category_description(api_config_category_t category);

/**
 * @brief Get configuration type name
 * @param type Configuration type
 * @return const char* Type name
 */
const char* api_config_get_type_name(api_config_type_t type);

/**
 * @brief Parse configuration value
 * @param value String value
 * @param type Configuration type
 * @param result Parsed result buffer
 * @param result_size Result buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_parse_value(const char *value, api_config_type_t type, void *result, size_t result_size);

/**
 * @brief Format configuration value
 * @param value Value to format
 * @param type Configuration type
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_format_value(const void *value, api_config_type_t type, char *buffer, size_t buffer_size);

#endif // API_CONFIG_H
