# 🚀 OHT-50 Backend API Documentation

## 📋 **Overview**

OHT-50 Backend API provides comprehensive robot control, mapping, localization, and sensor data management capabilities. The API is built with FastAPI and provides real-time communication through WebSocket connections.

### **Base URL**
- **Development:** `http://localhost:8000`
- **Production:** `https://api.oht50.com`

### **API Version**
- **Current Version:** v1
- **API Prefix:** `/api/v1`

---

## 🔐 **Authentication**

All API endpoints require JWT authentication except for health checks and public endpoints.

### **Authentication Header**
```http
Authorization: Bearer <your_jwt_token>
```

### **Token Format**
```json
{
  "sub": "username",
  "exp": 1640995200,
  "iat": 1640908800
}
```

---

## 🗺️ **Map Management API**

### **Start Mapping**
Start a new mapping session.

```http
POST /api/v1/map/start-mapping
```

**Request Body:**
```json
{
  "map_name": "Factory Floor Map",
  "resolution": 50.0,
  "width": 1000,
  "height": 1000
}
```

**Response:**
```json
{
  "success": true,
  "message": "Mapping started successfully",
  "data": {
    "map_id": "map_001",
    "session_id": "session_001",
    "start_time": "2025-01-28T10:00:00Z"
  }
}
```

### **Stop Mapping**
Stop the current mapping session and finalize the map.

```http
POST /api/v1/map/stop-mapping
```

**Response:**
```json
{
  "success": true,
  "map_id": "map_001",
  "session_id": "session_001",
  "message": "Mapping stopped successfully"
}
```

### **Get Mapping Status**
Get the current mapping status.

```http
GET /api/v1/map/status
```

**Response:**
```json
{
  "is_mapping": true,
  "current_map_id": "map_001",
  "current_session_id": "session_001",
  "total_scans": 150,
  "mapping_quality": 0.85
}
```

### **Get Current Map**
Get information about the current active map.

```http
GET /api/v1/map/current
```

**Response:**
```json
{
  "success": true,
  "data": {
    "map_id": "map_001",
    "name": "Factory Floor Map",
    "resolution": 50.0,
    "width": 1000,
    "height": 1000,
    "metadata": {
      "created_by": "system",
      "version": "1.0"
    },
    "created_at": "2025-01-28T10:00:00Z",
    "updated_at": "2025-01-28T10:30:00Z"
  }
}
```

### **Load Map**
Load an existing map.

```http
POST /api/v1/map/load/{map_id}
```

**Response:**
```json
{
  "success": true,
  "message": "Map loaded successfully",
  "data": {
    "map": {
      "map_id": "map_001",
      "name": "Factory Floor Map",
      "resolution": 50.0,
      "width": 1000,
      "height": 1000
    }
  }
}
```

### **Get Robot Position**
Get the current robot position using hybrid localization.

```http
GET /api/v1/map/robot-position
```

**Response:**
```json
{
  "success": true,
  "position": {
    "x": 100.0,
    "y": 200.0,
    "theta": 1.57,
    "confidence": 0.9,
    "source": "hybrid"
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **Get Occupancy Grid**
Get the current occupancy grid data.

```http
GET /api/v1/map/occupancy-grid
```

**Response:**
```json
{
  "success": true,
  "occupancy_grid": {
    "data": [[-1, 0, 1], [0, 1, 0]],
    "resolution": 50.0,
    "width": 1000,
    "height": 1000
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **Get Robot Trajectory**
Get the robot's trajectory from the current map.

```http
GET /api/v1/map/trajectory
```

**Response:**
```json
{
  "success": true,
  "data": {
    "trajectory": [
      {"x": 100, "y": 200, "timestamp": "2025-01-28T10:00:00Z"},
      {"x": 105, "y": 205, "timestamp": "2025-01-28T10:01:00Z"}
    ],
    "map_id": "map_001",
    "total_points": 2
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **Get Map List**
Get a list of all available maps.

```http
GET /api/v1/map/list
```

**Response:**
```json
{
  "success": true,
  "maps": [
    {
      "map_id": "map_001",
      "name": "Factory Floor Map",
      "resolution": 50.0,
      "width": 1000,
      "height": 1000,
      "created_at": "2025-01-28T10:00:00Z",
      "updated_at": "2025-01-28T10:30:00Z",
      "metadata": {}
    }
  ],
  "total_count": 1
}
```

### **Delete Map**
Delete a map.

```http
DELETE /api/v1/map/{map_id}
```

**Response:**
```json
{
  "success": true,
  "message": "Map map_001 deleted successfully"
}
```

### **Update Map**
Update map information.

```http
PUT /api/v1/map/{map_id}
```

**Request Body:**
```json
{
  "name": "Updated Map Name",
  "metadata": {
    "description": "Updated description"
  }
}
```

**Response:**
```json
{
  "success": true,
  "message": "Map map_001 updated successfully"
}
```

---

## 📡 **Sensor Data API**

### **Submit Sensor Data**
Submit sensor data for processing.

```http
POST /api/v1/sensors/data
```

**Request Body:**
```json
{
  "sensor_type": "rfid",
  "sensor_id": "RFID_001",
  "data": {
    "rfid_id": "TAG_001",
    "signal_strength": 0.8,
    "distance": 150.5,
    "angle": 45.0
  },
  "quality": 0.9
}
```

**Response:**
```json
{
  "success": true,
  "sensor_id": "RFID_001",
  "message": "Sensor data submitted successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **Get Sensor Status**
Get the status of a specific sensor.

```http
GET /api/v1/sensors/status/{sensor_id}
```

**Response:**
```json
{
  "sensor_id": "RFID_001",
  "sensor_type": "rfid",
  "status": "online",
  "last_reading_time": "2025-01-28T10:30:00Z",
  "error_count": 0,
  "health_score": 1.0,
  "updated_at": "2025-01-28T10:30:00Z"
}
```

### **Get All Sensor Status**
Get the status of all sensors.

```http
GET /api/v1/sensors/status
```

**Response:**
```json
[
  {
    "sensor_id": "RFID_001",
    "sensor_type": "rfid",
    "status": "online",
    "last_reading_time": "2025-01-28T10:30:00Z",
    "error_count": 0,
    "health_score": 1.0,
    "updated_at": "2025-01-28T10:30:00Z"
  }
]
```

### **Configure Sensor**
Configure a sensor.

```http
POST /api/v1/sensors/configure
```

**Request Body:**
```json
{
  "sensor_id": "RFID_001",
  "sensor_type": "rfid",
  "name": "RFID Reader 1",
  "description": "Main RFID reader",
  "configuration": {
    "baud_rate": 9600,
    "timeout": 1000
  },
  "calibration_data": {
    "offset_x": 0.0,
    "offset_y": 0.0
  }
}
```

**Response:**
```json
{
  "success": true,
  "sensor_id": "RFID_001",
  "message": "Sensor configured successfully"
}
```

### **Get Sensor Configuration**
Get the configuration of a specific sensor.

```http
GET /api/v1/sensors/configure/{sensor_id}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "sensor_id": "RFID_001",
    "sensor_type": "rfid",
    "name": "RFID Reader 1",
    "description": "Main RFID reader",
    "configuration": {
      "baud_rate": 9600,
      "timeout": 1000
    },
    "calibration_data": {
      "offset_x": 0.0,
      "offset_y": 0.0
    },
    "is_active": true,
    "created_at": "2025-01-28T10:00:00Z",
    "updated_at": "2025-01-28T10:30:00Z"
  }
}
```

### **Get Sensor List**
Get a list of all sensors.

```http
GET /api/v1/sensors/list
```

**Response:**
```json
{
  "success": true,
  "sensors": [
    {
      "sensor_id": "RFID_001",
      "sensor_type": "rfid",
      "name": "RFID Reader 1",
      "description": "Main RFID reader",
      "is_active": true,
      "status": "online",
      "health_score": 1.0,
      "last_reading_time": "2025-01-28T10:30:00Z",
      "created_at": "2025-01-28T10:00:00Z",
      "updated_at": "2025-01-28T10:30:00Z"
    }
  ],
  "total_count": 1
}
```

### **Get Sensor Data**
Get recent sensor data.

```http
GET /api/v1/sensors/data/{sensor_id}?limit=100
```

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "sensor_type": "rfid",
      "sensor_id": "RFID_001",
      "data": {
        "rfid_id": "TAG_001",
        "signal_strength": 0.8
      },
      "quality": 0.9,
      "timestamp": "2025-01-28T10:30:00Z"
    }
  ],
  "total_count": 1,
  "sensor_id": "RFID_001"
}
```

### **Calibrate Sensor**
Calibrate a sensor.

```http
POST /api/v1/sensors/calibrate/{sensor_id}
```

**Request Body:**
```json
{
  "offset_x": 0.1,
  "offset_y": 0.2,
  "scale_factor": 1.0
}
```

**Response:**
```json
{
  "success": true,
  "message": "Sensor RFID_001 calibrated successfully",
  "calibration_data": {
    "offset_x": 0.1,
    "offset_y": 0.2,
    "scale_factor": 1.0
  }
}
```

---

## 🎯 **Localization API**

### **Get Current Position**
Get the current robot position using hybrid localization.

```http
GET /api/v1/localization/position
```

**Response:**
```json
{
  "success": true,
  "data": {
    "position": {
      "x": 100.0,
      "y": 200.0,
      "theta": 1.57,
      "confidence": 0.9,
      "source": "hybrid"
    },
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

### **Update Position**
Update robot position from sensor data.

```http
POST /api/v1/localization/position/update
```

**Request Body:**
```json
{
  "sensor_type": "rfid",
  "sensor_id": "RFID_001",
  "position_data": {
    "x": 100.0,
    "y": 200.0,
    "theta": 1.57
  },
  "confidence": 0.9
}
```

**Response:**
```json
{
  "success": true,
  "message": "Position update submitted for processing",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **Get Position History**
Get robot position history.

```http
GET /api/v1/localization/history?limit=50
```

**Response:**
```json
{
  "success": true,
  "data": {
    "positions": [
      {
        "x": 100.0,
        "y": 200.0,
        "theta": 1.57,
        "confidence": 0.9,
        "source": "hybrid",
        "timestamp": "2025-01-28T10:30:00Z"
      }
    ],
    "total_count": 1,
    "limit": 50
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### **Get Localization Statistics**
Get localization statistics.

```http
GET /api/v1/localization/stats
```

**Response:**
```json
{
  "total_positions": 100,
  "average_confidence": 0.85,
  "sensor_usage": {
    "rfid": 40,
    "accelerometer": 30,
    "proximity": 20,
    "lidar": 10
  },
  "current_position": {
    "x": 100.0,
    "y": 200.0,
    "theta": 1.57,
    "confidence": 0.9
  },
  "position_history": [
    {
      "x": 100.0,
      "y": 200.0,
      "confidence": 0.9
    }
  ]
}
```

### **Set Localization Configuration**
Set localization configuration.

```http
POST /api/v1/localization/config
```

**Request Body:**
```json
{
  "config_key": "rfid_threshold",
  "config_value": {
    "threshold": 0.5,
    "timeout": 1000
  },
  "config_type": "rfid",
  "description": "RFID signal threshold configuration"
}
```

**Response:**
```json
{
  "success": true,
  "config_key": "rfid_threshold",
  "message": "Localization configuration set successfully"
}
```

### **Get Localization Configuration**
Get localization configuration.

```http
GET /api/v1/localization/config/{config_key}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "config_key": "rfid_threshold",
    "config_value": {
      "threshold": 0.5,
      "timeout": 1000
    },
    "config_type": "rfid",
    "description": "RFID signal threshold configuration",
    "is_active": true,
    "created_at": "2025-01-28T10:00:00Z",
    "updated_at": "2025-01-28T10:30:00Z"
  }
}
```

### **Get All Localization Configurations**
Get all localization configurations.

```http
GET /api/v1/localization/config
```

**Response:**
```json
{
  "success": true,
  "data": {
    "configurations": [
      {
        "config_key": "rfid_threshold",
        "config_value": {
          "threshold": 0.5,
          "timeout": 1000
        },
        "config_type": "rfid",
        "description": "RFID signal threshold configuration",
        "created_at": "2025-01-28T10:00:00Z",
        "updated_at": "2025-01-28T10:30:00Z"
      }
    ],
    "total_count": 1
  }
}
```

### **Delete Localization Configuration**
Delete localization configuration.

```http
DELETE /api/v1/localization/config/{config_key}
```

**Response:**
```json
{
  "success": true,
  "message": "Configuration rfid_threshold deleted successfully"
}
```

### **Reset Localization**
Reset localization engine.

```http
POST /api/v1/localization/reset
```

**Response:**
```json
{
  "success": true,
  "message": "Localization engine reset successfully"
}
```

---

## 🔌 **WebSocket API**

### **Connection**
Connect to WebSocket for real-time updates.

```javascript
const ws = new WebSocket('ws://localhost:8000/ws');
```

### **Message Types**

#### **Position Update**
```json
{
  "type": "position",
  "data": {
    "x": 100.0,
    "y": 200.0,
    "theta": 1.57,
    "confidence": 0.9,
    "source": "hybrid"
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "priority": 1
}
```

#### **Telemetry Update**
```json
{
  "type": "telemetry",
  "data": {
    "battery": 85,
    "temperature": 42.5,
    "speed": 0.5
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "priority": 2
}
```

#### **Sensor Data Update**
```json
{
  "type": "sensor_data",
  "data": {
    "sensor_type": "rfid",
    "sensor_id": "RFID_001",
    "data": {
      "rfid_id": "TAG_001",
      "signal_strength": 0.8
    },
    "quality": 0.9
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "priority": 2
}
```

#### **Map Update**
```json
{
  "type": "map_update",
  "data": {
    "map_id": "map_001",
    "occupancy_grid": [[-1, 0, 1], [0, 1, 0]],
    "resolution": 50.0
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "priority": 3
}
```

#### **Alert**
```json
{
  "type": "alert",
  "data": {
    "level": "warning",
    "message": "Low battery",
    "sensor_id": "BATTERY_001"
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "priority": 1
}
```

#### **Status Update**
```json
{
  "type": "status",
  "data": {
    "robot_status": "idle",
    "mapping_status": "active",
    "system_health": "good"
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "priority": 3
}
```

---

## 📊 **Performance Requirements**

### **Response Time Targets**
- **API Endpoints:** < 100ms
- **Emergency Stop:** < 10ms
- **WebSocket Latency:** < 20ms
- **Database Queries:** < 10ms

### **Update Frequencies**
- **Position Updates:** 20Hz (50ms interval)
- **Telemetry Updates:** 10Hz (100ms interval)
- **Sensor Data:** 10Hz (100ms interval)
- **Map Updates:** 5Hz (200ms interval)
- **Alerts:** 1Hz (1000ms interval)
- **Status Updates:** 5Hz (200ms interval)

### **System Requirements**
- **Memory Usage:** < 512MB
- **CPU Usage:** < 80%
- **Disk Usage:** < 90%
- **Uptime:** > 99.9%

---

## 🚨 **Error Handling**

### **HTTP Status Codes**
- **200 OK:** Request successful
- **201 Created:** Resource created successfully
- **400 Bad Request:** Invalid request data
- **401 Unauthorized:** Authentication required
- **403 Forbidden:** Insufficient permissions
- **404 Not Found:** Resource not found
- **422 Unprocessable Entity:** Validation error
- **429 Too Many Requests:** Rate limit exceeded
- **500 Internal Server Error:** Server error

### **Error Response Format**
```json
{
  "success": false,
  "error": "Error message",
  "detail": "Detailed error information",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

## 🔒 **Security**

### **Authentication**
- JWT tokens with 1-hour expiration
- Refresh token mechanism
- Role-based access control (RBAC)

### **Rate Limiting**
- 1000 requests per minute per user
- Burst limit of 100 requests

### **Security Headers**
- `X-Content-Type-Options: nosniff`
- `X-Frame-Options: DENY`
- `X-XSS-Protection: 1; mode=block`
- `Referrer-Policy: strict-origin-when-cross-origin`

---

## 📚 **Examples**

### **Complete Mapping Workflow**
```python
import requests

# 1. Start mapping
response = requests.post(
    "http://localhost:8000/api/v1/map/start-mapping",
    headers={"Authorization": "Bearer <token>"},
    json={
        "map_name": "Factory Floor",
        "resolution": 50.0,
        "width": 1000,
        "height": 1000
    }
)

# 2. Submit sensor data
response = requests.post(
    "http://localhost:8000/api/v1/sensors/data",
    headers={"Authorization": "Bearer <token>"},
    json={
        "sensor_type": "rfid",
        "sensor_id": "RFID_001",
        "data": {
            "rfid_id": "TAG_001",
            "signal_strength": 0.8
        },
        "quality": 0.9
    }
)

# 3. Get robot position
response = requests.get(
    "http://localhost:8000/api/v1/localization/position",
    headers={"Authorization": "Bearer <token>"}
)

# 4. Stop mapping
response = requests.post(
    "http://localhost:8000/api/v1/map/stop-mapping",
    headers={"Authorization": "Bearer <token>"}
)
```

### **WebSocket Connection**
```javascript
const ws = new WebSocket('ws://localhost:8000/ws');

ws.onopen = function(event) {
    console.log('WebSocket connected');
};

ws.onmessage = function(event) {
    const message = JSON.parse(event.data);
    console.log('Received:', message);
    
    switch(message.type) {
        case 'position':
            updateRobotPosition(message.data);
            break;
        case 'telemetry':
            updateTelemetry(message.data);
            break;
        case 'alert':
            showAlert(message.data);
            break;
    }
};

ws.onclose = function(event) {
    console.log('WebSocket disconnected');
};
```

---

## 📞 **Support**

For technical support or questions about the API:

- **Email:** dev@oht50.com
- **Documentation:** `/docs` (Swagger UI)
- **GitHub:** https://github.com/oht50/backend

---

**Last Updated:** 2025-01-28  
**Version:** 1.0.0
