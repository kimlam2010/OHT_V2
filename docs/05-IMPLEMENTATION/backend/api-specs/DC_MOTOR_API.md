# DC MOTOR MODULE API - OHT-50 Backend v2.0

## 📋 **Tổng quan**
API quản lý DC Motor Module (Address: 0x04) - Differential drive control, speed control, PID tuning.

## 🔗 **Base URL**
```
http://localhost:8000/api/v1/dc-motor
```

## 📊 **Endpoints**

### **1. Motor Status**

#### **GET /api/v1/dc-motor/status**
Lấy trạng thái tổng quan của DC Motor Module.

**Response:**
```json
{
  "module_id": "0x04",
  "module_type": "dc_motor",
  "status": "online",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "motor1": {
      "speed": 1500,
      "direction": "forward",
      "current": 2.5,
      "voltage": 12.0,
      "temperature": 35.0,
      "status": "running",
      "encoder": {
        "position": 15000,
        "velocity": 1500,
        "pulses": 90000
      }
    },
    "motor2": {
      "speed": 1500,
      "direction": "forward",
      "current": 2.3,
      "voltage": 12.0,
      "temperature": 34.5,
      "status": "running",
      "encoder": {
        "position": 15000,
        "velocity": 1500,
        "pulses": 90000
      }
    },
    "differential": {
      "linear_velocity": 1.5,
      "angular_velocity": 0.0,
      "wheelbase": 0.5,
      "wheel_diameter": 0.15
    },
    "control": {
      "mode": "pid",
      "target_speed": 1500,
      "target_direction": "forward"
    }
  }
}
```

### **2. Motor Control**

#### **POST /api/v1/dc-motor/control**
Điều khiển motor với speed và direction.

**Request:**
```json
{
  "motor1": {
    "speed": 1500,
    "direction": "forward"
  },
  "motor2": {
    "speed": 1500,
    "direction": "forward"
  },
  "mode": "differential"
}
```

#### **POST /api/v1/dc-motor/stop**
Dừng tất cả motors.

#### **POST /api/v1/dc-motor/emergency-stop**
Emergency stop - dừng khẩn cấp.

### **3. Speed Control**

#### **GET /api/v1/dc-motor/speed**
Lấy thông tin speed hiện tại.

**Response:**
```json
{
  "motor1": {
    "current_speed": 1500,
    "target_speed": 1500,
    "direction": "forward",
    "acceleration": 100,
    "deceleration": 100
  },
  "motor2": {
    "current_speed": 1500,
    "target_speed": 1500,
    "direction": "forward",
    "acceleration": 100,
    "deceleration": 100
  },
  "differential": {
    "linear_velocity": 1.5,
    "angular_velocity": 0.0
  }
}
```

#### **PUT /api/v1/dc-motor/speed**
Cập nhật target speed.

**Request:**
```json
{
  "motor1_speed": 2000,
  "motor2_speed": 2000,
  "acceleration": 200,
  "deceleration": 200
}
```

### **4. PID Control**

#### **GET /api/v1/dc-motor/pid**
Lấy cấu hình PID hiện tại.

**Response:**
```json
{
  "motor1": {
    "kp": 100,
    "ki": 10,
    "kd": 5,
    "setpoint": 1500,
    "error": 0,
    "integral": 0,
    "derivative": 0
  },
  "motor2": {
    "kp": 100,
    "ki": 10,
    "kd": 5,
    "setpoint": 1500,
    "error": 0,
    "integral": 0,
    "derivative": 0
  }
}
```

#### **PUT /api/v1/dc-motor/pid**
Cập nhật cấu hình PID.

**Request:**
```json
{
  "motor1": {
    "kp": 120,
    "ki": 15,
    "kd": 8
  },
  "motor2": {
    "kp": 120,
    "ki": 15,
    "kd": 8
  }
}
```

### **5. Encoder Data**

#### **GET /api/v1/dc-motor/encoder**
Lấy dữ liệu encoder.

**Response:**
```json
{
  "motor1": {
    "position": 15000,
    "velocity": 1500,
    "pulses": 90000,
    "resolution": 60,
    "gear_ratio": 20
  },
  "motor2": {
    "position": 15000,
    "velocity": 1500,
    "pulses": 90000,
    "resolution": 60,
    "gear_ratio": 20
  }
}
```

### **6. Differential Drive**

#### **POST /api/v1/dc-motor/differential**
Điều khiển differential drive.

**Request:**
```json
{
  "linear_velocity": 1.5,
  "angular_velocity": 0.5,
  "wheelbase": 0.5,
  "wheel_diameter": 0.15
}
```

#### **GET /api/v1/dc-motor/differential/status**
Lấy trạng thái differential drive.

**Response:**
```json
{
  "linear_velocity": 1.5,
  "angular_velocity": 0.0,
  "wheelbase": 0.5,
  "wheel_diameter": 0.15,
  "left_wheel_speed": 1500,
  "right_wheel_speed": 1500,
  "radius_of_curvature": 0
}
```

### **7. Configuration**

#### **GET /api/v1/dc-motor/config**
Lấy cấu hình hiện tại.

**Response:**
```json
{
  "motor_specs": {
    "type": "DC_BRUSHED",
    "voltage": 12,
    "power": 100,
    "max_speed": 5000,
    "max_current": 10
  },
  "encoder_specs": {
    "resolution": 60,
    "gear_ratio": 20,
    "wheel_diameter": 0.15
  },
  "control_specs": {
    "max_acceleration": 500,
    "max_deceleration": 500,
    "pid_update_rate": 100
  },
  "safety_specs": {
    "max_speed_limit": 4000,
    "max_current_limit": 8,
    "overtemperature_limit": 80
  }
}
```

#### **PUT /api/v1/dc-motor/config**
Cập nhật cấu hình.

**Request:**
```json
{
  "control_specs": {
    "max_acceleration": 600,
    "max_deceleration": 600,
    "pid_update_rate": 100
  },
  "safety_specs": {
    "max_speed_limit": 4000,
    "max_current_limit": 8,
    "overtemperature_limit": 80
  }
}
```

### **8. Safety & Protection**

#### **GET /api/v1/dc-motor/safety**
Lấy trạng thái safety.

**Response:**
```json
{
  "motor1": {
    "overtemperature": false,
    "overcurrent": false,
    "overspeed": false,
    "temperature": 35.0,
    "current": 2.5,
    "speed": 1500
  },
  "motor2": {
    "overtemperature": false,
    "overcurrent": false,
    "overspeed": false,
    "temperature": 34.5,
    "current": 2.3,
    "speed": 1500
  },
  "system": {
    "emergency_stop": false,
    "safety_zone": "normal",
    "estop_triggered": false
  }
}
```

#### **POST /api/v1/dc-motor/safety/reset**
Reset safety status.

## 🚨 **Error Codes**

| Code | Message | Description |
|------|---------|-------------|
| 400 | Invalid motor command | Lệnh điều khiển không hợp lệ |
| 404 | Motor module not found | Module không tồn tại |
| 409 | Motor in emergency stop | Motor đang trong emergency stop |
| 422 | Safety limit exceeded | Vượt quá giới hạn an toàn |
| 500 | Internal motor error | Lỗi nội bộ motor module |

## 📊 **WebSocket Events**

### **Motor Status Updates**
```json
{
  "event": "motor_status_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "motor1": {
      "speed": 1500,
      "current": 2.5,
      "temperature": 35.0
    },
    "motor2": {
      "speed": 1500,
      "current": 2.3,
      "temperature": 34.5
    }
  }
}
```

### **Encoder Updates**
```json
{
  "event": "encoder_update",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "motor1": {
      "position": 15000,
      "velocity": 1500
    },
    "motor2": {
      "position": 15000,
      "velocity": 1500
    }
  }
}
```

### **Safety Alerts**
```json
{
  "event": "safety_alert",
  "timestamp": "2025-01-28T10:30:00Z",
  "data": {
    "motor": "motor1",
    "type": "overtemperature",
    "value": 85.0,
    "limit": 80.0,
    "action": "shutdown"
  }
}
```

## 🧪 **Testing Examples**

### **Quick Test**
```bash
# Get motor status
curl http://localhost:8000/api/v1/dc-motor/status

# Control motors
curl -X POST http://localhost:8000/api/v1/dc-motor/control \
  -H "Content-Type: application/json" \
  -d '{"motor1": {"speed": 1500, "direction": "forward"}, "motor2": {"speed": 1500, "direction": "forward"}}'

# Stop motors
curl -X POST http://localhost:8000/api/v1/dc-motor/stop
```

### **Python Example**
```python
import requests

# Get motor status
response = requests.get("http://localhost:8000/api/v1/dc-motor/status")
motor_status = response.json()

# Control motors
control_command = {
    "motor1": {"speed": 1500, "direction": "forward"},
    "motor2": {"speed": 1500, "direction": "forward"},
    "mode": "differential"
}
response = requests.post(
    "http://localhost:8000/api/v1/dc-motor/control",
    json=control_command
)

# Update PID
pid_config = {
    "motor1": {"kp": 120, "ki": 15, "kd": 8},
    "motor2": {"kp": 120, "ki": 15, "kd": 8}
}
response = requests.put(
    "http://localhost:8000/api/v1/dc-motor/pid",
    json=pid_config
)
```

## 📝 **Notes**

- **Real-time control:** Sử dụng WebSocket để điều khiển real-time
- **Safety first:** Tất cả commands đều có safety checks
- **PID tuning:** Support tuning PID parameters
- **Differential drive:** Support differential drive control
- **Encoder feedback:** Real-time encoder data
- **Emergency stop:** Immediate stop capability
