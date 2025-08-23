/**
 * @file power_module_handler.c
 * @brief Power Module Handler Implementation for OHT-50 (Module 0x02)
 * @version 2.0.0
 * @date 2025-01-28
 * @author FW Team
 * @reference https://github.com/hiimshyy/stm32-power-management/blob/main/Docs/modbus_register_map.md
 */

#include "power_module_handler.h"
#include "hal_rs485.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

// Power module configuration
#define POWER_MODULE_ADDRESS 0x02
#define POWER_MODULE_TIMEOUT_MS 1000
#define POWER_MODULE_RETRY_COUNT 3

// Internal state
static struct {
    bool initialized;
    pthread_mutex_t mutex;
    power_module_data_t data;
    power_module_config_t config;
    power_module_status_t status;
    uint64_t last_update_ms;
    uint32_t capabilities;
} power_module_state = {0};

// Internal function prototypes
static uint64_t power_module_get_timestamp_ms(void);
static hal_status_t power_module_read_battery_data(void);
static hal_status_t power_module_read_charging_data(void);
static hal_status_t power_module_read_power_distribution(void);
static hal_status_t power_module_read_fault_status(void);
static hal_status_t power_module_read_system_info(void);
static hal_status_t power_module_validate_config(const power_module_config_t *config);
static void power_module_update_capabilities(void);

/**
 * @brief Initialize power module handler
 * @param config Power module configuration
 * @return HAL status
 */
hal_status_t power_module_handler_init(const power_module_config_t *config)
{
    hal_status_t status;
    
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate configuration
    status = power_module_validate_config(config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check if already initialized
    if (power_module_state.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize mutex first
    if (pthread_mutex_init(&power_module_state.mutex, NULL) != 0) {
        return HAL_STATUS_ERROR;
    }
    
    // Now lock mutex for data initialization
    pthread_mutex_lock(&power_module_state.mutex);
    
    // Copy configuration
    memcpy(&power_module_state.config, config, sizeof(power_module_config_t));
    
    // Initialize data structure
    memset(&power_module_state.data, 0, sizeof(power_module_data_t));
    power_module_state.data.device_id = POWER_MODULE_ADDRESS;
    power_module_state.data.firmware_version = 0x0200; // v2.0.0
    power_module_state.data.hardware_version = 0x0100; // v1.0.0
    power_module_state.data.serial_number = 0x12345678;
    power_module_state.data.build_date = 20250128;
    
    // Initialize status
    memset(&power_module_state.status, 0, sizeof(power_module_status_t));
    power_module_state.status.online = false;
    
    // Update capabilities
    power_module_update_capabilities();
    
    power_module_state.initialized = true;
    power_module_state.last_update_ms = power_module_get_timestamp_ms();
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    printf("Power module handler initialized successfully\n");
    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize power module handler
 * @return HAL status
 */
hal_status_t power_module_handler_deinit(void)
{
    pthread_mutex_lock(&power_module_state.mutex);
    
    if (!power_module_state.initialized) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Cleanup mutex
    pthread_mutex_destroy(&power_module_state.mutex);
    
    power_module_state.initialized = false;
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    printf("Power module handler deinitialized\n");
    return HAL_STATUS_OK;
}

/**
 * @brief Read power module data
 * @param data Pointer to store power module data
 * @return HAL status
 */
hal_status_t power_module_handler_read_data(power_module_data_t *data)
{
    hal_status_t status;
    
    if (!data) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    if (!power_module_state.initialized) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Read all data from power module
    status = power_module_read_battery_data();
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    status = power_module_read_charging_data();
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    status = power_module_read_power_distribution();
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    status = power_module_read_fault_status();
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    status = power_module_read_system_info();
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Update timestamp
    power_module_state.data.last_update_ms = power_module_get_timestamp_ms();
    power_module_state.last_update_ms = power_module_state.data.last_update_ms;
    
    // Copy data to output
    memcpy(data, &power_module_state.data, sizeof(power_module_data_t));
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Write power module register
 * @param register_addr Register address
 * @param value Register value
 * @return HAL status
 */
hal_status_t power_module_handler_write_register(uint16_t register_addr, uint16_t value)
{
    hal_status_t status;
    uint8_t tx_data[8];
    uint8_t rx_data[8];
    size_t rx_length;
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    if (!power_module_state.initialized) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Prepare Modbus write command
    tx_data[0] = POWER_MODULE_ADDRESS;  // Slave address
    tx_data[1] = 0x06;                  // Function code: Write Single Register
    tx_data[2] = (register_addr >> 8) & 0xFF;  // Register address high
    tx_data[3] = register_addr & 0xFF;         // Register address low
    tx_data[4] = (value >> 8) & 0xFF;          // Register value high
    tx_data[5] = value & 0xFF;                 // Register value low
    
    // Calculate CRC
    uint16_t crc = modbus_calculate_crc(tx_data, 6);
    tx_data[6] = crc & 0xFF;
    tx_data[7] = (crc >> 8) & 0xFF;
    
    // Send command
    status = hal_rs485_transmit(tx_data, 8);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Receive response
    status = hal_rs485_receive(rx_data, sizeof(rx_data), &rx_length);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Validate response
    if (rx_length < 8) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Check CRC
    uint16_t rx_crc = (rx_data[rx_length-1] << 8) | rx_data[rx_length-2];
    uint16_t calc_crc = modbus_calculate_crc(rx_data, rx_length-2);
    if (rx_crc != calc_crc) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Read power module register
 * @param register_addr Register address
 * @param value Pointer to store register value
 * @return HAL status
 */
hal_status_t power_module_handler_read_register(uint16_t register_addr, uint16_t *value)
{
    hal_status_t status;
    uint8_t tx_data[8];
    uint8_t rx_data[8];
    size_t rx_length;
    
    if (!value) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    if (!power_module_state.initialized) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Prepare Modbus read command
    tx_data[0] = POWER_MODULE_ADDRESS;  // Slave address
    tx_data[1] = 0x03;                  // Function code: Read Holding Registers
    tx_data[2] = (register_addr >> 8) & 0xFF;  // Register address high
    tx_data[3] = register_addr & 0xFF;         // Register address low
    tx_data[4] = 0x00;                         // Number of registers high
    tx_data[5] = 0x01;                         // Number of registers low (1 register)
    
    // Calculate CRC
    uint16_t crc = modbus_calculate_crc(tx_data, 6);
    tx_data[6] = crc & 0xFF;
    tx_data[7] = (crc >> 8) & 0xFF;
    
    // Send command
    status = hal_rs485_transmit(tx_data, 8);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Receive response
    status = hal_rs485_receive(rx_data, sizeof(rx_data), &rx_length);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Validate response
    if (rx_length < 7) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Check CRC
    uint16_t rx_crc = (rx_data[rx_length-1] << 8) | rx_data[rx_length-2];
    uint16_t calc_crc = modbus_calculate_crc(rx_data, rx_length-2);
    if (rx_crc != calc_crc) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_ERROR;
    }
    
    // Extract register value
    *value = (rx_data[3] << 8) | rx_data[4];
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Get power module status
 * @param status Pointer to store power module status
 * @return HAL status
 */
hal_status_t power_module_handler_get_status(power_module_status_t *status)
{
    if (!status) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    if (!power_module_state.initialized) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Copy status
    memcpy(status, &power_module_state.status, sizeof(power_module_status_t));
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    return HAL_STATUS_OK;
}

/**
 * @brief Set charging parameters
 * @param current Charge current (A)
 * @param voltage Charge voltage (V)
 * @param enable Enable/disable charging
 * @return HAL status
 */
hal_status_t power_module_handler_set_charging(float current, float voltage, bool enable)
{
    hal_status_t status;
    uint16_t current_reg, voltage_reg, enable_reg;
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    if (!power_module_state.initialized) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Validate parameters
    if (current < 0.0f) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (voltage < 0.0f) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    // Convert to register values
    current_reg = (uint16_t)(current * 10.0f);  // 0.1A resolution
    voltage_reg = (uint16_t)(voltage * 10.0f);  // 0.1V resolution
    enable_reg = enable ? 1 : 0;
    
    // Write registers
    status = power_module_handler_write_register(POWER_REG_I_SET, current_reg);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = power_module_handler_write_register(POWER_REG_V_SET, voltage_reg);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = power_module_handler_write_register(POWER_REG_CHARGE_REQUEST, enable_reg);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Control power outputs
 * @param output_12v 12V output control
 * @param output_5v 5V output control
 * @param output_3v3 3.3V output control
 * @return HAL status
 */
hal_status_t power_module_handler_control_outputs(bool output_12v, bool output_5v, bool output_3v3)
{
    hal_status_t status;
    uint16_t status_12v, status_5v, status_3v3;
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    if (!power_module_state.initialized) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Check if outputs are enabled in configuration
    if (output_12v && !power_module_state.config.output_12v_enabled) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (output_5v && !power_module_state.config.output_5v_enabled) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (output_3v3 && !power_module_state.config.output_3v3_enabled) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    // Convert to register values
    status_12v = output_12v ? 1 : 0;
    status_5v = output_5v ? 1 : 0;
    status_3v3 = output_3v3 ? 1 : 0;
    
    // Write registers
    status = power_module_handler_write_register(POWER_REG_RL_12V, status_12v);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = power_module_handler_write_register(POWER_REG_RL_5V, status_5v);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    status = power_module_handler_write_register(POWER_REG_RL_3V3, status_3v3);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Reset power module faults
 * @return HAL status
 */
hal_status_t power_module_handler_reset_faults(void)
{
    return power_module_handler_write_register(POWER_REG_RESET_ERROR_CMD, 1);
}

/**
 * @brief Get power module capabilities
 * @return Power module capabilities bitmap
 */
uint32_t power_module_handler_get_capabilities(void)
{
    uint32_t capabilities = 0;
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    if (power_module_state.initialized) {
        capabilities = power_module_state.capabilities;
    }
    
    pthread_mutex_unlock(&power_module_state.mutex);
    
    return capabilities;
}

// Internal functions

/**
 * @brief Get current timestamp in milliseconds
 * @return Timestamp in milliseconds
 */
static uint64_t power_module_get_timestamp_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}

/**
 * @brief Read battery data from power module
 * @return HAL status
 */
static hal_status_t power_module_read_battery_data(void)
{
    hal_status_t status;
    uint16_t value;
    
    // Read battery voltage
    status = power_module_handler_read_register(POWER_REG_BATTERY_VOLTAGE, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.battery_voltage = (float)value / 10.0f;
    }
    
    // Read battery current
    status = power_module_handler_read_register(POWER_REG_BATTERY_CURRENT, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.battery_current = (float)value / 10.0f;
    }
    
    // Read battery temperature (using correct register from reference)
    status = power_module_handler_read_register(POWER_REG_TEMPERATURE, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.battery_temp = (float)value; // No scaling needed
    }
    
    // Read battery SOC
    status = power_module_handler_read_register(POWER_REG_BATTERY_SOC, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.battery_soc = (uint8_t)value;
    }
    
    // Note: battery_health, battery_cycles, time_to_empty, time_to_full not available in reference
    // These fields will remain at default values
    
    return HAL_STATUS_OK;
}

/**
 * @brief Read charging data from power module
 * @return HAL status
 */
static hal_status_t power_module_read_charging_data(void)
{
    hal_status_t status;
    uint16_t value;
    
    // Read charge status
    status = power_module_handler_read_register(POWER_REG_CHARGE_STATUS, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.charge_status = (uint8_t)value;
    }
    
    // Note: charge_current_set, charge_voltage_set, charge_enable, charge_mode, charge_time_remaining 
    // not available in reference. These fields will remain at default values.
    
    return HAL_STATUS_OK;
}

/**
 * @brief Read power distribution data from power module
 * @return HAL status
 */
static hal_status_t power_module_read_power_distribution(void)
{
    hal_status_t status;
    uint16_t value;
    
    // Read 12V output voltage, current, and power
    status = power_module_handler_read_register(POWER_REG_V_OUT_12V, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.v_out_12v = (float)value / 10.0f;
    }
    
    status = power_module_handler_read_register(POWER_REG_I_OUT_12V, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.i_out_12v = (float)value / 10.0f;
    }
    
    status = power_module_handler_read_register(POWER_REG_P_OUT_12V, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.p_out_12v = (float)value / 10.0f;
    }
    
    // Read 5V output voltage, current, and power
    status = power_module_handler_read_register(POWER_REG_V_OUT_5V, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.v_out_5v = (float)value / 10.0f;
    }
    
    status = power_module_handler_read_register(POWER_REG_I_OUT_5V, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.i_out_5v = (float)value / 10.0f;
    }
    
    status = power_module_handler_read_register(POWER_REG_P_OUT_5V, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.p_out_5v = (float)value / 10.0f;
    }
    
    // Read 3.3V output voltage, current, and power
    status = power_module_handler_read_register(POWER_REG_V_OUT_3V3, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.v_out_3v3 = (float)value / 10.0f;
    }
    
    status = power_module_handler_read_register(POWER_REG_I_OUT_3V3, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.i_out_3v3 = (float)value / 10.0f;
    }
    
    status = power_module_handler_read_register(POWER_REG_P_OUT_3V3, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.p_out_3v3 = (float)value / 10.0f;
    }
    
    // Read relay states
    status = power_module_handler_read_register(POWER_REG_RL_12V, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.rl_12v = (uint8_t)value;
    }
    
    status = power_module_handler_read_register(POWER_REG_RL_5V, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.rl_5v = (uint8_t)value;
    }
    
    status = power_module_handler_read_register(POWER_REG_RL_3V3, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.rl_3v3 = (uint8_t)value;
    }
    
            status = power_module_handler_read_register(POWER_REG_RL_FAULT, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.rl_fault = (uint8_t)value;
    }
    
    // Read voltage usage threshold
    status = power_module_handler_read_register(POWER_REG_USE_V_THS, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.use_v_ths = (float)value / 100.0f;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Read fault status from power module
 * @return HAL status
 */
static hal_status_t power_module_read_fault_status(void)
{
    hal_status_t status;
    uint16_t value;
    
    // Read fault flags (using correct register from reference)
    status = power_module_handler_read_register(POWER_REG_FAULT_FLAGS, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.fault_flags = value;
        // Parse individual fault bits from fault_flags
        power_module_state.data.overvoltage_fault = (value & POWER_ALARM_OVERVOLTAGE) != 0;
        power_module_state.data.overcurrent_fault = (value & POWER_ALARM_OVERCURRENT) != 0;
        power_module_state.data.overtemp_fault = (value & POWER_ALARM_OVERTEMP) != 0;
        power_module_state.data.comm_fault = (value & POWER_ALARM_COMM_ERROR) != 0;
        power_module_state.data.short_circuit_fault = false; // Not available in reference
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Read system information from power module
 * @return HAL status
 */
static hal_status_t power_module_read_system_info(void)
{
    hal_status_t status;
    uint16_t value;
    
    // Read device ID
    status = power_module_handler_read_register(POWER_REG_DEVICE_ID, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.device_id = value;
    }
    
    // Read firmware version
    status = power_module_handler_read_register(POWER_REG_FIRMWARE_VERSION, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.firmware_version = value;
    }
    
    // Read system status
    status = power_module_handler_read_register(POWER_REG_SYSTEM_STATUS, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.system_status = value;
    }
    
    // Read system error
    status = power_module_handler_read_register(POWER_REG_SYSTEM_ERROR, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.system_error = value;
    }
    
    // Read hardware version
    status = power_module_handler_read_register(POWER_REG_HARDWARE_VERSION, &value);
    if (status == HAL_STATUS_OK) {
        power_module_state.data.hardware_version = value;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Validate power module configuration
 * @param config Power module configuration
 * @return HAL status
 */
static hal_status_t power_module_validate_config(const power_module_config_t *config)
{
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate Modbus configuration
    if (config->slave_id < 1 || config->slave_id > 247) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->baudrate_code < 1 || config->baudrate_code > 5) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->parity > 2) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->stop_bits < 1 || config->stop_bits > 2) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate battery thresholds
    if (config->max_cell_threshold_1 < config->min_cell_threshold_1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_cell_threshold_2 < config->min_cell_threshold_2) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_pack_threshold_1 < config->min_pack_threshold_1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->max_pack_threshold_2 < config->min_pack_threshold_2) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate charging configuration
    if (config->charge_voltage_set < 0.0f) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->charge_current_set < 0.0f) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate protection thresholds
    if (config->overvoltage_threshold < 0.0f) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->overcurrent_threshold < 0.0f) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->overtemp_threshold < -40.0f || config->overtemp_threshold > 85.0f) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate voltage usage threshold
    if (config->use_v_ths < 0.0f) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Update power module capabilities
 */
static void power_module_update_capabilities(void)
{
    power_module_state.capabilities = 0;
    
    // Basic capabilities
    power_module_state.capabilities |= POWER_CAP_VOLTAGE_MONITOR;
    power_module_state.capabilities |= POWER_CAP_CURRENT_MONITOR;
    power_module_state.capabilities |= POWER_CAP_TEMP_MONITOR;
    power_module_state.capabilities |= POWER_CAP_RELAY_CONTROL;
    
    // Protection capabilities
    power_module_state.capabilities |= POWER_CAP_OVERVOLTAGE_PROTECT;
    power_module_state.capabilities |= POWER_CAP_OVERCURRENT_PROTECT;
    power_module_state.capabilities |= POWER_CAP_OVERTEMP_PROTECT;
    
    // Advanced capabilities
    power_module_state.capabilities |= POWER_CAP_BATTERY_MONITOR;
    power_module_state.capabilities |= POWER_CAP_CHARGING_CONTROL;
    power_module_state.capabilities |= POWER_CAP_POWER_DISTRIBUTION;
}

/**
 * @brief Auto detect power module
 * @param slave_id Slave ID to detect (1-247)
 * @param timeout_ms Detection timeout in milliseconds
 * @return HAL status
 */
hal_status_t power_module_handler_auto_detect(uint8_t slave_id, uint32_t timeout_ms)
{
    if (slave_id < 1 || slave_id > 247) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!power_module_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    // Read module type to verify it's a power module
    uint16_t module_type;
    hal_status_t status = power_module_handler_read_register(POWER_REG_MODULE_TYPE, &module_type);
    
    if (status == HAL_STATUS_OK && module_type == 0x0002) {
        // Read device ID to confirm
        uint16_t device_id;
        status = power_module_handler_read_register(POWER_REG_DEVICE_ID, &device_id);
        
        if (status == HAL_STATUS_OK && device_id == slave_id) {
            power_module_state.status.online = true;
            power_module_state.status.last_communication_ms = power_module_get_timestamp_ms();
            printf("Power module detected at slave ID %d\n", slave_id);
        } else {
            power_module_state.status.online = false;
            status = HAL_STATUS_ERROR;
        }
    } else {
        power_module_state.status.online = false;
        status = HAL_STATUS_ERROR;
    }
    
    pthread_mutex_unlock(&power_module_state.mutex);
    return status;
}

/**
 * @brief Get module identification information
 * @param device_id Pointer to store device ID
 * @param module_type Pointer to store module type
 * @param firmware_version Pointer to store firmware version
 * @param hardware_version Pointer to store hardware version
 * @return HAL status
 */
hal_status_t power_module_handler_get_module_info(uint16_t *device_id, uint16_t *module_type, 
                                                  uint16_t *firmware_version, uint16_t *hardware_version)
{
    if (!device_id || !module_type || !firmware_version || !hardware_version) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!power_module_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    hal_status_t status = HAL_STATUS_OK;
    
    // Read device ID
    status = power_module_handler_read_register(POWER_REG_DEVICE_ID, device_id);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Read module type
    status = power_module_handler_read_register(POWER_REG_MODULE_TYPE, module_type);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Read firmware version
    status = power_module_handler_read_register(POWER_REG_FIRMWARE_VERSION, firmware_version);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Read hardware version
    status = power_module_handler_read_register(POWER_REG_HARDWARE_VERSION, hardware_version);
    
    pthread_mutex_unlock(&power_module_state.mutex);
    return status;
}

/**
 * @brief Get module name
 * @param name_buffer Buffer to store module name (minimum 9 characters)
 * @param buffer_size Size of name buffer
 * @return HAL status
 */
hal_status_t power_module_handler_get_module_name(char *name_buffer, size_t buffer_size)
{
    if (!name_buffer || buffer_size < 9) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!power_module_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    uint16_t name_low, name_high;
    hal_status_t status;
    
    // Read module name (low word)
    status = power_module_handler_read_register(POWER_REG_MODULE_NAME_LOW, &name_low);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Read module name (high word)
    status = power_module_handler_read_register(POWER_REG_MODULE_NAME_HIGH, &name_high);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Combine low and high words to form 32-bit module name
    uint32_t module_name = ((uint32_t)name_high << 16) | name_low;
    
    // Convert to string (assuming ASCII encoding)
    snprintf(name_buffer, buffer_size, "PWR%08X", module_name);
    
    pthread_mutex_unlock(&power_module_state.mutex);
    return HAL_STATUS_OK;
}

/**
 * @brief Reset system errors
 * @return HAL status
 */
hal_status_t power_module_handler_reset_system_errors(void)
{
    if (!power_module_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    // Write 1 to reset error command register
    hal_status_t status = power_module_handler_write_register(POWER_REG_RESET_ERROR_CMD, 1);
    
    if (status == HAL_STATUS_OK) {
        power_module_state.status.error_count = 0;
        printf("System errors reset successfully\n");
    }
    
    pthread_mutex_unlock(&power_module_state.mutex);
    return status;
}

/**
 * @brief Get system status
 * @param system_status Pointer to store system status
 * @param system_error Pointer to store system error
 * @return HAL status
 */
hal_status_t power_module_handler_get_system_status(uint16_t *system_status, uint16_t *system_error)
{
    if (!system_status || !system_error) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (!power_module_state.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    hal_status_t status;
    
    // Read system status
    status = power_module_handler_read_register(POWER_REG_SYSTEM_STATUS, system_status);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Read system error
    status = power_module_handler_read_register(POWER_REG_SYSTEM_ERROR, system_error);
    
    pthread_mutex_unlock(&power_module_state.mutex);
    return status;
}
