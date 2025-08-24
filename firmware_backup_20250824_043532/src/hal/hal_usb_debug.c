/**
 * @file hal_usb_debug.c
 * @brief HAL implementation for USB Debug Interface (CDC ACM)
 * @version 1.0.0
 * @date 2025-01-27
 * @team FIRMWARE
 * @task Task 3.1 (USB Debug Interface)
 */

#include "hal_usb_debug.h"
#include "hal_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/ioctl.h>

// Internal state
static struct {
    bool initialized;
    bool connected;
    int device_fd;
    usb_debug_config_t config;
    pthread_mutex_t mutex;
    pthread_t monitor_thread;
    bool monitor_running;
    usb_debug_status_t status;
    usb_debug_statistics_t statistics;
    usb_debug_device_info_t device_info;
    usb_debug_firmware_update_t firmware_update;
    usb_debug_event_callback_t event_callback;
    usb_debug_console_callback_t console_callback;
    uint8_t rx_buffer[USB_DEBUG_BUFFER_SIZE];
    uint8_t tx_buffer[USB_DEBUG_BUFFER_SIZE];
    uint64_t last_activity_time_us;
} usb_debug_state = {0};

// Internal function prototypes
static hal_status_t usb_debug_open_device(void);
static hal_status_t usb_debug_close_device(void);
static hal_status_t usb_debug_configure_serial(void);
static void* usb_debug_monitor_thread(void *arg);
static uint64_t usb_debug_get_timestamp_us(void);
static void usb_debug_trigger_event(usb_debug_event_t event, const void *data);

/**
 * @brief Initialize USB Debug HAL
 * @param config USB Debug configuration
 * @return HAL status
 */
hal_status_t hal_usb_debug_init(const usb_debug_config_t *config)
{
    hal_status_t status;
    
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    status = hal_usb_debug_validate_config(config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    pthread_mutex_lock(&usb_debug_state.mutex);
    
    if (usb_debug_state.initialized) {
        pthread_mutex_unlock(&usb_debug_state.mutex);
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&usb_debug_state.mutex, NULL) != 0) {
        pthread_mutex_unlock(&usb_debug_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Copy configuration
    memcpy(&usb_debug_state.config, config, sizeof(usb_debug_config_t));
    
    // Initialize device info
    usb_debug_state.device_info.device_type = HAL_DEVICE_TYPE_USB;
    usb_debug_state.device_info.status = HAL_DEVICE_STATUS_INITIALIZING;
    usb_debug_state.device_info.usb_state = USB_DEBUG_STATE_DISCONNECTED;
    strcpy(usb_debug_state.device_info.device_name, "USB_Debug_CDC");
    strcpy(usb_debug_state.device_info.device_version, "1.0.0");
    strcpy(usb_debug_state.device_info.firmware_version, "1.0.0");
    usb_debug_state.device_info.timestamp_us = usb_debug_get_timestamp_us();
    usb_debug_state.device_info.error_count = 0;
    usb_debug_state.device_info.warning_count = 0;
    usb_debug_state.device_info.firmware_update_supported = true;
    usb_debug_state.device_info.debug_console_supported = true;
    
    // Initialize status
    memset(&usb_debug_state.status, 0, sizeof(usb_debug_status_t));
    usb_debug_state.status.state = USB_DEBUG_STATE_DISCONNECTED;
    usb_debug_state.status.connected = false;
    
    // Initialize statistics
    memset(&usb_debug_state.statistics, 0, sizeof(usb_debug_statistics_t));
    
    // Initialize firmware update
    memset(&usb_debug_state.firmware_update, 0, sizeof(usb_debug_firmware_update_t));
    
    usb_debug_state.initialized = true;
    usb_debug_state.connected = false;
    usb_debug_state.device_fd = -1;
    usb_debug_state.last_activity_time_us = 0;
    
    // Start monitor thread
    usb_debug_state.monitor_running = true;
    if (pthread_create(&usb_debug_state.monitor_thread, NULL, usb_debug_monitor_thread, NULL) != 0) {
        usb_debug_state.initialized = false;
        pthread_mutex_unlock(&usb_debug_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    usb_debug_state.device_info.status = HAL_DEVICE_STATUS_OK;
    
    pthread_mutex_unlock(&usb_debug_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize USB Debug HAL
 * @return HAL status
 */
hal_status_t hal_usb_debug_deinit(void)
{
    pthread_mutex_lock(&usb_debug_state.mutex);
    
    if (!usb_debug_state.initialized) {
        pthread_mutex_unlock(&usb_debug_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Stop monitor thread
    usb_debug_state.monitor_running = false;
    pthread_mutex_unlock(&usb_debug_state.mutex);
    pthread_join(usb_debug_state.monitor_thread, NULL);
    pthread_mutex_lock(&usb_debug_state.mutex);
    
    // Close device if connected
    if (usb_debug_state.connected) {
        usb_debug_close_device();
    }
    
    // Cleanup mutex
    pthread_mutex_destroy(&usb_debug_state.mutex);
    
    usb_debug_state.initialized = false;
    
    pthread_mutex_unlock(&usb_debug_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Connect to USB Debug device
 * @return HAL status
 */
hal_status_t hal_usb_debug_connect(void)
{
    if (!usb_debug_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&usb_debug_state.mutex);
    
    if (usb_debug_state.connected) {
        pthread_mutex_unlock(&usb_debug_state.mutex);
        return HAL_STATUS_OK;
    }
    
    hal_status_t status = usb_debug_open_device();
    if (status == HAL_STATUS_OK) {
        status = usb_debug_configure_serial();
        if (status == HAL_STATUS_OK) {
            usb_debug_state.connected = true;
            usb_debug_state.status.connected = true;
            usb_debug_state.status.state = USB_DEBUG_STATE_CONNECTED;
            usb_debug_state.status.connection_time_ms = hal_get_timestamp_ms();
            usb_debug_state.statistics.total_connections++;
            
            usb_debug_trigger_event(USB_DEBUG_EVENT_CONNECTED, NULL);
        } else {
            usb_debug_close_device();
        }
    }
    
    pthread_mutex_unlock(&usb_debug_state.mutex);
    
    return status;
}

/**
 * @brief Disconnect from USB Debug device
 * @return HAL status
 */
hal_status_t hal_usb_debug_disconnect(void)
{
    if (!usb_debug_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&usb_debug_state.mutex);
    
    if (!usb_debug_state.connected) {
        pthread_mutex_unlock(&usb_debug_state.mutex);
        return HAL_STATUS_OK;
    }
    
    usb_debug_close_device();
    usb_debug_state.connected = false;
    usb_debug_state.status.connected = false;
    usb_debug_state.status.state = USB_DEBUG_STATE_DISCONNECTED;
    
    usb_debug_trigger_event(USB_DEBUG_EVENT_DISCONNECTED, NULL);
    
    pthread_mutex_unlock(&usb_debug_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Send data over USB Debug interface
 * @param data Data to send
 * @param length Data length
 * @return HAL status
 */
hal_status_t hal_usb_debug_send_data(const uint8_t *data, size_t length)
{
    if (!usb_debug_state.initialized || !usb_debug_state.connected) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!data || length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&usb_debug_state.mutex);
    
    ssize_t bytes_written = write(usb_debug_state.device_fd, data, length);
    if (bytes_written != (ssize_t)length) {
        usb_debug_state.statistics.connection_errors++;
        pthread_mutex_unlock(&usb_debug_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    usb_debug_state.status.bytes_sent += bytes_written;
    usb_debug_state.statistics.total_bytes_transferred += bytes_written;
    usb_debug_state.last_activity_time_us = usb_debug_get_timestamp_us();
    
    pthread_mutex_unlock(&usb_debug_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Receive data from USB Debug interface
 * @param buffer Buffer to receive data
 * @param buffer_size Buffer size
 * @param received_length Actual received length
 * @param timeout_ms Timeout in milliseconds
 * @return HAL status
 */
hal_status_t hal_usb_debug_receive_data(uint8_t *buffer, size_t buffer_size, size_t *received_length, uint32_t timeout_ms)
{
    if (!usb_debug_state.initialized || !usb_debug_state.connected) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!buffer || buffer_size == 0 || !received_length) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    (void)timeout_ms; // TODO: Implement timeout
    
    pthread_mutex_lock(&usb_debug_state.mutex);
    
    ssize_t bytes_read = read(usb_debug_state.device_fd, buffer, buffer_size);
    if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            *received_length = 0;
            pthread_mutex_unlock(&usb_debug_state.mutex);
            return HAL_STATUS_TIMEOUT;
        } else {
            usb_debug_state.statistics.connection_errors++;
            pthread_mutex_unlock(&usb_debug_state.mutex);
            return HAL_STATUS_ERROR;
        }
    }
    
    *received_length = bytes_read;
    usb_debug_state.status.bytes_received += bytes_read;
    usb_debug_state.statistics.total_bytes_transferred += bytes_read;
    usb_debug_state.last_activity_time_us = usb_debug_get_timestamp_us();
    
    pthread_mutex_unlock(&usb_debug_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get USB Debug status
 * @param status Status structure to fill
 * @return HAL status
 */
hal_status_t hal_usb_debug_get_status(usb_debug_status_t *status)
{
    if (!status) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!usb_debug_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&usb_debug_state.mutex);
    memcpy(status, &usb_debug_state.status, sizeof(usb_debug_status_t));
    pthread_mutex_unlock(&usb_debug_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate USB Debug configuration
 * @param config Configuration to validate
 * @return HAL status
 */
hal_status_t hal_usb_debug_validate_config(const usb_debug_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (strlen(config->device_path) == 0 || strlen(config->device_path) >= 64) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->baud_rate == 0 || config->baud_rate > 115200) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->timeout_ms == 0 || config->timeout_ms > 10000) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->buffer_size == 0 || config->buffer_size > USB_DEBUG_BUFFER_SIZE) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Console printf function
 * @param format Printf format string
 * @param ... Printf arguments
 * @return HAL status
 */
hal_status_t hal_usb_debug_console_printf(const char *format, ...)
{
    if (!format) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!usb_debug_state.initialized || !usb_debug_state.connected) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (len > 0 && len < (int)sizeof(buffer)) {
        return hal_usb_debug_send_data((uint8_t*)buffer, len);
    }
    
    return HAL_STATUS_ERROR;
}

/**
 * @brief Calculate CRC16 checksum
 * @param data Data to calculate checksum for
 * @param length Data length
 * @return CRC16 checksum
 */
uint16_t usb_debug_calculate_checksum(const uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFF;
    
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

// Internal functions implementation (stubs for now)
static hal_status_t usb_debug_open_device(void) {
    usb_debug_state.device_fd = open(usb_debug_state.config.device_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    return (usb_debug_state.device_fd >= 0) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static hal_status_t usb_debug_close_device(void) {
    if (usb_debug_state.device_fd >= 0) {
        close(usb_debug_state.device_fd);
        usb_debug_state.device_fd = -1;
    }
    return HAL_STATUS_OK;
}

static hal_status_t usb_debug_configure_serial(void) {
    struct termios tty;
    
    if (tcgetattr(usb_debug_state.device_fd, &tty) != 0) {
        return HAL_STATUS_ERROR;
    }
    
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
#ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS;
#endif
    tty.c_cflag |= CREAD | CLOCAL;
    
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;
    
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;
    
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;
    
    if (tcsetattr(usb_debug_state.device_fd, TCSANOW, &tty) != 0) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

static void* usb_debug_monitor_thread(void *arg) {
    (void)arg; // Unused parameter
    
    while (usb_debug_state.monitor_running) {
        usleep(100000); // 100ms
        
        // Auto-reconnect logic
        if (usb_debug_state.config.auto_reconnect && !usb_debug_state.connected) {
            hal_usb_debug_connect();
        }
    }
    
    return NULL;
}

static uint64_t usb_debug_get_timestamp_us(void) {
    return hal_get_timestamp_us();
}

static void usb_debug_trigger_event(usb_debug_event_t event, const void *data) {
    if (usb_debug_state.event_callback) {
        usb_debug_state.event_callback(event, data);
    }
}

// Stub implementations for remaining functions
hal_status_t hal_usb_debug_send_frame(const usb_debug_frame_t *frame) { (void)frame; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_receive_frame(usb_debug_frame_t *frame, uint32_t timeout_ms) { (void)frame; (void)timeout_ms; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_get_device_info(usb_debug_device_info_t *device_info) { (void)device_info; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_get_statistics(usb_debug_statistics_t *statistics) { (void)statistics; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_reset_statistics(void) { return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_set_config(const usb_debug_config_t *config) { (void)config; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_get_config(usb_debug_config_t *config) { (void)config; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_set_event_callback(usb_debug_event_callback_t callback) { (void)callback; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_set_console_callback(usb_debug_console_callback_t callback) { (void)callback; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_process_command(uint8_t command, const uint8_t *data, size_t length, uint8_t *response, size_t *response_length) { (void)command; (void)data; (void)length; (void)response; (void)response_length; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_send_response(uint8_t command, const uint8_t *data, size_t length) { (void)command; (void)data; (void)length; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_console_start(void) { return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_console_stop(void) { return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_console_send(const char *message) { (void)message; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_firmware_update_start(uint32_t firmware_size, const char *version) { (void)firmware_size; (void)version; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_firmware_update_data(const uint8_t *data, size_t length) { (void)data; (void)length; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_firmware_update_end(void) { return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_firmware_update_verify(void) { return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_firmware_update_abort(void) { return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_firmware_update_get_status(usb_debug_firmware_update_t *update_info) { (void)update_info; return HAL_STATUS_OK; }
hal_status_t usb_debug_validate_frame(const usb_debug_frame_t *frame) { (void)frame; return HAL_STATUS_OK; }
hal_status_t usb_debug_create_frame(usb_debug_frame_t *frame, uint8_t command, const uint8_t *data, size_t length) { (void)frame; (void)command; (void)data; (void)length; return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_health_check(void) { return HAL_STATUS_OK; }
hal_status_t hal_usb_debug_self_test(void) { return HAL_STATUS_OK; }
