# IMPLEMENTATION GUIDE: ROBOT CONTROL SYSTEM - PHASE 3

**Phiên bản:** 2.0  
**Ngày cập nhật:** 2025-01-28  
**Mục tiêu:** Implement core robot control system với sensor fusion position estimation (KHÔNG có encoder)

---

## 🎯 **OVERVIEW & ARCHITECTURE**

### **System Architecture:**
```
┌─────────────────────────────────────────────────────────────┐
│                    ROBOT CONTROL SYSTEM                     │
├─────────────────────────────────────────────────────────────┤
│  Motion Planning  │  Sensor Fusion Position  │  Speed Control │
│  (S-curve,       │  (LiDAR + RFID +         │  (Acceleration │
│   Trapezoid)     │   Acceleration + Docking) │   Limits)      │
├─────────────────────────────────────────────────────────────┤
│                    Safety System                            │
│              (E-Stop, Collision Detection)                 │
└─────────────────────────────────────────────────────────────┘
```

### **Hardware Integration:**
- **LiDAR:** Mapping, obstacle detection, safety monitoring
- **RFID Reader/Tags:** Station localization, position reference
- **Acceleration Sensors:** Dead reckoning, movement tracking
- **Docking Module:** Station entry detection, reference positioning
- **Safety Module:** Emergency stop, collision prevention

---

## 🚀 **IMPLEMENTATION COMPONENTS**

### **1. Motion Planning Algorithms**

#### **S-Curve Profile:**
```python
class SCurveMotionPlanner:
    def __init__(self, max_velocity, max_acceleration, max_jerk):
        self.max_velocity = max_velocity
        self.max_acceleration = max_acceleration
        self.max_jerk = max_jerk
        
    async def plan_scurve_trajectory(self, start_position: float, 
                                    end_position: float) -> dict:
        """Plan S-curve trajectory with smooth acceleration/deceleration"""
        distance = abs(end_position - start_position)
        
        # Calculate time segments
        t1 = self.max_acceleration / self.max_jerk  # Jerk to acceleration
        t2 = (self.max_velocity / self.max_acceleration) - t1  # Constant acceleration
        t3 = t1  # Acceleration to constant velocity
        t4 = distance / self.max_velocity  # Constant velocity
        t5 = t1  # Constant velocity to deceleration
        t6 = t2  # Constant deceleration
        t7 = t1  # Deceleration to stop
        
        total_time = t1 + t2 + t3 + t4 + t5 + t6 + t7
        
        return {
            'trajectory_type': 's_curve',
            'start_position': start_position,
            'end_position': end_position,
            'total_distance': distance,
            'total_time': total_time,
            'time_segments': {
                'jerk_to_accel': t1,
                'constant_accel': t2,
                'accel_to_velocity': t3,
                'constant_velocity': t4,
                'velocity_to_decel': t5,
                'constant_decel': t6,
                'decel_to_stop': t7
            },
            'max_velocity': self.max_velocity,
            'max_acceleration': self.max_acceleration,
            'max_jerk': self.max_jerk
        }
```

#### **Trapezoid Profile:**
```python
class TrapezoidMotionPlanner:
    def __init__(self, max_velocity, max_acceleration):
        self.max_velocity = max_velocity
        self.max_acceleration = max_acceleration
        
    async def plan_trapezoid_trajectory(self, start_position: float, 
                                       end_position: float) -> dict:
        """Plan trapezoid trajectory with acceleration, constant, deceleration"""
        distance = abs(end_position - start_position)
        
        # Calculate acceleration distance
        accel_distance = (self.max_velocity ** 2) / (2 * self.max_acceleration)
        
        if accel_distance * 2 <= distance:
            # Full trapezoid profile
            t1 = self.max_velocity / self.max_acceleration  # Acceleration
            t2 = (distance - 2 * accel_distance) / self.max_velocity  # Constant
            t3 = t1  # Deceleration
            total_time = t1 + t2 + t3
        else:
            # Triangular profile (no constant velocity phase)
            t1 = (distance / self.max_acceleration) ** 0.5
            t2 = 0
            t3 = t1
            total_time = t1 + t3
            
        return {
            'trajectory_type': 'trapezoid',
            'start_position': start_position,
            'end_position': end_position,
            'total_distance': distance,
            'total_time': total_time,
            'time_segments': {
                'acceleration': t1,
                'constant_velocity': t2,
                'deceleration': t3
            },
            'max_velocity': self.max_velocity,
            'max_acceleration': self.max_acceleration
        }
```

### **2. Sensor Fusion Position Estimation (KHÔNG CÓ ENCODER)**

#### **LiDAR-Based Position Estimation:**
```python
class LiDARPositionEstimator:
    def __init__(self):
        self.map_data = None
        self.current_scan = None
        self.slam_processor = SLAMProcessor()
        
    async def estimate_position_from_lidar(self) -> dict:
        """Estimate position using LiDAR scan and map matching"""
        try:
            # 1. Process LiDAR point cloud
            point_cloud = await self.process_lidar_scan()
            
            # 2. Extract features (walls, corners, obstacles)
            features = self.extract_features(point_cloud)
            
            # 3. Map matching using ICP algorithm
            position = await self.slam_processor.match_to_map(features)
            
            # 4. Calculate confidence score
            confidence = self.calculate_position_confidence(features, position)
            
            return {
                'position': position,
                'confidence': confidence,
                'method': 'lidar_slam',
                'timestamp': time.time(),
                'features_detected': len(features),
                'point_cloud_size': len(point_cloud)
            }
            
        except Exception as e:
            logger.error(f"LiDAR position estimation failed: {e}")
            return {
                'position': None,
                'confidence': 0.0,
                'method': 'lidar_slam',
                'error': str(e)
            }
            
    async def process_lidar_scan(self) -> list:
        """Process raw LiDAR data into point cloud"""
        # Implementation for LiDAR data processing
        # Returns list of (x, y, z) coordinates
        pass
        
    def extract_features(self, point_cloud: list) -> list:
        """Extract geometric features from point cloud"""
        # Implementation for feature extraction
        # Returns list of detected features
        pass
        
    def calculate_position_confidence(self, features: list, position: tuple) -> float:
        """Calculate confidence score for position estimate"""
        # Implementation for confidence calculation
        # Returns confidence score 0.0 to 1.0
        pass
```

#### **RFID-Based Localization:**
```python
class RFIDLocalization:
    def __init__(self):
        self.rfid_reader = RFIDReader()
        self.tag_database = TagDatabase()
        
    async def get_position_from_rfid(self) -> dict:
        """Get position from RFID tags using triangulation"""
        try:
            # 1. Scan for nearby RFID tags
            nearby_tags = await self.rfid_reader.scan_tags()
            
            if len(nearby_tags) >= 3:
                # 2. Triangulate position using multiple tags
                position = await self.triangulate_position(nearby_tags)
                confidence = 0.8
                method = 'rfid_triangulation'
            elif len(nearby_tags) == 2:
                # 3. Estimate position using two tags
                position = await self.estimate_from_two_tags(nearby_tags)
                confidence = 0.6
                method = 'rfid_two_tag'
            elif len(nearby_tags) == 1:
                # 4. Estimate position using single tag
                position = await self.estimate_from_single_tag(nearby_tags[0])
                confidence = 0.4
                method = 'rfid_single_tag'
            else:
                # 5. No tags detected
                position = None
                confidence = 0.0
                method = 'rfid_none'
                
            return {
                'position': position,
                'confidence': confidence,
                'method': method,
                'tags_detected': len(nearby_tags),
                'tag_ids': [tag['id'] for tag in nearby_tags],
                'timestamp': time.time()
            }
            
        except Exception as e:
            logger.error(f"RFID localization failed: {e}")
            return {
                'position': None,
                'confidence': 0.0,
                'method': 'rfid_error',
                'error': str(e)
            }
            
    async def triangulate_position(self, tags: list) -> tuple:
        """Triangulate position using three or more RFID tags"""
        # Implementation for triangulation algorithm
        # Returns (x, y) coordinates
        pass
        
    async def estimate_from_two_tags(self, tags: list) -> tuple:
        """Estimate position using two RFID tags"""
        # Implementation for two-tag estimation
        # Returns (x, y) coordinates
        pass
        
    async def estimate_from_single_tag(self, tag: dict) -> tuple:
        """Estimate position using single RFID tag"""
        # Implementation for single-tag estimation
        # Returns (x, y) coordinates
        pass
```

#### **Acceleration-Based Dead Reckoning:**
```python
class AccelerationDeadReckoning:
    def __init__(self):
        self.accel_sensor = AccelerationSensor()
        self.last_position = (0.0, 0.0)
        self.last_velocity = (0.0, 0.0)
        self.last_time = time.time()
        self.drift_correction = DriftCorrection()
        
    async def update_position_from_acceleration(self) -> dict:
        """Update position using acceleration integration with drift correction"""
        try:
            # 1. Read acceleration data
            accel_data = await self.accel_sensor.read()
            current_time = time.time()
            dt = current_time - self.last_time
            
            # 2. Integrate acceleration to velocity
            new_velocity_x = self.last_velocity[0] + accel_data['x'] * dt
            new_velocity_y = self.last_velocity[1] + accel_data['y'] * dt
            
            # 3. Apply velocity limits
            max_velocity = 2.0  # m/s
            new_velocity_x = max(-max_velocity, min(max_velocity, new_velocity_x))
            new_velocity_y = max(-max_velocity, min(max_velocity, new_velocity_y))
            
            # 4. Integrate velocity to position
            new_position_x = self.last_position[0] + new_velocity_x * dt
            new_position_y = self.last_position[1] + new_velocity_y * dt
            
            # 5. Apply drift correction
            corrected_position = await self.drift_correction.correct_position(
                (new_position_x, new_position_y)
            )
            
            # 6. Update state
            self.last_position = corrected_position
            self.last_velocity = (new_velocity_x, new_velocity_y)
            self.last_time = current_time
            
            return {
                'position': corrected_position,
                'velocity': (new_velocity_x, new_velocity_y),
                'method': 'acceleration_integration',
                'confidence': 0.3,  # Low confidence due to drift
                'drift_corrected': True,
                'timestamp': current_time
            }
            
        except Exception as e:
            logger.error(f"Acceleration dead reckoning failed: {e}")
            return {
                'position': None,
                'confidence': 0.0,
                'method': 'acceleration_error',
                'error': str(e)
            }
            
class DriftCorrection:
    def __init__(self):
        self.correction_factor = 0.95
        
    async def correct_position(self, position: tuple) -> tuple:
        """Apply drift correction to position estimate"""
        # Implementation for drift correction
        # Returns corrected (x, y) coordinates
        pass
```

#### **Docking Module Integration:**
```python
class DockingPositionDetector:
    def __init__(self):
        self.docking_sensor = DockingSensor()
        self.station_database = StationDatabase()
        
    async def get_docking_reference_position(self) -> dict:
        """Get reference position from docking station"""
        try:
            # 1. Detect docking station
            station_id = await self.docking_sensor.detect_station()
            
            if station_id:
                # 2. Get station reference position from database
                station_data = await self.station_database.get_station(station_id)
                reference_position = station_data['reference_position']
                
                # 3. Calculate relative position to station
                relative_position = await self.docking_sensor.get_relative_position()
                
                # 4. Calculate absolute position
                absolute_position = (
                    reference_position[0] + relative_position[0],
                    reference_position[1] + relative_position[1]
                )
                
                # 5. Calculate confidence based on station proximity
                proximity = await self.docking_sensor.get_proximity()
                confidence = min(0.9, 0.5 + (proximity * 0.4))  # 0.5 to 0.9
                
                return {
                    'position': absolute_position,
                    'confidence': confidence,
                    'method': 'docking_reference',
                    'station_id': station_id,
                    'proximity': proximity,
                    'timestamp': time.time()
                }
            
            return {
                'position': None,
                'confidence': 0.0,
                'method': 'docking_none',
                'station_id': None
            }
            
        except Exception as e:
            logger.error(f"Docking position detection failed: {e}")
            return {
                'position': None,
                'confidence': 0.0,
                'method': 'docking_error',
                'error': str(e)
            }
```

#### **Sensor Fusion Algorithm:**
```python
class SensorFusionPositionEstimator:
    def __init__(self):
        self.lidar_estimator = LiDARPositionEstimator()
        self.rfid_localizer = RFIDLocalization()
        self.accel_reckoning = AccelerationDeadReckoning()
        self.docking_detector = DockingPositionDetector()
        self.fusion_algorithm = KalmanFilter()
        
    async def estimate_position(self) -> dict:
        """Estimate position using sensor fusion of all available sensors"""
        try:
            # 1. Get position estimates from all sensors
            lidar_result = await self.lidar_estimator.estimate_position_from_lidar()
            rfid_result = await self.rfid_localizer.get_position_from_rfid()
            accel_result = await self.accel_reckoning.update_position_from_acceleration()
            docking_result = await self.docking_detector.get_docking_reference_position()
            
            # 2. Filter out failed estimates
            valid_estimates = []
            if lidar_result['position'] and lidar_result['confidence'] > 0.3:
                valid_estimates.append({
                    'position': lidar_result['position'],
                    'confidence': lidar_result['confidence'],
                    'method': 'lidar',
                    'weight': 0.4
                })
                
            if rfid_result['position'] and rfid_result['confidence'] > 0.3:
                valid_estimates.append({
                    'position': rfid_result['position'],
                    'confidence': rfid_result['confidence'],
                    'method': 'rfid',
                    'weight': 0.3
                })
                
            if accel_result['position'] and accel_result['confidence'] > 0.2:
                valid_estimates.append({
                    'position': accel_result['position'],
                    'confidence': accel_result['confidence'],
                    'method': 'acceleration',
                    'weight': 0.2
                })
                
            if docking_result['position'] and docking_result['confidence'] > 0.5:
                valid_estimates.append({
                    'position': docking_result['position'],
                    'confidence': docking_result['confidence'],
                    'method': 'docking',
                    'weight': 0.1
                })
            
            # 3. Apply sensor fusion algorithm
            if len(valid_estimates) >= 2:
                fused_position = await self.fusion_algorithm.fuse_positions(valid_estimates)
                confidence = self.calculate_fused_confidence(valid_estimates)
                method = 'sensor_fusion'
            elif len(valid_estimates) == 1:
                fused_position = valid_estimates[0]['position']
                confidence = valid_estimates[0]['confidence']
                method = f"single_sensor_{valid_estimates[0]['method']}"
            else:
                fused_position = None
                confidence = 0.0
                method = 'no_sensors'
            
            return {
                'position': fused_position,
                'confidence': confidence,
                'method': method,
                'sensors_used': len(valid_estimates),
                'sensor_details': {
                    'lidar': lidar_result,
                    'rfid': rfid_result,
                    'acceleration': accel_result,
                    'docking': docking_result
                },
                'timestamp': time.time()
            }
            
        except Exception as e:
            logger.error(f"Sensor fusion position estimation failed: {e}")
            return {
                'position': None,
                'confidence': 0.0,
                'method': 'fusion_error',
                'error': str(e)
            }
            
    def calculate_fused_confidence(self, estimates: list) -> float:
        """Calculate confidence score for fused position estimate"""
        if not estimates:
            return 0.0
            
        # Weighted average confidence
        total_weight = sum(est['weight'] for est in estimates)
        weighted_confidence = sum(
            est['confidence'] * est['weight'] for est in estimates
        )
        
        return weighted_confidence / total_weight if total_weight > 0 else 0.0
```

### **3. Speed Control với Acceleration Limits**

```python
class SpeedController:
    def __init__(self, max_speed: float, max_acceleration: float):
        self.max_speed = max_speed
        self.max_acceleration = max_acceleration
        self.current_speed = 0.0
        self.target_speed = 0.0
        self.safety_monitor = SafetySpeedMonitor()
        
    async def control_speed(self, target_speed: float, 
                           safety_conditions: dict) -> dict:
        """Control speed with acceleration limits and safety monitoring"""
        try:
            # 1. Check safety conditions
            safety_check = await self.safety_monitor.check_speed_safety(
                target_speed, safety_conditions
            )
            
            if not safety_check['safe']:
                # Emergency speed reduction
                target_speed = min(target_speed, safety_check['max_safe_speed'])
                logger.warning(f"Safety condition violated: {safety_check['reason']}")
            
            # 2. Calculate safe acceleration
            speed_diff = target_speed - self.current_speed
            safe_accel = min(abs(speed_diff), self.max_acceleration)
            
            # 3. Apply acceleration limit
            if speed_diff > 0:
                new_speed = self.current_speed + safe_accel
            else:
                new_speed = self.current_speed - safe_accel
                
            # 4. Ensure within limits
            new_speed = max(0.0, min(new_speed, self.max_speed))
            
            # 5. Update current speed
            self.current_speed = new_speed
            self.target_speed = target_speed
            
            return {
                'current_speed': new_speed,
                'target_speed': target_speed,
                'acceleration_applied': safe_accel,
                'safety_status': safety_check,
                'timestamp': time.time()
            }
            
        except Exception as e:
            logger.error(f"Speed control failed: {e}")
            # Emergency stop
            self.current_speed = 0.0
            return {
                'current_speed': 0.0,
                'target_speed': 0.0,
                'acceleration_applied': 0.0,
                'error': str(e),
                'emergency_stop': True
            }
            
class SafetySpeedMonitor:
    def __init__(self):
        self.lidar_safety = LiDARSafetyMonitor()
        
    async def check_speed_safety(self, target_speed: float, 
                                safety_conditions: dict) -> dict:
        """Check if target speed is safe given current conditions"""
        try:
            # 1. Check LiDAR safety (obstacles, proximity)
            lidar_safety = await self.lidar_safety.check_safety()
            
            # 2. Calculate safe speed based on conditions
            max_safe_speed = self.max_speed
            
            if not lidar_safety['safe']:
                max_safe_speed = min(max_safe_speed, lidar_safety['max_safe_speed'])
                
            # 3. Check other safety conditions
            if safety_conditions.get('emergency_stop'):
                max_safe_speed = 0.0
            elif safety_conditions.get('collision_risk'):
                max_safe_speed = min(max_safe_speed, 0.5)  # 50% of max speed
            elif safety_conditions.get('proximity_warning'):
                max_safe_speed = min(max_safe_speed, 0.7)  # 70% of max speed
                
            # 4. Determine if target speed is safe
            safe = target_speed <= max_safe_speed
            
            return {
                'safe': safe,
                'max_safe_speed': max_safe_speed,
                'lidar_safety': lidar_safety,
                'reason': 'normal' if safe else 'safety_condition_violated'
            }
            
        except Exception as e:
            logger.error(f"Speed safety check failed: {e}")
            return {
                'safe': False,
                'max_safe_speed': 0.0,
                'reason': f'safety_check_error: {e}'
            }
```

### **4. Safety System Integration**

```python
class SafetySystem:
    def __init__(self):
        self.emergency_stop = EmergencyStop()
        self.collision_detector = CollisionDetector()
        self.speed_monitor = SafetySpeedMonitor()
        self.lidar_safety = LiDARSafetyMonitor()
        
    async def monitor_safety(self, current_speed: float, 
                           position_estimate: dict,
                           motion_plan: dict) -> dict:
        """Monitor safety using all available sensors and conditions"""
        try:
            # 1. LiDAR obstacle detection
            obstacle_status = await self.lidar_safety.detect_obstacles()
            
            # 2. Collision detection
            collision_status = await self.collision_detector.check_collision_risk(
                current_speed, position_estimate, motion_plan
            )
            
            # 3. Speed safety check
            speed_safety = await self.speed_monitor.check_speed_safety(
                current_speed, {'obstacles': obstacle_status}
            )
            
            # 4. Position confidence check
            position_safe = position_estimate.get('confidence', 0.0) > 0.6
            
            # 5. Overall safety decision
            all_safe = all([
                obstacle_status['safe'],
                collision_status['safe'],
                speed_safety['safe'],
                position_safe
            ])
            
            # 6. Take action if unsafe
            if not all_safe:
                await self.emergency_stop.activate()
                action = 'emergency_stop'
            else:
                action = 'continue'
                
            return {
                'safe': all_safe,
                'action': action,
                'safety_checks': {
                    'obstacles': obstacle_status,
                    'collision': collision_status,
                    'speed': speed_safety,
                    'position': position_safe
                },
                'timestamp': time.time()
            }
            
        except Exception as e:
            logger.error(f"Safety monitoring failed: {e}")
            # Emergency stop on error
            await self.emergency_stop.activate()
            return {
                'safe': False,
                'action': 'emergency_stop',
                'error': str(e),
                'reason': 'safety_monitoring_error'
            }
            
class EmergencyStop:
    def __init__(self):
        self.activated = False
        
    async def activate(self):
        """Activate emergency stop"""
        self.activated = True
        logger.critical("🚨 EMERGENCY STOP ACTIVATED!")
        # Implementation for emergency stop
        pass
        
    async def reset(self):
        """Reset emergency stop"""
        self.activated = False
        logger.info("Emergency stop reset")
```

---

## 🔧 **INTEGRATION & CONTROL FLOW**

### **Main Control Loop:**
```python
class RobotControlSystem:
    def __init__(self):
        # Motion Planning
        self.motion_planner = MotionPlanner()
        
        # Position Estimation (NO ENCODER - Sensor Fusion)
        self.position_estimator = SensorFusionPositionEstimator()
        
        # Speed Control
        self.speed_controller = SpeedController(max_speed=2.0, max_acceleration=1.0)
        
        # Safety System
        self.safety_system = SafetySystem()
        
        # Control state
        self.control_active = False
        self.current_trajectory = None
        
    async def execute_movement(self, target_position: tuple, 
                             target_speed: float) -> dict:
        """Execute movement using sensor fusion position estimation"""
        try:
            # 1. Get current position estimate
            current_position = await self.position_estimator.estimate_position()
            
            if not current_position['position']:
                raise ValueError("Cannot determine current position")
                
            # 2. Plan motion trajectory
            trajectory = await self.motion_planner.plan_trajectory(
                start=current_position['position'],
                end=target_position,
                max_speed=target_speed
            )
            
            # 3. Execute trajectory with safety monitoring
            result = await self.execute_trajectory(trajectory, current_position)
            
            return {
                'success': True,
                'start_position': current_position['position'],
                'end_position': target_position,
                'trajectory': trajectory,
                'execution_result': result
            }
            
        except Exception as e:
            logger.error(f"Movement execution failed: {e}")
            return {
                'success': False,
                'error': str(e),
                'emergency_stop': True
            }
            
    async def execute_trajectory(self, trajectory: dict, 
                                position_estimate: dict) -> dict:
        """Execute planned trajectory with continuous safety monitoring"""
        try:
            self.control_active = True
            self.current_trajectory = trajectory
            
            # Implementation for trajectory execution
            # Continuous position estimation and safety monitoring
            # Speed control with acceleration limits
            
            return {
                'trajectory_completed': True,
                'execution_time': 0.0,
                'final_position': position_estimate['position']
            }
            
        except Exception as e:
            logger.error(f"Trajectory execution failed: {e}")
            await self.safety_system.emergency_stop.activate()
            return {
                'trajectory_completed': False,
                'error': str(e),
                'emergency_stop': True
            }
        finally:
            self.control_active = False
            self.current_trajectory = None
```

---

## 📊 **PERFORMANCE REQUIREMENTS**

### **Position Estimation Performance:**
```yaml
Position Estimation Targets:
  LiDAR-based: "< 100ms, accuracy ±50mm, confidence > 0.7"
  RFID-based: "< 50ms, accuracy ±100mm, confidence > 0.6"
  Acceleration-based: "< 20ms, accuracy ±200mm, confidence > 0.3"
  Docking reference: "< 30ms, accuracy ±25mm, confidence > 0.8"
  Sensor fusion: "< 150ms, accuracy ±75mm, confidence > 0.8"
```

### **Motion Planning Performance:**
```yaml
Motion Planning Targets:
  S-curve generation: "< 50ms"
  Trapezoid generation: "< 30ms"
  Trajectory validation: "< 20ms"
  Real-time adjustment: "< 100ms"
```

### **Speed Control Performance:**
```yaml
Speed Control Targets:
  Speed adjustment: "< 50ms"
  Acceleration limiting: "< 20ms"
  Safety monitoring: "< 30ms"
  Emergency response: "< 10ms"
```

---

## 🧪 **TESTING STRATEGY**

### **Unit Tests:**
```python
class TestRobotControlSystem:
    async def test_motion_planning(self):
        """Test motion planning algorithms"""
        planner = MotionPlanner()
        
        # Test S-curve planning
        s_curve = await planner.plan_scurve_trajectory(0.0, 10.0)
        assert s_curve['trajectory_type'] == 's_curve'
        assert s_curve['total_distance'] == 10.0
        
        # Test trapezoid planning
        trapezoid = await planner.plan_trapezoid_trajectory(0.0, 10.0)
        assert trapezoid['trajectory_type'] == 'trapezoid'
        assert trapezoid['total_distance'] == 10.0
        
    async def test_sensor_fusion(self):
        """Test sensor fusion position estimation"""
        estimator = SensorFusionPositionEstimator()
        
        # Test with mock sensor data
        position = await estimator.estimate_position()
        assert position['method'] in ['sensor_fusion', 'single_sensor_lidar']
        assert position['confidence'] >= 0.0
        
    async def test_speed_control(self):
        """Test speed control with acceleration limits"""
        controller = SpeedController(max_speed=2.0, max_acceleration=1.0)
        
        # Test speed increase
        result = await controller.control_speed(1.5, {})
        assert result['current_speed'] <= 1.5
        assert result['acceleration_applied'] <= 1.0
```

### **Integration Tests:**
```python
class TestRobotControlIntegration:
    async def test_end_to_end_movement(self):
        """Test complete movement execution"""
        control_system = RobotControlSystem()
        
        # Execute movement
        result = await control_system.execute_movement(
            target_position=(5.0, 5.0),
            target_speed=1.0
        )
        
        assert result['success'] == True
        assert 'trajectory' in result
        
    async def test_safety_integration(self):
        """Test safety system integration"""
        control_system = RobotControlSystem()
        
        # Simulate unsafe condition
        # Verify emergency stop activation
        pass
```

---

## 🚨 **SAFETY CONSIDERATIONS**

### **Critical Safety Features:**
1. **Emergency Stop:** Immediate stop on safety violation
2. **Collision Prevention:** LiDAR-based obstacle detection
3. **Speed Limiting:** Dynamic speed adjustment based on conditions
4. **Position Validation:** Confidence-based position acceptance
5. **Fail-Safe Operation:** Graceful degradation on sensor failure

### **Safety Protocols:**
- **E-Stop Activation:** < 10ms response time
- **Speed Reduction:** < 50ms on safety violation
- **Trajectory Abort:** < 100ms on collision risk
- **Position Fallback:** Use last known good position

---

## 📋 **IMPLEMENTATION CHECKLIST**

### **Week 1 Deliverables:**
- [ ] **Motion Planning Algorithms**
  - [ ] S-curve profile implementation
  - [ ] Trapezoid profile implementation
  - [ ] Trajectory validation
  - [ ] Unit tests for motion planning
  
- [ ] **Sensor Fusion Position Estimation**
  - [ ] LiDAR position estimator
  - [ ] RFID localization
  - [ ] Acceleration dead reckoning
  - [ ] Docking module integration
  - [ ] Sensor fusion algorithm
  - [ ] Unit tests for position estimation
  
- [ ] **Speed Control với Acceleration Limits**
  - [ ] Speed controller implementation
  - [ ] Acceleration limiting
  - [ ] Safety speed monitoring
  - [ ] Unit tests for speed control
  
- [ ] **Safety System Integration**
  - [ ] Emergency stop system
  - [ ] Collision detection
  - [ ] Safety monitoring
  - [ ] Unit tests for safety system
  
- [ ] **Integration Testing**
  - [ ] End-to-end movement test
  - [ ] Safety integration test
  - [ ] Performance validation
  - [ ] Error handling test

---

## 🔄 **CHANGELOG**

### **v2.0 (2025-01-28):**
- ✅ **MAJOR CHANGE:** Replaced encoder-based position control with sensor fusion
- ✅ **NEW:** LiDAR-based position estimation using SLAM
- ✅ **NEW:** RFID-based localization with triangulation
- ✅ **NEW:** Acceleration-based dead reckoning with drift correction
- ✅ **NEW:** Docking module integration for reference positioning
- ✅ **NEW:** Sensor fusion algorithm combining all sensors
- ✅ **UPDATED:** Speed control with LiDAR safety monitoring
- ✅ **ENHANCED:** Safety system with multi-sensor validation
- ✅ **REMOVED:** All encoder-related code and dependencies

### **v1.0 (2025-01-28):**
- ✅ Initial implementation guide created
- ✅ Basic motion planning algorithms
- ✅ Encoder-based position control (REMOVED)
- ✅ Basic speed control
- ✅ Basic safety system

---

**🚨 Lưu ý quan trọng:** Tài liệu này đã được cập nhật để **KHÔNG sử dụng encoder** và thay thế bằng **sensor fusion approach** sử dụng LiDAR, RFID, acceleration sensors, và docking module. Đây là phương án **ĐÚNG** cho hardware có sẵn của OHT-50.
