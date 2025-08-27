#include "hal_common.h"
#include <stdio.h>
#include <string.h>

// Mock HTTP server structure
typedef struct {
    uint16_t port;
    bool running;
} http_server_mock_t;

// Mock HTTP server instance
static http_server_mock_t g_http_server_mock = {0};

// Mock HTTP server functions
hal_status_t http_server_init(void* server, void* config) {
    (void)server;
    (void)config;
    g_http_server_mock.running = false;
    return HAL_STATUS_OK;
}

hal_status_t http_server_add_route(void* server, int method, const char* path, void* handler, bool auth_required) {
    (void)server;
    (void)method;
    (void)path;
    (void)handler;
    (void)auth_required;
    return HAL_STATUS_OK;
}

hal_status_t http_server_start(void* server) {
    (void)server;
    g_http_server_mock.running = true;
    return HAL_STATUS_OK;
}

hal_status_t http_server_stop(void* server) {
    (void)server;
    g_http_server_mock.running = false;
    return HAL_STATUS_OK;
}

hal_status_t http_server_get_status(void* server, void* status) {
    (void)server;
    (void)status;
    return HAL_STATUS_OK;
}
