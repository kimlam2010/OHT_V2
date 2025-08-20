/**
 * @file power_module_handler.h
 * @brief Power Module Handler for OHT-50 (Module 0x02)
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#ifndef POWER_MODULE_HANDLER_H
#define POWER_MODULE_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// Power module capabilities
#define POWER_CAP_VOLTAGE_MONITOR     (1 << 0)
#define POWER_CAP_CURRENT_MONITOR     (1 << 1)
#define POWER_CAP_TEMP_MONITOR        (1 << 2)
#define POWER_CAP_RELAY_CONTROL       (1 << 3)
#define POWER_CAP_OVERVOLTAGE_PROTECT (1 << 4)
#define POWER_CAP_OVERCURRENT_PROTECT (1 << 5)
#define POWER_CAP_OVERTEMP_PROTECT    (1 << 6)

// Power module alarm bit definitions
#define POWER_ALARM_OVERVOLTAGE    (1 << 0)
#define POWER_ALARM_UNDERVOLTAGE   (1 << 1)
#define POWER_ALARM_OVERCURRENT    (1 << 2)
#define POWER_ALARM_OVERTEMP       (1 << 3)
#define POWER_ALARM_COMM_ERROR     (1 << 4)
#define POWER_ALARM_RELAY_FAULT    (1 << 5)

// Power module data structure
typedef struct {
    float voltage_main;      // Main voltage (V)
    float current_main;      // Main current (A)
    float temp_main;         // Main temperature (°C)
    bool relay1_status;      // Relay 1 status
    bool relay2_status;      // Relay 2 status
    uint16_t alarm_status;   // Alarm status bitmap
    uint64_t last_update_ms; // Last update timestamp
} power_module_data_t;

// Power module configuration
typedef struct {
    float voltage_max;       // Maximum voltage threshold (V)
    float voltage_min;       // Minimum voltage threshold (V)
    float current_max;       // Maximum current threshold (A)
    float temp_max;          // Maximum temperature threshold (°C)
    bool relay1_enabled;     // Relay 1 enabled
    bool relay2_enabled;     // Relay 2 enabled
} power_module_config_t;

// Power module status
typedef struct {
    bool initialized;
    bool online;
    uint32_t error_count;
    uint32_t timeout_count;
    uint64_t last_seen_ms;
} power_module_status_t;

// Power module handler instance
typedef struct {
    power_module_data_t data;
    power_module_config_t config;
    power_module_status_t status;
    uint8_t address;         // Module address (0x02)
} power_module_handler_t;

// Function declarations
hal_status_t power_module_init(power_module_handler_t *handler, uint8_t address);
hal_status_t power_module_deinit(power_module_handler_t *handler);

// Data reading functions
hal_status_t power_module_read_voltage(power_module_handler_t *handler);
hal_status_t power_module_read_current(power_module_handler_t *handler);
hal_status_t power_module_read_temperature(power_module_handler_t *handler);
hal_status_t power_module_read_relay_status(power_module_handler_t *handler);
hal_status_t power_module_read_alarm_status(power_module_handler_t *handler);
hal_status_t power_module_read_all_data(power_module_handler_t *handler);

// Control functions
hal_status_t power_module_set_relay1(power_module_handler_t *handler, bool on);
hal_status_t power_module_set_relay2(power_module_handler_t *handler, bool on);
hal_status_t power_module_set_relays(power_module_handler_t *handler, bool relay1_on, bool relay2_on);

// Configuration functions
hal_status_t power_module_set_config(power_module_handler_t *handler, const power_module_config_t *config);
hal_status_t power_module_get_config(power_module_handler_t *handler, power_module_config_t *config);

// Status functions
hal_status_t power_module_get_status(power_module_handler_t *handler, power_module_status_t *status);
hal_status_t power_module_get_data(power_module_handler_t *handler, power_module_data_t *data);

// Safety functions
bool power_module_check_alarms(power_module_handler_t *handler);
hal_status_t power_module_emergency_shutdown(power_module_handler_t *handler);

// Utility functions
const char* power_module_get_alarm_description(uint16_t alarm_status);
void power_module_print_status(const power_module_handler_t *handler);
void power_module_print_data(const power_module_handler_t *handler);

#endif // POWER_MODULE_HANDLER_H
