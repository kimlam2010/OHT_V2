# TELEMETRY API - OHT-50 Backend v2.0

## 📋 **Tổng quan**
API telemetry cho OHT-50 Backend system - Architecture v2.0 với 5 mandatory modules, real-time data streaming, module-specific telemetry.

## 🔗 **Base URL**
```
http://localhost:8000/api/v1/telemetry
```

## 📊 **Architecture v2.0 Features**
- **5 Mandatory Modules:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **RS485 Communication:** Real-time module communication status
- **LiDAR USB Integration:** RPLIDAR A1M8 scan data streaming
- **24V System:** Power telemetry với 24V nominal voltage
- **Safety Integration:** E-Stop, safety zones, emergency events
- **Mission Management:** Real-time mission progress tracking
- **State Machine:** System state transitions và monitoring

## 📊 **Endpoints**

### **1. System Telemetry**

#### **GET /api/v1/telemetry/current**
Lấy snapshot telemetry hiện tại của toàn bộ hệ thống.

**Response:**
```json
{
  "success": true,
  "data": {
    "timestamp": "2025-01-28T10:30:00Z",
    "system_state": {
      "current_state": "moving",
      "state_duration": 45.2,
      "previous_state": "idle",
      "state_transitions": 12
    },
    "modules": {
      "power": {
        "status": "online",
        "address": "0x01",
        "battery": {
          "voltage": 24.2,
          "current": 2.5,
          "soc": 85,
          "temperature": 25.5,
          "capacity": 50,
          "cycle_count": 150
        },
        "charging": {
          "status": "not_charging",
          "mode": "idle",
          "current": 0.0,
          "voltage": 24.2
        },
        "power_distribution": {
          "5v": {"voltage": 5.02, "current": 1.2, "status": "normal"},
          "12v": {"voltage": 12.05, "current": 3.5, "status": "normal"},
          "24v": {"voltage": 24.1, "current": 2.0, "status": "normal"}
        }
      },
      "safety": {
        "status": "online",
        "address": "0x02",
        "estop": {
          "status": "clear",
          "triggered_by": null,
          "trigger_time": null
        },
        "safety_zones": {
          "zone_1": {"status": "clear", "violations": 0},
          "zone_2": {"status": "clear", "violations": 0}
        },
        "speed_limits": {
          "current_speed": 500.0,
          "max_allowed": 1000.0,
          "zone_restriction": false
        }
      },
      "travel_motor": {
        "status": "online",
        "address": "0x03",
        "motor": {
          "speed": 500.0,
          "direction": 45.0,
          "current": 2.1,
          "temperature": 35.2,
          "status": "running"
        },
        "encoder": {
          "count": 123456,
          "speed": 500.0,
          "position": 1250.5,
          "health": "healthy"
        },
        "controller": {
          "pid_status": "active",
          "target_speed": 500.0,
          "error": 0.0,
          "output": 75.0
        }
      },
      "dock_location": {
        "status": "online",
        "address": "0x04",
        "position": {
          "x": 1250.5,
          "y": 850.2,
          "z": 0.0,
          "accuracy": 2.0,
          "confidence": 0.95
        },
        "orientation": {
          "yaw": 45.2,
          "pitch": 0.1,
          "roll": 0.0,
          "accuracy": 1.5
        },
        "navigation": {
          "mode": "autonomous",
          "target": "dock_01",
          "distance_to_target": 150.5,
          "estimated_arrival": "2025-01-28T10:32:00Z"
        },
        "sensors": {
          "lidar": {
            "status": "active",
            "scan_frequency": 5.5,
            "points_per_scan": 360,
            "last_scan": "2025-01-28T10:30:00Z"
          },
          "rfid": {
            "status": "active",
            "tags_detected": 3,
            "last_read": "2025-01-28T10:29:55Z"
          },
          "imu": {
            "status": "active",
            "calibration": "calibrated",
            "drift_rate": 0.1
          }
        }
      },
      "master_control": {
        "status": "online",
        "address": "0x05",
        "mission": {
          "current_mission": "mission_002",
          "mission_status": "in_progress",
          "progress": 0.35,
          "estimated_completion": "2025-01-28T10:32:00Z"
        },
        "coordination": {
          "modules_online": 4,
          "modules_offline": 0,
          "average_response_time": 0.045,
          "total_errors": 0
        }
      }
    },
    "communication": {
      "rs485": {
        "status": "healthy",
        "baudrate": 115200,
        "error_rate": 0.001,
        "response_time": 0.045,
        "bus_load": 0.25
      },
      "ethernet": {
        "status": "connected",
        "ip_address": "192.168.1.100",
        "bandwidth_usage": 0.15,
        "packet_loss": 0.0
      },
      "center_connected": true
    },
    "system_health": {
      "overall_health": "healthy",
      "health_score": 0.95,
      "critical_alerts": 0,
      "warnings": 0,
      "performance": {
        "cpu_usage": 25.5,
        "memory_usage": 45.2,
        "disk_usage": 30.1,
        "network_latency": 0.05
      }
    }
  }
}
```

#### **GET /api/v1/telemetry/history**
Lấy lịch sử telemetry data.

**Parameters:**
- `limit` (int, 1-1000, default: 100): Số lượng records
- `module` (string, optional): Filter theo module (power, safety, travel_motor, dock_location, master_control)
- `start_time` (ISO datetime, optional): Thời gian bắt đầu
- `end_time` (ISO datetime, optional): Thời gian kết thúc

**Response:**
```json
{
  "success": true,
  "data": {
    "total_records": 100,
    "records": [
      {
        "timestamp": "2025-01-28T10:29:00Z",
        "system_state": {
          "current_state": "idle",
          "state_duration": 15.2
        },
        "modules": {
          "power": {
            "battery": {
              "voltage": 24.1,
              "soc": 84,
              "temperature": 25.2
            }
          },
          "safety": {
            "estop": {"status": "clear"},
            "safety_zones": {"zone_1": {"status": "clear"}}
          },
          "travel_motor": {
            "motor": {
              "speed": 0.0,
              "status": "stopped"
            }
          },
          "dock_location": {
            "position": {
              "x": 1250.0,
              "y": 850.0
            }
          },
          "master_control": {
            "mission": {
              "current_mission": "mission_002",
              "progress": 0.30
            }
          }
        }
      }
    ]
  }
}
```

### **2. Module-Specific Telemetry**

#### **GET /api/v1/telemetry/modules/{module_id}**
Lấy telemetry data cho module cụ thể.

**Parameters:**
- `module_id` (string): power, safety, travel_motor, dock_location, master_control

**Response (Power Module Example):**
```json
{
  "success": true,
  "data": {
    "module_id": "power",
    "module_address": "0x01",
    "timestamp": "2025-01-28T10:30:00Z",
    "status": "online",
    "telemetry": {
      "battery": {
        "voltage": 24.2,
        "current": 2.5,
        "soc": 85,
        "temperature": 25.5,
        "capacity": 50,
        "cycle_count": 150,
        "health": 0.85
      },
      "charging": {
        "status": "not_charging",
        "mode": "idle",
        "current": 0.0,
        "voltage": 24.2,
        "temperature": 25.5
      },
      "power_distribution": {
        "5v": {
          "voltage": 5.02,
          "current": 1.2,
          "status": "normal",
          "efficiency": 0.92
        },
        "12v": {
          "voltage": 12.05,
          "current": 3.5,
          "status": "normal",
          "efficiency": 0.89
        },
        "24v": {
          "voltage": 24.1,
          "current": 2.0,
          "status": "normal",
          "efficiency": 0.95
        }
      },
      "protection": {
        "overvoltage": false,
        "overcurrent": false,
        "overtemperature": false,
        "undervoltage": false
      }
    }
  }
}
```

#### **GET /api/v1/telemetry/modules/{module_id}/history**
Lấy lịch sử telemetry cho module cụ thể.

**Parameters:**
- `module_id` (string): Module identifier
- `limit` (int, 1-1000, default: 100): Số lượng records
- `start_time` (ISO datetime, optional): Thời gian bắt đầu
- `end_time` (ISO datetime, optional): Thời gian kết thúc

### **3. Safety Telemetry**

#### **GET /api/v1/telemetry/safety/events**
Lấy safety events và violations.

**Parameters:**
- `limit` (int, 1-1000, default: 100): Số lượng events
- `event_type` (string, optional): estop, zone_violation, speed_violation
- `start_time` (ISO datetime, optional): Thời gian bắt đầu
- `end_time` (ISO datetime, optional): Thời gian kết thúc

**Response:**
```json
{
  "success": true,
  "data": {
    "total_events": 5,
    "events": [
      {
        "event_id": "safety_001",
        "timestamp": "2025-01-28T10:15:00Z",
        "event_type": "zone_violation",
        "severity": "warning",
        "details": {
          "zone_id": "zone_1",
          "violation_type": "speed_limit_exceeded",
          "current_speed": 250.0,
          "max_allowed": 200.0,
          "position": {"x": 50, "y": 25}
        },
        "action_taken": "speed_reduction",
        "resolved": true,
        "resolution_time": "2025-01-28T10:15:05Z"
      }
    ]
  }
}
```

### **4. Location & Navigation Telemetry**

#### **GET /api/v1/telemetry/location/track**
Lấy location tracking data.

**Parameters:**
- `limit` (int, 1-1000, default: 100): Số lượng positions
- `start_time` (ISO datetime, optional): Thời gian bắt đầu
- `end_time` (ISO datetime, optional): Thời gian kết thúc

**Response:**
```json
{
  "success": true,
  "data": {
    "total_positions": 100,
    "positions": [
      {
        "timestamp": "2025-01-28T10:30:00Z",
        "position": {
          "x": 1250.5,
          "y": 850.2,
          "z": 0.0,
          "accuracy": 2.0,
          "confidence": 0.95
        },
        "orientation": {
          "yaw": 45.2,
          "pitch": 0.1,
          "roll": 0.0,
          "accuracy": 1.5
        },
        "velocity": {
          "linear": 0.5,
          "angular": 0.1,
          "direction": 45.2
        },
        "sources": {
          "primary": "lidar_slam",
          "secondary": "rfid",
          "backup": "imu"
        }
      }
    ]
  }
}
```

#### **GET /api/v1/telemetry/lidar/scan**
Lấy LiDAR scan data gần nhất.

**Response:**
```json
{
  "success": true,
  "data": {
    "scan_id": "scan_001",
    "timestamp": "2025-01-28T10:30:00Z",
    "sensor_info": {
      "model": "RPLIDAR_A1M8",
      "interface": "USB_2.0",
      "scan_frequency": 5.5,
      "points_per_scan": 360
    },
    "scan_data": {
      "points_count": 360,
      "obstacles_detected": 1,
      "map_updated": true,
      "quality_score": 0.95
    },
    "obstacles": [
      {
        "id": "obs_001",
        "position": {"x": 1300.0, "y": 900.0},
        "distance": 150.0,
        "angle": 45.0,
        "size": "small",
        "confidence": 0.92
      }
    ]
  }
}
```

### **5. Mission Telemetry**

#### **GET /api/v1/telemetry/missions/{mission_id}**
Lấy telemetry data cho mission cụ thể.

**Response:**
```json
{
  "success": true,
  "data": {
    "mission_id": "mission_002",
    "mission_name": "Transport to Loading Station",
    "status": "in_progress",
    "progress": 0.35,
    "start_time": "2025-01-28T10:25:00Z",
    "estimated_completion": "2025-01-28T10:32:00Z",
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
      "obstacles_avoided": 2,
      "safety_violations": 0
    }
  }
}
```

### **6. WebSocket Streaming**

#### **WebSocket /api/v1/telemetry/ws**
Stream telemetry data theo thời gian thực.

**Connection:**
```
ws://localhost:8000/api/v1/telemetry/ws
```

**Message Format:** JSON với event type và data

**Event Types:**
- `system_update`: System state và overall status
- `module_update`: Module-specific updates
- `safety_event`: Safety events và violations
- `location_update`: Position và navigation updates
- `mission_update`: Mission progress updates
- `lidar_scan`: LiDAR scan data
- `error`: Error events

**Example Messages:**

**System Update:**
```json
{
  "event": "system_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "system_state": {
      "current_state": "moving",
      "state_duration": 45.2,
      "previous_state": "idle"
    },
    "system_health": {
      "overall_health": "healthy",
      "health_score": 0.95,
      "critical_alerts": 0,
      "warnings": 0
    },
    "communication": {
      "rs485": {"status": "healthy", "error_rate": 0.001},
      "ethernet": {"status": "connected", "packet_loss": 0.0}
    }
  }
}
```

**Module Update (Power):**
```json
{
  "event": "module_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "module_id": "power",
    "module_address": "0x01",
    "status": "online",
    "telemetry": {
      "battery": {
        "voltage": 24.2,
        "soc": 85,
        "temperature": 25.5
      },
      "charging": {
        "status": "not_charging",
        "current": 0.0
      }
    }
  }
}
```

**Safety Event:**
```json
{
  "event": "safety_event",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "event_type": "zone_violation",
    "severity": "warning",
    "details": {
      "zone_id": "zone_1",
      "violation_type": "speed_limit_exceeded",
      "current_speed": 250.0,
      "max_allowed": 200.0,
      "position": {"x": 50, "y": 25}
    },
    "action_taken": "speed_reduction"
  }
}
```

**Location Update:**
```json
{
  "event": "location_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "position": {
      "x": 1250.5,
      "y": 850.2,
      "z": 0.0,
      "accuracy": 2.0,
      "confidence": 0.95
    },
    "orientation": {
      "yaw": 45.2,
      "pitch": 0.1,
      "roll": 0.0
    },
    "velocity": {
      "linear": 0.5,
      "angular": 0.1,
      "direction": 45.2
    },
    "sources": {
      "primary": "lidar_slam",
      "secondary": "rfid"
    }
  }
}
```

**Mission Update:**
```json
{
  "event": "mission_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "mission_id": "mission_002",
    "status": "in_progress",
    "progress": 0.35,
    "current_waypoint": 2,
    "distance_remaining": 550.5,
    "estimated_arrival": "2025-01-28T10:32:00Z"
  }
}
```

**LiDAR Scan:**
```json
{
  "event": "lidar_scan",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "scan_id": "scan_001",
    "points_count": 360,
    "obstacles_detected": 1,
    "map_updated": true,
    "quality_score": 0.95,
    "obstacles": [
      {
        "id": "obs_001",
        "position": {"x": 1300.0, "y": 900.0},
        "distance": 150.0,
        "angle": 45.0,
        "size": "small"
      }
    ]
  }
}
```
    "pos_mm": 1236.00,
    "vel_mms": 200.0,
    "acc_mms2": 150.0,
    "target": { "pos_mm": 1500.0, "vel_mms": 400.0 },
    "safety": { "estop": false, "zone_blocked": false, "interlock_active": false },
    "enc": { "health": "ok", "fault": null, "count": 123470 },
    "errors": [],
    "warn": []
  },
  "center_connected": true,
  "rs485_connected": true
}
```

## 4) POST /api/v1/telemetry/rate?hz={value}
- Chức năng: Cài đặt tần số phát (WS) thời gian thực
- Tham số: `hz` (float, 0.1..50.0; mặc định 10.0)
- Trả về: `{ success: true, hz }`

Ví dụ:
```http
POST /api/v1/telemetry/rate?hz=20
```
```json
{ "success": true, "hz": 20.0 }
```

## 5) POST /api/v1/telemetry/reset
- Chức năng: Reset trạng thái mock và xoá lịch sử
- Trả về: `{ success: true }`

Ví dụ:
```http
POST /api/v1/telemetry/reset
```
```json
{ "success": true }
```

## 6) GET /api/v1/telemetry/stats
- Chức năng: Thống kê nhanh lịch sử gần nhất trong bộ nhớ
- Trả về: `{ success: true, data: { count, last_ts, vel_min, vel_max, vel_avg } }`

Ví dụ:
```json
{
  "success": true,
  "data": {
    "count": 128,
    "last_ts": 1712345679.999,
    "vel_min": -200.0,
    "vel_max": 200.0,
    "vel_avg": 12.34
  }
}
```

### **7. Configuration**

#### **POST /api/v1/telemetry/rate**
Thay đổi tần số phát WebSocket.

**Request:**
```json
{
  "ws_hz": 5.0
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "ws_hz": 5.0,
    "message": "WebSocket rate updated successfully"
  }
}
```

#### **GET /api/v1/telemetry/rate**
Lấy tần số phát WebSocket hiện tại.

**Response:**
```json
{
  "success": true,
  "data": {
    "ws_hz": 5.0,
    "min_rate": 0.1,
    "max_rate": 50.0
  }
}
```

#### **POST /api/v1/telemetry/config**
Cập nhật telemetry configuration.

**Request:**
```json
{
  "streaming": {
    "enabled": true,
    "rate": 10.0,
    "modules": ["power", "safety", "travel_motor", "dock_location", "master_control"]
  },
  "storage": {
    "enabled": true,
    "retention_days": 30,
    "max_records": 10000
  },
  "filters": {
    "min_accuracy": 2.0,
    "min_confidence": 0.8,
    "exclude_events": ["debug", "info"]
  }
}
```

## 🚨 **Error Codes**

| Code | Message | Description |
|------|---------|-------------|
| 400 | Invalid telemetry parameters | Tham số telemetry không hợp lệ |
| 404 | Module not found | Module không tồn tại |
| 422 | Invalid time range | Khoảng thời gian không hợp lệ |
| 500 | Telemetry service error | Lỗi service telemetry |

## 🧪 **Testing Examples**

### **Quick Test**
```bash
# Get current telemetry
curl http://localhost:8000/api/v1/telemetry/current

# Get module telemetry
curl http://localhost:8000/api/v1/telemetry/modules/power

# Get safety events
curl http://localhost:8000/api/v1/telemetry/safety/events

# Get location tracking
curl http://localhost:8000/api/v1/telemetry/location/track

# Get mission telemetry
curl http://localhost:8000/api/v1/telemetry/missions/mission_002

# Update WebSocket rate
curl -X POST http://localhost:8000/api/v1/telemetry/rate \
  -H "Content-Type: application/json" \
  -d '{"ws_hz": 5.0}'
```

### **Python Example**
```python
import requests
import websocket
import json

# Get current telemetry
response = requests.get("http://localhost:8000/api/v1/telemetry/current")
telemetry = response.json()

# Get module-specific telemetry
response = requests.get("http://localhost:8000/api/v1/telemetry/modules/power")
power_telemetry = response.json()

# Get safety events
response = requests.get("http://localhost:8000/api/v1/telemetry/safety/events")
safety_events = response.json()

# WebSocket connection
def on_message(ws, message):
    data = json.loads(message)
    print(f"Event: {data['event']}")
    print(f"Data: {data['data']}")

def on_error(ws, error):
    print(f"Error: {error}")

def on_close(ws, close_status_code, close_msg):
    print("WebSocket connection closed")

def on_open(ws):
    print("WebSocket connection opened")

# Connect to WebSocket
ws = websocket.WebSocketApp(
    "ws://localhost:8000/api/v1/telemetry/ws",
    on_open=on_open,
    on_message=on_message,
    on_error=on_error,
    on_close=on_close
)

ws.run_forever()
```

## 📝 **Notes**

- **Real-time Streaming:** WebSocket events cho real-time updates
- **Module-specific Data:** Telemetry data cho từng module riêng biệt
- **Safety Integration:** Safety events và violations tracking
- **Location Tracking:** Position và navigation data
- **Mission Progress:** Real-time mission tracking
- **LiDAR Integration:** LiDAR scan data streaming
- **24V System:** Power telemetry với 24V nominal voltage
- **RS485 Communication:** Module communication status
- **Performance Monitoring:** System health và performance metrics

## 🔒 **Security & Logging**
- Mỗi request/response cần ghi log có `correlation_id`
- Áp dụng rate-limit cho internet access
- Authentication required cho sensitive endpoints
- Data encryption cho sensitive telemetry data

---

**Changelog v2.0:**
- ✅ Updated to Architecture v2.0
- ✅ Added 5 mandatory modules telemetry
- ✅ Added module-specific endpoints
- ✅ Added safety telemetry
- ✅ Added location & navigation telemetry
- ✅ Added mission telemetry
- ✅ Added LiDAR scan telemetry
- ✅ Enhanced WebSocket streaming
- ✅ Standardized to 24V nominal voltage
- ✅ Added RS485 communication status
- ✅ Added comprehensive testing examples
