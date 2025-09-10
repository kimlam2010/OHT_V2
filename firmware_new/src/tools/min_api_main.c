#include <stdio.h>
#include <unistd.h>
#include "../app/api/api_manager.h"
#include "../app/api/api_endpoints.h"
#include "../../hal/peripherals/hal_lidar.h"
#include "../app/websocket_server.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

static void *lidar_ws_broadcaster(void *arg){ (void)arg; 
    // Simple 10 Hz broadcaster with max reducer
    for(;;){
        lidar_scan_data_t scan_data; 
        if (hal_lidar_get_scan_data(&scan_data)==HAL_STATUS_OK && scan_data.scan_complete){
            uint32_t bins[360]; memset(bins,0,sizeof(bins));
            for (uint16_t i=0;i<scan_data.point_count;i++){
                uint16_t dist = scan_data.points[i].distance_mm;
                int ang = scan_data.points[i].angle_deg;
                int a = ((ang % 360) + 360) % 360;
                if (dist > bins[a]) bins[a] = dist;
            }

            // Interpolate gaps to ensure 360 filled values
            for (int a = 0; a < 360; a++) {
                if (bins[a] == 0) {
                    int left = -1, right = -1;
                    for (int i = 1; i < 180; i++) { int idx = (a - i + 360) % 360; if (bins[idx] > 0) { left = idx; break; } }
                    for (int i = 1; i < 180; i++) { int idx = (a + i) % 360; if (bins[idx] > 0) { right = idx; break; } }
                    if (left != -1 && right != -1) {
                        int dl = (a - left + 360) % 360;
                        int dr = (right - a + 360) % 360;
                        int dt = dl + dr; if (dt>0) bins[a] = (bins[left] * dr + bins[right] * dl) / dt;
                    } else if (left != -1) {
                        bins[a] = bins[left];
                    } else if (right != -1) {
                        bins[a] = bins[right];
                    }
                }
            }
            // Build compact JSON
            char *json = NULL; size_t cap = 1024 + 360*12; json = (char*)malloc(cap);
            if (json){
                size_t pos=0; pos += snprintf(json+pos, cap-pos,
                    "{\"type\":\"lidar_frame_360\",\"timestamp_us\":%lu,\"frame_360\":[",
                    scan_data.scan_timestamp_us);
                for (int a=0;a<360;a++){
                    if (pos+16>=cap){ size_t ns=cap*2; char *t=(char*)realloc(json,ns); if(!t){ free(json); json=NULL; break; } json=t; cap=ns; }
                    pos += snprintf(json+pos, cap-pos, "%s%u", (a>0)?",":"", bins[a]);
                }
                if (json){
                    if (pos+4>=cap){ char *t=(char*)realloc(json,cap+8); if(!t){ free(json); json=NULL; } else { json=t; cap+=8; } }
                    if (json){ pos += snprintf(json+pos, cap-pos, "]}"); ws_server_broadcast_text(json); free(json); }
                }
            }
        }
        usleep(100000); // 10 Hz
    }
    return NULL;
}

int main(){
    // Initialize LiDAR first
    lidar_config_t lidar_cfg = {
        .device_path = "/dev/ttyUSB0",
        .baud_rate = 460800,
        .scan_rate_hz = 10,
        .emergency_stop_mm = 500,
        .warning_mm = 1000,
        .safe_mm = 2000,
        .sample_rate_hz = 5000,
        .angular_resolution = 0.72
    };
    
    hal_status_t lidar_status = hal_lidar_init(&lidar_cfg);
    if (lidar_status != HAL_STATUS_OK) {
        printf("⚠️  LiDAR init failed (status=%d), using mock mode\n", lidar_status);
    } else {
        printf("✅ LiDAR initialized successfully\n");
    }
    
    // Initialize API server (HTTP)
    api_mgr_config_t cfg; cfg.http_port = 8080;
    if (api_manager_init(&cfg)!=0){ perror("api_manager_init"); return 1; }
    api_register_minimal_endpoints();
    if (api_manager_start()!=0){ perror("api_manager_start"); return 2; }
    printf("Minimal API server started on :%u\n", (unsigned)cfg.http_port);
    fflush(stdout);

    // Initialize WebSocket server on 8081
    ws_server_config_t ws_cfg = {0};
    ws_cfg.port = 8081; ws_cfg.timeout_ms = 5000; ws_cfg.max_clients = 10; ws_cfg.max_message_size = 4096; ws_cfg.max_frame_size = 8192; ws_cfg.ping_interval_ms = 30000; ws_cfg.pong_timeout_ms = 10000; ws_cfg.enable_compression=false; ws_cfg.enable_authentication=false; strncpy(ws_cfg.server_name, "OHT50-WS", sizeof(ws_cfg.server_name)-1);
    if (ws_server_init(&ws_cfg)==HAL_STATUS_OK){ ws_server_start(); printf("WebSocket server started on :%u\n", ws_cfg.port); }

    // Start LiDAR WS broadcaster thread
    pthread_t th; pthread_create(&th,NULL,lidar_ws_broadcaster,NULL); pthread_detach(th);
    for(;;) { sleep(1); }
    return 0;
}
