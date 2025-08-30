# REQ_BE_06 - LiDAR Integration Specifications

**Document ID:** REQ_BE_06  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Priority:** 🔴 Critical  

---

## 📋 OVERVIEW

This document specifies the LiDAR sensor integration requirements for the OHT-50 Backend System. The LiDAR sensor serves three critical functions: **Safety Monitoring**, **Map Scanning**, and **Path Planning**. This integration is essential for autonomous navigation and collision avoidance.

---

## 🎯 OBJECTIVES

### Primary Objectives:
- **Safety Monitoring:** Real-time obstacle detection and collision avoidance
- **Map Scanning:** Environment mapping and localization
- **Path Planning:** Dynamic route calculation and optimization

### Secondary Objectives:
- **Data Processing:** Efficient point cloud processing and filtering
- **Integration:** Seamless integration with existing firmware modules
- **Performance:** Low-latency processing for real-time control

---

## 🔧 TECHNICAL SPECIFICATIONS

### LiDAR Hardware Requirements:
```yaml
LiDAR Sensor Specifications:
  Model: "RPLIDAR A2M8" or equivalent
  Type: "2D LiDAR Scanner"
  Range: "0.15m - 12m"
  Angular Resolution: "0.9°"
  Scan Frequency: "8Hz"
  Interface: "USB 2.0 / UART"
  Power: "5V DC, 500mA"
  Operating Temperature: "-10°C to +50°C"
  Protection: "IP54"
```

### Communication Interface:
```yaml
LiDAR Communication:
  Protocol: "UART / USB"
  Baud Rate: "115200 bps"
  Data Format: "Binary / ASCII"
  Frame Rate: "8 FPS"
  Point Cloud Size: "360 points per scan"
  Data Structure: "Distance + Angle + Quality"
```

---

## 🏗️ ARCHITECTURE INTEGRATION

### System Architecture:
```
┌─────────────────────────────────────┐
│           Frontend Layer            │
│        (Map Visualization)          │
└─────────────────┬───────────────────┘
                  │ WebSocket
┌─────────────────▼───────────────────┐
│           Backend Layer             │
│      (LiDAR Processing Service)     │
└─────────────────┬───────────────────┘
                  │ UART/USB
┌─────────────────▼───────────────────┐
│         LiDAR Hardware              │
│      (RPLIDAR A2M8 Sensor)         │
└─────────────────────────────────────┘
```

### Backend Service Architecture:
```python
class LiDARIntegrationService:
    def __init__(self):
        self.lidar_port = "/dev/ttyUSB0"
        self.scan_frequency = 8  # Hz
        self.safety_zones = {
            "critical": 0.5,    # meters
            "warning": 1.0,     # meters
            "safe": 2.0         # meters
        }
        
    async def initialize_lidar(self):
        """Initialize LiDAR sensor connection"""
        
    async def start_scanning(self):
        """Start continuous LiDAR scanning"""
        
    async def process_point_cloud(self, raw_data):
        """Process raw LiDAR data into structured format"""
        
    async def detect_obstacles(self, point_cloud):
        """Detect obstacles in safety zones"""
        
    async def update_map(self, point_cloud):
        """Update environment map with new scan data"""
        
    async def calculate_path(self, start, goal, obstacles):
        """Calculate optimal path avoiding obstacles"""
```

---

## 🚨 SAFETY MONITORING

### Safety Zone Definitions:
```yaml
Safety Zones:
  Critical Zone:
    Distance: "0.0m - 0.5m"
    Action: "Emergency Stop"
    Response Time: "< 100ms"
    Priority: "Highest"
    
  Warning Zone:
    Distance: "0.5m - 1.0m"
    Action: "Reduce Speed"
    Response Time: "< 200ms"
    Priority: "High"
    
  Safe Zone:
    Distance: "1.0m - 2.0m"
    Action: "Monitor"
    Response Time: "< 500ms"
    Priority: "Normal"
```

### Obstacle Detection Algorithm:
```python
class ObstacleDetectionService:
    def __init__(self):
        self.safety_zones = {
            "critical": {"min": 0.0, "max": 0.5, "action": "emergency_stop"},
            "warning": {"min": 0.5, "max": 1.0, "action": "reduce_speed"},
            "safe": {"min": 1.0, "max": 2.0, "action": "monitor"}
        }
        
    async def detect_obstacles(self, point_cloud):
        """Detect obstacles in each safety zone"""
        obstacles = {
            "critical": [],
            "warning": [],
            "safe": []
        }
        
        for point in point_cloud:
            distance = point["distance"]
            angle = point["angle"]
            
            # Classify obstacle by zone
            for zone, config in self.safety_zones.items():
                if config["min"] <= distance <= config["max"]:
                    obstacles[zone].append({
                        "distance": distance,
                        "angle": angle,
                        "x": distance * math.cos(math.radians(angle)),
                        "y": distance * math.sin(math.radians(angle))
                    })
        
        return obstacles
```

---

## 🗺️ MAP SCANNING

### Map Data Structure:
```yaml
Map Data Format:
  Grid Resolution: "0.1m x 0.1m"
  Map Size: "50m x 50m"
  Data Type: "Occupancy Grid"
  Values:
    -1: "Unknown"
     0: "Free Space"
     1: "Occupied"
    50: "Robot Position"
```

### Map Update Service:
```python
class MapScanningService:
    def __init__(self):
        self.map_resolution = 0.1  # meters
        self.map_size = (500, 500)  # 50m x 50m grid
        self.occupancy_grid = np.full(self.map_size, -1)  # Unknown
        
    async def update_map(self, point_cloud, robot_position):
        """Update occupancy grid with new LiDAR scan"""
        for point in point_cloud:
            # Convert polar coordinates to grid coordinates
            grid_x, grid_y = self.polar_to_grid(
                point["distance"], 
                point["angle"], 
                robot_position
            )
            
            # Update occupancy grid
            if self.is_valid_grid_position(grid_x, grid_y):
                self.occupancy_grid[grid_y, grid_x] = 1  # Occupied
                
        # Mark robot position
        robot_grid_x, robot_grid_y = self.world_to_grid(robot_position)
        if self.is_valid_grid_position(robot_grid_x, robot_grid_y):
            self.occupancy_grid[robot_grid_y, robot_grid_x] = 50
```

---

## 🛤️ PATH PLANNING

### Path Planning Algorithm:
```python
class PathPlanningService:
    def __init__(self):
        self.algorithm = "A*"  # Default algorithm
        self.heuristic = "euclidean"
        self.path_smoothing = True
        
    async def calculate_path(self, start, goal, obstacles, map_data):
        """Calculate optimal path from start to goal"""
        # Validate inputs
        if not self.is_valid_position(start) or not self.is_valid_position(goal):
            raise ValueError("Invalid start or goal position")
            
        # Convert to grid coordinates
        start_grid = self.world_to_grid(start)
        goal_grid = self.world_to_grid(goal)
        
        # Calculate path using A* algorithm
        path_grid = await self.a_star_pathfinding(
            start_grid, 
            goal_grid, 
            map_data["occupancy_grid"]
        )
        
        # Convert back to world coordinates
        path_world = [self.grid_to_world(point) for point in path_grid]
        
        # Smooth path if enabled
        if self.path_smoothing:
            path_world = await self.smooth_path(path_world, obstacles)
            
        return {
            "path": path_world,
            "distance": self.calculate_path_distance(path_world),
            "estimated_time": self.estimate_travel_time(path_world),
            "safety_score": self.calculate_safety_score(path_world, obstacles)
        }
```

---

## 🔗 API INTEGRATION

### LiDAR API Endpoints:
```yaml
LiDAR API Endpoints:
  GET /lidar/status:
    Description: "Get LiDAR sensor status"
    Response:
      status: "connected|disconnected|error"
      scan_frequency: "8 Hz"
      last_scan: "2025-01-28T10:30:00Z"
      
  GET /lidar/scan:
    Description: "Get latest LiDAR scan data"
    Response:
      timestamp: "2025-01-28T10:30:00Z"
      point_cloud: "Array of distance/angle points"
      quality: "Average scan quality"
      
  GET /lidar/obstacles:
    Description: "Get detected obstacles"
    Response:
      obstacles: "Array of obstacle objects"
      safety_zones: "Zone classification"
      warnings: "Safety warnings"
      
  GET /lidar/map:
    Description: "Get current environment map"
    Response:
      occupancy_grid: "2D grid array"
      resolution: "0.1 meters"
      robot_position: "Current robot position"
      
  POST /lidar/path:
    Description: "Calculate path between two points"
    Request:
      start: "Start coordinates"
      goal: "Goal coordinates"
    Response:
      path: "Array of waypoints"
      distance: "Total path distance"
      estimated_time: "Estimated travel time"
```

---

## 📈 PERFORMANCE REQUIREMENTS

### Performance Metrics:
```yaml
Performance Requirements:
  Processing Latency:
    Point Cloud Processing: "< 50ms"
    Obstacle Detection: "< 100ms"
    Path Planning: "< 200ms"
    Safety Response: "< 100ms"
    
  Data Throughput:
    Scan Frequency: "8 Hz"
    Point Cloud Size: "360 points per scan"
    Data Rate: "2.88 kpoints/second"
    
  Accuracy Requirements:
    Distance Accuracy: "±2cm"
    Angular Accuracy: "±1°"
    Obstacle Detection: "> 95% accuracy"
    False Positive Rate: "< 5%"
    
  Reliability Requirements:
    Uptime: "> 99.5%"
    Mean Time Between Failures: "> 1000 hours"
    Recovery Time: "< 30 seconds"
```

---

## 🔒 SAFETY & COMPLIANCE

### Safety Requirements:
```yaml
Safety Requirements:
  Emergency Stop:
    Trigger Distance: "≤ 0.5m"
    Response Time: "< 100ms"
    Reliability: "> 99.9%"
    
  Warning System:
    Trigger Distance: "≤ 1.0m"
    Response Time: "< 200ms"
    Alert Type: "Visual and Audio"
    
  System Monitoring:
    Sensor Health: "Continuous monitoring"
    Data Quality: "Quality assessment"
    Fault Detection: "Automatic fault detection"
    
  Compliance Standards:
    IEC 61508: "Functional safety"
    ISO 13482: "Personal care robots"
    IEC 62061: "Safety of machinery"
```

---

## 📝 IMPLEMENTATION ROADMAP

### Phase 1: Basic Integration (Week 1-2)
- [ ] LiDAR hardware setup and connection
- [ ] Basic data acquisition and processing
- [ ] Simple obstacle detection
- [ ] Safety zone monitoring

### Phase 2: Advanced Features (Week 3-4)
- [ ] Map scanning and updating
- [ ] Path planning algorithms
- [ ] Real-time data processing
- [ ] Performance optimization

### Phase 3: Integration & Testing (Week 5-6)
- [ ] Full system integration
- [ ] Safety system validation
- [ ] Performance testing
- [ ] User acceptance testing

### Phase 4: Deployment & Monitoring (Week 7-8)
- [ ] Production deployment
- [ ] Performance monitoring
- [ ] Safety validation
- [ ] Documentation completion

---

**Last Updated:** 2025-01-28  
**Next Review:** 2025-02-28  
**Document Owner:** Backend Team
