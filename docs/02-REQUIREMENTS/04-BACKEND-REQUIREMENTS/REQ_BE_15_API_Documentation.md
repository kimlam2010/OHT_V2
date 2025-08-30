# REQ_BE_15 - API Documentation

**Document ID:** REQ_BE_15  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Author:** CTO Team  
**Standard Reference:** ISO/IEC/IEEE 29148:2018

---

## 1. DOCUMENT INFORMATION

### 1.1 Document Control
- **Document Title:** API Documentation
- **Document Type:** API Documentation Specification
- **Standard Reference:** ISO/IEC/IEEE 29148:2018, OpenAPI 3.0
- **Project:** OHT-50 Industrial Robot Control System
- **Component:** Backend API Documentation

### 1.2 Revision History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-28 | CTO Team | Initial creation |

---

## 2. API DOCUMENTATION OVERVIEW

### 2.1 Documentation Strategy
```yaml
Documentation Strategy:
  Standard: "OpenAPI 3.0 Specification"
  Format: "Interactive HTML documentation"
  Coverage: "100% API endpoints"
  Examples: "Real-world usage examples"
  Testing: "Try-it-out functionality"
  Versioning: "Semantic versioning"
  Maintenance: "Automated updates"
```

### 2.2 Documentation Structure
```
API Documentation Structure:
├── Overview
│   ├── Introduction
│   ├── Authentication
│   ├── Error handling
│   └── Rate limiting
├── Endpoints
│   ├── Authentication
│   ├── Robot Control
│   ├── Telemetry
│   ├── Safety
│   ├── LiDAR
│   └── Configuration
├── Models
│   ├── Request models
│   ├── Response models
│   └── Error models
└── Examples
    ├── Code examples
    ├── cURL examples
    └── SDK examples
```

---

## 3. OPENAPI SPECIFICATION

### 3.1 Base Configuration
```yaml
OpenAPI Configuration:
  openapi: "3.0.3"
  info:
    title: "OHT-50 Backend API"
    description: "Industrial Robot Control System API"
    version: "1.0.0"
    contact:
      name: "OHT-50 Development Team"
      email: "dev@oht50.com"
  
  servers:
    - url: "https://api.oht50.com/v1"
      description: "Production server"
    - url: "https://staging-api.oht50.com/v1"
      description: "Staging server"
    - url: "http://localhost:8000/v1"
      description: "Development server"
  
  security:
    - BearerAuth: []
    - ApiKeyAuth: []
```

### 3.2 Key Endpoints
```yaml
Key Endpoints:
  Authentication:
    - "POST /auth/login - User login"
    - "POST /auth/refresh - Refresh token"
    - "POST /auth/logout - User logout"
  
  Robot Control:
    - "GET /robots/{id}/status - Get robot status"
    - "POST /robots/{id}/control - Control robot"
    - "GET /robots/{id}/telemetry - Get telemetry"
  
  LiDAR:
    - "GET /lidar/status - Get LiDAR status"
    - "GET /lidar/scan - Get scan data"
    - "GET /lidar/obstacles - Get obstacles"
    - "GET /lidar/map - Get environment map"
    - "POST /lidar/path - Calculate path"
  
  Safety:
    - "GET /safety/status - Get safety status"
    - "POST /safety/emergency-stop - Emergency stop"
    - "GET /safety/zones - Get safety zones"
```

---

## 4. DATA MODELS

### 4.1 Core Models
```yaml
Core Models:
  RobotStatus:
    type: object
    properties:
      robot_id: { type: integer }
      status: { type: string, enum: [IDLE, MOVING, DOCKING, ERROR, EMERGENCY_STOP] }
      mode: { type: string, enum: [AUTO, MANUAL, SEMI, MAINTENANCE] }
      position: { $ref: "#/components/schemas/Position" }
      speed: { type: number, format: float }
      battery_level: { type: integer, minimum: 0, maximum: 100 }
      temperature: { type: number, format: float }
      last_update: { type: string, format: date-time }
  
  Position:
    type: object
    properties:
      x: { type: number, format: float }
      y: { type: number, format: float }
      z: { type: number, format: float }
      orientation: { type: number, format: float }
  
  LiDARStatus:
    type: object
    properties:
      connected: { type: boolean }
      scanning: { type: boolean }
      frequency: { type: number, format: float }
      range: { type: number, format: float }
      resolution: { type: number, format: float }
      last_scan: { type: string, format: date-time }
```

### 4.2 Error Models
```yaml
Error Models:
  ErrorResponse:
    type: object
    properties:
      error: { type: string }
      message: { type: string }
      details: { type: object }
      timestamp: { type: string, format: date-time }
      request_id: { type: string }
  
  ValidationError:
    allOf:
      - $ref: "#/components/schemas/ErrorResponse"
      - type: object
        properties:
          field_errors:
            type: array
            items:
              type: object
              properties:
                field: { type: string }
                message: { type: string }
```

---

## 5. EXAMPLES

### 5.1 Authentication Example
```bash
# Login
curl -X POST "https://api.oht50.com/v1/auth/login" \
  -H "Content-Type: application/json" \
  -d '{
    "username": "operator1",
    "password": "secure_password123"
  }'

# Response
{
  "access_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "token_type": "bearer",
  "expires_in": 3600
}
```

### 5.2 Robot Control Example
```bash
# Get robot status
curl -X GET "https://api.oht50.com/v1/robots/1/status" \
  -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."

# Control robot
curl -X POST "https://api.oht50.com/v1/robots/1/control" \
  -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..." \
  -H "Content-Type: application/json" \
  -d '{
    "command": "move_forward",
    "speed": 0.5,
    "duration": 5.0
  }'
```

### 5.3 LiDAR Example
```bash
# Get LiDAR status
curl -X GET "https://api.oht50.com/v1/lidar/status" \
  -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."

# Get obstacles
curl -X GET "https://api.oht50.com/v1/lidar/obstacles" \
  -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
```

---

## 6. SDK EXAMPLES

### 6.1 Python SDK
```python
from oht50_client import OHT50Client

client = OHT50Client(
    base_url="https://api.oht50.com/v1",
    username="operator1",
    password="secure_password123"
)

# Get robot status
robot_status = client.get_robot_status(robot_id=1)
print(f"Robot status: {robot_status.status}")

# Control robot
control_response = client.control_robot(
    robot_id=1,
    command="move_forward",
    speed=0.5
)
print(f"Control result: {control_response.message}")
```

### 6.2 JavaScript SDK
```javascript
import { OHT50Client } from '@oht50/client';

const client = new OHT50Client({
  baseUrl: 'https://api.oht50.com/v1',
  username: 'operator1',
  password: 'secure_password123'
});

// Get robot status
const robotStatus = await client.getRobotStatus(1);
console.log(`Robot status: ${robotStatus.status}`);

// Control robot
const controlResponse = await client.controlRobot(1, {
  command: 'move_forward',
  speed: 0.5
});
console.log(`Control result: ${controlResponse.message}`);
```

---

## 7. ERROR HANDLING

### 7.1 HTTP Status Codes
```yaml
HTTP Status Codes:
  Success: "200 OK, 201 Created, 204 No Content"
  Client Errors: "400 Bad Request, 401 Unauthorized, 403 Forbidden, 404 Not Found"
  Server Errors: "500 Internal Server Error, 502 Bad Gateway, 503 Service Unavailable"
```

### 7.2 Error Response Format
```json
{
  "error": "validation_error",
  "message": "Invalid input parameters",
  "details": {
    "field": "speed",
    "message": "Speed must be between 0 and 10"
  },
  "timestamp": "2025-01-28T10:30:00Z",
  "request_id": "req_12345"
}
```

---

## 8. REFERENCES

### 8.1 Related Documents
- REQ_BE_01: System Overview
- REQ_BE_02: API Specifications
- REQ_BE_03: Database Design
- REQ_BE_04: Security Requirements
- REQ_BE_05: Integration Specifications
- REQ_BE_06: LiDAR Integration

### 8.2 Standards References
- ISO/IEC/IEEE 29148:2018 - Requirements Engineering
- OpenAPI 3.0 Specification - API Documentation
- JSON Schema - Data Validation
- RFC 6749 - OAuth 2.0 Authorization Framework

---

## 9. APPROVAL

| Role | Name | Date | Signature |
|------|------|------|-----------|
| CTO | [Name] | [Date] | [Signature] |
| Technical Lead | [Name] | [Date] | [Signature] |
| API Developer | [Name] | [Date] | [Signature] |

---

**Document End**
