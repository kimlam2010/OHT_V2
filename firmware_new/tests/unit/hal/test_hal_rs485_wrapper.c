/**
 * @file test_hal_rs485_wrapper.c
 * @brief Test wrapper for HAL RS485 with mock functions
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "hal_rs485.h"
#include "mock_rs485.h"
#include <stdio.h>
#include <string.h>

// Override RS485 functions with mock implementations for testing

hal_status_t hal_rs485_init(const rs485_config_t *config) {
    if (!mock_rs485_state.initialized) {
        if (config == NULL) {
            return HAL_STATUS_INVALID_PARAMETER;
        }
        
        // Validate configuration using mock validation
        hal_status_t status = mock_rs485_validate_config(config);
        if (status != HAL_STATUS_OK) {
            return status;
        }
        
        // Store configuration
        mock_rs485_set_config(config);
        mock_rs485_set_initialized(true);
        mock_rs485_set_device_open(false);
        
        // Initialize device info
        mock_rs485_state.device_info.device_type = HAL_DEVICE_TYPE_UART;
        mock_rs485_state.device_info.status = HAL_DEVICE_STATUS_OK;
        mock_rs485_state.device_info.rs485_status = RS485_STATUS_IDLE;
        strcpy(mock_rs485_state.device_info.device_name, "Mock_RS485_UART1");
        strcpy(mock_rs485_state.device_info.device_version, "1.0.0");
        mock_rs485_state.device_info.timestamp_us = mock_rs485_state.timestamp_us;
        mock_rs485_state.device_info.error_count = 0;
        mock_rs485_state.device_info.warning_count = 0;
        
        // Initialize statistics
        memset(&mock_rs485_state.statistics, 0, sizeof(rs485_statistics_t));
        mock_rs485_state.statistics.timestamp_us = mock_rs485_state.timestamp_us;
        
        return HAL_STATUS_OK;
    }
    
    return HAL_STATUS_ALREADY_INITIALIZED;
}

hal_status_t hal_rs485_deinit(void) {
    if (mock_rs485_state.initialized) {
        mock_rs485_set_initialized(false);
        mock_rs485_set_device_open(false);
        return HAL_STATUS_OK;
    }
    
    return HAL_STATUS_NOT_INITIALIZED;
}

hal_status_t hal_rs485_open_device(void) {
    if (!mock_rs485_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (mock_rs485_state.device_open) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    mock_rs485_set_device_open(true);
    return HAL_STATUS_OK;
}

hal_status_t hal_rs485_close_device(void) {
    if (!mock_rs485_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    mock_rs485_set_device_open(false);
    return HAL_STATUS_OK;
}

hal_status_t hal_rs485_get_device_info(rs485_device_info_t *device_info) {
    if (!mock_rs485_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (device_info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *device_info = mock_rs485_state.device_info;
    return HAL_STATUS_OK;
}

hal_status_t hal_rs485_get_statistics(rs485_statistics_t *statistics) {
    if (!mock_rs485_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (statistics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *statistics = mock_rs485_state.statistics;
    return HAL_STATUS_OK;
}

hal_status_t hal_rs485_reset_statistics(void) {
    if (!mock_rs485_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&mock_rs485_state.statistics, 0, sizeof(rs485_statistics_t));
    mock_rs485_state.statistics.timestamp_us = mock_rs485_state.timestamp_us;
    
    return HAL_STATUS_OK;
}

// Mock validation functions that are called by the real implementation
hal_status_t rs485_validate_config(const rs485_config_t *config) {
    return mock_rs485_validate_config(config);
}

hal_status_t rs485_validate_modbus_config(const modbus_config_t *config) {
    return mock_rs485_validate_modbus_config(config);
}
