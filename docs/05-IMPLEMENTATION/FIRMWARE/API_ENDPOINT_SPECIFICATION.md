# API ENDPOINT SPECIFICATION - OHT-50 FIRMWARE

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Cập nhật lần cuối:** 2025-01-28  
**Trách nhiệm:** FW Team Lead

---

## 📋 **TỔNG QUAN**

Tài liệu này định nghĩa chi tiết các API endpoints cần được implement và debug trong firmware OHT-50.

---

## 🔧 **API OVERVIEW**

### **Base URL:** `http://localhost:8080/api/v1`

### **Authentication:** None (internal network only)

### **Response Format:** JSON

### **Error Handling:** Standard HTTP status codes

---

## 📊 **SYSTEM ENDPOINTS**

### **1. System Status**
```http
GET /api/v1/system/status
```

**Mô tả:** Lấy trạng thái tổng quan của hệ thống

**Response (200 OK):**
```json
{
  "status": "idle|moving|docking|fault|estop",
  "timestamp": "2025-01-28T10:30:00Z",
  "uptime": 3600,
  "version": "1.0.0",
  "modules": {
    "motor": "online|offline|error",
    "sensor": "online|offline|error", 
    "safety": "online|offline|error"
  },
  "safety": {
    "estop": false,
    "zones": {
      "zone1": "safe|violation",
      "zone2": "safe|violation"
    }
  }
}
```

**Error Response (500 Internal Server Error):**
```json
{
  "error": "Internal server error",
  "code": 500,
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **2. System Health**
```http
GET /api/v1/system/health
```

**Mô tả:** Kiểm tra sức khỏe hệ thống

**Response (200 OK):**
```json
{
  "status": "healthy|degraded|unhealthy",
  "timestamp": "2025-01-28T10:30:00Z",
  "checks": {
    "cpu": {
      "status": "ok|warning|error",
      "usage": 45.2
    },
    "memory": {
      "status": "ok|warning|error", 
      "usage": 67.8,
      "total": 2048,
      "used": 1389
    },
    "network": {
      "status": "ok|warning|error",
      "latency": 12
    },
    "storage": {
      "status": "ok|warning|error",
      "usage": 23.4
    }
  }
}
```

**Error Response (500 Internal Server Error):**
```json
{
  "error": "Health check failed",
  "code": 500,
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

## 🔌 **MODULE ENDPOINTS**

### **3. Modules List**
```http
GET /api/v1/modules
```

**Mô tả:** Lấy danh sách tất cả modules

**Response (200 OK):**
```json
{
  "modules": [
    {
      "id": "motor_001",
      "type": "motor",
      "status": "online|offline|error",
      "address": 1,
      "last_seen": "2025-01-28T10:30:00Z",
      "data": {
        "position": 1234.5,
        "velocity": 45.6,
        "current": 2.3
      }
    },
    {
      "id": "sensor_001", 
      "type": "sensor",
      "status": "online|offline|error",
      "address": 2,
      "last_seen": "2025-01-28T10:30:00Z",
      "data": {
        "distance": 150.0,
        "obstacle": false
      }
    },
    {
      "id": "safety_001",
      "type": "safety", 
      "status": "online|offline|error",
      "address": 3,
      "last_seen": "2025-01-28T10:30:00Z",
      "data": {
        "estop": false,
        "zones": {
          "zone1": "safe",
          "zone2": "safe"
        }
      }
    }
  ],
  "total": 3,
  "online": 3,
  "offline": 0,
  "errors": 0
}
```

**Error Response (500 Internal Server Error):**
```json
{
  "error": "Failed to retrieve modules",
  "code": 500,
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **4. Module Details**
```http
GET /api/v1/modules/{module_id}
```

**Mô tả:** Lấy thông tin chi tiết của module cụ thể

**Parameters:**
- `module_id` (string): ID của module

**Response (200 OK):**
```json
{
  "id": "motor_001",
  "type": "motor",
  "status": "online",
  "address": 1,
  "last_seen": "2025-01-28T10:30:00Z",
  "config": {
    "max_velocity": 100.0,
    "max_acceleration": 50.0,
    "position_limits": {
      "min": 0.0,
      "max": 5000.0
    }
  },
  "data": {
    "position": 1234.5,
    "velocity": 45.6,
    "current": 2.3,
    "temperature": 45.2,
    "voltage": 24.1
  },
  "errors": []
}
```

**Error Response (404 Not Found):**
```json
{
  "error": "Module not found",
  "code": 404,
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

## 📡 **TELEMETRY ENDPOINTS**

### **5. Telemetry Data**
```http
GET /api/v1/telemetry
```

**Mô tả:** Lấy dữ liệu telemetry thời gian thực

**Query Parameters:**
- `module_id` (optional): Filter theo module ID
- `type` (optional): Filter theo loại dữ liệu
- `limit` (optional): Số lượng records (default: 100)

**Response (200 OK):**
```json
{
  "telemetry": [
    {
      "timestamp": "2025-01-28T10:30:00Z",
      "module_id": "motor_001",
      "type": "position",
      "value": 1234.5,
      "unit": "mm"
    },
    {
      "timestamp": "2025-01-28T10:30:00Z", 
      "module_id": "motor_001",
      "type": "velocity",
      "value": 45.6,
      "unit": "mm/s"
    },
    {
      "timestamp": "2025-01-28T10:30:00Z",
      "module_id": "sensor_001",
      "type": "distance",
      "value": 150.0,
      "unit": "mm"
    }
  ],
  "total": 3,
  "from": "2025-01-28T10:29:00Z",
  "to": "2025-01-28T10:30:00Z"
}
```

**Error Response (500 Internal Server Error):**
```json
{
  "error": "Failed to retrieve telemetry",
  "code": 500,
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

## 🎮 **CONTROL ENDPOINTS**

### **6. System Control**
```http
POST /api/v1/system/control
```

**Mô tả:** Gửi lệnh điều khiển hệ thống

**Request Body:**
```json
{
  "command": "start|stop|pause|resume|emergency_stop",
  "parameters": {
    "target_position": 2000.0,
    "velocity": 50.0,
    "acceleration": 25.0
  }
}
```

**Response (200 OK):**
```json
{
  "status": "success",
  "command": "start",
  "timestamp": "2025-01-28T10:30:00Z",
  "message": "Command executed successfully"
}
```

**Error Response (400 Bad Request):**
```json
{
  "error": "Invalid command",
  "code": 400,
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **7. Module Control**
```http
POST /api/v1/modules/{module_id}/control
```

**Mô tả:** Gửi lệnh điều khiển module cụ thể

**Request Body:**
```json
{
  "command": "move|stop|reset|configure",
  "parameters": {
    "position": 2000.0,
    "velocity": 50.0
  }
}
```

**Response (200 OK):**
```json
{
  "status": "success",
  "module_id": "motor_001",
  "command": "move",
  "timestamp": "2025-01-28T10:30:00Z",
  "message": "Command sent to module"
}
```

---

## ⚙️ **CONFIGURATION ENDPOINTS**

### **8. System Configuration**
```http
GET /api/v1/system/config
```

**Mô tả:** Lấy cấu hình hệ thống

**Response (200 OK):**
```json
{
  "system": {
    "name": "OHT-50-Master",
    "version": "1.0.0",
    "serial_number": "OHT50-001",
    "network": {
      "ip": "192.168.1.100",
      "port": 8080,
      "protocol": "http"
    }
  },
  "modules": {
    "polling_interval": {
      "motor": 100,
      "sensor": 200,
      "safety": 50
    }
  },
  "safety": {
    "estop_timeout": 100,
    "zone_violation_timeout": 500,
    "emergency_stop_delay": 50
  }
}
```

### **9. Update Configuration**
```http
PUT /api/v1/system/config
```

**Mô tả:** Cập nhật cấu hình hệ thống

**Request Body:**
```json
{
  "modules": {
    "polling_interval": {
      "motor": 150,
      "sensor": 250,
      "safety": 75
    }
  }
}
```

**Response (200 OK):**
```json
{
  "status": "success",
  "message": "Configuration updated",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

## 📝 **LOGGING ENDPOINTS**

### **10. System Logs**
```http
GET /api/v1/system/logs
```

**Mô tả:** Lấy logs hệ thống

**Query Parameters:**
- `level` (optional): Filter theo log level (debug|info|warning|error)
- `module` (optional): Filter theo module
- `from` (optional): Thời gian bắt đầu
- `to` (optional): Thời gian kết thúc
- `limit` (optional): Số lượng records (default: 100)

**Response (200 OK):**
```json
{
  "logs": [
    {
      "timestamp": "2025-01-28T10:30:00Z",
      "level": "info",
      "module": "system",
      "message": "System started successfully"
    },
    {
      "timestamp": "2025-01-28T10:29:55Z",
      "level": "warning",
      "module": "motor_001",
      "message": "High temperature detected"
    }
  ],
  "total": 2
}
```

---

## 🚨 **ERROR HANDLING**

### **Standard Error Response Format:**
```json
{
  "error": "Error description",
  "code": 400,
  "timestamp": "2025-01-28T10:30:00Z",
  "details": {
    "field": "Additional error details"
  }
}
```

### **HTTP Status Codes:**
- `200 OK`: Request thành công
- `400 Bad Request`: Request không hợp lệ
- `404 Not Found`: Resource không tồn tại
- `500 Internal Server Error`: Lỗi server

---

## ⚡ **PERFORMANCE REQUIREMENTS**

### **Response Time:**
- **Critical endpoints:** < 50ms
- **Standard endpoints:** < 100ms
- **Data endpoints:** < 200ms

### **Throughput:**
- **Concurrent requests:** 100 requests/second
- **Data rate:** 1000 telemetry points/second

### **Memory Usage:**
- **Per request:** < 1KB
- **Total buffer:** < 1MB

---

## 🧪 **TESTING REQUIREMENTS**

### **Unit Tests:**
- [ ] Test all endpoint handlers
- [ ] Test error scenarios
- [ ] Test response formats
- [ ] Test performance limits

### **Integration Tests:**
- [ ] Test with real modules
- [ ] Test concurrent access
- [ ] Test error recovery
- [ ] Test load handling

### **Test Commands:**
```bash
# Test system status
curl -v http://localhost:8080/api/v1/system/status

# Test system health
curl -v http://localhost:8080/api/v1/system/health

# Test modules list
curl -v http://localhost:8080/api/v1/modules

# Test telemetry
curl -v http://localhost:8080/api/v1/telemetry

# Test module details
curl -v http://localhost:8080/api/v1/modules/motor_001

# Test system control
curl -X POST http://localhost:8080/api/v1/system/control \
  -H "Content-Type: application/json" \
  -d '{"command": "start"}'

# Test module control
curl -X POST http://localhost:8080/api/v1/modules/motor_001/control \
  -H "Content-Type: application/json" \
  -d '{"command": "move", "parameters": {"position": 2000.0}}'
```

---

## 📊 **IMPLEMENTATION CHECKLIST**

### **Core Implementation:**
- [ ] HTTP server setup
- [ ] Route handlers implementation
- [ ] JSON response formatting
- [ ] Error handling middleware
- [ ] Request validation

### **Endpoint Implementation:**
- [ ] System status endpoint
- [ ] System health endpoint
- [ ] Modules list endpoint
- [ ] Module details endpoint
- [ ] Telemetry endpoint
- [ ] Control endpoints
- [ ] Configuration endpoints
- [ ] Logging endpoints

### **Quality Assurance:**
- [ ] Response time optimization
- [ ] Memory usage optimization
- [ ] Error handling validation
- [ ] Security validation
- [ ] Documentation completion

---

**Changelog v1.0:**
- ✅ Created comprehensive API specification
- ✅ Defined all required endpoints
- ✅ Added detailed request/response formats
- ✅ Included error handling guidelines
- ✅ Added performance requirements
- ✅ Included testing requirements
- ✅ Added implementation checklist

**Trách nhiệm:** FW Team Lead  
**Ngày cập nhật:** 2025-01-28  
**Trạng thái:** 🔴 ACTIVE - IN PROGRESS
