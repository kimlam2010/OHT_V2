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
static ep_t g_eps[32];
static int g_ep_count=0;
static int g_srv_fd=-1;
static uint16_t g_port=8080;
static pthread_t g_thread;
static volatile int g_running=0;

static ssize_t send_all(int fd, const void *buf, size_t len){ const char *p=(const char*)buf; size_t off=0; while(off<len){ ssize_t n=send(fd,p+off,len-off,0); if(n<0){ if(errno==EINTR) continue; return -1;} if(n==0) break; off+=n;} return (ssize_t)off; }

static int parse_request_line(const char *buf, api_mgr_http_request_t *req){ char m[8]={0}, p[API_MANAGER_MAX_PATH_LENGTH]={0}; if(sscanf(buf, "%7s %255s", m, p)!=2) return -1; if(strcmp(m,"GET")==0) req->method=API_MGR_HTTP_GET; else if(strcmp(m,"POST")==0) req->method=API_MGR_HTTP_POST; else return -1; strncpy(req->path,p,sizeof(req->path)-1); req->path[sizeof(req->path)-1]='\0'; return 0; }

static int api_handle_module_status_by_id_router(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){ return api_handle_module_status_by_id(req,res); }

static int route_request(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
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
 // dynamic path: /api/v1/modules/{id}/status
 if(req->method==API_MGR_HTTP_GET){
  const char *prefix = "/api/v1/modules/";
  size_t plen = strlen(prefix);
  if(strncmp(req->path, prefix, plen)==0){
   const char *rest = req->path + plen; // e.g. "1/status"
   // parse digits for {id}
   const char *p = rest;
   if(*p){
    while(*p && *p>='0' && *p<='9') p++;
    if(p>rest && strcmp(p, "/status")==0){
     return api_handle_module_status_by_id_router(req,res);
    }
   }
  }
 }
 return api_manager_create_error_response(res, API_MGR_RESPONSE_NOT_FOUND, "Not Found");
}

static void *srv_loop(void *arg){ (void)arg; struct sockaddr_in addr={0}; addr.sin_family=AF_INET; addr.sin_addr.s_addr=htonl(INADDR_ANY); addr.sin_port=htons(g_port);
 g_srv_fd=socket(AF_INET, SOCK_STREAM, 0);
 int opt=1; setsockopt(g_srv_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
 bind(g_srv_fd,(struct sockaddr*)&addr,sizeof(addr));
 listen(g_srv_fd,BACKLOG);
 g_running=1;
 while(g_running){ int cfd=accept(g_srv_fd,NULL,NULL); if(cfd<0){ if(errno==EINTR) continue; break; }
 char buf[2048]={0}; ssize_t r=recv(cfd,buf,sizeof(buf)-1,0); if(r<=0){ close(cfd); continue; }
 struct timespec t0, t1; clock_gettime(CLOCK_MONOTONIC, &t0);
 api_mgr_http_request_t req={0}; api_mgr_http_response_t res={0};
 if(parse_request_line(buf,&req)!=0){ api_manager_create_error_response(&res,API_MGR_RESPONSE_BAD_REQUEST,"Bad Request"); }
 else { route_request(&req,&res); }
 char head[512]; int bl=(int)res.body_length; if(bl<0) bl=0; int n=snprintf(head,sizeof(head),
     "HTTP/1.1 %d OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\nConnection: close\r\n\r\n",
     (int)res.status_code, bl);
 send_all(cfd, head, (size_t)n);
 if(res.body && res.body_length){ send_all(cfd, res.body, res.body_length); }
 shutdown(cfd, SHUT_WR);
 free(res.body);
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

/* constructor bootstrap removed to avoid double-start conflicts; API is started from main */
