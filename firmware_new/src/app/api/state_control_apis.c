// =====================================
// STATE MACHINE CONTROL APIs - NEW IMPLEMENTATION
// =====================================

#include <stdio.h>
#include <string.h>
#include "api_endpoints.h"
#include "system_state_machine.h"

// POST /api/v1/state/move
int api_handle_state_move(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Validate current state allows movement
    system_status_t status;
    if (system_state_machine_get_status(&status) != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Cannot get system status");
    }
    
    if (status.current_state != SYSTEM_STATE_IDLE && status.current_state != SYSTEM_STATE_PAUSED) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
            "Cannot move from current state: %s", 
            system_state_machine_get_state_name(status.current_state));
        return api_manager_create_error_response(res, API_MGR_RESPONSE_BAD_REQUEST, error_msg);
    }
    
    // Execute state transition
    hal_status_t result = system_state_machine_enter_move();
    
    if (result == HAL_STATUS_OK) {
        return api_manager_create_success_response(res, 
            "{\"success\":true,\"message\":\"Entered MOVE state\"}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to enter MOVE state");
    }
}

// POST /api/v1/state/pause
int api_handle_state_pause(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Only allow pause from MOVE state
    system_status_t status;
    if (system_state_machine_get_status(&status) != HAL_STATUS_OK ||
        status.current_state != SYSTEM_STATE_MOVE) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_BAD_REQUEST,
            "Can only pause from MOVE state");
    }
    
    hal_status_t result = system_state_machine_enter_paused();
    
    if (result == HAL_STATUS_OK) {
        return api_manager_create_success_response(res,
            "{\"success\":true,\"message\":\"Entered PAUSED state\"}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to pause");
    }
}

// POST /api/v1/state/resume  
int api_handle_state_resume(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    system_status_t status;
    if (system_state_machine_get_status(&status) != HAL_STATUS_OK ||
        status.current_state != SYSTEM_STATE_PAUSED) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_BAD_REQUEST,
            "Can only resume from PAUSED state");
    }
    
    hal_status_t result = system_state_machine_resume_from_pause();
    
    if (result == HAL_STATUS_OK) {
        return api_manager_create_success_response(res,
            "{\"success\":true,\"message\":\"Resumed to MOVE state\"}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to resume");
    }
}

// POST /api/v1/state/stop
int api_handle_state_stop(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Stop can be called from most states
    system_status_t status;
    if (system_state_machine_get_status(&status) != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Cannot get system status");
    }
    
    // Don't allow stop from FAULT or ESTOP states
    if (status.current_state == SYSTEM_STATE_FAULT || status.current_state == SYSTEM_STATE_ESTOP) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_BAD_REQUEST,
            "Cannot stop from fault/emergency state - use reset instead");
    }
    
    hal_status_t result = system_state_machine_process_event(SYSTEM_EVENT_STOP_COMMAND);
    
    if (result == HAL_STATUS_OK) {
        return api_manager_create_success_response(res,
            "{\"success\":true,\"message\":\"System stopped - returned to IDLE state\"}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to stop system");
    }
}

// POST /api/v1/state/dock
int api_handle_state_dock(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Docking can be initiated from IDLE or MOVE state
    system_status_t status;
    if (system_state_machine_get_status(&status) != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Cannot get system status");
    }
    
    if (status.current_state != SYSTEM_STATE_IDLE && status.current_state != SYSTEM_STATE_MOVE) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), 
            "Cannot dock from current state: %s", 
            system_state_machine_get_state_name(status.current_state));
        return api_manager_create_error_response(res, API_MGR_RESPONSE_BAD_REQUEST, error_msg);
    }
    
    hal_status_t result = system_state_machine_enter_dock();
    
    if (result == HAL_STATUS_OK) {
        return api_manager_create_success_response(res,
            "{\"success\":true,\"message\":\"Entered DOCK state\"}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to enter dock state");
    }
}

// POST /api/v1/state/emergency
int api_handle_state_emergency(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Emergency stop can be triggered from any state
    hal_status_t result = system_state_machine_enter_estop();
    
    if (result == HAL_STATUS_OK) {
        return api_manager_create_success_response(res,
            "{\"success\":true,\"message\":\"Emergency stop activated\"}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to activate emergency stop");
    }
}

// POST /api/v1/state/reset
int api_handle_state_reset(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    // Reset can be used to recover from FAULT or ESTOP states
    system_status_t status;
    if (system_state_machine_get_status(&status) != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Cannot get system status");
    }
    
    hal_status_t result = HAL_STATUS_ERROR;
    
    if (status.current_state == SYSTEM_STATE_ESTOP) {
        result = system_state_machine_process_event(SYSTEM_EVENT_ESTOP_RESET);
    } else if (status.current_state == SYSTEM_STATE_FAULT) {
        result = system_state_machine_process_event(SYSTEM_EVENT_FAULT_CLEARED);
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_BAD_REQUEST,
            "Reset can only be used from FAULT or ESTOP states");
    }
    
    if (result == HAL_STATUS_OK) {
        return api_manager_create_success_response(res,
            "{\"success\":true,\"message\":\"System reset successfully\"}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR,
            "Failed to reset system");
    }
}
