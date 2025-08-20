#include "module_registry.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static module_info_t g_modules[MODULE_REGISTRY_MAX_MODULES];
static size_t g_count = 0;
static module_event_callback_t g_event_cb = NULL;
static bool g_scanning = false;

static void emit(module_event_t ev, const module_info_t *info) {
    if (g_event_cb) g_event_cb(ev, info);
}

int registry_init(void) {
    memset(g_modules, 0, sizeof(g_modules));
    g_count = 0;
    g_event_cb = NULL;
    g_scanning = false;
    return 0;
}

int registry_deinit(void) { return 0; }
int registry_clear(void) { g_count = 0; return 0; }

static int find_index(uint8_t address) {
    for (size_t i = 0; i < g_count; ++i) {
        if (g_modules[i].address == address) return (int)i;
    }
    return -1;
}

int registry_add_or_update(const module_info_t *info) {
    if (!info) return -1;
    int idx = find_index(info->address);
    if (idx < 0) {
        if (g_count >= MODULE_REGISTRY_MAX_MODULES) return -1;
        g_modules[g_count] = *info;
        emit(MODULE_EVENT_DISCOVERED, &g_modules[g_count]);
        g_count++;
    } else {
        g_modules[idx] = *info;
        emit(MODULE_EVENT_UPDATED, &g_modules[idx]);
    }
    return 0;
}

int registry_mark_online(uint8_t address, module_type_t type, const char *version) {
    int idx = find_index(address);
    if (idx < 0) {
        module_info_t mi = {0};
        mi.address = address;
        mi.type = type;
        mi.status = MODULE_STATUS_ONLINE;
        mi.last_seen_ms = hal_get_timestamp_ms();
        if (version) strncpy(mi.version, version, sizeof(mi.version)-1);
        strncpy(mi.name, "module", sizeof(mi.name)-1);
        registry_add_or_update(&mi);
        idx = find_index(address);
    }
    if (idx < 0) return -1;
    g_modules[idx].status = MODULE_STATUS_ONLINE;
    g_modules[idx].last_seen_ms = hal_get_timestamp_ms();
    if (version) strncpy(g_modules[idx].version, version, sizeof(g_modules[idx].version)-1);
    emit(MODULE_EVENT_ONLINE, &g_modules[idx]);
    return 0;
}

int registry_mark_offline(uint8_t address) {
    int idx = find_index(address);
    if (idx < 0) return -1;
    g_modules[idx].status = MODULE_STATUS_OFFLINE;
    emit(MODULE_EVENT_OFFLINE, &g_modules[idx]);
    return 0;
}

int registry_set_meta(uint8_t address, const char *name, module_type_t type) {
    int idx = find_index(address);
    if (idx < 0) return -1;
    if (name) strncpy(g_modules[idx].name, name, sizeof(g_modules[idx].name)-1);
    if (type != MODULE_TYPE_UNKNOWN) g_modules[idx].type = type;
    emit(MODULE_EVENT_UPDATED, &g_modules[idx]);
    return 0;
}

int registry_get(uint8_t address, module_info_t *out) {
    if (!out) return -1;
    int idx = find_index(address);
    if (idx < 0) return -1;
    *out = g_modules[idx];
    return 0;
}

size_t registry_list(module_info_t *out_array, size_t max_items) {
    if (!out_array || max_items == 0) return g_count;
    size_t n = (g_count < max_items) ? g_count : max_items;
    memcpy(out_array, g_modules, n * sizeof(module_info_t));
    return n;
}

int registry_get_all(module_info_t *out_array, size_t max_items, size_t *actual_count) {
    if (!out_array || !actual_count) return -1;
    size_t n = (g_count < max_items) ? g_count : max_items;
    memcpy(out_array, g_modules, n * sizeof(module_info_t));
    *actual_count = g_count;
    return 0;
}

void registry_set_event_callback(module_event_callback_t cb) { g_event_cb = cb; }

size_t registry_count_online(void) {
    size_t n = 0;
    for (size_t i = 0; i < g_count; ++i) if (g_modules[i].status == MODULE_STATUS_ONLINE) ++n;
    return n;
}

bool registry_has_offline_saved(void) {
    for (size_t i = 0; i < g_count; ++i) if (g_modules[i].status == MODULE_STATUS_OFFLINE) return true;
    return false;
}

// Simple YAML-like persistence (very basic; one module per line)
int registry_load_yaml(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    
    // Clear current registry but keep event callback
    module_event_callback_t saved_cb = g_event_cb;
    registry_clear();
    g_event_cb = saved_cb;
    
    char line[256];
    
    while (fgets(line, sizeof(line), fp)) {
        // Skip empty lines and comments
        if (line[0] == '\n' || line[0] == '#') continue;
        
        // Check for version header
        if (strncmp(line, "version:", 8) == 0) {
            continue;
        }
        
        // Parse module line: addr:0x02,type:motor,name:motor_main,version:1.0,status:offline
        module_info_t mi = {0};
        char addr_str[8] = {0};
        char type_str[16] = {0};
        char name_str[32] = {0};
        char ver_str[16] = {0};
        char status_str[16] = {0};
        
        if (sscanf(line, "addr:%7[^,],type:%15[^,],name:%31[^,],version:%15[^,],status:%15s", 
                   addr_str, type_str, name_str, ver_str, status_str) >= 3) {
            mi.address = (uint8_t)strtoul(addr_str, NULL, 0);
            
            // Parse module type
            if (strcmp(type_str, "motor") == 0) mi.type = MODULE_TYPE_MOTOR;
            else if (strcmp(type_str, "io") == 0) mi.type = MODULE_TYPE_IO;
            else if (strcmp(type_str, "dock") == 0) mi.type = MODULE_TYPE_DOCK;
            else if (strcmp(type_str, "sensor") == 0) mi.type = MODULE_TYPE_SENSOR;
            else if (strcmp(type_str, "power") == 0) mi.type = MODULE_TYPE_POWER;
            else mi.type = MODULE_TYPE_UNKNOWN;
            
            strncpy(mi.name, name_str, sizeof(mi.name)-1);
            strncpy(mi.version, ver_str, sizeof(mi.version)-1);
            
            // Parse status
            if (strcmp(status_str, "online") == 0) mi.status = MODULE_STATUS_ONLINE;
            else mi.status = MODULE_STATUS_OFFLINE;
            
            // Set last_seen to 0 for offline modules (will be updated when discovered)
            mi.last_seen_ms = 0;
            
            registry_add_or_update(&mi);
        }
    }
    fclose(fp);
    return 0;
}

int registry_save_yaml(const char *path) {
    FILE *fp = fopen(path, "w");
    if (!fp) return -1;
    
    // Write version header
    fprintf(fp, "# OHT-50 Module Registry\n");
    fprintf(fp, "# Generated: %s\n", "2025-01-28T00:00:00Z"); // TODO: add timestamp
    fprintf(fp, "version: 1\n\n");
    
    // Write modules
    for (size_t i = 0; i < g_count; ++i) {
        const char *type = "unknown";
        if (g_modules[i].type == MODULE_TYPE_MOTOR) type = "motor";
        else if (g_modules[i].type == MODULE_TYPE_IO) type = "io";
        else if (g_modules[i].type == MODULE_TYPE_DOCK) type = "dock";
        else if (g_modules[i].type == MODULE_TYPE_SENSOR) type = "sensor";
        else if (g_modules[i].type == MODULE_TYPE_POWER) type = "power";
        
        const char *st = (g_modules[i].status == MODULE_STATUS_ONLINE) ? "online" : "offline";
        fprintf(fp, "addr:0x%02X,type:%s,name:%s,version:%s,status:%s\n",
                g_modules[i].address, type, g_modules[i].name, g_modules[i].version, st);
    }
    fclose(fp);
    return 0;
}

void registry_set_scanning(bool scanning) { g_scanning = scanning; }
bool registry_is_scanning(void) { return g_scanning; }


