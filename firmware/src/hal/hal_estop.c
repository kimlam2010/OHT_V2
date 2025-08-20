#include "hal_estop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

// E-Stop configuration
static estop_config_t estop_config = {
    .channel1_pin = ESTOP_PIN,
    .channel2_pin = 0, // Single channel
    .response_timeout_ms = ESTOP_RESPONSE_TIME_MS,
    .debounce_time_ms = ESTOP_DEBOUNCE_TIME_MS,
    .dual_channel_required = false,
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
    if (estop_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Initializing E-Stop safety system...\n");

    // Copy configuration
    if (config != NULL) {
        estop_config = *config;
    }

    // Initialize status
    memset(&estop_status, 0, sizeof(estop_status));
    estop_status.state = ESTOP_STATE_SAFE;
    estop_status.fault = ESTOP_FAULT_NONE;
    estop_status.channel1_status = false;
    estop_status.channel2_status = false;
    estop_status.last_trigger_time = 0;
    estop_status.last_reset_time = 0;
    estop_status.trigger_count = 0;
    estop_status.fault_count = 0;

    // Export GPIO pin (single channel)
    hal_status_t status = gpio_export(estop_config.channel1_pin);
    if (status != HAL_STATUS_OK) {
        printf("Failed to export E-Stop GPIO pin %d\n", estop_config.channel1_pin);
        return status;
    }

    // Set GPIO direction (input)
    status = gpio_set_direction(estop_config.channel1_pin, false);
    if (status != HAL_STATUS_OK) {
        printf("Failed to set E-Stop direction\n");
        return status;
    }

    // Start E-Stop monitor thread
    estop_thread_running = true;
    if (pthread_create(&estop_monitor_thread, NULL, estop_monitor_thread_func, NULL) != 0) {
        printf("Failed to create E-Stop monitor thread\n");
        return HAL_STATUS_ERROR;
    }

    estop_initialized = true;
    printf("E-Stop safety system initialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_deinit(void) {
    if (!estop_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Deinitializing E-Stop safety system...\n");

    // Stop E-Stop monitor thread
    estop_thread_running = false;
    pthread_join(estop_monitor_thread, NULL);

    estop_initialized = false;
    printf("E-Stop safety system deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_state(estop_state_t *state) {
    if (!estop_initialized || state == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *state = estop_status.state;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_is_triggered(bool *triggered) {
    if (!estop_initialized || triggered == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *triggered = (estop_status.state == ESTOP_STATE_TRIGGERED);
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_reset(void) {
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Resetting E-Stop system...\n");

    // Check if both channels are safe
    bool channel1_safe, channel2_safe;
    hal_status_t status = gpio_get_value(estop_config.channel1_pin, &channel1_safe);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = gpio_get_value(estop_config.channel2_pin, &channel2_safe);
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
    status = gpio_get_value(estop_config.channel1_pin, &channel1_safe);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = gpio_get_value(estop_config.channel2_pin, &channel2_safe);
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
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *fault = estop_status.fault;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_clear_fault(void) {
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Clearing E-Stop fault...\n");
    estop_status.fault = ESTOP_FAULT_NONE;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_status(estop_status_t *status) {
    if (!estop_initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *status = estop_status;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_set_callback(estop_event_callback_t callback) {
    // Allow setting callback even if not initialized (for testing)
    estop_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_update(void) {
    // This function is called periodically to update E-Stop status
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    // Read channel status
    bool channel1_value, channel2_value;
    hal_status_t status = gpio_get_value(estop_config.channel1_pin, &channel1_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = gpio_get_value(estop_config.channel2_pin, &channel2_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    // Update status
    estop_status.channel1_status = channel1_value;
    estop_status.channel2_status = channel2_value;

    // Check for E-Stop trigger (active low)
    bool channel1_triggered = !channel1_value;
    bool channel2_triggered = !channel2_value;

    // Determine state based on channel configuration
    bool estop_triggered = false;
    if (estop_config.dual_channel_required) {
        // Both channels must be triggered for E-Stop
        estop_triggered = channel1_triggered && channel2_triggered;
    } else {
        // Either channel can trigger E-Stop
        estop_triggered = channel1_triggered || channel2_triggered;
    }

    // Handle state transitions
    if (estop_triggered && estop_status.state == ESTOP_STATE_SAFE) {
        estop_handle_trigger();
    } else if (!estop_triggered && estop_status.state == ESTOP_STATE_TRIGGERED) {
        // Auto-reset if enabled
        if (estop_config.auto_reset_enabled) {
            estop_status.state = ESTOP_STATE_SAFE;
            printf("E-Stop auto-reset: channels safe\n");
        }
    }

    // Check for faults
    if (estop_config.dual_channel_required) {
        if (channel1_triggered != channel2_triggered) {
            estop_handle_fault(ESTOP_FAULT_CHANNEL_MISMATCH);
        }
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_estop_test_channels(bool *channel1_status, bool *channel2_status) {
    if (!estop_initialized || channel1_status == NULL || channel2_status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    hal_status_t status = gpio_get_value(estop_config.channel1_pin, channel1_status);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = gpio_get_value(estop_config.channel2_pin, channel2_status);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_estop_validate_safety(void) {
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Validating E-Stop safety system...\n");

    // Test channel response time
    uint64_t start_time = get_timestamp_ms();
    bool channel1_value, channel2_value;
    
    hal_status_t status = gpio_get_value(estop_config.channel1_pin, &channel1_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = gpio_get_value(estop_config.channel2_pin, &channel2_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    uint64_t response_time = get_timestamp_ms() - start_time;
    
    if (response_time > estop_config.response_timeout_ms) {
        printf("E-Stop validation failed: response time %llu ms > %u ms\n", 
               response_time, estop_config.response_timeout_ms);
        return HAL_STATUS_ERROR;
    }

    printf("E-Stop safety validation passed: response time %llu ms\n", response_time);
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_response_time(uint32_t *response_time_ms) {
    if (!estop_initialized || response_time_ms == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *response_time_ms = estop_config.response_timeout_ms;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_set_config(const estop_config_t *config) {
    if (!estop_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    estop_config = *config;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_config(estop_config_t *config) {
    if (!estop_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *config = estop_config;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_handle_emergency(void) {
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
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
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Check if system meets SIL2 requirements
    bool channels_working = true;
    bool response_time_ok = true;
    bool dual_channel_ok = estop_config.dual_channel_required;

    // Test channels
    bool channel1_ok, channel2_ok;
    hal_status_t status = hal_estop_test_channels(&channel1_ok, &channel2_ok);
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
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Running E-Stop self-test...\n");

    // Test channel reading
    bool channel1_value, channel2_value;
    hal_status_t status = hal_estop_test_channels(&channel1_value, &channel2_value);
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
        return HAL_STATUS_INVALID_PARAMETER;
    }

    return gpio_get_value(estop_config.channel1_pin, status);
}

hal_status_t hal_estop_get_channel2_status(bool *status) {
    if (!estop_initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    return gpio_get_value(estop_config.channel2_pin, status);
}

hal_status_t hal_estop_test_channel1(void) {
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    bool value;
    hal_status_t status = gpio_get_value(estop_config.channel1_pin, &value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    printf("E-Stop channel 1 test: %s\n", value ? "SAFE" : "TRIGGERED");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_test_channel2(void) {
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    bool value;
    hal_status_t status = gpio_get_value(estop_config.channel2_pin, &value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    printf("E-Stop channel 2 test: %s\n", value ? "SAFE" : "TRIGGERED");
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_trigger_count(uint32_t *count) {
    if (!estop_initialized || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *count = estop_status.trigger_count;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_fault_count(uint32_t *count) {
    if (!estop_initialized || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *count = estop_status.fault_count;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_reset_statistics(void) {
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    estop_status.trigger_count = 0;
    estop_status.fault_count = 0;
    return HAL_STATUS_OK;
}

hal_status_t hal_estop_get_diagnostics(char *info, size_t max_len) {
    if (!estop_initialized || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
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
             estop_status.channel1_status ? "SAFE" : "TRIGGERED",
             estop_status.channel2_status ? "SAFE" : "TRIGGERED",
             estop_status.trigger_count,
             estop_status.fault_count,
             estop_config.response_timeout_ms,
             ESTOP_SAFETY_LEVEL);

    return HAL_STATUS_OK;
}

hal_status_t hal_estop_validate_hardware(void) {
    if (!estop_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Validating E-Stop hardware...\n");

    // Test GPIO access
    bool channel1_value, channel2_value;
    hal_status_t status = hal_estop_test_channels(&channel1_value, &channel2_value);
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
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%d", pin);
    fclose(fp);
    
    // Wait for GPIO to be available
    usleep(100000); // 100ms
    
    return HAL_STATUS_OK;
}

static hal_status_t gpio_set_direction(uint8_t pin, bool output) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
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
        return HAL_STATUS_ERROR;
    }
    
    int val;
    fscanf(fp, "%d", &val);
    fclose(fp);
    
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

static void estop_handle_trigger(void) {
    printf("E-Stop TRIGGERED!\n");
    
    estop_status.state = ESTOP_STATE_TRIGGERED;
    estop_status.last_trigger_time = get_timestamp_ms();
    estop_status.trigger_count++;

    // Call callback if set
    if (estop_callback != NULL) {
        estop_callback(ESTOP_STATE_TRIGGERED, estop_status.fault);
    }
}

static void estop_handle_fault(estop_fault_t fault) {
    printf("E-Stop fault detected: %d\n", fault);
    
    estop_status.fault = fault;
    estop_status.fault_count++;

    // Call callback if set
    if (estop_callback != NULL) {
        estop_callback(estop_status.state, fault);
    }
}
