/**
 * @file constants.h
 * @brief System constants for OHT-50 Firmware
 * @brief MISRA C:2012 compliant constants definition
 */

#ifndef OHT50_CONSTANTS_H
#define OHT50_CONSTANTS_H

#include <stdint.h>

// HAL Status constants - Using values from hal_common.h

// ============================================================================
// SYSTEM CONSTANTS
// ============================================================================

// LED Configuration - Moved to hal_led.h
// E-Stop Configuration - Moved to hal_estop.h  
// RS485 Configuration - Moved to hal_rs485.h

// Modbus Configuration - Using values from hal_rs485.h

// Module Addresses - Updated according to CTO requirements for 4 mandatory slave modules
// Master Control Module (0x01) is running this firmware, so we only need 4 slave modules
#define MODULE_ADDR_POWER                  0x02U  // Power Module (BẮT BUỘC)
#define MODULE_ADDR_SAFETY                 0x03U  // Safety Module (BẮT BUỘC)
#define MODULE_ADDR_TRAVEL_MOTOR           0x04U  // Travel Motor Module (BẮT BUỘC)
#define MODULE_ADDR_DOCK                   0x05U  // Dock & Location Module (BẮT BUỘC)
#define MODULE_ADDR_MAX                    0x06U

// CTO Requirements - 4 Mandatory Slave Modules (excluding Master Control)
#define MANDATORY_MODULES_COUNT            4U     // Number of mandatory slave modules required
#define MANDATORY_MODULE_ADDR_START        0x02U  // Start address for mandatory slave modules
#define MANDATORY_MODULE_ADDR_END          0x05U  // End address for mandatory slave modules

// ============================================================================
// TIMING CONSTANTS
// ============================================================================

// Sleep and Delay
#define SLEEP_DELAY_MS                     200U
#define HEARTBEAT_INTERVAL_MS              500U
#define DIAGNOSTICS_INTERVAL_MS            1000U
#define POWER_POLL_INTERVAL_MS             500U

// Timeouts
#define STARTUP_DEADLINE_MS                120000U
#define STATE_TIMEOUT_MS                   5000U
#define UPDATE_PERIOD_MS                   50U
#define SAFETY_CHECK_INTERVAL_MS           50U
// Safety timeouts moved to safety_types.h

// ============================================================================
// API CONFIGURATION
// ============================================================================

// HTTP Server
#define API_HTTP_PORT                      8080U
#define API_WEBSOCKET_PORT                 8081U
#define API_MAX_REQUEST_SIZE               8192U
#define API_MAX_RESPONSE_SIZE              16384U
#define API_REQUEST_TIMEOUT_MS             30000U
#define API_WEBSOCKET_TIMEOUT_MS           60000U

// JSON Buffer Sizes
#define JSON_ERROR_BUFFER_SIZE             512U
#define JSON_SUCCESS_BUFFER_SIZE           512U
#define JSON_RESPONSE_BUFFER_SIZE          1024U
#define JSON_DIAGNOSTICS_BUFFER_SIZE       2048U

// ============================================================================
// ARRAY SIZES
// ============================================================================

// String Lengths
#define MAX_NAME_LENGTH                    32U
#define MAX_VERSION_LENGTH                 16U
#define MAX_PATH_LENGTH                    64U
#define MAX_URL_LENGTH                     128U
#define MAX_HEADER_LENGTH                  256U
#define MAX_MESSAGE_LENGTH                 512U

// Buffer Sizes
#define MAX_MODULES                        16U
#define MAX_ENDPOINTS                      32U
#define MAX_WEBSOCKET_CLIENTS              8U
#define MAX_HEADERS                        16U

// ============================================================================
// NETWORK CONFIGURATION
// ============================================================================

// Network Timeouts
#define NETWORK_TIMEOUT_MS                 5000U
#define PING_TIMEOUT_MS                    1000U
#define CONNECT_TIMEOUT_MS                 10000U

// Network Ports
#define NETWORK_HTTP_PORT                  80U
#define NETWORK_HTTPS_PORT                 443U

// ============================================================================
// SAFETY CONSTANTS
// ============================================================================

// Safety Levels - Using values from safety_types.h

// Safety Timeouts
#define SAFETY_AUTO_RECOVERY_TIMEOUT_MS    5000U
#define SAFETY_FAULT_CLEAR_TIMEOUT_MS      5000U

// ============================================================================
// POWER MODULE CONSTANTS
// ============================================================================

// Power Module Registers - Moved to power_module_handler.h
// Power Module Type - Moved to power_module_handler.h

// ============================================================================
// MOTOR MODULE CONSTANTS
// ============================================================================

// Motor Module Registers
#define MOTOR_REG_POSITION_TARGET          0x0000U
#define MOTOR_REG_VELOCITY_TARGET          0x0001U
#define MOTOR_REG_ACCELERATION_LIMIT       0x0002U
#define MOTOR_REG_CURRENT_POSITION         0x0003U
#define MOTOR_REG_CURRENT_VELOCITY         0x0004U
#define MOTOR_REG_STATUS                   0x0005U
#define MOTOR_REG_FAULT_CODE               0x0006U

// Motor Module Type
#define MOTOR_MODULE_TYPE                  0x0001U

// ============================================================================
// DOCK MODULE CONSTANTS
// ============================================================================

// Dock Module Registers
#define DOCK_REG_POSITION_TARGET           0x0000U
#define DOCK_REG_CURRENT_POSITION          0x0001U
#define DOCK_REG_STATUS                    0x0002U
#define DOCK_REG_FAULT_CODE                0x0003U

// Dock Module Type
#define DOCK_MODULE_TYPE                   0x0003U

// ============================================================================
// VALIDATION CONSTANTS
// ============================================================================

// Range Validation
#define MIN_BAUD_RATE                      9600U
#define MAX_BAUD_RATE                      115200U
#define MIN_TIMEOUT_MS                     10U
#define MAX_TIMEOUT_MS                     60000U
#define MIN_PORT_NUMBER                    1U
#define MAX_PORT_NUMBER                    65535U

// ============================================================================
// ERROR CODES
// ============================================================================

// HAL Status Codes - Using values from hal_common.h

// ============================================================================
// DEBUG CONSTANTS
// ============================================================================

// Debug Levels
#define DEBUG_LEVEL_NONE                   0U
#define DEBUG_LEVEL_ERROR                  1U
#define DEBUG_LEVEL_WARNING                2U
#define DEBUG_LEVEL_INFO                   3U
#define DEBUG_LEVEL_DEBUG                  4U

// Debug Flags
#define DEBUG_ENABLE_LOGGING               1U
#define DEBUG_ENABLE_DIAGNOSTICS           1U
#define DEBUG_ENABLE_PERFORMANCE           1U

#endif // OHT50_CONSTANTS_H
