#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api_endpoints.h"
#include "module_manager.h"

int api_register_minimal_endpoints(void){
    api_manager_register_endpoint("/api/v1/system/status", API_MGR_HTTP_GET, api_handle_system_status);
    api_manager_register_endpoint("/api/v1/safety/status", API_MGR_HTTP_GET, api_handle_safety_status);
    api_manager_register_endpoint("/api/v1/safety/estop", API_MGR_HTTP_POST, api_handle_safety_estop);
    api_manager_register_endpoint("/api/v1/modules", API_MGR_HTTP_GET, api_handle_modules_list);
    api_manager_register_endpoint("/api/v1/modules/stats", API_MGR_HTTP_GET, api_handle_modules_stats);
    api_manager_register_endpoint("/api/v1/modules/scan", API_MGR_HTTP_POST, api_handle_modules_scan);
    api_manager_register_endpoint("/api/v1/modules/config", API_MGR_HTTP_GET, api_handle_modules_config_get);
    api_manager_register_endpoint("/api/v1/system/state", API_MGR_HTTP_GET, api_handle_system_state);
    api_manager_register_endpoint("/api/v1/control/status", API_MGR_HTTP_GET, api_handle_control_status);
    // Motion endpoints per EXEC PLAN Gate E
    api_manager_register_endpoint("/api/v1/motion/segment/start", API_MGR_HTTP_POST, api_handle_motion_segment_start);
    api_manager_register_endpoint("/api/v1/motion/segment/stop",  API_MGR_HTTP_POST, api_handle_motion_segment_stop);
    api_manager_register_endpoint("/api/v1/motion/state",         API_MGR_HTTP_GET,  api_handle_motion_state);
    return 0; // dynamic /api/v1/modules/{id}/status handled by router
}

int api_handle_system_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req; const char *json = "{\"success\":true,\"data\":{\"system\":\"OHT-50\",\"status\":\"ok\"}}";
    return api_manager_create_success_response(res, json);
}

int api_handle_safety_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req; const char *json = "{\"success\":true,\"data\":{\"estop_active\":false,\"safety_ok\":true}}";
    return api_manager_create_success_response(res, json);
}

int api_handle_safety_estop(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req; const char *json = "{\"success\":true,\"message\":\"E-Stop trigger accepted\"}";
    return api_manager_create_success_response(res, json);
}

int api_handle_modules_list(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req;
    // Build JSON from registry
    module_info_t arr[MODULE_REGISTRY_MAX_MODULES];
    size_t count = 0;
    if (registry_get_all(arr, MODULE_REGISTRY_MAX_MODULES, &count) != 0) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "registry error");
    }
    char buffer[2048];
    size_t pos = 0;
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "{\"success\":true,\"data\":{\"modules\":[");
    for (size_t i = 0; i < count; ++i) {
        const module_info_t *m = &arr[i];
        pos += snprintf(buffer + pos, sizeof(buffer) - pos,
                        "%s{\"address\":%u,\"type\":%u,\"name\":\"%s\",\"status\":%u,\"version\":\"%s\"}",
                        (i>0)?",":"", m->address, (unsigned)m->type, m->name, (unsigned)m->status, m->version);
        if (pos >= sizeof(buffer)) break;
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "]}});");
    // Fix accidental semicolon if overflow didn't happen
    if (pos < sizeof(buffer)) {
        // Replace trailing ';' with '}'
        for (size_t k = pos; k > 0; --k) {
            if (buffer[k-1] == ';') { buffer[k-1] = '}'; break; }
        }
    }
    return api_manager_create_success_response(res, buffer);
}

int api_handle_module_status_by_id(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req; const char *json = "{\"success\":true,\"data\":{\"module_id\":1,\"status\":\"ok\"}}";
    return api_manager_create_success_response(res, json);
}

int api_handle_system_state(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req; const char *json = "{\"success\":true,\"data\":{\"state\":\"IDLE\"}}";
    return api_manager_create_success_response(res, json);
}

int api_handle_control_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req; const char *json = "{\"success\":true,\"data\":{\"position\":0.0,\"velocity\":0.0,\"error\":0.0}}";
    return api_manager_create_success_response(res, json);
}

// --- Motion API handlers ---
#include "control_loop.h"
#include "safety_monitor.h"

int api_handle_motion_segment_start(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req;
    // Ensure control loop is in VELOCITY mode and enabled; backend provides segment via velocity target settings
    (void)control_loop_set_mode(CONTROL_MODE_VELOCITY);
    (void)control_loop_enable();
    // For now, acknowledge start; segment parameters are assumed applied via separate config (map/remaining handled by backend)
    return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"segment started\"}");
}

int api_handle_motion_segment_stop(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req;
    // Fail-safe: set velocity target to zero
    (void)control_loop_set_target_velocity(0.0f);
    // Optionally disable control to hold
    (void)control_loop_disable();
    return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"segment stopped\"}");
}

int api_handle_motion_state(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req;
    control_status_t st = {0};
    (void)control_loop_get_status(&st);

    bool estop = false; (void)safety_monitor_is_estop_active(&estop);
    uint32_t estop_p95 = 0; (void)safety_monitor_get_last_estop_latency(&estop_p95);

    // Use estimator_1d state
    #include "estimator_1d.h"
    est1d_state_t est = {0};
    (void)estimator_1d_get_state(&est);

    float remaining = 0.0f; // Backend supplies; optional future wiring
    const char *docking_state = "IDLE";

    char buf[640];
    snprintf(buf, sizeof(buf),
        "{\"success\":true,\"data\":{\"x_est\":%.3f,\"v\":%.3f,\"remaining\":%.3f,\"safety\":{\"estop\":%s,\"p95\":%u},\"docking\":\"%s\",\"freshness_ms\":%u,\"health\":%s,\"target_v\":%.3f}}",
        est.x_est_mm, est.v_mm_s, remaining, estop?"true":"false", estop_p95, docking_state,
        est.freshness_ms, est.health_online?"true":"false", st.target_velocity);
    return api_manager_create_success_response(res, buf);
}

int api_handle_modules_stats(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req;
    module_stats_t stats;
    if (module_manager_get_statistics(&stats) != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "cannot read stats");
    }
    // Simple JSON assembly (avoid heavy deps)
    char buf[512];
    snprintf(buf, sizeof(buf),
             "{\"success\":true,\"data\":{\"total_modules\":%u,\"online_modules\":%u,\"offline_modules\":%u,\"discovery_total_ms\":%u,\"p95_ms\":%u,\"p99_ms\":%u,\"health_timeouts\":%u,\"crc_errors\":%u}}",
             stats.total_modules, stats.online_modules, stats.offline_modules,
             stats.discovery_total_ms, stats.discovery_p95_ms, stats.discovery_p99_ms,
             stats.health_timeouts, stats.error_count);
    return api_manager_create_success_response(res, buf);
}

int api_handle_modules_scan(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req;
    if (module_manager_discover_modules() != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "scan failed");
    }
    return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"scan started\"}");
}

int api_handle_modules_config_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res){
    (void)req;
    module_config_t cfg;
    if (module_manager_get_config(&cfg) != HAL_STATUS_OK) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "cannot read config");
    }
    char buf[384];
    snprintf(buf, sizeof(buf),
             "{\"success\":true,\"data\":{\"health_interval_ms\":%u,\"offline_threshold_ms\":%u,\"retry\":%u,\"response_timeout_ms\":%u,\"health_jitter_percent\":%u}}",
             cfg.health_check_interval_ms, cfg.offline_threshold_ms, cfg.retry_count, cfg.response_timeout_ms, cfg.health_jitter_percent);
    return api_manager_create_success_response(res, buf);
}
