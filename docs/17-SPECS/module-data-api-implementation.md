# Module Data API Implementation Plan - OHT-50

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** Implementation plan cho module data API endpoints

---

## 🚀 **IMPLEMENTATION PHASES**

### **Phase 1: Core API Endpoints (Week 1-2)**
- Module status endpoints
- Basic module data retrieval
- WebSocket connection setup

### **Phase 2: Advanced Features (Week 3-4)**
- Module configuration management
- Command execution
- Historical data access

### **Phase 3: Optimization & Testing (Week 5-6)**
- Performance optimization
- Error handling
- Integration testing

---

## 🔌 **API ENDPOINT IMPLEMENTATION**

### **1. Module Status Endpoints:**

#### **GET /api/v1/modules/{module_id}/status**
```python
@router.get("/modules/{module_id}/status")
async def get_module_status(module_id: str):
    """Get current status of specific module"""
    try:
        # Get module data from firmware
        module_data = await firmware_service.get_module_data(module_id)
        
        # Process and validate data
        processed_data = await process_module_data(module_data)
        
        return APIResponse(
            success=True,
            data=processed_data,
            timestamp=datetime.utcnow().isoformat()
        )
    except ModuleNotFoundError:
        raise HTTPException(status_code=404, detail="Module not found")
    except Exception as e:
        logger.error(f"Failed to get module status: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")
```

#### **GET /api/v1/modules/status/all**
```python
@router.get("/modules/status/all")
async def get_all_modules_status():
    """Get status of all modules"""
    try:
        # Get all modules from firmware
        all_modules = await firmware_service.get_all_modules()
        
        # Process each module data
        processed_modules = {}
        for module_id, module_data in all_modules.items():
            processed_modules[module_id] = await process_module_data(module_data)
        
        return APIResponse(
            success=True,
            data=processed_modules,
            timestamp=datetime.utcnow().isoformat()
        )
    except Exception as e:
        logger.error(f"Failed to get all modules status: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")
```

### **2. Module Configuration Endpoints:**

#### **GET /api/v1/modules/{module_id}/config**
```python
@router.get("/modules/{module_id}/config")
async def get_module_config(module_id: str):
    """Get module configuration"""
    try:
        config = await firmware_service.get_module_config(module_id)
        return APIResponse(
            success=True,
            data=config,
            timestamp=datetime.utcnow().isoformat()
        )
    except ModuleNotFoundError:
        raise HTTPException(status_code=404, detail="Module not found")
    except Exception as e:
        logger.error(f"Failed to get module config: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")
```

#### **PUT /api/v1/modules/{module_id}/config**
```python
@router.put("/modules/{module_id}/config")
async def update_module_config(module_id: str, config: ModuleConfig):
    """Update module configuration"""
    try:
        # Validate configuration
        validation_result = await validate_module_config(module_id, config)
        if not validation_result.valid:
            raise HTTPException(status_code=400, detail=validation_result.errors)
        
        # Apply configuration
        result = await firmware_service.update_module_config(module_id, config)
        
        # Log configuration change
        await audit_logger.log_config_change(module_id, config, "update")
        
        return APIResponse(
            success=True,
            data=result,
            message="Configuration updated successfully",
            timestamp=datetime.utcnow().isoformat()
        )
    except ModuleNotFoundError:
        raise HTTPException(status_code=404, detail="Module not found")
    except ValidationError as e:
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        logger.error(f"Failed to update module config: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")
```

### **3. Module Commands Endpoints:**

#### **POST /api/v1/modules/{module_id}/commands**
```python
@router.post("/modules/{module_id}/commands")
async def execute_module_command(module_id: str, command: ModuleCommand):
    """Execute command on module"""
    try:
        # Validate command
        validation_result = await validate_module_command(module_id, command)
        if not validation_result.valid:
            raise HTTPException(status_code=400, detail=validation_result.errors)
        
        # Execute command
        result = await firmware_service.execute_module_command(module_id, command)
        
        # Log command execution
        await audit_logger.log_command_execution(module_id, command, result)
        
        return APIResponse(
            success=True,
            data=result,
            message="Command executed successfully",
            timestamp=datetime.utcnow().isoformat()
        )
    except ModuleNotFoundError:
        raise HTTPException(status_code=404, detail="Module not found")
    except CommandExecutionError as e:
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        logger.error(f"Failed to execute module command: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")
```

#### **POST /api/v1/modules/{module_id}/emergency_stop**
```python
@router.post("/modules/{module_id}/emergency_stop")
async def emergency_stop_module(module_id: str):
    """Emergency stop module"""
    try:
        # Execute emergency stop
        result = await firmware_service.emergency_stop_module(module_id)
        
        # Log emergency stop
        await audit_logger.log_emergency_stop(module_id, "api_request")
        
        return APIResponse(
            success=True,
            data=result,
            message="Emergency stop executed",
            timestamp=datetime.utcnow().isoformat()
        )
    except ModuleNotFoundError:
        raise HTTPException(status_code=404, detail="Module not found")
    except Exception as e:
        logger.error(f"Failed to emergency stop module: {e}")
        raise HTTPException(status_code=500, detail="Internal server error")
```

---

## 🔄 **WEBSOCKET IMPLEMENTATION**

### **WebSocket Connection Handler:**
```python
class ModuleDataWebSocket:
    def __init__(self):
        self.connections = set()
        self.module_subscriptions = {}
        
    async def connect(self, websocket: WebSocket):
        await websocket.accept()
        self.connections.add(websocket)
        
    async def disconnect(self, websocket: WebSocket):
        self.connections.discard(websocket)
        # Remove subscriptions
        for module_id, subscribers in self.module_subscriptions.items():
            subscribers.discard(websocket)
    
    async def subscribe_to_module(self, websocket: WebSocket, module_id: str):
        if module_id not in self.module_subscriptions:
            self.module_subscriptions[module_id] = set()
        self.module_subscriptions[module_id].add(websocket)
    
    async def broadcast_module_data(self, module_id: str, data: dict):
        if module_id in self.module_subscriptions:
            message = {
                "event": "module_data_update",
                "data": {
                    "module_id": module_id,
                    "data": data,
                    "timestamp": datetime.utcnow().isoformat()
                }
            }
            
            # Send to all subscribers
            for websocket in self.module_subscriptions[module_id].copy():
                try:
                    await websocket.send_json(message)
                except ConnectionClosed:
                    self.module_subscriptions[module_id].discard(websocket)
    
    async def broadcast_module_status_change(self, module_id: str, old_status: str, new_status: str):
        message = {
            "event": "module_status_change",
            "data": {
                "module_id": module_id,
                "old_status": old_status,
                "new_status": new_status,
                "timestamp": datetime.utcnow().isoformat()
            }
        }
        
        # Send to all connections
        for websocket in self.connections.copy():
            try:
                await websocket.send_json(message)
            except ConnectionClosed:
                self.connections.discard(websocket)
```

---

## 📊 **DATA PROCESSING SERVICE**

### **Module Data Processor:**
```python
class ModuleDataProcessor:
    def __init__(self):
        self.cache = RedisCache()
        self.validators = {
            'travel_motor': TravelMotorDataValidator(),
            'power': PowerDataValidator(),
            'safety': SafetyDataValidator()
        }
    
    async def process_module_data(self, module_id: str, raw_data: dict) -> dict:
        """Process and validate module data"""
        try:
            # Get module type
            module_type = await self.get_module_type(module_id)
            
            # Validate data
            validator = self.validators.get(module_type)
            if validator:
                validated_data = await validator.validate(raw_data)
            else:
                validated_data = raw_data
            
            # Process data
            processed_data = await self.process_data_by_type(module_type, validated_data)
            
            # Cache data
            await self.cache.set(f"module_data:{module_id}", processed_data, ttl=60)
            
            return processed_data
            
        except ValidationError as e:
            logger.error(f"Data validation failed for module {module_id}: {e}")
            raise
        except Exception as e:
            logger.error(f"Data processing failed for module {module_id}: {e}")
            raise
    
    async def process_data_by_type(self, module_type: str, data: dict) -> dict:
        """Process data based on module type"""
        if module_type == 'travel_motor':
            return await self.process_travel_motor_data(data)
        elif module_type == 'power':
            return await self.process_power_data(data)
        elif module_type == 'safety':
            return await self.process_safety_data(data)
        else:
            return data
    
    async def process_travel_motor_data(self, data: dict) -> dict:
        """Process travel motor specific data"""
        # Add calculated fields
        if 'velocity' in data and 'acceleration' in data:
            data['jerk'] = self.calculate_jerk(data['velocity'], data['acceleration'])
        
        # Add status indicators
        data['status'] = self.determine_motor_status(data)
        
        return data
    
    async def process_power_data(self, data: dict) -> dict:
        """Process power module specific data"""
        # Calculate efficiency
        if 'input_power' in data and 'output_power' in data:
            data['efficiency'] = (data['output_power'] / data['input_power']) * 100
        
        # Add status indicators
        data['status'] = self.determine_power_status(data)
        
        return data
    
    async def process_safety_data(self, data: dict) -> dict:
        """Process safety module specific data"""
        # Add safety status
        data['safety_status'] = self.determine_safety_status(data)
        
        # Add risk assessment
        data['risk_level'] = self.assess_risk_level(data)
        
        return data
```

---

## 🔒 **SECURITY IMPLEMENTATION**

### **Authentication Middleware:**
```python
class ModuleDataAuthMiddleware:
    def __init__(self, jwt_secret: str):
        self.jwt_secret = jwt_secret
        
    async def authenticate_request(self, request: Request) -> User:
        """Authenticate API request"""
        token = request.headers.get("Authorization")
        if not token or not token.startswith("Bearer "):
            raise HTTPException(status_code=401, detail="Invalid token")
        
        try:
            payload = jwt.decode(token[7:], self.jwt_secret, algorithms=["HS256"])
            user = await self.get_user(payload["user_id"])
            return user
        except jwt.ExpiredSignatureError:
            raise HTTPException(status_code=401, detail="Token expired")
        except jwt.InvalidTokenError:
            raise HTTPException(status_code=401, detail="Invalid token")
    
    async def authorize_module_access(self, user: User, module_id: str) -> bool:
        """Check if user has access to module"""
        # Check user permissions
        if user.role == "admin":
            return True
        
        # Check module-specific permissions
        module_permissions = await self.get_module_permissions(module_id)
        return user.role in module_permissions
```

### **Rate Limiting:**
```python
class ModuleDataRateLimiter:
    def __init__(self):
        self.rate_limits = {
            "module_status": {"requests": 1000, "window": 60},  # 1000 req/min
            "module_config": {"requests": 100, "window": 60},   # 100 req/min
            "module_commands": {"requests": 50, "window": 60},  # 50 req/min
        }
    
    async def check_rate_limit(self, endpoint: str, user_id: str) -> bool:
        """Check if user has exceeded rate limit"""
        limit_config = self.rate_limits.get(endpoint)
        if not limit_config:
            return True
        
        key = f"rate_limit:{endpoint}:{user_id}"
        current_requests = await self.cache.get(key) or 0
        
        if current_requests >= limit_config["requests"]:
            return False
        
        # Increment counter
        await self.cache.incr(key)
        await self.cache.expire(key, limit_config["window"])
        
        return True
```

---

## 📈 **MONITORING & LOGGING**

### **Performance Monitoring:**
```python
class ModuleDataMonitor:
    def __init__(self):
        self.metrics = MetricsCollector()
        
    async def record_api_call(self, endpoint: str, duration: float, status_code: int):
        """Record API call metrics"""
        self.metrics.record("api_calls_total", 1, tags={
            "endpoint": endpoint,
            "status_code": status_code
        })
        
        self.metrics.record("api_call_duration", duration, tags={
            "endpoint": endpoint
        })
        
        if duration > 1.0:  # Log slow requests
            logger.warning(f"Slow API call: {endpoint} took {duration}s")
    
    async def record_websocket_event(self, event_type: str, module_id: str):
        """Record WebSocket event metrics"""
        self.metrics.record("websocket_events_total", 1, tags={
            "event_type": event_type,
            "module_id": module_id
        })
```

### **Audit Logging:**
```python
class ModuleDataAuditLogger:
    def __init__(self):
        self.logger = logging.getLogger("audit")
        
    async def log_config_change(self, module_id: str, config: dict, action: str):
        """Log configuration changes"""
        self.logger.info(f"Config change: module={module_id}, action={action}, config={config}")
    
    async def log_command_execution(self, module_id: str, command: dict, result: dict):
        """Log command executions"""
        self.logger.info(f"Command execution: module={module_id}, command={command}, result={result}")
    
    async def log_emergency_stop(self, module_id: str, source: str):
        """Log emergency stops"""
        self.logger.critical(f"Emergency stop: module={module_id}, source={source}")
```

---

## 🧪 **TESTING IMPLEMENTATION**

### **Unit Tests:**
```python
class TestModuleDataAPI:
    async def test_get_module_status(self):
        """Test module status endpoint"""
        response = await client.get("/api/v1/modules/travel_motor_001/status")
        assert response.status_code == 200
        assert response.json()["success"] == True
        assert "data" in response.json()
    
    async def test_update_module_config(self):
        """Test module configuration update"""
        config = {"max_velocity": 1000, "acceleration": 500}
        response = await client.put("/api/v1/modules/travel_motor_001/config", json=config)
        assert response.status_code == 200
        assert response.json()["success"] == True
    
    async def test_emergency_stop(self):
        """Test emergency stop endpoint"""
        response = await client.post("/api/v1/modules/travel_motor_001/emergency_stop")
        assert response.status_code == 200
        assert response.json()["success"] == True
```

### **Integration Tests:**
```python
class TestModuleDataIntegration:
    async def test_end_to_end_data_flow(self):
        """Test complete data flow from firmware to frontend"""
        # 1. Get module data from firmware
        firmware_data = await firmware_service.get_module_data("travel_motor_001")
        
        # 2. Process data through API
        response = await client.get("/api/v1/modules/travel_motor_001/status")
        api_data = response.json()["data"]
        
        # 3. Verify data consistency
        assert api_data["module_id"] == "travel_motor_001"
        assert api_data["status"]["state"] in ["idle", "moving", "stopping", "error"]
    
    async def test_websocket_real_time_updates(self):
        """Test WebSocket real-time updates"""
        async with websockets.connect("ws://localhost:8000/ws/modules") as websocket:
            # Subscribe to module updates
            await websocket.send(json.dumps({
                "action": "subscribe",
                "module_id": "travel_motor_001"
            }))
            
            # Wait for update
            message = await websocket.recv()
            data = json.loads(message)
            
            assert data["event"] == "module_data_update"
            assert data["data"]["module_id"] == "travel_motor_001"
```

---

**Changelog v1.0:**
- ✅ Created comprehensive API implementation plan
- ✅ Defined all module data endpoints
- ✅ Implemented WebSocket real-time communication
- ✅ Added security and rate limiting
- ✅ Included monitoring and audit logging
- ✅ Added testing implementation
