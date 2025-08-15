#include "module_manager.h"
#include "hal_rs485.h"
#include <string.h>
#include <stdio.h>

#define MAX_MODULES 16
#define CONFIG_FILE_PATH "/etc/oht50/modules.json"

static module_registry_t module_registry[MAX_MODULES];
static bool manager_initialized = false;

// Default module operations for unknown modules
static bool default_init(uint8_t addr) { return true; }
static bool default_ping(uint8_t addr) { 
    uint8_t cmd[] = {0xAA, addr, 0x01, 0x00, 0x00, 0x00}; // PING command
    return rs485_transmit(cmd, sizeof(cmd));
}
static bool default_get_info(uint8_t addr, module_config_t *info) {
    uint8_t cmd[] = {0xAA, addr, 0x02, 0x00, 0x00, 0x00}; // GET_INFO command
    uint8_t response[64];
    uint16_t len = sizeof(response);
    
    if (rs485_transmit(cmd, sizeof(cmd)) && rs485_receive(response, &len)) {
        // Parse response and fill info
        info->address = addr;
        strcpy(info->name, "Unknown Module");
        info->type = 0;
        info->version = 1;
        info->enabled = true;
        info->timeout_ms = 1000;
        info->retry_count = 3;
        return true;
    }
    return false;
}
static bool default_read_data(uint8_t addr, uint8_t *data, uint16_t *len) { return false; }
static bool default_write_data(uint8_t addr, uint8_t *data, uint16_t len) { return false; }
static bool default_reset(uint8_t addr) { return true; }

static module_ops_t default_ops = {
    .init = default_init,
    .ping = default_ping,
    .get_info = default_get_info,
    .read_data = default_read_data,
    .write_data = default_write_data,
    .reset = default_reset
};

bool module_manager_init(void) {
    if (manager_initialized) return true;
    
    // Initialize RS485
    if (!rs485_init()) {
        return false;
    }
    
    // Clear registry
    memset(module_registry, 0, sizeof(module_registry));
    
    // Load configuration
    module_load_config(CONFIG_FILE_PATH);
    
    // Auto-discover modules
    module_discover();
    
    manager_initialized = true;
    return true;
}

bool module_register(uint8_t addr, const char *name, uint8_t type, const module_ops_t *ops) {
    if (addr >= MAX_MODULES) return false;
    
    module_registry[addr].config.address = addr;
    strncpy(module_registry[addr].config.name, name, sizeof(module_registry[addr].config.name) - 1);
    module_registry[addr].config.type = type;
    module_registry[addr].config.enabled = true;
    module_registry[addr].config.timeout_ms = 1000;
    module_registry[addr].config.retry_count = 3;
    
    if (ops) {
        module_registry[addr].ops = *ops;
    } else {
        module_registry[addr].ops = default_ops;
    }
    
    module_registry[addr].registered = true;
    
    // Save configuration
    module_save_config(CONFIG_FILE_PATH);
    
    return true;
}

bool module_unregister(uint8_t addr) {
    if (addr >= MAX_MODULES) return false;
    
    module_registry[addr].registered = false;
    memset(&module_registry[addr], 0, sizeof(module_registry_t));
    
    // Save configuration
    module_save_config(CONFIG_FILE_PATH);
    
    return true;
}

bool module_enable(uint8_t addr) {
    if (addr >= MAX_MODULES || !module_registry[addr].registered) return false;
    
    module_registry[addr].config.enabled = true;
    module_save_config(CONFIG_FILE_PATH);
    return true;
}

bool module_disable(uint8_t addr) {
    if (addr >= MAX_MODULES || !module_registry[addr].registered) return false;
    
    module_registry[addr].config.enabled = false;
    module_save_config(CONFIG_FILE_PATH);
    return true;
}

module_registry_t* module_get(uint8_t addr) {
    if (addr >= MAX_MODULES || !module_registry[addr].registered) return NULL;
    return &module_registry[addr];
}

bool module_ping_all(void) {
    bool all_ok = true;
    
    for (int i = 0; i < MAX_MODULES; i++) {
        if (module_registry[i].registered && module_registry[i].config.enabled) {
            if (!module_registry[i].ops.ping(i)) {
                printf("Module 0x%02X (%s) not responding\n", i, module_registry[i].config.name);
                all_ok = false;
            }
        }
    }
    
    return all_ok;
}

bool module_discover(void) {
    printf("Discovering modules...\n");
    
    for (uint8_t addr = 0x02; addr <= 0x07; addr++) {
        if (module_auto_register(addr)) {
            printf("Found module at address 0x%02X\n", addr);
        }
    }
    
    return true;
}

bool module_auto_register(uint8_t addr) {
    // Try to ping the module
    uint8_t cmd[] = {0xAA, addr, 0x01, 0x00, 0x00, 0x00}; // PING command
    uint8_t response[64];
    uint16_t len = sizeof(response);
    
    if (rs485_transmit(cmd, sizeof(cmd)) && rs485_receive(response, &len)) {
        // Module responded, try to get info
        module_config_t info;
        if (default_get_info(addr, &info)) {
            // Auto-register with default operations
            return module_register(addr, info.name, info.type, NULL);
        }
    }
    
    return false;
}

bool module_load_config(const char *config_file) {
    // Simple JSON parser for module configuration
    FILE *fp = fopen(config_file, "r");
    if (!fp) {
        printf("No module config file found, using defaults\n");
        return false;
    }
    
    // TODO: Implement JSON parsing
    // For now, just close the file
    fclose(fp);
    
    return true;
}

bool module_save_config(const char *config_file) {
    // Create directory if it doesn't exist
    system("mkdir -p /etc/oht50");
    
    FILE *fp = fopen(config_file, "w");
    if (!fp) {
        printf("Cannot write module config file\n");
        return false;
    }
    
    // Write JSON configuration
    fprintf(fp, "{\n  \"modules\": [\n");
    
    bool first = true;
    for (int i = 0; i < MAX_MODULES; i++) {
        if (module_registry[i].registered) {
            if (!first) fprintf(fp, ",\n");
            fprintf(fp, "    {\n");
            fprintf(fp, "      \"address\": \"0x%02X\",\n", i);
            fprintf(fp, "      \"name\": \"%s\",\n", module_registry[i].config.name);
            fprintf(fp, "      \"type\": %d,\n", module_registry[i].config.type);
            fprintf(fp, "      \"enabled\": %s,\n", module_registry[i].config.enabled ? "true" : "false");
            fprintf(fp, "      \"timeout_ms\": %d,\n", module_registry[i].config.timeout_ms);
            fprintf(fp, "      \"retry_count\": %d\n", module_registry[i].config.retry_count);
            fprintf(fp, "    }");
            first = false;
        }
    }
    
    fprintf(fp, "\n  ]\n}\n");
    fclose(fp);
    
    return true;
}
