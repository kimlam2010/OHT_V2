/**
 * @file mock_estop.h
 * @brief Mock E-Stop HAL functions for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#ifndef MOCK_ESTOP_H
#define MOCK_ESTOP_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_estop.h"

// Mock E-Stop state
typedef struct {
    bool initialized;
    bool pin_status;
    estop_state_t state;
    estop_fault_t fault;
    estop_config_t config;
    estop_status_t status;
    uint64_t timestamp_us;
    uint32_t response_timeout_ms;
    uint32_t debounce_time_ms;
    bool auto_reset_enabled;
} mock_estop_state_t;

extern mock_estop_state_t mock_estop_state;

// Mock E-Stop functions
void mock_estop_reset(void);
void mock_estop_set_initialized(bool initialized);
void mock_estop_set_pin_status(bool pin_status);
void mock_estop_set_state(estop_state_t state);
void mock_estop_set_fault(estop_fault_t fault);
void mock_estop_set_config(const estop_config_t *config);
void mock_estop_set_status(const estop_status_t *status);
void mock_estop_set_timestamp_us(uint64_t timestamp);
void mock_estop_set_response_timeout_ms(uint32_t timeout);
void mock_estop_set_debounce_time_ms(uint32_t debounce);
void mock_estop_set_auto_reset_enabled(bool enabled);

// Mock validation functions
hal_status_t mock_estop_validate_config(const estop_config_t *config);

#endif // MOCK_ESTOP_H
