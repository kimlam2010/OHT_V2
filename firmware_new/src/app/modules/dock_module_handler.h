#ifndef DOCK_MODULE_HANDLER_H
#define DOCK_MODULE_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hal_common.h"
#include "register_map.h"

// Forward declaration
typedef struct dock_module_handler dock_module_handler_t;

// ============================================================================
// MODBUS REGISTER MAP (Dock Module - Address 0x05)
// ============================================================================

// Position and Status Registers (0x7000-0x70FF)
#define DOCK_POSITION_TARGET_REG          0x7000  // Target dock position (mm)
#define DOCK_CURRENT_POSITION_REG         0x7001  // Current dock position (mm)
#define DOCK_APPROACH_SPEED_REG           0x7002  // Approach speed (mm/s)
#define DOCK_FINAL_SPEED_REG              0x7003  // Final docking speed (mm/s)
#define DOCK_ACCURACY_THRESHOLD_REG       0x7004  // Docking accuracy threshold (mm)
#define DOCK_STATUS_REG                   0x7005  // Dock status flags
#define DOCK_REG_IMU_STATUS               0x7005  // IMU status register
#define DOCK_DISTANCE_TO_DOCK_REG         0x7006  // Distance to dock (mm)
#define DOCK_ALIGNMENT_ANGLE_REG          0x7007  // Alignment angle (degrees * 10)

// Control Registers (0x8000-0x80FF)
#define DOCK_ENABLE_REG                   0x8000  // Enable/disable dock module
#define DOCK_START_DOCKING_REG            0x8001  // Start docking sequence
#define DOCK_STOP_DOCKING_REG             0x8002  // Stop docking sequence
#define DOCK_EMERGENCY_STOP_REG           0x8003  // Emergency stop docking
#define DOCK_RESET_FAULTS_REG             0x8004  // Reset fault conditions
#define DOCK_CALIBRATE_REG                0x8005  // Start calibration sequence
#define DOCK_SET_DOCK_POSITION_REG        0x8006  // Set dock reference position
#define DOCK_SET_APPROACH_DISTANCE_REG    0x8007  // Set approach distance (mm)

// Fault and Diagnostic Registers (0x9000-0x90FF)
#define DOCK_FAULT_STATUS_REG             0x9000  // Fault status flags
#define DOCK_FAULT_CODE_REG               0x9001  // Specific fault code
#define DOCK_FAULT_COUNT_REG              0x9002  // Fault occurrence count
#define DOCK_TEMPERATURE_REG              0x9003  // Module temperature (°C)
#define DOCK_VOLTAGE_REG                  0x9004  // Supply voltage (V * 10)
#define DOCK_CURRENT_REG                  0x9005  // Current consumption (mA)
#define DOCK_UPTIME_REG                   0x9006  // Module uptime (seconds)
#define DOCK_DOCKING_COUNT_REG            0x9007  // Successful docking count
#define DOCK_FAILED_DOCKING_COUNT_REG     0x9008  // Failed docking count
#define DOCK_LAST_DOCKING_TIME_REG        0x9009  // Last docking time (ms)

// Configuration Registers (0xA000-0xA0FF)
#define DOCK_CONFIG_APPROACH_DISTANCE_REG 0xA000  // Default approach distance (mm)
#define DOCK_CONFIG_FINAL_SPEED_REG       0xA001  // Default final speed (mm/s)
#define DOCK_CONFIG_ACCURACY_REG          0xA002  // Default accuracy threshold (mm)
#define DOCK_CONFIG_TIMEOUT_REG           0xA003  // Docking timeout (seconds)
#define DOCK_CONFIG_RETRY_COUNT_REG       0xA004  // Max retry attempts
#define DOCK_CONFIG_DEBOUNCE_TIME_REG     0xA005  // Sensor debounce time (ms)
#define DOCK_CONFIG_ALIGNMENT_TOLERANCE_REG 0xA006 // Alignment tolerance (degrees * 10)

// System Registers (0x0100-0x0109) - Auto Detect Support
#define DOCK_DEVICE_ID_REG                0x0100  // Device ID (Modbus slave address)
#define DOCK_CONFIG_BAUDRATE_REG          0x0101  // Config baudrate (1=9600, 2=19200, 3=38400,...)
#define DOCK_CONFIG_PARITY_REG            0x0102  // Config parity (0=None, 1=Even, 2=Odd)
#define DOCK_CONFIG_STOP_BITS_REG         0x0103  // Config stop bits (1=1, 2=2)
#define DOCK_MODULE_TYPE_REG              0x0104  // Module type (0x0005 = Dock Module)
#define DOCK_FIRMWARE_VERSION_REG         0x0105  // Firmware version (e.g. 0x0101 = v1.01)
#define DOCK_HARDWARE_VERSION_REG         0x0106  // Hardware version (e.g. 0x0101 = v1.01)
#define DOCK_SYSTEM_STATUS_REG            0x0107  // System status (bit field)
#define DOCK_SYSTEM_ERROR_REG             0x0108  // System error (global error code)
#define DOCK_RESET_ERROR_CMD_REG          0x0109  // Reset error command (write 1 to reset all error flags)
#define DOCK_MODULE_NAME_LOW_REG          0x00F8  // Module name (low word)
#define DOCK_MODULE_NAME_HIGH_REG         0x00F9  // Module name (high word)
#define DOCK_HARDWARE_VERSION_REG         0x00FA  // Hardware version
#define DOCK_SERIAL_NUMBER_LOW_REG        0x00FB  // Serial number (low word)
#define DOCK_SERIAL_NUMBER_HIGH_REG       0x00FC  // Serial number (high word)
#define DOCK_BUILD_DATE_LOW_REG           0x00FD  // Build date (low word)
#define DOCK_BUILD_DATE_HIGH_REG          0x00FE  // Build date (high word)
#define DOCK_CHECKSUM_REG                 0x00FF  // Register checksum

// ============================================================================
// DATA STRUCTURES
// ============================================================================

// Dock status flags
typedef enum {
    DOCK_STATUS_IDLE = 0x00,
    DOCK_STATUS_APPROACHING = 0x01,
    DOCK_STATUS_ALIGNING = 0x02,
    DOCK_STATUS_DOCKING = 0x04,
    DOCK_STATUS_DOCKED = 0x08,
    DOCK_STATUS_UNDOCKING = 0x10,
    DOCK_STATUS_FAULT = 0x20,
    DOCK_STATUS_CALIBRATING = 0x40,
    DOCK_STATUS_EMERGENCY_STOP = 0x80
} dock_status_t;

// Dock fault codes
typedef enum {
    DOCK_FAULT_NONE = 0x00,
    DOCK_FAULT_SENSOR_ERROR = 0x01,
    DOCK_FAULT_TIMEOUT = 0x02,
    DOCK_FAULT_ALIGNMENT_FAILED = 0x03,
    DOCK_FAULT_POSITION_ERROR = 0x04,
    DOCK_FAULT_EMERGENCY_STOP = 0x05,
    DOCK_FAULT_CALIBRATION_FAILED = 0x06,
    DOCK_FAULT_COMMUNICATION_ERROR = 0x07,
    DOCK_FAULT_TEMPERATURE_HIGH = 0x08,
    DOCK_FAULT_VOLTAGE_LOW = 0x09,
    DOCK_FAULT_MECHANICAL_ERROR = 0x0A
} dock_fault_code_t;

// Dock events
typedef enum {
    DOCK_EVENT_DOCKING_STARTED,
    DOCK_EVENT_APPROACHING,
    DOCK_EVENT_ALIGNING,
    DOCK_EVENT_DOCKING,
    DOCK_EVENT_DOCKED,
    DOCK_EVENT_UNDOCKING,
    DOCK_EVENT_UNDOCKED,
    DOCK_EVENT_FAULT,
    DOCK_EVENT_CALIBRATION_STARTED,
    DOCK_EVENT_CALIBRATION_COMPLETED,
    DOCK_EVENT_EMERGENCY_STOP
} dock_event_t;

// Dock configuration
typedef struct {
    uint16_t approach_distance;      // Default approach distance (mm)
    uint16_t final_speed;           // Default final docking speed (mm/s)
    uint16_t accuracy_threshold;    // Default accuracy threshold (mm)
    uint16_t timeout;               // Docking timeout (seconds)
    uint8_t retry_count;            // Max retry attempts
    uint16_t debounce_time;         // Sensor debounce time (ms)
    uint16_t alignment_tolerance;   // Alignment tolerance (degrees * 10)
    uint8_t address;                // Module address
} dock_config_t;

// Dock data structure
typedef struct {
    uint16_t position_target;       // Target dock position (mm)
    uint16_t current_position;      // Current dock position (mm)
    uint16_t approach_speed;        // Approach speed (mm/s)
    uint16_t final_speed;           // Final docking speed (mm/s)
    uint16_t accuracy_threshold;    // Docking accuracy threshold (mm)
    uint8_t status;                 // Dock status flags
    uint16_t distance_to_dock;      // Distance to dock (mm)
    uint16_t alignment_angle;       // Alignment angle (degrees * 10)
    uint8_t fault_status;           // Fault status flags
    uint8_t fault_code;             // Specific fault code
    uint16_t fault_count;           // Fault occurrence count
    uint16_t temperature;           // Module temperature (°C)
    uint16_t voltage;               // Supply voltage (V * 10)
    uint16_t current;               // Current consumption (mA)
    uint32_t uptime;                // Module uptime (seconds)
    uint16_t docking_count;         // Successful docking count
    uint16_t failed_docking_count;  // Failed docking count
    uint32_t last_docking_time;     // Last docking time (ms)
} dock_module_data_t;

// Dock module handler structure
typedef struct dock_module_handler {
    dock_module_data_t data;        // Current module data
    dock_config_t config;           // Module configuration
    bool initialized;               // Initialization flag
    bool enabled;                   // Enable status
    uint8_t address;                // Module address
    uint32_t last_update_time;      // Last update timestamp
    uint32_t fault_timestamp;       // Fault occurrence timestamp
    
    // Event callback function
    void (*event_callback)(dock_module_handler_t *handler, dock_event_t event);
    
    // Private data
    uint32_t docking_start_time;    // Docking sequence start time
    uint32_t calibration_start_time; // Calibration start time
    uint16_t retry_attempts;        // Current retry attempts
    bool emergency_stop_active;     // Emergency stop status
} dock_module_handler_t;

// ============================================================================
// DEFAULT CONFIGURATIONS
// ============================================================================

// Default dock configuration
#define DOCK_DEFAULT_APPROACH_DISTANCE    500     // 500mm
#define DOCK_DEFAULT_FINAL_SPEED          50      // 50mm/s
#define DOCK_DEFAULT_ACCURACY_THRESHOLD   2       // 2mm
#define DOCK_DEFAULT_TIMEOUT              30      // 30 seconds
#define DOCK_DEFAULT_RETRY_COUNT          3       // 3 attempts
#define DOCK_DEFAULT_DEBOUNCE_TIME        100     // 100ms
#define DOCK_DEFAULT_ALIGNMENT_TOLERANCE  50      // 5.0 degrees
#define DOCK_DEFAULT_ADDRESS              0x05    // Module address

// Limits
#define DOCK_MAX_APPROACH_DISTANCE        2000    // 2m
#define DOCK_MAX_FINAL_SPEED              200     // 200mm/s
#define DOCK_MAX_ACCURACY_THRESHOLD       10      // 10mm
#define DOCK_MAX_TIMEOUT                  120     // 2 minutes
#define DOCK_MAX_RETRY_COUNT              10      // 10 attempts
#define DOCK_MAX_DEBOUNCE_TIME            1000    // 1 second
#define DOCK_MAX_ALIGNMENT_TOLERANCE      200     // 20.0 degrees

// ============================================================================
// API FUNCTION PROTOTYPES
// ============================================================================

// ============================================================================
// Core Module Functions
// ============================================================================

/**
 * @brief Initialize dock module handler
 * @param handler Pointer to dock module handler
 * @param address Module address
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_init(dock_module_handler_t *handler, uint8_t address);

/**
 * @brief Deinitialize dock module handler
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_deinit(dock_module_handler_t *handler);

/**
 * @brief Update dock module status
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_update(dock_module_handler_t *handler);

/**
 * @brief Enable or disable dock module
 * @param handler Pointer to dock module handler
 * @param enable Enable flag
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_enable(dock_module_handler_t *handler, bool enable);

// ============================================================================
// Docking Control Functions
// ============================================================================

/**
 * @brief Start docking sequence
 * @param handler Pointer to dock module handler
 * @param target_position Target dock position (mm)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_start_docking(dock_module_handler_t *handler, uint16_t target_position);

/**
 * @brief Stop docking sequence
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_stop_docking(dock_module_handler_t *handler);

/**
 * @brief Emergency stop docking
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_emergency_stop(dock_module_handler_t *handler);

/**
 * @brief Start undocking sequence
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_start_undocking(dock_module_handler_t *handler);

// ============================================================================
// Position and Status Functions
// ============================================================================

/**
 * @brief Get current dock position
 * @param handler Pointer to dock module handler
 * @param position Pointer to store position value
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_position(dock_module_handler_t *handler, uint16_t *position);

/**
 * @brief Set target dock position
 * @param handler Pointer to dock module handler
 * @param position Target position (mm)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_position(dock_module_handler_t *handler, uint16_t position);

/**
 * @brief Get distance to dock
 * @param handler Pointer to dock module handler
 * @param distance Pointer to store distance value
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_distance_to_dock(dock_module_handler_t *handler, uint16_t *distance);

/**
 * @brief Get alignment angle
 * @param handler Pointer to dock module handler
 * @param angle Pointer to store angle value (degrees * 10)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_alignment_angle(dock_module_handler_t *handler, uint16_t *angle);

/**
 * @brief Get dock status
 * @param handler Pointer to dock module handler
 * @param status Pointer to store status value
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_status(dock_module_handler_t *handler, uint8_t *status);

// ============================================================================
// Configuration Functions
// ============================================================================

/**
 * @brief Set approach distance
 * @param handler Pointer to dock module handler
 * @param distance Approach distance (mm)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_approach_distance(dock_module_handler_t *handler, uint16_t distance);

/**
 * @brief Set final docking speed
 * @param handler Pointer to dock module handler
 * @param speed Final speed (mm/s)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_final_speed(dock_module_handler_t *handler, uint16_t speed);

/**
 * @brief Set accuracy threshold
 * @param handler Pointer to dock module handler
 * @param threshold Accuracy threshold (mm)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_accuracy_threshold(dock_module_handler_t *handler, uint16_t threshold);

/**
 * @brief Set docking timeout
 * @param handler Pointer to dock module handler
 * @param timeout Timeout value (seconds)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_timeout(dock_module_handler_t *handler, uint16_t timeout);

/**
 * @brief Set retry count
 * @param handler Pointer to dock module handler
 * @param retry_count Max retry attempts
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_retry_count(dock_module_handler_t *handler, uint8_t retry_count);

/**
 * @brief Set debounce time
 * @param handler Pointer to dock module handler
 * @param debounce_time Debounce time (ms)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_debounce_time(dock_module_handler_t *handler, uint16_t debounce_time);

/**
 * @brief Set alignment tolerance
 * @param handler Pointer to dock module handler
 * @param tolerance Alignment tolerance (degrees * 10)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_alignment_tolerance(dock_module_handler_t *handler, uint16_t tolerance);

// ============================================================================
// Calibration Functions
// ============================================================================

/**
 * @brief Start calibration sequence
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_start_calibration(dock_module_handler_t *handler);

/**
 * @brief Stop calibration sequence
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_stop_calibration(dock_module_handler_t *handler);

/**
 * @brief Set dock reference position
 * @param handler Pointer to dock module handler
 * @param position Reference position (mm)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_reference_position(dock_module_handler_t *handler, uint16_t position);

// ============================================================================
// Fault and Diagnostic Functions
// ============================================================================

/**
 * @brief Reset fault conditions
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_reset_faults(dock_module_handler_t *handler);

/**
 * @brief Get fault status
 * @param handler Pointer to dock module handler
 * @param fault_status Pointer to store fault status
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_fault_status(dock_module_handler_t *handler, uint8_t *fault_status);

/**
 * @brief Get fault code
 * @param handler Pointer to dock module handler
 * @param fault_code Pointer to store fault code
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_fault_code(dock_module_handler_t *handler, uint8_t *fault_code);

/**
 * @brief Get fault description
 * @param handler Pointer to dock module handler
 * @return Pointer to fault description string
 */
const char* dock_module_get_fault_description(dock_module_handler_t *handler);

/**
 * @brief Get module temperature
 * @param handler Pointer to dock module handler
 * @param temperature Pointer to store temperature value (°C)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_temperature(dock_module_handler_t *handler, uint16_t *temperature);

/**
 * @brief Get supply voltage
 * @param handler Pointer to dock module handler
 * @param voltage Pointer to store voltage value (V * 10)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_voltage(dock_module_handler_t *handler, uint16_t *voltage);

/**
 * @brief Get current consumption
 * @param handler Pointer to dock module handler
 * @param current Pointer to store current value (mA)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_current(dock_module_handler_t *handler, uint16_t *current);

// ============================================================================
// Statistics Functions
// ============================================================================

/**
 * @brief Get successful docking count
 * @param handler Pointer to dock module handler
 * @param count Pointer to store count value
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_docking_count(dock_module_handler_t *handler, uint16_t *count);

/**
 * @brief Get failed docking count
 * @param handler Pointer to dock module handler
 * @param count Pointer to store count value
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_failed_docking_count(dock_module_handler_t *handler, uint16_t *count);

/**
 * @brief Get last docking time
 * @param handler Pointer to dock module handler
 * @param time Pointer to store time value (ms)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_last_docking_time(dock_module_handler_t *handler, uint32_t *time);

/**
 * @brief Get module uptime
 * @param handler Pointer to dock module handler
 * @param uptime Pointer to store uptime value (seconds)
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_uptime(dock_module_handler_t *handler, uint32_t *uptime);

// ============================================================================
// Modbus Communication Functions
// ============================================================================

/**
 * @brief Read single register
 * @param handler Pointer to dock module handler
 * @param reg Register address
 * @param value Pointer to store register value
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_read_register(dock_module_handler_t *handler, uint16_t reg, uint16_t *value);

/**
 * @brief Write single register
 * @param handler Pointer to dock module handler
 * @param reg Register address
 * @param value Register value to write
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_write_register(dock_module_handler_t *handler, uint16_t reg, uint16_t value);

/**
 * @brief Read multiple registers
 * @param handler Pointer to dock module handler
 * @param start_reg Starting register address
 * @param count Number of registers to read
 * @param data Pointer to store register data
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_read_registers(dock_module_handler_t *handler, uint16_t start_reg, uint16_t count, uint16_t *data);

/**
 * @brief Write multiple registers
 * @param handler Pointer to dock module handler
 * @param start_reg Starting register address
 * @param count Number of registers to write
 * @param data Pointer to register data
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_write_registers(dock_module_handler_t *handler, uint16_t start_reg, uint16_t count, uint16_t *data);

// ============================================================================
// Event Handling Functions
// ============================================================================

/**
 * @brief Set event callback function
 * @param handler Pointer to dock module handler
 * @param callback Event callback function
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_set_event_callback(dock_module_handler_t *handler, 
                                           void (*callback)(dock_module_handler_t *handler, dock_event_t event));

/**
 * @brief Trigger event
 * @param handler Pointer to dock module handler
 * @param event Event to trigger
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_trigger_event(dock_module_handler_t *handler, dock_event_t event);

// ============================================================================
// Diagnostic and Utility Functions
// ============================================================================

/**
 * @brief Run self-test
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_self_test(dock_module_handler_t *handler);

/**
 * @brief Get module statistics
 * @param handler Pointer to dock module handler
 * @param stats Pointer to store statistics
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_statistics(dock_module_handler_t *handler, void *stats);

/**
 * @brief Validate configuration
 * @param handler Pointer to dock module handler
 * @return HAL_STATUS_OK if valid, error code if invalid
 */
hal_status_t dock_module_validate_config(dock_module_handler_t *handler);

/**
 * @brief Get module information
 * @param handler Pointer to dock module handler
 * @param info Pointer to store module information
 * @return HAL_STATUS_OK on success, error code on failure
 */
hal_status_t dock_module_get_info(dock_module_handler_t *handler, void *info);

#endif // DOCK_MODULE_HANDLER_H
