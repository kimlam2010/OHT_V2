# Đặc tả Yêu cầu API Integration - OHT-50 Master Module

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-28  
**Team:** FW  
**Mức độ ưu tiên:** CAO  
**Task:** FW-07 (API Integration & BE/FE Connectivity)

---

## 📋 **Tổng quan**

### **Mục tiêu**
Implement hoàn chỉnh hệ thống API cho OHT-50 Master Module để kết nối với Backend và Frontend, cung cấp:
- HTTP REST API endpoints hoạt động với dữ liệu thực
- WebSocket real-time communication
- Integration với core systems (system_controller, safety_manager, module_manager)
- Authentication và authorization
- Error handling và logging

### **Phạm vi**
- API Manager implementation hoàn chỉnh
- Core system integration
- HTTP/WebSocket server setup
- Module-specific endpoints
- Security implementation

---

## 🎯 **Yêu cầu chức năng**

### **1. Core System Integration (BẮT BUỘC)**

#### **1.1 System Controller Integration**
```c
// Thay thế mock data trong api_endpoints.c
hal_status_t system_get_status(void) {
    system_controller_status_t status;
    hal_status_t result = system_controller_get_status(&status);
    
    if (result == HAL_STATUS_OK) {
        // Convert to API format
        api_system_status_t api_status = {
            .system_name = "OHT-50",
            .version = "1.0.0",
            .status = system_controller_get_state_name(status.current_state),
            .uptime_ms = status.uptime_ms,
            .active_modules = status.system_ready ? 1 : 0,
            .estop_active = !status.safety_ok,
            .safety_ok = status.safety_ok
        };
        return HAL_STATUS_OK;
    }
    return result;
}
```

#### **1.2 Safety Manager Integration**
```c
hal_status_t safety_get_status(void) {
    safety_monitor_status_t safety_status;
    hal_status_t result = safety_monitor_get_status(&safety_status);
    
    if (result == HAL_STATUS_OK) {
        api_safety_status_t api_safety = {
            .estop_active = safety_status.estop_active,
            .safety_ok = safety_status.current_state == SAFETY_MONITOR_STATE_SAFE,
            .safety_level = (uint32_t)safety_status.current_state,
            .last_safety_check = safety_status.last_update_time
        };
        return HAL_STATUS_OK;
    }
    return result;
}
```

#### **1.3 Module Manager Integration**
```c
hal_status_t module_get_list(void) {
    uint8_t module_ids[16];
    uint32_t actual_count;
    hal_status_t result = module_manager_get_registered_modules(module_ids, 16, &actual_count);
    
    if (result == HAL_STATUS_OK) {
        api_modules_list_t modules_list = {0};
        modules_list.module_count = actual_count;
        
        for (uint32_t i = 0; i < actual_count; i++) {
            module_info_t module_info;
            if (module_manager_get_module_info(module_ids[i], &module_info) == HAL_STATUS_OK) {
                modules_list.modules[i].module_id = module_ids[i];
                strncpy(modules_list.modules[i].module_type, 
                       module_manager_get_type_name(module_info.type), 31);
                modules_list.modules[i].online = module_info.status == MODULE_STATUS_ONLINE;
                modules_list.modules[i].last_seen = module_info.last_seen_ms;
            }
        }
        return HAL_STATUS_OK;
    }
    return result;
}
```

### **2. HTTP Server Implementation**

#### **2.1 Server Configuration**
```c
// Trong api_manager.c - cập nhật config
api_mgr_config_t api_config = {
    .http_port = 8080,
    .websocket_port = 8081,
    .http_enabled = true,
    .websocket_enabled = true,
    .cors_enabled = true,
    .cors_origin = "*",
    .max_request_size = 4096,
    .max_response_size = 8192,
    .request_timeout_ms = 5000,
    .websocket_timeout_ms = 30000,
    .authentication_required = false, // Tạm thời disable
    .ssl_enabled = false
};
```

#### **2.2 Endpoint Registration**
```c
// Trong api_manager.c - register tất cả endpoints
hal_status_t api_manager_register_all_endpoints(void) {
    // System endpoints
    api_manager_register_endpoint(&(api_mgr_endpoint_t){
        .path = "/api/v1/system/status",
        .method = API_MGR_HTTP_GET,
        .handler = api_handle_system_status,
        .authentication_required = false
    });
    
    // Safety endpoints
    api_manager_register_endpoint(&(api_mgr_endpoint_t){
        .path = "/api/v1/safety/status",
        .method = API_MGR_HTTP_GET,
        .handler = api_handle_safety_status,
        .authentication_required = false
    });
    
    // Module endpoints
    api_manager_register_endpoint(&(api_mgr_endpoint_t){
        .path = "/api/v1/modules",
        .method = API_MGR_HTTP_GET,
        .handler = api_handle_modules_list,
        .authentication_required = false
    });
    
    // Configuration endpoints
    api_manager_register_endpoint(&(api_mgr_endpoint_t){
        .path = "/api/v1/config",
        .method = API_MGR_HTTP_GET,
        .handler = api_handle_config_get,
        .authentication_required = false
    });
    
    return HAL_STATUS_OK;
}
```

### **3. WebSocket Implementation**

#### **3.1 Real-time Data Broadcasting**
```c
// Trong telemetry_manager.c - broadcast telemetry data
hal_status_t telemetry_broadcast_system_status(void) {
    telemetry_data_t telemetry_data;
    hal_status_t result = telemetry_manager_get_data(&telemetry_data);
    
    if (result == HAL_STATUS_OK) {
        char json_buffer[2048];
        int json_len = telemetry_manager_serialize_json(&telemetry_data, json_buffer, sizeof(json_buffer));
        
        if (json_len > 0) {
            api_manager_broadcast_websocket_message(json_buffer, json_len);
        }
    }
    return result;
}
```

#### **3.2 WebSocket Message Handler**
```c
// Trong api_manager.c - handle WebSocket messages
static hal_status_t handle_websocket_message(const api_mgr_ws_message_t *message) {
    if (message->frame_type == API_MGR_WS_FRAME_TEXT) {
        // Parse JSON command
        if (strstr(message->payload, "\"command\":\"get_status\"") != NULL) {
            // Send immediate status update
            char response[512];
            snprintf(response, sizeof(response), 
                    "{\"type\":\"status_response\",\"data\":{\"timestamp\":%lu}}", 
                    (unsigned long)time(NULL));
            api_manager_send_websocket_message(message->client_id, response, strlen(response));
        }
    }
    return HAL_STATUS_OK;
}
```

### **4. Module-Specific Endpoints**

#### **4.1 Power Module Endpoints**
```c
hal_status_t api_handle_power_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    power_module_data_t power_data;
    hal_status_t result = power_module_handler_read_data(&power_data);
    
    if (result == HAL_STATUS_OK) {
        char json_buffer[1024];
        snprintf(json_buffer, sizeof(json_buffer),
                "{\"battery_voltage\":%.2f,\"battery_current\":%.2f,\"battery_soc\":%d,\"charge_status\":%d}",
                power_data.battery_voltage, power_data.battery_current, 
                power_data.battery_soc, power_data.charge_status);
        
        return api_create_success_response(response, json_buffer);
    }
    
    return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to read power data");
}
```

#### **4.2 Safety Module Endpoints**
```c
hal_status_t api_handle_safety_sensors(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    safety_module_data_t safety_data;
    safety_module_handler_t handler;
    
    // Initialize handler with default config
    safety_module_config_t config = {0};
    config.address = 0x03; // Safety module address
    safety_module_init(&handler, &config);
    
    hal_status_t result = safety_module_get_data(&handler, &safety_data);
    
    if (result == HAL_STATUS_OK) {
        char json_buffer[512];
        snprintf(json_buffer, sizeof(json_buffer),
                "{\"analog_sensors\":[%d,%d,%d,%d],\"digital_sensors\":%d,\"proximity_alert\":%s}",
                safety_data.analog_sensors[0], safety_data.analog_sensors[1],
                safety_data.analog_sensors[2], safety_data.analog_sensors[3],
                safety_data.digital_sensors, safety_data.proximity_alert ? "true" : "false");
        
        return api_create_success_response(response, json_buffer);
    }
    
    return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "Failed to read safety data");
}
```

### **5. Error Handling & Logging**

#### **5.1 Comprehensive Error Handling**
```c
// Trong api_endpoints.c - enhanced error handling
hal_status_t api_handle_system_status(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    // Validate request
    hal_status_t validation = api_validate_request(request, API_MGR_HTTP_GET, "/api/v1/system/status");
    if (validation != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_BAD_REQUEST, "Invalid request");
    }
    
    // Get system status
    api_system_status_t system_status;
    hal_status_t result = system_get_status();
    
    if (result != HAL_STATUS_OK) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to get system status: %d", result);
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, error_msg);
    }
    
    // Create JSON response
    char json_buffer[512];
    hal_status_t json_result = api_create_system_status_json(&system_status, json_buffer, sizeof(json_buffer));
    
    if (json_result != HAL_STATUS_OK) {
        return api_create_error_response(response, API_MGR_RESPONSE_INTERNAL_SERVER_ERROR, "JSON serialization failed");
    }
    
    return api_create_success_response(response, json_buffer);
}
```

#### **5.2 Logging Implementation**
```c
// Trong api_manager.c - logging functions
static void api_log_request(const api_mgr_http_request_t *request, const api_mgr_http_response_t *response) {
    printf("[API] %s %s -> %d (%s)\n", 
           api_manager_get_http_method_name(request->method),
           request->path,
           response->status_code,
           request->client_ip);
}

static void api_log_error(const char *context, hal_status_t error_code) {
    printf("[API-ERROR] %s failed with code: %d\n", context, error_code);
}
```

---

## 🔧 **Yêu cầu kỹ thuật**

### **1. Performance Requirements**
- **Response time:** < 100ms cho GET requests
- **WebSocket latency:** < 50ms cho real-time data
- **Concurrent connections:** Hỗ trợ ít nhất 10 clients
- **Memory usage:** < 2MB cho API manager

### **2. Reliability Requirements**
- **Uptime:** 99.9% availability
- **Error recovery:** Auto-recovery từ communication failures
- **Timeout handling:** Graceful timeout cho tất cả operations
- **Data consistency:** Consistent data giữa API và core systems

### **3. Security Requirements**
- **Input validation:** Validate tất cả input parameters
- **Buffer overflow protection:** Safe string handling
- **Authentication:** Session-based authentication (future)
- **Authorization:** Role-based access control (future)

---

## 📋 **Implementation Checklist**

### **Phase 1: Core Integration (Week 1)**
- [ ] Implement `system_get_status()` function
- [ ] Implement `safety_get_status()` function  
- [ ] Implement `module_get_list()` function
- [ ] Replace mock data trong tất cả endpoints
- [ ] Test core system integration

### **Phase 2: HTTP Server (Week 2)**
- [ ] Configure HTTP server với port 8080
- [ ] Register tất cả endpoints
- [ ] Implement CORS support
- [ ] Add request/response logging
- [ ] Test HTTP endpoints

### **Phase 3: WebSocket Server (Week 3)**
- [ ] Configure WebSocket server với port 8081
- [ ] Implement real-time data broadcasting
- [ ] Add message handling
- [ ] Implement connection management
- [ ] Test WebSocket communication

### **Phase 4: Module Endpoints (Week 4)**
- [ ] Implement power module endpoints
- [ ] Implement safety module endpoints
- [ ] Implement motor module endpoints
- [ ] Implement dock module endpoints
- [ ] Test module-specific functionality

### **Phase 5: Error Handling & Testing (Week 5)**
- [ ] Implement comprehensive error handling
- [ ] Add logging system
- [ ] Performance testing
- [ ] Integration testing với BE/FE
- [ ] Documentation update

---

## 🧪 **Testing Requirements**

### **1. Unit Testing**
```c
// Test system_get_status function
void test_system_get_status(void) {
    api_system_status_t status;
    hal_status_t result = system_get_status();
    
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, result);
    TEST_ASSERT_EQUAL_STRING("OHT-50", status.system_name);
    TEST_ASSERT_EQUAL_STRING("1.0.0", status.version);
}
```

### **2. Integration Testing**
```bash
# Test HTTP endpoints
curl -X GET http://localhost:8080/api/v1/system/status
curl -X GET http://localhost:8080/api/v1/safety/status
curl -X GET http://localhost:8080/api/v1/modules

# Test WebSocket connection
wscat -c ws://localhost:8081
```

### **3. Performance Testing**
```bash
# Load testing với multiple clients
ab -n 1000 -c 10 http://localhost:8080/api/v1/system/status
```

---

## 📚 **Documentation Requirements**

### **1. API Documentation**
- Complete API reference với examples
- Request/response schemas
- Error codes và messages
- Authentication/authorization guide

### **2. Integration Guide**
- Backend integration guide
- Frontend integration guide
- WebSocket protocol specification
- Error handling guidelines

### **3. Deployment Guide**
- Server configuration
- Environment setup
- Monitoring và logging
- Troubleshooting guide

---

## 🚨 **Risks & Mitigation**

### **1. Performance Risks**
- **Risk:** High latency với multiple clients
- **Mitigation:** Implement connection pooling và caching

### **2. Security Risks**
- **Risk:** Unauthorized access
- **Mitigation:** Implement authentication/authorization

### **3. Reliability Risks**
- **Risk:** System crashes due to memory issues
- **Mitigation:** Implement memory management và error recovery

---

## ✅ **Acceptance Criteria**

### **1. Functional Criteria**
- [ ] Tất cả API endpoints return real data thay vì mock data
- [ ] WebSocket server broadcast real-time updates
- [ ] HTTP server handle concurrent requests
- [ ] Error handling work correctly

### **2. Performance Criteria**
- [ ] Response time < 100ms cho GET requests
- [ ] WebSocket latency < 50ms
- [ ] Memory usage < 2MB
- [ ] Support 10+ concurrent connections

### **3. Quality Criteria**
- [ ] Code coverage > 90%
- [ ] No memory leaks
- [ ] Comprehensive error handling
- [ ] Complete documentation

---

**Changelog v1.0:**
- ✅ Created API integration specification
- ✅ Added core system integration requirements
- ✅ Added HTTP/WebSocket implementation details
- ✅ Added module-specific endpoints
- ✅ Added testing và documentation requirements
- ✅ Added implementation checklist

**🚨 Lưu ý:** Team FW phải implement theo thứ tự Phase 1-5 để đảm bảo stability và functionality.
