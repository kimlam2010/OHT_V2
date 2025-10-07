# 🔌 OHT-50 Network Management - Technical Documentation

**Version:** 1.1.0  
**Date:** 2025-01-28  
**Team:** Firmware & Network Integration  
**Status:** 📋 Design Phase - Simplified for OHT-50 Robot  
**Priority:** 🔴 High - Critical for System Connectivity

---

## 🎯 **OVERVIEW**

OHT-50 Network Management system provides **WiFi-based** network connectivity for OHT-50 robot systems. This system is designed specifically for **mobile robots** that move on overhead rails in industrial environments where **WiFi provides continuous connectivity** during movement.

**Key Design Principles:**
- ✅ **WiFi Primary** - 802.11ac industrial WiFi
- ✅ **Roaming Support** - Seamless handover between access points
- ✅ **Industrial Grade** - EMI/EMC compliant, high reliability
- ✅ **Mobile Connectivity** - Continuous connection during movement
- ✅ **Mobile App Support** - Remote control and monitoring

---

## 🏗️ **ARCHITECTURE DESIGN**

### **System Architecture:**
```
┌─────────────────────────────────────────────────────────┐
│                    OHT-50 FIRMWARE                     │
│  ┌─────────────────┐  ┌─────────────────┐              │
│  │   Network       │  │   Protocol      │              │
│  │   Manager       │  │   Gateway       │              │
│  │                 │  │                 │              │
│  │ • WiFi Config   │  │ • HTTP Server   │              │
│  │ • Roaming       │  │ • WebSocket     │              │
│  │ • Handover      │  │ • RS485         │              │
│  │ • Monitoring    │  │ • API Endpoints │              │
│  └─────────────────┘  └─────────────────┘              │
└─────────────────┬─────────────────┬─────────────────────┘
                  │                 │
    ┌─────────────▼─────┐ ┌─────────▼─────────┐
    │   Network Layer   │ │   Hardware Layer  │
    │                   │ │                   │
    │ • WiFi Interface  │ │ • RS485 Modules   │
    │ • Roaming Stack   │ │ • GPIO Controls   │
    │ • Network Stack   │ │ • Safety Systems  │
    └───────────────────┘ └───────────────────┘
```

### **Network Stack:**
```
┌─────────────────────────────────────┐
│         Application Layer           │
│  ┌─────────────────────────────────┐│
│  │     Network Management APIs     ││
│  │  • Status Monitoring            ││
│  │  • Configuration Management     ││
│  │  • WiFi Scanning & Connection   ││
│  │  • Failover Control             ││
│  └─────────────────────────────────┘│
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│         Network Manager             │
│  ┌─────────────────────────────────┐│
│  │  • Connection Management        ││
│  │  • Failover Logic               ││
│  │  • Network Monitoring           ││
│  │  • Configuration Storage        ││
│  └─────────────────────────────────┘│
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│         Linux Network Stack         │
│  ┌─────────────────────────────────┐│
│  │  • Network Interfaces           ││
│  │  • IP Configuration             ││
│  │  │  • DHCP Client               ││
│  │  │  • Static IP                 ││
│  │  • WiFi Management              ││
│  │  │  • wpa_supplicant            ││
│  │  │  • Network Scanning          ││
│  │  • Ethernet Management          ││
│  │  │  • Link Detection            ││
│  │  │  • Speed Negotiation         ││
│  └─────────────────────────────────┘│
└─────────────────┬───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│         Hardware Interfaces         │
│  ┌─────────────────────────────────┐│
│  │  • Ethernet Port (RJ45)         ││
│  │  • WiFi Antenna                 ││
│  │  • Network LEDs                 ││
│  │  • Power Management             ││
│  └─────────────────────────────────┘│
└─────────────────────────────────────┘
```

---

## 🔧 **TECHNICAL SPECIFICATIONS**

### **Hardware Requirements:**
- **Platform:** Orange Pi 5B (RK3588)
- **WiFi:** 802.11ac dual-band (2.4GHz + 5GHz) - **PRIMARY**
- **Antenna:** External WiFi antenna with IP65 rating
- **Industrial Grade:** EMI/EMC compliant
- **LEDs:** Network status indicators
- **Power:** Industrial power supply

### **Software Requirements:**
- **OS:** Linux (Orange Pi OS)
- **Network Stack:** Linux kernel networking
- **WiFi Management:** wpa_supplicant with roaming support
- **DHCP Client:** dhcpcd for dynamic IP
- **Network Tools:** iproute2, iw, iwlist, wpa_cli

### **Performance Requirements:**
- **Connection Time:** < 5 seconds (WiFi connection)
- **Roaming Time:** < 2 seconds (handover between APs)
- **API Response:** < 100ms
- **Network Latency:** < 10ms (WiFi network)
- **Uptime:** > 99.9%
- **Signal Strength:** > -70 dBm (reliable connection)

---

## 📊 **DATA STRUCTURES**

### **Network Configuration:**
```c
typedef struct {
    // Ethernet configuration
    bool ethernet_enabled;
    bool ethernet_dhcp_enabled;
    char ethernet_static_ip[16];
    char ethernet_gateway[16];
    char ethernet_dns[16];
    char ethernet_netmask[16];
    
    // WiFi configuration
    bool wifi_enabled;
    char wifi_ssid[32];
    char wifi_password[64];
    int wifi_security_type; // WPA2, WPA3, etc.
    int wifi_channel;
    bool wifi_hidden;
    
    // Network policies
    int connection_priority; // 1=Ethernet, 2=WiFi
    bool auto_failover_enabled;
    int failover_timeout_ms;
    bool mobile_app_access_enabled;
    int mobile_app_port;
    
    // Advanced settings
    bool network_monitoring_enabled;
    int monitoring_interval_ms;
    bool traffic_logging_enabled;
    int max_retry_attempts;
} network_config_t;
```

### **Network Status:**
```c
typedef struct {
    // Connection status
    bool ethernet_connected;
    bool wifi_connected;
    char active_connection[16]; // "ethernet" or "wifi"
    char primary_connection[16];
    char backup_connection[16];
    
    // Ethernet status
    char ethernet_ip[16];
    char ethernet_mac[18];
    int ethernet_speed; // Mbps
    bool ethernet_link_up;
    uint32_t ethernet_uptime_seconds;
    
    // WiFi status
    char wifi_ssid[32];
    char wifi_ip[16];
    char wifi_mac[18];
    int wifi_signal_strength; // dBm
    int wifi_frequency; // MHz
    int wifi_channel;
    uint32_t wifi_uptime_seconds;
    
    // Network statistics
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t connection_errors;
    uint32_t failover_count;
    
    // Health metrics
    float network_health_score; // 0-100%
    uint32_t last_failover_time;
    bool failover_in_progress;
} network_status_t;
```

### **WiFi Network Information:**
```c
typedef struct {
    char ssid[32];
    char bssid[18];
    int signal_strength; // dBm
    int frequency; // MHz
    int channel;
    char security[16]; // WPA2, WPA3, Open, etc.
    bool hidden;
    uint32_t last_seen;
} wifi_network_t;
```

---

## 🔌 **API ENDPOINTS**

### **Network Status APIs:**

#### **1. Get Network Status**
```http
GET /api/v1/network/status
```
**Response:**
```json
{
  "success": true,
  "data": {
    "ethernet": {
      "connected": true,
      "ip_address": "192.168.1.100",
      "mac_address": "00:11:22:33:44:55",
      "speed": 1000,
      "link_up": true,
      "uptime_seconds": 3600
    },
    "wifi": {
      "connected": false,
      "ssid": "",
      "ip_address": "",
      "mac_address": "00:11:22:33:44:66",
      "signal_strength": 0,
      "frequency": 0,
      "channel": 0,
      "uptime_seconds": 0
    },
    "active_connection": "ethernet",
    "primary_connection": "ethernet",
    "backup_connection": "wifi",
    "statistics": {
      "bytes_sent": 1024000,
      "bytes_received": 2048000,
      "packets_sent": 1500,
      "packets_received": 2000,
      "connection_errors": 0,
      "failover_count": 2
    },
    "health": {
      "network_health_score": 95.5,
      "last_failover_time": 1706441000,
      "failover_in_progress": false
    }
  }
}
```

#### **2. Get Network Configuration**
```http
GET /api/v1/network/config
```
**Response:**
```json
{
  "success": true,
  "data": {
    "ethernet": {
      "enabled": true,
      "dhcp_enabled": true,
      "static_ip": "192.168.1.100",
      "gateway": "192.168.1.1",
      "dns": "8.8.8.8",
      "netmask": "255.255.255.0"
    },
    "wifi": {
      "enabled": true,
      "ssid": "OHT-50-Network",
      "security_type": "WPA2",
      "channel": 6,
      "hidden": false
    },
    "policies": {
      "connection_priority": 1,
      "auto_failover_enabled": true,
      "failover_timeout_ms": 5000,
      "mobile_app_access": true,
      "mobile_app_port": 8080
    },
    "advanced": {
      "network_monitoring_enabled": true,
      "monitoring_interval_ms": 1000,
      "traffic_logging_enabled": true,
      "max_retry_attempts": 3
    }
  }
}
```

### **Network Configuration APIs:**

#### **3. Update Network Configuration**
```http
POST /api/v1/network/config
Authorization: Bearer oht50_admin_token_2025
Content-Type: application/json
```
**Request:**
```json
{
  "ethernet": {
    "enabled": true,
    "dhcp_enabled": false,
    "static_ip": "192.168.1.100",
    "gateway": "192.168.1.1",
    "dns": "8.8.8.8",
    "netmask": "255.255.255.0"
  },
  "wifi": {
    "enabled": true,
    "ssid": "Factory-WiFi",
    "password": "secure_password",
    "security_type": "WPA2",
    "channel": 11,
    "hidden": false
  },
  "policies": {
    "connection_priority": 1,
    "auto_failover_enabled": true,
    "failover_timeout_ms": 3000,
    "mobile_app_access": true,
    "mobile_app_port": 8080
  },
  "advanced": {
    "network_monitoring_enabled": true,
    "monitoring_interval_ms": 500,
    "traffic_logging_enabled": true,
    "max_retry_attempts": 5
  }
}
```

### **WiFi Management APIs:**

#### **4. Scan WiFi Networks**
```http
GET /api/v1/network/wifi/scan
```
**Response:**
```json
{
  "success": true,
  "data": {
    "networks": [
      {
        "ssid": "OHT-50-Network",
        "bssid": "00:11:22:33:44:55",
        "signal_strength": -45,
        "frequency": 2437,
        "channel": 6,
        "security": "WPA2",
        "hidden": false,
        "last_seen": 1706441400
      },
      {
        "ssid": "Factory-WiFi",
        "bssid": "00:11:22:33:44:66",
        "signal_strength": -60,
        "frequency": 2462,
        "channel": 11,
        "security": "WPA3",
        "hidden": false,
        "last_seen": 1706441400
      }
    ],
    "scan_time": 1706441400,
    "total_networks": 2
  }
}
```

#### **5. Connect to WiFi**
```http
POST /api/v1/network/wifi/connect
Authorization: Bearer oht50_operator_token_2025
Content-Type: application/json
```
**Request:**
```json
{
  "ssid": "OHT-50-Network",
  "password": "secure_password",
  "security_type": "WPA2",
  "hidden": false
}
```

#### **6. Disconnect WiFi**
```http
POST /api/v1/network/wifi/disconnect
Authorization: Bearer oht50_operator_token_2025
```

### **Network Control APIs:**

#### **7. Switch Network Connection**
```http
POST /api/v1/network/switch
Authorization: Bearer oht50_operator_token_2025
Content-Type: application/json
```
**Request:**
```json
{
  "connection_type": "wifi",
  "reason": "Manual switch for testing",
  "force_switch": false
}
```

#### **8. Enable/Disable Failover**
```http
POST /api/v1/network/failover
Authorization: Bearer oht50_admin_token_2025
Content-Type: application/json
```
**Request:**
```json
{
  "enabled": true,
  "timeout_ms": 5000,
  "retry_attempts": 3
}
```

---

## 🔒 **SECURITY IMPLEMENTATION**

### **Authentication Levels:**
- **Admin Token:** Full network configuration access
- **Operator Token:** WiFi connection và network switching
- **Read-only Access:** Network status và monitoring

### **Security Features:**
- **WiFi Encryption:** WPA2/WPA3 support
- **Network Isolation:** Separate networks for different purposes
- **Access Control:** Role-based network access
- **Audit Logging:** All network changes logged
- **Configuration Validation:** Input validation và sanitization

### **Security Headers:**
```c
// Security validation
int network_config_validate(const network_config_t *config) {
    // Validate IP addresses
    if (!is_valid_ip(config->ethernet_static_ip)) {
        return NETWORK_ERROR_INVALID_IP;
    }
    
    // Validate WiFi credentials
    if (strlen(config->wifi_password) < 8) {
        return NETWORK_ERROR_WEAK_PASSWORD;
    }
    
    // Validate security settings
    if (config->wifi_security_type < WPA2) {
        return NETWORK_ERROR_INSECURE_WIFI;
    }
    
    return NETWORK_SUCCESS;
}
```

---

## 📊 **MONITORING & DIAGNOSTICS**

### **Network Health Monitoring:**
```c
typedef struct {
    float connection_stability; // 0-100%
    float signal_quality; // 0-100%
    float throughput_efficiency; // 0-100%
    uint32_t error_rate; // errors per hour
    uint32_t latency_ms; // average latency
    bool network_healthy;
} network_health_t;

// Health monitoring functions
int network_monitor_get_health(network_health_t *health);
int network_monitor_start_monitoring(int interval_ms);
int network_monitor_stop_monitoring(void);
int network_monitor_get_statistics(network_stats_t *stats);
```

### **Diagnostic Commands:**
```c
// Network diagnostics
int network_diagnostic_ping(const char *host, int timeout_ms);
int network_diagnostic_traceroute(const char *host);
int network_diagnostic_speed_test(void);
int network_diagnostic_interface_status(void);
int network_diagnostic_wifi_scan(void);
```

---

## 🧪 **TESTING FRAMEWORK**

### **Unit Tests:**
```c
// Network configuration tests
void test_network_config_validation(void);
void test_network_config_save_load(void);
void test_network_config_reset(void);

// Network connection tests
void test_ethernet_connection(void);
void test_wifi_connection(void);
void test_network_failover(void);

// Network monitoring tests
void test_network_status_monitoring(void);
void test_network_health_monitoring(void);
void test_network_statistics(void);
```

### **Integration Tests:**
```bash
# Test network status API
curl http://localhost:8080/api/v1/network/status

# Test network configuration API
curl -H "Authorization: Bearer oht50_admin_token_2025" \
     -H "Content-Type: application/json" \
     -X POST http://localhost:8080/api/v1/network/config \
     -d '{"ethernet": {"enabled": true, "dhcp_enabled": true}}'

# Test WiFi scan API
curl http://localhost:8080/api/v1/network/wifi/scan

# Test WiFi connection API
curl -H "Authorization: Bearer oht50_operator_token_2025" \
     -H "Content-Type: application/json" \
     -X POST http://localhost:8080/api/v1/network/wifi/connect \
     -d '{"ssid": "Test-Network", "password": "test123", "security_type": "WPA2"}'

# Test network failover API
curl -H "Authorization: Bearer oht50_admin_token_2025" \
     -H "Content-Type: application/json" \
     -X POST http://localhost:8080/api/v1/network/failover \
     -d '{"enabled": true, "timeout_ms": 5000}'
```

### **Performance Tests:**
```c
// Network performance tests
void test_network_connection_speed(void);
void test_network_failover_time(void);
void test_network_api_response_time(void);
void test_network_throughput(void);
void test_network_latency(void);
```

---

## 📋 **IMPLEMENTATION CHECKLIST**

### **Phase 1 - Core Network Management:**
- [ ] Network Manager initialization
- [ ] Ethernet configuration (DHCP/Static)
- [ ] Basic WiFi connection
- [ ] Network status monitoring
- [ ] Configuration storage

### **Phase 2 - Advanced Features:**
- [ ] WiFi network scanning
- [ ] Network failover logic
- [ ] Network health monitoring
- [ ] Traffic statistics
- [ ] Mobile app access

### **Phase 3 - Optimization:**
- [ ] Performance optimization
- [ ] Advanced diagnostics
- [ ] Network security hardening
- [ ] Comprehensive testing
- [ ] Documentation completion

---

## 🔗 **DEPENDENCIES**

### **System Dependencies:**
- **Linux Kernel:** Network stack support
- **wpa_supplicant:** WiFi management
- **dhcpcd:** DHCP client
- **iproute2:** Network configuration tools
- **iw/iwlist:** WiFi scanning tools

### **Firmware Dependencies:**
- **API Framework:** HTTP server và WebSocket
- **Configuration Manager:** Settings storage
- **Logging System:** Network event logging
- **Security Framework:** Authentication và authorization

---

## 📝 **CHANGELOG**

### **v1.0.0 (2025-01-28) - Initial Design**
- ✅ Network architecture design
- ✅ API endpoints specification
- ✅ Data structures definition
- ✅ Security implementation plan
- ✅ Testing framework design
- ✅ Implementation roadmap

---

**📋 Generated by Firmware Team - OHT-50 Project**  
**🕒 Date: 2025-01-28**  
**✅ Status: v1.0 DESIGN COMPLETE**  
**🎯 Next: Implementation Phase**
