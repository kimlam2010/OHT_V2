# CONFIG API - OHT-50 Backend (v2.0)

## Mục lục
- [Tổng quan & Auth](#tổng-quan--auth)
- [Models chính](#models-chính)
- [Endpoints](#endpoints)
  - [GET /api/v1/config/](#get-apiv1config)
  - [GET /api/v1/config/history](#get-apiv1confighistory)
  - [GET /api/v1/config/version/{version}](#get-apiv1configversionversion)
  - [PUT /api/v1/config/](#put-apiv1config)
  - [POST /api/v1/config/rollback/{version}](#post-apiv1configrollbackversion)
  - [GET /api/v1/config/export](#get-apiv1configexport)
  - [POST /api/v1/config/import](#post-apiv1configimport)
  - [POST /api/v1/config/validate](#post-apiv1configvalidate)
  - [GET /api/v1/config/schema](#get-apiv1configschema)
- [Lưu ý hành vi](#lưu-ý-hành-vi)
- [Tham chiếu](#tham-chiếu)

---

## Tổng quan & Auth
- Tất cả endpoint có `Depends(verify_token)` → cần header `Authorization: Bearer <token>` khi `DEBUG=false`.
- Khi `DEBUG=true` (dev), có thể gọi không cần token.

## Models chính
- `SystemConfig` (cấu hình đầy đủ): các field chính
  - `version: int`
  - `modules: { power, safety, travel_motor, dock_location, master_control }`
  - `rs485: { bus_config, module_addresses, communication_params }`
  - `safety: { zones[], estop_reset_policy, speed_limits }`
  - `location: { lidar_config, positioning_params, navigation_settings }`
  - `telemetry: { ws_hz, history_size? }`
  - `comm: { rs485, can, module_discovery }`
- `ConfigHistory`: { version, timestamp, action, user, config_data }
- `ConfigResponse`: { success, message, config?, version? }

Chi tiết schema xem `backend/app/models/config.py` (đã có validator) và JSON Schema từ endpoint `/schema`.

## Endpoints

### GET /api/v1/config/
Trả về trực tiếp `SystemConfig`.
```bash
curl -s http://localhost:8000/api/v1/config/
# Prod (có token)
# curl -s -H 'Authorization: Bearer <TOKEN>' http://<host>:8000/api/v1/config/
```

### GET /api/v1/config/history
Tham số: `limit=1..50` (mặc định 10). Trả về danh sách `ConfigHistory`.
```bash
curl -s 'http://localhost:8000/api/v1/config/history?limit=5'
```

### GET /api/v1/config/version/{version}
Trả về `SystemConfig` theo version, 404 nếu không có.
```bash
curl -s http://localhost:8000/api/v1/config/version/1
```

### PUT /api/v1/config/
Cập nhật cấu hình; service tự tăng version. Query: `user=<name>`.
```bash
# file config.json chứa cấu hình đầy đủ (xem ví dụ mẫu trong models)
curl -s -X PUT 'http://localhost:8000/api/v1/config/?user=alice' \
  -H 'Content-Type: application/json' \
  -d @config.json
# → { "success": true, "message": "...", "config": {"version": 2, ...}, "version": 2 }
```

### POST /api/v1/config/rollback/{version}
Rollback về version cũ; service tạo version mới.
```bash
curl -s -X POST 'http://localhost:8000/api/v1/config/rollback/1?user=alice'
```

### GET /api/v1/config/export
Xuất cấu hình hiện tại dạng JSON hoặc YAML.
```bash
curl -s 'http://localhost:8000/api/v1/config/export?format=json'
curl -s 'http://localhost:8000/api/v1/config/export?format=yaml'
```

### POST /api/v1/config/import
Import cấu hình qua query `content` (string) và `format`.
```bash
# Ví dụ đơn giản (JSON ngắn ngay trong URL-encoded)
curl -s -X POST \
  --data-urlencode 'content={"version":0,"hardware":{"motor":{"interface":"rs485","r":0.05,"vmax_mms":100,"amax_mms2":100,"jmax_mms3":1000},"encoder":{"type":"incremental","resolution_ppr":1000,"invert":false}},"safety":{"zones":[{"start_mm":0,"end_mm":200,"vmax_mms":200}],"estop_reset_policy":"manual"},"telemetry":{"ws_hz":10},"comm":{"rs485":{"device":"/dev/ttyOHT485","baud":115200}}}' \
  'http://localhost:8000/api/v1/config/import?format=json&user=alice'
```

### POST /api/v1/config/validate
Validate cấu hình (không lưu).
```bash
curl -s -X POST 'http://localhost:8000/api/v1/config/validate' \
  -H 'Content-Type: application/json' \
  -d @config.json
# → { "valid": true, "message": "Configuration is valid" }
```

### GET /api/v1/config/schema
Trả về JSON schema của `SystemConfig`.
```bash
curl -s http://localhost:8000/api/v1/config/schema | jq .title
```

## 📋 **Module Configuration Examples**

### **Power Module Configuration (24V System)**
```json
{
  "modules": {
    "power": {
      "voltage_nominal": 24.0,
      "battery_capacity": 50.0,
      "max_voltage": 28.8,
      "min_voltage": 20.0,
      "charging_current": 5.0,
      "power_distribution": {
        "5v": {"enabled": true, "current_limit": 2.0},
        "12v": {"enabled": true, "current_limit": 5.0},
        "24v": {"enabled": true, "current_limit": 3.0}
      }
    }
  }
}
```

### **Safety Module Configuration**
```json
{
  "modules": {
    "safety": {
      "estop_enabled": true,
      "estop_reset_policy": "manual",
      "safety_zones": [
        {
          "id": "zone_1",
          "type": "restricted",
          "boundaries": [[0, 0], [100, 0], [100, 50], [0, 50]],
          "max_speed": 200.0
        }
      ],
      "speed_limits": {
        "default": 1000.0,
        "restricted_zones": 200.0,
        "emergency": 0.0
      }
    }
  }
}
```

### **Travel Motor Configuration (12V DC Brushed)**
```json
{
  "modules": {
    "travel_motor": {
      "motor_type": "DC_BRUSHED",
      "voltage": 12.0,
      "max_speed": 5000,
      "max_current": 10.0,
      "pid_settings": {
        "kp": 100,
        "ki": 10,
        "kd": 5
      },
      "encoder": {
        "resolution": 60,
        "gear_ratio": 20,
        "wheel_diameter": 0.15
      }
    }
  }
}
```

### **Dock & Location Configuration (LiDAR USB 2.0)**
```json
{
  "modules": {
    "dock_location": {
      "lidar": {
        "interface": "USB_2.0",
        "model": "RPLIDAR_A1M8",
        "scan_frequency": 5.5,
        "range_min": 0.15,
        "range_max": 12.0
      },
      "positioning": {
        "accuracy_target": 2.0,
        "update_rate": 10.0,
        "rfid_enabled": true
      },
      "navigation": {
        "path_planning": "A_STAR",
        "obstacle_avoidance": true,
        "safety_margin": 100.0
      }
    }
  }
}
```

### **Master Control Configuration**
```json
{
  "modules": {
    "master_control": {
      "coordination": {
        "module_sync_rate": 10.0,
        "state_management": true,
        "event_coordination": true
      },
      "system_state": {
        "default_state": "idle",
        "state_transitions": {
          "idle": ["moving", "docking", "fault"],
          "moving": ["idle", "docking", "estop"],
          "docking": ["idle", "fault", "estop"],
          "fault": ["idle", "estop"],
          "estop": ["idle"]
        }
      }
    }
  }
}
```

### **RS485 Communication Configuration**
```json
{
  "rs485": {
    "bus_config": {
      "baudrate": 115200,
      "data_bits": 8,
      "parity": "none",
      "stop_bits": 1,
      "timeout": 1.0
    },
    "module_addresses": {
      "power": "0x01",
      "safety": "0x02",
      "travel_motor": "0x03",
      "dock_location": "0x04",
      "master_control": "0x05"
    },
    "communication_params": {
      "retry_attempts": 3,
      "retry_delay": 0.1,
      "heartbeat_interval": 1.0
    }
  }
}
```

### **Module Discovery Configuration**
```json
{
  "comm": {
    "module_discovery": {
      "auto_discovery": true,
      "discovery_interval": 5.0,
      "hot_swap_enabled": true,
      "health_monitoring": {
        "enabled": true,
        "check_interval": 1.0,
        "timeout_threshold": 3.0
      }
    }
  }
}
```

## Lưu ý hành vi
- `GET /api/v1/config/` trả về object trực tiếp (không bọc `{ success, data }`).
- `PUT/rollback/import` trả về `ConfigResponse` (envelope có `success`, `version`, ...).
- `import` nhận nội dung qua query `content` (không upload file); dùng `--data-urlencode` cho chuỗi dài.

## Tham chiếu
- Code: `backend/app/api/v1/config.py`, `backend/app/services/config_service.py`
- Spec tổng: `docs/FIRMWARE/01_SPECIFICATIONS/API_SPEC.md`

---

**Changelog v2.0:**
- ✅ Updated to align with v2.0 architecture
- ✅ Added 5 mandatory modules configuration
- ✅ Added RS485 communication configuration
- ✅ Added module discovery configuration
- ✅ Added safety system configuration
- ✅ Added location/navigation configuration
- ✅ Standardized to 24V nominal voltage
- ✅ Added comprehensive configuration examples
- ✅ Updated data models for v2.0 architecture
