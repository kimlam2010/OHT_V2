/**
 * @file hal_usb.c
 * @brief HAL implementation for USB Communication Interface
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-16 (USB Communication Testing)
 */

#include "hal_usb.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <pthread.h>

// Internal state
static bool usb_initialized = false;
static usb_config_t current_config;
static usb_event_callback_t event_callback = NULL;
static pthread_mutex_t usb_mutex = PTHREAD_MUTEX_INITIALIZER;

// Device tracking
static usb_device_info_t connected_devices[USB_MAX_DEVICES];
static size_t device_count = 0;

// Statistics
static usb_statistics_t global_statistics;

hal_status_t hal_usb_init(const usb_config_t *config)
{
    if (!config) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    if (usb_initialized) {
        pthread_mutex_unlock(&usb_mutex);
        return HAL_STATUS_ERROR;
    }
    
    memcpy(&current_config, config, sizeof(usb_config_t));
    memset(&global_statistics, 0, sizeof(usb_statistics_t));
    memset(connected_devices, 0, sizeof(connected_devices));
    device_count = 0;
    
    usb_initialized = true;
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_OK;
}

hal_status_t hal_usb_deinit(void)
{
    pthread_mutex_lock(&usb_mutex);
    
    if (!usb_initialized) {
        pthread_mutex_unlock(&usb_mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Disconnect all devices
    for (size_t i = 0; i < device_count; i++) {
        if (connected_devices[i].connected) {
            hal_usb_disconnect(connected_devices[i].device_path);
        }
    }
    
    usb_initialized = false;
    device_count = 0;
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_OK;
}

hal_status_t hal_usb_connect(const char *device_path)
{
    if (!device_path || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    // Check if device already connected
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            if (connected_devices[i].connected) {
                pthread_mutex_unlock(&usb_mutex);
                return HAL_STATUS_ERROR;
            }
        }
    }
    
    // Add new device
    if (device_count >= USB_MAX_DEVICES) {
        pthread_mutex_unlock(&usb_mutex);
        return HAL_STATUS_ERROR;
    }
    
    strcpy(connected_devices[device_count].device_path, device_path);
    connected_devices[device_count].device_type = USB_DEVICE_TYPE_UNKNOWN;
    connected_devices[device_count].connected = true;
    connected_devices[device_count].baud_rate = current_config.baud_rate;
    connected_devices[device_count].bytes_transmitted = 0;
    connected_devices[device_count].bytes_received = 0;
    connected_devices[device_count].error_count = 0;
    connected_devices[device_count].last_activity_time = 0;
    
    device_count++;
    global_statistics.connection_count++;
    
    if (event_callback) {
        event_callback(USB_STATE_CONNECTED, USB_DEVICE_TYPE_UNKNOWN, device_path);
    }
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_OK;
}

hal_status_t hal_usb_disconnect(const char *device_path)
{
    if (!device_path || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            if (connected_devices[i].connected) {
                connected_devices[i].connected = false;
                global_statistics.disconnect_count++;
                
                if (event_callback) {
                    event_callback(USB_STATE_DISCONNECTED, connected_devices[i].device_type, device_path);
                }
                
                pthread_mutex_unlock(&usb_mutex);
                return HAL_STATUS_OK;
            }
        }
    }
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_ERROR;
}

hal_status_t hal_usb_send(const char *device_path, const uint8_t *data, size_t length)
{
    if (!device_path || !data || length == 0 || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    // Find device
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            if (connected_devices[i].connected) {
                // Simulate data transmission
                connected_devices[i].bytes_transmitted += length;
                global_statistics.bytes_transmitted += length;
                global_statistics.packets_transmitted++;
                
                if (event_callback) {
                    event_callback(USB_STATE_TRANSMITTING, connected_devices[i].device_type, device_path);
                }
                
                pthread_mutex_unlock(&usb_mutex);
                return HAL_STATUS_OK;
            }
        }
    }
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_ERROR;
}

hal_status_t hal_usb_receive(const char *device_path, uint8_t *data, size_t max_length, size_t *actual_length)
{
    if (!device_path || !data || !actual_length || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    // Find device
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            if (connected_devices[i].connected) {
                // Simulate data reception
                *actual_length = 0; // No data available in simulation
                
                if (event_callback) {
                    event_callback(USB_STATE_RECEIVING, connected_devices[i].device_type, device_path);
                }
                
                pthread_mutex_unlock(&usb_mutex);
                return HAL_STATUS_TIMEOUT;
            }
        }
    }
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_ERROR;
}

hal_status_t hal_usb_send_packet(const char *device_path, const usb_packet_t *packet)
{
    if (!device_path || !packet || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    return hal_usb_send(device_path, (const uint8_t*)packet, sizeof(usb_packet_t));
}

hal_status_t hal_usb_receive_packet(const char *device_path, usb_packet_t *packet, uint32_t timeout_ms)
{
    if (!device_path || !packet || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    size_t actual_length;
    return hal_usb_receive(device_path, (uint8_t*)packet, sizeof(usb_packet_t), &actual_length);
}

hal_status_t hal_usb_get_device_info(const char *device_path, usb_device_info_t *info)
{
    if (!device_path || !info || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            memcpy(info, &connected_devices[i], sizeof(usb_device_info_t));
            pthread_mutex_unlock(&usb_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_ERROR;
}

hal_status_t hal_usb_list_devices(usb_device_info_t *devices, size_t max_devices, size_t *actual_count)
{
    if (!devices || !actual_count || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    size_t count = (device_count < max_devices) ? device_count : max_devices;
    memcpy(devices, connected_devices, count * sizeof(usb_device_info_t));
    *actual_count = count;
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_OK;
}

hal_status_t hal_usb_get_statistics(const char *device_path, usb_statistics_t *stats)
{
    if (!stats || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    if (device_path) {
        // Get device-specific statistics
        for (size_t i = 0; i < device_count; i++) {
            if (strcmp(connected_devices[i].device_path, device_path) == 0) {
                memset(stats, 0, sizeof(usb_statistics_t));
                stats->bytes_transmitted = connected_devices[i].bytes_transmitted;
                stats->bytes_received = connected_devices[i].bytes_received;
                stats->connection_count = 1;
                stats->disconnect_count = 0;
                pthread_mutex_unlock(&usb_mutex);
                return HAL_STATUS_OK;
            }
        }
        pthread_mutex_unlock(&usb_mutex);
        return HAL_STATUS_ERROR;
    } else {
        // Get global statistics
        memcpy(stats, &global_statistics, sizeof(usb_statistics_t));
        pthread_mutex_unlock(&usb_mutex);
        return HAL_STATUS_OK;
    }
}

hal_status_t hal_usb_reset_statistics(const char *device_path)
{
    if (!usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    if (device_path) {
        // Reset device-specific statistics
        for (size_t i = 0; i < device_count; i++) {
            if (strcmp(connected_devices[i].device_path, device_path) == 0) {
                connected_devices[i].bytes_transmitted = 0;
                connected_devices[i].bytes_received = 0;
                connected_devices[i].error_count = 0;
                pthread_mutex_unlock(&usb_mutex);
                return HAL_STATUS_OK;
            }
        }
        pthread_mutex_unlock(&usb_mutex);
        return HAL_STATUS_ERROR;
    } else {
        // Reset global statistics
        memset(&global_statistics, 0, sizeof(usb_statistics_t));
        pthread_mutex_unlock(&usb_mutex);
        return HAL_STATUS_OK;
    }
}

hal_status_t hal_usb_set_event_callback(usb_event_callback_t callback)
{
    if (!usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    event_callback = callback;
    pthread_mutex_unlock(&usb_mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_usb_is_connected(const char *device_path, bool *connected)
{
    if (!device_path || !connected || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            *connected = connected_devices[i].connected;
            pthread_mutex_unlock(&usb_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    *connected = false;
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_OK;
}

hal_status_t hal_usb_get_state(const char *device_path, usb_state_t *state)
{
    if (!device_path || !state || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            if (connected_devices[i].connected) {
                *state = USB_STATE_READY;
            } else {
                *state = USB_STATE_DISCONNECTED;
            }
            pthread_mutex_unlock(&usb_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    *state = USB_STATE_DISCONNECTED;
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_OK;
}

hal_status_t hal_usb_configure(const char *device_path, const usb_config_t *config)
{
    if (!device_path || !config || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            connected_devices[i].baud_rate = config->baud_rate;
            pthread_mutex_unlock(&usb_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_ERROR;
}

hal_status_t hal_usb_get_config(const char *device_path, usb_config_t *config)
{
    if (!device_path || !config || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            memcpy(config, &current_config, sizeof(usb_config_t));
            strcpy(config->device_path, device_path);
            config->baud_rate = connected_devices[i].baud_rate;
            pthread_mutex_unlock(&usb_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_ERROR;
}

uint16_t hal_usb_calculate_checksum(const uint8_t *data, size_t length)
{
    if (!data || length == 0) {
        return 0;
    }
    
    uint16_t checksum = 0;
    for (size_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    
    return checksum;
}

bool hal_usb_validate_packet(const usb_packet_t *packet)
{
    if (!packet) {
        return false;
    }
    
    uint16_t calculated_checksum = hal_usb_calculate_checksum(packet->data, packet->length);
    return (calculated_checksum == packet->checksum);
}

hal_status_t hal_usb_flush(const char *device_path)
{
    if (!device_path || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    // Simulate buffer flush
    return HAL_STATUS_OK;
}

hal_status_t hal_usb_reset_device(const char *device_path)
{
    if (!device_path || !usb_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_lock(&usb_mutex);
    
    for (size_t i = 0; i < device_count; i++) {
        if (strcmp(connected_devices[i].device_path, device_path) == 0) {
            connected_devices[i].bytes_transmitted = 0;
            connected_devices[i].bytes_received = 0;
            connected_devices[i].error_count = 0;
            pthread_mutex_unlock(&usb_mutex);
            return HAL_STATUS_OK;
        }
    }
    
    pthread_mutex_unlock(&usb_mutex);
    return HAL_STATUS_ERROR;
}
