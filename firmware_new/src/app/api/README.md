# API Layer - OHT-50 Master Module

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** WebSocket và HTTP API integration cho OHT-50 Master Module

---

## 📋 **API Overview**

### **API Components:**
- **API Manager:** Core API management và coordination
- **HTTP Server:** REST API endpoints cho system control
- **WebSocket Server:** Real-time communication cho telemetry
- **API Endpoints:** System integration với real data thay vì mock data

### **API Features:**
- **Real-time Communication:** WebSocket cho telemetry streaming
- **REST API:** HTTP endpoints cho system control và monitoring
- **System Integration:** Real data từ system controller, safety manager, module manager
- **CORS Support:** Cross-origin resource sharing cho web frontend
- **Authentication Ready:** Framework cho authentication (có thể enable sau)

---

## 🚀 **Quick Start**

### **1. Initialize API System:**
```c
#include "api_manager.h"
#include "api_endpoints.h"

// Initialize API Manager
api_mgr_config_t api_config = {0};
api_manager_set_default_config(&api_config);
api_config.http_port = 8080;
api_config.websocket_port = 8081;
api_config.enable_cors = true;

hal_status_t result = api_manager_init(&api_config);
if (result != HAL_STATUS_OK) {
    // Handle error
}

// Initialize API Endpoints
result = api_endpoints_init();
if (result != HAL_STATUS_OK) {
    // Handle error
}

// Start services
result = api_manager_start();
if (result != HAL_STATUS_OK) {
    // Handle error
}
```

### **2. Test API Endpoints:**
```bash
# Test system status
curl -X GET http://localhost:8080/api/v1/system/status

# Test safety status
curl -X GET http://localhost:8080/api/v1/safety/status

# Test modules list
curl -X GET http://localhost:8080/api/v1/modules

# Test power status
curl -X GET http://localhost:8080/api/v1/modules/power/status

# Test motor status
curl -X GET http://localhost:8080/api/v1/modules/motor/status

# Test dock status
curl -X GET http://localhost:8080/api/v1/modules/dock/status

# Test safety sensors
curl -X GET http://localhost:8080/api/v1/safety/sensors
```

### **3. Test WebSocket Connection:**
```bash
# Using wscat (install with: npm install -g wscat)
wscat -c ws://localhost:8081

# Send test message
{"type": "test", "message": "Hello WebSocket!"}
```

---

## 📚 **API Endpoints**

### **System Endpoints:**

#### **GET /api/v1/system/status**
Get system status và information.

**Response:**
```json
{
  "system_name": "OHT-50",
  "version": "1.0.0",
  "status": "idle",
  "uptime_ms": 1234567,
  "active_modules": 4,
  "estop_active": false,
  "safety_ok": true
}
```

### **Safety Endpoints:**

#### **GET /api/v1/safety/status**
Get safety system status.

**Response:**
```json
{
  "estop_active": false,
  "safety_ok": true,
  "safety_level": 0,
  "last_safety_check": 1234567890
}
```

#### **GET /api/v1/safety/sensors**
Get safety sensor data.

**Response:**
```json
{
  "analog_sensors": [1024, 512, 768, 256],
  "digital_sensors": 15,
  "proximity_alert": false
}
```

### **Module Endpoints:**

#### **GET /api/v1/modules**
Get list of registered modules.

**Response:**
```json
{
  "module_count": 4,
  "modules": [
    {
      "module_id": 1,
      "module_type": "power",
      "online": true,
      "last_seen": 1234567890
    },
    {
      "module_id": 2,
      "module_type": "motor",
      "online": true,
      "last_seen": 1234567890
    }
  ]
}
```

#### **GET /api/v1/modules/power/status**
Get power module status.

**Response:**
```json
{
  "battery_voltage": 24.5,
  "battery_current": 2.3,
  "battery_soc": 87,
  "charge_status": 1
}
```

#### **GET /api/v1/modules/motor/status**
Get motor module status.

**Response:**
```json
{
  "position": 1500,
  "velocity": 100,
  "temperature": 45,
  "voltage": 24,
  "current": 5
}
```

#### **GET /api/v1/modules/dock/status**
Get dock module status.

**Response:**
```json
{
  "dock_status": 1,
  "dock_position": 0,
  "dock_ready": true
}
```

---

## 🔧 **WebSocket API**

### **Connection:**
```javascript
const ws = new WebSocket('ws://localhost:8081');

ws.onopen = function() {
    console.log('WebSocket connected');
};

ws.onmessage = function(event) {
    const data = JSON.parse(event.data);
    console.log('Received:', data);
};

ws.onclose = function() {
    console.log('WebSocket disconnected');
};
```

### **Message Format:**
```json
{
  "type": "telemetry",
  "timestamp": 1234567890,
  "data": {
    "system_status": "idle",
    "battery_level": 87,
    "position": {"x": 150.5, "y": 200.3},
    "safety_ok": true
  }
}
```

---

## 🛠️ **Custom Endpoints**

### **Adding Custom Endpoint:**
```c
// Custom endpoint handler
hal_status_t custom_handler(const api_mgr_http_request_t *request, api_mgr_http_response_t *response) {
    char custom_json[256];
    snprintf(custom_json, sizeof(custom_json),
        "{\n"
        "  \"message\": \"Hello from custom endpoint!\",\n"
        "  \"timestamp\": %lu\n"
        "}",
        hal_get_timestamp_ms()
    );
    
    return api_manager_create_success_response(response, custom_json);
}

// Register custom endpoint
api_mgr_endpoint_t custom_endpoint = {
    .method = API_MGR_HTTP_GET,
    .path = "/api/v1/custom/hello",
    .handler = custom_handler,
    .requires_authentication = false,
    .description = "Custom hello endpoint"
};

api_manager_register_endpoint(&custom_endpoint);
```

### **WebSocket Message Handler:**
```c
hal_status_t ws_message_handler(const char *message, size_t message_length, void *client) {
    // Process WebSocket message
    hal_log_message(HAL_LOG_LEVEL_INFO, "Received: %.*s", (int)message_length, message);
    
    // Send response back
    char response[256];
    snprintf(response, sizeof(response), "Echo: %.*s", (int)message_length, message);
    
    // Note: In real implementation, send back to specific client
    return HAL_STATUS_OK;
}

// Register WebSocket message handler
api_manager_register_websocket_handler(ws_message_handler);
```

---

## 📊 **Performance Requirements**

### **Response Time Targets:**
- **HTTP GET requests:** < 100ms
- **WebSocket messages:** < 50ms
- **Concurrent connections:** 10+ clients
- **Memory usage:** < 2MB cho API layer

### **Load Testing:**
```bash
# HTTP load testing
ab -n 1000 -c 10 http://localhost:8080/api/v1/system/status

# WebSocket load testing
# Use tools like Artillery.io or custom scripts
```

---

## 🔒 **Security**

### **Current Security Features:**
- **CORS Support:** Configurable cross-origin access
- **Input Validation:** Request validation và sanitization
- **Error Handling:** Comprehensive error handling
- **Rate Limiting:** Ready for implementation

### **Future Security Features:**
- **Authentication:** JWT token authentication
- **Authorization:** Role-based access control
- **HTTPS/WSS:** SSL/TLS encryption
- **API Keys:** API key authentication

---

## 🧪 **Testing**

### **Unit Testing:**
```bash
# Run API tests
cd /home/orangepi/Desktop/OHT_V2/firmware_new
make test-api
```

### **Integration Testing:**
```bash
# Test API endpoints
./scripts/test_api_endpoints.sh

# Test WebSocket connection
./scripts/test_websocket.sh
```

### **Manual Testing:**
```bash
# Test system status
curl -X GET http://localhost:8080/api/v1/system/status

# Test WebSocket
wscat -c ws://localhost:8081
```

---

## 📈 **Monitoring**

### **API Statistics:**
```c
// Get API Manager statistics
api_mgr_statistics_t stats;
api_manager_get_statistics(&stats);

printf("Total requests: %lu\n", stats.total_requests);
printf("Successful requests: %lu\n", stats.successful_requests);
printf("Failed requests: %lu\n", stats.failed_requests);
printf("WebSocket connections: %lu\n", stats.websocket_connections);
```

### **Health Check:**
```bash
# Check API health
curl -X GET http://localhost:8080/api/v1/system/status

# Check WebSocket health
wscat -c ws://localhost:8081
```

---

## 🚨 **Troubleshooting**

### **Common Issues:**

#### **1. Port Already in Use:**
```bash
# Check if port is in use
netstat -tulpn | grep :8080
netstat -tulpn | grep :8081

# Kill process using port
sudo kill -9 <PID>
```

#### **2. WebSocket Connection Failed:**
```bash
# Check WebSocket server status
curl -X GET http://localhost:8080/api/v1/system/status

# Check firewall settings
sudo ufw status
```

#### **3. API Endpoints Not Working:**
```bash
# Check API endpoints registration
curl -X GET http://localhost:8080/api/v1/modules

# Check system logs
tail -f /var/log/oht50.log
```

### **Debug Mode:**
```c
// Enable debug logging
hal_log_set_level(HAL_LOG_LEVEL_DEBUG);

// Check API Manager status
api_mgr_status_t status;
api_manager_get_status(&status);
printf("HTTP Server: %s\n", status.http_server_running ? "running" : "stopped");
printf("WebSocket Server: %s\n", status.websocket_server_running ? "running" : "stopped");
```

---

## 📚 **Related Documents**

### **API Documentation:**
- [API Manager Documentation](api_manager.h) - Core API management
- [HTTP Server Documentation](http_server.h) - HTTP server implementation
- [WebSocket Server Documentation](websocket_server.h) - WebSocket server implementation
- [API Endpoints Documentation](api_endpoints.h) - System integration endpoints

### **Integration Examples:**
- [API Integration Example](api_integration_example.c) - Complete integration example
- [Main Application Integration](../main.c) - Integration trong main application

### **System Integration:**
- [System Controller](../core/system_controller.h) - System control integration
- [Safety Manager](../managers/safety_manager.h) - Safety system integration
- [Module Manager](../managers/module_manager.h) - Module management integration

---

## 🔄 **Changelog**

### **v1.0.0 (2025-01-28):**
- ✅ **Phase 1 Complete:** API Manager Core Structure
- ✅ **Phase 1 Complete:** HTTP Server Foundation
- ✅ **Phase 1 Complete:** WebSocket Server Foundation
- ✅ **Phase 2 Complete:** System Controller Integration
- ✅ **Real Data Integration:** Thay thế mock data với real system data
- ✅ **API Endpoints:** Complete system, safety, và module endpoints
- ✅ **WebSocket Support:** Real-time telemetry broadcasting
- ✅ **CORS Support:** Cross-origin resource sharing
- ✅ **Error Handling:** Comprehensive error handling
- ✅ **Documentation:** Complete API documentation

### **Next Phase (v1.1.0):**
- 🔄 **Phase 3:** Advanced module endpoints
- 🔄 **Phase 4:** Configuration management
- 🔄 **Phase 5:** Security và authentication
- 🔄 **Performance Optimization:** Load testing và optimization
- 🔄 **Frontend Integration:** Complete frontend integration

---

**🚨 Lưu ý:** API system đã được implement với real system integration thay vì mock data. Tất cả endpoints return real data từ system controller, safety manager, và module manager.

**🎯 Status:** Phase 1 và Phase 2 hoàn thành. Ready cho Phase 3 implementation.
