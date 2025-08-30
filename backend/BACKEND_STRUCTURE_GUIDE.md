# 🏗️ BACKEND STRUCTURE GUIDE - OHT-50

**Phiên bản:** 1.0  
**Ngày tạo:** 2025-01-28  
**Mục tiêu:** Hướng dẫn team BE implement theo cấu trúc đã thiết kế

---

## 📁 **CẤU TRÚC ĐÃ TẠO**

```
backend/
├── app/                          # Main application package
│   ├── __init__.py              # Package initialization
│   ├── main.py                  # FastAPI app entry point
│   ├── config.py                # Configuration management
│   ├── api/                     # API endpoints
│   │   ├── __init__.py
│   │   ├── v1/                  # API version 1
│   │   │   ├── __init__.py      # V1 router (đã tạo)
│   │   │   ├── robot.py         # TODO: Robot control endpoints
│   │   │   ├── telemetry.py     # TODO: Telemetry endpoints
│   │   │   ├── safety.py        # TODO: Safety endpoints
│   │   │   ├── config.py        # TODO: Configuration endpoints
│   │   │   └── auth.py          # TODO: Authentication endpoints
│   │   └── websocket.py         # TODO: WebSocket endpoints
│   ├── core/                    # Core functionality
│   │   ├── __init__.py
│   │   ├── security.py          # Authentication & authorization
│   │   ├── database.py          # Database connection
│   │   ├── monitoring.py        # Performance monitoring
│   │   └── integration.py       # TODO: RS485 integration
│   ├── services/                # Business logic services
│   │   ├── __init__.py
│   │   ├── robot_control.py     # TODO: Robot control logic
│   │   ├── telemetry.py         # TODO: Telemetry processing
│   │   ├── safety.py            # TODO: Safety system logic
│   │   ├── lidar_processing.py  # TODO: LiDAR processing
│   │   └── firmware_integration.py # TODO: RS485 communication
│   ├── models/                  # Database models
│   │   ├── __init__.py
│   │   ├── robot.py             # TODO: Robot models
│   │   ├── telemetry.py         # TODO: Telemetry models
│   │   ├── safety.py            # TODO: Safety models
│   │   └── user.py              # TODO: User models
│   └── schemas/                 # Pydantic schemas
│       ├── __init__.py
│       ├── robot.py             # TODO: Robot schemas
│       ├── telemetry.py         # TODO: Telemetry schemas
│       └── safety.py            # TODO: Safety schemas
├── tests/                       # Test suite
│   ├── __init__.py
│   ├── conftest.py              # Test configuration
│   ├── unit/                    # Unit tests
│   ├── integration/             # Integration tests
│   └── performance/             # Performance tests
├── docker/                      # Docker configuration
│   ├── Dockerfile               # Container definition
│   └── docker-compose.yml       # Multi-service setup
├── docs/                        # Documentation
├── scripts/                     # Utility scripts
├── requirements.txt             # Python dependencies
├── pyproject.toml              # Project configuration
├── env.example                 # Environment variables template
└── README.md                   # Project documentation
```

---

## 🚀 **NEXT STEPS CHO TEAM BE**

### **Phase 1: Core Implementation (Priority 1)**

#### **1. Database Models (`app/models/`)**
```python
# app/models/robot.py
from sqlalchemy import Column, Integer, String, Float, DateTime, Boolean
from app.core.database import Base

class Robot(Base):
    __tablename__ = "robots"
    
    id = Column(Integer, primary_key=True, index=True)
    robot_id = Column(String, unique=True, index=True)
    status = Column(String)  # idle, moving, docked, error
    position_x = Column(Float)
    position_y = Column(Float)
    battery_level = Column(Integer)
    temperature = Column(Float)
    created_at = Column(DateTime)
    updated_at = Column(DateTime)
```

#### **2. Pydantic Schemas (`app/schemas/`)**
```python
# app/schemas/robot.py
from pydantic import BaseModel
from typing import Optional
from datetime import datetime

class RobotStatus(BaseModel):
    robot_id: str
    status: str
    position_x: Optional[float] = None
    position_y: Optional[float] = None
    battery_level: Optional[int] = None
    temperature: Optional[float] = None
    updated_at: datetime
```

#### **3. API Endpoints (`app/api/v1/`)**
```python
# app/api/v1/robot.py
from fastapi import APIRouter, Depends, HTTPException
from app.schemas.robot import RobotStatus
from app.services.robot_control import RobotControlService

router = APIRouter(prefix="/robot", tags=["robot"])

@router.get("/status", response_model=RobotStatus)
async def get_robot_status():
    """Get current robot status"""
    # Implementation here
    pass

@router.post("/control")
async def control_robot(command: dict):
    """Send control command to robot"""
    # Implementation here
    pass
```

### **Phase 2: Services Implementation (Priority 2)**

#### **1. Robot Control Service (`app/services/robot_control.py`)**
```python
# app/services/robot_control.py
from app.core.integration import RS485Service

class RobotControlService:
    def __init__(self):
        self.rs485_service = RS485Service()
    
    async def get_status(self) -> dict:
        """Get robot status via RS485"""
        # Implementation here
        pass
    
    async def send_command(self, command: dict) -> bool:
        """Send command to robot via RS485"""
        # Implementation here
        pass
```

#### **2. RS485 Integration (`app/core/integration.py`)**
```python
# app/core/integration.py
import asyncio
from pymodbus.client import ModbusSerialClient
from app.config import settings

class RS485Service:
    def __init__(self):
        self.client = ModbusSerialClient(
            port=settings.rs485_port,
            baudrate=settings.rs485_baud_rate,
            timeout=settings.rs485_timeout
        )
    
    async def read_register(self, address: int, register: int) -> int:
        """Read Modbus register"""
        # Implementation here
        pass
    
    async def write_register(self, address: int, register: int, value: int) -> bool:
        """Write Modbus register"""
        # Implementation here
        pass
```

### **Phase 3: Testing Implementation (Priority 3)**

#### **1. Unit Tests (`tests/unit/`)**
```python
# tests/unit/test_robot_control.py
import pytest
from app.services.robot_control import RobotControlService

class TestRobotControl:
    async def test_get_status(self):
        service = RobotControlService()
        status = await service.get_status()
        assert status is not None
```

#### **2. Integration Tests (`tests/integration/`)**
```python
# tests/integration/test_api_endpoints.py
import pytest
from fastapi.testclient import TestClient
from app.main import create_app

def test_robot_status_endpoint():
    app = create_app()
    client = TestClient(app)
    response = client.get("/api/v1/robot/status")
    assert response.status_code == 200
```

---

## 🔧 **IMPLEMENTATION ORDER**

### **Week 1: Foundation**
1. ✅ **Cấu trúc đã tạo** - Hoàn thành
2. **Database Models** - Implement robot, telemetry, safety models
3. **Pydantic Schemas** - Create request/response schemas
4. **Basic API endpoints** - GET /robot/status, POST /robot/control

### **Week 2: Core Services**
1. **RS485 Integration** - Implement Modbus RTU communication
2. **Robot Control Service** - Business logic for robot control
3. **Telemetry Service** - Data processing and storage
4. **Safety Service** - Emergency stop and safety logic

### **Week 3: Advanced Features**
1. **WebSocket Service** - Real-time communication
2. **Authentication** - JWT implementation
3. **Rate Limiting** - API protection
4. **Monitoring** - Performance metrics

### **Week 4: Testing & Deployment**
1. **Unit Tests** - Service layer testing
2. **Integration Tests** - API endpoint testing
3. **Performance Tests** - Load testing
4. **Docker Deployment** - Production deployment

---

## 📊 **PERFORMANCE REQUIREMENTS**

### **Response Time Targets**
- **GET /robot/status**: < 50ms
- **POST /robot/control**: < 100ms
- **GET /telemetry/current**: < 50ms
- **POST /safety/emergency**: < 10ms

### **WebSocket Latency**
- **Telemetry updates**: < 20ms
- **Status changes**: < 50ms
- **Alert notifications**: < 100ms

### **Database Operations**
- **Simple queries**: < 5ms
- **Complex queries**: < 50ms
- **Write operations**: < 10ms

---

## 🔒 **SECURITY REQUIREMENTS**

### **Authentication**
- JWT-based authentication
- Token expiration: 1 hour
- Secure password hashing (bcrypt)

### **Authorization**
- Role-based access control
- API endpoint protection
- Rate limiting: 1000 requests/minute

### **Data Protection**
- Input validation
- SQL injection prevention
- XSS protection
- CORS configuration

---

## 🧪 **TESTING STRATEGY**

### **Unit Tests**
- Service layer logic
- Utility functions
- Data validation

### **Integration Tests**
- API endpoints
- Database operations
- External service integration

### **Performance Tests**
- Load testing
- Stress testing
- Latency measurement

### **Security Tests**
- Authentication testing
- Authorization testing
- Input validation testing

---

## 🚀 **DEPLOYMENT CHECKLIST**

### **Development**
- [ ] Environment variables configured
- [ ] Database migrations applied
- [ ] Redis connection tested
- [ ] RS485 communication verified

### **Testing**
- [ ] All unit tests passing
- [ ] Integration tests passing
- [ ] Performance tests meeting targets
- [ ] Security tests passing

### **Production**
- [ ] Docker images built
- [ ] Health checks configured
- [ ] Monitoring alerts setup
- [ ] Backup procedures in place

---

## 📚 **RESOURCES**

### **Documentation**
- [FastAPI Documentation](https://fastapi.tiangolo.com/)
- [SQLAlchemy Documentation](https://docs.sqlalchemy.org/)
- [Pydantic Documentation](https://pydantic-docs.helpmanual.io/)
- [PyModbus Documentation](https://pymodbus.readthedocs.io/)

### **Best Practices**
- Follow PEP 8 coding standards
- Use type hints throughout
- Write comprehensive docstrings
- Implement proper error handling
- Add logging for debugging

---

**🎯 Mục tiêu:** Team BE implement theo cấu trúc này để đảm bảo performance, security và maintainability cho OHT-50 backend system.
