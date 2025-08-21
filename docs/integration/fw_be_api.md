# 🔗 FW-BE Integration API Documentation

## 📋 **Overview**
**Version:** 1.0.0  
**Date:** 2025-01-27  
**Team:** FW + Backend  
**Task:** FW-INT-03 (API Endpoints Implementation)  

---

## 🌐 **Base URL**
```
http://localhost:8080/api/v1
```

## 🔐 **Authentication**
- **Type:** Bearer Token
- **Header:** `Authorization: Bearer <token>`
- **Required:** For protected endpoints (marked with 🔒)

---

## 📡 **API Endpoints**

### **System Endpoints**

#### **GET /api/v1/system/status**
Get system status information.

**Response:**
```json
{
  "system_name": "OHT-50 Master Module",
  "version": "1.0.0",
  "status": "running",
  "uptime_ms": 1234567890,
  "active_modules": 3,
  "estop_active": false,
  "safety_ok": true
}
```

#### **GET /api/v1/system/health**
Get system health status.

**Response:**
```json
{
  "status": "healthy",
  "timestamp": 1234567890,
  "response_time_ms": 50,
  "details": "All systems operational"
}
```

### **Module Endpoints**

#### **GET /api/v1/modules**
Get list of all modules.

**Response:**
```json
{
  "modules": [
    {
      "module_id": 1,
      "module_type": "power",
      "status": "online",
      "online": true,
      "last_seen": 1234567890,
      "version": "1.0.0"
    },
    {
      "module_id": 2,
      "module_type": "motor",
      "status": "online",
      "online": true,
      "last_seen": 1234567890,
      "version": "1.0.0"
    },
    {
      "module_id": 3,
      "module_type": "dock",
      "status": "online",
      "online": true,
      "last_seen": 1234567890,
      "version": "1.0.0"
    }
  ],
  "module_count": 3
}
```

#### **GET /api/v1/modules/{id}**
Get specific module information.

**Parameters:**
- `id` (path): Module ID (1-255)

**Response:**
```json
{
  "module_id": 1,
  "module_type": "power",
  "status": "online",
  "online": true,
  "last_seen": 1234567890,
  "version": "1.0.0"
}
```

#### **POST /api/v1/modules/{id}/command** 🔒
Send command to specific module.

**Parameters:**
- `id` (path): Module ID (1-255)

**Request Body:**
```json
{
  "command": "start",
  "parameters": "speed=50",
  "timeout_ms": 5000
}
```

**Response:**
```json
{
  "module_id": 1,
  "command": "start",
  "status": "executed",
  "timestamp": 1234567890
}
```

### **Safety Endpoints**

#### **GET /api/v1/safety/status**
Get safety system status.

**Response:**
```json
{
  "estop_active": false,
  "safety_ok": true,
  "safety_level": 1,
  "safety_message": "All safety systems operational",
  "last_safety_check": 1234567890
}
```

#### **POST /api/v1/safety/estop** 🔒
Trigger emergency stop.

**Request Body:**
```json
{
  "estop_reason": "Manual emergency stop",
  "timestamp": 1234567890,
  "acknowledged": false
}
```

**Response:**
```json
{
  "estop_reason": "Manual emergency stop",
  "timestamp": 1234567890,
  "acknowledged": true,
  "status": "executed"
}
```

### **Configuration Endpoints**

#### **GET /api/v1/config**
Get system configuration.

**Response:**
```json
{
  "config_data": {
    "system": {
      "name": "OHT-50",
      "version": "1.0.0"
    },
    "network": {
      "port": 8080,
      "timeout": 30000
    }
  },
  "config_version": 1,
  "last_updated": 1234567890
}
```

#### **PUT /api/v1/config** 🔒
Update system configuration.

**Request Body:**
```json
{
  "system": {
    "name": "OHT-50",
    "version": "1.0.0"
  },
  "network": {
    "port": 8080,
    "timeout": 30000
  }
}
```

**Response:**
```json
{
  "status": "updated",
  "config_version": 2,
  "timestamp": 1234567890
}
```

### **Diagnostics Endpoints**

#### **GET /api/v1/diagnostics**
Get system diagnostics information.

**Response:**
```json
{
  "total_requests": 100,
  "successful_requests": 95,
  "failed_requests": 5,
  "uptime_ms": 1234567890,
  "system_info": "OHT-50 Master Module v1.0.0 running on Orange Pi 5B",
  "error_log": "No errors in last 24 hours"
}
```

---

## 📊 **HTTP Status Codes**

| Code | Description |
|------|-------------|
| 200 | OK - Request successful |
| 201 | Created - Resource created successfully |
| 400 | Bad Request - Invalid request format |
| 401 | Unauthorized - Authentication required |
| 403 | Forbidden - Access denied |
| 404 | Not Found - Resource not found |
| 405 | Method Not Allowed - HTTP method not supported |
| 500 | Internal Server Error - Server error |
| 503 | Service Unavailable - Service temporarily unavailable |

---

## 🔧 **Error Response Format**

All error responses follow this format:

```json
{
  "error": "Error Type",
  "status": 400,
  "message": "Detailed error message"
}
```

---

## 📝 **Usage Examples**

### **Python Example**
```python
import requests

# Base URL
base_url = "http://localhost:8080/api/v1"

# Get system status
response = requests.get(f"{base_url}/system/status")
if response.status_code == 200:
    status = response.json()
    print(f"System: {status['system_name']}")
    print(f"Status: {status['status']}")

# Send command to module (with authentication)
headers = {"Authorization": "Bearer your_token_here"}
data = {"command": "start", "parameters": "speed=50"}
response = requests.post(f"{base_url}/modules/1/command", 
                        json=data, headers=headers)
if response.status_code == 200:
    result = response.json()
    print(f"Command executed: {result['status']}")
```

### **cURL Examples**
```bash
# Get system status
curl -X GET http://localhost:8080/api/v1/system/status

# Get modules list
curl -X GET http://localhost:8080/api/v1/modules

# Send command to module
curl -X POST http://localhost:8080/api/v1/modules/1/command \
  -H "Authorization: Bearer your_token_here" \
  -H "Content-Type: application/json" \
  -d '{"command": "start", "parameters": "speed=50"}'

# Trigger emergency stop
curl -X POST http://localhost:8080/api/v1/safety/estop \
  -H "Authorization: Bearer your_token_here" \
  -H "Content-Type: application/json" \
  -d '{"estop_reason": "Manual emergency stop"}'
```

---

## 🔄 **Real-time Updates**

For real-time updates, use WebSocket connection (planned for future implementation):

```javascript
// WebSocket connection (future implementation)
const ws = new WebSocket('ws://localhost:8080/ws');

ws.onmessage = function(event) {
    const data = JSON.parse(event.data);
    console.log('Real-time update:', data);
};
```

---

## 📋 **Rate Limiting**

- **Requests per minute:** 100
- **Burst limit:** 10 requests per second
- **Rate limit headers:**
  - `X-RateLimit-Limit`
  - `X-RateLimit-Remaining`
  - `X-RateLimit-Reset`

---

## 🔒 **Security Considerations**

1. **Authentication:** Use Bearer tokens for protected endpoints
2. **HTTPS:** Use HTTPS in production environments
3. **Input Validation:** Validate all input parameters
4. **Rate Limiting:** Respect rate limits to avoid being blocked
5. **Error Handling:** Handle errors gracefully

---

## 📚 **Related Documentation**

- [FW-BE Integration Tasks](../FIRMWARE/03_TASKS/fw_be_integration_tasks.md)
- [HTTP Server Implementation](../FIRMWARE/01_HAL/http_server.md)
- [API Endpoints Implementation](../FIRMWARE/02_APP/api_endpoints.md)

---

**Status:** ✅ COMPLETED  
**Next Review:** After BE client implementation
