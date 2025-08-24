# SAFETY MODULE API - OHT-50 Backend v2.0

## 📋 **Tổng quan**
API quản lý Safety Module (Address: 0x02) - E-Stop handling, safety zone management, emergency procedures, safety monitoring.

## 🔗 **Base URL**
```
http://localhost:8000/api/v1/safety
```

## 📊 **Endpoints**

### **1. Safety Status**

#### **GET /api/v1/safety/status**
Lấy trạng thái tổng quan của Safety Module.

**Response:**
```json
{
  "module_id": "0x02",
  "module_type": "safety",
  "status": "online",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "estop": {
      "status": "clear",
      "triggered_by": null,
      "trigger_time": null,
      "reset_time": "2025-01-28T10:25:00Z"
    },
    "safety_zones": {
      "zone_1": {
        "status": "clear",
        "type": "restricted",
        "violations": 0,
        "last_violation": null
      },
      "zone_2": {
        "status": "clear",
        "type": "warning",
        "violations": 0,
        "last_violation": null
      }
    },
    "speed_limits": {
      "current_speed": 500.0,
      "max_allowed": 1000.0,
      "zone_restriction": false
    },
    "system_health": {
      "estop_circuit": "healthy",
      "safety_sensors": "healthy",
      "communication": "healthy",
      "power_supply": "healthy"
    }
  }
}
```

### **2. E-Stop Management**

#### **GET /api/v1/safety/estop/status**
Lấy trạng thái E-Stop.

**Response:**
```json
{
  "estop_status": "clear",
  "triggered_by": null,
  "trigger_time": null,
  "reset_time": "2025-01-28T10:25:00Z",
  "reset_policy": "manual",
  "circuit_health": "healthy",
  "last_test": "2025-01-28T09:00:00Z"
}
```

#### **POST /api/v1/safety/estop/trigger**
Kích hoạt E-Stop (software trigger).

**Request:**
```json
{
  "reason": "manual_trigger",
  "source": "api",
  "emergency_level": "critical"
}
```

#### **POST /api/v1/safety/estop/reset**
Reset E-Stop (chỉ khi đã clear).

**Request:**
```json
{
  "reason": "manual_reset",
  "operator": "admin",
  "verification_code": "12345"
}
```

### **3. Safety Zone Management**

#### **GET /api/v1/safety/zones**
Lấy danh sách safety zones.

**Response:**
```json
{
  "zones": [
    {
      "id": "zone_1",
      "name": "Restricted Area A",
      "type": "restricted",
      "status": "clear",
      "boundaries": [
        {"x": 0, "y": 0},
        {"x": 100, "y": 0},
        {"x": 100, "y": 50},
        {"x": 0, "y": 50}
      ],
      "max_speed": 200.0,
      "violations": 0,
      "last_violation": null
    },
    {
      "id": "zone_2",
      "name": "Warning Area B",
      "type": "warning",
      "status": "clear",
      "boundaries": [
        {"x": 100, "y": 0},
        {"x": 200, "y": 0},
        {"x": 200, "y": 50},
        {"x": 100, "y": 50}
      ],
      "max_speed": 500.0,
      "violations": 0,
      "last_violation": null
    }
  ]
}
```

#### **POST /api/v1/safety/zones**
Tạo safety zone mới.

**Request:**
```json
{
  "id": "zone_3",
  "name": "New Restricted Zone",
  "type": "restricted",
  "boundaries": [
    {"x": 200, "y": 0},
    {"x": 300, "y": 0},
    {"x": 300, "y": 50},
    {"x": 200, "y": 50}
  ],
  "max_speed": 100.0,
  "action": "stop"
}
```

#### **PUT /api/v1/safety/zones/{zone_id}**
Cập nhật safety zone.

**Request:**
```json
{
  "name": "Updated Zone Name",
  "max_speed": 150.0,
  "action": "slow_down"
}
```

#### **DELETE /api/v1/safety/zones/{zone_id}**
Xóa safety zone.

### **4. Safety Configuration**

#### **GET /api/v1/safety/config**
Lấy cấu hình safety system.

**Response:**
```json
{
  "estop_config": {
    "enabled": true,
    "reset_policy": "manual",
    "auto_reset_timeout": 0,
    "circuit_test_interval": 3600
  },
  "speed_limits": {
    "default_max_speed": 1000.0,
    "restricted_zone_speed": 200.0,
    "warning_zone_speed": 500.0,
    "emergency_speed": 0.0
  },
  "safety_zones": {
    "max_zones": 10,
    "zone_update_rate": 10.0,
    "violation_threshold": 3
  },
  "monitoring": {
    "health_check_interval": 1.0,
    "alert_threshold": 5,
    "log_level": "info"
  }
}
```

#### **PUT /api/v1/safety/config**
Cập nhật cấu hình safety system.

**Request:**
```json
{
  "estop_config": {
    "reset_policy": "automatic",
    "auto_reset_timeout": 30
  },
  "speed_limits": {
    "default_max_speed": 1200.0,
    "restricted_zone_speed": 150.0
  },
  "monitoring": {
    "health_check_interval": 0.5,
    "alert_threshold": 3
  }
}
```

### **5. Safety Monitoring**

#### **GET /api/v1/safety/monitoring/health**
Lấy trạng thái health của safety system.

**Response:**
```json
{
  "overall_health": "healthy",
  "components": {
    "estop_circuit": {
      "status": "healthy",
      "last_test": "2025-01-28T09:00:00Z",
      "test_result": "pass"
    },
    "safety_sensors": {
      "status": "healthy",
      "sensor_count": 4,
      "active_sensors": 4
    },
    "communication": {
      "status": "healthy",
      "response_time": 0.05,
      "error_rate": 0.0
    },
    "power_supply": {
      "status": "healthy",
      "voltage": 24.2,
      "current": 0.5
    }
  },
  "last_update": "2025-01-28T10:30:00Z"
}
```

#### **GET /api/v1/safety/monitoring/violations**
Lấy lịch sử safety violations.

**Response:**
```json
{
  "violations": [
    {
      "id": "violation_001",
      "timestamp": "2025-01-28T10:15:00Z",
      "type": "speed_limit_exceeded",
      "zone_id": "zone_1",
      "details": {
        "current_speed": 250.0,
        "max_allowed": 200.0,
        "position": {"x": 50, "y": 25}
      },
      "action_taken": "speed_reduction",
      "resolved": true,
      "resolution_time": "2025-01-28T10:15:05Z"
    }
  ],
  "total_violations": 1,
  "unresolved_violations": 0
}
```

### **6. Emergency Procedures**

#### **GET /api/v1/safety/emergency/procedures**
Lấy danh sách emergency procedures.

**Response:**
```json
{
  "procedures": [
    {
      "id": "proc_001",
      "name": "Emergency Stop",
      "description": "Immediate system shutdown",
      "trigger_conditions": ["estop_pressed", "safety_violation"],
      "actions": [
        "stop_all_motors",
        "activate_brakes",
        "enable_safety_lights",
        "send_alert"
      ],
      "priority": "critical"
    },
    {
      "id": "proc_002",
      "name": "Speed Reduction",
      "description": "Reduce speed in restricted zones",
      "trigger_conditions": ["zone_violation"],
      "actions": [
        "reduce_speed",
        "send_warning",
        "log_violation"
      ],
      "priority": "warning"
    }
  ]
}
```

#### **POST /api/v1/safety/emergency/execute**
Thực thi emergency procedure.

**Request:**
```json
{
  "procedure_id": "proc_001",
  "reason": "manual_trigger",
  "operator": "admin"
}
```

### **7. Safety Testing**

#### **POST /api/v1/safety/test/estop**
Test E-Stop circuit.

**Request:**
```json
{
  "test_type": "circuit_test",
  "duration": 5.0
}
```

#### **POST /api/v1/safety/test/zones**
Test safety zones.

**Request:**
```json
{
  "test_type": "zone_test",
  "test_positions": [
    {"x": 50, "y": 25},
    {"x": 150, "y": 25}
  ]
}
```

#### **GET /api/v1/safety/test/results**
Lấy kết quả test gần nhất.

**Response:**
```json
{
  "last_test": "2025-01-28T09:00:00Z",
  "test_type": "full_system_test",
  "results": {
    "estop_circuit": "pass",
    "safety_zones": "pass",
    "speed_limits": "pass",
    "communication": "pass"
  },
  "overall_result": "pass",
  "duration": 30.5
}
```

## 🚨 **Error Codes**

| Code | Message | Description |
|------|---------|-------------|
| 400 | Invalid safety configuration | Cấu hình safety không hợp lệ |
| 404 | Safety zone not found | Safety zone không tồn tại |
| 409 | E-Stop active | E-Stop đang active, không thể thực hiện |
| 422 | Safety violation detected | Phát hiện vi phạm safety |
| 500 | Internal safety error | Lỗi nội bộ safety module |

## 📊 **WebSocket Events**

### **E-Stop Events**
```json
{
  "event": "estop_triggered",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "triggered_by": "hardware",
    "reason": "emergency_button",
    "emergency_level": "critical"
  }
}
```

### **Safety Zone Events**
```json
{
  "event": "zone_violation",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "zone_id": "zone_1",
    "violation_type": "speed_limit_exceeded",
    "current_speed": 250.0,
    "max_allowed": 200.0,
    "position": {"x": 50, "y": 25}
  }
}
```

### **Safety Health Events**
```json
{
  "event": "safety_health_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "overall_health": "healthy",
    "components": {
      "estop_circuit": "healthy",
      "safety_sensors": "healthy",
      "communication": "healthy",
      "power_supply": "healthy"
    }
  }
}
```

## 🧪 **Testing Examples**

### **Quick Test**
```bash
# Get safety status
curl http://localhost:8000/api/v1/safety/status

# Trigger E-Stop
curl -X POST http://localhost:8000/api/v1/safety/estop/trigger \
  -H "Content-Type: application/json" \
  -d '{"reason": "test_trigger", "source": "api"}'

# Reset E-Stop
curl -X POST http://localhost:8000/api/v1/safety/estop/reset \
  -H "Content-Type: application/json" \
  -d '{"reason": "test_reset", "operator": "admin"}'
```

### **Python Example**
```python
import requests

# Get safety status
response = requests.get("http://localhost:8000/api/v1/safety/status")
safety_status = response.json()

# Create safety zone
zone_config = {
    "id": "test_zone",
    "name": "Test Zone",
    "type": "restricted",
    "boundaries": [
        {"x": 0, "y": 0},
        {"x": 100, "y": 0},
        {"x": 100, "y": 100},
        {"x": 0, "y": 100}
    ],
    "max_speed": 200.0,
    "action": "stop"
}
response = requests.post(
    "http://localhost:8000/api/v1/safety/zones",
    json=zone_config
)

# Test E-Stop circuit
test_config = {
    "test_type": "circuit_test",
    "duration": 5.0
}
response = requests.post(
    "http://localhost:8000/api/v1/safety/test/estop",
    json=test_config
)
```

## 📝 **Notes**

- **24V System:** Safety module hoạt động với hệ thống 24V nominal
- **Real-time monitoring:** Sử dụng WebSocket để nhận safety events real-time
- **Safety first:** Tất cả operations đều có safety checks
- **E-Stop priority:** E-Stop có priority cao nhất, override tất cả operations
- **Zone management:** Support multiple safety zones với different types
- **Health monitoring:** Continuous health monitoring của tất cả safety components
- **Emergency procedures:** Predefined emergency procedures cho different scenarios

---

**Changelog v1.0:**
- ✅ Created comprehensive Safety Module API
- ✅ Added E-Stop management endpoints
- ✅ Added safety zone management
- ✅ Added safety configuration
- ✅ Added safety monitoring
- ✅ Added emergency procedures
- ✅ Added safety testing
- ✅ Standardized to 24V nominal voltage
- ✅ Added WebSocket events
- ✅ Added comprehensive examples
