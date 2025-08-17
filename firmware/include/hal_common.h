/**
 * @file hal_common.h
 * @brief Common HAL definitions and types
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-12 (LiDAR Driver & USB Integration)
 */

#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Common status codes
typedef enum {
    HAL_STATUS_OK = 0,
    HAL_STATUS_ERROR = -1,
    HAL_STATUS_INVALID_PARAMETER = -2,
    HAL_STATUS_NOT_INITIALIZED = -3,
    HAL_STATUS_TIMEOUT = -4,
    HAL_STATUS_BUSY = -5,
    HAL_STATUS_NOT_SUPPORTED = -6
} hal_status_t;

// GPIO Pin Definitions for Orange Pi 5B
// Available pins: 54, 35, 28, 29, 58, 59, 131, 132

// LED Status Indicators (5 LEDs)
#define LED_POWER_PIN          54  // GPIO1_D6 - Power LED (Green)
#define LED_SYSTEM_PIN         35  // GPIO1_A3 - System LED (Blue)
#define LED_COMM_PIN           28  // GPIO0_D4 - Communication LED (Yellow)
#define LED_NETWORK_PIN        29  // GPIO0_D5 - Network LED (Green)
#define LED_ERROR_PIN          58  // GPIO1_D2 - Error LED (Red)

// E-Stop Safety System (Dual-channel)
#define ESTOP_CHANNEL1_PIN     59  // GPIO1_D3 - E-Stop Channel 1
#define ESTOP_CHANNEL2_PIN     131 // GPIO4_A3 - E-Stop Channel 2

// Relay Output
#define RELAY_OUTPUT_PIN       132 // GPIO4_A4 - Relay Output (24V DC, 2A max)

// Network Interfaces
#define NETWORK_ETH_INTERFACE  "eth0"
#define NETWORK_WIFI_INTERFACE "wlan0"

// LiDAR Interface
#define LIDAR_DEVICE_PATH      "/dev/ttyUSB0"
#define LIDAR_BAUD_RATE        460800

// RS485 Interface
#define RS485_DEVICE_PATH      "/dev/ttyOHT485"
#define RS485_BAUD_RATE        115200

// Common timeouts
#define HAL_TIMEOUT_MS         5000
#define HAL_DEBOUNCE_MS        50

// Common buffer sizes
#define HAL_BUFFER_SIZE        1024
#define HAL_STRING_SIZE        256

// HAL device status
typedef enum {
    HAL_DEVICE_STATUS_UNKNOWN = 0,        // Unknown status
    HAL_DEVICE_STATUS_OK = 1,             // Device OK
    HAL_DEVICE_STATUS_WARNING = 2,        // Device warning
    HAL_DEVICE_STATUS_ERROR = 3,          // Device error
    HAL_DEVICE_STATUS_FAULT = 4,          // Device fault
    HAL_DEVICE_STATUS_OFFLINE = 5,        // Device offline
    HAL_DEVICE_STATUS_CALIBRATING = 6,    // Device calibrating
    HAL_DEVICE_STATUS_INITIALIZING = 7    // Device initializing
} hal_device_status_t;

// HAL device types
typedef enum {
    HAL_DEVICE_TYPE_UNKNOWN = 0,          // Unknown device
    HAL_DEVICE_TYPE_LIDAR = 1,            // LiDAR sensor
    HAL_DEVICE_TYPE_ENCODER = 2,          // Encoder
    HAL_DEVICE_TYPE_MOTOR = 3,            // Motor
    HAL_DEVICE_TYPE_RELAY = 4,            // Relay
    HAL_DEVICE_TYPE_LIMIT_SWITCH = 5,     // Limit switch
    HAL_DEVICE_TYPE_ESTOP = 6,            // Emergency stop
    HAL_DEVICE_TYPE_UART = 7,             // UART interface
    HAL_DEVICE_TYPE_GPIO = 8,             // GPIO interface
    HAL_DEVICE_TYPE_CAN = 9               // CAN interface
} hal_device_type_t;

// HAL device information structure
typedef struct {
    hal_device_type_t device_type;        // Device type
    hal_device_status_t status;           // Device status
    uint32_t device_id;                   // Device ID
    char device_name[64];                 // Device name
    char device_version[32];              // Device version
    uint64_t timestamp_us;                // Timestamp in microseconds
    uint32_t error_count;                 // Error count
    uint32_t warning_count;               // Warning count
} hal_device_info_t;

// HAL configuration structure
typedef struct {
    uint32_t config_id;                   // Configuration ID
    uint32_t version;                     // Configuration version
    uint64_t timestamp_us;                // Configuration timestamp
    bool enabled;                         // Device enabled
    uint32_t timeout_ms;                  // Operation timeout
    uint32_t retry_count;                 // Retry count
} hal_config_t;

// HAL statistics structure
typedef struct {
    uint64_t total_operations;            // Total operations
    uint64_t successful_operations;       // Successful operations
    uint64_t failed_operations;           // Failed operations
    uint64_t total_time_us;               // Total time in microseconds
    uint64_t min_time_us;                 // Minimum time in microseconds
    uint64_t max_time_us;                 // Maximum time in microseconds
    uint64_t avg_time_us;                 // Average time in microseconds
    uint64_t last_operation_time_us;      // Last operation time
    uint64_t timestamp_us;                // Statistics timestamp
} hal_statistics_t;

// HAL error information structure
typedef struct {
    hal_status_t error_code;              // Error code
    uint32_t error_count;                 // Error count
    uint64_t first_error_time_us;         // First error time
    uint64_t last_error_time_us;          // Last error time
    char error_message[256];              // Error message
} hal_error_info_t;

// HAL callback function types
typedef void (*hal_callback_t)(void *context);
typedef void (*hal_error_callback_t)(hal_status_t error_code, void *context);
typedef void (*hal_data_callback_t)(const void *data, size_t data_size, void *context);

// HAL utility functions
const char* hal_status_to_string(hal_status_t status);
const char* hal_device_status_to_string(hal_device_status_t status);
const char* hal_device_type_to_string(hal_device_type_t device_type);

// HAL timestamp functions
uint64_t hal_get_timestamp_us(void);
uint64_t hal_get_timestamp_ms(void);
void hal_sleep_us(uint64_t microseconds);
void hal_sleep_ms(uint32_t milliseconds);

// HAL error handling functions
hal_status_t hal_set_error_callback(hal_error_callback_t callback, void *context);
hal_status_t hal_get_last_error(hal_error_info_t *error_info);
hal_status_t hal_clear_errors(void);

// HAL statistics functions
hal_status_t hal_get_statistics(hal_statistics_t *statistics);
hal_status_t hal_reset_statistics(void);

// HAL configuration functions
hal_status_t hal_load_configuration(const char *config_file);
hal_status_t hal_save_configuration(const char *config_file);
hal_status_t hal_get_configuration(hal_config_t *config);
hal_status_t hal_set_configuration(const hal_config_t *config);

// HAL device management functions
hal_status_t hal_register_device(hal_device_type_t device_type, const char *device_name);
hal_status_t hal_unregister_device(uint32_t device_id);
hal_status_t hal_get_device_info(uint32_t device_id, hal_device_info_t *device_info);
hal_status_t hal_get_device_list(hal_device_info_t *device_list, uint32_t *device_count);

// HAL safety functions
hal_status_t hal_safety_check(void);
hal_status_t hal_safety_enable(void);
hal_status_t hal_safety_disable(void);
bool hal_safety_is_enabled(void);

// HAL logging functions
typedef enum {
    HAL_LOG_LEVEL_DEBUG = 0,
    HAL_LOG_LEVEL_INFO = 1,
    HAL_LOG_LEVEL_WARNING = 2,
    HAL_LOG_LEVEL_ERROR = 3,
    HAL_LOG_LEVEL_FATAL = 4
} hal_log_level_t;

hal_status_t hal_log_init(const char *log_file);
hal_status_t hal_log_set_level(hal_log_level_t level);
hal_status_t hal_log_message(hal_log_level_t level, const char *format, ...);
hal_status_t hal_log_close(void);

// HAL version information
#define HAL_VERSION_MAJOR 1
#define HAL_VERSION_MINOR 0
#define HAL_VERSION_PATCH 0
#define HAL_VERSION_STRING "1.0.0"

hal_status_t hal_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch);
const char* hal_get_version_string(void);

#endif // HAL_COMMON_H
