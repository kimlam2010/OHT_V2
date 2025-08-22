/**
 * @file hal_watchdog.h
 * @brief HAL interface for watchdog system
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-15 (Watchdog System Implementation)
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

#ifndef HAL_WATCHDOG_H
#define HAL_WATCHDOG_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_common.h"

// Watchdog Configuration
#define WATCHDOG_DEFAULT_TIMEOUT_MS    5000    // Default timeout 5 seconds
#define WATCHDOG_MIN_TIMEOUT_MS        1000    // Minimum timeout 1 second
#define WATCHDOG_MAX_TIMEOUT_MS        30000   // Maximum timeout 30 seconds
#define WATCHDOG_FEED_INTERVAL_MS      1000    // Feed interval 1 second

// Watchdog States
typedef enum {
    WATCHDOG_STATE_DISABLED = 0,        // Watchdog disabled
    WATCHDOG_STATE_ENABLED,             // Watchdog enabled
    WATCHDOG_STATE_TIMEOUT,             // Watchdog timeout occurred
    WATCHDOG_STATE_FAULT                // Watchdog fault
} watchdog_state_t;

// Watchdog Fault Types
typedef enum {
    WATCHDOG_FAULT_NONE = 0,            // No fault
    WATCHDOG_FAULT_TIMEOUT,             // Timeout fault
    WATCHDOG_FAULT_HARDWARE_ERROR,      // Hardware error
    WATCHDOG_FAULT_CONFIG_ERROR         // Configuration error
} watchdog_fault_t;

// Watchdog Configuration Structure
typedef struct {
    uint32_t timeout_ms;                // Timeout in milliseconds
    uint32_t feed_interval_ms;          // Feed interval in milliseconds
    bool auto_feed_enabled;             // Auto feed enabled
    bool reset_on_timeout;              // Reset system on timeout
    hal_callback_t timeout_callback;    // Timeout callback function
    void *timeout_context;              // Timeout callback context
} watchdog_config_t;

// Watchdog Status Structure
typedef struct {
    watchdog_state_t state;             // Current state
    watchdog_fault_t fault;             // Current fault
    uint64_t last_feed_time;            // Last feed time
    uint64_t next_feed_time;            // Next feed time
    uint32_t timeout_count;             // Timeout count
    uint32_t feed_count;                // Feed count
    uint64_t uptime_ms;                 // Uptime in milliseconds
} watchdog_status_t;

// Watchdog Statistics Structure
typedef struct {
    uint64_t total_feeds;               // Total feeds
    uint64_t total_timeouts;            // Total timeouts
    uint64_t total_resets;              // Total resets
    uint64_t min_feed_interval_ms;      // Minimum feed interval
    uint64_t max_feed_interval_ms;      // Maximum feed interval
    uint64_t avg_feed_interval_ms;      // Average feed interval
    uint64_t timestamp_us;              // Statistics timestamp
} watchdog_statistics_t;

// Function Prototypes

/**
 * @brief Initialize watchdog system
 * @param config Watchdog configuration
 * @return HAL status
 */
hal_status_t hal_watchdog_init(const watchdog_config_t *config);

/**
 * @brief Deinitialize watchdog system
 * @return HAL status
 */
hal_status_t hal_watchdog_deinit(void);

/**
 * @brief Feed watchdog (reset timer)
 * @return HAL status
 */
hal_status_t hal_watchdog_feed(void);

/**
 * @brief Get watchdog status
 * @param status Pointer to store watchdog status
 * @return HAL status
 */
hal_status_t hal_watchdog_get_status(watchdog_status_t *status);

/**
 * @brief Get watchdog statistics
 * @param statistics Pointer to store watchdog statistics
 * @return HAL status
 */
hal_status_t hal_watchdog_get_statistics(watchdog_statistics_t *statistics);

/**
 * @brief Reset watchdog statistics
 * @return HAL status
 */
hal_status_t hal_watchdog_reset_statistics(void);

/**
 * @brief Set watchdog timeout callback
 * @param callback Timeout callback function
 * @param context Callback context
 * @return HAL status
 */
hal_status_t hal_watchdog_set_timeout_callback(hal_callback_t callback, void *context);

/**
 * @brief Enable watchdog auto-feed
 * @return HAL status
 */
hal_status_t hal_watchdog_enable_auto_feed(void);

/**
 * @brief Disable watchdog auto-feed
 * @return HAL status
 */
hal_status_t hal_watchdog_disable_auto_feed(void);

/**
 * @brief Check if watchdog is enabled
 * @return true if enabled, false otherwise
 */
bool hal_watchdog_is_enabled(void);

/**
 * @brief Get watchdog timeout value
 * @param timeout_ms Pointer to store timeout value
 * @return HAL status
 */
hal_status_t hal_watchdog_get_timeout(uint32_t *timeout_ms);

/**
 * @brief Set watchdog timeout value
 * @param timeout_ms Timeout value in milliseconds
 * @return HAL status
 */
hal_status_t hal_watchdog_set_timeout(uint32_t timeout_ms);

/**
 * @brief Force watchdog timeout (for testing)
 * @return HAL status
 */
hal_status_t hal_watchdog_force_timeout(void);

/**
 * @brief Reset watchdog system
 * @return HAL status
 */
hal_status_t hal_watchdog_reset(void);

#endif // HAL_WATCHDOG_H
