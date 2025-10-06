# 📡 OHT-50 Firmware API Documentation

**Version:** 2.6.0  
**Date:** 2025-01-28  
**Team:** Firmware & Backend Integration  
**Base URL:** `http://localhost:8080` (HTTP) | `ws://localhost:8081` (WebSocket)  
**Security:** Bearer Token Authentication | Performance Optimized | Error Handling Enhanced  
**Status:** ✅ Production Ready | ✅ Backend Integration Complete | ✅ Module Data Access APIs | ✅ WebSocket System Fixed (Issue #153) | ✅ Network Management APIs (Issue #160) | ✅ WiFi AP Mode APIs (Issue #168) | 🚀 Ready for Frontend Integration

---

## 🎯 **OVERVIEW**

OHT-50 Firmware cung cấp **50+ REST API endpoints** và **WebSocket real-time streaming** với **Security Framework** hoàn chỉnh để Backend có thể:
- 🤖 Điều khiển robot và monitor status
- 🛡️ Quản lý safety và emergency controls
- 📊 Lấy telemetry data real-time
- 🔧 Quản lý modules và system configuration
- 👁️ Truy cập LiDAR data và scan results
- 🌐 **WiFi AP Mode** cho fallback connectivity
- 🔄 **Auto-fallback** khi WiFi connection fails
- 🔒 **Secure authentication** với Bearer token
- 🚀 **Performance optimized** với adaptive timing
- 🔧 **Enhanced error handling** với standardized codes

### **📋 QUICK REFERENCE - API ENDPOINTS**
| Category | Count | Key Endpoints | Auth Required |
|----------|-------|---------------|---------------|
| **🔗 Connection** | 2 | `/health`, `/api/v1/status` | ❌ |
| **🤖 Robot Control** | 2 | `/api/v1/robot/status`, `/api/v1/robot/command` | 1/2 |
| **🛡️ Safety** | 2 | `/api/v1/safety/status`, `/api/v1/safety/estop` | 1/2 |
| **📊 System** | 2 | `/api/v1/system/status`, `/api/v1/system/state` | ❌ |
| **🔧 Modules** | 8 | `/api/v1/rs485/modules`, `/api/v1/modules/stats`, `/api/v1/modules/start-scan` | 4/8 |
| **🔍 Module Data Access** | 6 | `/api/v1/modules/{id}/telemetry`, `/api/v1/modules/{id}/config` | 3/6 |
| **⚡ Motion** | 3 | `/api/v1/motion/segment/start`, `/api/v1/motion/state` | 2/3 |
| **👁️ LiDAR** | 10 | `/api/v1/lidar/scan_data`, `/api/v1/lidar/scan_frame_360` | 2/10 |
| **🔄 Control** | 1 | `/api/v1/control/status` | ❌ |
| **⚙️ Config** | 3 | `/api/v1/config/state-machine`, `/api/v1/config/timeouts` | 2/3 |
| **📊 Statistics** | 1 | `/api/v1/state/statistics` | ❌ |
| **🚦 State** | 4 | `/api/v1/state/move`, `/api/v1/state/stop` | ✅ |
| **🌐 Network** | 6 | `/api/v1/network/status`, `/api/v1/network/wifi/scan`, `/api/v1/network/wifi/connect` | 3/6 |
| **🔌 WiFi AP Mode** | 12 | `/api/v1/network/ap/start`, `/api/v1/network/ap/status`, `/api/v1/network/fallback/enable` | 8/12 |
| **🌊 WebSocket** | 3 | `/health`, `/api/v1/status`, `/api/v1/robot/status` | ❌ |
| **TOTAL** | **61** | **49 REST + 3 WebSocket** | **26/49 (53%)** |

### **✅ Backend Integration Status**
- ✅ **HTTP API Integration:** Port 8080 - REST endpoints ready
- ✅ **WebSocket Integration:** Port 8081 - Real-time streaming active
- ✅ **Authentication System:** Bearer token validation implemented
- ✅ **Error Handling:** Standardized error responses với context
- ✅ **Backend Service Layer:** Integration complete
- ✅ **Database Integration:** Telemetry storage implemented
- ✅ **Frontend WebSocket:** Real-time UI updates ready
- 🚀 **Next Phase:** Frontend Integration & UI Development

---

## 🎨 **FRONTEND INTEGRATION READY**

### **🚀 Frontend Development Status**
- ✅ **API Endpoints:** All 50+ endpoints documented và tested
- ✅ **WebSocket Streaming:** Real-time data ready for UI
- ✅ **Authentication:** Bearer token system implemented
- ✅ **Error Handling:** Standardized responses for UI error handling
- ✅ **Performance:** Optimized for real-time UI updates
- ✅ **Documentation:** Complete examples và integration guides

### **🎯 Frontend Integration Points**
- **Dashboard UI:** Robot status, telemetry, system health
- **Control Panel:** Manual control, emergency stop, motion control
- **Map Interface:** LiDAR visualization, obstacle detection
- **Configuration:** System settings, module management
- **Monitoring:** Real-time alerts, performance metrics
- **WiFi AP Management:** AP status, client management, fallback controls

---

## 🏗️ **ARCHITECTURE OVERVIEW**

```
Frontend UI (React/Vue)     Backend Services (Python/Node.js)
       ↓                              ↓
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   HTTP Client   │    │   HTTP Client   │    │ WebSocket Client│
│   Port 8080     │    │   Port 8080     │    │   Port 8081     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
       ↓                        ↓                        ↓
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│  Firmware API   │    │  Firmware API   │    │ WebSocket Server│
│   HTTP Server   │    │   HTTP Server   │    │ + HTTP Backup   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
       ↓                        ↓                        ↓
┌─────────────────────────────────────────────────────────────────┐
│                    OHT-50 Firmware Core                        │
│        System | Safety | Modules | LiDAR | Communication       │
│        WiFi AP Mode | Fallback | Network Management           │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔒 **SECURITY & AUTHENTICATION**

### **Bearer Token Authentication**

**🔐 Required for Protected Endpoints:**
- Configuration changes (`POST /api/v1/config/*`)
- System control (`POST /api/v1/state/*`)
- Emergency controls (có audit logging)
- WiFi AP Mode operations (`POST /api/v1/network/ap/*`)

**Authentication Header:**
```http
Authorization: Bearer <token>
```

**Valid Tokens:**
```bash
# Admin Level (Full Access)
Authorization: Bearer oht50_admin_token_2025

# Operator Level (Control Access)
Authorization: Bearer oht50_operator_token_2025

# Readonly Level (Monitor Only)
Authorization: Bearer oht50_readonly_token_2025
```

**Authentication Error Response:**
```json
{
  "success": false,
  "error": "Authentication required",
  "message": "Please provide valid Bearer token in Authorization header",
  "auth_methods": ["Bearer token"],
  "example": "Authorization: Bearer oht50_admin_token_2025"
}
```

### WebSocket Authentication (bắt buộc)

- Kênh WebSocket (port 8081) yêu cầu xác thực trước khi nâng cấp kết nối.
- Hỗ trợ các cách truyền thông tin xác thực:
  - Header `Authorization: Bearer <token>`
  - Header `X-API-Key: <key>`
  - Subprotocol `Sec-WebSocket-Protocol: bearer, <token>`
  - Query `GET /ws?token=<token>`

Nếu xác thực thất bại: trả `401 Unauthorized` và không nâng cấp WebSocket.

Ví dụ (JavaScript):
```javascript
const token = "oht50_admin_token_2025";
// Subprotocol
const ws1 = new WebSocket("ws://localhost:8081/ws", ["bearer", token]);
// Query token
const ws2 = new WebSocket(`ws://localhost:8081/ws?token=${encodeURIComponent(token)}`);
```

Sau handshake: client chưa xác thực gửi TEXT/BINARY sẽ bị đóng với mã 1008 (POLICY_VIOLATION).

**Security Audit Logging:**
```bash
[API_SECURITY] 🔒 Configuration change attempt from client
[API_SECURITY] 🚨 EMERGENCY STOP triggered via API
[API_AUTH] ✅ Token validated: ADMIN
[API_AUTH] ❌ Invalid or expired token
```

**Backend Authentication Example:**
```python
# Set authentication header
headers = {
    "Authorization": "Bearer oht50_admin_token_2025",
    "Content-Type": "application/json"
}

# Make authenticated request
response = await fw_client.post("/api/v1/config/state-machine", 
                               json=config_data, headers=headers)
```

---

## 📋 **COMPLETE API ENDPOINTS LIST**

### **🔗 Connection & Health**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/health` | Health check | ❌ | 8080 |
| GET | `/api/v1/status` | Basic status | ❌ | 8080/8081 |

### **🤖 Robot Control**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/robot/status` | Get robot status | ❌ | 8080 |
| POST | `/api/v1/robot/command` | Send robot command | ✅ | 8080 |

### **🛡️ Safety & Emergency**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/safety/status` | Get safety status | ❌ | 8080 |
| POST | `/api/v1/safety/estop` | Emergency stop | ✅ | 8080 |

### **📊 System Management**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/system/status` | Get system status | ❌ | 8080 |
| GET | `/api/v1/system/state` | Get system state | ❌ | 8080 |

### **🔧 Module Management**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/rs485/modules` | Get RS485 modules | ❌ | 8080 |
| GET | `/api/v1/modules/stats` | Get module statistics | ❌ | 8080 |
| GET | `/api/v1/modules/{id}/status` | Get module status by ID | ❌ | 8080 |
| POST | `/api/v1/modules/start-scan` | Start RS485 discovery/scan | ✅ (API-Key/JWT) | 8080 |
| POST | `/api/v1/modules/pause-scan` | Pause RS485 scan | ✅ (API-Key/JWT) | 8080 |
| POST | `/api/v1/modules/resume-scan` | Resume paused scan | ✅ (API-Key/JWT) | 8080 |
| POST | `/api/v1/modules/stop-scan` | Stop RS485 scan | ✅ (API-Key/JWT) | 8080 |
| POST | `/api/v1/modules/discover` | Quick discovery refresh (lightweight) | ✅ (API-Key/JWT) | 8080 |
| GET | `/api/v1/modules/scan-status` | Get current RS485 scan status | ❌ | 8080 |

### **⚡ Motion Control**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| POST | `/api/v1/motion/segment/start` | Start motion segment | ✅ | 8080 |
| POST | `/api/v1/motion/segment/stop` | Stop motion segment | ✅ | 8080 |
| GET | `/api/v1/motion/state` | Get motion state | ❌ | 8080 |

### **👁️ LiDAR Integration**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/lidar/status` | Get LiDAR status | ❌ | 8080 |
| GET | `/api/v1/lidar/scan_data` | Get LiDAR scan data | ❌ | 8080 |
| GET | `/api/v1/lidar/scan_frame_full` | Get full LiDAR frame | ❌ | 8080 |
| GET | `/api/v1/lidar/scan_frame` | Get LiDAR frame with filter | ❌ | 8080 |
| GET | `/api/v1/lidar/scan_frame_360` | Get 360° reduced frame | ❌ | 8080 |
| POST | `/api/v1/lidar/start_scanning` | Start LiDAR scanning | ✅ | 8080 |
| POST | `/api/v1/lidar/stop_scanning` | Stop LiDAR scanning | ✅ | 8080 |
| GET | `/api/v1/lidar/config` | Get LiDAR configuration | ❌ | 8080 |
| GET | `/api/v1/lidar/safety_status` | Get LiDAR safety status | ❌ | 8080 |
| GET | `/api/v1/lidar/health` | LiDAR health check | ❌ | 8080 |

### **🔄 Control Operations**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/control/status` | Get control status | ❌ | 8080 |

### **⚙️ Configuration Management**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/config/state-machine` | Get state machine config | ❌ | 8080 |
| POST | `/api/v1/config/state-machine` | Set state machine config | ✅ | 8080 |
| POST | `/api/v1/config/timeouts` | Set timeouts config | ✅ | 8080 |

### **📊 Statistics & Monitoring**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/state/statistics` | Get state statistics | ❌ | 8080 |

### **🚦 State Control**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| POST | `/api/v1/state/move` | Move command | ✅ | 8080 |
| POST | `/api/v1/state/stop` | Stop command | ✅ | 8080 |
| POST | `/api/v1/state/emergency` | Emergency command | ✅ | 8080 |
| POST | `/api/v1/state/reset` | Reset command | ✅ | 8080 |

### **🌐 Network Management**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/api/v1/network/status` | Get network status | ❌ | 8080 |
| GET | `/api/v1/network/wifi/scan` | Scan for WiFi networks | ❌ | 8080 |
| POST | `/api/v1/network/wifi/connect` | Connect to WiFi network | ✅ | 8080 |
| POST | `/api/v1/network/wifi/disconnect` | Disconnect from WiFi | ✅ | 8080 |
| GET | `/api/v1/network/performance` | Get network performance | ❌ | 8080 |
| GET | `/api/v1/network/health` | Get network health status | ❌ | 8080 |

### **🔌 WiFi AP Mode APIs (NEW - Issue #168)**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| POST | `/api/v1/network/ap/start` | Start WiFi Access Point | ✅ | 8080 |
| POST | `/api/v1/network/ap/stop` | Stop WiFi Access Point | ✅ | 8080 |
| GET | `/api/v1/network/ap/status` | Get AP status | ❌ | 8080 |
| GET | `/api/v1/network/ap/config` | Get AP configuration | ✅ | 8080 |
| POST | `/api/v1/network/ap/config` | Set AP configuration | ✅ | 8080 |
| GET | `/api/v1/network/ap/clients` | Get connected clients | ❌ | 8080 |
| POST | `/api/v1/network/ap/clients/kick` | Kick client from AP | ✅ | 8080 |
| GET | `/api/v1/network/ap/statistics` | Get AP statistics | ✅ | 8080 |
| POST | `/api/v1/network/ap/statistics/reset` | Reset AP statistics | ✅ | 8080 |
| POST | `/api/v1/network/fallback/enable` | Enable auto-fallback | ✅ | 8080 |
| GET | `/api/v1/network/fallback/status` | Get fallback status | ❌ | 8080 |
| POST | `/api/v1/network/fallback/trigger` | Trigger fallback | ✅ | 8080 |

### **🌊 WebSocket Real-time**
| Method | Endpoint | Description | Auth Required | Port |
|--------|----------|-------------|---------------|------|
| GET | `/health` | WebSocket server health | ❌ | 8081 |
| GET | `/api/v1/status` | WebSocket server status | ❌ | 8081 |
| GET | `/api/v1/robot/status` | Robot status via WebSocket | ❌ | 8081 |
| WS | `/ws` | WebSocket connection | ❌ | 8081 |

---

## 📊 **API ENDPOINTS SUMMARY**

### **📈 Statistics:**
- **Total Endpoints:** 49 REST API endpoints
- **WebSocket:** 3 direct HTTP endpoints + 1 WebSocket streaming
- **Authentication Required:** 26 endpoints (53%)
- **Public Access:** 23 endpoints (47%)
- **Ports:** 8080 (HTTP), 8081 (WebSocket + HTTP backup)

### **🔒 Authentication Levels:**
- **Admin Token:** `oht50_admin_token_2025` - Full access
- **Operator Token:** `oht50_operator_token_2025` - Control access  
- **Readonly Token:** `oht50_readonly_token_2025` - Monitor only

### **📱 Usage by Category:**
- **LiDAR:** 10 endpoints (20%) - Most comprehensive
- **WiFi AP Mode:** 12 endpoints (24%) - NEW - Fallback connectivity
- **Control:** 8 endpoints (16%) - Robot & motion control
- **System:** 4 endpoints (8%) - Health & monitoring
- **Configuration:** 3 endpoints (6%) - Settings management

---

## 🔌 **WiFi AP Mode APIs - OHT-50 Robot Fallback Connectivity**

### **📋 Overview**
WiFi AP Mode APIs cung cấp quản lý WiFi Access Point cho OHT-50 robots như fallback connectivity khi WiFi connection fails. Bao gồm AP start/stop, client management, và auto-fallback functionality.

### **🔧 Base URL**
```
http://localhost:8080/api/v1/network/ap
```

### **🔐 Authentication**
- **Read Operations:** No authentication required
- **Write Operations:** Bearer token required
  - `oht50_operator_token_2025` - Operator level access
  - `oht50_admin_token_2025` - Admin level access

---

### **📡 AP Management Endpoints**

#### **1. Start WiFi Access Point**
```http
POST /api/v1/network/ap/start
Authorization: Bearer oht50_admin_token_2025
Content-Type: application/json

{
  "ssid": "OHT-50-Hotspot",
  "password": "oht50_secure_2025",
  "security_type": 2,
  "ip_address": "192.168.4.1",
  "netmask": "255.255.255.0",
  "channel": 6,
  "max_clients": 10
}
```

**Response:**
```json
{
  "success": true,
  "message": "WiFi AP started successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

#### **2. Stop WiFi Access Point**
```http
POST /api/v1/network/ap/stop
Authorization: Bearer oht50_admin_token_2025
```

**Response:**
```json
{
  "success": true,
  "message": "WiFi AP stopped successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

#### **3. Get AP Status**
```http
GET /api/v1/network/ap/status
```

**Response:**
```json
{
  "success": true,
  "data": {
    "status": 2,
    "ap_enabled": true,
    "ap_ssid": "OHT-50-Hotspot",
    "ap_ip": "192.168.4.1",
    "ap_channel": 6,
    "connected_clients": 2,
    "max_clients": 10,
    "uptime_seconds": 3600,
    "total_bytes_sent": 1024000,
    "total_bytes_received": 2048000,
    "cpu_usage_percent": 15.0,
    "memory_usage_percent": 25.0
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **⚙️ Configuration Endpoints**

#### **4. Get AP Configuration**
```http
GET /api/v1/network/ap/config
Authorization: Bearer oht50_operator_token_2025
```

**Response:**
```json
{
  "success": true,
  "data": {
    "ap_enabled": true,
    "ap_ssid": "OHT-50-Hotspot",
    "ap_security_type": 2,
    "ap_ip": "192.168.4.1",
    "ap_netmask": "255.255.255.0",
    "ap_channel": 6,
    "max_clients": 10,
    "auto_fallback": true,
    "fallback_timeout_ms": 30000
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

#### **5. Set AP Configuration**
```http
POST /api/v1/network/ap/config
Authorization: Bearer oht50_admin_token_2025
Content-Type: application/json

{
  "ap_enabled": true,
  "ap_ssid": "OHT-50-Hotspot",
  "ap_password": "oht50_secure_2025",
  "ap_security_type": 2,
  "ap_ip": "192.168.4.1",
  "ap_netmask": "255.255.255.0",
  "ap_channel": 6,
  "max_clients": 10,
  "auto_fallback": true,
  "fallback_timeout_ms": 30000
}
```

**Response:**
```json
{
  "success": true,
  "message": "AP configuration updated",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **👥 Client Management Endpoints**

#### **6. Get Connected Clients**
```http
GET /api/v1/network/ap/clients
```

**Response:**
```json
{
  "success": true,
  "data": {
    "clients": [
      {
        "mac_address": "aa:bb:cc:dd:ee:01",
        "ip_address": "192.168.4.2",
        "hostname": "mobile-device-1",
        "signal_strength_dbm": -45,
        "connected_time_seconds": 120,
        "bytes_sent": 1024000,
        "bytes_received": 2048000,
        "authenticated": true
      },
      {
        "mac_address": "aa:bb:cc:dd:ee:02",
        "ip_address": "192.168.4.3",
        "hostname": "tablet-device-1",
        "signal_strength_dbm": -52,
        "connected_time_seconds": 85,
        "bytes_sent": 512000,
        "bytes_received": 1536000,
        "authenticated": true
      }
    ],
    "count": 2
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

#### **7. Kick Client from AP**
```http
POST /api/v1/network/ap/clients/kick
Authorization: Bearer oht50_admin_token_2025
Content-Type: application/json

{
  "mac_address": "aa:bb:cc:dd:ee:01"
}
```

**Response:**
```json
{
  "success": true,
  "message": "Client kicked successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **📊 Statistics Endpoints**

#### **8. Get AP Statistics**
```http
GET /api/v1/network/ap/statistics
Authorization: Bearer oht50_operator_token_2025
```

**Response:**
```json
{
  "success": true,
  "data": {
    "ap_start_count": 5,
    "ap_stop_count": 2,
    "client_connections": 15,
    "client_disconnections": 13,
    "fallback_triggers": 3,
    "total_uptime_seconds": 7200,
    "total_bytes_sent": 10240000,
    "total_bytes_received": 20480000,
    "average_cpu_usage": 15.0,
    "average_memory_usage": 25.0
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

#### **9. Reset AP Statistics**
```http
POST /api/v1/network/ap/statistics/reset
Authorization: Bearer oht50_admin_token_2025
```

**Response:**
```json
{
  "success": true,
  "message": "AP statistics reset successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **🔄 Fallback Management Endpoints**

#### **10. Enable Auto-Fallback**
```http
POST /api/v1/network/fallback/enable
Authorization: Bearer oht50_admin_token_2025
Content-Type: application/json

{
  "enabled": true,
  "timeout_ms": 30000
}
```

**Response:**
```json
{
  "success": true,
  "message": "Auto-fallback configuration updated",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

#### **11. Get Fallback Status**
```http
GET /api/v1/network/fallback/status
```

**Response:**
```json
{
  "success": true,
  "data": {
    "enabled": true,
    "timeout_ms": 30000
  },
  "timestamp": "2025-01-28T10:30:00Z"
}
```

#### **12. Trigger Fallback**
```http
POST /api/v1/network/fallback/trigger
Authorization: Bearer oht50_admin_token_2025
```

**Response:**
```json
{
  "success": true,
  "message": "Fallback triggered successfully",
  "timestamp": "2025-01-28T10:30:00Z"
}
```

---

### **🔒 Security Configuration**

#### **Security Types**
- `0` - Open (no security)
- `2` - WPA2-PSK
- `3` - WPA3-PSK

#### **Validation Rules**
- **SSID:** 1-32 characters
- **Password:** 8-64 characters (for secured networks)
- **Channel:** 1-13 (2.4GHz)
- **IP Address:** Valid IPv4 format
- **Max Clients:** 1-50

---

### **⚡ Performance Requirements**

#### **Response Time Targets**
- **AP Start:** < 10 seconds
- **AP Stop:** < 5 seconds
- **AP Status:** < 100ms
- **Client List:** < 200ms
- **Fallback Trigger:** < 2 seconds

#### **AP Performance**
- **Connection Time:** < 30 seconds
- **Throughput:** > 20Mbps
- **Concurrent Clients:** > 5 clients
- **Range:** > 50 meters

---

### **🧪 Testing Examples**

#### **Test AP Start**
```bash
curl -H "Authorization: Bearer oht50_admin_token_2025" \
     -H "Content-Type: application/json" \
     -X POST http://localhost:8080/api/v1/network/ap/start \
     -d '{"ssid": "OHT-50-Hotspot", "password": "secure_ap_password", "channel": 6}'
```

#### **Test AP Status**
```bash
curl http://localhost:8080/api/v1/network/ap/status
```

#### **Test AP Clients**
```bash
curl http://localhost:8080/api/v1/network/ap/clients
```

#### **Test Fallback Enable**
```bash
curl -H "Authorization: Bearer oht50_admin_token_2025" \
     -H "Content-Type: application/json" \
     -X POST http://localhost:8080/api/v1/network/fallback/enable \
     -d '{"enabled": true, "timeout_ms": 30000}'
```

#### **Test Fallback Status**
```bash
curl http://localhost:8080/api/v1/network/fallback/status
```

---

### **📋 Error Codes**

| Code | Description |
|------|-------------|
| `WIFI_AP_SUCCESS` | Success |
| `WIFI_AP_ERROR_INVALID_PARAM` | Invalid parameter |
| `WIFI_AP_ERROR_INIT_FAILED` | Initialization failed |
| `WIFI_AP_ERROR_START_FAILED` | AP start failed |
| `WIFI_AP_ERROR_STOP_FAILED` | AP stop failed |
| `WIFI_AP_ERROR_NOT_INITIALIZED` | Manager not initialized |
| `WIFI_AP_ERROR_ALREADY_RUNNING` | AP already running |
| `WIFI_AP_ERROR_NOT_RUNNING` | AP not running |
| `WIFI_AP_ERROR_INVALID_SSID` | Invalid SSID |
| `WIFI_AP_ERROR_WEAK_PASSWORD` | Weak password |
| `WIFI_AP_ERROR_HAL_FAILED` | HAL operation failed |

---

### **🔗 Integration Notes**

#### **Frontend Integration**
- Use WebSocket for real-time AP status updates
- Implement fallback status monitoring
- Display connected clients in real-time
- Show AP performance metrics

#### **Backend Integration**
- Monitor AP status via periodic API calls
- Implement fallback trigger logic
- Log AP statistics for analysis
- Handle client management operations

#### **Mobile App Integration**
- Connect to robot AP when WiFi fails
- Display AP connection status
- Show signal strength and quality
- Implement reconnection logic

---

**📋 Generated by Firmware Team - OHT-50 Project**  
**🕒 Date: 2025-01-28**  
**✅ Status: v2.6.0 COMPLETE - WiFi AP Mode APIs Ready & All Systems Operational**  
**🏆 Achievement: 100% GitHub Issues Resolved (11/11 Issues) + WiFi AP Mode Complete**  
**✅ Backend Integration: Complete**  
**✅ Module Data Access: Complete**  
**✅ WebSocket Server: Fixed & Stable**  
**✅ Network Management: Complete (Issue #160)**  
**✅ WiFi AP Mode: Complete (Issue #168)**  
**🚀 Frontend Integration: Ready for Development**
