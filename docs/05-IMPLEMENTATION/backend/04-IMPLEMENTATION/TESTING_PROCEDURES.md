# TESTING PROCEDURES - OHT-50 Backend v2.0

## 📋 **Tổng quan**
Hướng dẫn testing procedures toàn diện cho OHT-50 Architecture v2.0.

## 🔧 **Architecture v2.0 Features**
- **5 Mandatory Modules:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **RS485 Communication:** Standardized cho tất cả modules
- **LiDAR USB Integration:** RPLIDAR A1M8 qua USB 2.0
- **24V System:** Power management với 24V nominal voltage

---

## 🧪 **1. Unit Testing**

### **1.1 Module API Tests**

#### **Power Module Tests**
```python
def test_power_status():
    """Test Power Module status endpoint"""
    response = requests.get("http://localhost:8000/api/v1/power/status")
    assert response.status_code == 200
    
    data = response.json()
    battery = data["data"]["battery"]
    
    # Validate 24V system
    assert 20.0 <= battery["voltage"] <= 28.8
    assert battery["capacity"] == 50.0
```

#### **Safety Module Tests**
```python
def test_safety_estop():
    """Test Safety Module E-Stop functionality"""
    # Trigger E-Stop
    response = requests.post("http://localhost:8000/api/v1/safety/estop/trigger")
    assert response.status_code == 200
    
    # Check system state
    status_response = requests.get("http://localhost:8000/api/v1/master-control/status")
    system_state = status_response.json()["data"]["system_state"]["current_state"]
    assert system_state == "estop"
```

#### **Travel Motor Tests**
```python
def test_motor_control():
    """Test Travel Motor control"""
    control_data = {"speed": 500.0, "direction": 45.0}
    response = requests.post("http://localhost:8000/api/v1/dc-motor/control", json=control_data)
    assert response.status_code == 200
```

#### **Dock & Location Tests**
```python
def test_lidar_status():
    """Test LiDAR USB integration"""
    response = requests.get("http://localhost:8000/api/v1/dock-location/lidar/status")
    assert response.status_code == 200
    
    lidar = response.json()["data"]
    assert lidar["status"] in ["active", "inactive", "error"]
```

---

## 🔗 **2. Integration Testing**

### **2.1 Module Communication Tests**
```python
def test_module_discovery():
    """Test automatic module discovery"""
    response = requests.get("http://localhost:8000/api/v1/master-control/status")
    modules = response.json()["data"]["modules"]
    
    expected_modules = ["power", "safety", "travel_motor", "dock_location", "master_control"]
    for module in expected_modules:
        assert modules[module]["status"] == "online"

def test_rs485_health():
    """Test RS485 communication health"""
    response = requests.get("http://localhost:8000/api/v1/telemetry/current")
    rs485 = response.json()["data"]["communication"]["rs485"]
    
    assert rs485["status"] == "healthy"
    assert rs485["error_rate"] <= 0.01
    assert rs485["response_time"] <= 0.1
```

### **2.2 Mission Execution Tests**
```python
def test_mission_execution():
    """Test complete mission execution"""
    mission_data = {
        "mission_name": "Test Mission",
        "mission_type": "transport",
        "waypoints": [
            {"x": 0, "y": 0, "action": "start"},
            {"x": 100, "y": 0, "action": "move"},
            {"x": 100, "y": 0, "action": "stop"}
        ]
    }
    
    # Create and execute mission
    create_response = requests.post("http://localhost:8000/api/v1/master-control/missions", json=mission_data)
    mission_id = create_response.json()["data"]["mission_id"]
    
    start_response = requests.post(f"http://localhost:8000/api/v1/master-control/missions/{mission_id}/start")
    assert start_response.status_code == 200
```

---

## 🚀 **3. Performance Testing**

### **3.1 Load Testing**
```python
async def test_concurrent_requests():
    """Test concurrent API requests"""
    async with aiohttp.ClientSession() as session:
        tasks = [session.get("http://localhost:8000/api/v1/telemetry/current") for _ in range(50)]
        responses = await asyncio.gather(*tasks)
        
        success_count = sum(1 for r in responses if r.status == 200)
        assert success_count >= 45  # 90% success rate
```

### **3.2 Response Time Testing**
```python
def test_response_times():
    """Test API response times"""
    start_time = time.time()
    response = requests.get("http://localhost:8000/api/v1/telemetry/current")
    response_time = time.time() - start_time
    
    assert response.status_code == 200
    assert response_time <= 1.0  # Max 1 second
```

---

## 🔒 **4. Security Testing**

### **4.1 Authentication Tests**
```python
def test_authentication():
    """Test authentication requirements"""
    response = requests.get("http://localhost:8000/api/v1/config/system")
    assert response.status_code == 401  # Unauthorized without token
```

### **4.2 Input Validation Tests**
```python
def test_input_validation():
    """Test input validation"""
    invalid_data = {"mission_name": "Test", "waypoints": [{"x": "invalid"}]}
    response = requests.post("http://localhost:8000/api/v1/master-control/missions", json=invalid_data)
    assert response.status_code == 400  # Bad Request
```

---

## 📊 **5. Test Automation**

### **5.1 pytest Configuration**
```ini
# pytest.ini
[tool:pytest]
testpaths = tests
python_files = test_*.py
addopts = -v --tb=short
markers =
    unit: Unit tests
    integration: Integration tests
    performance: Performance tests
```

### **5.2 Test Directory Structure**
```
tests/
├── unit/
│   ├── test_power_module.py
│   ├── test_safety_module.py
│   ├── test_travel_motor.py
│   ├── test_dock_location.py
│   └── test_master_control.py
├── integration/
│   ├── test_module_communication.py
│   └── test_mission_execution.py
├── performance/
│   └── test_load.py
└── security/
    └── test_authentication.py
```

---

## 📈 **6. Test Reporting**

### **6.1 Coverage Report**
```bash
# Run tests with coverage
pytest --cov=app --cov-report=html --cov-report=xml

# Generate coverage report
coverage report --show-missing
```

### **6.2 Performance Metrics**
```python
def generate_performance_report():
    """Generate performance test report"""
    return {
        "average_response_time": 0.15,
        "max_response_time": 0.8,
        "requests_per_second": 100,
        "error_rate": 0.001,
        "module_coverage": {
            "power": 95.0,
            "safety": 92.0,
            "travel_motor": 88.0,
            "dock_location": 90.0,
            "master_control": 94.0
        }
    }
```

---

## 📚 **Tham chiếu**

- [Module Integration Guide](MODULE_INTEGRATION_GUIDE.md)
- [API Specifications](api-specs/README.md)
- [Business Logic](../02-ARCHITECTURE/BUSINESS_LOGIC_SPEC.md)

---

**Changelog v2.0:**
- ✅ Created comprehensive testing procedures guide
- ✅ Added unit tests for all 5 mandatory modules
- ✅ Added integration tests for module communication
- ✅ Added performance and security tests
- ✅ Added test automation configuration
- ✅ Standardized to 24V nominal voltage
