#ifndef HAL_RELAY_H
#define HAL_RELAY_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// Relay GPIO Pin Definitions (updated for available pins)
#define RELAY1_OUTPUT_PIN      131 // GPIO4_A3 - Relay 1 output (freed from E-Stop)
#define RELAY2_OUTPUT_PIN      132 // GPIO4_A4 - Relay 2 output

// Relay Specifications
#define RELAY_VOLTAGE          24  // V DC
#define RELAY_CURRENT_MAX      2   // A
#define RELAY_TYPE             "Solid-State"

// Relay States
typedef enum {
    RELAY_STATE_OFF = 0,
    RELAY_STATE_ON,
    RELAY_STATE_PULSE,        // Pulse mode
    RELAY_STATE_FAULT         // Fault state
} relay_state_t;

// Relay Fault Types
typedef enum {
    RELAY_FAULT_NONE = 0,
    RELAY_FAULT_OVERCURRENT,   // Overcurrent detected
    RELAY_FAULT_OVERVOLTAGE,   // Overvoltage detected
    RELAY_FAULT_OVERTEMP,      // Overtemperature detected
    RELAY_FAULT_SHORT_CIRCUIT, // Short circuit detected
    RELAY_FAULT_OPEN_CIRCUIT   // Open circuit detected
} relay_fault_t;

// Relay Configuration Structure
typedef struct {
    uint8_t output_pin;
    uint32_t voltage_v;
    uint32_t current_max_a;
    uint32_t pulse_duration_ms;
    uint32_t pulse_interval_ms;
    bool overcurrent_protection;
    bool overtemperature_protection;
} relay_config_t;

// Relay Status Structure
typedef struct {
    relay_state_t state;
    relay_fault_t fault;
    bool output_status;
    uint32_t current_ma;
    uint32_t voltage_mv;
    uint32_t temperature_c;
    uint64_t last_switch_time;
    uint32_t switch_count;
    uint32_t fault_count;
} relay_status_t;

// Relay Event Callback
typedef void (*relay_event_callback_t)(relay_state_t state, relay_fault_t fault);

// Function Prototypes

/**
 * @brief Initialize relay system
 * @param config Relay configuration
 * @return HAL status
 */
hal_status_t hal_relay_init(const relay_config_t *config);

/**
 * @brief Deinitialize relay system
 * @return HAL status
 */
hal_status_t hal_relay_deinit(void);

/**
 * @brief Set relay state
 * @param state Relay state
 * @return HAL status
 */
hal_status_t hal_relay_set_state(relay_state_t state);

/**
 * @brief Get relay state
 * @param state Pointer to store relay state
 * @return HAL status
 */
hal_status_t hal_relay_get_state(relay_state_t *state);

/**
 * @brief Turn relay on
 * @return HAL status
 */
hal_status_t hal_relay_on(void);

/**
 * @brief Turn relay off
 * @return HAL status
 */
hal_status_t hal_relay_off(void);

// Dual Relay Support Functions

/**
 * @brief Set relay 1 state
 * @param state Relay state
 * @return HAL status
 */
hal_status_t hal_relay1_set_state(relay_state_t state);

/**
 * @brief Set relay 2 state
 * @param state Relay state
 * @return HAL status
 */
hal_status_t hal_relay2_set_state(relay_state_t state);

/**
 * @brief Get relay 1 state
 * @param state Pointer to store relay state
 * @return HAL status
 */
hal_status_t hal_relay1_get_state(relay_state_t *state);

/**
 * @brief Get relay 2 state
 * @param state Pointer to store relay state
 * @return HAL status
 */
hal_status_t hal_relay2_get_state(relay_state_t *state);

/**
 * @brief Turn relay 1 on
 * @return HAL status
 */
hal_status_t hal_relay1_on(void);

/**
 * @brief Turn relay 2 on
 * @return HAL status
 */
hal_status_t hal_relay2_on(void);

/**
 * @brief Turn relay 1 off
 * @return HAL status
 */
hal_status_t hal_relay1_off(void);

/**
 * @brief Turn relay 2 off
 * @return HAL status
 */
hal_status_t hal_relay2_off(void);

/**
 * @brief Get relay 1 status
 * @return true if relay 1 is on, false otherwise
 */
bool hal_relay1_get_status(void);

/**
 * @brief Get relay 2 status
 * @return true if relay 2 is on, false otherwise
 */
bool hal_relay2_get_status(void);

/**
 * @brief Toggle relay
 * @return HAL status
 */
hal_status_t hal_relay_toggle(void);

/**
 * @brief Pulse relay
 * @param duration_ms Pulse duration in milliseconds
 * @return HAL status
 */
hal_status_t hal_relay_pulse(uint32_t duration_ms);

/**
 * @brief Get relay output status
 * @param status Pointer to store output status
 * @return HAL status
 */
hal_status_t hal_relay_get_output_status(bool *status);

/**
 * @brief Get relay fault status
 * @param fault Pointer to store fault type
 * @return HAL status
 */
hal_status_t hal_relay_get_fault(relay_fault_t *fault);

/**
 * @brief Clear relay fault
 * @return HAL status
 */
hal_status_t hal_relay_clear_fault(void);

/**
 * @brief Get relay status
 * @param status Pointer to store relay status
 * @return HAL status
 */
hal_status_t hal_relay_get_status(relay_status_t *status);

/**
 * @brief Set relay event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t hal_relay_set_callback(relay_event_callback_t callback);

/**
 * @brief Update relay system (call periodically)
 * @return HAL status
 */
hal_status_t hal_relay_update(void);

/**
 * @brief Set relay configuration
 * @param config Relay configuration
 * @return HAL status
 */
hal_status_t hal_relay_set_config(const relay_config_t *config);

/**
 * @brief Get relay configuration
 * @param config Pointer to store relay configuration
 * @return HAL status
 */
hal_status_t hal_relay_get_config(relay_config_t *config);

// Monitoring Functions

/**
 * @brief Get relay current
 * @param current_ma Pointer to store current in mA
 * @return HAL status
 */
hal_status_t hal_relay_get_current(uint32_t *current_ma);

/**
 * @brief Get relay voltage
 * @param voltage_mv Pointer to store voltage in mV
 * @return HAL status
 */
hal_status_t hal_relay_get_voltage(uint32_t *voltage_mv);

/**
 * @brief Get relay temperature
 * @param temperature_c Pointer to store temperature in Celsius
 * @return HAL status
 */
hal_status_t hal_relay_get_temperature(uint32_t *temperature_c);

/**
 * @brief Check overcurrent condition
 * @param overcurrent Pointer to store overcurrent status
 * @return HAL status
 */
hal_status_t hal_relay_check_overcurrent(bool *overcurrent);

/**
 * @brief Check overtemperature condition
 * @param overtemperature Pointer to store overtemperature status
 * @return HAL status
 */
hal_status_t hal_relay_check_overtemperature(bool *overtemperature);

// Protection Functions

/**
 * @brief Enable overcurrent protection
 * @param enabled Enable/disable overcurrent protection
 * @return HAL status
 */
hal_status_t hal_relay_set_overcurrent_protection(bool enabled);

/**
 * @brief Enable overtemperature protection
 * @param enabled Enable/disable overtemperature protection
 * @return HAL status
 */
hal_status_t hal_relay_set_overtemperature_protection(bool enabled);

/**
 * @brief Set overcurrent threshold
 * @param threshold_ma Overcurrent threshold in mA
 * @return HAL status
 */
hal_status_t hal_relay_set_overcurrent_threshold(uint32_t threshold_ma);

/**
 * @brief Set overtemperature threshold
 * @param threshold_c Overtemperature threshold in Celsius
 * @return HAL status
 */
hal_status_t hal_relay_set_overtemperature_threshold(uint32_t threshold_c);

// Testing Functions

/**
 * @brief Test relay functionality
 * @return HAL status
 */
hal_status_t hal_relay_test(void);

/**
 * @brief Test relay pulse functionality
 * @param duration_ms Pulse duration for test
 * @return HAL status
 */
hal_status_t hal_relay_test_pulse(uint32_t duration_ms);

/**
 * @brief Validate relay hardware
 * @return HAL status
 */
hal_status_t hal_relay_validate_hardware(void);

// Statistics Functions

/**
 * @brief Get relay switch count
 * @param count Pointer to store switch count
 * @return HAL status
 */
hal_status_t hal_relay_get_switch_count(uint32_t *count);

/**
 * @brief Get relay fault count
 * @param count Pointer to store fault count
 * @return HAL status
 */
hal_status_t hal_relay_get_fault_count(uint32_t *count);

/**
 * @brief Reset relay statistics
 * @return HAL status
 */
hal_status_t hal_relay_reset_statistics(void);

// Diagnostic Functions

/**
 * @brief Get relay diagnostic information
 * @param info Pointer to store diagnostic info
 * @return HAL status
 */
hal_status_t hal_relay_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Perform relay self-test
 * @return HAL status
 */
hal_status_t hal_relay_self_test(void);

// Safety Functions

/**
 * @brief Emergency relay shutdown
 * @return HAL status
 */
hal_status_t hal_relay_emergency_shutdown(void);

/**
 * @brief Check relay safety status
 * @param safe Pointer to store safety status
 * @return HAL status
 */
hal_status_t hal_relay_check_safety(bool *safe);

#endif // HAL_RELAY_H
