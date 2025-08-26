/**
 * @file mock_gpio.h
 * @brief Mock GPIO functions for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#ifndef MOCK_GPIO_H
#define MOCK_GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Mock GPIO state
typedef struct {
    bool initialized;
    bool pins_configured[256];
    bool pin_values[256];
    bool pin_directions[256]; // true = output, false = input
    uint32_t pin_edges[256];
    uint32_t pin_bias[256];
    uint32_t pin_drive[256];
    bool pin_active_low[256];
    uint32_t pin_debounce[256];
    uint32_t export_count;
    uint32_t unexport_count;
    uint32_t write_count;
    uint32_t read_count;
    uint32_t configure_count;
    uint32_t set_edge_count;
    uint32_t set_bias_count;
    uint32_t set_drive_count;
} mock_gpio_state_t;

extern mock_gpio_state_t mock_gpio_state;

// Mock GPIO functions
void mock_gpio_reset(void);
bool mock_gpio_is_initialized(void);
bool mock_gpio_is_pin_configured(uint32_t pin);
bool mock_gpio_get_pin_value(uint32_t pin);
void mock_gpio_set_pin_value(uint32_t pin, bool value);
bool mock_gpio_get_pin_direction(uint32_t pin);
void mock_gpio_set_pin_direction(uint32_t pin, bool is_output);
uint32_t mock_gpio_get_pin_edge(uint32_t pin);
void mock_gpio_set_pin_edge(uint32_t pin, uint32_t edge);
uint32_t mock_gpio_get_pin_bias(uint32_t pin);
void mock_gpio_set_pin_bias(uint32_t pin, uint32_t bias);
uint32_t mock_gpio_get_pin_drive(uint32_t pin);
void mock_gpio_set_pin_drive(uint32_t pin, uint32_t drive);
bool mock_gpio_get_pin_active_low(uint32_t pin);
void mock_gpio_set_pin_active_low(uint32_t pin, bool active_low);
uint32_t mock_gpio_get_pin_debounce(uint32_t pin);
void mock_gpio_set_pin_debounce(uint32_t pin, uint32_t debounce);

// Mock file operations
int mock_gpio_export_pin(uint32_t pin);
int mock_gpio_unexport_pin(uint32_t pin);
int mock_gpio_set_direction(uint32_t pin, bool is_output);
int mock_gpio_set_value(uint32_t pin, bool value);
int mock_gpio_get_value(uint32_t pin, bool *value);
int mock_gpio_set_edge(uint32_t pin, const char *edge);
int mock_gpio_set_bias(uint32_t pin, const char *bias);
int mock_gpio_set_drive(uint32_t pin, const char *drive);

// Mock statistics
uint32_t mock_gpio_get_export_count(void);
uint32_t mock_gpio_get_unexport_count(void);
uint32_t mock_gpio_get_write_count(void);
uint32_t mock_gpio_get_read_count(void);
uint32_t mock_gpio_get_configure_count(void);
uint32_t mock_gpio_get_set_edge_count(void);
uint32_t mock_gpio_get_set_bias_count(void);
uint32_t mock_gpio_get_set_drive_count(void);

#endif // MOCK_GPIO_H
