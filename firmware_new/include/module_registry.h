/**
 * @file module_registry.h
 * @brief Dynamic module registry for OHT-50 (Phase 1)
 */

#ifndef MODULE_REGISTRY_H
#define MODULE_REGISTRY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Max modules supported in registry
#define MODULE_REGISTRY_MAX_MODULES 16

typedef enum {
    MODULE_TYPE_UNKNOWN = 0,
    MODULE_TYPE_POWER = 0x02,        // Power module (0x02)
    MODULE_TYPE_SAFETY = 0x03,       // Safety module (0x03)
    MODULE_TYPE_TRAVEL_MOTOR = 0x04, // Travel Motor module (0x04)
    MODULE_TYPE_DOCK = 0x05,         // Dock & Location module (0x05)
} module_type_t;

typedef enum {
    MODULE_STATUS_UNKNOWN = 0,
    MODULE_STATUS_ONLINE,
    MODULE_STATUS_OFFLINE,
} module_status_t;

// Power module capabilities
#define POWER_CAP_VOLTAGE_MONITOR     (1 << 0)
#define POWER_CAP_CURRENT_MONITOR     (1 << 1)
#define POWER_CAP_TEMP_MONITOR        (1 << 2)
#define POWER_CAP_RELAY_CONTROL       (1 << 3)
#define POWER_CAP_OVERVOLTAGE_PROTECT (1 << 4)
#define POWER_CAP_OVERCURRENT_PROTECT (1 << 5)
#define POWER_CAP_OVERTEMP_PROTECT    (1 << 6)

// Power module registers (Modbus)
#define POWER_REG_VOLTAGE_MAIN        0x0000  // Main voltage (V * 10)
#define POWER_REG_CURRENT_MAIN        0x0001  // Main current (A * 10)
#define POWER_REG_TEMP_MAIN           0x0002  // Main temperature (°C * 10)
#define POWER_REG_RELAY1_STATUS       0x0004  // Relay 1 status (0=OFF, 1=ON)
#define POWER_REG_RELAY2_STATUS       0x0005  // Relay 2 status (0=OFF, 1=ON)
#define POWER_REG_RELAY1_CONTROL      0x0006  // Relay 1 control (0=OFF, 1=ON)
#define POWER_REG_RELAY2_CONTROL      0x0007  // Relay 2 control (0=OFF, 1=ON)
#define POWER_REG_ALARM_STATUS        0x0008  // Alarm status (bitmap)
#define POWER_REG_DEVICE_ID           0x00F0  // Device ID
#define POWER_REG_MODULE_TYPE         0x00F7  // Module type (0x0002 = Power)

typedef struct {
    uint8_t address;                 // Modbus/RS485 address (e.g., 0x02)
    module_type_t type;              // Module type
    char name[32];                   // Friendly name
    module_status_t status;          // Online/Offline
    uint64_t last_seen_ms;           // Last seen timestamp (ms)
    uint32_t capabilities;           // Bitmask (future use)
    char version[16];                // Optional version string
} module_info_t;

// Module events
typedef enum {
    MODULE_EVENT_DISCOVERED = 0,
    MODULE_EVENT_ONLINE,
    MODULE_EVENT_OFFLINE,
    MODULE_EVENT_UPDATED,
    MODULE_EVENT_TIMEOUT,
} module_event_t;

typedef void (*module_event_callback_t)(module_event_t event, const module_info_t *info);

// Registry API
int registry_init(void);
int registry_deinit(void);
int registry_clear(void);

int registry_add_or_update(const module_info_t *info);
int registry_mark_online(uint8_t address, module_type_t type, const char *version);
int registry_mark_offline(uint8_t address);
int registry_set_meta(uint8_t address, const char *name, module_type_t type);

int registry_get(uint8_t address, module_info_t *out);
int registry_get_all(module_info_t *out_array, size_t max_items, size_t *actual_count);
size_t registry_list(module_info_t *out_array, size_t max_items);
size_t registry_count_online(void);
bool registry_has_offline_saved(void);

// CTO Requirements: Mandatory slave modules management
size_t registry_count_mandatory_online(void);
bool registry_all_mandatory_online(void);
int registry_get_missing_mandatory(uint8_t *missing_modules, size_t max_count, size_t *actual_count);

void registry_set_event_callback(module_event_callback_t cb);

// Persistence (simple YAML-like)
int registry_load_yaml(const char *path);
int registry_save_yaml(const char *path);

// Scan state (for LED policy)
void registry_set_scanning(bool scanning);
bool registry_is_scanning(void);

#endif // MODULE_REGISTRY_H


