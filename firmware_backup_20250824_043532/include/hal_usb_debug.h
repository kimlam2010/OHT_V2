/**
 * @file hal_usb_debug.h
 * @brief HAL header for USB Debug Interface (CDC ACM)
 * @version 1.0.0
 * @date 2025-01-27
 * @team FIRMWARE
 * @task Task 3.1 (USB Debug Interface)
 */

#ifndef HAL_USB_DEBUG_H
#define HAL_USB_DEBUG_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// USB Debug configuration
#define USB_DEBUG_DEVICE_PATH      "/dev/ttyACM0"
#define USB_DEBUG_BAUD_RATE        115200
#define USB_DEBUG_BUFFER_SIZE      1024
#define USB_DEBUG_TIMEOUT_MS       1000
#define USB_DEBUG_MAX_FRAME_SIZE   256

// USB Debug states
typedef enum {
    USB_DEBUG_STATE_DISCONNECTED = 0,
    USB_DEBUG_STATE_CONNECTED,
    USB_DEBUG_STATE_READY,
    USB_DEBUG_STATE_TRANSMITTING,
    USB_DEBUG_STATE_RECEIVING,
    USB_DEBUG_STATE_ERROR
} usb_debug_state_t;

// USB Debug events
typedef enum {
    USB_DEBUG_EVENT_CONNECTED = 0,
    USB_DEBUG_EVENT_DISCONNECTED,
    USB_DEBUG_EVENT_DATA_RECEIVED,
    USB_DEBUG_EVENT_DATA_SENT,
    USB_DEBUG_EVENT_ERROR,
    USB_DEBUG_EVENT_FIRMWARE_UPDATE_START,
    USB_DEBUG_EVENT_FIRMWARE_UPDATE_PROGRESS,
    USB_DEBUG_EVENT_FIRMWARE_UPDATE_COMPLETE,
    USB_DEBUG_EVENT_FIRMWARE_UPDATE_ERROR
} usb_debug_event_t;

// USB Debug commands
typedef enum {
    USB_DEBUG_CMD_PING = 0x01,
    USB_DEBUG_CMD_GET_VERSION = 0x02,
    USB_DEBUG_CMD_GET_STATUS = 0x03,
    USB_DEBUG_CMD_RESET = 0x04,
    USB_DEBUG_CMD_FIRMWARE_UPDATE_START = 0x10,
    USB_DEBUG_CMD_FIRMWARE_UPDATE_DATA = 0x11,
    USB_DEBUG_CMD_FIRMWARE_UPDATE_END = 0x12,
    USB_DEBUG_CMD_FIRMWARE_UPDATE_VERIFY = 0x13,
    USB_DEBUG_CMD_GET_LOGS = 0x20,
    USB_DEBUG_CMD_CLEAR_LOGS = 0x21,
    USB_DEBUG_CMD_DEBUG_CONSOLE = 0x30
} usb_debug_command_t;

// USB Debug configuration structure
typedef struct {
    char device_path[64];
    uint32_t baud_rate;
    uint32_t timeout_ms;
    uint16_t buffer_size;
    bool auto_reconnect;
    bool debug_console_enabled;
    bool firmware_update_enabled;
} usb_debug_config_t;

// USB Debug status structure
typedef struct {
    usb_debug_state_t state;
    bool connected;
    bool console_active;
    bool firmware_update_active;
    uint32_t bytes_sent;
    uint32_t bytes_received;
    uint32_t commands_processed;
    uint32_t error_count;
    uint64_t connection_time_ms;
    uint64_t last_activity_ms;
} usb_debug_status_t;

// USB Debug statistics
typedef struct {
    uint32_t total_connections;
    uint32_t successful_commands;
    uint32_t failed_commands;
    uint32_t firmware_updates;
    uint32_t console_sessions;
    uint64_t total_bytes_transferred;
    uint32_t connection_errors;
    uint32_t timeout_errors;
} usb_debug_statistics_t;

// USB Debug frame structure
typedef struct {
    uint8_t start_marker;      // 0xAA
    uint8_t command;           // USB debug command
    uint16_t length;           // Data length
    uint8_t data[USB_DEBUG_MAX_FRAME_SIZE];
    uint16_t checksum;         // CRC16
    uint8_t end_marker;        // 0x55
} __attribute__((packed)) usb_debug_frame_t;

// USB Debug device info
typedef struct {
    hal_device_type_t device_type;
    hal_device_status_t status;
    usb_debug_state_t usb_state;
    char device_name[32];
    char device_version[16];
    char firmware_version[16];
    uint64_t timestamp_us;
    uint32_t error_count;
    uint32_t warning_count;
    bool firmware_update_supported;
    bool debug_console_supported;
} usb_debug_device_info_t;

// USB Debug firmware update info
typedef struct {
    bool active;
    uint32_t total_size;
    uint32_t received_size;
    uint8_t progress_percent;
    uint32_t chunk_count;
    uint32_t current_chunk;
    uint16_t chunk_size;
    uint32_t checksum;
    char firmware_version[16];
    uint64_t start_time_ms;
    uint64_t estimated_time_ms;
} usb_debug_firmware_update_t;

// USB Debug event callback
typedef void (*usb_debug_event_callback_t)(usb_debug_event_t event, const void *data);

// USB Debug console callback
typedef void (*usb_debug_console_callback_t)(const char *command, char *response, size_t max_response_len);

// HAL USB Debug functions
hal_status_t hal_usb_debug_init(const usb_debug_config_t *config);
hal_status_t hal_usb_debug_deinit(void);
hal_status_t hal_usb_debug_connect(void);
hal_status_t hal_usb_debug_disconnect(void);
hal_status_t hal_usb_debug_send_frame(const usb_debug_frame_t *frame);
hal_status_t hal_usb_debug_receive_frame(usb_debug_frame_t *frame, uint32_t timeout_ms);
hal_status_t hal_usb_debug_send_data(const uint8_t *data, size_t length);
hal_status_t hal_usb_debug_receive_data(uint8_t *buffer, size_t buffer_size, size_t *received_length, uint32_t timeout_ms);

// USB Debug status and info functions
hal_status_t hal_usb_debug_get_status(usb_debug_status_t *status);
hal_status_t hal_usb_debug_get_device_info(usb_debug_device_info_t *device_info);
hal_status_t hal_usb_debug_get_statistics(usb_debug_statistics_t *statistics);
hal_status_t hal_usb_debug_reset_statistics(void);

// USB Debug configuration functions
hal_status_t hal_usb_debug_set_config(const usb_debug_config_t *config);
hal_status_t hal_usb_debug_get_config(usb_debug_config_t *config);
hal_status_t hal_usb_debug_validate_config(const usb_debug_config_t *config);

// USB Debug event handling
hal_status_t hal_usb_debug_set_event_callback(usb_debug_event_callback_t callback);
hal_status_t hal_usb_debug_set_console_callback(usb_debug_console_callback_t callback);

// USB Debug command processing
hal_status_t hal_usb_debug_process_command(uint8_t command, const uint8_t *data, size_t length, uint8_t *response, size_t *response_length);
hal_status_t hal_usb_debug_send_response(uint8_t command, const uint8_t *data, size_t length);

// USB Debug console functions
hal_status_t hal_usb_debug_console_start(void);
hal_status_t hal_usb_debug_console_stop(void);
hal_status_t hal_usb_debug_console_send(const char *message);
hal_status_t hal_usb_debug_console_printf(const char *format, ...);

// USB Debug firmware update functions
hal_status_t hal_usb_debug_firmware_update_start(uint32_t firmware_size, const char *version);
hal_status_t hal_usb_debug_firmware_update_data(const uint8_t *data, size_t length);
hal_status_t hal_usb_debug_firmware_update_end(void);
hal_status_t hal_usb_debug_firmware_update_verify(void);
hal_status_t hal_usb_debug_firmware_update_abort(void);
hal_status_t hal_usb_debug_firmware_update_get_status(usb_debug_firmware_update_t *update_info);

// USB Debug utility functions
uint16_t usb_debug_calculate_checksum(const uint8_t *data, size_t length);
hal_status_t usb_debug_validate_frame(const usb_debug_frame_t *frame);
hal_status_t usb_debug_create_frame(usb_debug_frame_t *frame, uint8_t command, const uint8_t *data, size_t length);

// USB Debug health check
hal_status_t hal_usb_debug_health_check(void);
hal_status_t hal_usb_debug_self_test(void);

#endif // HAL_USB_DEBUG_H
