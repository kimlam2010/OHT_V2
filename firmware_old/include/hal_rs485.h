/**
 * @file hal_rs485.h
 * @brief HAL interface for RS485/Modbus communication
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-02, EM-03 (RS485 Transceiver & UART/CAN Init)
 */

#ifndef HAL_RS485_H
#define HAL_RS485_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// RS485 Configuration
// Note: RS485_DEVICE_PATH is defined in hal_common.h as "/dev/ttyOHT485"
#define RS485_BAUD_RATE          115200
#define RS485_DATA_BITS          8
#define RS485_STOP_BITS          1
#define RS485_PARITY             0  // No parity

// Modbus Configuration
#define MODBUS_SLAVE_ID          1
#define MODBUS_TIMEOUT_MS        1000
#define MODBUS_RETRY_COUNT       3

// RS485 uses UART1 directly (no separate DE/RE pins needed)
// UART1 pins: GPIO1_D3 (TX), GPIO1_D4 (RX)

// Buffer sizes
#define RS485_BUFFER_SIZE        1024
#define MODBUS_FRAME_SIZE        256

// RS485 status
typedef enum {
    RS485_STATUS_IDLE = 0,
    RS485_STATUS_TRANSMITTING,
    RS485_STATUS_RECEIVING,
    RS485_STATUS_ERROR
} rs485_status_t;

// Modbus function codes
typedef enum {
    MODBUS_FC_READ_COILS = 0x01,
    MODBUS_FC_READ_DISCRETE_INPUTS = 0x02,
    MODBUS_FC_READ_HOLDING_REGISTERS = 0x03,
    MODBUS_FC_READ_INPUT_REGISTERS = 0x04,
    MODBUS_FC_WRITE_SINGLE_COIL = 0x05,
    MODBUS_FC_WRITE_SINGLE_REGISTER = 0x06,
    MODBUS_FC_WRITE_MULTIPLE_COILS = 0x0F,
    MODBUS_FC_WRITE_MULTIPLE_REGISTERS = 0x10
} modbus_function_code_t;

// RS485 configuration structure
typedef struct {
    char device_path[64];
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    uint32_t timeout_ms;
    uint32_t retry_count;
} rs485_config_t;

// Modbus configuration structure
typedef struct {
    uint8_t slave_id;
    uint32_t timeout_ms;
    uint32_t retry_count;
    bool enable_crc_check;
} modbus_config_t;

// RS485 statistics
typedef struct {
    uint64_t bytes_transmitted;
    uint64_t bytes_received;
    uint64_t frames_transmitted;
    uint64_t frames_received;
    uint64_t errors_crc;
    uint64_t errors_timeout;
    uint64_t errors_parity;
    uint64_t timestamp_us;
} rs485_statistics_t;

// Modbus frame structure
typedef struct {
    uint8_t slave_id;
    modbus_function_code_t function_code;
    uint16_t start_address;
    uint16_t quantity;
    uint8_t data[MODBUS_FRAME_SIZE];
    uint16_t data_length;
    uint16_t crc;
} modbus_frame_t;

// RS485 device info
typedef struct {
    hal_device_type_t device_type;
    hal_device_status_t status;
    rs485_status_t rs485_status;
    char device_name[64];
    char device_version[32];
    uint64_t timestamp_us;
    uint32_t error_count;
    uint32_t warning_count;
} rs485_device_info_t;

// HAL function prototypes
hal_status_t hal_rs485_init(const rs485_config_t *config);
hal_status_t hal_rs485_deinit(void);
hal_status_t hal_rs485_configure(const rs485_config_t *config);
hal_status_t hal_rs485_open(void);
hal_status_t hal_rs485_close(void);
hal_status_t hal_rs485_transmit(const uint8_t *data, size_t length);
hal_status_t hal_rs485_receive(uint8_t *buffer, size_t max_length, size_t *actual_length);
hal_status_t hal_rs485_get_status(rs485_status_t *status);
hal_status_t hal_rs485_get_statistics(rs485_statistics_t *statistics);
hal_status_t hal_rs485_reset_statistics(void);
hal_status_t hal_rs485_health_check(void);

// Modbus function prototypes
hal_status_t hal_modbus_init(const modbus_config_t *config);
hal_status_t hal_modbus_deinit(void);
hal_status_t hal_modbus_read_holding_registers(uint8_t slave_id, uint16_t start_addr, uint16_t quantity, uint16_t *registers);
hal_status_t hal_modbus_write_single_register(uint8_t slave_id, uint16_t address, uint16_t value);
hal_status_t hal_modbus_write_multiple_registers(uint8_t slave_id, uint16_t start_addr, uint16_t quantity, const uint16_t *registers);
hal_status_t hal_modbus_read_coils(uint8_t slave_id, uint16_t start_addr, uint16_t quantity, bool *coils);
hal_status_t hal_modbus_write_single_coil(uint8_t slave_id, uint16_t address, bool value);
hal_status_t hal_modbus_send_frame(const modbus_frame_t *frame);
hal_status_t hal_modbus_receive_frame(modbus_frame_t *frame);

// Utility functions
hal_status_t rs485_validate_config(const rs485_config_t *config);
hal_status_t modbus_validate_config(const modbus_config_t *config);
uint16_t modbus_calculate_crc(const uint8_t *data, size_t length);
bool modbus_verify_crc(const uint8_t *data, size_t length, uint16_t crc);
// No separate DE/RE pin control needed for UART1 RS485

#endif // HAL_RS485_H
