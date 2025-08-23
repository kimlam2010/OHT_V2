# 🔗 LiDAR Integration Guide - OHT-50

**Version:** 1.0.0  
**Date:** 2025-01-27  
**Team:** EMBED  
**Task:** EM-12 (LiDAR Driver & USB Integration)

---

## 📋 **Integration Overview**

### **Integration Objectives:**
- Tích hợp LiDAR sensor vào hệ thống OHT-50
- Implement HAL interface cho LiDAR
- Tích hợp với safety system (E-Stop)
- Tích hợp với control system
- Tích hợp với telemetry system

### **Integration Architecture:**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   LiDAR Sensor  │    │   HAL Driver    │    │  Safety System  │
│   (RPLIDAR A1)  │───▶│   (hal_lidar)   │───▶│   (E-Stop)      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │                       ▼                       │
         │              ┌─────────────────┐              │
         │              │  Control System │              │
         │              │   (Movement)    │              │
         │              └─────────────────┘              │
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│ Telemetry System│    │   State Machine │    │   Logging       │
│   (Data Log)    │    │   (Status)      │    │   (Events)      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

---

## 🔧 **HAL Interface Design**

### **HAL LiDAR Header (`hal_lidar.h`):**

```c
/**
 * @file hal_lidar.h
 * @brief HAL interface for LiDAR sensor
 * @version 1.0.0
 * @date 2025-01-27
 */

#ifndef HAL_LIDAR_H
#define HAL_LIDAR_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// LiDAR configuration constants
#define LIDAR_MAX_DISTANCE_MM     12000   // 12m max distance
#define LIDAR_MIN_DISTANCE_MM     150     // 0.15m min distance
#define LIDAR_SCAN_RATE_HZ        8       // 8Hz scan rate
#define LIDAR_POINTS_PER_SCAN     400     // 400 points per scan
#define LIDAR_ANGULAR_RESOLUTION  0.9f    // 0.9 degrees

// Safety thresholds (in mm)
#define LIDAR_EMERGENCY_STOP_MM   500     // 0.5m emergency stop
#define LIDAR_WARNING_MM          1000    // 1.0m warning
#define LIDAR_SAFE_MM             2000    // 2.0m safe distance

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
} lidar_scan_data_t;

typedef struct {
    uint16_t min_distance_mm;     // Minimum distance in scan
    uint16_t min_distance_angle;  // Angle of minimum distance
    bool obstacle_detected;       // Obstacle detected flag
    bool emergency_stop_triggered; // Emergency stop triggered
    bool warning_triggered;       // Warning triggered
    uint64_t timestamp_us;        // Timestamp
} lidar_safety_status_t;

typedef struct {
    char device_path[64];         // Device path (e.g., "/dev/ttyUSB0")
    uint32_t baud_rate;           // Baud rate (default: 115200)
    uint16_t scan_rate_hz;        // Scan rate in Hz
    uint16_t emergency_stop_mm;   // Emergency stop distance
    uint16_t warning_mm;          // Warning distance
    uint16_t safe_mm;             // Safe distance
} lidar_config_t;

// HAL function prototypes
hal_status_t hal_lidar_init(const lidar_config_t *config);
hal_status_t hal_lidar_deinit(void);
hal_status_t hal_lidar_configure(const lidar_config_t *config);
hal_status_t hal_lidar_start_scanning(void);
hal_status_t hal_lidar_stop_scanning(void);
hal_status_t hal_lidar_get_scan_data(lidar_scan_data_t *scan_data);
hal_status_t hal_lidar_check_safety(lidar_safety_status_t *safety_status);
hal_status_t hal_lidar_get_status(hal_device_status_t *status);
hal_status_t hal_lidar_reset(void);

// Utility functions
uint16_t lidar_calculate_min_distance(const lidar_scan_data_t *scan_data);
bool lidar_is_obstacle_detected(const lidar_scan_data_t *scan_data, uint16_t threshold_mm);
hal_status_t lidar_validate_config(const lidar_config_t *config);

#endif // HAL_LIDAR_H
```

### **HAL LiDAR Implementation (`hal_lidar.c`):**

```c
/**
 * @file hal_lidar.c
 * @brief HAL implementation for LiDAR sensor
 * @version 1.0.0
 * @date 2025-01-27
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
} lidar_state = {0};

// Internal function prototypes
static void* lidar_scan_thread(void *arg);
static hal_status_t lidar_open_device(void);
static hal_status_t lidar_close_device(void);
static hal_status_t lidar_configure_serial(void);
static hal_status_t lidar_send_command(const uint8_t *command, size_t len);
static hal_status_t lidar_read_response(uint8_t *buffer, size_t max_len, size_t *actual_len);
static uint64_t lidar_get_timestamp_us(void);

// RPLIDAR protocol commands
static const uint8_t LIDAR_CMD_GET_INFO[] = {0xA5, 0x25};
static const uint8_t LIDAR_CMD_GET_HEALTH[] = {0xA5, 0x26};
static const uint8_t LIDAR_CMD_START_SCAN[] = {0xA5, 0x20};
static const uint8_t LIDAR_CMD_STOP_SCAN[] = {0xA5, 0x25};

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
    status = lidar_send_command(LIDAR_CMD_START_SCAN, sizeof(LIDAR_CMD_START_SCAN));
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
    status = lidar_send_command(LIDAR_CMD_STOP_SCAN, sizeof(LIDAR_CMD_STOP_SCAN));
    
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
    
    // Calculate safety status from current scan
    uint16_t min_distance = lidar_calculate_min_distance(&lidar_state.current_scan);
    
    lidar_state.safety_status.min_distance_mm = min_distance;
    lidar_state.safety_status.obstacle_detected = (min_distance < lidar_state.config.safe_mm);
    lidar_state.safety_status.emergency_stop_triggered = (min_distance < lidar_state.config.emergency_stop_mm);
    lidar_state.safety_status.warning_triggered = (min_distance < lidar_state.config.warning_mm);
    lidar_state.safety_status.timestamp_us = lidar_get_timestamp_us();
    
    // Copy safety status
    memcpy(safety_status, &lidar_state.safety_status, sizeof(lidar_safety_status_t));
    
    pthread_mutex_unlock(&lidar_state.mutex);
    
    return HAL_STATUS_OK;
}

// Internal functions implementation...
// (Additional implementation details would go here)

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
    
    if (config->baud_rate == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->scan_rate_hz == 0 || config->scan_rate_hz > 20) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->emergency_stop_mm >= config->warning_mm ||
        config->warning_mm >= config->safe_mm) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}
```

---

## 🔒 **Safety Integration**

### **Safety System Integration:**

```c
/**
 * @file safety_lidar_integration.c
 * @brief Safety integration for LiDAR sensor
 */

#include "hal_lidar.h"
#include "safety_system.h"

// Safety integration function
hal_status_t safety_lidar_integration_init(void)
{
    // Initialize LiDAR with safety configuration
    lidar_config_t config = {
        .device_path = "/dev/ttyUSB0",
        .baud_rate = 115200,
        .scan_rate_hz = 8,
        .emergency_stop_mm = LIDAR_EMERGENCY_STOP_MM,
        .warning_mm = LIDAR_WARNING_MM,
        .safe_mm = LIDAR_SAFE_MM
    };
    
    return hal_lidar_init(&config);
}

// Safety monitoring thread
void* safety_lidar_monitor_thread(void *arg)
{
    lidar_safety_status_t safety_status;
    hal_status_t status;
    
    while (1) {
        // Check LiDAR safety status
        status = hal_lidar_check_safety(&safety_status);
        
        if (status == HAL_STATUS_OK) {
            // Handle emergency stop
            if (safety_status.emergency_stop_triggered) {
                safety_system_trigger_emergency_stop(SAFETY_SOURCE_LIDAR);
            }
            
            // Handle warning
            if (safety_status.warning_triggered) {
                safety_system_trigger_warning(SAFETY_SOURCE_LIDAR);
            }
        }
        
        // Sleep for 10ms (100Hz safety check)
        usleep(10000);
    }
    
    return NULL;
}
```

---

## 🔄 **Control System Integration**

### **Control System Integration:**

```c
/**
 * @file control_lidar_integration.c
 * @brief Control system integration for LiDAR
 */

#include "hal_lidar.h"
#include "control_system.h"

// Control system integration
hal_status_t control_lidar_integration_init(void)
{
    // Start LiDAR scanning
    hal_status_t status = hal_lidar_start_scanning();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Register LiDAR data callback
    control_system_register_sensor_callback(SENSOR_TYPE_LIDAR, 
                                          control_lidar_data_callback);
    
    return HAL_STATUS_OK;
}

// LiDAR data callback for control system
void control_lidar_data_callback(const void *data)
{
    const lidar_scan_data_t *scan_data = (const lidar_scan_data_t*)data;
    
    if (!scan_data || !scan_data->scan_complete) {
        return;
    }
    
    // Update control system with obstacle information
    control_system_update_obstacle_data(scan_data);
    
    // Check if movement should be restricted
    uint16_t min_distance = lidar_calculate_min_distance(scan_data);
    
    if (min_distance < LIDAR_WARNING_MM) {
        // Reduce speed or stop movement
        control_system_set_speed_limit(0.5f); // 50% speed limit
    } else {
        // Normal operation
        control_system_set_speed_limit(1.0f); // 100% speed
    }
}
```

---

## 📊 **Telemetry Integration**

### **Telemetry System Integration:**

```c
/**
 * @file telemetry_lidar_integration.c
 * @brief Telemetry integration for LiDAR
 */

#include "hal_lidar.h"
#include "telemetry_system.h"

// Telemetry data structures
typedef struct {
    uint16_t min_distance_mm;
    uint16_t min_distance_angle;
    bool obstacle_detected;
    bool emergency_stop_triggered;
    bool warning_triggered;
    uint64_t timestamp_us;
} telemetry_lidar_data_t;

// Telemetry integration
hal_status_t telemetry_lidar_integration_init(void)
{
    // Register LiDAR telemetry callback
    telemetry_system_register_sensor_callback(SENSOR_TYPE_LIDAR,
                                            telemetry_lidar_callback);
    
    return HAL_STATUS_OK;
}

// Telemetry callback
void telemetry_lidar_callback(const void *data)
{
    const lidar_safety_status_t *safety_status = (const lidar_safety_status_t*)data;
    
    if (!safety_status) {
        return;
    }
    
    // Create telemetry data
    telemetry_lidar_data_t telemetry_data = {
        .min_distance_mm = safety_status->min_distance_mm,
        .min_distance_angle = safety_status->min_distance_angle,
        .obstacle_detected = safety_status->obstacle_detected,
        .emergency_stop_triggered = safety_status->emergency_stop_triggered,
        .warning_triggered = safety_status->warning_triggered,
        .timestamp_us = safety_status->timestamp_us
    };
    
    // Send telemetry data
    telemetry_system_send_data(TELEMETRY_TYPE_LIDAR, &telemetry_data, sizeof(telemetry_data));
}
```

---

## 🧪 **Integration Testing**

### **Integration Test Script:**

```bash
#!/bin/bash
# Integration test script: test_lidar_integration.sh

echo "=== LiDAR Integration Test ==="

# 1. Test HAL initialization
echo "1. Testing HAL initialization..."
./test_hal_lidar_init
if [ $? -eq 0 ]; then
    echo "✅ HAL initialization PASSED"
else
    echo "❌ HAL initialization FAILED"
    exit 1
fi

# 2. Test safety integration
echo "2. Testing safety integration..."
./test_safety_lidar_integration
if [ $? -eq 0 ]; then
    echo "✅ Safety integration PASSED"
else
    echo "❌ Safety integration FAILED"
    exit 1
fi

# 3. Test control integration
echo "3. Testing control integration..."
./test_control_lidar_integration
if [ $? -eq 0 ]; then
    echo "✅ Control integration PASSED"
else
    echo "❌ Control integration FAILED"
    exit 1
fi

# 4. Test telemetry integration
echo "4. Testing telemetry integration..."
./test_telemetry_lidar_integration
if [ $? -eq 0 ]; then
    echo "✅ Telemetry integration PASSED"
else
    echo "❌ Telemetry integration FAILED"
    exit 1
fi

echo "✅ All integration tests PASSED"
```

---

## 📝 **Integration Checklist**

### **Pre-Integration:**
- [ ] LiDAR hardware installed và tested
- [ ] HAL driver implemented và tested
- [ ] Safety system ready
- [ ] Control system ready
- [ ] Telemetry system ready

### **Integration Steps:**
- [ ] HAL integration
- [ ] Safety system integration
- [ ] Control system integration
- [ ] Telemetry system integration
- [ ] System-wide testing

### **Post-Integration:**
- [ ] Performance validation
- [ ] Safety validation
- [ ] Documentation updated
- [ ] Training completed

---

## 🚨 **Troubleshooting**

### **Integration Issues:**

#### **HAL Integration Issues:**
- **Initialization fails:** Check device path, permissions
- **Communication fails:** Check baud rate, cable connection
- **Data corruption:** Check USB bandwidth, interference

#### **Safety Integration Issues:**
- **E-Stop not triggering:** Check threshold settings, integration
- **False triggers:** Adjust sensitivity, check environment
- **Slow response:** Optimize processing, check system load

#### **Control Integration Issues:**
- **Control conflicts:** Check priority settings, timing
- **Data latency:** Optimize data flow, reduce processing
- **System instability:** Check resource usage, conflicts

#### **Telemetry Integration Issues:**
- **Data loss:** Check bandwidth, buffer sizes
- **Format errors:** Check data structures, alignment
- **Timing issues:** Check synchronization, timestamps

---

**🎯 Integration Guide Complete!**
Tài liệu integration guide đã được tạo. Sẵn sàng cho bước tiếp theo: HAL driver implementation và testing.
