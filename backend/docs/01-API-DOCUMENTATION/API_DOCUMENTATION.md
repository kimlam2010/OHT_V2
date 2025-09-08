# 📚 OHT-50 Backend API Documentation

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-28  
**Base URL:** `http://127.0.0.1:8000`  
**API Version:** v1

---

## 📋 **TỔNG QUAN API**

OHT-50 Backend cung cấp **40+ API endpoints** được tổ chức thành 7 nhóm chính:

1. **Authentication API** (5 endpoints) - Xác thực và quản lý người dùng
2. **Robot Control API** (6 endpoints) - Điều khiển robot và lấy trạng thái
3. **Telemetry API** (6 endpoints) - Thu thập và xử lý dữ liệu telemetry
4. **Safety API** (5 endpoints) - Hệ thống an toàn và cảnh báo
5. **Speed Control API** (6 endpoints) - Quản lý tốc độ robot
6. **Configuration API** (4 endpoints) - Cấu hình hệ thống
7. **Monitoring API** (6 endpoints) - Giám sát và theo dõi hệ thống

---

## 🔐 **AUTHENTICATION**

### **Base Authentication**
Tất cả API endpoints (trừ `/health`) yêu cầu JWT token trong header:
```http
Authorization: Bearer <your_jwt_token>
```

### **Token Format**
- **Type:** JWT (JSON Web Token)
- **Expiry:** 30 phút (1800 giây)
- **Algorithm:** HS256
- **Header:** `Authorization: Bearer <token>`

---

## 🔐 **AUTHENTICATION API**

### **POST /api/v1/auth/login**
Đăng nhập vào hệ thống và nhận JWT token.

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
  "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9...",
  "token_type": "bearer",
  "expires_in": 1800,
  "user": {
    "id": 1,
    "username": "admin",
    "email": "admin@oht50.com",
    "role": "administrator",
    "is_active": true
  }
}
```

### **GET /api/v1/auth/me**
Lấy thông tin user hiện tại.

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
Đăng xuất khỏi hệ thống.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "message": "Successfully logged out"
}
```

### **POST /api/v1/auth/register**
Đăng ký user mới (chỉ admin).

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
Lấy danh sách tất cả users (chỉ admin).

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

## 🤖 **ROBOT CONTROL API**

### **GET /api/v1/robot/status**
Lấy trạng thái hiện tại của robot.

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

### **POST /api/v1/robot/control**
Điều khiển robot với lệnh cụ thể.

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

### **POST /api/v1/robot/command**
Gửi lệnh robot với format mở rộng.

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "command_type": "navigation",
  "target_position": {
    "x": 200.0,
    "y": 300.0
  },
  "speed_limit": 0.8,
  "safety_check": true
}
```

**Response:**
```json
{
  "success": true,
  "command_id": "nav_67890",
  "estimated_duration": 45.2,
  "path_clear": true
}
```

### **POST /api/v1/robot/emergency-stop**
Dừng khẩn cấp robot.

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
Lấy vị trí hiện tại của robot.

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
Lấy thông tin pin robot.

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

---

## 📊 **TELEMETRY API**

### **GET /api/v1/telemetry/current**
Lấy dữ liệu telemetry hiện tại.

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
      "scan_count": 360,
      "obstacles_detected": 3,
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

### **GET /api/v1/telemetry/summary**
Lấy tóm tắt thống kê telemetry.

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
Lấy dữ liệu LiDAR scan mới nhất.

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

### **GET /api/v1/telemetry/sensors/status**
Lấy trạng thái tất cả sensors.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "sensors": {
    "lidar": {
      "status": "active",
      "last_scan": "2025-01-28T10:30:00Z",
      "error_count": 0
    },
    "accelerometer": {
      "status": "active",
      "last_reading": "2025-01-28T10:30:00Z",
      "calibration": "valid"
    },
    "rfid": {
      "status": "active",
      "last_tag": "TAG_001",
      "signal_strength": 85
    }
  }
}
```

### **POST /api/v1/telemetry/collection/start**
Bắt đầu thu thập dữ liệu telemetry.

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "collection_mode": "continuous",
  "sampling_rate": 10,
  "duration": 3600
}
```

**Response:**
```json
{
  "success": true,
  "collection_id": "coll_12345",
  "start_time": "2025-01-28T10:30:00Z",
  "estimated_data_points": 36000
}
```

### **POST /api/v1/telemetry/collection/stop**
Dừng thu thập dữ liệu telemetry.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "collection_id": "coll_12345",
  "stop_time": "2025-01-28T10:35:00Z",
  "total_data_points": 3000
}
```

---

## 🛡️ **SAFETY API**

### **GET /api/v1/safety/status**
Lấy trạng thái hệ thống an toàn.

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
Kích hoạt dừng khẩn cấp.

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

### **POST /api/v1/safety/emergency**
Thực hiện hành động khẩn cấp.

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "action": "evacuate",
  "target_position": {
    "x": 0,
    "y": 0
  },
  "priority": "high"
}
```

**Response:**
```json
{
  "success": true,
  "action_id": "emerg_12345",
  "estimated_completion": "2025-01-28T10:40:00Z",
  "safety_clearance": true
}
```

### **GET /api/v1/safety/alerts**
Lấy danh sách cảnh báo an toàn.

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

### **POST /api/v1/safety/alerts/{alert_id}/acknowledge**
Xác nhận cảnh báo an toàn.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "alert_id": "alert_001",
  "acknowledged_at": "2025-01-28T10:35:00Z",
  "acknowledged_by": "admin"
}
```

---

## ⚡ **SPEED CONTROL API**

### **POST /api/v1/speed-control/set-speed**
Đặt tốc độ cho robot.

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "speed": 0.8,
  "acceleration": 0.5,
  "safety_limits": true
}
```

**Response:**
```json
{
  "success": true,
  "current_speed": 0.8,
  "max_speed": 1.0,
  "acceleration": 0.5,
  "safety_limits_active": true,
  "response_time": 0.05
}
```

### **GET /api/v1/speed-control/status**
Lấy trạng thái hệ thống điều khiển tốc độ.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "current_speed": 0.8,
  "target_speed": 0.8,
  "max_speed": 1.0,
  "acceleration": 0.5,
  "deceleration": 0.8,
  "safety_limits": {
    "enabled": true,
    "max_speed_limit": 1.0,
    "emergency_deceleration": 2.0
  },
  "performance": {
    "response_time": 0.05,
    "accuracy": 0.98
  }
}
```

### **GET /api/v1/speed-control/performance**
Lấy metrics hiệu suất hệ thống tốc độ.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "performance_metrics": {
    "average_response_time": 0.05,
    "max_response_time": 0.12,
    "accuracy": 0.98,
    "safety_violations": 0,
    "total_commands": 1250
  },
  "last_24h": {
    "commands_executed": 1250,
    "average_speed": 0.65,
    "safety_stops": 2
  }
}
```

### **POST /api/v1/speed-control/configure-limits**
Cấu hình giới hạn tốc độ.

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "max_speed": 1.0,
  "max_acceleration": 0.8,
  "emergency_deceleration": 2.0,
  "safety_zones": {
    "zone_1": {"max_speed": 0.5},
    "zone_2": {"max_speed": 0.3}
  }
}
```

**Response:**
```json
{
  "success": true,
  "configuration_applied": true,
  "new_limits": {
    "max_speed": 1.0,
    "max_acceleration": 0.8,
    "emergency_deceleration": 2.0
  }
}
```

### **POST /api/v1/speed-control/emergency-stop**
Dừng khẩn cấp hệ thống tốc độ.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "emergency_stop_activated": true,
  "stop_time": "2025-01-28T10:35:00Z",
  "deceleration_rate": 2.0,
  "final_speed": 0.0
}
```

### **POST /api/v1/speed-control/safety-status**
Lấy trạng thái an toàn hệ thống tốc độ.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "safety_status": "normal",
  "speed_within_limits": true,
  "acceleration_within_limits": true,
  "safety_zones": {
    "current_zone": "zone_1",
    "zone_speed_limit": 0.5,
    "current_speed": 0.3
  },
  "last_safety_check": "2025-01-28T10:30:00Z"
}
```

---

## ⚙️ **CONFIGURATION API**

### **GET /api/v1/config/system**
Lấy cấu hình hệ thống.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "system_config": {
    "robot_id": "OHT-50-001",
    "firmware_version": "v2.1.0",
    "operating_mode": "AUTO",
    "safety_enabled": true,
    "telemetry_rate": 10,
    "log_level": "INFO"
  },
  "network_config": {
    "firmware_url": "http://localhost:8081",
    "websocket_port": 8000,
    "api_timeout": 30
  },
  "last_updated": "2025-01-28T09:00:00Z"
}
```

### **PUT /api/v1/config/system**
Cập nhật cấu hình hệ thống.

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "operating_mode": "MANUAL",
  "telemetry_rate": 20,
  "log_level": "DEBUG",
  "safety_enabled": true
}
```

**Response:**
```json
{
  "success": true,
  "configuration_updated": true,
  "new_config": {
    "operating_mode": "MANUAL",
    "telemetry_rate": 20,
    "log_level": "DEBUG",
    "safety_enabled": true
  },
  "restart_required": false
}
```

### **GET /api/v1/config/robot**
Lấy cấu hình robot.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "robot_config": {
    "max_speed": 1.0,
    "max_acceleration": 0.8,
    "battery_threshold": 20,
    "temperature_limit": 60,
    "safety_zones": {
      "zone_1": {"max_speed": 0.5},
      "zone_2": {"max_speed": 0.3}
    }
  },
  "sensor_config": {
    "lidar_enabled": true,
    "rfid_enabled": true,
    "accelerometer_enabled": true
  }
}
```

### **PUT /api/v1/config/robot**
Cập nhật cấu hình robot.

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "max_speed": 1.2,
  "battery_threshold": 15,
  "temperature_limit": 65,
  "sensor_config": {
    "lidar_enabled": true,
    "rfid_enabled": true
  }
}
```

**Response:**
```json
{
  "success": true,
  "configuration_updated": true,
  "new_config": {
    "max_speed": 1.2,
    "battery_threshold": 15,
    "temperature_limit": 65
  },
  "restart_required": true
}
```

---

## 📈 **MONITORING API**

### **GET /health**
Kiểm tra sức khỏe hệ thống (không cần authentication).

**Response:**
```json
{
  "status": "healthy",
  "timestamp": "2025-01-28T10:30:00Z",
  "version": "3.1",
  "uptime": 86400,
  "health_score": 85.5,
  "components": {
    "database": "healthy",
    "firmware_connection": "healthy",
    "websocket": "healthy",
    "authentication": "healthy"
  }
}
```

### **GET /api/v1/monitoring/health**
Lấy thông tin chi tiết về sức khỏe hệ thống.

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
    "websocket": {"status": "healthy", "connections": 3},
    "authentication": {"status": "healthy", "active_sessions": 2}
  }
}
```

### **GET /api/v1/monitoring/metrics/current**
Lấy metrics hiện tại của hệ thống.

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
  },
  "robot_metrics": {
    "battery_level": 87,
    "temperature": 42.5,
    "current_speed": 0.8,
    "position_accuracy": 0.1
  }
}
```

### **GET /api/v1/monitoring/metrics/history**
Lấy lịch sử metrics của hệ thống.

**Headers:** `Authorization: Bearer <token>`

**Query Parameters:**
- `period`: `1h`, `24h`, `7d`, `30d` (default: `1h`)
- `metric`: `cpu`, `memory`, `disk`, `network` (default: all)

**Response:**
```json
{
  "period": "1h",
  "data_points": 60,
  "metrics": {
    "cpu_usage": [
      {"timestamp": "2025-01-28T10:00:00Z", "value": 45.2},
      {"timestamp": "2025-01-28T10:01:00Z", "value": 47.1}
    ],
    "memory_usage": [
      {"timestamp": "2025-01-28T10:00:00Z", "value": 67.8},
      {"timestamp": "2025-01-28T10:01:00Z", "value": 68.2}
    ]
  }
}
```

### **GET /api/v1/monitoring/alerts**
Lấy danh sách alerts hệ thống.

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

### **GET /api/v1/monitoring/logs**
Lấy system logs.

**Headers:** `Authorization: Bearer <token>`

**Query Parameters:**
- `level`: `DEBUG`, `INFO`, `WARNING`, `ERROR` (default: all)
- `limit`: số lượng logs (default: 100)
- `since`: timestamp bắt đầu (ISO format)

**Response:**
```json
{
  "logs": [
    {
      "timestamp": "2025-01-28T10:30:00Z",
      "level": "INFO",
      "message": "Robot status updated",
      "source": "robot_control",
      "details": {"robot_id": "OHT-50-001", "status": "idle"}
    }
  ],
  "total_count": 1,
  "has_more": false
}
```

---

## 📡 **WEBSOCKET ENDPOINTS**

### **ws://127.0.0.1:8000/ws/telemetry**
WebSocket stream cho dữ liệu telemetry real-time.

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
WebSocket stream cho status updates.

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

---

## 🔒 **ERROR CODES**

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

## 🧪 **TESTING**

### **Test Environment**
- **Base URL:** `http://127.0.0.1:8000`
- **Test User:** `admin` / `admin123`
- **Test Database:** SQLite in-memory

### **Example Test Script**
```bash
# Login và lấy token
TOKEN=$(curl -s -X POST "http://127.0.0.1:8000/api/v1/auth/login" \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "admin123"}' | \
  jq -r '.access_token')

# Test robot status
curl -X GET "http://127.0.0.1:8000/api/v1/robot/status" \
  -H "Authorization: Bearer $TOKEN"
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

**📅 Last Updated:** 2025-01-28  
**🚀 API Version:** v1  
**📊 Total Endpoints:** 40+  
**🔗 Base URL:** http://127.0.0.1:8000
