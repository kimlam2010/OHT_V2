/**
 * @file register_data_api.h
 * @brief Register Data API - Complete API with runtime values
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 * @task Complete Register Data API Implementation
 */

#ifndef REGISTER_DATA_API_H
#define REGISTER_DATA_API_H

#include "api_manager.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize register data API endpoints
 * @return 0 on success, -1 on error
 */
int register_data_api_init(void);

/**
 * @brief GET /api/v1/modules
 * List all online modules
 */
int api_get_modules_list(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief GET /api/v1/modules/{addr}/data
 * Get module data with register metadata + runtime values
 */
int api_get_module_data(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief POST /api/v1/modules/{addr}/registers/{reg_addr}
 * Write register value with mode validation
 */
int api_write_register(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

#ifdef __cplusplus
}
#endif

#endif // REGISTER_DATA_API_H

