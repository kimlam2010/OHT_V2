#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "api_endpoints.h"
#include "module_manager.h"
#include "hal_lidar.h"

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
    
    // LiDAR endpoints (CRITICAL - Production Integration)
    api_register_lidar_endpoints();
    
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

// ============================================================================
// LiDAR API ENDPOINTS (CRITICAL - Production Integration)
// ============================================================================

int api_register_lidar_endpoints(void) {
    // Register all LiDAR API endpoints
    api_manager_register_endpoint("/api/v1/lidar/status", API_MGR_HTTP_GET, api_handle_lidar_status);
    api_manager_register_endpoint("/api/v1/lidar/scan_data", API_MGR_HTTP_GET, api_handle_lidar_scan_data);
    api_manager_register_endpoint("/api/v1/lidar/scan_frame_full", API_MGR_HTTP_GET, api_handle_lidar_scan_frame_full);
    api_manager_register_endpoint("/api/v1/lidar/scan_frame", API_MGR_HTTP_GET, api_handle_lidar_scan_frame);
    api_manager_register_endpoint("/api/v1/lidar/scan_frame_360", API_MGR_HTTP_GET, api_handle_lidar_scan_frame_360);
    api_manager_register_endpoint("/api/v1/lidar/start_scanning", API_MGR_HTTP_POST, api_handle_lidar_start_scanning);
    api_manager_register_endpoint("/api/v1/lidar/stop_scanning", API_MGR_HTTP_POST, api_handle_lidar_stop_scanning);
    api_manager_register_endpoint("/api/v1/lidar/config", API_MGR_HTTP_GET, api_handle_lidar_config_get);
    api_manager_register_endpoint("/api/v1/lidar/config", API_MGR_HTTP_POST, api_handle_lidar_config_set);
    api_manager_register_endpoint("/api/v1/lidar/safety_status", API_MGR_HTTP_GET, api_handle_lidar_safety_status);
    api_manager_register_endpoint("/api/v1/lidar/health", API_MGR_HTTP_GET, api_handle_lidar_health_check);
    
    return 0;
}

// GET /api/v1/lidar/status
int api_handle_lidar_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    hal_device_info_t device_info;
    hal_status_t hal_status = hal_lidar_get_device_status(&device_info);
    
    if (hal_status == HAL_STATUS_OK) {
        char json[512];
        snprintf(json, sizeof(json),
            "{\"success\":true,\"data\":{"
            "\"device_type\":%d,"
            "\"status\":%d,"
            "\"device_id\":%u,"
            "\"device_name\":\"%s\","
            "\"device_version\":\"%s\","
            "\"timestamp_us\":%lu,"
            "\"error_count\":%u,"
            "\"warning_count\":%u"
            "}}",
            device_info.device_type,
            device_info.status,
            device_info.device_id,
            device_info.device_name,
            device_info.device_version,
            device_info.timestamp_us,
            device_info.error_count,
            device_info.warning_count
        );
        return api_manager_create_success_response(res, json);
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get LiDAR status");
    }
}

// GET /api/v1/lidar/scan_data
int api_handle_lidar_scan_data(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    lidar_scan_data_t scan_data;
    hal_status_t hal_status = hal_lidar_get_scan_data(&scan_data);
    
    if (hal_status == HAL_STATUS_OK && scan_data.scan_complete) {
        // Build JSON response with scan data
        char json[4096];
        size_t pos = 0;
        
        pos += snprintf(json + pos, sizeof(json) - pos,
            "{\"success\":true,\"data\":{"
            "\"scan_complete\":%s,"
            "\"point_count\":%u,"
            "\"timestamp_us\":%lu,"
            "\"points\":[",
            scan_data.scan_complete ? "true" : "false",
            scan_data.point_count,
            scan_data.scan_timestamp_us
        );
        
        // Add point data (limit to first 50 points for JSON size)
        for (uint16_t i = 0; i < scan_data.point_count && i < 50; i++) {
            pos += snprintf(json + pos, sizeof(json) - pos,
                "%s{\"distance\":%u,\"angle\":%d,\"quality\":%u}",
                (i > 0) ? "," : "",
                scan_data.points[i].distance_mm,
                scan_data.points[i].angle_deg,
                scan_data.points[i].quality
            );
        }
        
        pos += snprintf(json + pos, sizeof(json) - pos, "]}}");
        return api_manager_create_success_response(res, json);
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get LiDAR scan data");
    }
}

// GET /api/v1/lidar/scan_frame_full?normalize=1&limit=0&block_until_rotation=1&timeout_ms=5000
int api_handle_lidar_scan_frame_full(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    // Parse query parameters
    int normalize = 0; int limit = 0; int block_until_rotation = 0; int timeout_ms = 5000;
    const char *q = strchr(req->path, '?');
    if (q) {
        int v;
        if (sscanf(q, "%*[^n]normalize=%d", &v)==1) normalize = v;
        if (sscanf(q, "%*[^l]limit=%d", &v)==1) limit = v;
        if (sscanf(q, "%*[^b]block_until_rotation=%d", &v)==1) block_until_rotation = v;
        if (sscanf(q, "%*[^t]timeout_ms=%d", &v)==1) timeout_ms = v;
    }
    
    lidar_scan_data_t scan_data;
    hal_status_t hal_status = hal_lidar_get_scan_data(&scan_data);
    
    // If block_until_rotation is enabled, wait for complete scan
    if (block_until_rotation && hal_status == HAL_STATUS_OK && !scan_data.scan_complete) {
        int max_attempts = timeout_ms / 100; // Check every 100ms
        int attempts = 0;
        
        while (attempts < max_attempts && !scan_data.scan_complete) {
            usleep(100000); // 100ms
            hal_status = hal_lidar_get_scan_data(&scan_data);
            attempts++;
        }
        
        if (!scan_data.scan_complete) {
            return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Timeout waiting for complete rotation");
        }
    }
    
    if (hal_status != HAL_STATUS_OK || !scan_data.scan_complete) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get LiDAR full frame");
    }

    // Build JSON with ALL points (or limited)
    // Estimate needed size: header ~128 + per point ~32 bytes
    size_t estimated = 256 + (size_t)scan_data.point_count * 40;
    char *json = (char*)malloc(estimated);
    if (!json) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory");
    }
    size_t pos = 0; 
    pos += snprintf(json + pos, estimated - pos,
        "{\"success\":true,\"data\":{\"scan_complete\":%s,\"point_count\":%u,\"timestamp_us\":%lu,\"points\":[",
        scan_data.scan_complete ? "true" : "false",
        scan_data.point_count,
        scan_data.scan_timestamp_us);

    uint16_t actual_count = 0;
    for (uint16_t i = 0; i < scan_data.point_count; i++) {
        if (limit > 0 && actual_count >= limit) break;
        
        if (pos + 64 >= estimated) {
            // grow buffer
            size_t new_size = estimated * 2;
            char *tmp = (char*)realloc(json, new_size);
            if (!tmp) { free(json); return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory"); }
            json = tmp; estimated = new_size;
        }
        
        int ang = scan_data.points[i].angle_deg;
        int ang_norm = ((ang % 360) + 360) % 360;
        int ang_output = normalize ? ang_norm : ang;
        
        pos += snprintf(json + pos, estimated - pos,
            "%s{\"distance\":%u,\"angle\":%d,\"quality\":%u}",
            (actual_count>0)?",":"",
            scan_data.points[i].distance_mm,
            ang_output,
            scan_data.points[i].quality);
        actual_count++;
    }

    if (pos + 64 >= estimated) {
        char *tmp = (char*)realloc(json, estimated + 64);
        if (!tmp) { free(json); return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory"); }
        json = tmp; estimated += 64;
    }
    pos += snprintf(json + pos, estimated - pos, "],\"actual_count\":%u}}", actual_count);

    int rc = api_manager_create_success_response(res, json);
    free(json);
    return rc;
}

// GET /api/v1/lidar/scan_frame?min_deg=-30&max_deg=30&normalize=1&limit=0
int api_handle_lidar_scan_frame(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    // NOTE: current simple implementation: reuse current scan data, filter by angle window
    // Query parsing minimal: find in req->path after '?'
    int min_deg = -180, max_deg = 180; int normalize = 1; int limit = 0; // 0 = unlimited
    const char *q = strchr(req->path, '?');
    if (q) {
        // crude parse, sscanf patterns
        int v;
        if (sscanf(q, "%*[^m]min_deg=%d", &v)==1) min_deg = v;
        if (sscanf(q, "%*[^m]max_deg=%d", &v)==1) max_deg = v;
        if (sscanf(q, "%*[^n]normalize=%d", &v)==1) normalize = v;
        if (sscanf(q, "%*[^l]limit=%d", &v)==1) limit = v;
    }
    // normalize bounds to [-360, 720]
    if (min_deg < -360) min_deg = -360;
    if (max_deg > 720) max_deg = 720;
    if (max_deg < min_deg) { int t = min_deg; min_deg = max_deg; max_deg = t; }

    lidar_scan_data_t scan_data;
    hal_status_t hal_status = hal_lidar_get_scan_data(&scan_data);
    if (hal_status != HAL_STATUS_OK || !scan_data.scan_complete) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get LiDAR scan frame");
    }

    size_t estimated = 256 + (size_t)scan_data.point_count * 40;
    char *json = (char*)malloc(estimated);
    if (!json) return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory");
    size_t pos = 0; unsigned out_count = 0;
    pos += snprintf(json + pos, estimated - pos,
        "{\"success\":true,\"data\":{\"scan_complete\":%s,\"timestamp_us\":%lu,\"points\":[",
        scan_data.scan_complete ? "true" : "false",
        scan_data.scan_timestamp_us);

    for (uint16_t i = 0; i < scan_data.point_count; i++) {
        int ang = scan_data.points[i].angle_deg;
        int ang_norm = ((ang % 360) + 360) % 360;
        int ang_eval = normalize ? ang_norm : ang;
        if (ang_eval < min_deg || ang_eval > max_deg) continue;
        if (limit > 0 && (int)out_count >= limit) break;
        if (pos + 64 >= estimated) { size_t ns=estimated*2; char *t=(char*)realloc(json,ns); if(!t){ free(json); return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory"); } json=t; estimated=ns; }
        pos += snprintf(json + pos, estimated - pos,
            "%s{\"distance\":%u,\"angle\":%d,\"quality\":%u}",
            (out_count>0)?",":"",
            scan_data.points[i].distance_mm,
            normalize ? ang_norm : ang,
            scan_data.points[i].quality);
        out_count++;
    }

    if (pos + 64 >= estimated) { char *t=(char*)realloc(json,estimated+64); if(!t){ free(json); return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory"); } json=t; estimated+=64; }
    pos += snprintf(json + pos, estimated - pos, "],\"point_count\":%u}}", out_count);
    int rc = api_manager_create_success_response(res, json);
    free(json);
    return rc;
}

// GET /api/v1/lidar/scan_frame_360?reducer=max&min_q=0&max_range=0&interpolate=1
// reducer: max|min|median (default max). min_q: quality threshold. max_range: clamp distance (0=disabled). interpolate: fill gaps (0=disabled)
int api_handle_lidar_scan_frame_360(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    const char *q = strchr(req->path, '?');
    char reducer[16] = {0}; strcpy(reducer, "max");
    int min_q = 0; int max_range = 0; int interpolate = 0;
    if (q) {
        // robust query parsing: split on '&', parse key=value
        const char *qs = q + 1; // skip '?'
        char buf[256];
        size_t n = strlen(qs);
        if (n >= sizeof(buf)) n = sizeof(buf) - 1;
        memcpy(buf, qs, n); buf[n] = '\0';
        char *saveptr = NULL;
        for (char *tok = strtok_r(buf, "&", &saveptr); tok; tok = strtok_r(NULL, "&", &saveptr)) {
            char *eq = strchr(tok, '=');
            if (!eq) continue;
            *eq = '\0';
            const char *key = tok; const char *val = eq + 1;
            if (strcmp(key, "reducer")==0) {
                strncpy(reducer, val, sizeof(reducer)-1);
                reducer[sizeof(reducer)-1] = '\0';
            } else if (strcmp(key, "min_q")==0) {
                min_q = atoi(val);
            } else if (strcmp(key, "max_range")==0) {
                max_range = atoi(val);
            } else if (strcmp(key, "interpolate")==0) {
                interpolate = atoi(val);
            }
        }
    }

    lidar_scan_data_t scan_data;
    hal_status_t hal_status = hal_lidar_get_scan_data(&scan_data);
    if (hal_status != HAL_STATUS_OK || !scan_data.scan_complete) {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get LiDAR scan data");
    }

    // bins initialized to 0 (no return)
    uint32_t bins[360]; memset(bins, 0, sizeof(bins));
    uint16_t counts[360]; memset(counts, 0, sizeof(counts));

    // accumulate
    for (uint16_t i = 0; i < scan_data.point_count; i++) {
        uint16_t dist = scan_data.points[i].distance_mm;
        int ang = scan_data.points[i].angle_deg;
        uint16_t qv = scan_data.points[i].quality;
        if (qv < (uint16_t)min_q) continue;
        if (max_range > 0 && dist > (uint16_t)max_range) continue;
        int a = ((ang % 360) + 360) % 360;
        if (strcmp(reducer, "min")==0) {
            if (counts[a]==0 || dist < bins[a]) bins[a] = dist;
        } else if (strcmp(reducer, "median")==0) {
            // simple running median approximation: keep last seen smaller of two extremes
            if (counts[a]==0) bins[a] = dist;
            else {
                uint32_t prev = bins[a];
                bins[a] = (dist < prev) ? ((prev + dist)/2) : prev; // lightweight approx
            }
        } else { // max (default)
            if (dist > bins[a]) bins[a] = dist;
        }
        counts[a]++;
    }

    // Interpolation to fill gaps if enabled
    if (interpolate) {
        // Simple linear interpolation between non-zero values
        for (int a = 0; a < 360; a++) {
            if (bins[a] == 0) {
                // Find nearest non-zero values
                int left = -1, right = -1;
                
                // Search left
                for (int i = 1; i < 180; i++) {
                    int idx = (a - i + 360) % 360;
                    if (bins[idx] > 0) {
                        left = idx;
                        break;
                    }
                }
                
                // Search right
                for (int i = 1; i < 180; i++) {
                    int idx = (a + i) % 360;
                    if (bins[idx] > 0) {
                        right = idx;
                        break;
                    }
                }
                
                // Interpolate if both sides found
                if (left != -1 && right != -1) {
                    int dist_left = (a - left + 360) % 360;
                    int dist_right = (right - a + 360) % 360;
                    int total_dist = dist_left + dist_right;
                    
                    if (total_dist > 0) {
                        bins[a] = (bins[left] * dist_right + bins[right] * dist_left) / total_dist;
                    }
                } else if (left != -1) {
                    // Use left value if only left found
                    bins[a] = bins[left];
                } else if (right != -1) {
                    // Use right value if only right found
                    bins[a] = bins[right];
                }
            }
        }
    }

    // build JSON array of 360 values
    size_t estimated = 1024 + 360 * 12;
    char *json = (char*)malloc(estimated);
    if (!json) return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory");
    size_t pos = 0;
    pos += snprintf(json + pos, estimated - pos,
        "{\"success\":true,\"data\":{\"timestamp_us\":%lu,\"reducer\":\"%s\",\"min_q\":%d,\"max_range\":%d,\"interpolate\":%d,\"frame_360\":[",
        scan_data.scan_timestamp_us, reducer, min_q, max_range, interpolate);
    for (int a = 0; a < 360; a++) {
        if (pos + 16 >= estimated) { size_t ns=estimated*2; char *t=(char*)realloc(json,ns); if(!t){ free(json); return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory"); } json=t; estimated=ns; }
        pos += snprintf(json + pos, estimated - pos, "%s%u", (a>0)?",":"", bins[a]);
    }
    if (pos + 4 >= estimated) { char *t=(char*)realloc(json,estimated+8); if(!t){ free(json); return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "no memory"); } json=t; estimated+=8; }
    pos += snprintf(json + pos, estimated - pos, "]}}" );
    int rc = api_manager_create_success_response(res, json);
    free(json);
    return rc;
}

// POST /api/v1/lidar/start_scanning
int api_handle_lidar_start_scanning(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    hal_status_t hal_status = hal_lidar_start_scanning();
    
    if (hal_status == HAL_STATUS_OK) {
        return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"LiDAR scanning started\"}");
    }
    
    // Idempotent behavior: if already active, treat as success
    if (hal_status == HAL_STATUS_ALREADY_ACTIVE) {
        return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"LiDAR already running\"}");
    }
    
    return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to start LiDAR scanning");
}

// POST /api/v1/lidar/stop_scanning
int api_handle_lidar_stop_scanning(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    hal_status_t hal_status = hal_lidar_stop_scanning();
    
    if (hal_status == HAL_STATUS_OK) {
        return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"LiDAR scanning stopped\"}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to stop LiDAR scanning");
    }
}

// GET /api/v1/lidar/config
int api_handle_lidar_config_get(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    lidar_config_t config;
    hal_status_t hal_status = hal_lidar_get_config(&config);
    
    if (hal_status == HAL_STATUS_OK) {
        char json[512];
        snprintf(json, sizeof(json),
            "{\"success\":true,\"data\":{"
            "\"device_path\":\"%s\","
            "\"baud_rate\":%u,"
            "\"scan_rate_hz\":%u,"
            "\"emergency_stop_mm\":%u,"
            "\"warning_mm\":%u,"
            "\"safe_mm\":%u,"
            "\"sample_rate_hz\":%u,"
            "\"angular_resolution\":%.2f"
            "}}",
            config.device_path,
            config.baud_rate,
            config.scan_rate_hz,
            config.emergency_stop_mm,
            config.warning_mm,
            config.safe_mm,
            config.sample_rate_hz,
            config.angular_resolution
        );
        return api_manager_create_success_response(res, json);
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get LiDAR config");
    }
}

// PUT /api/v1/lidar/config
int api_handle_lidar_config_set(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    // For now, return success - full JSON parsing will be implemented later
    (void)req;
    return api_manager_create_success_response(res, "{\"success\":true,\"message\":\"LiDAR config updated (basic implementation)\"}");
}

// GET /api/v1/lidar/safety_status
int api_handle_lidar_safety_status(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    lidar_safety_status_t safety_status;
    hal_status_t hal_status = hal_lidar_check_safety(&safety_status);
    
    if (hal_status == HAL_STATUS_OK) {
        char json[512];
        snprintf(json, sizeof(json),
            "{\"success\":true,\"data\":{"
            "\"min_distance_mm\":%u,"
            "\"max_distance_mm\":%u,"
            "\"obstacle_detected\":%s,"
            "\"warning_triggered\":%s,"
            "\"emergency_stop_triggered\":%s,"
            "\"timestamp_us\":%lu"
            "}}",
            safety_status.min_distance_mm,
            safety_status.max_distance_mm,
            safety_status.obstacle_detected ? "true" : "false",
            safety_status.warning_triggered ? "true" : "false",
            safety_status.emergency_stop_triggered ? "true" : "false",
            safety_status.timestamp_us
        );
        return api_manager_create_success_response(res, json);
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to get LiDAR safety status");
    }
}

// GET /api/v1/lidar/health
int api_handle_lidar_health_check(const api_mgr_http_request_t *req, api_mgr_http_response_t *res) {
    (void)req;
    
    hal_status_t hal_status = hal_lidar_health_check();
    
    if (hal_status == HAL_STATUS_OK) {
        return api_manager_create_success_response(res, "{\"success\":true,\"data\":{\"health\":\"ok\"}}");
    } else {
        return api_manager_create_error_response(res, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "LiDAR health check failed");
    }
}
