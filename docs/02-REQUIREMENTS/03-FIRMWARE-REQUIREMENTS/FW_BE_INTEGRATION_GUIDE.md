# 🔧 FW_BE_INTEGRATION_GUIDE - Hướng dẫn tích hợp Firmware cho Backend

**Mục đích:** Tài liệu hướng dẫn BE team tích hợp với OHT-50 Master Module Firmware

---

## 📋 **Thông tin tài liệu:**
- **Phiên bản:** v1.0
- **Ngày tạo:** 2025-01-27
- **Người tạo:** FW Team
- **Trạng thái:** ✅ Hoàn thành
- **Đối tượng đọc:** BE Team, System Integration Team

---

## 🎯 **Tổng quan hệ thống:**

### **Kiến trúc FW:**
```
┌─────────────────────────────────────┐
│         Application Layer           │
│  ┌─────────────┬─────────────────┐  │
│  │ API Manager │  State Machine  │  │
│  ├─────────────┼─────────────────┤  │
│  │ Safety Mgr  │  Module Mgr     │  │
│  ├─────────────┼─────────────────┤  │
│  │ Comm Mgr    │  Power Handler  │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
┌─────────────────────────────────────┐
│         HAL Layer                   │
│  ┌─────────────┬─────────────────┐  │
│  │ RS485 HAL   │  GPIO HAL       │  │
│  ├─────────────┼─────────────────┤  │
│  │ LED HAL     │  E-Stop HAL     │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
┌─────────────────────────────────────┐
│         Hardware Layer              │
│  ┌─────────────┬─────────────────┐  │
│  │ Orange Pi   │  RS485 Trans    │  │
│  ├─────────────┼─────────────────┤  │
│  │ E-Stop      │  LED Indicators │  │
│  └─────────────┴─────────────────┘  │
└─────────────────────────────────────┘
```

### **Các module được hỗ trợ:**
- **Power Module (0x02):** Quản lý nguồn, pin, charger
- **Motor Module (0x03):** Điều khiển động cơ
- **IO Module (0x04):** Digital Input/Output
- **Dock Module (0x05):** Điều khiển docking
- **Sensor Module (0x06):** Cảm biến, IMU, LiDAR

---

## 🌐 **API Endpoints:**

### **1. System Status API:**
```http
GET /api/v1/system/status
```

**Response:**
```json
{
  "system_name": "OHT-50 Master Module",
  "version": "v1.0.0",
  "status": "IDLE",
  "uptime_ms": 1234567,
  "active_modules": 3,
  "estop_active": false,
  "safety_ok": true
}
```

### **2. System Health API:**
```http
GET /api/v1/system/health
```

**Response:**
```json
{
  "status": "HEALTHY",
  "timestamp": 1640995200000,
  "response_time_ms": 15,
  "details": "All systems operational"
}
```

### **3. Modules List API:**
```http
GET /api/v1/modules
```

**Response:**
```json
{
  "modules": [
    {
      "module_id": 2,
      "module_type": "POWER",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "v1.0.0"
    },
    {
      "module_id": 3,
      "module_type": "MOTOR",
      "status": "ONLINE",
      "online": true,
      "last_seen": 1640995200000,
      "version": "v1.0.0"
    }
  ],
  "module_count": 2
}
```

### **4. Module Command API:**
```http
POST /api/v1/modules/{id}/command
```

**Request:**
```json
{
  "command": "MOVE_TO_POSITION",
  "parameters": {
    "position": 1000,
    "velocity": 500,
    "acceleration": 100
  },
  "timeout_ms": 5000
}
```

**Response:**
```json
{
  "status": "SUCCESS",
  "message": "Command executed successfully",
  "data": {
    "command_id": "cmd_12345",
    "execution_time_ms": 150
  }
}
```

### **5. Safety Status API:**
```http
GET /api/v1/safety/status
```

**Response:**
```json
{
  "estop_active": false,
  "safety_ok": true,
  "safety_level": 0,
  "safety_message": "All safety systems operational",
  "last_safety_check": 1640995200000
}
```

### **6. E-Stop API:**
```http
POST /api/v1/safety/estop
```

**Request:**
```json
{
  "estop_reason": "Emergency stop triggered by user",
  "timestamp": 1640995200000,
  "acknowledged": false
}
```

### **7. Configuration API:**
```http
GET /api/v1/config
POST /api/v1/config
```

**Response:**
```json
{
  "config_data": {
    "system": {
      "name": "OHT-50",
      "version": "v1.0.0"
    },
    "modules": {
      "power": {
        "address": 2,
        "enabled": true
      },
      "motor": {
        "address": 3,
        "enabled": true
      }
    }
  },
  "config_version": 1,
  "last_updated": 1640995200000
}
```

### **8. Diagnostics API:**
```http
GET /api/v1/diagnostics
```

**Response:**
```json
{
  "total_requests": 1234,
  "successful_requests": 1200,
  "failed_requests": 34,
  "uptime_ms": 1234567,
  "system_info": "OHT-50 Master Module v1.0.0",
  "error_log": "Recent error messages..."
}
```

---

## 🔌 **WebSocket Real-time Communication:**

### **Connection:**
```javascript
const ws = new WebSocket('ws://192.168.1.100:8081');
```

### **Message Types:**

#### **1. System Status Updates:**
```json
{
  "type": "system_status",
  "data": {
    "state": "IDLE",
    "safety_ok": true,
    "estop_active": false,
    "timestamp": 1640995200000
  }
}
```

#### **2. Module Status Updates:**
```json
{
  "type": "module_status",
  "data": {
    "module_id": 2,
    "status": "ONLINE",
    "last_seen": 1640995200000
  }
}
```

#### **3. Safety Events:**
```json
{
  "type": "safety_event",
  "data": {
    "event": "ESTOP_TRIGGERED",
    "level": "EMERGENCY",
    "timestamp": 1640995200000
  }
}
```

#### **4. Module Data Updates:**
```json
{
  "type": "module_data",
  "data": {
    "module_id": 2,
    "module_type": "POWER",
    "data": {
      "voltage": 24.5,
      "current": 2.1,
      "temperature": 45
    },
    "timestamp": 1640995200000
  }
}
```

---

## 📊 **Data Structures:**

### **1. System States:**
```c
typedef enum {
    SYSTEM_STATE_INIT = 0,        // Initialization
    SYSTEM_STATE_IDLE,            // Ready for commands
    SYSTEM_STATE_MOVE,            // Moving
    SYSTEM_STATE_DOCK,            // Docking
    SYSTEM_STATE_FAULT,           // Fault state
    SYSTEM_STATE_ESTOP,           // Emergency stop
    SYSTEM_STATE_SHUTDOWN         // Shutdown
} system_state_t;
```

### **2. Module Types:**
```c
typedef enum {
    MODULE_TYPE_UNKNOWN = 0,
    MODULE_TYPE_MOTOR,            // Motor modules
    MODULE_TYPE_IO,               // I/O modules
    MODULE_TYPE_DOCK,             // Docking modules
    MODULE_TYPE_SENSOR,           // Sensor modules
    MODULE_TYPE_POWER             // Power modules
} module_type_t;
```

### **3. Module Status:**
```c
typedef enum {
    MODULE_STATUS_UNKNOWN = 0,
    MODULE_STATUS_ONLINE,         // Module responding
    MODULE_STATUS_OFFLINE         // Module not responding
} module_status_t;
```

### **4. Safety Levels:**
```c
typedef enum {
    SAFETY_LEVEL_NORMAL = 0,      // Normal operation
    SAFETY_LEVEL_WARNING,         // Warning condition
    SAFETY_LEVEL_CRITICAL,        // Critical condition
    SAFETY_LEVEL_EMERGENCY        // Emergency stop
} safety_level_t;
```

---

## 🔧 **Configuration Parameters:**

### **1. Network Configuration:**
```c
#define API_HTTP_PORT                      8080U
#define API_WEBSOCKET_PORT                 8081U
#define API_MAX_REQUEST_SIZE               8192U
#define API_MAX_RESPONSE_SIZE              16384U
#define API_REQUEST_TIMEOUT_MS             30000U
#define API_WEBSOCKET_TIMEOUT_MS           60000U
```

### **2. RS485 Configuration:**
```c
#define RS485_DEVICE_PATH                  "/dev/ttyOHT485"
#define RS485_BAUD_RATE                    115200U
#define RS485_DATA_BITS                    8U
#define RS485_STOP_BITS                    1U
#define RS485_PARITY                       0U
#define RS485_TIMEOUT_MS                   1500U
#define RS485_RETRY_COUNT                  2U
```

### **3. Module Addresses:**
```c
#define MODULE_ADDR_POWER                  0x02U
#define MODULE_ADDR_MOTOR                  0x03U
#define MODULE_ADDR_IO                     0x04U
#define MODULE_ADDR_DOCK                   0x05U
#define MODULE_ADDR_SENSOR                 0x06U
```

### **4. Timing Constants:**
```c
#define UPDATE_PERIOD_MS                   50U
#define SAFETY_CHECK_INTERVAL_MS           50U
#define POWER_POLL_INTERVAL_MS             500U
#define HEARTBEAT_INTERVAL_MS              500U
```

---

## 📡 **Modbus Register Mapping:**

### **Power Module (0x02):**
| Register | Name | Type | Description |
|----------|------|------|-------------|
| 0x0000 | Battery Voltage | Input | Battery voltage (V * 10) |
| 0x0001 | Battery Current | Input | Battery current (A * 100) |
| 0x0002 | Battery SOC | Input | State of charge (%) |
| 0x0003 | Temperature | Input | Temperature (°C) |
| 0x0004 | Relay1 Status | Input | Relay 1 status |
| 0x0005 | Relay2 Status | Input | Relay 2 status |
| 0x0006 | Relay1 Control | Holding | Relay 1 control |
| 0x0007 | Relay2 Control | Holding | Relay 2 control |

### **Motor Module (0x03):**
| Register | Name | Type | Description |
|----------|------|------|-------------|
| 0x0000 | Position Target | Holding | Target position (mm) |
| 0x0001 | Velocity Target | Holding | Target velocity (mm/s) |
| 0x0002 | Acceleration Limit | Holding | Acceleration limit (mm/s²) |
| 0x0003 | Current Position | Input | Current position (mm) |
| 0x0004 | Current Velocity | Input | Current velocity (mm/s) |
| 0x0005 | Status | Input | Motor status |
| 0x0006 | Fault Code | Input | Fault code |

### **Dock Module (0x05):**
| Register | Name | Type | Description |
|----------|------|------|-------------|
| 0x0000 | Position Target | Holding | Target position (mm) |
| 0x0001 | Current Position | Input | Current position (mm) |
| 0x0002 | Status | Input | Dock status |
| 0x0003 | Fault Code | Input | Fault code |

---

## 🚨 **Error Handling:**

### **HTTP Status Codes:**
- **200 OK:** Request successful
- **400 Bad Request:** Invalid request parameters
- **401 Unauthorized:** Authentication required
- **403 Forbidden:** Access denied
- **404 Not Found:** Resource not found
- **500 Internal Server Error:** Server error

### **Error Response Format:**
```json
{
  "error": {
    "code": "INVALID_PARAMETER",
    "message": "Invalid parameter value",
    "details": "Position must be between 0 and 10000",
    "timestamp": 1640995200000
  }
}
```

### **Common Error Codes:**
- `MODULE_NOT_FOUND`: Module not found
- `MODULE_OFFLINE`: Module is offline
- `INVALID_COMMAND`: Invalid command
- `TIMEOUT`: Command timeout
- `SAFETY_VIOLATION`: Safety violation
- `SYSTEM_FAULT`: System fault

---

## 🔒 **Security Considerations:**

### **1. Authentication:**
- API authentication is currently disabled for testing
- Future versions will support JWT tokens
- WebSocket connections require authentication

### **2. Authorization:**
- Different user levels: ADMIN, OPERATOR, VIEWER
- Resource-based access control
- Command execution permissions

### **3. Data Validation:**
- All input parameters are validated
- Range checking for numeric values
- Type checking for data types

---

## 📈 **Performance Metrics:**

### **1. Response Times:**
- **System Status:** < 10ms
- **Module List:** < 50ms
- **Module Command:** < 100ms
- **Safety Status:** < 5ms

### **2. Throughput:**
- **HTTP Requests:** 1000+ requests/second
- **WebSocket Messages:** 100+ messages/second
- **Modbus Transactions:** 100+ transactions/second

### **3. Resource Usage:**
- **CPU Usage:** < 30% under normal load
- **Memory Usage:** < 256MB
- **Network Bandwidth:** < 1Mbps

---

## 🧪 **Testing & Development:**

### **1. Dry Run Mode:**
```bash
./oht50_main --dry-run --debug
```

### **2. API Testing:**
```bash
# Test system status
curl http://192.168.1.100:8080/api/v1/system/status

# Test module list
curl http://192.168.1.100:8080/api/v1/modules

# Test module command
curl -X POST http://192.168.1.100:8080/api/v1/modules/3/command \
  -H "Content-Type: application/json" \
  -d '{"command":"MOVE_TO_POSITION","parameters":{"position":1000}}'
```

### **3. WebSocket Testing:**
```javascript
const ws = new WebSocket('ws://192.168.1.100:8081');
ws.onmessage = function(event) {
    console.log('Received:', JSON.parse(event.data));
};
```

---

## 📚 **Integration Examples:**

### **1. Python Integration:**
```python
import requests
import json

class OHT50Client:
    def __init__(self, host="192.168.1.100", port=8080):
        self.base_url = f"http://{host}:{port}/api/v1"
    
    def get_system_status(self):
        response = requests.get(f"{self.base_url}/system/status")
        return response.json()
    
    def get_modules(self):
        response = requests.get(f"{self.base_url}/modules")
        return response.json()
    
    def send_command(self, module_id, command, parameters):
        data = {
            "command": command,
            "parameters": parameters,
            "timeout_ms": 5000
        }
        response = requests.post(
            f"{self.base_url}/modules/{module_id}/command",
            json=data
        )
        return response.json()

# Usage
client = OHT50Client()
status = client.get_system_status()
print(f"System status: {status['status']}")
```

### **2. JavaScript Integration:**
```javascript
class OHT50Client {
    constructor(host = '192.168.1.100', port = 8080) {
        this.baseUrl = `http://${host}:${port}/api/v1`;
    }
    
    async getSystemStatus() {
        const response = await fetch(`${this.baseUrl}/system/status`);
        return await response.json();
    }
    
    async getModules() {
        const response = await fetch(`${this.baseUrl}/modules`);
        return await response.json();
    }
    
    async sendCommand(moduleId, command, parameters) {
        const response = await fetch(`${this.baseUrl}/modules/${moduleId}/command`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                command: command,
                parameters: parameters,
                timeout_ms: 5000
            })
        });
        return await response.json();
    }
}

// Usage
const client = new OHT50Client();
client.getSystemStatus().then(status => {
    console.log('System status:', status.status);
});
```

---

## 🔄 **State Machine Transitions:**

### **System States:**
```
INIT → IDLE → MOVE → IDLE
  ↓      ↓      ↓
FAULT ← ESTOP ← ESTOP
  ↓
SHUTDOWN
```

### **State Descriptions:**
- **INIT:** System initialization
- **IDLE:** Ready for commands
- **MOVE:** Moving state
- **DOCK:** Docking state
- **FAULT:** Fault condition
- **ESTOP:** Emergency stop
- **SHUTDOWN:** System shutdown

---

## 📋 **Deployment Checklist:**

### **1. Pre-deployment:**
- [ ] Firmware compiled and tested
- [ ] Hardware connections verified
- [ ] Network configuration set
- [ ] Safety systems tested

### **2. Deployment:**
- [ ] Firmware uploaded to device
- [ ] System booted successfully
- [ ] API endpoints accessible
- [ ] Module communication verified

### **3. Post-deployment:**
- [ ] System status monitoring
- [ ] Performance metrics collected
- [ ] Error logs reviewed
- [ ] User training completed

---

## 📞 **Support & Troubleshooting:**

### **1. Common Issues:**
- **Module not responding:** Check RS485 connection
- **API timeout:** Check network connectivity
- **Safety violation:** Check E-Stop status
- **System fault:** Check error logs

### **2. Debug Commands:**
```bash
# Check system status
curl http://192.168.1.100:8080/api/v1/system/status

# Check diagnostics
curl http://192.168.1.100:8080/api/v1/diagnostics

# Check module status
curl http://192.168.1.100:8080/api/v1/modules
```

### **3. Log Files:**
- **System logs:** `/var/log/oht50/system.log`
- **API logs:** `/var/log/oht50/api.log`
- **Error logs:** `/var/log/oht50/error.log`

---

## 📚 **References:**

### **Related Documents:**
- [Firmware Architecture](REQ_FW_Architecture.md)
- [Modbus Register Mapping](MODBUS_REGISTER_MAPPING_COMPLETION_REPORT.md)
- [RS485 Integration](REQ_RS485_System_Overview.md)
- [Safety System](REQ_RS485_Safety_Module_Integration.md)

### **API Documentation:**
- [OpenAPI Specification](../04-BACKEND-REQUIREMENTS/REQ_BE_Architecture.md)
- [WebSocket Protocol](../04-BACKEND-REQUIREMENTS/REQ_BE_Architecture.md)

---

**Changelog:**
- ✅ v1.0 (2025-01-27): Initial version with complete API documentation
- ✅ Added WebSocket real-time communication
- ✅ Added Modbus register mapping
- ✅ Added integration examples
- ✅ Added deployment checklist

**🚨 Lưu ý:** Tài liệu này được cập nhật theo source code firmware hiện tại. Mọi thay đổi trong firmware sẽ được cập nhật trong tài liệu này.
