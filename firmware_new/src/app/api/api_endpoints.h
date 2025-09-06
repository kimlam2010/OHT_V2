/**
 * @file api_endpoints.h
 * @brief API Endpoints for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task FW-01 (WebSocket & Integration Implementation)
 */

#ifndef API_ENDPOINTS_H
#define API_ENDPOINTS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Include HAL dependencies
#include "../../hal/common/hal_common.h"

// Include API Manager
#include "api_manager.h"

// Include System Controllers
#include "../core/system_controller.h"
#include "../core/system_state_machine.h"

// Include Managers
#include "../managers/safety_manager.h"
#include "../managers/module_manager.h"

// Include Module Handlers
#include "../modules/power_module_handler.h"
#include "../modules/safety_module_handler.h"
#include "../modules/travel_motor_module_handler.h"
#include "../modules/dock_module_handler.h"

// API Response Structures
typedef struct {
    char system_name[64];
    char version[32];
    char status[32];
    uint64_t uptime_ms;
    uint32_t active_modules;
    bool estop_active;
    bool safety_ok;
} api_system_status_t;

typedef struct {
    bool estop_active;
    bool safety_ok;
    uint32_t safety_level;
    uint64_t last_safety_check;
} api_safety_status_t;

typedef struct {
    uint32_t module_count;
    struct {
        uint8_t module_id;
        char module_type[32];
        bool online;
        uint64_t last_seen;
    } modules[16];
} api_modules_list_t;

typedef struct {
    float battery_voltage;
    float battery_current;
    uint8_t battery_soc;
    uint8_t charge_status;
} api_power_status_t;

typedef struct {
    int32_t position;
    int32_t velocity;
    int16_t temperature;
    uint16_t voltage;
    int16_t current;
} api_motor_status_t;

typedef struct {
    uint8_t dock_status;
    int32_t dock_position;
    bool dock_ready;
} api_dock_status_t;

typedef struct {
    uint16_t analog_sensors[4];
    uint8_t digital_sensors;
    bool proximity_alert;
} api_safety_sensors_t;

// API Endpoint Handler Functions
hal_status_t api_handle_system_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);
hal_status_t api_handle_safety_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);
hal_status_t api_handle_modules_list(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);
hal_status_t api_handle_power_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);
hal_status_t api_handle_motor_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);
hal_status_t api_handle_dock_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);
hal_status_t api_handle_safety_sensors(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

// API Data Retrieval Functions
hal_status_t api_get_system_status(api_system_status_t *status);
hal_status_t api_get_safety_status(api_safety_status_t *status);
hal_status_t api_get_modules_list(api_modules_list_t *modules);
hal_status_t api_get_power_status(api_power_status_t *status);
hal_status_t api_get_motor_status(api_motor_status_t *status);
hal_status_t api_get_dock_status(api_dock_status_t *status);
hal_status_t api_get_safety_sensors(api_safety_sensors_t *sensors);

// API JSON Serialization Functions
hal_status_t api_serialize_system_status_json(const api_system_status_t *status, char *buffer, size_t buffer_size);
hal_status_t api_serialize_safety_status_json(const api_safety_status_t *status, char *buffer, size_t buffer_size);
hal_status_t api_serialize_modules_list_json(const api_modules_list_t *modules, char *buffer, size_t buffer_size);
hal_status_t api_serialize_power_status_json(const api_power_status_t *status, char *buffer, size_t buffer_size);
hal_status_t api_serialize_motor_status_json(const api_motor_status_t *status, char *buffer, size_t buffer_size);
hal_status_t api_serialize_dock_status_json(const api_dock_status_t *status, char *buffer, size_t buffer_size);
hal_status_t api_serialize_safety_sensors_json(const api_safety_sensors_t *sensors, char *buffer, size_t buffer_size);

// API Endpoint Registration Functions
hal_status_t api_register_all_endpoints(void);
hal_status_t api_register_system_endpoints(void);
hal_status_t api_register_safety_endpoints(void);
hal_status_t api_register_module_endpoints(void);

// API Utility Functions
const char* api_get_system_state_name(system_controller_state_t state);
const char* api_get_safety_state_name(safety_monitor_state_t state);
const char* api_get_module_type_name(module_type_t type);
hal_status_t api_validate_request_method(const api_mgr_http_request_t *request, api_mgr_http_method_t expected_method);
hal_status_t api_validate_request_path(const api_mgr_http_request_t *request, const char *expected_path);

// API Error Handling Functions
hal_status_t api_handle_system_error(hal_status_t error_code, const char *context);
hal_status_t api_handle_module_error(hal_status_t error_code, const char *context, uint8_t module_id);
hal_status_t api_create_error_response(api_mgr_http_response_t *response, api_mgr_http_response_code_t status_code, const char *error_message);

// API Configuration Functions
hal_status_t api_endpoints_init(void);
hal_status_t api_endpoints_deinit(void);
hal_status_t api_endpoints_get_status(bool *initialized, uint32_t *endpoint_count);

// API Version Information
#define API_ENDPOINTS_VERSION_MAJOR 1
#define API_ENDPOINTS_VERSION_MINOR 0
#define API_ENDPOINTS_VERSION_PATCH 0
#define API_ENDPOINTS_VERSION_STRING "1.0.0"

hal_status_t api_endpoints_get_version(uint32_t *major, uint32_t *minor, uint32_t *patch);
const char* api_endpoints_get_version_string(void);

#endif // API_ENDPOINTS_H
