#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "api_manager.h"
#include "api_endpoints.h"

#define BACKLOG 16

typedef struct { const char *path; api_mgr_http_method_t method; int (*handler)(const api_mgr_http_request_t*, api_mgr_http_response_t*);} ep_t;
static ep_t g_eps[64];
static int g_ep_count=0;
static int g_srv_fd=-1;
static uint16_t g_port=8080;
static pthread_t g_thread;
static volatile int g_running=0;

static ssize_t send_all(int fd, const void *buf, size_t len){ const char *p=(const char*)buf; size_t off=0; while(off<len){ ssize_t n=send(fd,p+off,len-off,0); if(n<0){ if(errno==EINTR) continue; return -1;} if(n==0) break; off+=n;} return (ssize_t)off; }

static int parse_request_line(const char *buf, api_mgr_http_request_t *req){ char m[8]={0}, p[API_MANAGER_MAX_PATH_LENGTH]={0}; if(sscanf(buf, "%7s %255s", m, p)!=2) return -1; if(strcmp(m,"GET")==0) req->method=API_MGR_HTTP_GET; else if(strcmp(m,"POST")==0) req->method=API_MGR_HTTP_POST; else return -1; strncpy(req->path,p,sizeof(req->path)-1); req->path[sizeof(req->path)-1]='\0'; return 0; }

static int parse_http_request(const char *buf, api_mgr_http_request_t *req) {
    // Validate parameters
    if (!buf || !req) {
        printf("[ERROR] API_MANAGER::parse_http_request - Invalid parameters\n");
        return -1;
    }
    
    // Initialize request structure
    req->header_count = 0;
    
    // Parse request line first
    if (parse_request_line(buf, req) != 0) {
        printf("[ERROR] API_MANAGER::parse_http_request - Failed to parse request line\n");
        return -1;
    }
    
    // Parse headers
    const char *line_start = strchr(buf, '\n');
    if (line_start) {
        line_start++; // Skip first \n
        
        while (line_start && *line_start && req->header_count < API_MANAGER_MAX_HEADERS) {
            const char *line_end = strchr(line_start, '\n');
            if (!line_end) break;
            
            // Check for end of headers (empty line)
            if (line_end - line_start <= 2) break; // \r\n or just \n
            
            // Parse header: "Name: Value"
            const char *colon = strchr(line_start, ':');
            if (colon && colon < line_end) {
                size_t name_len = colon - line_start;
                size_t value_start = colon + 1 - line_start;
                
                // Skip whitespace after colon
                while (value_start < (size_t)(line_end - line_start) && 
                       (line_start[value_start] == ' ' || line_start[value_start] == '\t')) {
                    value_start++;
                }
                
                size_t value_len = line_end - line_start - value_start;
                if (line_end > line_start && *(line_end-1) == '\r') value_len--; // Remove \r
                
                if (name_len < sizeof(req->headers[0].name) && 
                    value_len < sizeof(req->headers[0].value)) {
                    strncpy(req->headers[req->header_count].name, line_start, name_len);
                    req->headers[req->header_count].name[name_len] = '\0';
                    
                    strncpy(req->headers[req->header_count].value, line_start + value_start, value_len);
                    req->headers[req->header_count].value[value_len] = '\0';
                    
                    req->header_count++;
                }
            }
            
            line_start = line_end + 1;
        }
    }
    
    // Find body for POST requests
    if (req->method == API_MGR_HTTP_POST) {
        const char *body_start = strstr(buf, "\r\n\r\n");
        if (body_start) {
            body_start += 4; // Skip past \r\n\r\n
            size_t body_len = strlen(body_start);
            if (body_len > 0) {
                req->body = malloc(body_len + 1);
                if (req->body) {
                    strncpy(req->body, body_start, body_len);
                    req->body[body_len] = '\0';
                    req->body_length = body_len;
                }
            }
        }
    }
    return 0;
}

static int api_handle_module_status_by_id_router(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){ return api_handle_module_status_by_id(req,res); }

static int route_request(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
 // Check authentication for protected endpoints
 if (req->method == API_MGR_HTTP_POST && 
     (strstr(req->path, "/config/") || strstr(req->path, "/state/"))) {
     
     // Extract Authorization header
     const char *auth_header = NULL;
     for (int h = 0; h < req->header_count; h++) {
         if (strcasecmp(req->headers[h].name, "Authorization") == 0) {
             auth_header = req->headers[h].value;
             break;
         }
     }
     
     // Validate authentication
     int auth_result = api_manager_validate_auth_header(auth_header);
     if (auth_result < 0) {
         return api_manager_create_auth_error_response(res);
     }
     
     printf("[API_SECURITY] ✅ Authenticated request: %s %s\n", 
            req->method == API_MGR_HTTP_POST ? "POST" : "GET", req->path);
 }
 
 // Compare path ignoring optional query string (anything after '?')
 for(int i=0;i<g_ep_count;i++){
  if(g_eps[i].method!=req->method) continue;
  const char *q = strchr(req->path, '?');
  size_t req_path_len = q ? (size_t)(q - req->path) : strlen(req->path);
  size_t ep_len = strlen(g_eps[i].path);
  if(ep_len==req_path_len && strncmp(g_eps[i].path, req->path, req_path_len)==0){
   return g_eps[i].handler(req,res);
  }
 }
 // dynamic path: /api/v1/modules/{id}/*
 if(req->method==API_MGR_HTTP_GET || req->method==API_MGR_HTTP_POST){
  const char *prefix = "/api/v1/modules/";
  size_t plen = strlen(prefix);
  if(strncmp(req->path, prefix, plen)==0){
   printf("[API_DEBUG] Found modules prefix, path: %s\n", req->path);
   const char *rest = req->path + plen; // e.g. "2/telemetry" or "2/data" or "2/registers/0x0000"
   // parse digits for {id}
   const char *p = rest;
   if(*p){
    while(*p && *p>='0' && *p<='9') p++;
    if(p>rest){
     // Check for different endpoints
     if(strcmp(p, "/status")==0){
      return api_handle_module_status_by_id_router(req,res);
     } else if(strcmp(p, "/telemetry")==0){
      printf("[API_DEBUG] Routing to module telemetry handler\n");
      return api_handle_module_telemetry(req,res);
     } else if(strcmp(p, "/data")==0){
      // NEW: Route to data endpoint (metadata + values)
      extern int api_get_module_data(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
      printf("[API_DEBUG] Routing to module data handler\n");
      return api_get_module_data(req,res);
     } else if(strncmp(p, "/registers/", 11)==0){
      // NEW: Route to register write endpoint
      if(req->method==API_MGR_HTTP_POST){
       extern int api_write_register(const api_mgr_http_request_t *req, api_mgr_http_response_t *res);
       printf("[API_DEBUG] Routing to register write handler\n");
       return api_write_register(req,res);
      }
     } else if(strcmp(p, "/config")==0){
      if(req->method==API_MGR_HTTP_GET){
       return api_handle_module_config_get(req,res);
      } else if(req->method==API_MGR_HTTP_POST){
       return api_handle_module_config_set(req,res);
      }
     } else if(strcmp(p, "/history")==0){
      return api_handle_module_history(req,res);
     } else if(strcmp(p, "/health")==0){
      return api_handle_module_health(req,res);
     } else if(strcmp(p, "/command")==0 && req->method==API_MGR_HTTP_POST){
      return api_handle_module_command(req,res);
     }
    }
   }
  }
 }
 return api_manager_create_error_response(res, API_MGR_RESPONSE_NOT_FOUND, "Not Found");
}

static void *srv_loop(void *arg){ (void)arg; struct sockaddr_in addr={0}; addr.sin_family=AF_INET; addr.sin_addr.s_addr=htonl(INADDR_ANY); addr.sin_port=htons(g_port);
 g_srv_fd=socket(AF_INET, SOCK_STREAM, 0);
 if(g_srv_fd<0){ fprintf(stderr,"[API] socket() failed: %s\n", strerror(errno)); return NULL; }
 int opt=1; setsockopt(g_srv_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
 if(bind(g_srv_fd,(struct sockaddr*)&addr,sizeof(addr))<0){ fprintf(stderr,"[API] bind() failed on port %d: %s\n", g_port, strerror(errno)); close(g_srv_fd); return NULL; }
 if(listen(g_srv_fd,BACKLOG)<0){ fprintf(stderr,"[API] listen() failed: %s\n", strerror(errno)); close(g_srv_fd); return NULL; }
 printf("[API] Successfully bound to port %d\n", g_port);
 g_running=1;
 while(g_running){ int cfd=accept(g_srv_fd,NULL,NULL); if(cfd<0){ if(errno==EINTR) continue; break; }
 char buf[2048]={0}; ssize_t r=recv(cfd,buf,sizeof(buf)-1,0); if(r<=0){ close(cfd); continue; }
 struct timespec t0, t1; clock_gettime(CLOCK_MONOTONIC, &t0);
 api_mgr_http_request_t req={0}; api_mgr_http_response_t res={0};
 if(parse_http_request(buf,&req)!=0){ api_manager_create_error_response(&res,API_MGR_RESPONSE_BAD_REQUEST,"Bad Request"); }
 else { route_request(&req,&res); }
 char head[512]; int bl=(int)res.body_length; if(bl<0) bl=0; int n=snprintf(head,sizeof(head),
     "HTTP/1.1 %d OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n",
     (int)res.status_code, bl);
 send_all(cfd, head, (size_t)n);
 if(res.body && res.body_length){ send_all(cfd, res.body, res.body_length); }
 shutdown(cfd, SHUT_WR);
 free(res.body);
 free(req.body);  // Clean up request body
 close(cfd);
 clock_gettime(CLOCK_MONOTONIC, &t1);
 double ms = (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) / 1.0e6;
 if (ms > 100.0) {
  fprintf(stderr, "[API] slow %s %s -> %.1f ms\n", req.method==API_MGR_HTTP_POST?"POST":"GET", req.path, ms);
 } else {
  fprintf(stderr, "[API] %s %s -> %.1f ms\n", req.method==API_MGR_HTTP_POST?"POST":"GET", req.path, ms);
 }
 }
 return NULL; }

int api_manager_init(const api_mgr_config_t *config){ g_port = config && config->http_port? config->http_port:8080; g_ep_count=0; return 0; }
int api_manager_start(void){ return pthread_create(&g_thread,NULL,srv_loop,NULL); }
int api_manager_stop(void){ g_running=0; if(g_srv_fd>=0) close(g_srv_fd); return 0; }
int api_manager_deinit(void){ return 0; }

int api_manager_register_endpoint(const char *path, api_mgr_http_method_t method,
                                  int (*handler)(const api_mgr_http_request_t*, api_mgr_http_response_t*)){
    if(g_ep_count>= (int)(sizeof(g_eps)/sizeof(g_eps[0]))) return -1;
    g_eps[g_ep_count++] = (ep_t){ .path=path, .method=method, .handler=handler };
    return 0;
}

int api_manager_create_success_response(api_mgr_http_response_t *response, const char *json){
    response->status_code = API_MGR_RESPONSE_OK;
    size_t len = json? strlen(json):0;
    response->body = (char*)malloc(len+1);
    if(!response->body) return -1;
    memcpy(response->body, json, len);
    response->body[len]='\0';
    response->body_length = len;
    return 0;
}

int api_manager_create_error_response(api_mgr_http_response_t *response, api_mgr_http_response_code_t code, const char *msg){
    response->status_code = code;
    char buf[256]; snprintf(buf,sizeof(buf),"{\"success\":false,\"message\":\"%s\"}", msg?msg:"error");
    size_t len=strlen(buf); response->body=(char*)malloc(len+1); if(!response->body) return -1; memcpy(response->body,buf,len+1); response->body_length=len; return 0;
}

// Security function implementations
int api_manager_validate_auth_header(const char *auth_header) {
    if (!auth_header) {
        printf("[API_AUTH] Missing Authorization header\n");
        return -1;
    }
    
    // Check Bearer token format
    if (strncmp(auth_header, "Bearer ", 7) != 0) {
        printf("[API_AUTH] Invalid Authorization format\n");
        return -1;
    }
    
    const char* token = auth_header + 7;
    
    // Simple token validation (same as HTTP server)
    const char* valid_tokens[] = {
        "oht50_admin_token_2025",
        "oht50_operator_token_2025", 
        "oht50_readonly_token_2025"
    };
    
    for (int i = 0; i < 3; i++) {
        if (strcmp(token, valid_tokens[i]) == 0) {
            printf("[API_AUTH] ✅ Token validated: %s\n", 
                   i == 0 ? "ADMIN" : i == 1 ? "OPERATOR" : "READONLY");
            return i; // Return role level (0=admin, 1=operator, 2=readonly)
        }
    }
    
    printf("[API_AUTH] ❌ Invalid or expired token\n");
    return -1;
}

int api_manager_create_auth_error_response(api_mgr_http_response_t *response) {
    const char* auth_error = 
        "{"
        "\"success\":false,"
        "\"error\":\"Authentication required\","
        "\"message\":\"Please provide valid Bearer token\","
        "\"example\":\"Authorization: Bearer oht50_admin_token_2025\""
        "}";
    return api_manager_create_error_response(response, API_MGR_RESPONSE_UNAUTHORIZED, auth_error);
}

/* constructor bootstrap removed to avoid double-start conflicts; API is started from main */
