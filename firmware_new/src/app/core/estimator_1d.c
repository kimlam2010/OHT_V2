#include "estimator_1d.h"
#include <string.h>

static est1d_state_t g_est = {0};

int estimator_1d_init(void){ memset(&g_est,0,sizeof(g_est)); g_est.health_online=false; return 0; }
int estimator_1d_reset(void){ return estimator_1d_init(); }

int estimator_1d_update_timestamp(uint64_t now_ms){
    if (g_est.last_update_ms==0) { g_est.last_update_ms = now_ms; g_est.freshness_ms = 0; return 0; }
    uint64_t dt = now_ms - g_est.last_update_ms; g_est.last_update_ms = now_ms; g_est.freshness_ms = (uint32_t)dt; return 0;
}

int estimator_1d_update_velocity_proxy(float v_mm_s, uint64_t now_ms){
    // Integrate v to x with simple clamp; IMU/ZUPT/RFID to be wired later per Gate B
    if (g_est.last_update_ms==0) g_est.last_update_ms = now_ms;
    float dt_s = (now_ms - g_est.last_update_ms) / 1000.0f;
    g_est.x_est_mm += v_mm_s * dt_s;
    g_est.v_mm_s = v_mm_s;
    g_est.last_update_ms = now_ms;
    g_est.health_online = true; // provisional until data source health is wired
    return 0;
}

int estimator_1d_anchor_rfid(float anchor_x_mm, float trust_0_1, uint64_t now_ms){
    (void)now_ms; if (trust_0_1 < 0.0f) trust_0_1 = 0.0f; if (trust_0_1 > 1.0f) trust_0_1 = 1.0f;
    g_est.x_est_mm = (1.0f - trust_0_1) * g_est.x_est_mm + trust_0_1 * anchor_x_mm;
    return 0;
}

int estimator_1d_apply_zupt(uint64_t now_ms){ (void)now_ms; g_est.v_mm_s = 0.0f; return 0; }

int estimator_1d_get_state(est1d_state_t *out){ if(!out) return -1; *out = g_est; return 0; }


