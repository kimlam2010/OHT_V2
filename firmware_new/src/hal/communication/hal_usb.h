/**
 * @file hal_usb.h
 * @brief HAL header for USB Communication Interface
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-16 (USB Communication Testing)
 */

#ifndef HAL_USB_H
#define HAL_USB_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// USB Device Definitions
#define USB_DEVICE_PATH         "/dev/ttyUSB0"
#define USB_BAUD_RATE           115200
#define USB_DATA_BITS           8
#define USB_STOP_BITS           1
#define USB_PARITY              0  // No parity
#define USB_TIMEOUT_MS          1000
#define USB_RETRY_COUNT         3
#define USB_BUFFER_SIZE         1024
#define USB_MAX_DEVICES         4

// USB Device Types
typedef enum {
    USB_DEVICE_TYPE_UNKNOWN = 0,
    USB_DEVICE_TYPE_SERIAL,
    USB_DEVICE_TYPE_HID,
    USB_DEVICE_TYPE_MASS_STORAGE,
    USB_DEVICE_TYPE_LIDAR,
    USB_DEVICE_TYPE_CAMERA
} usb_device_type_t;

// USB States
typedef enum {
    USB_STATE_DISCONNECTED = 0,
    USB_STATE_CONNECTING,
    USB_STATE_CONNECTED,
    USB_STATE_READY,
    USB_STATE_TRANSMITTING,
    USB_STATE_RECEIVING,
    USB_STATE_ERROR,
    USB_STATE_TIMEOUT
} usb_state_t;

// USB Configuration Structure
typedef struct {
    char device_path[64];
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    uint32_t timeout_ms;
    uint32_t retry_count;
    bool flow_control;
    bool auto_reconnect;
} usb_config_t;

// USB Device Information Structure
typedef struct {
    char device_path[64];
    usb_device_type_t device_type;
    char vendor_id[16];
    char product_id[16];
    char serial_number[32];
    uint32_t baud_rate;
    bool connected;
    uint64_t bytes_transmitted;
    uint64_t bytes_received;
    uint32_t error_count;
    uint64_t last_activity_time;
} usb_device_info_t;

// USB Packet Structure
typedef struct {
    uint8_t device_id;
    uint8_t command;
    uint16_t length;
    uint8_t data[USB_BUFFER_SIZE];
    uint16_t checksum;
    uint64_t timestamp_us;
} usb_packet_t;

// USB Statistics Structure
typedef struct {
    uint64_t packets_transmitted;
    uint64_t packets_received;
    uint64_t bytes_transmitted;
    uint64_t bytes_received;
    uint64_t errors_crc;
    uint64_t errors_timeout;
    uint64_t errors_parity;
    uint64_t errors_overflow;
    uint32_t connection_count;
    uint32_t disconnect_count;
    uint64_t uptime_seconds;
    uint64_t timestamp_us;
} usb_statistics_t;

// USB Event Callback
typedef void (*usb_event_callback_t)(usb_state_t state, usb_device_type_t device_type, const char* device_path);

// Function Prototypes

/**
 * @brief Initialize USB communication system
 * @param config USB configuration
 * @return HAL status
 */
hal_status_t hal_usb_init(const usb_config_t *config);

/**
 * @brief Deinitialize USB communication system
 * @return HAL status
 */
hal_status_t hal_usb_deinit(void);

/**
 * @brief Connect to USB device
 * @param device_path Device path
 * @return HAL status
 */
hal_status_t hal_usb_connect(const char *device_path);

/**
 * @brief Disconnect from USB device
 * @param device_path Device path
 * @return HAL status
 */
hal_status_t hal_usb_disconnect(const char *device_path);

/**
 * @brief Send data over USB
 * @param device_path Device path
 * @param data Data buffer
 * @param length Data length
 * @return HAL status
 */
hal_status_t hal_usb_send(const char *device_path, const uint8_t *data, size_t length);

/**
 * @brief Receive data from USB
 * @param device_path Device path
 * @param data Data buffer
 * @param max_length Maximum data length
 * @param actual_length Actual received length
 * @return HAL status
 */
hal_status_t hal_usb_receive(const char *device_path, uint8_t *data, size_t max_length, size_t *actual_length);

/**
 * @brief Send packet over USB
 * @param device_path Device path
 * @param packet USB packet
 * @return HAL status
 */
hal_status_t hal_usb_send_packet(const char *device_path, const usb_packet_t *packet);

/**
 * @brief Receive packet from USB
 * @param device_path Device path
 * @param packet USB packet
 * @param timeout_ms Timeout in milliseconds
 * @return HAL status
 */
hal_status_t hal_usb_receive_packet(const char *device_path, usb_packet_t *packet, uint32_t timeout_ms);

/**
 * @brief Get USB device information
 * @param device_path Device path
 * @param info Device information
 * @return HAL status
 */
hal_status_t hal_usb_get_device_info(const char *device_path, usb_device_info_t *info);

/**
 * @brief List connected USB devices
 * @param devices Device info array
 * @param max_devices Maximum number of devices
 * @param actual_count Actual device count
 * @return HAL status
 */
hal_status_t hal_usb_list_devices(usb_device_info_t *devices, size_t max_devices, size_t *actual_count);

/**
 * @brief Get USB statistics
 * @param device_path Device path
 * @param stats USB statistics
 * @return HAL status
 */
hal_status_t hal_usb_get_statistics(const char *device_path, usb_statistics_t *stats);

/**
 * @brief Reset USB statistics
 * @param device_path Device path
 * @return HAL status
 */
hal_status_t hal_usb_reset_statistics(const char *device_path);

/**
 * @brief Set USB event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t hal_usb_set_event_callback(usb_event_callback_t callback);

/**
 * @brief Check if USB device is connected
 * @param device_path Device path
 * @param connected Connection status
 * @return HAL status
 */
hal_status_t hal_usb_is_connected(const char *device_path, bool *connected);

/**
 * @brief Get USB device state
 * @param device_path Device path
 * @param state Device state
 * @return HAL status
 */
hal_status_t hal_usb_get_state(const char *device_path, usb_state_t *state);

/**
 * @brief Configure USB device
 * @param device_path Device path
 * @param config USB configuration
 * @return HAL status
 */
hal_status_t hal_usb_configure(const char *device_path, const usb_config_t *config);

/**
 * @brief Get USB configuration
 * @param device_path Device path
 * @param config USB configuration
 * @return HAL status
 */
hal_status_t hal_usb_get_config(const char *device_path, usb_config_t *config);

/**
 * @brief Calculate USB packet checksum
 * @param data Data buffer
 * @param length Data length
 * @return Checksum value
 */
uint16_t hal_usb_calculate_checksum(const uint8_t *data, size_t length);

/**
 * @brief Validate USB packet
 * @param packet USB packet
 * @return True if valid
 */
bool hal_usb_validate_packet(const usb_packet_t *packet);

/**
 * @brief Flush USB buffers
 * @param device_path Device path
 * @return HAL status
 */
hal_status_t hal_usb_flush(const char *device_path);

/**
 * @brief Reset USB device
 * @param device_path Device path
 * @return HAL status
 */
hal_status_t hal_usb_reset_device(const char *device_path);

#endif // HAL_USB_H
