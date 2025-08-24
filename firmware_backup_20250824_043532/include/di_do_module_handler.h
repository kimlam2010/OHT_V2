/**
 * @file di_do_module_handler.h
 * @brief DI/DO Module Handler for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * 
 * This module provides comprehensive Digital Input/Output and Analog Input/Output
 * functionality with advanced features like debouncing, edge detection, and
 * batch operations for efficient industrial automation applications.
 */

#ifndef DI_DO_MODULE_HANDLER_H
#define DI_DO_MODULE_HANDLER_H

#include "hal_common.h"
#include "communication_manager.h"
#include "safety_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// CONSTANTS AND LIMITS
// ============================================================================

#define DI_DO_MODULE_ADDRESS             0x06
#define DI_DO_MODULE_NAME                "DI/DO Module"

// Register Map (0xB000-0xE0FF)
#define DI_DO_DIGITAL_INPUTS_REG         0xB000  // Digital inputs status (16 channels)
#define DI_DO_DIGITAL_OUTPUTS_REG        0xB100  // Digital outputs control (16 channels)
#define DI_DO_ANALOG_INPUTS_REG          0xC000  // Analog inputs (8 channels, 12-bit)
#define DI_DO_ANALOG_OUTPUTS_REG         0xC100  // Analog outputs (8 channels, 12-bit)
#define DI_DO_DEBOUNCE_CONFIG_REG        0xD000  // Debounce configuration (16 channels)
#define DI_DO_EDGE_CONFIG_REG            0xD100  // Edge detection configuration
#define DI_DO_FILTER_CONFIG_REG          0xD200  // Analog filter configuration
#define DI_DO_STATUS_REG                 0xE000  // Module status and health
#define DI_DO_FAULT_REG                  0xE001  // Fault status and codes
#define DI_DO_TEMPERATURE_REG            0xE002  // Module temperature
#define DI_DO_VOLTAGE_REG                0xE003  // Supply voltage monitoring

// Configuration Limits
#define DI_DO_MAX_DIGITAL_CHANNELS       16
#define DI_DO_MAX_ANALOG_CHANNELS        8
#define DI_DO_MAX_DEBOUNCE_TIME_MS       1000
#define DI_DO_MIN_DEBOUNCE_TIME_MS       1
#define DI_DO_MAX_ANALOG_VALUE           4095    // 12-bit resolution
#define DI_DO_MIN_ANALOG_VALUE           0
#define DI_DO_MAX_FILTER_TIME_CONSTANT   1000    // ms
#define DI_DO_MIN_FILTER_TIME_CONSTANT   1       // ms

// Default Configuration
#define DI_DO_DEFAULT_DEBOUNCE_TIME_MS   50
#define DI_DO_DEFAULT_FILTER_TIME_MS     100
#define DI_DO_DEFAULT_UPDATE_RATE_MS     10
#define DI_DO_DEFAULT_EDGE_DETECTION     DI_DO_EDGE_RISING_FALLING

// ============================================================================
// DATA STRUCTURES
// ============================================================================

/**
 * @brief Digital input/output data structure
 */
typedef struct {
    uint16_t digital_inputs[DI_DO_MAX_DIGITAL_CHANNELS];     // Current digital input states
    uint16_t digital_outputs[DI_DO_MAX_DIGITAL_CHANNELS];    // Current digital output states
    uint16_t analog_inputs[DI_DO_MAX_ANALOG_CHANNELS];       // Current analog input values (12-bit)
    uint16_t analog_outputs[DI_DO_MAX_ANALOG_CHANNELS];      // Current analog output values (12-bit)
    uint16_t debounce_times[DI_DO_MAX_DIGITAL_CHANNELS];     // Debounce times per channel (ms)
    uint8_t edge_config[DI_DO_MAX_DIGITAL_CHANNELS];         // Edge detection configuration
    uint16_t filter_time_constants[DI_DO_MAX_ANALOG_CHANNELS]; // Analog filter time constants (ms)
    uint8_t channel_enabled[DI_DO_MAX_DIGITAL_CHANNELS + DI_DO_MAX_ANALOG_CHANNELS]; // Channel enable flags
    uint32_t last_update_time;                               // Last update timestamp
    uint32_t update_count;                                   // Update counter
} di_do_module_data_t;

/**
 * @brief DI/DO module configuration structure
 */
typedef struct {
    uint16_t debounce_time_ms;                              // Default debounce time
    uint16_t filter_time_ms;                                // Default analog filter time
    uint16_t update_rate_ms;                                // Update rate
    uint8_t edge_detection;                                 // Default edge detection mode
    bool enable_analog_filtering;                           // Enable analog filtering
    bool enable_digital_debouncing;                         // Enable digital debouncing
    bool enable_edge_detection;                             // Enable edge detection
    bool enable_health_monitoring;                          // Enable health monitoring
    uint16_t fault_threshold_temp;                          // Temperature fault threshold
    uint16_t fault_threshold_voltage;                       // Voltage fault threshold
    uint8_t retry_count;                                    // Communication retry count
    uint16_t timeout_ms;                                    // Communication timeout
} di_do_config_t;

/**
 * @brief DI/DO module handler structure
 */
typedef struct di_do_module_handler {
    uint8_t address;                                        // Module address
    bool initialized;                                       // Initialization flag
    bool enabled;                                           // Enable flag
    di_do_module_data_t data;                               // Module data
    di_do_config_t config;                                  // Module configuration
    uint32_t fault_flags;                                   // Fault flags
    uint8_t fault_code;                                     // Fault code
    uint32_t statistics[10];                                // Statistics counters
    void (*event_callback)(struct di_do_module_handler *handler, uint8_t event, uint8_t channel, uint16_t value);
} di_do_module_handler_t;

// ============================================================================
// ENUMS AND TYPES
// ============================================================================

/**
 * @brief Edge detection types
 */
typedef enum {
    DI_DO_EDGE_NONE = 0x00,                                // No edge detection
    DI_DO_EDGE_RISING = 0x01,                              // Rising edge only
    DI_DO_EDGE_FALLING = 0x02,                             // Falling edge only
    DI_DO_EDGE_RISING_FALLING = 0x03                       // Both rising and falling
} di_do_edge_type_t;

/**
 * @brief DI/DO events
 */
typedef enum {
    DI_DO_EVENT_DIGITAL_INPUT_CHANGED,                     // Digital input state changed
    DI_DO_EVENT_ANALOG_INPUT_CHANGED,                      // Analog input value changed
    DI_DO_EVENT_EDGE_DETECTED,                             // Edge detected on digital input
    DI_DO_EVENT_FAULT_DETECTED,                            // Fault detected
    DI_DO_EVENT_HEALTH_ALERT,                              // Health monitoring alert
    DI_DO_EVENT_COMMUNICATION_ERROR,                       // Communication error
    DI_DO_EVENT_CONFIGURATION_CHANGED,                     // Configuration changed
    DI_DO_EVENT_CALIBRATION_COMPLETED,                     // Calibration completed
    DI_DO_EVENT_SELF_TEST_COMPLETED,                       // Self-test completed
    DI_DO_EVENT_EMERGENCY_STOP                             // Emergency stop triggered
} di_do_event_t;

/**
 * @brief DI/DO fault codes
 */
typedef enum {
    DI_DO_FAULT_NONE = 0x00,                               // No fault
    DI_DO_FAULT_COMMUNICATION_ERROR = 0x01,                // Communication error
    DI_DO_FAULT_TEMPERATURE_HIGH = 0x02,                   // Temperature too high
    DI_DO_FAULT_VOLTAGE_LOW = 0x03,                        // Voltage too low
    DI_DO_FAULT_VOLTAGE_HIGH = 0x04,                       // Voltage too high
    DI_DO_FAULT_CHANNEL_FAULT = 0x05,                      // Channel fault
    DI_DO_FAULT_CONFIGURATION_ERROR = 0x06,                // Configuration error
    DI_DO_FAULT_CALIBRATION_ERROR = 0x07,                  // Calibration error
    DI_DO_FAULT_SELF_TEST_FAILED = 0x08,                   // Self-test failed
    DI_DO_FAULT_SAFETY_VIOLATION = 0x09,                   // Safety violation
    DI_DO_FAULT_HARDWARE_ERROR = 0x0A                      // Hardware error
} di_do_fault_code_t;

/**
 * @brief DI/DO status flags
 */
typedef enum {
    DI_DO_STATUS_ENABLED = 0x01,                           // Module enabled
    DI_DO_STATUS_COMMUNICATION_OK = 0x02,                  // Communication OK
    DI_DO_STATUS_HEALTH_OK = 0x04,                         // Health OK
    DI_DO_STATUS_CALIBRATED = 0x08,                        // Calibrated
    DI_DO_STATUS_SELF_TEST_PASSED = 0x10,                  // Self-test passed
    DI_DO_STATUS_SAFETY_OK = 0x20,                         // Safety OK
    DI_DO_STATUS_FAULT_ACTIVE = 0x40,                      // Fault active
    DI_DO_STATUS_EMERGENCY_STOP = 0x80                     // Emergency stop
} di_do_status_t;

// ============================================================================
// CORE MODULE FUNCTIONS
// ============================================================================

/**
 * @brief Initialize DI/DO module handler
 * @param handler Pointer to module handler
 * @param address Module address
 * @return HAL status
 */
hal_status_t di_do_module_init(di_do_module_handler_t *handler, uint8_t address);

/**
 * @brief Deinitialize DI/DO module handler
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_deinit(di_do_module_handler_t *handler);

/**
 * @brief Update DI/DO module (read inputs, process outputs)
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_update(di_do_module_handler_t *handler);

/**
 * @brief Enable/disable DI/DO module
 * @param handler Pointer to module handler
 * @param enable Enable flag
 * @return HAL status
 */
hal_status_t di_do_module_enable(di_do_module_handler_t *handler, bool enable);

// ============================================================================
// DIGITAL I/O FUNCTIONS
// ============================================================================

/**
 * @brief Read digital input channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-15)
 * @param value Pointer to store input value
 * @return HAL status
 */
hal_status_t di_do_module_read_digital_input(di_do_module_handler_t *handler, uint8_t channel, uint16_t *value);

/**
 * @brief Read all digital inputs
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_read_digital_inputs(di_do_module_handler_t *handler);

/**
 * @brief Write digital output channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-15)
 * @param value Output value
 * @return HAL status
 */
hal_status_t di_do_module_write_digital_output(di_do_module_handler_t *handler, uint8_t channel, uint16_t value);

/**
 * @brief Write all digital outputs
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_write_digital_outputs(di_do_module_handler_t *handler);

/**
 * @brief Toggle digital output channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-15)
 * @return HAL status
 */
hal_status_t di_do_module_toggle_digital_output(di_do_module_handler_t *handler, uint8_t channel);

// ============================================================================
// ANALOG I/O FUNCTIONS
// ============================================================================

/**
 * @brief Read analog input channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-7)
 * @param value Pointer to store input value
 * @return HAL status
 */
hal_status_t di_do_module_read_analog_input(di_do_module_handler_t *handler, uint8_t channel, uint16_t *value);

/**
 * @brief Read all analog inputs
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_read_analog_inputs(di_do_module_handler_t *handler);

/**
 * @brief Write analog output channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-7)
 * @param value Output value (12-bit)
 * @return HAL status
 */
hal_status_t di_do_module_write_analog_output(di_do_module_handler_t *handler, uint8_t channel, uint16_t value);

/**
 * @brief Write all analog outputs
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_write_analog_outputs(di_do_module_handler_t *handler);

// ============================================================================
// BATCH OPERATIONS
// ============================================================================

/**
 * @brief Batch read digital inputs
 * @param handler Pointer to module handler
 * @param start_channel Starting channel
 * @param count Number of channels
 * @param values Array to store values
 * @return HAL status
 */
hal_status_t di_do_module_batch_read_digital(di_do_module_handler_t *handler, uint8_t start_channel, uint8_t count, uint16_t *values);

/**
 * @brief Batch write digital outputs
 * @param handler Pointer to module handler
 * @param start_channel Starting channel
 * @param count Number of channels
 * @param values Array of values to write
 * @return HAL status
 */
hal_status_t di_do_module_batch_write_digital(di_do_module_handler_t *handler, uint8_t start_channel, uint8_t count, uint16_t *values);

/**
 * @brief Batch read analog inputs
 * @param handler Pointer to module handler
 * @param start_channel Starting channel
 * @param count Number of channels
 * @param values Array to store values
 * @return HAL status
 */
hal_status_t di_do_module_batch_read_analog(di_do_module_handler_t *handler, uint8_t start_channel, uint8_t count, uint16_t *values);

/**
 * @brief Batch write analog outputs
 * @param handler Pointer to module handler
 * @param start_channel Starting channel
 * @param count Number of channels
 * @param values Array of values to write
 * @return HAL status
 */
hal_status_t di_do_module_batch_write_analog(di_do_module_handler_t *handler, uint8_t start_channel, uint8_t count, uint16_t *values);

// ============================================================================
// CONFIGURATION FUNCTIONS
// ============================================================================

/**
 * @brief Set debounce time for digital input channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-15)
 * @param debounce_time_ms Debounce time in milliseconds
 * @return HAL status
 */
hal_status_t di_do_module_set_debounce_time(di_do_module_handler_t *handler, uint8_t channel, uint16_t debounce_time_ms);

/**
 * @brief Set edge detection for digital input channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-15)
 * @param edge_type Edge detection type
 * @return HAL status
 */
hal_status_t di_do_module_set_edge_detection(di_do_module_handler_t *handler, uint8_t channel, di_do_edge_type_t edge_type);

/**
 * @brief Set analog filter time constant
 * @param handler Pointer to module handler
 * @param channel Channel number (0-7)
 * @param time_constant_ms Filter time constant in milliseconds
 * @return HAL status
 */
hal_status_t di_do_module_set_analog_filter(di_do_module_handler_t *handler, uint8_t channel, uint16_t time_constant_ms);

/**
 * @brief Enable/disable digital input channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-15)
 * @param enable Enable flag
 * @return HAL status
 */
hal_status_t di_do_module_enable_digital_channel(di_do_module_handler_t *handler, uint8_t channel, bool enable);

/**
 * @brief Enable/disable analog input channel
 * @param handler Pointer to module handler
 * @param channel Channel number (0-7)
 * @param enable Enable flag
 * @return HAL status
 */
hal_status_t di_do_module_enable_analog_channel(di_do_module_handler_t *handler, uint8_t channel, bool enable);

/**
 * @brief Get module configuration
 * @param handler Pointer to module handler
 * @param config Pointer to store configuration
 * @return HAL status
 */
hal_status_t di_do_module_get_config(di_do_module_handler_t *handler, di_do_config_t *config);

/**
 * @brief Set module configuration
 * @param handler Pointer to module handler
 * @param config Pointer to configuration
 * @return HAL status
 */
hal_status_t di_do_module_set_config(di_do_module_handler_t *handler, const di_do_config_t *config);

// ============================================================================
// STATUS AND MONITORING FUNCTIONS
// ============================================================================

/**
 * @brief Get module status
 * @param handler Pointer to module handler
 * @param status Pointer to store status
 * @return HAL status
 */
hal_status_t di_do_module_get_status(di_do_module_handler_t *handler, uint8_t *status);

/**
 * @brief Get fault status
 * @param handler Pointer to module handler
 * @param fault_code Pointer to store fault code
 * @return HAL status
 */
hal_status_t di_do_module_get_fault(di_do_module_handler_t *handler, uint8_t *fault_code);

/**
 * @brief Reset faults
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_reset_faults(di_do_module_handler_t *handler);

/**
 * @brief Get temperature
 * @param handler Pointer to module handler
 * @param temperature Pointer to store temperature
 * @return HAL status
 */
hal_status_t di_do_module_get_temperature(di_do_module_handler_t *handler, uint16_t *temperature);

/**
 * @brief Get supply voltage
 * @param handler Pointer to module handler
 * @param voltage Pointer to store voltage
 * @return HAL status
 */
hal_status_t di_do_module_get_voltage(di_do_module_handler_t *handler, uint16_t *voltage);

/**
 * @brief Get statistics
 * @param handler Pointer to module handler
 * @param statistics Array to store statistics
 * @return HAL status
 */
hal_status_t di_do_module_get_statistics(di_do_module_handler_t *handler, uint32_t *statistics);

// ============================================================================
// DIAGNOSTICS AND TESTING FUNCTIONS
// ============================================================================

/**
 * @brief Run self-test
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_self_test(di_do_module_handler_t *handler);

/**
 * @brief Calibrate analog inputs
 * @param handler Pointer to module handler
 * @return HAL status
 */
hal_status_t di_do_module_calibrate_analog(di_do_module_handler_t *handler);

/**
 * @brief Get diagnostic information
 * @param handler Pointer to module handler
 * @param info Buffer to store diagnostic info
 * @param max_len Maximum length of info buffer
 * @return HAL status
 */
hal_status_t di_do_module_get_diagnostics(di_do_module_handler_t *handler, char *info, size_t max_len);

// ============================================================================
// MODBUS COMMUNICATION FUNCTIONS
// ============================================================================

/**
 * @brief Read Modbus register
 * @param handler Pointer to module handler
 * @param reg Register address
 * @param value Pointer to store value
 * @return HAL status
 */
hal_status_t di_do_module_read_register(di_do_module_handler_t *handler, uint16_t reg, uint16_t *value);

/**
 * @brief Write Modbus register
 * @param handler Pointer to module handler
 * @param reg Register address
 * @param value Value to write
 * @return HAL status
 */
hal_status_t di_do_module_write_register(di_do_module_handler_t *handler, uint16_t reg, uint16_t value);

/**
 * @brief Read multiple Modbus registers
 * @param handler Pointer to module handler
 * @param start_reg Starting register address
 * @param count Number of registers
 * @param values Array to store values
 * @return HAL status
 */
hal_status_t di_do_module_read_registers(di_do_module_handler_t *handler, uint16_t start_reg, uint16_t count, uint16_t *values);

/**
 * @brief Write multiple Modbus registers
 * @param handler Pointer to module handler
 * @param start_reg Starting register address
 * @param count Number of registers
 * @param values Array of values to write
 * @return HAL status
 */
hal_status_t di_do_module_write_registers(di_do_module_handler_t *handler, uint16_t start_reg, uint16_t count, uint16_t *values);

// ============================================================================
// EVENT HANDLING FUNCTIONS
// ============================================================================

/**
 * @brief Set event callback function
 * @param handler Pointer to module handler
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t di_do_module_set_event_callback(di_do_module_handler_t *handler, void (*callback)(di_do_module_handler_t *handler, uint8_t event, uint8_t channel, uint16_t value));

/**
 * @brief Trigger event
 * @param handler Pointer to module handler
 * @param event Event type
 * @param channel Channel number
 * @param value Event value
 * @return HAL status
 */
hal_status_t di_do_module_trigger_event(di_do_module_handler_t *handler, uint8_t event, uint8_t channel, uint16_t value);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Get fault description
 * @param fault_code Fault code
 * @return Fault description string
 */
const char* di_do_module_get_fault_description(uint8_t fault_code);

/**
 * @brief Get event description
 * @param event Event type
 * @return Event description string
 */
const char* di_do_module_get_event_description(uint8_t event);

/**
 * @brief Validate channel number
 * @param channel Channel number
 * @param is_analog True if analog channel
 * @return true if valid
 */
bool di_do_module_validate_channel(uint8_t channel, bool is_analog);

/**
 * @brief Validate configuration
 * @param config Configuration to validate
 * @return true if valid
 */
bool di_do_module_validate_config(const di_do_config_t *config);

#ifdef __cplusplus
}
#endif

#endif // DI_DO_MODULE_HANDLER_H
