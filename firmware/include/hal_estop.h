#ifndef HAL_ESTOP_H
#define HAL_ESTOP_H

#include "hal_common.h"
#include <stdint.h>
#include <stdbool.h>

// E-Stop GPIO Pin Definitions (updated for available pins)
#define ESTOP_CHANNEL1_PIN     59  // GPIO1_D3 - E-Stop Channel 1
#define ESTOP_CHANNEL2_PIN     131 // GPIO4_A3 - E-Stop Channel 2

// E-Stop Safety Parameters
#define ESTOP_RESPONSE_TIME_MS 100 // Maximum response time
#define ESTOP_DEBOUNCE_TIME_MS 50  // Debounce time for button
#define ESTOP_SAFETY_LEVEL     "SIL2" // Safety integrity level

// E-Stop States
typedef enum {
    ESTOP_STATE_SAFE = 0,     // System safe, E-Stop not triggered
    ESTOP_STATE_TRIGGERED,    // E-Stop triggered, emergency stop
    ESTOP_STATE_FAULT,        // E-Stop fault detected
    ESTOP_STATE_RESETTING     // E-Stop reset in progress
} estop_state_t;

// E-Stop Fault Types
typedef enum {
    ESTOP_FAULT_NONE = 0,
    ESTOP_FAULT_CHANNEL1_OPEN,    // Channel 1 open circuit
    ESTOP_FAULT_CHANNEL2_OPEN,    // Channel 2 open circuit
    ESTOP_FAULT_CHANNEL_MISMATCH, // Channel mismatch
    ESTOP_FAULT_RESPONSE_TIMEOUT, // Response timeout
    ESTOP_FAULT_HARDWARE_ERROR    // Hardware error
} estop_fault_t;

// E-Stop Configuration Structure
typedef struct {
    uint8_t channel1_pin;
    uint8_t channel2_pin;
    uint32_t response_timeout_ms;
    uint32_t debounce_time_ms;
    bool dual_channel_required;
    bool auto_reset_enabled;
} estop_config_t;

// E-Stop Status Structure
typedef struct {
    estop_state_t state;
    estop_fault_t fault;
    bool channel1_status;
    bool channel2_status;
    uint64_t last_trigger_time;
    uint64_t last_reset_time;
    uint32_t trigger_count;
    uint32_t fault_count;
} estop_status_t;

// E-Stop Event Callback
typedef void (*estop_event_callback_t)(estop_state_t state, estop_fault_t fault);

// Function Prototypes

/**
 * @brief Initialize E-Stop safety system
 * @param config E-Stop configuration
 * @return HAL status
 */
hal_status_t hal_estop_init(const estop_config_t *config);

/**
 * @brief Deinitialize E-Stop safety system
 * @return HAL status
 */
hal_status_t hal_estop_deinit(void);

/**
 * @brief Get E-Stop state
 * @param state Pointer to store E-Stop state
 * @return HAL status
 */
hal_status_t hal_estop_get_state(estop_state_t *state);

/**
 * @brief Check if E-Stop is triggered
 * @param triggered Pointer to store trigger status
 * @return HAL status
 */
hal_status_t hal_estop_is_triggered(bool *triggered);

/**
 * @brief Reset E-Stop system
 * @return HAL status
 */
hal_status_t hal_estop_reset(void);

/**
 * @brief Get E-Stop fault status
 * @param fault Pointer to store fault type
 * @return HAL status
 */
hal_status_t hal_estop_get_fault(estop_fault_t *fault);

/**
 * @brief Clear E-Stop fault
 * @return HAL status
 */
hal_status_t hal_estop_clear_fault(void);

/**
 * @brief Get E-Stop status
 * @param status Pointer to store E-Stop status
 * @return HAL status
 */
hal_status_t hal_estop_get_status(estop_status_t *status);

/**
 * @brief Set E-Stop event callback
 * @param callback Event callback function
 * @return HAL status
 */
hal_status_t hal_estop_set_callback(estop_event_callback_t callback);

/**
 * @brief Update E-Stop system (call periodically)
 * @return HAL status
 */
hal_status_t hal_estop_update(void);

/**
 * @brief Test E-Stop channels
 * @param channel1_status Pointer to store channel 1 status
 * @param channel2_status Pointer to store channel 2 status
 * @return HAL status
 */
hal_status_t hal_estop_test_channels(bool *channel1_status, bool *channel2_status);

/**
 * @brief Validate E-Stop safety system
 * @return HAL status
 */
hal_status_t hal_estop_validate_safety(void);

/**
 * @brief Get E-Stop response time
 * @param response_time_ms Pointer to store response time
 * @return HAL status
 */
hal_status_t hal_estop_get_response_time(uint32_t *response_time_ms);

/**
 * @brief Set E-Stop configuration
 * @param config E-Stop configuration
 * @return HAL status
 */
hal_status_t hal_estop_set_config(const estop_config_t *config);

/**
 * @brief Get E-Stop configuration
 * @param config Pointer to store E-Stop configuration
 * @return HAL status
 */
hal_status_t hal_estop_get_config(estop_config_t *config);

// Safety Functions

/**
 * @brief Handle emergency stop
 * @return HAL status
 */
hal_status_t hal_estop_handle_emergency(void);

/**
 * @brief Check safety compliance
 * @param compliant Pointer to store compliance status
 * @return HAL status
 */
hal_status_t hal_estop_check_safety_compliance(bool *compliant);

/**
 * @brief Perform safety self-test
 * @return HAL status
 */
hal_status_t hal_estop_self_test(void);

// Channel-specific Functions

/**
 * @brief Get channel 1 status
 * @param status Pointer to store channel 1 status
 * @return HAL status
 */
hal_status_t hal_estop_get_channel1_status(bool *status);

/**
 * @brief Get channel 2 status
 * @param status Pointer to store channel 2 status
 * @return HAL status
 */
hal_status_t hal_estop_get_channel2_status(bool *status);

/**
 * @brief Test channel 1
 * @return HAL status
 */
hal_status_t hal_estop_test_channel1(void);

/**
 * @brief Test channel 2
 * @return HAL status
 */
hal_status_t hal_estop_test_channel2(void);

// Statistics Functions

/**
 * @brief Get E-Stop trigger count
 * @param count Pointer to store trigger count
 * @return HAL status
 */
hal_status_t hal_estop_get_trigger_count(uint32_t *count);

/**
 * @brief Get E-Stop fault count
 * @param count Pointer to store fault count
 * @return HAL status
 */
hal_status_t hal_estop_get_fault_count(uint32_t *count);

/**
 * @brief Reset E-Stop statistics
 * @return HAL status
 */
hal_status_t hal_estop_reset_statistics(void);

// Diagnostic Functions

/**
 * @brief Get E-Stop diagnostic information
 * @param info Pointer to store diagnostic info
 * @return HAL status
 */
hal_status_t hal_estop_get_diagnostics(char *info, size_t max_len);

/**
 * @brief Validate E-Stop hardware
 * @return HAL status
 */
hal_status_t hal_estop_validate_hardware(void);

#endif // HAL_ESTOP_H
