# API ENDPOINTS SPECIFICATION - OHT-50 MASTER MODULE

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-27  
**Team:** FW  
**Base URL:** `http://localhost:8080/api/v1`

---

## 📋 TỔNG QUAN

Firmware OHT-50 Master Module cung cấp RESTful API với các endpoints chính sau:

### 🔗 Base Configuration
- **Base URL:** `/api/v1`
- **Content-Type:** `application/json`
- **Authentication:** Token-based (optional)
- **Rate Limiting:** 100 requests/minute
- **Timeout:** 30 seconds

---

## 🔧 API ENDPOINTS

### 1. SYSTEM ENDPOINTS

#### 1.1 GET `/system/status`
**Mô tả:** Lấy trạng thái tổng quan của hệ thống

**Response:**
```json
{
  "system_name": "OHT-50 Master Module",
  "version": "1.0.0",
  "status": "IDLE",
  "uptime_ms": 1234567890,
  "active_modules": 3,
  "estop_active": false,
  "safety_ok": true
}
```

**Status Codes:**
- `200 OK` - Thành công
- `500 Internal Server Error` - Lỗi server

#### 1.2 GET `/system/health`
**Mô tả:** Kiểm tra sức khỏe hệ thống

**Response:**
```json
{
  "status": "HEALTHY",
  "timestamp": 1640995200000,
  "response_time_ms": 15,
  "details": "All systems operational"
}
```

**Status Codes:**
- `200 OK` - Hệ thống khỏe mạnh
- `503 Service Unavailable` - Hệ thống có vấn đề

### 2. MODULE ENDPOINTS

#### 2.1 GET `/modules`
**Mô tả:** Lấy danh sách tất cả modules

**Response:**
```json
{
  "modules": [
    {
      "module_id": 1,
      "module_type": "MOTOR",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "1.0.0"
    },
    {
      "module_id": 2,
      "module_type": "POWER",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "1.0.0"
    }
  ],
  "module_count": 2
}
```

#### 2.2 GET `/modules/{id}`
**Mô tả:** Lấy thông tin chi tiết của module

**Parameters:**
- `id` (path): Module ID (1-16)

**Response:**
```json
{
  "module_id": 1,
  "module_type": "MOTOR",
  "status": "ONLINE",
  "online": true,
  "last_seen": 1640995200000,
  "version": "1.0.0",
  "health": 95,
  "temperature": 45.2,
  "voltage": 24.1,
  "current": 2.3
}
```

#### 2.3 POST `/modules/{id}/command`
**Mô tả:** Gửi lệnh đến module

**Parameters:**
- `id` (path): Module ID (1-16)

**Request Body:**
```json
{
  "command": "MOVE",
  "parameters": "{\"speed\": 100, \"direction\": \"FORWARD\"}",
  "timeout_ms": 5000
}
```

**Response:**
```json
{
  "status": "SUCCESS",
  "message": "Command executed successfully",
  "execution_time_ms": 150
}
```

### 3. SAFETY ENDPOINTS

#### 3.1 GET `/safety/status`
**Mô tả:** Lấy trạng thái an toàn hệ thống

**Response:**
```json
{
  "estop_active": false,
  "safety_ok": true,
  "safety_level": 0,
  "safety_message": "All safety systems operational",
  "last_safety_check": 1640995200000
}
```

#### 3.2 POST `/safety/estop`
**Mô tả:** Kích hoạt E-Stop

**Request Body:**
```json
{
  "estop_reason": "Emergency stop triggered by user",
  "timestamp": 1640995200000,
  "acknowledged": false
}
```

**Response:**
```json
{
  "status": "ESTOP_ACTIVATED",
  "message": "Emergency stop activated successfully",
  "timestamp": 1640995200000
}
```

### 4. CONFIGURATION ENDPOINTS

#### 4.1 GET `/config`
**Mô tả:** Lấy cấu hình hệ thống

**Response:**
```json
{
  "config_data": {
    "system": {
      "name": "OHT-50 Master Module",
      "version": "1.0.0",
      "gpio_config": {
        "led_power": 54,
        "led_system": 35,
        "led_comm": 28,
        "led_network": 29,
        "led_error": 58,
        "estop": 59,
        "relay1": 131,
        "relay2": 132
      }
    },
    "communication": {
      "rs485": {
        "baud_rate": 115200,
        "timeout_ms": 1000,
        "retry_count": 3
      }
    },
    "safety": {
      "response_time_ms": 100,
      "debounce_time_ms": 50
    }
  },
  "config_version": 2,
  "last_updated": 1640995200000
}
```

#### 4.2 PUT `/config`
**Mô tả:** Cập nhật cấu hình hệ thống

**Request Body:**
```json
{
  "config_data": {
    "communication": {
      "rs485": {
        "baud_rate": 115200,
        "timeout_ms": 1500
      }
    }
  },
  "config_version": 3
}
```

**Response:**
```json
{
  "status": "SUCCESS",
  "message": "Configuration updated successfully",
  "config_version": 3,
  "timestamp": 1640995200000
}
```

### 5. DIAGNOSTICS ENDPOINTS

#### 5.1 GET `/diagnostics`
**Mô tả:** Lấy thông tin chẩn đoán hệ thống

**Response:**
```json
{
  "total_requests": 1234,
  "successful_requests": 1200,
  "failed_requests": 34,
  "uptime_ms": 1234567890,
  "system_info": {
    "cpu_usage": 15.2,
    "memory_usage": 45.8,
    "temperature": 42.1,
    "disk_usage": 23.4
  },
  "error_log": [
    {
      "timestamp": 1640995200000,
      "level": "WARNING",
      "message": "Module 3 timeout"
    }
  ]
}
```

---

## 🔐 AUTHENTICATION

### Token-based Authentication
```http
Authorization: Bearer <token>
```

### Security Headers
```http
X-API-Key: <api_key>
X-Request-ID: <request_id>
```

---

## 📊 ERROR HANDLING

### Standard Error Response
```json
{
  "error": {
    "code": "E001",
    "message": "Invalid module ID",
    "details": "Module ID must be between 1 and 16",
    "timestamp": 1640995200000
  }
}
```

### Error Codes
- `E001` - Invalid parameters
- `E002` - Module not found
- `E003` - Module offline
- `E004` - Command timeout
- `E005` - Safety violation
- `E006` - Configuration error
- `E007` - System error

---

## 🔄 WEBSOCKET API

### WebSocket Endpoint
```
ws://localhost:8081/ws
```

### Message Types

#### 1. System Status Updates
```json
{
  "type": "system_status",
  "data": {
    "status": "IDLE",
    "uptime_ms": 1234567890,
    "active_modules": 3
  }
}
```

#### 2. Module Status Updates
```json
{
  "type": "module_status",
  "data": {
    "module_id": 1,
    "status": "ONLINE",
    "health": 95
  }
}
```

#### 3. Safety Events
```json
{
  "type": "safety_event",
  "data": {
    "event": "ESTOP_TRIGGERED",
    "timestamp": 1640995200000,
    "reason": "Emergency stop button pressed"
  }
}
```

---

## 📈 PERFORMANCE METRICS

### Response Time Targets
- **System Status:** < 50ms
- **Module List:** < 100ms
- **Module Command:** < 200ms
- **Configuration:** < 150ms
- **Diagnostics:** < 300ms

### Throughput Limits
- **HTTP API:** 100 requests/minute
- **WebSocket:** 1000 messages/minute
- **File Upload:** 10MB max

---

## 🧪 TESTING

### Test Endpoints
```bash
# System status
curl -X GET http://localhost:8080/api/v1/system/status

# Module list
curl -X GET http://localhost:8080/api/v1/modules

# Safety status
curl -X GET http://localhost:8080/api/v1/safety/status

# Configuration
curl -X GET http://localhost:8080/api/v1/config
```

### Load Testing
```bash
# Test with Apache Bench
ab -n 1000 -c 10 http://localhost:8080/api/v1/system/status
```

---

## 📚 EXAMPLES

### Complete API Workflow
```bash
# 1. Check system status
curl -X GET http://localhost:8080/api/v1/system/status

# 2. List modules
curl -X GET http://localhost:8080/api/v1/modules

# 3. Send command to module
curl -X POST http://localhost:8080/api/v1/modules/1/command \
  -H "Content-Type: application/json" \
  -d '{"command": "MOVE", "parameters": "{\"speed\": 100}"}'

# 4. Check safety status
curl -X GET http://localhost:8080/api/v1/safety/status
```

---

## 🔗 LIÊN KẾT

- **API Documentation:** `docs/05-IMPLEMENTATION/FIRMWARE/`
- **Testing Guide:** `docs/06-TESTING/`
- **Deployment Guide:** `docs/07-DEPLOYMENT/`

---

**Changelog v1.0.0:**
- ✅ Tạo tài liệu API endpoints chi tiết
- ✅ Định nghĩa request/response formats
- ✅ Thêm error handling
- ✅ WebSocket API specification
- ✅ Performance metrics
- ✅ Testing examples
