/**
 * @file io_module_handler.h
 * @brief IO Module Handler for OHT-50 Master Module
 * @version 1.0
 * @date 2025-01-27
 * 
 * This module provides comprehensive I/O control for digital and analog inputs/outputs
 * with debounce logic, edge detection, batch operations, and health monitoring.
 * 
 * Register Map (0x4000-0x60FF):
 * - Digital I/O: 0x4000-0x40FF
 * - Analog I/O: 0x5000-0x50FF  
 * - Configuration: 0x6000-0x60FF
 */

#ifndef IO_MODULE_HANDLER_H
#define IO_MODULE_HANDLER_H

#include "hal_common.h"
#include "communication_manager.h"
#include <stdint.h>
#include <stdbool.h>

// Forward declaration for callback function
typedef struct io_module_handler io_module_handler_t;

// IO Module Configuration
typedef struct {
    uint8_t address;                    // Module address (default: 0x04)
    uint16_t digital_input_count;       // Number of digital inputs (max: 16)
    uint16_t digital_output_count;      // Number of digital outputs (max: 16)
    uint16_t analog_input_count;        // Number of analog inputs (max: 8)
    uint16_t analog_output_count;       // Number of analog outputs (max: 8)
    uint16_t debounce_time_ms;          // Default debounce time (1-100ms)
    uint16_t sample_rate_hz;            // Analog sampling rate (1-1000Hz)
    bool enable_edge_detection;         // Enable edge detection
    bool enable_health_monitoring;      // Enable health monitoring
    bool enable_batch_operations;       // Enable batch read/write
} io_module_config_t;

// IO Module Data Structure
typedef struct {
    // Digital I/O Data
    uint16_t digital_inputs[16];        // Digital input values (0/1)
    uint16_t digital_outputs[16];       // Digital output values (0/1)
    uint16_t digital_input_history[16][8]; // Input history for debounce
    
    // Analog I/O Data
    uint16_t analog_inputs[8];          // Analog input values (0-4095)
    uint16_t analog_outputs[8];         // Analog output values (0-4095)
    uint16_t analog_input_history[8][4]; // Input history for filtering
    
    // Configuration Data
    uint16_t debounce_config[16];       // Debounce time per channel (ms)
    uint16_t edge_config[16];           // Edge detection config per channel
    uint16_t analog_config[8];          // Analog configuration per channel
    
    // Status Data
    uint8_t health_status;              // Module health status
    uint8_t fault_status;               // Fault status
    uint16_t error_count;               // Error counter
    uint32_t total_operations;          // Total operations counter
    uint32_t last_update_timestamp;     // Last update timestamp
} io_module_data_t;

// IO Event Types
typedef enum {
    IO_EVENT_DIGITAL_INPUT_CHANGED,     // Digital input state changed
    IO_EVENT_ANALOG_INPUT_CHANGED,      // Analog input value changed
    IO_EVENT_DIGITAL_OUTPUT_CHANGED,    // Digital output state changed
    IO_EVENT_ANALOG_OUTPUT_CHANGED,     // Analog output value changed
    IO_EVENT_EDGE_DETECTED,             // Edge detected (rising/falling)
    IO_EVENT_FAULT_DETECTED,            // Fault detected
    IO_EVENT_FAULT_CLEARED,             // Fault cleared
    IO_EVENT_HEALTH_STATUS_CHANGED,     // Health status changed
    IO_EVENT_BATCH_OPERATION_COMPLETE,  // Batch operation completed
    IO_EVENT_CONFIGURATION_CHANGED      // Configuration changed
} io_event_t;

// IO State Types
typedef enum {
    IO_STATE_IDLE,                      // Module idle
    IO_STATE_READING,                   // Reading I/O values
    IO_STATE_WRITING,                   // Writing I/O values
    IO_STATE_CONFIGURING,               // Configuring module
    IO_STATE_FAULT,                     // Module in fault state
    IO_STATE_RECOVERING                 // Module recovering from fault
} io_state_t;

// IO Fault Codes
typedef enum {
    IO_FAULT_NONE = 0x0000,            // No fault
    IO_FAULT_COMMUNICATION_ERROR = 0x0001, // Communication error
    IO_FAULT_INVALID_CHANNEL = 0x0002, // Invalid channel number
    IO_FAULT_INVALID_VALUE = 0x0004,   // Invalid I/O value
    IO_FAULT_CONFIGURATION_ERROR = 0x0008, // Configuration error
    IO_FAULT_HARDWARE_ERROR = 0x0010,  // Hardware error
    IO_FAULT_TIMEOUT_ERROR = 0x0020,   // Operation timeout
    IO_FAULT_OVERVOLTAGE = 0x0040,     // Overvoltage detected
    IO_FAULT_UNDERVOLTAGE = 0x0080,    // Undervoltage detected
    IO_FAULT_OVERCURRENT = 0x0100,     // Overcurrent detected
    IO_FAULT_TEMPERATURE_HIGH = 0x0200, // High temperature
    IO_FAULT_SELF_TEST_FAILED = 0x0400 // Self-test failed
} io_fault_code_t;

// Edge Detection Types
typedef enum {
    IO_EDGE_NONE = 0x00,               // No edge detection
    IO_EDGE_RISING = 0x01,             // Rising edge detection
    IO_EDGE_FALLING = 0x02,            // Falling edge detection
    IO_EDGE_BOTH = 0x03                // Both edges detection
} io_edge_type_t;

// IO Module Handler Structure
typedef struct io_module_handler {
    uint8_t address;                   // Module address
    io_module_data_t data;             // I/O data
    io_module_config_t config;         // Module configuration
    io_state_t state;                  // Current state
    io_fault_code_t fault_code;        // Current fault code
    bool initialized;                  // Initialization flag
    bool enabled;                      // Enable flag
    
    // Statistics
    uint32_t total_reads;              // Total read operations
    uint32_t total_writes;             // Total write operations
    uint32_t total_errors;             // Total errors
    uint32_t last_operation_timestamp; // Last operation timestamp
    
    // Event callback
    void (*event_callback)(io_module_handler_t *handler, io_event_t event);
} io_module_handler_t;

// Register Map Definitions
#define IO_DIGITAL_INPUTS_REG           0x4000
#define IO_DIGITAL_OUTPUTS_REG          0x4100
#define IO_ANALOG_INPUTS_REG            0x5000
#define IO_ANALOG_OUTPUTS_REG           0x5100
#define IO_DEBOUNCE_CONFIG_REG          0x6000
#define IO_EDGE_CONFIG_REG              0x6100
#define IO_ANALOG_CONFIG_REG            0x6200
#define IO_HEALTH_STATUS_REG            0x6300
#define IO_FAULT_STATUS_REG             0x6301
#define IO_ERROR_COUNT_REG              0x6302
#define IO_TOTAL_OPERATIONS_REG         0x6303

// Default Configuration
#define IO_DEFAULT_ADDRESS              0x04
#define IO_DEFAULT_DIGITAL_INPUTS       16
#define IO_DEFAULT_DIGITAL_OUTPUTS      16
#define IO_DEFAULT_ANALOG_INPUTS        8
#define IO_DEFAULT_ANALOG_OUTPUTS       8
#define IO_DEFAULT_DEBOUNCE_TIME_MS     10
#define IO_DEFAULT_SAMPLE_RATE_HZ       100
#define IO_DEFAULT_ENABLE_EDGE_DETECTION true
#define IO_DEFAULT_ENABLE_HEALTH_MONITORING true
#define IO_DEFAULT_ENABLE_BATCH_OPERATIONS true

// Limits
#define IO_MAX_DIGITAL_CHANNELS         16
#define IO_MAX_ANALOG_CHANNELS          8
#define IO_MAX_DEBOUNCE_TIME_MS         100
#define IO_MAX_SAMPLE_RATE_HZ           1000
#define IO_MAX_ANALOG_VALUE             4095

// API Function Prototypes

// Initialization and Configuration
hal_status_t io_module_init(io_module_handler_t *handler, const io_module_config_t *config);
hal_status_t io_module_deinit(io_module_handler_t *handler);
hal_status_t io_module_update(io_module_handler_t *handler);
hal_status_t io_module_enable(io_module_handler_t *handler, bool enable);
hal_status_t io_module_configure(io_module_handler_t *handler, const io_module_config_t *config);

// Digital I/O Operations
hal_status_t io_module_read_digital_input(io_module_handler_t *handler, uint8_t channel, bool *value);
hal_status_t io_module_read_digital_inputs(io_module_handler_t *handler, uint16_t *values);
hal_status_t io_module_write_digital_output(io_module_handler_t *handler, uint8_t channel, bool value);
hal_status_t io_module_write_digital_outputs(io_module_handler_t *handler, const uint16_t *values);
hal_status_t io_module_toggle_digital_output(io_module_handler_t *handler, uint8_t channel);

// Analog I/O Operations
hal_status_t io_module_read_analog_input(io_module_handler_t *handler, uint8_t channel, uint16_t *value);
hal_status_t io_module_read_analog_inputs(io_module_handler_t *handler, uint16_t *values);
hal_status_t io_module_write_analog_output(io_module_handler_t *handler, uint8_t channel, uint16_t value);
hal_status_t io_module_write_analog_outputs(io_module_handler_t *handler, const uint16_t *values);

// Batch Operations
hal_status_t io_module_batch_read(io_module_handler_t *handler, uint16_t *data, uint8_t count);
hal_status_t io_module_batch_write(io_module_handler_t *handler, const uint16_t *data, uint8_t count);
hal_status_t io_module_batch_read_digital(io_module_handler_t *handler, uint16_t *inputs, uint16_t *outputs);
hal_status_t io_module_batch_write_digital(io_module_handler_t *handler, const uint16_t *outputs);
hal_status_t io_module_batch_read_analog(io_module_handler_t *handler, uint16_t *inputs, uint16_t *outputs);
hal_status_t io_module_batch_write_analog(io_module_handler_t *handler, const uint16_t *outputs);

// Debounce and Edge Detection
hal_status_t io_module_set_debounce(io_module_handler_t *handler, uint8_t channel, uint16_t debounce_ms);
hal_status_t io_module_get_debounce(io_module_handler_t *handler, uint8_t channel, uint16_t *debounce_ms);
hal_status_t io_module_set_edge_detection(io_module_handler_t *handler, uint8_t channel, io_edge_type_t edge_type);
hal_status_t io_module_get_edge_detection(io_module_handler_t *handler, uint8_t channel, io_edge_type_t *edge_type);
hal_status_t io_module_clear_edge_events(io_module_handler_t *handler);

// Data Access
hal_status_t io_module_get_data(io_module_handler_t *handler, io_module_data_t *data);
hal_status_t io_module_set_digital_input(io_module_handler_t *handler, uint8_t channel, bool value);
hal_status_t io_module_set_analog_input(io_module_handler_t *handler, uint8_t channel, uint16_t value);
hal_status_t io_module_get_digital_output(io_module_handler_t *handler, uint8_t channel, bool *value);
hal_status_t io_module_get_analog_output(io_module_handler_t *handler, uint8_t channel, uint16_t *value);

// Status and Monitoring
hal_status_t io_module_get_status(io_module_handler_t *handler, io_state_t *state, io_fault_code_t *fault_code);
bool io_module_is_enabled(io_module_handler_t *handler);
bool io_module_has_faults(io_module_handler_t *handler);
const char* io_module_get_fault_description(io_module_handler_t *handler);
hal_status_t io_module_reset_faults(io_module_handler_t *handler);
hal_status_t io_module_get_health_status(io_module_handler_t *handler, uint8_t *health_status);

// Validation
hal_status_t io_module_validate_channel(io_module_handler_t *handler, uint8_t channel, bool is_analog);
bool io_module_validate_digital_value(bool value);
bool io_module_validate_analog_value(uint16_t value);
bool io_module_validate_debounce_time(uint16_t debounce_ms);
bool io_module_validate_sample_rate(uint16_t sample_rate_hz);

// Modbus Communication
hal_status_t io_module_read_register(io_module_handler_t *handler, uint16_t register_addr, uint16_t *value);
hal_status_t io_module_write_register(io_module_handler_t *handler, uint16_t register_addr, uint16_t value);
hal_status_t io_module_read_registers(io_module_handler_t *handler, uint16_t start_register, uint16_t count, uint16_t *values);
hal_status_t io_module_write_registers(io_module_handler_t *handler, uint16_t start_register, uint16_t count, const uint16_t *values);

// Event Handling
hal_status_t io_module_set_callback(io_module_handler_t *handler, 
                                   void (*callback)(io_module_handler_t *handler, io_event_t event));
hal_status_t io_module_trigger_event(io_module_handler_t *handler, io_event_t event);

// Diagnostics and Testing
hal_status_t io_module_get_diagnostics(io_module_handler_t *handler, char *info, size_t max_len);
hal_status_t io_module_self_test(io_module_handler_t *handler);
hal_status_t io_module_reset_statistics(io_module_handler_t *handler);
hal_status_t io_module_get_statistics(io_module_handler_t *handler, uint32_t *total_reads, uint32_t *total_writes, uint32_t *total_errors);

// Utility Functions
const char* io_module_get_state_string(io_state_t state);
const char* io_module_get_fault_string(io_fault_code_t fault_code);
const char* io_module_get_edge_string(io_edge_type_t edge_type);
uint32_t io_module_get_timestamp(void);

#endif // IO_MODULE_HANDLER_H
