/**
 * @file api_config.c
 * @brief API Configuration Management Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 4.1
 */

#include "api_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Global variables
static bool g_api_config_initialized = false;
static uint32_t g_config_parameter_count = 0;
static uint64_t g_last_config_update = 0;
static api_config_parameter_t g_config_parameters[128];  // Max 128 parameters

// Default configuration parameters
static const api_config_parameter_t g_default_parameters[] = {
    // System Configuration
    {
        .key = "system.name",
        .description = "System name",
        .type = API_CONFIG_TYPE_STRING,
        .category = API_CONFIG_CATEGORY_SYSTEM,
        .read_only = false,
        .requires_restart = true,
        .default_value = "OHT-50",
        .current_value = "OHT-50",
        .min_value = "",
        .max_value = "",
        .valid_values = ""
    },
    {
        .key = "system.version",
        .description = "System version",
        .type = API_CONFIG_TYPE_STRING,
        .category = API_CONFIG_CATEGORY_SYSTEM,
        .read_only = true,
        .requires_restart = false,
        .default_value = "1.0.0",
        .current_value = "1.0.0",
        .min_value = "",
        .max_value = "",
        .valid_values = ""
    },
    {
        .key = "system.debug_mode",
        .description = "Enable debug mode",
        .type = API_CONFIG_TYPE_BOOLEAN,
        .category = API_CONFIG_CATEGORY_SYSTEM,
        .read_only = false,
        .requires_restart = true,
        .default_value = "false",
        .current_value = "false",
        .min_value = "",
        .max_value = "",
        .valid_values = "true,false"
    },
    {
        .key = "system.log_level",
        .description = "Logging level",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_SYSTEM,
        .read_only = false,
        .requires_restart = false,
        .default_value = "2",
        .current_value = "2",
        .min_value = "0",
        .max_value = "5",
        .valid_values = "0=ERROR,1=WARNING,2=INFO,3=DEBUG,4=TRACE,5=VERBOSE"
    },
    
    // Safety Configuration
    {
        .key = "safety.estop_timeout",
        .description = "E-Stop timeout (ms)",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_SAFETY,
        .read_only = false,
        .requires_restart = false,
        .default_value = "100",
        .current_value = "100",
        .min_value = "10",
        .max_value = "1000",
        .valid_values = ""
    },
    {
        .key = "safety.safety_zones_enabled",
        .description = "Enable safety zones",
        .type = API_CONFIG_TYPE_BOOLEAN,
        .category = API_CONFIG_CATEGORY_SAFETY,
        .read_only = false,
        .requires_restart = false,
        .default_value = "true",
        .current_value = "true",
        .min_value = "",
        .max_value = "",
        .valid_values = "true,false"
    },
    {
        .key = "safety.max_speed_limit",
        .description = "Maximum speed limit (mm/s)",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_SAFETY,
        .read_only = false,
        .requires_restart = false,
        .default_value = "1000",
        .current_value = "1000",
        .min_value = "100",
        .max_value = "5000",
        .valid_values = ""
    },
    
    // Motor Configuration
    {
        .key = "motor.max_velocity",
        .description = "Maximum motor velocity (mm/s)",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_MOTOR,
        .read_only = false,
        .requires_restart = false,
        .default_value = "1000",
        .current_value = "1000",
        .min_value = "100",
        .max_value = "5000",
        .valid_values = ""
    },
    {
        .key = "motor.max_acceleration",
        .description = "Maximum motor acceleration (mm/s²)",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_MOTOR,
        .read_only = false,
        .requires_restart = false,
        .default_value = "2000",
        .current_value = "2000",
        .min_value = "100",
        .max_value = "10000",
        .valid_values = ""
    },
    {
        .key = "motor.position_tolerance",
        .description = "Position tolerance (mm)",
        .type = API_CONFIG_TYPE_FLOAT,
        .category = API_CONFIG_CATEGORY_MOTOR,
        .read_only = false,
        .requires_restart = false,
        .default_value = "1.0",
        .current_value = "1.0",
        .min_value = "0.1",
        .max_value = "10.0",
        .valid_values = ""
    },
    
    // Power Configuration
    {
        .key = "power.battery_low_threshold",
        .description = "Battery low threshold (%)",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_POWER,
        .read_only = false,
        .requires_restart = false,
        .default_value = "20",
        .current_value = "20",
        .min_value = "5",
        .max_value = "50",
        .valid_values = ""
    },
    {
        .key = "power.battery_critical_threshold",
        .description = "Battery critical threshold (%)",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_POWER,
        .read_only = false,
        .requires_restart = false,
        .default_value = "10",
        .current_value = "10",
        .min_value = "1",
        .max_value = "20",
        .valid_values = ""
    },
    {
        .key = "power.auto_shutdown_enabled",
        .description = "Enable auto shutdown on low battery",
        .type = API_CONFIG_TYPE_BOOLEAN,
        .category = API_CONFIG_CATEGORY_POWER,
        .read_only = false,
        .requires_restart = false,
        .default_value = "true",
        .current_value = "true",
        .min_value = "",
        .max_value = "",
        .valid_values = "true,false"
    },
    
    // Dock Configuration
    {
        .key = "dock.approach_distance",
        .description = "Dock approach distance (mm)",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_DOCK,
        .read_only = false,
        .requires_restart = false,
        .default_value = "100",
        .current_value = "100",
        .min_value = "50",
        .max_value = "500",
        .valid_values = ""
    },
    {
        .key = "dock.alignment_tolerance",
        .description = "Dock alignment tolerance (degrees)",
        .type = API_CONFIG_TYPE_FLOAT,
        .category = API_CONFIG_CATEGORY_DOCK,
        .read_only = false,
        .requires_restart = false,
        .default_value = "2.0",
        .current_value = "2.0",
        .min_value = "0.5",
        .max_value = "10.0",
        .valid_values = ""
    },
    {
        .key = "dock.docking_timeout",
        .description = "Docking timeout (seconds)",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_DOCK,
        .read_only = false,
        .requires_restart = false,
        .default_value = "30",
        .current_value = "30",
        .min_value = "5",
        .max_value = "120",
        .valid_values = ""
    },
    
    // Network Configuration
    {
        .key = "network.http_port",
        .description = "HTTP server port",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_NETWORK,
        .read_only = false,
        .requires_restart = true,
        .default_value = "8080",
        .current_value = "8080",
        .min_value = "1024",
        .max_value = "65535",
        .valid_values = ""
    },
    {
        .key = "network.websocket_port",
        .description = "WebSocket server port",
        .type = API_CONFIG_TYPE_INTEGER,
        .category = API_CONFIG_CATEGORY_NETWORK,
        .read_only = false,
        .requires_restart = true,
        .default_value = "8081",
        .current_value = "8081",
        .min_value = "1024",
        .max_value = "65535",
        .valid_values = ""
    },
    {
        .key = "network.cors_enabled",
        .description = "Enable CORS",
        .type = API_CONFIG_TYPE_BOOLEAN,
        .category = API_CONFIG_CATEGORY_NETWORK,
        .read_only = false,
        .requires_restart = true,
        .default_value = "true",
        .current_value = "true",
        .min_value = "",
        .max_value = "",
        .valid_values = "true,false"
    }
};

#define DEFAULT_PARAMETERS_COUNT (sizeof(g_default_parameters) / sizeof(g_default_parameters[0]))

/**
 * @brief Initialize configuration management
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_init(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Config: Initializing...");
    
    if (g_api_config_initialized) {
        hal_log_message(HAL_LOG_LEVEL_WARNING, "API Config: Already initialized");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize default parameters
    g_config_parameter_count = DEFAULT_PARAMETERS_COUNT;
    memcpy(g_config_parameters, g_default_parameters, sizeof(g_default_parameters));
    
    g_last_config_update = hal_get_timestamp_ms();
    g_api_config_initialized = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Config: Initialized with %u parameters", g_config_parameter_count);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize configuration management
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_deinit(void) {
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Config: Deinitializing...");
    
    g_api_config_initialized = false;
    g_config_parameter_count = 0;
    g_last_config_update = 0;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Config: Deinitialized");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get all configuration parameters
 * @param response Configuration response structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_all(api_config_response_t *response) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_config_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    response->success = true;
    strcpy(response->message, "Configuration retrieved successfully");
    response->parameter_count = g_config_parameter_count;
    
    for (uint32_t i = 0; i < g_config_parameter_count && i < 64; i++) {
        response->parameters[i] = g_config_parameters[i];
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get configuration parameters by category
 * @param category Configuration category
 * @param response Configuration response structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_category(api_config_category_t category, api_config_response_t *response) {
    if (response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_config_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    response->success = true;
    strcpy(response->message, "Category configuration retrieved successfully");
    response->parameter_count = 0;
    
    for (uint32_t i = 0; i < g_config_parameter_count && response->parameter_count < 64; i++) {
        if (g_config_parameters[i].category == category) {
            response->parameters[response->parameter_count] = g_config_parameters[i];
            response->parameter_count++;
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get specific configuration parameter
 * @param key Configuration key
 * @param parameter Configuration parameter structure
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_parameter(const char *key, api_config_parameter_t *parameter) {
    if (key == NULL || parameter == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_config_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    for (uint32_t i = 0; i < g_config_parameter_count; i++) {
        if (strcmp(g_config_parameters[i].key, key) == 0) {
            *parameter = g_config_parameters[i];
            return HAL_STATUS_OK;
        }
    }
    
    return HAL_STATUS_NOT_FOUND;
}

/**
 * @brief Set configuration parameter
 * @param request Configuration update request
 * @param response Configuration update response
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_set_parameter(const api_config_update_request_t *request, api_config_update_response_t *response) {
    if (request == NULL || response == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_config_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Find parameter
    api_config_parameter_t *parameter = NULL;
    for (uint32_t i = 0; i < g_config_parameter_count; i++) {
        if (strcmp(g_config_parameters[i].key, request->key) == 0) {
            parameter = &g_config_parameters[i];
            break;
        }
    }
    
    if (parameter == NULL) {
        response->success = false;
        strcpy(response->message, "Parameter not found");
        response->requires_restart = false;
        strcpy(response->validation_errors, "Parameter not found");
        return HAL_STATUS_NOT_FOUND;
    }
    
    // Check if read-only
    if (parameter->read_only) {
        response->success = false;
        strcpy(response->message, "Parameter is read-only");
        response->requires_restart = false;
        strcpy(response->validation_errors, "Parameter is read-only");
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate parameter
    hal_status_t validation_result = api_config_validate_parameter(request->key, request->value);
    if (validation_result != HAL_STATUS_OK) {
        response->success = false;
        strcpy(response->message, "Parameter validation failed");
        response->requires_restart = false;
        strcpy(response->validation_errors, "Parameter validation failed");
        return validation_result;
    }
    
    // If validate_only, don't apply changes
    if (request->validate_only) {
        response->success = true;
        strcpy(response->message, "Parameter validation successful");
        response->requires_restart = parameter->requires_restart;
        strcpy(response->validation_errors, "");
        return HAL_STATUS_OK;
    }
    
    // Apply changes
    strncpy(parameter->current_value, request->value, sizeof(parameter->current_value) - 1);
    parameter->current_value[sizeof(parameter->current_value) - 1] = '\0';
    
    g_last_config_update = hal_get_timestamp_ms();
    
    response->success = true;
    strcpy(response->message, "Parameter updated successfully");
    response->requires_restart = parameter->requires_restart;
    strcpy(response->validation_errors, "");
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Config: Parameter '%s' updated to '%s'", request->key, request->value);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate configuration parameter
 * @param key Configuration key
 * @param value Configuration value
 * @return hal_status_t HAL_STATUS_OK if valid, error code if invalid
 */
hal_status_t api_config_validate_parameter(const char *key, const char *value) {
    if (key == NULL || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_config_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Find parameter
    api_config_parameter_t *parameter = NULL;
    for (uint32_t i = 0; i < g_config_parameter_count; i++) {
        if (strcmp(g_config_parameters[i].key, key) == 0) {
            parameter = &g_config_parameters[i];
            break;
        }
    }
    
    if (parameter == NULL) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    // Validate based on type
    switch (parameter->type) {
        case API_CONFIG_TYPE_STRING:
            // String validation - check length
            if (strlen(value) > 255) {
                return HAL_STATUS_INVALID_PARAMETER;
            }
            break;
            
        case API_CONFIG_TYPE_INTEGER: {
            // Integer validation
            char *endptr;
            long int_val = strtol(value, &endptr, 10);
            if (*endptr != '\0') {
                return HAL_STATUS_INVALID_PARAMETER;
            }
            
            // Check min/max values
            if (strlen(parameter->min_value) > 0) {
                long min_val = strtol(parameter->min_value, &endptr, 10);
                if (int_val < min_val) {
                    return HAL_STATUS_INVALID_PARAMETER;
                }
            }
            if (strlen(parameter->max_value) > 0) {
                long max_val = strtol(parameter->max_value, &endptr, 10);
                if (int_val > max_val) {
                    return HAL_STATUS_INVALID_PARAMETER;
                }
            }
            break;
        }
        
        case API_CONFIG_TYPE_FLOAT: {
            // Float validation
            char *endptr;
            double float_val = strtod(value, &endptr);
            if (*endptr != '\0') {
                return HAL_STATUS_INVALID_PARAMETER;
            }
            
            // Check min/max values
            if (strlen(parameter->min_value) > 0) {
                double min_val = strtod(parameter->min_value, &endptr);
                if (float_val < min_val) {
                    return HAL_STATUS_INVALID_PARAMETER;
                }
            }
            if (strlen(parameter->max_value) > 0) {
                double max_val = strtod(parameter->max_value, &endptr);
                if (float_val > max_val) {
                    return HAL_STATUS_INVALID_PARAMETER;
                }
            }
            break;
        }
        
        case API_CONFIG_TYPE_BOOLEAN:
            // Boolean validation
            if (strcmp(value, "true") != 0 && strcmp(value, "false") != 0) {
                return HAL_STATUS_INVALID_PARAMETER;
            }
            break;
            
        case API_CONFIG_TYPE_JSON:
            // JSON validation - basic check
            if (strlen(value) == 0 || (value[0] != '{' && value[0] != '[')) {
                return HAL_STATUS_INVALID_PARAMETER;
            }
            break;
            
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Reset configuration to defaults
 * @param category Configuration category (or API_CONFIG_CATEGORY_MAX for all)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_reset_defaults(api_config_category_t category) {
    if (!g_api_config_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint32_t reset_count = 0;
    
    for (uint32_t i = 0; i < g_config_parameter_count; i++) {
        if (category == API_CONFIG_CATEGORY_MAX || g_config_parameters[i].category == category) {
            if (!g_config_parameters[i].read_only) {
                strncpy(g_config_parameters[i].current_value, g_config_parameters[i].default_value, 
                       sizeof(g_config_parameters[i].current_value) - 1);
                g_config_parameters[i].current_value[sizeof(g_config_parameters[i].current_value) - 1] = '\0';
                reset_count++;
            }
        }
    }
    
    g_last_config_update = hal_get_timestamp_ms();
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "API Config: Reset %u parameters to defaults", reset_count);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get configuration categories
 * @param categories Array of category info structures
 * @param max_categories Maximum number of categories
 * @param actual_count Actual number of categories returned
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_categories(api_config_category_info_t *categories, uint32_t max_categories, uint32_t *actual_count) {
    if (categories == NULL || actual_count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_api_config_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *actual_count = 0;
    
    // Count parameters per category
    uint32_t category_counts[API_CONFIG_CATEGORY_MAX] = {0};
    for (uint32_t i = 0; i < g_config_parameter_count; i++) {
        if (g_config_parameters[i].category < API_CONFIG_CATEGORY_MAX) {
            category_counts[g_config_parameters[i].category]++;
        }
    }
    
    // Create category info
    for (uint32_t cat = 0; cat < API_CONFIG_CATEGORY_MAX && *actual_count < max_categories; cat++) {
        if (category_counts[cat] > 0) {
            categories[*actual_count].category = (api_config_category_t)cat;
            strcpy(categories[*actual_count].name, api_config_get_category_name((api_config_category_t)cat));
            strcpy(categories[*actual_count].description, api_config_get_category_description((api_config_category_t)cat));
            categories[*actual_count].parameter_count = category_counts[cat];
            
            // Copy parameters for this category
            uint32_t param_idx = 0;
            for (uint32_t i = 0; i < g_config_parameter_count && param_idx < 32; i++) {
                if (g_config_parameters[i].category == cat) {
                    categories[*actual_count].parameters[param_idx] = g_config_parameters[i];
                    param_idx++;
                }
            }
            
            (*actual_count)++;
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get configuration status
 * @param initialized Pointer to initialized flag
 * @param parameter_count Pointer to parameter count
 * @param last_update Pointer to last update timestamp
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_status(bool *initialized, uint32_t *parameter_count, uint64_t *last_update) {
    if (initialized == NULL || parameter_count == NULL || last_update == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *initialized = g_api_config_initialized;
    *parameter_count = g_config_parameter_count;
    *last_update = g_last_config_update;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get configuration version
 * @param major Major version number
 * @param minor Minor version number
 * @param patch Patch version number
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_config_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch) {
    if (major == NULL || minor == NULL || patch == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *major = API_CONFIG_VERSION_MAJOR;
    *minor = API_CONFIG_VERSION_MINOR;
    *patch = API_CONFIG_VERSION_PATCH;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get configuration version string
 * @return const char* Version string
 */
const char* api_config_get_version_string(void) {
    return API_CONFIG_VERSION_STRING;
}

/**
 * @brief Get configuration category description
 * @param category Configuration category
 * @return const char* Category description
 */
const char* api_config_get_category_description(api_config_category_t category) {
    switch (category) {
        case API_CONFIG_CATEGORY_SYSTEM: return "System configuration parameters";
        case API_CONFIG_CATEGORY_SAFETY: return "Safety system configuration";
        case API_CONFIG_CATEGORY_MOTOR: return "Motor control configuration";
        case API_CONFIG_CATEGORY_POWER: return "Power management configuration";
        case API_CONFIG_CATEGORY_DOCK: return "Docking system configuration";
        case API_CONFIG_CATEGORY_NETWORK: return "Network configuration";
        default: return "Unknown category";
    }
}

/**
 * @brief Get configuration category name
 * @param category Configuration category
 * @return const char* Category name
 */
const char* api_config_get_category_name(api_config_category_t category) {
    switch (category) {
        case API_CONFIG_CATEGORY_SYSTEM: return "system";
        case API_CONFIG_CATEGORY_SAFETY: return "safety";
        case API_CONFIG_CATEGORY_MOTOR: return "motor";
        case API_CONFIG_CATEGORY_POWER: return "power";
        case API_CONFIG_CATEGORY_DOCK: return "dock";
        case API_CONFIG_CATEGORY_NETWORK: return "network";
        default: return "unknown";
    }
}

/**
 * @brief Get configuration type name
 * @param type Configuration type
 * @return const char* Type name
 */
const char* api_config_get_type_name(api_config_type_t type) {
    switch (type) {
        case API_CONFIG_TYPE_STRING: return "string";
        case API_CONFIG_TYPE_INTEGER: return "integer";
        case API_CONFIG_TYPE_FLOAT: return "float";
        case API_CONFIG_TYPE_BOOLEAN: return "boolean";
        case API_CONFIG_TYPE_JSON: return "json";
        default: return "unknown";
    }
}

// Placeholder functions for future implementation
hal_status_t api_config_create_backup(api_config_backup_t *backup) {
    (void)backup; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_config_restore_backup(const api_config_backup_t *backup) {
    (void)backup; // Unused parameter
    return HAL_STATUS_OK;
}

hal_status_t api_config_apply_changes(bool *requires_restart) {
    if (requires_restart != NULL) {
        *requires_restart = false;
    }
    return HAL_STATUS_OK;
}
