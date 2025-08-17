#include "hal_relay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

// Relay configuration
static relay_config_t relay_config = {
    .output_pin = RELAY_OUTPUT_PIN,
    .voltage_v = RELAY_VOLTAGE,
    .current_max_a = RELAY_CURRENT_MAX,
    .pulse_duration_ms = 100,
    .pulse_interval_ms = 1000,
    .overcurrent_protection = true,
    .overtemperature_protection = true
};

// Relay status
static relay_status_t relay_status = {0};
static bool relay_initialized = false;
static pthread_t relay_monitor_thread;
static bool relay_thread_running = false;
static relay_event_callback_t relay_callback = NULL;

// Protection thresholds
static uint32_t overcurrent_threshold_ma = 2000; // 2A
static uint32_t overtemperature_threshold_c = 85; // 85°C

// Internal functions
static uint64_t get_timestamp_ms(void);
static hal_status_t gpio_export(uint8_t pin);
static hal_status_t gpio_set_direction(uint8_t pin, bool output);
static hal_status_t gpio_set_value(uint8_t pin, bool value);
static hal_status_t gpio_get_value(uint8_t pin, bool *value);
static void* relay_monitor_thread_func(void *arg);
static void relay_handle_fault(relay_fault_t fault);

hal_status_t hal_relay_init(const relay_config_t *config) {
    if (relay_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Initializing relay system...\n");

    // Copy configuration
    if (config != NULL) {
        relay_config = *config;
    }

    // Initialize status
    memset(&relay_status, 0, sizeof(relay_status));
    relay_status.state = RELAY_STATE_OFF;
    relay_status.fault = RELAY_FAULT_NONE;
    relay_status.output_status = false;
    relay_status.current_ma = 0;
    relay_status.voltage_mv = 0;
    relay_status.temperature_c = 25; // Default room temperature
    relay_status.last_switch_time = 0;
    relay_status.switch_count = 0;
    relay_status.fault_count = 0;

    // Export GPIO pin
    hal_status_t status = gpio_export(relay_config.output_pin);
    if (status != HAL_STATUS_OK) {
        printf("Failed to export relay GPIO pin %d\n", relay_config.output_pin);
        return status;
    }

    // Set GPIO direction (output)
    status = gpio_set_direction(relay_config.output_pin, true);
    if (status != HAL_STATUS_OK) {
        printf("Failed to set relay GPIO direction\n");
        return status;
    }

    // Set initial state (OFF)
    status = gpio_set_value(relay_config.output_pin, false);
    if (status != HAL_STATUS_OK) {
        printf("Failed to set relay initial state\n");
        return status;
    }

    // Start relay monitor thread
    relay_thread_running = true;
    if (pthread_create(&relay_monitor_thread, NULL, relay_monitor_thread_func, NULL) != 0) {
        printf("Failed to create relay monitor thread\n");
        return HAL_STATUS_ERROR;
    }

    relay_initialized = true;
    printf("Relay system initialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_deinit(void) {
    if (!relay_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Deinitializing relay system...\n");

    // Stop relay monitor thread
    relay_thread_running = false;
    pthread_join(relay_monitor_thread, NULL);

    // Turn off relay
    gpio_set_value(relay_config.output_pin, false);

    relay_initialized = false;
    printf("Relay system deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_state(relay_state_t state) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    relay_status.state = state;
    relay_status.last_switch_time = get_timestamp_ms();
    relay_status.switch_count++;

    switch (state) {
        case RELAY_STATE_OFF:
            return gpio_set_value(relay_config.output_pin, false);
        case RELAY_STATE_ON:
            return gpio_set_value(relay_config.output_pin, true);
        case RELAY_STATE_PULSE:
            // Pulse will be handled by the monitor thread
            return HAL_STATUS_OK;
        case RELAY_STATE_FAULT:
            // Turn off relay in fault state
            return gpio_set_value(relay_config.output_pin, false);
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }
}

hal_status_t hal_relay_get_state(relay_state_t *state) {
    if (!relay_initialized || state == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *state = relay_status.state;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_on(void) {
    return hal_relay_set_state(RELAY_STATE_ON);
}

hal_status_t hal_relay_off(void) {
    return hal_relay_set_state(RELAY_STATE_OFF);
}

hal_status_t hal_relay_toggle(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    bool current_value;
    hal_status_t status = gpio_get_value(relay_config.output_pin, &current_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    return hal_relay_set_state(current_value ? RELAY_STATE_OFF : RELAY_STATE_ON);
}

hal_status_t hal_relay_pulse(uint32_t duration_ms) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Relay pulse: %u ms\n", duration_ms);

    // Turn on relay
    hal_status_t status = hal_relay_on();
    if (status != HAL_STATUS_OK) {
        return status;
    }

    // Wait for duration
    usleep(duration_ms * 1000);

    // Turn off relay
    return hal_relay_off();
}

hal_status_t hal_relay_get_output_status(bool *status) {
    if (!relay_initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    return gpio_get_value(relay_config.output_pin, status);
}

hal_status_t hal_relay_get_fault(relay_fault_t *fault) {
    if (!relay_initialized || fault == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *fault = relay_status.fault;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_clear_fault(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Clearing relay fault...\n");
    relay_status.fault = RELAY_FAULT_NONE;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_status(relay_status_t *status) {
    if (!relay_initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *status = relay_status;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_callback(relay_event_callback_t callback) {
    // Allow setting callback even if not initialized (for testing)
    relay_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_update(void) {
    // This function is called periodically to update relay status
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    // Update output status
    bool output_value;
    hal_status_t status = gpio_get_value(relay_config.output_pin, &output_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    relay_status.output_status = output_value;

    // Check for faults
    bool overcurrent = false;
    bool overtemperature = false;

    if (relay_config.overcurrent_protection) {
        status = hal_relay_check_overcurrent(&overcurrent);
        if (status != HAL_STATUS_OK) {
            return status;
        }
    }

    if (relay_config.overtemperature_protection) {
        status = hal_relay_check_overtemperature(&overtemperature);
        if (status != HAL_STATUS_OK) {
            return status;
        }
    }

    // Handle faults
    if (overcurrent) {
        relay_handle_fault(RELAY_FAULT_OVERCURRENT);
    } else if (overtemperature) {
        relay_handle_fault(RELAY_FAULT_OVERTEMP);
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_config(const relay_config_t *config) {
    if (!relay_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    relay_config = *config;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_config(relay_config_t *config) {
    if (!relay_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *config = relay_config;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_current(uint32_t *current_ma) {
    if (!relay_initialized || current_ma == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Simulate current reading (in real implementation, read from ADC)
    *current_ma = relay_status.current_ma;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_voltage(uint32_t *voltage_mv) {
    if (!relay_initialized || voltage_mv == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Simulate voltage reading (in real implementation, read from ADC)
    *voltage_mv = relay_status.voltage_mv;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_temperature(uint32_t *temperature_c) {
    if (!relay_initialized || temperature_c == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Simulate temperature reading (in real implementation, read from sensor)
    *temperature_c = relay_status.temperature_c;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_check_overcurrent(bool *overcurrent) {
    if (!relay_initialized || overcurrent == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Check if current exceeds threshold
    *overcurrent = (relay_status.current_ma > overcurrent_threshold_ma);
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_check_overtemperature(bool *overtemperature) {
    if (!relay_initialized || overtemperature == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Check if temperature exceeds threshold
    *overtemperature = (relay_status.temperature_c > overtemperature_threshold_c);
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_overcurrent_protection(bool enabled) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    relay_config.overcurrent_protection = enabled;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_overtemperature_protection(bool enabled) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    relay_config.overtemperature_protection = enabled;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_overcurrent_threshold(uint32_t threshold_ma) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    overcurrent_threshold_ma = threshold_ma;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_overtemperature_threshold(uint32_t threshold_c) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    overtemperature_threshold_c = threshold_c;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_test(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Testing relay functionality...\n");

    // Test ON
    hal_status_t status = hal_relay_on();
    if (status != HAL_STATUS_OK) {
        printf("Relay test failed: cannot turn ON\n");
        return status;
    }
    usleep(500000); // 500ms

    // Test OFF
    status = hal_relay_off();
    if (status != HAL_STATUS_OK) {
        printf("Relay test failed: cannot turn OFF\n");
        return status;
    }
    usleep(500000); // 500ms

    printf("Relay test passed\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_test_pulse(uint32_t duration_ms) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Testing relay pulse: %u ms\n", duration_ms);
    return hal_relay_pulse(duration_ms);
}

hal_status_t hal_relay_validate_hardware(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Validating relay hardware...\n");

    // Test GPIO access
    bool value;
    hal_status_t status = gpio_get_value(relay_config.output_pin, &value);
    if (status != HAL_STATUS_OK) {
        printf("Relay hardware validation failed: GPIO access error\n");
        return status;
    }

    printf("Relay hardware validation passed\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_switch_count(uint32_t *count) {
    if (!relay_initialized || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *count = relay_status.switch_count;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_fault_count(uint32_t *count) {
    if (!relay_initialized || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *count = relay_status.fault_count;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_reset_statistics(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    relay_status.switch_count = 0;
    relay_status.fault_count = 0;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_diagnostics(char *info, size_t max_len) {
    if (!relay_initialized || info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    snprintf(info, max_len,
             "Relay Diagnostics:\n"
             "State: %d\n"
             "Fault: %d\n"
             "Output: %s\n"
             "Current: %u mA\n"
             "Voltage: %u mV\n"
             "Temperature: %u °C\n"
             "Switch Count: %u\n"
             "Fault Count: %u\n"
             "Type: %s\n",
             relay_status.state,
             relay_status.fault,
             relay_status.output_status ? "ON" : "OFF",
             relay_status.current_ma,
             relay_status.voltage_mv,
             relay_status.temperature_c,
             relay_status.switch_count,
             relay_status.fault_count,
             RELAY_TYPE);

    return HAL_STATUS_OK;
}

hal_status_t hal_relay_self_test(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Running relay self-test...\n");

    // Test basic functionality
    hal_status_t status = hal_relay_test();
    if (status != HAL_STATUS_OK) {
        printf("Relay self-test failed: basic functionality\n");
        return status;
    }

    // Test hardware validation
    status = hal_relay_validate_hardware();
    if (status != HAL_STATUS_OK) {
        printf("Relay self-test failed: hardware validation\n");
        return status;
    }

    printf("Relay self-test passed\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_emergency_shutdown(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Relay emergency shutdown activated!\n");
    
    // Turn off relay immediately
    hal_status_t status = hal_relay_off();
    if (status != HAL_STATUS_OK) {
        return status;
    }

    // Set fault state
    relay_status.state = RELAY_STATE_FAULT;
    relay_status.fault = RELAY_FAULT_OVERCURRENT; // Assume overcurrent fault

    // Call callback if set
    if (relay_callback != NULL) {
        relay_callback(RELAY_STATE_FAULT, RELAY_FAULT_OVERCURRENT);
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_relay_check_safety(bool *safe) {
    if (!relay_initialized || safe == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Check if relay is in safe state
    bool overcurrent, overtemperature;
    hal_status_t status = hal_relay_check_overcurrent(&overcurrent);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    status = hal_relay_check_overtemperature(&overtemperature);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    *safe = !overcurrent && !overtemperature && (relay_status.fault == RELAY_FAULT_NONE);
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

static hal_status_t gpio_set_value(uint8_t pin, bool value) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%d", value ? 1 : 0);
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

static void* relay_monitor_thread_func(void *arg) {
    (void)arg; // Unused parameter
    
    while (relay_thread_running) {
        hal_relay_update();
        usleep(10000); // 10ms update rate
    }
    
    return NULL;
}

static void relay_handle_fault(relay_fault_t fault) {
    printf("Relay fault detected: %d\n", fault);
    
    relay_status.fault = fault;
    relay_status.fault_count++;
    relay_status.state = RELAY_STATE_FAULT;

    // Turn off relay in fault state
    gpio_set_value(relay_config.output_pin, false);

    // Call callback if set
    if (relay_callback != NULL) {
        relay_callback(RELAY_STATE_FAULT, fault);
    }
}
