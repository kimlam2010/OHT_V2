# Safety Configuration Guide

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-01-28  
**Module:** Safety Configuration  
**Team:** FIRMWARE  
**Task:** LIDAR Safety Integration

---

## 📋 **TỔNG QUAN**

Safety Configuration Guide mô tả cách cấu hình safety system của OHT-50 Master Module, bao gồm safety zones, monitoring parameters, và system behavior.

---

## 🎯 **CONFIGURATION OVERVIEW**

### **Configuration Categories**
- **Safety Zones:** Emergency, Warning, Safe zone distances
- **Monitoring Parameters:** Timeouts, check periods, enable flags
- **System Behavior:** E-Stop behavior, LED patterns, logging
- **Performance Tuning:** Update rates, response times

### **Configuration Sources**
1. **Factory Defaults:** Built-in safe defaults
2. **Persistent Storage:** `/etc/oht50/config.json`
3. **API Interface:** REST API endpoints
4. **Runtime Updates:** Dynamic configuration changes

---

## 🔧 **SAFETY ZONES CONFIGURATION**

### **Zone Definitions**
```c
typedef struct {
    bool enabled;                    // Enable/disable zones
    uint16_t emergency_zone_mm;      // Emergency zone distance
    uint16_t warning_zone_mm;        // Warning zone distance
    uint16_t safe_zone_mm;           // Safe zone distance
} basic_safety_zones_t;
```

### **Default Zone Values**
| Zone | Distance | Description | Action |
|------|----------|-------------|--------|
| **Emergency** | 500mm | Critical safety zone | Immediate E-Stop |
| **Warning** | 1000mm | Warning zone | Reduce speed, warning |
| **Safe** | 2000mm | Safe operation zone | Normal monitoring |

### **Zone Validation Rules**
```c
// Zone distances must be in ascending order
if (emergency_zone_mm >= warning_zone_mm ||
    warning_zone_mm >= safe_zone_mm) {
    return HAL_STATUS_INVALID_PARAMETER;
}

// Minimum distances
if (emergency_zone_mm < 100) return HAL_STATUS_INVALID_PARAMETER;
if (warning_zone_mm < 200) return HAL_STATUS_INVALID_PARAMETER;
if (safe_zone_mm < 500) return HAL_STATUS_INVALID_PARAMETER;

// Maximum distances
if (emergency_zone_mm > 2000) return HAL_STATUS_INVALID_PARAMETER;
if (warning_zone_mm > 5000) return HAL_STATUS_INVALID_PARAMETER;
if (safe_zone_mm > 10000) return HAL_STATUS_INVALID_PARAMETER;
```

---

## ⚙️ **MONITORING PARAMETERS**

### **Timeout Configuration**
```c
typedef struct {
    uint32_t estop_timeout_ms;           // E-Stop response timeout
    uint32_t zone_check_period_ms;       // Zone check frequency
    uint32_t interlock_check_period_ms;  // Interlock check frequency
    uint32_t sensor_check_period_ms;     // Sensor check frequency
    uint32_t watchdog_timeout_ms;        // Watchdog timeout
} safety_monitor_config_t;
```

### **Default Timeout Values**
| Parameter | Value | Description |
|-----------|-------|-------------|
| **estop_timeout_ms** | 100ms | E-Stop response timeout |
| **zone_check_period_ms** | 50ms | Zone check frequency (20Hz) |
| **interlock_check_period_ms** | 100ms | Interlock check frequency (10Hz) |
| **sensor_check_period_ms** | 200ms | Sensor check frequency (5Hz) |
| **watchdog_timeout_ms** | 1000ms | Watchdog timeout |

### **Enable Flags**
```c
typedef struct {
    bool enable_zone_monitoring;      // Enable zone monitoring
    bool enable_interlock_monitoring; // Enable interlock monitoring
    bool enable_sensor_monitoring;    // Enable sensor monitoring
    bool enable_watchdog_monitoring;  // Enable watchdog monitoring
} safety_monitor_config_t;
```

---

## 📄 **JSON CONFIGURATION FORMAT**

### **Complete Configuration Schema**
```json
{
  "safety_config": {
    "version": "1.0.0",
    "timestamp": 1234567890,
    "safety_zones": {
      "enabled": true,
      "emergency_zone_mm": 500,
      "warning_zone_mm": 1000,
      "safe_zone_mm": 2000
    },
    "monitor_config": {
      "estop_timeout_ms": 100,
      "zone_check_period_ms": 50,
      "interlock_check_period_ms": 100,
      "sensor_check_period_ms": 200,
      "watchdog_timeout_ms": 1000,
      "enable_zone_monitoring": true,
      "enable_interlock_monitoring": true,
      "enable_sensor_monitoring": true,
      "enable_watchdog_monitoring": true
    },
    "led_config": {
      "enable_visual_feedback": true,
      "blink_fast_period_ms": 100,
      "blink_slow_period_ms": 500
    },
    "logging_config": {
      "enable_safety_logging": true,
      "log_level": "INFO",
      "max_log_entries": 1000
    }
  }
}
```

### **Minimal Configuration**
```json
{
  "safety_zones": {
    "enabled": true,
    "emergency_zone_mm": 500,
    "warning_zone_mm": 1000,
    "safe_zone_mm": 2000
  }
}
```

---

## 🔌 **API CONFIGURATION ENDPOINTS**

### **Get Current Configuration**
```bash
GET /api/v1/safety/config
```

**Response:**
```json
{
  "status": "success",
  "data": {
    "safety_config": {
      "version": "1.0.0",
      "timestamp": 1234567890,
      "safety_zones": {
        "enabled": true,
        "emergency_zone_mm": 500,
        "warning_zone_mm": 1000,
        "safe_zone_mm": 2000
      },
      "monitor_config": {
        "estop_timeout_ms": 100,
        "zone_check_period_ms": 50,
        "interlock_check_period_ms": 100,
        "sensor_check_period_ms": 200,
        "watchdog_timeout_ms": 1000,
        "enable_zone_monitoring": true,
        "enable_interlock_monitoring": true,
        "enable_sensor_monitoring": true,
        "enable_watchdog_monitoring": true
      }
    }
  }
}
```

### **Update Configuration**
```bash
PUT /api/v1/safety/config
Content-Type: application/json

{
  "safety_zones": {
    "enabled": true,
    "emergency_zone_mm": 400,
    "warning_zone_mm": 800,
    "safe_zone_mm": 1500
  },
  "monitor_config": {
    "zone_check_period_ms": 25,
    "estop_timeout_ms": 50
  }
}
```

### **Export Configuration**
```bash
GET /api/v1/safety/config/export
```

**Response:** Configuration file download

### **Import Configuration**
```bash
POST /api/v1/safety/config/import
Content-Type: application/json

{
  "safety_config": {
    "version": "1.0.0",
    "safety_zones": {
      "enabled": true,
      "emergency_zone_mm": 500,
      "warning_zone_mm": 1000,
      "safe_zone_mm": 2000
    }
  }
}
```

### **Reset to Factory Defaults**
```bash
POST /api/v1/safety/config/reset
```

**Response:**
```json
{
  "status": "success",
  "message": "Configuration reset to factory defaults"
}
```

---

## 🎨 **LED CONFIGURATION**

### **LED Pattern Configuration**
```c
typedef struct {
    bool enable_visual_feedback;     // Enable LED feedback
    uint32_t blink_fast_period_ms;   // Fast blink period
    uint32_t blink_slow_period_ms;   // Slow blink period
} led_config_t;
```

### **Default LED Patterns**
| State | Power LED | System LED | Comm LED | Network LED | Error LED |
|-------|-----------|------------|----------|-------------|-----------|
| **Safe** | ON (Green) | ON (Blue) | ON (Yellow) | ON (Green) | OFF |
| **Warning** | ON (Green) | FAST (Blue) | SLOW (Yellow) | ON (Green) | OFF |
| **Critical** | ON (Green) | FAST (Blue) | FAST (Yellow) | SLOW (Green) | SLOW (Red) |
| **E-Stop** | ON (Green) | OFF | OFF | OFF | FAST (Red) |
| **Fault** | ON (Green) | OFF | OFF | OFF | SLOW (Red) |

### **LED Configuration API**
```bash
# Update LED configuration
PUT /api/v1/safety/config
{
  "led_config": {
    "enable_visual_feedback": true,
    "blink_fast_period_ms": 100,
    "blink_slow_period_ms": 500
  }
}
```

---

## 📊 **PERFORMANCE TUNING**

### **Update Rate Configuration**
```c
// High performance mode (50Hz)
zone_check_period_ms = 20;      // 50Hz zone checking
estop_timeout_ms = 50;          // 50ms E-Stop response

// Standard mode (20Hz)
zone_check_period_ms = 50;      // 20Hz zone checking
estop_timeout_ms = 100;         // 100ms E-Stop response

// Low power mode (10Hz)
zone_check_period_ms = 100;     // 10Hz zone checking
estop_timeout_ms = 200;         // 200ms E-Stop response
```

### **Memory Usage Optimization**
```c
// Minimal configuration
enable_zone_monitoring = true;
enable_interlock_monitoring = false;
enable_sensor_monitoring = false;
enable_watchdog_monitoring = false;

// Full monitoring
enable_zone_monitoring = true;
enable_interlock_monitoring = true;
enable_sensor_monitoring = true;
enable_watchdog_monitoring = true;
```

---

## 🔒 **SECURITY CONFIGURATION**

### **Access Control**
```c
typedef struct {
    bool require_authentication;     // Require API authentication
    bool require_admin_for_config;   // Require admin for config changes
    bool enable_audit_logging;       // Enable configuration audit logs
} security_config_t;
```

### **Configuration Validation**
```c
// Validate zone configuration
bool validate_zones(const basic_safety_zones_t *zones) {
    if (!zones->enabled) return true;
    
    // Check distance order
    if (zones->emergency_zone_mm >= zones->warning_zone_mm) return false;
    if (zones->warning_zone_mm >= zones->safe_zone_mm) return false;
    
    // Check minimum distances
    if (zones->emergency_zone_mm < 100) return false;
    if (zones->warning_zone_mm < 200) return false;
    if (zones->safe_zone_mm < 500) return false;
    
    return true;
}
```

---

## 🔄 **CONFIGURATION PERSISTENCE**

### **Storage Locations**
```
/etc/oht50/
├── config.json              # Primary configuration
├── config_backup.json       # Backup configuration
├── config_factory.json      # Factory defaults
└── config_history/          # Configuration history
    ├── config_v1.0.0.json
    ├── config_v1.0.1.json
    └── config_v1.1.0.json
```

### **Configuration Loading Order**
1. **Factory Defaults:** Built-in safe defaults
2. **Backup Configuration:** Last known good configuration
3. **Primary Configuration:** Current configuration
4. **Runtime Updates:** Dynamic configuration changes

### **Configuration Backup**
```bash
# Automatic backup before changes
safety_monitor_save_config();

# Manual backup
cp /etc/oht50/config.json /etc/oht50/config_backup_$(date +%Y%m%d_%H%M%S).json

# Restore from backup
cp /etc/oht50/config_backup.json /etc/oht50/config.json
safety_monitor_load_config();
```

---

## ⚠️ **TROUBLESHOOTING**

### **Configuration Issues**

#### **1. Invalid Zone Configuration**
```bash
# Check zone configuration
curl -X GET "http://localhost:8000/api/v1/safety/zones"

# Validate configuration
safety_monitor_validate_config

# Reset to factory defaults
curl -X POST "http://localhost:8000/api/v1/safety/config/reset"
```

#### **2. Configuration Not Loading**
```bash
# Check configuration file
cat /etc/oht50/config.json

# Check file permissions
ls -la /etc/oht50/config.json

# Check configuration status
safety_monitor_get_status
```

#### **3. Configuration Not Saving**
```bash
# Check storage space
df -h /etc/oht50/

# Check write permissions
ls -la /etc/oht50/

# Test configuration save
safety_monitor_save_config
```

### **Debug Commands**
```bash
# Enable configuration debug
export CONFIG_DEBUG=1

# Check configuration logs
tail -f /var/log/safety_config.log

# Monitor configuration changes
watch -n 1 'curl -s http://localhost:8000/api/v1/safety/config | jq'
```

---

## 📋 **CONFIGURATION CHECKLIST**

### **Pre-Deployment Checklist**
- [ ] **Zone Configuration:** Valid zone distances
- [ ] **Timeout Values:** Reasonable timeout values
- [ ] **Enable Flags:** Required monitoring enabled
- [ ] **LED Configuration:** Visual feedback configured
- [ ] **Security Settings:** Access control configured
- [ ] **Performance Settings:** Update rates appropriate
- [ ] **Backup Configuration:** Backup created
- [ ] **Validation Tests:** Configuration validated

### **Post-Deployment Checklist**
- [ ] **Configuration Loaded:** Configuration loaded successfully
- [ ] **API Endpoints:** All endpoints responding
- [ ] **Safety Zones:** Zones working correctly
- [ ] **LED Indicators:** Visual feedback working
- [ ] **E-Stop Response:** E-Stop triggering correctly
- [ ] **Performance:** Response times acceptable
- [ ] **Logging:** Configuration changes logged
- [ ] **Backup:** Configuration backed up

---

## 🔄 **MAINTENANCE PROCEDURES**

### **Regular Maintenance**
- **Daily:** Check configuration status
- **Weekly:** Validate zone configuration
- **Monthly:** Backup configuration
- **Quarterly:** Review performance settings
- **Annually:** Full configuration audit

### **Configuration Updates**
```bash
# 1. Backup current configuration
curl -X GET "http://localhost:8000/api/v1/safety/config/export" > backup.json

# 2. Update configuration
curl -X PUT "http://localhost:8000/api/v1/safety/config" \
  -H "Content-Type: application/json" \
  -d @new_config.json

# 3. Validate configuration
curl -X GET "http://localhost:8000/api/v1/safety/config"

# 4. Test safety system
# ... perform safety tests ...

# 5. Rollback if needed
curl -X POST "http://localhost:8000/api/v1/safety/config/import" \
  -H "Content-Type: application/json" \
  -d @backup.json
```

---

**Changelog v1.0:**
- ✅ Created comprehensive configuration guide
- ✅ Added JSON configuration format
- ✅ Added API endpoints documentation
- ✅ Added LED configuration guide
- ✅ Added performance tuning guide
- ✅ Added security configuration
- ✅ Added troubleshooting section
- ✅ Added maintenance procedures

**🚨 Lưu ý:** Luôn backup configuration trước khi thay đổi và validate sau khi apply.
