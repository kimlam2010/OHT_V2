# MODULE INTEGRATION GUIDE - OHT-50 Backend v2.0

## 📋 **Tổng quan**
Hướng dẫn tích hợp 5 mandatory modules cho OHT-50 Architecture v2.0, bao gồm setup, configuration, testing, và troubleshooting.

## 🔧 **Architecture v2.0 Features**
- **5 Mandatory Modules:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **RS485 Communication:** Standardized cho tất cả modules
- **LiDAR USB Integration:** RPLIDAR A1M8 qua USB 2.0
- **24V System:** Power management với 24V nominal voltage
- **Safety Integration:** E-Stop, safety zones, emergency procedures
- **Mission Management:** Complete mission lifecycle
- **State Machine:** System state management
- **Real-time Coordination:** Master Control Module orchestration

---

## 🔌 **1. Hardware Setup**

### **1.1 Module Address Configuration**

| Module | Address | Description | Default Settings |
|--------|---------|-------------|------------------|
| **Power Module** | 0x01 | 24V system management | Auto-configure |
| **Safety Module** | 0x02 | E-Stop & safety zones | Auto-configure |
| **Travel Motor** | 0x03 | 12V DC motor control | Auto-configure |
| **Dock & Location** | 0x04 | LiDAR USB integration | Auto-configure |
| **Master Control** | 0x05 | System coordination | Auto-configure |

### **1.2 RS485 Bus Configuration**

#### **Physical Setup**
```bash
# RS485 Bus Configuration
Baudrate: 115200 bps
Data Bits: 8
Parity: None
Stop Bits: 1
Termination: 120Ω resistors
Cable: Shielded twisted pair
Max Distance: 1200m
Max Nodes: 32
```

#### **Wiring Diagram**
```
Master Control (0x05)
    |
    | RS485 Bus
    |
    ├── Power Module (0x01)
    ├── Safety Module (0x02)
    ├── Travel Motor (0x03)
    └── Dock & Location (0x04)
```

### **1.3 USB 2.0 LiDAR Integration**

#### **LiDAR Connection**
```bash
# LiDAR USB Configuration
Device: RPLIDAR A1M8
Interface: USB 2.0
Data Rate: Up to 480 Mbps
Connection: Direct to Dock & Location Module
Driver: RPLIDAR SDK
```

#### **USB Setup**
```bash
# Check USB device
lsusb | grep RPLIDAR

# Set USB permissions
sudo usermod -a -G dialout $USER

# Test LiDAR connection
ros2 run rplidar_ros rplidar_node
```

---

## ⚙️ **2. Module Configuration**

### **2.1 Power Module (0x01)**

#### **Configuration Parameters**
```json
{
  "module_address": "0x01",
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
```

#### **Setup Commands**
```bash
# Power Module Setup
curl -X POST http://localhost:8000/api/v1/config/modules/power \
  -H "Content-Type: application/json" \
  -d @power_config.json

# Verify Power Module
curl http://localhost:8000/api/v1/power/status
```

### **2.2 Safety Module (0x02)**

#### **Configuration Parameters**
```json
{
  "module_address": "0x02",
  "estop_config": {
    "dual_channel": true,
    "auto_reset": false,
    "emergency_level": "critical"
  },
  "safety_zones": {
    "zone_1": {
      "type": "restricted",
      "boundaries": [[0, 0], [100, 0], [100, 50], [0, 50]],
      "max_speed": 0.0
    },
    "zone_2": {
      "type": "warning",
      "boundaries": [[100, 0], [200, 0], [200, 50], [100, 50]],
      "max_speed": 0.5
    }
  }
}
```

#### **Setup Commands**
```bash
# Safety Module Setup
curl -X POST http://localhost:8000/api/v1/config/modules/safety \
  -H "Content-Type: application/json" \
  -d @safety_config.json

# Test E-Stop
curl -X POST http://localhost:8000/api/v1/safety/estop/test
```

### **2.3 Travel Motor Module (0x03)**

#### **Configuration Parameters**
```json
{
  "module_address": "0x03",
  "motor_config": {
    "motor_type": "dc_brushed",
    "voltage": 12.0,
    "power": 100.0,
    "gearbox_ratio": 20.0
  },
  "pid_config": {
    "kp": 1.0,
    "ki": 0.1,
    "kd": 0.05,
    "max_output": 100.0
  },
  "encoder_config": {
    "type": "hall_effect",
    "resolution": 360,
    "direction": "clockwise"
  }
}
```

#### **Setup Commands**
```bash
# Travel Motor Setup
curl -X POST http://localhost:8000/api/v1/config/modules/travel_motor \
  -H "Content-Type: application/json" \
  -d @motor_config.json

# Test Motor Control
curl -X POST http://localhost:8000/api/v1/dc-motor/control \
  -H "Content-Type: application/json" \
  -d '{"speed": 0.5, "direction": 0}'
```

### **2.4 Dock & Location Module (0x04)**

#### **Configuration Parameters**
```json
{
  "module_address": "0x04",
  "lidar_config": {
    "model": "RPLIDAR_A1M8",
    "interface": "USB_2.0",
    "scan_frequency": 5.5,
    "range_min": 0.15,
    "range_max": 12.0
  },
  "positioning_config": {
    "accuracy_target": 2.0,
    "update_rate": 10.0,
    "rfid_enabled": true
  },
  "navigation_config": {
    "path_planning": "A_STAR",
    "obstacle_avoidance": true,
    "safety_margin": 100.0
  }
}
```

#### **Setup Commands**
```bash
# Dock & Location Setup
curl -X POST http://localhost:8000/api/v1/config/modules/dock_location \
  -H "Content-Type: application/json" \
  -d @dock_location_config.json

# Test LiDAR
curl http://localhost:8000/api/v1/dock-location/lidar/status

# Test Positioning
curl http://localhost:8000/api/v1/dock-location/position
```

### **2.5 Master Control Module (0x05)**

#### **Configuration Parameters**
```json
{
  "module_address": "0x05",
  "coordination_config": {
    "heartbeat_interval": 1.0,
    "timeout": 5.0,
    "retry_attempts": 3
  },
  "mission_config": {
    "max_concurrent_missions": 1,
    "mission_timeout": 3600,
    "auto_recovery": true
  },
  "communication_config": {
    "rs485_timeout": 1.0,
    "ethernet_timeout": 5.0,
    "websocket_timeout": 10.0
  }
}
```

#### **Setup Commands**
```bash
# Master Control Setup
curl -X POST http://localhost:8000/api/v1/config/modules/master_control \
  -H "Content-Type: application/json" \
  -d @master_control_config.json

# Test System Status
curl http://localhost:8000/api/v1/master-control/status
```

---

## 🧪 **3. Integration Testing**

### **3.1 Module Discovery Test**

#### **Test Script**
```python
import requests
import time

def test_module_discovery():
    """Test automatic module discovery"""
    print("Testing module discovery...")
    
    # Wait for modules to be discovered
    time.sleep(5)
    
    # Check all modules are online
    response = requests.get("http://localhost:8000/api/v1/master-control/status")
    status = response.json()
    
    modules = status["data"]["modules"]
    expected_modules = ["power", "safety", "travel_motor", "dock_location", "master_control"]
    
    for module in expected_modules:
        if modules[module]["status"] != "online":
            print(f"❌ Module {module} is not online")
            return False
        else:
            print(f"✅ Module {module} is online")
    
    print("✅ All modules discovered successfully")
    return True

if __name__ == "__main__":
    test_module_discovery()
```

### **3.2 Communication Test**

#### **RS485 Communication Test**
```python
def test_rs485_communication():
    """Test RS485 communication with all modules"""
    print("Testing RS485 communication...")
    
    response = requests.get("http://localhost:8000/api/v1/telemetry/current")
    telemetry = response.json()
    
    rs485_status = telemetry["data"]["communication"]["rs485"]
    
    if rs485_status["status"] == "healthy":
        print(f"✅ RS485 communication healthy")
        print(f"   Error rate: {rs485_status['error_rate']}")
        print(f"   Response time: {rs485_status['response_time']}s")
        return True
    else:
        print(f"❌ RS485 communication unhealthy")
        return False
```

### **3.3 Safety System Test**

#### **E-Stop Test**
```python
def test_estop_system():
    """Test E-Stop system"""
    print("Testing E-Stop system...")
    
    # Trigger E-Stop
    response = requests.post("http://localhost:8000/api/v1/safety/estop/trigger")
    
    if response.status_code == 200:
        print("✅ E-Stop triggered successfully")
        
        # Check system state
        time.sleep(1)
        status_response = requests.get("http://localhost:8000/api/v1/master-control/status")
        system_state = status_response.json()["data"]["system_state"]["current_state"]
        
        if system_state == "estop":
            print("✅ System entered E-Stop state")
        else:
            print(f"❌ System state: {system_state}")
            return False
        
        # Clear E-Stop
        clear_response = requests.post("http://localhost:8000/api/v1/safety/estop/clear")
        
        if clear_response.status_code == 200:
            print("✅ E-Stop cleared successfully")
            return True
        else:
            print("❌ Failed to clear E-Stop")
            return False
    else:
        print("❌ Failed to trigger E-Stop")
        return False
```

### **3.4 Mission Execution Test**

#### **Simple Mission Test**
```python
def test_mission_execution():
    """Test mission execution"""
    print("Testing mission execution...")
    
    # Create simple mission
    mission_data = {
        "mission_name": "Test Mission",
        "mission_type": "transport",
        "waypoints": [
            {"x": 0, "y": 0, "action": "start"},
            {"x": 100, "y": 0, "action": "move"},
            {"x": 100, "y": 0, "action": "stop"}
        ]
    }
    
    # Create mission
    response = requests.post("http://localhost:8000/api/v1/master-control/missions", 
                           json=mission_data)
    
    if response.status_code == 200:
        mission_id = response.json()["data"]["mission_id"]
        print(f"✅ Mission created: {mission_id}")
        
        # Start mission
        start_response = requests.post(f"http://localhost:8000/api/v1/master-control/missions/{mission_id}/start")
        
        if start_response.status_code == 200:
            print("✅ Mission started successfully")
            
            # Monitor mission progress
            for i in range(30):  # Monitor for 30 seconds
                time.sleep(1)
                status_response = requests.get(f"http://localhost:8000/api/v1/master-control/missions/{mission_id}")
                mission_status = status_response.json()["data"]["status"]
                
                if mission_status == "completed":
                    print("✅ Mission completed successfully")
                    return True
                elif mission_status == "failed":
                    print("❌ Mission failed")
                    return False
            
            print("⚠️ Mission timeout")
            return False
        else:
            print("❌ Failed to start mission")
            return False
    else:
        print("❌ Failed to create mission")
        return False
```

---

## 🔧 **4. Troubleshooting**

### **4.1 Common Issues**

#### **Module Not Responding**
```bash
# Check RS485 connection
sudo dmesg | grep tty

# Test RS485 communication
sudo stty -F /dev/ttyUSB0 115200
echo "test" > /dev/ttyUSB0

# Check module addresses
curl http://localhost:8000/api/v1/master-control/status
```

#### **LiDAR Not Detected**
```bash
# Check USB connection
lsusb | grep RPLIDAR

# Check USB permissions
sudo chmod 666 /dev/ttyUSB0

# Test LiDAR driver
ros2 run rplidar_ros rplidar_node --ros-args -p serial_port:=/dev/ttyUSB0
```

#### **E-Stop Not Working**
```bash
# Check E-Stop circuit
curl http://localhost:8000/api/v1/safety/status

# Test E-Stop manually
curl -X POST http://localhost:8000/api/v1/safety/estop/trigger

# Check safety module logs
tail -f /var/log/oht50/safety.log
```

### **4.2 Diagnostic Commands**

#### **System Health Check**
```bash
# Complete system health check
curl http://localhost:8000/api/v1/master-control/health

# Module-specific health check
curl http://localhost:8000/api/v1/power/status
curl http://localhost:8000/api/v1/safety/status
curl http://localhost:8000/api/v1/dc-motor/status
curl http://localhost:8000/api/v1/dock-location/status
```

#### **Communication Diagnostics**
```bash
# RS485 communication test
curl http://localhost:8000/api/v1/telemetry/current | jq '.data.communication.rs485'

# Module communication test
for module in power safety travel_motor dock_location master_control; do
    echo "Testing $module communication..."
    curl -s http://localhost:8000/api/v1/telemetry/modules/$module > /dev/null && echo "✅ OK" || echo "❌ FAILED"
done
```

---

## 📊 **5. Performance Monitoring**

### **5.1 Key Metrics**

| Metric | Target | Alert Threshold | Monitoring |
|--------|--------|----------------|------------|
| **Module Response Time** | < 100ms | > 200ms | Real-time |
| **RS485 Error Rate** | < 0.1% | > 1% | Continuous |
| **LiDAR Scan Frequency** | 5.5 Hz | < 4 Hz | Real-time |
| **Battery Voltage** | 24V ± 10% | < 20V | Continuous |
| **E-Stop Response Time** | < 100ms | > 200ms | Critical |

### **5.2 Monitoring Setup**

#### **Prometheus Configuration**
```yaml
# prometheus.yml
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'oht50-backend'
    static_configs:
      - targets: ['localhost:8000']
    metrics_path: '/metrics'
    scrape_interval: 5s
```

#### **Grafana Dashboard**
```json
{
  "dashboard": {
    "title": "OHT-50 Module Integration",
    "panels": [
      {
        "title": "Module Status",
        "type": "stat",
        "targets": [
          {
            "expr": "oht50_module_status{module=\"power\"}",
            "legendFormat": "Power Module"
          }
        ]
      }
    ]
  }
}
```

---

## 📚 **6. References**

### **6.1 Related Documents**
- [API Specifications](api-specs/README.md) - Complete API documentation
- [Business Logic](../02-ARCHITECTURE/BUSINESS_LOGIC_SPEC.md) - Business logic specification
- [Architecture](architecture/SYSTEM_ARCHITECTURE.md) - System architecture

### **6.2 Hardware Documentation**
- **RPLIDAR A1M8:** [Official Documentation](https://www.slamtec.com/en/Lidar/A1)
- **RS485 Protocol:** [Modbus RTU Specification](https://modbus.org/specs.php)
- **Orange Pi 5B:** [Hardware Manual](https://www.orangepi.org/html/hardWare/computerAndMicrocontrollers/service-and-support/Orange-Pi-5B.html)

---

**Changelog v2.0:**
- ✅ Created comprehensive module integration guide
- ✅ Added 5 mandatory modules setup instructions
- ✅ Added RS485 communication configuration
- ✅ Added LiDAR USB integration guide
- ✅ Added comprehensive testing procedures
- ✅ Added troubleshooting guide
- ✅ Added performance monitoring setup
- ✅ Standardized to 24V nominal voltage
- ✅ Added real-time monitoring configuration
