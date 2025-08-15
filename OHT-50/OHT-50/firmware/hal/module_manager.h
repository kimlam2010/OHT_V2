#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "board_config.h"

// Module configuration structure
typedef struct {
    uint8_t address;           // RS485 address (0x02-0x07)
    char name[32];            // Module name
    uint8_t type;             // Module type (POWER=1, MOTOR=2, DIO=3, etc.)
    uint8_t version;          // Firmware version
    bool enabled;             // Module enabled/disabled
    uint16_t timeout_ms;      // Communication timeout
    uint8_t retry_count;      // Retry attempts
} module_config_t;

// Module function pointers
typedef struct {
    bool (*init)(uint8_t addr);
    bool (*ping)(uint8_t addr);
    bool (*get_info)(uint8_t addr, module_config_t *info);
    bool (*read_data)(uint8_t addr, uint8_t *data, uint16_t *len);
    bool (*write_data)(uint8_t addr, uint8_t *data, uint16_t len);
    bool (*reset)(uint8_t addr);
} module_ops_t;

// Module registry entry
typedef struct {
    module_config_t config;
    module_ops_t ops;
    bool registered;
} module_registry_t;

// Module types
#define MODULE_TYPE_POWER     1
#define MODULE_TYPE_STEPPER   2
#define MODULE_TYPE_DC_MOTOR  3
#define MODULE_TYPE_DIO       4
#define MODULE_TYPE_AI        5
#define MODULE_TYPE_LOCATION  6

// Function prototypes
bool module_manager_init(void);
bool module_register(uint8_t addr, const char *name, uint8_t type, const module_ops_t *ops);
bool module_unregister(uint8_t addr);
bool module_enable(uint8_t addr);
bool module_disable(uint8_t addr);
module_registry_t* module_get(uint8_t addr);
bool module_ping_all(void);
bool module_load_config(const char *config_file);
bool module_save_config(const char *config_file);

// Auto-discovery functions
bool module_discover(void);
bool module_auto_register(uint8_t addr);

// Configuration file format: JSON
// {
//   "modules": [
//     {
//       "address": "0x02",
//       "name": "Power Module",
//       "type": 1,
//       "enabled": true,
//       "timeout_ms": 1000,
//       "retry_count": 3
//     }
//   ]
// }

#endif // MODULE_MANAGER_H
