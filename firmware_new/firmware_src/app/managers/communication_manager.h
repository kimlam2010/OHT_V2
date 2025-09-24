/**
 * @file communication_manager.h
 * @brief Communication Manager for RS485 and Modbus RTU protocol
 * @version 1.0.0
 * @date 2025-01-27
 * @author FW Team
 */

#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_common.h"
#include "hal_rs485.h"
#include "system_state_machine.h"
#include "module_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

// Communication Manager Status
typedef enum {
    COMM_MGR_STATUS_DISCONNECTED = 0,
    COMM_MGR_STATUS_CONNECTING,
    COMM_MGR_STATUS_CONNECTED,
    COMM_MGR_STATUS_ERROR,
    COMM_MGR_STATUS_TIMEOUT
} comm_mgr_status_t;

// Communication Manager Error Types
typedef enum {
    COMM_MGR_ERROR_NONE = 0,
    COMM_MGR_ERROR_RS485_INIT_FAILED,
    COMM_MGR_ERROR_MODBUS_INIT_FAILED,
    COMM_MGR_ERROR_TIMEOUT,
    COMM_MGR_ERROR_CRC_FAILED,
    COMM_MGR_ERROR_FRAME_ERROR,
    COMM_MGR_ERROR_BUFFER_OVERFLOW,
    COMM_MGR_ERROR_INVALID_RESPONSE,
    COMM_MGR_ERROR_COMMUNICATION_LOST
} comm_mgr_error_t;

// Use existing Modbus function codes from hal_rs485.h

// Modbus Exception Codes
typedef enum {
    MODBUS_EXCEPTION_NONE = 0x00,
    MODBUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS = 0x02,
    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE = 0x03,
    MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE = 0x04,
    MODBUS_EXCEPTION_ACKNOWLEDGE = 0x05,
    MODBUS_EXCEPTION_SLAVE_DEVICE_BUSY = 0x06,
    MODBUS_EXCEPTION_MEMORY_PARITY_ERROR = 0x08,
    MODBUS_EXCEPTION_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    MODBUS_EXCEPTION_GATEWAY_TARGET_DEVICE_FAILED = 0x0B
} modbus_exception_code_t;

// WebSocket/HTTP API Configuration
typedef struct {
    uint16_t websocket_port;               // WebSocket server port
    uint16_t http_port;                    // HTTP server port
    uint32_t max_connections;              // Maximum connections
    uint32_t heartbeat_interval_ms;        // Heartbeat interval
    uint32_t connection_timeout_ms;        // Connection timeout
    bool enable_ssl;                       // Enable SSL/TLS
    char ssl_cert_path[256];               // SSL certificate path
    char ssl_key_path[256];                // SSL private key path
} comm_mgr_api_config_t;

// Communication Manager Configuration
typedef struct {
    uint32_t baud_rate;                    // RS485 baud rate
    uint8_t data_bits;                     // Data bits (7 or 8)
    uint8_t stop_bits;                     // Stop bits (1 or 2)
    uint8_t parity;                        // Parity (0=none, 1=odd, 2=even)
    uint32_t timeout_ms;                   // Communication timeout
    uint32_t retry_count;                  // Number of retries
    uint32_t retry_delay_ms;               // Delay between retries
    uint8_t modbus_slave_id;               // Modbus slave ID
    bool enable_crc_check;                 // Enable CRC checking
    bool enable_echo_suppression;          // Enable echo suppression
    uint32_t buffer_size;                  // Communication buffer size
    comm_mgr_api_config_t api_config;      // WebSocket/HTTP API configuration
} comm_mgr_config_t;

// Communication Manager Statistics
typedef struct {
    uint32_t total_transmissions;          // Total transmissions
    uint32_t successful_transmissions;     // Successful transmissions
    uint32_t failed_transmissions;         // Failed transmissions
    uint32_t timeout_count;                // Timeout count
    uint32_t crc_error_count;              // CRC error count
    uint32_t frame_error_count;            // Frame error count
    uint32_t retry_count;                  // Total retries
    uint64_t last_communication_time;      // Last communication time
    uint32_t total_response_time;          // Total response time
    uint32_t response_count;               // Response count
    uint32_t average_response_time_ms;     // Average response time
} comm_mgr_stats_t;

// Communication Manager Status Information
typedef struct {
    comm_mgr_status_t status;              // Current status
    comm_mgr_error_t last_error;           // Last error
    uint32_t error_count;                  // Error count
    uint64_t last_error_time;              // Last error time
    bool rs485_initialized;                // RS485 initialized
    bool modbus_initialized;               // Modbus initialized
    uint8_t current_slave_id;              // Current slave ID
    uint32_t connection_uptime_ms;         // Connection uptime
    comm_mgr_stats_t statistics;           // Communication statistics
} comm_mgr_status_info_t;

// Modbus Request Structure
typedef struct {
    uint8_t slave_id;                      // Slave ID
    modbus_function_code_t function_code;  // Function code
    uint16_t start_address;                // Start address
    uint16_t quantity;                     // Quantity
    uint8_t *data;                         // Data buffer
    uint16_t data_length;                  // Data length
} comm_mgr_modbus_request_t;

// Modbus Response Structure
typedef struct {
    uint8_t slave_id;                      // Slave ID
    modbus_function_code_t function_code;  // Function code
    uint8_t *data;                         // Response data
    uint16_t data_length;                  // Data length
    modbus_exception_code_t exception_code; // Exception code
    bool is_exception;                     // Is exception response
} comm_mgr_modbus_response_t;

// Communication Event Types
typedef enum {
    COMM_MGR_EVENT_NONE = 0,
    COMM_MGR_EVENT_CONNECTED,              // Communication established
    COMM_MGR_EVENT_DISCONNECTED,           // Communication lost
    COMM_MGR_EVENT_ERROR,                  // Communication error
    COMM_MGR_EVENT_TIMEOUT,                // Communication timeout
    COMM_MGR_EVENT_RESPONSE_RECEIVED,      // Response received
    COMM_MGR_EVENT_RETRY_ATTEMPT,          // Retry attempt
    COMM_MGR_EVENT_MAX_RETRIES_EXCEEDED    // Max retries exceeded
} comm_mgr_event_t;

// Communication Event Callback
typedef void (*comm_mgr_event_callback_t)(comm_mgr_event_t event, const void *data);

// Public API Functions

/**
 * @brief Initialize Communication Manager
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t comm_manager_init(const comm_mgr_config_t *config);

/**
 * @brief Deinitialize Communication Manager
 * @return HAL status
 */
hal_status_t comm_manager_deinit(void);

/**
 * @brief Update Communication Manager
 * @return HAL status
 */
hal_status_t comm_manager_update(void);

/**
 * @brief Get Communication Manager status
 * @param status Status structure
 * @return HAL status
 */
hal_status_t comm_manager_get_status(comm_mgr_status_info_t *status);

/**
 * @brief Get Communication Manager configuration
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t comm_manager_get_config(comm_mgr_config_t *config);

/**
 * @brief Set Communication Manager configuration
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t comm_manager_set_config(const comm_mgr_config_t *config);

/**
 * @brief Get Communication Manager statistics
 * @param stats Statistics structure
 * @return HAL status
 */
hal_status_t comm_manager_get_statistics(comm_mgr_stats_t *stats);

/**
 * @brief Reset Communication Manager statistics
 * @return HAL status
 */
hal_status_t comm_manager_reset_statistics(void);

/**
 * @brief Set Communication Manager event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t comm_manager_set_callback(comm_mgr_event_callback_t callback);

// Modbus RTU Functions

/**
 * @brief Send Modbus RTU request
 * @param request Request structure
 * @param response Response structure
 * @return HAL status
 */
hal_status_t comm_manager_modbus_send_request(const comm_mgr_modbus_request_t *request, comm_mgr_modbus_response_t *response);

/**
 * @brief Read holding registers
 * @param slave_id Slave ID
 * @param start_address Start address
 * @param quantity Quantity
 * @param data Data buffer
 * @return HAL status
 */
hal_status_t comm_manager_modbus_read_holding_registers(uint8_t slave_id, uint16_t start_address, 
                                                       uint16_t quantity, uint16_t *data);

/**
 * @brief Read input registers
 * @param slave_id Slave ID
 * @param start_address Start address
 * @param quantity Quantity
 * @param data Data buffer
 * @return HAL status
 */
hal_status_t comm_manager_modbus_read_input_registers(uint8_t slave_id, uint16_t start_address, 
                                                     uint16_t quantity, uint16_t *data);

/**
 * @brief Write single register
 * @param slave_id Slave ID
 * @param address Register address
 * @param value Register value
 * @return HAL status
 */
hal_status_t comm_manager_modbus_write_single_register(uint8_t slave_id, uint16_t address, uint16_t value);

/**
 * @brief Write multiple registers
 * @param slave_id Slave ID
 * @param start_address Start address
 * @param quantity Quantity
 * @param data Data buffer
 * @return HAL status
 */
hal_status_t comm_manager_modbus_write_multiple_registers(uint8_t slave_id, uint16_t start_address, 
                                                         uint16_t quantity, const uint16_t *data);

/**
 * @brief Read coils
 * @param slave_id Slave ID
 * @param start_address Start address
 * @param quantity Quantity
 * @param data Data buffer
 * @return HAL status
 */
hal_status_t comm_manager_modbus_read_coils(uint8_t slave_id, uint16_t start_address, 
                                           uint16_t quantity, uint8_t *data);

/**
 * @brief Write single coil
 * @param slave_id Slave ID
 * @param address Coil address
 * @param value Coil value
 * @return HAL status
 */
hal_status_t comm_manager_modbus_write_single_coil(uint8_t slave_id, uint16_t address, bool value);

// Utility Functions

/**
 * @brief Get communication status name
 * @param status Communication status
 * @return Status name string
 */
const char* comm_manager_get_status_name(comm_mgr_status_t status);

/**
 * @brief Get communication error name
 * @param error Communication error
 * @return Error name string
 */
const char* comm_manager_get_error_name(comm_mgr_error_t error);

/**
 * @brief Get Modbus function code name
 * @param function_code Function code
 * @return Function code name string
 */
const char* comm_manager_get_function_code_name(modbus_function_code_t function_code);

/**
 * @brief Get Modbus exception code name
 * @param exception_code Exception code
 * @return Exception code name string
 */
const char* comm_manager_get_exception_code_name(modbus_exception_code_t exception_code);

/**
 * @brief Get Communication Manager diagnostics
 * @param info Diagnostic information buffer
 * @param max_len Maximum buffer length
 * @return HAL status
 */
hal_status_t comm_manager_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Perform Communication Manager self-test
 * @return HAL status
 */
hal_status_t comm_manager_self_test(void);

/**
 * @brief Reset Communication Manager
 * @return HAL status
 */
hal_status_t comm_manager_reset(void);

// Phase 1 scan API
/**
 * @brief Scan RS485 address range and update registry
 * @param start_addr e.g., 0x02
 * @param end_addr e.g., 0x07 (inclusive)
 * @return HAL status
 */
hal_status_t comm_manager_scan_range(uint8_t start_addr, uint8_t end_addr);

// Scan control APIs (Issue #147)
/**
 * @brief Request to stop current RS485 scan as soon as safely possible
 * @return HAL status
 */
hal_status_t comm_manager_stop_scanning(void);

/**
 * @brief Pause ongoing RS485 scan
 * @return HAL status
 */
hal_status_t comm_manager_pause_scanning(void);

/**
 * @brief Resume previously paused RS485 scan
 * @return HAL status
 */
hal_status_t comm_manager_resume_scanning(void);

/**
 * @brief Check if scan is currently active
 * @return true if active
 */
bool comm_manager_is_scanning(void);

// WebSocket/HTTP API Functions
/**
 * @brief Initialize WebSocket/HTTP API server
 * @param config API configuration
 * @return HAL status
 */
hal_status_t comm_manager_init_api_server(const comm_mgr_api_config_t *config);

/**
 * @brief Start WebSocket/HTTP API server
 * @return HAL status
 */
hal_status_t comm_manager_start_api_server(void);

/**
 * @brief Stop WebSocket/HTTP API server
 * @return HAL status
 */
hal_status_t comm_manager_stop_api_server(void);

// Health monitoring functions
float comm_manager_get_health_percentage(void);
bool comm_manager_is_hardware_detected(void);

/**
 * @brief Send telemetry data via WebSocket
 * @param data Telemetry data buffer
 * @param length Data length
 * @return HAL status
 */
hal_status_t comm_manager_send_telemetry(const uint8_t *data, size_t length);

/**
 * @brief Send status update via WebSocket
 * @param status Status data buffer
 * @param length Data length
 * @return HAL status
 */
hal_status_t comm_manager_send_status(const uint8_t *status, size_t length);

/**
 * @brief Handle HTTP API request
 * @param request HTTP request buffer
 * @param request_length Request length
 * @param response HTTP response buffer
 * @param response_length Response length (in/out)
 * @return HAL status
 */
hal_status_t comm_manager_handle_http_request(const uint8_t *request, size_t request_length,
                                             uint8_t *response, size_t *response_length);


#ifdef __cplusplus
}
#endif

#endif // COMMUNICATION_MANAGER_H
