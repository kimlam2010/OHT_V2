/**
 * @file test_api_endpoints.c
 * @brief API Endpoints Test Program
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-03 (API Endpoints Implementation)
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

#include "api_endpoints.h"
#include "http_server.h"
#include "hal_common.h"

// Test server instance
static http_server_t test_server;

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
static bool test_api_endpoints_initialization(void) {
    printf("Testing API endpoints initialization...\n");
    
    hal_status_t status = http_server_init(&test_server, NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server initialization failed: %d\n", status);
        return false;
    }
    
    status = api_endpoints_init(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ API endpoints initialization failed: %d\n", status);
        return false;
    }
    
    printf("✅ API endpoints initialization passed\n");
    return true;
}

static bool test_api_system_status(void) {
    printf("Testing API system status endpoint...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test GET /api/v1/system/status
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/api/v1/system/status", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send system status request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive system status response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "200 OK") == NULL) {
        printf("❌ System status response not OK: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "OHT-50 Master Module") == NULL) {
        printf("❌ System status response missing system name\n");
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
    
    printf("✅ API system status endpoint passed\n");
    return true;
}

static bool test_api_modules_list(void) {
    printf("Testing API modules list endpoint...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test GET /api/v1/modules
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/api/v1/modules", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send modules list request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive modules list response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "200 OK") == NULL) {
        printf("❌ Modules list response not OK: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "power") == NULL || strstr(response, "motor") == NULL || strstr(response, "dock") == NULL) {
        printf("❌ Modules list response missing module types\n");
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
    
    printf("✅ API modules list endpoint passed\n");
    return true;
}

static bool test_api_module_info(void) {
    printf("Testing API module info endpoint...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test GET /api/v1/modules/1
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/api/v1/modules/1", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send module info request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive module info response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "200 OK") == NULL) {
        printf("❌ Module info response not OK: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "power") == NULL) {
        printf("❌ Module info response missing module type\n");
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
    
    printf("✅ API module info endpoint passed\n");
    return true;
}

static bool test_api_safety_status(void) {
    printf("Testing API safety status endpoint...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test GET /api/v1/safety/status
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/api/v1/safety/status", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send safety status request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive safety status response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "200 OK") == NULL) {
        printf("❌ Safety status response not OK: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "safety_ok") == NULL) {
        printf("❌ Safety status response missing safety info\n");
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
    
    printf("✅ API safety status endpoint passed\n");
    return true;
}

static bool test_api_config_endpoints(void) {
    printf("Testing API config endpoints...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test GET /api/v1/config
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/api/v1/config", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send config get request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive config get response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "200 OK") == NULL) {
        printf("❌ Config get response not OK: %s\n", response);
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
    
    printf("✅ API config endpoints passed\n");
    return true;
}

static bool test_api_diagnostics(void) {
    printf("Testing API diagnostics endpoint...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test GET /api/v1/diagnostics
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/api/v1/diagnostics", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send diagnostics request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive diagnostics response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "200 OK") == NULL) {
        printf("❌ Diagnostics response not OK: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "total_requests") == NULL) {
        printf("❌ Diagnostics response missing request info\n");
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
    
    printf("✅ API diagnostics endpoint passed\n");
    return true;
}

static bool test_api_error_handling(void) {
    printf("Testing API error handling...\n");
    
    // Start server
    hal_status_t status = http_server_start(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server start failed: %d\n", status);
        return false;
    }
    
    // Wait for server to start
    usleep(100000); // 100ms
    
    // Test invalid endpoint
    int client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/api/v1/invalid", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send invalid request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    char response[1024];
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive invalid response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "404 Not Found") == NULL) {
        printf("❌ Invalid endpoint should return 404: %s\n", response);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    close(client);
    
    // Test invalid module ID
    client = create_test_client();
    if (client < 0) {
        printf("❌ Failed to create test client for invalid module\n");
        http_server_stop(&test_server);
        return false;
    }
    
    status = send_http_request(client, "GET", "/api/v1/modules/999", NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Send invalid module request failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    status = receive_http_response(client, response, sizeof(response));
    if (status != HAL_STATUS_OK) {
        printf("❌ Receive invalid module response failed: %d\n", status);
        close(client);
        http_server_stop(&test_server);
        return false;
    }
    
    if (strstr(response, "404 Not Found") == NULL) {
        printf("❌ Invalid module should return 404: %s\n", response);
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
    
    printf("✅ API error handling passed\n");
    return true;
}

static bool test_api_utility_functions(void) {
    printf("Testing API utility functions...\n");
    
    // Test module ID extraction
    int module_id = api_extract_module_id("/api/v1/modules/1");
    if (module_id != 1) {
        printf("❌ Module ID extraction failed: expected 1, got %d\n", module_id);
        return false;
    }
    
    module_id = api_extract_module_id("/api/v1/modules/2/command");
    if (module_id != 2) {
        printf("❌ Module ID extraction from command failed: expected 2, got %d\n", module_id);
        return false;
    }
    
    module_id = api_extract_module_id("/api/v1/invalid");
    if (module_id != -1) {
        printf("❌ Module ID extraction from invalid path failed: expected -1, got %d\n", module_id);
        return false;
    }
    
    // Test JSON creation functions
    api_system_status_t status = {
        .system_name = "Test System",
        .version = "1.0.0",
        .status = "running",
        .uptime_ms = 1000,
        .active_modules = 2,
        .estop_active = false,
        .safety_ok = true
    };
    
    char json_buffer[1024];
    hal_status_t result = api_create_system_status_json(&status, json_buffer, sizeof(json_buffer));
    if (result != HAL_STATUS_OK) {
        printf("❌ System status JSON creation failed: %d\n", result);
        return false;
    }
    
    if (strstr(json_buffer, "Test System") == NULL) {
        printf("❌ System status JSON missing system name\n");
        return false;
    }
    
    printf("✅ API utility functions passed\n");
    return true;
}

static bool test_api_cleanup(void) {
    printf("Testing API cleanup...\n");
    
    hal_status_t status = api_endpoints_deinit(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ API endpoints cleanup failed: %d\n", status);
        return false;
    }
    
    status = http_server_deinit(&test_server);
    if (status != HAL_STATUS_OK) {
        printf("❌ HTTP server cleanup failed: %d\n", status);
        return false;
    }
    
    printf("✅ API cleanup passed\n");
    return true;
}

int main(int argc, char *argv) {
    (void)argc;
    (void)argv;
    
    printf("=== API Endpoints Test Program ===\n");
    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("Version: 1.0.0\n");
    printf("Team: FW\n");
    printf("Task: FW-INT-03 (API Endpoints Implementation)\n\n");
    
    int passed = 0;
    int total = 0;
    
    // Run tests
    if (test_api_endpoints_initialization()) passed++;
    total++;
    
    if (test_api_system_status()) passed++;
    total++;
    
    if (test_api_modules_list()) passed++;
    total++;
    
    if (test_api_module_info()) passed++;
    total++;
    
    if (test_api_safety_status()) passed++;
    total++;
    
    if (test_api_config_endpoints()) passed++;
    total++;
    
    if (test_api_diagnostics()) passed++;
    total++;
    
    if (test_api_error_handling()) passed++;
    total++;
    
    if (test_api_utility_functions()) passed++;
    total++;
    
    if (test_api_cleanup()) passed++;
    total++;
    
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", passed);
    printf("Tests failed: %d\n", total - passed);
    printf("Total tests: %d\n", total);
    printf("Success rate: %.1f%%\n", (float)passed / total * 100);
    
    if (passed == total) {
        printf("🎉 All tests PASSED! API endpoints implementation is working correctly.\n");
        return 0;
    } else {
        printf("❌ Some tests FAILED! API endpoints implementation needs fixes.\n");
        return 1;
    }
}
