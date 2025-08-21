/**
 * @file hal_lidar.c
 * @brief HAL implementation for RPLIDAR C1M1 sensor
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-12 (LiDAR Driver & USB Integration)
 */

#include "hal_lidar.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>

// Internal state
static struct {
    bool initialized;
    bool scanning;
    int device_fd;
    lidar_config_t config;
    pthread_mutex_t mutex;
    pthread_t scan_thread;
    lidar_scan_data_t current_scan;
    lidar_safety_status_t safety_status;
    lidar_device_info_t device_info;
    uint64_t last_scan_timestamp_us;
    uint32_t scan_count;
    uint32_t error_count;
} lidar_state = {0};

// Internal function prototypes
static void* lidar_scan_thread(void *arg);
static hal_status_t lidar_open_device(void);
static hal_status_t lidar_close_device(void);
static hal_status_t lidar_configure_serial(void);
static hal_status_t lidar_send_command(const uint8_t *command, size_t len);
static hal_status_t lidar_read_response(uint8_t *buffer, size_t max_len, size_t *actual_len);
static uint64_t lidar_get_timestamp_us(void);
static hal_status_t lidar_parse_scan_data(const uint8_t *data, size_t len, lidar_scan_data_t *scan_data);
static hal_status_t lidar_process_safety_status(void);

/**
 * @brief Initialize LiDAR HAL
 * @param config LiDAR configuration
 * @return HAL status
 */
hal_status_t hal_lidar_init(const lidar_config_t *config)
{
    hal_status_t status;
    
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    status = lidar_validate_config(config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (lidar_state.initialized) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize mutex
    if (pthread_mutex_init(&lidar_state.mutex, NULL) != 0) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Copy configuration
    memcpy(&lidar_state.config, config, sizeof(lidar_config_t));
    
    // Open device
    status = lidar_open_device();
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return status;
    }
    
    // Configure serial port
    status = lidar_configure_serial();
    if (status != HAL_STATUS_OK) {
        lidar_close_device();
        pthread_mutex_unlock(&lidar_state.mutex);
        return status;
    }
    
    // Initialize scan data
    memset(&lidar_state.current_scan, 0, sizeof(lidar_scan_data_t));
    memset(&lidar_state.safety_status, 0, sizeof(lidar_safety_status_t));
    memset(&lidar_state.device_info, 0, sizeof(lidar_device_info_t));
    
    // Initialize statistics
    lidar_state.last_scan_timestamp_us = 0;
    lidar_state.scan_count = 0;
    lidar_state.error_count = 0;
    
    lidar_state.initialized = true;
    lidar_state.scanning = false;
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize LiDAR HAL
 * @return HAL status
 */
hal_status_t hal_lidar_deinit(void)
{
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (!lidar_state.initialized) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Stop scanning if active
    if (lidar_state.scanning) {
        hal_lidar_stop_scanning();
    }
    
    // Close device
    lidar_close_device();
    
    // Cleanup mutex
    pthread_mutex_destroy(&lidar_state.mutex);
    
    lidar_state.initialized = false;
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Start LiDAR scanning
 * @return HAL status
 */
hal_status_t hal_lidar_start_scanning(void)
{
    hal_status_t status;
    
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (!lidar_state.initialized) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (lidar_state.scanning) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_ALREADY_ACTIVE;
    }
    
    // Send start scan command
    uint8_t start_cmd[] = {LIDAR_START_FLAG, LIDAR_CMD_START_SCAN};
    status = lidar_send_command(start_cmd, sizeof(start_cmd));
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return status;
    }
    
    // Start scan thread
    if (pthread_create(&lidar_state.scan_thread, NULL, lidar_scan_thread, NULL) != 0) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    lidar_state.scanning = true;
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Stop LiDAR scanning
 * @return HAL status
 */
hal_status_t hal_lidar_stop_scanning(void)
{
    hal_status_t status;
    
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (!lidar_state.initialized || !lidar_state.scanning) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_OK;
    }
    
    // Send stop scan command
    uint8_t stop_cmd[] = {LIDAR_START_FLAG, LIDAR_CMD_STOP_SCAN};
    status = lidar_send_command(stop_cmd, sizeof(stop_cmd));
    
    lidar_state.scanning = false;
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    // Wait for scan thread to finish
    pthread_join(lidar_state.scan_thread, NULL);
    
    return status;
}

/**
 * @brief Get current scan data
 * @param scan_data Pointer to scan data structure
 * @return HAL status
 */
hal_status_t hal_lidar_get_scan_data(lidar_scan_data_t *scan_data)
{
    if (!scan_data) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (!lidar_state.initialized) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Copy current scan data
    memcpy(scan_data, &lidar_state.current_scan, sizeof(lidar_scan_data_t));
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Check safety status
 * @param safety_status Pointer to safety status structure
 * @return HAL status
 */
hal_status_t hal_lidar_check_safety(lidar_safety_status_t *safety_status)
{
    if (!safety_status) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (!lidar_state.initialized) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Process safety status from current scan
    hal_status_t status = lidar_process_safety_status();
    if (status == HAL_STATUS_OK) {
        // Copy safety status
        memcpy(safety_status, &lidar_state.safety_status, sizeof(lidar_safety_status_t));
    }
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return status;
}

/**
 * @brief Get device information
 * @param device_info Pointer to device info structure
 * @return HAL status
 */
hal_status_t hal_lidar_get_device_info(lidar_device_info_t *device_info)
{
    if (!device_info) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (!lidar_state.initialized) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Send get device info command
    uint8_t info_cmd[] = {LIDAR_START_FLAG, LIDAR_CMD_GET_INFO};
    hal_status_t status = lidar_send_command(info_cmd, sizeof(info_cmd));
    if (status == HAL_STATUS_OK) {
        // Read response and parse device info
        uint8_t response[32];
        size_t actual_len;
        status = lidar_read_response(response, sizeof(response), &actual_len);
        if (status == HAL_STATUS_OK && actual_len >= 20) {
            // Parse device info from response
            lidar_state.device_info.model = response[0];
            lidar_state.device_info.firmware_version = response[1];
            lidar_state.device_info.hardware_version = response[2];
            memcpy(&lidar_state.device_info.serial_number, &response[3], 4);
            lidar_state.device_info.device_healthy = (response[7] == 0);
            lidar_state.device_info.timestamp_us = lidar_get_timestamp_us();
        }
    }
    
    // Copy device info
    memcpy(device_info, &lidar_state.device_info, sizeof(lidar_device_info_t));
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return status;
}

/**
 * @brief Reset LiDAR device
 * @return HAL status
 */
hal_status_t hal_lidar_reset(void)
{
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (!lidar_state.initialized) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Send reset command
    uint8_t reset_cmd[] = {LIDAR_START_FLAG, LIDAR_CMD_RESET};
    hal_status_t status = lidar_send_command(reset_cmd, sizeof(reset_cmd));
    
    // Wait for reset to complete
    usleep(2000000); // 2 seconds
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return status;
}

/**
 * @brief Health check
 * @return HAL status
 */
hal_status_t hal_lidar_health_check(void)
{
    pthread_mutex_lock(&lidar_state.mutex);
    
    if (!lidar_state.initialized) {
        pthread_mutex_unlock(&lidar_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Send health check command
    uint8_t health_cmd[] = {LIDAR_START_FLAG, LIDAR_CMD_GET_HEALTH};
    hal_status_t status = lidar_send_command(health_cmd, sizeof(health_cmd));
    if (status == HAL_STATUS_OK) {
        // Read response
        uint8_t response[8];
        size_t actual_len;
        status = lidar_read_response(response, sizeof(response), &actual_len);
        if (status == HAL_STATUS_OK && actual_len >= 3) {
            // Check health status
            if (response[0] == 0) {
                status = HAL_STATUS_OK; // Healthy
            } else {
                status = HAL_STATUS_ERROR; // Unhealthy
            }
        }
    }
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return status;
}

// Utility functions implementation

/**
 * @brief Calculate minimum distance from scan data
 * @param scan_data Pointer to scan data
 * @return Minimum distance in mm
 */
uint16_t lidar_calculate_min_distance(const lidar_scan_data_t *scan_data)
{
    uint16_t min_distance = LIDAR_MAX_DISTANCE_MM;
    
    if (!scan_data || !scan_data->scan_complete) {
        return min_distance;
    }
    
    for (int i = 0; i < scan_data->point_count; i++) {
        if (scan_data->points[i].distance_mm > 0 && 
            scan_data->points[i].distance_mm < min_distance) {
            min_distance = scan_data->points[i].distance_mm;
        }
    }
    
    return min_distance;
}

/**
 * @brief Calculate maximum distance from scan data
 * @param scan_data Pointer to scan data
 * @return Maximum distance in mm
 */
uint16_t lidar_calculate_max_distance(const lidar_scan_data_t *scan_data)
{
    uint16_t max_distance = 0;
    
    if (!scan_data || !scan_data->scan_complete) {
        return max_distance;
    }
    
    for (int i = 0; i < scan_data->point_count; i++) {
        if (scan_data->points[i].distance_mm > max_distance) {
            max_distance = scan_data->points[i].distance_mm;
        }
    }
    
    return max_distance;
}

/**
 * @brief Check if obstacle is detected
 * @param scan_data Pointer to scan data
 * @param threshold_mm Distance threshold in mm
 * @return true if obstacle detected
 */
bool lidar_is_obstacle_detected(const lidar_scan_data_t *scan_data, uint16_t threshold_mm)
{
    uint16_t min_distance = lidar_calculate_min_distance(scan_data);
    return (min_distance < threshold_mm);
}

/**
 * @brief Validate LiDAR configuration
 * @param config Pointer to configuration
 * @return HAL status
 */
hal_status_t lidar_validate_config(const lidar_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (strlen(config->device_path) == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->baud_rate != LIDAR_BAUD_RATE) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->scan_rate_hz < LIDAR_SCAN_RATE_MIN_HZ || 
        config->scan_rate_hz > LIDAR_SCAN_RATE_MAX_HZ) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->emergency_stop_mm >= config->warning_mm ||
        config->warning_mm >= config->safe_mm) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

// Internal functions implementation (stubs for now)
static void* lidar_scan_thread(void *arg __attribute__((unused))) { return NULL; }
static hal_status_t lidar_open_device(void) { return HAL_STATUS_OK; }
static hal_status_t lidar_close_device(void) { return HAL_STATUS_OK; }
static hal_status_t lidar_configure_serial(void) { return HAL_STATUS_OK; }
static hal_status_t lidar_send_command(const uint8_t *command __attribute__((unused)), size_t len __attribute__((unused))) { return HAL_STATUS_OK; }
static hal_status_t lidar_read_response(uint8_t *buffer __attribute__((unused)), size_t max_len __attribute__((unused)), size_t *actual_len __attribute__((unused))) { return HAL_STATUS_OK; }
static uint64_t lidar_get_timestamp_us(void) { return 0; }
__attribute__((unused))
static hal_status_t lidar_parse_scan_data(const uint8_t *data, size_t len, lidar_scan_data_t *scan_data) { 
    (void)data; // Unused parameter
    (void)len; // Unused parameter
    (void)scan_data; // Unused parameter
    return HAL_STATUS_OK; 
}
static hal_status_t lidar_process_safety_status(void) { return HAL_STATUS_OK; }
