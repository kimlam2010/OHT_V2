/**
 * @file register_map.h
 * @brief Unified Register Map for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-Phase3-1 (Register Address Constants)
 */

#ifndef REGISTER_MAP_H
#define REGISTER_MAP_H

#include <stdint.h>

// ============================================================================
// MODULE ADDRESSES
// ============================================================================

#define MODULE_ADDR_POWER                  0x02U
#define MODULE_ADDR_SAFETY                 0x03U
#define MODULE_ADDR_TRAVEL_MOTOR           0x04U
#define MODULE_ADDR_DOCK                   0x05U
#define MODULE_ADDR_MAX                    0x08U

// Type validation for module addresses
_Static_assert(MODULE_ADDR_POWER <= 0xFF, "Module address out of range");
_Static_assert(MODULE_ADDR_SAFETY <= 0xFF, "Module address out of range");
_Static_assert(MODULE_ADDR_TRAVEL_MOTOR <= 0xFF, "Module address out of range");
_Static_assert(MODULE_ADDR_DOCK <= 0xFF, "Module address out of range");
_Static_assert(MODULE_ADDR_MAX <= 0xFF, "Module address out of range");

// ============================================================================
// POWER MODULE REGISTERS (0x0000-0x00FF)
// ============================================================================

// DalyBMS Status Registers (0x0000-0x002B)
#define POWER_REG_BATTERY_VOLTAGE          0x0000  // Battery pack voltage (V, /10.0 scaling)
#define POWER_REG_BATTERY_CURRENT          0x0001  // Battery pack current (A, /10.0 scaling)
#define POWER_REG_BATTERY_SOC              0x0002  // State of Charge (%, /10.0 scaling)
#define POWER_REG_MAX_CELL_V               0x0003  // Maximum cell voltage (mV)
#define POWER_REG_MIN_CELL_V               0x0004  // Minimum cell voltage (mV)
#define POWER_REG_CELL_DIFF                0x0007  // Voltage difference between cells (mV)
#define POWER_REG_TEMPERATURE              0x0008  // Average temperature (°C)
#define POWER_REG_CONNECTION_STATUS        0x0009  // BMS Connection Status
#define POWER_REG_CHARGE_MOS               0x000B  // Charge MOSFET status
#define POWER_REG_DISCHARGE_MOS            0x000C  // Discharge MOSFET status
#define POWER_REG_CELL_VOLTAGE_0           0x0014  // Cell 0 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_1           0x0015  // Cell 1 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_2           0x0016  // Cell 2 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_3           0x0017  // Cell 3 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_4           0x0018  // Cell 4 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_5           0x0019  // Cell 5 voltage (mV)
#define POWER_REG_TEMPERATURE_0            0x001A  // Temperature sensor 0 (°C)
#define POWER_REG_TEMPERATURE_1            0x001B  // Temperature sensor 1 (°C)
#define POWER_REG_CELL_BALANCE_0           0x001C  // Cell 0 balancing status
#define POWER_REG_CELL_BALANCE_1           0x001D  // Cell 1 balancing status
#define POWER_REG_CELL_BALANCE_2           0x001E  // Cell 2 balancing status
#define POWER_REG_CELL_BALANCE_3           0x001F  // Cell 3 balancing status
#define POWER_REG_CELL_BALANCE_4           0x0020  // Cell 4 balancing status
#define POWER_REG_CELL_BALANCE_5           0x0021  // Cell 5 balancing status
#define POWER_REG_CELL_BALANCE_ACTIVE      0x0022  // Global balancing status
#define POWER_REG_FAULT_FLAGS              0x0023  // Fault status flags

// SK60X Data Registers (0x0030-0x003F)
#define POWER_REG_V_SET                    0x0030  // Voltage setpoint (V, /10.0 scaling)
#define POWER_REG_I_SET                    0x0031  // Current setpoint (A, /10.0 scaling)
#define POWER_REG_V_OUT                    0x0032  // Output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT                    0x0033  // Output current (A, /10.0 scaling)
#define POWER_REG_P_OUT                    0x0034  // Output power (W, /10.0 scaling)
#define POWER_REG_V_IN                     0x0035  // Input voltage (V, /10.0 scaling)
#define POWER_REG_I_IN                     0x0036  // Input current (A, /10.0 scaling)
#define POWER_REG_TEMP                     0x0037  // Temperature of SK60X (°C)
#define POWER_REG_STATUS                   0x003B  // Operational status
#define POWER_REG_ON_OFF                   0x003C  // Output ON/OFF state
#define POWER_REG_CHARGE_RELAY             0x003D  // Charge relay status
#define POWER_REG_CHARGE_STATE             0x003E  // Charge state (0=IDLE, 1=WAITING, 2=CHARGE)
#define POWER_REG_CHARGE_REQUEST           0x003F  // Charge request

// INA219 Sensor Values (0x0040-0x0048)
#define POWER_REG_V_OUT_12V                0x0040  // 12V output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT_12V                0x0041  // 12V output current (A, /10.0 scaling)
#define POWER_REG_P_OUT_12V                0x0042  // 12V output power (W, /10.0 scaling)
#define POWER_REG_V_OUT_5V                 0x0043  // 5V output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT_5V                 0x0044  // 5V output current (A, /10.0 scaling)
#define POWER_REG_P_OUT_5V                 0x0045  // 5V output power (W, /10.0 scaling)
#define POWER_REG_V_OUT_3V3                0x0046  // 3.3V output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT_3V3                0x0047  // 3.3V output current (A, /10.0 scaling)
#define POWER_REG_P_OUT_3V3                0x0048  // 3.3V output power (W, /10.0 scaling)

// Relay State (0x0049-0x004D)
#define POWER_REG_RL_12V                   0x0049  // State of 12V relay
#define POWER_REG_RL_5V                    0x004A  // State of 5V relay
#define POWER_REG_RL_3V3                   0x004B  // State of 3V3 relay
#define POWER_REG_RL_FAUL                  0x004C  // State of Faul relay
#define POWER_REG_USE_V_THS                0x004D  // Voltage usage threshold (/100.0 scaling)

// System Registers (0x0100-0x0109) - Auto Detect Support
#define POWER_REG_DEVICE_ID                0x0100  // Device ID (Modbus slave address)
#define POWER_REG_CONFIG_BAUDRATE          0x0101  // Config baudrate (1=9600, 2=19200, 3=38400,...)
#define POWER_REG_CONFIG_PARITY            0x0102  // Config parity (0=None, 1=Even, 2=Odd)
#define POWER_REG_CONFIG_STOP_BITS         0x0103  // Config stop bits (1=1, 2=2)
#define POWER_REG_MODULE_TYPE              0x0104  // Module type (0x0002 = Power Module)
#define POWER_REG_FIRMWARE_VERSION         0x0105  // Firmware version (e.g. 0x0101 = v1.01)
#define POWER_REG_HARDWARE_VERSION         0x0106  // Hardware version (e.g. 0x0101 = v1.01)
#define POWER_REG_SYSTEM_STATUS            0x0107  // System status (bit field)
#define POWER_REG_SYSTEM_ERROR             0x0108  // System error (global error code)
#define POWER_REG_RESET_ERROR_CMD          0x0109  // Reset error command (write 1 to reset all error flags)
#define POWER_REG_MODULE_NAME_LOW          0x00F8  // Module name (low word)
#define POWER_REG_MODULE_NAME_HIGH         0x00F9  // Module name (high word)
#define POWER_REG_HARDWARE_VERSION_ALT     0x00FA  // Hardware version (alternative)
#define POWER_REG_SERIAL_NUMBER_LOW        0x00FB  // Serial number (low word)
#define POWER_REG_SERIAL_NUMBER_HIGH       0x00FC  // Serial number (high word)
#define POWER_REG_BUILD_DATE_LOW           0x00FD  // Build date (low word)
#define POWER_REG_BUILD_DATE_HIGH          0x00FE  // Build date (high word)
#define POWER_REG_CHECKSUM                 0x00FF  // Register checksum

// ============================================================================
// SAFETY MODULE REGISTERS (0x0000-0x00FF)
// ============================================================================

// System Status Registers (0x0000-0x0007)
#define SAFETY_STATUS_REG                  0x0000  // System status
#define SAFETY_EMERGENCY_STOP_REG          0x0001  // Emergency stop status
#define SAFETY_SAFETY_ZONE_REG             0x0002  // Safety zone status
#define SAFETY_PROXIMITY_ALERT_REG         0x0003  // Proximity alert status
#define SAFETY_RELAY_OUTPUT_REG            0x0004  // Relay output status
#define SAFETY_SAFETY_ERROR_REG            0x0005  // Safety error status
#define SAFETY_SYSTEM_TEMP_REG             0x0006  // System temperature
#define SAFETY_SYSTEM_VOLTAGE_REG          0x0007  // System voltage

// Analog Input Registers (0x0010-0x0017)
#define SAFETY_ANALOG_INPUT_1_REG          0x0010  // Analog sensor 1 distance (mm)
#define SAFETY_ANALOG_INPUT_2_REG          0x0011  // Analog sensor 2 distance (mm)
#define SAFETY_ANALOG_INPUT_3_REG          0x0012  // Analog sensor 3 distance (mm)
#define SAFETY_ANALOG_INPUT_4_REG          0x0013  // Analog sensor 4 distance (mm)
#define SAFETY_ANALOG_RAW_1_REG            0x0014  // Raw ADC value sensor 1
#define SAFETY_ANALOG_RAW_2_REG            0x0015  // Raw ADC value sensor 2
#define SAFETY_ANALOG_RAW_3_REG            0x0016  // Raw ADC value sensor 3
#define SAFETY_ANALOG_RAW_4_REG            0x0017  // Raw ADC value sensor 4

// Digital Input Registers (0x0020-0x0024)
#define SAFETY_DIGITAL_INPUT_REG           0x0020  // Digital sensors status (bitmap)
#define SAFETY_DI1_STATUS_REG              0x0021  // Digital sensor 1 status
#define SAFETY_DI2_STATUS_REG              0x0022  // Digital sensor 2 status
#define SAFETY_DI3_STATUS_REG              0x0023  // Digital sensor 3 status
#define SAFETY_DI4_STATUS_REG              0x0024  // Digital sensor 4 status

// Relay Control Registers (0x0030-0x0034)
#define SAFETY_RELAY_CONTROL_REG           0x0030  // Relay control status (bitmap)
#define SAFETY_RELAY_1_CONTROL_REG         0x0031  // Relay 1 control
#define SAFETY_RELAY_2_CONTROL_REG         0x0032  // Relay 2 control
#define SAFETY_RELAY_3_CONTROL_REG         0x0033  // Relay 3 control
#define SAFETY_RELAY_4_CONTROL_REG         0x0034  // Relay 4 control

// Zone Threshold Registers (0x0040-0x0043)
#define SAFETY_ZONE_1_THRESHOLD_REG        0x0040  // Zone 1 threshold (mm)
#define SAFETY_ZONE_2_THRESHOLD_REG        0x0041  // Zone 2 threshold (mm)
#define SAFETY_ZONE_3_THRESHOLD_REG        0x0042  // Zone 3 threshold (mm)
#define SAFETY_ZONE_4_THRESHOLD_REG        0x0043  // Zone 4 threshold (mm)

// Configuration Registers (0x0050-0x0053)
#define SAFETY_PROXIMITY_THRESHOLD_REG     0x0050  // Proximity threshold (mm)
#define SAFETY_RESPONSE_TIME_REG           0x0051  // Response time (ms)
#define SAFETY_AUTO_RESET_ENABLE_REG       0x0052  // Auto reset enable
#define SAFETY_SAFETY_MODE_REG             0x0053  // Safety mode

// System Registers (0x0100-0x0109) - Auto Detect Support
#define SAFETY_DEVICE_ID_REG               0x0100  // Device ID (Modbus slave address)
#define SAFETY_CONFIG_BAUDRATE_REG         0x0101  // Config baudrate (1=9600, 2=19200, 3=38400,...)
#define SAFETY_CONFIG_PARITY_REG           0x0102  // Config parity (0=None, 1=Even, 2=Odd)
#define SAFETY_CONFIG_STOP_BITS_REG        0x0103  // Config stop bits (1=1, 2=2)
#define SAFETY_MODULE_TYPE_REG             0x0104  // Module type (0x0003 = Safety Module)
#define SAFETY_FIRMWARE_VERSION_REG        0x0105  // Firmware version (e.g. 0x0101 = v1.01)
#define SAFETY_HARDWARE_VERSION_REG        0x0106  // Hardware version (e.g. 0x0101 = v1.01)
#define SAFETY_SYSTEM_STATUS_REG           0x0107  // System status (bit field)
#define SAFETY_SYSTEM_ERROR_REG            0x0108  // System error (global error code)
#define SAFETY_RESET_ERROR_CMD_REG         0x0109  // Reset error command (write 1 to reset all error flags)
#define SAFETY_MODULE_NAME_LOW_REG         0x00F8  // Module name (low word)
#define SAFETY_MODULE_NAME_HIGH_REG        0x00F9  // Module name (high word)
#define SAFETY_HARDWARE_VERSION_ALT_REG    0x00FA  // Hardware version (alternative)
#define SAFETY_SERIAL_NUMBER_LOW_REG       0x00FB  // Serial number (low word)
#define SAFETY_SERIAL_NUMBER_HIGH_REG      0x00FC  // Serial number (high word)
#define SAFETY_BUILD_DATE_LOW_REG          0x00FD  // Build date (low word)
#define SAFETY_BUILD_DATE_HIGH_REG         0x00FE  // Build date (high word)
#define SAFETY_CHECKSUM_REG                0x00FF  // Register checksum

// ============================================================================
// TRAVEL MOTOR MODULE REGISTERS (0x0000-0x00FF)
// ============================================================================

// Motor 1 Control Registers (0x0000-0x000C)
#define DC_MOTOR_REG_MOTOR1_CONTROL_MODE  0x0000  // Control mode (1=ONOFF, 2=LINEAR, 3=PID)
#define DC_MOTOR_REG_MOTOR1_ENABLE        0x0001  // Enable/disable motor 1
#define DC_MOTOR_REG_MOTOR1_SPEED         0x0002  // Speed setpoint (0-255)
#define DC_MOTOR_REG_MOTOR1_LINEAR_INPUT  0x0003  // Linear input (0-100%)
#define DC_MOTOR_REG_MOTOR1_LINEAR_UNIT   0x0004  // Linear unit (0-20%)
#define DC_MOTOR_REG_MOTOR1_LINEAR_STATE  0x0005  // Linear state (0=DECEL, 1=ACCEL)
#define DC_MOTOR_REG_MOTOR1_ACTUAL_SPEED  0x0006  // Actual speed
#define DC_MOTOR_REG_MOTOR1_DIRECTION     0x0007  // Direction (0=Idle, 1=Forward, 2=Reverse)
#define DC_MOTOR_REG_MOTOR1_PID_KP        0x0008  // PID Kp gain (×100)
#define DC_MOTOR_REG_MOTOR1_PID_KI        0x0009  // PID Ki gain (×100)
#define DC_MOTOR_REG_MOTOR1_PID_KD        0x000A  // PID Kd gain (×100)
#define DC_MOTOR_REG_MOTOR1_STATUS_WORD   0x000B  // Status word (bit field)
#define DC_MOTOR_REG_MOTOR1_ERROR_CODE    0x000C  // Error code

// Motor 2 Control Registers (0x0010-0x001C)
#define DC_MOTOR_REG_MOTOR2_CONTROL_MODE  0x0010  // Control mode (1=ONOFF, 2=LINEAR, 3=PID)
#define DC_MOTOR_REG_MOTOR2_ENABLE        0x0011  // Enable/disable motor 2
#define DC_MOTOR_REG_MOTOR2_SPEED         0x0012  // Speed setpoint (0-255)
#define DC_MOTOR_REG_MOTOR2_LINEAR_INPUT  0x0013  // Linear input (0-100%)
#define DC_MOTOR_REG_MOTOR2_LINEAR_UNIT   0x0014  // Linear unit (0-20%)
#define DC_MOTOR_REG_MOTOR2_LINEAR_STATE  0x0015  // Linear state (0=DECEL, 1=ACCEL)
#define DC_MOTOR_REG_MOTOR2_ACTUAL_SPEED  0x0016  // Actual speed
#define DC_MOTOR_REG_MOTOR2_DIRECTION     0x0017  // Direction (0=Idle, 1=Forward, 2=Reverse)
#define DC_MOTOR_REG_MOTOR2_PID_KP        0x0018  // PID Kp gain (×100)
#define DC_MOTOR_REG_MOTOR2_PID_KI        0x0019  // PID Ki gain (×100)
#define DC_MOTOR_REG_MOTOR2_PID_KD        0x001A  // PID Kd gain (×100)
#define DC_MOTOR_REG_MOTOR2_STATUS_WORD   0x001B  // Status word (bit field)
#define DC_MOTOR_REG_MOTOR2_ERROR_CODE    0x001C  // Error code

// Digital Input Status Registers (0x0020-0x0024)
#define DC_MOTOR_REG_DI_STATUS_WORD       0x0020  // Digital input status word (4 inputs)
#define DC_MOTOR_REG_DI1_ASSIGNMENT       0x0021  // DI1 assignment
#define DC_MOTOR_REG_DI2_ASSIGNMENT       0x0022  // DI2 assignment
#define DC_MOTOR_REG_DI3_ASSIGNMENT       0x0023  // DI3 assignment
#define DC_MOTOR_REG_DI4_ASSIGNMENT       0x0024  // DI4 assignment

// Digital Output Control Registers (0x0030-0x0034)
#define DC_MOTOR_REG_DO_STATUS_WORD       0x0030  // Digital output status word (2 outputs)
#define DC_MOTOR_REG_DO1_CONTROL          0x0031  // DO1 control (0=off, 1=on)
#define DC_MOTOR_REG_DO1_ASSIGNMENT       0x0032  // DO1 assignment
#define DC_MOTOR_REG_DO2_CONTROL          0x0033  // DO2 control (0=off, 1=on)
#define DC_MOTOR_REG_DO2_ASSIGNMENT       0x0034  // DO2 assignment

// System Registers (0x0100-0x0106)
// Auto Detect Support Registers (0x0100-0x0109) - Unified
#define DC_MOTOR_REG_DEVICE_ID            0x0100  // Device ID (Modbus slave address)
#define DC_MOTOR_REG_CONFIG_BAUDRATE      0x0101  // Config baudrate (1=9600, 2=19200, 3=38400,...)
#define DC_MOTOR_REG_CONFIG_PARITY        0x0102  // Config parity (0=None, 1=Even, 2=Odd)
#define DC_MOTOR_REG_CONFIG_STOP_BITS     0x0103  // Config stop bits (1=1, 2=2)
#define DC_MOTOR_REG_MODULE_TYPE          0x0104  // Module type (0x0004 = Travel Motor)
#define DC_MOTOR_REG_FIRMWARE_VERSION     0x0105  // Firmware version (e.g. 0x0101 = v1.01)
#define DC_MOTOR_REG_HARDWARE_VERSION     0x0106  // Hardware version (e.g. 0x0101 = v1.01)
#define DC_MOTOR_REG_SYSTEM_STATUS        0x0107  // System status (bit field)
#define DC_MOTOR_REG_SYSTEM_ERROR         0x0108  // System error (global error code)
#define DC_MOTOR_REG_RESET_ERROR_CMD      0x0109  // Reset error command (write 1 to reset all error flags)
#define DC_MOTOR_REG_MODULE_NAME_LOW      0x00F8  // Module name (low word)
#define DC_MOTOR_REG_MODULE_NAME_HIGH     0x00F9  // Module name (high word)
#define DC_MOTOR_REG_HARDWARE_VERSION_ALT 0x00FA  // Hardware version (alternative)
#define DC_MOTOR_REG_SERIAL_NUMBER_LOW    0x00FB  // Serial number (low word)
#define DC_MOTOR_REG_SERIAL_NUMBER_HIGH   0x00FC  // Serial number (high word)
#define DC_MOTOR_REG_BUILD_DATE_LOW       0x00FD  // Build date (low word)
#define DC_MOTOR_REG_BUILD_DATE_HIGH      0x00FE  // Build date (high word)
#define DC_MOTOR_REG_CHECKSUM             0x00FF  // Register checksum

// ============================================================================
// DOCK MODULE REGISTERS (0x0000-0x00FF)
// ============================================================================

// IMU Data Registers (0x0000-0x000F)
#define DOCK_REG_ACCEL_X                  0x0000  // Accelerometer X-axis (mg)
#define DOCK_REG_ACCEL_Y                  0x0001  // Accelerometer Y-axis (mg)
#define DOCK_REG_ACCEL_Z                  0x0002  // Accelerometer Z-axis (mg)
#define DOCK_REG_GYRO_X                   0x0003  // Gyroscope X-axis (mdps)
#define DOCK_REG_GYRO_Y                   0x0004  // Gyroscope Y-axis (mdps)
#define DOCK_REG_GYRO_Z                   0x0005  // Gyroscope Z-axis (mdps)
#define DOCK_REG_MAG_X                    0x0006  // Magnetometer X-axis (mgauss)
#define DOCK_REG_MAG_Y                    0x0007  // Magnetometer Y-axis (mgauss)
#define DOCK_REG_MAG_Z                    0x0008  // Magnetometer Z-axis (mgauss)
#define DOCK_REG_TEMPERATURE              0x0009  // IMU temperature (°C)
#define DOCK_REG_QUATERNION_W             0x000A  // Quaternion W component
#define DOCK_REG_QUATERNION_X             0x000B  // Quaternion X component
#define DOCK_REG_QUATERNION_Y             0x000C  // Quaternion Y component
#define DOCK_REG_QUATERNION_Z             0x000D  // Quaternion Z component
#define DOCK_REG_EULER_YAW                0x000E  // Euler Yaw angle (degrees)
#define DOCK_REG_EULER_PITCH              0x000F  // Euler Pitch angle (degrees)
#define DOCK_REG_EULER_ROLL               0x0010  // Euler Roll angle (degrees)

// Magnetic Sensor Registers (0x0020-0x002F)
#define DOCK_REG_MAGNETIC_FIELD_X         0x0020  // Magnetic field X (uT)
#define DOCK_REG_MAGNETIC_FIELD_Y         0x0021  // Magnetic field Y (uT)
#define DOCK_REG_MAGNETIC_FIELD_Z         0x0022  // Magnetic field Z (uT)
#define DOCK_REG_MAGNETIC_STRENGTH        0x0023  // Magnetic field strength (uT)
#define DOCK_REG_MAGNETIC_HEADING         0x0024  // Magnetic heading (degrees)
#define DOCK_REG_MAGNETIC_DECLINATION     0x0025  // Magnetic declination (degrees)
#define DOCK_REG_MAGNETIC_INCLINATION     0x0026  // Magnetic inclination (degrees)

// Navigation Data Registers (0x0030-0x003F)
#define DOCK_REG_POSITION_X               0x0030  // Position X (mm)
#define DOCK_REG_POSITION_Y               0x0031  // Position Y (mm)
#define DOCK_REG_POSITION_Z               0x0032  // Position Z (mm)
#define DOCK_REG_VELOCITY_X               0x0033  // Velocity X (mm/s)
#define DOCK_REG_VELOCITY_Y               0x0034  // Velocity Y (mm/s)
#define DOCK_REG_VELOCITY_Z               0x0035  // Velocity Z (mm/s)
#define DOCK_REG_ACCELERATION_X           0x0036  // Acceleration X (mm/s²)
#define DOCK_REG_ACCELERATION_Y           0x0037  // Acceleration Y (mm/s²)
#define DOCK_REG_ACCELERATION_Z           0x0038  // Acceleration Z (mm/s²)
#define DOCK_REG_ORIENTATION_YAW          0x0039  // Orientation Yaw (degrees)
#define DOCK_REG_ORIENTATION_PITCH        0x003A  // Orientation Pitch (degrees)
#define DOCK_REG_ORIENTATION_ROLL         0x003B  // Orientation Roll (degrees)

// Docking Control Registers (0x0040-0x004F)
#define DOCK_REG_DOCKING_STATUS           0x0040  // Docking status
#define DOCK_REG_DOCKING_MODE             0x0041  // Docking mode
#define DOCK_REG_DOCKING_TARGET_X         0x0042  // Target position X (mm)
#define DOCK_REG_DOCKING_TARGET_Y         0x0043  // Target position Y (mm)
#define DOCK_REG_DOCKING_TARGET_Z         0x0044  // Target position Z (mm)
#define DOCK_REG_DOCKING_TARGET_YAW       0x0045  // Target orientation Yaw (degrees)
#define DOCK_REG_DOCKING_ERROR_X          0x0046  // Position error X (mm)
#define DOCK_REG_DOCKING_ERROR_Y          0x0047  // Position error Y (mm)
#define DOCK_REG_DOCKING_ERROR_Z          0x0048  // Position error Z (mm)
#define DOCK_REG_DOCKING_ERROR_YAW        0x0049  // Orientation error Yaw (degrees)
#define DOCK_REG_DOCKING_DISTANCE         0x004A  // Distance to dock (mm)
#define DOCK_REG_DOCKING_ALIGNMENT        0x004B  // Alignment status
#define DOCK_REG_DOCKING_SPEED            0x004C  // Docking speed (mm/s)
#define DOCK_REG_DOCKING_TIMEOUT          0x004D  // Docking timeout (ms)
#define DOCK_REG_DOCKING_RETRY_COUNT      0x004E  // Docking retry count
#define DOCK_REG_DOCKING_SUCCESS_COUNT    0x004F  // Successful docking count

// System Registers (0x0100-0x0109) - Auto Detect Support
#define DOCK_REG_DEVICE_ID                0x0100  // Device ID (Modbus slave address)
#define DOCK_REG_CONFIG_BAUDRATE          0x0101  // Config baudrate (1=9600, 2=19200, 3=38400,...)
#define DOCK_REG_CONFIG_PARITY            0x0102  // Config parity (0=None, 1=Even, 2=Odd)
#define DOCK_REG_CONFIG_STOP_BITS         0x0103  // Config stop bits (1=1, 2=2)
#define DOCK_REG_MODULE_TYPE              0x0104  // Module type (0x0005 = Dock Module)
#define DOCK_REG_FIRMWARE_VERSION         0x0105  // Firmware version (e.g. 0x0101 = v1.01)
#define DOCK_REG_HARDWARE_VERSION         0x0106  // Hardware version (e.g. 0x0101 = v1.01)
#define DOCK_REG_SYSTEM_STATUS            0x0107  // System status (bit field)
#define DOCK_REG_SYSTEM_ERROR             0x0108  // System error (global error code)
#define DOCK_REG_RESET_ERROR_CMD          0x0109  // Reset error command (write 1 to reset all error flags)
#define DOCK_REG_MODULE_NAME_LOW          0x00F8  // Module name (low word)
#define DOCK_REG_MODULE_NAME_HIGH         0x00F9  // Module name (high word)
#define DOCK_REG_HARDWARE_VERSION_ALT     0x00FA  // Hardware version (alternative)
#define DOCK_REG_SERIAL_NUMBER_LOW        0x00FB  // Serial number (low word)
#define DOCK_REG_SERIAL_NUMBER_HIGH       0x00FC  // Serial number (high word)
#define DOCK_REG_BUILD_DATE_LOW           0x00FD  // Build date (low word)
#define DOCK_REG_BUILD_DATE_HIGH          0x00FE  // Build date (high word)
#define DOCK_REG_CHECKSUM                 0x00FF  // Register checksum

// ============================================================================
// MODULE TYPE CONSTANTS (Use module_type_t enum from module_manager.h)
// ============================================================================

// Note: Module type constants are defined in module_manager.h as enum module_type_t
// MODULE_TYPE_POWER = 0x02
// MODULE_TYPE_SAFETY = 0x03  
// MODULE_TYPE_TRAVEL_MOTOR = 0x04
// MODULE_TYPE_DOCK = 0x05

// ============================================================================
// REGISTER ACCESS TYPES
// ============================================================================

// Register access types
#define REG_ACCESS_READ_ONLY              0x01    // Read only
#define REG_ACCESS_WRITE_ONLY             0x02    // Write only
#define REG_ACCESS_READ_WRITE             0x03    // Read/Write

// Register data types
#define REG_TYPE_UINT8                    0x01    // 8-bit unsigned
#define REG_TYPE_UINT16                   0x02    // 16-bit unsigned
#define REG_TYPE_UINT32                   0x03    // 32-bit unsigned
#define REG_TYPE_INT8                     0x04    // 8-bit signed
#define REG_TYPE_INT16                    0x05    // 16-bit signed
#define REG_TYPE_INT32                    0x06    // 32-bit signed
#define REG_TYPE_FLOAT                    0x07    // 32-bit float

// ============================================================================
// VALIDATION FUNCTIONS
// ============================================================================

// Validate module address
static inline bool is_valid_module_address(uint8_t address) {
    return (address >= MODULE_ADDR_POWER && address <= MODULE_ADDR_DOCK);
}

// Validate register address for Power Module
static inline bool is_valid_power_register(uint16_t reg) {
    return (reg <= 0x00FF);
}

// Validate register address for Safety Module
static inline bool is_valid_safety_register(uint16_t reg) {
    return (reg <= 0x00FF);
}

// Validate register address for Travel Motor Module
static inline bool is_valid_travel_motor_register(uint16_t reg) {
    return (reg <= 0x0106);
}

// Validate register address for Dock Module
static inline bool is_valid_dock_register(uint16_t reg) {
    return (reg <= 0x00FF);
}

// Get module type from address
static inline uint16_t get_module_type_from_address(uint8_t address) {
    switch (address) {
        case MODULE_ADDR_POWER: return 0x0002; // MODULE_TYPE_POWER
        case MODULE_ADDR_SAFETY: return 0x0003; // MODULE_TYPE_SAFETY
        case MODULE_ADDR_TRAVEL_MOTOR: return 0x0004; // MODULE_TYPE_TRAVEL_MOTOR
        case MODULE_ADDR_DOCK: return 0x0005; // MODULE_TYPE_DOCK
        default: return 0x0000;
    }
}

// Get module name from address
static inline const char* get_module_name_from_address(uint8_t address) {
    switch (address) {
        case MODULE_ADDR_POWER: return "Power Module";
        case MODULE_ADDR_SAFETY: return "Safety Module";
        case MODULE_ADDR_TRAVEL_MOTOR: return "Travel Motor Module";
        case MODULE_ADDR_DOCK: return "Dock & Location Module";
        default: return "Unknown Module";
    }
}

#endif // REGISTER_MAP_H
