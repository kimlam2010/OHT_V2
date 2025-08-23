/**
 * @file motor_module_handler.c
 * @brief Motor Module Handler Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-29 (Motor Module Implementation)
 */

#include "motor_module_handler.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Default configuration
static const motor_module_config_t default_config = {
    .address = 0x03,
    .command_timeout_ms = 1000,
    .response_timeout_ms = 500,
    .default_velocity = 1000,
    .default_acceleration = 500,
    .default_jerk = 100,
    .enable_safety_checks = true,
    .enable_position_limits = true,
    .enable_velocity_limits = true,
    .enable_acceleration_limits = true
};

// Forward declarations
static hal_status_t handle_motor_event(motor_module_handler_t *handler, motor_event_t event);
static hal_status_t update_motor_state(motor_module_handler_t *handler);
static hal_status_t check_motor_faults(motor_module_handler_t *handler);
static hal_status_t validate_motor_limits(motor_module_handler_t *handler);
static hal_status_t simulate_motor_movement(motor_module_handler_t *handler);

// Motor Module Handler implementation

hal_status_t motor_module_init(motor_module_handler_t *handler, const motor_module_config_t *config) {
    printf("[MOTOR] motor_module_init called\n");
    
    if (handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Clear the handler structure first
    memset(handler, 0, sizeof(motor_module_handler_t));
    
    if (handler->initialized) {
        printf("[MOTOR] Already initialized, returning\n");
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    

    
    // Set configuration
    if (config != NULL) {
        printf("[MOTOR] Using provided config\n");
        handler->address = config->address;
        handler->command_timeout_ms = config->command_timeout_ms;
        handler->response_timeout_ms = config->response_timeout_ms;
    } else {
        printf("[MOTOR] Using default config\n");
        handler->address = default_config.address;
        handler->command_timeout_ms = default_config.command_timeout_ms;
        handler->response_timeout_ms = default_config.response_timeout_ms;
    }
    
    // Initialize motor data
    handler->data.position_target = 0;
    handler->data.velocity_target = (config != NULL) ? config->default_velocity : default_config.default_velocity;
    handler->data.acceleration_limit = (config != NULL) ? config->default_acceleration : default_config.default_acceleration;
    handler->data.jerk_limit = (config != NULL) ? config->default_jerk : default_config.default_jerk;
    
    // Set position limits
    handler->data.position_limit_min = 0;
    handler->data.position_limit_max = MOTOR_MODULE_MAX_POSITION;
    handler->data.velocity_limit_max = MOTOR_MODULE_MAX_VELOCITY;
    handler->data.acceleration_limit_max = MOTOR_MODULE_MAX_ACCELERATION;
    
    // Initialize current values
    handler->data.current_position = 0;
    handler->data.current_velocity = 0;
    handler->data.current_acceleration = 0;
    
    // Initialize status
    handler->data.enable_status = 0;
    handler->data.fault_status = 0;
    handler->data.target_reached = 0;
    handler->data.motion_complete = 1;
    
    // Initialize fault information
    handler->data.fault_code = MOTOR_FAULT_NONE;
    strcpy(handler->data.fault_description, "No faults");
    
    // Initialize state
    handler->data.state = MOTOR_STATE_DISABLED;
    handler->data.last_update_time = hal_get_timestamp_us();
    handler->data.move_start_time = 0;
    handler->data.stop_start_time = 0;
    
    // Initialize handler state
    handler->state = MOTOR_STATE_DISABLED;
    handler->last_fault = MOTOR_FAULT_NONE;
    handler->last_command_time = 0;
    handler->last_response_time = 0;
    handler->initialized = true;
    handler->enabled = false;
    handler->fault_detected = false;
    handler->emergency_stop_active = false;
    
    // Initialize statistics
    handler->total_moves = 0;
    handler->successful_moves = 0;
    handler->failed_moves = 0;
    handler->total_stops = 0;
    handler->emergency_stops = 0;
    handler->total_runtime = 0;
    handler->total_distance = 0;
    
    printf("[MOTOR] Initialized successfully (Address: 0x%02X)\n", handler->address);
    return HAL_STATUS_OK;
}

hal_status_t motor_module_deinit(motor_module_handler_t *handler) {
    if (handler == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("[MOTOR] Deinitializing motor module\n");
    
    // Emergency stop if moving
    if (handler->state == MOTOR_STATE_MOVING) {
        motor_module_emergency_stop(handler);
    }
    
    // Clear handler structure
    memset(handler, 0, sizeof(motor_module_handler_t));
    
    return HAL_STATUS_OK;
}

hal_status_t motor_module_update(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    uint64_t current_time = hal_get_timestamp_us();
    
    // Update motor state
    hal_status_t status = update_motor_state(handler);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check for faults
    status = check_motor_faults(handler);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Validate motor limits
    status = validate_motor_limits(handler);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Simulate motor movement (for testing)
    status = simulate_motor_movement(handler);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Update timestamps
    handler->data.last_update_time = current_time;
    handler->last_response_time = current_time;
    
    return HAL_STATUS_OK;
}

hal_status_t motor_module_enable(motor_module_handler_t *handler, bool enable) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("[MOTOR] %s motor module\n", enable ? "Enabling" : "Disabling");
    
    // Check safety before enabling
    if (enable) {
        hal_status_t status = motor_module_check_safety(handler);
        if (status != HAL_STATUS_OK) {
            printf("[MOTOR] Safety check failed, cannot enable\n");
            return status;
        }
    }
    
    // Write enable register
    hal_status_t status = motor_module_write_register(handler, MOTOR_ENABLE_REG, enable ? 1 : 0);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to write enable register\n");
        return status;
    }
    
    // Update local state
    handler->enabled = enable;
    handler->data.enable_status = enable ? 1 : 0;
    
    if (enable) {
        handler->state = MOTOR_STATE_ENABLED;
        handler->data.state = MOTOR_STATE_ENABLED;
        handle_motor_event(handler, MOTOR_EVENT_ENABLED);
    } else {
        handler->state = MOTOR_STATE_DISABLED;
        handler->data.state = MOTOR_STATE_DISABLED;
        handle_motor_event(handler, MOTOR_EVENT_DISABLED);
    }
    
    printf("[MOTOR] Motor module %s\n", enable ? "enabled" : "disabled");
    return HAL_STATUS_OK;
}

hal_status_t motor_module_move_to_position(motor_module_handler_t *handler, uint16_t position, 
                                         uint16_t velocity, uint16_t acceleration) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!handler->enabled) {
        printf("[MOTOR] Motor not enabled, cannot move\n");
        return HAL_STATUS_ERROR;
    }
    
    printf("[MOTOR] Moving to position %u\n", position);
    
    // Validate parameters
    if (!motor_module_validate_position(handler, position)) {
        printf("[MOTOR] Invalid position: %u\n", position);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (velocity > 0 && !motor_module_validate_velocity(handler, velocity)) {
        printf("[MOTOR] Invalid velocity: %u\n", velocity);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (acceleration > 0 && !motor_module_validate_acceleration(handler, acceleration)) {
        printf("[MOTOR] Invalid acceleration: %u\n", acceleration);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check safety
    hal_status_t status = motor_module_check_safety(handler);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Safety check failed, cannot move\n");
        return status;
    }
    
    // Set target values
    handler->data.position_target = position;
    if (velocity > 0) {
        handler->data.velocity_target = velocity;
    }
    if (acceleration > 0) {
        handler->data.acceleration_limit = acceleration;
    }
    
    // Write target registers
    status = motor_module_write_register(handler, MOTOR_POSITION_TARGET_REG, position);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to write position target\n");
        return status;
    }
    
    status = motor_module_write_register(handler, MOTOR_VELOCITY_TARGET_REG, handler->data.velocity_target);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to write velocity target\n");
        return status;
    }
    
    status = motor_module_write_register(handler, MOTOR_ACCELERATION_LIMIT_REG, handler->data.acceleration_limit);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to write acceleration limit\n");
        return status;
    }
    
    // Send move command
    status = motor_module_write_register(handler, MOTOR_MOVE_COMMAND_REG, 1);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to send move command\n");
        return status;
    }
    
    // Update state
    handler->state = MOTOR_STATE_MOVING;
    handler->data.state = MOTOR_STATE_MOVING;
    handler->data.target_reached = 0;
    handler->data.motion_complete = 0;
    handler->data.move_start_time = hal_get_timestamp_us();
    handler->last_command_time = hal_get_timestamp_us();
    
    // Update statistics
    handler->total_moves++;
    
    handle_motor_event(handler, MOTOR_EVENT_MOVE_STARTED);
    
    printf("[MOTOR] Move command sent successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t motor_module_stop(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("[MOTOR] Stopping motor (soft stop)\n");
    
    // Send stop command
    hal_status_t status = motor_module_write_register(handler, MOTOR_STOP_COMMAND_REG, 1);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to send stop command\n");
        return status;
    }
    
    // Update state
    handler->state = MOTOR_STATE_STOPPING;
    handler->data.state = MOTOR_STATE_STOPPING;
    handler->data.stop_start_time = hal_get_timestamp_us();
    handler->last_command_time = hal_get_timestamp_us();
    
    // Update statistics
    handler->total_stops++;
    
    handle_motor_event(handler, MOTOR_EVENT_STOP_STARTED);
    
    printf("[MOTOR] Stop command sent successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t motor_module_emergency_stop(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("[MOTOR] Emergency stop motor\n");
    
    // Send emergency stop command
    hal_status_t status = motor_module_write_register(handler, MOTOR_EMERGENCY_STOP_REG, 1);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to send emergency stop command\n");
        return status;
    }
    
    // Update state
    handler->state = MOTOR_STATE_EMERGENCY_STOP;
    handler->data.state = MOTOR_STATE_EMERGENCY_STOP;
    handler->emergency_stop_active = true;
    handler->last_command_time = hal_get_timestamp_us();
    
    // Update statistics
    handler->emergency_stops++;
    
    handle_motor_event(handler, MOTOR_EVENT_EMERGENCY_STOP);
    
    printf("[MOTOR] Emergency stop command sent successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t motor_module_hard_stop(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("[MOTOR] Hard stop motor\n");
    
    // Send hard stop command
    hal_status_t status = motor_module_write_register(handler, MOTOR_HARD_STOP_REG, 1);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to send hard stop command\n");
        return status;
    }
    
    // Update state
    handler->state = MOTOR_STATE_STOPPING;
    handler->data.state = MOTOR_STATE_STOPPING;
    handler->data.stop_start_time = hal_get_timestamp_us();
    handler->last_command_time = hal_get_timestamp_us();
    
    // Update statistics
    handler->total_stops++;
    
    handle_motor_event(handler, MOTOR_EVENT_STOP_STARTED);
    
    printf("[MOTOR] Hard stop command sent successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t motor_module_home(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!handler->enabled) {
        printf("[MOTOR] Motor not enabled, cannot home\n");
        return HAL_STATUS_ERROR;
    }
    
    printf("[MOTOR] Homing motor\n");
    
    // Send home command
    hal_status_t status = motor_module_write_register(handler, MOTOR_HOME_COMMAND_REG, 1);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to send home command\n");
        return status;
    }
    
    // Update state
    handler->state = MOTOR_STATE_HOMING;
    handler->data.state = MOTOR_STATE_HOMING;
    handler->last_command_time = hal_get_timestamp_us();
    
    printf("[MOTOR] Home command sent successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t motor_module_reset_faults(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("[MOTOR] Resetting faults\n");
    
    // Send reset faults command
    hal_status_t status = motor_module_write_register(handler, MOTOR_RESET_FAULTS_REG, 1);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to send reset faults command\n");
        return status;
    }
    
    // Clear local fault state
    handler->fault_detected = false;
    handler->data.fault_status = 0;
    handler->data.fault_code = MOTOR_FAULT_NONE;
    strcpy(handler->data.fault_description, "No faults");
    
    if (handler->state == MOTOR_STATE_FAULT) {
        handler->state = MOTOR_STATE_DISABLED;
        handler->data.state = MOTOR_STATE_DISABLED;
    }
    
    handler->last_command_time = hal_get_timestamp_us();
    
    handle_motor_event(handler, MOTOR_EVENT_FAULT_CLEARED);
    
    printf("[MOTOR] Faults reset successfully\n");
    return HAL_STATUS_OK;
}

// Data Access Functions

hal_status_t motor_module_get_data(motor_module_handler_t *handler, motor_module_data_t *data) {
    if (handler == NULL || data == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    memcpy(data, &handler->data, sizeof(motor_module_data_t));
    return HAL_STATUS_OK;
}

hal_status_t motor_module_set_position_target(motor_module_handler_t *handler, uint16_t position) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!motor_module_validate_position(handler, position)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->data.position_target = position;
    return motor_module_write_register(handler, MOTOR_POSITION_TARGET_REG, position);
}

hal_status_t motor_module_set_velocity_target(motor_module_handler_t *handler, uint16_t velocity) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!motor_module_validate_velocity(handler, velocity)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->data.velocity_target = velocity;
    return motor_module_write_register(handler, MOTOR_VELOCITY_TARGET_REG, velocity);
}

hal_status_t motor_module_set_acceleration_limit(motor_module_handler_t *handler, uint16_t acceleration) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!motor_module_validate_acceleration(handler, acceleration)) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->data.acceleration_limit = acceleration;
    return motor_module_write_register(handler, MOTOR_ACCELERATION_LIMIT_REG, acceleration);
}

// Status and Monitoring Functions

hal_status_t motor_module_get_status(motor_module_handler_t *handler, motor_state_t *state, 
                                   motor_fault_code_t *fault_code) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (state != NULL) {
        *state = handler->state;
    }
    
    if (fault_code != NULL) {
        *fault_code = handler->data.fault_code;
    }
    
    return HAL_STATUS_OK;
}

bool motor_module_is_moving(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return false;
    }
    
    return (handler->state == MOTOR_STATE_MOVING);
}

bool motor_module_is_enabled(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return false;
    }
    
    return handler->enabled;
}

bool motor_module_has_faults(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return false;
    }
    
    return handler->fault_detected;
}

const char* motor_module_get_fault_description(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return "Handler not initialized";
    }
    
    return handler->data.fault_description;
}

// Safety and Validation Functions

hal_status_t motor_module_check_safety(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check E-Stop status
    safety_status_t safety_status;
    hal_status_t status = safety_manager_get_status(&safety_status);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Failed to get safety status\n");
        return status;
    }
    
    if (!safety_status.safety_circuit_ok) {
        printf("[MOTOR] Safety violation detected\n");
        motor_module_emergency_stop(handler);
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

bool motor_module_validate_position(motor_module_handler_t *handler, uint16_t position) {
    if (handler == NULL || !handler->initialized) {
        return false;
    }
    
    return (position >= handler->data.position_limit_min && 
            position <= handler->data.position_limit_max);
}

bool motor_module_validate_velocity(motor_module_handler_t *handler, uint16_t velocity) {
    if (handler == NULL || !handler->initialized) {
        return false;
    }
    
    return (velocity > 0 && velocity <= handler->data.velocity_limit_max);
}

bool motor_module_validate_acceleration(motor_module_handler_t *handler, uint16_t acceleration) {
    if (handler == NULL || !handler->initialized) {
        return false;
    }
    
    return (acceleration > 0 && acceleration <= handler->data.acceleration_limit_max);
}

// Modbus Communication Functions

hal_status_t motor_module_read_register(motor_module_handler_t *handler, uint16_t register_addr, uint16_t *value) {
    if (handler == NULL || value == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create Modbus request
    comm_mgr_modbus_request_t request = {
        .slave_id = handler->address,
        .function_code = MODBUS_FC_READ_HOLDING_REGISTERS,
        .start_address = register_addr,
        .quantity = 1,
        .data = NULL
    };
    
    comm_mgr_modbus_response_t response;
    hal_status_t status = comm_manager_modbus_send_request(&request, &response);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Modbus read failed for register 0x%04X\n", register_addr);
        return status;
    }
    
    if (response.data_length >= 2) {
        *value = (response.data[0] << 8) | response.data[1];
    } else {
        printf("[MOTOR] Invalid response data length for register 0x%04X\n", register_addr);
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t motor_module_write_register(motor_module_handler_t *handler, uint16_t register_addr, uint16_t value) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create Modbus request
    uint8_t data[2] = {(value >> 8) & 0xFF, value & 0xFF};
    comm_mgr_modbus_request_t request = {
        .slave_id = handler->address,
        .function_code = MODBUS_FC_WRITE_SINGLE_REGISTER,
        .start_address = register_addr,
        .quantity = 1,
        .data = data
    };
    
    comm_mgr_modbus_response_t response;
    hal_status_t status = comm_manager_modbus_send_request(&request, &response);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Modbus write failed for register 0x%04X\n", register_addr);
        return status;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t motor_module_read_registers(motor_module_handler_t *handler, uint16_t start_register, 
                                       uint16_t count, uint16_t *data) {
    if (handler == NULL || data == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create Modbus request
    comm_mgr_modbus_request_t request = {
        .slave_id = handler->address,
        .function_code = MODBUS_FC_READ_HOLDING_REGISTERS,
        .start_address = start_register,
        .quantity = count,
        .data = NULL
    };
    
    comm_mgr_modbus_response_t response;
    hal_status_t status = comm_manager_modbus_send_request(&request, &response);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Modbus read failed for registers 0x%04X-%0x%04X\n", start_register, start_register + count - 1);
        return status;
    }
    
    if (response.data_length >= count * 2) {
        for (uint16_t i = 0; i < count; i++) {
            data[i] = (response.data[i * 2] << 8) | response.data[i * 2 + 1];
        }
    } else {
        printf("[MOTOR] Invalid response data length for registers\n");
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t motor_module_write_registers(motor_module_handler_t *handler, uint16_t start_register, 
                                        uint16_t count, const uint16_t *data) {
    if (handler == NULL || data == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Create Modbus request
    uint8_t *modbus_data = malloc(count * 2);
    if (modbus_data == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    for (uint16_t i = 0; i < count; i++) {
        modbus_data[i * 2] = (data[i] >> 8) & 0xFF;
        modbus_data[i * 2 + 1] = data[i] & 0xFF;
    }
    
    comm_mgr_modbus_request_t request = {
        .slave_id = handler->address,
        .function_code = MODBUS_FC_WRITE_MULTIPLE_REGISTERS,
        .start_address = start_register,
        .quantity = count,
        .data = modbus_data
    };
    
    comm_mgr_modbus_response_t response;
    hal_status_t status = comm_manager_modbus_send_request(&request, &response);
    
    free(modbus_data);
    
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Modbus write failed for registers 0x%04X-%0x%04X\n", start_register, start_register + count - 1);
        return status;
    }
    
    return HAL_STATUS_OK;
}

// Event and Callback Functions

hal_status_t motor_module_set_callback(motor_module_handler_t *handler, 
                                     void (*callback)(motor_module_handler_t *handler, motor_event_t event)) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    handler->event_callback = callback;
    return HAL_STATUS_OK;
}

const char* motor_module_get_state_name(motor_state_t state) {
    switch (state) {
        case MOTOR_STATE_DISABLED: return "DISABLED";
        case MOTOR_STATE_ENABLED: return "ENABLED";
        case MOTOR_STATE_MOVING: return "MOVING";
        case MOTOR_STATE_STOPPING: return "STOPPING";
        case MOTOR_STATE_STOPPED: return "STOPPED";
        case MOTOR_STATE_FAULT: return "FAULT";
        case MOTOR_STATE_EMERGENCY_STOP: return "EMERGENCY_STOP";
        case MOTOR_STATE_HOMING: return "HOMING";
        default: return "UNKNOWN";
    }
}

const char* motor_module_get_event_name(motor_event_t event) {
    switch (event) {
        case MOTOR_EVENT_NONE: return "NONE";
        case MOTOR_EVENT_ENABLED: return "ENABLED";
        case MOTOR_EVENT_DISABLED: return "DISABLED";
        case MOTOR_EVENT_MOVE_STARTED: return "MOVE_STARTED";
        case MOTOR_EVENT_MOVE_COMPLETED: return "MOVE_COMPLETED";
        case MOTOR_EVENT_STOP_STARTED: return "STOP_STARTED";
        case MOTOR_EVENT_STOP_COMPLETED: return "STOP_COMPLETED";
        case MOTOR_EVENT_FAULT_DETECTED: return "FAULT_DETECTED";
        case MOTOR_EVENT_FAULT_CLEARED: return "FAULT_CLEARED";
        case MOTOR_EVENT_EMERGENCY_STOP: return "EMERGENCY_STOP";
        case MOTOR_EVENT_TARGET_REACHED: return "TARGET_REACHED";
        case MOTOR_EVENT_POSITION_LIMIT: return "POSITION_LIMIT";
        case MOTOR_EVENT_VELOCITY_LIMIT: return "VELOCITY_LIMIT";
        case MOTOR_EVENT_ACCELERATION_LIMIT: return "ACCELERATION_LIMIT";
        case MOTOR_EVENT_HOME_COMPLETED: return "HOME_COMPLETED";
        default: return "UNKNOWN";
    }
}

const char* motor_module_get_fault_name(motor_fault_code_t fault_code) {
    switch (fault_code) {
        case MOTOR_FAULT_NONE: return "NONE";
        case MOTOR_FAULT_OVERCURRENT: return "OVERCURRENT";
        case MOTOR_FAULT_OVERVOLTAGE: return "OVERVOLTAGE";
        case MOTOR_FAULT_UNDERVOLTAGE: return "UNDERVOLTAGE";
        case MOTOR_FAULT_OVERTEMPERATURE: return "OVERTEMPERATURE";
        case MOTOR_FAULT_POSITION_ERROR: return "POSITION_ERROR";
        case MOTOR_FAULT_VELOCITY_ERROR: return "VELOCITY_ERROR";
        case MOTOR_FAULT_COMMUNICATION_ERROR: return "COMMUNICATION_ERROR";
        case MOTOR_FAULT_SAFETY_VIOLATION: return "SAFETY_VIOLATION";
        case MOTOR_FAULT_EMERGENCY_STOP: return "EMERGENCY_STOP";
        case MOTOR_FAULT_ENCODER_ERROR: return "ENCODER_ERROR";
        case MOTOR_FAULT_DRIVER_ERROR: return "DRIVER_ERROR";
        case MOTOR_FAULT_POSITION_LIMIT_EXCEEDED: return "POSITION_LIMIT_EXCEEDED";
        case MOTOR_FAULT_VELOCITY_LIMIT_EXCEEDED: return "VELOCITY_LIMIT_EXCEEDED";
        case MOTOR_FAULT_ACCELERATION_LIMIT_EXCEEDED: return "ACCELERATION_LIMIT_EXCEEDED";
        case MOTOR_FAULT_TIMEOUT: return "TIMEOUT";
        default: return "UNKNOWN";
    }
}

// Utility Functions

hal_status_t motor_module_get_diagnostics(motor_module_handler_t *handler, char *info, size_t max_len) {
    if (handler == NULL || info == NULL || !handler->initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(info, max_len,
        "Motor Module Diagnostics:\n"
        "Address: 0x%02X\n"
        "State: %s\n"
        "Enabled: %s\n"
        "Fault Detected: %s\n"
        "Emergency Stop: %s\n"
        "Current Position: %u\n"
        "Target Position: %u\n"
        "Current Velocity: %u\n"
        "Target Velocity: %u\n"
        "Current Acceleration: %u\n"
        "Acceleration Limit: %u\n"
        "Target Reached: %s\n"
        "Motion Complete: %s\n"
        "Fault Code: %s\n"
        "Fault Description: %s\n"
        "Total Moves: %u\n"
        "Successful Moves: %u\n"
        "Failed Moves: %u\n"
        "Total Stops: %u\n"
        "Emergency Stops: %u\n"
        "Total Runtime: %llu ms\n"
        "Total Distance: %lu units\n"
        "Last Command: %llu ms ago\n"
        "Last Response: %llu ms ago\n",
        handler->address,
        motor_module_get_state_name(handler->state),
        handler->enabled ? "YES" : "NO",
        handler->fault_detected ? "YES" : "NO",
        handler->emergency_stop_active ? "YES" : "NO",
        handler->data.current_position,
        handler->data.position_target,
        handler->data.current_velocity,
        handler->data.velocity_target,
        handler->data.current_acceleration,
        handler->data.acceleration_limit,
        handler->data.target_reached ? "YES" : "NO",
        handler->data.motion_complete ? "YES" : "NO",
        motor_module_get_fault_name(handler->data.fault_code),
        handler->data.fault_description,
        handler->total_moves,
        handler->successful_moves,
        handler->failed_moves,
        handler->total_stops,
        handler->emergency_stops,
        handler->total_runtime / 1000ULL,
        handler->total_distance,
        handler->last_command_time > 0 ? (hal_get_timestamp_us() - handler->last_command_time) / 1000ULL : 0,
        handler->last_response_time > 0 ? (hal_get_timestamp_us() - handler->last_response_time) / 1000ULL : 0
    );
    
    return HAL_STATUS_OK;
}

hal_status_t motor_module_self_test(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("[MOTOR] Running self-test...\n");
    
    // Test register read/write
    uint16_t test_value = 0x1234;
    hal_status_t status = motor_module_write_register(handler, MOTOR_POSITION_TARGET_REG, test_value);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Self-test failed: Register write error\n");
        return status;
    }
    
    uint16_t read_value;
    status = motor_module_read_register(handler, MOTOR_POSITION_TARGET_REG, &read_value);
    if (status != HAL_STATUS_OK) {
        printf("[MOTOR] Self-test failed: Register read error\n");
        return status;
    }
    
    if (read_value != test_value) {
        printf("[MOTOR] Self-test failed: Register read/write mismatch\n");
        return HAL_STATUS_ERROR;
    }
    
    printf("[MOTOR] Self-test passed\n");
    return HAL_STATUS_OK;
}

hal_status_t motor_module_reset_statistics(motor_module_handler_t *handler) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    handler->total_moves = 0;
    handler->successful_moves = 0;
    handler->failed_moves = 0;
    handler->total_stops = 0;
    handler->emergency_stops = 0;
    handler->total_runtime = 0;
    handler->total_distance = 0;
    
    return HAL_STATUS_OK;
}

hal_status_t motor_module_get_statistics(motor_module_handler_t *handler, uint32_t *total_moves, 
                                       uint32_t *successful_moves, uint32_t *failed_moves, 
                                       uint64_t *total_runtime) {
    if (handler == NULL || !handler->initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (total_moves != NULL) {
        *total_moves = handler->total_moves;
    }
    if (successful_moves != NULL) {
        *successful_moves = handler->successful_moves;
    }
    if (failed_moves != NULL) {
        *failed_moves = handler->failed_moves;
    }
    if (total_runtime != NULL) {
        *total_runtime = handler->total_runtime;
    }
    
    return HAL_STATUS_OK;
}

// Private Functions

static hal_status_t handle_motor_event(motor_module_handler_t *handler, motor_event_t event) {
    printf("[MOTOR] Event: %s\n", motor_module_get_event_name(event));
    
    if (handler->event_callback != NULL) {
        handler->event_callback(handler, event);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t update_motor_state(motor_module_handler_t *handler) {
    // Read current status registers
    uint16_t fault_status, target_reached, motion_complete;
    
    hal_status_t status = motor_module_read_register(handler, MOTOR_FAULT_STATUS_REG, &fault_status);
    if (status == HAL_STATUS_OK) {
        handler->data.fault_status = fault_status;
    }
    
    status = motor_module_read_register(handler, MOTOR_TARGET_REACHED_REG, &target_reached);
    if (status == HAL_STATUS_OK) {
        handler->data.target_reached = target_reached;
    }
    
    status = motor_module_read_register(handler, MOTOR_MOTION_COMPLETE_REG, &motion_complete);
    if (status == HAL_STATUS_OK) {
        handler->data.motion_complete = motion_complete;
    }
    
    // Update state based on status
    if (handler->data.fault_status > 0) {
        if (handler->state != MOTOR_STATE_FAULT) {
            handler->state = MOTOR_STATE_FAULT;
            handler->data.state = MOTOR_STATE_FAULT;
            handle_motor_event(handler, MOTOR_EVENT_FAULT_DETECTED);
        }
    } else if (handler->data.target_reached && handler->state == MOTOR_STATE_MOVING) {
        handler->state = MOTOR_STATE_STOPPED;
        handler->data.state = MOTOR_STATE_STOPPED;
        handler->successful_moves++;
        handle_motor_event(handler, MOTOR_EVENT_MOVE_COMPLETED);
        handle_motor_event(handler, MOTOR_EVENT_TARGET_REACHED);
    } else if (handler->data.motion_complete && handler->state == MOTOR_STATE_STOPPING) {
        handler->state = MOTOR_STATE_STOPPED;
        handler->data.state = MOTOR_STATE_STOPPED;
        handle_motor_event(handler, MOTOR_EVENT_STOP_COMPLETED);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t check_motor_faults(motor_module_handler_t *handler) {
    if (handler->data.fault_status > 0 && !handler->fault_detected) {
        handler->fault_detected = true;
        
        // Read fault code
        uint16_t fault_code;
        hal_status_t status = motor_module_read_register(handler, MOTOR_FAULT_CODE_REG, &fault_code);
        if (status == HAL_STATUS_OK) {
            handler->data.fault_code = (motor_fault_code_t)fault_code;
            strcpy(handler->data.fault_description, motor_module_get_fault_name(handler->data.fault_code));
        }
        
        handle_motor_event(handler, MOTOR_EVENT_FAULT_DETECTED);
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t validate_motor_limits(motor_module_handler_t *handler) {
    // Check position limits
    if (handler->data.current_position < handler->data.position_limit_min) {
        handle_motor_event(handler, MOTOR_EVENT_POSITION_LIMIT);
        return HAL_STATUS_ERROR;
    }
    
    if (handler->data.current_position > handler->data.position_limit_max) {
        handle_motor_event(handler, MOTOR_EVENT_POSITION_LIMIT);
        return HAL_STATUS_ERROR;
    }
    
    // Check velocity limits
    if (handler->data.current_velocity > handler->data.velocity_limit_max) {
        handle_motor_event(handler, MOTOR_EVENT_VELOCITY_LIMIT);
        return HAL_STATUS_ERROR;
    }
    
    // Check acceleration limits
    if (handler->data.current_acceleration > handler->data.acceleration_limit_max) {
        handle_motor_event(handler, MOTOR_EVENT_ACCELERATION_LIMIT);
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t simulate_motor_movement(motor_module_handler_t *handler) {
    // Simulate motor movement for testing (remove in production)
    if (handler->state == MOTOR_STATE_MOVING) {
        uint64_t current_time = hal_get_timestamp_us();
        uint64_t move_duration = current_time - handler->data.move_start_time;
        
        // Simulate movement towards target
        if (handler->data.current_position < handler->data.position_target) {
            handler->data.current_position++;
        } else if (handler->data.current_position > handler->data.position_target) {
            handler->data.current_position--;
        }
        
        // Simulate reaching target after some time
        if (move_duration > 5000000) { // 5 seconds
            handler->data.target_reached = 1;
            handler->data.motion_complete = 1;
        }
    }
    
    return HAL_STATUS_OK;
}
