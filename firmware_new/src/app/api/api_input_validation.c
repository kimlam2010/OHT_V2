/**
 * @file api_input_validation.c
 * @brief API Input Validation Utilities for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation) - Phase 5.2
 */

#include "api_input_validation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>
#include <uuid/uuid.h>

// Global validation state
static bool g_validation_initialized = false;

/**
 * @brief Initialize input validation system
 */
hal_status_t api_validation_init(void) {
    if (g_validation_initialized) {
        return HAL_STATUS_OK;
    }
    
    g_validation_initialized = true;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "Input validation system initialized");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize input validation system
 */
hal_status_t api_validation_deinit(void) {
    if (!g_validation_initialized) {
        return HAL_STATUS_OK;
    }
    
    g_validation_initialized = false;
    
    hal_log_message(HAL_LOG_LEVEL_INFO, "Input validation system deinitialized");
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate JSON input
 */
hal_status_t api_validation_validate_json(const char *json, const api_validation_rule_t *rules,
                                         uint32_t rule_count, const api_validation_context_t *context,
                                         api_validation_result_t *result) {
    if (json == NULL || rules == NULL || result == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_validation_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Initialize result
    api_validation_create_result(result);
    
    // Validate each rule
    for (uint32_t i = 0; i < rule_count; i++) {
        const api_validation_rule_t *rule = &rules[i];
        
        // Check if field exists in JSON
        char field_value[API_VALIDATION_MAX_VALUE_LEN];
        bool field_exists = api_validation_parse_json_field(json, rule->field_name, field_value);
        
        // Check required field
        if (rule->required && !field_exists) {
            api_validation_error_t error;
            api_validation_validate_required(rule->field_name, NULL, &error);
            api_validation_add_error(result, &error);
            continue;
        }
        
        // Skip validation if field doesn't exist and not required
        if (!field_exists) {
            continue;
        }
        
        // Validate field value
        api_validation_error_t error;
        if (!api_validation_validate_field(rule->field_name, field_value, rule, &error)) {
            api_validation_add_error(result, &error);
        }
    }
    
    result->valid = (result->error_count == 0);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate HTTP request
 */
hal_status_t api_validation_validate_request(const api_mgr_http_request_t *request,
                                            const api_validation_rule_t *rules, uint32_t rule_count,
                                            api_validation_result_t *result) {
    if (request == NULL || rules == NULL || result == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!g_validation_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Create validation context
    api_validation_context_t context;
    strncpy(context.endpoint, request->path, sizeof(context.endpoint) - 1);
    strncpy(context.method, (request->method == API_MGR_HTTP_GET) ? "GET" :
                           (request->method == API_MGR_HTTP_POST) ? "POST" :
                           (request->method == API_MGR_HTTP_PUT) ? "PUT" :
                           (request->method == API_MGR_HTTP_DELETE) ? "DELETE" : "UNKNOWN",
            sizeof(context.method) - 1);
    context.timestamp = hal_get_timestamp_ms();
    
    // Generate request ID
    api_error_generate_request_id(context.request_id);
    
    // Validate JSON body if present
    if (request->body != NULL && request->body_length > 0) {
        return api_validation_validate_json(request->body, rules, rule_count, &context, result);
    }
    
    // Initialize result
    api_validation_create_result(result);
    result->valid = true;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate field value
 */
bool api_validation_validate_field(const char *field_name, const char *value,
                                  const api_validation_rule_t *rule, api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || rule == NULL || error == NULL) {
        return false;
    }
    
    // Initialize error
    memset(error, 0, sizeof(api_validation_error_t));
    strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
    strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
    
    // Validate based on type
    switch (rule->type) {
        case API_VALIDATION_TYPE_STRING:
            return api_validation_validate_string(field_name, value, rule, error);
        case API_VALIDATION_TYPE_INTEGER:
            return api_validation_validate_integer(field_name, value, rule, error);
        case API_VALIDATION_TYPE_FLOAT:
            return api_validation_validate_float(field_name, value, rule, error);
        case API_VALIDATION_TYPE_BOOLEAN:
            return api_validation_validate_boolean(field_name, value, error);
        case API_VALIDATION_TYPE_EMAIL:
            return api_validation_validate_email(field_name, value, error);
        case API_VALIDATION_TYPE_URL:
            return api_validation_validate_url(field_name, value, error);
        case API_VALIDATION_TYPE_IP_ADDRESS:
            return api_validation_validate_ip_address(field_name, value, error);
        case API_VALIDATION_TYPE_MAC_ADDRESS:
            return api_validation_validate_mac_address(field_name, value, error);
        case API_VALIDATION_TYPE_UUID:
            return api_validation_validate_uuid(field_name, value, error);
        case API_VALIDATION_TYPE_DATE:
            return api_validation_validate_date(field_name, value, error);
        case API_VALIDATION_TYPE_TIME:
            return api_validation_validate_time(field_name, value, error);
        case API_VALIDATION_TYPE_DATETIME:
            return api_validation_validate_datetime(field_name, value, error);
        case API_VALIDATION_TYPE_ENUM:
            return api_validation_validate_enum(field_name, value, rule, error);
        case API_VALIDATION_TYPE_PATTERN:
            return api_validation_validate_pattern(field_name, value, rule, error);
        default:
            strcpy(error->error_message, "Unknown validation type");
            return false;
    }
}

/**
 * @brief Validate required field
 */
bool api_validation_validate_required(const char *field_name, const char *value,
                                     api_validation_error_t *error) {
    if (field_name == NULL || error == NULL) {
        return false;
    }
    
    if (value == NULL || strlen(value) == 0) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strcpy(error->error_message, "Field is required");
        strcpy(error->expected_format, "non-empty value");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate string field
 */
bool api_validation_validate_string(const char *field_name, const char *value,
                                   const api_validation_rule_t *rule, api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || rule == NULL || error == NULL) {
        return false;
    }
    
    size_t value_len = strlen(value);
    
    // Check length constraints
    if (rule->min_length > 0 && value_len < (size_t)rule->min_length) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        snprintf(error->error_message, sizeof(error->error_message),
                "String length %zu is less than minimum length %d", value_len, rule->min_length);
        snprintf(error->expected_format, sizeof(error->expected_format),
                "minimum length %d", rule->min_length);
        return false;
    }
    
    if (rule->max_length > 0 && value_len > (size_t)rule->max_length) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        snprintf(error->error_message, sizeof(error->error_message),
                "String length %zu exceeds maximum length %d", value_len, rule->max_length);
        snprintf(error->expected_format, sizeof(error->expected_format),
                "maximum length %d", rule->max_length);
        return false;
    }
    
    // Check pattern if specified
    if (strlen(rule->pattern) > 0) {
        return api_validation_validate_pattern(field_name, value, rule, error);
    }
    
    return true;
}

/**
 * @brief Validate integer field
 */
bool api_validation_validate_integer(const char *field_name, const char *value,
                                    const api_validation_rule_t *rule, api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || rule == NULL || error == NULL) {
        return false;
    }
    
    // Check if value is a valid integer
    char *endptr;
    long int_val = strtol(value, &endptr, 10);
    
    if (*endptr != '\0') {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid integer format");
        strcpy(error->expected_format, "integer");
        return false;
    }
    
    // Check range constraints
    if (rule->min_value != 0 || rule->max_value != 0) {
        if (int_val < rule->min_value) {
            strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
            strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
            snprintf(error->error_message, sizeof(error->error_message),
                    "Value %ld is less than minimum value %ld", int_val, rule->min_value);
            snprintf(error->expected_format, sizeof(error->expected_format),
                    "minimum value %ld", rule->min_value);
            return false;
        }
        
        if (int_val > rule->max_value) {
            strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
            strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
            snprintf(error->error_message, sizeof(error->error_message),
                    "Value %ld exceeds maximum value %ld", int_val, rule->max_value);
            snprintf(error->expected_format, sizeof(error->expected_format),
                    "maximum value %ld", rule->max_value);
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Validate float field
 */
bool api_validation_validate_float(const char *field_name, const char *value,
                                  const api_validation_rule_t *rule, api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || rule == NULL || error == NULL) {
        return false;
    }
    
    // Check if value is a valid float
    char *endptr;
    double float_val = strtod(value, &endptr);
    
    if (*endptr != '\0') {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid float format");
        strcpy(error->expected_format, "float");
        return false;
    }
    
    // Check range constraints
    if (rule->min_float != 0.0 || rule->max_float != 0.0) {
        if (float_val < rule->min_float) {
            strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
            strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
            snprintf(error->error_message, sizeof(error->error_message),
                    "Value %f is less than minimum value %f", float_val, rule->min_float);
            snprintf(error->expected_format, sizeof(error->expected_format),
                    "minimum value %f", rule->min_float);
            return false;
        }
        
        if (float_val > rule->max_float) {
            strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
            strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
            snprintf(error->error_message, sizeof(error->error_message),
                    "Value %f exceeds maximum value %f", float_val, rule->max_float);
            snprintf(error->expected_format, sizeof(error->expected_format),
                    "maximum value %f", rule->max_float);
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Validate boolean field
 */
bool api_validation_validate_boolean(const char *field_name, const char *value,
                                    api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Check if value is a valid boolean
    if (strcasecmp(value, "true") != 0 && strcasecmp(value, "false") != 0 &&
        strcasecmp(value, "1") != 0 && strcasecmp(value, "0") != 0 &&
        strcasecmp(value, "yes") != 0 && strcasecmp(value, "no") != 0) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid boolean format");
        strcpy(error->expected_format, "true, false, 1, 0, yes, no");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate email field
 */
bool api_validation_validate_email(const char *field_name, const char *value,
                                  api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Simple email validation regex
    const char *email_pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
    
    regex_t regex;
    int ret = regcomp(&regex, email_pattern, REG_EXTENDED);
    if (ret != 0) {
        return false;
    }
    
    ret = regexec(&regex, value, 0, NULL, 0);
    regfree(&regex);
    
    if (ret != 0) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid email format");
        strcpy(error->expected_format, "valid email address");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate URL field
 */
bool api_validation_validate_url(const char *field_name, const char *value,
                                api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Simple URL validation
    if (strncmp(value, "http://", 7) != 0 && strncmp(value, "https://", 8) != 0) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid URL format");
        strcpy(error->expected_format, "http:// or https:// URL");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate IP address field
 */
bool api_validation_validate_ip_address(const char *field_name, const char *value,
                                       api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Simple IPv4 validation
    int octets[4];
    int count = sscanf(value, "%d.%d.%d.%d", &octets[0], &octets[1], &octets[2], &octets[3]);
    
    if (count != 4) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid IP address format");
        strcpy(error->expected_format, "IPv4 address (x.x.x.x)");
        return false;
    }
    
    for (int i = 0; i < 4; i++) {
        if (octets[i] < 0 || octets[i] > 255) {
            strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
            strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
            strcpy(error->error_message, "Invalid IP address range");
            strcpy(error->expected_format, "IPv4 address (0-255.0-255.0-255.0-255)");
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Validate MAC address field
 */
bool api_validation_validate_mac_address(const char *field_name, const char *value,
                                        api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Simple MAC address validation
    int bytes[6];
    int count = sscanf(value, "%02x:%02x:%02x:%02x:%02x:%02x",
                      &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5]);
    
    if (count != 6) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid MAC address format");
        strcpy(error->expected_format, "MAC address (xx:xx:xx:xx:xx:xx)");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate UUID field
 */
bool api_validation_validate_uuid(const char *field_name, const char *value,
                                 api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Validate UUID format
    uuid_t uuid;
    int ret = uuid_parse(value, uuid);
    
    if (ret != 0) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid UUID format");
        strcpy(error->expected_format, "UUID (xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate date field
 */
bool api_validation_validate_date(const char *field_name, const char *value,
                                 api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Simple date validation (YYYY-MM-DD)
    int year, month, day;
    int count = sscanf(value, "%d-%d-%d", &year, &month, &day);
    
    if (count != 3) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid date format");
        strcpy(error->expected_format, "YYYY-MM-DD");
        return false;
    }
    
    // Basic range validation
    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid date range");
        strcpy(error->expected_format, "valid date (1900-2100)");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate time field
 */
bool api_validation_validate_time(const char *field_name, const char *value,
                                 api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Simple time validation (HH:MM:SS)
    int hour, minute, second;
    int count = sscanf(value, "%d:%d:%d", &hour, &minute, &second);
    
    if (count != 3) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid time format");
        strcpy(error->expected_format, "HH:MM:SS");
        return false;
    }
    
    // Basic range validation
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid time range");
        strcpy(error->expected_format, "valid time (00:00:00 - 23:59:59)");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate datetime field
 */
bool api_validation_validate_datetime(const char *field_name, const char *value,
                                     api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || error == NULL) {
        return false;
    }
    
    // Simple datetime validation (YYYY-MM-DD HH:MM:SS)
    int year, month, day, hour, minute, second;
    int count = sscanf(value, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    
    if (count != 6) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid datetime format");
        strcpy(error->expected_format, "YYYY-MM-DD HH:MM:SS");
        return false;
    }
    
    // Basic range validation
    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31 ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Invalid datetime range");
        strcpy(error->expected_format, "valid datetime");
        return false;
    }
    
    return true;
}

/**
 * @brief Validate enum field
 */
bool api_validation_validate_enum(const char *field_name, const char *value,
                                 const api_validation_rule_t *rule, api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || rule == NULL || error == NULL) {
        return false;
    }
    
    // Check if value is in enum list
    for (uint32_t i = 0; i < rule->enum_count; i++) {
        if (strcmp(value, rule->enum_values[i]) == 0) {
            return true;
        }
    }
    
    strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
    strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
    strcpy(error->error_message, "Value not in allowed enum values");
    
    // Create expected format string
    char expected_format[512] = "one of: ";
    for (uint32_t i = 0; i < rule->enum_count && i < 10; i++) { // Limit to 10 values
        if (i > 0) {
            strcat(expected_format, ", ");
        }
        strcat(expected_format, rule->enum_values[i]);
    }
    if (rule->enum_count > 10) {
        strcat(expected_format, "...");
    }
    
    strncpy(error->expected_format, expected_format, sizeof(error->expected_format) - 1);
    
    return false;
}

/**
 * @brief Validate pattern field
 */
bool api_validation_validate_pattern(const char *field_name, const char *value,
                                    const api_validation_rule_t *rule, api_validation_error_t *error) {
    if (field_name == NULL || value == NULL || rule == NULL || error == NULL) {
        return false;
    }
    
    if (strlen(rule->pattern) == 0) {
        return true;
    }
    
    regex_t regex;
    int ret = regcomp(&regex, rule->pattern, REG_EXTENDED);
    if (ret != 0) {
        return false;
    }
    
    ret = regexec(&regex, value, 0, NULL, 0);
    regfree(&regex);
    
    if (ret != 0) {
        strncpy(error->field_name, field_name, sizeof(error->field_name) - 1);
        strncpy(error->provided_value, value, sizeof(error->provided_value) - 1);
        strcpy(error->error_message, "Value does not match required pattern");
        strncpy(error->expected_format, rule->pattern, sizeof(error->expected_format) - 1);
        return false;
    }
    
    return true;
}

/**
 * @brief Create validation rule
 */
hal_status_t api_validation_create_rule(api_validation_type_t type, const char *field_name,
                                       bool required, api_validation_rule_t *rule) {
    if (field_name == NULL || rule == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memset(rule, 0, sizeof(api_validation_rule_t));
    
    rule->type = type;
    strncpy(rule->field_name, field_name, sizeof(rule->field_name) - 1);
    rule->required = required;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set validation rule pattern
 */
hal_status_t api_validation_set_pattern(api_validation_rule_t *rule, const char *pattern) {
    if (rule == NULL || pattern == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    strncpy(rule->pattern, pattern, sizeof(rule->pattern) - 1);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set validation rule length constraints
 */
hal_status_t api_validation_set_length(api_validation_rule_t *rule, int32_t min_length, int32_t max_length) {
    if (rule == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    rule->min_length = min_length;
    rule->max_length = max_length;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set validation rule range constraints
 */
hal_status_t api_validation_set_range(api_validation_rule_t *rule, int64_t min_value, int64_t max_value) {
    if (rule == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    rule->min_value = min_value;
    rule->max_value = max_value;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set validation rule float range constraints
 */
hal_status_t api_validation_set_float_range(api_validation_rule_t *rule, double min_value, double max_value) {
    if (rule == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    rule->min_float = min_value;
    rule->max_float = max_value;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set validation rule enum values
 */
hal_status_t api_validation_set_enum(api_validation_rule_t *rule, const char **enum_values, uint32_t enum_count) {
    if (rule == NULL || enum_values == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    rule->enum_values = (char**)enum_values;
    rule->enum_count = enum_count;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set validation rule custom message
 */
hal_status_t api_validation_set_custom_message(api_validation_rule_t *rule, const char *message) {
    if (rule == NULL || message == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    strncpy(rule->custom_message, message, sizeof(rule->custom_message) - 1);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Parse JSON field value
 */
bool api_validation_parse_json_field(const char *json, const char *field_name, char *value) {
    if (json == NULL || field_name == NULL || value == NULL) {
        return false;
    }
    
    // Simple JSON parsing - look for "field_name":"value"
    char search_pattern[256];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":\"", field_name);
    
    const char *start = strstr(json, search_pattern);
    if (start == NULL) {
        return false;
    }
    
    start += strlen(search_pattern);
    const char *end = strchr(start, '"');
    if (end == NULL) {
        return false;
    }
    
    size_t value_len = end - start;
    if (value_len >= API_VALIDATION_MAX_VALUE_LEN) {
        value_len = API_VALIDATION_MAX_VALUE_LEN - 1;
    }
    
    strncpy(value, start, value_len);
    value[value_len] = '\0';
    
    return true;
}

/**
 * @brief Check if JSON field exists
 */
bool api_validation_json_field_exists(const char *json, const char *field_name) {
    if (json == NULL || field_name == NULL) {
        return false;
    }
    
    char search_pattern[256];
    snprintf(search_pattern, sizeof(search_pattern), "\"%s\":", field_name);
    
    return strstr(json, search_pattern) != NULL;
}

/**
 * @brief Create validation result
 */
hal_status_t api_validation_create_result(api_validation_result_t *result) {
    if (result == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memset(result, 0, sizeof(api_validation_result_t));
    result->valid = true;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Add validation error to result
 */
hal_status_t api_validation_add_error(api_validation_result_t *result, const api_validation_error_t *error) {
    if (result == NULL || error == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (result->error_count >= API_VALIDATION_MAX_ERRORS) {
        return HAL_STATUS_ERROR;
    }
    
    memcpy(&result->errors[result->error_count], error, sizeof(api_validation_error_t));
    result->error_count++;
    result->valid = false;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Clear validation result
 */
hal_status_t api_validation_clear_result(api_validation_result_t *result) {
    if (result == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memset(result, 0, sizeof(api_validation_result_t));
    result->valid = true;
    
    return HAL_STATUS_OK;
}

/**
 * @brief Serialize validation result to JSON
 */
int api_validation_serialize_result_json(const api_validation_result_t *result, 
                                        char *buffer, size_t buffer_size) {
    if (result == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    int written = snprintf(buffer, buffer_size,
        "{"
        "\"valid\":%s,"
        "\"error_count\":%u,"
        "\"errors\":[",
        result->valid ? "true" : "false",
        result->error_count
    );
    
    for (uint32_t i = 0; i < result->error_count && written < (int)buffer_size; i++) {
        if (i > 0) {
            written += snprintf(buffer + written, buffer_size - written, ",");
        }
        
        written += snprintf(buffer + written, buffer_size - written,
            "{"
            "\"field_name\":\"%s\","
            "\"error_message\":\"%s\","
            "\"provided_value\":\"%s\","
            "\"expected_format\":\"%s\""
            "}",
            result->errors[i].field_name,
            result->errors[i].error_message,
            result->errors[i].provided_value,
            result->errors[i].expected_format
        );
    }
    
    written += snprintf(buffer + written, buffer_size - written, "]}");
    
    return (written < (int)buffer_size) ? written : -1;
}
