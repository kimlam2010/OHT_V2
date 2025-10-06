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

// Power Module Register Map (Simplified)
static const register_info_t power_module_register_map[] = {
    {POWER_REG_BATTERY_VOLTAGE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Battery Pack Voltage", "V", 0, 65535, 0},
    {POWER_REG_BATTERY_CURRENT, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Battery Pack Current", "A", 0, 65535, 0},
    {POWER_REG_BATTERY_SOC, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "State of Charge", "%", 0, 1000, 0}, // 0-100.0%
    {POWER_REG_MAX_CELL_V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Maximum Cell Voltage", "mV", 0, 65535, 0},
    {POWER_REG_MIN_CELL_V, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Minimum Cell Voltage", "mV", 0, 65535, 0},
    {POWER_REG_TEMPERATURE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Average Temperature", "C", 0, 200, 0}, // -50 to 150C
    {POWER_REG_CONNECTION_STATUS, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "BMS Connection Status", "bool", 0, 1, 0},
    {POWER_REG_CHARGE_MOS, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, false, "Charge MOSFET Status", "bool", 0, 1, 0},
    {POWER_REG_DISCHARGE_MOS, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT8, REG_ACCESS_ADMIN, false, "Discharge MOSFET Status", "bool", 0, 1, 0},
    {POWER_REG_DEVICE_ID, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_SYSTEM, false, "Device ID", "N/A", 0, 65535, 0},
    {POWER_REG_CONFIG_BAUDRATE, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_SYSTEM, false, "Config Baudrate", "enum", 1, 5, 1}, // 1=9600, 5=115200
    {POWER_REG_RESET_ERROR_CMD, REG_MODE_WRITE_ONCE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, false, "Reset Error Command", "cmd", 0, 1, 0},
    // End marker
    {0xFFFF, 0, 0, 0, false, NULL, NULL, 0, 0, 0}
};
static const size_t power_module_register_map_size = sizeof(power_module_register_map) / sizeof(power_module_register_map[0]);

// Safety Module Register Map (Simplified)
static const register_info_t safety_module_register_map[] = {
    {SAFETY_STATUS_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, true, "Safety System Status", "flags", 0, 65535, 0},
    {SAFETY_EMERGENCY_STOP_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, true, "Emergency Stop Status", "bool", 0, 1, 0},
    {SAFETY_SAFETY_ZONE_REG, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, true, "Safety Zone Status", "enum", 0, 3, 0},
    {SAFETY_ZONE_1_THRESHOLD_REG, REG_MODE_READ_WRITE, REG_DATA_TYPE_UINT16, REG_ACCESS_ADMIN, true, "Zone 1 Threshold", "mm", 0, 10000, 1000},
    // End marker
    {0xFFFF, 0, 0, 0, false, NULL, NULL, 0, 0, 0}
};
static const size_t safety_module_register_map_size = sizeof(safety_module_register_map) / sizeof(safety_module_register_map[0]);

// Travel Motor Module Register Map (Simplified)
static const register_info_t travel_motor_module_register_map[] = {
    {0x0000, REG_MODE_READ_WRITE, REG_DATA_TYPE_INT16, REG_ACCESS_USER, false, "Motor Speed", "RPM", 0, 3000, 0},
    {0x0001, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT32, REG_ACCESS_USER, false, "Motor Position", "counts", 0, 0xFFFF, 0},
    // End marker
    {0xFFFF, 0, 0, 0, false, NULL, NULL, 0, 0, 0}
};
static const size_t travel_motor_module_register_map_size = sizeof(travel_motor_module_register_map) / sizeof(travel_motor_module_register_map[0]);

// Dock Module Register Map (Simplified)
static const register_info_t dock_module_register_map[] = {
    {DOCK_REG_DOCKING_STATUS, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT8, REG_ACCESS_USER, false, "Docking Status", "enum", 0, 3, 0},
    {DOCK_REG_DOCKING_DISTANCE, REG_MODE_READ_ONLY, REG_DATA_TYPE_UINT16, REG_ACCESS_USER, false, "Distance to Dock", "mm", 0, 5000, 0},
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

    for (size_t i = 0; i < map_size; i++) {
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