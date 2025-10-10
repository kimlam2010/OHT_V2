/**
 * @file register_value_cache.c
 * @brief Register Value Cache System Implementation
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include "register_value_cache.h"
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

// Global cache storage (indexed by module address 0-255)
static module_register_cache_t g_module_cache[256];

// Thread safety mutex
static pthread_mutex_t g_cache_mutex = PTHREAD_MUTEX_INITIALIZER;

// Cache statistics
static cache_statistics_t g_cache_stats = {0};

// Cache initialized flag
static bool g_cache_initialized = false;

// Helper function to get current timestamp in milliseconds
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000ULL + (uint64_t)(tv.tv_usec) / 1000ULL;
}

// Helper function to find register entry in cache
static register_value_entry_t* find_register_entry(module_register_cache_t *cache, uint16_t reg_addr) {
    for (uint16_t i = 0; i < cache->register_count; i++) {
        if (cache->registers[i].address == reg_addr && cache->registers[i].valid) {
            return &cache->registers[i];
        }
    }
    return NULL;
}

hal_status_t register_cache_init(void) {
    pthread_mutex_lock(&g_cache_mutex);
    
    // Initialize all module caches
    memset(g_module_cache, 0, sizeof(g_module_cache));
    for (int i = 0; i < 256; i++) {
        g_module_cache[i].module_address = (uint8_t)i;
        g_module_cache[i].initialized = false;
        g_module_cache[i].register_count = 0;
        g_module_cache[i].last_update_ms = 0;
    }
    
    // Reset statistics
    memset(&g_cache_stats, 0, sizeof(g_cache_stats));
    
    g_cache_initialized = true;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    printf("[CACHE] Register value cache initialized\n");
    return HAL_STATUS_OK;
}

hal_status_t register_cache_deinit(void) {
    pthread_mutex_lock(&g_cache_mutex);
    
    g_cache_initialized = false;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    printf("[CACHE] Register value cache deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t register_cache_store(uint8_t module_addr, uint16_t reg_addr, uint16_t value) {
    if (!g_cache_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_cache_mutex);
    
    module_register_cache_t *cache = &g_module_cache[module_addr];
    uint64_t timestamp = get_timestamp_ms();
    
    // Initialize module cache if needed
    if (!cache->initialized) {
        cache->initialized = true;
        cache->register_count = 0;
    }
    
    // Find existing entry or create new one
    register_value_entry_t *entry = find_register_entry(cache, reg_addr);
    
    if (entry == NULL) {
        // Create new entry
        if (cache->register_count >= MAX_CACHED_REGISTERS_PER_MODULE) {
            pthread_mutex_unlock(&g_cache_mutex);
            printf("[CACHE] Warning: Cache full for module 0x%02X\n", module_addr);
            return HAL_STATUS_ERROR;
        }
        
        entry = &cache->registers[cache->register_count];
        cache->register_count++;
    }
    
    // Update entry
    entry->address = reg_addr;
    entry->value = value;
    entry->timestamp_ms = timestamp;
    entry->valid = true;
    
    // Update module timestamp
    cache->last_update_ms = timestamp;
    
    // Update statistics
    g_cache_stats.total_stores++;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t register_cache_store_batch(uint8_t module_addr, uint16_t start_addr, 
                                        const uint16_t *values, uint16_t count) {
    if (!g_cache_initialized || values == NULL || count == 0) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Store each value individually (could be optimized but this is simple and thread-safe)
    for (uint16_t i = 0; i < count; i++) {
        hal_status_t status = register_cache_store(module_addr, start_addr + i, values[i]);
        if (status != HAL_STATUS_OK) {
            return status;
        }
    }
    
    return HAL_STATUS_OK;
}

hal_status_t register_cache_get(uint8_t module_addr, uint16_t reg_addr, 
                                uint16_t *value, uint64_t *timestamp_ms) {
    if (!g_cache_initialized || value == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_cache_mutex);
    
    module_register_cache_t *cache = &g_module_cache[module_addr];
    
    g_cache_stats.total_reads++;
    
    if (!cache->initialized) {
        g_cache_stats.cache_misses++;
        pthread_mutex_unlock(&g_cache_mutex);
        return HAL_STATUS_NOT_FOUND;
    }
    
    register_value_entry_t *entry = find_register_entry(cache, reg_addr);
    
    if (entry == NULL) {
        g_cache_stats.cache_misses++;
        pthread_mutex_unlock(&g_cache_mutex);
        return HAL_STATUS_NOT_FOUND;
    }
    
    // Found entry
    *value = entry->value;
    if (timestamp_ms != NULL) {
        *timestamp_ms = entry->timestamp_ms;
    }
    
    g_cache_stats.cache_hits++;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t register_cache_get_all(uint8_t module_addr, register_value_entry_t *entries, 
                                    uint16_t max_entries, uint16_t *count) {
    if (!g_cache_initialized || entries == NULL || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_cache_mutex);
    
    module_register_cache_t *cache = &g_module_cache[module_addr];
    
    if (!cache->initialized) {
        *count = 0;
        pthread_mutex_unlock(&g_cache_mutex);
        return HAL_STATUS_OK;
    }
    
    uint16_t num_entries = cache->register_count < max_entries ? cache->register_count : max_entries;
    
    // Copy valid entries
    uint16_t copied = 0;
    for (uint16_t i = 0; i < cache->register_count && copied < num_entries; i++) {
        if (cache->registers[i].valid) {
            entries[copied] = cache->registers[i];
            copied++;
        }
    }
    
    *count = copied;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t register_cache_clear_module(uint8_t module_addr) {
    if (!g_cache_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_cache_mutex);
    
    module_register_cache_t *cache = &g_module_cache[module_addr];
    
    memset(cache->registers, 0, sizeof(cache->registers));
    cache->register_count = 0;
    cache->last_update_ms = 0;
    cache->initialized = false;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    printf("[CACHE] Cleared cache for module 0x%02X\n", module_addr);
    return HAL_STATUS_OK;
}

hal_status_t register_cache_clear_all(void) {
    if (!g_cache_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    pthread_mutex_lock(&g_cache_mutex);
    
    for (int i = 0; i < 256; i++) {
        g_module_cache[i].register_count = 0;
        g_module_cache[i].last_update_ms = 0;
        g_module_cache[i].initialized = false;
        memset(g_module_cache[i].registers, 0, sizeof(g_module_cache[i].registers));
    }
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    printf("[CACHE] Cleared all module caches\n");
    return HAL_STATUS_OK;
}

bool register_cache_has_data(uint8_t module_addr) {
    if (!g_cache_initialized) {
        return false;
    }
    
    pthread_mutex_lock(&g_cache_mutex);
    
    bool has_data = g_module_cache[module_addr].initialized && 
                    g_module_cache[module_addr].register_count > 0;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    return has_data;
}

hal_status_t register_cache_get_statistics(cache_statistics_t *stats) {
    if (!g_cache_initialized || stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_cache_mutex);
    
    *stats = g_cache_stats;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    return HAL_STATUS_OK;
}

hal_status_t register_cache_get_last_update(uint8_t module_addr, uint64_t *timestamp_ms) {
    if (!g_cache_initialized || timestamp_ms == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&g_cache_mutex);
    
    module_register_cache_t *cache = &g_module_cache[module_addr];
    
    if (!cache->initialized) {
        pthread_mutex_unlock(&g_cache_mutex);
        return HAL_STATUS_NOT_FOUND;
    }
    
    *timestamp_ms = cache->last_update_ms;
    
    pthread_mutex_unlock(&g_cache_mutex);
    
    return HAL_STATUS_OK;
}

