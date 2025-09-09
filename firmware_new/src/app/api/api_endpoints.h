#pragma once
#include "api_manager.h"

int api_register_minimal_endpoints(void);

int api_handle_system_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_safety_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_safety_estop(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_modules_list(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_module_status_by_id(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_system_state(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_control_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
// New: Module Manager extended endpoints
int api_handle_modules_stats(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_modules_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_modules_config_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

// Motion API (EXEC PLAN Gate E)
int api_handle_motion_segment_start(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_motion_segment_stop(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_motion_state(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
