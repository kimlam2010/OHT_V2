/**
 * @file system_config.h
 * @brief System Configuration for OHT-50 Master Module
 * @version 2.0
 * @date 2025-01-27
 * @team FW
 * @task GPIO Pin Mapping Update
 */

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "hal_common.h"

// System Configuration Version
#define SYSTEM_GPIO_CONFIG_VERSION "2.0"

// GPIO Pin Definitions (Updated 2025-01-27)
#define LED_POWER_GPIO      54  // GPIO1_D6 - Power LED (Green)
#define LED_SYSTEM_GPIO     35  // GPIO1_A3 - System LED (Blue)
#define LED_COMM_GPIO       28  // GPIO0_D4 - Communication LED (Yellow)
#define LED_NETWORK_GPIO    29  // GPIO0_D5 - Network LED (Green)
#define LED_ERROR_GPIO      58  // GPIO1_D2 - Error LED (Red)

#define ESTOP_GPIO          59  // GPIO1_D3 - E-Stop (Single channel)
#define RELAY1_GPIO         131 // GPIO4_A3 - Relay 1 output
#define RELAY2_GPIO         132 // GPIO4_A4 - Relay 2 output

// System Parameters
#define SYSTEM_MAX_MODULES  16  // Maximum number of slave modules
#define SYSTEM_TIMEOUT_MS   5000 // System timeout in milliseconds
#define SYSTEM_UPDATE_RATE_MS 10 // System update rate in milliseconds

// Safety Parameters
#define SAFETY_LEVEL_SIL2   true // SIL2 safety level
#define SAFETY_RESPONSE_TIME_MS 100 // Safety response time
#define SAFETY_DEBOUNCE_TIME_MS 50 // Safety debounce time

// Communication Parameters
#define RS485_BAUDRATE      115200 // RS485 baudrate
#define RS485_TIMEOUT_MS    1000   // RS485 timeout
#define RS485_RETRY_COUNT   3      // RS485 retry count

// Network Parameters
#define NETWORK_TIMEOUT_MS  5000   // Network timeout
#define NETWORK_RETRY_COUNT 3      // Network retry count
#define WIFI_SCAN_INTERVAL_MS 30000 // WiFi scan interval

// LED Parameters
#define LED_BLINK_SLOW_MS   1000   // Slow blink period
#define LED_BLINK_FAST_MS   200    // Fast blink period
#define LED_PULSE_PERIOD_MS 500    // Pulse period

// Relay Parameters
#define RELAY_VOLTAGE_V      24     // Relay voltage
#define RELAY_CURRENT_MAX_A  2      // Maximum relay current
#define RELAY_PULSE_DURATION_MS 100 // Relay pulse duration

// System Status
typedef enum {
    SYSTEM_STATUS_OK = 0,
    SYSTEM_STATUS_WARNING,
    SYSTEM_STATUS_ERROR,
    SYSTEM_STATUS_FAULT,
    SYSTEM_STATUS_EMERGENCY
} system_status_t;

// System Configuration Structure
typedef struct {
    char version[16];
    uint8_t led_power_gpio;
    uint8_t led_system_gpio;
    uint8_t led_comm_gpio;
    uint8_t led_network_gpio;
    uint8_t led_error_gpio;
    uint8_t estop_gpio;
    uint8_t relay1_gpio;
    uint8_t relay2_gpio;
    uint32_t system_timeout_ms;
    uint32_t system_update_rate_ms;
    bool safety_level_sil2;
    uint32_t safety_response_time_ms;
    uint32_t safety_debounce_time_ms;
    uint32_t rs485_baudrate;
    uint32_t rs485_timeout_ms;
    uint32_t rs485_retry_count;
    uint32_t network_timeout_ms;
    uint32_t network_retry_count;
    uint32_t wifi_scan_interval_ms;
    uint32_t led_blink_slow_ms;
    uint32_t led_blink_fast_ms;
    uint32_t led_pulse_period_ms;
    uint32_t relay_voltage_v;
    uint32_t relay_current_max_a;
    uint32_t relay_pulse_duration_ms;
} system_config_t;

// Function Prototypes

/**
 * @brief Initialize system configuration
 * @param config System configuration
 * @return HAL status
 */
hal_status_t system_config_init(const system_config_t *config);

/**
 * @brief Get system configuration
 * @param config Pointer to store system configuration
 * @return HAL status
 */
hal_status_t system_config_get(system_config_t *config);

/**
 * @brief Set system configuration
 * @param config System configuration
 * @return HAL status
 */
hal_status_t system_config_set(const system_config_t *config);

/**
 * @brief Get system status
 * @param status Pointer to store system status
 * @return HAL status
 */
hal_status_t system_config_get_status(system_status_t *status);

/**
 * @brief Validate system configuration
 * @param config System configuration
 * @return true if valid, false otherwise
 */
bool system_config_validate(const system_config_t *config);

/**
 * @brief Get default system configuration
 * @param config Pointer to store default configuration
 * @return HAL status
 */
hal_status_t system_config_get_default(system_config_t *config);

#endif // SYSTEM_CONFIG_H
