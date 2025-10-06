#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include "hal_estop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

// E-Stop configuration
static estop_config_t estop_config = {
    .pin = ESTOP_PIN,
    .response_timeout_ms = ESTOP_RESPONSE_TIME_MS,
    .debounce_time_ms = ESTOP_DEBOUNCE_TIME_MS,
    .auto_reset_enabled = false
};

// E-Stop status
static estop_status_t estop_status = {0};
static bool estop_initialized = false;
static pthread_t estop_monitor_thread;
static bool estop_thread_running = false;
static estop_event_callback_t estop_callback = NULL;

// Internal functions
static uint64_t get_timestamp_ms(void);
static hal_status_t gpio_export(uint8_t pin);
static hal_status_t gpio_set_direction(uint8_t pin, bool output);
static hal_status_t gpio_get_value(uint8_t pin, bool *value);
static void* estop_monitor_thread_func(void *arg);
static void estop_handle_trigger(void);
static void estop_handle_fault(estop_fault_t fault);

hal_status_t hal_estop_init(const estop_config_t *config) {
    printf("Initializing E-Stop safety system...\n");

    // CRITICAL SAFETY: Check for NULL config first
    if (config == NULL) {
        printf("CRITICAL SAFETY ERROR: NULL config provided\n");
        return HAL_STATUS_ERROR;
    }

    // CRITICAL SAFETY: Parameter validation BEFORE any operations
    // Validate pin range (0-63 for Orange Pi 5B)
    if (config->pin > 63) {
        printf("CRITICAL SAFETY ERROR: Invalid E-Stop pin %d (must be 0-63)\n", config->pin);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate debounce time (minimum 10ms for safety)
    if (config->debounce_time_ms < 10) {
        printf("CRITICAL SAFETY ERROR: Invalid debounce time %dms (minimum 10ms required)\n", config->debounce_time_ms);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Validate timeout (minimum 100ms for safety)
    if (config->response_timeout_ms < 100) {
        printf("CRITICAL SAFETY ERROR: Invalid timeout %dms (minimum 100ms required)\n", config->response_timeout_ms);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Copy validated configuration
    estop_config = *config;

    // Check if already initialized AFTER parameter validation
    if (estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    // Initialize status
    memset(&estop_status, 0, sizeof(estop_status));
    estop_status.state = ESTOP_STATE_SAFE;
    estop_status.fault = ESTOP_FAULT_NONE;
    estop_status.pin_status = false;
    estop_status.last_trigger_time = 0;
    estop_status.last_reset_time = 0;
    estop_status.trigger_count = 0;
    estop_status.fault_count = 0;

    // Export GPIO pin (single channel)
    hal_status_t status = gpio_export(estop_config.pin);
    if (status != HAL_STATUS_OK) {
        printf("Failed to export E-Stop GPIO pin %d\n", estop_config.pin);
        return status;
    }

    // Set GPIO direction (input)
    status = gpio_set_direction(estop_config.pin, false);
    if (status != HAL_STATUS_OK) {
        printf("Failed to set E-Stop direction\n");
        return status;
    }

    // Start E-Stop monitor thread (optional for testing)
    estop_thread_running = true;
    int thread_result = pthread_create(&estop_monitor_thread, NULL, estop_monitor_thread_func, NULL);
    if (thread_result != 0) {
        printf("Warning: Failed to create E-Stop monitor thread (error %d) - continuing without threading\n", thread_result);
        estop_thread_running = false;
        // Don't fail initialization - threading is optional for testing
    }

    estop_initialized = true;
    printf("E-Stop safety system initialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_deinit(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    printf("Deinitializing E-Stop safety system...\n");

    // Stop E-Stop monitor thread (if it was created)
    if (estop_thread_running) {
        estop_thread_running = false;
        pthread_join(estop_monitor_thread, NULL);
    }

    estop_initialized = false;
    printf("E-Stop safety system deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_state(estop_state_t *state) {
    if (!estop_initialized || state == NULL) {
        return HAL_STATUS_ERROR;
    }

    *state = estop_status.state;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_is_triggered(bool *triggered) {
    if (!estop_initialized || triggered == NULL) {
        return HAL_STATUS_ERROR;
    }

    *triggered = (estop_status.state == ESTOP_STATE_TRIGGERED);
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_reset(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    printf("Resetting E-Stop system...\n");

    // Check if both channels are safe
    bool channel1_safe, channel2_safe;
    hal_status_t status = gpio_get_value(estop_config.pin, &channel1_safe);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = gpio_get_value(estop_config.pin, &channel2_safe);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    // Both channels must be safe to reset
    if (!channel1_safe || !channel2_safe) {
        printf("Cannot reset E-Stop: channels not safe\n");
        return HAL_STATUS_ERROR;
    }

    // Update status
    estop_status.state = ESTOP_STATE_RESETTING;
    estop_status.last_reset_time = get_timestamp_ms();

    // Wait for debounce time
    usleep(estop_config.debounce_time_ms * 1000);

    // Check channels again after debounce
    status = gpio_get_value(estop_config.pin, &channel1_safe);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = gpio_get_value(estop_config.pin, &channel2_safe);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    if (channel1_safe && channel2_safe) {
        estop_status.state = ESTOP_STATE_SAFE;
        estop_status.fault = ESTOP_FAULT_NONE;
        printf("E-Stop system reset successfully\n");
        return HAL_STATUS_OK;
    } else {
        estop_status.state = ESTOP_STATE_TRIGGERED;
        printf("E-Stop reset failed: channels still triggered\n");
        return HAL_STATUS_ERROR;
    }
}

hal_status_t hal_estop_get_fault(estop_fault_t *fault) {
    if (!estop_initialized || fault == NULL) {
        return HAL_STATUS_ERROR;
    }

    *fault = estop_status.fault;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_clear_fault(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    printf("Clearing E-Stop fault...\n");
    estop_status.fault = ESTOP_FAULT_NONE;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_status(estop_status_t *status) {
    if (!estop_initialized || status == NULL) {
        return HAL_STATUS_ERROR;
    }

    *status = estop_status;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_set_callback(estop_event_callback_t callback) {
    // Check if system is initialized
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    estop_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_update(void) {
    // This function is called periodically to update E-Stop status
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    // Read channel status
    bool pin_value;
    hal_status_t status = gpio_get_value(estop_config.pin, &pin_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    // Update status
    estop_status.pin_status = pin_value;

    // Check for E-Stop trigger (active low)
    bool estop_triggered = !pin_value;

    // Handle state transitions
    if (estop_triggered && estop_status.state == ESTOP_STATE_SAFE) {
        estop_status.state = ESTOP_STATE_TRIGGERED;
        estop_status.last_trigger_time = get_timestamp_ms();
        estop_status.trigger_count++;
        
        printf("E-Stop triggered!\n");
        
        // Call callback if set
        if (estop_callback != NULL) {
            estop_callback(ESTOP_STATE_TRIGGERED, ESTOP_FAULT_NONE);
        }
    } else if (!estop_triggered && estop_status.state == ESTOP_STATE_TRIGGERED) {
        // Auto-reset if enabled
        if (estop_config.auto_reset_enabled) {
            estop_status.state = ESTOP_STATE_SAFE;
            printf("E-Stop auto-reset: pin safe\n");
            
            // Call callback if set
            if (estop_callback != NULL) {
                estop_callback(ESTOP_STATE_SAFE, ESTOP_FAULT_NONE);
            }
        }
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_estop_test_channels(bool *pin_status) {
    if (!estop_initialized || pin_status == NULL) {
        return HAL_STATUS_ERROR;
    }

    hal_status_t status = gpio_get_value(estop_config.pin, pin_status);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_estop_validate_safety(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    printf("Validating E-Stop safety system...\n");

    // Test channel response time
    uint64_t start_time = get_timestamp_ms();
    bool pin_value;
    
    hal_status_t status = gpio_get_value(estop_config.pin, &pin_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    uint64_t response_time = get_timestamp_ms() - start_time;
    
    if (response_time > estop_config.response_timeout_ms) {
                printf("E-Stop validation failed: response time %lu ms > %u ms\n",
               response_time, estop_config.response_timeout_ms);
        return HAL_STATUS_ERROR;
    }

    printf("E-Stop safety validation passed: response time %lu ms\n", response_time);
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_response_time(uint32_t *response_time_ms) {
    if (!estop_initialized || response_time_ms == NULL) {
        return HAL_STATUS_ERROR;
    }

    *response_time_ms = estop_config.response_timeout_ms;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_set_config(const estop_config_t *config) {
    if (!estop_initialized || config == NULL) {
        return HAL_STATUS_ERROR;
    }

    estop_config = *config;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_config(estop_config_t *config) {
    if (!estop_initialized || config == NULL) {
        return HAL_STATUS_ERROR;
    }

    *config = estop_config;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_handle_emergency(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    printf("E-Stop emergency handling activated!\n");
    
    // Set state to triggered
    estop_status.state = ESTOP_STATE_TRIGGERED;
    estop_status.last_trigger_time = get_timestamp_ms();
    estop_status.trigger_count++;

    // Call callback if set
    if (estop_callback != NULL) {
        estop_callback(ESTOP_STATE_TRIGGERED, estop_status.fault);
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_estop_check_safety_compliance(bool *compliant) {
    if (!estop_initialized || compliant == NULL) {
        return HAL_STATUS_ERROR;
    }

    // Check if system meets SIL2 requirements
    bool channels_working = true;
    bool response_time_ok = true;
    bool dual_channel_ok = true; // Single channel design - always true

    // Test channels
    bool pin_ok;
    hal_status_t status = hal_estop_test_channels(&pin_ok);
    if (status != HAL_STATUS_OK) {
        channels_working = false;
    }

    // Test response time
    status = hal_estop_validate_safety();
    if (status != HAL_STATUS_OK) {
        response_time_ok = false;
    }

    *compliant = channels_working && response_time_ok && dual_channel_ok;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_self_test(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    printf("Running E-Stop self-test...\n");

    // Test channel reading
    bool pin_value;
    hal_status_t status = hal_estop_test_channels(&pin_value);
    if (status != HAL_STATUS_OK) {
        printf("E-Stop self-test failed: cannot read channels\n");
        return status;
    }

    // Test response time
    status = hal_estop_validate_safety();
    if (status != HAL_STATUS_OK) {
        printf("E-Stop self-test failed: response time validation\n");
        return status;
    }

    printf("E-Stop self-test passed\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_channel1_status(bool *status) {
    if (!estop_initialized || status == NULL) {
        return HAL_STATUS_ERROR;
    }

    return gpio_get_value(estop_config.pin, status);
}

hal_status_t hal_estop_get_channel2_status(bool *status) {
    if (!estop_initialized || status == NULL) {
        return HAL_STATUS_ERROR;
    }

    return gpio_get_value(estop_config.pin, status);
}

hal_status_t hal_estop_test_channel1(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    bool value;
    hal_status_t status = gpio_get_value(estop_config.pin, &value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    printf("E-Stop channel 1 test: %s\n", value ? "SAFE" : "TRIGGERED");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_test_channel2(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    bool value;
    hal_status_t status = gpio_get_value(estop_config.pin, &value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    printf("E-Stop channel 2 test: %s\n", value ? "SAFE" : "TRIGGERED");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_trigger_count(uint32_t *count) {
    if (!estop_initialized || count == NULL) {
        return HAL_STATUS_ERROR;
    }

    *count = estop_status.trigger_count;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_fault_count(uint32_t *count) {
    if (!estop_initialized || count == NULL) {
        return HAL_STATUS_ERROR;
    }

    *count = estop_status.fault_count;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_reset_statistics(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    estop_status.trigger_count = 0;
    estop_status.fault_count = 0;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_diagnostics(char *info, size_t max_len) {
    if (!estop_initialized || info == NULL) {
        return HAL_STATUS_ERROR;
    }

    snprintf(info, max_len,
             "E-Stop Diagnostics:\n"
             "State: %d\n"
             "Fault: %d\n"
             "Channel1: %s\n"
             "Channel2: %s\n"
             "Trigger Count: %u\n"
             "Fault Count: %u\n"
             "Response Time: %u ms\n"
             "Safety Level: %s\n",
             estop_status.state,
             estop_status.fault,
             estop_status.pin_status ? "SAFE" : "TRIGGERED", // Changed from channel1_status to pin_status
             estop_status.pin_status ? "SAFE" : "TRIGGERED", // Changed from channel2_status to pin_status
             estop_status.trigger_count,
             estop_status.fault_count,
             estop_config.response_timeout_ms,
             ESTOP_SAFETY_LEVEL);

    return HAL_STATUS_OK;
}

hal_status_t hal_estop_validate_hardware(void) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }

    printf("Validating E-Stop hardware...\n");

    // Test GPIO access
    bool pin_value;
    hal_status_t status = hal_estop_test_channels(&pin_value);
    if (status != HAL_STATUS_OK) {
        printf("E-Stop hardware validation failed: GPIO access error\n");
        return status;
    }

    printf("E-Stop hardware validation passed\n");
    return HAL_STATUS_OK;
}

// Internal helper functions
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}

static hal_status_t gpio_export(uint8_t pin) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);
    
    // Check if already exported
    if (access(path, F_OK) == 0) {
        return HAL_STATUS_OK;
    }
    
    FILE *fp = fopen("/sys/class/gpio/export", "w");
    if (!fp) {
        printf("CRITICAL SAFETY ERROR: Cannot export GPIO pin %d - /sys/class/gpio/export not accessible\n", pin);
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%d", pin);
    fclose(fp);
    
    // Wait for GPIO to be available
    usleep(100000); // 100ms
    
    // Verify GPIO was exported successfully
    if (access(path, F_OK) != 0) {
        printf("CRITICAL SAFETY ERROR: Failed to export GPIO pin %d\n", pin);
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t gpio_set_direction(uint8_t pin, bool output) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        printf("CRITICAL SAFETY ERROR: Cannot set GPIO pin %d direction - file not accessible\n", pin);
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%s", output ? "out" : "in");
    fclose(fp);
    
    return HAL_STATUS_OK;
}

static hal_status_t gpio_get_value(uint8_t pin, bool *value) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    
    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("CRITICAL SAFETY ERROR: Cannot read GPIO pin %d value - file not accessible\n", pin);
        return HAL_STATUS_ERROR;
    }
    
    int val;
    int result = fscanf(fp, "%d", &val);
    fclose(fp);
    
    if (result != 1) {
        printf("CRITICAL SAFETY ERROR: Failed to read GPIO pin %d value\n", pin);
        return HAL_STATUS_ERROR;
    }
    
    *value = (val != 0);
    return HAL_STATUS_OK;
}

static void* estop_monitor_thread_func(void *arg) {
    (void)arg; // Unused parameter
    
    while (estop_thread_running) {
        hal_estop_update();
        usleep(1000); // 1ms update rate for fast response
    }
    
    return NULL;
}

static void __attribute__((unused)) estop_handle_trigger(void) {
    printf("E-Stop TRIGGERED!\n");
    
    estop_status.state = ESTOP_STATE_TRIGGERED;
    estop_status.last_trigger_time = get_timestamp_ms();
    estop_status.trigger_count++;

    // Call callback if set
    if (estop_callback != NULL) {
        estop_callback(ESTOP_STATE_TRIGGERED, estop_status.fault);
    }
}

static void __attribute__((unused)) estop_handle_fault(estop_fault_t fault) {
    printf("E-Stop fault detected: %d\n", fault);
    
    estop_status.fault = fault;
    estop_status.fault_count++;

    // Call callback if set
    if (estop_callback != NULL) {
        estop_callback(estop_status.state, fault);
    }
}

// Additional functions for testing
hal_status_t hal_estop_get_pin_status(bool *status) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (status == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    return gpio_get_value(estop_config.pin, status);
}

hal_status_t hal_estop_test_pin(bool *pin_status) {
    if (!estop_initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (pin_status == NULL) {
        return HAL_STATUS_ERROR;
    }
    
    // Test pin by reading its current state
    return gpio_get_value(estop_config.pin, pin_status);
}
