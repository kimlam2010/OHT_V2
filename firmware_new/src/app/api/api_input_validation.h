/**
 * @file api_input_validation.h
 * @brief API Input Validation Utilities for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.2
 */

#ifndef API_INPUT_VALIDATION_H
#define API_INPUT_VALIDATION_H

#include "hal_common.h"
#include "api_error_handling.h"

// Input Validation Version
#define API_VALIDATION_VERSION_MAJOR 1
#define API_VALIDATION_VERSION_MINOR 0
#define API_VALIDATION_VERSION_PATCH 0
#define API_VALIDATION_VERSION_STRING "1.0.0"

// Validation Limits
#define API_VALIDATION_MAX_FIELD_NAME_LEN 64
#define API_VALIDATION_MAX_VALUE_LEN 256
#define API_VALIDATION_MAX_PATTERN_LEN 128
#define API_VALIDATION_MAX_ERRORS 32

// Validation Types
typedef enum {
    API_VALIDATION_TYPE_NONE = 0,
    API_VALIDATION_TYPE_REQUIRED,       // Field is required
    API_VALIDATION_TYPE_STRING,         // String validation
    API_VALIDATION_TYPE_INTEGER,        // Integer validation
    API_VALIDATION_TYPE_FLOAT,          // Float validation
    API_VALIDATION_TYPE_BOOLEAN,        // Boolean validation
    API_VALIDATION_TYPE_EMAIL,          // Email validation
    API_VALIDATION_TYPE_URL,            // URL validation
    API_VALIDATION_TYPE_IP_ADDRESS,     // IP address validation
    API_VALIDATION_TYPE_MAC_ADDRESS,    // MAC address validation
    API_VALIDATION_TYPE_UUID,           // UUID validation
    API_VALIDATION_TYPE_DATE,           // Date validation
    API_VALIDATION_TYPE_TIME,           // Time validation
    API_VALIDATION_TYPE_DATETIME,       // DateTime validation
    API_VALIDATION_TYPE_ENUM,           // Enum validation
    API_VALIDATION_TYPE_ARRAY,          // Array validation
    API_VALIDATION_TYPE_OBJECT,         // Object validation
    API_VALIDATION_TYPE_PATTERN,        // Regex pattern validation
    API_VALIDATION_TYPE_LENGTH,         // Length validation
    API_VALIDATION_TYPE_RANGE,          // Range validation
    API_VALIDATION_TYPE_MAX
} api_validation_type_t;

// Validation Rule Structure
typedef struct {
    api_validation_type_t type;         // Validation type
    char field_name[API_VALIDATION_MAX_FIELD_NAME_LEN]; // Field name
    bool required;                      // Field is required
    char pattern[API_VALIDATION_MAX_PATTERN_LEN]; // Regex pattern
    int32_t min_length;                 // Minimum length
    int32_t max_length;                 // Maximum length
    int64_t min_value;                  // Minimum value
    int64_t max_value;                  // Maximum value
    double min_float;                   // Minimum float value
    double max_float;                   // Maximum float value
    char **enum_values;                 // Enum values array
    uint32_t enum_count;                // Number of enum values
    char custom_message[256];           // Custom error message
} api_validation_rule_t;

// Validation Result Structure
typedef struct {
    bool valid;                         // Validation result
    uint32_t error_count;               // Number of validation errors
    api_validation_error_t errors[API_VALIDATION_MAX_ERRORS]; // Validation errors
} api_validation_result_t;

// Validation Context Structure
typedef struct {
    char endpoint[128];                 // API endpoint
    char method[16];                    // HTTP method
    char request_id[64];                // Request ID
    uint64_t timestamp;                 // Validation timestamp
} api_validation_context_t;

// Input Validation Functions

/**
 * @brief Initialize input validation system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_init(void);

/**
 * @brief Deinitialize input validation system
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_deinit(void);

/**
 * @brief Validate JSON input
 * @param json JSON string
 * @param rules Validation rules array
 * @param rule_count Number of validation rules
 * @param context Validation context
 * @param result Validation result (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_validate_json(const char *json, const api_validation_rule_t *rules,
                                         uint32_t rule_count, const api_validation_context_t *context,
                                         api_validation_result_t *result);

/**
 * @brief Validate HTTP request
 * @param request HTTP request
 * @param rules Validation rules array
 * @param rule_count Number of validation rules
 * @param result Validation result (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_validate_request(const api_mgr_http_request_t *request,
                                            const api_validation_rule_t *rules, uint32_t rule_count,
                                            api_validation_result_t *result);

/**
 * @brief Validate field value
 * @param field_name Field name
 * @param value Field value
 * @param rule Validation rule
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_field(const char *field_name, const char *value,
                                  const api_validation_rule_t *rule, api_validation_error_t *error);

// Field Validation Functions

/**
 * @brief Validate required field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_required(const char *field_name, const char *value,
                                     api_validation_error_t *error);

/**
 * @brief Validate string field
 * @param field_name Field name
 * @param value Field value
 * @param rule Validation rule
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_string(const char *field_name, const char *value,
                                   const api_validation_rule_t *rule, api_validation_error_t *error);

/**
 * @brief Validate integer field
 * @param field_name Field name
 * @param value Field value
 * @param rule Validation rule
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_integer(const char *field_name, const char *value,
                                    const api_validation_rule_t *rule, api_validation_error_t *error);

/**
 * @brief Validate float field
 * @param field_name Field name
 * @param value Field value
 * @param rule Validation rule
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_float(const char *field_name, const char *value,
                                  const api_validation_rule_t *rule, api_validation_error_t *error);

/**
 * @brief Validate boolean field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_boolean(const char *field_name, const char *value,
                                    api_validation_error_t *error);

/**
 * @brief Validate email field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_email(const char *field_name, const char *value,
                                  api_validation_error_t *error);

/**
 * @brief Validate URL field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_url(const char *field_name, const char *value,
                                api_validation_error_t *error);

/**
 * @brief Validate IP address field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_ip_address(const char *field_name, const char *value,
                                       api_validation_error_t *error);

/**
 * @brief Validate MAC address field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_mac_address(const char *field_name, const char *value,
                                        api_validation_error_t *error);

/**
 * @brief Validate UUID field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_uuid(const char *field_name, const char *value,
                                 api_validation_error_t *error);

/**
 * @brief Validate date field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_date(const char *field_name, const char *value,
                                 api_validation_error_t *error);

/**
 * @brief Validate time field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_time(const char *field_name, const char *value,
                                 api_validation_error_t *error);

/**
 * @brief Validate datetime field
 * @param field_name Field name
 * @param value Field value
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_datetime(const char *field_name, const char *value,
                                     api_validation_error_t *error);

/**
 * @brief Validate enum field
 * @param field_name Field name
 * @param value Field value
 * @param rule Validation rule
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_enum(const char *field_name, const char *value,
                                 const api_validation_rule_t *rule, api_validation_error_t *error);

/**
 * @brief Validate pattern field
 * @param field_name Field name
 * @param value Field value
 * @param rule Validation rule
 * @param error Validation error (output)
 * @return bool true if valid, false if invalid
 */
bool api_validation_validate_pattern(const char *field_name, const char *value,
                                    const api_validation_rule_t *rule, api_validation_error_t *error);

// Utility Functions

/**
 * @brief Create validation rule
 * @param type Validation type
 * @param field_name Field name
 * @param required Field is required
 * @param rule Validation rule (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_create_rule(api_validation_type_t type, const char *field_name,
                                       bool required, api_validation_rule_t *rule);

/**
 * @brief Set validation rule pattern
 * @param rule Validation rule
 * @param pattern Regex pattern
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_set_pattern(api_validation_rule_t *rule, const char *pattern);

/**
 * @brief Set validation rule length constraints
 * @param rule Validation rule
 * @param min_length Minimum length
 * @param max_length Maximum length
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_set_length(api_validation_rule_t *rule, int32_t min_length, int32_t max_length);

/**
 * @brief Set validation rule range constraints
 * @param rule Validation rule
 * @param min_value Minimum value
 * @param max_value Maximum value
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_set_range(api_validation_rule_t *rule, int64_t min_value, int64_t max_value);

/**
 * @brief Set validation rule float range constraints
 * @param rule Validation rule
 * @param min_value Minimum float value
 * @param max_value Maximum float value
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_set_float_range(api_validation_rule_t *rule, double min_value, double max_value);

/**
 * @brief Set validation rule enum values
 * @param rule Validation rule
 * @param enum_values Enum values array
 * @param enum_count Number of enum values
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_set_enum(api_validation_rule_t *rule, const char **enum_values, uint32_t enum_count);

/**
 * @brief Set validation rule custom message
 * @param rule Validation rule
 * @param message Custom error message
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_set_custom_message(api_validation_rule_t *rule, const char *message);

// JSON Parsing Functions

/**
 * @brief Parse JSON field value
 * @param json JSON string
 * @param field_name Field name
 * @param value Field value (output)
 * @return bool true if field found, false if not found
 */
bool api_validation_parse_json_field(const char *json, const char *field_name, char *value);

/**
 * @brief Check if JSON field exists
 * @param json JSON string
 * @param field_name Field name
 * @return bool true if field exists, false if not
 */
bool api_validation_json_field_exists(const char *json, const char *field_name);

// Validation Result Functions

/**
 * @brief Create validation result
 * @param result Validation result (output)
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_create_result(api_validation_result_t *result);

/**
 * @brief Add validation error to result
 * @param result Validation result
 * @param error Validation error
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_add_error(api_validation_result_t *result, const api_validation_error_t *error);

/**
 * @brief Clear validation result
 * @param result Validation result
 * @return hal_status_t HAL_STATUS_OK on success, error code on failure
 */
hal_status_t api_validation_clear_result(api_validation_result_t *result);

// JSON Serialization Functions

/**
 * @brief Serialize validation result to JSON
 * @param result Validation result
 * @param buffer JSON buffer
 * @param buffer_size Buffer size
 * @return int Number of bytes written, -1 on error
 */
int api_validation_serialize_result_json(const api_validation_result_t *result, 
                                        char *buffer, size_t buffer_size);

#endif // API_INPUT_VALIDATION_H
