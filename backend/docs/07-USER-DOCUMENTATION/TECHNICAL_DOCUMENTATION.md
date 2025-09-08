# 🔧 OHT-50 Backend Technical Documentation

## 📋 **Overview**

OHT-50 Backend is a high-performance, real-time robot control and mapping system built with FastAPI, SQLAlchemy, and WebSocket technology. It provides comprehensive robot control, mapping, localization, and sensor data management capabilities.

### **Architecture**
- **Framework:** FastAPI (Python 3.11+)
- **Database:** SQLite with async SQLAlchemy
- **Authentication:** JWT with role-based access control
- **Real-time Communication:** WebSocket
- **Testing:** Pytest with async support
- **Documentation:** OpenAPI/Swagger

---

## 🏗️ **System Architecture**

### **High-Level Architecture**
```
┌─────────────────────────────────────┐
│         Frontend (React)            │
└─────────────────┬───────────────────┘
                  │ HTTP/WebSocket
┌─────────────────▼───────────────────┐
│         Backend (FastAPI)           │
├─────────────────┬───────────────────┤
│  API Layer      │  Service Layer    │
│  - Map APIs     │  - MapService     │
│  - Sensor APIs  │  - SensorProcessor│
│  - Localization │  - Localization   │
│  - WebSocket    │  - RealtimeUpdates│
└─────────────────┼───────────────────┘
                  │ HTTP API
┌─────────────────▼───────────────────┐
│         Firmware (HTTP API)         │
└─────────────────┬───────────────────┘
                  │ RS485/Modbus
┌─────────────────▼───────────────────┐
│         Hardware Modules            │
│  - RFID Readers                     │
│  - Accelerometers                   │
│  - Proximity Sensors                │
│  - LiDAR Scanners                   │
└─────────────────────────────────────┘
```

### **Component Architecture**
```
┌─────────────────────────────────────┐
│         API Layer                   │
├─────────────────┬───────────────────┤
│  Map API        │  Sensor API       │
│  - start/stop   │  - data submit    │
│  - status       │  - configuration  │
│  - position     │  - calibration    │
│  - occupancy    │  - status         │
└─────────────────┼───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│         Service Layer               │
├─────────────────┬───────────────────┤
│  MapService     │  SensorProcessor  │
│  - mapping      │  - validation     │
│  - localization │  - filtering      │
│  - storage      │  - quality        │
└─────────────────┼───────────────────┘
                  │
┌─────────────────▼───────────────────┐
│         Data Layer                  │
├─────────────────┬───────────────────┤
│  Database       │  Cache            │
│  - SQLite       │  - In-memory      │
│  - Async        │  - Sensor data    │
│  - Migrations   │  - Position       │
└─────────────────────────────────────┘
```

---

## 🗄️ **Database Schema**

### **Core Tables**

#### **Maps Table**
```sql
CREATE TABLE maps (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    map_id VARCHAR(50) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    resolution DECIMAL(10,3) NOT NULL,
    width INTEGER NOT NULL,
    height INTEGER NOT NULL,
    occupancy_grid BLOB,
    map_metadata JSON,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### **Map Sessions Table**
```sql
CREATE TABLE map_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id VARCHAR(50) UNIQUE NOT NULL,
    map_id VARCHAR(50) NOT NULL,
    start_time TIMESTAMP NOT NULL,
    end_time TIMESTAMP,
    total_scans INTEGER DEFAULT 0,
    mapping_quality DECIMAL(3,2),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (map_id) REFERENCES maps(map_id)
);
```

#### **Robot Positions Table**
```sql
CREATE TABLE robot_positions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id VARCHAR(50) NOT NULL,
    x DECIMAL(10,3) NOT NULL,
    y DECIMAL(10,3) NOT NULL,
    theta DECIMAL(10,3) NOT NULL,
    confidence DECIMAL(3,2) NOT NULL,
    source VARCHAR(20) NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (session_id) REFERENCES map_sessions(session_id)
);
```

#### **RFID Tags Table**
```sql
CREATE TABLE rfid_tags (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    tag_id VARCHAR(50) UNIQUE NOT NULL,
    x DECIMAL(10,3) NOT NULL,
    y DECIMAL(10,3) NOT NULL,
    z DECIMAL(10,3) DEFAULT 0.0,
    tag_type VARCHAR(20) DEFAULT 'landmark',
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### **Localization Configs Table**
```sql
CREATE TABLE localization_configs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    config_key VARCHAR(50) UNIQUE NOT NULL,
    config_value JSON NOT NULL,
    config_type VARCHAR(20) NOT NULL,
    description TEXT,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### **Sensor Configurations Table**
```sql
CREATE TABLE sensor_configurations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sensor_id VARCHAR(50) UNIQUE NOT NULL,
    sensor_type VARCHAR(20) NOT NULL,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    configuration JSON,
    calibration_data JSON,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### **Sensor Readings Table**
```sql
CREATE TABLE sensor_readings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sensor_type VARCHAR(20) NOT NULL,
    sensor_id VARCHAR(50) NOT NULL,
    data JSON NOT NULL,
    quality DECIMAL(3,2) NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### **Sensor Status Table**
```sql
CREATE TABLE sensor_status (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sensor_id VARCHAR(50) UNIQUE NOT NULL,
    sensor_type VARCHAR(20) NOT NULL,
    status VARCHAR(20) NOT NULL,
    last_reading_time TIMESTAMP,
    error_count INTEGER DEFAULT 0,
    health_score DECIMAL(3,2) DEFAULT 1.0,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### **Sensor Calibrations Table**
```sql
CREATE TABLE sensor_calibrations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sensor_id VARCHAR(50) NOT NULL,
    calibration_type VARCHAR(20) NOT NULL,
    calibration_data JSON NOT NULL,
    calibration_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    FOREIGN KEY (sensor_id) REFERENCES sensor_configurations(sensor_id)
);
```

---

## 🔧 **Service Architecture**

### **MapService**
```python
class MapService:
    """Core mapping service for robot navigation and mapping"""
    
    def __init__(self):
        self.current_map = None
        self.current_session = None
        self.is_mapping = False
        self.occupancy_grid = None
        self.robot_position = None
    
    async def start_mapping(self, map_name: str, resolution: float, 
                           width: int, height: int) -> Dict[str, Any]:
        """Start a new mapping session"""
        
    async def stop_mapping(self) -> Dict[str, Any]:
        """Stop current mapping session and finalize map"""
        
    async def get_robot_position(self) -> Dict[str, Any]:
        """Get current robot position using hybrid localization"""
        
    async def update_occupancy_grid(self, scan_data: List[float]) -> None:
        """Update occupancy grid with new scan data"""
        
    async def get_map_data(self, map_id: str) -> Dict[str, Any]:
        """Get map data by ID"""
        
    async def save_map(self, map_data: Dict[str, Any]) -> str:
        """Save map to database"""
        
    async def load_map(self, map_id: str) -> Dict[str, Any]:
        """Load map from database"""
```

### **HybridLocalizationEngine**
```python
class HybridLocalizationEngine:
    """Hybrid localization engine combining multiple sensors"""
    
    def __init__(self):
        self.sensor_weights = {
            "rfid": 0.4,
            "accelerometer": 0.3,
            "proximity": 0.2,
            "lidar": 0.1
        }
        self.position_history = []
        self.current_position = None
    
    async def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize localization engine"""
        
    async def update_position(self, sensor_data: Dict[str, Any]) -> Dict[str, Any]:
        """Update robot position using sensor fusion"""
        
    async def fuse_sensors(self, sensor_readings: List[SensorReading]) -> Dict[str, Any]:
        """Fuse data from multiple sensors"""
        
    async def calculate_confidence(self, position: Dict[str, Any]) -> float:
        """Calculate position confidence score"""
        
    async def get_position_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get robot position history"""
        
    async def get_localization_stats(self) -> Dict[str, Any]:
        """Get localization statistics"""
```

### **FirmwareIntegrationService**
```python
class FirmwareIntegrationService:
    """Service for communicating with firmware via HTTP API"""
    
    def __init__(self, firmware_url: str = "http://localhost:8081"):
        self.firmware_url = firmware_url
        self.http_client = httpx.AsyncClient(base_url=firmware_url)
        self.status = FirmwareStatus.DISCONNECTED
        self.connection_errors = 0
    
    async def initialize(self) -> bool:
        """Initialize connection to firmware"""
        
    async def get_sensor_data(self, sensor_type: SensorType, 
                             sensor_id: str = None) -> Optional[SensorReading]:
        """Get sensor data from firmware"""
        
    async def send_robot_command(self, command: Dict[str, Any]) -> bool:
        """Send robot command to firmware"""
        
    async def get_robot_status(self) -> Optional[Dict[str, Any]]:
        """Get robot status from firmware"""
        
    async def configure_sensor(self, sensor_id: str, 
                              config: Dict[str, Any]) -> bool:
        """Configure sensor via firmware"""
        
    async def calibrate_sensor(self, sensor_id: str, 
                              calibration_data: Dict[str, Any]) -> bool:
        """Calibrate sensor via firmware"""
        
    async def heartbeat(self) -> bool:
        """Send heartbeat to firmware"""
```

### **SensorDataProcessor**
```python
class SensorDataProcessor:
    """Service for processing and validating sensor data"""
    
    def __init__(self):
        self.validation_rules = {}
        self.data_history = {}
        self.max_history_size = 100
    
    async def process_sensor_data(self, sensor_reading: SensorReading) -> ProcessingResult:
        """Process sensor data with validation and filtering"""
        
    async def validate_sensor_data(self, sensor_reading: SensorReading) -> Dict[str, Any]:
        """Validate sensor data against rules"""
        
    async def apply_filters(self, sensor_reading: SensorReading) -> Dict[str, Any]:
        """Apply filters to sensor data"""
        
    async def calculate_quality_score(self, sensor_reading: SensorReading, 
                                     filtered_data: Dict[str, Any]) -> float:
        """Calculate quality score for sensor data"""
        
    async def apply_additional_processing(self, sensor_reading: SensorReading, 
                                         filtered_data: Dict[str, Any]) -> Dict[str, Any]:
        """Apply additional sensor-specific processing"""
```

### **RealtimeUpdatesService**
```python
class RealtimeUpdatesService:
    """Service for real-time data streaming via WebSocket"""
    
    def __init__(self):
        self.update_queues = {}
        self.subscribers = {}
        self.update_tasks = {}
        self.is_running = False
        self.frequencies = {
            UpdateType.POSITION: UpdateFrequency.HIGH,  # 20Hz
            UpdateType.TELEMETRY: UpdateFrequency.MEDIUM,  # 10Hz
            UpdateType.SENSOR_DATA: UpdateFrequency.MEDIUM,  # 10Hz
            UpdateType.MAP_UPDATE: UpdateFrequency.LOW,  # 5Hz
            UpdateType.ALERT: UpdateFrequency.VERY_LOW,  # 1Hz
            UpdateType.STATUS: UpdateFrequency.LOW  # 5Hz
        }
    
    async def start(self) -> bool:
        """Start real-time updates service"""
        
    async def stop(self):
        """Stop real-time updates service"""
        
    async def subscribe(self, client_id: str, update_types: List[UpdateType]) -> bool:
        """Subscribe client to update types"""
        
    async def unsubscribe(self, client_id: str, update_types: List[UpdateType] = None) -> bool:
        """Unsubscribe client from update types"""
        
    async def publish_update(self, update: RealtimeUpdate) -> bool:
        """Publish real-time update"""
        
    async def publish_position_update(self, position_data: Dict[str, Any]) -> bool:
        """Publish position update"""
        
    async def publish_telemetry_update(self, telemetry_data: Dict[str, Any]) -> bool:
        """Publish telemetry update"""
        
    async def publish_sensor_data_update(self, sensor_data: Dict[str, Any]) -> bool:
        """Publish sensor data update"""
        
    async def publish_map_update(self, map_data: Dict[str, Any]) -> bool:
        """Publish map update"""
        
    async def publish_alert(self, alert_data: Dict[str, Any]) -> bool:
        """Publish alert"""
```

---

## 🔄 **Data Flow**

### **Mapping Workflow**
```
1. Start Mapping
   ├── Create new map record
   ├── Initialize occupancy grid
   ├── Start mapping session
   └── Return map_id and session_id

2. Sensor Data Collection
   ├── Receive sensor data from firmware
   ├── Validate and process data
   ├── Update robot position
   ├── Update occupancy grid
   └── Store in database

3. Real-time Updates
   ├── Publish position updates (20Hz)
   ├── Publish telemetry updates (10Hz)
   ├── Publish sensor data (10Hz)
   └── Publish map updates (5Hz)

4. Stop Mapping
   ├── Finalize occupancy grid
   ├── Calculate mapping quality
   ├── Save map to database
   └── End mapping session
```

### **Localization Workflow**
```
1. Sensor Data Input
   ├── RFID data (position landmarks)
   ├── Accelerometer data (motion)
   ├── Proximity data (obstacles)
   └── LiDAR data (environment)

2. Data Processing
   ├── Validate sensor data
   ├── Apply filters and smoothing
   ├── Calculate quality scores
   └── Store in history

3. Sensor Fusion
   ├── Weight sensor contributions
   ├── Fuse position estimates
   ├── Calculate confidence score
   └── Update robot position

4. Position Update
   ├── Store position in database
   ├── Publish real-time update
   ├── Update trajectory
   └── Log statistics
```

### **Sensor Data Processing**
```
1. Data Reception
   ├── Receive from firmware via HTTP
   ├── Validate data format
   ├── Check data quality
   └── Store in cache

2. Data Processing
   ├── Apply validation rules
   ├── Apply filters (noise reduction)
   ├── Calculate quality metrics
   └── Apply sensor-specific processing

3. Data Storage
   ├── Store in database
   ├── Update sensor status
   ├── Maintain history
   └── Cleanup old data

4. Real-time Broadcasting
   ├── Publish to WebSocket
   ├── Update subscribers
   ├── Handle priority
   └── Monitor performance
```

---

## ⚡ **Performance Optimization**

### **Database Optimization**
- **Connection Pooling:** Async SQLAlchemy with NullPool
- **Indexing:** Strategic indexes on frequently queried columns
- **Query Optimization:** Efficient queries with proper joins
- **Migration Management:** Alembic for schema versioning

### **Memory Management**
- **Data Caching:** In-memory cache for frequently accessed data
- **History Limits:** Configurable history size limits
- **Garbage Collection:** Regular cleanup of old data
- **Memory Monitoring:** Real-time memory usage tracking

### **Real-time Performance**
- **WebSocket Optimization:** Efficient message broadcasting
- **Queue Management:** Non-blocking queues with overflow protection
- **Update Batching:** Batch updates for better performance
- **Priority System:** High-priority updates for critical data

### **API Performance**
- **Response Caching:** Cache frequently requested data
- **Async Processing:** Non-blocking I/O operations
- **Request Validation:** Early validation to prevent processing
- **Error Handling:** Efficient error handling and logging

---

## 🔒 **Security Implementation**

### **Authentication & Authorization**
```python
# JWT Token Creation
def create_access_token(data: dict, expires_delta: Optional[timelta] = None):
    to_encode = data.copy()
    if expires_delta:
        expire = datetime.utcnow() + expires_delta
    else:
        expire = datetime.utcnow() + timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)
    return encoded_jwt

# User Authentication
async def get_current_user(token: str = Depends(security), 
                          db: AsyncSession = Depends(get_db)):
    credentials_exception = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Could not validate credentials",
        headers={"WWW-Authenticate": "Bearer"},
    )
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        username: str = payload.get("sub")
        if username is None:
            raise credentials_exception
    except JWTError:
        raise credentials_exception
    
    result = await db.execute(select(User).filter(User.username == username))
    user = result.scalars().first()
    if user is None:
        raise credentials_exception
    return user
```

### **Input Validation**
```python
# Pydantic Models for Validation
class MapCreateRequest(BaseModel):
    map_name: str = Field(..., min_length=1, max_length=100)
    resolution: float = Field(..., gt=0, le=1000)
    width: int = Field(..., gt=0, le=10000)
    height: int = Field(..., gt=0, le=10000)

class SensorDataRequest(BaseModel):
    sensor_type: str = Field(..., regex="^(rfid|accelerometer|proximity|lidar)$")
    sensor_id: str = Field(..., min_length=1, max_length=50)
    data: Dict[str, Any] = Field(...)
    quality: float = Field(..., ge=0.0, le=1.0)
```

### **Rate Limiting**
```python
# Rate Limiting Middleware
class RateLimiter:
    def __init__(self, requests_per_minute: int = 1000, burst_limit: int = 100):
        self.requests_per_minute = requests_per_minute
        self.burst_limit = burst_limit
        self.requests = {}
    
    async def check_limit(self, request: Request) -> bool:
        client_ip = request.client.host
        now = time.time()
        
        if client_ip not in self.requests:
            self.requests[client_ip] = []
        
        # Clean old requests
        self.requests[client_ip] = [
            req_time for req_time in self.requests[client_ip] 
            if now - req_time < 60
        ]
        
        # Check limits
        if len(self.requests[client_ip]) >= self.requests_per_minute:
            return False
        
        self.requests[client_ip].append(now)
        return True
```

---

## 🧪 **Testing Strategy**

### **Test Types**
1. **Unit Tests:** Individual component testing
2. **Integration Tests:** Component interaction testing
3. **End-to-End Tests:** Complete workflow testing
4. **Performance Tests:** Performance requirement validation
5. **Security Tests:** Security vulnerability testing

### **Test Coverage**
- **Target Coverage:** > 90%
- **Critical Paths:** 100% coverage
- **Error Scenarios:** Comprehensive error testing
- **Edge Cases:** Boundary condition testing

### **Test Data Management**
- **Mock Services:** Mock firmware and external services
- **Test Database:** Separate test database
- **Test Fixtures:** Reusable test data
- **Cleanup:** Automatic test cleanup

---

## 📊 **Monitoring & Logging**

### **Logging Configuration**
```python
# Logging Setup
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('oht50_backend.log'),
        logging.StreamHandler()
    ]
)

# Service-specific Loggers
logger = logging.getLogger(__name__)
```

### **Performance Monitoring**
```python
# Performance Metrics
class PerformanceMonitor:
    def __init__(self):
        self.metrics = {}
        self.alerts = []
    
    async def record_metric(self, metric_name: str, value: float, tags: Dict = None):
        """Record performance metric"""
        timestamp = time.time()
        self.metrics[metric_name] = {
            "value": value,
            "timestamp": timestamp,
            "tags": tags or {}
        }
        
        # Check thresholds
        await self.check_thresholds(metric_name, value)
    
    async def check_thresholds(self, metric_name: str, value: float):
        """Check performance thresholds and generate alerts"""
        thresholds = {
            "api_response_time": 100,  # ms
            "websocket_latency": 50,   # ms
            "database_query_time": 10, # ms
            "cpu_usage": 80,           # %
            "memory_usage": 85         # %
        }
        
        if metric_name in thresholds and value > thresholds[metric_name]:
            alert = {
                "metric": metric_name,
                "value": value,
                "threshold": thresholds[metric_name],
                "timestamp": datetime.utcnow().isoformat()
            }
            self.alerts.append(alert)
            await self.send_alert(alert)
```

### **Health Checks**
```python
# Health Check Endpoint
@app.get("/health")
async def health_check():
    """Health check endpoint"""
    try:
        # Check database connection
        await database.execute("SELECT 1")
        
        # Check firmware connection
        firmware_status = await firmware_service.heartbeat()
        
        # Check system resources
        cpu_percent = psutil.cpu_percent(interval=1)
        memory = psutil.virtual_memory()
        
        return {
            "status": "healthy",
            "timestamp": datetime.utcnow().isoformat(),
            "services": {
                "database": "healthy",
                "firmware": "healthy" if firmware_status else "unhealthy"
            },
            "system": {
                "cpu_usage": cpu_percent,
                "memory_usage": memory.percent
            }
        }
    except Exception as e:
        return {
            "status": "unhealthy",
            "error": str(e),
            "timestamp": datetime.utcnow().isoformat()
        }
```

---

## 🚀 **Deployment**

### **Docker Configuration**
```dockerfile
# Dockerfile
FROM python:3.11-slim

WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    gcc \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application code
COPY . .

# Create non-root user
RUN useradd --create-home --shell /bin/bash app
USER app

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8000/health || exit 1

# Expose port
EXPOSE 8000

# Start application
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]
```

### **Docker Compose**
```yaml
# docker-compose.yml
version: '3.8'

services:
  backend:
    build: .
    ports:
      - "8000:8000"
    environment:
      - ENVIRONMENT=production
      - DATABASE_URL=sqlite:///oht50.db
      - JWT_SECRET=your-secret-key
      - FIRMWARE_URL=http://firmware:8081
    volumes:
      - ./data:/app/data
    restart: unless-stopped
    depends_on:
      - redis
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8000/health"]
      interval: 30s
      timeout: 10s
      retries: 3

  redis:
    image: redis:7-alpine
    ports:
      - "6379:6379"
    volumes:
      - redis_data:/data
    restart: unless-stopped

volumes:
  redis_data:
```

### **Production Deployment**
```bash
# Production deployment script
#!/bin/bash

# Build and deploy
docker-compose -f docker-compose.prod.yml up -d --build

# Run database migrations
docker-compose exec backend alembic upgrade head

# Verify deployment
curl -f http://localhost:8000/health

# Setup monitoring
docker-compose exec backend python -m app.monitoring.setup
```

---

## 📚 **API Reference**

### **OpenAPI Documentation**
- **Swagger UI:** `http://localhost:8000/docs`
- **ReDoc:** `http://localhost:8000/redoc`
- **OpenAPI JSON:** `http://localhost:8000/openapi.json`

### **API Endpoints Summary**
- **Map Management:** 12 endpoints
- **Sensor Data:** 8 endpoints  
- **Localization:** 8 endpoints
- **WebSocket:** Real-time streaming
- **Health Check:** 1 endpoint

### **Response Formats**
- **Success Response:** `{"success": true, "data": {...}}`
- **Error Response:** `{"success": false, "error": "message"}`
- **Pagination:** `{"data": [...], "total_count": 100, "page": 1, "limit": 50}`

---

## 🔧 **Configuration**

### **Environment Variables**
```bash
# Database
DATABASE_URL=sqlite:///oht50.db

# Authentication
JWT_SECRET=your-secret-key
JWT_ALGORITHM=HS256
ACCESS_TOKEN_EXPIRE_MINUTES=30

# Firmware
FIRMWARE_URL=http://localhost:8081
FIRMWARE_TIMEOUT=5

# Performance
MAX_CONNECTIONS=100
REQUEST_TIMEOUT=30
RATE_LIMIT_REQUESTS=1000
RATE_LIMIT_BURST=100

# Logging
LOG_LEVEL=INFO
LOG_FILE=oht50_backend.log

# Security
CORS_ORIGINS=["http://localhost:3000"]
ALLOWED_HOSTS=["localhost", "127.0.0.1"]
```

### **Configuration Files**
- **Database:** `alembic.ini`
- **Logging:** `logging.conf`
- **Security:** `security.yaml`
- **Performance:** `performance.yaml`

---

## 🐛 **Troubleshooting**

### **Common Issues**

#### **Database Connection Issues**
```bash
# Check database file permissions
ls -la oht50.db

# Check database integrity
sqlite3 oht50.db "PRAGMA integrity_check;"

# Reset database
rm oht50.db
alembic upgrade head
```

#### **Firmware Connection Issues**
```bash
# Test firmware connectivity
curl -f http://localhost:8081/api/v1/health

# Check firewall settings
sudo ufw status

# Verify network configuration
netstat -tlnp | grep 8081
```

#### **Performance Issues**
```bash
# Check system resources
htop
free -h
df -h

# Check application logs
tail -f oht50_backend.log

# Monitor API performance
curl -w "@curl-format.txt" -o /dev/null -s http://localhost:8000/api/v1/map/status
```

### **Debug Mode**
```bash
# Enable debug mode
export DEBUG=true
export LOG_LEVEL=DEBUG

# Start with debug logging
uvicorn app.main:app --reload --log-level debug
```

---

## 📞 **Support**

### **Development Team**
- **Backend Lead:** backend-lead@oht50.com
- **DevOps Engineer:** devops@oht50.com
- **Technical Support:** support@oht50.com

### **Documentation**
- **API Docs:** `/docs`
- **Technical Docs:** `/docs/technical`
- **User Guide:** `/docs/user`
- **GitHub:** https://github.com/oht50/backend

### **Issue Reporting**
- **GitHub Issues:** https://github.com/oht50/backend/issues
- **Bug Reports:** bugs@oht50.com
- **Feature Requests:** features@oht50.com

---

**Last Updated:** 2025-01-28  
**Version:** 1.0.0  
**Maintainer:** OHT-50 Backend Team
