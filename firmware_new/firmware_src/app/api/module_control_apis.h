/**
 * @file module_control_apis.h
 * @brief Module Control APIs Header - Issue #100 Fix
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task Issue #100 Fix - Stop Scan Modules API
 */

#ifndef MODULE_CONTROL_APIS_H
#define MODULE_CONTROL_APIS_H

#include "api_endpoints.h"
#include "module_manager.h"
#include "module_polling_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

// Module control API endpoint functions

/**
 * @brief Handle GET /api/v1/modules/status
 * @param req HTTP request
 * @param res HTTP response
 * @return int 0 on success, -1 on error
 */
int api_handle_modules_status_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Handle POST /api/v1/modules/start-scan
 * @param req HTTP request
 * @param res HTTP response
 * @return int 0 on success, -1 on error
 */
int api_handle_modules_start_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Handle POST /api/v1/modules/stop-scan
 * @param req HTTP request
 * @param res HTTP response
 * @return int 0 on success, -1 on error
 */
int api_handle_modules_stop_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Handle POST /api/v1/modules/pause-scan
 */
int api_handle_modules_pause_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Handle POST /api/v1/modules/resume-scan
 */
int api_handle_modules_resume_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Handle POST /api/v1/modules/discover
 * @param req HTTP request
 * @param res HTTP response
 * @return int 0 on success, -1 on error
 */
int api_handle_modules_discover(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Handle GET /api/v1/modules/list
 * @param req HTTP request
 * @param res HTTP response
 * @return int 0 on success, -1 on error
 */
int api_handle_modules_list(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Handle POST /api/v1/modules/reset
 * @param req HTTP request
 * @param res HTTP response
 * @return int 0 on success, -1 on error
 */
int api_handle_modules_reset(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

/**
 * @brief Handle POST /api/v1/modules/health-check
 * @param req HTTP request
 * @param res HTTP response
 * @return int 0 on success, -1 on error
 */
int api_handle_modules_health_check(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);

#ifdef __cplusplus
}
#endif

#endif // MODULE_CONTROL_APIS_H
