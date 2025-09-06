# OHT-50 Backend API Documentation

## Overview

OHT-50 Backend provides REST API and WebSocket endpoints for robot control, telemetry, and safety management.

## Base URL

```
http://localhost:8000
```

## Authentication

All protected endpoints require JWT Bearer token authentication:

```
Authorization: Bearer <your-jwt-token>
```

## API Endpoints

### Health Check

#### GET /health
Check system health status.

**Response:**
```json
{
  "status": "healthy"
}
```

### API v1

#### GET /api/v1/
Get API v1 status.

**Response:**
```json
{
  "message": "OHT-50 API v1",
  "status": "active"
}
```

### Robot Control

#### GET /api/v1/robot/status
Get current robot status.

**Response:**
```json
{
  "robot_id": "OHT-50-001",
  "status": "idle",
  "position_x": 150.5,
  "position_y": 200.3,
  "battery_level": 87,
  "temperature": 42.5,
  "updated_at": "2025-01-28T10:30:00Z"
}
```

#### POST /api/v1/robot/control
Send control command to robot.

**Request:**
```json
{
  "command_type": "move",
  "parameters": {
    "direction": "forward",
    "speed": 50
  }
}
```

**Response:**
```json
{
  "success": true,
  "message": "Command sent successfully"
}
```

### Telemetry

#### GET /api/v1/telemetry/current
Get current telemetry data.

**Response:**
```json
{
  "robot_id": "OHT-50-001",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "position": {"x": 150.5, "y": 200.3},
    "velocity": {"x": 0.0, "y": 0.0},
    "battery": 87,
    "temperature": 42.5
  }
}
```

### Safety

#### POST /api/v1/safety/emergency
Trigger emergency stop.

**Response:**
```json
{
  "success": true,
  "message": "Emergency stop activated"
}
```

## WebSocket Endpoints

### Telemetry Stream

#### WebSocket: /ws/telemetry
Real-time telemetry data stream.

**Message Format:**
```json
{
  "type": "telemetry",
  "data": {
    "robot_id": "OHT-50-001",
    "position": {"x": 150.5, "y": 200.3},
    "battery": 87,
    "temperature": 42.5
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

### Control Commands

#### WebSocket: /ws/control
Real-time control command interface.

**Send Command:**
```json
{
  "type": "control",
  "command": {
    "action": "move",
    "parameters": {
      "direction": "forward",
      "speed": 50
    }
  }
}
```

**Receive Acknowledgment:**
```json
{
  "type": "ack",
  "command": {
    "action": "move",
    "parameters": {
      "direction": "forward",
      "speed": 50
    }
  }
}
```

### Alert Notifications

#### WebSocket: /ws/alerts
Real-time alert notifications.

**Message Format:**
```json
{
  "type": "alert",
  "data": {
    "level": "warning",
    "message": "Battery level low",
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

## Error Responses

### 400 Bad Request
```json
{
  "detail": "Invalid command parameters"
}
```

### 401 Unauthorized
```json
{
  "detail": "Could not validate credentials"
}
```

### 404 Not Found
```json
{
  "detail": "Robot not found"
}
```

### 500 Internal Server Error
```json
{
  "detail": "Internal server error"
}
```

## Performance Requirements

- **API Response Time**: < 50ms
- **WebSocket Latency**: < 20ms
- **Emergency Stop**: < 10ms

## Rate Limiting

- **Requests per minute**: 1000
- **Burst limit**: 100 requests

## Monitoring

### Metrics Endpoint

#### GET /metrics
Prometheus metrics endpoint for monitoring.

### Health Check

#### GET /health
System health status for load balancers and monitoring systems.
