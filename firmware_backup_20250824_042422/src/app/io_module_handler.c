/**
 * @file io_module_handler.c
 * @brief IO Module Handler Implementation for OHT-50 Master Module
 * @version 1.0
 * @date 2025-01-27
 */

#include "io_module_handler.h"
#include "safety_manager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Default configuration
static const io_module_config_t default_config = {
    .address = IO_DEFAULT_ADDRESS,
    .digital_input_count = IO_DEFAULT_DIGITAL_INPUTS,
    .digital_output_count = IO_DEFAULT_DIGITAL_OUTPUTS,
    .analog_input_count = IO_DEFAULT_ANALOG_INPUTS,
    .analog_output_count = IO_DEFAULT_ANALOG_OUTPUTS,
    .debounce_time_ms = IO_DEFAULT_DEBOUNCE_TIME_MS,
    .sample_rate_hz = IO_DEFAULT_SAMPLE_RATE_HZ,
    .enable_edge_detection = IO_DEFAULT_ENABLE_EDGE_DETECTION,
    .enable_health_monitoring = IO_DEFAULT_ENABLE_HEALTH_MONITORING,
    .enable_batch_operations = IO_DEFAULT_ENABLE_BATCH_OPERATIONS
};

// Static function prototypes
static hal_status_t handle_io_event(io_module_handler_t *handler, io_event_t event);
static hal_status_t update_io_state(io_module_handler_t *handler);
static hal_status_t check_io_faults(io_module_handler_t *handler);
static hal_status_t validate_io_limits(io_module_handler_t *handler);
static hal_status_t simulate_io_operations(io_module_handler_t *handler);
static hal_status_t apply_debounce_logic(io_module_handler_t *handler, uint8_t channel);
static hal_status_t check_edge_detection(io_module_handler_t *handler, uint8_t channel);

// Initialization and Configuration
hal_status_t io_module_init(io_module_handler_t *handler, const io_module_config_t *config) {
    printf("[IO] io_module_init called\n");
    
    if (handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Clear the handler structure first
    memset(handler, 0, sizeof(io_module_handler_t));
    
    if (handler->initialized) {
        printf("[IO] Already initialized, returning\n");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Set configuration
    if (config != NULL) {
        printf("[IO] Using provided config\n");
        handler->config = *config;
    } else {
        printf("[IO] Using default config\n");
        handler->config = default_config;
    }
    
    // Validate configuration
    if (!io_module_validate_debounce_time(handler->config.debounce_time_ms)) {
        printf("[IO] Invalid debounce time: %u ms\n", handler->config.debounce_time_ms);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_sample_rate(handler->config.sample_rate_hz)) {
        printf("[IO] Invalid sample rate: %u Hz\n", handler->config.sample_rate_hz);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize data structure
    memset(&handler->data, 0, sizeof(io_module_data_t));
    
    // Set default debounce configuration
    for (int i = 0; i < IO_MAX_DIGITAL_CHANNELS; i++) {
        handler->data.debounce_config[i] = handler->config.debounce_time_ms;
        handler->data.edge_config[i] = IO_EDGE_NONE;
    }
    
    // Set default analog configuration
    for (int i = 0; i < IO_MAX_ANALOG_CHANNELS; i++) {
        handler->data.analog_config[i] = handler->config.sample_rate_hz;
    }
    
    // Initialize state
    handler->state = IO_STATE_IDLE;
    handler->fault_code = IO_FAULT_NONE;
    handler->initialized = true;
    handler->enabled = false;
    
    // Initialize statistics
    handler->total_reads = 0;
    handler->total_writes = 0;
    handler->total_errors = 0;
    handler->last_operation_timestamp = hal_get_timestamp_ms();
    
    printf("[IO] Initialized successfully (Address: 0x%02X)\n", handler->config.address);
    return HAL_STATUS_OK;
}

hal_status_t io_module_deinit(io_module_handler_t *handler) {
    if (handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Disable module first
    io_module_enable(handler, false);
    
    // Clear all data
    memset(&handler->data, 0, sizeof(io_module_data_t));
    handler->initialized = false;
    handler->enabled = false;
    handler->event_callback = NULL;
    
    printf("[IO] Deinitialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t io_module_update(io_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->enabled) {
        return HAL_STATUS_OK; // Module disabled, no update needed
    }
    
    // Update timestamp
    handler->data.last_update_timestamp = hal_get_timestamp_ms();
    
    // Update state
    hal_status_t status = update_io_state(handler);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check for faults
    status = check_io_faults(handler);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Apply debounce logic to all digital inputs
    for (uint8_t i = 0; i < handler->config.digital_input_count; i++) {
        apply_debounce_logic(handler, i);
    }
    
    // Check edge detection
    if (handler->config.enable_edge_detection) {
        for (uint8_t i = 0; i < handler->config.digital_input_count; i++) {
            check_edge_detection(handler, i);
        }
    }
    
    // Simulate I/O operations (for testing)
    simulate_io_operations(handler);
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_enable(io_module_handler_t *handler, bool enable) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    printf("[IO] %s IO module\n", enable ? "Enabling" : "Disabling");
    
    // Check safety before enabling
    if (enable) {
        safety_status_t safety_status;
        hal_status_t status = safety_manager_get_status(&safety_status);
        if (status != HAL_STATUS_OK) {
            printf("[IO] Failed to get safety status\n");
            return status;
        }
        
        if (!safety_status.safety_circuit_ok) {
            printf("[IO] Safety check failed, cannot enable\n");
            return HAL_STATUS_ERROR;
        }
    }
    
    handler->enabled = enable;
    
    if (enable) {
        handler->state = IO_STATE_IDLE;
        handler->fault_code = IO_FAULT_NONE;
        printf("[IO] Module enabled successfully\n");
    } else {
        handler->state = IO_STATE_IDLE;
        printf("[IO] Module disabled successfully\n");
    }
    
    return HAL_STATUS_OK;
}

// Digital I/O Operations
hal_status_t io_module_read_digital_input(io_module_handler_t *handler, uint8_t channel, bool *value) {
    if (handler == NULL || !handler->initialized || !handler->enabled || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_channel(handler, channel, false)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Read from Modbus register
    uint16_t reg_value;
    hal_status_t status = io_module_read_register(handler, IO_DIGITAL_INPUTS_REG + channel, &reg_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    *value = (reg_value != 0);
    handler->total_reads++;
    handler->last_operation_timestamp = hal_get_timestamp_ms();
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_write_digital_output(io_module_handler_t *handler, uint8_t channel, bool value) {
    if (handler == NULL || !handler->initialized || !handler->enabled) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_channel(handler, channel, false)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_digital_value(value)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Write to Modbus register
    uint16_t reg_value = value ? 1 : 0;
    hal_status_t status = io_module_write_register(handler, IO_DIGITAL_OUTPUTS_REG + channel, reg_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Update local data
    handler->data.digital_outputs[channel] = reg_value;
    handler->total_writes++;
    handler->last_operation_timestamp = hal_get_timestamp_ms();
    
    // Trigger event
    io_module_trigger_event(handler, IO_EVENT_DIGITAL_OUTPUT_CHANGED);
    
    return HAL_STATUS_OK;
}

// Analog I/O Operations
hal_status_t io_module_read_analog_input(io_module_handler_t *handler, uint8_t channel, uint16_t *value) {
    if (handler == NULL || !handler->initialized || !handler->enabled || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_channel(handler, channel, true)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Read from Modbus register
    hal_status_t status = io_module_read_register(handler, IO_ANALOG_INPUTS_REG + channel, value);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    handler->total_reads++;
    handler->last_operation_timestamp = hal_get_timestamp_ms();
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_write_analog_output(io_module_handler_t *handler, uint8_t channel, uint16_t value) {
    if (handler == NULL || !handler->initialized || !handler->enabled) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_channel(handler, channel, true)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_analog_value(value)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Write to Modbus register
    hal_status_t status = io_module_write_register(handler, IO_ANALOG_OUTPUTS_REG + channel, value);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Update local data
    handler->data.analog_outputs[channel] = value;
    handler->total_writes++;
    handler->last_operation_timestamp = hal_get_timestamp_ms();
    
    // Trigger event
    io_module_trigger_event(handler, IO_EVENT_ANALOG_OUTPUT_CHANGED);
    
    return HAL_STATUS_OK;
}

// Batch Operations
hal_status_t io_module_batch_read_digital(io_module_handler_t *handler, uint16_t *inputs, uint16_t *outputs) {
    if (handler == NULL || !handler->initialized || !handler->enabled) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (inputs == NULL || outputs == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Read digital inputs
    hal_status_t status = io_module_read_registers(handler, IO_DIGITAL_INPUTS_REG, 
                                                  handler->config.digital_input_count, inputs);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Read digital outputs
    status = io_module_read_registers(handler, IO_DIGITAL_OUTPUTS_REG, 
                                     handler->config.digital_output_count, outputs);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    handler->total_reads++;
    handler->last_operation_timestamp = hal_get_timestamp_ms();
    
    // Trigger event
    io_module_trigger_event(handler, IO_EVENT_BATCH_OPERATION_COMPLETE);
    
    return HAL_STATUS_OK;
}

// Debounce and Edge Detection
hal_status_t io_module_set_debounce(io_module_handler_t *handler, uint8_t channel, uint16_t debounce_ms) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_channel(handler, channel, false)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_debounce_time(debounce_ms)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->data.debounce_config[channel] = debounce_ms;
    
    // Write to Modbus register
    hal_status_t status = io_module_write_register(handler, IO_DEBOUNCE_CONFIG_REG + channel, debounce_ms);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_set_edge_detection(io_module_handler_t *handler, uint8_t channel, io_edge_type_t edge_type) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!io_module_validate_channel(handler, channel, false)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->data.edge_config[channel] = edge_type;
    
    // Write to Modbus register
    hal_status_t status = io_module_write_register(handler, IO_EDGE_CONFIG_REG + channel, edge_type);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    return HAL_STATUS_OK;
}

// Status and Monitoring
hal_status_t io_module_get_status(io_module_handler_t *handler, io_state_t *state, io_fault_code_t *fault_code) {
    if (handler == NULL || !handler->initialized || state == NULL || fault_code == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *state = handler->state;
    *fault_code = handler->fault_code;
    
    return HAL_STATUS_OK;
}

bool io_module_is_enabled(io_module_handler_t *handler) {
    return (handler != NULL && handler->initialized && handler->enabled);
}

bool io_module_has_faults(io_module_handler_t *handler) {
    return (handler != NULL && handler->initialized && handler->fault_code != IO_FAULT_NONE);
}

const char* io_module_get_fault_description(io_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return "Handler not initialized";
    }
    
    return io_module_get_fault_string(handler->fault_code);
}

hal_status_t io_module_reset_faults(io_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->fault_code = IO_FAULT_NONE;
    handler->data.fault_status = 0;
    handler->data.error_count = 0;
    
    // Write to Modbus register
    hal_status_t status = io_module_write_register(handler, IO_FAULT_STATUS_REG, 0);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Trigger event
    io_module_trigger_event(handler, IO_EVENT_FAULT_CLEARED);
    
    return HAL_STATUS_OK;
}

// Validation
hal_status_t io_module_validate_channel(io_module_handler_t *handler, uint8_t channel, bool is_analog) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (is_analog) {
        if (channel >= handler->config.analog_input_count && channel >= handler->config.analog_output_count) {
            return HAL_STATUS_INVALID_PARAMETER;
        }
    } else {
        if (channel >= handler->config.digital_input_count && channel >= handler->config.digital_output_count) {
            return HAL_STATUS_INVALID_PARAMETER;
        }
    }
    
    return HAL_STATUS_OK;
}

bool io_module_validate_digital_value(bool value) {
    // Digital values are always valid (true/false)
    return true;
}

bool io_module_validate_analog_value(uint16_t value) {
    return (value <= IO_MAX_ANALOG_VALUE);
}

bool io_module_validate_debounce_time(uint16_t debounce_ms) {
    return (debounce_ms >= 1 && debounce_ms <= IO_MAX_DEBOUNCE_TIME_MS);
}

bool io_module_validate_sample_rate(uint16_t sample_rate_hz) {
    return (sample_rate_hz >= 1 && sample_rate_hz <= IO_MAX_SAMPLE_RATE_HZ);
}

// Modbus Communication
hal_status_t io_module_read_register(io_module_handler_t *handler, uint16_t register_addr, uint16_t *value) {
    if (handler == NULL || !handler->initialized || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    comm_mgr_modbus_request_t request = {
        .slave_id = handler->address,
        .function_code = MODBUS_FC_READ_HOLDING_REGISTERS,
        .start_address = register_addr,
        .quantity = 1,
        .data = NULL
    };
    
    comm_mgr_modbus_response_t response;
    hal_status_t status = comm_manager_modbus_send_request(&request, &response);
    if (status != HAL_STATUS_OK) {
        printf("[IO] Modbus read failed for register 0x%04X\n", register_addr);
        return status;
    }
    
    if (response.data_length >= 2) {
        *value = (response.data[0] << 8) | response.data[1];
    } else {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_write_register(io_module_handler_t *handler, uint16_t register_addr, uint16_t value) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    uint8_t data[2] = {(uint8_t)(value >> 8), (uint8_t)(value & 0xFF)};
    
    comm_mgr_modbus_request_t request = {
        .slave_id = handler->address,
        .function_code = MODBUS_FC_WRITE_SINGLE_REGISTER,
        .start_address = register_addr,
        .quantity = 1,
        .data = data
    };
    
    comm_mgr_modbus_response_t response;
    hal_status_t status = comm_manager_modbus_send_request(&request, &response);
    if (status != HAL_STATUS_OK) {
        printf("[IO] Modbus write failed for register 0x%04X\n", register_addr);
        return status;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_read_registers(io_module_handler_t *handler, uint16_t start_register, uint16_t count, uint16_t *values) {
    if (handler == NULL || !handler->initialized || values == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    comm_mgr_modbus_request_t request = {
        .slave_id = handler->address,
        .function_code = MODBUS_FC_READ_HOLDING_REGISTERS,
        .start_address = start_register,
        .quantity = count,
        .data = NULL
    };
    
    comm_mgr_modbus_response_t response;
    hal_status_t status = comm_manager_modbus_send_request(&request, &response);
    if (status != HAL_STATUS_OK) {
        printf("[IO] Modbus read failed for registers 0x%04X-%0x%04X\n", start_register, start_register + count - 1);
        return status;
    }
    
    // Parse response data
    for (uint16_t i = 0; i < count && (i * 2 + 1) < response.data_length; i++) {
        values[i] = (response.data[i * 2] << 8) | response.data[i * 2 + 1];
    }
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_write_registers(io_module_handler_t *handler, uint16_t start_register, uint16_t count, const uint16_t *values) {
    if (handler == NULL || !handler->initialized || values == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Prepare data for write
    uint8_t data[count * 2];
    for (uint16_t i = 0; i < count; i++) {
        data[i * 2] = (uint8_t)(values[i] >> 8);
        data[i * 2 + 1] = (uint8_t)(values[i] & 0xFF);
    }
    
    comm_mgr_modbus_request_t request = {
        .slave_id = handler->address,
        .function_code = MODBUS_FC_WRITE_MULTIPLE_REGISTERS,
        .start_address = start_register,
        .quantity = count,
        .data = data
    };
    
    comm_mgr_modbus_response_t response;
    hal_status_t status = comm_manager_modbus_send_request(&request, &response);
    if (status != HAL_STATUS_OK) {
        printf("[IO] Modbus write failed for registers 0x%04X-%0x%04X\n", start_register, start_register + count - 1);
        return status;
    }
    
    return HAL_STATUS_OK;
}

// Event Handling
hal_status_t io_module_set_callback(io_module_handler_t *handler, 
                                   void (*callback)(io_module_handler_t *handler, io_event_t event)) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t io_module_trigger_event(io_module_handler_t *handler, io_event_t event) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return handle_io_event(handler, event);
}

// Diagnostics and Testing
hal_status_t io_module_get_diagnostics(io_module_handler_t *handler, char *info, size_t max_len) {
    if (handler == NULL || !handler->initialized || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(info, max_len,
             "IO Module Diagnostics:\n"
             "  Address: 0x%02X\n"
             "  State: %s\n"
             "  Enabled: %s\n"
             "  Fault Code: 0x%04X (%s)\n"
             "  Digital Inputs: %u\n"
             "  Digital Outputs: %u\n"
             "  Analog Inputs: %u\n"
             "  Analog Outputs: %u\n"
             "  Total Reads: %lu\n"
             "  Total Writes: %lu\n"
             "  Total Errors: %lu\n"
             "  Last Operation: %lu ms ago\n"
             "  Health Status: 0x%02X\n"
             "  Error Count: %u\n",
             handler->address,
             io_module_get_state_string(handler->state),
             handler->enabled ? "Yes" : "No",
             handler->fault_code,
             io_module_get_fault_string(handler->fault_code),
             handler->config.digital_input_count,
             handler->config.digital_output_count,
             handler->config.analog_input_count,
             handler->config.analog_output_count,
             (unsigned long)handler->total_reads,
             (unsigned long)handler->total_writes,
             (unsigned long)handler->total_errors,
             (unsigned long)(hal_get_timestamp_ms() - handler->last_operation_timestamp),
             handler->data.health_status,
             handler->data.error_count);
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_self_test(io_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    printf("[IO] Starting self-test...\n");
    
    // Test digital I/O
    for (uint8_t i = 0; i < handler->config.digital_output_count; i++) {
        bool test_value = (i % 2 == 0);
        hal_status_t status = io_module_write_digital_output(handler, i, test_value);
        if (status != HAL_STATUS_OK) {
            printf("[IO] Self-test failed: digital output %u\n", i);
            return status;
        }
    }
    
    // Test analog I/O
    for (uint8_t i = 0; i < handler->config.analog_output_count; i++) {
        uint16_t test_value = (i * 500) % IO_MAX_ANALOG_VALUE;
        hal_status_t status = io_module_write_analog_output(handler, i, test_value);
        if (status != HAL_STATUS_OK) {
            printf("[IO] Self-test failed: analog output %u\n", i);
            return status;
        }
    }
    
    // Test configuration
    for (uint8_t i = 0; i < handler->config.digital_input_count; i++) {
        hal_status_t status = io_module_set_debounce(handler, i, 20);
        if (status != HAL_STATUS_OK) {
            printf("[IO] Self-test failed: debounce config %u\n", i);
            return status;
        }
    }
    
    printf("[IO] Self-test completed successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t io_module_reset_statistics(io_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->total_reads = 0;
    handler->total_writes = 0;
    handler->total_errors = 0;
    handler->last_operation_timestamp = hal_get_timestamp_ms();
    
    return HAL_STATUS_OK;
}

hal_status_t io_module_get_statistics(io_module_handler_t *handler, uint32_t *total_reads, uint32_t *total_writes, uint32_t *total_errors) {
    if (handler == NULL || !handler->initialized || total_reads == NULL || total_writes == NULL || total_errors == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *total_reads = handler->total_reads;
    *total_writes = handler->total_writes;
    *total_errors = handler->total_errors;
    
    return HAL_STATUS_OK;
}

// Utility Functions
const char* io_module_get_state_string(io_state_t state) {
    switch (state) {
        case IO_STATE_IDLE: return "IDLE";
        case IO_STATE_READING: return "READING";
        case IO_STATE_WRITING: return "WRITING";
        case IO_STATE_CONFIGURING: return "CONFIGURING";
        case IO_STATE_FAULT: return "FAULT";
        case IO_STATE_RECOVERING: return "RECOVERING";
        default: return "UNKNOWN";
    }
}

const char* io_module_get_fault_string(io_fault_code_t fault_code) {
    switch (fault_code) {
        case IO_FAULT_NONE: return "No Fault";
        case IO_FAULT_COMMUNICATION_ERROR: return "Communication Error";
        case IO_FAULT_INVALID_CHANNEL: return "Invalid Channel";
        case IO_FAULT_INVALID_VALUE: return "Invalid Value";
        case IO_FAULT_CONFIGURATION_ERROR: return "Configuration Error";
        case IO_FAULT_HARDWARE_ERROR: return "Hardware Error";
        case IO_FAULT_TIMEOUT_ERROR: return "Timeout Error";
        case IO_FAULT_OVERVOLTAGE: return "Overvoltage";
        case IO_FAULT_UNDERVOLTAGE: return "Undervoltage";
        case IO_FAULT_OVERCURRENT: return "Overcurrent";
        case IO_FAULT_TEMPERATURE_HIGH: return "High Temperature";
        case IO_FAULT_SELF_TEST_FAILED: return "Self-Test Failed";
        default: return "Unknown Fault";
    }
}

const char* io_module_get_edge_string(io_edge_type_t edge_type) {
    switch (edge_type) {
        case IO_EDGE_NONE: return "None";
        case IO_EDGE_RISING: return "Rising";
        case IO_EDGE_FALLING: return "Falling";
        case IO_EDGE_BOTH: return "Both";
        default: return "Unknown";
    }
}

uint32_t io_module_get_timestamp(void) {
    return hal_get_timestamp_ms();
}

// Static function implementations
static hal_status_t handle_io_event(io_module_handler_t *handler, io_event_t event) {
    if (handler->event_callback != NULL) {
        handler->event_callback(handler, event);
    }
    return HAL_STATUS_OK;
}

static hal_status_t update_io_state(io_module_handler_t *handler) {
    // Simple state machine for I/O operations
    switch (handler->state) {
        case IO_STATE_IDLE:
            // Stay in idle state
            break;
        case IO_STATE_READING:
            handler->state = IO_STATE_IDLE;
            break;
        case IO_STATE_WRITING:
            handler->state = IO_STATE_IDLE;
            break;
        case IO_STATE_CONFIGURING:
            handler->state = IO_STATE_IDLE;
            break;
        case IO_STATE_FAULT:
            // Stay in fault state until reset
            break;
        case IO_STATE_RECOVERING:
            handler->state = IO_STATE_IDLE;
            break;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t check_io_faults(io_module_handler_t *handler) {
    // Check for communication faults
    if (handler->total_errors > 10) {
        handler->fault_code = IO_FAULT_COMMUNICATION_ERROR;
        handler->state = IO_STATE_FAULT;
        io_module_trigger_event(handler, IO_EVENT_FAULT_DETECTED);
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t validate_io_limits(io_module_handler_t *handler) {
    // Validate configuration limits
    if (handler->config.debounce_time_ms > IO_MAX_DEBOUNCE_TIME_MS) {
        handler->fault_code = IO_FAULT_CONFIGURATION_ERROR;
        return HAL_STATUS_ERROR;
    }
    
    if (handler->config.sample_rate_hz > IO_MAX_SAMPLE_RATE_HZ) {
        handler->fault_code = IO_FAULT_CONFIGURATION_ERROR;
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t simulate_io_operations(io_module_handler_t *handler) {
    // Simulate I/O operations for testing (when no real hardware)
    static uint32_t last_simulation = 0;
    uint32_t current_time = hal_get_timestamp_ms();
    
    if (current_time - last_simulation > 1000) { // Every second
        // Simulate digital input changes
        for (uint8_t i = 0; i < handler->config.digital_input_count; i++) {
            uint16_t old_value = handler->data.digital_inputs[i];
            handler->data.digital_inputs[i] = (current_time / 1000 + i) % 2;
            
            if (old_value != handler->data.digital_inputs[i]) {
                io_module_trigger_event(handler, IO_EVENT_DIGITAL_INPUT_CHANGED);
            }
        }
        
        // Simulate analog input changes
        for (uint8_t i = 0; i < handler->config.analog_input_count; i++) {
            handler->data.analog_inputs[i] = (current_time / 100 + i * 100) % IO_MAX_ANALOG_VALUE;
        }
        
        last_simulation = current_time;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t apply_debounce_logic(io_module_handler_t *handler, uint8_t channel) {
    if (channel >= IO_MAX_DIGITAL_CHANNELS) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Simple debounce implementation
    uint16_t debounce_time = handler->data.debounce_config[channel];
    if (debounce_time > 0) {
        // Shift history and add current value
        for (int i = 7; i > 0; i--) {
            handler->data.digital_input_history[channel][i] = handler->data.digital_input_history[channel][i-1];
        }
        handler->data.digital_input_history[channel][0] = handler->data.digital_inputs[channel];
        
        // Check if all values in history are the same
        uint16_t first_value = handler->data.digital_input_history[channel][0];
        bool all_same = true;
        for (int i = 1; i < 8; i++) {
            if (handler->data.digital_input_history[channel][i] != first_value) {
                all_same = false;
                break;
            }
        }
        
        if (all_same) {
            // Debounced value is stable
            handler->data.digital_inputs[channel] = first_value;
        }
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t check_edge_detection(io_module_handler_t *handler, uint8_t channel) {
    if (channel >= IO_MAX_DIGITAL_CHANNELS) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    io_edge_type_t edge_config = handler->data.edge_config[channel];
    if (edge_config == IO_EDGE_NONE) {
        return HAL_STATUS_OK;
    }
    
    // Check for edge detection (simplified implementation)
    static uint16_t previous_values[IO_MAX_DIGITAL_CHANNELS] = {0};
    uint16_t current_value = handler->data.digital_inputs[channel];
    
    if (previous_values[channel] != current_value) {
        bool rising_edge = (previous_values[channel] == 0 && current_value == 1);
        bool falling_edge = (previous_values[channel] == 1 && current_value == 0);
        
        if ((edge_config == IO_EDGE_RISING && rising_edge) ||
            (edge_config == IO_EDGE_FALLING && falling_edge) ||
            (edge_config == IO_EDGE_BOTH && (rising_edge || falling_edge))) {
            
            io_module_trigger_event(handler, IO_EVENT_EDGE_DETECTED);
        }
        
        previous_values[channel] = current_value;
    }
    
    return HAL_STATUS_OK;
}
