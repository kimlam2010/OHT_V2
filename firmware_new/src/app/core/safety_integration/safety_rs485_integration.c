/**
 * @file safety_rs485_integration.c
 * @brief Safety RS485 Integration Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Phase 1.3 - Register Reading Function Implementation
 * 
 * 🚨 SAFETY CRITICAL: This implementation provides RS485 communication with safety modules
 * 🔒 SECURITY: All functions are standalone and do NOT modify existing safety_monitor code
 * ⚠️  WARNING: Must complete all register reads within 50ms for safety compliance
 */

#include "safety_rs485_integration.h"
#include "../../managers/communication_manager.h"
#include "../../managers/module_manager.h"
#include "safety_monitor.h"
#include "../../../hal/common/hal_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>

// 📊 **GLOBAL VARIABLES - THREAD SAFE**
static bool g_safety_rs485_initialized = false;
static safety_rs485_stats_t g_safety_stats = {0};
static safety_rs485_status_t g_safety_status = {0};
static bool g_debug_logging_enabled = false;

// 🔧 **MODULE CONFIGURATION TABLE**
static const module_safety_config_t g_module_configs[] = {
    // Power Module (0x02)
    {
        .module_address = SAFETY_RS485_POWER_MODULE_ADDR,
        .module_type = MODULE_TYPE_POWER,
        .is_critical = false,
        .timeout_ms = SAFETY_RS485_POWER_TIMEOUT_MS,
        .safety_action = SAFETY_RS485_ACTION_ESTOP_DELAYED,
        .health_check_interval_ms = 5000,
        .max_consecutive_failures = 3,
        .expected_response_time_ms = 30,
        .max_acceptable_response_time_ms = SAFETY_RS485_MAX_RESPONSE_TIME_MS
    },
    // Safety Module (0x03) - CRITICAL
    {
        .module_address = SAFETY_RS485_SAFETY_MODULE_ADDR,
        .module_type = MODULE_TYPE_SAFETY,
        .is_critical = true,
        .timeout_ms = SAFETY_RS485_CRITICAL_TIMEOUT_MS,
        .safety_action = SAFETY_RS485_ACTION_ESTOP_IMMEDIATE,
        .health_check_interval_ms = 100,
        .max_consecutive_failures = 1,
        .expected_response_time_ms = 20,
        .max_acceptable_response_time_ms = SAFETY_RS485_MAX_RESPONSE_TIME_MS
    },
    // Travel Motor Module (0x04)
    {
        .module_address = SAFETY_RS485_TRAVEL_MODULE_ADDR,
        .module_type = MODULE_TYPE_TRAVEL_MOTOR,
        .is_critical = false,
        .timeout_ms = SAFETY_RS485_TRAVEL_TIMEOUT_MS,
        .safety_action = SAFETY_RS485_ACTION_WARNING,
        .health_check_interval_ms = 2000,
        .max_consecutive_failures = 5,
        .expected_response_time_ms = 25,
        .max_acceptable_response_time_ms = SAFETY_RS485_MAX_RESPONSE_TIME_MS
    },
    // Dock Module (0x06)
    {
        .module_address = SAFETY_RS485_DOCK_MODULE_ADDR,
        .module_type = MODULE_TYPE_DOCK,
        .is_critical = false,
        .timeout_ms = SAFETY_RS485_DOCK_TIMEOUT_MS,
        .safety_action = SAFETY_RS485_ACTION_LOG_ONLY,
        .health_check_interval_ms = 10000,
        .max_consecutive_failures = 10,
        .expected_response_time_ms = 40,
        .max_acceptable_response_time_ms = SAFETY_RS485_MAX_RESPONSE_TIME_MS
    }
};

#define SAFETY_RS485_NUM_MODULES (sizeof(g_module_configs) / sizeof(g_module_configs[0]))

// 🕐 **UTILITY FUNCTIONS**

/**
 * @brief Get current timestamp in milliseconds
 * @return Current timestamp in ms
 */
static uint64_t safety_rs485_get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

/**
 * @brief Log debug message if debug logging is enabled
 * @param format Printf-style format string
 * @param ... Arguments
 */
static void safety_rs485_debug_log(const char *format, ...) {
    if (!g_debug_logging_enabled) return;
    
    va_list args;
    va_start(args, format);
    printf("[SAFETY_RS485_DEBUG] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/**
 * @brief Log error message (always enabled)
 * @param format Printf-style format string  
 * @param ... Arguments
 */
static void safety_rs485_error_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[SAFETY_RS485_ERROR] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

/**
 * @brief Log warning message (always enabled)
 * @param format Printf-style format string
 * @param ... Arguments
 */
static void safety_rs485_warning_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[SAFETY_RS485_WARNING] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

// 🔧 **INITIALIZATION FUNCTIONS**

hal_status_t safety_rs485_init(void) {
    if (g_safety_rs485_initialized) {
        safety_rs485_warning_log("Safety RS485 integration already initialized");
        return HAL_OK;
    }
    
    // Initialize statistics
    memset(&g_safety_stats, 0, sizeof(g_safety_stats));
    memset(&g_safety_status, 0, sizeof(g_safety_status));
    
    // Set initialization timestamp
    g_safety_stats.integration_start_time_ms = safety_rs485_get_timestamp_ms();
    g_safety_status.integration_active = true;
    
    // Initialize response time tracking
    g_safety_stats.min_response_time_ms = UINT32_MAX;
    g_safety_stats.max_response_time_ms = 0;
    g_safety_stats.avg_response_time_ms = 0;
    
    g_safety_rs485_initialized = true;
    
    safety_rs485_debug_log("Safety RS485 integration initialized successfully");
    return HAL_OK;
}

hal_status_t safety_rs485_deinit(void) {
    if (!g_safety_rs485_initialized) {
        return HAL_ERROR;
    }
    
    g_safety_status.integration_active = false;
    g_safety_rs485_initialized = false;
    
    safety_rs485_debug_log("Safety RS485 integration deinitialized");
    return HAL_OK;
}

// 🔍 **VALIDATION FUNCTIONS**

hal_status_t safety_rs485_validate_response(const safety_module_response_t *response) {
    if (!response) {
        safety_rs485_error_log("NULL response pointer");
        return HAL_ERROR;
    }
    
    // Validate safety status range
    if (response->safety_status > SAFETY_RS485_MAX_STATUS_VALUE) {
        safety_rs485_error_log("Invalid safety status: %u (max: %u)", 
                              response->safety_status, SAFETY_RS485_MAX_STATUS_VALUE);
        return HAL_ERROR;
    }
    
    // Validate sensor distances
    if (response->critical_values.sensor1_distance_mm > SAFETY_RS485_MAX_DISTANCE_MM) {
        safety_rs485_error_log("Invalid sensor1 distance: %u mm (max: %u mm)",
                              response->critical_values.sensor1_distance_mm, SAFETY_RS485_MAX_DISTANCE_MM);
        return HAL_ERROR;
    }
    
    if (response->critical_values.sensor2_distance_mm > SAFETY_RS485_MAX_DISTANCE_MM) {
        safety_rs485_error_log("Invalid sensor2 distance: %u mm (max: %u mm)",
                              response->critical_values.sensor2_distance_mm, SAFETY_RS485_MAX_DISTANCE_MM);
        return HAL_ERROR;
    }
    
    // Validate timestamps
    uint64_t current_time = safety_rs485_get_timestamp_ms();
    if (response->timestamps.response_timestamp_ms > current_time + 1000) { // Allow 1s future tolerance
        safety_rs485_error_log("Invalid response timestamp: %llu (current: %llu)",
                              response->timestamps.response_timestamp_ms, current_time);
        return HAL_ERROR;
    }
    
    // Validate response time
    if (response->timestamps.response_time_ms > SAFETY_RS485_MAX_RESPONSE_TIME_MS * 2) {
        safety_rs485_error_log("Invalid response time: %u ms (max: %u ms)",
                              response->timestamps.response_time_ms, SAFETY_RS485_MAX_RESPONSE_TIME_MS * 2);
        return HAL_ERROR;
    }
    
    safety_rs485_debug_log("Response validation successful");
    return HAL_OK;
}

// 📡 **CORE REGISTER READING FUNCTIONS**

hal_status_t safety_rs485_read_module_data(uint8_t module_addr, safety_module_response_t *response) {
    if (!g_safety_rs485_initialized) {
        safety_rs485_error_log("Safety RS485 integration not initialized");
        return HAL_ERROR;
    }
    
    if (!response) {
        safety_rs485_error_log("NULL response pointer");
        return HAL_ERROR;
    }
    
    // Initialize response structure
    memset(response, 0, sizeof(safety_module_response_t));
    response->timestamps.request_timestamp_ms = safety_rs485_get_timestamp_ms();
    
    safety_rs485_debug_log("Reading data from module 0x%02X (%s)", 
                          module_addr, safety_rs485_get_module_name(module_addr));
    
    hal_status_t overall_status = HAL_OK;
    uint32_t total_errors = 0;
    
    // 🔍 **READ REGISTER 0x0000: Safety Status**
    {
        uint16_t status_data = 0;
        hal_status_t status = comm_manager_modbus_read_holding_registers(
            module_addr, SAFETY_RS485_REG_STATUS, 1, &status_data);
        
        if (status == HAL_OK) {
            response->safety_status = (uint8_t)(status_data & 0xFF);
            safety_rs485_debug_log("Module 0x%02X status: %u", module_addr, response->safety_status);
        } else {
            safety_rs485_error_log("Failed to read status from module 0x%02X", module_addr);
            total_errors++;
            overall_status = HAL_ERROR;
        }
    }
    
    // 🔍 **READ REGISTER 0x0001: E-Stop Status**
    {
        uint16_t estop_data = 0;
        hal_status_t status = comm_manager_modbus_read_holding_registers(
            module_addr, SAFETY_RS485_REG_ESTOP, 1, &estop_data);
        
        if (status == HAL_OK) {
            response->estop_active = (estop_data != 0);
            safety_rs485_debug_log("Module 0x%02X E-Stop: %s", 
                                  module_addr, response->estop_active ? "ACTIVE" : "INACTIVE");
        } else {
            safety_rs485_error_log("Failed to read E-Stop from module 0x%02X", module_addr);
            total_errors++;
            overall_status = HAL_ERROR;
        }
    }
    
    // 🔍 **READ REGISTERS 0x0010-0x0011: Sensor Distances**
    {
        uint16_t sensor_data[2] = {0};
        hal_status_t status = comm_manager_modbus_read_holding_registers(
            module_addr, SAFETY_RS485_REG_SENSOR1_DIST, 2, sensor_data);
        
        if (status == HAL_OK) {
            response->critical_values.sensor1_distance_mm = sensor_data[0];
            response->critical_values.sensor2_distance_mm = sensor_data[1];
            safety_rs485_debug_log("Module 0x%02X sensors: S1=%u mm, S2=%u mm", 
                                  module_addr, sensor_data[0], sensor_data[1]);
        } else {
            safety_rs485_error_log("Failed to read sensors from module 0x%02X", module_addr);
            total_errors++;
            overall_status = HAL_ERROR;
        }
    }
    
    // 🔍 **READ REGISTER 0x0020: Digital Inputs**
    {
        uint16_t digital_data = 0;
        hal_status_t status = comm_manager_modbus_read_holding_registers(
            module_addr, SAFETY_RS485_REG_DIGITAL_INPUTS, 1, &digital_data);
        
        if (status == HAL_OK) {
            response->critical_values.digital_inputs = digital_data;
            safety_rs485_debug_log("Module 0x%02X digital inputs: 0x%04X", module_addr, digital_data);
        } else {
            safety_rs485_error_log("Failed to read digital inputs from module 0x%02X", module_addr);
            total_errors++;
            overall_status = HAL_ERROR;
        }
    }
    
    // 🔍 **READ REGISTER 0x0005: Error Codes**
    {
        uint16_t error_data = 0;
        hal_status_t status = comm_manager_modbus_read_holding_registers(
            module_addr, SAFETY_RS485_REG_ERROR_CODES, 1, &error_data);
        
        if (status == HAL_OK) {
            response->error_codes = (uint8_t)(error_data & 0xFF);
            if (response->error_codes != 0) {
                safety_rs485_warning_log("Module 0x%02X error codes: 0x%02X", module_addr, response->error_codes);
            }
        } else {
            safety_rs485_error_log("Failed to read error codes from module 0x%02X", module_addr);
            total_errors++;
            overall_status = HAL_ERROR;
        }
    }
    
    // 📊 **CALCULATE RESPONSE TIME AND UPDATE STATISTICS**
    response->timestamps.response_timestamp_ms = safety_rs485_get_timestamp_ms();
    response->timestamps.response_time_ms = (uint32_t)(
        response->timestamps.response_timestamp_ms - response->timestamps.request_timestamp_ms);
    
    // Update connection status
    response->connection_online = (overall_status == HAL_OK);
    if (response->connection_online) {
        response->timestamps.last_valid_response_ms = response->timestamps.response_timestamp_ms;
    }
    
    // Update communication statistics
    if (overall_status == HAL_OK) {
        response->comm_stats.successful_reads++;
        g_safety_stats.successful_checks++;
        
        // Update response time statistics
        if (response->timestamps.response_time_ms < g_safety_stats.min_response_time_ms) {
            g_safety_stats.min_response_time_ms = response->timestamps.response_time_ms;
        }
        if (response->timestamps.response_time_ms > g_safety_stats.max_response_time_ms) {
            g_safety_stats.max_response_time_ms = response->timestamps.response_time_ms;
        }
        
        // Update average (simple moving average)
        static uint32_t response_count = 0;
        response_count++;
        g_safety_stats.avg_response_time_ms = 
            (g_safety_stats.avg_response_time_ms * (response_count - 1) + response->timestamps.response_time_ms) / response_count;
        
    } else {
        response->comm_stats.failed_reads++;
        response->comm_stats.timeout_count += total_errors;
        g_safety_stats.failed_checks++;
    }
    
    // Update per-module statistics
    switch (module_addr) {
        case SAFETY_RS485_POWER_MODULE_ADDR:
            g_safety_stats.module_checks.power_module_checks++;
            break;
        case SAFETY_RS485_SAFETY_MODULE_ADDR:
            g_safety_stats.module_checks.safety_module_checks++;
            if (overall_status == HAL_OK) {
                g_safety_status.safety_module_online = true;
            }
            break;
        case SAFETY_RS485_TRAVEL_MODULE_ADDR:
            g_safety_stats.module_checks.travel_module_checks++;
            break;
        case SAFETY_RS485_DOCK_MODULE_ADDR:
            g_safety_stats.module_checks.dock_module_checks++;
            break;
    }
    
    // Update total statistics
    g_safety_stats.total_checks++;
    g_safety_stats.last_check_timestamp_ms = response->timestamps.response_timestamp_ms;
    if (overall_status == HAL_OK) {
        g_safety_stats.last_successful_check_ms = response->timestamps.response_timestamp_ms;
    }
    
    // 🚨 **PERFORMANCE WARNING**
    if (response->timestamps.response_time_ms > SAFETY_RS485_MAX_RESPONSE_TIME_MS) {
        safety_rs485_warning_log("PERFORMANCE WARNING: Module 0x%02X response time %u ms exceeds limit %u ms",
                                module_addr, response->timestamps.response_time_ms, SAFETY_RS485_MAX_RESPONSE_TIME_MS);
    }
    
    // 🔍 **VALIDATE RESPONSE DATA**
    if (overall_status == HAL_OK) {
        hal_status_t validation_status = safety_rs485_validate_response(response);
        if (validation_status != HAL_OK) {
            safety_rs485_error_log("Response validation failed for module 0x%02X", module_addr);
            overall_status = HAL_ERROR;
        }
    }
    
    safety_rs485_debug_log("Module 0x%02X read complete: status=%s, response_time=%u ms, errors=%u",
                          module_addr, 
                          (overall_status == HAL_OK) ? "SUCCESS" : "FAILED",
                          response->timestamps.response_time_ms,
                          total_errors);
    
    return overall_status;
}

// 🔧 **UTILITY FUNCTIONS IMPLEMENTATION**

const char* safety_rs485_get_action_name(safety_action_t action) {
    switch (action) {
        case SAFETY_RS485_ACTION_LOG_ONLY:        return "LOG_ONLY";
        case SAFETY_RS485_ACTION_WARNING:         return "WARNING";
        case SAFETY_RS485_ACTION_DEGRADED:        return "DEGRADED";
        case SAFETY_RS485_ACTION_ESTOP_DELAYED:   return "ESTOP_DELAYED";
        case SAFETY_RS485_ACTION_ESTOP_IMMEDIATE: return "ESTOP_IMMEDIATE";
        default:                                  return "UNKNOWN";
    }
}

const char* safety_rs485_get_module_name(uint8_t module_addr) {
    switch (module_addr) {
        case SAFETY_RS485_POWER_MODULE_ADDR:  return "Power Module";
        case SAFETY_RS485_SAFETY_MODULE_ADDR: return "Safety Module";
        case SAFETY_RS485_TRAVEL_MODULE_ADDR: return "Travel Motor Module";
        case SAFETY_RS485_LIFTER_MODULE_ADDR: return "Lifter Module";
        case SAFETY_RS485_DOCK_MODULE_ADDR:   return "Dock Module";
        default:                              return "Unknown Module";
    }
}

bool safety_rs485_is_module_critical(uint8_t module_addr) {
    for (size_t i = 0; i < SAFETY_RS485_NUM_MODULES; i++) {
        if (g_module_configs[i].module_address == module_addr) {
            return g_module_configs[i].is_critical;
        }
    }
    return false; // Unknown modules are not critical
}

uint32_t safety_rs485_get_module_timeout(uint8_t module_addr) {
    for (size_t i = 0; i < SAFETY_RS485_NUM_MODULES; i++) {
        if (g_module_configs[i].module_address == module_addr) {
            return g_module_configs[i].timeout_ms;
        }
    }
    return 10000; // Default timeout for unknown modules
}

safety_action_t safety_rs485_get_module_action(uint8_t module_addr) {
    for (size_t i = 0; i < SAFETY_RS485_NUM_MODULES; i++) {
        if (g_module_configs[i].module_address == module_addr) {
            return g_module_configs[i].safety_action;
        }
    }
    return SAFETY_RS485_ACTION_LOG_ONLY; // Default action for unknown modules
}

// 📊 **STATISTICS FUNCTIONS**

hal_status_t safety_rs485_get_statistics(safety_rs485_stats_t *stats) {
    if (!stats) {
        return HAL_ERROR;
    }
    
    if (!g_safety_rs485_initialized) {
        return HAL_ERROR;
    }
    
    memcpy(stats, &g_safety_stats, sizeof(safety_rs485_stats_t));
    return HAL_OK;
}

hal_status_t safety_rs485_get_status(safety_rs485_status_t *status) {
    if (!status) {
        return HAL_ERROR;
    }
    
    if (!g_safety_rs485_initialized) {
        return HAL_ERROR;
    }
    
    memcpy(status, &g_safety_status, sizeof(safety_rs485_status_t));
    return HAL_OK;
}

hal_status_t safety_rs485_reset_statistics(void) {
    if (!g_safety_rs485_initialized) {
        return HAL_ERROR;
    }
    
    // Save integration start time
    uint64_t start_time = g_safety_stats.integration_start_time_ms;
    
    // Reset all statistics
    memset(&g_safety_stats, 0, sizeof(g_safety_stats));
    
    // Restore integration start time
    g_safety_stats.integration_start_time_ms = start_time;
    g_safety_stats.min_response_time_ms = UINT32_MAX;
    
    safety_rs485_debug_log("Statistics reset successfully");
    return HAL_OK;
}

// 🐛 **DEBUG FUNCTIONS**

hal_status_t safety_rs485_set_debug_logging(bool enable) {
    g_debug_logging_enabled = enable;
    safety_rs485_debug_log("Debug logging %s", enable ? "ENABLED" : "DISABLED");
    return HAL_OK;
}

hal_status_t safety_rs485_get_diagnostics(char *info, size_t max_len) {
    if (!info || max_len == 0) {
        return HAL_ERROR;
    }
    
    if (!g_safety_rs485_initialized) {
        snprintf(info, max_len, "Safety RS485 integration not initialized");
        return HAL_ERROR;
    }
    
    snprintf(info, max_len,
        "Safety RS485 Integration Diagnostics:\n"
        "- Integration Active: %s\n"
        "- Safety Module Online: %s\n"
        "- Total Checks: %llu\n"
        "- Successful Checks: %llu\n"
        "- Failed Checks: %llu\n"
        "- Success Rate: %.2f%%\n"
        "- Min Response Time: %u ms\n"
        "- Max Response Time: %u ms\n"
        "- Avg Response Time: %u ms\n"
        "- Power Module Checks: %u\n"
        "- Safety Module Checks: %u\n"
        "- Travel Module Checks: %u\n"
        "- Dock Module Checks: %u\n",
        g_safety_status.integration_active ? "YES" : "NO",
        g_safety_status.safety_module_online ? "YES" : "NO",
        g_safety_stats.total_checks,
        g_safety_stats.successful_checks,
        g_safety_stats.failed_checks,
        g_safety_stats.total_checks > 0 ? 
            (float)g_safety_stats.successful_checks * 100.0f / (float)g_safety_stats.total_checks : 0.0f,
        g_safety_stats.min_response_time_ms == UINT32_MAX ? 0 : g_safety_stats.min_response_time_ms,
        g_safety_stats.max_response_time_ms,
        g_safety_stats.avg_response_time_ms,
        g_safety_stats.module_checks.power_module_checks,
        g_safety_stats.module_checks.safety_module_checks,
        g_safety_stats.module_checks.travel_module_checks,
        g_safety_stats.module_checks.dock_module_checks
    );
    
    return HAL_OK;
}

// 🧪 **SELF-TEST FUNCTION**

hal_status_t safety_rs485_self_test(void) {
    safety_rs485_debug_log("Starting Safety RS485 integration self-test...");
    
    if (!g_safety_rs485_initialized) {
        safety_rs485_error_log("Self-test failed: Integration not initialized");
        return HAL_ERROR;
    }
    
    hal_status_t overall_result = HAL_OK;
    uint32_t tests_passed = 0;
    uint32_t tests_total = 0;
    
    // Test 1: Test Safety Module (0x03) communication
    {
        tests_total++;
        safety_module_response_t response;
        hal_status_t result = safety_rs485_read_module_data(SAFETY_RS485_SAFETY_MODULE_ADDR, &response);
        
        if (result == HAL_OK) {
            safety_rs485_debug_log("✅ Test 1 PASSED: Safety Module communication OK");
            tests_passed++;
        } else {
            safety_rs485_error_log("❌ Test 1 FAILED: Safety Module communication failed");
            overall_result = HAL_ERROR;
        }
    }
    
    // Test 2: Test response validation
    {
        tests_total++;
        safety_module_response_t test_response = {0};
        test_response.safety_status = 1;
        test_response.critical_values.sensor1_distance_mm = 1000;
        test_response.critical_values.sensor2_distance_mm = 2000;
        test_response.timestamps.response_timestamp_ms = safety_rs485_get_timestamp_ms();
        test_response.timestamps.response_time_ms = 25;
        
        hal_status_t result = safety_rs485_validate_response(&test_response);
        
        if (result == HAL_OK) {
            safety_rs485_debug_log("✅ Test 2 PASSED: Response validation OK");
            tests_passed++;
        } else {
            safety_rs485_error_log("❌ Test 2 FAILED: Response validation failed");
            overall_result = HAL_ERROR;
        }
    }
    
    // Test 3: Test utility functions
    {
        tests_total++;
        const char *module_name = safety_rs485_get_module_name(SAFETY_RS485_SAFETY_MODULE_ADDR);
        bool is_critical = safety_rs485_is_module_critical(SAFETY_RS485_SAFETY_MODULE_ADDR);
        uint32_t timeout = safety_rs485_get_module_timeout(SAFETY_RS485_SAFETY_MODULE_ADDR);
        
        if (module_name && strlen(module_name) > 0 && is_critical && timeout == 0) {
            safety_rs485_debug_log("✅ Test 3 PASSED: Utility functions OK");
            tests_passed++;
        } else {
            safety_rs485_error_log("❌ Test 3 FAILED: Utility functions failed");
            overall_result = HAL_ERROR;
        }
    }
    
    safety_rs485_debug_log("Self-test completed: %u/%u tests passed", tests_passed, tests_total);
    
    if (overall_result == HAL_OK) {
        safety_rs485_debug_log("✅ ALL TESTS PASSED - Safety RS485 integration is healthy");
    } else {
        safety_rs485_error_log("❌ SOME TESTS FAILED - Safety RS485 integration has issues");
    }
    
    return overall_result;
}

// 🚨 **PLACEHOLDER FUNCTIONS FOR FUTURE PHASES**

hal_status_t safety_rs485_check_critical_modules(void) {
    // Phase 2.1 implementation placeholder
    safety_rs485_debug_log("safety_rs485_check_critical_modules() - Phase 2.1 implementation pending");
    return HAL_OK;
}

hal_status_t safety_rs485_trigger_immediate_estop(uint8_t module_addr, const char *reason) {
    // Phase 2.2 implementation placeholder
    safety_rs485_error_log("IMMEDIATE E-STOP triggered by module 0x%02X: %s", module_addr, reason ? reason : "Unknown");
    return HAL_OK;
}

hal_status_t safety_rs485_trigger_delayed_estop(uint8_t module_addr, uint32_t delay_ms, const char *reason) {
    // Phase 2.2 implementation placeholder
    safety_rs485_warning_log("DELAYED E-STOP (%u ms) triggered by module 0x%02X: %s", 
                            delay_ms, module_addr, reason ? reason : "Unknown");
    return HAL_OK;
}

hal_status_t safety_rs485_set_warning_mode(uint8_t module_addr, const char *reason) {
    // Phase 2.2 implementation placeholder
    safety_rs485_warning_log("WARNING MODE set by module 0x%02X: %s", module_addr, reason ? reason : "Unknown");
    g_safety_stats.warnings_triggered++;
    return HAL_OK;
}

hal_status_t safety_rs485_log_module_loss(uint8_t module_addr, const char *reason) {
    // Phase 2.2 implementation placeholder
    safety_rs485_warning_log("MODULE LOSS logged for module 0x%02X: %s", module_addr, reason ? reason : "Unknown");
    return HAL_OK;
}

// Phase 3 configuration functions - placeholders
hal_status_t safety_rs485_load_config(const char *config_path) {
    safety_rs485_debug_log("safety_rs485_load_config() - Phase 3 implementation pending");
    return HAL_OK;
}

hal_status_t safety_rs485_save_config(const char *config_path) {
    safety_rs485_debug_log("safety_rs485_save_config() - Phase 3 implementation pending");
    return HAL_OK;
}

hal_status_t safety_rs485_get_module_config(uint8_t module_addr, module_safety_config_t *config) {
    if (!config) return HAL_ERROR;
    
    for (size_t i = 0; i < SAFETY_RS485_NUM_MODULES; i++) {
        if (g_module_configs[i].module_address == module_addr) {
            memcpy(config, &g_module_configs[i], sizeof(module_safety_config_t));
            return HAL_OK;
        }
    }
    return HAL_ERROR;
}

hal_status_t safety_rs485_set_module_config(uint8_t module_addr, const module_safety_config_t *config) {
    safety_rs485_debug_log("safety_rs485_set_module_config() - Phase 3 implementation pending");
    return HAL_OK;
}
