# API ENDPOINTS SPECIFICATION - OHT-50 MASTER MODULE

**Phiên bản:** 3.0.0  
**Ngày cập nhật:** 2025-01-28  
**Team:** FW  
**Base URL:** `http://localhost:8080/api/v1`

---

## 📋 TỔNG QUAN

Firmware OHT-50 Master Module cung cấp RESTful API với các endpoints chính sau, được cập nhật cho kiến trúc mới v2.0 với 5 module bắt buộc:

### 🔗 Base Configuration
- **Base URL:** `/api/v1`
- **Content-Type:** `application/json`
- **Authentication:** Token-based (optional)
- **Rate Limiting:** 100 requests/minute
- **Timeout:** 30 seconds
- **Architecture:** New v2.0 with 5 mandatory modules

---

## 🏗️ KIẾN TRÚC MỚI V2.0 - API MAPPING

### **5 Module Bắt Buộc:**
1. **Power Module (0x01)** - Quản lý nguồn và BMS
2. **Safety Module (0x02)** - Hệ thống an toàn SIL2
3. **Travel Motor Module (0x03)** - Điều khiển động cơ di chuyển
4. **Dock & Location Module (0x05)** - Định vị và docking
5. **Master Control Module (0x00)** - Điều khiển tổng thể

### **Module Tùy Chọn:**
- **Lifter Motor Module (0x04)** - Điều khiển động cơ nâng
- **Cargo Door Module (0x06)** - Điều khiển cửa hàng hóa
- **Safety Extended Module (0x07)** - An toàn mở rộng

### **Module Mở Rộng (Plug-and-Play):**
- **RFID Reader Module (0x08)** - Đọc RFID
- **Camera Module (0x09)** - Xử lý hình ảnh
- **Environmental Sensor Module (0x0A)** - Cảm biến môi trường
- **Custom Modules (0x0B-0x1F)** - Module tùy chỉnh

---

## 🔧 API ENDPOINTS - IMPLEMENTED

### 1. SYSTEM ENDPOINTS

#### 1.1 GET `/system/status`
**Mô tả:** Lấy trạng thái tổng quan của hệ thống

**Request:**
```bash
curl -X GET http://localhost:8080/api/v1/system/status
```

**Response:**
```json
{
  "system": {
    "state": "IDLE",
    "uptime": 1234567890,
    "version": "1.0.0"
  }
}
```

**Status Codes:**
- `200 OK` - Thành công
- `500 Internal Server Error` - Lỗi server

**Error Response:**
```json
{
  "error": "Failed to get system state",
  "code": 500,
  "timestamp": 1640995200000
}
```

#### 1.2 GET `/system/health`
**Mô tả:** Kiểm tra sức khỏe hệ thống

**Request:**
```bash
curl -X GET http://localhost:8080/api/v1/system/health
```

**Response:**
```json
{
  "health": {
    "status": "healthy",
    "timestamp": 1640995200000
  }
}
```

**Status Codes:**
- `200 OK` - Hệ thống khỏe mạnh
- `503 Service Unavailable` - Hệ thống có vấn đề

### 2. NETWORK ENDPOINTS

#### 2.1 GET `/network/status`
**Mô tả:** Lấy trạng thái mạng

**Request:**
```bash
curl -X GET http://localhost:8080/api/v1/network/status
```

**Response:**
```json
{
  "network": {
    "state": "unknown",
    "active_interface": "none",
    "failover_count": 0
  }
}
```

#### 2.2 GET `/network/config`
**Mô tả:** Lấy cấu hình mạng

**Request:**
```bash
curl -X GET http://localhost:8080/api/v1/network/config
```

**Response:**
```json
{
  "network_config": {
    "failover_mode": "disabled",
    "auto_failover": false
  }
}
```

#### 2.3 POST `/network/config`
**Mô tả:** Cập nhật cấu hình mạng

**Request:**
```bash
curl -X POST http://localhost:8080/api/v1/network/config \
  -H "Content-Type: application/json" \
  -d '{
    "failover_mode": "enabled",
    "auto_failover": true
  }'
```

**Response:**
```json
{
  "success": true,
  "message": "Network configuration updated"
}
```

### 3. COMMUNICATION ENDPOINTS

#### 3.1 GET `/communication/status`
**Mô tả:** Lấy trạng thái giao tiếp

**Request:**
```bash
curl -X GET http://localhost:8080/api/v1/communication/status
```

**Response:**
```json
{
  "communication": {
    "state": "active",
    "active_modules": 1,
    "total_modules": 1
  }
}
```

### 4. SAFETY ENDPOINTS

#### 4.1 GET `/safety/status`
**Mô tả:** Lấy trạng thái hệ thống an toàn

**Request:**
```bash
curl -X GET http://localhost:8080/api/v1/safety/status
```

**Response:**
```json
{
  "safety": {
    "state": "safe",
    "estop_active": false,
    "faults": 0
  }
}
```

### 5. CONFIGURATION ENDPOINTS

#### 5.1 GET `/config`
**Mô tả:** Lấy cấu hình hệ thống

**Request:**
```bash
curl -X GET http://localhost:8080/api/v1/config
```

**Response:**
```json
{
  "configuration": {
    "version": "1.0.0",
    "build_date": "2025-01-27",
    "features": [
      "network_redundancy",
      "security",
      "websocket"
    ]
  }
}
```

#### 5.2 POST `/config`
**Mô tả:** Cập nhật cấu hình hệ thống

**Request:**
```bash
curl -X POST http://localhost:8080/api/v1/config \
  -H "Content-Type: application/json" \
  -d '{
    "feature_enabled": true,
    "timeout": 30000
  }'
```

**Response:**
```json
{
  "success": true,
  "message": "Configuration updated successfully"
}
```

---

## 📊 ERROR CODES & TROUBLESHOOTING

### **HTTP Status Codes:**
- `200 OK` - Request thành công
- `400 Bad Request` - Request không hợp lệ
- `401 Unauthorized` - Chưa xác thực
- `403 Forbidden` - Không có quyền truy cập
- `404 Not Found` - Endpoint không tồn tại
- `405 Method Not Allowed` - Method không được hỗ trợ
- `500 Internal Server Error` - Lỗi server
- `503 Service Unavailable` - Service không khả dụng

### **Error Response Format:**
```json
{
  "error": "Error description",
  "code": 500,
  "timestamp": 1640995200000,
  "details": "Additional error details"
}
```

### **Common Error Scenarios:**

#### **1. System State Error (500)**
**Problem:** Không thể lấy trạng thái hệ thống
**Cause:** System state machine chưa được khởi tạo
**Solution:** 
```bash
# Kiểm tra system state machine
curl -X GET http://localhost:8080/api/v1/system/status
```

#### **2. Network Configuration Error (400)**
**Problem:** Cấu hình mạng không hợp lệ
**Cause:** JSON format sai hoặc thiếu required fields
**Solution:**
```bash
# Kiểm tra JSON format
curl -X POST http://localhost:8080/api/v1/network/config \
  -H "Content-Type: application/json" \
  -d '{"failover_mode": "enabled"}'
```

#### **3. Authentication Error (401)**
**Problem:** Chưa xác thực
**Cause:** Thiếu token hoặc token không hợp lệ
**Solution:**
```bash
# Thêm Authorization header
curl -X GET http://localhost:8080/api/v1/system/status \
  -H "Authorization: Bearer <your-token>"
```

---

## 🔧 INTEGRATION EXAMPLES

### **1. System Monitoring Script**
```bash
#!/bin/bash

# Monitor system health
while true; do
    echo "=== System Health Check ==="
    
    # Check system status
    STATUS=$(curl -s http://localhost:8080/api/v1/system/status)
    echo "System Status: $STATUS"
    
    # Check system health
    HEALTH=$(curl -s http://localhost:8080/api/v1/system/health)
    echo "System Health: $HEALTH"
    
    # Check safety status
    SAFETY=$(curl -s http://localhost:8080/api/v1/safety/status)
    echo "Safety Status: $SAFETY"
    
    sleep 30
done
```

### **2. Configuration Management**
```bash
#!/bin/bash

# Backup current configuration
echo "Backing up current configuration..."
curl -s http://localhost:8080/api/v1/config > config_backup.json

# Update configuration
echo "Updating configuration..."
curl -X POST http://localhost:8080/api/v1/config \
  -H "Content-Type: application/json" \
  -d @new_config.json

# Verify configuration
echo "Verifying configuration..."
curl -s http://localhost:8080/api/v1/config
```

### **3. Safety Monitoring**
```bash
#!/bin/bash

# Monitor safety status
while true; do
    SAFETY_STATUS=$(curl -s http://localhost:8080/api/v1/safety/status)
    
    # Check if E-Stop is active
    if echo "$SAFETY_STATUS" | grep -q '"estop_active":true'; then
        echo "WARNING: E-Stop is active!"
        # Send alert
        echo "Safety alert: E-Stop activated" | mail -s "OHT-50 Safety Alert" admin@company.com
    fi
    
    sleep 5
done
```

---

## 🚀 PERFORMANCE & OPTIMIZATION

### **Response Time Benchmarks:**
- **System Status:** < 50ms
- **Health Check:** < 30ms
- **Safety Status:** < 40ms
- **Configuration:** < 60ms

### **Optimization Tips:**
1. **Use Connection Pooling:** Reuse HTTP connections
2. **Implement Caching:** Cache frequently accessed data
3. **Batch Requests:** Combine multiple requests when possible
4. **Monitor Performance:** Track response times and errors

### **Monitoring Example:**
```bash
# Performance monitoring script
for endpoint in status health safety config; do
    start_time=$(date +%s%N)
    response=$(curl -s -w "%{http_code}" http://localhost:8080/api/v1/$endpoint)
    end_time=$(date +%s%N)
    
    duration=$(( (end_time - start_time) / 1000000 ))
    echo "Endpoint: $endpoint, Duration: ${duration}ms, Status: $response"
done
```

---

## 📚 API VERSIONING

### **Current Version:** v1.0.0
- **Base URL:** `/api/v1`
- **Stability:** Stable
- **Deprecation:** None

### **Version Migration:**
When new versions are released:
1. **Backward Compatibility:** Maintained for 6 months
2. **Deprecation Notice:** Sent via headers
3. **Migration Guide:** Provided in documentation

### **Version Header:**
```bash
# Check API version
curl -I http://localhost:8080/api/v1/system/status
```

---

## 🔒 SECURITY CONSIDERATIONS

### **Authentication:**
- **Token-based:** Bearer token authentication
- **Session Management:** Automatic token refresh
- **Access Control:** Role-based permissions

### **Data Protection:**
- **HTTPS:** Encrypted communication (when enabled)
- **Input Validation:** All inputs validated
- **Rate Limiting:** 100 requests/minute per client

### **Security Headers:**
```bash
# Security headers included in responses
X-Content-Type-Options: nosniff
X-Frame-Options: DENY
X-XSS-Protection: 1; mode=block
```

---

## 📋 TESTING & VALIDATION

### **API Testing Checklist:**
- [ ] All endpoints respond correctly
- [ ] Error handling works properly
- [ ] Authentication functions correctly
- [ ] Rate limiting is enforced
- [ ] Performance meets requirements
- [ ] Security measures are active

### **Test Commands:**
```bash
# Test all endpoints
for endpoint in status health network/status communication/status safety/status config; do
    echo "Testing: $endpoint"
    curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/api/v1/$endpoint
    echo ""
done
```

---

**Status:** ✅ **COMPLETED - PHASE 4 TASK 1**  
**Next Steps:** Update Safety Documentation (Phase 4 Task 2)

**Changelog v3.0.0:**
- ✅ Added complete API documentation với examples
- ✅ Added error codes và troubleshooting guides
- ✅ Added integration examples cho common use cases
- ✅ Added performance benchmarks và optimization tips
- ✅ Added security considerations và testing checklist
- ✅ Updated với implemented endpoints từ firmware
