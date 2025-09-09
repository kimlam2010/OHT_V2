#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

#define API_MANAGER_MAX_HEADERS 16
#define API_MANAGER_MAX_PATH_LENGTH 256
#define API_MANAGER_MAX_CLIENTS 8

typedef enum { API_MGR_HTTP_GET=0, API_MGR_HTTP_POST } api_mgr_http_method_t;
typedef enum { API_MGR_RESPONSE_OK=200, API_MGR_RESPONSE_BAD_REQUEST=400, API_MGR_RESPONSE_NOT_FOUND=404, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR=500 } api_mgr_http_response_code_t;

typedef struct { char name[64]; char value[256]; } api_mgr_http_header_t;
typedef struct {
    api_mgr_http_method_t method;
    char path[API_MANAGER_MAX_PATH_LENGTH];
} api_mgr_http_request_t;

typedef struct {
    api_mgr_http_response_code_t status_code;
    api_mgr_http_header_t headers[API_MANAGER_MAX_HEADERS];
    uint32_t header_count;
    char *body;
    size_t body_length;
} api_mgr_http_response_t;

typedef struct { uint16_t http_port; } api_mgr_config_t;

int api_manager_init(const api_mgr_config_t *config);
int api_manager_start(void);
int api_manager_stop(void);
int api_manager_deinit(void);

int api_manager_register_endpoint(const char *path, api_mgr_http_method_t method,
                                  int (*handler)(const api_mgr_http_request_t*, api_mgr_http_response_t*));

int api_manager_create_success_response(api_mgr_http_response_t *response, const char *json);
int api_manager_create_error_response(api_mgr_http_response_t *response, api_mgr_http_response_code_t code, const char *msg);
