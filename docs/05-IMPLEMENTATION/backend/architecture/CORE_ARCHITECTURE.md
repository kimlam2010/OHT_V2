# CORE ARCHITECTURE - OHT-50 Backend

**Phiên bản:** v1.0  
**Phạm vi:** Kiến trúc tổng thể hệ thống backend OHT-50  
**Cập nhật:** 2024-12-19

---

## 📋 **Tổng quan**

Tài liệu này mô tả kiến trúc tổng thể của hệ thống backend OHT-50, bao gồm layered architecture, event-driven design, và các thành phần chính của hệ thống.

---

## 🏗️ **1. Kiến trúc tổng thể**

### **1.1 Layered Architecture**

```mermaid
graph TB
    subgraph "Presentation Layer"
        API[FastAPI REST API]
        WS[WebSocket Server]
        UI[Frontend Integration]
    end
    
    subgraph "Business Logic Layer"
        BL[Business Logic Services]
        SM[State Management]
        VM[Validation & Rules]
    end
    
    subgraph "Integration Layer"
        FW[Firmware Integration]
        CT[Center Integration]
        EX[External Services]
    end
    
    subgraph "Data Layer"
        DB[(PostgreSQL)]
        CACHE[(Redis Cache)]
        FS[File Storage]
    end
    
    API --> BL
    WS --> BL
    UI --> BL
    BL --> SM
    BL --> VM
    BL --> FW
    BL --> CT
    BL --> EX
    BL --> DB
    BL --> CACHE
    BL --> FS
```

### **1.2 Component Overview**

| Layer | Components | Responsibilities |
|-------|------------|------------------|
| **Presentation** | FastAPI, WebSocket | API endpoints, real-time communication |
| **Business Logic** | Mission Manager, Safety Manager | Core business rules, state management |
| **Integration** | Firmware Client, Center Client | External system communication |
| **Data** | PostgreSQL, Redis | Data persistence, caching |

---

## 🔧 **2. Core Components**

### **2.1 Application Core**

#### **FastAPI Application**
```python
class OHT50Application:
    def __init__(self):
        self.app = FastAPI(
            title="OHT-50 Backend",
            version="1.0.0",
            description="Backend system for OHT-50 autonomous vehicle"
        )
        self.setup_middleware()
        self.setup_routes()
        self.setup_events()
    
    def setup_middleware(self):
        # CORS, authentication, logging
        pass
    
    def setup_routes(self):
        # API routes, WebSocket endpoints
        pass
    
    def setup_events(self):
        # Startup/shutdown events
        pass
```

#### **Service Layer**
```python
class ServiceLayer:
    def __init__(self):
        self.mission_service = MissionService()
        self.safety_service = SafetyService()
        self.telemetry_service = TelemetryService()
        self.config_service = ConfigurationService()
        self.state_service = StateService()
```

### **2.2 Event-Driven Architecture**

#### **Event Bus**
```python
class EventBus:
    def __init__(self):
        self.subscribers = defaultdict(list)
        self.event_queue = asyncio.Queue()
    
    async def publish(self, event: Event):
        await self.event_queue.put(event)
    
    async def subscribe(self, event_type: str, handler: Callable):
        self.subscribers[event_type].append(handler)
    
    async def process_events(self):
        while True:
            event = await self.event_queue.get()
            await self.handle_event(event)
```

#### **Event Types**
```python
class EventType(str, Enum):
    # System Events
    SYSTEM_STARTUP = "system.startup"
    SYSTEM_SHUTDOWN = "system.shutdown"
    SYSTEM_ERROR = "system.error"
    
    # Mission Events
    MISSION_STARTED = "mission.started"
    MISSION_COMPLETED = "mission.completed"
    MISSION_FAILED = "mission.failed"
    
    # Safety Events
    SAFETY_VIOLATION = "safety.violation"
    ESTOP_TRIGGERED = "estop.triggered"
    SAFETY_CLEAR = "safety.clear"
    
    # Telemetry Events
    TELEMETRY_UPDATE = "telemetry.update"
    POSITION_CHANGE = "position.change"
    STATUS_CHANGE = "status.change"
```

---

## 🔌 **3. Integration Architecture**

### **3.1 Center Integration**

#### **Center Client**
```python
class CenterClient:
    def __init__(self, base_url: str, api_key: str):
        self.base_url = base_url
        self.api_key = api_key
        self.session = aiohttp.ClientSession()
    
    async def send_status(self, status: SystemStatus):
        """Send system status to center"""
        pass
    
    async def receive_mission(self) -> Optional[Mission]:
        """Receive mission from center"""
        pass
    
    async def request_permission(self, action: Action) -> bool:
        """Request permission for action"""
        pass
```

#### **Integration Patterns**
- **REST API:** HTTP/HTTPS communication
- **WebSocket:** Real-time bidirectional communication
- **Message Queue:** Reliable message delivery
- **Event Streaming:** Real-time event propagation

### **3.2 Firmware Integration**

#### **Firmware Client**
```python
class FirmwareClient:
    def __init__(self):
        self.rs485_client = RS485Client()
        self.ethernet_client = EthernetClient()
    
    async def send_command(self, command: FirmwareCommand):
        """Send command to firmware"""
        pass
    
    async def read_telemetry(self) -> TelemetryData:
        """Read telemetry from firmware"""
        pass
    
    async def get_module_status(self, module_id: str) -> ModuleStatus:
        """Get status of specific module"""
        pass
```

#### **Communication Protocols**
- **RS485:** Modbus RTU cho motor control
- **Ethernet:** TCP/IP cho high-level commands
- **UART:** Serial communication cho debug
- **GPIO:** Digital I/O cho sensors/actuators

---

## 💾 **4. Data Architecture**

### **4.1 Data Models**

#### **Core Models**
```python
class SystemConfig(BaseModel):
    id: str
    version: str
    max_velocity: float
    max_acceleration: float
    safety_zones: List[SafetyZone]
    created_at: datetime
    updated_at: datetime

class TelemetryData(BaseModel):
    timestamp: datetime
    position: Position
    velocity: Velocity
    acceleration: Acceleration
    battery_level: float
    temperature: float
    status: SystemStatus

class Mission(BaseModel):
    id: str
    mission_type: MissionType
    start_position: Position
    target_position: Position
    priority: Priority
    status: MissionStatus
    created_at: datetime
    started_at: Optional[datetime]
    completed_at: Optional[datetime]
```

#### **Repository Pattern**
```python
class Repository[T]:
    def __init__(self, model: Type[T]):
        self.model = model
    
    async def create(self, entity: T) -> T:
        """Create new entity"""
        pass
    
    async def read(self, id: str) -> Optional[T]:
        """Read entity by ID"""
        pass
    
    async def update(self, entity: T) -> T:
        """Update existing entity"""
        pass
    
    async def delete(self, id: str) -> bool:
        """Delete entity by ID"""
        pass
    
    async def list(self, filters: Dict = None) -> List[T]:
        """List entities with filters"""
        pass
```

### **4.2 Database Design**

#### **PostgreSQL Schema**
```sql
-- System Configuration
CREATE TABLE system_configs (
    id UUID PRIMARY KEY,
    version VARCHAR(50) NOT NULL,
    config_data JSONB NOT NULL,
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);

-- Telemetry Data
CREATE TABLE telemetry_data (
    id UUID PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    position_x FLOAT NOT NULL,
    position_y FLOAT NOT NULL,
    position_z FLOAT NOT NULL,
    velocity_linear FLOAT NOT NULL,
    velocity_angular FLOAT NOT NULL,
    battery_level FLOAT NOT NULL,
    temperature FLOAT NOT NULL,
    status VARCHAR(50) NOT NULL
);

-- Missions
CREATE TABLE missions (
    id UUID PRIMARY KEY,
    mission_type VARCHAR(50) NOT NULL,
    start_position JSONB NOT NULL,
    target_position JSONB NOT NULL,
    priority INTEGER NOT NULL,
    status VARCHAR(50) NOT NULL,
    created_at TIMESTAMP DEFAULT NOW(),
    started_at TIMESTAMP,
    completed_at TIMESTAMP
);
```

#### **Redis Caching**
```python
class CacheManager:
    def __init__(self):
        self.redis = redis.Redis(host='localhost', port=6379, db=0)
    
    async def get(self, key: str) -> Optional[str]:
        """Get value from cache"""
        pass
    
    async def set(self, key: str, value: str, ttl: int = 3600):
        """Set value in cache with TTL"""
        pass
    
    async def delete(self, key: str):
        """Delete value from cache"""
        pass
```

---

## 🛡️ **5. Security Architecture**

### **5.1 Authentication & Authorization**

#### **JWT Authentication**
```python
class AuthManager:
    def __init__(self, secret_key: str):
        self.secret_key = secret_key
    
    def create_token(self, user_id: str, roles: List[str]) -> str:
        """Create JWT token"""
        payload = {
            "user_id": user_id,
            "roles": roles,
            "exp": datetime.utcnow() + timedelta(minutes=30)
        }
        return jwt.encode(payload, self.secret_key, algorithm="HS256")
    
    def verify_token(self, token: str) -> Optional[Dict]:
        """Verify JWT token"""
        try:
            return jwt.decode(token, self.secret_key, algorithms=["HS256"])
        except jwt.InvalidTokenError:
            return None
```

#### **Role-Based Access Control**
```python
class RBACManager:
    def __init__(self):
        self.roles = {
            "admin": ["read", "write", "delete", "configure"],
            "operator": ["read", "write"],
            "viewer": ["read"]
        }
    
    def has_permission(self, user_roles: List[str], permission: str) -> bool:
        """Check if user has permission"""
        for role in user_roles:
            if role in self.roles and permission in self.roles[role]:
                return True
        return False
```

### **5.2 Input Validation**

#### **Pydantic Models**
```python
class ConfigUpdateRequest(BaseModel):
    max_velocity: float = Field(gt=0, le=10.0)
    max_acceleration: float = Field(gt=0, le=5.0)
    safety_zones: List[SafetyZone]
    
    @validator('max_velocity')
    def validate_velocity(cls, v):
        if v > 2.0:
            raise ValueError('Velocity too high for safety')
        return v
```

---

## 📊 **6. Monitoring & Observability**

### **6.1 Logging**

#### **Structured Logging**
```python
import structlog

logger = structlog.get_logger()

def log_system_event(event_type: str, data: Dict):
    logger.info(
        "system_event",
        event_type=event_type,
        data=data,
        timestamp=datetime.utcnow().isoformat()
    )
```

#### **Log Levels**
- **DEBUG:** Detailed debugging information
- **INFO:** General operational messages
- **WARNING:** Warning messages
- **ERROR:** Error conditions
- **CRITICAL:** Critical system failures

### **6.2 Metrics**

#### **Prometheus Metrics**
```python
from prometheus_client import Counter, Histogram, Gauge

# Counters
mission_counter = Counter('missions_total', 'Total missions', ['status'])
error_counter = Counter('errors_total', 'Total errors', ['type'])

# Histograms
response_time = Histogram('http_request_duration_seconds', 'HTTP request duration')

# Gauges
system_status = Gauge('system_status', 'System status', ['component'])
battery_level = Gauge('battery_level_percent', 'Battery level percentage')
```

### **6.3 Health Checks**

#### **Health Endpoints**
```python
@app.get("/health")
async def health_check():
    return {
        "status": "healthy",
        "timestamp": datetime.utcnow().isoformat(),
        "version": "1.0.0",
        "components": {
            "database": check_database_health(),
            "firmware": check_firmware_health(),
            "center": check_center_health()
        }
    }
```

---

## ⚙️ **7. Configuration Management**

### **7.1 Configuration Sources**

#### **Environment Variables**
```python
class Settings(BaseSettings):
    # Database
    database_url: str = "postgresql://user:pass@localhost/oht50"
    
    # Center Integration
    center_api_url: str = "https://center.example.com/api"
    center_api_key: str
    
    # Firmware
    rs485_port: str = "/dev/ttyS1"
    rs485_baudrate: int = 115200
    
    # Security
    jwt_secret_key: str
    cors_origins: List[str] = ["http://localhost:3000"]
    
    class Config:
        env_file = ".env"
```

#### **Configuration Files**
```yaml
# config.yaml
system:
  max_velocity: 2.0
  max_acceleration: 1.0
  safety_zones:
    - id: "zone-001"
      type: "restricted"
      boundaries: [[0, 0], [100, 0], [100, 50], [0, 50]]

telemetry:
  rate: 10
  storage_retention_days: 30

firmware:
  rs485:
    port: "/dev/ttyS1"
    baudrate: 115200
    timeout: 1.0
```

### **7.2 Configuration Validation**

#### **Schema Validation**
```python
class ConfigValidator:
    def validate_config(self, config: Dict) -> ValidationResult:
        """Validate configuration against schema"""
        try:
            validated_config = SystemConfig(**config)
            return ValidationResult(success=True, data=validated_config)
        except ValidationError as e:
            return ValidationResult(success=False, errors=e.errors())
```

---

## 🧪 **8. Testing Architecture**

### **8.1 Testing Strategy**

#### **Test Pyramid**
```
    /\
   /  \     E2E Tests (Few)
  /____\    Integration Tests (Some)
 /______\   Unit Tests (Many)
```

#### **Test Types**
- **Unit Tests:** Individual components
- **Integration Tests:** Component interactions
- **End-to-End Tests:** Full system workflows
- **Performance Tests:** Load and stress testing
- **Security Tests:** Vulnerability assessment

### **8.2 Test Infrastructure**

#### **Test Database**
```python
@pytest.fixture
async def test_db():
    """Create test database"""
    database_url = "postgresql://test:test@localhost/test_oht50"
    engine = create_async_engine(database_url)
    
    async with engine.begin() as conn:
        await conn.run_sync(Base.metadata.create_all)
    
    yield engine
    
    async with engine.begin() as conn:
        await conn.run_sync(Base.metadata.drop_all)
```

#### **Mock Services**
```python
class MockFirmwareClient:
    def __init__(self):
        self.telemetry_data = []
        self.commands_sent = []
    
    async def send_command(self, command: FirmwareCommand):
        self.commands_sent.append(command)
        return CommandResponse(success=True)
    
    async def read_telemetry(self) -> TelemetryData:
        return TelemetryData(
            timestamp=datetime.utcnow(),
            position=Position(x=0, y=0, z=0),
            velocity=Velocity(linear=0, angular=0),
            battery_level=100.0,
            temperature=25.0,
            status=SystemStatus.IDLE
        )
```

---

## 🚀 **9. Deployment Architecture**

### **9.1 Container Architecture**

#### **Docker Configuration**
```dockerfile
# Dockerfile
FROM python:3.11-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install -r requirements.txt

COPY . .

EXPOSE 8000

CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
```

#### **Docker Compose**
```yaml
# docker-compose.yml
version: '3.8'

services:
  backend:
    build: .
    ports:
      - "8000:8000"
    environment:
      - DATABASE_URL=postgresql://user:pass@db:5432/oht50
    depends_on:
      - db
      - redis
  
  db:
    image: postgres:15
    environment:
      - POSTGRES_DB=oht50
      - POSTGRES_USER=user
      - POSTGRES_PASSWORD=pass
    volumes:
      - postgres_data:/var/lib/postgresql/data
  
  redis:
    image: redis:7-alpine
    ports:
      - "6379:6379"
  
  prometheus:
    image: prom/prometheus
    ports:
      - "9090:9090"
    volumes:
      - ./prometheus.yml:/etc/prometheus/prometheus.yml

volumes:
  postgres_data:
```

### **9.2 Production Deployment**

#### **Systemd Service**
```ini
# /etc/systemd/system/oht50-backend.service
[Unit]
Description=OHT-50 Backend Service
After=network.target postgresql.service

[Service]
Type=simple
User=oht50
WorkingDirectory=/opt/oht50-backend
Environment=PATH=/opt/oht50-backend/venv/bin
ExecStart=/opt/oht50-backend/venv/bin/uvicorn main:app --host 0.0.0.0 --port 8000
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

---

## 📚 **10. References**

### **10.1 Related Documents**
- [Business Logic Specification](business/BUSINESS_LOGIC_SPEC.md) - Core business logic
- [API Specifications](api-specs/README.md) - API documentation
- [Deployment Guide](DEPLOYMENT.md) - Deployment procedures

### **10.2 Technology Stack**
- **Framework:** FastAPI
- **Database:** PostgreSQL
- **Cache:** Redis
- **Message Queue:** Redis (temporary)
- **Monitoring:** Prometheus + Grafana
- **Logging:** Structured logging
- **Testing:** pytest + pytest-asyncio

---

**Changelog v1.0:**
- ✅ Created comprehensive core architecture documentation
- ✅ Defined layered architecture and components
- ✅ Added event-driven architecture design
- ✅ Included integration patterns and protocols
- ✅ Added data architecture and models
- ✅ Defined security architecture
- ✅ Added monitoring and observability
- ✅ Included testing and deployment architecture
- ✅ Referenced related documents and technology stack
