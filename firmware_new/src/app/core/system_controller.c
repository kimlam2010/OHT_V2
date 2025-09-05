/**
 * @file system_controller.c
 * @brief System Controller Implementation for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (System Controller Implementation)
 */

#include "system_controller.h"
#include "hal_common.h"
#include "hal_estop.h"
#include "hal_led.h"
#include "hal_relay.h"
#include "hal_rs485.h"
#include "hal_network.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// System Controller Instance
static struct {
    system_controller_config_t config;
    system_controller_status_t status;
    system_controller_stats_t stats;
    system_controller_performance_t performance;
    system_controller_error_info_t error_info;
    system_controller_event_callback_t event_callback;
    system_controller_error_callback_t error_callback;
    bool initialized;
    bool activated;
    uint64_t last_update_time;
    uint64_t last_performance_check;
    uint64_t last_error_reset;
    uint32_t update_count;
    uint32_t error_count;
    uint32_t recovery_attempts;
} system_controller_instance = {0};

// Constants
#define PERFORMANCE_CHECK_PERIOD_MS 1000
#define ERROR_RESET_PERIOD_MS 5000
#define MAX_ERROR_COUNT 10
#define SYSTEM_CONTROLLER_VERSION "1.0.0"

// Event queue (ring buffer) for decoupled event ingestion and dispatch
#define SYSTEM_CONTROLLER_EVENT_QUEUE_SIZE 32
typedef struct {
    system_controller_event_t event_type;
    char details[128];
    uint64_t timestamp_ms;
} system_controller_queued_event_t;

static struct {
    system_controller_queued_event_t buffer[SYSTEM_CONTROLLER_EVENT_QUEUE_SIZE];
    uint32_t head; // write index
    uint32_t tail; // read index
    uint32_t count;
} system_controller_event_queue = {0};

// Default configuration
static const system_controller_config_t default_config = {
    .update_period_ms = 10,
    .timeout_ms = 5000,
    .error_retry_count = 3,
    .error_retry_delay_ms = 1000,
    .enable_auto_recovery = true,
    .enable_error_logging = true,
    .enable_performance_monitoring = true,
    .enable_diagnostics = true,
    .max_error_count = MAX_ERROR_COUNT,
    .error_reset_timeout_ms = ERROR_RESET_PERIOD_MS
};

// Internal function prototypes
static uint64_t system_controller_get_timestamp_ms(void);
static void system_controller_log_event_internal(system_controller_event_t event, const char* message);
static hal_status_t system_controller_validate_state_transition(system_controller_state_t new_state);
static hal_status_t system_controller_check_safety(void);
static hal_status_t system_controller_check_communication(void);
static hal_status_t system_controller_check_control(void);
static hal_status_t system_controller_update_performance(void);
static void system_controller_dispatch_events(void);
static void system_controller_handle_event(system_controller_event_t event, const char* details);
static bool system_controller_event_queue_push(system_controller_event_t event, const char* details);
static bool system_controller_event_queue_pop(system_controller_queued_event_t* out_event);

// Implementation

hal_status_t system_controller_init(const system_controller_config_t *config)
{
    // Check if already initialized
    if (system_controller_instance.initialized) {
        return HAL_STATUS_ALREADY_INITIALIZED;
    }
    
    // Initialize instance
    memset(&system_controller_instance, 0, sizeof(system_controller_instance));
    
    // Set configuration
    if (config != NULL) {
        system_controller_instance.config = *config;
    } else {
        system_controller_instance.config = default_config;
    }
    
    // Initialize status
    system_controller_instance.status.current_state = SYSTEM_CONTROLLER_STATE_INIT;
    system_controller_instance.status.previous_state = SYSTEM_CONTROLLER_STATE_INIT;
    system_controller_instance.status.last_event = SYSTEM_CONTROLLER_EVENT_NONE;
    system_controller_instance.status.current_error = SYSTEM_CONTROLLER_ERROR_NONE;
    system_controller_instance.status.state_entry_time = system_controller_get_timestamp_ms();
    system_controller_instance.status.last_update_time = system_controller_get_timestamp_ms();
    
    // Initialize timing
    system_controller_instance.last_update_time = system_controller_get_timestamp_ms();
    system_controller_instance.last_performance_check = system_controller_get_timestamp_ms();
    system_controller_instance.last_error_reset = system_controller_get_timestamp_ms();
    
    // Initialize performance metrics
    system_controller_instance.performance.cpu_usage_percent = 0;
    system_controller_instance.performance.memory_usage_percent = 0;
    system_controller_instance.performance.update_frequency_hz = 100;
    system_controller_instance.performance.response_time_ms = 0;
    system_controller_instance.performance.error_rate_percent = 0;
    system_controller_instance.performance.recovery_time_ms = 0;
    
    // Initialize error info
    system_controller_instance.error_info.error_type = SYSTEM_CONTROLLER_ERROR_NONE;
    system_controller_instance.error_info.error_code = 0;
    system_controller_instance.error_info.error_time = 0;
    system_controller_instance.error_info.error_count = 0;
    system_controller_instance.error_info.error_resolved = true;
    memset(system_controller_instance.error_info.error_message, 0, sizeof(system_controller_instance.error_info.error_message));
    memset(system_controller_instance.error_info.error_context, 0, sizeof(system_controller_instance.error_info.error_context));
    
    // Set initialized flag
    system_controller_instance.initialized = true;
    
    // Log initialization
    system_controller_log_event_internal(SYSTEM_CONTROLLER_EVENT_NONE, "System controller initialized");
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_deinit(void)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Deactivate if active
    if (system_controller_instance.activated) {
        system_controller_deactivate();
    }
    
    // Log deinitialization
    system_controller_log_event_internal(SYSTEM_CONTROLLER_EVENT_SHUTDOWN, "System controller deinitialized");
    
    // Clear instance
    memset(&system_controller_instance, 0, sizeof(system_controller_instance));
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_update(void)
{
    hal_status_t status = HAL_STATUS_OK;
    uint64_t start_time = system_controller_get_timestamp_ms();
    
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Update last update time
    system_controller_instance.status.last_update_time = start_time;
    system_controller_instance.status.uptime_ms = start_time - system_controller_instance.status.state_entry_time;
    
    // Check if system is ready
    bool ready = false;
    status = system_controller_is_ready(&ready);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    system_controller_instance.status.system_ready = ready;
    
    // Update performance metrics
    if (system_controller_instance.config.enable_performance_monitoring) {
        status = system_controller_update_performance();
        if (status != HAL_STATUS_OK) {
            return status;
        }
    }
    
    // Check safety status
    status = system_controller_check_safety();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check communication status
    status = system_controller_check_communication();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Check control status
    status = system_controller_check_control();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Update statistics
    system_controller_instance.stats.total_events++;
    system_controller_instance.update_count++;
    
    // Dispatch queued events after health checks to avoid reentrancy
    system_controller_dispatch_events();

    // Calculate response time
    uint64_t end_time = system_controller_get_timestamp_ms();
    system_controller_instance.performance.response_time_ms = (uint32_t)(end_time - start_time);
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_activate(void)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (system_controller_instance.activated) {
        return HAL_STATUS_ALREADY_ACTIVE;
    }
    
    // Check if system is ready
    bool ready = false;
    hal_status_t status = system_controller_is_ready(&ready);
    if (status != HAL_STATUS_OK || !ready) {
        return HAL_STATUS_ERROR;
    }
    
    // Activate system controller
    system_controller_instance.activated = true;
    
    // Log activation
    system_controller_log_event_internal(SYSTEM_CONTROLLER_EVENT_ACTIVATE, "System controller activated");
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_deactivate(void)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (!system_controller_instance.activated) {
        return HAL_STATUS_ERROR;
    }
    
    // Deactivate system controller
    system_controller_instance.activated = false;
    
    // Log deactivation
    system_controller_log_event_internal(SYSTEM_CONTROLLER_EVENT_DEACTIVATE, "System controller deactivated");
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_get_status(system_controller_status_t *status)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *status = system_controller_instance.status;
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_get_stats(system_controller_stats_t *stats)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (stats == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *stats = system_controller_instance.stats;
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_get_performance(system_controller_performance_t *performance)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (performance == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *performance = system_controller_instance.performance;
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_get_error_info(system_controller_error_info_t *error_info)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (error_info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *error_info = system_controller_instance.error_info;
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_is_ready(bool *ready)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    if (ready == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check if all subsystems are ready
    *ready = system_controller_instance.status.safety_ok &&
             system_controller_instance.status.communication_ok &&
             system_controller_instance.status.control_ok;
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_process_event(system_controller_event_t event, const char* details)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    // Enqueue event for deferred handling in update()
    if (!system_controller_event_queue_push(event, details)) {
        // Queue full: treat as error event and record
        system_controller_log_event_internal(SYSTEM_CONTROLLER_EVENT_ERROR, "Event queue full");
        system_controller_instance.error_count++;
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

hal_status_t system_controller_set_state(system_controller_state_t new_state)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Validate state transition
    hal_status_t status = system_controller_validate_state_transition(new_state);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Update state
    system_controller_state_t old_state = system_controller_instance.status.current_state;
    system_controller_instance.status.previous_state = old_state;
    system_controller_instance.status.current_state = new_state;
    system_controller_instance.status.state_entry_time = system_controller_get_timestamp_ms();
    system_controller_instance.status.state_transition_count++;
    
    // Update statistics
    system_controller_instance.stats.state_transitions++;
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_reset_errors(void)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Reset error information
    system_controller_instance.error_info.error_type = SYSTEM_CONTROLLER_ERROR_NONE;
    system_controller_instance.error_info.error_code = 0;
    system_controller_instance.error_info.error_count = 0;
    system_controller_instance.error_info.error_resolved = true;
    memset(system_controller_instance.error_info.error_message, 0, sizeof(system_controller_instance.error_info.error_message));
    memset(system_controller_instance.error_info.error_context, 0, sizeof(system_controller_instance.error_info.error_context));
    
    // Reset counters
    system_controller_instance.error_count = 0;
    system_controller_instance.recovery_attempts = 0;
    system_controller_instance.last_error_reset = system_controller_get_timestamp_ms();
    
    // Update status
    system_controller_instance.status.current_error = SYSTEM_CONTROLLER_ERROR_NONE;
    
    // Log reset
    system_controller_log_event_internal(SYSTEM_CONTROLLER_EVENT_ERROR, "Errors reset");
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_set_event_callback(system_controller_event_callback_t callback)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    system_controller_instance.event_callback = callback;
    
    return HAL_STATUS_OK;
}

hal_status_t system_controller_set_error_callback(system_controller_error_callback_t callback)
{
    if (!system_controller_instance.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    system_controller_instance.error_callback = callback;
    
    return HAL_STATUS_OK;
}

const char* system_controller_get_version(void)
{
    return SYSTEM_CONTROLLER_VERSION;
}

// Internal functions

static uint64_t system_controller_get_timestamp_ms(void)
{
    // Use HAL timestamp in microseconds and convert to milliseconds
    return (uint64_t)(hal_get_timestamp_us() / 1000ULL);
}

static void system_controller_log_event_internal(system_controller_event_t event, const char* message)
{
    if (!system_controller_instance.config.enable_error_logging) {
        return;
    }
    
    // Implement proper logging
    printf("[SYSTEM_CONTROLLER] Event: %d, Message: %s\n", 
           (int)event, message ? message : "N/A");
    
    // Call callback if available
    if (system_controller_instance.event_callback != NULL) {
        system_controller_instance.event_callback(SYSTEM_CONTROLLER_STATE_INIT, event, message);
    }
}

static hal_status_t system_controller_validate_state_transition(system_controller_state_t new_state)
{
    system_controller_state_t current_state = system_controller_instance.status.current_state;
    
    // Validate state transitions based on current state
    switch (current_state) {
        case SYSTEM_CONTROLLER_STATE_INIT:
            if (new_state != SYSTEM_CONTROLLER_STATE_IDLE && 
                new_state != SYSTEM_CONTROLLER_STATE_FAULT) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SYSTEM_CONTROLLER_STATE_IDLE:
            // Can transition to any state except INIT
            if (new_state == SYSTEM_CONTROLLER_STATE_INIT) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SYSTEM_CONTROLLER_STATE_ACTIVE:
            // Can transition to any state except INIT
            if (new_state == SYSTEM_CONTROLLER_STATE_INIT) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SYSTEM_CONTROLLER_STATE_FAULT:
            if (new_state == SYSTEM_CONTROLLER_STATE_INIT || 
                new_state == SYSTEM_CONTROLLER_STATE_ACTIVE) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SYSTEM_CONTROLLER_STATE_EMERGENCY:
            if (new_state == SYSTEM_CONTROLLER_STATE_INIT || 
                new_state == SYSTEM_CONTROLLER_STATE_ACTIVE) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        case SYSTEM_CONTROLLER_STATE_SHUTDOWN:
            // Can only transition to INIT
            if (new_state != SYSTEM_CONTROLLER_STATE_INIT) {
                return HAL_STATUS_INVALID_STATE;
            }
            break;
            
        default:
            return HAL_STATUS_INVALID_STATE;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t system_controller_check_safety(void)
{
    bool safe = false;
    hal_status_t status = safety_monitor_is_safe(&safe);
    
    if (status != HAL_STATUS_OK) {
        system_controller_instance.status.safety_ok = false;
        return status;
    }
    
    system_controller_instance.status.safety_ok = safe;
    
    return HAL_STATUS_OK;
}

static hal_status_t system_controller_check_communication(void)
{
    // Implement communication check
    // Check RS485 communication (simplified check)
    // TODO: Implement proper RS485 connection check
    system_controller_instance.status.communication_ok = true;
    
    // Check network communication
    bool network_connected = false;
    hal_status_t network_status = hal_network_is_connected(NETWORK_TYPE_ETHERNET, &network_connected);
    if (network_status != HAL_STATUS_OK || !network_connected) {
        system_controller_instance.status.communication_ok = false;
        return HAL_STATUS_ERROR;
    }
    
    system_controller_instance.status.communication_ok = true;
    return HAL_STATUS_OK;
}

static hal_status_t system_controller_check_control(void)
{
    // Implement control check
    // Check control loop status
    control_status_t control_status;
    hal_status_t status = control_loop_get_status(&control_status);
    if (status != HAL_STATUS_OK) {
        system_controller_instance.status.control_ok = false;
        return status;
    }
    
    // Check if control loop is active and responding
    if (control_status.state == CONTROL_STATE_DISABLED || control_status.state == CONTROL_STATE_ERROR || 
        control_status.state == CONTROL_STATE_FAULT || control_status.limits_violated || control_status.safety_violated) {
        system_controller_instance.status.control_ok = false;
        return HAL_STATUS_ERROR;
    }
    
    system_controller_instance.status.control_ok = true;
    return HAL_STATUS_OK;
}

static hal_status_t system_controller_update_performance(void)
{
    uint64_t current_time = system_controller_get_timestamp_ms();
    
    // Update performance metrics periodically
    if (current_time - system_controller_instance.last_performance_check >= PERFORMANCE_CHECK_PERIOD_MS) {
        // Calculate update frequency
        if (system_controller_instance.update_count > 0) {
            system_controller_instance.performance.update_frequency_hz = 
                (uint32_t)(system_controller_instance.update_count * 1000 / PERFORMANCE_CHECK_PERIOD_MS);
        }
        
        // Calculate error rate
        if (system_controller_instance.stats.total_events > 0) {
            system_controller_instance.performance.error_rate_percent = 
                (uint32_t)(system_controller_instance.error_count * 100 / system_controller_instance.stats.total_events);
        }
        
        // Implement CPU and memory usage calculation
        // Get system resource usage
        FILE *meminfo = fopen("/proc/meminfo", "r");
        if (meminfo) {
            char line[256];
            uint64_t total_mem = 0, free_mem = 0;
            while (fgets(line, sizeof(line), meminfo)) {
                if (sscanf(line, "MemTotal: %lu kB", &total_mem) == 1) {
                    // Found total memory
                } else if (sscanf(line, "MemAvailable: %lu kB", &free_mem) == 1) {
                    // Found available memory
                }
            }
            fclose(meminfo);
            
            if (total_mem > 0) {
                system_controller_instance.performance.memory_usage_percent = 
                    (uint32_t)((total_mem - free_mem) * 100 / total_mem);
            }
        }
        
        // CPU usage calculation (simplified)
        static uint64_t last_cpu_time = 0;
        uint64_t current_cpu_time = system_controller_get_timestamp_ms();
        if (last_cpu_time > 0) {
            uint64_t cpu_delta = current_cpu_time - last_cpu_time;
            system_controller_instance.performance.cpu_usage_percent = 
                (uint32_t)(cpu_delta * 100 / system_controller_instance.config.update_period_ms);
        }
        last_cpu_time = current_cpu_time;
        
        // Update last performance check time
        system_controller_instance.last_performance_check = current_time;
        system_controller_instance.update_count = 0;
    }
    
    return HAL_STATUS_OK;
}

// Event queue helpers and dispatcher
static bool system_controller_event_queue_push(system_controller_event_t event, const char* details)
{
    if (system_controller_event_queue.count >= SYSTEM_CONTROLLER_EVENT_QUEUE_SIZE) {
        return false;
    }
    system_controller_queued_event_t *slot = &system_controller_event_queue.buffer[system_controller_event_queue.head];
    slot->event_type = event;
    slot->timestamp_ms = system_controller_get_timestamp_ms();
    if (details) {
        snprintf(slot->details, sizeof(slot->details), "%s", details);
    } else {
        slot->details[0] = '\0';
    }
    system_controller_event_queue.head = (system_controller_event_queue.head + 1U) % SYSTEM_CONTROLLER_EVENT_QUEUE_SIZE;
    system_controller_event_queue.count++;
    return true;
}

static bool system_controller_event_queue_pop(system_controller_queued_event_t* out_event)
{
    if (system_controller_event_queue.count == 0) {
        return false;
    }
    if (out_event) {
        *out_event = system_controller_event_queue.buffer[system_controller_event_queue.tail];
    }
    system_controller_event_queue.tail = (system_controller_event_queue.tail + 1U) % SYSTEM_CONTROLLER_EVENT_QUEUE_SIZE;
    system_controller_event_queue.count--;
    return true;
}

static void system_controller_dispatch_events(void)
{
    // Limit the number of events handled per update to avoid starvation
    const uint32_t max_events_per_update = 8;
    uint32_t handled = 0;
    system_controller_queued_event_t qe;
    while (handled < max_events_per_update && system_controller_event_queue_pop(&qe)) {
        system_controller_handle_event(qe.event_type, qe.details);
        handled++;
    }
}

static void system_controller_handle_event(system_controller_event_t event, const char* details)
{
    // Log event
    system_controller_log_event_internal(event, details);

    // Update status
    system_controller_instance.status.last_event = event;

    // Handle specific events
    switch (event) {
        case SYSTEM_CONTROLLER_EVENT_INIT_COMPLETE:
            (void)system_controller_set_state(SYSTEM_CONTROLLER_STATE_IDLE);
            break;
        case SYSTEM_CONTROLLER_EVENT_ACTIVATE:
            (void)system_controller_activate();
            (void)system_controller_set_state(SYSTEM_CONTROLLER_STATE_ACTIVE);
            break;
        case SYSTEM_CONTROLLER_EVENT_DEACTIVATE:
            (void)system_controller_deactivate();
            (void)system_controller_set_state(SYSTEM_CONTROLLER_STATE_IDLE);
            break;
        case SYSTEM_CONTROLLER_EVENT_FAULT_DETECTED:
            system_controller_instance.status.current_error = SYSTEM_CONTROLLER_ERROR_HARDWARE;
            (void)system_controller_set_state(SYSTEM_CONTROLLER_STATE_FAULT);
            system_controller_instance.error_count++;
            break;
        case SYSTEM_CONTROLLER_EVENT_FAULT_CLEARED:
            (void)system_controller_reset_errors();
            (void)system_controller_set_state(SYSTEM_CONTROLLER_STATE_IDLE);
            break;
        case SYSTEM_CONTROLLER_EVENT_EMERGENCY:
            system_controller_instance.status.current_error = SYSTEM_CONTROLLER_ERROR_SAFETY_VIOLATION;
            (void)system_controller_set_state(SYSTEM_CONTROLLER_STATE_EMERGENCY);
            system_controller_instance.error_count++;
            break;
        case SYSTEM_CONTROLLER_EVENT_SHUTDOWN:
            (void)system_controller_set_state(SYSTEM_CONTROLLER_STATE_SHUTDOWN);
            break;
        case SYSTEM_CONTROLLER_EVENT_ERROR:
            system_controller_instance.error_count++;
            break;
        case SYSTEM_CONTROLLER_EVENT_NONE:
        default:
            break;
    }

    // Enforce max error policy without recursive enqueue
    if (system_controller_instance.error_count >= system_controller_instance.config.max_error_count &&
        system_controller_instance.status.current_state != SYSTEM_CONTROLLER_STATE_FAULT) {
        system_controller_instance.status.current_error = SYSTEM_CONTROLLER_ERROR_CONTROL;
        (void)system_controller_set_state(SYSTEM_CONTROLLER_STATE_FAULT);
    }
}
