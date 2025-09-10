/**
 * @file power_module_handler.h
 * @brief Power Module Handler for OHT-50 (Module 0x02)
 * @version 2.0.0
 * @date 2025-01-28
 * @author FW Team
 * @reference https://github.com/hiimshyy/stm32-power-management/blob/main/Docs/modbus_register_map.md
 */

#ifndef POWER_MODULE_HANDLER_H
#define POWER_MODULE_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"
#include "register_map.h"

// Power module capabilities
#define POWER_CAP_VOLTAGE_MONITOR     (1 << 0)
#define POWER_CAP_CURRENT_MONITOR     (1 << 1)
#define POWER_CAP_TEMP_MONITOR        (1 << 2)
#define POWER_CAP_RELAY_CONTROL       (1 << 3)
#define POWER_CAP_OVERVOLTAGE_PROTECT (1 << 4)
#define POWER_CAP_OVERCURRENT_PROTECT (1 << 5)
#define POWER_CAP_OVERTEMP_PROTECT    (1 << 6)
#define POWER_CAP_BATTERY_MONITOR     (1 << 7)
#define POWER_CAP_CHARGING_CONTROL    (1 << 8)
#define POWER_CAP_POWER_DISTRIBUTION  (1 << 9)

// Timeout constants for different operations
#define POWER_MODULE_TIMEOUT_READ_REGISTER_MS      500   // Read single register
#define POWER_MODULE_TIMEOUT_WRITE_REGISTER_MS     500   // Write single register
#define POWER_MODULE_TIMEOUT_READ_DATA_MS          1000  // Read complete data set
#define POWER_MODULE_TIMEOUT_AUTO_DETECT_MS        2000  // Auto detect module
#define POWER_MODULE_TIMEOUT_POLL_INTERVAL_MS      100   // Polling interval
#define POWER_MODULE_TIMEOUT_CONNECTION_MS         5000  // Connection timeout
#define POWER_MODULE_TIMEOUT_RESPONSE_MS           1000  // Response timeout

// Retry constants
#define POWER_MODULE_RETRY_COUNT_DEFAULT           3     // Default retry count
#define POWER_MODULE_RETRY_DELAY_MS                50    // Base delay between retries
#define POWER_MODULE_RETRY_BACKOFF_MULTIPLIER      2     // Exponential backoff multiplier
#define POWER_MODULE_RETRY_MAX_DELAY_MS            1000  // Maximum delay between retries

// Power module alarm bit definitions
#define POWER_ALARM_OVERVOLTAGE    (1 << 0)
#define POWER_ALARM_UNDERVOLTAGE   (1 << 1)
#define POWER_ALARM_OVERCURRENT    (1 << 2)
#define POWER_ALARM_OVERTEMP       (1 << 3)
#define POWER_ALARM_COMM_ERROR     (1 << 4)
#define POWER_ALARM_RELAY_FAULT    (1 << 5)
#define POWER_ALARM_BATTERY_LOW    (1 << 6)
#define POWER_ALARM_CHARGING_FAULT (1 << 7)

// Power module register map (based on STM32 Power Management)
// Modbus Configuration Registers (0x0100-0x0106)
#define POWER_REG_SLAVE_ID             0x0100  // Modbus slave address (1-247)
#define POWER_REG_BAUDRATE_CODE        0x0101  // UART baudrate setting (1=9600, 2=19200, 3=38400, 4=57600, 5=115200)
#define POWER_REG_PARITY               0x0102  // UART parity (0=None, 1=Even, 2=Odd)
#define POWER_REG_STOP_BITS            0x0103  // UART stop bits (1 or 2)
#define POWER_REG_FC_MASK              0x0104  // Supported function codes (bitmask: 0x01=FC3, 0x02=FC6, 0x04=FC16)
#define POWER_REG_APPLY_CONFIG         0x0105  // Apply config flag (write 1 to apply)
#define POWER_REG_CONFIG_CRC           0x0106  // Optional CRC for config validation

// DalyBMS Status Registers (0x0000-0x002B)
#define POWER_REG_BATTERY_VOLTAGE      0x0000  // Voltage of battery pack (V, /10.0 scaling)
#define POWER_REG_BATTERY_CURRENT      0x0001  // Current of battery pack (A, /10.0 scaling)
#define POWER_REG_BATTERY_SOC          0x0002  // State of Charge (%, /10.0 scaling)
#define POWER_REG_MAX_CELL_V           0x0003  // Maximum voltage among cells (mV)
#define POWER_REG_MIN_CELL_V           0x0004  // Minimum voltage among cells (mV)
#define POWER_REG_MAX_CELL_INDEX       0x0005  // Index of max voltage cell
#define POWER_REG_MIN_CELL_INDEX       0x0006  // Index of min voltage cell
#define POWER_REG_CELL_DIFF            0x0007  // Voltage difference between cells (mV)
#define POWER_REG_TEMPERATURE          0x0008  // Average temperature (°C)
#define POWER_REG_CONNECTION_STATUS    0x0009  // BMS Connection Status (0/1 = NG/OK)
#define POWER_REG_CHARGE_DISCHARGE_STATUS 0x000A  // Charge/discharge status flags
#define POWER_REG_CHARGE_MOS           0x000B  // Charge MOSFET status
#define POWER_REG_DISCHARGE_MOS        0x000C  // Discharge MOSFET status
#define POWER_REG_BMS_LIFE_CYCLE       0x000D  // Number of BMS power cycles
#define POWER_REG_RESIDUAL_CAPACITY    0x000E  // Remaining battery capacity (mAh)
#define POWER_REG_NUM_CELLS            0x000F  // Number of battery cells
#define POWER_REG_NUM_TEMP_SENSORS     0x0010  // Number of temperature sensors
#define POWER_REG_CHARGE_STATUS        0x0011  // Charging in progress (1 = Yes)
#define POWER_REG_DISCHARGE_STATUS     0x0012  // Discharging in progress (1 = Yes)
#define POWER_REG_CHARGE_DISCHARGE_CYCLE 0x0013  // Number of full charge-discharge cycles
#define POWER_REG_CELL_VOLTAGE_0       0x0014  // Voltage of cell 0 (mV)
#define POWER_REG_CELL_VOLTAGE_1       0x0015  // Voltage of cell 1 (mV)
#define POWER_REG_CELL_VOLTAGE_2       0x0016  // Voltage of cell 2 (mV)
#define POWER_REG_CELL_VOLTAGE_3       0x0017  // Voltage of cell 3 (mV)
#define POWER_REG_CELL_VOLTAGE_4       0x0018  // Voltage of cell 4 (mV)
#define POWER_REG_CELL_VOLTAGE_5       0x0019  // Voltage of cell 5 (mV)
#define POWER_REG_TEMPERATURE_0        0x001A  // Temperature sensor 0 (°C)
#define POWER_REG_TEMPERATURE_1        0x001B  // Temperature sensor 1 (°C)
#define POWER_REG_CELL_BALANCE_0       0x001C  // Cell 0 balancing status
#define POWER_REG_CELL_BALANCE_1       0x001D  // Cell 1 balancing status
#define POWER_REG_CELL_BALANCE_2       0x001E  // Cell 2 balancing status
#define POWER_REG_CELL_BALANCE_3       0x001F  // Cell 3 balancing status
#define POWER_REG_CELL_BALANCE_4       0x0020  // Cell 4 balancing status
#define POWER_REG_CELL_BALANCE_5       0x0021  // Cell 5 balancing status
#define POWER_REG_CELL_BALANCE_ACTIVE  0x0022  // Global balancing status
#define POWER_REG_FAULT_FLAGS          0x0023  // Fault status flags
#define POWER_REG_MAX_CELL_THRESHOLD_1 0x0024  // Max cell voltage threshold 1 (mV)
#define POWER_REG_MIN_CELL_THRESHOLD_1 0x0025  // Min cell voltage threshold 1 (mV)
#define POWER_REG_MAX_CELL_THRESHOLD_2 0x0026  // Max cell voltage threshold 2 (mV)
#define POWER_REG_MIN_CELL_THRESHOLD_2 0x0027  // Min cell voltage threshold 2 (mV)
#define POWER_REG_MAX_PACK_THRESHOLD_1 0x0028  // Max pack voltage threshold 1 (mV)
#define POWER_REG_MIN_PACK_THRESHOLD_1 0x0029  // Min pack voltage threshold 1 (mV)
#define POWER_REG_MAX_PACK_THRESHOLD_2 0x002A  // Max pack voltage threshold 2 (mV)
#define POWER_REG_MIN_PACK_THRESHOLD_2 0x002B  // Min pack voltage threshold 2 (mV)

// SK60X Data Registers (0x0030-0x003F)
#define POWER_REG_V_SET                0x0030  // Voltage setpoint (V, /10.0 scaling)
#define POWER_REG_I_SET                0x0031  // Current setpoint (A, /10.0 scaling)
#define POWER_REG_V_OUT                0x0032  // Output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT                0x0033  // Output current (A, /10.0 scaling)
#define POWER_REG_P_OUT                0x0034  // Output power (W, /10.0 scaling)
#define POWER_REG_V_IN                 0x0035  // Input voltage (V, /10.0 scaling)
#define POWER_REG_I_IN                 0x0036  // Input current (A, /10.0 scaling)
#define POWER_REG_TEMP                 0x0037  // Temperature of SK60X (°C)
#define POWER_REG_H_USE                0x0038  // Time used - hours
#define POWER_REG_M_USE                0x0039  // Time used - minutes
#define POWER_REG_S_USE                0x003A  // Time used - seconds
#define POWER_REG_STATUS               0x003B  // Operational status
#define POWER_REG_ON_OFF               0x003C  // Output ON/OFF state
#define POWER_REG_CHARGE_RELAY         0x003D  // 0 = NOT, 1 = CHARGE
#define POWER_REG_CHARGE_STATE         0x003E  // 0 = IDLE, 1 = WAITING, 2 = CHARGE
#define POWER_REG_CHARGE_REQUEST       0x003F  // 0 = NOT, 1 = REQUEST

// INA219 Sensor Values (0x0040-0x0048)
#define POWER_REG_V_OUT_12V            0x0040  // Voltage of 12V output (V, /10.0 scaling)
#define POWER_REG_I_OUT_12V            0x0041  // Current of 12V output (A, /10.0 scaling)
#define POWER_REG_P_OUT_12V            0x0042  // Power of 12V output (W, /10.0 scaling)
#define POWER_REG_V_OUT_5V             0x0043  // Voltage of 5V output (V, /10.0 scaling)
#define POWER_REG_I_OUT_5V             0x0044  // Current of 5V output (A, /10.0 scaling)
#define POWER_REG_P_OUT_5V             0x0045  // Power of 5V output (W, /10.0 scaling)
#define POWER_REG_V_OUT_3V3            0x0046  // Voltage of 3.3V output (V, /10.0 scaling)
#define POWER_REG_I_OUT_3V3            0x0047  // Current of 3.3V output (A, /10.0 scaling)
#define POWER_REG_P_OUT_3V3            0x0048  // Power of 3.3V output (W, /10.0 scaling)

// Relay state (0x0049-0x004D)
#define POWER_REG_RL_12V               0x0049  // State of 12V relay
#define POWER_REG_RL_5V                0x004A  // State of 5V relay
#define POWER_REG_RL_3V3               0x004B  // State of 3V3 relay
#define POWER_REG_RL_FAULT             0x004C  // State of Fault relay
#define POWER_REG_USE_V_THS            0x004D  // Setting voltage usage threshold (/100.0 scaling)

// System Registers (0x0100-0x0109) - Auto Detect Support
#define POWER_REG_DEVICE_ID              0x0100  // Device ID (Modbus slave address)
#define POWER_REG_CONFIG_BAUDRATE        0x0101  // Config baudrate (1=9600, 2=19200, 3=38400,...)
#define POWER_REG_CONFIG_PARITY          0x0102  // Config parity (0=None, 1=Even, 2=Odd)
#define POWER_REG_CONFIG_STOP_BITS       0x0103  // Config stop bits (1=1, 2=2)
#define POWER_REG_MODULE_TYPE            0x0104  // Module type (0x0002 = Power Module)
#define POWER_REG_FIRMWARE_VERSION       0x0105  // Firmware version (e.g. 0x0101 = v1.01)
#define POWER_REG_HARDWARE_VERSION       0x0106  // Hardware version (e.g. 0x0101 = v1.01)
#define POWER_REG_SYSTEM_STATUS          0x0107  // System status (bit field)
#define POWER_REG_SYSTEM_ERROR           0x0108  // System error (global error code)
#define POWER_REG_RESET_ERROR_CMD        0x0109  // Reset error command (write 1 to reset all error flags)
#define POWER_REG_MODULE_NAME_LOW        0x00F8  // Module name (low word)
#define POWER_REG_MODULE_NAME_HIGH       0x00F9  // Module name (high word)
#define POWER_REG_HARDWARE_VERSION       0x00FA  // Hardware version
#define POWER_REG_SERIAL_NUMBER_LOW      0x00FB  // Serial number (low word)
#define POWER_REG_SERIAL_NUMBER_HIGH     0x00FC  // Serial number (high word)
#define POWER_REG_BUILD_DATE_LOW         0x00FD  // Build date (low word)
#define POWER_REG_BUILD_DATE_HIGH        0x00FE  // Build date (high word)
#define POWER_REG_CHECKSUM               0x00FF  // Register checksum

// Power module data structure
typedef struct {
    // Modbus configuration
    uint8_t slave_id;              // Modbus slave address
    uint8_t baudrate_code;         // UART baudrate setting
    uint8_t parity;                // UART parity
    uint8_t stop_bits;             // UART stop bits
    uint8_t fc_mask;               // Supported function codes
    uint16_t config_crc;           // Config CRC
    
    // DalyBMS battery information
    float battery_voltage;         // Battery voltage (V)
    float battery_current;         // Battery current (A)
    float battery_soc;             // State of charge (%)
    uint16_t max_cell_v;           // Maximum cell voltage (mV)
    uint16_t min_cell_v;           // Minimum cell voltage (mV)
    uint8_t max_cell_index;        // Index of max voltage cell
    uint8_t min_cell_index;        // Index of min voltage cell
    uint8_t cell_diff;             // Voltage difference between cells (mV)
    uint8_t temperature;           // Average temperature (°C)
    bool connection_status;        // BMS Connection Status
    uint8_t charge_discharge_status; // Charge/discharge status flags
    bool charge_mos;               // Charge MOSFET status
    bool discharge_mos;            // Discharge MOSFET status
    uint8_t bms_life_cycle;        // Number of BMS power cycles
    uint8_t residual_capacity;     // Remaining battery capacity (mAh)
    uint8_t num_cells;             // Number of battery cells
    uint8_t num_temp_sensors;      // Number of temperature sensors
    bool charge_status;            // Charging in progress
    bool discharge_status;         // Discharging in progress
    uint8_t charge_discharge_cycle; // Number of full charge-discharge cycles
    
    // Cell voltages (6 cells)
    uint16_t cell_voltage[6];      // Voltage of each cell (mV)
    uint8_t temperature_sensors[2]; // Temperature sensor values (°C)
    bool cell_balance[6];          // Per-cell balancing status
    bool cell_balance_active;      // Global balancing status
    uint8_t fault_flags;           // Fault status flags
    
    // Cell voltage thresholds
    uint16_t max_cell_threshold_1; // Max cell voltage threshold 1 (mV)
    uint16_t min_cell_threshold_1; // Min cell voltage threshold 1 (mV)
    uint16_t max_cell_threshold_2; // Max cell voltage threshold 2 (mV)
    uint16_t min_cell_threshold_2; // Min cell voltage threshold 2 (mV)
    uint16_t max_pack_threshold_1; // Max pack voltage threshold 1 (mV)
    uint16_t min_pack_threshold_1; // Min pack voltage threshold 1 (mV)
    uint16_t max_pack_threshold_2; // Max pack voltage threshold 2 (mV)
    uint16_t min_pack_threshold_2; // Min pack voltage threshold 2 (mV)
    
    // SK60X charging control
    float v_set;                   // Voltage setpoint (V)
    float i_set;                   // Current setpoint (A)
    float v_out;                   // Output voltage (V)
    float i_out;                   // Output current (A)
    float p_out;                   // Output power (W)
    float v_in;                    // Input voltage (V)
    float i_in;                    // Input current (A)
    uint8_t temp;                  // Temperature of SK60X (°C)
    uint8_t h_use;                 // Time used - hours
    uint8_t m_use;                 // Time used - minutes
    uint8_t s_use;                 // Time used - seconds
    bool status;                   // Operational status
    bool on_off;                   // Output ON/OFF state
    bool charge_relay;             // Charge relay status
    uint16_t charge_state;         // Charge state (0=IDLE, 1=WAITING, 2=CHARGE)
    bool charge_request;           // Charge request
    
    // INA219 power distribution
    float v_out_12v;               // Voltage of 12V output (V)
    float i_out_12v;               // Current of 12V output (A)
    float p_out_12v;               // Power of 12V output (W)
    float v_out_5v;                // Voltage of 5V output (V)
    float i_out_5v;                // Current of 5V output (A)
    float p_out_5v;                // Power of 5V output (W)
    float v_out_3v3;               // Voltage of 3.3V output (V)
    float i_out_3v3;               // Current of 3.3V output (A)
    float p_out_3v3;               // Power of 3.3V output (W)
    
    // Relay states
    uint8_t rl_12v;                // State of 12V relay
    uint8_t rl_5v;                 // State of 5V relay
    uint8_t rl_3v3;                // State of 3V3 relay
    uint8_t rl_fault;              // State of Fault relay
    float use_v_ths;               // Setting voltage usage threshold
    
    // System information (from System Registers)
    uint16_t device_id;            // Device ID (Modbus slave address)
    uint16_t firmware_version;     // Firmware version (e.g. 0x0101 = v1.01)
    uint16_t system_status;        // System status (bit field)
    uint16_t system_error;         // System error (global error code)
    uint16_t hardware_version;     // Hardware version
    uint32_t serial_number;        // Serial number
    uint32_t build_date;           // Build date
    uint16_t module_type;          // Module type (0x0002 = Power Module)
    uint32_t module_name;          // Module name (32-bit)
    uint16_t checksum;             // Register checksum
    
    uint64_t last_update_ms;       // Last update timestamp
} power_module_data_t;

// Power module configuration
typedef struct {
    // Modbus configuration
    uint8_t slave_id;              // Modbus slave address (1-247)
    uint8_t baudrate_code;         // UART baudrate setting
    uint8_t parity;                // UART parity
    uint8_t stop_bits;             // UART stop bits
    uint8_t fc_mask;               // Supported function codes
    
    // Battery thresholds
    uint16_t max_cell_threshold_1; // Max cell voltage threshold 1 (mV)
    uint16_t min_cell_threshold_1; // Min cell voltage threshold 1 (mV)
    uint16_t max_cell_threshold_2; // Max cell voltage threshold 2 (mV)
    uint16_t min_cell_threshold_2; // Min cell voltage threshold 2 (mV)
    uint16_t max_pack_threshold_1; // Max pack voltage threshold 1 (mV)
    uint16_t min_pack_threshold_1; // Min pack voltage threshold 1 (mV)
    uint16_t max_pack_threshold_2; // Max pack voltage threshold 2 (mV)
    uint16_t min_pack_threshold_2; // Min pack voltage threshold 2 (mV)
    
    // Charging configuration
    float charge_voltage_set;      // Charge voltage setting (V)
    float charge_current_set;      // Charge current setting (A)
    bool charge_enable;            // Charge enable/disable
    
    // Output configuration
    bool output_12v_enabled;       // 12V output enabled
    bool output_5v_enabled;        // 5V output enabled
    bool output_3v3_enabled;       // 3.3V output enabled
    
    // Protection thresholds
    float overvoltage_threshold;   // Overvoltage threshold (V)
    float overcurrent_threshold;   // Overcurrent threshold (A)
    float overtemp_threshold;      // Overtemperature threshold (°C)
    
    // Voltage usage threshold
    float use_v_ths;               // Setting voltage usage threshold
} power_module_config_t;

// Power module status
typedef struct {
    bool initialized;
    bool online;
    uint32_t error_count;
    uint32_t warning_count;
    uint64_t uptime_seconds;
    uint64_t last_communication_ms;
} power_module_status_t;

// Function prototypes

/**
 * @brief Initialize power module handler
 * @param config Power module configuration
 * @return HAL status
 */
hal_status_t power_module_handler_init(const power_module_config_t *config);

/**
 * @brief Deinitialize power module handler
 * @return HAL status
 */
hal_status_t power_module_handler_deinit(void);

/**
 * @brief Read power module data
 * @param data Pointer to store power module data
 * @return HAL status
 */
hal_status_t power_module_handler_read_data(power_module_data_t *data);

/**
 * @brief Write power module register
 * @param register_addr Register address
 * @param value Register value
 * @return HAL status
 */
hal_status_t power_module_handler_write_register(uint16_t register_addr, uint16_t value);

/**
 * @brief Read power module register
 * @param register_addr Register address
 * @param value Pointer to store register value
 * @return HAL status
 */
hal_status_t power_module_handler_read_register(uint16_t register_addr, uint16_t *value);

/**
 * @brief Get power module status
 * @param status Pointer to store power module status
 * @return HAL status
 */
hal_status_t power_module_handler_get_status(power_module_status_t *status);

/**
 * @brief Set charging parameters
 * @param current Charge current (A)
 * @param voltage Charge voltage (V)
 * @param enable Enable/disable charging
 * @return HAL status
 */
hal_status_t power_module_handler_set_charging(float current, float voltage, bool enable);

/**
 * @brief Control power outputs
 * @param output_12v 12V output control
 * @param output_5v 5V output control
 * @param output_3v3 3.3V output control
 * @return HAL status
 */
hal_status_t power_module_handler_control_outputs(bool output_12v, bool output_5v, bool output_3v3);

/**
 * @brief Reset power module faults
 * @return HAL status
 */
hal_status_t power_module_handler_reset_faults(void);

/**
 * @brief Get power module capabilities
 * @return Power module capabilities bitmap
 */
uint32_t power_module_handler_get_capabilities(void);

/**
 * @brief Auto detect power module
 * @param slave_id Slave ID to detect (1-247)
 * @param timeout_ms Detection timeout in milliseconds
 * @return HAL status
 */
hal_status_t power_module_handler_auto_detect(uint8_t slave_id, uint32_t timeout_ms);

/**
 * @brief Get module identification information
 * @param device_id Pointer to store device ID
 * @param module_type Pointer to store module type
 * @param firmware_version Pointer to store firmware version
 * @param hardware_version Pointer to store hardware version
 * @return HAL status
 */
hal_status_t power_module_handler_get_module_info(uint16_t *device_id, uint16_t *module_type, 
                                                  uint16_t *firmware_version, uint16_t *hardware_version);

/**
 * @brief Get module name
 * @param name_buffer Buffer to store module name (minimum 9 characters)
 * @param buffer_size Size of name buffer
 * @return HAL status
 */
hal_status_t power_module_handler_get_module_name(char *name_buffer, size_t buffer_size);

/**
 * @brief Reset system errors
 * @return HAL status
 */
hal_status_t power_module_handler_reset_system_errors(void);

/**
 * @brief Get system status
 * @param system_status Pointer to store system status
 * @param system_error Pointer to store system error
 * @return HAL status
 */
hal_status_t power_module_handler_get_system_status(uint16_t *system_status, uint16_t *system_error);

/**
 * @brief Poll power module data (continuous polling function)
 * @return HAL status
 */
hal_status_t power_module_handler_poll_data(void);

// Timeout handling functions
/**
 * @brief Read register with timeout protection
 * @param register_addr Register address to read
 * @param value Pointer to store register value
 * @param timeout_ms Timeout in milliseconds
 * @return HAL status
 */
hal_status_t power_module_handler_read_register_with_timeout(uint16_t register_addr, uint16_t *value, uint32_t timeout_ms);

/**
 * @brief Write register with timeout protection
 * @param register_addr Register address to write
 * @param value Register value to write
 * @param timeout_ms Timeout in milliseconds
 * @return HAL status
 */
hal_status_t power_module_handler_write_register_with_timeout(uint16_t register_addr, uint16_t value, uint32_t timeout_ms);

/**
 * @brief Check if timeout has occurred
 * @param start_time Start timestamp in milliseconds
 * @param timeout_ms Timeout duration in milliseconds
 * @return true if timeout occurred, false otherwise
 */
bool power_module_check_timeout(uint64_t start_time, uint32_t timeout_ms);

// Retry mechanism functions
/**
 * @brief Calculate retry delay with exponential backoff
 * @param attempt Current attempt number (0-based)
 * @param base_delay_ms Base delay in milliseconds
 * @param multiplier Backoff multiplier
 * @param max_delay_ms Maximum delay in milliseconds
 * @return Calculated delay in milliseconds
 */
uint32_t power_module_calculate_retry_delay(uint8_t attempt, uint32_t base_delay_ms, 
                                          uint32_t multiplier, uint32_t max_delay_ms);

/**
 * @brief Read register with retry mechanism
 * @param register_addr Register address to read
 * @param value Pointer to store register value
 * @param max_retries Maximum number of retries
 * @return HAL status
 */
hal_status_t power_module_handler_read_register_with_retry(uint16_t register_addr, uint16_t *value, uint8_t max_retries);

/**
 * @brief Read battery data with retry mechanism
 * @return HAL status
 */
hal_status_t power_module_read_battery_data_with_retry(void);

#endif // POWER_MODULE_HANDLER_H
