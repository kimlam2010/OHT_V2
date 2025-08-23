# LiDAR Integration Guide - OHT-50 (v2.0)

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Updated để tích hợp với Dock & Location module qua USB

---

## 🎯 **MỤC TIÊU**

Hướng dẫn tích hợp LiDAR vào Dock & Location module qua USB để tạo map và kết hợp xác định location cho robot OHT-50.

---

## 🧭 **LIDAR TRONG DOCK & LOCATION MODULE**

### **Tích hợp LiDAR:**
- **Mapping:** Tạo bản đồ môi trường làm việc
- **Localization:** Kết hợp với IMU và magnetic sensors để xác định vị trí
- **Obstacle Detection:** Phát hiện chướng ngại vật trong quá trình di chuyển
- **Path Planning:** Hỗ trợ lập kế hoạch đường đi
- **Safety:** Tích hợp vào hệ thống an toàn

### **Kiến trúc tích hợp:**
```
Dock & Location Module (ID: 0x05)
├── IMU (MPU6050)
├── Magnetic Sensors (2x Hall effect)
├── RFID Reader
├── Dock Alignment Sensors
├── GPS/GNSS (Optional)
└── LiDAR (RPLIDAR A1M8) ← USB Connection
```

---

## 🔧 **HARDWARE SPECIFICATIONS**

### **LiDAR Specifications:**
```
Model:               RPLIDAR A1M8
Type:                2D Laser Scanner
Range:               0.15m - 12m
Angular Resolution:  0.9° (400 samples/revolution)
Scan Frequency:      5.5Hz
Rotation Speed:      330 RPM
Accuracy:            ±2cm (0.15m-1m), ±3cm (1m-6m), ±5cm (6m-12m)
Operating Temperature: -10°C to +50°C
Power Supply:        5V DC via USB
Interface:           USB 2.0
Protection:          IP40
```

### **Mounting Requirements:**
```
Height:              300-500mm từ mặt đất
Orientation:         Horizontal scan plane
Clearance:           ≥100mm clearance xung quanh
Stability:           Vibration isolation recommended
Protection:          Dust và moisture protection
```

---

## 📡 **COMMUNICATION INTEGRATION**

### **USB Integration:**
```
Module:              Dock & Location Module (ID: 0x05)
Interface:           USB 2.0
Connection:          Direct USB connection
Data Format:         LiDAR scan data + processed information
Update Rate:         5Hz (scan frequency)
Driver:              RPLIDAR SDK
```

### **Register Map Extension:**
```
// LiDAR Data (0x0070-0x007F)
0x0070: LiDAR Status (0=OK, 1=Error, 2=Calibrating)
0x0071: Scan Frequency (Hz × 10)
0x0072: Rotation Speed (RPM)
0x0073: Points per Scan (16-bit)
0x0074: Scan Quality (%)
0x0075: Detection Range (mm)
0x0076: Obstacle Count
0x0077: Obstacle Distance (mm)
0x0078: Obstacle Angle (deg × 100)
0x0079: Map Status (0=Not Available, 1=Available, 2=Updating)
0x007A: Map Version (16-bit)
0x007B: Localization Status (0=Lost, 1=Good, 2=Excellent)
0x007C: Localization Confidence (%)
0x007D: LiDAR Temperature (°C)
0x007E: LiDAR Health (%)
0x007F: LiDAR Fault Code

// Mapping Data (0x0080-0x008F)
0x0080: Map Width (cells)
0x0081: Map Height (cells)
0x0082: Map Resolution (mm/cell)
0x0083: Current Position X (cells)
0x0084: Current Position Y (cells)
0x0085: Current Orientation (deg × 100)
0x0086: Target Position X (cells)
0x0087: Target Position Y (cells)
0x0088: Path Clear (0=Blocked, 1=Clear)
0x0089: Safety Zone Status (16-bit bitfield)
0x008A: Mapping Progress (%)
0x008B: Map Update Rate (Hz)
0x008C: Localization Update Rate (Hz)
0x008D: Obstacle Detection Rate (Hz)
0x008E: Path Planning Status (0=Idle, 1=Planning, 2=Following)
0x008F: Path Planning Error Code
```

---

## 🗺️ **MAPPING FUNCTIONALITY**

### **SLAM (Simultaneous Localization and Mapping):**
```c
// SLAM algorithm integration
typedef struct {
    float map_resolution;        // Map resolution (m/cell)
    int map_width;              // Map width (cells)
    int map_height;             // Map height (cells)
    uint8_t *occupancy_grid;    // Occupancy grid map
    float current_x;            // Current X position (m)
    float current_y;            // Current Y position (m)
    float current_theta;        // Current orientation (rad)
    float confidence;           // Localization confidence
} slam_data_t;
```

### **Map Generation Process:**
```
1. Initialization
   - Start LiDAR scanning via USB
   - Initialize occupancy grid
   - Set initial position (0,0,0)

2. Mapping Loop
   - Collect LiDAR scan data via USB
   - Update occupancy grid
   - Estimate robot position
   - Detect loop closures

3. Map Optimization
   - Optimize map consistency
   - Update robot trajectory
   - Save final map
```

### **Map Storage:**
```
Format:             Binary occupancy grid
Resolution:         50mm/cell (configurable)
Size:              Up to 1000x1000 cells
Storage:           Flash memory hoặc SD card
Backup:            Cloud storage (optional)
Version Control:   Map versioning system
```

---

## 🧭 **LOCALIZATION INTEGRATION**

### **Multi-Sensor Fusion:**
```c
// Sensor fusion cho localization
typedef struct {
    // LiDAR data (via USB)
    float lidar_x, lidar_y, lidar_theta;
    float lidar_confidence;
    
    // IMU data
    float imu_x, imu_y, imu_theta;
    float imu_velocity_x, imu_velocity_y;
    
    // Magnetic data
    float magnetic_x, magnetic_y;
    float magnetic_heading;
    
    // Fused result
    float fused_x, fused_y, fused_theta;
    float fused_confidence;
} localization_data_t;
```

### **Localization Algorithm:**
```
1. LiDAR Localization (USB)
   - Scan matching với map
   - ICP (Iterative Closest Point)
   - AMCL (Adaptive Monte Carlo Localization)

2. Multi-Sensor Fusion
   - Kalman filter
   - Particle filter
   - Weighted average

3. Confidence Estimation
   - LiDAR scan quality
   - Map consistency
   - Sensor agreement
```

---

## 🚧 **OBSTACLE DETECTION**

### **Real-time Obstacle Detection:**
```c
// Obstacle detection parameters
typedef struct {
    float detection_range;      // Detection range (m)
    float safety_distance;      // Safety distance (m)
    float warning_distance;     // Warning distance (m)
    int max_obstacles;          // Maximum obstacles to track
    float update_rate;          // Update rate (Hz)
} obstacle_detection_config_t;

// Obstacle data
typedef struct {
    float distance;             // Distance to obstacle (m)
    float angle;                // Angle to obstacle (deg)
    float velocity;             // Obstacle velocity (m/s)
    uint8_t type;               // Obstacle type
    uint8_t priority;           // Priority level
} obstacle_data_t;
```

### **Safety Zones:**
```
Zone 1 (Emergency): 0-0.5m - Immediate stop
Zone 2 (Warning):   0.5-1.0m - Reduce speed
Zone 3 (Safe):      1.0-2.0m - Normal operation
Zone 4 (Clear):     >2.0m - Full speed
```

---

## 🛤️ **PATH PLANNING**

### **Path Planning Integration:**
```c
// Path planning parameters
typedef struct {
    float start_x, start_y;     // Start position
    float goal_x, goal_y;       // Goal position
    float max_speed;            // Maximum speed (m/s)
    float safety_margin;        // Safety margin (m)
    int algorithm;              // Planning algorithm
} path_planning_config_t;

// Path data
typedef struct {
    int waypoint_count;         // Number of waypoints
    float *waypoints_x;         // X coordinates
    float *waypoints_y;         // Y coordinates
    float *waypoints_speed;     // Speed at waypoints
    float total_distance;       // Total path distance
    float estimated_time;       // Estimated travel time
} path_data_t;
```

### **Planning Algorithms:**
```
1. A* Algorithm
   - Grid-based path planning
   - Optimal path finding
   - Real-time replanning

2. RRT (Rapidly-exploring Random Tree)
   - Sampling-based planning
   - Dynamic obstacle avoidance
   - Smooth path generation

3. DWA (Dynamic Window Approach)
   - Local path planning
   - Real-time obstacle avoidance
   - Velocity control
```

---

## 🔒 **SAFETY INTEGRATION**

### **Safety Features:**
```c
// Safety integration với LiDAR (USB)
- Emergency Stop: Immediate stop khi obstacle detected trong emergency zone
- Speed Control: Reduce speed based on obstacle distance
- Path Replanning: Automatic path replanning khi obstacle detected
- Zone Monitoring: Continuous monitoring of safety zones
- Collision Prevention: Predictive collision avoidance
```

### **Safety Response Times:**
```
Emergency Stop: < 10ms
Speed Reduction: < 50ms
Path Replanning: < 200ms
Zone Update: < 100ms
```

---

## 🔧 **IMPLEMENTATION REQUIREMENTS**

### **Hardware Requirements:**
```
- RPLIDAR A1M8 sensor
- USB 2.0 cable (Type-A to Type-B)
- Power supply via USB (5V, 1.5A)
- Mounting bracket với vibration isolation
- Protection cover (dust/moisture)
- Cable management
```

### **Software Requirements:**
```
- LiDAR driver (RPLIDAR SDK)
- USB device driver
- SLAM algorithm implementation
- Localization algorithm
- Path planning algorithm
- Obstacle detection algorithm
- Multi-sensor fusion
- Map management system
```

### **Integration Requirements:**
```
- USB communication protocol
- Modbus RTU register mapping (for Dock & Location module)
- Real-time data processing
- Map storage và retrieval
- Configuration management
- Error handling và recovery
```

---

## 📋 **TESTING PROCEDURES**

### **Functional Testing:**
```
1. LiDAR Hardware Test
   - USB connection test
   - Power supply test
   - Communication test
   - Scan data validation
   - Temperature monitoring

2. Mapping Test
   - Map generation test
   - Map accuracy test
   - Map storage test
   - Map loading test

3. Localization Test
   - Position accuracy test
   - Localization stability test
   - Multi-sensor fusion test
   - Confidence estimation test

4. Obstacle Detection Test
   - Detection range test
   - Detection accuracy test
   - Response time test
   - False positive test

5. Path Planning Test
   - Path generation test
   - Obstacle avoidance test
   - Replanning test
   - Execution accuracy test
```

### **Performance Testing:**
```
- Mapping speed: < 30s cho 10m x 10m area
- Localization accuracy: ±5cm position, ±2° orientation
- Obstacle detection range: 0.15m - 12m
- Update rate: 5Hz cho mapping, 10Hz cho localization
- Memory usage: < 1MB cho map storage
- USB bandwidth: < 10% of USB 2.0 capacity
```

---

## 📊 **CONFIGURATION PARAMETERS**

### **LiDAR Configuration:**
```json
{
  "lidar": {
    "interface": "usb",
    "device_path": "/dev/ttyUSB0",
    "scan_frequency": 5.5,
    "rotation_speed": 330,
    "detection_range": 12.0,
    "angular_resolution": 0.9,
    "update_rate": 5
  },
  "mapping": {
    "resolution": 0.05,
    "map_width": 1000,
    "map_height": 1000,
    "slam_algorithm": "gmapping",
    "optimization_rate": 1.0
  },
  "localization": {
    "algorithm": "amcl",
    "particle_count": 1000,
    "update_rate": 10,
    "confidence_threshold": 0.8
  },
  "obstacle_detection": {
    "safety_distance": 0.5,
    "warning_distance": 1.0,
    "max_obstacles": 10,
    "update_rate": 10
  },
  "path_planning": {
    "algorithm": "astar",
    "safety_margin": 0.3,
    "replanning_rate": 5,
    "smoothing": true
  }
}
```

---

**Changelog:**
- v2.0 (2025-01-27): Updated để tích hợp với Dock & Location module qua USB
- v1.0 (2025-01-27): Initial LiDAR integration guide

**Status:** Updated để tích hợp với Dock & Location module qua USB  
**Next Steps:** Implementation theo new USB integration requirements
