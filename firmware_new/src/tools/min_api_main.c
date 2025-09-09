#include <stdio.h>
#include <unistd.h>
#include "../app/api/api_manager.h"
#include "../app/api/api_endpoints.h"
int main(){
    api_mgr_config_t cfg; cfg.http_port = 8080;
    if (api_manager_init(&cfg)!=0){ perror("api_manager_init"); return 1; }
    api_register_minimal_endpoints();
    if (api_manager_start()!=0){ perror("api_manager_start"); return 2; }
    printf("Minimal API server started on :%u\n", (unsigned)cfg.http_port);
    fflush(stdout);
    for(;;) { sleep(1); }
    return 0;
}
