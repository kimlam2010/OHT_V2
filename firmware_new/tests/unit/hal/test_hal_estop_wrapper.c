/**
 * @file test_hal_estop_wrapper.c
 * @brief Test wrapper for HAL E-Stop with mock functions
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "hal_estop.h"
#include "mock_estop.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Override E-Stop functions with mock implementations for testing

hal_status_t hal_estop_init(const estop_config_t *config) {
    if (mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (config == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    // Validate configuration using mock validation
    hal_status_t status = mock_estop_validate_config(config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Store configuration
    mock_estop_set_config(config);
    mock_estop_set_initialized(true);
    mock_estop_set_state(ESTOP_STATE_SAFE);
    mock_estop_set_fault(ESTOP_FAULT_NONE);
    mock_estop_set_pin_status(true); // Default to safe
    
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_deinit(void) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_estop_set_initialized(false);
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_status(estop_status_t *status) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (status == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    // Update status based on current mock state
    status->state = mock_estop_state.state;
    status->fault = mock_estop_state.fault;
    status->pin_status = mock_estop_state.pin_status;
    status->last_trigger_time = 0;
    status->last_reset_time = 0;
    status->trigger_count = 0;
    status->fault_count = 0;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_test_pin(bool *pin_status) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (pin_status == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    // Return current pin status
    *pin_status = mock_estop_state.pin_status;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_reset(void) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    mock_estop_set_state(ESTOP_STATE_SAFE);
    mock_estop_set_fault(ESTOP_FAULT_NONE);
    mock_estop_set_pin_status(true);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_device_info(hal_device_info_t *device_info) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (device_info == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    // Fill device info with mock data
    device_info->device_type = HAL_DEVICE_TYPE_ESTOP;
    device_info->status = HAL_DEVICE_STATUS_OK;
    device_info->device_id = 0;
    strcpy(device_info->device_name, "Mock_EStop");
    strcpy(device_info->device_version, "1.0.0");
    device_info->timestamp_us = mock_estop_state.timestamp_us;
    device_info->error_count = 0;
    device_info->warning_count = 0;
    
    return HAL_STATUS_OK;
}

// Mock validation functions that are called by the real implementation
hal_status_t estop_validate_config(const estop_config_t *config) {
    return mock_estop_validate_config(config);
}

// Additional E-Stop functions needed by tests
hal_status_t hal_estop_get_config(estop_config_t *config) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (config == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    *config = mock_estop_state.config;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_is_triggered(bool *triggered) {
    if (!mock_estop_state.initialized) { return HAL_STATUS_ERROR; }
    if (triggered == NULL) { return HAL_STATUS_ERROR; }
    *triggered = false; // Mock E-Stop is never triggered by default
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_pin_status(bool *pin_status) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (pin_status == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    *pin_status = mock_estop_state.pin_status;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_set_callback(estop_event_callback_t callback) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock implementation - just return success
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_validate_safety(void) {
    if (!mock_estop_state.initialized) { return HAL_STATUS_ERROR; }
    return HAL_STATUS_OK; // Mock E-Stop always validates safety
}

hal_status_t hal_estop_check_safety_compliance(bool *compliant) {
    if (!mock_estop_state.initialized) { return HAL_STATUS_ERROR; }
    if (compliant == NULL) { return HAL_STATUS_ERROR; }
    *compliant = true; // Mock E-Stop always compliant
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_self_test(void) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock self-test always passes
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_diagnostics(char *info, size_t max_len) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (info == NULL || max_len == 0) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock implementation - return mock diagnostic info
    snprintf(info, max_len, "Mock E-Stop Diagnostics: State=%d, Fault=%d, Pin=%s", 
             mock_estop_state.state, mock_estop_state.fault, 
             mock_estop_state.pin_status ? "HIGH" : "LOW");
    
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_validate_hardware(void) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock hardware validation always passes
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_trigger_count(uint32_t *trigger_count) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (trigger_count == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    *trigger_count = 0; // Mock trigger count
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_fault_count(uint32_t *fault_count) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (fault_count == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    *fault_count = 0; // Mock fault count
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_reset_statistics(void) {
    if (!mock_estop_state.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Mock statistics reset
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_update(void) {
    if (!mock_estop_state.initialized) { return HAL_STATUS_ERROR; }
    
    // Update mock timestamp
    mock_estop_state.timestamp_us += 1000;
    return HAL_STATUS_OK;
}
