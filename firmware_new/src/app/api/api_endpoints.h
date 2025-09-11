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

// LiDAR API (CRITICAL - Production Integration)
int api_register_lidar_endpoints(void);
int api_handle_lidar_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_scan_data(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_start_scanning(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_stop_scanning(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_config_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_config_set(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_safety_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
int api_handle_lidar_health_check(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
// Full-frame LiDAR data (no point limit)
int api_handle_lidar_scan_frame_full(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
// Angle-filtered frame via GET query (?min_deg=&max_deg=&normalize=1&limit=)
int api_handle_lidar_scan_frame(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
// 360-bin frame (0..359 degrees) via GET query (?reducer=max&min_q=0&max_range=0)
int api_handle_lidar_scan_frame_360(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
