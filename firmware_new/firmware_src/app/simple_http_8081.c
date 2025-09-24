/**
 * @file simple_http_8081.c
 * @brief Simple HTTP Server for Port 8081 - Issue #113 Fix
 * @version 1.0.0
 * @date 2025-09-19
 * @team FW
 * @task Issue #113 Fix - Port 8081 Timeout Issue
 */

#include "simple_http_8081.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

// Global instance
static simple_http_server_t g_http_8081 = {0};
static pthread_t g_http_8081_thread = 0;
static volatile bool g_http_8081_running = false;

// Thread function
static void* simple_http_8081_thread(void* arg);
static void simple_http_8081_handle_client(int client_socket);
static void simple_http_8081_send_response(int client_socket, const char* response);

/**
 * @brief Start simple HTTP server on port 8081
 * @return int 0 on success, -1 on failure
 */
int simple_http_8081_start(void) {
    if (g_http_8081_running) {
        return 0; // Already running
    }
    
    // Create socket
    g_http_8081.server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_http_8081.server_socket < 0) {
        fprintf(stderr, "[HTTP-8081] Failed to create socket: %s\n", strerror(errno));
        return -1;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(g_http_8081.server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "[HTTP-8081] Failed to set socket options: %s\n", strerror(errno));
        close(g_http_8081.server_socket);
        return -1;
    }
    
    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(g_http_8081.server_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8081);
    
    if (bind(g_http_8081.server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "[HTTP-8081] Failed to bind socket: %s\n", strerror(errno));
        close(g_http_8081.server_socket);
        return -1;
    }
    
    // Listen for connections
    if (listen(g_http_8081.server_socket, 10) < 0) {
        fprintf(stderr, "[HTTP-8081] Failed to listen: %s\n", strerror(errno));
        close(g_http_8081.server_socket);
        return -1;
    }
    
    // Start server thread
    g_http_8081_running = true;
    if (pthread_create(&g_http_8081_thread, NULL, simple_http_8081_thread, NULL) != 0) {
        fprintf(stderr, "[HTTP-8081] Failed to create thread: %s\n", strerror(errno));
        g_http_8081_running = false;
        close(g_http_8081.server_socket);
        return -1;
    }
    
    printf("[HTTP-8081] ✅ Simple HTTP Server started on port 8081\n");
    return 0;
}

/**
 * @brief Stop simple HTTP server
 */
void simple_http_8081_stop(void) {
    if (!g_http_8081_running) {
        return;
    }
    
    g_http_8081_running = false;
    
    if (g_http_8081.server_socket >= 0) {
        close(g_http_8081.server_socket);
        g_http_8081.server_socket = -1;
    }
    
    if (g_http_8081_thread != 0) {
        pthread_join(g_http_8081_thread, NULL);
        g_http_8081_thread = 0;
    }
    
    printf("[HTTP-8081] ✅ Simple HTTP Server stopped\n");
}

/**
 * @brief Server thread function
 */
static void* simple_http_8081_thread(void* arg) {
    (void)arg;
    
    printf("[HTTP-8081] Server thread started\n");
    
    while (g_http_8081_running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(g_http_8081.server_socket, 
                                  (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            if (g_http_8081_running) {
                fprintf(stderr, "[HTTP-8081] Accept failed: %s\n", strerror(errno));
            }
            continue;
        }
        
        // Handle client request
        simple_http_8081_handle_client(client_socket);
        close(client_socket);
    }
    
    printf("[HTTP-8081] Server thread stopped\n");
    return NULL;
}

/**
 * @brief Handle client request
 */
static void simple_http_8081_handle_client(int client_socket) {
    char buffer[1024];
    ssize_t received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (received <= 0) {
        return;
    }
    
    buffer[received] = '\0';
    
    // Parse request line
    char method[16], path[256], version[16];
    if (sscanf(buffer, "%15s %255s %15s", method, path, version) != 3) {
        simple_http_8081_send_response(client_socket, 
            "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n{\"error\":\"Invalid request\"}");
        return;
    }
    
    printf("[HTTP-8081] %s %s\n", method, path);
    
    // Handle specific endpoints
    if (strcmp(path, "/health") == 0) {
        time_t now = time(NULL);
        char response[512];
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"success\":true,\"status\":\"healthy\",\"service\":\"http-8081\",\"port\":8081,\"timestamp\":%ld}",
            now);
        simple_http_8081_send_response(client_socket, response);
    }
    else if (strcmp(path, "/api/v1/status") == 0) {
        time_t now = time(NULL);
        char response[512];
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"success\":true,\"data\":{\"service\":\"http-8081\",\"port\":8081,\"timestamp\":%ld}}",
            now);
        simple_http_8081_send_response(client_socket, response);
    }
    else if (strcmp(path, "/api/v1/robot/status") == 0) {
        time_t now = time(NULL);
        char response[512];
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"success\":true,\"data\":{\"robot_id\":\"OHT-50-001\",\"status\":\"idle\",\"port\":8081,\"timestamp\":%ld}}",
            now);
        simple_http_8081_send_response(client_socket, response);
    }
    else {
        // Redirect to port 8080
        char response[512];
        snprintf(response, sizeof(response),
            "HTTP/1.1 302 Found\r\n"
            "Location: http://localhost:8080%s\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"redirect\":\"http://localhost:8080%s\",\"message\":\"Use port 8080 for full API\"}",
            path, path);
        simple_http_8081_send_response(client_socket, response);
    }
}

/**
 * @brief Send HTTP response
 */
static void simple_http_8081_send_response(int client_socket, const char* response) {
    send(client_socket, response, strlen(response), 0);
}
