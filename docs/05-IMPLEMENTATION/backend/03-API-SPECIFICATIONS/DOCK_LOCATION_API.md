# DOCK & LOCATION MODULE API - OHT-50 Backend v2.0

## 📋 **Tổng quan**
API quản lý Dock & Location Module (Address: 0x04) - Positioning, navigation, docking, LiDAR mapping, RFID integration, path planning.

## 🔗 **Base URL**
```
http://localhost:8000/api/v1/dock-location
```

## 📊 **Endpoints**

### **1. Location Status**

#### **GET /api/v1/dock-location/status**
Lấy trạng thái tổng quan của Dock & Location Module.

**Response:**
```json
{
  "module_id": "0x04",
  "module_type": "dock_location",
  "status": "online",
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
      "roll": 0.0,
      "accuracy": 1.5
    },
    "navigation": {
      "mode": "autonomous",
      "target": "dock_01",
      "distance_to_target": 150.5,
      "estimated_arrival": "2025-01-28T10:32:00Z"
    },
    "docking": {
      "status": "not_docking",
      "dock_id": null,
      "docking_progress": 0.0,
      "alignment_accuracy": null
    },
    "sensors": {
      "lidar": {
        "status": "active",
        "scan_frequency": 5.5,
        "points_per_scan": 360,
        "range_min": 0.15,
        "range_max": 12.0
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
  }
}
```

### **2. Positioning & Navigation**

#### **GET /api/v1/dock-location/position**
Lấy vị trí hiện tại của robot.

**Response:**
```json
{
  "position": {
    "x": 1250.5,
    "y": 850.2,
    "z": 0.0,
    "accuracy": 2.0,
    "confidence": 0.95,
    "timestamp": "2025-01-28T10:30:00Z"
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
```

#### **POST /api/v1/dock-location/navigate**
Bắt đầu navigation đến target.

**Request:**
```json
{
  "target": {
    "type": "position",
    "x": 2000.0,
    "y": 1500.0,
    "orientation": 90.0
  },
  "mode": "autonomous",
  "speed_limit": 1000.0,
  "obstacle_avoidance": true,
  "safety_margin": 100.0
}
```

**Response:**
```json
{
  "navigation_id": "nav_001",
  "status": "started",
  "estimated_duration": 120.0,
  "path_length": 850.5,
  "waypoints": [
    {"x": 1250.5, "y": 850.2},
    {"x": 1500.0, "y": 1000.0},
    {"x": 1800.0, "y": 1200.0},
    {"x": 2000.0, "y": 1500.0}
  ]
}
```

#### **GET /api/v1/dock-location/navigation/{nav_id}**
Lấy trạng thái navigation.

**Response:**
```json
{
  "navigation_id": "nav_001",
  "status": "navigating",
  "progress": 0.35,
  "current_waypoint": 2,
  "distance_remaining": 550.5,
  "estimated_arrival": "2025-01-28T10:32:00Z",
  "obstacles_detected": 0,
  "speed_current": 800.0,
  "speed_target": 1000.0
}
```

#### **POST /api/v1/dock-location/navigation/{nav_id}/cancel**
Hủy navigation.

**Request:**
```json
{
  "reason": "manual_cancel",
  "operator": "admin"
}
```

### **3. Docking Management**

#### **GET /api/v1/dock-location/docks**
Lấy danh sách docking stations.

**Response:**
```json
{
  "docks": [
    {
      "id": "dock_01",
      "name": "Charging Station 1",
      "type": "charging",
      "position": {
        "x": 2000.0,
        "y": 1500.0,
        "orientation": 90.0
      },
      "status": "available",
      "capabilities": ["charging", "data_transfer"],
      "approach_zone": {
        "x": 1950.0,
        "y": 1450.0,
        "width": 100.0,
        "length": 100.0
      }
    },
    {
      "id": "dock_02",
      "name": "Loading Station 1",
      "type": "loading",
      "position": {
        "x": 3000.0,
        "y": 2000.0,
        "orientation": 180.0
      },
      "status": "occupied",
      "capabilities": ["loading", "unloading"],
      "approach_zone": {
        "x": 2950.0,
        "y": 1950.0,
        "width": 100.0,
        "length": 100.0
      }
    }
  ]
}
```

#### **POST /api/v1/dock-location/dock**
Bắt đầu docking procedure.

**Request:**
```json
{
  "dock_id": "dock_01",
  "mode": "automatic",
  "speed": 200.0,
  "alignment_tolerance": 5.0
}
```

**Response:**
```json
{
  "docking_id": "dock_001",
  "status": "approaching",
  "dock_id": "dock_01",
  "estimated_duration": 30.0,
  "current_phase": "approach",
  "alignment_accuracy": 15.0
}
```

#### **GET /api/v1/dock-location/docking/{dock_id}**
Lấy trạng thái docking.

**Response:**
```json
{
  "docking_id": "dock_001",
  "status": "docking",
  "dock_id": "dock_01",
  "progress": 0.65,
  "current_phase": "fine_alignment",
  "alignment_accuracy": 3.2,
  "distance_to_dock": 25.5,
  "estimated_completion": "2025-01-28T10:30:45Z"
}
```

#### **POST /api/v1/dock-location/undock**
Bắt đầu undocking procedure.

**Request:**
```json
{
  "mode": "automatic",
  "target_position": {
    "x": 1950.0,
    "y": 1450.0,
    "orientation": 90.0
  }
}
```

### **4. LiDAR Management**

#### **GET /api/v1/dock-location/lidar/status**
Lấy trạng thái LiDAR sensor.

**Response:**
```json
{
  "lidar": {
    "status": "active",
    "model": "RPLIDAR_A1M8",
    "interface": "USB_2.0",
    "scan_frequency": 5.5,
    "points_per_scan": 360,
    "range_min": 0.15,
    "range_max": 12.0,
    "angular_resolution": 1.0,
    "last_scan": "2025-01-28T10:30:00Z",
    "points_in_scan": 360,
    "health": {
      "motor_speed": 5.5,
      "temperature": 45.2,
      "voltage": 5.02,
      "current": 0.8
    }
  }
}
```

#### **GET /api/v1/dock-location/lidar/scan**
Lấy scan data gần nhất.

**Response:**
```json
{
  "scan_id": "scan_001",
  "timestamp": "2025-01-28T10:30:00Z",
  "points": [
    {"angle": 0.0, "distance": 2500.0, "quality": 100},
    {"angle": 1.0, "distance": 2510.0, "quality": 98},
    {"angle": 2.0, "distance": 2520.0, "quality": 95}
  ],
  "obstacles": [
    {
      "id": "obs_001",
      "position": {"x": 1300.0, "y": 900.0},
      "distance": 150.0,
      "angle": 45.0,
      "size": "small"
    }
  ],
  "map_update": true
}
```

#### **POST /api/v1/dock-location/lidar/calibrate**
Calibrate LiDAR sensor.

**Request:**
```json
{
  "calibration_type": "full",
  "reference_points": [
    {"x": 0.0, "y": 0.0},
    {"x": 1000.0, "y": 0.0},
    {"x": 1000.0, "y": 1000.0}
  ]
}
```

#### **GET /api/v1/dock-location/lidar/map**
Lấy current map data.

**Response:**
```json
{
  "map_id": "map_001",
  "timestamp": "2025-01-28T10:30:00Z",
  "resolution": 50.0,
  "size": {
    "width": 10000.0,
    "height": 8000.0
  },
  "origin": {
    "x": 0.0,
    "y": 0.0
  },
  "obstacles": [
    {
      "id": "wall_001",
      "type": "wall",
      "points": [
        {"x": 1000.0, "y": 0.0},
        {"x": 1000.0, "y": 1000.0}
      ]
    }
  ],
  "free_space": [
    {
      "id": "area_001",
      "type": "navigable",
      "boundaries": [
        {"x": 0.0, "y": 0.0},
        {"x": 1000.0, "y": 0.0},
        {"x": 1000.0, "y": 1000.0},
        {"x": 0.0, "y": 1000.0}
      ]
    }
  ]
}
```

### **5. RFID Integration**

#### **GET /api/v1/dock-location/rfid/status**
Lấy trạng thái RFID reader.

**Response:**
```json
{
  "rfid": {
    "status": "active",
    "model": "UHF_RFID_Reader",
    "frequency": 915.0,
    "power": 30.0,
    "read_range": 3.0,
    "last_read": "2025-01-28T10:29:55Z",
    "tags_detected": 3,
    "health": {
      "antenna_status": "healthy",
      "signal_strength": 85,
      "error_rate": 0.01
    }
  }
}
```

#### **GET /api/v1/dock-location/rfid/tags**
Lấy danh sách RFID tags detected.

**Response:**
```json
{
  "tags": [
    {
      "id": "tag_001",
      "epc": "E20034120102000000000001",
      "type": "location_marker",
      "position": {
        "x": 1000.0,
        "y": 500.0,
        "orientation": 0.0
      },
      "signal_strength": 85,
      "last_seen": "2025-01-28T10:29:55Z",
      "data": {
        "marker_id": "marker_001",
        "zone": "zone_a",
        "direction": "north"
      }
    },
    {
      "id": "tag_002",
      "epc": "E20034120102000000000002",
      "type": "dock_marker",
      "position": {
        "x": 2000.0,
        "y": 1500.0,
        "orientation": 90.0
      },
      "signal_strength": 92,
      "last_seen": "2025-01-28T10:29:50Z",
      "data": {
        "dock_id": "dock_01",
        "dock_type": "charging"
      }
    }
  ]
}
```

#### **POST /api/v1/dock-location/rfid/scan**
Trigger RFID scan.

**Request:**
```json
{
  "scan_duration": 5.0,
  "power_level": 30.0,
  "filter_tags": ["location_marker", "dock_marker"]
}
```

### **6. Path Planning**

#### **POST /api/v1/dock-location/path/plan**
Tạo path plan từ current position đến target.

**Request:**
```json
{
  "start": {
    "x": 1250.5,
    "y": 850.2,
    "orientation": 45.2
  },
  "goal": {
    "x": 2000.0,
    "y": 1500.0,
    "orientation": 90.0
  },
  "algorithm": "A_STAR",
  "constraints": {
    "max_speed": 1000.0,
    "safety_margin": 100.0,
    "avoid_obstacles": true,
    "prefer_known_paths": true
  }
}
```

**Response:**
```json
{
  "path_id": "path_001",
  "status": "planned",
  "waypoints": [
    {
      "id": "wp_001",
      "x": 1250.5,
      "y": 850.2,
      "orientation": 45.2,
      "speed": 1000.0,
      "action": "start"
    },
    {
      "id": "wp_002",
      "x": 1500.0,
      "y": 1000.0,
      "orientation": 60.0,
      "speed": 1000.0,
      "action": "move"
    },
    {
      "id": "wp_003",
      "x": 1800.0,
      "y": 1200.0,
      "orientation": 75.0,
      "speed": 800.0,
      "action": "move"
    },
    {
      "id": "wp_004",
      "x": 2000.0,
      "y": 1500.0,
      "orientation": 90.0,
      "speed": 200.0,
      "action": "final_approach"
    }
  ],
  "metrics": {
    "total_distance": 850.5,
    "estimated_duration": 120.0,
    "safety_score": 0.95,
    "efficiency_score": 0.88
  }
}
```

#### **GET /api/v1/dock-location/path/{path_id}**
Lấy path plan details.

**Response:**
```json
{
  "path_id": "path_001",
  "status": "active",
  "current_waypoint": 2,
  "progress": 0.35,
  "waypoints": [...],
  "metrics": {...},
  "obstacles_avoided": 2,
  "replanning_count": 0
}
```

### **7. Configuration**

#### **GET /api/v1/dock-location/config**
Lấy cấu hình Dock & Location Module.

**Response:**
```json
{
  "positioning": {
    "accuracy_target": 2.0,
    "update_rate": 10.0,
    "fusion_algorithm": "ekf",
    "sensor_weights": {
      "lidar": 0.6,
      "rfid": 0.3,
      "imu": 0.1
    }
  },
  "navigation": {
    "path_planning": "A_STAR",
    "obstacle_avoidance": true,
    "safety_margin": 100.0,
    "replanning_threshold": 50.0,
    "max_speed": 1000.0
  },
  "docking": {
    "approach_speed": 200.0,
    "alignment_tolerance": 5.0,
    "docking_speed": 50.0,
    "retry_attempts": 3
  },
  "lidar": {
    "scan_frequency": 5.5,
    "range_min": 0.15,
    "range_max": 12.0,
    "angular_resolution": 1.0,
    "usb_interface": "USB_2.0"
  },
  "rfid": {
    "frequency": 915.0,
    "power": 30.0,
    "read_range": 3.0,
    "scan_interval": 1.0
  }
}
```

#### **PUT /api/v1/dock-location/config**
Cập nhật cấu hình.

**Request:**
```json
{
  "positioning": {
    "accuracy_target": 1.5,
    "update_rate": 20.0
  },
  "navigation": {
    "safety_margin": 150.0,
    "max_speed": 1200.0
  },
  "docking": {
    "approach_speed": 150.0,
    "alignment_tolerance": 3.0
  }
}
```

## 🚨 **Error Codes**

| Code | Message | Description |
|------|---------|-------------|
| 400 | Invalid navigation target | Target không hợp lệ |
| 404 | Dock not found | Dock không tồn tại |
| 409 | Navigation in progress | Navigation đang thực hiện |
| 422 | Path planning failed | Không thể tạo path plan |
| 500 | Sensor error | Lỗi sensor (LiDAR/RFID/IMU) |

## 📊 **WebSocket Events**

### **Position Updates**
```json
{
  "event": "position_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "position": {"x": 1250.5, "y": 850.2, "z": 0.0},
    "orientation": {"yaw": 45.2, "pitch": 0.1, "roll": 0.0},
    "accuracy": 2.0,
    "confidence": 0.95
  }
}
```

### **Navigation Events**
```json
{
  "event": "navigation_progress",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "navigation_id": "nav_001",
    "progress": 0.35,
    "current_waypoint": 2,
    "distance_remaining": 550.5
  }
}
```

### **Docking Events**
```json
{
  "event": "docking_progress",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "docking_id": "dock_001",
    "status": "docking",
    "progress": 0.65,
    "alignment_accuracy": 3.2
  }
}
```

### **LiDAR Events**
```json
{
  "event": "lidar_scan",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "scan_id": "scan_001",
    "points_count": 360,
    "obstacles_detected": 1,
    "map_updated": true
  }
}
```

## 🧪 **Testing Examples**

### **Quick Test**
```bash
# Get location status
curl http://localhost:8000/api/v1/dock-location/status

# Start navigation
curl -X POST http://localhost:8000/api/v1/dock-location/navigate \
  -H "Content-Type: application/json" \
  -d '{
    "target": {
      "type": "position",
      "x": 2000.0,
      "y": 1500.0,
      "orientation": 90.0
    },
    "mode": "autonomous"
  }'

# Get LiDAR status
curl http://localhost:8000/api/v1/dock-location/lidar/status
```

### **Python Example**
```python
import requests

# Get current position
response = requests.get("http://localhost:8000/api/v1/dock-location/position")
position = response.json()

# Start docking
dock_config = {
    "dock_id": "dock_01",
    "mode": "automatic",
    "speed": 200.0,
    "alignment_tolerance": 5.0
}
response = requests.post(
    "http://localhost:8000/api/v1/dock-location/dock",
    json=dock_config
)

# Get RFID tags
response = requests.get("http://localhost:8000/api/v1/dock-location/rfid/tags")
tags = response.json()

# Plan path
path_request = {
    "start": {"x": 1250.5, "y": 850.2, "orientation": 45.2},
    "goal": {"x": 2000.0, "y": 1500.0, "orientation": 90.0},
    "algorithm": "A_STAR"
}
response = requests.post(
    "http://localhost:8000/api/v1/dock-location/path/plan",
    json=path_request
)
```

## 📝 **Notes**

- **LiDAR USB Integration:** RPLIDAR A1M8 kết nối qua USB 2.0
- **Multi-sensor fusion:** Kết hợp LiDAR, RFID, IMU cho positioning chính xác
- **Real-time navigation:** Path planning và obstacle avoidance real-time
- **Docking automation:** Tự động docking với precision alignment
- **RFID localization:** Sử dụng RFID tags cho absolute positioning
- **Map building:** LiDAR SLAM cho dynamic map building
- **Safety integration:** Tích hợp với safety zones và speed limits
- **24V System:** Module hoạt động với hệ thống 24V nominal

---

**Changelog v1.0:**
- ✅ Created comprehensive Dock & Location Module API
- ✅ Added positioning và navigation endpoints
- ✅ Added docking management
- ✅ Added LiDAR USB integration
- ✅ Added RFID integration
- ✅ Added path planning
- ✅ Added configuration management
- ✅ Standardized to 24V nominal voltage
- ✅ Added WebSocket events
- ✅ Added comprehensive examples
