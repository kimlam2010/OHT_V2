/**
 * @file mock_rs485.h
 * @brief Mock RS485 HAL functions for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#ifndef MOCK_RS485_H
#define MOCK_RS485_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_rs485.h"

// Mock RS485 state
typedef struct {
    bool initialized;
    bool device_open;
    rs485_config_t config;
    modbus_config_t modbus_config;
    rs485_statistics_t statistics;
    rs485_device_info_t device_info;
    uint64_t timestamp_us;
    uint32_t retry_count;
    uint32_t max_retries;
    uint32_t retry_delay_ms;
} mock_rs485_state_t;

extern mock_rs485_state_t mock_rs485_state;

// Mock RS485 functions
void mock_rs485_reset(void);
void mock_rs485_set_initialized(bool initialized);
void mock_rs485_set_device_open(bool device_open);
void mock_rs485_set_config(const rs485_config_t *config);
void mock_rs485_set_modbus_config(const modbus_config_t *config);
void mock_rs485_set_statistics(const rs485_statistics_t *stats);
void mock_rs485_set_device_info(const rs485_device_info_t *info);
void mock_rs485_set_timestamp_us(uint64_t timestamp);
void mock_rs485_set_retry_count(uint32_t retry_count);
void mock_rs485_set_max_retries(uint32_t max_retries);
void mock_rs485_set_retry_delay_ms(uint32_t delay_ms);

// Mock validation functions
hal_status_t mock_rs485_validate_config(const rs485_config_t *config);
hal_status_t mock_rs485_validate_modbus_config(const modbus_config_t *config);

#endif // MOCK_RS485_H
