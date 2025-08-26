/**
 * @file mock_estop.c
 * @brief Mock E-Stop HAL functions implementation for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "mock_estop.h"
#include <string.h>

// Global mock E-Stop state
mock_estop_state_t mock_estop_state = {0};

void mock_estop_reset(void) {
    memset(&mock_estop_state, 0, sizeof(mock_estop_state_t));
    mock_estop_state.timestamp_us = 1000; // Start with positive timestamp
    mock_estop_state.state = ESTOP_STATE_SAFE;
    mock_estop_state.fault = ESTOP_FAULT_NONE;
    mock_estop_state.pin_status = true; // Default to safe (pin high)
}

void mock_estop_set_initialized(bool initialized) {
    mock_estop_state.initialized = initialized;
}

void mock_estop_set_pin_status(bool pin_status) {
    mock_estop_state.pin_status = pin_status;
}

void mock_estop_set_state(estop_state_t state) {
    mock_estop_state.state = state;
}

void mock_estop_set_fault(estop_fault_t fault) {
    mock_estop_state.fault = fault;
}

void mock_estop_set_config(const estop_config_t *config) {
    if (config != NULL) {
        mock_estop_state.config = *config;
    }
}

void mock_estop_set_status(const estop_status_t *status) {
    if (status != NULL) {
        mock_estop_state.status = *status;
    }
}

void mock_estop_set_timestamp_us(uint64_t timestamp) {
    mock_estop_state.timestamp_us = timestamp;
}

void mock_estop_set_response_timeout_ms(uint32_t timeout) {
    mock_estop_state.response_timeout_ms = timeout;
}

void mock_estop_set_debounce_time_ms(uint32_t debounce) {
    mock_estop_state.debounce_time_ms = debounce;
}

void mock_estop_set_auto_reset_enabled(bool enabled) {
    mock_estop_state.auto_reset_enabled = enabled;
}

// Mock validation functions
hal_status_t mock_estop_validate_config(const estop_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    if (config->pin >= 256) {
        return HAL_STATUS_ERROR;
    }
    
    if (config->response_timeout_ms == 0) {
        return HAL_STATUS_ERROR;
    }
    
    if (config->debounce_time_ms == 0) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}
