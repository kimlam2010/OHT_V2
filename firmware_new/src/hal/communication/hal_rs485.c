/**
 * @file hal_rs485.c
 * @brief HAL implementation for RS485/Modbus communication
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-02, EM-03 (RS485 Transceiver & UART/CAN Init)
 */

#include "hal_rs485.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

// Internal state
static struct {
    bool initialized;
    bool device_open;
    int device_fd;
    rs485_config_t config;
    modbus_config_t modbus_config;
    pthread_mutex_t mutex;
    rs485_statistics_t statistics;
    rs485_device_info_t device_info;
    uint64_t last_operation_time_us;
    uint32_t retry_count;
    uint32_t max_retries;
    uint32_t retry_delay_ms;
} rs485_state = {0};

// Internal function prototypes
static hal_status_t rs485_open_device(void);
static hal_status_t rs485_close_device(void);
static hal_status_t rs485_configure_serial(void);
static uint64_t rs485_get_timestamp_us(void);

/**
 * @brief Initialize RS485 HAL
 * @param config RS485 configuration
 * @return HAL status
 */
hal_status_t hal_rs485_init(const rs485_config_t *config)
{
    hal_status_t status;
    
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    status = rs485_validate_config(config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (rs485_state.initialized) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&rs485_state.mutex, NULL) != 0) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Copy configuration
    memcpy(&rs485_state.config, config, sizeof(rs485_config_t));
    
    // Initialize device info
    rs485_state.device_info.device_type = HAL_DEVICE_TYPE_UART;
    rs485_state.device_info.status = HAL_DEVICE_STATUS_INITIALIZING;
    rs485_state.device_info.rs485_status = RS485_STATUS_IDLE;
    strcpy(rs485_state.device_info.device_name, "RS485_UART1");
    strcpy(rs485_state.device_info.device_version, "1.0.0");
    rs485_state.device_info.timestamp_us = rs485_get_timestamp_us();
    rs485_state.device_info.error_count = 0;
    rs485_state.device_info.warning_count = 0;
    
    // Initialize statistics
    memset(&rs485_state.statistics, 0, sizeof(rs485_statistics_t));
    rs485_state.last_operation_time_us = 0;
    
    // Initialize retry parameters
    rs485_state.retry_count = 0;
    rs485_state.max_retries = 3;
    rs485_state.retry_delay_ms = 100;
    
    rs485_state.initialized = true;
    rs485_state.device_open = false;
    rs485_state.device_fd = -1;  // Initialize device file descriptor
    
    pthread_mutex_unlock(&rs485_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize RS485 HAL
 * @return HAL status
 */
hal_status_t hal_rs485_deinit(void)
{
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Close device if open
    if (rs485_state.device_open) {
        rs485_close_device();
    }
    
    // Cleanup mutex
    pthread_mutex_destroy(&rs485_state.mutex);
    
    rs485_state.initialized = false;
    
    pthread_mutex_unlock(&rs485_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Open RS485 device
 * @return HAL status
 */
hal_status_t hal_rs485_open(void)
{
    hal_status_t status;
    
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (rs485_state.device_open) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_ALREADY_ACTIVE;
    }
    
    // Open device
    status = rs485_open_device();
    if (status == HAL_STATUS_OK) {
        // Configure serial port
        status = rs485_configure_serial();
        if (status == HAL_STATUS_OK) {
            rs485_state.device_open = true;
            rs485_state.device_info.status = HAL_DEVICE_STATUS_OK;
            rs485_state.device_info.rs485_status = RS485_STATUS_IDLE;
        } else {
            rs485_close_device();
        }
    }
    
    pthread_mutex_unlock(&rs485_state.mutex);
    
    return status;
}

/**
 * @brief Close RS485 device
 * @return HAL status
 */
hal_status_t hal_rs485_close(void)
{
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized || !rs485_state.device_open) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_OK;
    }
    
    hal_status_t status = rs485_close_device();
    rs485_state.device_open = false;
    rs485_state.device_info.status = HAL_DEVICE_STATUS_OFFLINE;
    rs485_state.device_info.rs485_status = RS485_STATUS_IDLE;
    
    pthread_mutex_unlock(&rs485_state.mutex);
    
    return status;
}

/**
 * @brief Transmit data over RS485 with retry logic
 * @param data Data to transmit
 * @param length Data length
 * @return HAL status
 */
hal_status_t hal_rs485_transmit(const uint8_t *data, size_t length)
{
    if (!data || length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized || !rs485_state.device_open) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("[HAL-RS485-TX] Transmitting %zu bytes to %s\n", length, rs485_state.config.device_path);
    
    hal_status_t result = HAL_STATUS_ERROR;
    uint32_t current_retry = 0;
    uint32_t current_delay = rs485_state.retry_delay_ms;
    
    while (current_retry <= rs485_state.max_retries) {
        // Update status for transmission
        rs485_state.device_info.rs485_status = RS485_STATUS_TRANSMITTING;
        
        // Write data directly to UART1
        ssize_t written = write(rs485_state.device_fd, data, length);
        
        // Update status
        rs485_state.device_info.rs485_status = RS485_STATUS_IDLE;
        
        if (written == (ssize_t)length) {
            // Success - update statistics
            rs485_state.statistics.bytes_transmitted += length;
            rs485_state.statistics.frames_transmitted++;
            rs485_state.statistics.timestamp_us = rs485_get_timestamp_us();
            rs485_state.last_operation_time_us = rs485_state.statistics.timestamp_us;
            rs485_state.retry_count = 0; // Reset retry count on success
            
            printf("[HAL-RS485-TX] Success: %zu bytes written\n", written);
            result = HAL_STATUS_OK;
            break;
        } else {
            // Failure - update error statistics
            rs485_state.statistics.errors_timeout++;
            rs485_state.device_info.error_count++;
            rs485_state.retry_count++;
            
            printf("[HAL-RS485-TX] Error: written=%zd, expected=%zu, retry=%u/%u\n", 
                   written, length, current_retry, rs485_state.max_retries);
            
            if (current_retry < rs485_state.max_retries) {
                // Exponential backoff: delay *= 2
                usleep(current_delay * 1000);
                current_delay *= 2;
                current_retry++;
            } else {
                // Max retries reached
                printf("[HAL-RS485-TX] Max retries reached, giving up\n");
                result = HAL_STATUS_IO_ERROR;
                break;
            }
        }
    }
    
    pthread_mutex_unlock(&rs485_state.mutex);
    return result;
}

/**
 * @brief Receive data from RS485
 * @param buffer Buffer to store received data
 * @param max_length Maximum buffer length
 * @param actual_length Actual received length
 * @return HAL status
 */
hal_status_t hal_rs485_receive(uint8_t *buffer, size_t max_length, size_t *actual_length)
{
    if (!buffer || !actual_length || max_length == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized || !rs485_state.device_open) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Update status for reception
    rs485_state.device_info.rs485_status = RS485_STATUS_RECEIVING;
    
    // Read data with timeout
    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(rs485_state.device_fd, &read_fds);
    
    timeout.tv_sec = rs485_state.config.timeout_ms / 1000;
    timeout.tv_usec = (rs485_state.config.timeout_ms % 1000) * 1000;
    
    printf("[HAL-RS485-RX] Waiting for data (timeout=%u ms)...\n", rs485_state.config.timeout_ms);
    int select_result = select(rs485_state.device_fd + 1, &read_fds, NULL, NULL, &timeout);
    
    if (select_result > 0) {
        ssize_t received = read(rs485_state.device_fd, buffer, max_length);
        
        if (received > 0) {
            *actual_length = received;
            
            // Update statistics
            rs485_state.statistics.bytes_received += received;
            rs485_state.statistics.frames_received++;
            rs485_state.statistics.timestamp_us = rs485_get_timestamp_us();
            rs485_state.last_operation_time_us = rs485_state.statistics.timestamp_us;
            
            printf("[HAL-RS485-RX] Success: received %zd bytes\n", received);
            
            // Update status
            rs485_state.device_info.rs485_status = RS485_STATUS_IDLE;
            
            pthread_mutex_unlock(&rs485_state.mutex);
            return HAL_STATUS_OK;
        } else {
            printf("[HAL-RS485-RX] Read error: received=%zd\n", received);
        }
    } else if (select_result == 0) {
        printf("[HAL-RS485-RX] Timeout after %u ms\n", rs485_state.config.timeout_ms);
    } else {
        printf("[HAL-RS485-RX] Select error: %d\n", select_result);
    }
    
    // Update status
    rs485_state.device_info.rs485_status = RS485_STATUS_IDLE;
    
    // Update error statistics
    rs485_state.statistics.errors_timeout++;
    rs485_state.device_info.error_count++;
    
    pthread_mutex_unlock(&rs485_state.mutex);
    return HAL_STATUS_TIMEOUT;
}

/**
 * @brief Get RS485 status
 * @param status Pointer to status structure
 * @return HAL status
 */
hal_status_t hal_rs485_get_status(rs485_status_t *status)
{
    if (!status) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    *status = rs485_state.device_info.rs485_status;
    
    pthread_mutex_unlock(&rs485_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get RS485 statistics
 * @param statistics Pointer to statistics structure
 * @return HAL status
 */
hal_status_t hal_rs485_get_statistics(rs485_statistics_t *statistics)
{
    if (!statistics) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memcpy(statistics, &rs485_state.statistics, sizeof(rs485_statistics_t));
    
    pthread_mutex_unlock(&rs485_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Reset RS485 statistics
 * @return HAL status
 */
hal_status_t hal_rs485_reset_statistics(void)
{
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    memset(&rs485_state.statistics, 0, sizeof(rs485_statistics_t));
    rs485_state.statistics.timestamp_us = rs485_get_timestamp_us();
    
    pthread_mutex_unlock(&rs485_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Health check
 * @return HAL status
 */
hal_status_t hal_rs485_health_check(void)
{
    pthread_mutex_lock(&rs485_state.mutex);
    
    if (!rs485_state.initialized) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check if device is open and accessible
    if (!rs485_state.device_open) {
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Simple health check - try to get device status
    int flags = fcntl(rs485_state.device_fd, F_GETFL);
    if (flags == -1) {
        rs485_state.device_info.status = HAL_DEVICE_STATUS_ERROR;
        pthread_mutex_unlock(&rs485_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    rs485_state.device_info.status = HAL_DEVICE_STATUS_OK;
    
    pthread_mutex_unlock(&rs485_state.mutex);
    
    return HAL_STATUS_OK;
}

// Modbus functions (stubs for now)
hal_status_t hal_modbus_init(const modbus_config_t *config __attribute__((unused))) { return HAL_STATUS_OK; }
hal_status_t hal_modbus_deinit(void) { return HAL_STATUS_OK; }
hal_status_t hal_modbus_read_holding_registers(uint8_t slave_id __attribute__((unused)), uint16_t start_addr __attribute__((unused)), uint16_t quantity __attribute__((unused)), uint16_t *registers __attribute__((unused))) { return HAL_STATUS_OK; }
hal_status_t hal_modbus_write_single_register(uint8_t slave_id __attribute__((unused)), uint16_t address __attribute__((unused)), uint16_t value __attribute__((unused))) { return HAL_STATUS_OK; }
hal_status_t hal_modbus_write_multiple_registers(uint8_t slave_id __attribute__((unused)), uint16_t start_addr __attribute__((unused)), uint16_t quantity __attribute__((unused)), const uint16_t *registers __attribute__((unused))) { return HAL_STATUS_OK; }
hal_status_t hal_modbus_read_coils(uint8_t slave_id __attribute__((unused)), uint16_t start_addr __attribute__((unused)), uint16_t quantity __attribute__((unused)), bool *coils __attribute__((unused))) { return HAL_STATUS_OK; }
hal_status_t hal_modbus_write_single_coil(uint8_t slave_id __attribute__((unused)), uint16_t address __attribute__((unused)), bool value __attribute__((unused))) { return HAL_STATUS_OK; }
hal_status_t hal_modbus_send_frame(const modbus_frame_t *frame __attribute__((unused))) { return HAL_STATUS_OK; }
hal_status_t hal_modbus_receive_frame(modbus_frame_t *frame __attribute__((unused))) { return HAL_STATUS_OK; }

// Utility functions
hal_status_t rs485_validate_config(const rs485_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (strlen(config->device_path) == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->baud_rate == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t modbus_validate_config(const modbus_config_t *config __attribute__((unused))) { return HAL_STATUS_OK; }

uint16_t modbus_calculate_crc(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    
    return crc;
}

bool modbus_verify_crc(const uint8_t *data, size_t length, uint16_t crc) {
    uint16_t calculated_crc = modbus_calculate_crc(data, length);
    return (calculated_crc == crc);
}
// No separate DE/RE pin control needed for UART1 RS485

// Internal functions
static hal_status_t rs485_open_device(void) {
    // Open RS485 device file
    rs485_state.device_fd = open(rs485_state.config.device_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (rs485_state.device_fd == -1) {
        printf("Failed to open RS485 device %s: %s\n", rs485_state.config.device_path, strerror(errno));
        return HAL_STATUS_IO_ERROR;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t rs485_close_device(void) {
    if (rs485_state.device_fd != -1) {
        close(rs485_state.device_fd);
        rs485_state.device_fd = -1;
    }
    return HAL_STATUS_OK;
}

static hal_status_t rs485_configure_serial(void) {
    struct termios tty;
    
    // Get current settings
    if (tcgetattr(rs485_state.device_fd, &tty) != 0) {
        printf("Failed to get serial attributes: %s\n", strerror(errno));
        return HAL_STATUS_IO_ERROR;
    }
    
    // Set baud rate
    speed_t baud_rate;
    switch (rs485_state.config.baud_rate) {
        case 9600: baud_rate = B9600; break;
        case 19200: baud_rate = B19200; break;
        case 38400: baud_rate = B38400; break;
        case 57600: baud_rate = B57600; break;
        case 115200: baud_rate = B115200; break;
        default: baud_rate = B9600; break;
    }
    
    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);
    
    // Set data bits, stop bits, parity
    tty.c_cflag &= ~PARENB;  // No parity
    tty.c_cflag &= ~CSTOPB;  // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      // 8 data bits
    
    // Set other flags
    tty.c_cflag |= CREAD | CLOCAL;  // Enable receiver, ignore modem control lines
    #ifdef CRTSCTS
    tty.c_cflag &= ~CRTSCTS;        // No hardware flow control
    #endif
    
    // Set input flags
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // No software flow control
    tty.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Raw input
    
    // Set output flags
    tty.c_oflag &= ~OPOST;  // Raw output
    
    // Set local flags
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Raw mode
    
    // Set timeout
    tty.c_cc[VTIME] = rs485_state.config.timeout_ms / 100;  // Timeout in deciseconds
    tty.c_cc[VMIN] = 0;  // Non-blocking
    
    // Apply settings
    if (tcsetattr(rs485_state.device_fd, TCSANOW, &tty) != 0) {
        printf("Failed to set serial attributes: %s\n", strerror(errno));
        return HAL_STATUS_IO_ERROR;
    }
    
    return HAL_STATUS_OK;
}

static uint64_t rs485_get_timestamp_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}
