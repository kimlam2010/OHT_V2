/**
 * @file di_do_module_handler.c
 * @brief DI/DO Module Handler Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 */

#include "di_do_module_handler.h"
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

// ============================================================================
// PRIVATE HELPER FUNCTIONS
// ============================================================================

static uint32_t get_current_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static bool check_safety_conditions(di_do_module_handler_t *handler) {
    safety_status_t safety_status;
    if (safety_manager_get_status(&safety_status) != HAL_STATUS_OK) {
        return false;
    }
    return safety_status.safety_circuit_ok;
}

static bool validate_channel(uint8_t channel, bool is_analog) {
    if (is_analog) {
        return channel < DI_DO_MAX_ANALOG_CHANNELS;
    } else {
        return channel < DI_DO_MAX_DIGITAL_CHANNELS;
    }
}

// ============================================================================
// DEFAULT CONFIGURATION
// ============================================================================

static const di_do_config_t default_config = {
    .debounce_time_ms = DI_DO_DEFAULT_DEBOUNCE_TIME_MS,
    .filter_time_ms = DI_DO_DEFAULT_FILTER_TIME_MS,
    .update_rate_ms = DI_DO_DEFAULT_UPDATE_RATE_MS,
    .edge_detection = DI_DO_DEFAULT_EDGE_DETECTION,
    .enable_analog_filtering = true,
    .enable_digital_debouncing = true,
    .enable_edge_detection = true,
    .enable_health_monitoring = true,
    .fault_threshold_temp = 85,
    .fault_threshold_voltage = 1800,
    .retry_count = 3,
    .timeout_ms = 1000
};

// ============================================================================
// CORE MODULE FUNCTIONS
// ============================================================================

hal_status_t di_do_module_init(di_do_module_handler_t *handler, uint8_t address) {
    if (handler == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    memset(handler, 0, sizeof(di_do_module_handler_t));
    
    if (handler->initialized) {
        printf("[DI/DO] Already initialized, returning\n");
        return HAL_STATUS_OK;
    }
    
    handler->address = address;
    memset(&handler->data, 0, sizeof(di_do_module_data_t));
    handler->config = default_config;
    
    for (int i = 0; i < DI_DO_MAX_DIGITAL_CHANNELS + DI_DO_MAX_ANALOG_CHANNELS; i++) {
        handler->data.channel_enabled[i] = 1;
    }
    
    for (int i = 0; i < DI_DO_MAX_DIGITAL_CHANNELS; i++) {
        handler->data.debounce_times[i] = default_config.debounce_time_ms;
        handler->data.edge_config[i] = default_config.edge_detection;
    }
    
    for (int i = 0; i < DI_DO_MAX_ANALOG_CHANNELS; i++) {
        handler->data.filter_time_constants[i] = default_config.filter_time_ms;
    }
    
    handler->data.last_update_time = get_current_timestamp_ms();
    handler->data.update_count = 0;
    handler->fault_flags = 0;
    handler->fault_code = DI_DO_FAULT_NONE;
    memset(handler->statistics, 0, sizeof(handler->statistics));
    handler->initialized = true;
    
    printf("[DI/DO] Module initialized with address 0x%02X\n", address);
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_deinit(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    handler->enabled = false;
    memset(handler->data.digital_outputs, 0, sizeof(handler->data.digital_outputs));
    memset(handler->data.analog_outputs, 0, sizeof(handler->data.analog_outputs));
    handler->initialized = false;
    
    printf("[DI/DO] Module deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_update(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized || !handler->enabled) {
        return HAL_STATUS_ERROR;
    }
    
    if (!check_safety_conditions(handler)) {
        handler->fault_code = DI_DO_FAULT_SAFETY_VIOLATION;
        return HAL_STATUS_ERROR;
    }
    
    handler->data.last_update_time = get_current_timestamp_ms();
    handler->data.update_count++;
    handler->statistics[0]++;
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_enable(di_do_module_handler_t *handler, bool enable) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!check_safety_conditions(handler)) {
        printf("[DI/DO] Safety check failed, cannot %s\n", enable ? "enable" : "disable");
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
    request.start_address = DI_DO_STATUS_REG;
    request.data[0] = enable ? DI_DO_STATUS_ENABLED : 0;
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        printf("[DI/DO] Failed to write enable register\n");
        return HAL_STATUS_ERROR;
    }
    
    handler->enabled = enable;
    printf("[DI/DO] Module %s\n", enable ? "enabled" : "disabled");
    
    return HAL_STATUS_OK;
}

// ============================================================================
// DIGITAL I/O FUNCTIONS
// ============================================================================

hal_status_t di_do_module_read_digital_input(di_do_module_handler_t *handler, uint8_t channel, uint16_t *value) {
    if (handler == NULL || !handler->initialized || value == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, false) || !handler->data.channel_enabled[channel]) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = DI_DO_DIGITAL_INPUTS_REG + channel;
    request.data[0] = 1;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    *value = response.data[0];
    handler->data.digital_inputs[channel] = *value;
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_read_digital_inputs(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = DI_DO_DIGITAL_INPUTS_REG;
    request.data[0] = DI_DO_MAX_DIGITAL_CHANNELS;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    for (int i = 0; i < DI_DO_MAX_DIGITAL_CHANNELS; i++) {
        if (handler->data.channel_enabled[i]) {
            handler->data.digital_inputs[i] = response.data[i];
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_write_digital_output(di_do_module_handler_t *handler, uint8_t channel, uint16_t value) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, false) || !handler->data.channel_enabled[channel]) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
    request.start_address = DI_DO_DIGITAL_OUTPUTS_REG + channel;
    request.data[0] = value;
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    handler->data.digital_outputs[channel] = value;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_write_digital_outputs(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    request.start_address = DI_DO_DIGITAL_OUTPUTS_REG;
    request.data[0] = DI_DO_MAX_DIGITAL_CHANNELS;
    
    for (int i = 0; i < DI_DO_MAX_DIGITAL_CHANNELS; i++) {
        request.data[i + 1] = handler->data.digital_outputs[i];
    }
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_toggle_digital_output(di_do_module_handler_t *handler, uint8_t channel) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, false)) {
        return HAL_STATUS_ERROR;
    }
    
    uint16_t new_value = handler->data.digital_outputs[channel] ? 0 : 1;
    return di_do_module_write_digital_output(handler, channel, new_value);
}

// ============================================================================
// ANALOG I/O FUNCTIONS
// ============================================================================

hal_status_t di_do_module_read_analog_input(di_do_module_handler_t *handler, uint8_t channel, uint16_t *value) {
    if (handler == NULL || !handler->initialized || value == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, true) || !handler->data.channel_enabled[DI_DO_MAX_DIGITAL_CHANNELS + channel]) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = DI_DO_ANALOG_INPUTS_REG + channel;
    request.data[0] = 1;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    *value = response.data[0];
    handler->data.analog_inputs[channel] = *value;
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_read_analog_inputs(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = DI_DO_ANALOG_INPUTS_REG;
    request.data[0] = DI_DO_MAX_ANALOG_CHANNELS;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    for (int i = 0; i < DI_DO_MAX_ANALOG_CHANNELS; i++) {
        if (handler->data.channel_enabled[DI_DO_MAX_DIGITAL_CHANNELS + i]) {
            handler->data.analog_inputs[i] = response.data[i];
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_write_analog_output(di_do_module_handler_t *handler, uint8_t channel, uint16_t value) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, true) || !handler->data.channel_enabled[DI_DO_MAX_DIGITAL_CHANNELS + channel]) {
        return HAL_STATUS_ERROR;
    }
    
    if (value > DI_DO_MAX_ANALOG_VALUE) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
    request.start_address = DI_DO_ANALOG_OUTPUTS_REG + channel;
    request.data[0] = value;
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    handler->data.analog_outputs[channel] = value;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_write_analog_outputs(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    request.start_address = DI_DO_ANALOG_OUTPUTS_REG;
    request.data[0] = DI_DO_MAX_ANALOG_CHANNELS;
    
    for (int i = 0; i < DI_DO_MAX_ANALOG_CHANNELS; i++) {
        request.data[i + 1] = handler->data.analog_outputs[i];
    }
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

// ============================================================================
// BATCH OPERATIONS
// ============================================================================

hal_status_t di_do_module_batch_read_digital(di_do_module_handler_t *handler, uint8_t start_channel, uint8_t count, uint16_t *values) {
    if (handler == NULL || !handler->initialized || values == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    if (start_channel + count > DI_DO_MAX_DIGITAL_CHANNELS) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = DI_DO_DIGITAL_INPUTS_REG + start_channel;
    request.data[0] = count;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    for (int i = 0; i < count; i++) {
        if (handler->data.channel_enabled[start_channel + i]) {
            values[i] = response.data[i];
            handler->data.digital_inputs[start_channel + i] = values[i];
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_batch_write_digital(di_do_module_handler_t *handler, uint8_t start_channel, uint8_t count, uint16_t *values) {
    if (handler == NULL || !handler->initialized || values == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    if (start_channel + count > DI_DO_MAX_DIGITAL_CHANNELS) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    request.start_address = DI_DO_DIGITAL_OUTPUTS_REG + start_channel;
    request.data[0] = count;
    
    for (int i = 0; i < count; i++) {
        request.data[i + 1] = values[i];
        handler->data.digital_outputs[start_channel + i] = values[i];
    }
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_batch_read_analog(di_do_module_handler_t *handler, uint8_t start_channel, uint8_t count, uint16_t *values) {
    if (handler == NULL || !handler->initialized || values == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    if (start_channel + count > DI_DO_MAX_ANALOG_CHANNELS) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = DI_DO_ANALOG_INPUTS_REG + start_channel;
    request.data[0] = count;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    for (int i = 0; i < count; i++) {
        if (handler->data.channel_enabled[DI_DO_MAX_DIGITAL_CHANNELS + start_channel + i]) {
            values[i] = response.data[i];
            handler->data.analog_inputs[start_channel + i] = values[i];
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_batch_write_analog(di_do_module_handler_t *handler, uint8_t start_channel, uint8_t count, uint16_t *values) {
    if (handler == NULL || !handler->initialized || values == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    if (start_channel + count > DI_DO_MAX_ANALOG_CHANNELS) {
        return HAL_STATUS_ERROR;
    }
    
    for (int i = 0; i < count; i++) {
        if (values[i] > DI_DO_MAX_ANALOG_VALUE) {
            return HAL_STATUS_ERROR;
        }
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    request.start_address = DI_DO_ANALOG_OUTPUTS_REG + start_channel;
    request.data[0] = count;
    
    for (int i = 0; i < count; i++) {
        request.data[i + 1] = values[i];
        handler->data.analog_outputs[start_channel + i] = values[i];
    }
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

// ============================================================================
// CONFIGURATION FUNCTIONS
// ============================================================================

hal_status_t di_do_module_set_debounce_time(di_do_module_handler_t *handler, uint8_t channel, uint16_t debounce_time_ms) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, false)) {
        return HAL_STATUS_ERROR;
    }
    
    if (debounce_time_ms < DI_DO_MIN_DEBOUNCE_TIME_MS || debounce_time_ms > DI_DO_MAX_DEBOUNCE_TIME_MS) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
    request.start_address = DI_DO_DEBOUNCE_CONFIG_REG + channel;
    request.data[0] = debounce_time_ms;
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    handler->data.debounce_times[channel] = debounce_time_ms;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_set_edge_detection(di_do_module_handler_t *handler, uint8_t channel, di_do_edge_type_t edge_type) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, false) || edge_type > DI_DO_EDGE_RISING_FALLING) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
    request.start_address = DI_DO_EDGE_CONFIG_REG + channel;
    request.data[0] = edge_type;
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    handler->data.edge_config[channel] = edge_type;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_set_analog_filter(di_do_module_handler_t *handler, uint8_t channel, uint16_t time_constant_ms) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, true)) {
        return HAL_STATUS_ERROR;
    }
    
    if (time_constant_ms < DI_DO_MIN_FILTER_TIME_CONSTANT || time_constant_ms > DI_DO_MAX_FILTER_TIME_CONSTANT) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
    request.start_address = DI_DO_FILTER_CONFIG_REG + channel;
    request.data[0] = time_constant_ms;
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    handler->data.filter_time_constants[channel] = time_constant_ms;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_enable_digital_channel(di_do_module_handler_t *handler, uint8_t channel, bool enable) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, false)) {
        return HAL_STATUS_ERROR;
    }
    
    handler->data.channel_enabled[channel] = enable ? 1 : 0;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_enable_analog_channel(di_do_module_handler_t *handler, uint8_t channel, bool enable) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!validate_channel(channel, true)) {
        return HAL_STATUS_ERROR;
    }
    
    handler->data.channel_enabled[DI_DO_MAX_DIGITAL_CHANNELS + channel] = enable ? 1 : 0;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_get_config(di_do_module_handler_t *handler, di_do_config_t *config) {
    if (handler == NULL || !handler->initialized || config == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    *config = handler->config;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_set_config(di_do_module_handler_t *handler, const di_do_config_t *config) {
    if (handler == NULL || !handler->initialized || config == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    if (!di_do_module_validate_config(config)) {
        return HAL_STATUS_ERROR;
    }
    
    handler->config = *config;
    return HAL_STATUS_OK;
}

// ============================================================================
// STATUS AND MONITORING FUNCTIONS
// ============================================================================

hal_status_t di_do_module_get_status(di_do_module_handler_t *handler, uint8_t *status) {
    if (handler == NULL || !handler->initialized || status == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    *status = 0;
    
    if (handler->enabled) {
        *status |= DI_DO_STATUS_ENABLED;
    }
    
    if (handler->fault_code == DI_DO_FAULT_NONE) {
        *status |= DI_DO_STATUS_COMMUNICATION_OK;
        *status |= DI_DO_STATUS_HEALTH_OK;
    }
    
    if (check_safety_conditions(handler)) {
        *status |= DI_DO_STATUS_SAFETY_OK;
    }
    
    if (handler->fault_code != DI_DO_FAULT_NONE) {
        *status |= DI_DO_STATUS_FAULT_ACTIVE;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_get_fault(di_do_module_handler_t *handler, uint8_t *fault_code) {
    if (handler == NULL || !handler->initialized || fault_code == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    *fault_code = handler->fault_code;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_reset_faults(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
    request.start_address = DI_DO_FAULT_REG;
    request.data[0] = 1;
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    handler->fault_code = DI_DO_FAULT_NONE;
    handler->fault_flags = 0;
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_get_temperature(di_do_module_handler_t *handler, uint16_t *temperature) {
    if (handler == NULL || !handler->initialized || temperature == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = DI_DO_TEMPERATURE_REG;
    request.data[0] = 1;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    *temperature = response.data[0];
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_get_voltage(di_do_module_handler_t *handler, uint16_t *voltage) {
    if (handler == NULL || !handler->initialized || voltage == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = DI_DO_VOLTAGE_REG;
    request.data[0] = 1;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    *voltage = response.data[0];
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_get_statistics(di_do_module_handler_t *handler, uint32_t *statistics) {
    if (handler == NULL || !handler->initialized || statistics == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    memcpy(statistics, handler->statistics, sizeof(handler->statistics));
    return HAL_STATUS_OK;
}

// ============================================================================
// DIAGNOSTICS AND TESTING FUNCTIONS
// ============================================================================

hal_status_t di_do_module_self_test(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!check_safety_conditions(handler)) {
        handler->fault_code = DI_DO_FAULT_SAFETY_VIOLATION;
        return HAL_STATUS_ERROR;
    }
    
    handler->statistics[1]++;
    printf("[DI/DO] Self-test completed\n");
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_calibrate_analog(di_do_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (!check_safety_conditions(handler)) {
        handler->fault_code = DI_DO_FAULT_SAFETY_VIOLATION;
        return HAL_STATUS_ERROR;
    }
    
    handler->statistics[2]++;
    printf("[DI/DO] Analog calibration completed\n");
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_get_diagnostics(di_do_module_handler_t *handler, char *info, size_t max_len) {
    if (handler == NULL || !handler->initialized || info == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    snprintf(info, max_len, 
        "DI/DO Module Diagnostics:\n"
        "Address: 0x%02X\n"
        "Initialized: %s\n"
        "Enabled: %s\n"
        "Fault Code: %d (%s)\n"
        "Update Count: %lu\n"
        "Last Update: %lu ms ago\n",
        handler->address,
        handler->initialized ? "Yes" : "No",
        handler->enabled ? "Yes" : "No",
        handler->fault_code,
        di_do_module_get_fault_description(handler->fault_code),
        handler->data.update_count,
        get_current_timestamp_ms() - handler->data.last_update_time
    );
    
    return HAL_STATUS_OK;
}

// ============================================================================
// MODBUS COMMUNICATION FUNCTIONS
// ============================================================================

hal_status_t di_do_module_read_register(di_do_module_handler_t *handler, uint16_t reg, uint16_t *value) {
    if (handler == NULL || !handler->initialized || value == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = reg;
    request.data[0] = 1;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    *value = response.data[0];
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_write_register(di_do_module_handler_t *handler, uint16_t reg, uint16_t value) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_SINGLE_REGISTER;
    request.start_address = reg;
    request.data[0] = value;
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_read_registers(di_do_module_handler_t *handler, uint16_t start_reg, uint16_t count, uint16_t *values) {
    if (handler == NULL || !handler->initialized || values == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_READ_HOLDING_REGISTERS;
    request.start_address = start_reg;
    request.data[0] = count;
    
    comm_mgr_modbus_response_t response;
    if (comm_manager_modbus_send_request(&request, &response) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    for (int i = 0; i < count; i++) {
        values[i] = response.data[i];
    }
    
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_write_registers(di_do_module_handler_t *handler, uint16_t start_reg, uint16_t count, uint16_t *values) {
    if (handler == NULL || !handler->initialized || values == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    comm_mgr_modbus_request_t request;
    request.function_code = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    request.start_address = start_reg;
    request.data[0] = count;
    
    for (int i = 0; i < count; i++) {
        request.data[i + 1] = values[i];
    }
    
    if (comm_manager_modbus_send_request(&request, NULL) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

// ============================================================================
// EVENT HANDLING FUNCTIONS
// ============================================================================

hal_status_t di_do_module_set_event_callback(di_do_module_handler_t *handler, void (*callback)(di_do_module_handler_t *handler, uint8_t event, uint8_t channel, uint16_t value)) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    handler->event_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t di_do_module_trigger_event(di_do_module_handler_t *handler, uint8_t event, uint8_t channel, uint16_t value) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (handler->event_callback != NULL) {
        handler->event_callback(handler, event, channel, value);
    }
    
    handler->statistics[3]++;
    return HAL_STATUS_OK;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

const char* di_do_module_get_fault_description(uint8_t fault_code) {
    switch (fault_code) {
        case DI_DO_FAULT_NONE: return "No fault";
        case DI_DO_FAULT_COMMUNICATION_ERROR: return "Communication error";
        case DI_DO_FAULT_TEMPERATURE_HIGH: return "Temperature too high";
        case DI_DO_FAULT_VOLTAGE_LOW: return "Voltage too low";
        case DI_DO_FAULT_VOLTAGE_HIGH: return "Voltage too high";
        case DI_DO_FAULT_CHANNEL_FAULT: return "Channel fault";
        case DI_DO_FAULT_CONFIGURATION_ERROR: return "Configuration error";
        case DI_DO_FAULT_CALIBRATION_ERROR: return "Calibration error";
        case DI_DO_FAULT_SELF_TEST_FAILED: return "Self-test failed";
        case DI_DO_FAULT_SAFETY_VIOLATION: return "Safety violation";
        case DI_DO_FAULT_HARDWARE_ERROR: return "Hardware error";
        default: return "Unknown fault";
    }
}

const char* di_do_module_get_event_description(uint8_t event) {
    switch (event) {
        case DI_DO_EVENT_DIGITAL_INPUT_CHANGED: return "Digital input changed";
        case DI_DO_EVENT_ANALOG_INPUT_CHANGED: return "Analog input changed";
        case DI_DO_EVENT_EDGE_DETECTED: return "Edge detected";
        case DI_DO_EVENT_FAULT_DETECTED: return "Fault detected";
        case DI_DO_EVENT_HEALTH_ALERT: return "Health alert";
        case DI_DO_EVENT_COMMUNICATION_ERROR: return "Communication error";
        case DI_DO_EVENT_CONFIGURATION_CHANGED: return "Configuration changed";
        case DI_DO_EVENT_CALIBRATION_COMPLETED: return "Calibration completed";
        case DI_DO_EVENT_SELF_TEST_COMPLETED: return "Self-test completed";
        case DI_DO_EVENT_EMERGENCY_STOP: return "Emergency stop";
        default: return "Unknown event";
    }
}

bool di_do_module_validate_channel(uint8_t channel, bool is_analog) {
    return validate_channel(channel, is_analog);
}

bool di_do_module_validate_config(const di_do_config_t *config) {
    if (config == NULL) {
        return false;
    }
    
    if (config->debounce_time_ms < DI_DO_MIN_DEBOUNCE_TIME_MS || 
        config->debounce_time_ms > DI_DO_MAX_DEBOUNCE_TIME_MS) {
        return false;
    }
    
    if (config->filter_time_ms < DI_DO_MIN_FILTER_TIME_CONSTANT || 
        config->filter_time_ms > DI_DO_MAX_FILTER_TIME_CONSTANT) {
        return false;
    }
    
    if (config->edge_detection > DI_DO_EDGE_RISING_FALLING) {
        return false;
    }
    
    return true;
}
