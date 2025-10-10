/**
 * @file register_value_cache.h
 * @brief Register Value Cache System for storing runtime register values
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 * @task Complete Register Data API - Cache system for polling integration
 */

#ifndef REGISTER_VALUE_CACHE_H
#define REGISTER_VALUE_CACHE_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// Maximum number of registers per module to cache
#define MAX_CACHED_REGISTERS_PER_MODULE 256

// Register value cache entry
typedef struct {
    uint16_t address;           // Register address (0x0000-0xFFFF)
    uint16_t value;             // Register value
    uint64_t timestamp_ms;      // Timestamp when value was cached (milliseconds)
    bool valid;                 // Is this cache entry valid?
} register_value_entry_t;

// Module register cache
typedef struct {
    uint8_t module_address;                                     // Module address (0x00-0xFF)
    register_value_entry_t registers[MAX_CACHED_REGISTERS_PER_MODULE];  // Cached register values
    uint16_t register_count;                                    // Number of cached registers
    uint64_t last_update_ms;                                    // Last update timestamp
    bool initialized;                                           // Is this module cache initialized?
} module_register_cache_t;

// Cache statistics
typedef struct {
    uint32_t total_stores;      // Total store operations
    uint32_t total_reads;       // Total read operations
    uint32_t cache_hits;        // Successful reads
    uint32_t cache_misses;      // Failed reads (not found)
    uint32_t expired_entries;   // Expired cache entries
} cache_statistics_t;

/**
 * @brief Initialize register value cache system
 * @return HAL status
 */
hal_status_t register_cache_init(void);

/**
 * @brief Deinitialize register value cache system
 * @return HAL status
 */
hal_status_t register_cache_deinit(void);

/**
 * @brief Store register value in cache
 * @param module_addr Module address (0x00-0xFF)
 * @param reg_addr Register address (0x0000-0xFFFF)
 * @param value Register value
 * @return HAL status
 */
hal_status_t register_cache_store(uint8_t module_addr, uint16_t reg_addr, uint16_t value);

/**
 * @brief Store multiple register values in cache (batch operation)
 * @param module_addr Module address
 * @param start_addr Starting register address
 * @param values Array of values
 * @param count Number of values
 * @return HAL status
 */
hal_status_t register_cache_store_batch(uint8_t module_addr, uint16_t start_addr, 
                                        const uint16_t *values, uint16_t count);

/**
 * @brief Get register value from cache
 * @param module_addr Module address
 * @param reg_addr Register address
 * @param value Pointer to store value
 * @param timestamp_ms Pointer to store timestamp (optional, can be NULL)
 * @return HAL status (HAL_STATUS_OK if found, HAL_STATUS_NOT_FOUND if not cached)
 */
hal_status_t register_cache_get(uint8_t module_addr, uint16_t reg_addr, 
                                uint16_t *value, uint64_t *timestamp_ms);

/**
 * @brief Get all cached registers for a module
 * @param module_addr Module address
 * @param entries Array to store register entries
 * @param max_entries Maximum number of entries to retrieve
 * @param count Pointer to store actual number of entries retrieved
 * @return HAL status
 */
hal_status_t register_cache_get_all(uint8_t module_addr, register_value_entry_t *entries, 
                                    uint16_t max_entries, uint16_t *count);

/**
 * @brief Clear all cached values for a module
 * @param module_addr Module address
 * @return HAL status
 */
hal_status_t register_cache_clear_module(uint8_t module_addr);

/**
 * @brief Clear all cached values for all modules
 * @return HAL status
 */
hal_status_t register_cache_clear_all(void);

/**
 * @brief Check if module has cached data
 * @param module_addr Module address
 * @return true if module has cached data, false otherwise
 */
bool register_cache_has_data(uint8_t module_addr);

/**
 * @brief Get cache statistics
 * @param stats Pointer to store statistics
 * @return HAL status
 */
hal_status_t register_cache_get_statistics(cache_statistics_t *stats);

/**
 * @brief Get last update timestamp for a module
 * @param module_addr Module address
 * @param timestamp_ms Pointer to store timestamp
 * @return HAL status
 */
hal_status_t register_cache_get_last_update(uint8_t module_addr, uint64_t *timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif // REGISTER_VALUE_CACHE_H

