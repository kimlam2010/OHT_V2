/**
 * @file module_polling_manager.h
 * @brief Module Polling Manager for OHT-50
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 * 
 * Manages dynamic polling of all discovered modules based on their types
 */

#ifndef MODULE_POLLING_MANAGER_H
#define MODULE_POLLING_MANAGER_H

#include "hal_common.h"
#include "module_manager.h"

// Module polling intervals (in milliseconds)
#define POLLING_INTERVAL_POWER_MS     1000    // 1 second - High priority: Critical battery data
#define POLLING_INTERVAL_POWER_MEDIUM_MS 5000    // 5 seconds - Medium priority: Charging & power distribution
#define POLLING_INTERVAL_POWER_LOW_MS    30000   // 30 seconds - Low priority: System information
#define POLLING_INTERVAL_MOTOR_MS     1000    // 1 second  
#define POLLING_INTERVAL_SENSOR_MS    500     // 0.5 seconds
#define POLLING_INTERVAL_LIDAR_MS     100     // 0.1 seconds
#define POLLING_INTERVAL_UNKNOWN_MS   2000    // 2 seconds

// Use existing module_type_t from module_manager.h
typedef module_type_t module_polling_type_t;

// Module polling data structure
typedef struct {
    uint8_t address;
    module_polling_type_t type;
    uint64_t last_poll_ms;
    uint32_t poll_interval_ms;
    bool is_online;
    bool handler_initialized;
} module_polling_info_t;

// Module polling manager structure
typedef struct {
    module_polling_info_t modules[256];  // Use 256 instead of MODULE_ADDR_MAX + 1
    uint32_t total_modules;
    bool initialized;
} module_polling_manager_t;

// Function prototypes
hal_status_t module_polling_manager_init(void);
hal_status_t module_polling_manager_update(void);
hal_status_t module_polling_manager_add_module(uint8_t address, module_polling_type_t type);
hal_status_t module_polling_manager_remove_module(uint8_t address);
hal_status_t module_polling_manager_poll_module(uint8_t address);
hal_status_t module_polling_manager_get_module_info(uint8_t address, module_polling_info_t *info);

// Module-specific polling functions
hal_status_t module_polling_power_module(uint8_t address);
hal_status_t module_polling_motor_module(uint8_t address);
hal_status_t module_polling_sensor_module(uint8_t address);
hal_status_t module_polling_lidar_module(uint8_t address);
hal_status_t module_polling_unknown_module(uint8_t address);

// Utility functions
const char* module_polling_type_to_string(module_polling_type_t type);
uint32_t module_polling_get_interval(module_polling_type_t type);

#endif // MODULE_POLLING_MANAGER_H
