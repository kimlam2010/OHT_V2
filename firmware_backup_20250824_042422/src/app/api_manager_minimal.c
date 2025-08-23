/**
 * @file api_manager_minimal.c
 * @brief Minimal API Manager for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-01 (HTTP Server Implementation)
 */

#include "http_server.h"
#include "api_endpoints.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Minimal API Manager structure
typedef struct {
    bool initialized;
    bool http_server_running;
    http_server_t http_server;
    http_server_config_t http_config;
} api_manager_minimal_t;

static api_manager_minimal_t g_api_manager = {0};

// Minimal API Manager implementation
hal_status_t api_manager_init(const void *config) {
    if (g_api_manager.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    memset(&g_api_manager, 0, sizeof(g_api_manager));
    
    // Configure HTTP server
    g_api_manager.http_config.port = 8080;
    g_api_manager.http_config.max_connections = 10;
    g_api_manager.http_config.timeout_ms = 30000;
    g_api_manager.http_config.enable_cors = true;
    strcpy(g_api_manager.http_config.cors_origin, "*");
    g_api_manager.http_config.enable_auth = false;
    g_api_manager.http_config.enable_logging = true;
    strcpy(g_api_manager.http_config.log_file, "/var/log/oht50/http_server.log");
    
    // Initialize HTTP server
    hal_status_t status = http_server_init(&g_api_manager.http_server, &g_api_manager.http_config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Initialize API endpoints
    status = api_endpoints_init(&g_api_manager.http_server);
    if (status != HAL_STATUS_OK) {
        http_server_deinit(&g_api_manager.http_server);
        return status;
    }
    
    g_api_manager.initialized = true;
    printf("[API] API Manager initialized successfully\n");
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_deinit(void) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (g_api_manager.http_server_running) {
        api_manager_stop();
    }
    
    api_endpoints_deinit(&g_api_manager.http_server);
    http_server_deinit(&g_api_manager.http_server);
    
    memset(&g_api_manager, 0, sizeof(g_api_manager));
    printf("[API] API Manager deinitialized\n");
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_start(void) {
    if (!g_api_manager.initialized) {
        return HAL_STATUS_ERROR;
    }
    
    if (g_api_manager.http_server_running) {
        return HAL_STATUS_OK; // Already running
    }
    
    hal_status_t status = http_server_start(&g_api_manager.http_server);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    g_api_manager.http_server_running = true;
    printf("[API] API Manager started - HTTP server on port %d\n", g_api_manager.http_config.port);
    
    return HAL_STATUS_OK;
}

hal_status_t api_manager_stop(void) {
    if (!g_api_manager.initialized || !g_api_manager.http_server_running) {
        return HAL_STATUS_ERROR;
    }
    
    hal_status_t status = http_server_stop(&g_api_manager.http_server);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    g_api_manager.http_server_running = false;
    printf("[API] API Manager stopped\n");
    
    return HAL_STATUS_OK;
}
