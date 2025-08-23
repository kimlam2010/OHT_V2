# API ENDPOINTS SPECIFICATION - OHT-50 MASTER MODULE

**Phiên bản:** 2.0.0  
**Ngày cập nhật:** 2025-01-27  
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

## 🔧 API ENDPOINTS

### 1. SYSTEM ENDPOINTS

#### 1.1 GET `/system/status`
**Mô tả:** Lấy trạng thái tổng quan của hệ thống

**Response:**
```json
{
  "system_name": "OHT-50 Master Module",
  "version": "2.0.0",
  "architecture": "v2.0",
  "status": "IDLE",
  "navigation_state": "IDLE",
  "uptime_ms": 1234567890,
  "active_modules": 5,
  "mandatory_modules": {
    "power": "ONLINE",
    "safety": "ONLINE",
    "travel_motor": "ONLINE",
    "dock_location": "ONLINE",
    "master_control": "ONLINE"
  },
  "estop_active": false,
  "safety_ok": true,
  "location_based_safety": true
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
  "details": "All systems operational",
  "mandatory_modules_health": {
    "power": "HEALTHY",
    "safety": "HEALTHY",
    "travel_motor": "HEALTHY",
    "dock_location": "HEALTHY",
    "master_control": "HEALTHY"
  },
  "rs485_communication": "HEALTHY",
  "location_system": "HEALTHY"
}
```

**Status Codes:**
- `200 OK` - Hệ thống khỏe mạnh
- `503 Service Unavailable` - Hệ thống có vấn đề

#### 1.3 GET `/system/architecture`
**Mô tả:** Lấy thông tin kiến trúc hệ thống (NEW)

**Response:**
```json
{
  "architecture_version": "2.0.0",
  "mandatory_modules": [
    {
      "id": 1,
      "name": "Power Module",
      "address": "0x01",
      "type": "MANDATORY",
      "status": "ONLINE"
    },
    {
      "id": 2,
      "name": "Safety Module",
      "address": "0x02",
      "type": "MANDATORY",
      "status": "ONLINE"
    },
    {
      "id": 3,
      "name": "Travel Motor Module",
      "address": "0x03",
      "type": "MANDATORY",
      "status": "ONLINE"
    },
    {
      "id": 5,
      "name": "Dock & Location Module",
      "address": "0x05",
      "type": "MANDATORY",
      "status": "ONLINE"
    },
    {
      "id": 0,
      "name": "Master Control Module",
      "address": "0x00",
      "type": "MANDATORY",
      "status": "ONLINE"
    }
  ],
  "optional_modules": [],
  "plug_and_play_modules": [],
  "communication_standard": "RS485/Modbus RTU"
}
```

### 2. MODULE ENDPOINTS

#### 2.1 GET `/modules`
**Mô tả:** Lấy danh sách tất cả modules

**Response:**
```json
{
  "modules": [
    {
      "module_id": 1,
      "module_type": "POWER",
      "module_name": "Power Module",
      "address": "0x01",
      "type": "MANDATORY",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "2.0.0",
      "communication": "RS485"
    },
    {
      "module_id": 2,
      "module_type": "SAFETY",
      "module_name": "Safety Module",
      "address": "0x02",
      "type": "MANDATORY",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "2.0.0",
      "communication": "RS485"
    },
    {
      "module_id": 3,
      "module_type": "TRAVEL_MOTOR",
      "module_name": "Travel Motor Module",
      "address": "0x03",
      "type": "MANDATORY",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "2.0.0",
      "communication": "RS485"
    },
    {
      "module_id": 5,
      "module_type": "DOCK_LOCATION",
      "module_name": "Dock & Location Module",
      "address": "0x05",
      "type": "MANDATORY",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "2.0.0",
      "communication": "RS485"
    },
    {
      "module_id": 0,
      "module_type": "MASTER_CONTROL",
      "module_name": "Master Control Module",
      "address": "0x00",
      "type": "MANDATORY",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "2.0.0",
      "communication": "RS485"
    }
  ],
  "module_count": 5,
  "mandatory_modules_count": 5,
  "optional_modules_count": 0,
  "plug_and_play_modules_count": 0
}
```

#### 2.2 GET `/modules/{id}`
**Mô tả:** Lấy thông tin chi tiết của module

**Parameters:**
- `id` (path): Module ID (0-32)

**Response:**
```json
{
  "module_id": 3,
  "module_type": "TRAVEL_MOTOR",
  "module_name": "Travel Motor Module",
  "address": "0x03",
  "type": "MANDATORY",
  "status": "ONLINE",
  "online": true,
  "last_seen": 1640995200000,
  "version": "2.0.0",
  "communication": "RS485",
  "specifications": {
    "motors": "2x DC Brushed 12V, 100W each",
    "control": "PID speed control by % speed",
    "sensors": "Hall effect speed sensors (60 PPR)",
    "gearbox": "Planetary 20:1 ratio",
    "wheels": "Omni-directional 150mm"
  },
  "telemetry": {
    "speed": 0.5,
    "speed_unit": "m/s",
    "speed_percentage": 25,
    "position": 0,
    "position_unit": "mm",
    "current": 2.5,
    "current_unit": "A",
    "voltage": 12.0,
    "voltage_unit": "V",
    "temperature": 45,
    "temperature_unit": "°C"
  }
}
```

#### 2.3 POST `/modules/{id}/command`
**Mô tả:** Gửi lệnh đến module

**Parameters:**
- `id` (path): Module ID (0-32)

**Request Body:**
```json
{
  "command": "SET_SPEED",
  "parameters": {
    "speed_percentage": 50,
    "target_speed": 1.0,
    "speed_unit": "m/s"
  }
}
```

**Response:**
```json
{
  "success": true,
  "command": "SET_SPEED",
  "module_id": 3,
  "timestamp": 1640995200000,
  "result": "Speed set to 50% (1.0 m/s)"
}
```

### 3. SAFETY ENDPOINTS

#### 3.1 GET `/safety/status`
**Mô tả:** Lấy trạng thái hệ thống an toàn

**Response:**
```json
{
  "safety_status": "SAFE",
  "estop_active": false,
  "dual_channel_estop": {
    "channel_1": "OK",
    "channel_2": "OK",
    "status": "DUAL_CHANNEL_OK"
  },
  "location_based_safety": {
    "enabled": true,
    "safe_zones": ["ZONE_A", "ZONE_B"],
    "current_zone": "ZONE_A",
    "speed_limit": 1.0,
    "speed_limit_unit": "m/s"
  },
  "safety_levels": {
    "normal": true,
    "warning": false,
    "critical": false,
    "emergency": false
  },
  "safety_sensors": {
    "light_curtain": "OK",
    "safety_mats": "OK",
    "door_sensors": "OK",
    "emergency_brake": "OK"
  }
}
```

#### 3.2 POST `/safety/estop`
**Mô tả:** Kích hoạt E-Stop

**Request Body:**
```json
{
  "estop_type": "EMERGENCY",
  "reason": "Manual activation",
  "channel": "BOTH"
}
```

**Response:**
```json
{
  "success": true,
  "estop_activated": true,
  "timestamp": 1640995200000,
  "reason": "Manual activation",
  "channel": "BOTH"
}
```

### 4. LOCATION ENDPOINTS

#### 4.1 GET `/location/status`
**Mô tả:** Lấy trạng thái hệ thống định vị (NEW)

**Response:**
```json
{
  "location_status": "ACTIVE",
  "position": {
    "x": 1250.5,
    "y": 750.2,
    "z": 0.0,
    "unit": "mm"
  },
  "orientation": {
    "pitch": 0.5,
    "roll": 0.2,
    "yaw": 45.0,
    "unit": "degrees"
  },
  "velocity": {
    "linear": 0.5,
    "angular": 0.0,
    "unit": "m/s"
  },
  "sensors": {
    "imu": "OK",
    "magnetic_sensors": "OK",
    "rfid": "OK",
    "dock_alignment": "OK",
    "lidar": "OK"
  },
  "lidar_integration": {
    "status": "ACTIVE",
    "interface": "USB",
    "model": "RPLIDAR A1M8",
    "scan_frequency": 5.5,
    "scan_frequency_unit": "Hz"
  },
  "navigation_state": "IDLE"
}
```

#### 4.2 GET `/location/map`
**Mô tả:** Lấy thông tin bản đồ (NEW)

**Response:**
```json
{
  "map_status": "AVAILABLE",
  "map_version": "1.0.0",
  "map_size": {
    "width": 10000,
    "height": 8000,
    "unit": "mm"
  },
  "waypoints": [
    {
      "id": "WP001",
      "name": "Dock Station 1",
      "position": {
        "x": 1000,
        "y": 500,
        "z": 0
      },
      "type": "DOCK"
    },
    {
      "id": "WP002",
      "name": "Charging Station",
      "position": {
        "x": 2000,
        "y": 1000,
        "z": 0
      },
      "type": "CHARGING"
    }
  ],
  "obstacles": [],
  "safe_zones": [
    {
      "id": "ZONE_A",
      "name": "Main Working Area",
      "bounds": {
        "x_min": 0,
        "x_max": 5000,
        "y_min": 0,
        "y_max": 3000
      },
      "speed_limit": 1.0
    }
  ]
}
```

### 5. NAVIGATION ENDPOINTS

#### 5.1 GET `/navigation/status`
**Mô tả:** Lấy trạng thái navigation (NEW)

**Response:**
```json
{
  "navigation_state": "IDLE",
  "current_waypoint": null,
  "target_waypoint": null,
  "path": [],
  "path_progress": 0,
  "estimated_arrival": null,
  "obstacles_detected": 0,
  "safety_distance": 500,
  "safety_distance_unit": "mm"
}
```

#### 5.2 POST `/navigation/move`
**Mô tả:** Lệnh di chuyển đến waypoint

**Request Body:**
```json
{
  "target_waypoint": "WP001",
  "speed": 0.8,
  "speed_unit": "m/s",
  "safety_mode": "NORMAL"
}
```

**Response:**
```json
{
  "success": true,
  "navigation_started": true,
  "target_waypoint": "WP001",
  "estimated_duration": 30,
  "estimated_duration_unit": "seconds",
  "navigation_state": "NAVIGATING"
}
```

### 6. POWER ENDPOINTS

#### 6.1 GET `/power/status`
**Mô tả:** Lấy trạng thái hệ thống nguồn

**Response:**
```json
{
  "power_status": "NORMAL",
  "battery": {
    "voltage": 48.5,
    "voltage_unit": "V",
    "current": 2.5,
    "current_unit": "A",
    "soc": 85,
    "soc_unit": "%",
    "temperature": 25,
    "temperature_unit": "°C"
  },
  "charging": {
    "status": "NOT_CHARGING",
    "connected": false,
    "current": 0.0,
    "current_unit": "A"
  },
  "bms": {
    "status": "OK",
    "protection": {
      "overvoltage": false,
      "undervoltage": false,
      "overcurrent": false,
      "overtemperature": false
    }
  }
}
```

### 7. WEBSOCKET ENDPOINTS

#### 7.1 WebSocket `/ws/telemetry`
**Mô tả:** Real-time telemetry data

**Connection:**
```
ws://localhost:8080/ws/telemetry
```

**Message Types:**
```json
{
  "type": "TELEMETRY",
  "timestamp": 1640995200000,
  "data": {
    "system_status": "IDLE",
    "navigation_state": "IDLE",
    "position": {
      "x": 1250.5,
      "y": 750.2,
      "z": 0.0
    },
    "velocity": {
      "linear": 0.0,
      "angular": 0.0
    },
    "battery": {
      "soc": 85,
      "voltage": 48.5
    },
    "safety": {
      "estop_active": false,
      "safety_status": "SAFE"
    }
  }
}
```

---

## 📊 ERROR CODES

### **System Error Codes:**
- `1000` - System initialization error
- `1001` - Module discovery failed
- `1002` - Communication error
- `1003` - Safety system error

### **Module Error Codes:**
- `2000` - Module not found
- `2001` - Module communication error
- `2002` - Module configuration error
- `2003` - Module health check failed

### **Safety Error Codes:**
- `3000` - E-Stop activation error
- `3001` - Safety sensor error
- `3002` - Location-based safety error
- `3003` - Dual-channel E-Stop error

### **Navigation Error Codes:**
- `4000` - Path planning error
- `4001` - Obstacle detection error
- `4002` - Waypoint not found
- `4003` - Navigation timeout

---

## 🔧 CONFIGURATION

### **API Configuration:**
```json
{
  "api": {
    "version": "2.0.0",
    "base_url": "/api/v1",
    "rate_limit": 100,
    "timeout": 30,
    "authentication": "token_based"
  },
  "modules": {
    "mandatory_count": 5,
    "communication": "RS485",
    "auto_discovery": true,
    "hot_swap": true
  },
  "safety": {
    "dual_channel_estop": true,
    "location_based_safety": true,
    "sil2_compliance": true
  }
}
```

---

**Status:** Updated for New Architecture v2.0  
**Next Steps:** Implement new endpoints for location and navigation
