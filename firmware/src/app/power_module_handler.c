/**
 * @file power_module_handler.c
 * @brief Power Module Handler implementation for OHT-50 (Module 0x02)
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include "power_module_handler.h"
#include "communication_manager.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "safety_manager.h"

// Default configuration
static const power_module_config_t default_config = {
    .voltage_max = 28.0f,    // 28V max
    .voltage_min = 20.0f,    // 20V min
    .current_max = 10.0f,    // 10A max
    .temp_max = 60.0f,       // 60°C max
    .relay1_enabled = true,
    .relay2_enabled = true
};

hal_status_t power_module_init(power_module_handler_t *handler, uint8_t address) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    printf("[POWER] Initializing power module 0x%02X\n", address);
    
    // Initialize handler structure
    memset(handler, 0, sizeof(power_module_handler_t));
    handler->address = address;
    
    // Set default configuration
    memcpy(&handler->config, &default_config, sizeof(power_module_config_t));
    
    // Initialize status
    handler->status.initialized = true;
    handler->status.online = false;
    handler->status.error_count = 0;
    handler->status.timeout_count = 0;
    handler->status.last_seen_ms = 0;
    
    // Initialize data
    handler->data.voltage_main = 0.0f;
    handler->data.current_main = 0.0f;
    handler->data.temp_main = 0.0f;
    handler->data.relay1_status = false;
    handler->data.relay2_status = false;
    handler->data.alarm_status = 0;
    handler->data.last_update_ms = 0;
    
    printf("[POWER] Power module 0x%02X initialized\n", address);
    return HAL_STATUS_OK;
}

hal_status_t power_module_deinit(power_module_handler_t *handler) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    printf("[POWER] Deinitializing power module 0x%02X\n", handler->address);
    
    // Emergency shutdown if online
    if (handler->status.online) {
        power_module_emergency_shutdown(handler);
    }
    
    // Clear handler
    memset(handler, 0, sizeof(power_module_handler_t));
    
    return HAL_STATUS_OK;
}

hal_status_t power_module_read_voltage(power_module_handler_t *handler) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    uint16_t raw_voltage = 0;
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        handler->address, POWER_REG_VOLTAGE_MAIN, 1, &raw_voltage);
    if (status != HAL_STATUS_OK) {
        // Busy/offset fallback: wait 200ms, try +1 address
        hal_sleep_ms(200);
        status = comm_manager_modbus_read_holding_registers(
            handler->address, POWER_REG_VOLTAGE_MAIN + 1, 1, &raw_voltage);
    }
    
    if (status == HAL_STATUS_OK) {
        handler->data.voltage_main = (float)raw_voltage / 10.0f; // V*10 -> V
        handler->data.last_update_ms = hal_get_timestamp_ms();
        handler->status.last_seen_ms = hal_get_timestamp_ms();
        handler->status.online = true;
        handler->status.error_count = 0;
        
        printf("[POWER] 0x%02X Voltage: %.1fV\n", handler->address, handler->data.voltage_main);
    } else {
        handler->status.error_count++;
        handler->status.online = false;
        printf("[POWER] 0x%02X Failed to read voltage (error %d)\n", handler->address, status);
    }
    
    return status;
}

hal_status_t power_module_read_current(power_module_handler_t *handler) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    uint16_t raw_current = 0;
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        handler->address, POWER_REG_CURRENT_MAIN, 1, &raw_current);
    if (status != HAL_STATUS_OK) {
        hal_sleep_ms(200);
        status = comm_manager_modbus_read_holding_registers(
            handler->address, POWER_REG_CURRENT_MAIN + 1, 1, &raw_current);
    }
    
    if (status == HAL_STATUS_OK) {
        handler->data.current_main = (float)raw_current / 10.0f; // A*10 -> A
        handler->data.last_update_ms = hal_get_timestamp_ms();
        handler->status.last_seen_ms = hal_get_timestamp_ms();
        handler->status.online = true;
        handler->status.error_count = 0;
        
        printf("[POWER] 0x%02X Current: %.1fA\n", handler->address, handler->data.current_main);
    } else {
        handler->status.error_count++;
        handler->status.online = false;
        printf("[POWER] 0x%02X Failed to read current (error %d)\n", handler->address, status);
    }
    
    return status;
}

hal_status_t power_module_read_temperature(power_module_handler_t *handler) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    uint16_t raw_temp = 0;
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        handler->address, POWER_REG_TEMP_MAIN, 1, &raw_temp);
    if (status != HAL_STATUS_OK) {
        hal_sleep_ms(200);
        status = comm_manager_modbus_read_holding_registers(
            handler->address, POWER_REG_TEMP_MAIN + 1, 1, &raw_temp);
    }
    
    if (status == HAL_STATUS_OK) {
        handler->data.temp_main = (float)raw_temp / 10.0f; // 0.1°C -> °C
        handler->data.last_update_ms = hal_get_timestamp_ms();
        handler->status.last_seen_ms = hal_get_timestamp_ms();
        handler->status.online = true;
        handler->status.error_count = 0;
        
        printf("[POWER] 0x%02X Temperature: %.1f°C\n", handler->address, handler->data.temp_main);
    } else {
        handler->status.error_count++;
        handler->status.online = false;
        printf("[POWER] 0x%02X Failed to read temperature (error %d)\n", handler->address, status);
    }
    
    return status;
}

hal_status_t power_module_read_relay_status(power_module_handler_t *handler) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    uint16_t relay1_status = 0, relay2_status = 0;
    hal_status_t status1 = comm_manager_modbus_read_holding_registers(
        handler->address, POWER_REG_RELAY1_STATUS, 1, &relay1_status);
    hal_status_t status2 = comm_manager_modbus_read_holding_registers(
        handler->address, POWER_REG_RELAY2_STATUS, 1, &relay2_status);
    
    if (status1 == HAL_STATUS_OK && status2 == HAL_STATUS_OK) {
        handler->data.relay1_status = (relay1_status != 0);
        handler->data.relay2_status = (relay2_status != 0);
        handler->data.last_update_ms = hal_get_timestamp_ms();
        handler->status.last_seen_ms = hal_get_timestamp_ms();
        handler->status.online = true;
        handler->status.error_count = 0;
        
        printf("[POWER] 0x%02X Relays: R1=%s R2=%s\n", 
               handler->address,
               handler->data.relay1_status ? "ON" : "OFF",
               handler->data.relay2_status ? "ON" : "OFF");
    } else {
        handler->status.error_count++;
        handler->status.online = false;
        printf("[POWER] 0x%02X Failed to read relay status (errors %d, %d)\n", 
               handler->address, status1, status2);
    }
    
    return (status1 == HAL_STATUS_OK && status2 == HAL_STATUS_OK) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t power_module_read_alarm_status(power_module_handler_t *handler) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    uint16_t alarm_status = 0;
    hal_status_t status = comm_manager_modbus_read_holding_registers(
        handler->address, POWER_REG_ALARM_STATUS, 1, &alarm_status);
    
    if (status == HAL_STATUS_OK) {
        handler->data.alarm_status = alarm_status;
        handler->data.last_update_ms = hal_get_timestamp_ms();
        handler->status.last_seen_ms = hal_get_timestamp_ms();
        handler->status.online = true;
        handler->status.error_count = 0;
        
        if (alarm_status != 0) {
            printf("[POWER] 0x%02X ALARM: %s\n", handler->address, 
                   power_module_get_alarm_description(alarm_status));
        }
    } else {
        handler->status.error_count++;
        handler->status.online = false;
        printf("[POWER] 0x%02X Failed to read alarm status (error %d)\n", handler->address, status);
    }
    
    return status;
}

hal_status_t power_module_read_all_data(power_module_handler_t *handler) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    printf("[POWER] Reading all data from module 0x%02X\n", handler->address);
    
    hal_status_t status_v = power_module_read_voltage(handler);
    hal_status_t status_i = power_module_read_current(handler);
    hal_status_t status_t = power_module_read_temperature(handler);
    hal_status_t status_r = power_module_read_relay_status(handler);
    hal_status_t status_a = power_module_read_alarm_status(handler);
    
    // Check if any read was successful
    if (status_v == HAL_STATUS_OK || status_i == HAL_STATUS_OK || 
        status_t == HAL_STATUS_OK || status_r == HAL_STATUS_OK || 
        status_a == HAL_STATUS_OK) {
        handler->status.online = true;
        handler->status.last_seen_ms = hal_get_timestamp_ms();

        // Safety guards: escalate faults based on configured thresholds
        // Voltage/current/temperature bounds are checked in power_module_check_alarms
        // Here we trigger system safety faults for critical conditions
        if (handler->data.current_main > handler->config.current_max) {
            safety_manager_trigger_fault(SAFETY_FAULT_OVERCURRENT);
        }
        if (handler->data.temp_main > handler->config.temp_max) {
            safety_manager_trigger_fault(SAFETY_FAULT_OVERTEMPERATURE);
        }
        return HAL_STATUS_OK;
    } else {
        handler->status.online = false;
        handler->status.timeout_count++;
        return HAL_STATUS_ERROR;
    }
}

hal_status_t power_module_set_relay1(power_module_handler_t *handler, bool on) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    if (!handler->config.relay1_enabled) return HAL_STATUS_ERROR;
    
    uint16_t control_value = on ? 1 : 0;
    hal_status_t status = comm_manager_modbus_write_single_register(
        handler->address, POWER_REG_RELAY1_CONTROL, control_value);
    
    if (status == HAL_STATUS_OK) {
        printf("[POWER] 0x%02X Relay 1 set to %s\n", handler->address, on ? "ON" : "OFF");
        handler->status.last_seen_ms = hal_get_timestamp_ms();
        handler->status.online = true;
    } else {
        printf("[POWER] 0x%02X Failed to set relay 1 (error %d)\n", handler->address, status);
        handler->status.error_count++;
    }
    
    return status;
}

hal_status_t power_module_set_relay2(power_module_handler_t *handler, bool on) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    if (!handler->config.relay2_enabled) return HAL_STATUS_ERROR;
    
    uint16_t control_value = on ? 1 : 0;
    hal_status_t status = comm_manager_modbus_write_single_register(
        handler->address, POWER_REG_RELAY2_CONTROL, control_value);
    
    if (status == HAL_STATUS_OK) {
        printf("[POWER] 0x%02X Relay 2 set to %s\n", handler->address, on ? "ON" : "OFF");
        handler->status.last_seen_ms = hal_get_timestamp_ms();
        handler->status.online = true;
    } else {
        printf("[POWER] 0x%02X Failed to set relay 2 (error %d)\n", handler->address, status);
        handler->status.error_count++;
    }
    
    return status;
}

hal_status_t power_module_set_relays(power_module_handler_t *handler, bool relay1_on, bool relay2_on) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    hal_status_t status1 = power_module_set_relay1(handler, relay1_on);
    hal_status_t status2 = power_module_set_relay2(handler, relay2_on);
    
    return (status1 == HAL_STATUS_OK && status2 == HAL_STATUS_OK) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

hal_status_t power_module_set_config(power_module_handler_t *handler, const power_module_config_t *config) {
    if (!handler || !config) return HAL_STATUS_INVALID_PARAMETER;
    
    printf("[POWER] 0x%02X Updating configuration\n", handler->address);
    
    // Validate configuration
    if (config->voltage_max <= config->voltage_min) {
        printf("[POWER] 0x%02X Invalid voltage range: max=%.1fV, min=%.1fV\n", 
               handler->address, config->voltage_max, config->voltage_min);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    if (config->current_max <= 0.0f || config->temp_max <= 0.0f) {
        printf("[POWER] 0x%02X Invalid thresholds: current=%.1fA, temp=%.1f°C\n", 
               handler->address, config->current_max, config->temp_max);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Apply configuration
    memcpy(&handler->config, config, sizeof(power_module_config_t));
    
    printf("[POWER] 0x%02X Configuration updated: Vmax=%.1fV, Vmin=%.1fV, Imax=%.1fA, Tmax=%.1f°C\n",
           handler->address, config->voltage_max, config->voltage_min, config->current_max, config->temp_max);
    
    return HAL_STATUS_OK;
}

hal_status_t power_module_get_config(power_module_handler_t *handler, power_module_config_t *config) {
    if (!handler || !config) return HAL_STATUS_INVALID_PARAMETER;
    
    memcpy(config, &handler->config, sizeof(power_module_config_t));
    return HAL_STATUS_OK;
}

hal_status_t power_module_get_status(power_module_handler_t *handler, power_module_status_t *status) {
    if (!handler || !status) return HAL_STATUS_INVALID_PARAMETER;
    
    memcpy(status, &handler->status, sizeof(power_module_status_t));
    return HAL_STATUS_OK;
}

hal_status_t power_module_get_data(power_module_handler_t *handler, power_module_data_t *data) {
    if (!handler || !data) return HAL_STATUS_INVALID_PARAMETER;
    
    memcpy(data, &handler->data, sizeof(power_module_data_t));
    return HAL_STATUS_OK;
}

bool power_module_check_alarms(power_module_handler_t *handler) {
    if (!handler) return false;
    
    bool has_alarm = false;
    
    // Check voltage alarms
    if (handler->data.voltage_main > handler->config.voltage_max) {
        handler->data.alarm_status |= POWER_ALARM_OVERVOLTAGE;
        has_alarm = true;
        printf("[POWER] 0x%02X OVERVOLTAGE ALARM: %.1fV > %.1fV\n", 
               handler->address, handler->data.voltage_main, handler->config.voltage_max);
    }
    
    if (handler->data.voltage_main < handler->config.voltage_min) {
        handler->data.alarm_status |= POWER_ALARM_UNDERVOLTAGE;
        has_alarm = true;
        printf("[POWER] 0x%02X UNDERVOLTAGE ALARM: %.1fV < %.1fV\n", 
               handler->address, handler->data.voltage_main, handler->config.voltage_min);
    }
    
    // Check current alarm
    if (handler->data.current_main > handler->config.current_max) {
        handler->data.alarm_status |= POWER_ALARM_OVERCURRENT;
        has_alarm = true;
        printf("[POWER] 0x%02X OVERCURRENT ALARM: %.1fA > %.1fA\n", 
               handler->address, handler->data.current_main, handler->config.current_max);
    }
    
    // Check temperature alarm
    if (handler->data.temp_main > handler->config.temp_max) {
        handler->data.alarm_status |= POWER_ALARM_OVERTEMP;
        has_alarm = true;
        printf("[POWER] 0x%02X OVERTEMP ALARM: %.1f°C > %.1f°C\n", 
               handler->address, handler->data.temp_main, handler->config.temp_max);
    }
    
    return has_alarm;
}

hal_status_t power_module_emergency_shutdown(power_module_handler_t *handler) {
    if (!handler) return HAL_STATUS_INVALID_PARAMETER;
    
    printf("[POWER] 0x%02X EMERGENCY SHUTDOWN\n", handler->address);
    
    // Turn off all relays
    hal_status_t status1 = power_module_set_relay1(handler, false);
    hal_status_t status2 = power_module_set_relay2(handler, false);
    
    // Set offline status
    handler->status.online = false;
    
    printf("[POWER] 0x%02X Emergency shutdown completed\n", handler->address);
    
    return (status1 == HAL_STATUS_OK && status2 == HAL_STATUS_OK) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

const char* power_module_get_alarm_description(uint16_t alarm_status) {
    static char alarm_desc[256];
    alarm_desc[0] = '\0';
    
    if (alarm_status == 0) {
        strcpy(alarm_desc, "No alarms");
        return alarm_desc;
    }
    
    if (alarm_status & POWER_ALARM_OVERVOLTAGE) strcat(alarm_desc, "OVERVOLTAGE ");
    if (alarm_status & POWER_ALARM_UNDERVOLTAGE) strcat(alarm_desc, "UNDERVOLTAGE ");
    if (alarm_status & POWER_ALARM_OVERCURRENT) strcat(alarm_desc, "OVERCURRENT ");
    if (alarm_status & POWER_ALARM_OVERTEMP) strcat(alarm_desc, "OVERTEMP ");
    if (alarm_status & POWER_ALARM_COMM_ERROR) strcat(alarm_desc, "COMM_ERROR ");
    if (alarm_status & POWER_ALARM_RELAY_FAULT) strcat(alarm_desc, "RELAY_FAULT ");
    
    return alarm_desc;
}

void power_module_print_status(const power_module_handler_t *handler) {
    if (!handler) return;
    
    printf("[POWER] Module 0x%02X Status:\n", handler->address);
    printf("  Initialized: %s\n", handler->status.initialized ? "YES" : "NO");
    printf("  Online: %s\n", handler->status.online ? "YES" : "NO");
    printf("  Error count: %u\n", handler->status.error_count);
    printf("  Timeout count: %u\n", handler->status.timeout_count);
    printf("  Last seen: %llu ms ago\n", 
           hal_get_timestamp_ms() - handler->status.last_seen_ms);
}

void power_module_print_data(const power_module_handler_t *handler) {
    if (!handler) return;
    
    printf("[POWER] Module 0x%02X Data:\n", handler->address);
    printf("  Voltage: %.1fV\n", handler->data.voltage_main);
    printf("  Current: %.1fA\n", handler->data.current_main);
    printf("  Temperature: %.1f°C\n", handler->data.temp_main);
    printf("  Relay 1: %s\n", handler->data.relay1_status ? "ON" : "OFF");
    printf("  Relay 2: %s\n", handler->data.relay2_status ? "ON" : "OFF");
    printf("  Alarm status: 0x%04X (%s)\n", handler->data.alarm_status, 
           power_module_get_alarm_description(handler->data.alarm_status));
    printf("  Last update: %llu ms ago\n", 
           hal_get_timestamp_ms() - handler->data.last_update_ms);
}
