# 🗺️ MAP & LOCALIZATION TECHNICAL SPECIFICATION

**Phiên bản:** 1.0  
**Ngày:** 2025-01-28  
**Mục tiêu:** Chi tiết kỹ thuật cho Map & Localization System  
**Phương án:** Hybrid (Dock Location + LiDAR HAL)  

---

## 📋 **TECHNICAL OVERVIEW**

### **🎯 System Architecture:**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Frontend UI   │◄──►│   Backend API   │◄──►│   Firmware      │
│                 │    │                 │    │                 │
│ - Map Display   │    │ - Map Service   │    │ - RFID Reader   │
│ - Controls      │    │ - Localization  │    │ - Accelerometer │
│ - Status        │    │ - Storage       │    │ - Proximity     │
└─────────────────┘    └─────────────────┘    │ - LiDAR HAL     │
         │                       │             └─────────────────┘
         │                       ▼                       │
         │              ┌─────────────────┐              │
         │              │   Database      │              │
         │              │                 │              │
         └──────────────►│ - Maps          │◄─────────────┘
                        │ - Positions     │
                        │ - Sessions      │
                        └─────────────────┘
```

---

## 🔧 **CORE COMPONENTS**

### **1. MapService Class:**
```python
class MapService:
    def __init__(self):
        self.current_map: Optional[MapData] = None
        self.robot_pose: RobotPose = RobotPose()
        self.map_storage: Dict[str, MapData] = {}
        self.localization_engine: HybridLocalizationEngine()
        
    async def start_mapping(self, map_id: str) -> MappingSession
    async def stop_mapping(self, session_id: str) -> bool
    async def get_robot_position(self) -> RobotPose
    async def update_map(self, lidar_data: LiDARScan) -> bool
    async def load_map(self, map_id: str) -> Optional[MapData]
    async def save_map(self, map_data: MapData) -> bool
```

### **2. HybridLocalizationEngine:**
```python
class HybridLocalizationEngine:
    def __init__(self):
        self.rfid_weight = 0.5  # RFID confidence weight
        self.sensor_fusion_weight = 0.3  # Sensor fusion weight
        self.lidar_weight = 0.2  # LiDAR confidence weight
        
    async def localize_robot(self, rfid_data: RFIDData, sensor_data: SensorData, lidar_data: LiDARScan) -> RobotPose
    def _estimate_from_rfid(self, rfid_data: RFIDData) -> RobotPose
    def _estimate_from_sensor_fusion(self, sensor_data: SensorData) -> RobotPose
    def _estimate_from_lidar(self, lidar_data: LiDARScan) -> RobotPose
    def _fuse_estimates(self, rfid_pose: RobotPose, sensor_pose: RobotPose, lidar_pose: RobotPose) -> RobotPose
```

### **3. OccupancyGrid:**
```python
class OccupancyGrid:
    def __init__(self, width: int, height: int, resolution: float):
        self.width = width
        self.height = height
        self.resolution = resolution  # mm per cell
        self.grid = np.full((height, width), -1, dtype=np.int8)
        # -1: unknown, 0: free, 1: occupied, 50: robot
        
    def update_from_lidar(self, lidar_scan: LiDARScan, robot_pose: RobotPose)
    def get_occupancy(self, x: float, y: float) -> int
    def mark_robot_position(self, x: float, y: float)
```

---

## 📊 **DATA STRUCTURES**

### **1. RobotPose:**
```python
@dataclass
class RobotPose:
    x: float = 0.0  # mm
    y: float = 0.0  # mm
    theta: float = 0.0  # radians
    confidence: float = 0.0  # 0.0 - 1.0
    source: str = "unknown"  # "dock", "lidar", "hybrid", "fallback"
    timestamp: datetime = field(default_factory=datetime.now)
```

### **2. MapData:**
```python
@dataclass
class MapData:
    map_id: str
    name: str
    resolution: float  # mm per cell
    width: int  # cells
    height: int  # cells
    occupancy_grid: OccupancyGrid
    robot_trajectory: List[RobotPose]
    dock_positions: List[DockPosition]
    metadata: MapMetadata
```

### **3. LiDARScan:**
```python
@dataclass
class LiDARScan:
    points: List[LiDARPoint]
    timestamp: datetime
    quality: float  # 0.0 - 1.0
    
@dataclass
class LiDARPoint:
    distance: float  # mm
    angle: float  # degrees
    quality: float  # 0.0 - 1.0
```

### **4. RFIDData:**
```python
@dataclass
class RFIDData:
    rfid_id: str
    position_x: float  # mm (from RFID tag)
    position_y: float  # mm (from RFID tag)
    confidence: float  # 0.0 - 1.0
    timestamp: datetime

### **5. SensorData:**
```python
@dataclass
class SensorData:
    accelerometer_x: float  # m/s²
    accelerometer_y: float  # m/s²
    accelerometer_z: float  # m/s²
    proximity_sensor_1: bool  # True if near dock
    proximity_sensor_2: bool  # True if near dock
    confidence: float  # 0.0 - 1.0
    timestamp: datetime
```

---

## 🔄 **ALGORITHMS**

### **1. Hybrid Localization Algorithm:**
```python
async def hybrid_localization(rfid_data: RFIDData, sensor_data: SensorData, lidar_data: LiDARScan) -> RobotPose:
    # Step 1: Get RFID-based estimate
    rfid_pose = estimate_from_rfid(rfid_data)
    
    # Step 2: Get sensor fusion estimate
    sensor_pose = estimate_from_sensor_fusion(sensor_data)
    
    # Step 3: Get LiDAR-based estimate
    lidar_pose = estimate_from_lidar(lidar_data, current_map)
    
    # Step 4: Fuse estimates based on confidence
    if rfid_pose.confidence > 0.8:
        # High confidence RFID - use as primary
        final_pose = rfid_pose
        final_pose.source = "rfid"
    elif sensor_pose.confidence > 0.6:
        # Good sensor fusion - use sensor fusion
        final_pose = sensor_pose
        final_pose.source = "sensor_fusion"
    elif lidar_pose.confidence > 0.5:
        # Good LiDAR - use LiDAR
        final_pose = lidar_pose
        final_pose.source = "lidar"
    else:
        # Low confidence - fuse all
        final_pose = fuse_estimates(rfid_pose, sensor_pose, lidar_pose)
        final_pose.source = "hybrid"
    
    return final_pose
```

### **2. Coordinate Transformation:**
```python
def polar_to_cartesian(distance: float, angle: float, 
                      robot_x: float, robot_y: float, robot_theta: float) -> tuple:
    """Convert LiDAR polar coordinates to map Cartesian coordinates"""
    angle_rad = math.radians(angle)
    
    # Calculate relative position from robot
    relative_x = distance * math.cos(angle_rad)
    relative_y = distance * math.sin(angle_rad)
    
    # Transform to map coordinates using robot's position and orientation
    map_x = robot_x + relative_x * math.cos(robot_theta) - relative_y * math.sin(robot_theta)
    map_y = robot_y + relative_x * math.sin(robot_theta) + relative_y * math.cos(robot_theta)
    
    return (map_x, map_y)
```

### **3. Occupancy Grid Update:**
```python
def update_occupancy_grid(grid: OccupancyGrid, lidar_scan: LiDARScan, robot_pose: RobotPose):
    """Update occupancy grid with new LiDAR scan"""
    for point in lidar_scan.points:
        if point.quality < 0.5:  # Skip low quality points
            continue
            
        # Convert to map coordinates
        map_x, map_y = polar_to_cartesian(
            point.distance, point.angle, 
            robot_pose.x, robot_pose.y, robot_pose.theta
        )
        
        # Convert to grid coordinates
        grid_x = int(map_x / grid.resolution)
        grid_y = int(map_y / grid.resolution)
        
        # Update grid cell
        if 0 <= grid_x < grid.width and 0 <= grid_y < grid.height:
            grid.grid[grid_y, grid_x] = 1  # Occupied
    
    # Mark robot position
    robot_grid_x = int(robot_pose.x / grid.resolution)
    robot_grid_y = int(robot_pose.y / grid.resolution)
    if 0 <= robot_grid_x < grid.width and 0 <= robot_grid_y < grid.height:
        grid.grid[robot_grid_y, robot_grid_x] = 50  # Robot position
```

---

## 📡 **API SPECIFICATIONS**

### **1. Map Management APIs:**

#### **POST /api/v1/map/start-mapping**
```json
{
  "map_id": "string",
  "name": "string",
  "resolution": 50.0,
  "width": 1000,
  "height": 1000
}
```

**Response:**
```json
{
  "success": true,
  "session_id": "uuid",
  "message": "Mapping started successfully",
  "map_id": "string"
}
```

#### **POST /api/v1/map/stop-mapping**
```json
{
  "session_id": "uuid"
}
```

**Response:**
```json
{
  "success": true,
  "message": "Mapping stopped successfully",
  "total_scans": 150,
  "mapping_quality": 0.85
}
```

#### **GET /api/v1/map/status**
**Response:**
```json
{
  "success": true,
  "is_mapping": true,
  "session_id": "uuid",
  "current_map_id": "string",
  "total_scans": 75,
  "mapping_quality": 0.82,
  "robot_position": {
    "x": 1500.0,
    "y": 2000.0,
    "theta": 1.57,
    "confidence": 0.9,
    "source": "dock"
  }
}
```

### **2. Localization APIs:**

#### **GET /api/v1/map/robot-position**
**Response:**
```json
{
  "success": true,
  "position": {
    "x": 1500.0,
    "y": 2000.0,
    "theta": 1.57,
    "confidence": 0.9,
    "source": "hybrid",
    "timestamp": "2025-01-28T10:30:00Z"
  }
}
```

#### **GET /api/v1/map/occupancy-grid**
**Response:**
```json
{
  "success": true,
  "grid": {
    "data": [[-1, 0, 1, ...], [0, 1, 0, ...], ...],
    "resolution": 50.0,
    "width": 1000,
    "height": 1000
  }
}
```

### **3. Map Storage APIs:**

#### **GET /api/v1/map/list**
**Response:**
```json
{
  "success": true,
  "maps": [
    {
      "map_id": "map_001",
      "name": "Factory Floor A",
      "created_at": "2025-01-28T10:00:00Z",
      "updated_at": "2025-01-28T10:30:00Z",
      "size": "1000x1000",
      "resolution": 50.0
    }
  ]
}
```

#### **POST /api/v1/map/load/{map_id}**
**Response:**
```json
{
  "success": true,
  "message": "Map loaded successfully",
  "map_data": {
    "map_id": "map_001",
    "name": "Factory Floor A",
    "occupancy_grid": {...},
    "robot_trajectory": [...],
    "metadata": {...}
  }
}
```

---

## 🗄️ **DATABASE SCHEMA**

### **1. Maps Table:**
```sql
CREATE TABLE maps (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    map_id VARCHAR(50) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    resolution FLOAT NOT NULL DEFAULT 50.0,
    width INTEGER NOT NULL DEFAULT 1000,
    height INTEGER NOT NULL DEFAULT 1000,
    occupancy_grid JSON NOT NULL,
    robot_trajectory JSON DEFAULT '[]',
    dock_positions JSON DEFAULT '[]',
    metadata JSON DEFAULT '{}',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_maps_map_id ON maps(map_id);
CREATE INDEX idx_maps_created_at ON maps(created_at);
```

### **2. Map Sessions Table:**
```sql
CREATE TABLE map_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id VARCHAR(50) UNIQUE NOT NULL,
    map_id VARCHAR(50) NOT NULL,
    start_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    end_time TIMESTAMP NULL,
    is_active BOOLEAN DEFAULT TRUE,
    total_scans INTEGER DEFAULT 0,
    mapping_quality FLOAT DEFAULT 0.0,
    FOREIGN KEY (map_id) REFERENCES maps(map_id)
);

CREATE INDEX idx_sessions_session_id ON map_sessions(session_id);
CREATE INDEX idx_sessions_map_id ON map_sessions(map_id);
CREATE INDEX idx_sessions_active ON map_sessions(is_active);
```

### **3. Robot Positions Table:**
```sql
CREATE TABLE robot_positions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    map_id VARCHAR(50) NOT NULL,
    session_id VARCHAR(50),
    x FLOAT NOT NULL,
    y FLOAT NOT NULL,
    theta FLOAT NOT NULL,
    confidence FLOAT NOT NULL,
    source VARCHAR(20) NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (map_id) REFERENCES maps(map_id),
    FOREIGN KEY (session_id) REFERENCES map_sessions(session_id)
);

CREATE INDEX idx_positions_map_id ON robot_positions(map_id);
CREATE INDEX idx_positions_timestamp ON robot_positions(timestamp);
CREATE INDEX idx_positions_source ON robot_positions(source);
```

---

## ⚡ **PERFORMANCE SPECIFICATIONS**

### **1. Response Time Requirements:**
- Map status: < 50ms
- Robot position: < 100ms
- Map data (1000x1000): < 200ms
- Start/stop mapping: < 500ms
- Load map: < 1000ms

### **2. Throughput Requirements:**
- LiDAR processing: 10Hz (100ms interval)
- Map updates: 5Hz (200ms interval)
- Position updates: 20Hz (50ms interval)
- Database writes: 100 writes/second

### **3. Memory Requirements:**
- Occupancy grid (1000x1000): ~1MB
- Robot trajectory (1000 points): ~100KB
- Map metadata: ~10KB
- **Total per map: ~1.1MB**

### **4. CPU Requirements:**
- LiDAR processing: < 10% CPU
- Map updates: < 5% CPU
- Localization: < 15% CPU
- **Total: < 30% CPU**

---

## 🔒 **SECURITY & VALIDATION**

### **1. Input Validation:**
```python
def validate_map_parameters(map_id: str, resolution: float, width: int, height: int):
    if not map_id or len(map_id) > 50:
        raise ValueError("Invalid map_id")
    if resolution <= 0 or resolution > 1000:
        raise ValueError("Invalid resolution")
    if width <= 0 or width > 10000:
        raise ValueError("Invalid width")
    if height <= 0 or height > 10000:
        raise ValueError("Invalid height")
```

### **2. Authentication:**
- All endpoints require JWT token
- Map write operations require "map:write" permission
- Map read operations require "map:read" permission

### **3. Data Integrity:**
- Occupancy grid checksums
- Robot position validation
- Map metadata validation
- Database transaction rollback on errors

---

## 🧪 **TESTING STRATEGY**

### **1. Unit Tests:**
- MapService methods
- HybridLocalizationEngine
- OccupancyGrid operations
- Coordinate transformations

### **2. Integration Tests:**
- API endpoint functionality
- Database operations
- Firmware integration
- Real-time data flow

### **3. Performance Tests:**
- Response time benchmarks
- Memory usage monitoring
- CPU usage monitoring
- Throughput testing

### **4. E2E Tests:**
- Complete mapping workflow
- Localization accuracy
- Map storage and retrieval
- Error handling scenarios

---

## 📈 **MONITORING & METRICS**

### **1. Key Metrics:**
- Mapping success rate
- Localization accuracy
- Response times
- Memory usage
- CPU usage
- Error rates

### **2. Alerts:**
- Mapping failure
- Localization accuracy < 80%
- Response time > 200ms
- Memory usage > 80%
- CPU usage > 70%

### **3. Logging:**
- Map operations
- Localization events
- Performance metrics
- Error conditions
- User actions

---

## 🚀 **DEPLOYMENT CONSIDERATIONS**

### **1. Environment Variables:**
```bash
MAP_RESOLUTION=50.0
MAP_WIDTH=1000
MAP_HEIGHT=1000
LIDAR_PROCESSING_RATE=10
MAP_UPDATE_RATE=5
POSITION_UPDATE_RATE=20
```

### **2. Database Migration:**
- Create tables on startup
- Migrate existing data
- Backup before updates
- Rollback capability

### **3. Health Checks:**
- Database connectivity
- Firmware connectivity
- Memory usage
- CPU usage
- Service availability

---

**Changelog v1.0:**
- ✅ Created comprehensive technical specification
- ✅ Defined core components và data structures
- ✅ Detailed algorithms và coordinate transformations
- ✅ Specified API endpoints với request/response formats
- ✅ Designed database schema với indexes
- ✅ Set performance requirements và monitoring
- ✅ Outlined testing strategy và deployment considerations
