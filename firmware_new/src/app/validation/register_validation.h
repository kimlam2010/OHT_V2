/**
 * @file register_validation.h
 * @brief Register validation header for issue #135 fix
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task Fix RS485/Modbus data validation issues
 */

#ifndef REGISTER_VALIDATION_H
#define REGISTER_VALIDATION_H

#include <stdint.h>
#include <stdbool.h>
#include "register_map.h"

// Function prototypes
bool register_validation_validate_read_request(uint8_t module_addr, uint16_t start_addr, uint16_t quantity);
bool register_validation_validate_data(uint8_t module_addr, uint16_t start_addr, const uint16_t *data, uint16_t quantity);
bool register_validation_get_recommended_params(uint8_t module_addr, uint16_t *start_addr, uint16_t *quantity);

// Data range validation functions
bool validate_power_data_ranges(uint16_t start_addr, const uint16_t *data, uint16_t quantity);
bool validate_motor_data_ranges(uint16_t start_addr, const uint16_t *data, uint16_t quantity);
bool validate_safety_data_ranges(uint16_t start_addr, const uint16_t *data, uint16_t quantity);
bool validate_dock_data_ranges(uint16_t start_addr, const uint16_t *data, uint16_t quantity);

#endif // REGISTER_VALIDATION_H
