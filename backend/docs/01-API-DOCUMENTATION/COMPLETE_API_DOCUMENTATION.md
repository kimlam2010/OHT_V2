# 📚 OHT-50 Backend API - Complete Documentation

**Phiên bản:** 3.1  
**Ngày cập nhật:** 2025-10-08  
**Base URL:** `http://127.0.0.1:8000`  
**Total Endpoints:** 44 APIs (Core + Network)  
**WebSocket Endpoints:** 3  

---

## 🎯 **TỔNG QUAN HỆ THỐNG**

OHT-50 Backend là hệ thống điều khiển robot tự động với bộ **Core API + Network API (44 endpoints)** được tổ chức thành **7 nhóm chính**:

### **📊 THỐNG KÊ API**
- **🔐 Authentication:** 7 endpoints
- **🤖 Robot Control:** 8 endpoints  
- **📊 Telemetry:** 5 endpoints
- **🛡️ Safety:** 5 endpoints
- **📈 Monitoring:** 5 endpoints
- **🌐 Network & WiFi:** 12 endpoints ⭐ NEW
- **🏥 Health/System:** 2 endpoints
- **🌐 WebSocket:** 3 endpoints

---

## 🔐 **AUTHENTICATION SYSTEM**

### **Base Authentication**
Tất cả API endpoints (trừ `/health`) yêu cầu JWT token:
```http
Authorization: Bearer <your_jwt_token>
```

### **Token Information**
- **Type:** JWT (JSON Web Token)
- **Expiry:** 30 phút (1800 giây)
- **Algorithm:** HS256
- **Header:** `Authorization: Bearer <token>`

---

## 🏥 **HEALTH & SYSTEM APIs**

### **GET /health**
**Mục đích:** Kiểm tra sức khỏe hệ thống (không cần authentication)

**Request:**
```http
GET /health
```

**Response:**
```json
{
  "status": "healthy",
  "timestamp": "2025-01-28T10:30:00Z",
  "version": "1.0.0",
  "checks": {
    "system": "ok",
    "database": "ok", 
    "rs485": "ok"
  }
}
```

**Example Usage:**
```bash
curl -X GET "http://127.0.0.1:8000/health"
```

### **GET /api/v1/health/detailed**
**Mục đích:** Thông tin chi tiết về sức khỏe hệ thống

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "overall_status": "healthy",
  "system_health": "good",
  "components": {
    "database": {"status": "healthy", "response_time": 5.2},
    "firmware": {"status": "healthy", "response_time": 15.8},
    "rs485": {"status": "healthy", "active_modules": 3}
  },
  "performance": {
    "cpu_usage": 45.2,
    "memory_usage": 67.8,
    "disk_usage": 23.5
  }
}
```

---

## 🔐 **AUTHENTICATION APIs**

### **POST /api/v1/auth/login**
**Mục đích:** Đăng nhập vào hệ thống

**Request:**
```json
{
  "username": "admin",
  "password": "admin123"
}
```

**Response:**
```json
{
  "access_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "token_type": "bearer",
  "expires_in": 1800,
  "user": {
    "id": 1,
    "username": "admin",
    "email": "admin@oht50.com",
    "role": "administrator"
  }
}
```

**Example Usage:**
```bash
curl -X POST "http://127.0.0.1:8000/api/v1/auth/login" \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "admin123"}'
```

### **GET /api/v1/auth/me**
**Mục đích:** Lấy thông tin user hiện tại

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "id": 1,
  "username": "admin",
  "email": "admin@oht50.com",
  "role": "administrator",
  "is_active": true,
  "created_at": "2025-01-28T10:00:00Z"
}
```

### **POST /api/v1/auth/logout**
**Mục đích:** Đăng xuất khỏi hệ thống

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "message": "Successfully logged out"
}
```

### **POST /api/v1/auth/register**
**Mục đích:** Đăng ký user mới (chỉ admin)

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "username": "newuser",
  "email": "user@oht50.com",
  "password": "password123",
  "role": "operator"
}
```

**Response:**
```json
{
  "id": 2,
  "username": "newuser",
  "email": "user@oht50.com",
  "role": "operator",
  "is_active": true
}
```

### **GET /api/v1/auth/users**
**Mục đích:** Lấy danh sách tất cả users (chỉ admin)

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
[
  {
    "id": 1,
    "username": "admin",
    "email": "admin@oht50.com",
    "role": "administrator",
    "is_active": true
  },
  {
    "id": 2,
    "username": "operator1",
    "email": "op1@oht50.com",
    "role": "operator",
    "is_active": true
  }
]
```

---

## 🤖 **ROBOT CONTROL APIs**

### **GET /api/v1/robot/status**
**Mục đích:** Lấy trạng thái hiện tại của robot

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "robot_id": "OHT-50-001",
  "status": "idle",
  "operating_mode": "AUTO",
  "position": {
    "x": 150.5,
    "y": 200.3,
    "z": 0.0
  },
  "battery_level": 87,
  "temperature": 42.5,
  "connection_status": "connected",
  "last_update": "2025-01-28T10:30:00Z"
}
```

**Example Usage:**
```bash
curl -X GET "http://127.0.0.1:8000/api/v1/robot/status" \
  -H "Authorization: Bearer $TOKEN"
```

### **POST /api/v1/robot/control**
**Mục đích:** Điều khiển robot với lệnh cụ thể

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "command": "move",
  "parameters": {
    "direction": "forward",
    "speed": 0.5,
    "distance": 100
  }
}
```

**Response:**
```json
{
  "success": true,
  "command_id": "cmd_12345",
  "message": "Command executed successfully",
  "execution_time": 0.15
}
```

### **POST /api/v1/robot/emergency-stop**
**Mục đích:** Dừng khẩn cấp robot

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "stop_time": "2025-01-28T10:35:00Z",
  "reason": "emergency_stop_activated",
  "position": {
    "x": 175.2,
    "y": 225.8
  }
}
```

### **GET /api/v1/robot/position**
**Mục đích:** Lấy vị trí hiện tại của robot

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "position": {
    "x": 150.5,
    "y": 200.3,
    "z": 0.0
  },
  "orientation": 45.2,
  "accuracy": 0.1,
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **GET /api/v1/robot/battery**
**Mục đích:** Lấy thông tin pin robot

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "battery_level": 87,
  "voltage": 24.2,
  "current": 2.5,
  "temperature": 42.5,
  "charging_status": "not_charging",
  "estimated_runtime": 180
}
```

### **POST /api/v1/robot/move/forward**
**Mục đích:** Di chuyển robot tiến

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "speed": 0.5,
  "distance": 100
}
```

**Response:**
```json
{
  "success": true,
  "movement_id": "move_12345",
  "estimated_duration": 20.5,
  "target_position": {
    "x": 250.5,
    "y": 200.3
  }
}
```

### **POST /api/v1/robot/move/backward**
**Mục đích:** Di chuyển robot lùi

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "speed": 0.3,
  "distance": 50
}
```

### **POST /api/v1/robot/move/stop**
**Mục đích:** Dừng robot

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "stop_time": "2025-01-28T10:35:00Z",
  "final_position": {
    "x": 175.2,
    "y": 225.8
  }
}
```

### **GET /api/v1/robot/speed**
**Mục đích:** Lấy tốc độ hiện tại của robot

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "current_speed": 0.5,
  "max_speed": 1.0,
  "acceleration": 0.2,
  "deceleration": 0.3
}
```

### **POST /api/v1/robot/speed**
**Mục đích:** Đặt tốc độ robot

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "speed": 0.8,
  "acceleration": 0.5
}
```

**Response:**
```json
{
  "success": true,
  "new_speed": 0.8,
  "acceleration": 0.5,
  "response_time": 0.05
}
```

---

## 📊 **TELEMETRY APIs**

### **GET /api/v1/telemetry/current**
**Mục đích:** Lấy dữ liệu telemetry hiện tại

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "timestamp": "2025-01-28T10:30:00Z",
  "robot_status": {
    "status": "idle",
    "position": {"x": 150.5, "y": 200.3},
    "battery_level": 87,
    "temperature": 42.5
  },
  "sensors": {
    "lidar": {
      "status": "active",
      "range_max": 10.0
    },
    "accelerometer": {
      "x": 0.1,
      "y": -0.2,
      "z": 9.8
    }
  },
  "performance": {
    "cpu_usage": 45.2,
    "memory_usage": 67.8,
    "network_latency": 12.5
  }
}
```

### **GET /api/v1/telemetry/history**
**Mục đích:** Lấy lịch sử telemetry

**Headers:** `Authorization: Bearer <token>`

**Query Parameters:**
- `start_time`: ISO timestamp
- `end_time`: ISO timestamp  
- `limit`: số lượng records (default: 100)

**Response:**
```json
{
  "period": {
    "start": "2025-01-28T09:00:00Z",
    "end": "2025-01-28T10:30:00Z"
  },
  "data_points": 90,
  "telemetry": [
    {
      "timestamp": "2025-01-28T09:00:00Z",
      "battery_level": 90,
      "temperature": 40.5,
      "position": {"x": 100.0, "y": 150.0}
    }
  ]
}
```

### **GET /api/v1/telemetry/summary**
**Mục đích:** Lấy tóm tắt thống kê telemetry

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "period": "last_hour",
  "total_data_points": 3600,
  "average_battery": 85.2,
  "average_temperature": 41.8,
  "total_distance": 1250.5,
  "obstacles_avoided": 12,
  "performance_score": 92.5
}
```

### **GET /api/v1/telemetry/lidar/scan**
**Mục đích:** Lấy dữ liệu LiDAR scan mới nhất

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "scan_id": "scan_12345",
  "timestamp": "2025-01-28T10:30:00Z",
  "point_count": 360,
  "points": [
    {"angle": 0, "distance": 5.2, "intensity": 0.8},
    {"angle": 1, "distance": 5.1, "intensity": 0.7}
  ],
  "obstacles": [
    {
      "id": "obs_1",
      "position": {"x": 2.5, "y": 3.1},
      "size": 0.5,
      "confidence": 0.95
    }
  ]
}
```

---

## 🛡️ **SAFETY APIs**

### **GET /api/v1/safety/status**
**Mục đích:** Lấy trạng thái hệ thống an toàn

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "safety_status": "normal",
  "emergency_stop": false,
  "safety_zones": {
    "zone_1": "clear",
    "zone_2": "clear"
  },
  "obstacle_detection": {
    "enabled": true,
    "sensitivity": "medium",
    "obstacles_count": 0
  },
  "last_safety_check": "2025-01-28T10:30:00Z"
}
```

### **POST /api/v1/safety/emergency-stop**
**Mục đích:** Kích hoạt dừng khẩn cấp

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "emergency_stop_activated": true,
  "timestamp": "2025-01-28T10:35:00Z",
  "reason": "manual_activation",
  "robot_position": {
    "x": 175.2,
    "y": 225.8
  }
}
```

### **GET /api/v1/safety/alerts**
**Mục đích:** Lấy danh sách cảnh báo an toàn

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "alerts": [
    {
      "id": "alert_001",
      "type": "obstacle_detected",
      "severity": "medium",
      "message": "Obstacle detected in path",
      "timestamp": "2025-01-28T10:25:00Z",
      "acknowledged": false
    }
  ],
  "total_count": 1,
  "unacknowledged_count": 1
}
```

---

## 📈 **MONITORING APIs**

### **GET /api/v1/monitoring/health**
**Mục đích:** Lấy thông tin chi tiết về sức khỏe hệ thống

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "system_health": {
    "overall_score": 85.5,
    "status": "healthy",
    "last_check": "2025-01-28T10:30:00Z"
  },
  "performance_metrics": {
    "cpu_usage": 45.2,
    "memory_usage": 67.8,
    "disk_usage": 23.5,
    "network_latency": 12.5
  },
  "component_status": {
    "database": {"status": "healthy", "response_time": 5.2},
    "firmware": {"status": "healthy", "response_time": 15.8},
    "websocket": {"status": "healthy", "connections": 3}
  }
}
```

### **GET /api/v1/monitoring/metrics/current**
**Mục đích:** Lấy metrics hiện tại của hệ thống

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "timestamp": "2025-01-28T10:30:00Z",
  "system_metrics": {
    "cpu_usage": 45.2,
    "memory_usage": 67.8,
    "disk_usage": 23.5,
    "network_io": {"bytes_sent": 1024000, "bytes_received": 2048000}
  },
  "application_metrics": {
    "active_connections": 3,
    "requests_per_second": 15.2,
    "average_response_time": 45.8,
    "error_rate": 0.02
  }
}
```

### **GET /api/v1/monitoring/alerts**
**Mục đích:** Lấy danh sách alerts hệ thống

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "alerts": [
    {
      "id": "alert_001",
      "type": "high_cpu_usage",
      "severity": "warning",
      "message": "CPU usage above 80%",
      "timestamp": "2025-01-28T10:25:00Z",
      "acknowledged": false,
      "resolved": false
    }
  ],
  "summary": {
    "total_alerts": 1,
    "critical": 0,
    "warning": 1,
    "info": 0,
    "unacknowledged": 1
  }
}
```

---

## 🌐 **NETWORK & WIFI APIs (12 Endpoints)** ⭐ NEW

Backend Network APIs tích hợp với Firmware (port 8080) để quản lý WiFi và network connectivity.

### **📊 Network Monitoring (3 endpoints - Public)**

#### **GET /api/v1/network/status**
**Mô tả:** Lấy trạng thái mạng hiện tại  
**Auth:** ❌ Public  
**Integration:** ✅ Kết nối với Firmware `http://localhost:8080/api/v1/network/status`

**Request:**
```bash
curl http://localhost:8000/api/v1/network/status | jq
```

**Response (200 OK):**
```json
{
  "success": true,
  "data": {
    "connected": false,
    "current_ssid": "",
    "signal_strength": 0,
    "ip_address": "192.168.1.35",
    "gateway": "192.168.1.1",
    "dns": "8.8.8.8",
    "bytes_sent": 0,
    "bytes_received": 0,
    "latency_ms": 0,
    "roaming_active": false,
    "status": "connected",
    "ap_status": "stopped",
    "fallback_enabled": false,
    "config": {},
    "last_heartbeat": "2025-10-08T06:57:52Z",
    "connection_errors": 0
  },
  "timestamp": "2025-10-08T06:57:52Z"
}
```

**Data Fields:**
- `connected` - WiFi connection status (boolean)
- `current_ssid` - SSID đang kết nối (string)
- `signal_strength` - Cường độ tín hiệu dBm (int)
- `ip_address` - IP address hiện tại (string)
- `gateway` - Gateway address (string)
- `dns` - DNS server (string)
- `bytes_sent/received` - Network traffic (int)
- `latency_ms` - Network latency (float)
- `status` - Backend-Firmware connection status (string)
- `ap_status` - WiFi AP mode status (string)
- `fallback_enabled` - Chế độ dự phòng (boolean)

---

#### **GET /api/v1/network/health**
**Mô tả:** Kiểm tra sức khỏe network service  
**Auth:** ❌ Public  
**Integration:** ✅ Backend → Firmware health check

**Request:**
```bash
curl http://localhost:8000/api/v1/network/health | jq
```

**Response (200 OK):**
```json
{
  "success": true,
  "data": {
    "status": "healthy",
    "service": "network",
    "firmware_connected": true,
    "connection_errors": 0,
    "last_heartbeat": "2025-10-08T06:57:52Z",
    "uptime_seconds": 0,
    "memory_usage_percent": 0,
    "cpu_usage_percent": 0
  },
  "error": null,
  "timestamp": "2025-10-08T06:57:53Z"
}
```

**Health Status:**
- `healthy` - Service hoạt động bình thường
- `unhealthy` - Service có vấn đề
- `firmware_connected: true` - Kết nối với Firmware OK
- `connection_errors: 0` - Không có lỗi kết nối

---

#### **GET /api/v1/network/performance**
**Mô tả:** Lấy metrics hiệu suất mạng  
**Auth:** ❌ Public  
**Integration:** ✅ Firmware performance metrics

**Request:**
```bash
curl http://localhost:8000/api/v1/network/performance | jq
```

**Response (200 OK):**
```json
{
  "success": true,
  "data": {
    "bandwidth_mbps": 100.5,
    "latency_ms": 5.2,
    "packet_loss_percent": 0.1,
    "signal_strength_dbm": -45,
    "connection_quality": "excellent",
    "throughput_mbps": 95.3,
    "jitter_ms": 2.1,
    "uptime_seconds": 86400
  },
  "timestamp": "2025-10-08T06:57:54Z"
}
```

**Performance Metrics:**
- `bandwidth_mbps` - Băng thông (Mbps)
- `latency_ms` - Độ trễ (milliseconds)
- `packet_loss_percent` - Tỷ lệ mất gói (%)
- `signal_strength_dbm` - Cường độ tín hiệu (dBm)
- `connection_quality` - Chất lượng kết nối (excellent/good/fair/poor)
- `throughput_mbps` - Throughput thực tế (Mbps)
- `jitter_ms` - Jitter (milliseconds)
- `uptime_seconds` - Thời gian hoạt động (seconds)

---

### **📡 WiFi Client Management (3 endpoints)**

#### **GET /api/v1/network/wifi/scan**
**Mô tả:** Quét các WiFi networks khả dụng  
**Auth:** ❌ Public  
**Integration:** ✅ Firmware `GET /api/v1/network/wifi/scan`

**Request:**
```bash
curl http://localhost:8000/api/v1/network/wifi/scan | jq
```

**Response (200 OK):**
```json
{
  "success": true,
  "data": {
    "networks": [
      {
        "ssid": "OHT-50-Network",
        "bssid": "00:11:22:33:44:55",
        "signal_strength": -45,
        "frequency": 5000,
        "security": "WPA2",
        "is_connected": false,
        "is_saved": false
      },
      {
        "ssid": "OHT-50-Backup",
        "bssid": "00:11:22:33:44:66",
        "signal_strength": -55,
        "frequency": 2400,
        "security": "WPA3",
        "is_connected": false,
        "is_saved": false
      }
    ],
    "network_count": 5
  },
  "timestamp": "2025-10-08T06:57:55Z"
}
```

**WiFi Network Fields:**
- `ssid` - Tên WiFi network (string)
- `bssid` - MAC address của Access Point (string)
- `signal_strength` - Cường độ tín hiệu dBm (int, -100 to 0)
- `frequency` - Tần số MHz (2400=2.4GHz, 5000=5GHz)
- `security` - Loại bảo mật (WPA2/WPA3/Open)
- `is_connected` - Đang kết nối với network này (boolean)
- `is_saved` - Network đã được lưu (boolean)

---

#### **POST /api/v1/network/wifi/connect**
**Mô tả:** Kết nối đến WiFi network  
**Auth:** ✅ ADMIN Token Required  
**Integration:** ✅ Firmware `POST /api/v1/network/wifi/connect`

**Request:**
```bash
curl -X POST http://localhost:8000/api/v1/network/wifi/connect \
  -H "Authorization: Bearer oht50_admin_token_2025" \
  -H "Content-Type: application/json" \
  -d '{
    "ssid": "Factory-WiFi",
    "password": "SecurePassword123",
    "security": "WPA2"
  }' | jq
```

**Request Body:**
```json
{
  "ssid": "Factory-WiFi",
  "password": "SecurePassword123",
  "security": "WPA2"
}
```

**Response (200 OK):**
```json
{
  "success": true,
  "message": "Connected to Factory-WiFi",
  "data": {
    "ssid": "Factory-WiFi",
    "ip_address": "192.168.1.50",
    "signal_strength": -45,
    "connected_at": "2025-10-08T07:00:00Z"
  },
  "timestamp": "2025-10-08T07:00:00Z"
}
```

**Error Response (500):**
```json
{
  "success": false,
  "message": "Failed to connect to WiFi",
  "error": "Connection timeout"
}
```

---

#### **POST /api/v1/network/wifi/disconnect**
**Mô tả:** Ngắt kết nối WiFi hiện tại  
**Auth:** ✅ ADMIN Token Required  
**Integration:** ✅ Firmware `POST /api/v1/network/wifi/disconnect`

**Request:**
```bash
curl -X POST http://localhost:8000/api/v1/network/wifi/disconnect \
  -H "Authorization: Bearer oht50_admin_token_2025" | jq
```

**Response (200 OK):**
```json
{
  "success": true,
  "message": "Disconnected from WiFi",
  "timestamp": "2025-10-08T07:05:00Z"
}
```

---

### **📱 WiFi Access Point Management (4 endpoints)**

#### **POST /api/v1/network/ap/start**
**Mô tả:** Khởi động WiFi AP mode  
**Auth:** ✅ ADMIN Token Required  
**Integration:** ✅ Firmware `POST /api/v1/network/ap/start`

**Request:**
```bash
curl -X POST http://localhost:8000/api/v1/network/ap/start \
  -H "Authorization: Bearer oht50_admin_token_2025" \
  -H "Content-Type: application/json" \
  -d '{
    "ssid": "OHT-50-AP",
    "password": "OhtPass123!",
    "channel": 6,
    "security": "WPA2",
    "hidden": false,
    "max_clients": 10
  }' | jq
```

**Request Body:**
```json
{
  "ssid": "OHT-50-AP",
  "password": "OhtPass123!",
  "channel": 6,
  "security": "WPA2",
  "hidden": false,
  "max_clients": 10
}
```

**Password Requirements:**
- Minimum 8 characters
- Recommended: 12+ characters với uppercase, lowercase, numbers, special chars

**Response (200 OK):**
```json
{
  "success": true,
  "message": "WiFi AP started successfully",
  "data": {
    "ssid": "OHT-50-AP",
    "ip_address": "192.168.4.1",
    "channel": 6,
    "security": "WPA2",
    "max_clients": 10,
    "started_at": "2025-10-08T07:10:00Z"
  },
  "timestamp": "2025-10-08T07:10:00Z"
}
```

**Use Cases:**
- Kết nối Mobile App khi không có WiFi
- Remote access trong emergency
- Commissioning và maintenance
- Backup connectivity

---

#### **POST /api/v1/network/ap/stop**
**Mô tả:** Dừng WiFi AP mode  
**Auth:** ✅ ADMIN Token Required  
**Integration:** ✅ Firmware `POST /api/v1/network/ap/stop`

**Request:**
```bash
curl -X POST http://localhost:8000/api/v1/network/ap/stop \
  -H "Authorization: Bearer oht50_admin_token_2025" | jq
```

**Response (200 OK):**
```json
{
  "success": true,
  "message": "WiFi AP stopped successfully",
  "timestamp": "2025-10-08T07:15:00Z"
}
```

---

#### **POST /api/v1/network/ap/config**
**Mô tả:** Cấu hình WiFi AP settings  
**Auth:** ✅ ADMIN Token Required  
**Integration:** ✅ Firmware `POST /api/v1/network/ap/config`

**Request:**
```bash
curl -X POST http://localhost:8000/api/v1/network/ap/config \
  -H "Authorization: Bearer oht50_admin_token_2025" \
  -H "Content-Type: application/json" \
  -d '{
    "ssid": "OHT-50-AP-Updated",
    "channel": 11,
    "max_clients": 20
  }' | jq
```

**Request Body (all fields optional):**
```json
{
  "ssid": "OHT-50-AP-Updated",
  "password": "NewPassword123!",
  "channel": 11,
  "security": "WPA2",
  "hidden": false,
  "max_clients": 20,
  "bandwidth_limit": 100
}
```

**Response (200 OK):**
```json
{
  "success": true,
  "message": "AP configuration updated",
  "data": {
    "ssid": "OHT-50-AP-Updated",
    "channel": 11,
    "max_clients": 20
  },
  "timestamp": "2025-10-08T07:20:00Z"
}
```

---

#### **GET /api/v1/network/ap/clients**
**Mô tả:** Lấy danh sách devices kết nối vào AP  
**Auth:** ❌ Public  
**Integration:** ✅ Firmware `GET /api/v1/network/ap/clients`

**Request:**
```bash
curl http://localhost:8000/api/v1/network/ap/clients | jq
```

**Response (200 OK - No clients):**
```json
{
  "success": true,
  "data": {
    "clients": [],
    "client_count": 0
  },
  "timestamp": "2025-10-08T07:25:00Z"
}
```

**Response (200 OK - With clients):**
```json
{
  "success": true,
  "data": {
    "clients": [
      {
        "mac_address": "AA:BB:CC:DD:EE:22",
        "ip_address": "192.168.4.2",
        "hostname": "mobile-phone",
        "device_type": "mobile",
        "signal_strength": -40,
        "connected_at": "2025-10-08T07:20:00Z",
        "last_seen": "2025-10-08T07:25:00Z",
        "bytes_sent": 1024000,
        "bytes_received": 512000,
        "session_duration": 300,
        "is_connected": true,
        "is_blocked": false
      }
    ],
    "client_count": 1
  },
  "timestamp": "2025-10-08T07:25:00Z"
}
```

**AP Client Fields:**
- `mac_address` - MAC address của client (string)
- `ip_address` - IP được cấp cho client (string)
- `hostname` - Hostname của device (string)
- `device_type` - Loại thiết bị: mobile/laptop/tablet (string)
- `signal_strength` - Cường độ tín hiệu dBm (int)
- `connected_at` - Thời điểm kết nối (ISO timestamp)
- `last_seen` - Lần cuối thấy device (ISO timestamp)
- `bytes_sent/received` - Data usage (bytes)
- `session_duration` - Thời gian kết nối (seconds)
- `is_connected` - Vẫn đang kết nối (boolean)
- `is_blocked` - Device bị block (boolean)

---

### **🔄 Fallback Connectivity (2 endpoints)**

#### **POST /api/v1/network/fallback/enable**
**Mô tả:** Bật chế độ dự phòng (Ethernet → WiFi auto-failover)  
**Auth:** ✅ ADMIN Token Required  
**Integration:** ✅ Firmware `POST /api/v1/network/fallback/enable`

**Request:**
```bash
curl -X POST http://localhost:8000/api/v1/network/fallback/enable \
  -H "Authorization: Bearer oht50_admin_token_2025" | jq
```

**Response (200 OK):**
```json
{
  "success": true,
  "message": "Fallback connectivity enabled",
  "data": {
    "fallback_enabled": true,
    "primary_interface": "ethernet",
    "backup_interface": "wifi",
    "auto_failover": true,
    "failover_timeout_ms": 5000
  },
  "timestamp": "2025-10-08T07:30:00Z"
}
```

**Fallback Features:**
- Auto-failover khi Ethernet mất kết nối
- Tự động switch sang WiFi backup
- Tự động switch lại Ethernet khi available
- Configurable failover timeout

---

#### **POST /api/v1/network/fallback/disable**
**Mô tả:** Tắt chế độ dự phòng  
**Auth:** ✅ ADMIN Token Required  
**Integration:** ✅ Firmware `POST /api/v1/network/fallback/disable`

**Request:**
```bash
curl -X POST http://localhost:8000/api/v1/network/fallback/disable \
  -H "Authorization: Bearer oht50_admin_token_2025" | jq
```

**Response (200 OK):**
```json
{
  "success": true,
  "message": "Fallback connectivity disabled",
  "timestamp": "2025-10-08T07:35:00Z"
}
```

---

## 🔑 **NETWORK AUTHENTICATION**

### **Admin Token (cho các operations như AP start/stop, WiFi connect):**
```bash
ADMIN_TOKEN="oht50_admin_token_2025"

# Usage:
curl -H "Authorization: Bearer oht50_admin_token_2025" \
     http://localhost:8000/api/v1/network/ap/start
```

### **Protected Network Endpoints:**
- 🔴 `POST /network/wifi/connect` - ADMIN
- 🔴 `POST /network/wifi/disconnect` - ADMIN
- 🔴 `POST /network/ap/start` - ADMIN
- 🔴 `POST /network/ap/stop` - ADMIN
- 🔴 `POST /network/ap/config` - ADMIN
- 🔴 `POST /network/fallback/enable` - ADMIN
- 🔴 `POST /network/fallback/disable` - ADMIN

### **Public Network Endpoints:**
- 🔵 `GET /network/status` - Public
- 🔵 `GET /network/health` - Public
- 🔵 `GET /network/performance` - Public
- 🔵 `GET /network/wifi/scan` - Public
- 🔵 `GET /network/ap/clients` - Public

---

## 📊 **NETWORK API INTEGRATION FLOW**

```
┌────────────────────────────────────────┐
│     FRONTEND (Dashboard/Mobile)        │
│     WiFi Settings, AP Mode Control     │
└───────────────┬────────────────────────┘
                │ HTTP/REST
                ▼
┌────────────────────────────────────────┐
│   BACKEND (Python FastAPI) - Port 8000│
│   ✅ 12 Network API Endpoints          │
│   ✅ Network Integration Service       │
│   ✅ FW Client (HTTP)                  │
└───────────────┬────────────────────────┘
                │ HTTP API
                ▼
┌────────────────────────────────────────┐
│   FIRMWARE (C Service) - Port 8080    │
│   ✅ 27 Network API Endpoints          │
│   ✅ WiFi Management                   │
│   ✅ AP Mode Control                   │
│   ✅ Hardware Interface                │
└────────────────────────────────────────┘
```

---

## 🧪 **NETWORK API TESTING EXAMPLES**

### **Test Script: Full Network API Testing**

```bash
#!/bin/bash
BASE_URL="http://localhost:8000/api/v1"
ADMIN_TOKEN="oht50_admin_token_2025"

echo "=== 🔵 TEST 1: Network Status ==="
curl -s $BASE_URL/network/status | jq .

echo -e "\n=== 🔵 TEST 2: Network Health ==="
curl -s $BASE_URL/network/health | jq .

echo -e "\n=== 🔵 TEST 3: Network Performance ==="
curl -s $BASE_URL/network/performance | jq .

echo -e "\n=== 🟢 TEST 4: WiFi Scan ==="
curl -s $BASE_URL/network/wifi/scan | jq '.data.networks[0:2]'

echo -e "\n=== 🟢 TEST 5: WiFi Connect ==="
curl -s -X POST $BASE_URL/network/wifi/connect \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "ssid": "TestNetwork",
    "password": "SecurePass@2025",
    "security": "WPA2"
  }' | jq .

echo -e "\n=== 🟡 TEST 6: Start AP Mode ==="
curl -s -X POST $BASE_URL/network/ap/start \
  -H "Authorization: Bearer $ADMIN_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "ssid": "OHT-50-TEST",
    "password": "TestPass123!",
    "channel": 6,
    "security": "WPA2",
    "hidden": false,
    "max_clients": 10
  }' | jq .

sleep 5

echo -e "\n=== 🟡 TEST 7: Get AP Clients ==="
curl -s $BASE_URL/network/ap/clients | jq .

echo -e "\n=== 🟡 TEST 8: Stop AP Mode ==="
curl -s -X POST $BASE_URL/network/ap/stop \
  -H "Authorization: Bearer $ADMIN_TOKEN" | jq .

echo -e "\n=== 🟣 TEST 9: Enable Fallback ==="
curl -s -X POST $BASE_URL/network/fallback/enable \
  -H "Authorization: Bearer $ADMIN_TOKEN" | jq .

echo -e "\n=== 🟣 TEST 10: Disable Fallback ==="
curl -s -X POST $BASE_URL/network/fallback/disable \
  -H "Authorization: Bearer $ADMIN_TOKEN" | jq .
```

---

<!-- RS485 section removed in Core API mode -->

## 🌐 **WEBSOCKET ENDPOINTS**

### **ws://127.0.0.1:8000/ws/telemetry**
**Mục đích:** Real-time telemetry data stream

**Authentication:** JWT token trong query parameter
```
ws://127.0.0.1:8000/ws/telemetry?token=<jwt_token>
```

**Message Format:**
```json
{
  "type": "telemetry",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "robot_status": "idle",
    "battery_level": 87,
    "temperature": 42.5,
    "position": {"x": 150.5, "y": 200.3}
  }
}
```

### **ws://127.0.0.1:8000/ws/status**
**Mục đích:** Real-time status updates

**Authentication:** JWT token trong query parameter
```
ws://127.0.0.1:8000/ws/status?token=<jwt_token>
```

**Message Format:**
```json
{
  "type": "status_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "robot_id": "OHT-50-001",
    "status": "moving",
    "operating_mode": "AUTO",
    "alerts": []
  }
}
```

<!-- RS485 WebSocket removed in Core API mode -->

---

## 🔒 **ERROR CODES & RESPONSES**

### **HTTP Status Codes**
| Code | Mô tả | Nguyên nhân |
|------|-------|-------------|
| `200` | OK | Request thành công |
| `201` | Created | Resource được tạo thành công |
| `400` | Bad Request | Request không hợp lệ |
| `401` | Unauthorized | Token không hợp lệ hoặc hết hạn |
| `403` | Forbidden | Không có quyền truy cập |
| `404` | Not Found | Resource không tồn tại |
| `422` | Unprocessable Entity | Dữ liệu validation lỗi |
| `500` | Internal Server Error | Lỗi server |

### **Error Response Format**
```json
{
  "error": {
    "code": "INVALID_TOKEN",
    "message": "Token has expired",
    "details": "Token expired at 2025-01-28T10:00:00Z"
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "request_id": "req_12345"
}
```

---

## 📊 **RATE LIMITING**

### **Rate Limits**
- **Authentication endpoints:** 10 requests/minute
- **Robot control endpoints:** 100 requests/minute
- **Telemetry endpoints:** 1000 requests/minute
- **Monitoring endpoints:** 200 requests/minute

### **Rate Limit Headers**
```http
X-RateLimit-Limit: 100
X-RateLimit-Remaining: 95
X-RateLimit-Reset: 1640995200
```

---

## 🧪 **TESTING EXAMPLES**

### **Complete Test Script**
```bash
#!/bin/bash

# Base URL
BASE_URL="http://127.0.0.1:8000"

# 1. Health Check
echo "=== Health Check ==="
curl -X GET "$BASE_URL/health"

# 2. Login
echo -e "\n=== Login ==="
TOKEN=$(curl -s -X POST "$BASE_URL/api/v1/auth/login" \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "admin123"}' | \
  jq -r '.access_token')

echo "Token: $TOKEN"

# 3. Robot Status
echo -e "\n=== Robot Status ==="
curl -X GET "$BASE_URL/api/v1/robot/status" \
  -H "Authorization: Bearer $TOKEN"

# 4. Telemetry
echo -e "\n=== Telemetry ==="
curl -X GET "$BASE_URL/api/v1/telemetry/current" \
  -H "Authorization: Bearer $TOKEN"

# 5. Safety Status
echo -e "\n=== Safety Status ==="
curl -X GET "$BASE_URL/api/v1/safety/status" \
  -H "Authorization: Bearer $TOKEN"
```

### **Python Test Script**
```python
import requests
import json

# Configuration
BASE_URL = "http://127.0.0.1:8000"
USERNAME = "admin"
PASSWORD = "admin123"

# Login
def login():
    response = requests.post(f"{BASE_URL}/api/v1/auth/login", 
                           json={"username": USERNAME, "password": PASSWORD})
    return response.json()["access_token"]

# Test API
def test_api(token):
    headers = {"Authorization": f"Bearer {token}"}
    
    # Health check
    health = requests.get(f"{BASE_URL}/health")
    print(f"Health: {health.json()}")
    
    # Robot status
    robot = requests.get(f"{BASE_URL}/api/v1/robot/status", headers=headers)
    print(f"Robot: {robot.json()}")
    
    # Telemetry
    telemetry = requests.get(f"{BASE_URL}/api/v1/telemetry/current", headers=headers)
    print(f"Telemetry: {telemetry.json()}")

# Run tests
if __name__ == "__main__":
    token = login()
    test_api(token)
```

---

## 📚 **ADDITIONAL RESOURCES**

### **Interactive Documentation**
- **Swagger UI:** http://127.0.0.1:8000/docs
- **ReDoc:** http://127.0.0.1:8000/redoc

### **SDK và Libraries**
- **Python:** `httpx`, `websockets`
- **JavaScript:** `axios`, `ws`
- **cURL:** Examples trong documentation

### **Support**
- **Documentation:** This file và Swagger UI
- **Issues:** GitHub Issues
- **Team:** Backend Development Team

---

## 📊 **API SUMMARY**

### **Total Endpoints: 100+**
- **Authentication:** 7 endpoints
- **Robot Control:** 25 endpoints
- **Telemetry:** 15 endpoints
- **Safety:** 5 endpoints
- **Monitoring:** 15 endpoints
- **RS485:** 20 endpoints
- **System:** 10 endpoints
- **WebSocket:** 3 endpoints

### **Performance Targets**
- **API Response Time:** < 50ms
- **WebSocket Latency:** < 20ms
- **Database Queries:** < 10ms
- **System Uptime:** > 99.9%

---

**📅 Last Updated:** 2025-01-28  
**🚀 API Version:** v1  
**📊 Total Endpoints:** 100+  
**🔗 Base URL:** http://127.0.0.1:8000  
**📚 Documentation:** Complete & Tested
