/**
 * @file simple_http_8081.h
 * @brief Simple HTTP Server for Port 8081 - Issue #113 Fix
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Issue #113 Fix - Port 8081 Timeout Issue
 */

#ifndef SIMPLE_HTTP_8081_H
#define SIMPLE_HTTP_8081_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Simple HTTP Server structure
 */
typedef struct {
    int server_socket;
    bool running;
    uint16_t port;
} simple_http_server_t;

/**
 * @brief Start simple HTTP server on port 8081
 * @return int 0 on success, -1 on failure
 */
int simple_http_8081_start(void);

/**
 * @brief Stop simple HTTP server
 */
void simple_http_8081_stop(void);

#endif // SIMPLE_HTTP_8081_H
