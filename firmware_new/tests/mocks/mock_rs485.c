/**
 * @file mock_rs485.c
 * @brief Mock RS485 HAL functions implementation for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "mock_rs485.h"
#include <string.h>

// Global mock RS485 state
mock_rs485_state_t mock_rs485_state = {0};

void mock_rs485_reset(void) {
    memset(&mock_rs485_state, 0, sizeof(mock_rs485_state_t));
    mock_rs485_state.timestamp_us = 1000; // Start with positive timestamp
}

void mock_rs485_set_initialized(bool initialized) {
    mock_rs485_state.initialized = initialized;
}

void mock_rs485_set_device_open(bool device_open) {
    mock_rs485_state.device_open = device_open;
}

void mock_rs485_set_config(const rs485_config_t *config) {
    if (config != NULL) {
        mock_rs485_state.config = *config;
    }
}

void mock_rs485_set_modbus_config(const modbus_config_t *config) {
    if (config != NULL) {
        mock_rs485_state.modbus_config = *config;
    }
}

void mock_rs485_set_statistics(const rs485_statistics_t *stats) {
    if (stats != NULL) {
        mock_rs485_state.statistics = *stats;
    }
}

void mock_rs485_set_device_info(const rs485_device_info_t *info) {
    if (info != NULL) {
        mock_rs485_state.device_info = *info;
    }
}

void mock_rs485_set_timestamp_us(uint64_t timestamp) {
    mock_rs485_state.timestamp_us = timestamp;
}

void mock_rs485_set_retry_count(uint32_t retry_count) {
    mock_rs485_state.retry_count = retry_count;
}

void mock_rs485_set_max_retries(uint32_t max_retries) {
    mock_rs485_state.max_retries = max_retries;
}

void mock_rs485_set_retry_delay_ms(uint32_t delay_ms) {
    mock_rs485_state.retry_delay_ms = delay_ms;
}

// Mock validation functions
hal_status_t mock_rs485_validate_config(const rs485_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (strlen(config->device_path) == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->baud_rate == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->data_bits < 5 || config->data_bits > 8) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->stop_bits < 1 || config->stop_bits > 2) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->parity > 2) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->timeout_ms == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->retry_count == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t mock_rs485_validate_modbus_config(const modbus_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->slave_id == 0 || config->slave_id > 247) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->timeout_ms == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->retry_count == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}
