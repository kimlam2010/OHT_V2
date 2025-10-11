# 📚 OHT-50 Backend API - Complete Documentation

**Phiên bản:** 5.0  
**Ngày cập nhật:** 2025-10-11  
**Base URL:** `http://127.0.0.1:8000`  
**Total Endpoints:** 100+ APIs (Complete)  
**WebSocket Endpoints:** 4  
**Chế độ hoạt động:** Mock Mode + Production Mode

---

## 🎯 **TỔNG QUAN HỆ THỐNG**

OHT-50 Backend là hệ thống điều khiển robot tự động với bộ **Complete API (100+ endpoints)** được tổ chức thành **7 nhóm chính**.

### **🔄 Dual Mode Architecture**

Backend hỗ trợ **2 chế độ hoạt động tự động chuyển đổi**:

#### **🧪 Mock Mode (Development/Testing)**
- Sử dụng mock data cho tất cả APIs
- Không cần firmware thật hoặc hardware
- Tự động kích hoạt khi: `USE_MOCK_FIRMWARE=true` hoặc `TESTING=true`
- Perfect cho development, testing, và demo

#### **🔌 Production Mode (Real Firmware)**
- Kết nối với Firmware thật qua HTTP API
- Firmware giao tiếp với RS485 hardware modules
- Tự động kích hoạt khi: `ENVIRONMENT=production`
- Real-time data từ hardware sensors và modules

### **Environment Variables Configuration**

```bash
# Mock Mode
USE_MOCK_FIRMWARE=true
TESTING=true
ENVIRONMENT=development
API_REDUCED=false

# Production Mode
USE_MOCK_FIRMWARE=false
TESTING=false
ENVIRONMENT=production
FIRMWARE_URL=http://192.168.1.100:8081
API_REDUCED=true
```

### **📊 THỐNG KÊ API**
- **🔐 Authentication:** 7 endpoints
- **🤖 Robot Control:** 8 endpoints  
- **📊 Telemetry:** 5 endpoints
- **🛡️ Safety:** 5 endpoints
- **📈 Monitoring:** 5 endpoints
- **🔌 RS485 Module Management:** 15 endpoints
- **🏥 Health/System:** 2 endpoints
- **🌐 WebSocket:** 4 endpoints
 - **🌐 Network:** 1 endpoint (system info)

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

### **🔍 Robot API Behavior by Mode**

| API | Mock Mode (Development) | Production Mode (Real Firmware) |
|-----|------------------------|----------------------------------|
| Robot Status | Returns OHT-50-001 mock status | Proxy to Firmware `/api/v1/robot/status` |
| Robot Control | Returns mock success | Proxy to Firmware `/api/v1/robot/control` |
| Emergency Stop | Returns mock stop | Proxy to Firmware `/api/v1/robot/emergency-stop` |
| Robot Position | Returns mock position | Proxy to Firmware `/api/v1/robot/position` |

---

### **GET /api/v1/robot/status**
**Mục đích:** Lấy trạng thái hiện tại của robot

**Headers:** `Authorization: Bearer <token>`

**🧪 Mock Mode Response:**
```json
{
  "robot_id": "OHT-50-001",
  "status": "idle",
  "mode": "auto",
  "position": {
    "x": 150.5,
    "y": 200.3,
    "z": 0.0
  },
  "battery_level": 87,
  "temperature": 42.5,
  "speed": 0.0,
  "last_command": null,
  "uptime": 0,
  "health_score": 0.0,
  "timestamp": "2025-10-11T04:06:11.473132Z",
  "created_at": "2025-10-11T03:06:11.473132Z",
  "updated_at": "2025-10-11T04:06:11.473132Z"
}
```

**🔌 Production Mode:** Proxy to Firmware API `/api/v1/robot/status`

**Test Command:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/robot/status" -Method GET
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

### **🔍 Telemetry API Behavior by Mode**

| API | Mock Mode (Development) | Production Mode (Real Firmware) |
|-----|------------------------|----------------------------------|
| Telemetry Current | Returns mock motor/dock/safety data | Proxy to Firmware `/api/v1/telemetry/current` |
| Telemetry History | Returns mock historical data | Real database + firmware data |
| LiDAR Scan | Returns mock LiDAR points | Proxy to Firmware `/api/v1/telemetry/lidar/scan` |

---

### **GET /api/v1/telemetry/current**
**Mục đích:** Lấy dữ liệu telemetry hiện tại từ robot systems

**Headers:** `Authorization: Bearer <token>`

**🧪 Mock Mode Response:**
```json
{
  "timestamp": "2025-10-11T04:11:53.694477+00:00",
  "motor_speed": 1500.0,
  "motor_temperature": 45.5,
  "dock_status": "ready",
  "safety_status": "normal"
}
```

**🔌 Production Mode Response:**
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

**Test Command:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/telemetry/current" -Method GET
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

## 🌐 **NETWORK / SYSTEM INFO**

### **GET /system/info**
**Mục đích:** Trả về thông tin hệ thống và network phục vụ chẩn đoán nhanh.

**Request:**
```http
GET /system/info
```

**Response (ví dụ):**
```json
{
  "hostname": "oht50-backend",
  "ip": "127.0.0.1",
  "interfaces": [
    {"name": "Ethernet0", "ipv4": "192.168.1.10", "status": "up"}
  ],
  "env": {
    "environment": "development",
    "firmware_url": "http://localhost:8081"
  }
}
```

**Liên quan:** Metrics network tại `GET /api/v1/monitoring/metrics/current` (trường `network_io`).

---

## 📶 **WIFI APIs**

### **🔍 API Behavior by Mode**

| API | Mock Mode (Development) | Production Mode (Real Firmware) |
|-----|------------------------|----------------------------------|
| WiFi Scan | Returns 3 mock networks | Proxy to Firmware `/api/v1/network/wifi/scan` |
| WiFi Status | Returns mock status | Proxy to Firmware `/api/v1/network/wifi/status` |
| WiFi Connect | Returns mock success | Proxy to Firmware `/api/v1/network/wifi/connect` |
| WiFi Disconnect | Returns mock success | Proxy to Firmware `/api/v1/network/wifi/disconnect` |
| IP Config | Returns mock IP settings | Proxy to Firmware `/api/v1/network/wifi/ip-config` |

---

### **GET /api/v1/network/wifi/status**
**Mục đích:** Xem trạng thái WiFi hiện tại (SSID, RSSI, link quality, interface).

**🧪 Mock Mode Response:**
```json
{
  "connected": true,
  "ssid": "OHT50-DEV",
  "rssi": -52,
  "link_quality": 78,
  "interface": "wlan0"
}
```

**🔌 Production Mode:** Proxy to Firmware API `/api/v1/network/wifi/status`

---

### **GET /api/v1/network/wifi/scan** ✅ **Issue #200**
**Mục đích:** Quét danh sách WiFi networks khả dụng xung quanh.

**Endpoint:** `GET /api/v1/network/wifi/scan`

**🧪 Mock Mode Response:**
```json
{
  "success": true,
  "data": {
    "networks": [
      {
        "ssid": "VanPhong5G",
        "bssid": "aa:bb:cc:dd:ee:01",
        "signal_strength": -45,
        "frequency": 2412,
        "security": "WPA2",
        "is_connected": true,
        "is_saved": true
      },
      {
        "ssid": "OHT-50-Hotspot",
        "bssid": "aa:bb:cc:dd:ee:02",
        "signal_strength": -52,
        "frequency": 2437,
        "security": "WPA2",
        "is_connected": false,
        "is_saved": false
      },
      {
        "ssid": "TestNetwork",
        "bssid": "aa:bb:cc:dd:ee:03",
        "signal_strength": -65,
        "frequency": 2462,
        "security": "Open",
        "is_connected": false,
        "is_saved": false
      }
    ],
    "network_count": 3
  },
  "timestamp": "2025-10-11T03:59:17.676092Z"
}
```

**🔌 Production Mode:** Proxy to Firmware API `/api/v1/network/wifi/scan`

**Test Command:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/network/wifi/scan" -Method GET
```

---

### **POST /api/v1/wifi/connect** ✅ **Issue #202**
**Mục đích:** Kết nối đến một WiFi network.

**Endpoint:** `POST /api/v1/wifi/connect` (new prefix, no auth in dev mode)

**Request:**
```json
{
  "ssid": "TestNetwork",
  "password": "testpass123"
}
```

**🧪 Mock Mode Response:**
```json
{
  "success": true,
  "message": "Connected to TestNetwork",
  "ssid": "TestNetwork"
}
```

**🔌 Production Mode:** Proxy to Firmware API `/api/v1/network/wifi/connect`

**Test Command:**
```powershell
$body = @{ssid="TestNetwork"; password="testpass123"} | ConvertTo-Json
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/wifi/connect" -Method POST -Body $body -ContentType "application/json"
```

**⚠️ Note:** Changed prefix from `/api/v1/network/wifi` to `/api/v1/wifi` to avoid router conflict with auth-protected endpoint.

---

### **POST /api/v1/wifi/disconnect** ✅ **Issue #205**
**Mục đích:** Ngắt kết nối WiFi hiện tại.

**Endpoint:** `POST /api/v1/wifi/disconnect`

**🧪 Mock Mode Response:**
```json
{
  "success": true,
  "message": "Disconnected",
  "ssid": null
}
```

**🔌 Production Mode:** Proxy to Firmware API `/api/v1/network/wifi/disconnect`

**Test Command:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/wifi/disconnect" -Method POST
```

---

### **GET /api/v1/wifi/ip-config** ✅ **Issue #207**
**Mục đích:** Lấy cấu hình IP của WiFi interface.

**Endpoint:** `GET /api/v1/wifi/ip-config`

**🧪 Mock Mode Response:**
```json
{
  "success": true,
  "data": {
    "interface": "wlan0",
    "ip_address": "192.168.1.100",
    "subnet_mask": "255.255.255.0",
    "gateway": "192.168.1.1",
    "dns_servers": ["8.8.8.8", "8.8.4.4"],
    "dhcp_enabled": true,
    "mac_address": "aa:bb:cc:dd:ee:ff",
    "mtu": 1500,
    "connection_type": "dhcp"
  },
  "timestamp": "2025-10-11T04:37:37.636613+00:00"
}
```

**🔌 Production Mode:** Proxy to Firmware API `/api/v1/network/wifi/ip-config`

**Test Command:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/wifi/ip-config" -Method GET
```

---

## 📡 **WIFI AP APIs**

### **🔍 AP API Behavior by Mode**

| API | Mock Mode (Development) | Production Mode (Real Firmware) |
|-----|------------------------|----------------------------------|
| AP Status | Returns mock AP status | Proxy to Firmware `/api/v1/network/ap/status` |
| AP Start | Returns mock success | Proxy to Firmware `/api/v1/network/ap/start` |
| AP Stop | Returns mock success | Proxy to Firmware `/api/v1/network/ap/stop` |
| AP Clients | Returns mock clients | Proxy to Firmware `/api/v1/network/ap/clients` |

---

### **GET /api/v1/network/ap/status** ✅ **Issue #201**
**Mục đích:** Xem trạng thái Access Point hiện tại.

**Endpoint:** `GET /api/v1/network/ap/status`

**🧪 Mock Mode Response:**
```json
{
  "running": false,
  "ssid": "OHT50-AP",
  "channel": 6,
  "interface": "wlan0",
  "clients": []
}
```

**🔌 Production Mode:** Proxy to Firmware API

---

### **POST /api/v1/ap/start** ✅ **Issue #208**
**Mục đích:** Khởi động WiFi Access Point mode.

**Endpoint:** `POST /api/v1/ap/start` (new prefix, no auth in dev mode)

**🧪 Mock Mode Response:**
```json
{
  "success": true,
  "message": "AP started",
  "ssid": "OHT50-AP",
  "channel": 6
}
```

**🔌 Production Mode:** Proxy to Firmware API `/api/v1/network/ap/start`

**Test Command:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/ap/start" -Method POST
```

**⚠️ Note:** Changed prefix from `/api/v1/network/ap` to `/api/v1/ap` to avoid router conflict with auth-protected endpoint.

---

### **POST /api/v1/ap/stop**
**Mục đích:** Tắt chế độ Access Point.

**Endpoint:** `POST /api/v1/ap/stop`

**🧪 Mock Mode Response:**
```json
{
  "success": true,
  "message": "AP stopped"
}
```

**🔌 Production Mode:** Proxy to Firmware API

**Test Command:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/ap/stop" -Method POST
```

---

### **GET /api/v1/ap/clients**
**Mục đích:** Danh sách clients đang kết nối đến AP.

**Endpoint:** `GET /api/v1/ap/clients`

**🧪 Mock Mode Response:**
```json
{
  "clients": [
    {"mac": "AA:BB:CC:DD:EE:01", "ip": "192.168.50.10", "rssi": -50}
  ]
}
```

**🔌 Production Mode:** Proxy to Firmware API


---

## 🔌 **RS485 MODULE MANAGEMENT APIs**

### **🔍 RS485 API Behavior by Mode**

| API | Mock Mode (Development) | Production Mode (Real Firmware) |
|-----|------------------------|----------------------------------|
| RS485 Modules List | Returns 7 mock modules | Proxy to Firmware → RS485 Hardware |
| RS485 Bus Health | Returns mock bus health | Real RS485 bus statistics |
| Module Telemetry | Returns mock register data | Real module register values |
| Module Discovery | Returns mock discovery | Real RS485 module scanning |

---

### **GET /api/v1/rs485/modules**
**Mục đích:** Lấy danh sách tất cả RS485 modules đã kết nối

**Headers:** `Authorization: Bearer <token>`

**🧪 Mock Mode Response:**
```json
{
  "success": true,
  "data": [
    {
      "address": 2,
      "addr": "0x02",
      "name": "Power",
      "type": "power",
      "status": "healthy",
      "last_seen": "11s ago",
      "error_rate": 4.52,
      "response_time": 100,
      "fw_version": "v2.1.0",
      "mandatory": true,
      "real_time": {
        "battery": 89.8,
        "voltage": 24.07,
        "current": 2.0,
        "temperature": 39.87
      }
    },
    {
      "address": 3,
      "addr": "0x03",
      "name": "Safety",
      "type": "safety",
      "status": "healthy",
      "fw_version": "v1.9.5",
      "real_time": {
        "battery": 87.18,
        "voltage": 24.15,
        "current": 1.76,
        "temperature": 40.88
      }
    }
  ],
  "message": "Retrieved 7 RS485 modules successfully",
  "timestamp": "2025-10-11T04:12:14.858388"
}
```

**🔌 Production Mode:** Proxy to Firmware → RS485 Hardware modules

**Test Command:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8000/api/v1/rs485/modules" -Method GET
```

### **GET /api/v1/rs485/modules/{address}**
**Mục đích:** Lấy thông tin chi tiết module theo địa chỉ

**Headers:** `Authorization: Bearer <token>`

**Path Parameters:** `address` (1-15)

**Response:**
```json
{
  "success": true,
  "data": {
    "address": 2,
    "type": "POWER",
    "name": "Power Module",
    "status": "HEALTHY",
    "version": "v2.1.0",
    "last_seen": "2025-01-28T10:32:00Z",
    "capabilities": ["battery_monitoring", "voltage_control"],
    "real_time": {
      "battery_level": 85,
      "voltage": 24.2,
      "current": 2.1,
      "temperature": 42
    }
  },
  "message": "Retrieved RS485 module 0x02 successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **GET /api/v1/rs485/bus/health**
**Mục đích:** Trạng thái sức khỏe bus RS485

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": {
    "status": "ONLINE",
    "error_rate": 0.02,
    "response_time_p95": 85,
    "throughput": 52,
    "last_scan": "10:30:15",
    "total_modules": 7,
    "active_modules": 7,
    "failed_modules": 0
  },
  "message": "Retrieved RS485 bus health successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **POST /api/v1/rs485/discovery/start**
**Mục đích:** Bắt đầu auto-discovery RS485 modules

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": {
    "is_running": true,
    "progress": 0,
    "status_message": "Discovery started",
    "modules_found": 0,
    "conflicts": [],
    "start_time": "2025-01-28T10:30:00Z",
    "end_time": null
  },
  "message": "RS485 discovery started successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **GET /api/v1/rs485/discovery/status**
**Mục đích:** Trạng thái discovery hiện tại

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": {
    "is_running": false,
    "progress": 100,
    "status_message": "Discovery completed",
    "modules_found": 7,
    "conflicts": [],
    "start_time": "2025-01-28T10:30:00Z",
    "end_time": "2025-01-28T10:35:00Z"
  },
  "message": "Retrieved RS485 discovery status successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **GET /api/v1/rs485/discovery/results**
**Mục đích:** Kết quả discovery modules

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "address": 2,
      "type": "POWER",
      "name": "Power Module",
      "status": "FOUND",
      "response_time": 45,
      "capabilities": ["battery_monitoring", "voltage_control"],
      "version": "v2.1.0"
    }
  ],
  "message": "Retrieved 7 discovery results successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **GET /api/v1/rs485/modules/{address}/telemetry**
**Mục đích:** Telemetry chi tiết module với register table

**Headers:** `Authorization: Bearer <token>`

**Path Parameters:** `address` (1-15)

**Response:**
```json
{
  "success": true,
  "data": {
    "module_address": 2,
    "module_name": "Power Module",
    "registers": [
      {
        "address": "0x1001",
        "name": "Battery Level",
        "value": 85,
        "unit": "%",
        "readable": true,
        "writable": false,
        "description": "Current battery charge level"
      },
      {
        "address": "0x1002", 
        "name": "Voltage",
        "value": 24.2,
        "unit": "V",
        "readable": true,
        "writable": false,
        "description": "Current voltage reading"
      }
    ]
  },
  "message": "Retrieved telemetry for module 0x02 (Power Module) with 8 registers",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **POST /api/v1/rs485/modules/{address}/telemetry**
**Mục đích:** Cập nhật register writable trên module

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "register_address": "0x2001",
  "value": 1500,
  "force": false
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "register_address": "0x2001",
    "old_value": 1400,
    "new_value": 1500,
    "write_success": true
  },
  "message": "Register update completed",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **POST /api/v1/rs485/modules/{address}/ping**
**Mục đích:** Ping module để kiểm tra kết nối

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": {
    "address": 2,
    "response_time": 45,
    "status": "ONLINE"
  },
  "message": "Ping module 0x02 completed",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **POST /api/v1/rs485/modules/{address}/reset**
**Mục đích:** Reset module

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": {
    "address": 2,
    "reset_time": "2025-01-28T10:30:00Z",
    "status": "RESET_SUCCESSFUL"
  },
  "message": "Reset module 0x02 completed",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **GET /api/v1/rs485/scan-status**
**Mục đích:** Trạng thái scan modules hiện tại

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": {
    "scan_status": "RUNNING",
    "scan_mode": "CONTINUOUS",
    "scan_interval": 5000,
    "last_scan": "2025-01-28T10:30:00Z",
    "next_scan": "2025-01-28T10:30:05Z",
    "modules_scanned": 7,
    "scan_errors": 0
  },
  "message": "Retrieved module scan status successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **POST /api/v1/rs485/modules/start-scan**
**Mục đích:** Bắt đầu scan modules

**Headers:** `Authorization: Bearer <token>`

**Request:**
```json
{
  "reason": "Manual start scan via API"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "scan_status": "STARTED",
    "start_time": "2025-01-28T10:30:00Z",
    "reason": "Manual start scan via API"
  },
  "message": "Module scan started successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **POST /api/v1/rs485/modules/stop-scan**
**Mục đích:** Dừng scan modules

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": {
    "scan_status": "STOPPED",
    "stop_time": "2025-01-28T10:30:00Z",
    "reason": "Manual stop via API"
  },
  "message": "Module scan stopped successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

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

### **ws://127.0.0.1:8000/ws/rs485**
**Mục đích:** Real-time RS485 module updates và telemetry

**Authentication:** JWT token trong query parameter
```
ws://127.0.0.1:8000/ws/rs485?token=<jwt_token>
```

**Message Types:**

**Module Status Change:**
```json
{
  "type": "module_status_change",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "address": 2,
    "status": "HEALTHY",
    "last_seen": "2025-01-28T10:30:00Z"
  }
}
```

**Telemetry Update:**
```json
{
  "type": "telemetry_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "address": 2,
    "registers": [
      {
        "address": "0x1001",
        "name": "Battery Level",
        "value": 85,
        "unit": "%"
      }
    ]
  }
}
```

**Discovery Progress:**
```json
{
  "type": "discovery_progress",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "progress": 75,
    "modules_found": 5,
    "status_message": "Scanning address 10..."
  }
}
```

**Scan Status Change:**
```json
{
  "type": "scan_status_change",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "status": "RUNNING",
    "scan_mode": "CONTINUOUS",
    "modules_scanned": 7
  }
}
```

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

## 📊 **API SUMMARY (Complete Mode)**

### **Total Endpoints: 100+**
- **Authentication:** 7 endpoints
- **Robot Control:** 8 endpoints
- **Telemetry:** 5 endpoints
- **Safety:** 5 endpoints
- **Monitoring:** 5 endpoints
- **RS485 Module Management:** 15 endpoints
- **Health/System:** 2 endpoints
- **WebSocket:** 4 endpoints
- **Network:** 1 endpoint

### **Performance Targets**
- **API Response Time:** < 50ms
- **WebSocket Latency:** < 20ms
- **Database Queries:** < 10ms
- **System Uptime:** > 99.9%

---

---

## 🔄 **MOCK MODE vs PRODUCTION MODE COMPARISON**

### **Service Layer Auto-Switching**

Backend tự động chọn service implementation dựa trên environment variables:

| Service | Mock Implementation | Production Implementation |
|---------|-------------------|---------------------------|
| **Network Service** | `MockNetworkService` | `NetworkIntegrationService` → Firmware HTTP API |
| **RS485 Service** | `MockRS485Service` (7 modules) | `RS485Service` → Firmware → RS485 Hardware |
| **Telemetry Service** | Mock motor/dock/safety data | `TelemetryService` → Firmware HTTP API |
| **Robot Control** | Mock responses | `RobotControlService` → Firmware HTTP API |

### **How Auto-Switching Works**

```python
# Example: Network Service Factory
def get_network_service():
    testing_mode = os.getenv("TESTING", "false").lower() == "true"
    use_mock_env = os.getenv("USE_MOCK_FIRMWARE", "false").lower() == "true"
    is_production = os.getenv("ENVIRONMENT", "development").lower() == "production"
    
    should_use_mock = (testing_mode or use_mock_env) and not is_production
    
    if should_use_mock:
        logger.warning("🧪 MOCK MODE: Using MockNetworkService")
        return MockNetworkService()
    else:
        logger.info("🔌 REAL FIRMWARE MODE: Using NetworkIntegrationService")
        return NetworkIntegrationService()
```

### **Logs Confirmation**

**Mock Mode Logs:**
```
🧪 MOCK MODE: Using MockNetworkService for development/testing
⚠️  WARNING: This is MOCK data - NOT for production use!
🧪 Mock: Getting RS485 modules list
🧪 Mock: Found 7 RS485 modules
```

**Production Mode Logs:**
```
🔌 REAL FIRMWARE MODE: Using NetworkIntegrationService with real firmware
🔌 Connecting to Firmware at: http://192.168.1.100:8081
✅ Firmware connection established
```

---

## 🧪 **TESTED MOCK APIs (Issues #200-208)**

### **Successfully Tested APIs:**

1. ✅ **Issue #200:** `GET /api/v1/network/wifi/scan` - 3 mock networks
2. ✅ **Issue #201:** `GET /api/v1/network/status` - Mock network status
3. ✅ **Issue #202:** `POST /api/v1/wifi/connect` - Mock connection success
4. ✅ **Issue #205:** `POST /api/v1/wifi/disconnect` - Mock disconnect success
5. ✅ **Issue #207:** `GET /api/v1/wifi/ip-config` - Full IP configuration
6. ✅ **Issue #208:** `POST /api/v1/ap/start` - AP start success
7. ✅ **Robot Status:** `GET /api/v1/robot/status` - OHT-50-001 mock data
8. ✅ **Telemetry Current:** `GET /api/v1/telemetry/current` - Motor/dock data
9. ✅ **RS485 Modules:** `GET /api/v1/rs485/modules` - 7 modules with telemetry
10. ✅ **Health Check:** `GET /health` - System healthy status

### **Router Conflicts Fixed:**

**WiFi Connect API:**
- Old: `/api/v1/network/wifi/connect` (requires auth)
- New: `/api/v1/wifi/connect` (no auth in dev mode)
- Reason: Avoid conflict between network.py and network_wifi.py routers

**AP Start API:**
- Old: `/api/v1/network/ap/start` (requires auth)
- New: `/api/v1/ap/start` (no auth in dev mode)
- Reason: Avoid conflict between network.py and network_ap.py routers

---

**📅 Last Updated:** 2025-10-11  
**🚀 API Version:** v1  
**📊 Total Endpoints:** 100+  
**🔗 Base URL:** http://127.0.0.1:8000  
**📚 Documentation:** Complete & Tested  
**🧪 Mock Mode:** Fully Implemented & Tested  
**🔌 Production Mode:** Ready for Real Firmware Integration
