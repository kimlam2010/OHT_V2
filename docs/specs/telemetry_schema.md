# Telemetry Schema - OHT-50 (v0.2)

## Mục lục
- [Mục tiêu](#mục-tiêu)
- [JSON Schema](#json-schema)
- [Cross-links](#cross-links)

---

## Mục tiêu
Chuẩn hoá schema telemetry cho API/WS, khớp `backend/app/models/telemetry.py`.

## JSON Schema
```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "TelemetryData",
  "type": "object",
  "properties": {
    "ts": { "type": "number", "description": "Unix timestamp" },
    "status": { "$ref": "#/definitions/TelemetryStatus" },
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
  "required": ["ts", "status"],
  "definitions": {
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
        "interlock_active": { "type": "boolean" }
      },
      "required": ["estop", "zone_blocked", "interlock_active"]
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
        "state": { "type": "string" },
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

## Cross-links
- API: `docs/backend/TELEMETRY_API.md`
- WS Events: `docs/backend/WEBSOCKET_EVENTS.md`
- Models: `backend/app/models/telemetry.py`
