/**
 * @file safety_module_handler.h
 * @brief Safety Module Handler for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-Phase3-1 (Safety Module Implementation)
 */

#ifndef SAFETY_MODULE_HANDLER_H
#define SAFETY_MODULE_HANDLER_H

#include "hal_common.h"
#include "communication_manager.h"
#include "register_map.h"
#include "safety_types.h"
#include <stdint.h>
#include <stdbool.h>

// Forward declaration for callback function
typedef struct safety_module_handler safety_module_handler_t;

// Safety Module Configuration
#define SAFETY_MODULE_MAX_ADDRESSES      16
#define SAFETY_MODULE_MAX_SENSORS        4
#define SAFETY_MODULE_MAX_RELAYS         4
#define SAFETY_MODULE_MAX_DISTANCE_MM    5000   // 5m max distance
#define SAFETY_MODULE_MIN_DISTANCE_MM    50     // 50mm min distance
#define SAFETY_MODULE_RESPONSE_TIME_MS   50     // < 50ms response time

// Safety Module Register Map (Address 0x0000-0x00FF)
#define SAFETY_STATUS_REG                0x0000
#define SAFETY_EMERGENCY_STOP_REG        0x0001
#define SAFETY_SAFETY_ZONE_REG           0x0002
#define SAFETY_PROXIMITY_ALERT_REG       0x0003
#define SAFETY_RELAY_OUTPUT_REG          0x0004
#define SAFETY_SAFETY_ERROR_REG          0x0005
#define SAFETY_SYSTEM_TEMP_REG           0x0006
#define SAFETY_SYSTEM_VOLTAGE_REG        0x0007

#define SAFETY_ANALOG_INPUT_1_REG        0x0010
#define SAFETY_ANALOG_INPUT_2_REG        0x0011
#define SAFETY_ANALOG_INPUT_3_REG        0x0012
#define SAFETY_ANALOG_INPUT_4_REG        0x0013
#define SAFETY_ANALOG_RAW_1_REG          0x0014
#define SAFETY_ANALOG_RAW_2_REG          0x0015
#define SAFETY_ANALOG_RAW_3_REG          0x0016
#define SAFETY_ANALOG_RAW_4_REG          0x0017

#define SAFETY_DIGITAL_INPUT_REG         0x0020
#define SAFETY_DI1_STATUS_REG            0x0021
#define SAFETY_DI2_STATUS_REG            0x0022
#define SAFETY_DI3_STATUS_REG            0x0023
#define SAFETY_DI4_STATUS_REG            0x0024

#define SAFETY_RELAY_CONTROL_REG         0x0030
#define SAFETY_RELAY_1_CONTROL_REG       0x0031
#define SAFETY_RELAY_2_CONTROL_REG       0x0032
#define SAFETY_RELAY_3_CONTROL_REG       0x0033
#define SAFETY_RELAY_4_CONTROL_REG       0x0034

#define SAFETY_ZONE_1_THRESHOLD_REG      0x0040
#define SAFETY_ZONE_2_THRESHOLD_REG      0x0041
#define SAFETY_ZONE_3_THRESHOLD_REG      0x0042
#define SAFETY_ZONE_4_THRESHOLD_REG      0x0043

#define SAFETY_PROXIMITY_THRESHOLD_REG   0x0050
#define SAFETY_RESPONSE_TIME_REG         0x0051
#define SAFETY_AUTO_RESET_ENABLE_REG     0x0052
#define SAFETY_SAFETY_MODE_REG           0x0053

// System Registers (0x0100-0x0109) - Auto Detect Support
#define SAFETY_DEVICE_ID_REG             0x0100  // Device ID (Modbus slave address)
#define SAFETY_CONFIG_BAUDRATE_REG       0x0101  // Config baudrate (1=9600, 2=19200, 3=38400,...)
#define SAFETY_CONFIG_PARITY_REG         0x0102  // Config parity (0=None, 1=Even, 2=Odd)
#define SAFETY_CONFIG_STOP_BITS_REG      0x0103  // Config stop bits (1=1, 2=2)
#define SAFETY_MODULE_TYPE_REG           0x0104  // Module type (0x0003 = Safety Module)
#define SAFETY_FIRMWARE_VERSION_REG      0x0105  // Firmware version (e.g. 0x0101 = v1.01)
#define SAFETY_HARDWARE_VERSION_REG      0x0106  // Hardware version (e.g. 0x0101 = v1.01)
#define SAFETY_SYSTEM_STATUS_REG         0x0107  // System status (bit field)
#define SAFETY_SYSTEM_ERROR_REG          0x0108  // System error (global error code)
#define SAFETY_RESET_ERROR_CMD_REG       0x0109  // Reset error command (write 1 to reset all error flags)
#define SAFETY_MODULE_NAME_LOW_REG       0x00F8  // Module name (low word)
#define SAFETY_MODULE_NAME_HIGH_REG      0x00F9  // Module name (high word)
#define SAFETY_HARDWARE_VERSION_REG      0x00FA  // Hardware version
#define SAFETY_SERIAL_NUMBER_LOW_REG     0x00FB  // Serial number (low word)
#define SAFETY_SERIAL_NUMBER_HIGH_REG    0x00FC  // Serial number (high word)
#define SAFETY_BUILD_DATE_LOW_REG        0x00FD  // Build date (low word)
#define SAFETY_BUILD_DATE_HIGH_REG       0x00FE  // Build date (high word)
#define SAFETY_CHECKSUM_REG              0x00FF  // Register checksum

// Safety Module Fault Codes - Using unified safety_fault_t from safety_types.h

// Safety Module States
typedef enum {
    SAFETY_STATE_DISABLED = 0,
    SAFETY_STATE_ENABLED,
    SAFETY_STATE_WARNING,
    SAFETY_STATE_CRITICAL,
    SAFETY_STATE_EMERGENCY_STOP,
    SAFETY_STATE_FAULT,
    SAFETY_STATE_MAINTENANCE
} safety_state_t;

// Safety Module Events - Using unified safety_event_t from safety_types.h

// Safety Zone Levels
typedef enum {
    SAFETY_ZONE_LEVEL_NONE = 0,
    SAFETY_ZONE_LEVEL_WARNING,    // Yellow zone
    SAFETY_ZONE_LEVEL_CRITICAL,   // Orange zone
    SAFETY_ZONE_LEVEL_EMERGENCY   // Red zone
} safety_zone_level_t;

// Safety Module Configuration Structure
typedef struct {
    uint8_t address;                    // Module address (0x03)
    uint32_t response_timeout_ms;       // Response timeout
    uint32_t update_interval_ms;        // Update interval
    uint16_t zone_thresholds[4];        // Zone thresholds (mm)
    uint16_t proximity_threshold;       // Proximity threshold (mm)
    bool auto_reset_enabled;            // Auto reset enabled
    uint8_t safety_mode;                // Safety mode
    bool enable_debouncing;             // Enable input debouncing
    uint32_t debounce_time_ms;          // Debounce time
} safety_module_config_t;

// Safety Module Status Structure
typedef struct {
    safety_state_t state;               // Current state
    safety_fault_t fault_code;     // Current fault code
    uint8_t fault_count;                // Fault count
    uint64_t last_update_time;          // Last update timestamp
    uint32_t response_time_ms;          // Response time
    uint8_t health_percentage;          // Health percentage
    bool emergency_stop_active;         // Emergency stop status
    bool safety_violation;              // Safety violation flag
} safety_module_status_t;

// Safety Module Data Structure
typedef struct {
    uint16_t analog_sensors[4];         // Analog sensor readings (mm)
    uint16_t analog_raw[4];             // Raw analog values (ADC)
    uint8_t digital_sensors;            // Digital sensor status (bitmap)
    uint8_t relay_outputs;              // Relay output status (bitmap)
    uint8_t safety_zones;               // Safety zone status (bitmap)
    bool proximity_alert;               // Proximity alert status
    int16_t system_temperature;         // System temperature (°C * 10)
    uint16_t system_voltage;            // System voltage (V * 10)
} safety_module_data_t;

// Safety Module Statistics Structure
typedef struct {
    uint32_t total_events;              // Total events
    uint32_t warning_events;            // Warning events
    uint32_t critical_events;           // Critical events
    uint32_t emergency_stop_events;     // Emergency stop events
    uint32_t fault_events;              // Fault events
    uint32_t sensor_activations;        // Sensor activations
    uint32_t relay_activations;         // Relay activations
    uint32_t zone_violations;           // Zone violations
    uint32_t communication_errors;      // Communication errors
    uint64_t total_uptime_ms;           // Total uptime
    uint64_t last_event_time;           // Last event timestamp
} safety_module_stats_t;

// Safety Module Handler Structure
typedef struct safety_module_handler {
    uint8_t address;                    // Module address
    safety_module_config_t config;      // Configuration
    safety_module_status_t status;      // Status
    safety_module_data_t data;          // Current data
    safety_module_stats_t statistics;   // Statistics
    bool initialized;                   // Initialization flag
    bool enabled;                       // Enabled flag
    uint64_t last_command_time;         // Last command timestamp
    uint64_t last_response_time;        // Last response timestamp
} safety_module_handler_t;

// Event callback function type - Using unified safety_event_callback_t from safety_types.h

// Function Prototypes

// Initialization and Configuration
hal_status_t safety_module_init(safety_module_handler_t *handler, const safety_module_config_t *config);
hal_status_t safety_module_deinit(safety_module_handler_t *handler);
hal_status_t safety_module_enable(safety_module_handler_t *handler, bool enable);
hal_status_t safety_module_get_config(safety_module_handler_t *handler, safety_module_config_t *config);
hal_status_t safety_module_set_config(safety_module_handler_t *handler, const safety_module_config_t *config);

// Status and Information
hal_status_t safety_module_get_status(safety_module_handler_t *handler, safety_module_status_t *status);
hal_status_t safety_module_get_data(safety_module_handler_t *handler, safety_module_data_t *data);
hal_status_t safety_module_get_statistics(safety_module_handler_t *handler, safety_module_stats_t *stats);
hal_status_t safety_module_get_diagnostics(safety_module_handler_t *handler, char *info, size_t max_len);

// Sensor Data Access
hal_status_t safety_module_get_analog_sensor(safety_module_handler_t *handler, uint8_t sensor_number, uint16_t *distance);
hal_status_t safety_module_get_analog_raw(safety_module_handler_t *handler, uint8_t sensor_number, uint16_t *raw_value);
hal_status_t safety_module_get_digital_sensors(safety_module_handler_t *handler, uint8_t *sensors);
hal_status_t safety_module_is_digital_sensor_active(safety_module_handler_t *handler, uint8_t sensor_number, bool *active);

// Relay Control
hal_status_t safety_module_set_relay(safety_module_handler_t *handler, uint8_t relay_number, bool state);
hal_status_t safety_module_get_relay(safety_module_handler_t *handler, uint8_t relay_number, bool *state);
hal_status_t safety_module_set_relays(safety_module_handler_t *handler, uint8_t relays);
hal_status_t safety_module_get_relays(safety_module_handler_t *handler, uint8_t *relays);

// Safety Logic
hal_status_t safety_module_check_safety(safety_module_handler_t *handler, bool *safe);
hal_status_t safety_module_get_safety_zones(safety_module_handler_t *handler, uint8_t *zones);
hal_status_t safety_module_is_zone_violated(safety_module_handler_t *handler, uint8_t zone_number, bool *violated);
hal_status_t safety_module_get_zone_level(safety_module_handler_t *handler, uint8_t zone_number, safety_zone_level_t *level);
hal_status_t safety_module_is_proximity_alert(safety_module_handler_t *handler, bool *alert);

// Emergency Stop
hal_status_t safety_module_is_emergency_stop_active(safety_module_handler_t *handler, bool *active);
hal_status_t safety_module_clear_emergency_stop(safety_module_handler_t *handler);
hal_status_t safety_module_test_emergency_stop(safety_module_handler_t *handler);

// Fault Management
hal_status_t safety_module_get_fault_code(safety_module_handler_t *handler, safety_fault_t *fault_code);
hal_status_t safety_module_clear_faults(safety_module_handler_t *handler);
hal_status_t safety_module_get_fault_count(safety_module_handler_t *handler, uint8_t *fault_count);

// Threshold Management
hal_status_t safety_module_set_zone_threshold(safety_module_handler_t *handler, uint8_t zone_number, uint16_t threshold);
hal_status_t safety_module_get_zone_threshold(safety_module_handler_t *handler, uint8_t zone_number, uint16_t *threshold);
hal_status_t safety_module_set_proximity_threshold(safety_module_handler_t *handler, uint16_t threshold);
hal_status_t safety_module_get_proximity_threshold(safety_module_handler_t *handler, uint16_t *threshold);

// Register Access
hal_status_t safety_module_read_register(safety_module_handler_t *handler, uint16_t reg, uint16_t *value);
hal_status_t safety_module_write_register(safety_module_handler_t *handler, uint16_t reg, uint16_t value);

// Event Handling
hal_status_t safety_module_set_event_callback(safety_module_handler_t *handler, safety_event_callback_t callback);
hal_status_t safety_module_clear_event_callback(safety_module_handler_t *handler);

// Utility Functions
hal_status_t safety_module_self_test(safety_module_handler_t *handler);
hal_status_t safety_module_reset(safety_module_handler_t *handler);
hal_status_t safety_module_update(safety_module_handler_t *handler);

// Validation Functions
hal_status_t safety_module_validate_config(const safety_module_config_t *config);
hal_status_t safety_module_validate_address(uint8_t address);
hal_status_t safety_module_validate_sensor_number(uint8_t sensor_number);
hal_status_t safety_module_validate_relay_number(uint8_t relay_number);
hal_status_t safety_module_validate_zone_number(uint8_t zone_number);

// Conversion Functions
safety_zone_level_t safety_module_distance_to_zone_level(uint16_t distance, uint16_t threshold);
const char* safety_module_get_fault_name(safety_fault_t fault_code);
const char* safety_module_get_state_name(safety_state_t state);
const char* safety_module_get_event_name(safety_event_t event);
const char* safety_module_get_zone_level_name(safety_zone_level_t level);

#endif // SAFETY_MODULE_HANDLER_H
