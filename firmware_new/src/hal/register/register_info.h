/**
 * @file register_info.h
 * @brief Register Information Header for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task Issue #179 - Register Address and Mode Support
 */

#ifndef REGISTER_INFO_H
#define REGISTER_INFO_H

#include "register_map.h"

// ============================================================================
// REGISTER INFO FUNCTIONS DECLARATIONS
// ============================================================================

/**
 * @brief Get register information for a specific module and register address
 * @param module_addr Module address (MODULE_ADDR_*)
 * @param register_addr Register address
 * @return Pointer to register_info_t structure, NULL if not found
 */
const register_info_t* get_register_info(uint8_t module_addr, uint16_t register_addr);

/**
 * @brief Validate register access based on mode and user access level
 * @param module_addr Module address
 * @param register_addr Register address
 * @param access_mode Requested access mode (REG_MODE_*)
 * @param user_access_level User access level (REG_ACCESS_*)
 * @return true if access is allowed, false otherwise
 */
bool validate_register_access(uint8_t module_addr, uint16_t register_addr, uint8_t access_mode, uint8_t user_access_level);

/**
 * @brief Check if register is safety critical
 * @param module_addr Module address
 * @param register_addr Register address
 * @return true if safety critical, false otherwise
 */
bool is_register_safe_critical(uint8_t module_addr, uint16_t register_addr);

/**
 * @brief Get register description
 * @param module_addr Module address
 * @param register_addr Register address
 * @return Register description string
 */
const char* get_register_description(uint8_t module_addr, uint16_t register_addr);

/**
 * @brief Get register unit of measurement
 * @param module_addr Module address
 * @param register_addr Register address
 * @return Register unit string
 */
const char* get_register_unit(uint8_t module_addr, uint16_t register_addr);

/**
 * @brief Get register minimum value
 * @param module_addr Module address
 * @param register_addr Register address
 * @return Minimum allowed value
 */
uint16_t get_register_min_value(uint8_t module_addr, uint16_t register_addr);

/**
 * @brief Get register maximum value
 * @param module_addr Module address
 * @param register_addr Register address
 * @return Maximum allowed value
 */
uint16_t get_register_max_value(uint8_t module_addr, uint16_t register_addr);

/**
 * @brief Get register default value
 * @param module_addr Module address
 * @param register_addr Register address
 * @return Default value
 */
uint16_t get_register_default_value(uint8_t module_addr, uint16_t register_addr);

/**
 * @brief Get register data type
 * @param module_addr Module address
 * @param register_addr Register address
 * @return Register data type (REG_DATA_TYPE_*)
 */
uint8_t get_register_data_type(uint8_t module_addr, uint16_t register_addr);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Get all registers for a module
 * @param module_addr Module address
 * @param register_count Pointer to store register count
 * @return Pointer to array of register_info_t structures
 */
const register_info_t* get_module_registers(uint8_t module_addr, uint16_t* register_count);

/**
 * @brief Validate register value against min/max limits
 * @param module_addr Module address
 * @param register_addr Register address
 * @param value Value to validate
 * @return true if value is valid, false otherwise
 */
bool validate_register_value(uint8_t module_addr, uint16_t register_addr, uint16_t value);

/**
 * @brief Get register info summary as string
 * @param module_addr Module address
 * @param register_addr Register address
 * @param buffer Buffer to store summary string
 * @param buffer_size Buffer size
 * @return true if successful, false otherwise
 */
bool get_register_info_summary(uint8_t module_addr, uint16_t register_addr, char* buffer, size_t buffer_size);

// ============================================================================
// ISSUE #203 - REGISTER METADATA API SUPPORT
// ============================================================================

/**
 * @brief Register list response structure for API
 */
typedef struct {
    const register_info_t* registers;  // Pointer to register array (static data)
    uint16_t count;                    // Number of registers
    uint8_t module_addr;               // Module address
    const char* module_name;           // Module name
    bool valid;                        // Response validity flag
} register_list_response_t;

/**
 * @brief Get all registers for a specific module (Issue #203)
 * @param module_addr Module address (MODULE_ADDR_*)
 * @param count_out Pointer to store register count
 * @return Pointer to register array (static data - do not free), NULL if module not found
 */
const register_info_t* get_module_registers_array(uint8_t module_addr, uint16_t* count_out);

/**
 * @brief Get register list response for HTTP API (Issue #203)
 * @param module_addr Module address
 * @return Pointer to register_list_response_t (static data - do not free), NULL if module not found
 */
const register_list_response_t* get_module_register_list_api(uint8_t module_addr);
bool get_register_info_summary(uint8_t module_addr, uint16_t register_addr, char* buffer, size_t buffer_size);

/**
 * @brief Print register information for debugging
 * @param module_addr Module address
 * @param register_addr Register address
 */
void print_register_info(uint8_t module_addr, uint16_t register_addr);

#endif // REGISTER_INFO_H
