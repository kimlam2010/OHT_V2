# MASTER CONTROL MODULE API - OHT-50 Backend v2.0

## 📋 **Tổng quan**
API quản lý Master Control Module (Address: 0x05) - System coordination, mission management, module orchestration, state machine control, system health monitoring.

## 🔗 **Base URL**
```
http://localhost:8000/api/v1/master-control
```

## 📊 **Endpoints**

### **1. System Status**

#### **GET /api/v1/master-control/status**
Lấy trạng thái tổng quan của Master Control Module và toàn bộ hệ thống.

**Response:**
```json
{
  "module_id": "0x05",
  "module_type": "master_control",
  "status": "online",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "system_state": {
      "current_state": "idle",
      "previous_state": "moving",
      "state_duration": 45.2,
      "state_transitions": 12
    },
    "modules": {
      "power": {
        "status": "online",
        "address": "0x01",
        "last_communication": "2025-01-28T10:29:55Z",
        "health": "healthy"
      },
      "safety": {
        "status": "online",
        "address": "0x02",
        "last_communication": "2025-01-28T10:29:58Z",
        "health": "healthy"
      },
      "travel_motor": {
        "status": "online",
        "address": "0x03",
        "last_communication": "2025-01-28T10:29:52Z",
        "health": "healthy"
      },
      "dock_location": {
        "status": "online",
        "address": "0x04",
        "last_communication": "2025-01-28T10:29:50Z",
        "health": "healthy"
      }
    },
    "mission": {
      "current_mission": "mission_001",
      "mission_status": "completed",
      "progress": 100.0,
      "estimated_completion": null
    },
    "system_health": {
      "overall_health": "healthy",
      "critical_alerts": 0,
      "warnings": 0,
      "last_health_check": "2025-01-28T10:30:00Z"
    }
  }
}
```

### **2. State Machine Management**

#### **GET /api/v1/master-control/state**
Lấy trạng thái state machine hiện tại.

**Response:**
```json
{
  "current_state": "idle",
  "state_info": {
    "name": "idle",
    "description": "System is idle, waiting for commands",
    "duration": 45.2,
    "entry_time": "2025-01-28T10:29:15Z",
    "allowed_transitions": ["moving", "docking", "fault", "estop"]
  },
  "state_history": [
    {
      "state": "moving",
      "entry_time": "2025-01-28T10:25:00Z",
      "exit_time": "2025-01-28T10:29:15Z",
      "duration": 255.0,
      "trigger": "mission_completed"
    },
    {
      "state": "docking",
      "entry_time": "2025-01-28T10:20:00Z",
      "exit_time": "2025-01-28T10:25:00Z",
      "duration": 300.0,
      "trigger": "docking_completed"
    }
  ],
  "state_machine_config": {
    "auto_transitions": true,
    "state_timeout": 300.0,
    "fault_recovery": true
  }
}
```

#### **POST /api/v1/master-control/state/transition**
Thực hiện state transition.

**Request:**
```json
{
  "target_state": "moving",
  "trigger": "manual_command",
  "parameters": {
    "mission_id": "mission_002",
    "target_position": {"x": 2000.0, "y": 1500.0}
  }
}
```

**Response:**
```json
{
  "transition_id": "trans_001",
  "status": "success",
  "from_state": "idle",
  "to_state": "moving",
  "trigger": "manual_command",
  "timestamp": "2025-01-28T10:30:05Z",
  "validation_passed": true
}
```

#### **GET /api/v1/master-control/state/transitions**
Lấy danh sách state transitions có thể thực hiện.

**Response:**
```json
{
  "current_state": "idle",
  "available_transitions": [
    {
      "target_state": "moving",
      "trigger": "mission_start",
      "conditions": ["all_modules_healthy", "battery_sufficient"],
      "description": "Start mission execution"
    },
    {
      "target_state": "docking",
      "trigger": "docking_request",
      "conditions": ["dock_available", "position_near_dock"],
      "description": "Start docking procedure"
    },
    {
      "target_state": "fault",
      "trigger": "system_error",
      "conditions": ["error_detected"],
      "description": "Enter fault state"
    },
    {
      "target_state": "estop",
      "trigger": "emergency_stop",
      "conditions": ["estop_triggered"],
      "description": "Emergency stop"
    }
  ]
}
```

### **3. Mission Management**

#### **GET /api/v1/master-control/missions**
Lấy danh sách missions.

**Response:**
```json
{
  "missions": [
    {
      "id": "mission_001",
      "name": "Transport to Loading Station",
      "type": "transport",
      "status": "completed",
      "priority": "high",
      "created_at": "2025-01-28T10:00:00Z",
      "started_at": "2025-01-28T10:05:00Z",
      "completed_at": "2025-01-28T10:25:00Z",
      "duration": 1200.0,
      "waypoints": [
        {"x": 1000.0, "y": 500.0, "action": "pickup"},
        {"x": 2000.0, "y": 1500.0, "action": "delivery"}
      ]
    },
    {
      "id": "mission_002",
      "name": "Return to Charging Station",
      "type": "return",
      "status": "pending",
      "priority": "medium",
      "created_at": "2025-01-28T10:30:00Z",
      "waypoints": [
        {"x": 1250.5, "y": 850.2, "action": "start"},
        {"x": 2000.0, "y": 1500.0, "action": "dock"}
      ]
    }
  ],
  "current_mission": "mission_002",
  "pending_missions": 1,
  "completed_missions": 1
}
```

#### **POST /api/v1/master-control/missions**
Tạo mission mới.

**Request:**
```json
{
  "name": "New Transport Mission",
  "type": "transport",
  "priority": "high",
  "waypoints": [
    {
      "x": 1250.5,
      "y": 850.2,
      "action": "start",
      "speed": 1000.0
    },
    {
      "x": 2000.0,
      "y": 1500.0,
      "action": "pickup",
      "speed": 500.0
    },
    {
      "x": 3000.0,
      "y": 2000.0,
      "action": "delivery",
      "speed": 500.0
    }
  ],
  "constraints": {
    "max_speed": 1000.0,
    "safety_margin": 100.0,
    "battery_threshold": 20.0
  }
}
```

**Response:**
```json
{
  "mission_id": "mission_003",
  "status": "created",
  "estimated_duration": 180.0,
  "validation": {
    "path_feasible": true,
    "battery_sufficient": true,
    "safety_approved": true
  }
}
```

#### **GET /api/v1/master-control/missions/{mission_id}**
Lấy chi tiết mission.

**Response:**
```json
{
  "id": "mission_002",
  "name": "Return to Charging Station",
  "type": "return",
  "status": "in_progress",
  "progress": 0.35,
  "current_waypoint": 1,
  "waypoints": [
    {
      "id": "wp_001",
      "x": 1250.5,
      "y": 850.2,
      "action": "start",
      "status": "completed",
      "completion_time": "2025-01-28T10:30:05Z"
    },
    {
      "id": "wp_002",
      "x": 2000.0,
      "y": 1500.0,
      "action": "dock",
      "status": "in_progress",
      "estimated_completion": "2025-01-28T10:32:00Z"
    }
  ],
  "metrics": {
    "distance_traveled": 550.5,
    "time_elapsed": 65.0,
    "battery_consumed": 5.2,
    "obstacles_avoided": 2
  }
}
```

#### **POST /api/v1/master-control/missions/{mission_id}/start**
Bắt đầu mission.

**Request:**
```json
{
  "operator": "admin",
  "verification": "mission_approved"
}
```

#### **POST /api/v1/master-control/missions/{mission_id}/pause**
Tạm dừng mission.

**Request:**
```json
{
  "reason": "manual_pause",
  "operator": "admin"
}
```

#### **POST /api/v1/master-control/missions/{mission_id}/resume**
Tiếp tục mission.

**Request:**
```json
{
  "operator": "admin"
}
```

#### **POST /api/v1/master-control/missions/{mission_id}/cancel**
Hủy mission.

**Request:**
```json
{
  "reason": "manual_cancel",
  "operator": "admin"
}
```

### **4. Module Coordination**

#### **GET /api/v1/master-control/modules**
Lấy trạng thái tất cả modules.

**Response:**
```json
{
  "modules": {
    "power": {
      "address": "0x01",
      "status": "online",
      "health": "healthy",
      "last_communication": "2025-01-28T10:29:55Z",
      "response_time": 0.05,
      "error_count": 0,
      "data": {
        "battery_voltage": 24.2,
        "battery_soc": 85,
        "charging_status": "not_charging"
      }
    },
    "safety": {
      "address": "0x02",
      "status": "online",
      "health": "healthy",
      "last_communication": "2025-01-28T10:29:58Z",
      "response_time": 0.03,
      "error_count": 0,
      "data": {
        "estop_status": "clear",
        "safety_zones": "clear",
        "speed_limits": "normal"
      }
    },
    "travel_motor": {
      "address": "0x03",
      "status": "online",
      "health": "healthy",
      "last_communication": "2025-01-28T10:29:52Z",
      "response_time": 0.04,
      "error_count": 0,
      "data": {
        "speed": 0.0,
        "direction": 0.0,
        "motor_status": "stopped"
      }
    },
    "dock_location": {
      "address": "0x04",
      "status": "online",
      "health": "healthy",
      "last_communication": "2025-01-28T10:29:50Z",
      "response_time": 0.06,
      "error_count": 0,
      "data": {
        "position": {"x": 1250.5, "y": 850.2},
        "navigation_status": "idle",
        "docking_status": "not_docking"
      }
    }
  },
  "communication_stats": {
    "total_modules": 4,
    "online_modules": 4,
    "offline_modules": 0,
    "average_response_time": 0.045,
    "total_errors": 0
  }
}
```

#### **POST /api/v1/master-control/modules/{module_id}/command**
Gửi command đến module cụ thể.

**Request:**
```json
{
  "command": "set_speed",
  "parameters": {
    "speed": 500.0,
    "direction": 45.0
  },
  "priority": "normal",
  "timeout": 5.0
}
```

**Response:**
```json
{
  "command_id": "cmd_001",
  "status": "sent",
  "module_address": "0x03",
  "command": "set_speed",
  "timestamp": "2025-01-28T10:30:10Z",
  "response": {
    "status": "success",
    "data": {
      "speed_set": 500.0,
      "direction_set": 45.0
    }
  }
}
```

#### **GET /api/v1/master-control/modules/{module_id}/health**
Lấy health status của module.

**Response:**
```json
{
  "module_address": "0x03",
  "module_type": "travel_motor",
  "health_status": "healthy",
  "last_check": "2025-01-28T10:30:00Z",
  "metrics": {
    "response_time": 0.04,
    "error_rate": 0.0,
    "uptime": 86400.0,
    "communication_quality": 0.99
  },
  "alerts": [],
  "recommendations": []
}
```

### **5. System Health Monitoring**

#### **GET /api/v1/master-control/health**
Lấy tổng quan system health.

**Response:**
```json
{
  "overall_health": "healthy",
  "health_score": 0.95,
  "last_check": "2025-01-28T10:30:00Z",
  "components": {
    "hardware": {
      "status": "healthy",
      "score": 0.98,
      "issues": []
    },
    "software": {
      "status": "healthy",
      "score": 0.95,
      "issues": []
    },
    "communication": {
      "status": "healthy",
      "score": 0.92,
      "issues": []
    },
    "safety": {
      "status": "healthy",
      "score": 1.0,
      "issues": []
    }
  },
  "alerts": {
    "critical": 0,
    "warning": 0,
    "info": 2
  },
  "performance_metrics": {
    "cpu_usage": 25.5,
    "memory_usage": 45.2,
    "disk_usage": 30.1,
    "network_latency": 0.05
  }
}
```

#### **GET /api/v1/master-control/health/detailed**
Lấy detailed health report.

**Response:**
```json
{
  "system_health": {
    "overall_status": "healthy",
    "health_score": 0.95,
    "last_comprehensive_check": "2025-01-28T10:30:00Z"
  },
  "module_health": {
    "power": {
      "status": "healthy",
      "battery_health": 0.85,
      "charging_system": "healthy",
      "power_distribution": "healthy"
    },
    "safety": {
      "status": "healthy",
      "estop_circuit": "healthy",
      "safety_sensors": "healthy",
      "emergency_systems": "healthy"
    },
    "travel_motor": {
      "status": "healthy",
      "motor_health": 0.92,
      "controller_health": 0.95,
      "sensor_health": 0.88
    },
    "dock_location": {
      "status": "healthy",
      "lidar_health": 0.90,
      "rfid_health": 0.95,
      "imu_health": 0.93
    }
  },
  "communication_health": {
    "rs485_bus": {
      "status": "healthy",
      "error_rate": 0.001,
      "response_time": 0.045,
      "bus_load": 0.25
    },
    "ethernet": {
      "status": "healthy",
      "bandwidth_usage": 0.15,
      "packet_loss": 0.0
    }
  },
  "environmental_health": {
    "temperature": {
      "current": 25.5,
      "status": "normal",
      "range": [15.0, 35.0]
    },
    "humidity": {
      "current": 45.2,
      "status": "normal",
      "range": [20.0, 80.0]
    }
  }
}
```

### **6. System Configuration**

#### **GET /api/v1/master-control/config**
Lấy system configuration.

**Response:**
```json
{
  "system_config": {
    "robot_id": "OHT-50-001",
    "firmware_version": "2.0.0",
    "hardware_version": "1.0.0",
    "serial_number": "SN2025001"
  },
  "state_machine_config": {
    "auto_transitions": true,
    "state_timeout": 300.0,
    "fault_recovery": true,
    "estop_priority": "highest"
  },
  "mission_config": {
    "max_concurrent_missions": 1,
    "mission_timeout": 3600.0,
    "retry_attempts": 3,
    "battery_threshold": 20.0
  },
  "communication_config": {
    "rs485": {
      "baudrate": 115200,
      "timeout": 1.0,
      "retry_attempts": 3
    },
    "ethernet": {
      "ip_address": "192.168.1.100",
      "port": 8000,
      "heartbeat_interval": 1.0
    }
  },
  "safety_config": {
    "estop_enabled": true,
    "safety_zones_enabled": true,
    "speed_limits_enabled": true,
    "emergency_procedures": "enabled"
  }
}
```

#### **PUT /api/v1/master-control/config**
Cập nhật system configuration.

**Request:**
```json
{
  "mission_config": {
    "max_concurrent_missions": 2,
    "battery_threshold": 15.0
  },
  "safety_config": {
    "speed_limits_enabled": true
  }
}
```

### **7. System Control**

#### **POST /api/v1/master-control/system/startup**
Thực hiện system startup sequence.

**Request:**
```json
{
  "mode": "normal",
  "skip_self_test": false,
  "operator": "admin"
}
```

#### **POST /api/v1/master-control/system/shutdown**
Thực hiện system shutdown sequence.

**Request:**
```json
{
  "mode": "graceful",
  "timeout": 30.0,
  "operator": "admin"
}
```

#### **POST /api/v1/master-control/system/restart**
Restart system.

**Request:**
```json
{
  "mode": "soft",
  "operator": "admin"
}
```

#### **POST /api/v1/master-control/system/emergency_stop**
Emergency stop toàn bộ hệ thống.

**Request:**
```json
{
  "reason": "emergency_stop",
  "operator": "admin",
  "level": "critical"
}
```

## 🚨 **Error Codes**

| Code | Message | Description |
|------|---------|-------------|
| 400 | Invalid state transition | State transition không hợp lệ |
| 404 | Mission not found | Mission không tồn tại |
| 409 | System busy | Hệ thống đang bận |
| 422 | Mission validation failed | Mission validation thất bại |
| 500 | System error | Lỗi hệ thống |

## 📊 **WebSocket Events**

### **State Machine Events**
```json
{
  "event": "state_transition",
  "timestamp": "2025-01-28T10:30:05Z",
  "data": {
    "from_state": "idle",
    "to_state": "moving",
    "trigger": "mission_start",
    "transition_id": "trans_001"
  }
}
```

### **Mission Events**
```json
{
  "event": "mission_progress",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "mission_id": "mission_002",
    "progress": 0.35,
    "current_waypoint": 1,
    "status": "in_progress"
  }
}
```

### **System Health Events**
```json
{
  "event": "system_health_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "overall_health": "healthy",
    "health_score": 0.95,
    "alerts": {"critical": 0, "warning": 0, "info": 2}
  }
}
```

### **Module Communication Events**
```json
{
  "event": "module_communication",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "module_address": "0x03",
    "status": "online",
    "response_time": 0.04,
    "error_count": 0
  }
}
```

## 🧪 **Testing Examples**

### **Quick Test**
```bash
# Get system status
curl http://localhost:8000/api/v1/master-control/status

# Get current state
curl http://localhost:8000/api/v1/master-control/state

# Create mission
curl -X POST http://localhost:8000/api/v1/master-control/missions \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Test Mission",
    "type": "transport",
    "waypoints": [
      {"x": 1000.0, "y": 500.0, "action": "start"},
      {"x": 2000.0, "y": 1500.0, "action": "delivery"}
    ]
  }'

# Get system health
curl http://localhost:8000/api/v1/master-control/health
```

### **Python Example**
```python
import requests

# Get system status
response = requests.get("http://localhost:8000/api/v1/master-control/status")
system_status = response.json()

# Create mission
mission_data = {
    "name": "Transport Mission",
    "type": "transport",
    "priority": "high",
    "waypoints": [
        {"x": 1250.5, "y": 850.2, "action": "start"},
        {"x": 2000.0, "y": 1500.0, "action": "delivery"}
    ]
}
response = requests.post(
    "http://localhost:8000/api/v1/master-control/missions",
    json=mission_data
)

# Start mission
mission_id = response.json()["mission_id"]
response = requests.post(
    f"http://localhost:8000/api/v1/master-control/missions/{mission_id}/start",
    json={"operator": "admin"}
)

# Get module health
response = requests.get("http://localhost:8000/api/v1/master-control/modules")
modules = response.json()

# Send command to module
command_data = {
    "command": "set_speed",
    "parameters": {"speed": 500.0, "direction": 45.0}
}
response = requests.post(
    "http://localhost:8000/api/v1/master-control/modules/0x03/command",
    json=command_data
)
```

## 📝 **Notes**

- **System Coordination:** Master Control Module điều phối tất cả modules
- **State Machine:** Quản lý state transitions và system states
- **Mission Management:** Tạo, quản lý và thực thi missions
- **Module Communication:** Giao tiếp với tất cả modules qua RS485
- **Health Monitoring:** Continuous monitoring system health
- **Safety Integration:** Tích hợp với safety system và emergency procedures
- **24V System:** Module hoạt động với hệ thống 24V nominal
- **Real-time Control:** Real-time coordination và control

---

**Changelog v1.0:**
- ✅ Created comprehensive Master Control Module API
- ✅ Added state machine management
- ✅ Added mission management
- ✅ Added module coordination
- ✅ Added system health monitoring
- ✅ Added system configuration
- ✅ Added system control
- ✅ Standardized to 24V nominal voltage
- ✅ Added WebSocket events
- ✅ Added comprehensive examples
