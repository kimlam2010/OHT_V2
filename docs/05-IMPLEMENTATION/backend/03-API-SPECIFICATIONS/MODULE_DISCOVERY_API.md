# MODULE DISCOVERY API - OHT-50 Backend v2.0

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-28  
**Mục đích:** Auto-discovery và plug-and-play capability cho OHT-50 modules

---

## 📋 TỔNG QUAN

API này cung cấp auto-discovery và plug-and-play capability cho OHT-50 system, cho phép tự động phát hiện, đăng ký, và quản lý modules trên RS485 bus.

### **🎯 Features:**
- **Auto-discovery:** Tự động phát hiện modules mới
- **Hot-swap:** Thay đổi modules khi đang hoạt động
- **Module Registration:** Đăng ký và quản lý modules
- **Health Monitoring:** Giám sát sức khỏe modules
- **Configuration Management:** Quản lý cấu hình modules

---

## 🔗 BASE URL

```
Base URL: http://localhost:8000/api/v1/discovery
WebSocket: ws://localhost:8000/api/v1/discovery/stream
```

---

## 📡 ENDPOINTS

### **1. DISCOVERY MANAGEMENT**

#### **GET /discovery/status**
Lấy trạng thái discovery system.

**Response:**
```json
{
  "status": "active",
  "discovered_modules": 5,
  "total_modules": 5,
  "last_discovery": "2025-01-28T10:30:00Z",
  "discovery_interval": 30,
  "auto_discovery_enabled": true
}
```

#### **POST /discovery/start**
Bắt đầu quá trình discovery.

**Request:**
```json
{
  "timeout": 60,
  "scan_all_addresses": true,
  "register_automatically": true
}
```

**Response:**
```json
{
  "status": "started",
  "discovery_id": "discovery_20250128_103000",
  "estimated_duration": 60,
  "scanning_addresses": "0x01-0x1F"
}
```

#### **POST /discovery/stop**
Dừng quá trình discovery.

**Response:**
```json
{
  "status": "stopped",
  "discovery_id": "discovery_20250128_103000",
  "modules_found": 5,
  "duration": 45
}
```

#### **GET /discovery/scan**
Thực hiện scan nhanh cho modules.

**Query Parameters:**
- `addresses`: Range addresses (e.g., "0x01-0x0F")
- `timeout`: Timeout in seconds (default: 30)

**Response:**
```json
{
  "scan_id": "scan_20250128_103000",
  "status": "completed",
  "modules_found": [
    {
      "address": "0x01",
      "module_type": "power",
      "module_name": "Power Module",
      "firmware_version": "2.0.0",
      "status": "online",
      "response_time": 15
    },
    {
      "address": "0x02",
      "module_type": "safety",
      "module_name": "Safety Module",
      "firmware_version": "2.0.0",
      "status": "online",
      "response_time": 12
    }
  ],
  "scan_duration": 25
}
```

---

### **2. MODULE REGISTRATION**

#### **GET /discovery/modules**
Lấy danh sách tất cả modules đã đăng ký.

**Query Parameters:**
- `status`: Filter by status (online, offline, error)
- `type`: Filter by module type
- `address`: Filter by address

**Response:**
```json
{
  "modules": [
    {
      "address": "0x01",
      "module_type": "power",
      "module_name": "Power Module",
      "firmware_version": "2.0.0",
      "hardware_version": "1.0.0",
      "serial_number": "PWR001",
      "status": "online",
      "last_seen": "2025-01-28T10:30:00Z",
      "registration_date": "2025-01-28T09:00:00Z",
      "health_score": 95,
      "capabilities": [
        "voltage_monitoring",
        "current_monitoring",
        "temperature_monitoring",
        "charging_control"
      ]
    }
  ],
  "total_modules": 5,
  "online_modules": 5,
  "offline_modules": 0
}
```

#### **POST /discovery/modules/register**
Đăng ký module thủ công.

**Request:**
```json
{
  "address": "0x06",
  "module_type": "lifter_motor",
  "module_name": "Lifter Motor Module",
  "firmware_version": "1.0.0",
  "hardware_version": "1.0.0",
  "serial_number": "LFT001",
  "capabilities": [
    "position_control",
    "load_monitoring",
    "safety_limits"
  ]
}
```

**Response:**
```json
{
  "status": "registered",
  "module_id": "module_0x06_lifter",
  "registration_date": "2025-01-28T10:30:00Z",
  "health_check": "passed"
}
```

#### **DELETE /discovery/modules/{address}**
Hủy đăng ký module.

**Response:**
```json
{
  "status": "unregistered",
  "address": "0x06",
  "unregistration_date": "2025-01-28T10:30:00Z"
}
```

---

### **3. MODULE HEALTH MONITORING**

#### **GET /discovery/modules/{address}/health**
Lấy thông tin sức khỏe của module.

**Response:**
```json
{
  "address": "0x01",
  "health_score": 95,
  "status": "healthy",
  "last_health_check": "2025-01-28T10:30:00Z",
  "response_time": 15,
  "error_count": 0,
  "warnings": [],
  "metrics": {
    "uptime": 86400,
    "communication_errors": 0,
    "response_time_avg": 12,
    "temperature": 45,
    "voltage": 24.2
  }
}
```

#### **POST /discovery/modules/{address}/health-check**
Thực hiện health check cho module.

**Response:**
```json
{
  "address": "0x01",
  "health_check_id": "health_20250128_103000",
  "status": "passed",
  "duration": 5,
  "tests": [
    {
      "test": "communication",
      "status": "passed",
      "duration": 2
    },
    {
      "test": "response_time",
      "status": "passed",
      "duration": 1
    },
    {
      "test": "data_validity",
      "status": "passed",
      "duration": 2
    }
  ]
}
```

---

### **4. CONFIGURATION MANAGEMENT**

#### **GET /discovery/config**
Lấy cấu hình discovery system.

**Response:**
```json
{
  "auto_discovery_enabled": true,
  "discovery_interval": 30,
  "health_check_interval": 60,
  "timeout_settings": {
    "scan_timeout": 30,
    "response_timeout": 5,
    "registration_timeout": 10
  },
  "address_range": {
    "start": "0x01",
    "end": "0x1F"
  },
  "module_types": {
    "mandatory": ["power", "safety", "travel_motor", "dock_location", "master_control"],
    "optional": ["lifter_motor", "cargo_door", "safety_extended"],
    "plug_play": ["rfid_reader", "camera", "environmental_sensor"]
  }
}
```

#### **PUT /discovery/config**
Cập nhật cấu hình discovery system.

**Request:**
```json
{
  "auto_discovery_enabled": true,
  "discovery_interval": 30,
  "health_check_interval": 60,
  "timeout_settings": {
    "scan_timeout": 30,
    "response_timeout": 5,
    "registration_timeout": 10
  }
}
```

**Response:**
```json
{
  "status": "updated",
  "config": {
    "auto_discovery_enabled": true,
    "discovery_interval": 30,
    "health_check_interval": 60
  },
  "applied_at": "2025-01-28T10:30:00Z"
}
```

---

### **5. WEBSOCKET EVENTS**

#### **WebSocket Connection:**
```
ws://localhost:8000/api/v1/discovery/stream
```

#### **Event Types:**

**Module Discovered:**
```json
{
  "event": "module_discovered",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "address": "0x06",
    "module_type": "lifter_motor",
    "module_name": "Lifter Motor Module",
    "firmware_version": "1.0.0",
    "status": "online"
  }
}
```

**Module Status Changed:**
```json
{
  "event": "module_status_changed",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "address": "0x01",
    "previous_status": "online",
    "current_status": "offline",
    "reason": "communication_timeout"
  }
}
```

**Health Check Result:**
```json
{
  "event": "health_check_result",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "address": "0x01",
    "health_score": 95,
    "status": "healthy",
    "response_time": 15
  }
}
```

---

## 🔧 ERROR CODES

| Code | Message | Description |
|------|---------|-------------|
| 400 | Invalid Address Range | Address range không hợp lệ |
| 404 | Module Not Found | Module không tồn tại |
| 408 | Discovery Timeout | Discovery timeout |
| 409 | Module Already Registered | Module đã được đăng ký |
| 500 | Discovery System Error | Lỗi hệ thống discovery |
| 503 | Discovery Service Unavailable | Service discovery không khả dụng |

---

## 📝 EXAMPLES

### **Start Discovery:**
```bash
curl -X POST http://localhost:8000/api/v1/discovery/start \
  -H "Content-Type: application/json" \
  -d '{
    "timeout": 60,
    "scan_all_addresses": true,
    "register_automatically": true
  }'
```

### **Get Module List:**
```bash
curl -X GET "http://localhost:8000/api/v1/discovery/modules?status=online"
```

### **Health Check:**
```bash
curl -X POST http://localhost:8000/api/v1/discovery/modules/0x01/health-check
```

---

## 🔗 RELATED DOCUMENTS

- [Module Specifications](../FIRMWARE/MODULE_SPECIFICATIONS.md)
- [RS485 Communication](../RS485_COMMUNICATION_API.md)
- [System Status API](../SYSTEM_STATUS_API.md)

---

**Status:** API Specification Complete  
**Version:** 1.0.0  
**Next Steps:** Implementation và testing
