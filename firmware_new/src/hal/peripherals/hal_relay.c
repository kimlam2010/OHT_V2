#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include "hal_relay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

// Relay configurations (dual relay support)
static relay_config_t relay1_config = {
    .output_pin = RELAY1_OUTPUT_PIN,
    .voltage_v = RELAY_VOLTAGE,
    .current_max_a = RELAY_CURRENT_MAX,
    .pulse_duration_ms = 100,
    .pulse_interval_ms = 1000,
    .overcurrent_protection = true,
    .overtemperature_protection = true
};

static relay_config_t relay2_config = {
    .output_pin = RELAY2_OUTPUT_PIN,
    .voltage_v = RELAY_VOLTAGE,
    .current_max_a = RELAY_CURRENT_MAX,
    .pulse_duration_ms = 100,
    .pulse_interval_ms = 1000,
    .overcurrent_protection = true,
    .overtemperature_protection = true
};

// Relay status (dual relay support)
static relay_status_t relay1_status = {0};
static relay_status_t relay2_status = {0};
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

    printf("Initializing dual relay system...\n");

    // Copy configuration (use relay1_config as primary)
    if (config != NULL) {
        relay1_config = *config;
        relay1_config.output_pin = RELAY1_OUTPUT_PIN; // Ensure correct pin
    }

    // Initialize relay 1 status
    memset(&relay1_status, 0, sizeof(relay_status_t));
    relay1_status.state = RELAY_STATE_OFF;
    relay1_status.fault = RELAY_FAULT_NONE;
    relay1_status.output_status = false;
    relay1_status.current_ma = 0;
    relay1_status.voltage_mv = 0;
    relay1_status.temperature_c = 25; // Default room temperature
    relay1_status.last_switch_time = 0;
    relay1_status.switch_count = 0;
    relay1_status.fault_count = 0;

    // Initialize relay 2 status
    memset(&relay2_status, 0, sizeof(relay_status_t));
    relay2_status.state = RELAY_STATE_OFF;
    relay2_status.fault = RELAY_FAULT_NONE;
    relay2_status.output_status = false;
    relay2_status.current_ma = 0;
    relay2_status.voltage_mv = 0;
    relay2_status.temperature_c = 25; // Default room temperature
    relay2_status.last_switch_time = 0;
    relay2_status.switch_count = 0;
    relay2_status.fault_count = 0;

    // Export GPIO pins (dual relay)
    hal_status_t status = gpio_export(relay1_config.output_pin);
    if (status != HAL_STATUS_OK) {
        printf("Failed to export relay 1 GPIO pin %d\n", relay1_config.output_pin);
        return status;
    }

    status = gpio_export(relay2_config.output_pin);
    if (status != HAL_STATUS_OK) {
        printf("Failed to export relay 2 GPIO pin %d\n", relay2_config.output_pin);
        return status;
    }

    // Set GPIO directions (output)
    status = gpio_set_direction(relay1_config.output_pin, true);
    if (status != HAL_STATUS_OK) {
        printf("Failed to set relay 1 GPIO direction\n");
        return status;
    }

    status = gpio_set_direction(relay2_config.output_pin, true);
    if (status != HAL_STATUS_OK) {
        printf("Failed to set relay 2 GPIO direction\n");
        return status;
    }

    // Set initial states (OFF)
    status = gpio_set_value(relay1_config.output_pin, false);
    if (status != HAL_STATUS_OK) {
        printf("Failed to set relay 1 initial state\n");
        return status;
    }

    status = gpio_set_value(relay2_config.output_pin, false);
    if (status != HAL_STATUS_OK) {
        printf("Failed to set relay 2 initial state\n");
        return status;
    }

    // Start relay monitor thread
    relay_thread_running = true;
    if (pthread_create(&relay_monitor_thread, NULL, relay_monitor_thread_func, NULL) != 0) {
        printf("Failed to create relay monitor thread\n");
        return HAL_STATUS_ERROR;
    }

    relay_initialized = true;
    printf("Dual relay system initialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_deinit(void) {
    if (!relay_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Deinitializing dual relay system...\n");

    // Stop relay monitor thread
    relay_thread_running = false;
    pthread_join(relay_monitor_thread, NULL);

    // Turn off both relays
    gpio_set_value(relay1_config.output_pin, false);
    gpio_set_value(relay2_config.output_pin, false);

    relay_initialized = false;
    printf("Dual relay system deinitialized\n");
    return HAL_STATUS_OK;
}

// Dual Relay Support Functions

hal_status_t hal_relay1_set_state(relay_state_t state) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    relay1_status.state = state;
    relay1_status.last_switch_time = get_timestamp_ms();
    relay1_status.switch_count++;
    
    bool output_value = (state == RELAY_STATE_ON);
    hal_status_t status = gpio_set_value(relay1_config.output_pin, output_value);
    if (status == HAL_STATUS_OK) {
        relay1_status.output_status = output_value;
    }
    
    return status;
}

hal_status_t hal_relay2_set_state(relay_state_t state) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    relay2_status.state = state;
    relay2_status.last_switch_time = get_timestamp_ms();
    relay2_status.switch_count++;
    
    bool output_value = (state == RELAY_STATE_ON);
    hal_status_t status = gpio_set_value(relay2_config.output_pin, output_value);
    if (status == HAL_STATUS_OK) {
        relay2_status.output_status = output_value;
    }
    
    return status;
}

hal_status_t hal_relay1_get_state(relay_state_t *state) {
    if (!relay_initialized || state == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *state = relay1_status.state;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay2_get_state(relay_state_t *state) {
    if (!relay_initialized || state == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *state = relay2_status.state;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay1_on(void) {
    return hal_relay1_set_state(RELAY_STATE_ON);
}

hal_status_t hal_relay2_on(void) {
    return hal_relay2_set_state(RELAY_STATE_ON);
}

hal_status_t hal_relay1_off(void) {
    return hal_relay1_set_state(RELAY_STATE_OFF);
}

hal_status_t hal_relay2_off(void) {
    return hal_relay2_set_state(RELAY_STATE_OFF);
}

bool hal_relay1_get_status(void) {
    return relay1_status.output_status;
}

bool hal_relay2_get_status(void) {
    return relay2_status.output_status;
}

// Legacy single relay functions (for backward compatibility)
hal_status_t hal_relay_set_state(relay_state_t state) {
    return hal_relay1_set_state(state); // Use relay 1 as primary
}

hal_status_t hal_relay_get_state(relay_state_t *state) {
    return hal_relay1_get_state(state);
}

hal_status_t hal_relay_on(void) {
    return hal_relay1_on();
}

hal_status_t hal_relay_off(void) {
    return hal_relay1_off();
}

hal_status_t hal_relay_toggle(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    bool current_value;
    hal_status_t status = gpio_get_value(relay1_config.output_pin, &current_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    return hal_relay1_set_state(current_value ? RELAY_STATE_OFF : RELAY_STATE_ON);
}

hal_status_t hal_relay_pulse(uint32_t duration_ms) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Relay 1 pulse: %u ms\n", duration_ms);

    // Turn on relay
    hal_status_t status = hal_relay1_on();
    if (status != HAL_STATUS_OK) {
        return status;
    }

    // Wait for duration
    usleep(duration_ms * 1000);

    // Turn off relay
    return hal_relay1_off();
}

hal_status_t hal_relay_get_output_status(bool *status) {
    if (!relay_initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    return gpio_get_value(relay1_config.output_pin, status);
}

hal_status_t hal_relay_get_fault(relay_fault_t *fault) {
    if (!relay_initialized || fault == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *fault = relay1_status.fault;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_clear_fault(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    relay1_status.fault = RELAY_FAULT_NONE;
    relay2_status.fault = RELAY_FAULT_NONE;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_status(relay_status_t *status) {
    if (!relay_initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *status = relay1_status; // Return relay 1 status as primary
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_update(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    // Update relay 1 status
    bool output_value;
    hal_status_t status = gpio_get_value(relay1_config.output_pin, &output_value);
    if (status == HAL_STATUS_OK) {
        relay1_status.output_status = output_value;
    }

    // Update relay 2 status
    status = gpio_get_value(relay2_config.output_pin, &output_value);
    if (status == HAL_STATUS_OK) {
        relay2_status.output_status = output_value;
    }

    return HAL_STATUS_OK;
}

// Additional functions for test compatibility

hal_status_t hal_relay_set_callback(relay_event_callback_t callback) {
    relay_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_current(uint32_t *current_ma) {
    if (!relay_initialized || current_ma == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *current_ma = relay1_status.current_ma;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_voltage(uint32_t *voltage_mv) {
    if (!relay_initialized || voltage_mv == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *voltage_mv = relay1_status.voltage_mv;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_temperature(uint32_t *temperature_c) {
    if (!relay_initialized || temperature_c == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *temperature_c = relay1_status.temperature_c;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_check_overcurrent(bool *overcurrent) {
    if (!relay_initialized || overcurrent == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *overcurrent = (relay1_status.current_ma > overcurrent_threshold_ma);
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_check_overtemperature(bool *overtemperature) {
    if (!relay_initialized || overtemperature == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *overtemperature = (relay1_status.temperature_c > overtemperature_threshold_c);
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_check_safety(bool *safe) {
    if (!relay_initialized || safe == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    bool overcurrent, overtemperature;
    hal_relay_check_overcurrent(&overcurrent);
    hal_relay_check_overtemperature(&overtemperature);
    *safe = !overcurrent && !overtemperature && (relay1_status.fault == RELAY_FAULT_NONE);
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_overcurrent_protection(bool enabled) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    relay1_config.overcurrent_protection = enabled;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_overtemperature_protection(bool enabled) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    relay1_config.overtemperature_protection = enabled;
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

hal_status_t hal_relay_get_config(relay_config_t *config) {
    if (!relay_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *config = relay1_config;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_set_config(const relay_config_t *config) {
    if (!relay_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    relay1_config = *config;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_validate_hardware(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    bool value;
    hal_status_t status = gpio_get_value(relay1_config.output_pin, &value);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_self_test(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("Relay self-test: Starting...\n");
    
    // Test relay 1
    hal_status_t status = hal_relay1_on();
    if (status != HAL_STATUS_OK) {
        printf("Relay self-test: Failed to turn on relay 1\n");
        return status;
    }
    
    usleep(100000); // 100ms
    
    status = hal_relay1_off();
    if (status != HAL_STATUS_OK) {
        printf("Relay self-test: Failed to turn off relay 1\n");
        return status;
    }
    
    // Test relay 2
    status = hal_relay2_on();
    if (status != HAL_STATUS_OK) {
        printf("Relay self-test: Failed to turn on relay 2\n");
        return status;
    }
    
    usleep(100000); // 100ms
    
    status = hal_relay2_off();
    if (status != HAL_STATUS_OK) {
        printf("Relay self-test: Failed to turn off relay 2\n");
        return status;
    }
    
    printf("Relay self-test: Passed\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_switch_count(uint32_t *count) {
    if (!relay_initialized || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *count = relay1_status.switch_count;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_fault_count(uint32_t *count) {
    if (!relay_initialized || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    *count = relay1_status.fault_count;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_reset_statistics(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    relay1_status.switch_count = 0;
    relay1_status.fault_count = 0;
    relay2_status.switch_count = 0;
    relay2_status.fault_count = 0;
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_get_diagnostics(char *diagnostics, size_t size) {
    if (!relay_initialized || diagnostics == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    snprintf(diagnostics, size,
             "Relay Diagnostics:\n"
             "Relay 1: State=%d, Fault=%d, Output=%s, Switch Count=%u, Fault Count=%u\n"
             "Relay 2: State=%d, Fault=%d, Output=%s, Switch Count=%u, Fault Count=%u\n"
             "Current: %u mA, Voltage: %u mV, Temperature: %u°C\n"
             "Overcurrent Protection: %s, Overtemperature Protection: %s\n",
             relay1_status.state, relay1_status.fault, 
             relay1_status.output_status ? "ON" : "OFF",
             relay1_status.switch_count, relay1_status.fault_count,
             relay2_status.state, relay2_status.fault,
             relay2_status.output_status ? "ON" : "OFF",
             relay2_status.switch_count, relay2_status.fault_count,
             relay1_status.current_ma, relay1_status.voltage_mv, relay1_status.temperature_c,
             relay1_config.overcurrent_protection ? "Enabled" : "Disabled",
             relay1_config.overtemperature_protection ? "Enabled" : "Disabled");
    
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_emergency_shutdown(void) {
    if (!relay_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    printf("Relay emergency shutdown: Turning off both relays\n");
    
    relay1_status.state = RELAY_STATE_FAULT;
    relay2_status.state = RELAY_STATE_FAULT;
    
    gpio_set_value(relay1_config.output_pin, false);
    gpio_set_value(relay2_config.output_pin, false);
    
    if (relay_callback != NULL) {
        relay_callback(RELAY_STATE_FAULT, RELAY_FAULT_NONE);
    }
    
    return HAL_STATUS_OK;
}

hal_status_t hal_relay_test_pulse(uint32_t duration_ms) {
    return hal_relay_pulse(duration_ms);
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
        printf("RELAY ERROR: Cannot export GPIO pin %d - /sys/class/gpio/export not accessible\n", pin);
        return HAL_STATUS_ERROR;
    }
    
    fprintf(fp, "%d", pin);
    fclose(fp);
    
    // Wait for GPIO to be available
    usleep(100000); // 100ms
    
    // Verify GPIO was exported successfully
    if (access(path, F_OK) != 0) {
        printf("RELAY ERROR: Failed to export GPIO pin %d\n", pin);
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

static hal_status_t gpio_set_direction(uint8_t pin, bool output) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    
    FILE *fp = fopen(path, "w");
    if (!fp) {
        printf("RELAY ERROR: Cannot set GPIO pin %d direction - file not accessible\n", pin);
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
        printf("RELAY ERROR: Cannot set GPIO pin %d value - file not accessible\n", pin);
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
        printf("RELAY ERROR: Cannot read GPIO pin %d value - file not accessible\n", pin);
        return HAL_STATUS_ERROR;
    }
    
    int val;
    int result = fscanf(fp, "%d", &val);
    fclose(fp);
    
    if (result != 1) {
        printf("RELAY ERROR: Failed to read GPIO pin %d value\n", pin);
        return HAL_STATUS_ERROR;
    }
    
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

__attribute__((unused))
static void relay_handle_fault(relay_fault_t fault) {
    printf("Relay fault detected: %d\n", fault);
    
    relay1_status.fault = fault;
    relay1_status.fault_count++;
    relay1_status.state = RELAY_STATE_FAULT;

    relay2_status.fault = fault;
    relay2_status.fault_count++;
    relay2_status.state = RELAY_STATE_FAULT;

    if (relay_callback != NULL) {
        relay_callback(RELAY_STATE_FAULT, fault);
    }
}
