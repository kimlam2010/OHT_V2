/**
 * @file register_info.c
 * @brief Register Information and Validation Implementation (Issue #179 Support)
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include "register_info.h"
#include "register_map.h"
#include <stddef.h> // For NULL
#include <stdio.h>  // For printf (debugging)

// ============================================================================
// REGISTER MAPS FOR EACH MODULE (SIMPLIFIED VERSION)
// ============================================================================

// ============================================================================
// POWER MODULE REGISTER MAP (COMPLETE - 51 registers)
// ============================================================================
static const register_info_t power_module_register_map[] = {
    // DalyBMS Status Registers (0x0000-0x002B)
    {POWER_REG_BATTERY_VOLTAGE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Battery Pack Voltage", "V", 0, 65535, 0},
    {POWER_REG_BATTERY_CURRENT, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Battery Pack Current", "A", 0, 65535, 0},
    {POWER_REG_BATTERY_SOC, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "State of Charge", "%", 0, 1000, 0},
    {POWER_REG_MAX_CELL_V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Maximum Cell Voltage", "mV", 0, 65535, 0},
    {POWER_REG_MIN_CELL_V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Minimum Cell Voltage", "mV", 0, 65535, 0},
    {POWER_REG_CELL_DIFF, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Cell Voltage Difference", "mV", 0, 5000, 0},
    {POWER_REG_TEMPERATURE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Average Temperature", "C", 0, 200, 25},
    {POWER_REG_CONNECTION_STATUS, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "BMS Connection Status", "bool", 0, 1, 0},
    {POWER_REG_CHARGE_MOS, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Charge MOSFET Control", "bool", 0, 1, 0},
    {POWER_REG_DISCHARGE_MOS, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Discharge MOSFET Control", "bool", 0, 1, 0},
    
    // Cell Voltages (0x0014-0x0019)
    {POWER_REG_CELL_VOLTAGE_0, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Cell 0 Voltage", "mV", 2500, 4200, 3700},
    {POWER_REG_CELL_VOLTAGE_1, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Cell 1 Voltage", "mV", 2500, 4200, 3700},
    {POWER_REG_CELL_VOLTAGE_2, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Cell 2 Voltage", "mV", 2500, 4200, 3700},
    {POWER_REG_CELL_VOLTAGE_3, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Cell 3 Voltage", "mV", 2500, 4200, 3700},
    {POWER_REG_CELL_VOLTAGE_4, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Cell 4 Voltage", "mV", 2500, 4200, 3700},
    {POWER_REG_CELL_VOLTAGE_5, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Cell 5 Voltage", "mV", 2500, 4200, 3700},
    
    // Temperature Sensors (0x001A-0x001B)
    {POWER_REG_TEMPERATURE_0, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Temperature Sensor 0", "C", 0, 100, 25},
    {POWER_REG_TEMPERATURE_1, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Temperature Sensor 1", "C", 0, 100, 25},
    
    // Cell Balancing (0x001C-0x0023)
    {POWER_REG_CELL_BALANCE_0, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Cell 0 Balancing Status", "bool", 0, 1, 0},
    {POWER_REG_CELL_BALANCE_1, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Cell 1 Balancing Status", "bool", 0, 1, 0},
    {POWER_REG_CELL_BALANCE_2, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Cell 2 Balancing Status", "bool", 0, 1, 0},
    {POWER_REG_CELL_BALANCE_3, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Cell 3 Balancing Status", "bool", 0, 1, 0},
    {POWER_REG_CELL_BALANCE_4, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Cell 4 Balancing Status", "bool", 0, 1, 0},
    {POWER_REG_CELL_BALANCE_5, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Cell 5 Balancing Status", "bool", 0, 1, 0},
    {POWER_REG_CELL_BALANCE_ACTIVE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Global Balancing Active", "bool", 0, 1, 0},
    {POWER_REG_FAULT_FLAGS, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Fault Status Flags", "bitmap", 0, 65535, 0},
    
    // SK60X Data Registers (0x0030-0x003F)
    {POWER_REG_V_SET, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Voltage Setpoint", "V", 0, 600, 0},
    {POWER_REG_I_SET, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Current Setpoint", "A", 0, 600, 0},
    {POWER_REG_V_OUT, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Output Voltage", "V", 0, 600, 0},
    {POWER_REG_I_OUT, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Output Current", "A", 0, 600, 0},
    {POWER_REG_P_OUT, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Output Power", "W", 0, 6000, 0},
    {POWER_REG_V_IN, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Input Voltage", "V", 0, 600, 0},
    {POWER_REG_I_IN, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Input Current", "A", 0, 600, 0},
    {POWER_REG_TEMP, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "SK60X Temperature", "C", 0, 100, 25},
    {POWER_REG_STATUS, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Operational Status", "bitmap", 0, 65535, 0},
    {POWER_REG_ON_OFF, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Output ON/OFF", "bool", 0, 1, 0},
    {POWER_REG_CHARGE_RELAY, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Charge Relay Control", "bool", 0, 1, 0},
    {POWER_REG_CHARGE_STATE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Charge State", "enum", 0, 2, 0},
    {POWER_REG_CHARGE_REQUEST, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, false, "Charge Request", "bool", 0, 1, 0},
    
    // INA219 Sensor Values (0x0040-0x0048)
    {POWER_REG_V_OUT_12V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "12V Output Voltage", "V", 0, 150, 120},
    {POWER_REG_I_OUT_12V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "12V Output Current", "A", 0, 100, 0},
    {POWER_REG_P_OUT_12V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "12V Output Power", "W", 0, 1000, 0},
    {POWER_REG_V_OUT_5V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "5V Output Voltage", "V", 0, 60, 50},
    {POWER_REG_I_OUT_5V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "5V Output Current", "A", 0, 100, 0},
    {POWER_REG_P_OUT_5V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "5V Output Power", "W", 0, 500, 0},
    {POWER_REG_V_OUT_3V3, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "3.3V Output Voltage", "V", 0, 40, 33},
    {POWER_REG_I_OUT_3V3, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "3.3V Output Current", "A", 0, 100, 0},
    {POWER_REG_P_OUT_3V3, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "3.3V Output Power", "W", 0, 100, 0},
    
    // Relay State (0x0049-0x004D)
    {POWER_REG_RL_12V, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "12V Relay State", "bool", 0, 1, 0},
    {POWER_REG_RL_5V, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "5V Relay State", "bool", 0, 1, 0},
    {POWER_REG_RL_3V3, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "3.3V Relay State", "bool", 0, 1, 0},
    
    // System Registers (0x0100-0x0109)
    {POWER_REG_DEVICE_ID, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_SYSTEM, false, "Device ID", "addr", 1, 247, 2},
    {POWER_REG_MODULE_TYPE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_SYSTEM, false, "Module Type", "enum", 0, 65535, 0x0002},
    {POWER_REG_FIRMWARE_VERSION, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Firmware Version", "version", 0, 65535, 0x0100},
    {POWER_REG_HARDWARE_VERSION, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Hardware Version", "version", 0, 65535, 0x0100},
    {POWER_REG_SYSTEM_STATUS, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "System Status", "bitmap", 0, 65535, 0},
    {POWER_REG_SYSTEM_ERROR, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "System Error Code", "error", 0, 65535, 0},
    {POWER_REG_RESET_ERROR_CMD, REG_MODE_WRITE_ONCE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, true, "Reset Error Command", "cmd", 0, 1, 0},
    
    // End marker
    {0xFFFF, 0, 0, 0, false, NULL, NULL, 0, 0, 0}
};
static const size_t power_module_register_map_size = sizeof(power_module_register_map) / sizeof(power_module_register_map[0]);

// ============================================================================
// SAFETY MODULE REGISTER MAP (COMPLETE - 30 registers)
// ============================================================================
static const register_info_t safety_module_register_map[] = {
    // System Status Registers (0x0000-0x0007)
    {SAFETY_STATUS_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Safety System Status", "bitmap", 0, 65535, 0},
    {SAFETY_EMERGENCY_STOP_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, true, "Emergency Stop Status", "bool", 0, 1, 0},
    {SAFETY_SAFETY_ZONE_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, true, "Safety Zone Active", "enum", 0, 4, 0},
    {SAFETY_PROXIMITY_ALERT_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, true, "Proximity Alert", "bool", 0, 1, 0},
    {SAFETY_RELAY_OUTPUT_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Safety Relay Output", "bitmap", 0, 15, 0},
    {SAFETY_SAFETY_ERROR_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Safety Error Code", "error", 0, 65535, 0},
    {SAFETY_SYSTEM_TEMP_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "System Temperature", "C", 0, 100, 25},
    {SAFETY_SYSTEM_VOLTAGE_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "System Voltage", "V", 0, 300, 24},
    
    // Analog Input Registers (0x0010-0x0017)
    {SAFETY_ANALOG_INPUT_1_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Analog Sensor 1 Distance", "mm", 0, 10000, 0},
    {SAFETY_ANALOG_INPUT_2_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Analog Sensor 2 Distance", "mm", 0, 10000, 0},
    {SAFETY_ANALOG_INPUT_3_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Analog Sensor 3 Distance", "mm", 0, 10000, 0},
    {SAFETY_ANALOG_INPUT_4_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Analog Sensor 4 Distance", "mm", 0, 10000, 0},
    {SAFETY_ANALOG_RAW_1_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Analog Raw ADC 1", "counts", 0, 4095, 0},
    {SAFETY_ANALOG_RAW_2_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Analog Raw ADC 2", "counts", 0, 4095, 0},
    {SAFETY_ANALOG_RAW_3_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Analog Raw ADC 3", "counts", 0, 4095, 0},
    {SAFETY_ANALOG_RAW_4_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Analog Raw ADC 4", "counts", 0, 4095, 0},
    
    // Digital Input Registers (0x0020-0x0024)
    {SAFETY_DIGITAL_INPUT_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Digital Input Status", "bitmap", 0, 15, 0},
    {SAFETY_DI1_STATUS_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Digital Input 1", "bool", 0, 1, 0},
    {SAFETY_DI2_STATUS_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Digital Input 2", "bool", 0, 1, 0},
    {SAFETY_DI3_STATUS_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Digital Input 3", "bool", 0, 1, 0},
    {SAFETY_DI4_STATUS_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Digital Input 4", "bool", 0, 1, 0},
    
    // Relay Control Registers (0x0030-0x0034)
    {SAFETY_RELAY_CONTROL_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Relay Control Bitmap", "bitmap", 0, 15, 0},
    {SAFETY_RELAY_1_CONTROL_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Relay 1 Control", "bool", 0, 1, 0},
    {SAFETY_RELAY_2_CONTROL_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Relay 2 Control", "bool", 0, 1, 0},
    {SAFETY_RELAY_3_CONTROL_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Relay 3 Control", "bool", 0, 1, 0},
    {SAFETY_RELAY_4_CONTROL_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Relay 4 Control", "bool", 0, 1, 0},
    
    // Zone Threshold Registers (0x0040-0x0043)
    {SAFETY_ZONE_1_THRESHOLD_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, true, "Zone 1 Threshold", "mm", 0, 10000, 1000},
    {SAFETY_ZONE_2_THRESHOLD_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, true, "Zone 2 Threshold", "mm", 0, 10000, 2000},
    {SAFETY_ZONE_3_THRESHOLD_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, true, "Zone 3 Threshold", "mm", 0, 10000, 3000},
    {SAFETY_ZONE_4_THRESHOLD_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, true, "Zone 4 Threshold", "mm", 0, 10000, 5000},
    
    // End marker
    {0xFFFF, 0, 0, 0, false, NULL, NULL, 0, 0, 0}
};
static const size_t safety_module_register_map_size = sizeof(safety_module_register_map) / sizeof(safety_module_register_map[0]);

// ============================================================================
// TRAVEL MOTOR MODULE REGISTER MAP (COMPLETE - 25 registers)
// ============================================================================
static const register_info_t travel_motor_module_register_map[] = {
    // Motor 1 Control Registers (0x0000-0x000C)
    {DC_MOTOR_REG_MOTOR1_CONTROL_MODE, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, false, "Motor 1 Control Mode", "enum", 1, 3, 1},
    {DC_MOTOR_REG_MOTOR1_ENABLE, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Motor 1 Enable", "bool", 0, 1, 0},
    {DC_MOTOR_REG_MOTOR1_SPEED, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 1 Speed Setpoint", "0-255", 0, 255, 0},
    {DC_MOTOR_REG_MOTOR1_LINEAR_INPUT, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 1 Linear Input", "%", 0, 100, 0},
    {DC_MOTOR_REG_MOTOR1_LINEAR_UNIT, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, false, "Motor 1 Linear Unit", "%", 0, 20, 5},
    {DC_MOTOR_REG_MOTOR1_LINEAR_STATE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 1 Linear State", "enum", 0, 1, 0},
    {DC_MOTOR_REG_MOTOR1_ACTUAL_SPEED, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 1 Actual Speed", "0-255", 0, 255, 0},
    {DC_MOTOR_REG_MOTOR1_DIRECTION, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 1 Direction", "enum", 0, 2, 0},
    {DC_MOTOR_REG_MOTOR1_PID_KP, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Motor 1 PID Kp", "x100", 0, 10000, 100},
    {DC_MOTOR_REG_MOTOR1_PID_KI, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Motor 1 PID Ki", "x100", 0, 10000, 10},
    {DC_MOTOR_REG_MOTOR1_PID_KD, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Motor 1 PID Kd", "x100", 0, 10000, 10},
    {DC_MOTOR_REG_MOTOR1_STATUS_WORD, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Motor 1 Status", "bitmap", 0, 65535, 0},
    {DC_MOTOR_REG_MOTOR1_ERROR_CODE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Motor 1 Error Code", "error", 0, 65535, 0},
    
    // Motor 2 Control Registers (0x0010-0x001C)  
    {DC_MOTOR_REG_MOTOR2_CONTROL_MODE, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, false, "Motor 2 Control Mode", "enum", 1, 3, 1},
    {DC_MOTOR_REG_MOTOR2_ENABLE, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, true, "Motor 2 Enable", "bool", 0, 1, 0},
    {DC_MOTOR_REG_MOTOR2_SPEED, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 2 Speed Setpoint", "0-255", 0, 255, 0},
    {DC_MOTOR_REG_MOTOR2_LINEAR_INPUT, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 2 Linear Input", "%", 0, 100, 0},
    {DC_MOTOR_REG_MOTOR2_LINEAR_UNIT, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, false, "Motor 2 Linear Unit", "%", 0, 20, 5},
    {DC_MOTOR_REG_MOTOR2_LINEAR_STATE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 2 Linear State", "enum", 0, 1, 0},
    {DC_MOTOR_REG_MOTOR2_ACTUAL_SPEED, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 2 Actual Speed", "0-255", 0, 255, 0},
    {DC_MOTOR_REG_MOTOR2_DIRECTION, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Motor 2 Direction", "enum", 0, 2, 0},
    {DC_MOTOR_REG_MOTOR2_PID_KP, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Motor 2 PID Kp", "x100", 0, 10000, 100},
    {DC_MOTOR_REG_MOTOR2_PID_KI, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Motor 2 PID Ki", "x100", 0, 10000, 10},
    {DC_MOTOR_REG_MOTOR2_PID_KD, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Motor 2 PID Kd", "x100", 0, 10000, 10},
    {DC_MOTOR_REG_MOTOR2_STATUS_WORD, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Motor 2 Status", "bitmap", 0, 65535, 0},
    
    // End marker
    {0xFFFF, 0, 0, 0, false, NULL, NULL, 0, 0, 0}
};
static const size_t travel_motor_module_register_map_size = sizeof(travel_motor_module_register_map) / sizeof(travel_motor_module_register_map[0]);

// ============================================================================
// DOCK MODULE REGISTER MAP (COMPLETE - 40 registers)
// ============================================================================
static const register_info_t dock_module_register_map[] = {
    // IMU Data Registers (0x0000-0x0010)
    {DOCK_REG_ACCEL_X, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Accelerometer X", "mg", -32768, 32767, 0},
    {DOCK_REG_ACCEL_Y, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Accelerometer Y", "mg", -32768, 32767, 0},
    {DOCK_REG_ACCEL_Z, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Accelerometer Z", "mg", -32768, 32767, 1000},
    {DOCK_REG_GYRO_X, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Gyroscope X", "mdps", -32768, 32767, 0},
    {DOCK_REG_GYRO_Y, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Gyroscope Y", "mdps", -32768, 32767, 0},
    {DOCK_REG_GYRO_Z, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Gyroscope Z", "mdps", -32768, 32767, 0},
    {DOCK_REG_MAG_X, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Magnetometer X", "mgauss", -32768, 32767, 0},
    {DOCK_REG_MAG_Y, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Magnetometer Y", "mgauss", -32768, 32767, 0},
    {DOCK_REG_MAG_Z, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Magnetometer Z", "mgauss", -32768, 32767, 0},
    {DOCK_REG_TEMPERATURE, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "IMU Temperature", "C", -40, 85, 25},
    {DOCK_REG_QUATERNION_W, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Quaternion W", "norm", -32768, 32767, 0},
    {DOCK_REG_QUATERNION_X, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Quaternion X", "norm", -32768, 32767, 0},
    {DOCK_REG_QUATERNION_Y, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Quaternion Y", "norm", -32768, 32767, 0},
    {DOCK_REG_QUATERNION_Z, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Quaternion Z", "norm", -32768, 32767, 0},
    {DOCK_REG_EULER_YAW, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Euler Yaw", "deg", -180, 180, 0},
    {DOCK_REG_EULER_PITCH, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Euler Pitch", "deg", -90, 90, 0},
    {DOCK_REG_EULER_ROLL, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Euler Roll", "deg", -180, 180, 0},
    
    // Magnetic Sensor Registers (0x0020-0x0026)
    {DOCK_REG_MAGNETIC_FIELD_X, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Magnetic Field X", "uT", -32768, 32767, 0},
    {DOCK_REG_MAGNETIC_FIELD_Y, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Magnetic Field Y", "uT", -32768, 32767, 0},
    {DOCK_REG_MAGNETIC_FIELD_Z, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Magnetic Field Z", "uT", -32768, 32767, 0},
    {DOCK_REG_MAGNETIC_STRENGTH, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Magnetic Field Strength", "uT", 0, 10000, 0},
    {DOCK_REG_MAGNETIC_HEADING, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Magnetic Heading", "deg", 0, 360, 0},
    {DOCK_REG_MAGNETIC_DECLINATION, REG_MODE_READ_WRITE, REG_DATA_TYPE_INT16, REG_ACCESS_ADMIN, false, "Magnetic Declination", "deg", -180, 180, 0},
    {DOCK_REG_MAGNETIC_INCLINATION, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Magnetic Inclination", "deg", -90, 90, 0},
    
    // Navigation Data Registers (0x0030-0x003B)
    {DOCK_REG_POSITION_X, REG_MODE_READ_WRITE, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Position X", "mm", -32768, 32767, 0},
    {DOCK_REG_POSITION_Y, REG_MODE_READ_WRITE, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Position Y", "mm", -32768, 32767, 0},
    {DOCK_REG_POSITION_Z, REG_MODE_READ_WRITE, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Position Z", "mm", -32768, 32767, 0},
    {DOCK_REG_VELOCITY_X, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Velocity X", "mm/s", -10000, 10000, 0},
    {DOCK_REG_VELOCITY_Y, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Velocity Y", "mm/s", -10000, 10000, 0},
    {DOCK_REG_VELOCITY_Z, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Velocity Z", "mm/s", -10000, 10000, 0},
    {DOCK_REG_ACCELERATION_X, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Acceleration X", "mm/s^2", -10000, 10000, 0},
    {DOCK_REG_ACCELERATION_Y, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Acceleration Y", "mm/s^2", -10000, 10000, 0},
    {DOCK_REG_ACCELERATION_Z, REG_MODE_READ_ONLY, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Acceleration Z", "mm/s^2", -10000, 10000, 0},
    {DOCK_REG_ORIENTATION_YAW, REG_MODE_READ_WRITE, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Orientation Yaw", "deg", -180, 180, 0},
    {DOCK_REG_ORIENTATION_PITCH, REG_MODE_READ_WRITE, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Orientation Pitch", "deg", -90, 90, 0},
    {DOCK_REG_ORIENTATION_ROLL, REG_MODE_READ_WRITE, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Orientation Roll", "deg", -180, 180, 0},
    
    // Docking Control Registers (0x0040-0x004F)
    {DOCK_REG_DOCKING_STATUS, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Docking Status", "enum", 0, 5, 0},
    {DOCK_REG_DOCKING_MODE, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, false, "Docking Mode", "enum", 0, 3, 0},
    {DOCK_REG_DOCKING_DISTANCE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Distance to Dock", "mm", 0, 10000, 0},
    {DOCK_REG_DOCKING_ALIGNMENT, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Alignment Status", "bool", 0, 1, 0},
    
    // End marker
    {0xFFFF, 0, 0, 0, false, NULL, NULL, 0, 0, 0}
};
static const size_t dock_module_register_map_size = sizeof(dock_module_register_map) / sizeof(dock_module_register_map[0]);

// ============================================================================
// REGISTER INFO FUNCTIONS IMPLEMENTATION
// ============================================================================

const register_info_t* get_register_info(uint8_t module_addr, uint16_t register_addr) {
    const register_info_t* map = NULL;
    size_t map_size = 0;

    switch (module_addr) {
        case MODULE_ADDR_POWER:
            map = power_module_register_map;
            map_size = power_module_register_map_size;
            break;
        case MODULE_ADDR_SAFETY:
            map = safety_module_register_map;
            map_size = safety_module_register_map_size;
            break;
        case MODULE_ADDR_TRAVEL_MOTOR:
            map = travel_motor_module_register_map;
            map_size = travel_motor_module_register_map_size;
            break;
        case MODULE_ADDR_DOCK:
            map = dock_module_register_map;
            map_size = dock_module_register_map_size;
            break;
        default:
            return NULL;
    }

    // Iterate until end marker (0xFFFF). Do NOT treat 0xFFFF as a valid register.
    for (size_t i = 0; i < map_size; i++) {
        if (map[i].address == 0xFFFF) {
            break; // reached end of the map
        }
        if (map[i].address == register_addr) {
            return &map[i];
        }
    }
    return NULL;
}

bool validate_register_access(uint8_t module_addr, uint16_t register_addr, uint8_t access_mode, uint8_t user_access_level) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    if (info == NULL) {
        printf("[REG_INFO] Access validation failed: Register 0x%04X not found for module 0x%02X\n", register_addr, module_addr);
        return false; // Register not found
    }

    // Check if user has sufficient access level
    if (user_access_level < info->access_level) {
        printf("[REG_INFO] Access denied: Insufficient access level for register 0x%04X (required %d, user %d)\n", register_addr, info->access_level, user_access_level);
        return false;
    }

    // Check if the requested access mode is supported by the register
    if (access_mode == REG_MODE_READ_ONLY && !(info->mode & (REG_MODE_READ_ONLY | REG_MODE_READ_WRITE))) {
        printf("[REG_INFO] Access denied: Read not supported for register 0x%04X (mode %d)\n", register_addr, info->mode);
        return false;
    }
    if (access_mode == REG_MODE_WRITE_ONLY && !(info->mode & (REG_MODE_WRITE_ONLY | REG_MODE_READ_WRITE | REG_MODE_WRITE_ONCE))) {
        printf("[REG_INFO] Access denied: Write not supported for register 0x%04X (mode %d)\n", register_addr, info->mode);
        return false;
    }
    // If access_mode is READ_WRITE, it implies both read and write are requested.
    // The check above for READ_ONLY and WRITE_ONLY covers this implicitly.

    return true;
}

bool is_register_safe_critical(uint8_t module_addr, uint16_t register_addr) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    return (info != NULL && info->is_safe_register);
}

const char* get_register_description(uint8_t module_addr, uint16_t register_addr) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    return (info != NULL) ? info->description : NULL;
}

const char* get_register_unit(uint8_t module_addr, uint16_t register_addr) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    return (info != NULL) ? info->unit : NULL;
}

uint16_t get_register_min_value(uint8_t module_addr, uint16_t register_addr) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    return (info != NULL) ? info->min_value : 0; // Default to 0 if not found
}

uint16_t get_register_max_value(uint8_t module_addr, uint16_t register_addr) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    return (info != NULL) ? info->max_value : 0xFFFF; // Default to max uint16 if not found
}

uint16_t get_register_default_value(uint8_t module_addr, uint16_t register_addr) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    return (info != NULL) ? info->default_value : 0; // Default to 0 if not found
}

uint8_t get_register_data_type(uint8_t module_addr, uint16_t register_addr) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    return (info != NULL) ? info->data_type : 0; // Default to 0 if not found
}

// Validate value within min/max for a given register. Returns true when valid.
bool validate_register_value(uint8_t module_addr, uint16_t register_addr, uint16_t value) {
    const register_info_t* info = get_register_info(module_addr, register_addr);
    if (info == NULL) {
        return false;
    }
    if (value < info->min_value || value > info->max_value) {
        printf("[REG_INFO] Value out of range for 0x%04X: %u not in [%u, %u]\n",
               register_addr, (unsigned)value, (unsigned)info->min_value, (unsigned)info->max_value);
        return false;
    }
    return true;
}

// ============================================================================
// ISSUE #203 - REGISTER METADATA API SUPPORT
// ============================================================================

/**
 * @brief Count registers in a register map (stops at end marker 0xFFFF)
 */
static uint16_t count_registers_in_map(const register_info_t* map) {
    if (map == NULL) {
        return 0;
    }
    
    uint16_t count = 0;
    while (map[count].address != 0xFFFF && count < 1000) { // Safety limit
        count++;
    }
    return count;
}

const register_info_t* get_module_registers_array(uint8_t module_addr, uint16_t* count_out) {
    const register_info_t* map = NULL;
    
    switch (module_addr) {
        case MODULE_ADDR_POWER:
            map = power_module_register_map;
            break;
        case MODULE_ADDR_SAFETY:
            map = safety_module_register_map;
            break;
        case MODULE_ADDR_TRAVEL_MOTOR:
            map = travel_motor_module_register_map;
            break;
        case MODULE_ADDR_DOCK:
            map = dock_module_register_map;
            break;
        default:
            printf("[REG_INFO] Invalid module address: 0x%02X\n", module_addr);
            if (count_out) *count_out = 0;
            return NULL;
    }
    
    if (map != NULL && count_out) {
        *count_out = count_registers_in_map(map);
    }
    
    return map;
}

const register_list_response_t* get_module_register_list_api(uint8_t module_addr) {
    // Static response structure (one per module type)
    static register_list_response_t power_response = {0};
    static register_list_response_t safety_response = {0};
    static register_list_response_t motor_response = {0};
    static register_list_response_t dock_response = {0};
    
    register_list_response_t* response = NULL;
    
    // Select response structure based on module
    switch (module_addr) {
        case MODULE_ADDR_POWER:
            response = &power_response;
            break;
        case MODULE_ADDR_SAFETY:
            response = &safety_response;
            break;
        case MODULE_ADDR_TRAVEL_MOTOR:
            response = &motor_response;
            break;
        case MODULE_ADDR_DOCK:
            response = &dock_response;
            break;
        default:
            printf("[REG_INFO] Invalid module address for API: 0x%02X\n", module_addr);
            return NULL;
    }
    
    // Initialize response if not already done
    if (!response->valid) {
        response->module_addr = module_addr;
        response->module_name = get_module_name_from_address(module_addr);
        response->registers = get_module_registers_array(module_addr, &response->count);
        response->valid = (response->registers != NULL && response->count > 0);
    }
    
    return response->valid ? response : NULL;
}