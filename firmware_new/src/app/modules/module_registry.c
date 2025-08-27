#include "module_registry.h"
#include "hal_common.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static module_info_t g_modules[MODULE_REGISTRY_MAX_MODULES];
static size_t g_count = 0;
static module_event_callback_t g_event_cb = NULL;
static bool g_scanning = false;

// Registry-specific callback type (for backward compatibility)
typedef void (*registry_event_callback_t)(module_event_t event, uint8_t address, const module_info_t *info);
static registry_event_callback_t g_registry_cb = NULL;

static void emit(module_event_t ev, const module_info_t *info) {
    if (g_event_cb) {
        g_event_cb(ev, info);
    }
    if (g_registry_cb) {
        g_registry_cb(ev, info->address, info);
    }
}

int registry_init(void) {
    memset(g_modules, 0, sizeof(g_modules));
    g_count = 0;
    g_event_cb = NULL;
    g_registry_cb = NULL;
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

int registry_get_all(module_info_t *out_array, size_t max_items, size_t *actual_count) {
    if (!out_array || !actual_count) return -1;
    size_t n = (g_count < max_items) ? g_count : max_items;
    memcpy(out_array, g_modules, n * sizeof(module_info_t));
    *actual_count = n;
    return 0;
}

size_t registry_list(module_info_t *out_array, size_t max_items) {
    if (!out_array || max_items == 0) return g_count;
    size_t n = (g_count < max_items) ? g_count : max_items;
    memcpy(out_array, g_modules, n * sizeof(module_info_t));
    return n;
}

size_t registry_count_online(void) {
    size_t count = 0;
    for (size_t i = 0; i < g_count; ++i) {
        if (g_modules[i].status == MODULE_STATUS_ONLINE) {
            count++;
        }
    }
    return count;
}

// CTO Requirements: Count mandatory slave modules online (addresses 0x02-0x05)
size_t registry_count_mandatory_online(void) {
    size_t count = 0;
    for (size_t i = 0; i < g_count; ++i) {
        if (g_modules[i].status == MODULE_STATUS_ONLINE) {
            // Check if this is a mandatory slave module (address 0x02-0x05)
            if (g_modules[i].address >= MANDATORY_MODULE_ADDR_START && 
                g_modules[i].address <= MANDATORY_MODULE_ADDR_END) {
                count++;
            }
        }
    }
    return count;
}

// CTO Requirements: Check if all mandatory slave modules are online
bool registry_all_mandatory_online(void) {
    return registry_count_mandatory_online() == MANDATORY_MODULES_COUNT;
}

// CTO Requirements: Get list of missing mandatory slave modules
int registry_get_missing_mandatory(uint8_t *missing_modules, size_t max_count, size_t *actual_count) {
    if (!missing_modules || !actual_count) return -1;
    
    *actual_count = 0;
    bool found_mandatory[MANDATORY_MODULES_COUNT] = {false};
    
    // Mark found mandatory slave modules
    for (size_t i = 0; i < g_count; ++i) {
        if (g_modules[i].status == MODULE_STATUS_ONLINE) {
            if (g_modules[i].address >= MANDATORY_MODULE_ADDR_START && 
                g_modules[i].address <= MANDATORY_MODULE_ADDR_END) {
                size_t idx = g_modules[i].address - MANDATORY_MODULE_ADDR_START;
                if (idx < MANDATORY_MODULES_COUNT) {
                    found_mandatory[idx] = true;
                }
            }
        }
    }
    
    // Find missing mandatory slave modules
    for (size_t i = 0; i < MANDATORY_MODULES_COUNT && *actual_count < max_count; ++i) {
        if (!found_mandatory[i]) {
            missing_modules[*actual_count] = MANDATORY_MODULE_ADDR_START + i;
            (*actual_count)++;
        }
    }
    
    return 0;
}

bool registry_has_offline_saved(void) {
    for (size_t i = 0; i < g_count; ++i) {
        if (g_modules[i].status == MODULE_STATUS_OFFLINE) {
            return true;
        }
    }
    return false;
}

void registry_set_event_callback(module_event_callback_t cb) {
    g_event_cb = cb;
}

// Legacy callback function for backward compatibility
void registry_set_legacy_callback(registry_event_callback_t cb) {
    g_registry_cb = cb;
}

int registry_load_yaml(const char *path) {
    // TODO: Implement YAML loading
    (void)path;
    return 0;
}

int registry_save_yaml(const char *path) {
    // TODO: Implement YAML saving
    (void)path;
    return 0;
}

void registry_set_scanning(bool scanning) {
    g_scanning = scanning;
}

bool registry_is_scanning(void) {
    return g_scanning;
}
