/**
 * @file register_json_serializer.h
 * @brief JSON Serializer for Register Metadata
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 * @task Issue #203 - Register Metadata API Implementation
 */

#ifndef REGISTER_JSON_SERIALIZER_H
#define REGISTER_JSON_SERIALIZER_H

#include "../../../src/hal/register/register_info.h"
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// ENUM TO STRING CONVERTERS
// ============================================================================

/**
 * @brief Convert register mode enum to string
 * @param mode Register mode (REG_MODE_*)
 * @return Mode string ("READ", "WRITE", "READ_WRITE", "WRITE_ONCE")
 */
const char* register_mode_to_string(uint8_t mode);

/**
 * @brief Convert data type enum to string
 * @param data_type Data type (REG_DATA_TYPE_*)
 * @return Data type string ("UINT8", "UINT16", etc.)
 */
const char* register_data_type_to_string(uint8_t data_type);

/**
 * @brief Convert access level enum to string
 * @param access_level Access level (REG_ACCESS_*)
 * @return Access level string ("USER", "ADMIN", "SYSTEM")
 */
const char* register_access_level_to_string(uint8_t access_level);

// ============================================================================
// JSON SERIALIZERS
// ============================================================================

/**
 * @brief Serialize single register to JSON string
 * @param reg Pointer to register_info_t
 * @return Dynamically allocated JSON string (caller must free)
 * @note Returns NULL on error
 */
char* serialize_register_to_json(const register_info_t* reg);

/**
 * @brief Serialize register list to JSON string
 * @param response Pointer to register_list_response_t
 * @return Dynamically allocated JSON string (caller must free)
 * @note Returns NULL on error
 */
char* serialize_register_list_to_json(const register_list_response_t* response);

/**
 * @brief Create error JSON response
 * @param error_message Error message
 * @return Dynamically allocated JSON string (caller must free)
 */
char* create_error_json(const char* error_message);

/**
 * @brief Estimate buffer size needed for JSON serialization
 * @param register_count Number of registers
 * @return Estimated buffer size in bytes
 */
size_t estimate_json_buffer_size(uint16_t register_count);

#endif // REGISTER_JSON_SERIALIZER_H

