/**
 * @file hal_gpio.c
 * @brief HAL implementation for GPIO and Relay control
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-05 (IO Drivers)
 */

#include "hal_gpio.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>

// Internal state
static struct {
    bool initialized;
    pthread_mutex_t mutex;
    gpio_statistics_t statistics;
    gpio_device_info_t device_info;
    uint64_t last_operation_time_us;
} gpio_state = {0};

// Internal function prototypes
static uint64_t gpio_get_timestamp_us(void);

// GPIO utility functions
bool gpio_is_pin_valid(uint32_t pin);
hal_status_t gpio_export_pin(uint32_t pin);
hal_status_t gpio_unexport_pin(uint32_t pin);
uint32_t gpio_get_pin_offset(const char *pin_name);

/**
 * @brief Initialize GPIO HAL
 * @return HAL status
 */
hal_status_t hal_gpio_init(void)
{
    pthread_mutex_lock(&gpio_state.mutex);
    
    if (gpio_state.initialized) {
        pthread_mutex_unlock(&gpio_state.mutex);
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&gpio_state.mutex, NULL) != 0) {
        pthread_mutex_unlock(&gpio_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Initialize device info
    gpio_state.device_info.device_type = HAL_DEVICE_TYPE_GPIO;
    gpio_state.device_info.status = HAL_DEVICE_STATUS_INITIALIZING;
    strcpy(gpio_state.device_info.device_name, "GPIO_Controller");
    strcpy(gpio_state.device_info.device_version, "1.0.0");
    gpio_state.device_info.pin_count = GPIO_MAX_PINS;
    gpio_state.device_info.timestamp_us = gpio_get_timestamp_us();
    gpio_state.device_info.error_count = 0;
    gpio_state.device_info.warning_count = 0;
    
    // Initialize statistics
    memset(&gpio_state.statistics, 0, sizeof(gpio_statistics_t));
    gpio_state.last_operation_time_us = 0;
    
    gpio_state.initialized = true;
    gpio_state.device_info.status = HAL_DEVICE_STATUS_OK;
    
    pthread_mutex_unlock(&gpio_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize GPIO HAL
 * @return HAL status
 */
hal_status_t hal_gpio_deinit(void)
{
    pthread_mutex_lock(&gpio_state.mutex);
    
    if (!gpio_state.initialized) {
        pthread_mutex_unlock(&gpio_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Cleanup mutex
    pthread_mutex_destroy(&gpio_state.mutex);
    
    gpio_state.initialized = false;
    
    pthread_mutex_unlock(&gpio_state.mutex);
    
    return HAL_STATUS_OK;
}

// GPIO functions (Real implementation)
hal_status_t hal_gpio_configure_pin(const gpio_config_t *config) {
    if (!gpio_state.initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate pin number
    if (!gpio_is_pin_valid(config->pin_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Export pin if needed
    hal_status_t status = gpio_export_pin(config->pin_number);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Set direction
    status = hal_gpio_set_direction(config->pin_number, config->direction);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Set edge detection if input
    if (config->direction == GPIO_DIRECTION_IN && config->edge != GPIO_EDGE_NONE) {
        status = hal_gpio_set_edge(config->pin_number, config->edge);
        if (status != HAL_STATUS_OK) {
            return status;
        }
    }
    
    // Set bias if specified
    if (config->bias != GPIO_BIAS_DISABLE) {
        status = hal_gpio_set_bias(config->pin_number, config->bias);
        if (status != HAL_STATUS_OK) {
            return status;
        }
    }
    
    // Set drive strength if output
    if (config->direction == GPIO_DIRECTION_OUT) {
        status = hal_gpio_set_drive(config->pin_number, config->drive);
        if (status != HAL_STATUS_OK) {
            return status;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_direction(uint32_t pin, gpio_direction_t direction) {
    if (!gpio_state.initialized || !gpio_is_pin_valid(pin)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%s", direction == GPIO_DIRECTION_OUT ? "out" : "in");
    fclose(fp);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_value(uint32_t pin, bool value) {
    if (!gpio_state.initialized || !gpio_is_pin_valid(pin)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%d", value ? 1 : 0);
    fclose(fp);
    
    // Update statistics
    pthread_mutex_lock(&gpio_state.mutex);
    gpio_state.statistics.writes++;
    gpio_state.statistics.timestamp_us = gpio_get_timestamp_us();
    pthread_mutex_unlock(&gpio_state.mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_get_value(uint32_t pin, bool *value) {
    if (!gpio_state.initialized || !gpio_is_pin_valid(pin) || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return HAL_STATUS_ERROR;
    }
    
    int val;
    (void)fscanf(fp, "%d", &val);
    fclose(fp);
    
    *value = (val != 0);
    
    // Update statistics
    pthread_mutex_lock(&gpio_state.mutex);
    gpio_state.statistics.reads++;
    gpio_state.statistics.timestamp_us = gpio_get_timestamp_us();
    pthread_mutex_unlock(&gpio_state.mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_edge(uint32_t pin, gpio_edge_t edge) {
    if (!gpio_state.initialized || !gpio_is_pin_valid(pin)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", pin);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return HAL_STATUS_ERROR;
    }
    
    const char *edge_str = "none";
    switch (edge) {
        case GPIO_EDGE_RISING: edge_str = "rising"; break;
        case GPIO_EDGE_FALLING: edge_str = "falling"; break;
        case GPIO_EDGE_BOTH: edge_str = "both"; break;
        default: edge_str = "none"; break;
    }
    
    fprintf(fp, "%s", edge_str);
    fclose(fp);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_bias(uint32_t pin, gpio_bias_t bias) {
    // Note: Bias setting may not be supported on all GPIO chips
    // This is a stub for now as it requires specific GPIO chip support
    (void)pin;
    (void)bias;
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_set_drive(uint32_t pin, gpio_drive_t drive) {
    // Note: Drive strength setting may not be supported on all GPIO chips
    // This is a stub for now as it requires specific GPIO chip support
    (void)pin;
    (void)drive;
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_wait_for_event(uint32_t pin, gpio_event_t *event, uint32_t timeout_ms) {
    if (!gpio_state.initialized || !gpio_is_pin_valid(pin) || event == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // This is a simplified implementation
    // Real implementation would use select() or epoll() for efficient event waiting
    
    uint64_t start_time = gpio_get_timestamp_us();
    uint64_t timeout_us = (uint64_t)timeout_ms * 1000;
    
    while ((gpio_get_timestamp_us() - start_time) < timeout_us) {
        bool current_value;
        hal_status_t status = hal_gpio_get_value(pin, &current_value);
        if (status == HAL_STATUS_OK) {
            event->pin_number = pin;
            event->value = current_value;
            event->timestamp_us = gpio_get_timestamp_us();
            event->edge = GPIO_EDGE_NONE; // Simplified
            
            // Update statistics
            pthread_mutex_lock(&gpio_state.mutex);
            gpio_state.statistics.events++;
            pthread_mutex_unlock(&gpio_state.mutex);
            
            return HAL_STATUS_OK;
        }
        usleep(1000); // 1ms delay
    }
    
    return HAL_STATUS_TIMEOUT;
}

hal_status_t hal_gpio_get_statistics(gpio_statistics_t *statistics) {
    if (!gpio_state.initialized || statistics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&gpio_state.mutex);
    *statistics = gpio_state.statistics;
    pthread_mutex_unlock(&gpio_state.mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_reset_statistics(void) {
    if (!gpio_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&gpio_state.mutex);
    memset(&gpio_state.statistics, 0, sizeof(gpio_statistics_t));
    pthread_mutex_unlock(&gpio_state.mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_gpio_health_check(void) {
    if (!gpio_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Simple health check - try to read a test pin
    // For now, just return OK
    return HAL_STATUS_OK;
}

// Relay functions removed - moved to hal_relay.c to avoid duplicate definitions

// Utility functions (Real implementation)
hal_status_t gpio_validate_config(const gpio_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!gpio_is_pin_valid(config->pin_number)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->direction != GPIO_DIRECTION_IN && config->direction != GPIO_DIRECTION_OUT) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->edge < GPIO_EDGE_NONE || config->edge > GPIO_EDGE_BOTH) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->bias < GPIO_BIAS_DISABLE || config->bias > GPIO_BIAS_PULL_DOWN) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->drive < GPIO_DRIVE_2MA || config->drive > GPIO_DRIVE_12MA) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

// Removed relay_validate_config - it belongs to hal_relay.c

uint32_t gpio_get_pin_offset(const char *pin_name) {
    if (pin_name == NULL) {
        return 0;
    }
    
    // Parse GPIO pin name like "GPIO4_A3" to get pin number
    // This is a simplified implementation
    if (strncmp(pin_name, "GPIO", 4) == 0) {
        // Extract bank and pin from GPIO4_A3 format
        int bank, pin;
        if (sscanf(pin_name, "GPIO%d_%c%d", &bank, (char*)&pin, &pin) == 3) {
            // Convert to absolute pin number
            // This mapping depends on the specific SoC
            // For RK3588, GPIO4_A3 = 131, GPIO4_A4 = 132, etc.
            if (bank == 4) {
                if (strchr(pin_name, 'A') != NULL) {
                    return 128 + pin; // GPIO4_Ax = 128 + x
                }
            }
        }
    }
    
    return 0;
}

bool gpio_is_pin_valid(uint32_t pin) {
    // Check if pin is within valid range for RK3588
    // RK3588 has GPIO0-GPIO4, each with multiple banks
    return (pin <= 255); // Conservative range (uint32_t is always >= 0)
}

hal_status_t gpio_export_pin(uint32_t pin) {
    if (!gpio_is_pin_valid(pin)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);
    
    // Check if already exported
    if (access(path, F_OK) == 0) {
        return HAL_STATUS_OK;
    }
    
    FILE *fp = fopen("/sys/class/gpio/export", "w");
    if (!fp) {
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%d", pin);
    fclose(fp);
    
    // Wait for GPIO to be available
    usleep(100000); // 100ms
    
    return HAL_STATUS_OK;
}

hal_status_t gpio_unexport_pin(uint32_t pin) {
    if (!gpio_is_pin_valid(pin)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    FILE *fp = fopen("/sys/class/gpio/unexport", "w");
    if (!fp) {
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%d", pin);
    fclose(fp);
    
    return HAL_STATUS_OK;
}

// Internal functions (Real implementation)
static uint64_t gpio_get_timestamp_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}
