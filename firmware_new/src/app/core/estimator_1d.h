#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float x_est_mm;            // Estimated position (mm)
    float v_mm_s;              // Estimated velocity (mm/s)
    uint64_t last_update_ms;   // Last estimator update time (ms)
    uint32_t freshness_ms;     // Freshness (ms)
    bool health_online;        // Health status
} est1d_state_t;

int estimator_1d_init(void);
int estimator_1d_reset(void);
int estimator_1d_update_timestamp(uint64_t now_ms);
int estimator_1d_update_velocity_proxy(float v_mm_s, uint64_t now_ms);
int estimator_1d_anchor_rfid(float anchor_x_mm, float trust_0_1, uint64_t now_ms);
int estimator_1d_apply_zupt(uint64_t now_ms);
int estimator_1d_get_state(est1d_state_t *out);

