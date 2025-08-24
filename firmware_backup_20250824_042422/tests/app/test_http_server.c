/**
 * @file test_http_server.c
 * @brief HTTP Server Test Program
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-01 (HTTP Server Implementation)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#include "http_server.h"
#include "hal_common.h"

// Test server instance
static http_server_t test_server;

// Test route handlers
static hal_status_t test_health_handler(const http_request_t *request, http_response_t *response) {
    (void)request;
    
    const char *json_response = "{\"status\":\"healthy\",\"timestamp\":1234567890}";
    return http_response_set_json(response, json_response);
}

static hal_status_t test_status_handler(const http_request_t *request, http_response_t *response) {
    (void)request;
    
    const char *json_response = "{\"system\":\"OHT-50\",\"version\":\"1.0.0\",\"uptime\":3600}";
    return http_response_set_json(response, json_response);
}

static hal_status_t test_echo_handler(const http_request_t *request, http_response_t *response) {
    char json_response[512];
    snprintf(json_response, sizeof(json_response),
             "{\"method\":\"%s\",\"path\":\"%s\",\"body\":\"%s\"}",
             http_get_method_string(request->method),
             request->path,
             request->body);
    
    return http_response_set_json(response, json_response);
}

static hal_status_t test_auth_handler(const http_request_t *request, http_response_t *response) {
    (void)request;
    
    const char *json_response = "{\"message\":\"Authenticated successfully\"}";
    return http_response_set_json(response, json_response);
}

// Test client functions
static int create_test_client(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(HTTP_SERVER_DEFAULT_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

static hal_status_t send_http_request(int sock, const char *method, const char *path, const char *body) {
    char request[1024];
    int len = snprintf(request, sizeof(request),
                      "%s %s HTTP/1.1\r\n"
                      "Host: localhost:%d\r\n"
                      "Content-Length: %zu\r\n"
                      "\r\n"
                      "%s",
                      method, path, HTTP_SERVER_DEFAULT_PORT,
                      body ? strlen(body) : 0,
                      body ? body : "");
    
    int sent = send(sock, request, len, 0);
    return (sent == len) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static hal_status_t receive_http_response(int sock, char *response, int max_len) {
    int received = recv(sock, response, max_len - 1, 0);
    if (received <= 0) {
        return HAL_STATUS_ERROR;
    }
    
    response[received] = '\0';
    return HAL_STATUS_OK;
}

// Test functions
static bool test_http_server_initialization(void) {
    printf("Testing HTTP server initialization...\n");
    
    hal_status_t status = http_server_init(&test_server, NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server initialization failed: %d\n", status);
        return false;
    }
    
    printf("✅ HTTP server initialization passed\n");
    return true;
}

static bool test_http_server_configuration(void) {
    printf("Testing HTTP server configuration...\n");
    
    http_server_config_t config = HTTP_SERVER_DEFAULT_CONFIG;
    config.port = 8081;
    config.enable_auth = true;
    strcpy(config.auth_token, "test_token");
    
    hal_status_t status = http_server_set_config(&test_server, &config);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server configuration failed: %d\n", status);
        return false;
    }
    
    http_server_config_t retrieved_config;
    status = http_server_get_config(&test_server, &retrieved_config);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server get configuration failed: %d\n", status);
        return false;
    }
    
    if (retrieved_config.port != 8081 || !retrieved_config.enable_auth) {
        printf("❌ HTTP server configuration mismatch\n");
        return false;
    }
    
    printf("✅ HTTP server configuration passed\n");
    return true;
}

static bool test_http_server_routes(void) {
    printf("Testing HTTP server routes...\n");
    
    hal_status_t status = http_server_add_route(&test_server, HTTP_METHOD_GET, "/health", test_health_handler, false);
    if (status != HAL_STATUS_OK) {
        printf("❌ Add health route failed: %d\n", status);
        return false;
    }
    
    status = http_server_add_route(&test_server, HTTP_METHOD_GET, "/status", test_status_handler, false);
    if (status != HAL_STATUS_OK) {
        printf("❌ Add status route failed: %d\n", status);
        return false;
    }
    
    status = http_server_add_route(&test_server, HTTP_METHOD_POST, "/echo", test_echo_handler, false);
    if (status != HAL_STATUS_OK) {
        printf("❌ Add echo route failed: %d\n", status);
        return false;
    }
    
    status = http_server_add_route(&test_server, HTTP_METHOD_GET, "/auth", test_auth_handler, true);
    if (status != HAL_STATUS_OK) {
        printf("❌ Add auth route failed: %d\n", status);
        return false;
    }
    
    printf("✅ HTTP server routes passed\n");
    return true;
}

static bool test_http_server_start_stop(void) {
    printf("Testing HTTP server start/stop...\n");
    
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait a bit for server to start
    usleep(100000); // 100ms
    
    http_server_status_t server_status;
    status = http_server_get_status(&test_server, &server_status);
    if (status != HAL_STATUS_OK) {
        printf("❌ Get server status failed: %d\n", status);
        return false;
    }
    
    if (!server_status.is_running) {
        printf("❌ Server not running\n");
        return false;
    }
    
    status = http_server_stop(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server stop failed: %d\n", status);
        return false;
    }
    
    printf("✅ HTTP server start/stop passed\n");
    return true;
}

static bool test_http_server_requests(void) {
    printf("Testing HTTP server requests...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test GET /health
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/health", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send health request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive health response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "200 OK") == NULL) {
        printf("❌ Health response not OK: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    close(client);
    
    // Test POST /echo
    client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client for echo\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "POST", "/echo", "{\"test\":\"data\"}");
    if (status != HAL_STATUS_OK) {
        printf("❌ Send echo request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive echo response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "200 OK") == NULL) {
        printf("❌ Echo response not OK: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    close(client);
    
    // Stop server
    status = http_server_stop(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server stop failed: %d\n", status);
        return false;
    }
    
    printf("✅ HTTP server requests passed\n");
    return true;
}

static bool test_http_server_authentication(void) {
    printf("Testing HTTP server authentication...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test unauthorized access
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/auth", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send auth request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive auth response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "401 Unauthorized") == NULL) {
        printf("❌ Auth response should be unauthorized: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    close(client);
    
    // Stop server
    status = http_server_stop(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server stop failed: %d\n", status);
        return false;
    }
    
    printf("✅ HTTP server authentication passed\n");
    return true;
}

static bool test_http_server_cleanup(void) {
    printf("Testing HTTP server cleanup...\n");
    
    hal_status_t status = http_server_deinit(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server cleanup failed: %d\n", status);
        return false;
    }
    
    printf("✅ HTTP server cleanup passed\n");
    return true;
}

int main(int argc, char *argv) {
    (void)argc;
    (void)argv;
    
    printf("=== HTTP Server Test Program ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("Version: 1.0.0\n");
    printf("Team: FW\n");
    printf("Task: FW-INT-01 (HTTP Server Implementation)\n\n");
    
    int passed = 0;
    int total = 0;
    
    // Run tests
    if (test_http_server_initialization()) passed++;
    total++;
    
    if (test_http_server_configuration()) passed++;
    total++;
    
    if (test_http_server_routes()) passed++;
    total++;
    
    if (test_http_server_start_stop()) passed++;
    total++;
    
    if (test_http_server_requests()) passed++;
    total++;
    
    if (test_http_server_authentication()) passed++;
    total++;
    
    if (test_http_server_cleanup()) passed++;
    total++;
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", passed);
    printf("Tests failed: %d\n", total - passed);
    printf("Total tests: %d\n", total);
    printf("Success rate: %.1f%%\n", (float)passed / total * 100);
    
    if (passed == total) {
        printf("🎉 All tests PASSED! HTTP server implementation is working correctly.\n");
        return 0;
    } else {
        printf("❌ Some tests FAILED! HTTP server implementation needs fixes.\n");
        return 1;
    }
}
