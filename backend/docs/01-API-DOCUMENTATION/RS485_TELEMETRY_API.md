# 📊 RS485 Telemetry API Documentation - Issue #91

**Version:** 1.0  
**Date:** 2025-01-18  
**Status:** ✅ Implemented  

## 🎯 **Overview**

The RS485 Telemetry API provides detailed register-level access to RS485 module telemetry data. This API allows you to:

- 📊 **Read module telemetry tables** with all register details
- ✏️ **Update writable registers** with safety validation
- 📡 **Real-time updates** via WebSocket integration
- 🛡️ **Safety checks** with threshold validation and force override

## 🌐 **API Endpoints**

### **GET /api/v1/rs485/modules/{address}/telemetry**

Get complete telemetry table for a specific RS485 module.

#### **Request**
```http
GET /api/v1/rs485/modules/2/telemetry
Authorization: Bearer <jwt_token>
```

#### **Path Parameters**
- `address` (integer, required): Module address (1-15)

#### **Response Format**
```json
{
  "success": true,
  "data": {
    "module_addr": "0x02",
    "module_name": "Power", 
    "registers": [
      {
        "address": "0x0001",
        "name": "Battery Level",
        "value": 85.4,
        "unit": "%",
        "mode": "R",
        "status": "normal",
        "threshold_warning": 20.0,
        "threshold_critical": 10.0
      },
      {
        "address": "0x0002",
        "name": "Input Voltage",
        "value": 24.2,
        "unit": "V",
        "mode": "R",
        "status": "normal",
        "threshold_warning": 22.0,
        "threshold_critical": 20.0
      },
      {
        "address": "0x0010",
        "name": "Power Limit",
        "value": 500.0,
        "unit": "W",
        "mode": "RW",
        "status": "normal",
        "threshold_warning": 800.0,
        "threshold_critical": 1000.0
      }
    ],
    "last_updated": "2025-01-18T13:47:15.123456"
  },
  "message": "Retrieved telemetry for module 0x02 (Power) with 5 registers",
  "timestamp": "2025-01-18T13:47:15.123456"
}
```

#### **Register Fields**
- `address`: Register hex address (e.g., "0x0001")
- `name`: Human-readable register name
- `value`: Current register value
- `unit`: Value unit (%, V, A, °C, W, etc.)
- `mode`: Access mode
  - `R`: Read-only
  - `W`: Write-only  
  - `RW`: Read-write
- `status`: Current status
  - `normal`: Value within normal range
  - `warning`: Value exceeds warning threshold
  - `critical`: Value exceeds critical threshold
- `threshold_warning`: Warning threshold value (optional)
- `threshold_critical`: Critical threshold value (optional)

#### **Response Codes**
- `200`: Success
- `401`: Unauthorized (invalid/missing token)
- `403`: Forbidden (insufficient permissions)
- `404`: Module not found
- `422`: Invalid address parameter
- `500`: Internal server error

---

### **POST /api/v1/rs485/modules/{address}/telemetry**

Update a writable register on the specified module.

#### **Request**
```http
POST /api/v1/rs485/modules/2/telemetry
Authorization: Bearer <jwt_token>
Content-Type: application/json

{
  "register_address": "0x0010",
  "value": 600.0,
  "force": false
}
```

#### **Path Parameters**
- `address` (integer, required): Module address (1-15)

#### **Request Body**
```json
{
  "register_address": "0x0010",  // Register address to update
  "value": 600.0,               // New register value
  "force": false                // Force update despite thresholds
}
```

#### **Request Fields**
- `register_address` (string, required): Hex address of register to update
- `value` (number, required): New value to write to register
- `force` (boolean, optional): Force update even if value exceeds critical threshold

#### **Response Format**
```json
{
  "success": true,
  "data": {
    "success": true,
    "message": "Register 0x0010 updated successfully",
    "old_value": 500.0,
    "new_value": 600.0,
    "register_name": "Power Limit",
    "timestamp": "2025-01-18T13:47:15.123456"
  },
  "message": "Register update completed",
  "timestamp": "2025-01-18T13:47:15.123456"
}
```

#### **Error Responses**

**Read-only Register:**
```json
{
  "success": false,
  "data": {
    "success": false,
    "message": "Register 0x0001 is read-only (mode: R)",
    "timestamp": "2025-01-18T13:47:15.123456"
  },
  "message": "Register update completed",
  "timestamp": "2025-01-18T13:47:15.123456"
}
```

**Threshold Exceeded:**
```json
{
  "success": false,
  "data": {
    "success": false,
    "message": "Value 1200.0 would exceed critical threshold 1000.0. Use force=true to override.",
    "timestamp": "2025-01-18T13:47:15.123456"
  },
  "message": "Register update completed",
  "timestamp": "2025-01-18T13:47:15.123456"
}
```

#### **Response Codes**
- `200`: Success (check `success` field in response)
- `401`: Unauthorized (invalid/missing token)
- `403`: Forbidden (insufficient permissions)
- `404`: Module or register not found
- `422`: Invalid request payload
- `500`: Internal server error

---

## 📡 **WebSocket Integration**

The telemetry API integrates with WebSocket for real-time updates.

### **WebSocket Endpoint**
```
ws://localhost:8000/ws/rs485
```

### **Register Update Events**

When a register is updated via POST API, all connected WebSocket clients receive:

```json
{
  "type": "rs485_register_update",
  "data": {
    "module_addr": "0x02",
    "register_address": "0x0010",
    "update_result": {
      "success": true,
      "message": "Register 0x0010 updated successfully",
      "old_value": 500.0,
      "new_value": 600.0,
      "register_name": "Power Limit"
    },
    "timestamp": "2025-01-18T13:47:15.123456"
  },
  "timestamp": "2025-01-18T13:47:15.123456"
}
```

### **Telemetry Update Events**

Periodic telemetry updates are broadcast as:

```json
{
  "type": "rs485_telemetry_update",
  "data": {
    "module_addr": "0x02",
    "telemetry": {
      "module_addr": "0x02",
      "module_name": "Power",
      "registers": [...],
      "last_updated": "2025-01-18T13:47:15.123456"
    },
    "timestamp": "2025-01-18T13:47:15.123456"
  },
  "timestamp": "2025-01-18T13:47:15.123456"
}
```

---

## 🛡️ **Security & Permissions**

### **Authentication**
All telemetry endpoints require JWT authentication:
```http
Authorization: Bearer <jwt_token>
```

### **Permissions**
- **GET /telemetry**: Requires `monitoring:read` permission
- **POST /telemetry**: Requires `system:configure` permission

### **Safety Features**
1. **Read-only Protection**: Cannot write to R-mode registers
2. **Threshold Validation**: Prevents dangerous values without force flag
3. **Audit Logging**: All register updates are logged
4. **Real-time Notification**: WebSocket broadcasts for immediate awareness

---

## 📊 **Module Register Maps**

### **Power Module (0x02)**
| Address | Name | Unit | Mode | Description |
|---------|------|------|------|-------------|
| 0x0001 | Battery Level | % | R | Current battery charge |
| 0x0002 | Input Voltage | V | R | DC input voltage |
| 0x0003 | Output Current | A | R | Load current |
| 0x0004 | Temperature | °C | R | Module temperature |
| 0x0010 | Power Limit | W | RW | Maximum power output |

### **Safety Module (0x03)**
| Address | Name | Unit | Mode | Description |
|---------|------|------|------|-------------|
| 0x0001 | E-Stop Status | - | R | Emergency stop state |
| 0x0002 | Safety Zone | - | R | Current safety zone |
| 0x0003 | Sensor Status | - | R | Safety sensor bitmask |
| 0x0004 | Response Time | ms | R | Safety response time |
| 0x0010 | Safety Timeout | ms | RW | Safety timeout setting |

### **Travel Module (0x04)**
| Address | Name | Unit | Mode | Description |
|---------|------|------|------|-------------|
| 0x0001 | Position | mm | R | Current position |
| 0x0002 | Velocity | mm/s | R | Current velocity |
| 0x0003 | Motor Current | A | R | Motor drive current |
| 0x0004 | Encoder Status | - | R | Encoder health |
| 0x0010 | Max Speed | mm/s | RW | Maximum speed limit |

### **Dock & Location Module (0x05)**
| Address | Name | Unit | Mode | Description |
|---------|------|------|------|-------------|
| 0x0001 | Dock Status | - | R | Docking state |
| 0x0002 | Position X | mm | R | X coordinate |
| 0x0003 | Position Y | mm | R | Y coordinate |
| 0x0004 | Orientation | deg | R | Angular position |
| 0x0010 | Dock Timeout | s | RW | Docking timeout |

---

## 🧪 **Testing Examples**

### **cURL Examples**

**Get Telemetry:**
```bash
curl -X GET "http://localhost:8000/api/v1/rs485/modules/2/telemetry" \
  -H "Authorization: Bearer your-jwt-token"
```

**Update Register:**
```bash
curl -X POST "http://localhost:8000/api/v1/rs485/modules/2/telemetry" \
  -H "Authorization: Bearer your-jwt-token" \
  -H "Content-Type: application/json" \
  -d '{
    "register_address": "0x0010",
    "value": 600.0,
    "force": false
  }'
```

**Force Update:**
```bash
curl -X POST "http://localhost:8000/api/v1/rs485/modules/2/telemetry" \
  -H "Authorization: Bearer your-jwt-token" \
  -H "Content-Type: application/json" \
  -d '{
    "register_address": "0x0010",
    "value": 1200.0,
    "force": true
  }'
```

### **Python Examples**

```python
import requests
import json

# API base URL
BASE_URL = "http://localhost:8000/api/v1/rs485"
TOKEN = "your-jwt-token"
HEADERS = {"Authorization": f"Bearer {TOKEN}"}

# Get telemetry data
def get_telemetry(module_addr):
    response = requests.get(
        f"{BASE_URL}/modules/{module_addr}/telemetry",
        headers=HEADERS
    )
    return response.json()

# Update register
def update_register(module_addr, register_addr, value, force=False):
    payload = {
        "register_address": register_addr,
        "value": value,
        "force": force
    }
    response = requests.post(
        f"{BASE_URL}/modules/{module_addr}/telemetry",
        headers=HEADERS,
        json=payload
    )
    return response.json()

# Example usage
telemetry = get_telemetry(2)  # Get Power module telemetry
print(f"Module: {telemetry['data']['module_name']}")
print(f"Registers: {len(telemetry['data']['registers'])}")

# Update power limit
result = update_register(2, "0x0010", 600.0)
print(f"Update result: {result['data']['message']}")
```

### **JavaScript/WebSocket Example**

```javascript
// WebSocket connection
const ws = new WebSocket('ws://localhost:8000/ws/rs485');

ws.onopen = function(event) {
    console.log('Connected to RS485 WebSocket');
};

ws.onmessage = function(event) {
    const message = JSON.parse(event.data);
    
    if (message.type === 'rs485_register_update') {
        console.log('Register updated:', message.data);
        // Update UI with new register value
        updateRegisterDisplay(
            message.data.module_addr,
            message.data.register_address,
            message.data.update_result
        );
    }
    
    if (message.type === 'rs485_telemetry_update') {
        console.log('Telemetry updated:', message.data);
        // Update telemetry table
        updateTelemetryTable(message.data.telemetry);
    }
};

// API calls
async function getTelemetry(moduleAddr) {
    const response = await fetch(`/api/v1/rs485/modules/${moduleAddr}/telemetry`, {
        headers: {
            'Authorization': `Bearer ${token}`
        }
    });
    return await response.json();
}

async function updateRegister(moduleAddr, registerAddr, value, force = false) {
    const response = await fetch(`/api/v1/rs485/modules/${moduleAddr}/telemetry`, {
        method: 'POST',
        headers: {
            'Authorization': `Bearer ${token}`,
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            register_address: registerAddr,
            value: value,
            force: force
        })
    });
    return await response.json();
}
```

---

## 🔧 **Implementation Notes**

### **Mock Service**
During development, the API uses a mock service that:
- Generates realistic telemetry data for each module type
- Simulates register value variations (±10%)
- Enforces read-only and threshold restrictions
- Provides different register sets per module type

### **Real Firmware Integration**
For production deployment:
- Replace mock service with real firmware communication
- Implement actual register read/write via RS485
- Add hardware-specific error handling
- Ensure thread-safe register access

### **Performance Considerations**
- Telemetry data is generated on-demand (not cached)
- WebSocket broadcasts are non-blocking
- Register updates include safety validation
- API responses include comprehensive error information

---

## 🚨 **Error Handling**

### **Common Error Scenarios**

1. **Module Not Found (404)**
   - Module address not in discovered modules
   - Module offline or disconnected

2. **Register Not Found (200 with success=false)**
   - Invalid register address for module type
   - Register not implemented in current firmware

3. **Read-Only Register (200 with success=false)**
   - Attempting to write to R-mode register
   - Use GET endpoint to read register value

4. **Threshold Exceeded (200 with success=false)**
   - Value exceeds critical threshold
   - Use `force: true` to override safety check

5. **Authentication Failed (401)**
   - Missing or invalid JWT token
   - Token expired or revoked

6. **Permission Denied (403)**
   - Insufficient user permissions
   - Contact admin for required permissions

### **Error Recovery**
- All errors include descriptive messages
- WebSocket connections auto-reconnect on disconnect
- Register updates are atomic (succeed or fail completely)
- System logs all error conditions for debugging

---

**📝 Changelog:**
- **v1.0 (2025-01-18)**: Initial implementation for Issue #91
  - GET /modules/{addr}/telemetry endpoint
  - POST /modules/{addr}/telemetry endpoint  
  - WebSocket integration for real-time updates
  - Comprehensive test coverage
  - Mock service with realistic data
