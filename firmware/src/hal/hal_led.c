#include "hal_led.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

// LED configuration
static led_config_t led_configs[5] = {
    {LED_POWER_PIN, LED_STATE_OFF, LED_PATTERN_NONE, 1000, 500},    // Power LED
    {LED_SYSTEM_PIN, LED_STATE_OFF, LED_PATTERN_NONE, 1000, 500},   // System LED
    {LED_COMM_PIN, LED_STATE_OFF, LED_PATTERN_NONE, 1000, 500},     // Comm LED
    {LED_NETWORK_PIN, LED_STATE_OFF, LED_PATTERN_NONE, 1000, 500},  // Network LED
    {LED_ERROR_PIN, LED_STATE_OFF, LED_PATTERN_NONE, 1000, 500}     // Error LED
};

// LED status tracking
static led_status_t led_status[5];
static bool led_initialized = false;
static pthread_t led_update_thread;
static bool led_thread_running = false;

// Internal functions
static uint64_t get_timestamp_ms(void);
static hal_status_t gpio_export(uint8_t pin);
static hal_status_t gpio_set_direction(uint8_t pin, bool output);
static hal_status_t gpio_set_value(uint8_t pin, bool value);
static hal_status_t gpio_get_value(uint8_t pin, bool *value);
static void* led_update_thread_func(void *arg);

hal_status_t hal_led_init(void) {
    if (led_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Initializing LED system...\n");

    // Initialize LED status
    memset(led_status, 0, sizeof(led_status));
    
    for (int i = 0; i < 5; i++) {
        led_status[i].initialized = false;
        led_status[i].current_state = LED_STATE_OFF;
        led_status[i].current_pattern = LED_PATTERN_NONE;
        led_status[i].last_toggle_time = 0;
        led_status[i].led_value = false;
    }

    // Export GPIO pins
    for (int i = 0; i < 5; i++) {
        hal_status_t status = gpio_export(led_configs[i].pin);
        if (status != HAL_STATUS_OK) {
            printf("Failed to export GPIO pin %d\n", led_configs[i].pin);
            return status;
        }

        status = gpio_set_direction(led_configs[i].pin, true);
        if (status != HAL_STATUS_OK) {
            printf("Failed to set GPIO pin %d direction\n", led_configs[i].pin);
            return status;
        }

        led_status[i].initialized = true;
    }

    // Start LED update thread
    led_thread_running = true;
    if (pthread_create(&led_update_thread, NULL, led_update_thread_func, NULL) != 0) {
        printf("Failed to create LED update thread\n");
        return HAL_STATUS_ERROR;
    }

    led_initialized = true;
    printf("LED system initialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_led_deinit(void) {
    if (!led_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Deinitializing LED system...\n");

    // Stop LED update thread
    led_thread_running = false;
    pthread_join(led_update_thread, NULL);

    // Turn off all LEDs
    for (int i = 0; i < 5; i++) {
        if (led_status[i].initialized) {
            gpio_set_value(led_configs[i].pin, false);
        }
    }

    led_initialized = false;
    printf("LED system deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_led_set_state(uint8_t pin, led_state_t state) {
    if (!led_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    // Find LED index
    int led_index = -1;
    for (int i = 0; i < 5; i++) {
        if (led_configs[i].pin == pin) {
            led_index = i;
            break;
        }
    }

    if (led_index == -1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    led_status[led_index].current_state = state;
    
    // Handle immediate state changes
    switch (state) {
        case LED_STATE_ON:
            return gpio_set_value(pin, true);
        case LED_STATE_OFF:
            return gpio_set_value(pin, false);
        case LED_STATE_BLINK_SLOW:
        case LED_STATE_BLINK_FAST:
        case LED_STATE_PULSE:
            // These will be handled by the update thread
            return HAL_STATUS_OK;
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }
}

hal_status_t hal_led_get_state(uint8_t pin, led_state_t *state) {
    if (!led_initialized || state == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Find LED index
    int led_index = -1;
    for (int i = 0; i < 5; i++) {
        if (led_configs[i].pin == pin) {
            led_index = i;
            break;
        }
    }

    if (led_index == -1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *state = led_status[led_index].current_state;
    return HAL_STATUS_OK;
}

hal_status_t hal_led_set_pattern(uint8_t pin, led_pattern_t pattern) {
    if (!led_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    // Find LED index
    int led_index = -1;
    for (int i = 0; i < 5; i++) {
        if (led_configs[i].pin == pin) {
            led_index = i;
            break;
        }
    }

    if (led_index == -1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    led_status[led_index].current_pattern = pattern;
    led_status[led_index].last_toggle_time = get_timestamp_ms();
    
    return HAL_STATUS_OK;
}

hal_status_t hal_led_get_pattern(uint8_t pin, led_pattern_t *pattern) {
    if (!led_initialized || pattern == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Find LED index
    int led_index = -1;
    for (int i = 0; i < 5; i++) {
        if (led_configs[i].pin == pin) {
            led_index = i;
            break;
        }
    }

    if (led_index == -1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *pattern = led_status[led_index].current_pattern;
    return HAL_STATUS_OK;
}

hal_status_t hal_led_on(uint8_t pin) {
    return hal_led_set_state(pin, LED_STATE_ON);
}

hal_status_t hal_led_off(uint8_t pin) {
    return hal_led_set_state(pin, LED_STATE_OFF);
}

hal_status_t hal_led_toggle(uint8_t pin) {
    if (!led_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    bool current_value;
    hal_status_t status = gpio_get_value(pin, &current_value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    return gpio_set_value(pin, !current_value);
}

hal_status_t hal_led_set_brightness(uint8_t pin, uint8_t brightness) {
    // For now, implement as simple on/off based on threshold
    if (brightness > 50) {
        return hal_led_on(pin);
    } else {
        return hal_led_off(pin);
    }
}

hal_status_t hal_led_get_brightness(uint8_t pin, uint8_t *brightness) {
    if (!led_initialized || brightness == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    bool value;
    hal_status_t status = gpio_get_value(pin, &value);
    if (status != HAL_STATUS_OK) {
        return status;
    }

    *brightness = value ? 100 : 0;
    return HAL_STATUS_OK;
}

hal_status_t hal_led_update(void) {
    // This function is called periodically to update LED patterns
    if (!led_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    uint64_t current_time = get_timestamp_ms();

    for (int i = 0; i < 5; i++) {
        if (!led_status[i].initialized) {
            continue;
        }

        uint32_t period_ms = 0;
        bool should_toggle = false;

        // Determine pattern behavior
        switch (led_status[i].current_pattern) {
            case LED_PATTERN_BLINK_SLOW:
                period_ms = 1000; // 1Hz
                break;
            case LED_PATTERN_BLINK_FAST:
                period_ms = 200;  // 5Hz
                break;
            case LED_PATTERN_PULSE:
                period_ms = 500;  // 2Hz
                break;
            case LED_PATTERN_ERROR:
                period_ms = 100;  // 10Hz
                break;
            case LED_PATTERN_WARNING:
                period_ms = 500;  // 2Hz
                break;
            default:
                continue;
        }

        // Check if it's time to toggle
        if (current_time - led_status[i].last_toggle_time >= period_ms) {
            should_toggle = true;
            led_status[i].last_toggle_time = current_time;
        }

        if (should_toggle) {
            led_status[i].led_value = !led_status[i].led_value;
            gpio_set_value(led_configs[i].pin, led_status[i].led_value);
        }
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_led_get_status(uint8_t pin, led_status_t *status) {
    if (!led_initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Find LED index
    int led_index = -1;
    for (int i = 0; i < 5; i++) {
        if (led_configs[i].pin == pin) {
            led_index = i;
            break;
        }
    }

    if (led_index == -1) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *status = led_status[led_index];
    return HAL_STATUS_OK;
}

// Convenience functions
hal_status_t hal_led_power_set(led_state_t state) {
    return hal_led_set_state(LED_POWER_PIN, state);
}

hal_status_t hal_led_system_set(led_state_t state) {
    return hal_led_set_state(LED_SYSTEM_PIN, state);
}

hal_status_t hal_led_comm_set(led_state_t state) {
    return hal_led_set_state(LED_COMM_PIN, state);
}

hal_status_t hal_led_network_set(led_state_t state) {
    return hal_led_set_state(LED_NETWORK_PIN, state);
}

hal_status_t hal_led_error_set(led_state_t state) {
    return hal_led_set_state(LED_ERROR_PIN, state);
}

// System pattern functions
hal_status_t hal_led_system_ready(void) {
    hal_status_t status;
    
    status = hal_led_power_set(LED_STATE_ON);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_system_set(LED_STATE_BLINK_SLOW);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_comm_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_network_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_error_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_led_system_error(void) {
    hal_status_t status;
    
    status = hal_led_power_set(LED_STATE_ON);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_system_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_comm_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_network_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_error_set(LED_STATE_BLINK_FAST);
    if (status != HAL_STATUS_OK) return status;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_led_system_warning(void) {
    hal_status_t status;
    
    status = hal_led_power_set(LED_STATE_ON);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_system_set(LED_STATE_BLINK_FAST);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_comm_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_network_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_error_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_led_system_shutdown(void) {
    hal_status_t status;
    
    status = hal_led_power_set(LED_STATE_BLINK_SLOW);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_system_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_comm_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_network_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
    status = hal_led_error_set(LED_STATE_OFF);
    if (status != HAL_STATUS_OK) return status;
    
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
    (void)fscanf(fp, "%d", &val);
    fclose(fp);
    
    *value = (val != 0);
    return HAL_STATUS_OK;
}

static void* led_update_thread_func(void *arg) {
    (void)arg; // Unused parameter
    
    while (led_thread_running) {
        hal_led_update();
        usleep(10000); // 10ms update rate
    }
    
    return NULL;
}
