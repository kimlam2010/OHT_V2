/**
 * OHT-50 Firmware - Hardware Abstraction Layer (HAL)
 * 
 * @file hal.h
 * @version 1.0.0
 * @date 2025-01-27
 * @author FW Team
 */

#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Error codes
#define HAL_OK 0
#define HAL_ERROR -1
#define HAL_TIMEOUT -2
#define HAL_INVALID_PARAM -3

// GPIO definitions
typedef enum {
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT = 1
} gpio_direction_t;

typedef enum {
    GPIO_EDGE_NONE = 0,
    GPIO_EDGE_RISING = 1,
    GPIO_EDGE_FALLING = 2,
    GPIO_EDGE_BOTH = 3
} gpio_edge_t;

typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_t;

// PWM definitions
typedef struct {
    uint32_t frequency_hz;    // PWM frequency in Hz
    uint32_t duty_cycle;      // Duty cycle (0-10000 for 0-100%)
    uint32_t dead_time_ns;    // Dead time in nanoseconds
} pwm_config_t;

// ADC definitions
typedef struct {
    uint32_t sampling_rate_hz;  // Sampling rate in Hz
    uint32_t resolution_bits;   // ADC resolution in bits
    float reference_voltage;    // Reference voltage
} adc_config_t;

// UART definitions
typedef enum {
    UART_BAUD_9600 = 9600,
    UART_BAUD_19200 = 19200,
    UART_BAUD_38400 = 38400,
    UART_BAUD_57600 = 57600,
    UART_BAUD_115200 = 115200
} uart_baudrate_t;

typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 1,
    UART_PARITY_ODD = 2
} uart_parity_t;

typedef enum {
    UART_STOP_1 = 1,
    UART_STOP_2 = 2
} uart_stop_bits_t;

typedef struct {
    uart_baudrate_t baudrate;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
    uint32_t data_bits;
    uint32_t timeout_ms;
} uart_config_t;

// Timer definitions
typedef enum {
    TIMER_MODE_PERIODIC = 0,
    TIMER_MODE_ONE_SHOT = 1,
    TIMER_MODE_CAPTURE = 2
} timer_mode_t;

typedef struct {
    timer_mode_t mode;
    uint32_t period_us;        // Period in microseconds
    uint32_t prescaler;        // Timer prescaler
} timer_config_t;

// RS485 definitions
typedef enum {
    RS485_MODE_RECEIVE = 0,
    RS485_MODE_TRANSMIT = 1
} rs485_mode_t;

typedef struct {
    uart_config_t uart_config;
    uint32_t de_delay_us;      // DE delay in microseconds
    uint32_t re_delay_us;      // RE delay in microseconds
    bool auto_mode_switch;     // Auto mode switching
} rs485_config_t;

// Function prototypes

// HAL initialization and cleanup
int hal_init(void);
void hal_cleanup(void);

// GPIO functions
int gpio_init(uint32_t pin);
int gpio_deinit(uint32_t pin);
int gpio_set_direction(uint32_t pin, gpio_direction_t direction);
int gpio_set_pull(uint32_t pin, gpio_pull_t pull);
int gpio_set_edge(uint32_t pin, gpio_edge_t edge);
int gpio_read(uint32_t pin, bool* value);
int gpio_write(uint32_t pin, bool value);
int gpio_set_debounce(uint32_t pin, uint32_t debounce_ms);
int gpio_wait_for_edge(uint32_t pin, gpio_edge_t edge, uint32_t timeout_ms);

// PWM functions
int pwm_init(uint32_t channel);
int pwm_deinit(uint32_t channel);
int pwm_set_config(uint32_t channel, const pwm_config_t* config);
int pwm_start(uint32_t channel);
int pwm_stop(uint32_t channel);
int pwm_set_frequency(uint32_t channel, uint32_t frequency_hz);
int pwm_set_duty_cycle(uint32_t channel, uint32_t duty_cycle);
int pwm_set_dead_time(uint32_t channel, uint32_t dead_time_ns);

// ADC functions
int adc_init(uint32_t channel);
int adc_deinit(uint32_t channel);
int adc_set_config(uint32_t channel, const adc_config_t* config);
int adc_read(uint32_t channel, uint32_t* value);
int adc_read_voltage(uint32_t channel, float* voltage);
int adc_start_continuous(uint32_t channel);
int adc_stop_continuous(uint32_t channel);
int adc_set_callback(uint32_t channel, void (*callback)(uint32_t channel, uint32_t value));

// UART functions
int uart_init(uint32_t uart_id);
int uart_deinit(uint32_t uart_id);
int uart_set_config(uint32_t uart_id, const uart_config_t* config);
int uart_write(uint32_t uart_id, const uint8_t* data, uint32_t length);
int uart_read(uint32_t uart_id, uint8_t* data, uint32_t length);
int uart_read_timeout(uint32_t uart_id, uint8_t* data, uint32_t length, uint32_t timeout_ms);
int uart_get_rx_count(uint32_t uart_id, uint32_t* count);
int uart_get_tx_count(uint32_t uart_id, uint32_t* count);
int uart_flush_rx(uint32_t uart_id);
int uart_flush_tx(uint32_t uart_id);

// Timer functions
int timer_init(uint32_t timer_id);
int timer_deinit(uint32_t timer_id);
int timer_set_config(uint32_t timer_id, const timer_config_t* config);
int timer_start(uint32_t timer_id);
int timer_stop(uint32_t timer_id);
int timer_reset(uint32_t timer_id);
int timer_get_count(uint32_t timer_id, uint32_t* count);
int timer_set_callback(uint32_t timer_id, void (*callback)(uint32_t timer_id));
int timer_capture_start(uint32_t timer_id);
int timer_capture_stop(uint32_t timer_id);
int timer_get_capture_value(uint32_t timer_id, uint32_t* value);

// RS485 functions
int rs485_init(uint32_t uart_id);
int rs485_deinit(uint32_t uart_id);
int rs485_set_config(uint32_t uart_id, const rs485_config_t* config);
int rs485_set_mode(uint32_t uart_id, rs485_mode_t mode);
int rs485_write(uint32_t uart_id, const uint8_t* data, uint32_t length);
int rs485_read(uint32_t uart_id, uint8_t* data, uint32_t length);
int rs485_read_timeout(uint32_t uart_id, uint8_t* data, uint32_t length, uint32_t timeout_ms);
int rs485_get_mode(uint32_t uart_id, rs485_mode_t* mode);
int rs485_set_de_pin(uint32_t uart_id, uint32_t de_pin);
int rs485_set_re_pin(uint32_t uart_id, uint32_t re_pin);

// Utility functions
uint64_t hal_get_time_us(void);
uint32_t hal_get_time_ms(void);
void hal_delay_us(uint32_t microseconds);
void hal_delay_ms(uint32_t milliseconds);

// Error handling
const char* hal_get_error_string(int error_code);
void hal_set_error_callback(void (*callback)(int error_code, const char* message));

#ifdef __cplusplus
}
#endif

#endif // HAL_H
