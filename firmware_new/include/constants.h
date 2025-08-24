/**
 * @file constants.h
 * @brief System constants for OHT-50 Firmware
 * @brief MISRA C:2012 compliant constants definition
 */

#ifndef OHT50_CONSTANTS_H
#define OHT50_CONSTANTS_H

#include <stdint.h>

// ============================================================================
// SYSTEM CONSTANTS
// ============================================================================

// LED Configuration
#define LED_COUNT                          5U
#define LED_POWER_PIN                      1U
#define LED_SYSTEM_PIN                     2U
#define LED_COMM_PIN                       3U
#define LED_NETWORK_PIN                    4U
#define LED_ERROR_PIN                      5U

// E-Stop Configuration
#define ESTOP_PIN                          6U
#define ESTOP_RESPONSE_TIME_MS             100U
#define ESTOP_DEBOUNCE_TIME_MS             50U

// RS485 Configuration
#define RS485_DEVICE_PATH                  "/dev/ttyOHT485"
#define RS485_BAUD_RATE                    115200U
#define RS485_DATA_BITS                    8U
#define RS485_STOP_BITS                    1U
#define RS485_PARITY                       0U
#define RS485_TIMEOUT_MS                   1500U
#define RS485_RETRY_COUNT                  2U
#define RS485_RETRY_DELAY_MS               100U
#define RS485_BUFFER_SIZE                  256U

// Modbus Configuration
#define MODBUS_TIMEOUT_MS                  1500U
#define MODBUS_RETRY_COUNT                 2U
#define MODBUS_SLAVE_ID                    1U

// Module Addresses
#define MODULE_ADDR_POWER                  0x02U
#define MODULE_ADDR_MOTOR                  0x03U
#define MODULE_ADDR_IO                     0x04U
#define MODULE_ADDR_DOCK                   0x05U
#define MODULE_ADDR_SENSOR                 0x06U
#define MODULE_ADDR_MAX                    0x07U

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
#define SAFETY_CIRCUIT_TIMEOUT_MS          100U
#define SENSOR_TIMEOUT_MS                  100U

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

// Safety Levels
#define SAFETY_LEVEL_NORMAL                0U
#define SAFETY_LEVEL_WARNING               1U
#define SAFETY_LEVEL_CRITICAL              2U
#define SAFETY_LEVEL_EMERGENCY             3U

// Safety Timeouts
#define SAFETY_AUTO_RECOVERY_TIMEOUT_MS    5000U
#define SAFETY_FAULT_CLEAR_TIMEOUT_MS      5000U

// ============================================================================
// POWER MODULE CONSTANTS
// ============================================================================

// Power Module Registers
#define POWER_REG_VOLTAGE_MAIN             0x0000U
#define POWER_REG_CURRENT_MAIN             0x0001U
#define POWER_REG_TEMP_MAIN                0x0002U
#define POWER_REG_RELAY1_STATUS            0x0004U
#define POWER_REG_RELAY2_STATUS            0x0005U
#define POWER_REG_RELAY1_CONTROL           0x0006U
#define POWER_REG_RELAY2_CONTROL           0x0007U
#define POWER_REG_ALARM_STATUS             0x0008U
#define POWER_REG_DEVICE_ID                0x00F0U
#define POWER_REG_MODULE_TYPE              0x00F7U

// Power Module Type
#define POWER_MODULE_TYPE                  0x0002U

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

// HAL Status Codes
#define HAL_STATUS_OK                      0
#define HAL_STATUS_ERROR                   -1
#define HAL_STATUS_INVALID_PARAMETER       -2
#define HAL_STATUS_NOT_INITIALIZED         -3
#define HAL_STATUS_TIMEOUT                 -4
#define HAL_STATUS_BUSY                    -5

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
