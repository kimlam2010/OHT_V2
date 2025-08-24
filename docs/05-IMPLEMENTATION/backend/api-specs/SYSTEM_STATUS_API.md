# SYSTEM STATUS API - OHT-50 Backend v2.0

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-28  
**Mục đích:** Quản lý system status và health monitoring cho OHT-50

---

## 📋 TỔNG QUAN

API này cung cấp system status và health monitoring cho OHT-50 system, bao gồm overall system health, module status, và system performance metrics.

### **🎯 Features:**
- **System Health Monitoring:** Giám sát sức khỏe tổng thể hệ thống
- **Module Status Overview:** Tổng quan trạng thái tất cả modules
- **Performance Metrics:** Metrics hiệu suất hệ thống
- **System Diagnostics:** Diagnostic tools cho troubleshooting
- **Status History:** Lịch sử trạng thái hệ thống

---

## 🔗 BASE URL

```
Base URL: http://localhost:8000/api/v1/system
WebSocket: ws://localhost:8000/api/v1/system/stream
```

---

## 📡 ENDPOINTS

### **1. SYSTEM OVERVIEW**

#### **GET /system/status**
Lấy trạng thái tổng thể của hệ thống.

**Response:**
```json
{
  "system_status": "operational",
  "overall_health": 95,
  "operational_mode": "autonomous",
  "uptime": 86400,
  "last_restart": "2025-01-27T10:00:00Z",
  "modules": {
    "total": 5,
    "online": 5,
    "offline": 0,
    "error": 0
  },
  "safety_status": "safe",
  "mission_status": "idle",
  "last_update": "2025-01-28T10:30:00Z"
}
```

#### **GET /system/health**
Lấy thông tin chi tiết về sức khỏe hệ thống.

**Response:**
```json
{
  "system_health": {
    "overall_score": 95,
    "status": "healthy",
    "last_assessment": "2025-01-28T10:30:00Z"
  },
  "component_health": {
    "power_system": 98,
    "safety_system": 95,
    "motor_system": 92,
    "navigation_system": 90,
    "communication_system": 95
  },
  "critical_alerts": [],
  "warnings": [
    {
      "type": "performance",
      "message": "Motor temperature approaching limit",
      "severity": "warning",
      "timestamp": "2025-01-28T10:25:00Z"
    }
  ]
}
```

---

### **2. MODULE STATUS OVERVIEW**

#### **GET /system/modules/status**
Lấy trạng thái tất cả modules.

**Response:**
```json
{
  "modules": [
    {
      "address": "0x01",
      "module_type": "power",
      "module_name": "Power Module",
      "status": "online",
      "health_score": 98,
      "last_communication": "2025-01-28T10:30:00Z",
      "operational_status": "normal"
    },
    {
      "address": "0x02",
      "module_type": "safety",
      "module_name": "Safety Module",
      "status": "online",
      "health_score": 95,
      "last_communication": "2025-01-28T10:30:00Z",
      "operational_status": "normal"
    },
    {
      "address": "0x03",
      "module_type": "travel_motor",
      "module_name": "Travel Motor Module",
      "status": "online",
      "health_score": 92,
      "last_communication": "2025-01-28T10:30:00Z",
      "operational_status": "warning"
    }
  ],
  "summary": {
    "total_modules": 5,
    "online_modules": 5,
    "offline_modules": 0,
    "error_modules": 0,
    "average_health": 95
  }
}
```

#### **GET /system/modules/{address}/status**
Lấy trạng thái chi tiết của module cụ thể.

**Response:**
```json
{
  "address": "0x01",
  "module_type": "power",
  "module_name": "Power Module",
  "status": "online",
  "health_score": 98,
  "operational_status": "normal",
  "last_communication": "2025-01-28T10:30:00Z",
  "uptime": 86400,
  "firmware_version": "2.0.0",
  "hardware_version": "1.0.0",
  "performance_metrics": {
    "response_time": 12,
    "error_rate": 0.1,
    "temperature": 45,
    "voltage": 24.2
  }
}
```

---

### **3. PERFORMANCE MONITORING**

#### **GET /system/performance**
Lấy metrics hiệu suất hệ thống.

**Response:**
```json
{
  "performance_metrics": {
    "system_throughput": 1000,
    "average_response_time": 15,
    "error_rate": 0.1,
    "cpu_usage": 45,
    "memory_usage": 60,
    "disk_usage": 30
  },
  "communication_metrics": {
    "total_communications": 100000,
    "successful_communications": 99900,
    "failed_communications": 100,
    "average_latency": 12
  },
  "mission_metrics": {
    "missions_completed": 50,
    "missions_failed": 2,
    "average_mission_time": 300,
    "success_rate": 96
  }
}
```

#### **GET /system/performance/history**
Lấy lịch sử performance metrics.

**Query Parameters:**
- `start_time`: Start time for history
- `end_time`: End time for history
- `interval`: Data interval (1m, 5m, 15m, 1h)

**Response:**
```json
{
  "history": [
    {
      "timestamp": "2025-01-28T10:00:00Z",
      "system_health": 95,
      "cpu_usage": 45,
      "memory_usage": 60,
      "error_rate": 0.1
    },
    {
      "timestamp": "2025-01-28T10:05:00Z",
      "system_health": 94,
      "cpu_usage": 47,
      "memory_usage": 62,
      "error_rate": 0.1
    }
  ],
  "summary": {
    "average_health": 94.5,
    "peak_cpu_usage": 50,
    "peak_memory_usage": 65
  }
}
```

---

### **4. SYSTEM DIAGNOSTICS**

#### **POST /system/diagnostics/start**
Bắt đầu diagnostic session.

**Request:**
```json
{
  "diagnostic_type": "comprehensive",
  "modules": ["0x01", "0x02", "0x03"],
  "duration": 300
}
```

**Response:**
```json
{
  "diagnostic_id": "diag_20250128_103000",
  "status": "started",
  "estimated_duration": 300,
  "modules_to_test": ["0x01", "0x02", "0x03"]
}
```

#### **GET /system/diagnostics/{diagnostic_id}**
Lấy kết quả diagnostic.

**Response:**
```json
{
  "diagnostic_id": "diag_20250128_103000",
  "status": "completed",
  "start_time": "2025-01-28T10:30:00Z",
  "end_time": "2025-01-28T10:35:00Z",
  "results": {
    "overall_health": 95,
    "modules_tested": 3,
    "issues_found": 1,
    "recommendations": [
      "Check motor temperature monitoring",
      "Consider reducing load on travel motor"
    ]
  },
  "module_results": [
    {
      "address": "0x01",
      "health_score": 98,
      "issues": [],
      "recommendations": []
    },
    {
      "address": "0x03",
      "health_score": 85,
      "issues": ["High temperature"],
      "recommendations": ["Check motor cooling"]
    }
  ]
}
```

---

### **5. SYSTEM CONTROL**

#### **POST /system/restart**
Khởi động lại hệ thống.

**Request:**
```json
{
  "restart_type": "soft",
  "reason": "Performance optimization"
}
```

**Response:**
```json
{
  "status": "restarting",
  "restart_id": "restart_20250128_103000",
  "estimated_duration": 60,
  "reason": "Performance optimization"
}
```

#### **POST /system/shutdown**
Tắt hệ thống.

**Request:**
```json
{
  "shutdown_type": "graceful",
  "reason": "Maintenance"
}
```

**Response:**
```json
{
  "status": "shutting_down",
  "shutdown_id": "shutdown_20250128_103000",
  "estimated_duration": 30,
  "reason": "Maintenance"
}
```

---

### **6. WEBSOCKET EVENTS**

#### **WebSocket Connection:**
```
ws://localhost:8000/api/v1/system/stream
```

#### **Event Types:**

**System Status Change:**
```json
{
  "event": "system_status_change",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "previous_status": "operational",
    "current_status": "warning",
    "reason": "High motor temperature"
  }
}
```

**Module Status Update:**
```json
{
  "event": "module_status_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "address": "0x03",
    "module_type": "travel_motor",
    "status": "warning",
    "health_score": 85,
    "reason": "High temperature"
  }
}
```

**Performance Alert:**
```json
{
  "event": "performance_alert",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "metric": "cpu_usage",
    "value": 85,
    "threshold": 80,
    "severity": "warning"
  }
}
```

---

## 🔧 ERROR CODES

| Code | Message | Description |
|------|---------|-------------|
| 400 | Invalid Request | Request không hợp lệ |
| 404 | System Not Found | System không tồn tại |
| 503 | System Unavailable | System không khả dụng |
| 500 | System Error | Lỗi hệ thống |

---

## 📝 EXAMPLES

### **Get System Status:**
```bash
curl -X GET http://localhost:8000/api/v1/system/status
```

### **Get Module Status:**
```bash
curl -X GET http://localhost:8000/api/v1/system/modules/status
```

### **Start Diagnostics:**
```bash
curl -X POST http://localhost:8000/api/v1/system/diagnostics/start \
  -H "Content-Type: application/json" \
  -d '{
    "diagnostic_type": "comprehensive",
    "modules": ["0x01", "0x02", "0x03"]
  }'
```

---

## 🔗 RELATED DOCUMENTS

- [Module Discovery API](./MODULE_DISCOVERY_API.md)
- [Module Specifications](../FIRMWARE/MODULE_SPECIFICATIONS.md)
- [Business Logic](../business/BUSINESS_LOGIC_SPEC.md)

---

**Status:** API Specification Complete  
**Version:** 1.0.0  
**Next Steps:** Implementation và testing
