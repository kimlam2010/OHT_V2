# LiDAR Integration Guide - OHT-50

## 📋 **Tổng quan**
Hướng dẫn tích hợp LiDAR với hệ thống navigation và safety của OHT-50.

**Ngày tạo:** 2025-01-27  
**Phiên bản:** v1.0  
**Trạng thái:** Draft

---

## 🎯 **Mục tiêu**

### **Integration Objectives**
- Tích hợp LiDAR với navigation system
- Implement obstacle detection algorithm
- Integration với safety system
- Real-time data processing và analysis

### **Success Criteria**
- LiDAR data integrated với navigation
- Obstacle detection functional
- Safety system integration working
- Real-time performance achieved

---

## 🔧 **System Architecture**

### **LiDAR Integration Architecture**
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   LiDAR Sensor  │───▶│   LiDAR Driver  │───▶│  Data Processor │
│                 │    │                 │    │                 │
│ USB Interface   │    │ Python/C++ API  │    │ Filter & Smooth │
│ Real-time Data  │    │ Threading       │    │ Obstacle Detect │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                        │
                                ▼                        ▼
                       ┌─────────────────┐    ┌─────────────────┐
                       │  Navigation     │    │   Safety        │
                       │  System         │    │   System        │
                       │                 │    │                 │
                       │ Path Planning   │    │ E-Stop Trigger  │
                       │ Collision Avoid │    │ Speed Control   │
                       └─────────────────┘    └─────────────────┘
```

### **Data Flow**
```
LiDAR Raw Data → Driver Processing → Data Filtering → Obstacle Detection → Navigation/Safety
     ↓              ↓                    ↓                ↓                    ↓
 360° Scan      Threading API        Noise Filter    Distance Check      Path Planning
 Distance       Real-time Loop       Smoothing       Angle Analysis      E-Stop Control
```

---

## 💻 **Software Integration**

### **1. LiDAR Driver Class**
```python
#!/usr/bin/env python3
"""
LiDAR Driver for OHT-50 Integration
"""

import threading
import time
import numpy as np
from rplidar import RPLidar
from typing import List, Tuple, Optional, Callable

class OHT50LiDARDriver:
    """LiDAR driver with integration support"""
    
    def __init__(self, config: dict):
        self.config = config
        self.lidar = None
        self.running = False
        self.scan_data = []
        self.obstacles = []
        self.callbacks = []
        
        # Threading
        self.scan_thread = None
        self.lock = threading.Lock()
        
    def initialize(self) -> bool:
        """Initialize LiDAR connection"""
        try:
            self.lidar = RPLidar(self.config['device_port'])
            print("✅ LiDAR initialized for OHT-50 integration")
            return True
        except Exception as e:
            print(f"❌ LiDAR initialization failed: {e}")
            return False
    
    def start_scanning(self) -> bool:
        """Start continuous scanning"""
        if not self.lidar:
            return False
            
        self.running = True
        self.scan_thread = threading.Thread(target=self._scan_loop)
        self.scan_thread.daemon = True
        self.scan_thread.start()
        print("✅ LiDAR scanning started")
        return True
    
    def _scan_loop(self):
        """Continuous scanning loop"""
        while self.running:
            try:
                for scan in self.lidar.iter_scans():
                    if not self.running:
                        break
                    self._process_scan(scan)
            except Exception as e:
                print(f"❌ Scan error: {e}")
                time.sleep(1)
    
    def _process_scan(self, scan: List[Tuple[float, float]]):
        """Process scan data"""
        with self.lock:
            self.scan_data = scan
            self._detect_obstacles()
            self._notify_callbacks()
    
    def _detect_obstacles(self):
        """Detect obstacles in scan data"""
        obstacles = []
        for angle, distance in self.scan_data:
            if distance < self.config['obstacle_threshold']:
                obstacles.append((angle, distance))
        self.obstacles = obstacles
    
    def _notify_callbacks(self):
        """Notify registered callbacks"""
        for callback in self.callbacks:
            try:
                callback(self.scan_data, self.obstacles)
            except Exception as e:
                print(f"❌ Callback error: {e}")
    
    def register_callback(self, callback: Callable):
        """Register callback for scan data"""
        self.callbacks.append(callback)
    
    def get_scan_data(self) -> List[Tuple[float, float]]:
        """Get latest scan data"""
        with self.lock:
            return self.scan_data.copy()
    
    def get_obstacles(self) -> List[Tuple[float, float]]:
        """Get detected obstacles"""
        with self.lock:
            return self.obstacles.copy()
    
    def stop(self):
        """Stop LiDAR scanning"""
        self.running = False
        if self.scan_thread:
            self.scan_thread.join()
        if self.lidar:
            self.lidar.stop()
            self.lidar.disconnect()
        print("✅ LiDAR stopped")
```

### **2. Navigation Integration**
```python
#!/usr/bin/env python3
"""
Navigation Integration with LiDAR
"""

import math
import numpy as np
from typing import List, Tuple, Optional

class NavigationIntegration:
    """Navigation system integration with LiDAR"""
    
    def __init__(self, config: dict):
        self.config = config
        self.current_path = []
        self.obstacle_map = []
        self.safety_zones = []
        
    def update_obstacle_map(self, obstacles: List[Tuple[float, float]]):
        """Update obstacle map from LiDAR data"""
        self.obstacle_map = obstacles
        self._update_safety_zones()
    
    def _update_safety_zones(self):
        """Update safety zones based on obstacles"""
        safety_zones = []
        
        for angle, distance in self.obstacle_map:
            # Convert polar to cartesian
            x = distance * math.cos(math.radians(angle))
            y = distance * math.sin(math.radians(angle))
            
            # Check if in safety zone
            if distance < self.config['safety_zone']:
                safety_zones.append((x, y, distance))
        
        self.safety_zones = safety_zones
    
    def check_collision_risk(self, target_path: List[Tuple[float, float]]) -> bool:
        """Check collision risk for target path"""
        for path_point in target_path:
            for obstacle_x, obstacle_y, distance in self.safety_zones:
                # Calculate distance to path point
                dx = path_point[0] - obstacle_x
                dy = path_point[1] - obstacle_y
                point_distance = math.sqrt(dx*dx + dy*dy)
                
                if point_distance < self.config['collision_threshold']:
                    return True  # Collision risk detected
        
        return False  # No collision risk
    
    def generate_safe_path(self, start: Tuple[float, float], 
                          end: Tuple[float, float]) -> List[Tuple[float, float]]:
        """Generate safe path avoiding obstacles"""
        # Simple path planning - can be enhanced with A* or RRT
        path = [start]
        
        # Check if direct path is safe
        if not self.check_collision_risk([end]):
            path.append(end)
            return path
        
        # Find alternative path
        # This is a simplified version - real implementation would use
        # proper path planning algorithms
        alternative_path = self._find_alternative_path(start, end)
        path.extend(alternative_path)
        
        return path
    
    def _find_alternative_path(self, start: Tuple[float, float], 
                              end: Tuple[float, float]) -> List[Tuple[float, float]]:
        """Find alternative path avoiding obstacles"""
        # Simplified alternative path finding
        # In real implementation, use A* or RRT algorithm
        
        # For now, return a simple detour
        mid_point = ((start[0] + end[0])/2, (start[1] + end[1])/2)
        
        # Add some offset to avoid obstacles
        offset = 0.5  # 50cm offset
        if start[1] < end[1]:  # Moving forward
            mid_point = (mid_point[0], mid_point[1] + offset)
        else:  # Moving backward
            mid_point = (mid_point[0], mid_point[1] - offset)
        
        return [mid_point, end]
    
    def get_navigation_status(self) -> dict:
        """Get navigation status"""
        return {
            'obstacles_detected': len(self.obstacle_map),
            'safety_zones_active': len(self.safety_zones),
            'collision_risk': len(self.safety_zones) > 0,
            'path_clear': len(self.safety_zones) == 0
        }
```

### **3. Safety Integration**
```python
#!/usr/bin/env python3
"""
Safety Integration with LiDAR
"""

import time
from typing import List, Tuple, Optional

class SafetyIntegration:
    """Safety system integration with LiDAR"""
    
    def __init__(self, config: dict):
        self.config = config
        self.emergency_stop_active = False
        self.speed_limited = False
        self.last_safety_check = time.time()
        
    def check_safety_conditions(self, obstacles: List[Tuple[float, float]]) -> dict:
        """Check safety conditions based on LiDAR data"""
        safety_status = {
            'emergency_stop': False,
            'speed_limit': False,
            'warning': False,
            'safe': True
        }
        
        # Check for emergency stop conditions
        for angle, distance in obstacles:
            if distance < self.config['emergency_stop_distance']:
                safety_status['emergency_stop'] = True
                safety_status['safe'] = False
                break
        
        # Check for speed limit conditions
        if not safety_status['emergency_stop']:
            for angle, distance in obstacles:
                if distance < self.config['speed_limit_distance']:
                    safety_status['speed_limit'] = True
                    safety_status['safe'] = False
                    break
        
        # Check for warning conditions
        if not safety_status['emergency_stop'] and not safety_status['speed_limit']:
            for angle, distance in obstacles:
                if distance < self.config['warning_distance']:
                    safety_status['warning'] = True
                    break
        
        return safety_status
    
    def apply_safety_measures(self, safety_status: dict) -> dict:
        """Apply safety measures based on status"""
        actions = {
            'stop_motor': False,
            'reduce_speed': False,
            'activate_warning': False,
            'log_event': False
        }
        
        if safety_status['emergency_stop']:
            actions['stop_motor'] = True
            actions['log_event'] = True
            self.emergency_stop_active = True
            print("🚨 EMERGENCY STOP ACTIVATED - Obstacle too close!")
        
        elif safety_status['speed_limit']:
            actions['reduce_speed'] = True
            actions['log_event'] = True
            self.speed_limited = True
            print("⚠️ SPEED LIMITED - Obstacle detected")
        
        elif safety_status['warning']:
            actions['activate_warning'] = True
            print("⚠️ WARNING - Obstacle approaching")
        
        else:
            # Clear safety conditions
            if self.emergency_stop_active:
                print("✅ Emergency stop cleared")
                self.emergency_stop_active = False
            
            if self.speed_limited:
                print("✅ Speed limit cleared")
                self.speed_limited = False
        
        return actions
    
    def get_safety_status(self) -> dict:
        """Get current safety status"""
        return {
            'emergency_stop_active': self.emergency_stop_active,
            'speed_limited': self.speed_limited,
            'last_check': self.last_safety_check,
            'system_healthy': True
        }
```

---

## 🔄 **Integration Workflow**

### **1. System Initialization**
```python
# Initialize LiDAR integration
def initialize_lidar_integration():
    """Initialize complete LiDAR integration"""
    
    # Configuration
    config = {
        'device_port': '/dev/ttyUSB0',
        'obstacle_threshold': 1.0,      # 1m
        'safety_zone': 0.5,            # 50cm
        'emergency_stop_distance': 0.3, # 30cm
        'speed_limit_distance': 0.8,   # 80cm
        'warning_distance': 1.5,       # 1.5m
        'collision_threshold': 0.2,    # 20cm
    }
    
    # Initialize components
    lidar_driver = OHT50LiDARDriver(config)
    navigation = NavigationIntegration(config)
    safety = SafetyIntegration(config)
    
    # Setup callbacks
    def lidar_callback(scan_data, obstacles):
        navigation.update_obstacle_map(obstacles)
        safety_status = safety.check_safety_conditions(obstacles)
        safety_actions = safety.apply_safety_measures(safety_status)
        
        # Apply safety actions
        if safety_actions['stop_motor']:
            # Stop motor command
            pass
        elif safety_actions['reduce_speed']:
            # Reduce speed command
            pass
    
    lidar_driver.register_callback(lidar_callback)
    
    # Start LiDAR
    if lidar_driver.initialize():
        lidar_driver.start_scanning()
        return lidar_driver, navigation, safety
    else:
        return None, None, None
```

### **2. Real-time Operation**
```python
# Real-time operation loop
def run_lidar_integration():
    """Run LiDAR integration in real-time"""
    
    lidar_driver, navigation, safety = initialize_lidar_integration()
    
    if not lidar_driver:
        print("❌ LiDAR integration failed to initialize")
        return
    
    try:
        print("🚀 LiDAR integration running...")
        
        while True:
            # Get current status
            nav_status = navigation.get_navigation_status()
            safety_status = safety.get_safety_status()
            
            # Display status
            print(f"Navigation: {nav_status}")
            print(f"Safety: {safety_status}")
            
            # Check for user input to stop
            if input("Press 'q' to quit: ").lower() == 'q':
                break
            
            time.sleep(1)
    
    except KeyboardInterrupt:
        print("\n🛑 Stopping LiDAR integration...")
    finally:
        lidar_driver.stop()
        print("✅ LiDAR integration stopped")
```

---

## 🧪 **Integration Testing**

### **Test 1: Basic Integration Test**
```python
#!/usr/bin/env python3
"""
Basic Integration Test
"""

def test_basic_integration():
    """Test basic LiDAR integration"""
    print("🔧 Basic Integration Test")
    print("=" * 50)
    
    # Initialize integration
    lidar_driver, navigation, safety = initialize_lidar_integration()
    
    if not lidar_driver:
        print("❌ Integration initialization failed")
        return False
    
    try:
        # Test for 10 seconds
        print("📊 Testing integration for 10 seconds...")
        start_time = time.time()
        
        while time.time() - start_time < 10:
            # Get data
            scan_data = lidar_driver.get_scan_data()
            obstacles = lidar_driver.get_obstacles()
            
            # Update systems
            navigation.update_obstacle_map(obstacles)
            safety_status = safety.check_safety_conditions(obstacles)
            
            # Display status
            print(f"Scan points: {len(scan_data)}, Obstacles: {len(obstacles)}")
            print(f"Safety: {safety_status}")
            
            time.sleep(1)
        
        print("✅ Basic integration test completed")
        return True
        
    except Exception as e:
        print(f"❌ Integration test failed: {e}")
        return False
    finally:
        lidar_driver.stop()

if __name__ == "__main__":
    success = test_basic_integration()
    print(f"\nTest Result: {'✅ PASS' if success else '❌ FAIL'}")
```

### **Test 2: Navigation Integration Test**
```python
#!/usr/bin/env python3
"""
Navigation Integration Test
"""

def test_navigation_integration():
    """Test navigation integration"""
    print("🔧 Navigation Integration Test")
    print("=" * 50)
    
    # Initialize navigation
    config = {
        'safety_zone': 0.5,
        'collision_threshold': 0.2,
    }
    
    navigation = NavigationIntegration(config)
    
    # Test path planning
    start = (0.0, 0.0)
    end = (2.0, 0.0)
    
    # Test with no obstacles
    path = navigation.generate_safe_path(start, end)
    print(f"Path without obstacles: {path}")
    
    # Test with obstacles
    obstacles = [(0.0, 1.0, 0.3)]  # Obstacle at (0, 1) with distance 0.3m
    navigation.update_obstacle_map([(90.0, 0.3)])  # 90 degrees, 0.3m
    
    path_with_obstacles = navigation.generate_safe_path(start, end)
    print(f"Path with obstacles: {path_with_obstacles}")
    
    # Check collision risk
    collision_risk = navigation.check_collision_risk([end])
    print(f"Collision risk: {collision_risk}")
    
    # Get navigation status
    status = navigation.get_navigation_status()
    print(f"Navigation status: {status}")
    
    return True

if __name__ == "__main__":
    success = test_navigation_integration()
    print(f"\nTest Result: {'✅ PASS' if success else '❌ FAIL'}")
```

---

## 📊 **Performance Monitoring**

### **Performance Metrics**
```python
class PerformanceMonitor:
    """Monitor LiDAR integration performance"""
    
    def __init__(self):
        self.metrics = {
            'scan_rate': 0.0,
            'processing_time': 0.0,
            'obstacle_detection_time': 0.0,
            'navigation_update_time': 0.0,
            'safety_check_time': 0.0,
        }
    
    def update_metrics(self, scan_rate: float, processing_time: float):
        """Update performance metrics"""
        self.metrics['scan_rate'] = scan_rate
        self.metrics['processing_time'] = processing_time
    
    def get_performance_report(self) -> dict:
        """Get performance report"""
        return {
            'metrics': self.metrics,
            'status': 'healthy' if self.metrics['scan_rate'] >= 5.0 else 'degraded',
            'recommendations': self._get_recommendations()
        }
    
    def _get_recommendations(self) -> List[str]:
        """Get performance recommendations"""
        recommendations = []
        
        if self.metrics['scan_rate'] < 5.0:
            recommendations.append("Increase scan rate for better performance")
        
        if self.metrics['processing_time'] > 0.1:
            recommendations.append("Optimize processing time")
        
        return recommendations
```

---

## 🚨 **Troubleshooting**

### **Common Integration Issues**

#### **1. Data Synchronization**
- Use threading locks for data access
- Implement proper callback mechanisms
- Monitor data flow timing

#### **2. Performance Issues**
- Optimize processing algorithms
- Use efficient data structures
- Monitor system resources

#### **3. Safety System Conflicts**
- Implement proper priority handling
- Use state machines for safety logic
- Add redundancy checks

#### **4. Navigation Errors**
- Validate path planning algorithms
- Add collision detection redundancy
- Implement fallback navigation

---

## 📋 **Integration Checklist**

### **Pre-Integration**
- [ ] LiDAR hardware tested
- [ ] Driver functionality verified
- [ ] Navigation system ready
- [ ] Safety system configured
- [ ] Performance requirements defined

### **Integration**
- [ ] Driver integration completed
- [ ] Navigation integration tested
- [ ] Safety integration verified
- [ ] Performance monitoring active
- [ ] Error handling implemented

### **Post-Integration**
- [ ] Integration tests passed
- [ ] Performance validated
- [ ] Documentation updated
- [ ] Team training completed
- [ ] Deployment ready

---

## 📚 **References**

### **Technical Documentation**
- LiDAR Driver Documentation
- Navigation System Specifications
- Safety System Requirements

### **Related Documents**
- `docs/specs/lidar_wiring_guide.md` - Hardware wiring
- `docs/specs/lidar_setup_guide.md` - Setup guide
- `docs/specs/lidar_test_procedures.md` - Test procedures

---

**Changelog:**
- v1.0 (2025-01-27): Initial version with complete integration guide
