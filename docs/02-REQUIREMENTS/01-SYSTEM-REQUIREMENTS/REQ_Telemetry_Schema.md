# Telemetry Schema - OHT-50 (v2.0)

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated với location và navigation data

---

## 🎯 **MỤC TIÊU**

Chuẩn hoá schema telemetry cho API/WS, khớp `backend/app/models/telemetry.py` và bao gồm location-based data.

---

## 📊 **JSON SCHEMA**

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "TelemetryData",
  "type": "object",
  "properties": {
    "ts": { "type": "number", "description": "Unix timestamp" },
    "status": { "$ref": "#/definitions/TelemetryStatus" },
    "location": { "$ref": "#/definitions/TelemetryLocation" },
    "navigation": { "$ref": "#/definitions/TelemetryNavigation" },
    "dock": { "$ref": "#/definitions/TelemetryDock" },
    "mission_id": { "type": ["string", "null"] },
    "station_id": { "type": ["string", "null"] },
    "cargo_present": { "type": "boolean" },
    "cargo_locked": { "type": "boolean" },
    "cpu_usage": { "type": ["number", "null"] },
    "memory_usage": { "type": ["number", "null"] },
    "temperature": { "type": ["number", "null"] },
    "center_connected": { "type": "boolean" },
    "rs485_connected": { "type": "boolean" },
    "tag_id": { "type": ["string", "null"] },
    "rssi": { "type": ["integer", "null"] }
  },
  "required": ["ts", "status", "location"],
  "definitions": {
    "TelemetryLocation": {
      "type": "object",
      "properties": {
        "position": {
          "type": "object",
          "properties": {
            "x": { "type": "number", "description": "X position in mm" },
            "y": { "type": "number", "description": "Y position in mm" },
            "z": { "type": "number", "description": "Z position in mm" }
          },
          "required": ["x", "y", "z"]
        },
        "orientation": {
          "type": "object",
          "properties": {
            "pitch": { "type": "number", "description": "Pitch angle in degrees" },
            "roll": { "type": "number", "description": "Roll angle in degrees" },
            "yaw": { "type": "number", "description": "Yaw angle in degrees" }
          },
          "required": ["pitch", "roll", "yaw"]
        },
        "velocity": {
          "type": "object",
          "properties": {
            "x": { "type": "number", "description": "X velocity in mm/s" },
            "y": { "type": "number", "description": "Y velocity in mm/s" },
            "z": { "type": "number", "description": "Z velocity in mm/s" }
          },
          "required": ["x", "y", "z"]
        },
        "acceleration": {
          "type": "object",
          "properties": {
            "x": { "type": "number", "description": "X acceleration in mm/s²" },
            "y": { "type": "number", "description": "Y acceleration in mm/s²" },
            "z": { "type": "number", "description": "Z acceleration in mm/s²" }
          },
          "required": ["x", "y", "z"]
        },
        "accuracy": { "type": "number", "description": "Position accuracy in mm" },
        "system_status": { "type": "string", "enum": ["ok", "error", "calibrating"] },
        "imu_status": { "type": "string", "enum": ["ok", "error"] },
        "magnetic_status": { "type": "string", "enum": ["ok", "error"] }
      },
      "required": ["position", "orientation", "accuracy", "system_status"]
    },
    "TelemetryNavigation": {
      "type": "object",
      "properties": {
        "status": { "type": "string", "enum": ["idle", "navigating", "positioning", "arrived", "error"] },
        "target": {
          "type": "object",
          "properties": {
            "x": { "type": "number", "description": "Target X position in mm" },
            "y": { "type": "number", "description": "Target Y position in mm" },
            "z": { "type": "number", "description": "Target Z position in mm" }
          },
          "required": ["x", "y", "z"]
        },
        "progress": { "type": "number", "description": "Navigation progress in %", "minimum": 0, "maximum": 100 },
        "distance_to_target": { "type": "number", "description": "Distance to target in mm" },
        "estimated_time": { "type": "number", "description": "Estimated time to target in seconds" },
        "path_clear": { "type": "boolean", "description": "Path is clear of obstacles" },
        "obstacle_detected": { "type": "boolean", "description": "Obstacle detected" },
        "obstacle_distance": { "type": ["number", "null"], "description": "Distance to obstacle in mm" },
        "obstacle_angle": { "type": ["number", "null"], "description": "Angle to obstacle in degrees" },
        "speed_limit": { "type": "number", "description": "Current speed limit in mm/s" },
        "zone_type": { "type": "string", "enum": ["safe", "warning", "restricted", "emergency"] }
      },
      "required": ["status", "target", "progress", "distance_to_target", "path_clear", "obstacle_detected"]
    },
    "TelemetryDock": {
      "type": "object",
      "properties": {
        "status": { "type": "string", "enum": ["not_docking", "approaching", "aligning", "docked", "undocking", "error"] },
        "station_id": { "type": ["string", "null"], "description": "Current station ID" },
        "target_station_id": { "type": ["string", "null"], "description": "Target station ID" },
        "alignment_status": { "type": "string", "enum": ["not_aligned", "aligning", "aligned", "error"] },
        "distance": { "type": "number", "description": "Distance to dock in mm" },
        "angle": { "type": "number", "description": "Alignment angle in degrees" },
        "charging_status": { "type": "string", "enum": ["not_charging", "charging", "full", "error"] },
        "charging_current": { "type": ["number", "null"], "description": "Charging current in mA" },
        "charging_voltage": { "type": ["number", "null"], "description": "Charging voltage in V" },
        "rfid_tag_id": { "type": ["string", "null"], "description": "RFID tag ID" },
        "rfid_signal_strength": { "type": ["integer", "null"], "description": "RFID signal strength (RSSI)" },
        "safety_status": { "type": "string", "enum": ["safe", "warning", "danger"] }
      },
      "required": ["status", "alignment_status", "distance", "angle", "charging_status", "safety_status"]
    },
    "TelemetryTarget": {
      "type": "object",
      "properties": {
        "pos_mm": { "type": "number" },
        "vel_mms": { "type": "number" }
      },
      "required": ["pos_mm", "vel_mms"]
    },
    "TelemetrySafety": {
      "type": "object",
      "properties": {
        "estop": { "type": "boolean" },
        "zone_blocked": { "type": "boolean" },
        "interlock_active": { "type": "boolean" },
        "location_safe": { "type": "boolean", "description": "Location-based safety OK" },
        "obstacle_detected": { "type": "boolean", "description": "Obstacle detected" },
        "safety_zone_clear": { "type": "boolean", "description": "Safety zone is clear" },
        "emergency_stop": { "type": "boolean", "description": "Emergency stop active" }
      },
      "required": ["estop", "zone_blocked", "interlock_active", "location_safe", "obstacle_detected", "safety_zone_clear", "emergency_stop"]
    },
    "TelemetryEncoder": {
      "type": "object",
      "properties": {
        "health": { "type": "string" },
        "fault": { "type": ["string", "null"] },
        "count": { "type": "integer" }
      },
      "required": ["health", "count"]
    },
    "TelemetryStatus": {
      "type": "object",
      "properties": {
        "state": { "type": "string", "enum": ["idle", "move", "navigating", "positioning", "dock", "undocking", "fault", "estop"] },
        "pos_mm": { "type": "number" },
        "vel_mms": { "type": "number" },
        "acc_mms2": { "type": "number" },
        "target": { "$ref": "#/definitions/TelemetryTarget" },
        "safety": { "$ref": "#/definitions/TelemetrySafety" },
        "enc": { "$ref": "#/definitions/TelemetryEncoder" },
        "errors": { "type": "array", "items": { "type": "string" } },
        "warn": { "type": "array", "items": { "type": "string" } }
      },
      "required": ["state", "pos_mm", "vel_mms", "acc_mms2", "target", "safety", "enc", "errors", "warn"]
    }
  }
}
```

---

## 📡 **TELEMETRY EVENTS**

### **Location Update Event:**
```javascript
{
  "event": "location_update",
  "data": {
    "ts": 1640995200000,
    "location": {
      "position": {"x": 1000, "y": 2000, "z": 0},
      "orientation": {"pitch": 0, "roll": 0, "yaw": 45},
      "velocity": {"x": 100, "y": 0, "z": 0},
      "acceleration": {"x": 0, "y": 0, "z": 0},
      "accuracy": 10,
      "system_status": "ok",
      "imu_status": "ok",
      "magnetic_status": "ok"
    }
  }
}
```

### **Navigation Update Event:**
```javascript
{
  "event": "navigation_update",
  "data": {
    "ts": 1640995200000,
    "navigation": {
      "status": "navigating",
      "target": {"x": 1500, "y": 2500, "z": 0},
      "progress": 75,
      "distance_to_target": 500,
      "estimated_time": 30,
      "path_clear": true,
      "obstacle_detected": false,
      "speed_limit": 1000,
      "zone_type": "safe"
    }
  }
}
```

### **Dock Update Event:**
```javascript
{
  "event": "dock_update",
  "data": {
    "ts": 1640995200000,
    "dock": {
      "status": "aligning",
      "station_id": "STATION_001",
      "target_station_id": "STATION_001",
      "alignment_status": "aligning",
      "distance": 50,
      "angle": 2.5,
      "charging_status": "not_charging",
      "charging_current": null,
      "charging_voltage": null,
      "rfid_tag_id": "TAG_001",
      "rfid_signal_strength": -45,
      "safety_status": "safe"
    }
  }
}
```

### **Safety Update Event:**
```javascript
{
  "event": "safety_update",
  "data": {
    "ts": 1640995200000,
    "safety": {
      "estop": false,
      "zone_blocked": false,
      "interlock_active": false,
      "location_safe": true,
      "obstacle_detected": false,
      "safety_zone_clear": true,
      "emergency_stop": false
    }
  }
}
```

---

## 🔄 **DATA FLOW**

### **Telemetry Data Sources:**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Location      │    │   Navigation    │    │   Dock          │
│   Module        │───▶│   System        │───▶│   Module        │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Master Module                               │
│  • Data aggregation                                            │
│  • Event filtering                                             │
│  • Rate limiting                                               │
│  • Priority management                                         │
└─────────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Backend/Center                              │
│  • Data storage                                                │
│  • Real-time processing                                        │
│  • Analytics                                                   │
│  • Alerting                                                    │
└─────────────────────────────────────────────────────────────────┘
```

---

## ⏱️ **UPDATE FREQUENCIES**

### **High Priority (50ms):**
- Safety status
- E-Stop status
- Emergency events

### **Medium Priority (100ms):**
- Location position
- Navigation status
- Dock status

### **Low Priority (500ms):**
- System health
- Performance metrics
- Configuration status

### **Event-Based:**
- Error conditions
- State changes
- User commands
- System events

---

## 📊 **DATA STORAGE**

### **Local Storage:**
```c
// Ring buffer for local storage
#define TELEMETRY_BUFFER_SIZE 10000
#define TELEMETRY_RETENTION_DAYS 7

typedef struct {
    uint64_t timestamp;
    telemetry_data_t data;
    uint8_t priority;
    uint8_t source;
} telemetry_entry_t;
```

### **Remote Storage:**
```sql
-- Telemetry table structure
CREATE TABLE telemetry (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP,
    location_data JSONB,
    navigation_data JSONB,
    dock_data JSONB,
    safety_data JSONB,
    system_data JSONB,
    created_at TIMESTAMP DEFAULT NOW()
);

-- Indexes for efficient querying
CREATE INDEX idx_telemetry_timestamp ON telemetry(timestamp);
CREATE INDEX idx_telemetry_location ON telemetry USING GIN(location_data);
CREATE INDEX idx_telemetry_navigation ON telemetry USING GIN(navigation_data);
```

---

## 🔒 **SECURITY CONSIDERATIONS**

### **Data Privacy:**
- Location data encryption
- Access control for sensitive data
- Audit trail for data access
- Data retention policies

### **Data Integrity:**
- Checksum validation
- Timestamp synchronization
- Data validation
- Error detection và correction

---

## 📋 **IMPLEMENTATION REQUIREMENTS**

### **Backend Models:**
```python
# Location model
class LocationData(BaseModel):
    position: Position
    orientation: Orientation
    velocity: Velocity
    acceleration: Acceleration
    accuracy: float
    system_status: str
    imu_status: str
    magnetic_status: str

# Navigation model
class NavigationData(BaseModel):
    status: str
    target: Position
    progress: float
    distance_to_target: float
    estimated_time: float
    path_clear: bool
    obstacle_detected: bool
    speed_limit: float
    zone_type: str

# Dock model
class DockData(BaseModel):
    status: str
    station_id: Optional[str]
    target_station_id: Optional[str]
    alignment_status: str
    distance: float
    angle: float
    charging_status: str
    safety_status: str
```

---

**Changelog:**
- v2.0 (2025-01-27): Added location, navigation, và dock telemetry data
- v0.2 (2025-01-27): Initial telemetry schema

**Cross-links:**
- API: `docs/backend/TELEMETRY_API.md`
- WS Events: `docs/backend/WEBSOCKET_EVENTS.md`
- Models: `backend/app/models/telemetry.py`
