/**
 * @file hal_lidar.h
 * @brief HAL interface for RPLIDAR C1M1 sensor
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-12 (LiDAR Driver & USB Integration)
 */

#ifndef HAL_LIDAR_H
#define HAL_LIDAR_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// RPLIDAR C1M1 Specifications
#define LIDAR_MAX_DISTANCE_MM     12000   // 12m max distance (white object)
#define LIDAR_MIN_DISTANCE_MM      50     // 0.05m min distance
#define LIDAR_SCAN_RATE_MIN_HZ     8      // 8Hz min scan rate
#define LIDAR_SCAN_RATE_MAX_HZ     12     // 12Hz max scan rate
#define LIDAR_SCAN_RATE_TYPICAL_HZ 10     // 10Hz typical scan rate
#define LIDAR_SAMPLE_RATE_HZ       5000   // 5KHz sample rate
#define LIDAR_ANGULAR_RESOLUTION   0.72f  // 0.72 degrees typical
#define LIDAR_ACCURACY_MM          30     // ±30mm accuracy
#define LIDAR_RESOLUTION_MM        15     // 15mm resolution

// Communication settings
#define LIDAR_BAUD_RATE            460800 // 460800 baud rate
#define LIDAR_UART_DATA_BITS       8      // 8 data bits
#define LIDAR_UART_STOP_BITS       1      // 1 stop bit
#define LIDAR_UART_PARITY          0      // No parity

// Safety thresholds (in mm)
#define LIDAR_EMERGENCY_STOP_MM    500     // 0.5m emergency stop
#define LIDAR_WARNING_MM           1000    // 1.0m warning
#define LIDAR_SAFE_MM              2000    // 2.0m safe distance

// RPLIDAR protocol constants
#define LIDAR_START_FLAG           0xA5
#define LIDAR_CMD_GET_INFO         0x25
#define LIDAR_CMD_GET_HEALTH       0x26
#define LIDAR_CMD_START_SCAN       0x20
#define LIDAR_CMD_STOP_SCAN        0x25
#define LIDAR_CMD_RESET            0x40

// Scan data constants
#define LIDAR_POINTS_PER_SCAN      500     // ~500 points per scan (360°/0.72°)
#define LIDAR_SCAN_BUFFER_SIZE     1024    // Buffer size for scan data

// LiDAR data structures
typedef struct {
    uint16_t distance_mm;         // Distance in mm
    uint16_t angle_deg;           // Angle in degrees (0-359)
    uint8_t quality;              // Signal quality (0-255)
    uint64_t timestamp_us;        // Timestamp in microseconds
} lidar_point_t;

typedef struct {
    lidar_point_t points[LIDAR_POINTS_PER_SCAN];
    uint16_t point_count;
    uint64_t scan_timestamp_us;
    bool scan_complete;
    uint8_t scan_quality;         // Overall scan quality
} lidar_scan_data_t;

typedef struct {
    uint16_t min_distance_mm;     // Minimum distance in scan
    uint16_t min_distance_angle;  // Angle of minimum distance
    uint16_t max_distance_mm;     // Maximum distance in scan
    uint16_t max_distance_angle;  // Angle of maximum distance
    bool obstacle_detected;       // Obstacle detected flag
    bool emergency_stop_triggered; // Emergency stop triggered
    bool warning_triggered;       // Warning triggered
    uint64_t timestamp_us;        // Timestamp
} lidar_safety_status_t;

typedef struct {
    char device_path[64];         // Device path (e.g., "/dev/ttyUSB0")
    uint32_t baud_rate;           // Baud rate (default: 460800)
    uint16_t scan_rate_hz;        // Scan rate in Hz (8-12Hz)
    uint16_t emergency_stop_mm;   // Emergency stop distance
    uint16_t warning_mm;          // Warning distance
    uint16_t safe_mm;             // Safe distance
    uint16_t sample_rate_hz;      // Sample rate in Hz
    float angular_resolution;     // Angular resolution in degrees
} lidar_config_t;

typedef struct {
    uint8_t model;                // Device model
    uint8_t firmware_version;     // Firmware version
    uint8_t hardware_version;     // Hardware version
    uint32_t serial_number;       // Serial number
    bool device_healthy;          // Device health status
    uint64_t timestamp_us;        // Timestamp
} lidar_device_info_t;

// HAL function prototypes
hal_status_t hal_lidar_init(const lidar_config_t *config);
/**
 * @brief Deinitialize LiDAR HAL
 * @return HAL status
 */
hal_status_t hal_lidar_deinit(void);
/**
 * @brief Reset LiDAR state completely (for testing purposes)
 * @return HAL status
 */
hal_status_t hal_lidar_reset_state(void);
hal_status_t hal_lidar_configure(const lidar_config_t *config);
hal_status_t hal_lidar_start_scanning(void);
hal_status_t hal_lidar_stop_scanning(void);
hal_status_t hal_lidar_get_scan_data(lidar_scan_data_t *scan_data);
hal_status_t hal_lidar_check_safety(lidar_safety_status_t *safety_status);
hal_status_t hal_lidar_get_device_info(lidar_device_info_t *device_info);
hal_status_t hal_lidar_get_status(hal_device_status_t *status);
hal_status_t hal_lidar_reset(void);
hal_status_t hal_lidar_health_check(void);

// Utility functions
uint16_t lidar_calculate_min_distance(const lidar_scan_data_t *scan_data);
uint16_t lidar_calculate_max_distance(const lidar_scan_data_t *scan_data);
bool lidar_is_obstacle_detected(const lidar_scan_data_t *scan_data, uint16_t threshold_mm);
hal_status_t lidar_validate_config(const lidar_config_t *config);
float lidar_calculate_scan_rate(const lidar_scan_data_t *scan_data, uint64_t time_window_us);

// Safety functions
hal_status_t lidar_safety_init(void);
hal_status_t lidar_safety_check_distance(uint16_t distance_mm, uint16_t angle_deg);
bool lidar_safety_is_emergency_stop_triggered(void);
bool lidar_safety_is_warning_triggered(void);

#endif // HAL_LIDAR_H
