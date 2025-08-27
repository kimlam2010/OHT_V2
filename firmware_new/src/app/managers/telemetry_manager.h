/**
 * @file telemetry_manager.h
 * @brief Telemetry Manager for OHT-50 system
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#ifndef TELEMETRY_MANAGER_H
#define TELEMETRY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_common.h"
#include "system_state_machine.h"
#include "module_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

// Telemetry Manager Status
typedef enum {
    TELEMETRY_STATUS_OK = 0,
    TELEMETRY_STATUS_ERROR,
    TELEMETRY_STATUS_BUFFER_FULL,
    TELEMETRY_STATUS_INVALID_DATA
} telemetry_mgr_status_t;

// Telemetry Location Data
typedef struct {
    struct {
        float x;  // X position in mm
        float y;  // Y position in mm
        float z;  // Z position in mm
    } position;
    
    struct {
        float pitch;  // Pitch angle in degrees
        float roll;   // Roll angle in degrees
        float yaw;    // Yaw angle in degrees
    } orientation;
    
    struct {
        float x;  // X velocity in mm/s
        float y;  // Y velocity in mm/s
        float z;  // Z velocity in mm/s
    } velocity;
    
    struct {
        float x;  // X acceleration in mm/s²
        float y;  // Y acceleration in mm/s²
        float z;  // Z acceleration in mm/s²
    } acceleration;
    
    float accuracy;  // Position accuracy in mm
    
    enum {
        LOCATION_SYSTEM_OK = 0,
        LOCATION_SYSTEM_ERROR,
        LOCATION_SYSTEM_CALIBRATING
    } system_status;
    
    enum {
        IMU_STATUS_OK = 0,
        IMU_STATUS_ERROR
    } imu_status;
    
    enum {
        MAGNETIC_STATUS_OK = 0,
        MAGNETIC_STATUS_ERROR
    } magnetic_status;
} telemetry_location_t;

// Telemetry Navigation Data
typedef struct {
    enum {
        NAV_STATUS_IDLE = 0,
        NAV_STATUS_NAVIGATING,
        NAV_STATUS_POSITIONING,
        NAV_STATUS_ARRIVED,
        NAV_STATUS_ERROR
    } status;
    
    struct {
        float x;  // Target X position in mm
        float y;  // Target Y position in mm
        float z;  // Target Z position in mm
    } target;
    
    float progress;           // Navigation progress in %
    float distance_to_target; // Distance to target in mm
    float estimated_time;     // Estimated time to target in seconds
    bool path_clear;          // Path is clear of obstacles
    bool obstacle_detected;   // Obstacle detected
    float obstacle_distance;  // Distance to obstacle in mm
    float obstacle_angle;     // Angle to obstacle in degrees
    float speed_limit;        // Current speed limit in mm/s
    
    enum {
        ZONE_TYPE_SAFE = 0,
        ZONE_TYPE_WARNING,
        ZONE_TYPE_RESTRICTED,
        ZONE_TYPE_EMERGENCY
    } zone_type;
} telemetry_navigation_t;

// Telemetry Dock Data
typedef struct {
    enum {
        DOCK_STATUS_NOT_DOCKING = 0,
        DOCK_STATUS_APPROACHING,
        DOCK_STATUS_ALIGNING,
        DOCK_STATUS_DOCKED,
        DOCK_STATUS_UNDOCKING,
        DOCK_STATUS_ERROR
    } status;
    
    char station_id[32];      // Current station ID
    char target_station_id[32]; // Target station ID
    
    enum {
        ALIGNMENT_NOT_ALIGNED = 0,
        ALIGNMENT_ALIGNING,
        ALIGNMENT_ALIGNED,
        ALIGNMENT_ERROR
    } alignment_status;
    
    float distance;           // Distance to dock in mm
    float angle;              // Alignment angle in degrees
    
    enum {
        CHARGING_NOT_CHARGING = 0,
        CHARGING_CHARGING,
        CHARGING_FULL,
        CHARGING_ERROR
    } charging_status;
    
    float charging_current;   // Charging current in mA
    float charging_voltage;   // Charging voltage in V
    char rfid_tag_id[32];     // RFID tag ID
    int32_t rfid_signal_strength; // RFID signal strength (RSSI)
    
    enum {
        DOCK_SAFETY_SAFE = 0,
        DOCK_SAFETY_WARNING,
        DOCK_SAFETY_DANGER
    } safety_status;
} telemetry_dock_t;

// Telemetry Safety Data
typedef struct {
    bool estop;              // E-Stop active
    bool zone_blocked;       // Zone blocked
    bool interlock_active;   // Interlock active
    bool location_safe;      // Location-based safety OK
    bool obstacle_detected;  // Obstacle detected
    bool safety_zone_clear;  // Safety zone is clear
    bool emergency_stop;     // Emergency stop active
} telemetry_safety_t;

// Telemetry Encoder Data
typedef struct {
    enum {
        ENCODER_HEALTH_OK = 0,
        ENCODER_HEALTH_WARNING,
        ENCODER_HEALTH_ERROR
    } health;
    
    char fault[64];          // Fault description
    int32_t count;           // Encoder count
} telemetry_encoder_t;

// Telemetry Target Data
typedef struct {
    float pos_mm;   // Target position in mm
    float vel_mms;  // Target velocity in mm/s
} telemetry_target_t;

// Telemetry Status Data
typedef struct {
    system_state_t state;    // System state
    float pos_mm;            // Current position in mm
    float vel_mms;           // Current velocity in mm/s
    float acc_mms2;          // Current acceleration in mm/s²
    telemetry_target_t target; // Target data
    telemetry_safety_t safety; // Safety data
    telemetry_encoder_t enc;   // Encoder data
    char errors[256];        // Error messages
    char warnings[256];      // Warning messages
} telemetry_status_t;

// Statistics structure
typedef struct {
    uint32_t total_updates;
    uint32_t location_updates;
    uint32_t navigation_updates;
    uint32_t dock_updates;
    uint32_t safety_updates;
    uint32_t system_updates;
    uint32_t events_sent;
    uint32_t json_serializations;
    uint64_t last_update_time;
    uint64_t total_processing_time;
} telemetry_stats_t;

// Main Telemetry Data Structure
typedef struct {
    uint64_t ts;                    // Unix timestamp
    telemetry_status_t status;      // System status
    telemetry_location_t location;  // Location data
    telemetry_navigation_t navigation; // Navigation data
    telemetry_dock_t dock;          // Dock data
    char mission_id[64];            // Mission ID
    char station_id[32];            // Station ID
    bool cargo_present;             // Cargo present
    bool cargo_locked;              // Cargo locked
    float cpu_usage;                // CPU usage %
    float memory_usage;             // Memory usage %
    float temperature;              // Temperature in °C
    bool center_connected;          // Center connected
    bool rs485_connected;           // RS485 connected
    char tag_id[32];                // Tag ID
    int32_t rssi;                   // RSSI
} telemetry_data_t;

// Telemetry Manager Configuration
typedef struct {
    uint32_t update_rate_ms;        // Update rate in milliseconds
    uint32_t buffer_size;           // Ring buffer size
    bool enable_location;           // Enable location telemetry
    bool enable_navigation;         // Enable navigation telemetry
    bool enable_dock;               // Enable dock telemetry
    bool enable_system;             // Enable system telemetry
    bool enable_events;             // Enable event broadcasting
} telemetry_config_t;

// Telemetry Event Types
typedef enum {
    TELEMETRY_EVENT_NONE = 0,
    TELEMETRY_EVENT_LOCATION_UPDATE,
    TELEMETRY_EVENT_NAVIGATION_UPDATE,
    TELEMETRY_EVENT_DOCK_UPDATE,
    TELEMETRY_EVENT_SAFETY_ALERT,
    TELEMETRY_EVENT_SYSTEM_STATUS,
    TELEMETRY_EVENT_ERROR
} telemetry_event_t;

// Telemetry Event Callback
typedef void (*telemetry_event_callback_t)(telemetry_event_t event, const telemetry_data_t *data);

// Function Prototypes

/**
 * @brief Initialize telemetry manager
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t telemetry_manager_init(const telemetry_config_t *config);

/**
 * @brief Deinitialize telemetry manager
 * @return HAL status
 */
hal_status_t telemetry_manager_deinit(void);

/**
 * @brief Update telemetry data
 * @return HAL status
 */
hal_status_t telemetry_manager_update(void);

/**
 * @brief Set telemetry event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t telemetry_manager_set_callback(telemetry_event_callback_t callback);

/**
 * @brief Get current telemetry data
 * @param data Pointer to telemetry data structure
 * @return HAL status
 */
hal_status_t telemetry_manager_get_data(telemetry_data_t *data);

/**
 * @brief Update location data
 * @param location Location data
 * @return HAL status
 */
hal_status_t telemetry_manager_update_location(const telemetry_location_t *location);

/**
 * @brief Update navigation data
 * @param navigation Navigation data
 * @return HAL status
 */
hal_status_t telemetry_manager_update_navigation(const telemetry_navigation_t *navigation);

/**
 * @brief Update dock data
 * @param dock Dock data
 * @return HAL status
 */
hal_status_t telemetry_manager_update_dock(const telemetry_dock_t *dock);

/**
 * @brief Update safety data
 * @param safety Safety data
 * @return HAL status
 */
hal_status_t telemetry_manager_update_safety(const telemetry_safety_t *safety);

/**
 * @brief Update system status
 * @param status System status
 * @return HAL status
 */
hal_status_t telemetry_manager_update_status(const telemetry_status_t *status);

/**
 * @brief Serialize telemetry data to JSON
 * @param data Telemetry data
 * @param json_buffer JSON buffer
 * @param buffer_size Buffer size
 * @return Number of bytes written
 */
int telemetry_manager_serialize_json(const telemetry_data_t *data, char *json_buffer, size_t buffer_size);

/**
 * @brief Get telemetry statistics
 * @param stats Statistics structure
 * @return HAL status
 */
hal_status_t telemetry_manager_get_statistics(telemetry_stats_t *stats);

/**
 * @brief Reset telemetry statistics
 * @return HAL status
 */
hal_status_t telemetry_manager_reset_statistics(void);

#ifdef __cplusplus
}
#endif

#endif // TELEMETRY_MANAGER_H
