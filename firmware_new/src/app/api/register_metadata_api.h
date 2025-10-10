/**
 * @file register_metadata_api.h
 * @brief HTTP API Endpoints for Register Metadata (Issue #203)
 * @version 1.0.1
 * @date 2025-01-28
 * @author FW Team
 * @task Issue #203 - Register Metadata API Implementation
 * @fix Use API Manager instead of HTTP Server
 */

#ifndef REGISTER_METADATA_API_H
#define REGISTER_METADATA_API_H

#include "api_manager.h"
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// API ENDPOINTS
// ============================================================================

/**
 * @brief GET /api/v1/modules/{addr}/registers
 * Get all registers for a module
 * API Manager wrapper
 */
int api_get_module_registers_wrapper(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief GET /api/v1/modules/{addr}/registers/{reg_addr}
 * Get metadata for a specific register
 * API Manager wrapper
 */
int api_get_single_register_wrapper(const api_mgr_http_request_t *request, api_mgr_http_response_t *response);

/**
 * @brief Register all register metadata API endpoints
 * @return 0 on success, -1 on error
 */
int register_metadata_api_init(void);

#endif // REGISTER_METADATA_API_H

