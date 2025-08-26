/**
 * @file mock_gpio.c
 * @brief Mock GPIO functions implementation for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#include "mock_gpio.h"
#include <string.h>

// Global mock GPIO state
mock_gpio_state_t mock_gpio_state = {0};

void mock_gpio_reset(void) {
    memset(&mock_gpio_state, 0, sizeof(mock_gpio_state_t));
}

bool mock_gpio_is_initialized(void) {
    return mock_gpio_state.initialized;
}

bool mock_gpio_is_pin_configured(uint32_t pin) {
    if (pin >= 256) return false;
    return mock_gpio_state.pins_configured[pin];
}

bool mock_gpio_get_pin_value(uint32_t pin) {
    if (pin >= 256) return false;
    return mock_gpio_state.pin_values[pin];
}

void mock_gpio_set_pin_value(uint32_t pin, bool value) {
    if (pin >= 256) return;
    mock_gpio_state.pin_values[pin] = value;
    mock_gpio_state.write_count++;
}

bool mock_gpio_get_pin_direction(uint32_t pin) {
    if (pin >= 256) return false;
    return mock_gpio_state.pin_directions[pin];
}

void mock_gpio_set_pin_direction(uint32_t pin, bool is_output) {
    if (pin >= 256) return;
    mock_gpio_state.pin_directions[pin] = is_output;
}

uint32_t mock_gpio_get_pin_edge(uint32_t pin) {
    if (pin >= 256) return 0;
    return mock_gpio_state.pin_edges[pin];
}

void mock_gpio_set_pin_edge(uint32_t pin, uint32_t edge) {
    if (pin >= 256) return;
    mock_gpio_state.pin_edges[pin] = edge;
    mock_gpio_state.set_edge_count++;
}

uint32_t mock_gpio_get_pin_bias(uint32_t pin) {
    if (pin >= 256) return 0;
    return mock_gpio_state.pin_bias[pin];
}

void mock_gpio_set_pin_bias(uint32_t pin, uint32_t bias) {
    if (pin >= 256) return;
    mock_gpio_state.pin_bias[pin] = bias;
    mock_gpio_state.set_bias_count++;
}

uint32_t mock_gpio_get_pin_drive(uint32_t pin) {
    if (pin >= 256) return 0;
    return mock_gpio_state.pin_drive[pin];
}

void mock_gpio_set_pin_drive(uint32_t pin, uint32_t drive) {
    if (pin >= 256) return;
    mock_gpio_state.pin_drive[pin] = drive;
    mock_gpio_state.set_drive_count++;
}

bool mock_gpio_get_pin_active_low(uint32_t pin) {
    if (pin >= 256) return false;
    return mock_gpio_state.pin_active_low[pin];
}

void mock_gpio_set_pin_active_low(uint32_t pin, bool active_low) {
    if (pin >= 256) return;
    mock_gpio_state.pin_active_low[pin] = active_low;
}

uint32_t mock_gpio_get_pin_debounce(uint32_t pin) {
    if (pin >= 256) return 0;
    return mock_gpio_state.pin_debounce[pin];
}

void mock_gpio_set_pin_debounce(uint32_t pin, uint32_t debounce) {
    if (pin >= 256) return;
    mock_gpio_state.pin_debounce[pin] = debounce;
}

// Mock file operations
int mock_gpio_export_pin(uint32_t pin) {
    if (pin >= 256) return -1;
    mock_gpio_state.pins_configured[pin] = true;
    mock_gpio_state.export_count++;
    return 0;
}

int mock_gpio_unexport_pin(uint32_t pin) {
    if (pin >= 256) return -1;
    mock_gpio_state.pins_configured[pin] = false;
    mock_gpio_state.unexport_count++;
    return 0;
}

int mock_gpio_set_direction(uint32_t pin, bool is_output) {
    if (pin >= 256) return -1;
    mock_gpio_state.pin_directions[pin] = is_output;
    return 0;
}

int mock_gpio_set_value(uint32_t pin, bool value) {
    if (pin >= 256) return -1;
    mock_gpio_state.pin_values[pin] = value;
    mock_gpio_state.write_count++;
    return 0;
}

int mock_gpio_get_value(uint32_t pin, bool *value) {
    if (pin >= 256 || value == NULL) return -1;
    *value = mock_gpio_state.pin_values[pin];
    mock_gpio_state.read_count++;
    return 0;
}

int mock_gpio_set_edge(uint32_t pin, const char *edge) {
    if (pin >= 256 || edge == NULL) return -1;
    // Store edge as integer for simplicity
    if (strcmp(edge, "rising") == 0) {
        mock_gpio_state.pin_edges[pin] = 1;
    } else if (strcmp(edge, "falling") == 0) {
        mock_gpio_state.pin_edges[pin] = 2;
    } else if (strcmp(edge, "both") == 0) {
        mock_gpio_state.pin_edges[pin] = 3;
    } else {
        mock_gpio_state.pin_edges[pin] = 0; // none
    }
    mock_gpio_state.set_edge_count++;
    return 0;
}

int mock_gpio_set_bias(uint32_t pin, const char *bias) {
    if (pin >= 256 || bias == NULL) return -1;
    // Store bias as integer for simplicity
    if (strcmp(bias, "pull-up") == 0) {
        mock_gpio_state.pin_bias[pin] = 1;
    } else if (strcmp(bias, "pull-down") == 0) {
        mock_gpio_state.pin_bias[pin] = 2;
    } else {
        mock_gpio_state.pin_bias[pin] = 0; // disable
    }
    mock_gpio_state.set_bias_count++;
    return 0;
}

int mock_gpio_set_drive(uint32_t pin, const char *drive) {
    if (pin >= 256 || drive == NULL) return -1;
    // Store drive as integer for simplicity
    if (strcmp(drive, "2ma") == 0) {
        mock_gpio_state.pin_drive[pin] = 2;
    } else if (strcmp(drive, "4ma") == 0) {
        mock_gpio_state.pin_drive[pin] = 4;
    } else if (strcmp(drive, "8ma") == 0) {
        mock_gpio_state.pin_drive[pin] = 8;
    } else {
        mock_gpio_state.pin_drive[pin] = 2; // default
    }
    mock_gpio_state.set_drive_count++;
    return 0;
}

// Mock statistics
uint32_t mock_gpio_get_export_count(void) {
    return mock_gpio_state.export_count;
}

uint32_t mock_gpio_get_unexport_count(void) {
    return mock_gpio_state.unexport_count;
}

uint32_t mock_gpio_get_write_count(void) {
    return mock_gpio_state.write_count;
}

uint32_t mock_gpio_get_read_count(void) {
    return mock_gpio_state.read_count;
}

uint32_t mock_gpio_get_configure_count(void) {
    return mock_gpio_state.configure_count;
}

uint32_t mock_gpio_get_set_edge_count(void) {
    return mock_gpio_state.set_edge_count;
}

uint32_t mock_gpio_get_set_bias_count(void) {
    return mock_gpio_state.set_bias_count;
}

uint32_t mock_gpio_get_set_drive_count(void) {
    return mock_gpio_state.set_drive_count;
}
