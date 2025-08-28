/**
 * @file travel_motor_module_handler.h
 * @brief Travel Motor Module Handler for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-Phase3-1 (Travel Motor Module Implementation)
 */

#ifndef TRAVEL_MOTOR_MODULE_HANDLER_H
#define TRAVEL_MOTOR_MODULE_HANDLER_H

#include "hal_common.h"
#include "communication_manager.h"
#include "safety_manager.h"
#include "register_map.h"
#include <stdint.h>
#include <stdbool.h>

// Forward declaration for callback function
typedef struct motor_module_handler motor_module_handler_t;





// Motor Module Configuration
#define MOTOR_MODULE_MAX_ADDRESSES      16
#define MOTOR_MODULE_MAX_FAULTS         8
#define MOTOR_MODULE_MAX_POSITION       65535  // 16-bit position
#define MOTOR_MODULE_MAX_VELOCITY       10000  // 10,000 units/sec
#define MOTOR_MODULE_MAX_ACCELERATION   5000   // 5,000 units/sec²
#define MOTOR_MODULE_STOP_ACCURACY_MM   2      // ±2mm accuracy requirement
#define MOTOR_MODULE_RESPONSE_TIME_MS   100    // < 100ms response time

// Motor Module Register Map (Address 0x1000-0x30FF)
#define MOTOR_POSITION_TARGET_REG       0x1000
#define MOTOR_VELOCITY_TARGET_REG       0x1001
#define MOTOR_ACCELERATION_LIMIT_REG    0x1002
#define MOTOR_JERK_LIMIT_REG            0x1003
#define MOTOR_POSITION_LIMIT_MIN_REG    0x1004
#define MOTOR_POSITION_LIMIT_MAX_REG    0x1005
#define MOTOR_VELOCITY_LIMIT_MAX_REG    0x1006
#define MOTOR_ACCELERATION_LIMIT_MAX_REG 0x1007

#define MOTOR_ENABLE_REG                0x2000
#define MOTOR_MOVE_COMMAND_REG          0x2001
#define MOTOR_STOP_COMMAND_REG          0x2002
#define MOTOR_HOME_COMMAND_REG          0x2003
#define MOTOR_RESET_FAULTS_REG          0x2004
#define MOTOR_EMERGENCY_STOP_REG        0x2005
#define MOTOR_SOFT_STOP_REG             0x2006
#define MOTOR_HARD_STOP_REG             0x2007

#define MOTOR_FAULT_STATUS_REG          0x3000
#define MOTOR_CURRENT_POSITION_REG      0x3001
#define MOTOR_CURRENT_VELOCITY_REG      0x3002
#define MOTOR_REG_MOTOR1_SPEED          0x3002  // Motor 1 speed register
#define MOTOR_CURRENT_ACCELERATION_REG  0x3003
#define MOTOR_TARGET_REACHED_REG        0x3004
#define MOTOR_MOTION_COMPLETE_REG       0x3005
#define MOTOR_FAULT_CODE_REG            0x3006
#define MOTOR_FAULT_DESCRIPTION_REG     0x3007

// Motor Module Fault Codes
typedef enum {
    MOTOR_FAULT_NONE = 0,
    MOTOR_FAULT_OVERCURRENT,
    MOTOR_FAULT_OVERVOLTAGE,
    MOTOR_FAULT_UNDERVOLTAGE,
    MOTOR_FAULT_OVERTEMPERATURE,
    MOTOR_FAULT_POSITION_ERROR,
    MOTOR_FAULT_VELOCITY_ERROR,
    MOTOR_FAULT_COMMUNICATION_ERROR,
    MOTOR_FAULT_SAFETY_VIOLATION,
    MOTOR_FAULT_EMERGENCY_STOP,
    MOTOR_FAULT_ENCODER_ERROR,
    MOTOR_FAULT_DRIVER_ERROR,
    MOTOR_FAULT_POSITION_LIMIT_EXCEEDED,
    MOTOR_FAULT_VELOCITY_LIMIT_EXCEEDED,
    MOTOR_FAULT_ACCELERATION_LIMIT_EXCEEDED,
    MOTOR_FAULT_TIMEOUT
} motor_fault_code_t;

// Motor Module States
typedef enum {
    MOTOR_STATE_DISABLED = 0,
    MOTOR_STATE_ENABLED,
    MOTOR_STATE_MOVING,
    MOTOR_STATE_STOPPING,
    MOTOR_STATE_STOPPED,
    MOTOR_STATE_FAULT,
    MOTOR_STATE_EMERGENCY_STOP,
    MOTOR_STATE_HOMING
} motor_state_t;

// Motor Module Events
typedef enum {
    MOTOR_EVENT_NONE = 0,
    MOTOR_EVENT_ENABLED,
    MOTOR_EVENT_DISABLED,
    MOTOR_EVENT_MOVE_STARTED,
    MOTOR_EVENT_MOVE_COMPLETED,
    MOTOR_EVENT_STOP_STARTED,
    MOTOR_EVENT_STOP_COMPLETED,
    MOTOR_EVENT_FAULT_DETECTED,
    MOTOR_EVENT_FAULT_CLEARED,
    MOTOR_EVENT_EMERGENCY_STOP,
    MOTOR_EVENT_TARGET_REACHED,
    MOTOR_EVENT_POSITION_LIMIT,
    MOTOR_EVENT_VELOCITY_LIMIT,
    MOTOR_EVENT_ACCELERATION_LIMIT,
    MOTOR_EVENT_HOME_COMPLETED
} motor_event_t;

// Motor Module Data Structure
typedef struct {
    // Target values
    uint16_t position_target;
    uint16_t velocity_target;
    uint16_t acceleration_limit;
    uint16_t jerk_limit;
    
    // Limits
    uint16_t position_limit_min;
    uint16_t position_limit_max;
    uint16_t velocity_limit_max;
    uint16_t acceleration_limit_max;
    
    // Current values
    uint16_t current_position;
    uint16_t current_velocity;
    uint16_t current_acceleration;
    
    // Status
    uint8_t enable_status;
    uint8_t fault_status;
    uint8_t target_reached;
    uint8_t motion_complete;
    
    // Fault information
    motor_fault_code_t fault_code;
    char fault_description[64];
    
    // State
    motor_state_t state;
    uint64_t last_update_time;
    uint64_t move_start_time;
    uint64_t stop_start_time;
} motor_module_data_t;

// Motor Module Handler Structure
typedef struct motor_module_handler {
    uint8_t address;
    motor_module_data_t data;
    motor_state_t state;
    motor_fault_code_t last_fault;
    uint64_t last_command_time;
    uint64_t last_response_time;
    uint32_t command_timeout_ms;
    uint32_t response_timeout_ms;
    bool initialized;
    bool enabled;
    bool fault_detected;
    bool emergency_stop_active;
    
    // Statistics
    uint32_t total_moves;
    uint32_t successful_moves;
    uint32_t failed_moves;
    uint32_t total_stops;
    uint32_t emergency_stops;
    uint64_t total_runtime;
    uint64_t total_distance;
    
    // Event callback
    void (*event_callback)(motor_module_handler_t *handler, motor_event_t event);
} motor_module_handler_t;

// Motor Module Configuration
typedef struct {
    uint8_t address;
    uint32_t command_timeout_ms;
    uint32_t response_timeout_ms;
    uint16_t default_velocity;
    uint16_t default_acceleration;
    uint16_t default_jerk;
    bool enable_safety_checks;
    bool enable_position_limits;
    bool enable_velocity_limits;
    bool enable_acceleration_limits;
} motor_module_config_t;

// Public API Functions

/**
 * @brief Initialize Motor Module Handler
 * @param handler Motor module handler structure
 * @param config Configuration structure
 * @return HAL status
 */
hal_status_t motor_module_init(motor_module_handler_t *handler, const motor_module_config_t *config);

/**
 * @brief Deinitialize Motor Module Handler
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_deinit(motor_module_handler_t *handler);

/**
 * @brief Update Motor Module Handler (call periodically)
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_update(motor_module_handler_t *handler);

/**
 * @brief Enable/Disable Motor Module
 * @param handler Motor module handler structure
 * @param enable Enable flag
 * @return HAL status
 */
hal_status_t motor_module_enable(motor_module_handler_t *handler, bool enable);

/**
 * @brief Move Motor to Position
 * @param handler Motor module handler structure
 * @param position Target position
 * @param velocity Target velocity (0 = use default)
 * @param acceleration Acceleration limit (0 = use default)
 * @return HAL status
 */
hal_status_t motor_module_move_to_position(motor_module_handler_t *handler, uint16_t position, 
                                         uint16_t velocity, uint16_t acceleration);

/**
 * @brief Stop Motor (Soft Stop)
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_stop(motor_module_handler_t *handler);

/**
 * @brief Emergency Stop Motor
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_emergency_stop(motor_module_handler_t *handler);

/**
 * @brief Hard Stop Motor
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_hard_stop(motor_module_handler_t *handler);

/**
 * @brief Home Motor
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_home(motor_module_handler_t *handler);

/**
 * @brief Reset Motor Faults
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_reset_faults(motor_module_handler_t *handler);

// Data Access Functions

/**
 * @brief Get Motor Module Data
 * @param handler Motor module handler structure
 * @param data Data structure (output)
 * @return HAL status
 */
hal_status_t motor_module_get_data(motor_module_handler_t *handler, motor_module_data_t *data);

/**
 * @brief Set Motor Module Target Position
 * @param handler Motor module handler structure
 * @param position Target position
 * @return HAL status
 */
hal_status_t motor_module_set_position_target(motor_module_handler_t *handler, uint16_t position);

/**
 * @brief Set Motor Module Target Velocity
 * @param handler Motor module handler structure
 * @param velocity Target velocity
 * @return HAL status
 */
hal_status_t motor_module_set_velocity_target(motor_module_handler_t *handler, uint16_t velocity);

/**
 * @brief Set Motor Module Acceleration Limit
 * @param handler Motor module handler structure
 * @param acceleration Acceleration limit
 * @return HAL status
 */
hal_status_t motor_module_set_acceleration_limit(motor_module_handler_t *handler, uint16_t acceleration);

// Status and Monitoring Functions

/**
 * @brief Get Motor Module Status
 * @param handler Motor module handler structure
 * @param state Current state (output)
 * @param fault_code Current fault code (output)
 * @return HAL status
 */
hal_status_t motor_module_get_status(motor_module_handler_t *handler, motor_state_t *state, 
                                   motor_fault_code_t *fault_code);

/**
 * @brief Check if Motor is Moving
 * @param handler Motor module handler structure
 * @return true if moving, false otherwise
 */
bool motor_module_is_moving(motor_module_handler_t *handler);

/**
 * @brief Check if Motor is Enabled
 * @param handler Motor module handler structure
 * @return true if enabled, false otherwise
 */
bool motor_module_is_enabled(motor_module_handler_t *handler);

/**
 * @brief Check if Motor has Faults
 * @param handler Motor module handler structure
 * @return true if faults detected, false otherwise
 */
bool motor_module_has_faults(motor_module_handler_t *handler);

/**
 * @brief Get Motor Fault Description
 * @param handler Motor module handler structure
 * @return Fault description string
 */
const char* motor_module_get_fault_description(motor_module_handler_t *handler);

// Safety and Validation Functions

/**
 * @brief Check Motor Safety Status
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_check_safety(motor_module_handler_t *handler);

/**
 * @brief Validate Motor Position
 * @param handler Motor module handler structure
 * @param position Position to validate
 * @return true if valid, false otherwise
 */
bool motor_module_validate_position(motor_module_handler_t *handler, uint16_t position);

/**
 * @brief Validate Motor Velocity
 * @param handler Motor module handler structure
 * @param velocity Velocity to validate
 * @return true if valid, false otherwise
 */
bool motor_module_validate_velocity(motor_module_handler_t *handler, uint16_t velocity);

/**
 * @brief Validate Motor Acceleration
 * @param handler Motor module handler structure
 * @param acceleration Acceleration to validate
 * @return true if valid, false otherwise
 */
bool motor_module_validate_acceleration(motor_module_handler_t *handler, uint16_t acceleration);

// Modbus Communication Functions

/**
 * @brief Read Motor Register
 * @param handler Motor module handler structure
 * @param register_addr Register address
 * @param value Register value (output)
 * @return HAL status
 */
hal_status_t motor_module_read_register(motor_module_handler_t *handler, uint16_t register_addr, uint16_t *value);

/**
 * @brief Write Motor Register
 * @param handler Motor module handler structure
 * @param register_addr Register address
 * @param value Register value
 * @return HAL status
 */
hal_status_t motor_module_write_register(motor_module_handler_t *handler, uint16_t register_addr, uint16_t value);

/**
 * @brief Read Multiple Motor Registers
 * @param handler Motor module handler structure
 * @param start_register Starting register address
 * @param count Number of registers to read
 * @param data Register data array (output)
 * @return HAL status
 */
hal_status_t motor_module_read_registers(motor_module_handler_t *handler, uint16_t start_register, 
                                       uint16_t count, uint16_t *data);

/**
 * @brief Write Multiple Motor Registers
 * @param handler Motor module handler structure
 * @param start_register Starting register address
 * @param count Number of registers to write
 * @param data Register data array
 * @return HAL status
 */
hal_status_t motor_module_write_registers(motor_module_handler_t *handler, uint16_t start_register, 
                                        uint16_t count, const uint16_t *data);

// Event and Callback Functions

/**
 * @brief Set Motor Module Event Callback
 * @param handler Motor module handler structure
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t motor_module_set_callback(motor_module_handler_t *handler, 
                                     void (*callback)(motor_module_handler_t *handler, motor_event_t event));

/**
 * @brief Get Motor Module State Name
 * @param state Motor state
 * @return State name string
 */
const char* motor_module_get_state_name(motor_state_t state);

/**
 * @brief Get Motor Module Event Name
 * @param event Motor event
 * @return Event name string
 */
const char* motor_module_get_event_name(motor_event_t event);

/**
 * @brief Get Motor Module Fault Name
 * @param fault_code Motor fault code
 * @return Fault name string
 */
const char* motor_module_get_fault_name(motor_fault_code_t fault_code);

// Utility Functions

/**
 * @brief Get Motor Module Diagnostics
 * @param handler Motor module handler structure
 * @param info Diagnostic information buffer
 * @param max_len Maximum buffer length
 * @return HAL status
 */
hal_status_t motor_module_get_diagnostics(motor_module_handler_t *handler, char *info, size_t max_len);

/**
 * @brief Perform Motor Module Self-Test
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_self_test(motor_module_handler_t *handler);

/**
 * @brief Reset Motor Module Statistics
 * @param handler Motor module handler structure
 * @return HAL status
 */
hal_status_t motor_module_reset_statistics(motor_module_handler_t *handler);

/**
 * @brief Get Motor Module Statistics
 * @param handler Motor module handler structure
 * @param total_moves Total moves (output)
 * @param successful_moves Successful moves (output)
 * @param failed_moves Failed moves (output)
 * @param total_runtime Total runtime in ms (output)
 * @return HAL status
 */
hal_status_t motor_module_get_statistics(motor_module_handler_t *handler, uint32_t *total_moves, 
                                       uint32_t *successful_moves, uint32_t *failed_moves, 
                                       uint64_t *total_runtime);

/**
 * @brief Poll motor module data (continuous polling function)
 * @param handler Motor module handler
 * @return HAL status
 */
hal_status_t motor_module_handler_poll_data(motor_module_handler_t *handler);

#endif // TRAVEL_MOTOR_MODULE_HANDLER_H
