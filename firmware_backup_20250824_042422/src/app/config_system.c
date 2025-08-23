/**
 * @file config_system.c
 * @brief Configuration System Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include "config_system.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

// Internal configuration state
static struct {
    master_config_t config;
    char config_file_path[256];
    bool initialized;
    void (*change_callback)(config_section_t section);
} g_config_system = {0};

// Default configuration values
static const master_config_t DEFAULT_CONFIG = {
    .system = {
        .device_name = "OHT-50-Master",
        .device_version = "1.0.0",
        .startup_timeout_ms = 120000,  // 120 seconds
        .watchdog_timeout_ms = 5000,   // 5 seconds
        .debug_mode = false,
        .log_level = 2
    },
    .safety = {
        .estop_timeout_ms = 100,       // 100ms
        .safety_check_interval_ms = 1000, // 1 second
        .fault_clear_timeout_ms = 5000,   // 5 seconds
        .dual_channel_estop = true,
        .emergency_stop_delay_ms = 50
    },
    .communication = {
        .rs485_baud_rate = 115200,
        .rs485_timeout_ms = 1000,
        .rs485_retry_count = 3,
        .rs485_retry_delay_ms = 100,
        .rs485_auto_discovery = true,
        .discovery_interval_ms = 30000  // 30 seconds
    },
    .modules = {
        .power_module_address = 0x02,
        .motor_module_address = 0x03,
        .io_module_address = 0x04,
        .module_timeout_ms = 5000,
        .module_heartbeat_interval_ms = 1000
    },
    .performance = {
        .control_loop_frequency_hz = 100,
        .telemetry_interval_ms = 100,
        .diagnostics_interval_ms = 1000,
        .performance_monitoring = true,
        .metrics_update_interval_ms = 5000
    },
    .last_modified_time = 0,
    .config_version = 1
};

// Internal function prototypes
static hal_status_t load_config_from_file(void);
static hal_status_t save_config_to_file(void);
static hal_status_t validate_system_config(const system_config_t *config);
static hal_status_t validate_safety_config(const safety_config_t *config);
static hal_status_t validate_communication_config(const communication_config_t *config);
static hal_status_t validate_module_config(const module_config_t *config);
static hal_status_t validate_performance_config(const performance_config_t *config);

/**
 * @brief Initialize configuration system
 * @param config_file_path Path to configuration file
 * @return HAL status
 */
hal_status_t config_system_init(const char *config_file_path)
{
    if (g_config_system.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    if (!config_file_path) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize structure
    memset(&g_config_system, 0, sizeof(g_config_system));
    
    // Copy configuration file path
    strncpy(g_config_system.config_file_path, config_file_path, sizeof(g_config_system.config_file_path) - 1);
    
    // Set default configuration
    memcpy(&g_config_system.config, &DEFAULT_CONFIG, sizeof(master_config_t));
    
    // Try to load existing configuration
    hal_status_t load_status = load_config_from_file();
    if (load_status != HAL_STATUS_OK) {
        printf("[CONFIG] Using default configuration (load failed: %d)\n", load_status);
        // Save default configuration
        save_config_to_file();
    }
    
    g_config_system.initialized = true;
    printf("[CONFIG] Configuration system initialized: %s\n", config_file_path);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize configuration system
 * @return HAL status
 */
hal_status_t config_system_deinit(void)
{
    if (!g_config_system.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Save configuration before deinitializing
    save_config_to_file();
    
    g_config_system.initialized = false;
    printf("[CONFIG] Configuration system deinitialized\n");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Load configuration from file
 * @return HAL status
 */
hal_status_t config_system_load(void)
{
    if (!g_config_system.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    return load_config_from_file();
}

/**
 * @brief Save configuration to file
 * @return HAL status
 */
hal_status_t config_system_save(void)
{
    if (!g_config_system.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    return save_config_to_file();
}

/**
 * @brief Reset configuration to defaults
 * @return HAL status
 */
hal_status_t config_system_reset_to_defaults(void)
{
    if (!g_config_system.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memcpy(&g_config_system.config, &DEFAULT_CONFIG, sizeof(master_config_t));
    g_config_system.config.last_modified_time = hal_get_timestamp_us();
    
    // Save to file
    hal_status_t status = save_config_to_file();
    if (status == HAL_STATUS_OK) {
        printf("[CONFIG] Configuration reset to defaults\n");
    }
    
    return status;
}

/**
 * @brief Get master configuration
 * @param config Pointer to configuration structure
 * @return HAL status
 */
hal_status_t config_system_get_master_config(master_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_config_system.config, sizeof(master_config_t));
    return HAL_STATUS_OK;
}

/**
 * @brief Set master configuration
 * @param config Pointer to configuration structure
 * @return HAL status
 */
hal_status_t config_system_set_master_config(const master_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    config_validation_t validation = config_system_validate_config(config);
    if (!validation.valid) {
        printf("[CONFIG] Configuration validation failed: %s\n", validation.error_message);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_config_system.config, config, sizeof(master_config_t));
    g_config_system.config.last_modified_time = hal_get_timestamp_us();
    
    // Save to file
    hal_status_t status = save_config_to_file();
    if (status == HAL_STATUS_OK && g_config_system.change_callback) {
        g_config_system.change_callback(CONFIG_SECTION_SYSTEM);
    }
    
    return status;
}

/**
 * @brief Get system configuration
 * @param config Pointer to system configuration structure
 * @return HAL status
 */
hal_status_t config_system_get_system_config(system_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_config_system.config.system, sizeof(system_config_t));
    return HAL_STATUS_OK;
}

/**
 * @brief Set system configuration
 * @param config Pointer to system configuration structure
 * @return HAL status
 */
hal_status_t config_system_set_system_config(const system_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    if (validate_system_config(config) != HAL_STATUS_OK) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_config_system.config.system, config, sizeof(system_config_t));
    g_config_system.config.last_modified_time = hal_get_timestamp_us();
    
    // Save to file
    hal_status_t status = save_config_to_file();
    if (status == HAL_STATUS_OK && g_config_system.change_callback) {
        g_config_system.change_callback(CONFIG_SECTION_SYSTEM);
    }
    
    return status;
}

/**
 * @brief Get safety configuration
 * @param config Pointer to safety configuration structure
 * @return HAL status
 */
hal_status_t config_system_get_safety_config(safety_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_config_system.config.safety, sizeof(safety_config_t));
    return HAL_STATUS_OK;
}

/**
 * @brief Set safety configuration
 * @param config Pointer to safety configuration structure
 * @return HAL status
 */
hal_status_t config_system_set_safety_config(const safety_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    if (validate_safety_config(config) != HAL_STATUS_OK) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_config_system.config.safety, config, sizeof(safety_config_t));
    g_config_system.config.last_modified_time = hal_get_timestamp_us();
    
    // Save to file
    hal_status_t status = save_config_to_file();
    if (status == HAL_STATUS_OK && g_config_system.change_callback) {
        g_config_system.change_callback(CONFIG_SECTION_SAFETY);
    }
    
    return status;
}

/**
 * @brief Get communication configuration
 * @param config Pointer to communication configuration structure
 * @return HAL status
 */
hal_status_t config_system_get_communication_config(communication_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_config_system.config.communication, sizeof(communication_config_t));
    return HAL_STATUS_OK;
}

/**
 * @brief Set communication configuration
 * @param config Pointer to communication configuration structure
 * @return HAL status
 */
hal_status_t config_system_set_communication_config(const communication_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    if (validate_communication_config(config) != HAL_STATUS_OK) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_config_system.config.communication, config, sizeof(communication_config_t));
    g_config_system.config.last_modified_time = hal_get_timestamp_us();
    
    // Save to file
    hal_status_t status = save_config_to_file();
    if (status == HAL_STATUS_OK && g_config_system.change_callback) {
        g_config_system.change_callback(CONFIG_SECTION_COMMUNICATION);
    }
    
    return status;
}

/**
 * @brief Get module configuration
 * @param config Pointer to module configuration structure
 * @return HAL status
 */
hal_status_t config_system_get_module_config(module_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_config_system.config.modules, sizeof(module_config_t));
    return HAL_STATUS_OK;
}

/**
 * @brief Set module configuration
 * @param config Pointer to module configuration structure
 * @return HAL status
 */
hal_status_t config_system_set_module_config(const module_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    if (validate_module_config(config) != HAL_STATUS_OK) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_config_system.config.modules, config, sizeof(module_config_t));
    g_config_system.config.last_modified_time = hal_get_timestamp_us();
    
    // Save to file
    hal_status_t status = save_config_to_file();
    if (status == HAL_STATUS_OK && g_config_system.change_callback) {
        g_config_system.change_callback(CONFIG_SECTION_MODULES);
    }
    
    return status;
}

/**
 * @brief Get performance configuration
 * @param config Pointer to performance configuration structure
 * @return HAL status
 */
hal_status_t config_system_get_performance_config(performance_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_config_system.config.performance, sizeof(performance_config_t));
    return HAL_STATUS_OK;
}

/**
 * @brief Set performance configuration
 * @param config Pointer to performance configuration structure
 * @return HAL status
 */
hal_status_t config_system_set_performance_config(const performance_config_t *config)
{
    if (!g_config_system.initialized || !config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    if (validate_performance_config(config) != HAL_STATUS_OK) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_config_system.config.performance, config, sizeof(performance_config_t));
    g_config_system.config.last_modified_time = hal_get_timestamp_us();
    
    // Save to file
    hal_status_t status = save_config_to_file();
    if (status == HAL_STATUS_OK && g_config_system.change_callback) {
        g_config_system.change_callback(CONFIG_SECTION_PERFORMANCE);
    }
    
    return status;
}

/**
 * @brief Validate configuration
 * @param config Pointer to configuration structure
 * @return Validation result
 */
config_validation_t config_system_validate_config(const master_config_t *config)
{
    config_validation_t result = {0};
    result.valid = true;
    
    if (!config) {
        result.valid = false;
        strcpy(result.error_message, "Configuration is NULL");
        return result;
    }
    
    // Validate each section
    if (validate_system_config(&config->system) != HAL_STATUS_OK) {
        result.valid = false;
        result.failed_section = CONFIG_SECTION_SYSTEM;
        strcpy(result.error_message, "System configuration validation failed");
        return result;
    }
    
    if (validate_safety_config(&config->safety) != HAL_STATUS_OK) {
        result.valid = false;
        result.failed_section = CONFIG_SECTION_SAFETY;
        strcpy(result.error_message, "Safety configuration validation failed");
        return result;
    }
    
    if (validate_communication_config(&config->communication) != HAL_STATUS_OK) {
        result.valid = false;
        result.failed_section = CONFIG_SECTION_COMMUNICATION;
        strcpy(result.error_message, "Communication configuration validation failed");
        return result;
    }
    
    if (validate_module_config(&config->modules) != HAL_STATUS_OK) {
        result.valid = false;
        result.failed_section = CONFIG_SECTION_MODULES;
        strcpy(result.error_message, "Module configuration validation failed");
        return result;
    }
    
    if (validate_performance_config(&config->performance) != HAL_STATUS_OK) {
        result.valid = false;
        result.failed_section = CONFIG_SECTION_PERFORMANCE;
        strcpy(result.error_message, "Performance configuration validation failed");
        return result;
    }
    
    return result;
}

/**
 * @brief Register configuration change callback
 * @param callback Callback function
 * @return HAL status
 */
hal_status_t config_system_register_change_callback(void (*callback)(config_section_t section))
{
    if (!g_config_system.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    g_config_system.change_callback = callback;
    return HAL_STATUS_OK;
}

/**
 * @brief Get last modified time
 * @param timestamp Pointer to timestamp
 * @return HAL status
 */
hal_status_t config_system_get_last_modified_time(uint64_t *timestamp)
{
    if (!g_config_system.initialized || !timestamp) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *timestamp = g_config_system.config.last_modified_time;
    return HAL_STATUS_OK;
}

// Internal helper functions

/**
 * @brief Load configuration from file
 * @return HAL status
 */
static hal_status_t load_config_from_file(void)
{
    FILE *file = fopen(g_config_system.config_file_path, "rb");
    if (!file) {
        return HAL_STATUS_NOT_FOUND;
    }
    
    size_t read_size = fread(&g_config_system.config, 1, sizeof(master_config_t), file);
    fclose(file);
    
    if (read_size != sizeof(master_config_t)) {
        return HAL_STATUS_ERROR;
    }
    
    printf("[CONFIG] Configuration loaded from: %s\n", g_config_system.config_file_path);
    return HAL_STATUS_OK;
}

/**
 * @brief Save configuration to file
 * @return HAL status
 */
static hal_status_t save_config_to_file(void)
{
    FILE *file = fopen(g_config_system.config_file_path, "wb");
    if (!file) {
        return HAL_STATUS_IO_ERROR;
    }
    
    size_t write_size = fwrite(&g_config_system.config, 1, sizeof(master_config_t), file);
    fclose(file);
    
    if (write_size != sizeof(master_config_t)) {
        return HAL_STATUS_ERROR;
    }
    
    printf("[CONFIG] Configuration saved to: %s\n", g_config_system.config_file_path);
    return HAL_STATUS_OK;
}

/**
 * @brief Validate system configuration
 * @param config System configuration
 * @return HAL status
 */
static hal_status_t validate_system_config(const system_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->startup_timeout_ms < 1000 || config->startup_timeout_ms > 300000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->watchdog_timeout_ms < 1000 || config->watchdog_timeout_ms > 30000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->log_level > 5) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate safety configuration
 * @param config Safety configuration
 * @return HAL status
 */
static hal_status_t validate_safety_config(const safety_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->estop_timeout_ms < 10 || config->estop_timeout_ms > 1000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->safety_check_interval_ms < 100 || config->safety_check_interval_ms > 10000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->fault_clear_timeout_ms < 1000 || config->fault_clear_timeout_ms > 60000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->emergency_stop_delay_ms > 1000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate communication configuration
 * @param config Communication configuration
 * @return HAL status
 */
static hal_status_t validate_communication_config(const communication_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->rs485_baud_rate != 9600 && config->rs485_baud_rate != 19200 && 
        config->rs485_baud_rate != 38400 && config->rs485_baud_rate != 57600 && 
        config->rs485_baud_rate != 115200) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->rs485_timeout_ms < 100 || config->rs485_timeout_ms > 10000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->rs485_retry_count > 10) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->discovery_interval_ms < 5000 || config->discovery_interval_ms > 300000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate module configuration
 * @param config Module configuration
 * @return HAL status
 */
static hal_status_t validate_module_config(const module_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->power_module_address < 0x02 || config->power_module_address > 0x07) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->motor_module_address < 0x02 || config->motor_module_address > 0x07) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->io_module_address < 0x02 || config->io_module_address > 0x07) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->module_timeout_ms < 1000 || config->module_timeout_ms > 30000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->module_heartbeat_interval_ms < 100 || config->module_heartbeat_interval_ms > 10000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate performance configuration
 * @param config Performance configuration
 * @return HAL status
 */
static hal_status_t validate_performance_config(const performance_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->control_loop_frequency_hz < 10 || config->control_loop_frequency_hz > 1000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->telemetry_interval_ms < 10 || config->telemetry_interval_ms > 10000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->diagnostics_interval_ms < 100 || config->diagnostics_interval_ms > 60000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->metrics_update_interval_ms < 1000 || config->metrics_update_interval_ms > 60000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}
