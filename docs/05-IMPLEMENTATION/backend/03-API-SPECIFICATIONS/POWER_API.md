# POWER MODULE API - OHT-50 Backend v2.0

## 📋 **Tổng quan**
API quản lý Power Module (Address: 0x02) - Battery management, charging control, power distribution.

## 🔗 **Base URL**
```
http://localhost:8000/api/v1/power
```

## 📊 **Endpoints**

### **1. Power Status**

#### **GET /api/v1/power/status**
Lấy trạng thái tổng quan của Power Module.

**Response:**
```json
{
  "module_id": "0x02",
  "module_type": "power",
  "status": "online",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "battery": {
      "voltage": 24.2,
      "current": 2.5,
      "soc": 85,
      "temperature": 25.5,
      "capacity": 50,
      "cycle_count": 150
    },
    "charging": {
      "status": "charging",
      "mode": "cc_cv",
      "current": 5.0,
      "voltage": 48.0
    },
    "power_distribution": {
      "output_5v": {
        "voltage": 5.02,
        "current": 1.2,
        "status": "normal"
      },
      "output_12v": {
        "voltage": 12.05,
        "current": 3.5,
        "status": "normal"
      },
      "output_24v": {
        "voltage": 24.1,
        "current": 2.0,
        "status": "normal"
      },
      "output_24v": {
        "voltage": 24.2,
        "current": 1.8,
        "status": "normal"
      }
    },
    "protection": {
      "overvoltage": false,
      "overcurrent": false,
      "overtemperature": false,
      "undervoltage": false
    }
  }
}
```

### **2. Battery Management**

#### **GET /api/v1/power/battery**
Lấy thông tin chi tiết về battery.

**Response:**
```json
{
  "voltage": 24.2,
  "current": 2.5,
  "soc": 85,
  "temperature": 25.5,
  "capacity": 50,
  "cycle_count": 150,
  "health": "good",
  "estimated_runtime": 3600
}
```

#### **PUT /api/v1/power/battery/config**
Cập nhật cấu hình battery.

**Request:**
```json
{
  "max_voltage": 28.8,
  "min_voltage": 20.0,
  "max_current": 10.0,
  "max_temperature": 60.0
}
```

### **3. Charging Control**

#### **POST /api/v1/power/charge/start**
Bắt đầu sạc pin.

**Request:**
```json
{
  "mode": "cc_cv",
  "target_voltage": 28.8,
  "max_current": 5.0
}
```

#### **POST /api/v1/power/charge/stop**
Dừng sạc pin.

#### **GET /api/v1/power/charge/status**
Lấy trạng thái sạc.

**Response:**
```json
{
  "status": "charging",
  "mode": "cc_cv",
  "current": 5.0,
  "voltage": 28.8,
  "progress": 85,
  "estimated_completion": "2025-01-28T12:30:00Z"
}
```

### **4. Power Distribution**

#### **GET /api/v1/power/distribution**
Lấy trạng thái power distribution.

**Response:**
```json
{
  "outputs": {
    "5v": {
      "voltage": 5.02,
      "current": 1.2,
      "power": 6.024,
      "status": "normal",
      "enabled": true
    },
    "12v": {
      "voltage": 12.05,
      "current": 3.5,
      "power": 42.175,
      "status": "normal",
      "enabled": true
    },
    "24v": {
      "voltage": 24.1,
      "current": 2.0,
      "power": 48.2,
      "status": "normal",
      "enabled": true
    },
          "24v": {
        "voltage": 24.2,
        "current": 1.8,
        "power": 43.56,
        "status": "normal",
        "enabled": true
      }
  },
  "total_power": 183.159
}
```

#### **PUT /api/v1/power/distribution/{output}**
Cập nhật cấu hình output.

**Request:**
```json
{
  "enabled": true,
  "voltage_limit": 5.0,
  "current_limit": 2.0
}
```

### **5. Protection & Safety**

#### **GET /api/v1/power/protection**
Lấy trạng thái protection.

**Response:**
```json
{
  "overvoltage": {
    "status": false,
    "threshold": 50.0,
    "current_value": 48.2
  },
  "overcurrent": {
    "status": false,
    "threshold": 10.0,
    "current_value": 2.5
  },
  "overtemperature": {
    "status": false,
    "threshold": 60.0,
    "current_value": 25.5
  },
  "undervoltage": {
    "status": false,
    "threshold": 20.0,
    "current_value": 24.2
  }
}
```

#### **POST /api/v1/power/protection/reset**
Reset protection status.

### **6. Configuration**

#### **GET /api/v1/power/config**
Lấy cấu hình hiện tại.

**Response:**
```json
{
  "battery": {
    "type": "LiFePO4",
    "capacity": 50,
    "max_voltage": 28.8,
    "min_voltage": 20.0,
    "max_current": 10.0,
    "max_temperature": 60.0
  },
  "charging": {
    "cc_voltage": 28.8,
    "cv_current": 0.1,
    "float_voltage": 28.8,
    "max_charge_current": 5.0
  },
  "distribution": {
    "outputs": {
      "5v": {"enabled": true, "voltage_limit": 5.0, "current_limit": 2.0},
      "12v": {"enabled": true, "voltage_limit": 12.0, "current_limit": 5.0},
      "24v": {"enabled": true, "voltage_limit": 24.0, "current_limit": 3.0}
    }
  }
}
```

#### **PUT /api/v1/power/config**
Cập nhật cấu hình.

**Request:**
```json
{
  "battery": {
    "max_voltage": 28.8,
    "min_voltage": 20.0,
    "max_current": 10.0,
    "max_temperature": 60.0
  },
  "charging": {
    "cc_voltage": 28.8,
    "cv_current": 0.1,
    "float_voltage": 28.8,
    "max_charge_current": 5.0
  }
}
```

## 🚨 **Error Codes**

| Code | Message | Description |
|------|---------|-------------|
| 400 | Invalid configuration | Cấu hình không hợp lệ |
| 404 | Power module not found | Module không tồn tại |
| 409 | Charging in progress | Đang sạc, không thể thay đổi |
| 422 | Protection triggered | Bảo vệ đã kích hoạt |
| 500 | Internal power error | Lỗi nội bộ power module |

## 📊 **WebSocket Events**

### **Power Status Updates**
```json
{
  "event": "power_status_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "voltage": 24.2,
    "current": 2.5,
    "soc": 85,
    "temperature": 25.5
  }
}
```

### **Charging Status Updates**
```json
{
  "event": "charging_status_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "status": "charging",
    "progress": 85,
    "current": 5.0,
    "voltage": 28.8
  }
}
```

### **Protection Alerts**
```json
{
  "event": "protection_alert",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "type": "overvoltage",
    "threshold": 30.0,
    "current_value": 31.2,
    "action": "shutdown"
  }
}
```

## 🧪 **Testing Examples**

### **Quick Test**
```bash
# Get power status
curl http://localhost:8000/api/v1/power/status

# Start charging
curl -X POST http://localhost:8000/api/v1/power/charge/start \
  -H "Content-Type: application/json" \
  -d '{"mode": "cc_cv", "target_voltage": 28.8, "max_current": 5.0}'

# Get battery info
curl http://localhost:8000/api/v1/power/battery
```

### **Python Example**
```python
import requests

# Get power status
response = requests.get("http://localhost:8000/api/v1/power/status")
power_status = response.json()

# Start charging
charging_config = {
    "mode": "cc_cv",
    "target_voltage": 28.8,
    "max_current": 5.0
}
response = requests.post(
    "http://localhost:8000/api/v1/power/charge/start",
    json=charging_config
)
```

## 📝 **Notes**

- **Real-time monitoring:** Sử dụng WebSocket để nhận updates real-time
- **Safety first:** Tất cả operations đều có protection checks
- **Configuration validation:** Tất cả config changes đều được validate
- **Error handling:** Comprehensive error codes và messages
