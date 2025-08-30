# REQ_BE_02 - API Specifications

**Document ID:** REQ_BE_02  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Author:** CTO Team  

---

## 1. DOCUMENT INFORMATION

### 1.1 Document Control
- **Document Title:** API Specifications
- **Document Type:** Requirements Specification
- **Standard Reference:** OpenAPI 3.1.0, ISO/IEC/IEEE 29148:2018
- **Project:** OHT-50 Industrial Robot Control System
- **Component:** Backend API Services

### 1.2 Revision History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-28 | CTO Team | Initial creation |

---

## 2. API OVERVIEW

### 2.1 Base URL
```
Development: http://localhost:8000/api/v1
Production: https://oht50-backend.company.com/api/v1
```

### 2.2 Authentication
- **Type:** Bearer Token (JWT)
- **Header:** `Authorization: Bearer <token>`
- **Token Expiry:** 24 hours
- **Refresh Token:** Available for token renewal

### 2.3 Response Format
```json
{
  "success": true,
  "data": {},
  "message": "Operation successful",
  "timestamp": "2025-01-28T10:30:00Z",
  "request_id": "req_123456789"
}
```

---

## 3. REST API ENDPOINTS

### 3.1 Authentication Endpoints

#### 3.1.1 POST /auth/login
**Purpose:** User authentication and token generation

**Request Body:**
```json
{
  "username": "operator1",
  "password": "secure_password",
  "remember_me": false
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "access_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "refresh_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "expires_in": 86400,
    "user": {
      "id": 1,
      "username": "operator1",
      "role": "operator",
      "permissions": ["read", "control", "configure"]
    }
  }
}
```

#### 3.1.2 POST /auth/refresh
**Purpose:** Refresh access token

**Request Body:**
```json
{
  "refresh_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

#### 3.1.3 POST /auth/logout
**Purpose:** User logout and token invalidation

---

### 3.2 LiDAR Sensor Endpoints

#### 3.2.1 GET /lidar/status
**Purpose:** Get LiDAR sensor status and health information

**Response:**
```json
{
  "success": true,
  "data": {
    "status": "connected|disconnected|error",
    "scan_frequency": "8 Hz",
    "last_scan": "2025-01-28T10:30:00Z",
    "health": {
      "temperature": 42.5,
      "voltage": 5.1,
      "error_count": 0
    }
  }
}
```

#### 3.2.2 GET /lidar/scan
**Purpose:** Get latest LiDAR scan data

**Response:**
```json
{
  "success": true,
  "data": {
    "timestamp": "2025-01-28T10:30:00Z",
    "point_cloud": [
      {"distance": 1.5, "angle": 0, "quality": 0.8},
      {"distance": 1.2, "angle": 1, "quality": 0.9}
    ],
    "quality": 0.85
  }
}
```

#### 3.2.3 GET /lidar/obstacles
**Purpose:** Get detected obstacles and safety zones

**Response:**
```json
{
  "success": true,
  "data": {
    "obstacles": [
      {
        "id": 1,
        "centroid": {"x": 1.0, "y": 0.5},
        "size": {"width": 0.3, "height": 0.3},
        "zone": "warning"
      }
    ],
    "safety_zones": {
      "critical": 0,
      "warning": 1,
      "safe": 0
    }
  }
}
```

#### 3.2.4 GET /lidar/map
**Purpose:** Get current environment map

**Response:**
```json
{
  "success": true,
  "data": {
    "occupancy_grid": [[0, 1, 0], [0, 0, 1]],
    "resolution": 0.1,
    "robot_position": {"x": 0, "y": 0},
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

#### 3.2.5 POST /lidar/path
**Purpose:** Calculate optimal path between two points

**Request Body:**
```json
{
  "start": {"x": 0, "y": 0},
  "goal": {"x": 10, "y": 10},
  "algorithm": "A*"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "path": [
      {"x": 0, "y": 0},
      {"x": 5, "y": 5},
      {"x": 10, "y": 10}
    ],
    "distance": 14.14,
    "estimated_time": 28.28,
    "safety_score": 0.95
  }
}
```

### 3.3 Robot Control Endpoints

#### 3.2.1 GET /robot/status
**Purpose:** Get current robot status

**Response:**
```json
{
  "success": true,
  "data": {
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
    "speed": 2.5,
    "safety_status": "safe",
    "last_update": "2025-01-28T10:30:00Z"
  }
}
```

#### 3.2.2 POST /robot/control
**Purpose:** Send control commands to robot

**Request Body:**
```json
{
  "command": "move",
  "parameters": {
    "direction": "forward",
    "speed": 2.0,
    "distance": 10.0
  },
  "priority": "normal"
}
```

**Available Commands:**
- `move`: Movement commands
- `stop`: Emergency stop
- `pause`: Pause operation
- `resume`: Resume operation
- `home`: Return to home position
- `set_mode`: Change operation mode

#### 3.2.3 POST /robot/emergency-stop
**Purpose:** Emergency stop command

**Response:**
```json
{
  "success": true,
  "data": {
    "command_id": "cmd_123456789",
    "status": "executed",
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

---

### 3.3 Telemetry Endpoints

#### 3.3.1 GET /telemetry/current
**Purpose:** Get current telemetry data

**Response:**
```json
{
  "success": true,
  "data": {
    "robot_telemetry": {
      "position": {"x": 150.5, "y": 200.3, "z": 0.0},
      "velocity": {"x": 0.0, "y": 2.5, "z": 0.0},
      "acceleration": {"x": 0.0, "y": 0.0, "z": 0.0},
      "orientation": {"roll": 0.0, "pitch": 0.0, "yaw": 45.0}
    },
    "system_telemetry": {
      "battery_voltage": 24.5,
      "battery_current": 2.1,
      "battery_level": 87,
      "temperature": 42.5,
      "cpu_usage": 45,
      "memory_usage": 62
    },
    "sensor_telemetry": {
      "proximity_sensors": [true, false, true, false],
      "nfc_reader": "tag_12345",
      "accelerometer": {"x": 0.1, "y": 0.0, "z": 9.8},
      "compass": 45.0
    },
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

#### 3.3.2 GET /telemetry/history
**Purpose:** Get historical telemetry data

**Query Parameters:**
- `start_time`: ISO 8601 timestamp
- `end_time`: ISO 8601 timestamp
- `interval`: Data interval (1s, 5s, 10s, 1m)
- `limit`: Maximum number of records

#### 3.3.3 POST /telemetry/stream
**Purpose:** Start real-time telemetry streaming

**Request Body:**
```json
{
  "stream_type": "all",
  "interval": 1000,
  "duration": 3600000
}
```

---

### 3.4 Safety Endpoints

#### 3.4.1 GET /safety/status
**Purpose:** Get safety system status

**Response:**
```json
{
  "success": true,
  "data": {
    "emergency_stop": false,
    "safety_zones": {
      "zone_1": "clear",
      "zone_2": "clear",
      "zone_3": "occupied"
    },
    "obstacles": [],
    "safety_switches": {
      "e_stop": false,
      "door_sensor": true,
      "light_curtain": true
    },
    "last_check": "2025-01-28T10:30:00Z"
  }
}
```

#### 3.4.2 POST /safety/acknowledge
**Purpose:** Acknowledge safety alerts

**Request Body:**
```json
{
  "alert_id": "alert_123456789",
  "acknowledged_by": "operator1"
}
```

---

### 3.5 Configuration Endpoints

#### 3.5.1 GET /config/system
**Purpose:** Get system configuration

**Response:**
```json
{
  "success": true,
  "data": {
    "robot_config": {
      "max_speed": 5.0,
      "max_acceleration": 2.0,
      "safety_distance": 0.5,
      "battery_threshold": 15
    },
    "network_config": {
      "ethernet_enabled": true,
      "wifi_enabled": true,
      "rs485_baudrate": 115200
    },
    "sensor_config": {
      "proximity_sensors": [true, true, true, true],
      "nfc_enabled": true,
      "accelerometer_enabled": true
    }
  }
}
```

#### 3.5.2 PUT /config/system
**Purpose:** Update system configuration

**Request Body:**
```json
{
  "robot_config": {
    "max_speed": 4.5,
    "safety_distance": 0.6
  }
}
```

---

### 3.6 Module Management Endpoints

#### 3.6.1 GET /modules
**Purpose:** Get connected modules list

**Response:**
```json
{
  "success": true,
  "data": {
    "modules": [
      {
        "id": "motor_001",
        "type": "travel_motor",
        "address": 1,
        "status": "online",
        "last_seen": "2025-01-28T10:30:00Z"
      },
      {
        "id": "dock_001",
        "type": "dock_location",
        "address": 2,
        "status": "online",
        "last_seen": "2025-01-28T10:30:00Z"
      }
    ]
  }
}
```

#### 3.6.2 GET /modules/{module_id}
**Purpose:** Get specific module details

#### 3.6.3 POST /modules/discover
**Purpose:** Discover new modules

---

## 4. WEBSOCKET API

### 4.1 Connection
```
WebSocket URL: ws://localhost:8000/ws
Authentication: Bearer token in query parameter
```

### 4.2 Event Types

#### 4.2.1 Telemetry Events
```json
{
  "event": "telemetry",
  "data": {
    "robot_telemetry": {...},
    "system_telemetry": {...},
    "sensor_telemetry": {...},
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

#### 4.2.2 Status Events
```json
{
  "event": "status_change",
  "data": {
    "robot_id": "OHT-50-001",
    "old_status": "moving",
    "new_status": "idle",
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

#### 4.2.3 Alert Events
```json
{
  "event": "alert",
  "data": {
    "alert_id": "alert_123456789",
    "severity": "warning",
    "message": "Battery level below 20%",
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

#### 4.2.4 Command Events
```json
{
  "event": "command_response",
  "data": {
    "command_id": "cmd_123456789",
    "status": "completed",
    "result": "success",
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

---

## 5. ERROR HANDLING

### 5.1 Error Response Format
```json
{
  "success": false,
  "error": {
    "code": "INVALID_COMMAND",
    "message": "Invalid command parameters",
    "details": {
      "field": "speed",
      "issue": "Value must be between 0 and 5"
    }
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "request_id": "req_123456789"
}
```

### 5.2 HTTP Status Codes
- **200:** Success
- **201:** Created
- **400:** Bad Request
- **401:** Unauthorized
- **403:** Forbidden
- **404:** Not Found
- **422:** Validation Error
- **500:** Internal Server Error
- **503:** Service Unavailable

### 5.3 Error Codes
- **AUTH_FAILED:** Authentication failed
- **INVALID_TOKEN:** Invalid or expired token
- **INSUFFICIENT_PERMISSIONS:** User lacks required permissions
- **INVALID_COMMAND:** Invalid command parameters
- **ROBOT_BUSY:** Robot is currently busy
- **SAFETY_VIOLATION:** Safety conditions not met
- **MODULE_OFFLINE:** Required module is offline
- **SYSTEM_ERROR:** Internal system error

---

## 6. RATE LIMITING

### 6.1 Limits
- **API Requests:** 1000 requests per minute per user
- **WebSocket Connections:** 10 concurrent connections per user
- **Telemetry Streams:** 5 concurrent streams per user
- **Command Execution:** 100 commands per minute per user

### 6.2 Rate Limit Headers
```
X-RateLimit-Limit: 1000
X-RateLimit-Remaining: 999
X-RateLimit-Reset: 1643366400
```

---

## 7. VERSIONING

### 7.1 Version Strategy
- **URL Versioning:** `/api/v1/`, `/api/v2/`
- **Backward Compatibility:** Maintained for 12 months
- **Deprecation Notice:** 6 months advance notice
- **Migration Support:** Automatic migration tools

---

## 8. TESTING

### 8.1 API Testing
- **Unit Tests:** Individual endpoint testing
- **Integration Tests:** End-to-end workflow testing
- **Performance Tests:** Load and stress testing
- **Security Tests:** Authentication and authorization testing

### 8.2 Test Environment
- **Mock Data:** Realistic test data sets
- **Hardware Simulation:** Firmware module simulation
- **Automated Testing:** CI/CD pipeline integration

---

## 9. DOCUMENTATION

### 9.1 OpenAPI Specification
- **Format:** OpenAPI 3.1.0
- **Location:** `/api/v1/docs`
- **Interactive Testing:** Swagger UI integration
- **Code Generation:** Client SDK generation

### 9.2 API Documentation
- **Endpoint Documentation:** Detailed endpoint descriptions
- **Request/Response Examples:** Real-world usage examples
- **Error Handling:** Comprehensive error documentation
- **Authentication Guide:** Step-by-step authentication guide

---

## 10. REFERENCES

### 10.1 Related Documents
- REQ_BE_01: System Overview
- REQ_BE_03: Database Design
- REQ_BE_04: Security Requirements
- REQ_BE_05: Integration Specifications

### 10.2 Standards References
- OpenAPI 3.1.0 Specification
- RFC 6749: OAuth 2.0
- RFC 7519: JSON Web Token (JWT)
- ISO/IEC/IEEE 29148:2018

---

## 11. APPROVAL

| Role | Name | Date | Signature |
|------|------|------|-----------|
| CTO | [Name] | [Date] | [Signature] |
| Technical Lead | [Name] | [Date] | [Signature] |
| Quality Assurance | [Name] | [Date] | [Signature] |

---

**Document End**
