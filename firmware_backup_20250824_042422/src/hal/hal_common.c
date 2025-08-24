/**
 * @file hal_common.c
 * @brief Common HAL utility functions implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 */

#include "hal_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>

// Global variables
static hal_error_callback_t g_error_callback = NULL;
static void *g_error_context = NULL;
static hal_error_info_t g_last_error = {0};
static hal_statistics_t g_statistics = {0};
static hal_config_t g_config = {0};

// HAL utility functions
const char* hal_status_to_string(hal_status_t status) {
    switch (status) {
        case HAL_STATUS_OK: return "OK";
        case HAL_STATUS_ERROR: return "ERROR";
        case HAL_STATUS_INVALID_PARAMETER: return "INVALID_PARAMETER";
        case HAL_STATUS_NOT_INITIALIZED: return "NOT_INITIALIZED";
        case HAL_STATUS_TIMEOUT: return "TIMEOUT";
        case HAL_STATUS_BUSY: return "BUSY";
        case HAL_STATUS_NOT_SUPPORTED: return "NOT_SUPPORTED";
        default: return "UNKNOWN";
    }
}

const char* hal_device_status_to_string(hal_device_status_t status) {
    switch (status) {
        case HAL_DEVICE_STATUS_UNKNOWN: return "UNKNOWN";
        case HAL_DEVICE_STATUS_OK: return "OK";
        case HAL_DEVICE_STATUS_WARNING: return "WARNING";
        case HAL_DEVICE_STATUS_ERROR: return "ERROR";
        case HAL_DEVICE_STATUS_FAULT: return "FAULT";
        case HAL_DEVICE_STATUS_OFFLINE: return "OFFLINE";
        case HAL_DEVICE_STATUS_CALIBRATING: return "CALIBRATING";
        case HAL_DEVICE_STATUS_INITIALIZING: return "INITIALIZING";
        default: return "UNKNOWN";
    }
}

const char* hal_device_type_to_string(hal_device_type_t device_type) {
    switch (device_type) {
        case HAL_DEVICE_TYPE_UNKNOWN: return "UNKNOWN";
        case HAL_DEVICE_TYPE_LIDAR: return "LIDAR";
        case HAL_DEVICE_TYPE_ENCODER: return "ENCODER";
        case HAL_DEVICE_TYPE_MOTOR: return "MOTOR";
        case HAL_DEVICE_TYPE_RELAY: return "RELAY";
        case HAL_DEVICE_TYPE_LIMIT_SWITCH: return "LIMIT_SWITCH";
        case HAL_DEVICE_TYPE_ESTOP: return "ESTOP";
        case HAL_DEVICE_TYPE_UART: return "UART";
        case HAL_DEVICE_TYPE_GPIO: return "GPIO";
        case HAL_DEVICE_TYPE_CAN: return "CAN";
        case HAL_DEVICE_TYPE_USB: return "USB";
        default: return "UNKNOWN";
    }
}

// HAL timestamp functions
uint64_t hal_get_timestamp_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

uint64_t hal_get_timestamp_ms(void) {
    return hal_get_timestamp_us() / 1000ULL;
}

void hal_sleep_us(uint64_t microseconds) {
    usleep((unsigned int)microseconds);
}

void hal_sleep_ms(uint32_t milliseconds) {
    usleep((unsigned int)milliseconds * 1000);
}

// HAL error handling functions
hal_status_t hal_set_error_callback(hal_error_callback_t callback, void *context) {
    g_error_callback = callback;
    g_error_context = context;
    return HAL_STATUS_OK;
}

hal_status_t hal_get_last_error(hal_error_info_t *error_info) {
    if (error_info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(error_info, &g_last_error, sizeof(hal_error_info_t));
    return HAL_STATUS_OK;
}

hal_status_t hal_clear_errors(void) {
    memset(&g_last_error, 0, sizeof(hal_error_info_t));
    return HAL_STATUS_OK;
}

// Internal error handling function
void hal_set_error(hal_status_t error_code, const char *error_message) {
    uint64_t current_time = hal_get_timestamp_us();
    
    g_last_error.error_code = error_code;
    g_last_error.error_count++;
    
    if (g_last_error.first_error_time_us == 0) {
        g_last_error.first_error_time_us = current_time;
    }
    g_last_error.last_error_time_us = current_time;
    
    if (error_message != NULL) {
        strncpy(g_last_error.error_message, error_message, sizeof(g_last_error.error_message) - 1);
        g_last_error.error_message[sizeof(g_last_error.error_message) - 1] = '\0';
    }
    
    // Call error callback if set
    if (g_error_callback != NULL) {
        g_error_callback(error_code, g_error_context);
    }
}

// HAL statistics functions
hal_status_t hal_get_statistics(hal_statistics_t *statistics) {
    if (statistics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(statistics, &g_statistics, sizeof(hal_statistics_t));
    return HAL_STATUS_OK;
}

hal_status_t hal_reset_statistics(void) {
    memset(&g_statistics, 0, sizeof(hal_statistics_t));
    g_statistics.timestamp_us = hal_get_timestamp_us();
    return HAL_STATUS_OK;
}

// Internal statistics update function
void hal_update_statistics(uint64_t operation_time_us, bool success) {
    uint64_t current_time = hal_get_timestamp_us();
    
    g_statistics.total_operations++;
    g_statistics.total_time_us += operation_time_us;
    g_statistics.last_operation_time_us = operation_time_us;
    
    if (success) {
        g_statistics.successful_operations++;
    } else {
        g_statistics.failed_operations++;
    }
    
    // Update min/max times
    if (g_statistics.min_time_us == 0 || operation_time_us < g_statistics.min_time_us) {
        g_statistics.min_time_us = operation_time_us;
    }
    if (operation_time_us > g_statistics.max_time_us) {
        g_statistics.max_time_us = operation_time_us;
    }
    
    // Update average time
    g_statistics.avg_time_us = g_statistics.total_time_us / g_statistics.total_operations;
    g_statistics.timestamp_us = current_time;
}

// HAL configuration functions
hal_status_t hal_load_configuration(const char *config_file) {
    if (config_file == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // TODO: Implement configuration file loading
    // For now, use default configuration
    g_config.config_id = 1;
    g_config.version = 1;
    g_config.timestamp_us = hal_get_timestamp_us();
    g_config.enabled = true;
    g_config.timeout_ms = HAL_TIMEOUT_MS;
    g_config.retry_count = 3;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_save_configuration(const char *config_file) {
    if (config_file == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // TODO: Implement configuration file saving
    return HAL_STATUS_OK;
}

hal_status_t hal_get_configuration(hal_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(config, &g_config, sizeof(hal_config_t));
    return HAL_STATUS_OK;
}

hal_status_t hal_set_configuration(const hal_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(&g_config, config, sizeof(hal_config_t));
    g_config.timestamp_us = hal_get_timestamp_us();
    return HAL_STATUS_OK;
}

// HAL device management functions
hal_status_t hal_register_device(hal_device_type_t device_type, const char *device_name) {
    (void)device_type; // Unused parameter
    (void)device_name; // Unused parameter
    if (device_name == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // TODO: Implement device registration
    return HAL_STATUS_OK;
}

hal_status_t hal_unregister_device(uint32_t device_id) {
    (void)device_id; // Unused parameter
    // TODO: Implement device unregistration
    return HAL_STATUS_OK;
}

hal_status_t hal_get_device_info(uint32_t device_id, hal_device_info_t *device_info) {
    (void)device_id; // Unused parameter
    if (device_info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // TODO: Implement device info retrieval
    return HAL_STATUS_OK;
}

hal_status_t hal_get_device_list(hal_device_info_t *device_list, uint32_t *device_count) {
    if (device_list == NULL || device_count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // TODO: Implement device list retrieval
    *device_count = 0;
    return HAL_STATUS_OK;
}

// HAL safety functions
hal_status_t hal_safety_check(void) {
    // TODO: Implement safety check
    return HAL_STATUS_OK;
}

hal_status_t hal_safety_enable(void) {
    // TODO: Implement safety enable
    return HAL_STATUS_OK;
}

hal_status_t hal_safety_disable(void) {
    // TODO: Implement safety disable
    return HAL_STATUS_OK;
}

bool hal_safety_is_enabled(void) {
    // TODO: Implement safety status check
    return true;
}

// HAL logging functions
static hal_log_level_t g_log_level = HAL_LOG_LEVEL_INFO;
static FILE *g_log_file = NULL;

hal_status_t hal_log_init(const char *log_file) {
    if (log_file == NULL) {
        g_log_file = stderr;
    } else {
        g_log_file = fopen(log_file, "a");
        if (g_log_file == NULL) {
            return HAL_STATUS_ERROR;
        }
    }
    return HAL_STATUS_OK;
}

hal_status_t hal_log_set_level(hal_log_level_t level) {
    g_log_level = level;
    return HAL_STATUS_OK;
}

hal_status_t hal_log_message(hal_log_level_t level, const char *format, ...) {
    if (level < g_log_level || format == NULL) {
        return HAL_STATUS_OK;
    }
    
    if (g_log_file == NULL) {
        g_log_file = stderr;
    }
    
    // Get timestamp
    uint64_t timestamp = hal_get_timestamp_ms();
    
    // Get log level string
    const char *level_str;
    switch (level) {
        case HAL_LOG_LEVEL_DEBUG: level_str = "DEBUG"; break;
        case HAL_LOG_LEVEL_INFO: level_str = "INFO"; break;
        case HAL_LOG_LEVEL_WARNING: level_str = "WARNING"; break;
        case HAL_LOG_LEVEL_ERROR: level_str = "ERROR"; break;
        case HAL_LOG_LEVEL_FATAL: level_str = "FATAL"; break;
        default: level_str = "UNKNOWN"; break;
    }
    
    // Print timestamp and level
    fprintf(g_log_file, "[%lu] [%s] ", (unsigned long)timestamp, level_str);
    
    // Print message
    va_list args;
    va_start(args, format);
    vfprintf(g_log_file, format, args);
    va_end(args);
    
    fprintf(g_log_file, "\n");
    fflush(g_log_file);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_log_close(void) {
    if (g_log_file != NULL && g_log_file != stderr) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    return HAL_STATUS_OK;
}

// HAL version information
hal_status_t hal_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch) {
    if (major == NULL || minor == NULL || patch == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *major = HAL_VERSION_MAJOR;
    *minor = HAL_VERSION_MINOR;
    *patch = HAL_VERSION_PATCH;
    
    return HAL_STATUS_OK;
}

const char* hal_get_version_string(void) {
    return HAL_VERSION_STRING;
}
